#!/bin/sh
# Regenerate xnu/patches/0082-nord2-early-probe.patch from the edited CI
# buildtree, then check that it applies to the pristine CI sources and
# reproduces the buildtree exactly.
#
#   tools/nord2-mkpatch.sh
#
# Pristine sources come from research/ci-39158169212/xnu-buildtree/buildtree.tgz
# (the CI tree already carries patches 0001-0081). Add a file to FILES when
# the Nord 2 work starts touching it.
set -eu
cd "$(dirname "$0")/.."
BT=research/ci-39158169212/buildtree/xnu
TGZ=research/ci-39158169212/xnu-buildtree/buildtree.tgz
PATCH=xnu/patches/0082-nord2-early-probe.patch
FILES="osfmk/arm64/sweet_early.c osfmk/arm64/sweet_early.h osfmk/arm64/start.s osfmk/arm64/cpu.c \
osfmk/arm/arm_init.c osfmk/arm64/arm_vm_init.c osfmk/console/serial_console.c \
bsd/kern/bsd_init.c bsd/dev/sweet_ufs.c bsd/dev/sweet_usb.c bsd/dev/sweet_touch.c osfmk/arm64/sleh.c osfmk/kern/startup.c \
iokit/Kernel/arm/SweetBattery.cpp iokit/Kernel/arm/SweetFramebuffer.cpp iokit/Kernel/IOStartIOKit.cpp iokit/KernelConfigTables.cpp \
pexpert/pexpert/arm64/SWEET.h makedefs/MakeInc.def"

tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
# shellcheck disable=SC2046
tar xzf "$TGZ" -C "$tmp" $(for f in $FILES; do echo "xnu/$f"; done)

: > "$tmp/new.patch"
for f in $FILES; do
	diff -u --label "a/$f" --label "b/$f" "$tmp/xnu/$f" "$BT/$f" >> "$tmp/new.patch" || true
done

cp -r "$tmp/xnu" "$tmp/check"
(cd "$tmp/check" && patch -p1 -s < "$tmp/new.patch")
for f in $FILES; do
	cmp -s "$tmp/check/$f" "$BT/$f" || { echo "MISMATCH $f" >&2; exit 1; }
done
cp "$tmp/new.patch" "$PATCH"
echo "$PATCH: $(wc -l < "$PATCH") lines, applies cleanly, matches the buildtree"
