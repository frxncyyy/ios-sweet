# Nord 2 Wi-Fi bring-up

## 2026-10-20: conninfra power probe passed; console/display regression

Authorized power/fastlog image5d917ab1…5d773f06d flashed to boot and rebooted
at21:39:18CEST. Native USB appeared at21:40:13. The probe reached stage3:
HW ID20010101, configuration3, success1 and restored1. All8 SMP workers
passed; FT3518 and PMIC keys initialized correctly. This proves guarded
conninfra power/access/restoration; firmware/networking remain unimplemented.

GUI ownership took100.0895s from stage6, including58.4852s framebuffer work,
with zero scrolls and2343 pushes. Usermanagerd interval fell65.8542->7.4368s.
However first3 presents failed (done0); user confirms lower-screen corruption
and no lockscreen. Do not count this as a successful fast GUI boot. Private
initial log/hardware-result under wifi/power/; capture continues. Known-good
iOS fallback is already authorized; prepared restore runner awaits fastboot.

Old console push clears INTSTA with the wrong W1C convention, unlike GUI
refresh which correctly uses W0C, and starts even after a BUSY timeout.
Faster rendering may expose the stale-DONE/retrigger defect. Sol prepared
proper W0C/idle sequencing with bounded waits,10Hz fast-console throttling
and completion/timeout/skip counters. Default/panic output stays unthrottled.
Exact-C sanitizer tests, local compile and updated display regression pass;
patch0082 reproduces source. Full CI/image/hardware validation pending.


## 2026-10-20: snapshot hardware success, cold power probe prepared

The user authorized the exact snapshot image `bf0ea8bb…ba34fa88`. Boot-only
flash and reboot succeeded at21:05:38CEST. The image is currently running;
the exclusive runner must not be reused. All24 PMIC reads succeeded, eight
cores passed, eight initial GUI presents completed, and the user confirmed
lockscreen/touch/buttons work.

Measured SPM CONN_PWR_CON=0x12, PWR_STATUS/2ND=0x10580e01, RC_CFG1=0x401425;
AXI protection status=0xe70c500e/0xe7, GALS=0, reset=0. CONN is powered off
with isolation/clock-disable set. The bootloader left bank0 protection bit13
clear, bit14 set and bank2 bit2 set. VCN13/VCN18 control/monitor read0,
OP_EN=0x8000, OP_CFG=0; VCN13 analog selector0x202e=0x400 is1.3V. The read
addresses match **MT6359P** (`mt6885/upmu_hw.h`), not generic MT6359's map.
Raw24 values are saved in private `wifi/snapshot/hardware-result.json`.

USB first appeared only after the user reseated the cable. Three captures
then ended on EP0 timeout while iOS continued. Host client now has optional
bounded timeout/retry settings; tolerant capture uses5s ×6 attempts maximum,
at the same immutable read offsets. No new device write commands were added.

