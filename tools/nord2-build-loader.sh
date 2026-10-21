#!/bin/sh
# Rebuild the loader from current sources. No device access or kernel relink.
# Usage: tools/nord2-build-loader.sh OUTPUT_DIRECTORY
set -eu
cd "$(dirname "$0")/.."
[ "$#" -eq 1 ] || { echo "usage: $0 OUTPUT_DIRECTORY" >&2; exit 2; }
dest=$1
mkdir -p "$dest"
tc=../toolchains/proton-clang/bin
for source in start.S loader.c wifi_rom_fdt.c; do
    "$tc/clang" --target=aarch64-none-elf -O2 -ffreestanding -nostdlib -fno-pic \
        -mgeneral-regs-only -mstrict-align -fno-stack-protector -fno-builtin \
        -Wall -Wextra -Werror -c "shim/nord2-xnu/$source" -o "$dest/$source.o"
done
"$tc/ld.lld" -T shim/nord2-xnu/xnu.ld --build-id=none \
    "$dest/start.S.o" "$dest/loader.c.o" "$dest/wifi_rom_fdt.c.o" \
    -o "$dest/loader.elf"
"$tc/llvm-objcopy" -O binary "$dest/loader.elf" "$dest/loader-Image"
