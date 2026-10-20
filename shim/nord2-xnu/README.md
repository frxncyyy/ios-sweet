# Nord 2 XNU and iOS bring-up

As of 2026-10-21, the OnePlus Nord 2 (MediaTek MT6893) boots the installed
iOS 16 userland to the lockscreen. Eight XNU cores, the command-mode display,
FT3518 touchscreen, power/volume buttons and native USB logging have passed
hardware tests. The iOS HFS filesystem occupies the former Android userdata
partition. **Do not boot normal Android over it.** The current fallback is
the tested ADB recovery in RAM, which mounts no block filesystem.

The loader packages Sweet XNU built for MT6893 with the preserved Android
Linux kernel and device trees. It supports both bounded diagnostic stops and
continuous iOS mode at point23. Current iOS images use the tested rescue
ramdisk for a permissible Linux return. Unsafe controller or restoration
state halts instead. Native USB exposes live logs; the early console and
ramoops pmsg log remain useful before USB starts.

The user confirmed substantially faster startup after console optimization,
skipping the missing RTC service wait and restoring the standard Display Zoom
dimensions. The latest measured display-ownership marker is47.72 seconds
from loader stage6; it is not an exact power-on-to-lockscreen measurement.
UFS is currently G4, one lane in each direction, HS seriesB. About2GB of boot
reads took7.31 aggregate command seconds with no errors in the latest report.
Two-lane negotiation is still a diagnostic experiment. Wi-Fi power and four
firmware files in RAM have been verified; firmware execution and networking
are not working yet. See `WIFI-LOG.md` for the next initialization stages.

Exact current images, authorization, capture results and the next action are
in the active handoff at the top of `NOTES.md`. The sections below preserve
the earlier diagnostic stages and build mechanics.

## Earlier diagnostic milestones

As of 2026-10-20, the corrected CPU-driver matching and
unambiguous `nord2stopat=8` argument have passed on hardware: XNU returns
from `kperf_init_early`, reaches the stop before BSD, and chains back to
Android with pmsg intact. Point 9 has also passed: BSD reaches root
filesystem selection, including network-thread scheduling. Points 10–12
also passed: inherited UFSHCI register inspection, DMA/SCSI reads of LUN2
capacity/GPT, and read-only publication of `ufsroot0` before mounting.
The inherited host state was restored before returning to Android.

Those pre-root probes do not write UFS media. They do write the
pmsg and framebuffer reservations, the DSI/MUTEX registers used to present
frames, and the MediaTek watchdog restart register. Past early bring-up it
also initializes the GIC and architected virtual timer.

### Verified RAM-root userspace probe

`nord2-hfs-pid1.img` adds an 8 MiB HFS+ RAM root to the point-12 kernel.
The image helper accepts `--ramdisk IMAGE` immediately after its two
positional arguments; use `rd=md0 nord2stopat=0 ufsrw=0 rootrw=0`.
The root remains in RAM and the UFS driver is not selected. The checker
verifies the RAMDisk ADT address/size against its payload segment and the
reserved memory layout.

`tools/nord2-init-probe.S`, wrapped with `tools/mkinit.py`, is the diagnostic
`/sbin/launchd`, not Apple's launchd. It opens `/dev/console`, requires
`getpid()==1`, and verifies the exact contents of `/nord2-probe.txt`.
Success prints `NORD2 USERLAND: HFS read verified; getpid=1; EL0 syscalls OK`,
then deliberately exits with code 66. PID1's exit triggers the existing
Nord2 panic/Android-return hook; that final panic is expected. Exit codes
70–74 or an earlier panic are failures. Require the success marker, HFS
mount evidence and Android return, not just a reboot. All these markers
were observed on the Nord2 on 2026-10-20, followed by verified restoration
of the original Android boot, before userdata was replaced. This historical
probe preceded the successful Apple launchd and full iOS userland boot.

### ADB recovery without Android userdata

`tools/nord2-mkrescue.py` builds a minimal recovery ramdisk from the device's
existing recovery backup, keeping the original boot's Linux kernel and DT.
It runs authenticated, root USB ADB and mounts no block filesystem. The host
public key is embedded in the private output image; no private key is copied.
The standalone image has passed on hardware: root shell, kernel log collection,
unmounted userdata and exact boot-image readback were verified.

ADB reports this environment as `recovery`; use `adb wait-for-recovery` or
direct `adb shell` commands, not `wait-for-device` or `sys.boot_completed`.
The marker is `/nord2-rescue`. Check that a pstore file exists using `adb shell`
before reading it with `exec-out`: the latter can return exit0 while emitting
an error message. A cold reset can leave pstore empty.

