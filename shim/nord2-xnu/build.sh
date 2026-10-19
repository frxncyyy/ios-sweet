#!/bin/sh
# Build the Nord 2 XNU early-probe boot image. No device access.
#
#   shim/nord2-xnu/build.sh
#
# Needs:
#   research/ci-39158169212/...        CI XNU buildtree and object archive
#   research/nord2/kernel.relink       locally relinked kernel (Sweet sources
#                                      patched for NORD2; see README)
#   backup/nord2/boot.img              verified original boot partition dump
#   ../toolchains/cctools              Apple ld/ar/strip
#
# Steps: rebuild the five modified XNU objects, replace them in the archive,
# relink with research/nord2/link-command.json, strip the symbol table, build
# the ADT and the payload, build this loader, pack a boot image and run the
# static checker. Relinking and packing refuse to overwrite an existing
# artifact, so a stale file is never silently reused.
#
# Decompressed size: LK decompresses the kernel to 0x40080000 into a buffer
# bounded by KERNEL_DECOMPRESS_SIZE, which MediaTek's platform rules.mk sets
# to 0x0c800000 (200 MiB) on this generation. The packed blob is about 68 MiB
# (2 MiB loader + 52.8 MiB stock Image + 13.5 MiB payload) and ends at
# 0x444c0410, below both the buffer limit and the ramdisk at 0x51100000.
set -eu
cd "$(dirname "$0")/../.."
out=shim/out/nord2-xnu
mkdir -p "$out"

BT=research/ci-39158169212/buildtree
ARCHIVE=research/ci-39158169212/xnu-kernel/kernel.development.sweet.link/kernel.development.sweet.a
CC="python3 tools/nord2-xnu-cc.py $BT"
EXTRA="-DNORD2=1"

# 1. Modified XNU sources.
SRCS="osfmk/arm64/sweet_early.c osfmk/arm64/start.s osfmk/arm/arm_init.c \
osfmk/arm64/arm_vm_init.c osfmk/console/serial_console.c bsd/kern/bsd_init.c \
osfmk/arm64/sleh.c osfmk/kern/startup.c iokit/Kernel/arm/SweetBattery.cpp"
for src in $SRCS; do
    obj="$out/$(basename "$src").o"
    comp="${src%%/*}"
    # IOLib.h needs Apple-only __builtin_xnu_* builtins; the fallbacks keep the
    # templates parseable (they are not instantiated by SweetBattery).
    cppflags=
    case "$src" in
        *.cpp) cppflags='-D__builtin_xnu_type_summary(x)=0xffff -D__builtin_xnu_types_compatible(a,b)=1 -D__builtin_xnu_type_signature(x)="NORD"' ;;
    esac
    # shellcheck disable=SC2086
    $CC "$comp" "$BT/xnu/$src" "$obj" $EXTRA $cppflags
done

# 2. Replace those objects in a copy of the CI archive.
cp "$ARCHIVE" "$out/kernel-nord2.a"
for src in $SRCS; do
    base="$(basename "$src")"
    ../toolchains/cctools/bin/aarch64-apple-darwin-ar d "$out/kernel-nord2.a" "${base%.c}.o" "${base%.s}.o" "${base%.cpp}.o" "${base%.cpp}.cpo" 2>/dev/null || true
done
../toolchains/cctools/bin/aarch64-apple-darwin-ar r "$out/kernel-nord2.a" \
    "$out/sweet_early.c.o" "$out/start.s.o" "$out/arm_init.c.o" \
    "$out/arm_vm_init.c.o" "$out/serial_console.c.o" "$out/bsd_init.c.o" \
    "$out/SweetBattery.cpp.o" \
    "$out/sleh.c.o" "$out/startup.c.o"
../toolchains/cctools/bin/aarch64-apple-darwin-ranlib "$out/kernel-nord2.a"

# 3. Relink with the saved argv, swapping the archive and output path.
python3 - "$out" <<'PY'
import json, os, subprocess, sys
out = sys.argv[1]
argv = json.load(open('research/nord2/link-command.json'))
archive = os.path.abspath(f'{out}/kernel-nord2.a')
target = os.path.abspath(f'{out}/kernel.nord2')
for i, a in enumerate(argv):
    if a.endswith('kernel.development.sweet.a'):
        argv[i] = archive
    elif a.endswith('kernel.relink'):
        argv[i] = target
if os.path.exists(target):
    raise SystemExit(f'{target} exists; remove it explicitly to relink')
r = subprocess.run(argv, cwd=os.getcwd())
sys.exit(r.returncode)
PY

# 4. Strip the symbol table (the early probe has no symboliser; this drops
#    ~7 MiB from the payload) and build the ADT and the payload. The payload
#    carries the boot arguments; nord2stop=1 is also the kernel default, but
#    writing it out keeps the image self-describing.
../toolchains/cctools/bin/aarch64-apple-darwin-strip -S -x "$out/kernel.nord2" \
    -o "$out/kernel.nord2.strip"
