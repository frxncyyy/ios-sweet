#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
out=shim/out/nord2
defines=
packflags=
if [ "${1:-}" = "--chain-stock" ]; then
    out=shim/out/nord2-chain
    defines=-DCHAIN_STOCK=1
    packflags=--chain-stock
fi
mkdir -p "$out"
for source in start.S probe.c; do
    clang --target=aarch64-none-elf -O2 -ffreestanding -nostdlib -fno-pic \
        -mgeneral-regs-only -mstrict-align -fno-stack-protector -fno-builtin \
        -Wall -Wextra -Werror $defines -c "shim/nord2/$source" -o "$out/$source.o"
done
ld.lld -T shim/nord2/probe.ld --build-id=none "$out/start.S.o" "$out/probe.c.o" -o "$out/probe.elf"
ld.lld -T shim/nord2/probe.ld --build-id=none --oformat=binary \
    "$out/start.S.o" "$out/probe.c.o" -o "$out/Image"
python3 tools/nord2-pack-probe.py backup/nord2/boot.img "$out/Image" "$out/probe-boot.img" $packflags