The XNU image helper accepts `--base-boot RESCUE.img --zopfli ZOPFLI_BINARY`.
Zopfli produces a smaller standard gzip stream, verified byte-for-byte after
decompression, so XNU and the unmodified rescue ramdisk fit in the 32 MiB boot
partition. The combined point13 image returned to ADB recovery on hardware
with the complete XNU pmsg record intact.
Generic panic return also restores any owned UFS request list before Linux
runs; an unsafe DMA state halts instead of handing it to the next kernel.

## Artifact layout

`tools/nord2-pack-xnu.py` reuses the stock Android boot v2 header, ramdisk
and DT table, and replaces only the gzip kernel stream:

```
0x40080000  loader                      (this shim, zero-padded to 2 MiB)
0x40280000  stock Android Image         (byte-identical, BSS zero-padded to
                                         the header's image_size)
0x43744000  SWEETXNU payload            (16 KiB aligned; current stock kernel)
```

LK decompresses the whole stream to `0x40080000` and, because the loader's
Image header branches from its first instruction, runs the loader. The
loader finds the stock Image header at `0x40280000`. The original kernel is
preserved byte for byte at this relocated address, and the chain-back needs
no copying. The payload address is derived from the stock Image's
`image_size` (including BSS); it is not a hard-coded offset in the packer.
The decompressed end varies with the payload. The image checker reports it
and verifies both the ramdisk bound and the loader's stricter source-end bound
at `0x46280000`.

The loader places the payload's Mach-O segments in a 992 MiB window in the
upper DDR bank:

```
phys_base 0x1_42000000   mem_size 0x3e000000   virt_base 0xfffffe0006000000
```

`start.s` identity-maps 30 complete 32 MiB blocks after allowing for the
kernel's head offset (`0x1_42000000..0x1_7e000000`); the ADT and `boot_args` sit
just past the kernel image, and the chain-back trampoline is copied to
`0x1_42004000`, the first free page of the mapped window.

## Chain-back

The boot args carry `/chosen` with three zero placeholders that the loader
patches in RAM:

| property | size | value |
| --- | --- | --- |
| `nord2-stock-entry` | 8 | `0x40280000` (stock Image entry) |
| `nord2-stock-dtb` | 8 | the live FDT physical address |
| `nord2-xnu-region` | 16 | first mapped / end of the identity window |

`nord2_maybe_return()`, called from `arm_init.c` after `cpu_init()`, caches
them and, unless the boot argument `nord2stop=0` was given, returns to
Android. Later stops use the same cached values. Before entering
`nord2_leave_xnu()` in `start.s`, the return path masks interrupts and
restores the bootstrap TTBR0 table: kernel-thread switches otherwise
replace it with `invalid_ttep`. The assembly copies its trampoline into the
window, cleans the whole window to the point of coherency, turns the MMU off
with the `SCTLR_EL1` value the CPU probe observed at stock entry
(`0x30d00800`), zeroes `x1..x3`, and branches to the stock entry with
`x0 = dtb`. `nord2stop=1` is also the kernel default.

After `arm_vm_init()`, `nord2_console_remap()` gives pmsg, framebuffer and
console/watchdog MMIO permanent kernel mappings in TTBR1 so logging
survives context switches. Keep `cpus=1` for these tests: secondary CPU
bring-up and returning to Android with secondary CPUs active are unverified.

### Bounded arm_vm_init walk

`arm_vm_init` contains fifteen
numbered gates, compiled in and chosen by the boot argument `nord2vmstop`:
a gate stops the kernel and logs `NORD2-NEXT=0x…`. The loader reads the last
`NORD2-NEXT` out of the preserved ring and rewrites `nord2vmstop=` in the
payload command line, so `nord2-xnu-vmstep.img` advances one gate per plain
reboot; the last `NORD2-NEXT` printed before a hang names the failing step.
The full `arm_init` path has since passed on hardware; these gates remain
available for regressions.

### Later stops

Use `nord2stop=0 nord2vm=1 nord2fb=1 nord2stopat=N` to run to a later
stop and chain back. Use ordinary decimal or explicit hexadecimal (`0x08`),
never zero-padded decimal: XNU parses `0007` as octal 7 but `0008` as a
string, so it does not select point 8. The legacy automatic VM gate walker
also writes zero-padded decimal and must be corrected before reuse.