python3 tools/mkadt-nord2.py "$out/nord2-probe.adt"
python3 tools/mkpayload.py "$out/kernel.nord2.strip" "$out/nord2-probe.adt" "$out/payload.bin" \
    --cmdline "debug=0xc cpus=1 keepsyms=1 -noprogress nord2stop=1"
# Escalation: continue past cpu_init, stop before arm_vm_init; then enter it.
python3 tools/mkpayload.py "$out/kernel.nord2.strip" "$out/nord2-probe.adt" \
    "$out/payload-continue.bin" \
    --cmdline "debug=0xc cpus=1 keepsyms=1 -noprogress nord2stop=0"
python3 tools/mkpayload.py "$out/kernel.nord2.strip" "$out/nord2-probe.adt" \
    "$out/payload-vm.bin" \
    --cmdline "debug=0xc cpus=1 keepsyms=1 -noprogress nord2stop=0 nord2vm=1"
# Bounded arm_vm_init walk: the loader advances nord2vmstop by one per reboot
# (the four digits are rewritten in RAM from the preserved NORD2-NEXT tag).
python3 tools/mkpayload.py "$out/kernel.nord2.strip" "$out/nord2-probe.adt" \
    "$out/payload-vmstep.bin" \
    --cmdline "debug=0xc cpus=1 keepsyms=1 -noprogress nord2stop=0 nord2vm=1 nord2vmstop=0001"
# Phase 2: the same arm_vm_init entry, with the on-screen console enabled.
python3 tools/mkpayload.py "$out/kernel.nord2.strip" "$out/nord2-probe.adt" \
    "$out/payload-vmfb.bin" \
    --cmdline "debug=0xc cpus=1 keepsyms=1 -noprogress nord2stop=0 nord2vm=1 nord2fb=1"
# Full early bring-up but stop at the end of arm_init() and return to Android
# with the pmsg log (nord2stopat=0001, see nord2_stop_at()).
python3 tools/mkpayload.py "$out/kernel.nord2.strip" "$out/nord2-probe.adt" \
    "$out/payload-late.bin" \
    --cmdline "debug=0xc cpus=1 keepsyms=1 -noprogress nord2stop=0 nord2vm=1 nord2fb=1 nord2stopat=0001"
# Stop hook 2: after IOKitBSDInit(), i.e. after the Sweet IOKit drivers
# (AMFI, AKS, WDT, SURF, HID, battery) have registered.
python3 tools/mkpayload.py "$out/kernel.nord2.strip" "$out/nord2-probe.adt" \
    "$out/payload-late2.bin" \
    --cmdline "debug=0xc cpus=1 keepsyms=1 -noprogress nord2stop=0 nord2vm=1 nord2fb=1 nord2stopat=0002"

# 5. Loader. The probe entry point; also buildable as -DNORD2_LOADER_ONLY=1
#    (validate only) or -DNORD2_COPY_ONLY=1 (also copy the segments), which
#    both chain straight back to Android and are useful when isolating a hang.
clang --target=aarch64-none-elf -O2 -ffreestanding -nostdlib -fno-pic \
    -mgeneral-regs-only -mstrict-align -fno-stack-protector -fno-builtin \
    -Wall -Wextra -Werror -c shim/nord2-xnu/start.S -o "$out/start.S.o"
clang --target=aarch64-none-elf -O2 -ffreestanding -nostdlib -fno-pic \
    -mgeneral-regs-only -mstrict-align -fno-stack-protector -fno-builtin \
    -Wall -Wextra -Werror -c shim/nord2-xnu/loader.c -o "$out/loader.c.o"
ld.lld -T shim/nord2-xnu/xnu.ld --build-id=none "$out/start.S.o" "$out/loader.c.o" \
    -o "$out/loader.elf"
ld.lld -T shim/nord2-xnu/xnu.ld --build-id=none --oformat=binary \
    "$out/start.S.o" "$out/loader.c.o" -o "$out/loader-Image"

# 6. Boot images and static verification.
for spec in "boot:payload.bin" "continue:payload-continue.bin" "vm:payload-vm.bin" \
            "vmstep:payload-vmstep.bin" "vmfb:payload-vmfb.bin" "late:payload-late.bin" "late2:payload-late2.bin"; do
    name="${spec%%:*}"
    payload="${spec#*:}"
    python3 tools/nord2-pack-xnu.py backup/nord2/boot.img "$out/loader-Image" \
        "$out/$payload" "$out/nord2-xnu-$name.img"
    python3 tools/nord2-check-xnu.py "$out/nord2-xnu-$name.img" "$out/$payload" | tail -4
done