Next probe `nord2wifi=2` is implemented and CI-built, but not hardware-tested:
from a guarded cold RC-mode state, enable only VCN18/VCN13 at inherited
voltage, set radio GPIO172..182, enable GALS/CONN and check power/protection
acknowledgements before reading CONN sleep status/HW ID. Then isolate the bus,
turn CONN off and restore owned pin/rail/clock fields. It does not release
WFSYS CPU, load firmware, perform calibration/DMA or transmit. VCN33/voltage
selectors/modem/BT/GPS state are not programmed. Failed isolation/power-off
prevents dropping the rails and returning to Linux. Actual-C fault tests
cover cold guards, command bounds, partial-on failures, missing rails/acks,
bad ID/mapping, restoration failure/retry. Local compile and input regression
tests pass. The user also requested a Sol agent investigating slow boot;
its sweet_early.c change is integrated as opt-in `nord2fastlog=1`: circular
row clearing replaces expensive scrolling while preserving USB/pmsg logs.
New timing at GUI ownership distinguishes framebuffer processing from elapsed
time since stage6. The first-GUI marker precedes the first successful present;
it is not a lockscreen completion measurement. Exact-C20k-line sanitizer tests
and local compilation pass. No hardware speedup claim yet. Combined
[CI39482559337](https://github.com/frxncyyy/ios-sweet/actions/runs/39482559337)
passed at source67496ae. Kernel17265160bytes SHA-256
`74e135136ab6421887155f3396c5328a990d411d7c761aa0e8676004c506b10c`
matches artifact SHA256SUMS; all1096 captured compile-command lines use
generic ARMv8.2+crypto. Linked spot checks confirm PMIF writes/guards,
restoration refusal, circular console/panic fallback and timer placement.
RCpc2 scan2190749 instructions clean. Frozen image
`shim/out/nord2-xnu/nord2-ios-wifi-power-fastlog-stop23.img`:
33546240bytes,8192bytes below32MiB, SHA-256
`5d917ab14ea2412321026906a078d1b71de255f564afffca89974995d773f06d`.
Payload14505936bytes SHA-256
`bc44cc61598c64611ca88606a08cfd6497ca0f7a6597b90f22da1efefb7d784b`.
Same iOS23/display/input/eight-core arguments plus `nord2wifi=2 nord2fastlog=1`;
default15-iteration Zopfli. Static checks pass; independent unpacking confirms
original Linux/DT and tested RAM rescue are byte-exact. Manifest, verifier,
build log and syntax-checked exact-hash runner are private under `wifi/power/`.
Runner `--authorized-wifi-power-fastlog-boot` has NOT been executed.
Exact boot-only test authorization/fastboot is the next step. Capture uses
5s timeout and5 retries. Check HW ID20010101, restoration=1, GUI present/input
and raw timing counters; the marker precedes lockscreen completion.
This remains a domain-power test, not working Wi-Fi networking.
Final tolerant snapshot capture saved229445bytes with
no USB error; exact digest/results under private hardware-tolerant/.

## 2026-10-20: firmware acquired; first XNU diagnostic implemented

The authorized rescue/return sequence completed: exact rescue flashed only
boot at 20:44:05 CEST, then original working input/power iOS image restored
at 20:49:37. Both commands and reboots succeeded. The restored native USB
log confirms 8/8 SMP, HFS root, FT3518 ID54:52 and valid key reads. A retry
USB capture also records all first eight GUI presents done=1 and a touch
gesture. The preceding capture stopped on intermittent EP0 timeouts.
Private `research/nord2/wifi/run-approved-collection.py` has executed both
stages; do not rerun the exclusive runner.

Vendor logical partition: 544354304 bytes at super offset7708082176.
Metadata slot0 has matching primary and backup; super is11039408128 bytes.
Image SHA-256 `bf8de3f4d228c105cd950ac78cc1085bb3f65f9dfc52d4e0c8db10e51780b8b5`
matches a second independent on-phone read/hash. No phone filesystem mounted
or written. EROFS vendor uses LZ4 big pclusters; local erofs-utils1.9.4,
verified against the installed Arch keyring, successfully checked/decompressed
the firmware directory. Tools were unpacked locally, not installed globally.
Full result, raw metadata, rescue mount/log evidence, exact firmware hashes
and physical/compressed extent listings are under private `research/nord2/wifi/`.

| File | Extracted bytes |
| --- | ---: |
| soc3_0_ram_wmmcu_1a_1_hdr.bin | 49868 |
| soc3_0_ram_wifi_1a_1_hdr.bin | 296916 |
| soc3_0_patch_wmmcu_1a_1_hdr.bin | 5726 |
| WIFI_RAM_CODE_soc3_0_1a_1.bin | 973648 |
| conninfra.cfg | 32 |

Configuration contains `co_clock_flag=1` and `pre_cal_mode=3`; no tcxo_gpio
entry. Selected source chooses26MHz co-clock unless tcxo_gpio is nonzero.
Do not blindly apply the optional GPIO155 TCXO mux: that is volume-down on
this board. Android's clock-buffer callback says XO_WCN is hardware-controlled.
Selected platform power path uses `clk_prepare_enable`/runtime PM, so compare
its actual MT6893 clock-domain implementation with the alternative POS sequence
before porting power transitions.

Concrete discrepancy to preserve for the next stage: the selected
`drivers/clk/mediatek/Makefile` builds `clk-mt6885-pg.o` for
CONFIG_COMMON_CLK_MT6893. Its `spm_mtcmos_ctrl_conn` defines the second
protection-bank write/ack mask as `1 << 2` (`0x4`) and releases protections
in the order bank0 bit14, bank2 bit2, bank0 bit13. The alternative POS branch
writes `0x2` to bank2 but polls status bit2 and uses a different release
order. Do not copy that alternative literal into the selected clock-domain
sequence. Both SPM power-status acknowledgements use bit1; CONN_PWR_CON bits
0..4 are reset-release, isolation, primary-on, secondary-on, clock-disable.
Future power code must replace Linux's unbounded waits with explicit timeouts
and cleanup, and must not touch CONN MMIO until its access prerequisites hold.

Collector hardware correction: `adb exec-out` merges remote stderr with the
binary stream. Added Toybox `dd status=none`; the initial attempt failed its
exact-length check and did not save a corrupt image. Six host metadata tests
pass, and real geometry/header/table/hash validation now passes. Rescue ADB
state is `recovery`; `wait-for-device` waits for a different state.

The opt-in XNU `nord2wifi=1` diagnostic reads only AP-side SPM/AXI/GALS/reset/
GPIO state plus24 MT6359 clock/rail registers. It uses the input thread's
PMIF transport before polling, after `ni_saved` arms return cleanup. It does
not map the CONN register region, write PMIC values, load radio firmware, start
DMA, or transmit RF. The same guarded reader retains TOPSTATUS for keys.
No snapshot hardware test yet. Exact-C ASan/UBSan tests in
`tools/test-nord2-wifi.py` cover all65536 PMIC addresses, key behavior,
unowned/in-flight transactions,1000us timeout, AP-only MMIO, bootarg/mapping
gates and cleanup order. Local kernel C compile passes; patch0082 applies
cleanly and exactly reproduces the source. Full Apple-clang
[CI39478743302](https://github.com/frxncyyy/ios-sweet/actions/runs/39478743302)
passed at source4693483. Kernel17281112bytes, SHA-256
`0aa1672f5024ddd3411de2e180364d505880594bb5a42587f5cbd7d3462f9963`,
matching artifact SHA256SUMS; all1094 captured compile-command lines use
generic ARMv8.2+crypto. Linked RCpc2 scan of2189609 instructions passes.

Frozen `shim/out/nord2-xnu/nord2-ios-wifi-snapshot-stop23.img` is33544192bytes
(10240bytes spare), SHA-256
`bf0ea8bbbe8ea1de039f7199765a0daeb1d357c857999a0838674949ba34fa88`.
Payload14505920bytes, SHA-256
`75ae374a6d64427a2fe70539ffc370af367db6f869fcba4149a30bfe76446856`.
All static checks pass. Independent unpacking confirms the original Linux
kernel, both DT representations and tested rescue RAMdisk are byte-exact.
Linked disassembly confirms the read command/timeout, boot-argument gate,
cleanup ordering, AP mappings and24-read loop. Private `wifi/snapshot/`
contains the build log, manifest, verifier and guarded flash runner.
Hardware test and exact authorization have since completed as recorded above.
Normal iOS mounts its existing filesystem read/write, while this added
diagnostic only observes radio power state.
It neither enables the radio nor supplies networking.

Firmware headers were decoded against Android's ROM_EMI_HEADER and CONNAC
tailer definitions. The two ROM payloads target EMI offsets0x230000 and
0x2b0000. Main RAM code has five regions; the last two have the EMI flag,
targeting offsets0x31e000 and0x3c3800. All fit the reserved conninfra EMI
range `0x8fb00000+0x450000`. Its tailer CRC matches the bytes preceding it;
the ROMs' internal CRC fields have not yet been independently validated.

Private `firmware/download-layout.json` records those destination regions.
`firmware/verified-storage-layout.json` records the16 compressed/plain EROFS
extents for the four blobs, their offsets within vendor, stored SHA-256
digests and complete file digests. Independent bounded LZ4 decoding/plain
copy reproduces every file byte-for-byte against erofs-utils extraction.
This provides a potential way to load from preserved vendor without adding
1.3MiB to the nearly full boot image. No XNU vendor reader/decoder is
implemented yet: it would need validated super/GPT bounds, serialization with
the root UFS driver and stored/decompressed digest checks before use.

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
At this initial checkpoint there was no vendor/super backup. The subsequent
collection above obtained firmware from preserved Android super without
mounting or rewriting iOS userdata.

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
Both have since been flashed in the authorized collection/restore sequence
recorded above; do not repeat that sequence.

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
