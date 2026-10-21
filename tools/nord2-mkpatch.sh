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
bsd/conf/files bsd/kern/bsd_init.c bsd/dev/sweet_ufs.c bsd/dev/sweet_usb.c bsd/dev/sweet_touch.c osfmk/arm64/sleh.c osfmk/kern/startup.c \
iokit/conf/files.arm64 iokit/Kernel/arm/SweetBattery.cpp iokit/Kernel/arm/SweetFramebuffer.cpp iokit/Kernel/IOStartIOKit.cpp iokit/KernelConfigTables.cpp \
pexpert/pexpert/arm64/SWEET.h makedefs/MakeInc.def"

# Canonical firmware sources live beside the shim for focused host tests.
# Generate guarded XNU copies so this patch also links without NORD2.
NEW_FILES="bsd/dev/wifi_firmware.c bsd/dev/wifi_firmware.h bsd/dev/wifi_firmware_probe.c bsd/dev/wifi_firmware_probe.h bsd/dev/wifi_emi_snapshot.c bsd/dev/wifi_emi_snapshot.h bsd/dev/wifi_emi_probe.c bsd/dev/wifi_emi_probe.h bsd/dev/ufs_two_lane.c bsd/dev/ufs_two_lane.h bsd/dev/ufs_two_lane_adapter.inc bsd/dev/zoom_reset.c bsd/dev/zoom_reset.h bsd/dev/wifi_common.c bsd/dev/wifi_common.h bsd/dev/wifi_common_adapter.inc bsd/dev/wifi_cold_snapshot.c bsd/dev/wifi_cold_snapshot.h"
NEW_FILES="$NEW_FILES bsd/dev/ufs_two_lane_live.inc bsd/dev/wifi_common_scope.c bsd/dev/wifi_common_scope.h bsd/dev/wifi_adie_id.c bsd/dev/wifi_adie_id.h bsd/dev/wifi_adie_strap.c bsd/dev/wifi_adie_strap.h bsd/dev/wifi_adie_adapter.inc bsd/dev/wifi_emi_legacy.c bsd/dev/wifi_emi_legacy.h"
NEW_FILES="$NEW_FILES bsd/dev/wifi_persistent.c bsd/dev/wifi_persistent.h bsd/dev/wifi_persistent_adapter.c bsd/dev/wifi_persistent_adapter.h bsd/dev/wifi_persistent_bind.inc"
NEW_FILES="$NEW_FILES bsd/dev/wifi_wake.c bsd/dev/wifi_wake.h bsd/dev/wifi_wake_adapter.c bsd/dev/wifi_wake_adapter.h bsd/dev/wifi_wake_bind.inc"
NEW_FILES="$NEW_FILES bsd/dev/wifi_wfsys.c bsd/dev/wifi_wfsys.h bsd/dev/wifi_wfsys_adapter.c bsd/dev/wifi_wfsys_adapter.h bsd/dev/wifi_wfsys_bind.inc"
NEW_FILES="$NEW_FILES bsd/dev/wifi_rom_contract.c bsd/dev/wifi_rom_contract.h bsd/dev/wifi_rom_plan.c bsd/dev/wifi_rom_plan.h bsd/dev/wifi_rom_place.c bsd/dev/wifi_rom_place.h bsd/dev/wifi_rom_xnu.c bsd/dev/wifi_rom_xnu.h bsd/dev/wifi_rom_powered.c bsd/dev/wifi_rom_powered.h bsd/dev/wifi_rom_powered_adapter.c bsd/dev/wifi_rom_powered_adapter.h bsd/dev/wifi_rom_bind.inc"
NEW_FILES="$NEW_FILES iokit/Kernel/arm/nord2_battery.c iokit/Kernel/arm/nord2_battery.h iokit/Kernel/arm/mtk_i2c_read.c iokit/Kernel/arm/mtk_i2c_read.h iokit/Kernel/arm/nord2_battery_platform.cpp iokit/Kernel/arm/nbg_telemetry.h"
NEW_FILES="$NEW_FILES bsd/dev/dma_pool_contract.c bsd/dev/dma_pool_contract.h bsd/dev/pool_probe.c bsd/dev/pool_probe.h bsd/dev/pool_adapter.c bsd/dev/pool_adapter.h bsd/dev/pool_bind.inc bsd/dev/wifi_dma_preflight.c bsd/dev/wifi_dma_preflight.h"
MAIN_FILES="wifi_rx_readback wifi_normal_transport wifi_normal_rx wifi_scan_bss wifi_scan_plan wifi_normal_config wifi_normal_init wifi_regulatory_policy wifi_normal_backend wifi_basic_config wifi_basic_resources wifi_basic_metadata wifi_basic_primitives wifi_capability wifi_capability_dispatcher wifi_main_command wifi_main_dmashdl wifi_main_init_plan wifi_main_initializer wifi_main_transport wifi_main_layout wifi_main_fw_place wifi_main_fw_xnu wifi_main_phy_backup wifi_main_phy_exchange wifi_main_phy_force wifi_main_phy_xnu"
for name in $MAIN_FILES; do
    NEW_FILES="$NEW_FILES bsd/dev/$name.c bsd/dev/$name.h"
    cp "shim/nord2-xnu/$name.h" "$BT/bsd/dev/$name.h"
    {
        printf '#include <arm64/sweet_early.h>\n#if defined(NORD2)\n'
        cat "shim/nord2-xnu/$name.c"
        printf '\n#endif /* NORD2 */\n'
    } > "$BT/bsd/dev/$name.c"
