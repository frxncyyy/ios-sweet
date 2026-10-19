#!/usr/bin/env python3
"""Build the minimal Apple Device Tree for the Nord 2 XNU early probe.

Every address comes from dtb/nord2/live.dts and was cross-checked against
/proc/iomem and the successful bare-metal probe (see NOTES.md). This ADT is
for the *early* probe only: it deliberately has no display, touch, UFS or
storage nodes, because none of those drivers has been ported to MT6893.

Usage:  mkadt-nord2.py OUT.adt [--dump]

Kernel-managed RAM is a 992 MiB window in the upper DDR bank:
0x1_42000000..0x1_80000000. It is 32 MiB aligned (the 16K-page bootstrap map
uses 32 MiB L2 blocks), lies entirely in the live FDT's second memory bank
(0x1_40000000 + 0xfcc00000), and ends 0xae000000 below the first reserved
carve-out of that bank (0x2_2e000000). The 16 MiB dummy-read reservation at
the very start of the bank is skipped by the 32 MiB offset.
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import adt  # noqa: E402

# Upper DDR bank window. Must match shim/nord2-xnu/loader.c.
PHYS_BASE = 0x142000000
MEM_SIZE = 0x3e000000

# MT6893 platform, from dtb/nord2/live.dts:
#   gic:  reg = <0x0c000000 0x40000 0x0c040000 0x200000>, arm,gic-v3
#   timer: CNTFRQ_EL0 = 13 MHz (probe), architected virtual timer PPI 27
#   CPU:   reg = <0x000 0x100 ... 0x700> (Aff1:Aff0), A55 0-3, A78 4-7
GICD, GICD_SIZE = 0x0c000000, 0x40000
GICR, GICR_SIZE = 0x0c040000, 0x200000
TIMEBASE = 13000000
VTIMER_INTID = 27
IPI_SGI = 1
SOC_BASE = 0x01000000

# Synthetic device identity. The probe does not run iOS userland, so this is
# only here to keep XNU's platform code on the path it takes on a real board.
SWEET_ECID = 0x4e4f524432000001		# 'NORD2' + 1
BOOT_MANIFEST_HASH = b'\x00' * 48

u32 = lambda *v: struct.pack('<%dI' % len(v), *v)
u64 = lambda *v: struct.pack('<%dQ' % len(v), *v)
cstr = lambda s: s.encode() + b'\0'


def node(name, props=(), children=()):
    n = adt.Node()
    n.props = [('name', 0, cstr(name))] + [(k, 0, v) for k, v in props]
    n.children = list(children)
    return n


def build():
    ph = iter(range(1, 1000))
    phandle = lambda: ('AAPL,phandle', u32(next(ph)))

    chosen = node('chosen', [
        phandle(),
        ('dram-base', u64(PHYS_BASE)),
        ('dram-size', u64(MEM_SIZE)),
        ('random-seed', b'\x5a' * 64),
        ('debug-enabled', u32(1)),
        ('firmware-version', cstr('nord2-probe')),
        ('system-firmware-version', cstr('nord2-probe')),
        ('boot-manifest-hash', BOOT_MANIFEST_HASH),
        ('crypto-hash-method', cstr('sha2-384')),
        ('display-scale', u32(3)),
        ('unique-chip-id', struct.pack('<Q', SWEET_ECID)),
        ('chip-id', u32(0x8015)),
        ('certificate-production-status', u32(0)),
        ('effective-production-status-ap', u32(0)),
        ('certificate-security-mode', u32(1)),
        ('effective-security-mode-sep', u32(1)),
        # Filled in by the loader with physical addresses and the XNU window
        # that the chain-back trampoline must clean. Zeros here are a
        # deliberate failure mode: XNU refuses to jump if they stay zero.
        ('nord2-stock-entry', u64(0)),
        ('nord2-stock-dtb', u64(0)),
        ('nord2-xnu-region', u64(0, 0)),
    ], [node('memory-map', [phandle()]),
        node('asmb', [phandle(), ('lp-sip0', u64(0x6f)), ('lp-sip1', u32(1))])])

    # Eight cores in the live FDT's order: A55 (0-3) then A78 (4-7).
    cpu_nodes = []
    for core in range(8):
        cpu_nodes.append(node(f'cpu{core}', [
            phandle(),
            ('device_type', cstr('cpu')),
            ('compatible', cstr('arm,cortex-a78' if core >= 4 else 'arm,cortex-a55') + cstr('ARM,v8')),
            ('reg', u32(core << 8)),
            ('cpu-id', u32(core)),
            ('cluster-id', u32(0)),
            ('cluster-type', u32(0)),
            ('die-cluster-id', u32(0)),
            ('cluster-core-id', u32(core)),
            ('l2-cache-id', u32(core)),
            ('l3-cache-id', u32(0)),
            ('state', cstr('running' if core == 0 else 'stopped')),
            ('timebase-frequency', u32(TIMEBASE)),
            ('clock-frequency', u32(0x77359400 if core < 4 else 0x9af8da00)),
            ('bus-frequency', u32(100000000)),
            ('memory-frequency', u32(1866000000)),
            ('peripheral-frequency', u32(TIMEBASE)),
            ('fixed-frequency', u32(TIMEBASE)),
        ]))
    cpus = node('cpus', [phandle(), ('#address-cells', u32(1)),
                         ('#size-cells', u32(0))], cpu_nodes)

    # pe_fiq reads reg as 4 x u64 relative to the arm-io base. PPI 27 is the
    # architected virtual timer on MT6893 (probe: CNTFRQ_EL0 = 13 MHz).
    gic = node('gic', [
        phandle(),
        ('compatible', cstr('arm,gic-v3')),
        ('interrupt-controller', b''),
        ('virtual-timer-intid', u32(VTIMER_INTID)),
        ('reg', u64(GICD - SOC_BASE, GICD_SIZE, GICR - SOC_BASE, GICR_SIZE)),
    ])
    arm_io = node('arm-io', [
        phandle(),
        ('device_type', cstr('nord2-io')),
        ('compatible', cstr('nord2-io') + cstr('arm-io')),
        ('#address-cells', u32(2)),
        ('#size-cells', u32(2)),
        ('ranges', u64(0, SOC_BASE, 0x100000000 - SOC_BASE)),
    ], [gic,
        # watchdogd: the MT6893 watchdog is not ported; "no-pmu" keeps the
        # boot CPU driver from touching it.
        node('wdt', [phandle(), ('no-pmu', u32(1))])])

    defaults = node('defaults', [phandle(), ('no-effaceable-storage', u32(1))])

    buttons = node('buttons', [
        phandle(),
        ('button-names', cstr('volup') + cstr('voldown') + cstr('hold')),
        ('home-button-type', u32(2)),
        ('opposed-power-vol-buttons', u32(1)),
    ])

    return node('device-tree', [
        phandle(),
        ('compatible', cstr('D22AP') + cstr('iPhone10,3') + cstr('xiaomi,sweet') + cstr('mediatek,mt6893') + cstr('AppleARM')),
        ('model', cstr('iPhone10,3')),
        ('target-type', cstr('D22')),
        ('target-sub-type', cstr('D22AP')),
        ('manufacturer', cstr('Apple Inc.')),
        ('#address-cells', u32(2)),
        ('#size-cells', u32(2)),
    ], [chosen, defaults, cpus, arm_io, buttons])


if __name__ == '__main__':
    root = build()
    blob = adt.serialize(root)
    with open(sys.argv[1], 'wb') as f:
        f.write(blob)
    if '--dump' in sys.argv:
        adt.dump(adt.parse(blob)[0])
    print(f'{sys.argv[1]}: {len(blob)} bytes', file=sys.stderr)
