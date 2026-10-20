# Nord 2 Wi-Fi bring-up

## 2026-10-20: hardware/source identification; firmware collection prepared

Wi-Fi is not working yet. No new radio code has run on hardware. Preserve the
working input/display image (`fd6d75ca…49995f20`); it still runs on the phone.

The selected Android kernel at `work/nord2-current-kernel`, commit `04d00c25`,
selects CONNAC2X2_SOC3_0, HIF=AXI, WLAN_CHIP_ID=6893 in
`drivers/misc/mediatek/connectivity/Makefile`. This is an integrated radio, not
an SDIO peripheral. The Wi-Fi DT resource spans `0x18000000+0x700000`, with
GIC SPI 375 and 378. Android's real boot log identifies conninfra HW
`0x20010101`, A-die `0x66358a11`, RC mode, conninfra EMI at `0x8fb00000` and
MCIF EMI at `0x8a000000`. These low-memory ranges are outside XNU's current
`0x142000000..0x180000000` RAM window, but still require explicit ownership.

Primary source paths in that Android tree:

- `connectivity/conninfra/platform/consys_hw.c:consys_hw_pwr_on` gives the order:
  common PMIC rails, clock buffer, pinmux, power domain and AXI protection,
  chip ID, EMI mappings, D-die, SPI master, A-die, AFE calibration, PLL,
  subsystem status and low-power setup.
- `connectivity/conninfra/platform/mt6893/mt6893_pos.c` has the annotated register
  sequences, bounded status polls, SPI/A-die setup and calibration.
- `connectivity/conninfra/platform/mt6893/mt6893_pmic.c` controls VCN18, VCN13,
  VCN33_1/2 and RC-mode operation. Existing touch/key PMIF access must be
  serialized with future radio PMIF operations.
- `connectivity/wlan/core/gen4m/chips/soc3_0/soc3_0.c` contains WFSYS power,
  EMI ROM loading, WFDMA configuration, firmware names and download hooks.
  Its chip code has a GPL-2.0/BSD dual-license header; conninfra platform files
  have GPL-2.0 headers. Preserve attribution/license when porting source.

All these paths are relative to `drivers/misc/mediatek/`.
Do not read powered-off CONN MMIO without first checking AP-side power and
bus protection; a bounded software loop cannot recover a stalled bus access.

Android boot requested:

- `soc3_0_ram_wmmcu_1a_1_hdr.bin`
- `soc3_0_ram_wifi_1a_1_hdr.bin`
- `soc3_0_patch_wmmcu_1a_1_hdr.bin`
- `WIFI_RAM_CODE_soc3_0_1a_1.bin`

Private evidence: `backup/nord2/post-probe-dmesg.txt` around line 1874 onward.
The `/data/vendor/firmware/update` lookup fails first, then the driver succeeds
using the vendor fallback. Missing `wifi_sigma.cfg` is also seen in working
Android and is not proof that mandatory firmware is absent.

Existing `nvdata.img` contains `/APCFG/APRDEB/WIFI` (6146 bytes) and
`WIFI_CUSTOM` (6 bytes). Keep both private; use read-only debugfs, never repair
or modify the backup. Both were extracted to private
`research/nord2/wifi/calibration/` with source paths and SHA-256 manifest.
The on-disk NVRAM wrapper/checksum still needs matching against the Android
userspace loader before passing these bytes to firmware. The kernel receives
NVRAM through a userspace write into `g_aucNvram`; its configured maximum is
8192 bytes, not proof of the file's payload length.
The persist backup's `/firmware` directory is empty.
No vendor/super backup is present. Firmware must be collected from preserved
Android super; no reason to mount or rewrite iOS userdata.

`tools/nord2-read-vendor.py` is a read-only ADB collector for the tested RAM
rescue. It requires the rescue marker, root ADB and no data/vendor mounts,
validates AOSP LP geometry/header/table checksums and matching metadata copies,
checks extent/device bounds, and streams only vendor/odm into a private host
image. RAM use is bounded. It has no reboot, flash, disk write or mount command.
Running without `--partition` collects/prints metadata only. Tests:
`python3 tools/test-nord2-read-vendor.py`.

The exact tested rescue remains `research/nord2/rescue/nord2-adb-rescue.img`,
SHA-256 `d78bf9fe720a1d293b863ea0a8b30e0df4b5eafb097b1343a3c30e7c3d03bac5`.
Return image is `shim/out/nord2-xnu/nord2-ios-input-power-stop23.img`,
SHA-256 `fd6d75caf4c7e95431bdb40eb50ca79650e8272824f54fd5f783cdec49995f20`.
Neither has been flashed as part of Wi-Fi work yet.

Static iOS ABI evidence is saved privately in `research/nord2/wifi/ios-abi/`.
The IPSW's `/usr/sbin/wifid` (2606496 bytes, exact digest in identity.json)
has a required load command for `IO80211.framework/IO80211`. Its undefined
symbols include Apple80211Open, BindToInterfaceWithParams, Get/SetPowerState,
ScanAsync, AssociateAsync and EventMonitoringInit2. Strings reference
IO80211Interface and errors obtaining its BSD name. These are static load/
symbol/string observations only, not recovered user-client selectors or
observed runtime calls. Next ABI target is that framework in the same IPSW's
dyld cache. REA's existing doctor report shows no installed native analysis
engine; no engine or client configuration was changed. Existing llvm-objdump,
llvm-nm and strings supplied this limited evidence.

After firmware collection: implement/test conninfra cold power-on and ROM
ready first, then WFDMA firmware commands and scan, association/encryption,
packet transport and iOS Wi-Fi control integration. Existing Sweet ECM ifnet
code provides a BSD packet interface example, but does not implement Wi-Fi.
The XNU tree currently has no IO80211 driver/control implementation; exposing
an Ethernet interface alone would not make iOS Wi-Fi settings work.