| N | Stop |
| --- | --- |
| 1 | End of `arm_init` |
| 2 | Before `SweetBattery::registerService` (also an older BSD IOKit hook) |
| 3 | After `SweetBattery::registerService` |
| 4 | After `StartIOKitMatching` |
| 5 | Entry to `bsd_init` |
| 6 | After the copyright print in `bsd_init` |
| 7 | After `vm_set_restrictions`, before kperf |
| 8 | After `kperf_init_early`, before `bsd_init` |
| 9 | In `bsd_init`, after `inittodr`, before selecting/mounting the root filesystem |
| 10 | Same location as point 9, after a read-only UFSHCI register snapshot |
| 11 | Same location, after a bounded LUN2 capacity/GPT read probe and controller-state restoration |
| 12 | After publishing/selecting `ufsroot0`, before mounting; restores the UFS host and returns |
| 13 | Before root selection, read-only MTU3 USB/clock snapshot; returns to the supplied Linux ramdisk |

Point numbers are identifiers, not execution order. Never infer success
from a reboot alone: inspect the stop marker and the Android return marker.

Point 10 has passed on the Nord 2: UFSHCI 2.0 is enabled, supports 64-bit
addressing, and has no pending requests. Point 11 also passed: capacity, boot/userdata extents and both GPT CRCs
match the Android baseline, followed by controller restoration and a normal
return to Android. It accepts only TUR, READ CAPACITY(10) and READ(10)
of GPT metadata in LBAs 1–63; checks GPT CRCs and bounds; then restores the
inherited request list and interrupt state before returning to Android.
If a timeout cannot be cleared or restoration fails, it retains its DMA
memory and waits for a manual reboot. No root filesystem is mounted.

Point 12 passed on the Nord2 before mounting. Use `rd=ufsroot0 ufsrw=0 rootrw=0`.
The Nord 2 root driver validates LUN2 capacity and GPT CRCs, accepts only
userdata, and omits Sweet's Qualcomm input/USB and background log startup.
The diagnostic additionally forces read-only operation in the driver.

## Reading the result

After Android starts, the log is `/sys/fs/pstore/pmsg-ramoops-0` (read it
before the next reboot). Run `tools/nord2-pull-logs.sh LABEL` immediately to
save it with Android's boot reason and kernel log. Successful late stops
show the requested marker followed by `RETURN TO STOCK ANDROID KERNEL`.
Watchdog/manual cold resets can clear DRAM and leave pstore empty; in that
case the on-screen log is the remaining evidence. Logs from the earlier
CPU probe are in `backup/nord2/probe-v2-*`.

## Building

Build the complete kernel with the repository's `xnu.yml` GitHub Actions
workflow on macOS (Apple clang). The current fork is `frxncyyy/ios-sweet`,
branch `codex/nord2-bringup`. Download both `xnu-kernel` and `xnu-buildtree`;
record the run ID and head commit in `NOTES.md`.

Package a CI kernel with the existing loader:

```sh
tools/nord2-image-from-kernel.sh PATH/TO/kernel.development.sweet \
  shim/out/nord2-xnu/NEW-TEST.img \
  debug=0xc cpus=1 keepsyms=1 -noprogress \
  nord2stop=0 nord2vm=1 nord2fb=1 nord2stopat=8
```

The helper strips symbols, builds the current ADT and payload, packs the
boot image, and runs `tools/nord2-check-xnu.py`. It refuses an existing output.
It needs the existing `shim/out/nord2-xnu/loader-Image`, cctools in
`../toolchains/cctools`, and the verified `backup/nord2/boot.img`.

Local C/assembly changes can be compiled with `tools/nord2-xnu-cc.py`.
`shim/nord2-xnu/build.sh` supports the older local relink path using the
extracted CI tree and archive in `research/ci-39158169212/`. It does not
rebuild every C++ object from the latest patches. Do not use it to package a
new C++ change, and do not substitute upstream-clang `__builtin_xnu_*`
fallbacks for Apple compiler metadata.

XNU-side edits are carried by `xnu/patches/0082-nord2-early-probe.patch`.
`tools/nord2-mkpatch.sh` regenerates it from the working buildtree, verifies
that it applies to the pristine 0001–0081 CI tree, and compares every
resulting file. Add any newly touched source file to its `FILES` list.

## Flashing

This directory builds an image; it is not permission to flash it. `fastboot
boot` fails on this phone even for the original image, so the current test
procedure flashes **boot only** after exact-image authorization and manual
entry into fastboot. Use the active handoff's verified image and single-use
runner. Diagnostic images may intentionally halt and require manual fastboot
before returning to working iOS or the tested RAM ADB rescue.

The original `backup/nord2/boot.img` remains preserved (33554432 bytes,
SHA-256 `04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01`),
but restoring and booting normal Android is no longer the default recovery:
userdata now contains iOS. Continuous iOS images mount that filesystem read/
write. Keep bootloaders, recovery, modem, persist and backups intact.
