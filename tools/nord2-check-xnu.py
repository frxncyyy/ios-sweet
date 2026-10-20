#!/usr/bin/env python3
"""Static verification of the Nord 2 XNU early-probe boot image.

  nord2-check-xnu.py BOOT_IMG [PAYLOAD]

BOOT_IMG  the image produced by tools/nord2-pack-xnu.py
PAYLOAD   the SWEETXNU payload it embeds (default: the matching payload in
          the same directory as BOOT_IMG)

The check never touches a device. It re-derives the layout the loader and
start.s rely on and refuses to be quiet about anything unexpected:

  * boot v2 header: addresses, page size, header version, id (SHA-1);
  * the ramdisk and DT table are byte-identical to backup/nord2/boot.img;
  * the decompressed kernel is [loader 2 MiB][stock Image padded to its
    image_size][SWEETXNU payload at a 16 KiB boundary];
  * the payload's window is 0x1_42000000 + 0x3e000000, its entry and its
    /chosen chain-back properties (nord2-stock-entry/dtb/xnu-region) hold
    the values the loader fills in;
  * CPU nodes match a SweetCPU personality present in the embedded kernel,
    so a stale kernel/ADT combination cannot silently omit the CPU driver;
  * the blob stays below the ramdisk load address (0x51100000) and the
    whole image below the 32 MiB partition.
"""
import argparse
import hashlib
import re
import struct
import sys
import zlib
from pathlib import Path

import adt as apple_adt

PARTITION_SIZE = 33554432
PAGE = 2048
KERNEL_PA = 0x40080000
RAMDISK_PA = 0x51100000
LOADER_RESERVE = 0x200000
PHYS_BASE = 0x142000000
MEM_SIZE = 0x3e000000
VIRT_BASE = 0xfffffe0006000000
BLOCK_SHIFT = 25

STOCK_BOOT = Path(__file__).resolve().parent.parent / 'backup/nord2/boot.img'


def le32(b, o):
    return struct.unpack_from('<I', b, o)[0]


def align(n, a=PAGE):
    return (n + a - 1) // a * a


def unmatched_cpus(kernel, root):
    """Check this port's string IONameMatch personalities against CPU compatibles."""
    names = set()
    for entry in re.findall(rb'\{[^{}\x00]*\}', kernel):
        if not re.search(rb"'IOClass'\s*=\s*SweetCPU;", entry):
            continue
        if not re.search(rb"'IOProviderClass'\s*=\s*IOPlatformDevice;", entry):
            continue
        match = re.search(rb"'IONameMatch'\s*=\s*'([^']+)';", entry)
        if match:
            names.add(match[1])
    cpus = next((n.children for n in root.children if n.name == 'cpus'), [])
    missing = [n.name for n in cpus
               if not names.intersection((n.prop('compatible') or b'').split(b'\0'))]
    return names, missing if cpus else ['/cpus absent or empty']


