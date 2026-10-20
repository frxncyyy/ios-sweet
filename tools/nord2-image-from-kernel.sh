#!/bin/sh
# Build the Nord 2 probe boot image from a kernel built by the CI workflow
# (the README path: GitHub macOS runner, Apple clang, xnu/patches applied).
#
#   tools/nord2-image-from-kernel.sh KERNEL_MACHO OUT.img [OPTIONS] [CMDLINE...]
#
# KERNEL_MACHO  out-XXXX/kernel.development.sweet (unstripped is fine)
# OUT.img       boot image to create (must not exist)
# --ramdisk     optional root image carried in RAM; supply rd=md0 in CMDLINE
# --base-boot   boot image providing the Linux kernel/ramdisk/DT (default backup)
# --zopfli      local Zopfli executable, for a smaller kernel gzip stream
# --product-adt raw iPhone X ADT for /product identity (required for Nord2 GUI)
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
base=backup/nord2/boot.img
zopfli=
product_adt=
while [ $# -gt 0 ]; do
    case "$1" in
        --ramdisk|--base-boot|--zopfli|--product-adt)
            if [ $# -lt 2 ] || [ ! -f "$2" ]; then
                echo "$1 requires an existing file" >&2
                exit 2
            fi
            case "$1" in
                --ramdisk) ramdisk=$2 ;;
                --base-boot) base=$2 ;;
                --zopfli) zopfli=$2 ;;
                --product-adt) product_adt=$2 ;;
            esac
            shift 2 ;;
        *) break ;;
    esac
done
if [ -e "$out" ]; then
    echo "$out exists; remove it explicitly" >&2
    exit 1
fi

# Reject the compiler-target regression observed in the two-core probe before
# doing any packing. This check does not establish full hardware compatibility.
python3 tools/nord2-check-rcpc2.py "$kernel"

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT

CT=../toolchains/cctools/bin/aarch64-apple-darwin
"$CT-strip" -S -x "$kernel" -o "$tmp/kernel.strip"
cmdline=${*:-"debug=0xc cpus=1 keepsyms=1 -noprogress nord2stop=0 nord2vm=1 nord2fb=1 nord2stopat=2"}
case " $cmdline " in
    *" nord2display=1 "*)
        if [ -z "$product_adt" ]; then
            echo "nord2display=1 requires --product-adt D22AP.adt" >&2
            exit 1
        fi ;;
esac
if [ -n "$product_adt" ]; then
    python3 tools/mkadt-nord2.py "$tmp/nord2.adt" --product-adt "$product_adt"
else
    python3 tools/mkadt-nord2.py "$tmp/nord2.adt"
fi
if [ -n "$ramdisk" ]; then
    python3 tools/mkpayload.py "$tmp/kernel.strip" "$tmp/nord2.adt" "$tmp/payload.bin" \
        --cmdline "$cmdline" --ramdisk "$ramdisk"
else
    python3 tools/mkpayload.py "$tmp/kernel.strip" "$tmp/nord2.adt" "$tmp/payload.bin" \
        --cmdline "$cmdline"
fi
set -- "$base" shim/out/nord2-xnu/loader-Image "$tmp/payload.bin" "$out"
if [ -n "$zopfli" ]; then
    set -- "$@" --zopfli "$zopfli"
fi
python3 tools/nord2-pack-xnu.py "$@"
python3 tools/nord2-check-xnu.py "$out" "$tmp/payload.bin" --stock "$base"