done
for name in wifi_firmware wifi_firmware_probe wifi_emi_snapshot wifi_emi_probe ufs_two_lane zoom_reset wifi_common wifi_cold_snapshot wifi_common_scope wifi_adie_id wifi_adie_strap wifi_emi_legacy wifi_persistent wifi_persistent_adapter wifi_wake wifi_wake_adapter wifi_wfsys wifi_wfsys_adapter wifi_rom_contract wifi_rom_plan wifi_rom_place wifi_rom_xnu wifi_rom_powered wifi_rom_powered_adapter dma_pool_contract pool_probe pool_adapter wifi_dma_preflight; do
    cp "shim/nord2-xnu/$name.h" "$BT/bsd/dev/$name.h"
    {
        printf '#include <arm64/sweet_early.h>\n#if defined(NORD2)\n'
        cat "shim/nord2-xnu/$name.c"
        printf '\n#endif /* NORD2 */\n'
    } > "$BT/bsd/dev/$name.c"
done

cp shim/nord2-xnu/ufs_two_lane_adapter.inc "$BT/bsd/dev/ufs_two_lane_adapter.inc"
cp shim/nord2-xnu/ufs_two_lane_live.inc "$BT/bsd/dev/ufs_two_lane_live.inc"
cp shim/nord2-xnu/wifi_common_adapter.inc "$BT/bsd/dev/wifi_common_adapter.inc"
cp shim/nord2-xnu/wifi_adie_adapter.inc "$BT/bsd/dev/wifi_adie_adapter.inc"
cp shim/nord2-xnu/wifi_persistent_bind.inc "$BT/bsd/dev/wifi_persistent_bind.inc"
cp shim/nord2-xnu/wifi_wake_bind.inc "$BT/bsd/dev/wifi_wake_bind.inc"
cp shim/nord2-xnu/wifi_wfsys_bind.inc "$BT/bsd/dev/wifi_wfsys_bind.inc"
cp shim/nord2-xnu/wifi_rom_bind.inc "$BT/bsd/dev/wifi_rom_bind.inc"
cp shim/nord2-xnu/pool_bind.inc "$BT/bsd/dev/pool_bind.inc"
for name in wifi_normal_transport_bind.inc wifi_normal_backend_bind.inc wifi_normal_backend_forward.inc wifi_normal_config_tables.inc wifi_basic_config_bind.inc wifi_capability_bind.inc wifi_async_event.h wifi_main_wfdma_native.h wifi_main_halt.h wifi_main_wfdma_bind.inc wifi_main_bind.inc failure_drain.inc; do
    cp "shim/nord2-xnu/$name" "$BT/bsd/dev/$name"
    NEW_FILES="$NEW_FILES bsd/dev/$name"
done
# Fuel helper TUs carry their own NORD2/host-test guards in canonical source.
for name in SweetBattery.cpp nord2_battery.c nord2_battery.h mtk_i2c_read.c mtk_i2c_read.h nord2_battery_platform.cpp nbg_telemetry.h; do
    cp "shim/nord2-xnu/$name" "$BT/iokit/Kernel/arm/$name"
done


tmp=$(mktemp -d)
trap 'rm -rf "$tmp"' EXIT
# shellcheck disable=SC2046
tar xzf "$TGZ" -C "$tmp" $(for f in $FILES; do echo "xnu/$f"; done)

: > "$tmp/new.patch"
for f in $FILES; do
	diff -u --label "a/$f" --label "b/$f" "$tmp/xnu/$f" "$BT/$f" >> "$tmp/new.patch" || true
done

for f in $NEW_FILES; do
    diff -u --label /dev/null --label "b/$f" /dev/null "$BT/$f" >> "$tmp/new.patch" || true
done

cp -r "$tmp/xnu" "$tmp/check"
(cd "$tmp/check" && patch -p1 -s < "$tmp/new.patch")
for f in $FILES $NEW_FILES; do
	cmp -s "$tmp/check/$f" "$BT/$f" || { echo "MISMATCH $f" >&2; exit 1; }
done
cp "$tmp/new.patch" "$PATCH"
echo "$PATCH: $(wc -l < "$PATCH") lines, applies cleanly, matches the buildtree"