class Checker:
    def __init__(self, payload):
        self.payload = payload
        self.failures = []

    def check(self, name, ok, detail=''):
        print(('ok   ' if ok else 'FAIL ') + name + (f' -- {detail}' if detail else ''))
        if not ok:
            self.failures.append(name)

    def run(self, img, stock):
        p = self.payload
        page = PAGE
        self.check('magic ANDROID!', img[:8] == b'ANDROID!')
        self.check('boot header v2', le32(img, 40) == 2)
        self.check('page size 2048', le32(img, 36) == page)
        self.check('kernel_addr 0x40080000', le32(img, 12) == KERNEL_PA)
        self.check('ramdisk_addr 0x51100000', le32(img, 20) == RAMDISK_PA)
        self.check('second_size 0', le32(img, 24) == 0)
        self.check('recovery_dtbo_size 0', le32(img, 1632) == 0)

        ks, rs, ds = (le32(img, o) for o in (8, 16, 1648))
        sks, srs, sds = (le32(stock, o) for o in (8, 16, 1648))
        self.check('ramdisk_size unchanged', rs == srs, str(rs))
        self.check('dtb_size unchanged', ds == sds, str(ds))

        rstart = page + align(ks)
        dstart = rstart + align(rs)
        ramdisk = img[rstart:rstart + rs]
        dtb = img[dstart:dstart + ds]
        srs_off = page + align(sks)
        sds_off = srs_off + align(srs)
        self.check('ramdisk byte-identical', ramdisk == stock[srs_off:srs_off + srs])
        self.check('dtb byte-identical', dtb == stock[sds_off:sds_off + sds])

        h = hashlib.sha1()
        for part in (img[page:page + ks], ramdisk, b'', b'', dtb):
            h.update(part)
            h.update(struct.pack('<I', len(part)))
        self.check('boot v2 id (SHA-1)', img[576:608] == h.digest().ljust(32, b'\0'))
        self.check('image fits the 32 MiB partition', len(img) <= PARTITION_SIZE,
                   f'{len(img)} bytes')

        dec = zlib.decompressobj(31)
        blob = dec.decompress(img[page:page + ks]) + dec.flush()
        self.check('gzip kernel stream complete', dec.eof)
        sdec = zlib.decompressobj(31)
        simage = sdec.decompress(stock[page:page + sks]) + sdec.flush()
        self.check('appended DTBs preserved', dec.unused_data == sdec.unused_data)

        image_size = struct.unpack_from('<Q', simage, 16)[0]
        stock_part = blob[LOADER_RESERVE:LOADER_RESERVE + image_size]
        self.check('stock Image at +2 MiB, byte-identical',
                   stock_part[:len(simage)] == simage)
        self.check('stock BSS zero-padded to image_size',
                   stock_part[len(simage):] == b'\0' * (image_size - len(simage)))
        payload_off = align(LOADER_RESERVE + image_size, 0x4000)
        self.check('blob is exactly loader + stock + payload',
                   len(blob) == payload_off + len(p))
        self.check('SWEETXNU payload at the 16 KiB boundary',
                   blob[payload_off:payload_off + len(p)] == p)
        loader = blob[:LOADER_RESERVE]
        self.check('loader is an arm64 Image', loader[56:60] == b'ARM\x64')
        self.check('loader branches straight to its entry',
                   loader[0:4] == b'\x00\x02\x00\x14')
        self.check('loader fits the 2 MiB reserve',
                   struct.unpack_from('<Q', loader, 16)[0] <= LOADER_RESERVE)
        self.check('blob stays below the ramdisk load address',
                   KERNEL_PA + len(blob) <= RAMDISK_PA,
                   f'blob end {KERNEL_PA + len(blob):#x}')
        self.check('payload fits the loader source scan bound',
                   len(blob) <= LOADER_RESERVE + 0x6000000)

        magic, version, nsegs = struct.unpack_from('<8sII', p, 0)
        vb, entry, adt_off, adt_size, kend, pbase, msize = \
            struct.unpack_from('<7Q', p, 0x10)
        self.check('SWEETXNU v1', magic == b'SWEETXNU' and version == 1)
        self.check('segment count sane', 0 < nsegs <= 64, str(nsegs))
        self.check('window is the Nord 2 probe window',
                   pbase == PHYS_BASE and msize == MEM_SIZE,
                   f'{pbase:#x}+{msize:#x}')
        self.check('virt_base', vb == VIRT_BASE, hex(vb))
        segs = [struct.unpack_from('<4Q', p, 0x2a8 + i * 32) for i in range(nsegs)]
        inside = [i for i, s in enumerate(segs)
                  if s[0] <= entry < s[0] + s[1]]
        self.check('entry is 4-byte aligned and inside a segment',
                   entry % 4 == 0 and bool(inside),
                   f'{entry:#x} in segment {inside}')
        self.check('entry is in executable text',
                   bool(inside) and inside[0] >= 2, f'segment {inside}')

        head_off = struct.unpack_from('<Q', p, 0x2a8)[0] - vb
        blocks = (msize - head_off) >> BLOCK_SHIFT
        region_start = pbase
        region_end = pbase + (blocks << BLOCK_SHIFT)
        print(f'     32 MiB identity window {region_start:#x}..{region_end:#x} '
              f'({blocks} blocks), trampoline {region_start + 0x4000:#x}')
        kend_pa = kend - vb + pbase
        ba_pa = align(kend_pa + adt_size, 0x4000)
        self.check('ADT + boot_args fit the window', ba_pa + 0x4000 <= region_end,
                   f'kernel_end {kend_pa:#x}, boot_args {ba_pa:#x}')
        self.check('ADT inside the payload',
                   adt_off + adt_size <= len(p), f'adt {adt_off:#x}+{adt_size:#x}')

        adt = p[adt_off:adt_off + adt_size]
        root, _ = apple_adt.parse(adt)
        kernel = b''.join(p[off:off + size] for _, _, off, size in segs)
        cpu_matches, unmatched = unmatched_cpus(kernel, root)
        self.check('CPU nodes match an embedded SweetCPU personality', not unmatched,
                   f'matches={sorted(n.decode() for n in cpu_matches)}, '
                   f'unmatched={unmatched}')
        nprops = le32(adt, 0)
        q = 8
        for _ in range(nprops):
            q += 36 + ((le32(adt, q + 32) & 0x7fffffff) + 3 & ~3)
        cn = le32(adt, q)
        q += 8
        found = {}
        for _ in range(cn):
            name = adt[q:q + 32].split(b'\0')[0].decode()
            ln = le32(adt, q + 32) & 0x7fffffff
            found[name] = adt[q + 36:q + 36 + ln]
            q += 36 + ((ln + 3) & ~3)
        self.check('/chosen nord2-stock-entry, 8 bytes, zero',
                   found.get('nord2-stock-entry') == b'\0' * 8)
        self.check('/chosen nord2-stock-dtb, 8 bytes, zero',
                   found.get('nord2-stock-dtb') == b'\0' * 8)
        self.check('/chosen nord2-xnu-region, 16 bytes, zero',
                   found.get('nord2-xnu-region') == b'\0' * 16)

        cmdline = p[0x48:0x48 + 608].split(b'\0')[0].decode()
        self.check('cmdline carries the escalation boot arguments',
                   'nord2stop=' in cmdline, cmdline)
        stops = [arg for arg in cmdline.split()
                 if arg.split('=', 1)[0] in ('nord2stopat', 'nord2vmstop', 'nord2vmloop')]
        self.check('stop arguments avoid implicit octal/string parsing',
                   all(re.fullmatch(r'[^=]+=(?:0|[1-9][0-9]*|0x[0-9a-fA-F]+)', arg)
                       for arg in stops), ', '.join(stops))
        args = cmdline.split()
        if 'nord2stopat=13' in args:
            self.check('point 13 has the bounded MediaTek USB snapshot',
                       b'NORD2 USB: MTU3 snapshot begin (read-only)' in kernel and
                       b'after read-only MTU3 snapshot' in kernel and
                       b'NORD2 USB: clock gated; IPPC/MAC not read' in kernel)
            self.check('point 13 keeps both disk write flags disabled',
                       [a for a in args if a.startswith('ufsrw=')] == ['ufsrw=0'] and
                       [a for a in args if a.startswith('rootrw=')] == ['rootrw=0'])
        if any(f'nord2stopat={p}' in args for p in (14, 15, 16)):
            self.check('USB console has the bounded native MTU3 transport',
                       b'NORD2 USB: native console for 45 seconds; EP0 PIO, no DMA' in kernel and
                       b'after bounded MTU3 USB console' in kernel and
                       b'NORD2 USB: inherited QMU active; refused' in kernel)
            self.check('USB console keeps both disk write flags disabled',
                       [a for a in args if a.startswith('ufsrw=')] == ['ufsrw=0'] and
                       [a for a in args if a.startswith('rootrw=')] == ['rootrw=0'])
        if 'nord2stopat=15' in args:
            self.check('point 15 includes saved PHY state and stop marker',
                       b'NORD2 USB: PHY[%03x]=%08x' in kernel and
                       b'after bounded MTU3 PHY USB console' in kernel and
                       b'NORD2 USB: PHY snapshot unavailable; refused' in kernel)
        if 'nord2stopat=16' in args:
            self.check('point 16 has the bounded background console',
                       b'NORD2 USB: background console ready; continuing BSD boot' in kernel and
                       b'after background MTU3 USB console' in kernel)
            self.check('point 16 uses only the diagnostic RAM root',
                       [a for a in args if a.startswith('rd=')] == ['rd=md0'])
        if 'nord2stopat=17' in args or 'nord2stopat=18' in args:
            self.check('PSCI probe has bounded startup and OFF-before-return checks',
                       b'NORD2 SMP: PSCI probe begin;' in kernel and
                       b'after bounded PSCI CPU_ON/OFF probe' in kernel and
                       b'NORD2 SMP: secondary still active; manual reboot required' in kernel)
            self.check('PSCI probe keeps XNU single CPU and stops before disk mount',
                       [a for a in args if a.startswith('cpus=')] == ['cpus=1'] and
                       [a for a in args if a.startswith('rd=')] == ['rd=ufsroot0'] and
                       [a for a in args if a.startswith('ufsrw=')] == ['ufsrw=0'] and
                       [a for a in args if a.startswith('rootrw=')] == ['rootrw=0'])
        if 'nord2stopat=18' in args:
            self.check('point 18 has the low32 fixed-mailbox probe',
                       b'NORD2 SMP: low32 trampoline copied and verified; fixed mailbox' in kernel and
                       b'after low32 PSCI CPU_ON/OFF probe' in kernel)
            self.check('low trampoline/mailboxes use only zero loader padding',
                       struct.unpack_from('<Q', loader, 16)[0] <= 0x100000 and
                       loader[0x100000:0x108000] == bytes(0x8000))
        chosen = next((n for n in root.children if n.name == 'chosen'), None)
        mmap = next((n for n in chosen.children if n.name == 'memory-map'), None) if chosen else None
        ramdisk = mmap.prop('RAMDisk') if mmap else None
        if ramdisk is not None or 'rd=md0' in args:
            self.check('RAM root selects md0 unambiguously',
                       [a for a in args if a.startswith('rd=')] == ['rd=md0'])
            valid = ramdisk is not None and len(ramdisk) == 16
            self.check('RAMDisk has physical address and size', valid)
            if valid:
                rd_pa, rd_size = struct.unpack('<QQ', ramdisk)
                rd_va = rd_pa - pbase + vb
                matches = [s for s in segs if s[0] == rd_va and s[1] == rd_size]
                self.check('RAMDisk has one backed, page-aligned segment',
                           rd_size > 0 and rd_pa % 0x4000 == 0 and
                           rd_size % 0x4000 == 0 and len(matches) == 1 and
                           0 < matches[0][3] <= rd_size and
                           matches[0][2] + matches[0][3] <= adt_off)
                self.check('RAMDisk is reserved before ADT and inside RAM',
                           pbase <= rd_pa and rd_pa + rd_size == kend_pa and
                           rd_pa + rd_size <= region_end)
                self.check('RAMDisk follows all kernel segments without overlap',
                           len(matches) == 1 and
                           all(s == matches[0] or s[0] + s[1] <= rd_va for s in segs))
        if 'rd=ufsroot0' in args:
            ufs = next((n for n in root.children if n.name == 'sweet-ufs'), None)
            self.check('Nord 2 root uses the verified UFS controller and LUN2',
                       ufs is not None and
                       ufs.prop('reg') == struct.pack('<QQ', 0x11270000, 0x2300) and
                       ufs.prop('lun') == struct.pack('<I', 2))
            self.check('embedded kernel includes Nord 2 UFS root support',
                       b'NORD2 UFS: root probe host restored, ready=' in kernel)
        stopat = [a.split('=', 1)[1] for a in stops if a.startswith('nord2stopat=')]
        try:
            root_probe = any(int(v, 0) == 12 for v in stopat)
        except ValueError:
            root_probe = False  # Already rejected by the stop-argument check.
        if root_probe:
            self.check('point 12 selects UFS root with both write flags disabled',
                       len(stopat) == 1 and
                       [a for a in args if a.startswith('rd=')] == ['rd=ufsroot0'] and
                       [a for a in args if a.startswith('ufsrw=')] == ['ufsrw=0'] and
                       [a for a in args if a.startswith('rootrw=')] == ['rootrw=0'])


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('image', type=Path)
    ap.add_argument('payload', type=Path, nargs='?')
    ap.add_argument('--stock', type=Path, default=STOCK_BOOT,
                    help='verified original boot.img (default: %(default)s)')
    a = ap.parse_args()
    payload = a.payload or a.image.with_name('payload.bin')
    c = Checker(payload.read_bytes())
    c.run(a.image.read_bytes(), a.stock.read_bytes())
    print()
    print(f'{a.image}: {a.image.stat().st_size} bytes, '
          f'SHA256 {hashlib.sha256(a.image.read_bytes()).hexdigest()}')
    print(f'{payload}: {payload.stat().st_size} bytes, '
          f'SHA256 {hashlib.sha256(payload.read_bytes()).hexdigest()}')
    print('FAILURES: ' + (', '.join(c.failures) if c.failures else 'none'))
    sys.exit(1 if c.failures else 0)


if __name__ == '__main__':
    main()
