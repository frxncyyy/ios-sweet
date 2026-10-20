#!/bin/sh
# Build the Nord 2 probe boot image from a kernel built by the CI workflow
# (the README path: GitHub macOS runner, Apple clang, xnu/patches applied).
#
#   tools/nord2-image-from-kernel.sh KERNEL_MACHO OUT.img [--ramdisk IMAGE] [CMDLINE...]
#
# KERNEL_MACHO  out-XXXX/kernel.development.sweet (unstripped is fine)
# OUT.img       boot image to create (must not exist)
# --ramdisk     optional root image carried in RAM; supply rd=md0 in CMDLINE
# CMDLINE       optional override of the payload boot arguments
#
# Steps: strip the symbols, build the ADT, build the SWEETXNU payload, pack the
# boot image around the verified original header/ramdisk/DT and run the static
# checker. Nothing here touches a device.
set -eu
cd "$(dirname "$0")/.."

if [ $# -lt 2 ]; then
    echo "usage: $0 KERNEL_MACHO OUT.img [--ramdisk IMAGE] [CMDLINE...]" >&2
    exit 2
fi
kernel=$1
out=$2
shift 2
ramdisk=
if [ "${1:-}" = --ramdisk ]; then
    if [ $# -lt 2 ] || [ ! -f "$2" ]; then
        echo "--ramdisk requires an existing image file" >&2
        exit 2
    fi
    ramdisk=$2
    shift 2
fi
if [ -e "$out" ]; then
    echo "$out exists; remove it explicitly" >&2
    exit 1
fi

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

CT=../toolchains/cctools/bin/aarch64-apple-darwin
"$CT-strip" -S -x "$kernel" -o "$tmp/kernel.strip"
python3 tools/mkadt-nord2.py "$tmp/nord2.adt"
cmdline=${*:-"debug=0xc cpus=1 keepsyms=1 -noprogress nord2stop=0 nord2vm=1 nord2fb=1 nord2stopat=2"}
if [ -n "$ramdisk" ]; then
    python3 tools/mkpayload.py "$tmp/kernel.strip" "$tmp/nord2.adt" "$tmp/payload.bin" \
        --cmdline "$cmdline" --ramdisk "$ramdisk"
else
    python3 tools/mkpayload.py "$tmp/kernel.strip" "$tmp/nord2.adt" "$tmp/payload.bin" \
        --cmdline "$cmdline"
fi
python3 tools/nord2-pack-xnu.py backup/nord2/boot.img shim/out/nord2-xnu/loader-Image \
    "$tmp/payload.bin" "$out"
python3 tools/nord2-check-xnu.py "$out" "$tmp/payload.bin"
