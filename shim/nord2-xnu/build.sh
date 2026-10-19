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
# Steps: rebuild the four modified XNU objects, replace them in the archive,
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
CC="python3 tools/nord2-xnu-cc.py $BT osfmk"
EXTRA="-DNORD2=1"

# 1. Modified XNU sources.
for src in osfmk/arm64/sweet_early.c osfmk/arm64/start.s osfmk/arm/arm_init.c osfmk/arm64/arm_vm_init.c; do
    obj="$out/$(basename "$src").o"
    # shellcheck disable=SC2086
    $CC "$BT/xnu/$src" "$obj" $EXTRA
done

# 2. Replace those objects in a copy of the CI archive.
cp "$ARCHIVE" "$out/kernel-nord2.a"
for src in osfmk/arm64/sweet_early.c osfmk/arm64/start.s osfmk/arm/arm_init.c osfmk/arm64/arm_vm_init.c; do
    base="$(basename "$src")"
    ../toolchains/cctools/bin/aarch64-apple-darwin-ar d "$out/kernel-nord2.a" "${base%.c}.o" "${base%.s}.o" 2>/dev/null || true
done
../toolchains/cctools/bin/aarch64-apple-darwin-ar r "$out/kernel-nord2.a" \
    "$out/sweet_early.c.o" "$out/start.s.o" "$out/arm_init.c.o" "$out/arm_vm_init.c.o"
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

# 5. Loader. Three variants of the same source, in escalation order:
#    - loader-only-Image:  validate the FDT/payload, then chain to Android
#      (no segment copy, no XNU): isolates LK's decompression of the blob.
#    - copy-Image:         also copies the segments into the XNU window, then
#      chains: isolates the window writes.
#    - loader-Image:       the probe (runs XNU, stops after cpu_init).
#    Every variant leaves a pmsg log and returns to Android, even on failure.
build_loader() {
    variant="$1"
    extra="$2"
    for source in start.S loader.c; do
        clang --target=aarch64-none-elf -O2 -ffreestanding -nostdlib -fno-pic \
            -mgeneral-regs-only -mstrict-align -fno-stack-protector -fno-builtin \
            -Wall -Wextra -Werror $extra -c "shim/nord2-xnu/$source" -o "$out/$variant-$source.o"
    done
    ld.lld -T shim/nord2-xnu/xnu.ld --build-id=none "$out/$variant-start.S.o" \
        "$out/$variant-loader.c.o" -o "$out/$variant.elf"
    ld.lld -T shim/nord2-xnu/xnu.ld --build-id=none --oformat=binary \
        "$out/$variant-start.S.o" "$out/$variant-loader.c.o" -o "$out/$variant-Image"
}
build_loader loader ""
build_loader loader-only "-DNORD2_LOADER_ONLY=1"
build_loader copy "-DNORD2_COPY_ONLY=1"

# 6. Boot images and static verification.
for variant in boot loaderonly copy; do
    case "$variant" in
        boot) src=loader ;;
        loaderonly) src=loader-only ;;
        copy) src=copy ;;
    esac
    python3 tools/nord2-pack-xnu.py backup/nord2/boot.img "$out/$src-Image" \
        "$out/payload.bin" "$out/nord2-xnu-$variant.img"
    python3 tools/nord2-check-xnu.py "$out/nord2-xnu-$variant.img" "$out/payload.bin" | tail -4
done
