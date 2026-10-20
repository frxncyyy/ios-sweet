# iOS 16 on sweet (Redmi Note 10 Pro, SM7150) - working notes


## Nord 2 active handoff — 2026-10-20 (this workspace)

- **COMBINED NEXT CANDIDATE CI39491596110/sourcece25bf3 RUNNING.**
  Includes opt-inRTCskip+extendedreadonlyUFSlimits+priorGUIretry/diagnostics
  andfirmwareRAMverification. Intendedimage nord2-ios-rtc-ufs-wifi-stop23.img;
  private rtc-ufs-wifi build/verify prepared, notpacked/flashed/authorized.
  Private recompressedrescuebase nowselected; defaultZopfli15kernelcompression.
  base.img28594176B SHAed05ea02d0499fe4109e6498a6852e769c346226aa4a56a220338673f8a2f5bf,
  gains290816B aligned. ExactCPIO19476076B SHA
 95989a2ddff7d3fad8c63df9916a7d2510e09499f0b1710ac9fbf078ce60979f.
  Independentheader/ID/parser verifiesLinux/bothDTs/allCPIOcontentsunchanged;
  only gzipcontainer/headerRAMdisksize+ID differ. ContainerhardwareUNTESTED,
  originaltestedrescuefilepreservedforfallback. Imagepending, noimageclaims.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39491596110
  Isolated wifi_emi_snapshot.{c,h} prepared withhosttests/nativeARMcompile:
  exactSMCstructuredREAD, preservesrawreturns, validatesselectors/groups/
  regions, maximum128queries, stopsunsupported, neverSET/CLEAR/MMIO writes.
  Notlinked/intoboot, noSMCexecuted, noeffectivepermissionclaim/deadlineclaim.
  Private emi-permission-plan.md documentslimitations. RootreviewPASS.

- **MEASURED: UFS G4x1 RATE B; NEXT RTC-SKIP/LIMITS SOURCE READY.**
  Authorized8eb3f477 boot reports all6DME_GET valid0x3f: RX/TXgear4,1laneeach,
  HSseries2,pwrmode0x11, noerror/pending. BeforeGUIownership:57793commands,
 51995reads/5586writes/210syncs,2462855168B read/92409856B write,
  cmd126888358ticks=9.76064s,doorbell120998182ticks=9.30755s@13MHz,
  errors0/timeouts0. This is aggregate command time, NOT full boot throughput.
  Console W0C/rate fix:423completedpushes,0busy/frame timeouts,2183skips;
  framebuffer4.82882s vs58.4852s prior; GUIownership93.68362s (notlockscreen).
  First2presentsdone1; userstillseesconsole andpowerdims. No enabledlayerlogs.
  Hardware snapshot+SHA/result under ufs-profile/. Final600scapture956649B,
  SHA1fefffbc13910364f013d287b2f22d1884acfe83e782a03aa0b40f431e7178ec; clientexit0.
  Phone leftinXNU; no nextflash/rollbackexecuted.
  MissingIORTC sourcewait30s found beforeuserspace. New opt-in nord2nortc=1
  skips only that wait, retains clock_initialize_calendar andlogsrawtiming.
  ExactC++hostmodel/default/skip/timeout/callorder +localcompile PASS.
  UFSprofile now also reads8allowlistedlocalGETs (available/connectedRX/TX,
  terminations,UniProversion,TXadapt) andrawAHIT/HCS/IS. First6fieldsunchanged;
  separateLIMITSvalid/understood/error/result output, pendingrecoveryretained.
  ExactCsanitizers+localcompile+vendorregression+peerreview PASS. NoSET/tuning.
  Nextimage includes these plus existinge3a9afaGUIretry/diag/vendorprobe.
  CI39490076167/e3a9afa PASSED; notpacked/flashed, supersededbynextcandidate.
  Hypotheticalx2IOtime improvement savesatmostabout4.9s ofmeasured93.7s;
  driver/callerCPUwaits andactualcriticalpath stillunmeasured. Futurex2test
  must be before rootpublication/writableenable; noSETimplementation yet.

- **UFS PROFILE IMAGE FLASHED; HARDWARE CAPTURE RUNNING.**
  User authorized exact8eb3f477…ec01dfe8 and suppliedfastboot. Guarded
  ufs-profile/run-approved-boot.py EXECUTED: boot-only flash/reboot OKAY at
 22:17:12CEST. Never rerun this exclusive runner. WaitingnativeUSB then
  captureshardware-live; do not assume lsusb fastboot label meansfastboot.
  Later CI39490076167/sourcee3a9afa remains independent and includes vendor
  extraction/GUIretry/diagnostics; not onphoneyet. No normalAndroid boot.
  NextWiFi pureROMplanner prepared/tested/reviewed, not kernelintegrated:
  shim/nord2-xnu/wifi_rom_plan.{c,h} +test. FullfileSHA+exact48Bheader targets,
  fourbounded/disjoint payload/datecode spans; no destinationaccess or CRC
  validityclaim. Separate private next-hardware-plan.md and
  emi-permission-plan.md document remaining ownership/MPU/WFSYS prerequisites.
  Selected Android emimpu_old uses structured READ SMC C200050B, not rawMPU
  reads. Minimal8region27/31 reads are diagnosticonly; effectivepermission
  proof needs overlappingregions/priority. No SMC implementation/hardwaretest.

- **NEXT SOURCE: VENDOR FIRMWARE VERIFY + GUI OWNERSHIP RETRY.**
  Canonical loader committed b7af65b. Vendor API and isolated probe now wired
  into XNU through generated guarded sources in patch0082/bsd/conf/files.
  nord2wififw=1 starts a separate thread only after su.ready/root publication
  and UFS mutex release. Allocates about1.4MiB with Z_NOPAGEWAIT, validates
  four files, logs metadata/timing, wipes/frees and terminates. No EMI/CPU/RF
  writes. API/loader/probe sanitizer tests, three local C compiles and peer
  review PASS; backup GPT fixture caveat remains. Full CI39490076167/sourcee3a9afa dispatched/running.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39490076167
  Display source/model proves first ownership failure consumed fDirty: now
  restore dirty under fLock without replacing any newer pending record;
  timer cadence retries. Bounded nord2fbdiag=1 reports eight swap records
  and limited counters (copy/empty/missing/ownership/timer/power), defaultoff.
  Model covers retry with no new swap and concurrent newer frame; local C++
  compile+display regression PASS. This does NOT prove cause of empty GUI.
  b3a8 is uncompressed RGB10 + alpha plane, not BGRA; no conversion added.
  Root regenerated0082 (5808lines), exact reproduction PASS. Evidence under
  research/nord2/{ufs-vendor,fb-diagnostics,wifi/kernel-extraction}/.
  Profile-only CI39488497958/source242e41f PASSED (separate candidate): kernel
  SHA feb127944a6f3a867ede0aa703518aded39080729e8e31ca79bd8dc0bc56efe9,
  17266104B, all1096 compile-command lines genericARMv8.2+crypto. Frozen image
  shim/out/nord2-xnu/nord2-ios-ufs-profile-fastlog-stop23.img:33550336B,
  SHA8eb3f47705826255fb8841327ce8dd504caf7b5f90e89eb05f6c2864ec01dfe8,
 4096B spare. Static checker/RCpc2/independent unpack PASS; originalLinux,
  bothDTs and testedrescue unchanged. Private ufs-profile/{manifest.json,
  verify-image.py,run-approved-boot.py}; runner NOT EXECUTED, exact new user
  authorization needed. This first image includes UFS measurement and console
  W0C/10Hz fix, NOT later GUI-retry/firmware probe (CIe3a9afa).
  Latest fallback capture restored-ios-followup470975B: present1done0 then
  present2done1/overlay removed afterphysicalpower, no enabled swaplayerlogs;
  userstillseesconsole, no lockscreen confirmation.

- **USER PRIORITY: UFS/WI-FI AND FAST BOOT; UFS PROFILE PREPARED.**
  User explicitly authorizes parallel Sol agents. boot_speed implemented
  opt-in nord2ufsprofile=1: six allowlisted local DME_GETs report actual
  RX/TX gear/active lanes, HS series and power mode; aggregate command/read/
  write/sync counts, bytes, command/doorbell total/max ticks, errors/timeouts.
  No link/PHY/clock/voltage/media tuning. UIC writes only GET command/args and
  own W1C completion; bounded100ms/100k polls; pending completion retained
  across SCSI acknowledgement and blocks unsafe Linux restoration.
  Runs under existing UFS ownership/mutex after takeover/GPT; reports once
  at first GUI after copying stats and unlocking. Root integrated header and
  GUI hook after console release; display host test verifies placement.
  Actual-C sanitizer tests and both local translation-unit compiles PASS;
  patch0082 exact reproduction PASS. Evidence research/nord2/ufs-profile/.
  Command ticks exclude caller mutex wait/bounce memcpy; doorbell time includes
  polling/scheduling. No hardware UFS speed result yet. Full CI39488497958/source242e41f running.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39488497958
  Console-only correction CI39486360460/source8c3ca35 PASSED, artifact saved;
  next hardware candidate will include that fix plus UFS measurement.
  Android logs prove G4/two lanes/rateB after Linux negotiation (private
  backup/nord2/logs/20261020-124040-usb-phy-stop15-retry/dmesg.txt:2247),
  not XNU's inherited state. Nord2 does NOT start persistent UFS logger
  (sole call guarded !NORD2); no periodic logger flush cost on this boot.
  Wi-Fi memory loader prepared in separate shim/nord2-xnu/wifi_firmware.c/.h:
 16 stored extents/four reconstructed hashes, bounded LZ4, caller buffers,
  no embedded firmware/hardware access. Host real-vendor/corruption/sanitizer
  tests pass and peer review finds no blocking issue. Not kernel-integrated;
  verified GPT/super/LP vendor-read API now prepared in active sweet_ufs.c,
  separately tested/reviewed under research/nord2/ufs-vendor; not in CI242e41f.
  Gated nord2wififw=1, max64KiB, pins both GPT/LP metadata copies, retains
  root bounds/LUN/write policy; read failure leaves output untouched. Backup
  GPT test fixture is synthesized, hardware validation remains pending.
  Loader local kernel compile passed; firmware probe bridge is being prepared.

- **CONN POWER/ID/RESTORE PASSED; FASTLOG DISPLAY REGRESSION, FIX PREPARED.**
  User authorized exact5d917ab1…5d773f06d boot-only test and supplied fastboot.
  Flash/reboot both OKAY at21:39:18CEST. Power runner EXECUTED; never rerun
  exclusive flash log. Power/fastlog capture is saved
  under private wifi/power/hardware-live/; image has since been replaced by fallback. Radio stage3 ID20010101 cfg3
  success=1/restored=1, all8 SMP PASS, FT3518/key initialization succeeds.
  First-GUI ownership100.0895s since stage6; framebuffer58.4852s (58.43%),
  scrolls0/pushes2343. This is NOT lockscreen timing: first3 presents done=0,
  inherited-overlay removal absent. User confirms no lockscreen and lower
  screen becomes noisy after about1s of boot logs (photo214007).
  UMD interval65.8542->7.4368s; launchd early tasks72.8737->9.40794s.
  Private initial evidence356955bytes SHA
  870f85a886632abe9e38955a62d21bb7d93fd156467b0eac8c27bc3203e025a2.
  Final capture773212bytes SHA
  780cae64d85ea36f8dac76399696c1528a23193472c7e4ced743f763ef1941aa.
  Capture ended on USB disconnect(libUSB-4), not a recorded kernel panic;
  hardware-result.json marks display regression.
  Verified fallback runner wifi/power/restore-working-ios.py EXECUTED after
  phone appeared in fastboot. Working fd6d75ca…49995f20 image flashed to boot
  and rebooted OKAY at21:50:16CEST; restored-ios-live capture230762B complete.
  User confirms console still visible, GUI not restored. Followup USB capture
  running; physicalpower display-on eventually gives present done1 and overlay
  removal, but NO swap-layer logs. Backboardd now creates960x2079 BGRA and
  b3a8 surfaces; earlierworkingfd6 used1125x2436 BGRA. Cause unestablished.
  Device is still XNU (USB18d1:d001 shared by our console; lsusb product label
  alone must NOT be mistaken for fastboot; fastboot devices is empty). Both
  exclusive runners have executed; DO NOT rerun. Preserve userdata; never
  normal Android.
  Source defect found: old console push used W1C semantics for W0C INTSTA,
  did not clear stale FRAME_DONE before START, and retriggered despite BUSY
  timeout. Faster logging may expose this race; causality not yet isolated.
  Sol correction now in sweet_early.c: proper W0C, no START when BUSY,10ms
  idle/20ms completion deadlines plus hard iteration bounds; fast-mode10Hz
  rate limit, default/panic unthrottled. Adds completion/timeout/skip counters.
  Exact-C ASan/UBSan MMIO/time tests and local ARMv8.2 compile PASS; display
  regression model updated for one-time GUI timing after lock release, PASS.
  Patch0082 regenerated/exact reproduction PASS. Commit8c3ca35 pushed;
  CI39486360460 PASSED; next image adds UFS profiling.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39486360460
  UFS maximum speed is UNVERIFIED; user asks to investigate. No UFS tuning
  writes or benchmark claim. Source audit: inherits LK link/PHY/clocks, no
  gear/lane/HS-rate measurement or negotiation. Controller advertises32
  slots, XNU uses slot0/global mutex/1MiB bounce and polling. Android requests
  up to HS-G4/two lanes/rateB; this is not the measured inherited state.
  Next UFS measurement: aggregate command/doorbell latency, counts and bytes;
  DME_GET link-state probe separately bounded. ufscache=0 selects CPU DMA
  maintenance, not the UFS device-cache state.

- **WI-FI SNAPSHOT PASSED; POWER/FASTLOG IMAGE PROVENANCE.**
  User authorized bf0ea8bb…ba34fa88; boot-only flash and reboot succeeded
  21:05:38CEST. Private wifi/snapshot/run-approved-boot.py EXECUTED; do not
  rerun exclusive flash log. This snapshot has since been replaced by the authorized power/fastlog test.
  All24 PMIC reads succeeded; all8 cores PASS, FT3518/key initialization OK.
  Eight GUI presents done=1; user confirms lockscreen, touch and buttons work.
  CONN power=0x12, both status=0x10580e01 (CONN bit1 off), RC=0x401425.
  AXI protection=0xe70c500e/0xe7, GALS=0, reset=0. VCN13/VCN18 CON0/MON=0;
  OP_EN=0x8000, OP_CFG=0. VCN13 selector0x202e=0x400 is1.3V.
  These are MT6359P/mt6885 upmu_hw.h addresses; generic mt6359/registers.h
  has a different layout and MUST NOT be used for writes.
  USB initially absent until user reseated cable; subsequent EP0 timeouts
  interrupted three captures. Tolerant capture uses new optional host
  --timeout-ms5000 --timeout-retries5 (max30s/request, read-only immutable
  offsets; defaults unchanged). Logs/results under wifi/snapshot/.
  New nord2wifi=2 probe in sweet_touch.c prepares only VCN18/VCN13 RC-mode
  enable, radio GPIO172..182, GALS/SPM CONN domain; checks AP acknowledgements
  before CONN sleep/ID reads. No firmware, WFSYS CPU release, DMA or RF.
  Preserves voltage settings, refuses non-cold/RC/1.3V state, restores only
  owned bits. Failed power-off/protection restoration retains rails and
  prevents Linux return. Exact-C ASan/UBSan fault tests and local C compile
  PASS; input regression/fuzz PASS. Hardware test now running above.
  User additionally requests faster boot (5–6min) and explicitly requested
  Sol agent boot_speed completed sweet_early.c performance work: opt-in
  nord2fastlog=1 clears two circular console rows instead of copying31 rows,
  reducing per-newline framebuffer traffic from8570880 to276480bytes.
  Default/panic scrolling and full USB/pmsg logs remain. New NORD2 BOOT
  first-GUI marker reports elapsed_ticks/fb_ticks/freq/scrolls/pushes at
  GUI ownership, before first successful present; this is NOT a lockscreen
  completion timestamp. Counts begin at stage6 fb_init. No measured speedup
  yet: usermanagerd task spans65.854s in calendar-relative logs, which
  do not establish hardware uptime or a fixed timeout.
  Exact-C fastlog sanitizer test20000 lines and local compile PASS. Root
  regenerated0082 against pristine tree and verified exact reproduction.
  Combined CI39482559337/source67496ae PASSED.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39482559337
  Kernel17265160bytes SHA-256
  74e135136ab6421887155f3396c5328a990d411d7c761aa0e8676004c506b10c
  matches artifact SHA256SUMS. All1096 captured compile-command lines use
  generic ARMv8.2+crypto. Linked spot checks confirm PMIF command/slot bounds,
  restoration refusal, fastlog circular rows/panic fallback and GUI timing
  after console release; private wifi/power/critical-functions-check.json.
  Linked RCpc2 scan2190749 instructions clean. Frozen image:
  shim/out/nord2-xnu/nord2-ios-wifi-power-fastlog-stop23.img
  33546240bytes (8192bytes spare), SHA-256
  5d917ab14ea2412321026906a078d1b71de255f564afffca89974995d773f06d.
  Payload14505936bytes SHA-256
  bc44cc61598c64611ca88606a08cfd6497ca0f7a6597b90f22da1efefb7d784b.
  Same full iOS23/display/input/eight-core command line with nord2wifi=2
  and nord2fastlog=1; default15-iteration Zopfli. Static checker all PASS.
  Independent unpack verifies original Linux, both DT representations and
  tested rescue RAMdisk byte-exact. Private wifi/power/{manifest.json,
  verify-image.py,image-build.log,run-approved-boot.py} prepared.
  Guarded runner --authorized-wifi-power-fastlog-boot EXECUTED after exact
  user authorization; flash/reboot success recorded above.
  Capture uses5s timeout/5 retries; preserve current snapshot logs/baseline.
  Success requires power ID20010101/restored=1, GUI presents and working
  touch/buttons. Use raw NORD2 BOOT counters for measured console/elapsed
  time; user still needs to confirm lockscreen time. Radio networking is
  not implemented yet. On failure use tested rescue/working iOS fallback,
  never normal Android because userdata is the installed iOS filesystem.
  Tolerant final snapshot capture
  229445bytes SHA718cc62aee9d764ed05248974000e1d838f7159a23bc34859c9ee2fe4026d8c1
  ended at deadline without USB error; historical small captures remain.

- **WI-FI FIRMWARE COLLECTED; WORKING iOS BOOT RESTORED.**
  User authorized exact rescue -> read vendor -> restore iOS sequence.
  Both boot-only flashes/reboots succeeded, at20:44:05 and20:49:37CEST.
  Private research/nord2/wifi/{rescue,return-ios}-flash.log; exclusive runner
  has executed both stages, DO NOT rerun it. No other partition written.
  Read vendor544354304bytes from super offset7708082176; primary/backup LP
  SHA-256 validation and independent on-device full-vendor SHA-256 agree:
  bf8de3f4d228c105cd950ac78cc1085bb3f65f9dfc52d4e0c8db10e51780b8b5.
  Vendor is EROFS, extracted with local signature-verified Arch erofs-utils;
  firmware-directory decompression/integrity check passes. Four required
  firmware blobs plus conninfra.cfg saved privately with SHA/layout manifests.
  cfg: co_clock_flag=1, pre_cal_mode=3. See WIFI-LOG.md for sizes and locations.
  Collector fixed: adb exec-out merges remote stderr, so dd needs status=none.
  Initial metadata read correctly rejected excess bytes; no corrupt image saved.
  RAM rescue reports ADB state recovery: use wait-for-recovery, not wait-for-device.
  Restored iOS USB log confirms 8/8 SMP PASS, HFS root and FT3518/key reads OK.
  Retry capture confirms all first8 GUI presents done=1 and a touch gesture.
  Private restored-ios-live/, restored-ios-gui/ and restored-ios-gui-retry/
  hold boot evidence. Earlier capture ended on intermittent USB timeouts.
  First XNU Wi-Fi diagnostic implemented and now tested above: nord2wifi=1
  snapshots AP-side power/bus/pinmux and24 PMIC rail/clock registers.
  Runs serially on the input thread before polling, after fallback cleanup
  is armed; no CONN MMIO, PMIC value writes, firmware/DMA or RF transmission.
  Exact-C ASan/UBSan host checks PASS all65536 PMIC addresses, key behavior,
  timeout/ownership, AP address allowlist, bootarg gates and cleanup order.
  Local C compile PASS; 0082 regenerated and verified against pristine tree.
  Full Apple-clang CI39478743302/source4693483 PASSED:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39478743302
  Kernel17281112bytes SHA-256
  0aa1672f5024ddd3411de2e180364d505880594bb5a42587f5cbd7d3462f9963,
  matches artifact SHA256SUMS. All1094 captured compile-command log lines
  use generic ARMv8.2+crypto. Linked RCpc2 scan2189609 instructions clean.
  Frozen shim/out/nord2-xnu/nord2-ios-wifi-snapshot-stop23.img:
  33544192bytes (10240bytes spare), SHA-256
  bf0ea8bbbe8ea1de039f7199765a0daeb1d357c857999a0838674949ba34fa88.
  Payload14505920bytes SHA-256
  75ae374a6d64427a2fe70539ffc370af367db6f869fcba4149a30bfe76446856.
  Same userland23/display/input command line plus nord2wifi=1; default15
  Zopfli. Static checker all PASS; independent verification confirms original
  Linux, both DT representations and tested rescue RAMdisk byte-exact.
  Linked PMIF read and inlined snapshot disassembly confirms read opcode,
  INIT_DONE/timeout, opt-in gating, cleanup order, AP maps and24-read loop.
  Private wifi/snapshot/{manifest.json,verify-image.py,image-build.log,
  critical-functions-check.json,linked-*.txt,run-approved-boot.py} saved.
  Guarded runner --authorized-wifi-snapshot-boot EXECUTED after exact user
  approval. Snapshot/GUI/input hardware result is recorded above.
  If a later boot fails, use tested rescue and restore working iOS;
  never boot normal Android.
  This image does not enable the radio or implement Wi-Fi networking.
  Firmware ROM headers and five RAM-code region descriptors parsed privately.
  Main RAM-code tailer CRC matches. All16 vendor file extents independently
  decoded with liblz4/plain copy and match erofs-utils byte-for-byte.
  Private firmware/{download-layout,verified-storage-layout}.json records
  target EMI addresses and compressed source offsets/hashes. This establishes
  a possible read-only vendor loader path, not an implemented XNU loader.
  It may avoid embedding1.3MiB firmware in the almost-full boot image.

- **WI-FI INITIAL RESEARCH (collection and restore now completed above).**
  Exact driver is MT6893 CONNAC2X2/SOC3_0 over AXI, conninfra HW20010101,
  A-die66358a11 in RC mode. Selected Android source includes full conninfra and
  gen4m driver; XNU currently lacks the radio and iOS Wi-Fi control interface.
  Calibration WIFI6146/WIFI_CUSTOM6 bytes extracted read-only from existing
  nvdata backup into private research/nord2/wifi/calibration; not committed.
  Exact four firmware names and power-on order in shim/nord2-xnu/WIFI-LOG.md.
  Preserved Android vendor in super supplied the verified local image above.
  New tools/nord2-read-vendor.py validates LP checksums/copies/bounds and
  streams vendor/odm from RAM rescue without phone writes or mounts.
  Six corruption/boundary tests PASS. Known rescue d78bf9fe…3d03bac5 and
  working iOS fd6d75ca…49995f20 hashes reverified. Both stages of private
  research/nord2/wifi/run-approved-collection.py have EXECUTED after explicit
  user approval/fastboot. The first read-only Wi-Fi snapshot is now tested above.
  Static inspection of local iOS wifid confirms required IO80211.framework
  and Apple80211 scan/associate/power/event API imports; no selectors recovered.
  Private ios-abi/ holds exact identity/import/string evidence. REA native
  engine unavailable per existing doctor; used existing LLVM tools only.


- **MILESTONE: TOUCH, POWER, BOTH VOLUME BUTTONS WORK; ORANGE OVERLAY GONE.**
  User confirms "all work" after testing swipe/taps, volume-up/down and short
  power press/wake on nord2-ios-input-power-stop23.img. This is the current
  working baseline, left running on the phone. Preserve image and evidence;
  do not restore/reboot/reinstall without a concrete next task.
  Captured PMIC key0(power)/key1(up) down/up, display off/on in response,
  16 touch transitions with changing coordinates, Sweet HID dispatch and
  subsequent exec Preferences. All first8 GUI presents done=1; all8 cores
  passed scheduler probe. Volume-down confirmed by user on both input images.
  User separately confirms stale LK orange warning is gone from lockscreen.
  Final60-second USB capture243188bytes ended at deadline, no transport error
  or captured kernel panic. SHA-256
  07d4dd004741de23b7f10f19ff1656786e74ad83cc8e6d6a8c7986e148ad9e1f.
  Private research/nord2/input-power/hardware-result.json and hardware-gui/
  capture record the completed interaction test. Earlier captures ended on
  intermittent EP0 timeouts and were restarted without reboot. Idle touch
  packet rejections still appear; valid gestures work, raw idle-packet cause
  unknown. Multitouch/long-term stability were not established by this test.

- **INPUT CORRECTION IMPLEMENTATION/PROVENANCE (hardware success above).**
  On the first input image only volume-down worked; power/up/touch failed.
  First input boot d74b1b18…5a9e59f6 successfully runs iOS and reports GPIO155
  press/release events. FT3518 ID is00:00/not ready; PMIC reads fail/status0.
  Both inherited LK layers were disabled and first eight GUI presents done=1
  in live logs; user now confirms the orange warning is gone over the UI.
  Evidence research/nord2/input-ui/hardware-live{,-late}/, flash.log.
  Cause of PMIC failure identified in selected Android kernel source:
  drivers/soc/mediatek/mtk-pmic-wrap_v1.c uses MT6885 PMIF SWINF2,
  NOT the unused legacy WACS2 HAL. New read-only TOPSTATUS command0x2a uses
  ACCc80/RDATAc94/VLD_CLRca4/STAca8, FSM bits1..3, INIT_DONE bit15.
  Touch rail availability was unknown; corrected driver checks MT6360 PMU
  ID0x34:0 and CRC-protected LDO2 on I2C5/GPIO202/203 address0x64.
  Snapshots enable0x11/selector0x15, enables only VTP at board DT3.0V (0xaa),
  preserves an already enabled3.0V rail, validates every write by readback.
  Uses the vendor CRC8 polynomial07 protocol including dummy write byte.
  Restores original voltage/enable before rescue; failures refuse Linux return.
  All seven pin fields/four IES masks restored; other rails untouched.
  Exact-C ASan/UBSan tests cover CRC, identity/write allowlist, inherited rail,
  enable/readback/restore, failed-write restoration refusal, PMIF/transport
  timeouts, report decoding/fuzz. Latest evidence research/nord2/input-power/.
  Full Apple-clang CI39468314328/source066ccc735f366b5df58b7dbe46d4239e15e183b0
  PASSED: https://github.com/frxncyyy/ios-sweet/actions/runs/39468314328
  Kernel17264664bytes SHA-256
  d2aceb52a2cdb445a8d65fb804bfa6f02bbea5a13ffff1232a48e587b2a27aa5,
  matching artifact SHA256SUMS. All1097 captured compile-command log lines
  carry generic ARMv8.2+crypto (parallel log output is interleaved; this is
  not a translation-unit count). Linked RCpc2 scan2189365 instructions clean.
  Disassembly confirms PMIF offsets/INIT_DONE/raw read address/bounded poll
  and three short-circuit steps when restoring VTP.
  First image's final ten-minute capture235875bytes ended at deadline with
  no USB transport error or captured kernel panic; SHA-256
  4cf4e5dadfaabb101bfde7040858d8a93226abbed77f769baf7086fb3ef1d2ff.
  Frozen shim/out/nord2-xnu/nord2-ios-input-power-stop23.img:
  33544192bytes (10240bytes spare), SHA-256
  fd6d75caf4c7e95431bdb40eb50ca79650e8272824f54fd5f783cdec49995f20.
  Payload14505920bytes SHA-256
  311147857b1e624ba820b592000383b836ded9a9c62097b71b914552ef72f1d0.
  Same full-userland23/input/display cmdline; default15-iteration Zopfli.
  Static checker all PASS; independent verification confirms original Linux,
  both DT representations and tested rescue RAMdisk byte-exact.
  Private research/nord2/input-power/{manifest.json,verify-image.py,
  image-build.log,critical-functions-check.json,linked-functions.txt,
  run-approved-boot.py} saved. User explicitly authorized exact boot-only
  image and supplied fastboot. Runner --authorized-input-power-boot EXECUTED:
  flash boot and reboot OKAY,2026-10-20 20:09:33CEST. No other partition
  flashed; native USB capture starts in hardware-live/client-hardware.log.
  Do not rerun exclusive runner or overwrite logs. Hardware interaction now
  succeeds as recorded in the milestone above.
  Initial hardware evidence: all8 scheduler PASS; MT6360 ID0x53, inherited
  VTP enable0x80/selector0x40 (disabled), after setup status0xc4/3.0V ready.
  FT3518 now reports54:52 ready=1 powered=1, PMIF TOPSTATUS0xe ok=1.
  Some touch reports rejected, mostly IRQ high/idle; exact packet cause is
  not yet known. User completed swipe, volume up/down and short power/wake
  testing and confirmed all work. No captured kernel panic.

- **FIRST INPUT/OVERLAY IMAGE PROVENANCE (results above).**
  User now requests touchscreen and buttons, and reports LK orange warning
  remains over the UI. New opt-in nord2input=1 driver in sweet_touch.c reads
  FT3518 via GPIO204/205 software I2C, reset20/IRQ21; decodes Oplus x4 reports
  into existing Sweet HID. Power and volume-up are MT6359 TOPSTATUS bits1/3
  through read-only WACS2; volume-down GPIO155. Do not use keypad DT's
  misleading volume-up GPIO20 (Linux uses PMIC HOMEKEY instead).
  No PMIC voltage/enable writes: inherited MT6360 LDO2 VTP availability still
  needs a hardware ID read. GPIO/IES/PWRAP return cleanup added after SMP off.
  Display validates saved dual-pipe layout and disables only LK's second
  framebuffer layer when first GUI frame is ready; restores it before rescue.
  Does not patch LK or remove the warning during bootloader execution.
  Exact C host models pass ASan/UBSan, 100k malformed-report fuzz, I2C/PWRAP
  timeout/ownership checks, display guard/preservation/restoration checks.
  Three modified C translation units compile locally. Full Apple-clang
  CI39464319716/source812d941c97e292558f992c8b16e7c0d1b31a9613 PASSED:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39464319716
  All1096 captured compile commands use generic ARMv8.2+crypto; linked RCpc2
  scan clean. Artifact SHA256SUMS matches kernel17264344bytes, SHA-256
  48d7d1c41272821afb9d6ee7a9b234375d4fecf8b71aea3f62ddee46c00684db.
  Linked disassembly confirms input/mode23/SMP gates, bounded PWRAP cleanup,
  overlay saved-state flag before MMIO and the exact two-pipe field updates.
  Frozen shim/out/nord2-xnu/nord2-ios-input-stop23.img,33542144bytes,
  12288bytes spare below32MiB. SHA-256:
  d74b1b18db5f7ab669f1bace7e6b5d30730b616c843b1022f111311d5a9e59f6.
  Payload14505904bytes SHA-256
  84440e543c97d72e652db6765872adc2e9487ff5080101a26c4e4744e9dac440.
  Same GUI/full-userland23 cmdline plus nord2input=1. Default15-iteration
  Zopfli; all static checks pass. Independent verification confirms original
  Linux, both DT representations and tested rescue RAMdisk are byte-exact.
  Private manifest.json,verify-image.py,image-build.log and guarded
  run-approved-boot.py --authorized-input-boot are prepared/syntax-checked.
  User explicitly authorized this exact boot-only test and supplied fastboot.
  Runner EXECUTED: flash boot and reboot both OKAY at2026-10-20 19:28:45CEST.
  Installed HFS left in place; no partition except boot was flashed.
  Native USB capture saved in hardware-live, client-hardware.log
  and flash.log alongside. Do not rerun the exclusive runner/overwrite logs.
  Verified RAM ADB rescue fallback remains authorized. Volume-down works;
  touch/PMIC failed. See correction above; preserve this image and its logs.
  See shim/nord2-xnu/INPUT-LOG.md and research/nord2/input-ui/.

- **MILESTONE: iOS 16.7.16 LOCKSCREEN ON ONEPLUS NORD 2, eight XNU cores.**
  User explicitly reports "HA BOOTATO! VEDO LA LOCKSCREEN" on 2026-10-20
  around 18:54 CEST, about 7–8 minutes after the 18:46:54 reboot. Exact authorized
  nord2-ios-display-stop23.img was flashed ONLY to boot successfully:
  SHA-256: 061333bbb5dffeb1945cd1dce2108bb5039cfe1c10c6b32b73a924ff23f8dca1.
  Full CI 39458357257/source d7cef5a, packaging b331176, frozen record d34a930.
  All 8 scheduler workers/timers/IPIs PASS. HFS journal replay completed and
  writable iOSRoot mounted. Original Apple launchd, SpringBoard and backboardd
  run. AppleCLCD registered 1080x2400/4352 at 0x76e10000, canvas 1125x2436;
  backboardd matched /product and disp0, opened AppleCLCD successfully.
  backboardd created IOSurfaces 2/3 at 1125x2436/BGRA/stride 4608 and submitted
  swaps; all first 8 logged NORD2 command-mode presents report done=1.
  SpringBoard creates its own surfaces too. This confirms the real GUI path
  from Apple userland through CPU rendering/IOSurface to the MediaTek panel.
  Touch and physical buttons have NOT been ported/verified; this milestone
  proves display/userland boot, not full phone functionality or stability.
  Phone left running this image. Do NOT reboot/restore/reinstall without a
  concrete next task; no normal Android boot on HFS userdata. ADB rescue
  fallback remains authorized. Frozen image and original backups preserved.
  Private evidence: research/nord2/display-userland/{flash.log,manifest.json,
  hardware-live/,hardware-live-late/,hardware-live-retry/,hardware-lockscreen/}.
  Native USB v2 advertises 4 MiB and captures beyond 64 KiB. Earlier readers ended
  at 37504/46547 bytes, then 116712 bytes due transient EP0 TIMEOUT(-7), not a
  reboot. Client now retries the same immutable read offset up to 4 times and
  records transport-events.jsonl plus explicit stop_reason/transport_error.
  A recovered read after two timeouts was observed; consecutive four timeouts still
  end a capture. Final hardware-lockscreen capture 208551 bytes, SHA-256:
  b76912771cbe6d5e93611fbf8e0e09d50a0b6cc3aaae03118d18f5e53aeed239.
  It ended at the requested 45 seconds, no transport error. Contains all first 8
  successful presents; no captured SpringBoard/backboardd crash or kernel panic.
  Crashes captured for driverkitd and PosterBoard (wallpaper service); these
  did not prevent the observed lockscreen. Evidence lockscreen-verified.json.
  Host fault test verifies bounded retries, identical offsets, disconnect
  refusal and empty responses. Python syntax and diff checks pass.
  No kernel modifications or root filesystem edits were needed after this
  authorized boot. IOAcceleratorES polling was an intermediate startup state,
  NOT a proven blocker: it progressed to software-rendered GUI with time.
  Optional REA doctor found no native engine/active Codex registration;
  no settings or engines changed, no binary analysis/patch performed, and
  the investigation was unnecessary once the hardware reached lockscreen.
  Current active-source sampler remains disabled for NORD2; enable only if
  later diagnostics warrant it. Keep logs private (UUIDs/IDs). Do not rerun
  run-approved-boot.py blindly: exclusive flash.log already exists.

- **PREVIOUS IMAGE RESULT: REAL APPLE USERLAND RUNS; GUI NOT YET SHOWN.**
  Exact authorized `nord2-ios-userland-stop23.img` flashed only to boot and
  rebooted on 2026-10-20 18:16:56 CEST. SHA-256
  `662285c21b8bcd2a5294b4be7175fce69b2d7b47e45aebf4286dd63434bd5c91`.
  All8 scheduler workers/timers/IPIs passed, counter32768, then native USB
  became active, UFS userdata opened read/write and HFS iOSRoot mounted.
  ORIGINAL APPLE launchd reached its ignition sequence, initialized first-boot
  data and exec'd SpringBoard, backboardd, IOMFB_FDR_Loader and many daemons.
  This is the first real Apple userland boot, unlike the earlier diagnostic PID1.
  User reports scrolling XNU text, not a GUI. No kernel panic in captured log;
  driverkitd sig9 reason namespace1/code7 was observed (not evidence that the
  whole boot failed). backboardd enumerated arm-io children. No AppleCLCD
  registration or surface swap occurred in the capture.
  First reader exited0 at300seconds with49143bytes; a later read reached the
  persistent capacity65524, SHA-256
  `10d0c9f3cf66ec88427d38ca76c9b5fbf4f838a42d53b3d4032f2e22820ebb3a`.
  Captures: research/nord2/ios-userland-rw/{hardware-live,hardware-live-late}/;
  flash.log and client-hardware.log alongside. Private raw logs contain IDs:
  do not commit them. That capture preceded the GUI-image reboot recorded
  above. USB remained responsive; log saturation was not a kernel hang. No shell, unrestricted memory or disk commands are available
  through this USB interface.

- **GUI IMAGE IMPLEMENTATION AND PROVENANCE (hardware result above).**
  Confirmed code-level display blocker: Nord2 ADT omitted disp0 and the
  loader intentionally supplies zero boot_video fields. Sweet's framebuffer
  expects continuous scanout, while Nord2 requires a DSI command trigger.
  New nord2display=1 (mode23 only) publishes disp0/nord2,display, reuses the
  verified physical buffer0x76e10000,1080x2400,stride4352, advertises iPhoneX
  canvas1125x2436, and refreshes the inherited MT6893 pipeline after presents.
  Bootloader/Linux/boot_args video stay unchanged. No Qualcomm display MMIO.
  First GUI copy drains console drawing; subsequent logs keep recording while
  pixels belong to the GUI, panic may draw again. Refresh is bounded10ms busy
  wait+20ms FRAME_DONE wait. INTSTA W0C confirmed in user's mtk_dsi.c IRQ code.
  Existing text-console DSI path unchanged. A host check models config/mapping
  gates, handoff, panic exclusion, timeout and completion/acknowledgement.
  Found `_cnputs` previously called raw nord2_putc, bypassing the SMP lock;
  now every byte goes through sweet_early_putc. pmsg remains first65524 bytes
  for rescue. A separate coherent4MiB append-only RAM log starts after console
  remapping and continues past pmsg saturation. Native USB protocol2 retains
  old requests and adds bounded read-only metadata/data with32-bit offsets;
  client supports both protocols and reports capacity explicitly.
  Host decoder/ASan/UBSan checks, SMP startup/shutdown/8-thread serialization,
  display timeout tests, local two-C-file compiles and RCpc2 checks passed.
  Local C++ is unavailable due to Apple's __builtin_xnu_type_* extensions;
  full Apple-clang CI is required. No fake builtins or local kernel relink.
  Evidence research/nord2/display-userland/. Full CI39458357257, source
  d7cef5a99bb7c7d9b6bda620decfc57aa8a21cdc, PASSED:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39458357257
  Client also passed a real protocol1 read from the running phone and exact
  mock reads of all4MiB using32-bit offsets. Packaging commit b331176 adds
  --product-adt: copy only non-placeholder /product properties from the
  same IPSW's D22AP tree, as Sweet does. Mode23 omitted this node too;
  backboardd logged its absence. No Apple hardware nodes are copied.
  Private source research/nord2/ipsw/d22ap.adt SHA-256
  `9e670987c80493bd69aba7be62e3179c04663324d9249b0973675baac175dbf1`;
  IM4P compared byte-exact with the local20H392 IPSW before extraction.
  Generated ADT12708bytes; filtered-property equality, iPhoneX name, scale3
  and RAM/CPU/UFS preservation checked. No second kernel CI is needed for
  this host packaging change. Kernel17263464bytes, SHA-256
  `ee6a1b007dc58925a46ede711504d79f0e2eb724752d33620d9b15b0d424d4a1`.
  Artifact SHA256SUMS matched; all1096 captured compile commands use generic
  ARMv8.2+crypto. Linked RCpc2 scan2188857 instructions clean. Disassembly
  confirms display opt-in/mapping guards, bounded refresh/W0C, C++ hook calls,
  serialized console entry and acquire/bounded stream reader.
  Packaging and independent verification PASSED. Frozen image:
  shim/out/nord2-xnu/nord2-ios-display-stop23.img,33544192bytes (10240spare),
  SHA-256 `061333bbb5dffeb1945cd1dce2108bb5039cfe1c10c6b32b73a924ff23f8dca1`.
  Payload14505856bytes, SHA-256
  `4179e144cf9b52e02fd7ff4ebb8fddc84e99c4d9d378083f6d6f06012a358524`.
  Same full-userland23 cmdline plus nord2display=1. Full CI kernel directly
  packed with default15-iteration Zopfli. Static checker passes GUI node,
  product identity and all prior CPU/UFS/layout gates. Original Linux and
  both DT representations/tested rescue RAMdisk are byte-exact.
  Private manifest.json,verify-image.py,payload.bin,image-build.log saved.
  run-approved-boot.py --authorized-display-boot is prepared and syntax
  checked, then EXECUTED after exact user authorization. It pins exact
  image/backup/CI hashes and fastboot identity, flashes only boot, and starts
  USB wait300/read600 before reboot.
  The prior root readback is historical installation evidence; current HFS
  contents have changed during iOS first boot. No reinstall is needed.
  Exact GUI-image flash authorization was received and the image is now
  running. Original mode23 image662285c2…34bd5c91 is retained for evidence.
  Fallback remains ADB rescue.
  Preserve frozen23 and all earlier images; prepare and verify any future
  image before requesting its exact boot-only approval.

- **FULL iOS ROOT INSTALLED AND VERIFIED before first boot.**
  Source research/nord2/rootfs-vm/ios16-root-20H392-nord2-v1.img,
  17179869184bytes, SHA-256
  `8b1d9de6775674afe790a161419ea23e824c5e045f21a66c6fc886125c991cb3`.
  User authorized replacing Android data without backup and asked a Sol
  agent to flash it. /root/install_ios_resume completed the entire bounded
  transfer and full16GiB device SHA-256 readback, exact match. Evidence:
  research/nord2/ios-userland-rw/install-bounded/{result.json,
  device-readback.sha256,write-progress.json,final-rescue-state.txt} and
  install-bounded.log. Only userdata prefix was changed, no GPT edits.
  iOS has now initialized writable /var; the original full image hash is
  therefore NOT expected to match the running filesystem. Do not reinstall.
  Earlier attempts install-retry (~5.5GB consumed) and install-after-crash
  (~7.6GB consumed) were interrupted by host OOM; those figures were NOT
  confirmed writes. Kernel journal identified adb65482 at4.2GiB anonymous
  RSS. Installer fixed with sequential64MiB requests, waiting for each remote
  dd conv=notrunc,fsync exit before the next. Actual ADB stayed38-60MiB.
  Toybox nocreat is advertised but rejected, hence block-node/identity guards
  plus notrunc/fsync instead. RAM seek and fake-ADB fault/boundary tests pass.
  Never return to the old unbounded stdin writer. Historical failed logs
  remain private in install/,install-retry/,install-after-crash/.

- **Previous mode23 provenance and recovery (superseded on phone).**
  Full Apple-clang CI39450476117/source4e3f65181f0ac08dd1f524d0da405bf676c49c9b
  PASSED: https://github.com/frxncyyy/ios-sweet/actions/runs/39450476117
  Kernel research/ci-39450476117/xnu-kernel/kernel.development.sweet,
  17279464B, SHA-256
  `f0a5accca1c5462f37b33b9a8e1f7f33098905a95cd7c907da9a38db71148259`.
  All1090 captured commands generic ARMv8.2+crypto, RCpc2 scan clean.
  Frozen shim/out/nord2-xnu/nord2-ios-userland-stop23.img,33542144B,
  12288bytes below32MiB boot limit. Payload14502688B, SHA-256
  `b0ae1984d2e57e17009c9f47afc90d197a04c69b769cfb9a4167d14807eba819`.
  Original Linux, both DT representations and tested rescue RAMdisk verified
  byte-for-byte. Full CI kernel packed directly with default15-iteration Zopfli.
  Args: debug=0xc cpus=8 keepsyms=1 -noprogress nord2stop=0 nord2vm=1
  nord2fb=1 nord2stopat=23 rd=ufsroot0 ufsroot=userdata ufsrw=1 rootrw=1
  unentitled_ios_sim_launch=1 ignition_level=1 wdt_test=no_user_checkin
  launchd_unsecure_cache=1 wdt=-1. All8 gate precedes USB/UFS; only23 runs USB
  continuously. Panic still quiesces all7 secondaries before hardware restore
  and return to RAM ADB rescue. Exact frozen23 + fallback already authorized.
  Private run-approved-boot.py has EXECUTED, flash.log exists: do not rerun it
  blindly. For failure/recovery use tested research/nord2/rescue/nord2-adb-rescue.img
  SHA d78bf9fe720a1d293b863ea0a8b30e0df4b5eafb097b1343a3c30e7c3d03bac5;
  bootloader/manual fastboot might be needed. Do NOT boot normal Android with
  HFS userdata. Keep original boot/recovery backups for future reinstall.
  Frozen23 static config/UFS partial-block strategy/write-bound tests all pass.
  Installation and artifact records committed785462f,c63449f,291dda2,550c644.

- **Point22 prepared and frozen, SUPERSEDED by user request; never flashed.**
  Full CI39446150677 PASSED/sourcec992fd5:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39446150677
  Kernel17263080B SHA-256
  `8a610dcca545a8c5d96b2ea8fd73dd396c99e5cb8086af59ac1dc59e55c9c5d9`;
  checksum verified, all1097 captured commands generic ARMv8.2+crypto;
  RCpc2 scan and linked config/readiness/write-flag checks passed.
  Frozen shim/out/nord2-xnu/nord2-ios-readonly-stop22.img,33542144B,
  SHA-256 `9ad9b5384b4f980b6cebe1f5d830d8bb78d20b3f1f4e56345d28a31bf754f3a8`.
  Payload SHA `a2881f651b39845f4c5dd4171ed526a6c7ce0ca4161bb6c490f7d0718ade6ddd`.
  Both DT representations/original Linux/tested rescue RAMdisk byte-exact;
  static checks passed. cpus8,read-only userdata,45-second USB. No exact
  flash authorization requested; do NOT run this additional RO test now.
  Evidence research/nord2/ios-userland/{manifest.json,payload.bin,image-build.log}.

- **CURRENT RESULT: point21 eight-core XNU + native USB + RAM PID1 PASSED.**
  Exact authorized imagec5ce4445…f368e86a flashed only to boot; returned to
  authenticated root ADB RAM rescue without any block/data filesystem mount.
  Original boot restoration was not needed. Current boot remains point21;
  At point21 completion phone was ADB recovery and userdata unchanged;
  later installation/crash state is recorded above.
  All8 scheduler workers passed on their actual cores, correct A55/A78 MIDR,
  ticks33/18/18/18/18/18/18/17, IPI replies1..8; counter32768 and PASS1.
  USB service bound to CPU0; HFS md0 mounted; diagnostic PID1 verified HFS
  read/getpid1/EL0 syscalls and emitted all10 timed ticks over native USB.
  PID1 intentional exit66 caused the expected sole panic on CPU1, routed
  return to CPU0; all7 secondaries confirmed OFF before USB restoration.
  Both final cleanup markers and Linux return marker are complete.
  One observation: CPU0 return-caller line starts "RD2 SMP" (first2 characters
  missing during cross-core handoff). This does not affect the complete
  shutdown/restoration markers. Do not claim zero console drops throughout;
  the zero-dropped counter is measured before USB/root/userland begins.
  Native live log17145B SHA-256
  `fe58fdc519aa8c17f476597cbbc6a29c908f67a4a2458b958e5b513384c3d499`,
  exactly matches the pmsg prefix and includes HFS, all10 ticks and exit66.
  88 USB status samples; last requests185,resets2,logreads87,stalls0,configured1.
  Pmsg17425B SHA-256
  `635faa4aa1daa2b7f309f7361eb00e13fdf5bc455e9f12d914a5d8c503e2339f`,
  backup/nord2/logs/20261020-170250-smp-usb-userland-stop21/pstore/pmsg-ramoops-0.
  Evidence research/nord2/smp/userland/verified.json,hardware-live/,rescue.txt,
  flash.log,rescue-collector.log. Both capture processes exited0.
  This validates bounded SMP+RAM userland+live USB together, including a
  secondary-core panic return. Apple launchd/full iOS are still untested.
  Next prepare the first full iOS root boot deliberately: current mode21
  requires rd=md0 and disables disk writes. Preserve this tested image and
  backups; do not merely change rd=ufsroot0. Exact installation/boot artifacts
  and recovery procedure must be ready before the next destructive approval.

- **Point21 build and authorization record (hardware result above).**
  User said "vai" after the8-core milestone; implementation/preparation is
  authorized. No new boot image has been flashed in this phase. Phone last
  observed in manual fastboot. Preserve all prior images and backups.
  New mode21 permits SMP startup only with cpus8, rd=md0, ufsrw=0/rootrw=0.
  It reuses the tested bounded all8 scheduler/timer/IPI/atomic-worker probe;
  only full PASS publishes the userland-ready flag and continues to USB.
  BSD ordering now runs SMP gate before USB, then RAM-root selection.
  USB21 requires that pass flag, pins its sole PIO service to CPU0 before
  any register access, and publishes ready/done with release/acquire.
  CPU0 masks IRQs for each bounded PIO iteration; secondary panic requests
  CPU0 return and powers off. Existing all-secondary-OFF guard precedes
  MAC/PHY cleanup. USB cleanup refuses non-CPU0 SMP ownership and failed
  cleanup blocks Linux return. Normal45-second timeout still bounds the
  test if HFS/PID1 stalls; PID1's intentional exit66 should end it earlier.
  Append-only NC pmsg now has writer/read publication DMBs so USB cannot
  read a newly advertised length before data from another core is visible.
  Added return-caller CPU and controller-restored log markers.
  Existing mode16 still requires cpus1; mode20 still accepts2/4/8 without
  mounting a root. No full iOS/UFS write mode enabled by these changes.
  Local generic ARMv8.2 compilation passes for sweet_early.c,sweet_usb.c,
  bsd_init.c; RCpc2 scans clean. Exact-source host ASan/UBSan tests pass:
  SMP configuration/return/IRQ guards, RAM-only/write-flag rejection,
  USB background pass/ready/failure/timeout gates, migration from all8 CPUs
  and failed-migration refusal,100000 malformed control requests, cleanup
  owner checks and1000 PHY rollback states. Patch applies and matches sources.
  Evidence research/nord2/smp/userland/. tools/nord2-check-usb-smp.py added;
  other relevant host/static checkers extended for mode21.
  Retain existing8MiB RAM HFS diagnostic image:
  research/nord2/rootfs-live/nord2-hfs-pid1.img,
  SHAb5fa35f397e784c1da04988503ee43d9ce23e7925829a84be181d4ceba07308a.
  Full Apple-clang CI39437387549 PASSED, source1f5efba:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39437387549
  Downloaded SHA256SUMS verified; kernel17263040B SHA-256
  `9d9fb7ebf4d998b85dbe643f4ad3e3f08c1ceb522787bc8bf0daffbe1145f052`.
  All1096 captured SWEET compile commands explicitly generic ARMv8.2+crypto.
  RCpc2 scan clean. Linked disassembly verifies CPU0 binding/migration/check
  before USB enumeration, completion STLR, readiness/stop LDAR, guarded USB
  cleanup, SGI/affinity checks before cleanup, and pmsg publication DMB SY.
  Evidence userland/kernel.json,critical-functions-{check.json,disassembly.txt}.
  Frozen candidate BUILT and independently verified:
  `shim/out/nord2-xnu/nord2-smp-usb-userland-stop21.img`,33552384B
  (2048B below the33554432B boot partition), SHA-256
  `c5ce4445204cf81fa4d77afba3126775a9a14f15344947f6bd83ce01f368e86a`.
  Payload22891392B SHA-256
  `29959651fd87a75e8f544e5f247b35b875876093264537f68111279137838ee2`.
  Cmdline: debug=0xc cpus=8 keepsyms=1 -noprogress nord2stop=0 nord2vm=1
  nord2fb=1 nord2stopat=21 rd=md0 ufsrw=0 rootrw=0.
  Full CI kernel used directly; default15-iteration Zopfli compression.
  Static checker PASS; original Linux and both DT representations exact;
  tested ADB rescue ramdisk exact; embedded8MiB diagnostic HFS image exact.
  Manifest/extracted payload/image-build.log and private verification scripts
  under research/nord2/smp/userland/. User explicitly authorized this exact
  boot-only test and original Android boot restoration if rescue fails.
  Verified hashes/device/boot size; boot-only flash and reboot succeeded.
  Native USB reader (wait240/read60) and rescue collector (label
  smp-usb-userland-stop21) started before reboot. Hardware result PASSED above.
  Preserve image and raw logs. Do not request renewed approval for this test
  or its recovery. If rescue fails restore original Android boot under the
  existing authorization, with manual fastboot if needed.
  No additional user-data access required. Hardware success must show all8
  SMP PASS, USB live HFS/PID1 success +10 timer ticks, expected PID1 exit66,
  coordinated all7 OFF, USB restoration and authenticated root ADB rescue.

- **CURRENT RESULT: all8 cores VERIFIED running XNU; full2→4→8 sequence PASSED.**
  User explicitly authorized the NEW exact generic2/4/8 sequence; completed.
  Previous original-image sequence remains stopped. Current phone: root ADB
  RAM recovery after successful8-core test, diagnostic8 image still in boot.
  No original boot restoration needed because all recovery returns succeeded.
  New full Apple-clang kernel from successful CI39424762428/source98efcd7,
  with explicit generic CPU target to remove the observed RCpc2 fault.
  Each image33542144B fits boot33554432B with12288B margin. Static checker
  PASS; original Linux kernel/both DT representations and tested root ADB
  rescue ramdisk independently byte-identical. Payloads differ ONLY in the
  cpus=2/4/8 argument. No userdata mount, no root mount, no disk writes,
  no USB takeover. Same bounded worker/timer/IPI/atomic-counter test and
  coordinated all-secondary-OFF guard. Linked code checked: observer LDAR,
  IRQ EOI before OFF, timer disable/cache clean (alias clean_mmu_dcache)
  before SMC CPU_OFF; failed OFF parks; primary SGI/AFF check before cleanup.
  Evidence research/nord2/smp/generic: manifests,payloads,kernel.json,
  image-N-build.log,critical-functions-check.json,atomic-regression.json.
  - `shim/out/nord2-xnu/nord2-smp-generic2-stop20.img`
    SHA-256 `4fba039538f45f62bdc929c9691ce3e310e4567f65e479936bf51765c6294c25`;
    payload `cba0f5bb3e82c848c505e293cb1ee71b809b65a4f70cabe61d65da881de372a5`.
  - `shim/out/nord2-xnu/nord2-smp-generic4-stop20.img`
    SHA-256 `f0db75801b4cbef20a8341d53808a691914e675e9e9e0e2d17acfe1ebcda513b`;
    payload `22610ea62dac38eb5e42c3694001bed6ba4946f5c19db6352b972445d06b95b3`.
  - `shim/out/nord2-xnu/nord2-smp-generic8-stop20.img`
    SHA-256 `cb172efbfe4c059c8a9c1f7ade25feac758b051847f84602f0669f8251ffc276`;
    payload `843f86ec2e28e15993837ac3c377341a59d92a943ea1e29fcd8a47a51c86335b`.
  Authorized: conditional2→4→8 boot-only tests, rescue after each and
  original boot restoration if rescue fails. Manual fastboot may be needed
  between tests/recovery. No renewed permission for these unchanged images.
  The three exact authorized tests are now completed; no further test is
  pending in that sequence. Preserve all frozen images and raw evidence.
  Backup boot SHA04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01.
  Collect immediately: tools/nord2-pull-logs.sh --rescue smp-genericN-stop20.
  Require N/N workers, correct CPU/MIDR,done1,ticks>0,IPI replies core+1,
  shared counter=N*4096,console_dropped0,PASS1,all-secondary-OFF marker,
  authenticated root rescue marker and no block/data filesystem mounts.
  Test2 PASSED at15:04 CEST: worker0/1 actual0/1, A55 MIDR412fd050,
  timer ticks33/17, IPI replies1/2 and IRQ counts36/90, counter8192,
  console_dropped0, scheduler2/2 PASS1. All secondaries OFF before Linux;
  authenticated root ADB rescue and no block/data mounts verified.
  Pmsg13775B SHA-256 `fca148ba5c97a3851def890dc1bb08e4817ff95841eea2fb5e74287dc644e8bd`,
  `backup/nord2/logs/20261020-150252-smp-generic2-stop20/pstore/pmsg-ramoops-0`.
  Evidence generic/hardware-test-2/verified.json. Stop sequence at any failure.
  Test4 PASSED at15:08 CEST: all4 A55 workers on correct cores, timers
  33/18/18/17, correct IPI replies1..4, counter16384, console_dropped0,
  scheduler4/4 PASS1. All secondaries OFF; root ADB/no block-data mounts
  verified. Pmsg14256B SHA-256 `55a8933ce58643597739573cbcbc501e233eb714babc1d557e19515dafdd7bcc`,
  `backup/nord2/logs/20261020-150624-smp-generic4-stop20/pstore/pmsg-ramoops-0`.
  Evidence generic/hardware-test-4/verified.json.
  Test8 PASSED at16:00 CEST: all8 workers on actual CPUs0..7; cores0..3
  Cortex-A55 MIDR412fd050,cores4..7 Cortex-A78 MIDR411fd410. Each completed
  sixteen timed sleeps and4096 shared atomic increments; ticks33/18/18/18/
  18/18/18/17, correct IPI replies1..8, observed IPI IRQs50/70/41/12/3/3/3/3.
  Scheduler8/8 PASS1; exact shared counter32768, console_dropped0. All7
  secondary affinities confirmed OFF before chain-back; authenticated root
  ADB rescue marker and no block/data mounts independently verified.
  Pmsg15208B SHA-256 `b093c6203206815b42395be60a3ff4f5952fa23a2e131451bf0d90165327ba40`,
  `backup/nord2/logs/20261020-155848-smp-generic8-stop20/pstore/pmsg-ramoops-0`.
  Evidence generic/hardware-test-8/verified.json; full sequence aggregate
  generic/sequence-verified.json. Successful kernel SMP execution now proven
  for all8, beyond the earlier firmware-only CPU_ON/OFF tests. This remains
  a bounded diagnostic before root selection, not long-duration validation.

  NEXT PHASE (now implemented in source, hardware pending; see point21 above):
  integrate8-core operation with RAM HFS/PID1 and live USB before installing
  the prepared full iOS root. These frozen point20 images permit multi-CPU
  startup only at stop20; the frozen point16 USB image requires cpus1.
  Do NOT change cpus=8 on an old USB/userland image. Point21 deliberately
  extends startup, USB ownership and coordinated shutdown guards together.
  Request authorization for its verified exact boot image. Existing full iOS16
  HFS root is still local only; userdata untouched by this entire sequence.
  Actual Apple launchd/full iOS, graphics/touch and sustained SMP load are
  not verified by these tests. Preserve original boot/recovery backups.

- **Previous result: point20/2 CPUs FAILED before secondary startup; sequence4/8 STOPPED.**
  User authorized the exact three images conditionally,2→4→8; do not flash
  remaining4/8 images after this failure. Boot-only flash/reboot of2 succeeded
  at14:40 CEST. XNU panicked before the coordinated-start marker or scheduler
  probe: PCfffffe00074824fc, instruction99448108, in
  sweet_cpu_signal_is_enabled. Decode is LDAPUR w8,[x8,#72] (FEAT_LRCPC2).
  Root ADB RAM rescue returned, marker verified and no block/data mounts.
  No original restore was needed. Pmsg10616B SHA-256
  `a02873cffdd3fd4950a2ef4d64995fbe9ed70e7d1568a17799e7abdeb3ebf54c`,
  `backup/nord2/logs/20261020-144059-smp-xnu2-stop20/pstore/pmsg-ramoops-0`.
  Evidence research/nord2/smp/xnu2/hardware-test-2/{flash.log,rescue.txt,verified.json}.
  Local clang23.1.1 selects target-cpu apple-m1 for arm64-apple-macos14.0
  despite -march=armv8.2-a+crypto; this can inherit unsupported extensions.
  Explicit -mcpu=generic removes this CPU default. Full CI39422376218 passed,
  downloaded kernel hash verified; its same observer uses LDAPR rather than
  LDAPUR, so successful CI alone did not validate the local relink's ISA.
  Fix prepared: both local helper and MakeInc.def now use -mcpu=generic
  -march=armv8.2-a+crypto. Recompiled actual cpu.c: observer now ADD+LDAR;
  no LDAPR/LDAPUR/STLUR in that object. Evidence research/nord2/smp/generic.
  Patch applies and reproduces sources. Next run full Apple-clang CI with
  this target and use its complete kernel to avoid stale local objects.
  Source98efcd7 pushed; full rebuild PASSED:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39424762428
  Small atomic acquire/release regression reproduces LDAPUR/STLUR with the
  implicit Apple CPU and LDAR/STLR with generic; saved under same evidence dir.
  Image helper now rejects LDAPUR/STLUR using an explicit RCpc2-capable
  disassembly before packing. This is a targeted regression guard, not a
  complete ISA audit. It rejects the failed cpu.o and kernel, accepts the
  generic cpu.o, and refuses decoder errors/empty output.
  CI kernel17262800B SHA-256
  `4a0b54f783b87488870ebe45ca78962403c2c62f81469eceed9ab820b1c4df0f`.
  Downloaded SHA256SUMS verified;965 SWEET compile commands all explicitly
  generic ARMv8.2+crypto;2,188,178 decoded instructions contain no LDAPUR/STLUR.
  Complete CI kernel used for new images (no local archive relink).
  Built generic2/generic4/generic8 stop20 with unchanged diagnostic scope.
  New candidates will require fresh approval; old three images stay frozen.

- **Previous authorization/build: point20 images, conditional2→4→8 (stopped at2).**
  Point19 proved all7 firmware entries/shutdowns, including high PA, but no
  secondary XNU scheduler execution yet. Now arm SMP return from cpu_start()
  immediately before PE_cpu_start(); accept only stop20 and cpus2/4/8.
  IRQ hook checks a global stop request after IAR and before normal handlers,
  EOIs the interrupt, disables the local timer and calls CPU_OFF on secondary
  CPUs after cache cleaning. A nonboot panic/return signals CPU0 then powers
  itself off; CPU0 sends SGI1 to all7, requires every AFFINITY_INFO64=OFF
  within5s, and only then permits USB/UFS cleanup and Linux memory reuse.
  Failed OFF parks; failed quiescence halts, requiring manual fastboot.
  No printf/scheduler locks in the final shutdown guard, only bounded early
  logging. ARM TF-A reference follows cache maintenance before OFF publication:
  https://github.com/ARM-software/arm-trusted-firmware/blob/master/lib/psci/psci_off.c
  Linux arch/arm64/kernel/psci.c also calls CPU_OFF with the normal MMU active;
  this vendor firmware's cached XNU shutdown is still untested until point20.
  Early console remains atomic-free before MMU; pre-MMU secondary writes are
  suppressed. After first CPU startup, IRQ-masked recursive per-core ownership
  serializes pmsg/framebuffer, with50ms contention timeout to prevent panic
  deadlock. Per-core timer/IPI counters replace verbose global tick logging.
  BSD point20 binds coordinator to CPU0, launches one bound worker per core,
  validates actual CPU/MIDR,16 timed wakeups, shared atomic4096 increments per
  core and cpu_xcall replies. Require ticks>0 per core, exact total8192 for2,
  zero dropped console bytes, PASS=1, then all secondaries OFF before return.
  This stops BEFORE root selection; no USB/UFS takeover or userdata mount.
  Source changes sweet_early.c/.h,cpu.c,sleh.c; only three C objects replaced
  in a copy of tested point19 archive. No assembly change or fake Apple C++
  builtins. Local compilation/link and exact-source ASan/UBSan guards pass,
  including all7 affinities, ON/PENDING/errors/timeouts, secondary→CPU0 route,
  EOI/clean/OFF ordering, failed OFF park, console timeout/recursion and
  concurrent8-thread writes; prior PSCI probe regression tests pass.
  Patch applies cleanly and matches sources. Evidence research/nord2/smp/xnu2;
  kernel 21964208 bytes SHA-256 `828661510ad5a7eb92095f2fef6e46ae9bfbb5aabf80c93dd07b7dcb039f5c60`.
  Three frozen images, each33316864B, all static checks PASS:
  - `shim/out/nord2-xnu/nord2-smp-xnu2-stop20.img`
    SHA-256 `806c8cb82a4909b78a5f57947bc5e302fedbef7c85bbf758ae88007f8ad9eaf0`;
    payload `fb88343dc504a438ba58b64580c3f7001402dea4bf163bfe0c43e0bd36e3a466`.
  - `shim/out/nord2-xnu/nord2-smp-xnu4-stop20.img`
    SHA-256 `26f16a77aaa50cb1a92d431dc0193fbb6a4e4b823cbd3aa35f65c8e89c295894`;
    payload `1e0e7cfd4105f6389c98c9855b32c638f10479d7b19ca4c28d5f34e6af287eb1`.
  - `shim/out/nord2-xnu/nord2-smp-xnu8-stop20.img`
    SHA-256 `b34dc4a45ad4b6616d14ebe3c5829d433617c456bfdc348ad1a4f4ac6ed2f59d`;
    payload `4b44378b372f256a9ddb64736a1b938395f799f0e0186b73ba16127078230c21`.
  Original Linux kernel and both DT representations match original backup;
  rescue ramdisk matches tested image byte-for-byte. Decoded payloads differ
  ONLY in the single cpus=2/4/8 argument byte. No root mount/disk-write flags.
  Verification manifests and extracted payloads under research/nord2/smp/xnu2.
  Initial unfinished compression was canceled before any candidate existed
  to remove printf from shutdown failure diagnostics; these are final images.
  Linked disassembly confirms MMU/DAIF console guards and coordinated shutdown
  before USB cleanup (static prepare_return was inlined into nord2_leave_to).
  Source9b881eb pushed; full Apple-clang CI PASSED:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39422376218
  User explicitly authorized ALL THREE exact images in conditional2→4→8
  sequence, plus original-boot restoration if rescue fails. Do not request
  permission again for these unchanged images. Wait for successful CI before
  first flash (CI success now confirmed). Manual fastboot is still needed between successful tests.
  For each: verify device/product/boot size/hash/backup, flash boot only,
  reboot, immediately run tools/nord2-pull-logs.sh --rescue smp-xnuN-stop20.
  Require scheduler result N/N, counter=N*4096, PASS=1, per-core ticks and
  correct IPI replies, all-secondary-OFF marker, and verified root rescue
  before advancing. Stop at first failure. Original Android boot restore is
  included in requested authorization if rescue fails; manual fastboot may
  be needed. Phone already fastboot. Between successful tests, ask only for
  manual fastboot (not renewed permission if all3 were explicitly approved).


- **Previous result: point19 PASSED low+high, all7 secondaries; ADB rescue returned.**
  This follows the observed point18 context_id=0. Only mode19 treats context
  as diagnostic; slot is derived from MPIDR in the unchanged stackless stub.
  Completion, CPU identity/type, EL, MMU/cache-off, timer frequency and CPU_OFF
  remain mandatory. First starts/stops all seven cores using PA40180000;
  only if all seven pass and all are OFF, clears every mailbox and repeats
  with the same stub at its linked PA above4GiB. This separates entry-address
  support from context delivery. A failure aborts remaining cores/passes.
  XNU itself remains cpus1 and no disk/root/USB path is activated. Linux
  return still requires every secondary OFF, then clears all32KiB scratch.
  Exact-code ASan/UBSan tests cover all7 affinities and14 low/high startup
  positions: bad identities, CPU_ON failure and never-OFF all stop correctly;
  stale completion cannot pass the second round. Low/high result validation
  and prior return guards pass. C compiles; patch applies and matches source.
  Local relink replaces only sweet_early.o in a copy of frozen point18 archive;
  stackless assembly is unchanged. Kernel21967712B SHA-256
  `fd3e8f2b1ca1e0a66261fb925220a0c1f015d34fd6e9c613dc9bf055a87780e5`.
  Evidence `research/nord2/smp/fixed/`. Image built, all static checks pass:
  `shim/out/nord2-xnu/nord2-smp-fixed-stop19.img`,33314816B, SHA-256
  `04c41d246d9ba9e5725829fdb6177cf878590c2056d6019167ce624449f04a69`.
  Payload SHA `93928adad4c261fd434b9dae5fa2d24e665fd355ccee45be45897588faca94e3`.
  Original Linux kernel/both DT representations and tested rescue ramdisk
  independently compared byte-for-byte; unchanged. Linked188B secondary stub
  byte-identical to point18, SHA64cb547ace2ad90be3dc8d23ff3d98dc0737fa7fa8ca778d03523874c9c8321c;
  all branches internal, no stack or external PC-relative accesses, stores
  confined to its mailbox. Manifest and disassembly verification saved.
  User authorized this exact boot-only image. Flash/reboot passed at14:10
  CEST. BOTH pass0 (entry40180000) and pass1 (entry1434900cc) report7/7;
  all14 CPU_ON/OFF cycles succeeded. Cores1..3 MIDR412fd050 (A55),4..7
  MIDR411fd410 (A78), EL1, SCTLR30d00800, CNTFRQ13000000. Every core
  reported context_id=0 despite requested nonzero value; fixed mailboxes
  worked. High physical entry support is now verified, no low trampoline
  required merely to enter secondary XNU code. Primary+all7 are startable,
  but XNU scheduler still ran only on CPU0: do not report functional XNU SMP.
  All seven OFF checked before chain-back; root ADB rescue/marker and no
  block-backed/data filesystem mounts verified. No original restore needed.
  Pmsg19065B SHA-256 `967a8dce9c6b9a7add68a0da2c309b710576cb8b83251d5d4b074e1744d29d56`,
  `backup/nord2/logs/20261020-141027-smp-fixed-stop19/pstore/pmsg-ramoops-0`.
  Evidence `research/nord2/smp/fixed/hardware-test/{verified.json,rescue.txt,flash.log}`.
  Timer IRQ messages bisect a few printf lines; parser removes only complete
  timer-tick records before checking14 per-core result lines; raw pmsg intact.
  Next implement coordinated kernel-secondary shutdown and serialized early
  logging, then bounded actual XNU2-CPU scheduler/timer/IPI test before4/8.
  Source6870fbb full Apple-clang CI PASSED:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39419145258
  Downloaded run.json/build.log/xnu-kernel under research/ci-39419145258;
  kernel SHA15ca446aafbf5848085950a0e5d9b16be15f1c6cf48440c5ff3bf37cc7f2ffe5
  matches artifact SHA256SUMS. Hardware used frozen local relink.


- **Previous device: point18 authorized/flashed; core1 executed, strict context check failed; ADB rescue returned.**
  Point17 did not verify even core1 execution; entry and context were above
  4GiB and context_id was the only mailbox pointer. Neither high-address nor
  context handoff support is established. Do not call either the cause yet.
  Point18 copies a188B position-independent stackless entry to PA40180000,
  with fixed per-core128B mailboxes at40184000. Both are inside zero padding
  of the2MiB loader prefix40080000..40280000, outside loader code/BSS/stack
  and below the original Android kernel40280000. Runtime refuses nonzero
  scratch before any write; image checker independently checks loader size
  and entire32KiB scratch hole. Stub derives mailbox slot from MPIDR Aff1
  (1..7), recording actual firmware x0 context without dereferencing it.
  It then records EL/MPIDR/MIDR/SCTLR/CNTFRQ and requests CPU_OFF as before.
  New logs include entry/mailbox PA, magic, context and affinity state.
  Scratch is cleared only after ALL seven affinities are OFF; generic
  panic return uses the same guard. Failed quiescence retains scratch and
  refuses Linux return. Disk writes/root mounting/USB remain disabled.
  Same exact-code host guard tests pass plus scratch-retention/clear cases;
  C/assembly compile; objdump verification: no stack or external PC-relative
  references, every branch inside the188B copied range, only mailbox stores.
  Kernel21967832B SHA
  `4a674612ecd28baef3bc170e0a4517524e177a30e8b81effd7de7c8f4faa8d27`.
  Evidence `research/nord2/smp/low32/`. Image built and all static checks pass:
  `shim/out/nord2-xnu/nord2-smp-low32-stop18.img`,33312768B, SHA-256
  `d6ae9e2f9ec78b5b854ce8d2f2f58506eefb526ba27eae8316d7990616fdb5f4`.
  Payload SHA `13d92d735f36caf6b0b68cf336cf8eb80db922f02526d97b337087052b644610`.
  Original Linux kernel, both DT representations and tested rescue ramdisk
  independently compared: unchanged. Manifest saved, candidate frozen.
  User authorized this exact image; boot-only flash/reboot PASSED at13:50:51
  CEST. Probe copied/read back low stub and core1 CPU_ON returned0. Core1
  completed the mailbox and CPU_OFF: EL1, MPIDR81000100, MIDR412fd050 (A55),
  SCTLR30d00800 (M/C off), CNTFRQ13000000, magic4e32534d, state1, affinityOFF1.
  Firmware delivered x0/context_id=0 instead of requested40184080. This alone
  fails the strict point18 result check, stops the loop and reports0/7;
  distinguish that from actual execution/shutdown of core1, now proven.
  Cores2..7 were NOT started. All seven OFF checked before return; root ADB
  rescue/marker and no block filesystem/data mounts verified. Original boot
  restoration unnecessary because rescue succeeded. Point18 image remains in
  boot. Evidence `research/nord2/smp/low32/hardware-test/verified.json`;
  pmsg `backup/nord2/logs/20261020-135431-smp-low32-stop18/pstore/pmsg-ramoops-0`,
  15352B, SHA-256 `66e9a0b0824394c8e8cf66c663a9160208cd91c45c7e81df89425603bd1606cd`.
  Next: retain observed context as diagnostic, use fixed mailboxes to test
  all secondaries without depending on firmware context delivery. This result
  does NOT establish whether the firmware supports entries above4GiB.
  Point17 full Apple-clang CI PASSED at ff8d66e:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39414925445
  Run/log and artifact saved under `research/ci-39414925445/`. Download retry
  succeeded; kernel SHA-256
  `42bd46182a92a365a7ddabcb5868ac1d70ce5cd84e712caf7a55ac9dc1d44d75`
  matches the artifact SHA256SUMS. Source point18 commit55ed523 is pushed.
  Full Apple-clang CI for point19 at6870fbb passed:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39419145258

- **Current user priority: enable all eight CPU cores (or as many as practical)
  BEFORE installing/starting full iOS userland. No userdata install yet.**
  Point16 USB/userland diagnostic passed (details below). User then requested
  SMP first. Live DT confirms four Cortex-A55 + four Cortex-A78; actual reg
  affinities are 0x000..0x700 in steps0x100, despite cpu@ node labels. PSCI1.0,
  SMC conduit. ADT and legacy SweetCPU already accept these affinities/eight
  CPUs; this is not proof of Nord2 secondary startup, GIC/timers or scheduling.
  Existing Linux return and early console assume a single active CPU.
  USB point16 explicitly rejects cpus!=1; do not remove that guard blindly.
  **Preparing point17 firmware prerequisite**, XNU still cpus1: sequentially
  CPU_ON each of seven secondaries into a stackless physical assembly entry.
  Each writes only its own 128-byte mailbox (uncached alias in XNU), records
  CurrentEL/MPIDR/MIDR/SCTLR/CNTFRQ and context, then calls PSCI CPU_OFF.
  Require OFF from AFFINITY_INFO plus complete/matching mailbox; timeout5s.
  Read-only PSCI feature/affinity preflight, all seven initially OFF required.
  Return guard checks all seven OFF before any USB/UFS cleanup or Linux jump;
  failed OFF refuses chain-back and requires manual reboot. No UFS takeover,
  root mount or USB probe in this test. Logs recovered through ADB rescue.
  Sources sweet_early.c/.h,start.s,bsd_init.c; exact return/wait guards pass
  host ASan/UBSan (ON/PENDING/unknown/error/timeout/late-OFF/empty mailbox).
  C and assembly compile, patch applies/matches; disassembly confirms mailbox
  offsets, no stack, CPU_OFF0x84000002, and park on failed OFF.
  Local relink replaces only those three objects in the verified point16
  archive. Kernel21965608B SHA
  `405767f0bb59920cca89e792994f2ce3ef72015890d56fcefc7bee76fdb3b2db`.
  Evidence `research/nord2/smp/psci/`; image built and statically verified:
  `shim/out/nord2-xnu/nord2-smp-psci-stop17.img`,33312768B, SHA-256
  `e3974352333f4db100f7af5eb8cf189b7910095df8a283e3e0510881aea62620`.
  Payload SHA `0a1900bdc747773788f6a7714fdfe6c9bd504c01d904566512051d5682c8364a`.
  Static checks and independent comparison to original Linux kernel/both DT
  representations and tested rescue ramdisk all pass. Manifest and extracted
  payload in `research/nord2/smp/psci/`. User authorized this exact boot-only
  test; flash/reboot PASSED at13:26:28 CEST after hash/device/product/size checks.
  Evidence `research/nord2/smp/psci/hardware-test/flash.log`. ADB rescue log
  collector completed: firmware probe FAILED SAFELY, rescue return PASSED.
  Firmware PSCI1.1, CPU_ON core1/affinity100 returned0, but its completion state
  and printed mailbox fields stayed zero across5s. Point17 did not print
  magic/context, so their values are unknown. Probe stopped, reported0/7. All seven affinities were
  OFF before chain-back; authenticated root rescue returned, no blockfs/data
  mounts. Do not infer the secondary executed our entry or that SMP works.
  Pmsg15449B SHA `c0301be03cb6dbad4b55d785d55cd93d7b5d7416de50d18ef19d52efe30231f6`,
  `backup/nord2/logs/20261020-132632-smp-psci-stop17/pstore/pmsg-ramoops-0`.
  `hardware-test/verified.json` records the failure and safe return. Original
  boot restore not needed because rescue succeeded. Next inspect entry/context
  handoff; high physical entry/context support is not established. Earlier attempted
  Linux CPU baseline read found no ADB because phone was in fastboot;
  changed nothing. Full Apple-clang CI of ff8d66e dispatched:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39414925445
  Passing this only proves firmware startup/shutdown, NOT XNU SMP. After it,
  add coordinated secondary shutdown for kernel return, serialize diagnostic
  logging, then validate XNU with2/4/8 CPUs, per-core timer/IPI and workload
  evidence. Integrate concurrent USB only once its cleanup/reader is safe.
  Point16 full Apple-clang CI PASSED at f12b7d0:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39413072917
  Artifact/log downloaded under `research/ci-39413072917/`. CI kernel SHA-256
  `892739078cbbf08d1d75161a9da4d2c1528d6ca6b2e3f0e328df6a6b7c10c299`,
  matches artifact SHA256SUMS. Tested point16 image used the frozen local
  relink, not this new CI kernel.
  First run39412479509 failed due to my incorrect machine_config=NORD2;
  this branch uses machine_config=SWEET with NORD2 defined by patch0082.
  The rerun used SWEET and passed; this was a dispatch error, not a C failure.

- **Earlier milestone: point16 concurrent USB + HFS diagnostic PID1 PASSED;
  authenticated root ADB RAM rescue returned. Userdata unchanged.**
  Boot at that time contained the exact authorized image
  `shim/out/nord2-xnu/nord2-usb-userland-stop16.img`,33323008B, SHA-256
  `94ff103461f8d72af9d46134dd4c13349261d1739672d3b849d4b5a8eeee7989`.
  Payload SHA `e417ad487d0a80fbb501f6bb842f865376d4c7394afa0d5b18a0ba19201e46bb`.
  Boot-only flash/reboot passed at13:01 CEST after exact device/product/size
  and original backup hash checks. Native USB enumerated at13:02:41;
  received HFS mount, getpid1/EL0 success, ALL TEN one-second ticks and the
  expected diagnostic PID1 exit66. Background EP0 polling and BSD/userland
  therefore ran concurrently on the real phone. Live log17144B, SHA-256
  `8df33a70df7f47c9bd4e1bbd574e3928ce033afd6ebdd21d6f0928499863b0e4`,
  `research/nord2/usb/background/hardware-live/xnu-live.log`.
  66 status samples; last request141/reset2/log_read65/stall0/configured1.
  Expected initproc panic then generic USB cleanup returned to ADB rescue.
  Saved pmsg17322B, SHA-256
  `5e726f0ae613637e1de72f27eb95e201d35d5175b93f1a9b710cb9b80bcd732a`,
  `backup/nord2/logs/20261020-130111-usb-userland-stop16/pstore/pmsg-ramoops-0`.
  Live bytes equal its exact prefix. Root rescue marker and absence of
  block-backed filesystems or /data verified. No original-boot restore was
  needed: authorization specified it only if rescue return failed.
  Evidence `research/nord2/usb/background/hardware-test/verified.json`,
  `flash.log`, `host-kernel.log`, `mounts.txt`; build layout in `manifest.json`.
  Initial reader expired across approval/context BEFORE the actual test;
  `client-hardware.log` is that pre-test timeout. `client-active.log` is the
  successful reader restarted before XNU enumeration.
  This proves concurrent native EP0 logging, not bulk/CDC-ECM/full USB or
  Apple launchd. /sbin/launchd in the RAM image is our static diagnostic.
  Full Apple-clang CI of point16 passed; user now prioritizes SMP before
  moving from RAM diagnostics to the prepared iOS root on UFS.
  Do not write userdata until the exact install/boot artifacts are reviewed
  and the destructive write confirmed (broad deletion already authorized).

- **Previous device: point15 native USB LIVE LOG PASSED; ADB RAM rescue returned.
  Boot contained the authorized point15 image; userdata unchanged.**
  After installing/reloading the dedicated udev rule, repeated the SAME
  image using only fastboot reboot (phone had already returned to fastboot;
  attempted adb preflight found no device, so sent no ADB reboot).
  Native USB delivered14961B while XNU was active and ADB absent:
  `research/nord2/usb/phy-console/retry-live/xnu-live.log`,
  SHA-256 `3f1c8db40a7c05479d5e7bfea7003c0280a9f7ab07cfaf816cacc93d9fb7505f`.
  Protocol NUSB/version1/configured1,96 status samples,201 total requests,
  95 log reads,2 bus resets,0 stalls. Authenticated ADB rescue returned.
  Saved pmsg `backup/nord2/logs/20261020-124040-usb-phy-stop15-retry/pstore/pmsg-ramoops-0`,
  15484B, SHA-256
  `655a9ba01f5f2b6d2146522e303f6de9983d28ee8fe70174ac7d0523f3d54176`.
  Live bytes equal the exact prefix of recovered pmsg. Root rescue marker
  and absence of block filesystem mounts verified. Evidence:
  `research/nord2/usb/phy-console/{retry-verified.json,retry-host-kernel.log}`.
  This proves native USB EP0 log/status transport, not yet concurrent boot
  logging, bulk endpoints, CDC-ECM or a complete iOS USB stack.
  Background polling thread with HFS diagnostic PID1 now tested above;
  live progress received while XNU mounted and executed diagnostic userland.
  Preserve existing authorized images; a new candidate needs its own approval.
  Background candidate now built and authorized: `nord2-usb-userland-stop16.img`.
  Only point16 creates a kernel thread (requires explicit cpus=1), waits for
  its readiness with scheduler sleeps, then allows BSD to mount the RAM root.
  Thread uses the tested PHY/EP0 path; masks IRQ/FIQ only during each PIO
  iteration and sleeps1ms with interrupts restored. Existing point14/15
  synchronous loops remain. Deadline45s returns to rescue if PID1 stalls;
  normal expected end is diagnostic PID1 exit66 -> panic return cleanup.
  No UFS selected (`rd=md0 ufsrw=0 rootrw=0`).
  New `NORD2_LIVE_PROBE` compile mode in tools/nord2-init-probe.S emits ten
  messages separated by select_nocancel(407),nfds0,timeval1s. Exit75 means
  timer syscall failed. Default build remains byte-identical to old probe.
  Fresh 8MiB HFS root built and fsck/read-back verified in isolated KVM copy:
  `research/nord2/rootfs-live/nord2-hfs-pid1.img`, SHA-256
  `b5fa35f397e784c1da04988503ee43d9ce23e7925829a84be181d4ceba07308a`;
  diagnostic binary16635B SHA
  `18f1270d9dd9f535a07f95834fe490f3eb240a59d202d732a3191e8a3eddac9e`.
  Source C compilation, decoder/PHY tests and new background-start
  ready/failure/timeout/single-CPU guard tests passed; patch applies/matches.
  Local relink only replaces sweet_usb.o in the tested point15 archive.
  Kernel21961184B SHA
  `8cb2119f7b3c1505f3e2cc9eb0fba8cc9e7ce81f7b24a2d8d1e86c39d99c9002`;
  evidence `research/nord2/usb/background/`. Built and authorized; hardware
  success recorded above. Native reader used --read-seconds60; HFS/getpid1
  and all ten live ticks were present in the USB log.
  First point15 run before host permission correction follows:
  User explicitly authorized `nord2-usb-phy-stop15.img` after it was built
  and verified. Expected product, single-device identity, boot partition size
  and exact candidate/original-backup hashes checked; boot flash/reboot passed.
  Evidence `research/nord2/usb/phy-console/hardware-test/flash.log`.
  PC enumerated "Nord2 XNU console" at high-speed, descriptors byte-identical
  to the expected device/configuration descriptors. XNU reports resets2,
  requests10, log_reads0, stalls0, point15 stop and Linux return.
  Reader failed with LIBUSB_ERROR_ACCESS(-3); no live log was read yet.
  Saved pmsg `backup/nord2/logs/20261020-123430-usb-phy-stop15/pstore/pmsg-ramoops-0`,
  15482B, SHA-256
  `74fc5260f15b42a950559406c1157ace338810554c753c8f629db5d42ececcb0`.
  Evidence `research/nord2/usb/phy-console/hardware-test/verified.json`.
  PHY before: ACR0=00463c6e ACR5=40801000 ACR6=00000442 ACR3=c0040000,
  ACR4=0000001a DTM0=02be00d4 DTM1=0. Active DTM0=02000000,
  DTM1=00000226 ACR6=00100442; the inherited forced suspend/pulldown/
  transceiver/termination bits were released. MAC connected POWER60e1,
  final POWER60f1 DEV2b000003 OPSTATE12 EP0CSR40.
  Root rescue/marker verified, no block filesystem/data mounted.
  Userdata unchanged. Restore original boot only if rescue return fails.
  Host access correction: existing system70-uaccess rule matches ADB/fastboot
  INTERFACE descriptors, not VID/PID. Our vendor interfaceff5301 did not match.
  New `tools/70-nord2-xnu-console.rules` grants active-session uaccess only
  when VID18d1/PIDd001 + exact product and serial"nord2-xnu" match.
  Syntax verified; installed through pkexec/authentication at
  `/etc/udev/rules.d/70-nord2-xnu-console.rules`, exact contents compared.
  `pkexec udevadm control --reload` succeeded after system authentication.
  Same-image retry completed successfully above, without another boot write.
  Prior point14 result follows:
  User approved the exact boot-only image and original-boot restoration if
  rescue return fails, then brought the phone to fastboot. Product oplus6893,
  boot size32MiB, non-userspace bootloader and both image/backup hashes checked.
  Flash/reboot passed at 2026-10-20 12:20 CEST. XNU reached the native-console
  loop and completed it: resets=0 requests=0 log_reads=0 stalls=0, then point14
  stop and Linux return. ADB rescue returned at12:23; root shell/marker verified,
  no block-backed filesystem or /data mounted. User also saw the return marker.
  No original-boot restore needed (authorized only if rescue return failed).
  Saved pmsg `backup/nord2/logs/20261020-122100-usb-console-stop14/pstore/pmsg-ramoops-0`,
  14850B, SHA-256
  `c79bb0e57bd96a114f2637d5debe4a7ab15a2a4bb4e242a5b7f21f404f99a58a`.
  Host journal shows no native connection during the XNU window, only Linux
  ADB afterward. Reader timed out; no live log was captured. Evidence:
  `research/nord2/usb/ep0/hardware-test/{verified.json,host-kernel.log}`.
  Next: inspect/restore USB2 PHY state as well as MAC. Point13 proved clocks
  and MAC registers readable, not that the analog PHY remained operational.
  Linux `phy-mtk-tphy.c` uses generic-tphy-v2 COM at11e40300 and explicitly
  clears suspend/termination/pulldown/UART overrides, BC1.1 switch and GPIO
  mode during power-on. These have not been read or restored by XNU yet.
  Point14 implementation and failed-attachment evidence committed as5278c0a.
  **Next candidate: point15, USB2 PHY activation plus the same EP0 console.**
  Source adds a guarded snapshot of seven COM registers (base11e40300;
  offsets0,14,18,1c,20,68,6c), Linux's suspend pulse/delays, release of pin
  overrides, BC1.1/GPIO/test-pullup overrides and device-mode values.
  Analog calibration, supply/clock registers, charger IC and Type-C routing
  are preserved. Generic return disconnects MAC before restoring all seven
  saved PHY registers, with suspend/termination controls last.
  Logs include inherited PHY values and post-test MAC/PHY readback.
  Point14 remains available without PHY writes; point13 remains read-only.
  Exact-code ASan/UBSan tests passed for failed map/read, allowed bit scope,
  pulse order, delays, full rollback after disconnect, 1000 inherited states.
  Request decoder/pmsg and read-only probe regression tests still pass.
  Tool `tools/nord2-check-usb-phy.py`; evidence
  `research/nord2/usb/{phy-host-check.log,phy-compile.log,phy-console/}`.
  C compile and patch apply/match passed. Local relink replaces only
  sweet_usb.o in a copy of the tested point14 archive; kernel SHA-256
  `05dc0fc424d826b7196ab6b429a5857bf92567a05ccab348b43ea7a3f1d8daff`.
  Image `shim/out/nord2-xnu/nord2-usb-phy-stop15.img`,33312768B, SHA-256
  `e5cfe515f53bc38b1f1e534d5abff5da1281c3ae0bc2aeb25a4f8fa82b416a63`.
  Payload SHA-256
  `17131df6192cf9dd2bd3f8645ab4374a648257d5dfdfb374cf17e3041541874c`.
  All static image/layout checks passed; original Linux kernel and both DT
  representations match the original backup exactly, rescue ramdisk matches
  the tested unstripped rescue. Manifest in `phy-console/manifest.json`.
  Cmdline differs from point14 only by `nord2stopat=15`.
  This exact image is authorized/flashed; hardware result recorded above.
  Source committed/pushed as28b69c923ad6c3ac1dd81b25297f45b08f908d40.
  Full Apple-clang rebuild PASSED:
  https://github.com/frxncyyy/ios-sweet/actions/runs/39408508581
  Downloaded run.json/build.log and xnu-kernel under
  `research/ci-39408508581/`. CI kernel17224624B SHA-256
  `29f27e75e228a11877c184183bd8dd04786cbe3ac66275ca45379113c3b2946c`
  matches artifact SHA256SUMS. Hardware tests used the frozen local relink,
  not this newly downloaded CI kernel.
  Image `shim/out/nord2-xnu/nord2-usb-console-stop14.img`,33312768B, SHA-256
  `80d3b5f0e3b428f5036784e924082e419f933bd95d59ef088f4dd239df159c6f`;
  payload `48fde2c0e404f2d45f53f08e08bd9f53a41c436774ace1358c633ac9bcc30800`.
  Kernel `research/nord2/usb/ep0/kernel.nord2-usb-console`, SHA-256
  `ed3456bbe6e2eba48336e97354f451d1d2674dc8267c92772f542dd26b9ffa8d`.
  Logs/evidence in `research/nord2/usb/ep0/{hardware-test,hardware-live,client-hardware.log}`.
  Initial reader expired while waiting for user/continuation, before flash;
  `client.log` is that pre-test timeout, not a hardware failure.
  Cmdline `debug=0xc cpus=1 keepsyms=1 -noprogress nord2stop=0 nord2vm=1
  nord2fb=1 nord2stopat=14 rd=ufsroot0 ufsrw=0 rootrw=0`.
  Static checks passed, including original Linux kernel/DT and tested rescue
  ramdisk preserved exactly, source-end bound and no root mount before stop14.

- **Previous device state: USB point13 passed; phone was in ADB RAM rescue.
  Userdata unchanged.** Boot still contains the combined point13 image.
  No restore was required (authorization requested restoration on failure).
  Saved pmsg `backup/nord2/logs/20261020-115727-usb-snapshot-stop13/pstore/pmsg-ramoops-0`,
  16818B, SHA-256 `93d581ef09ad0b08fbe96b5256e235ff876bee93d8f988ca8ddbcf792fd73f08`.
  Clock and complete MAC snapshot, point13 stop and Linux-return markers
  all observed; authenticated root ADB returned. Evidence:
  `research/nord2/usb/stop13-test/{flash.log,verified.json}`.
  Observed INFRA_CG3=00000100, TOP_CFG9=01000102, TOP_CFG10=05050201;
  device power=0, USB2 reset status=1, port=48. MAC remains powered/clocked.
  IPPC HWID20180724/version10010000; MAC cap endpoints8/8, FIFO8192/8192,
  MISC_CTRL80000003 (36-bit DMA and forced VBUS), POWER_MANAGEMENT60a1
  (high-speed enabled, soft-connect off), EP0CSR40. EP1 TX/RX QMU inactive
  and request-list addresses zero. This supports reusing the bootloader's
  hardware initialization for an EP0 enumeration test; it does not prove
  USB enumeration/data transport in XNU.
  Full Apple-clang CI at head `c3d382778d27086fd09a54c5998a6dae5373e8c1`
  PASSED: https://github.com/frxncyyy/ios-sweet/actions/runs/39404509443
  (`research/ci-39404509443/`, run.json/build.log/xnu-kernel).
  CI kernel17207320B SHA
  `00229d652c6cede97fea1e66266eefdd230795908e938f8d7da9064f25fa78e7`,
  matches published SHA256SUMS. Tree through c3d3827 pushed to fork;
  local follow-up8ffa8c0 records the point13 result.
  **Native USB console at point14 (hardware result above).**
  This is EP0 enumeration + bounded vendor-IN log/status requests, not yet
  CDC-ECM/data endpoints or full iOS USB. No DMA is enabled. Refuses active
  inherited QMU queues or an already-connected MAC; uses verified existing
  clocks/PHY/VBUS. Serves for45s with interrupts masked and explicit watchdog
  service, then disconnects/restores MAC state and returns to ADB rescue.
  Generic panic return also calls its no-op-unless-owned cleanup.
  - Native descriptor uses the current recovery VID18d1/PIDd001;
    a dedicated host udev rule is needed (see point15 result above).
    Product is "Nord2 XNU console",
    serial "nord2-xnu", vendor interfaceff/53/01 (NOT ADB).
    `tools/nord2-usb-console.py OUTPUT [--wait 180]` selects that exact product
    via sysfs/libusb, reads protocol status request0x52 and pmsg request0x51
    (wValue=byte offset, wIndex0, max1024 per transfer). System libusb used,
    no pip/global packages or host network setup needed. Start it BEFORE
    flashing/rebooting the eventual authorized point14 image.
  - EP0 descriptor/decoder and pmsg reader tests pass ASan/UBSan, including
    100000 malformed requests and boundary/header validation. Read-only
    point13 harness still passes. Native client ignores current ADB recovery
    (negative selection test passed). Hardware test did not enumerate;
    bounded loop and disconnect/return path did complete.
    Files: `tools/nord2-check-usb-control.py`, `tools/nord2-check-usb-probe.py`;
    evidence `research/nord2/usb/ep0/`. Both changed C files compile.
  - Working sources edited in `research/ci-39158169212/buildtree/xnu`,
    patch0082 regenerated; updated header copied to compilation tree
    `research/ci-39398655023/buildtree/xnu/osfmk/arm64/sweet_early.h`.
    Relink replaces sweet_usb.o/sweet_early.o in a COPY of the tested
    point13 archive. `nord2-usb-console-stop14.img` Zopfli build passed;
    exact candidate and authorization are recorded above.
  Standalone ADB RAM rescue was tested successfully earlier:
  User explicitly authorized boot-only `nord2-adb-rescue.img`, SHA-256
  `d78bf9fe720a1d293b863ea0a8b30e0df4b5eafb097b1343a3c30e7c3d03bac5`,
  and original-boot restoration if it failed. Flash/reboot passed. ADB reports
  `recovery` (use direct shell or wait-for-recovery, NOT wait-for-device).
  Root shell works; `/nord2-rescue` marker and original Linux kernel match.
  No block-backed filesystem or `/data` is mounted. Exact image prefix was
  read back from boot and matched; full partition hash differs because of
  remaining partition padding. Evidence:
  `research/nord2/rescue/test/{flash.log,verified.json,boot-readback.img,mounts.txt,dmesg.txt}`.
  Pstore is mounted but contains no previous record after bootloader reset;
  do not claim an XNU pmsg was retrieved in this standalone rescue test.
  `adb exec-out` can return exit0 with an error in stdout: check existence
  with `adb shell` before collecting a binary log. `getprop` is absent in the
  minimal rescue; this does not prevent root shell/dd/dmesg access.
  Userdata is `/dev/block/sdc57`, start25231360 and size224657344 in 512-byte
  sectors, exactly LUN2 LBA3153920/count28082168 at 4096 bytes.
  The recovery partition and userdata have not been written.
  Source builder `tools/nord2-mkrescue.py` retains authenticated USB ADB and
  embeds only the host public key in private output artifacts. Linux kernel
  and DT remain byte-identical to the original boot. Unstripped rescue size
  28,884,992 bytes; optional metadata-only stripping produces a different,
  UNTESTED compact image (`648b295c…40447e`), not currently used.
  - Generic XNU return now quiesces/restores the UFS host if it owns it,
    before changing translation state. Failed cleanup halts with DMA memory
    retained. No locks/allocations in panic cleanup. C compiles and exact-code
    ASan/UBSan checks passed (inactive/idempotent/writable-root/pending-clear/
    unsafe-DMA cases). Patch0082 regenerated and applied cleanly.
    Evidence/local relink: `research/ci-39398655023/return-rescue/`.
    Kernel SHA `433e767f57c8e76ddce2e5ed11600eee0fdb6106744701bebd30eb1d966fe84f`.
  - Preparing XNU+rescue combined boot: default gzip exceeds the 32MiB boot
    limit. Packer now accepts local Zopfli and checks decompressed bytes
    exactly; helper accepts --base-boot/--zopfli. Local Zopfli from
    https://github.com/google/zopfli commit
    `df1517dd8518113a245e4d203a04b8a0e02ab493`; no global installation.
    `nord2-rescue-stop12.img` built and all static checks passed: 33,314,816B,
    SHA-256 `7ec48eb45e4533fae5b40a0438153c6c31b4c7d9ee14a6c1cd8a336843503d4e`.
    Unstripped, already-tested rescue ramdisk is preserved exactly.
    This stop12/rescue candidate was not flashed. Combined USB point13/14
    images were tested later as recorded above; userdata remains unchanged.
  - User proposed native XNU USB and encouraged a complete implementation.
    Local MT6893 driver and live DT identify MediaTek MTU3, MAC11201000,
    IPPC11203e00, USB2 PHY11e40000, high-speed. Existing Sweet driver is
    Qualcomm DWC3 and remains excluded. Plan: MTU3 enumeration/control,
    then data endpoints for console/transfers; Linux EP0 is PIO, QMU handles
    data DMA. Need inspect inherited clock/PHY/controller state on XNU.
    This was the initial USB plan; current point13/14 results are above.
    First source milestone: `nord2_usb_probe()` in `bsd/dev/sweet_usb.c`
    is compiled only on Nord2, completely excludes the Qualcomm DWC3 code,
    and runs only at `nord2stopat=13` before root selection. It checks
    INFRA_CG3 bits1/31 and TOP_CFG9 bit31/TOP_CFG10 bit7 before IPPC access,
    then device power/reset/port state before MAC access. No register writes,
    FIFO reads, DMA, clock changes or disk access. It returns to the Linux
    ramdisk via the normal stop hook. Local C builds passed; exact-code
    ASan/UBSan harness uses read-only mappings and covers inactive selection,
    all mapping failures, four clock gates, five device gates and full
    register reads (`tools/nord2-check-usb-probe.py`, evidence `research/nord2/usb/`).
    - Reuse candidate for later MTU3 implementation: existing Sweet USB CDC-ECM
      descriptors, XNU ifnet integration and UDP log sender (192.168.7.1:5140).
      Replace DWC3 hardware operations with MTU3 EP0/QMU operations. Existing
      Nord2 `_cnputs` feeds only pmsg, not sweet_log_append; wire a RAM log
      reader/feed before claiming the existing UDP sender will contain logs.
      Keep UFS persistent log writer disabled. QMU 36-bit DMA capability is
      MISC_CTRL bit31; check it before using buffers in XNU's >4GiB region.
      IPPC offset0xa4 selects GPD generation in Linux. Full USB device data
      transport, clock/PHY activation and safe QMU teardown remain unimplemented.
    - Hardware source is `../nord2-current-kernel`, commit
      `04d00c2565764c3922c9b4826b5acff1106e533f` (MT6893 uses clk-mt6885.c).
      Live DT raw properties saved under `research/nord2/usb/` confirm the
      register ranges, SPI96, sys_ck/ref_ck and high-speed maximum.
      Debugfs is disabled in the current Linux kernel; a temporary debugfs
      mount attempt in `/tmp` failed with ENODEV and changed no disk state.
    - New local kernel with USB snapshot:
      `research/ci-39398655023/return-rescue/kernel.nord2-usb-snapshot`,
      SHA-256 `abbfb05ef2a39728df892e491f94445780220ed6e48b6bc80f14b60f0138d518`.
      Candidate `shim/out/nord2-xnu/nord2-usb-snapshot-stop13.img`:
      33,308,672 bytes, SHA-256
      `19b1aa82a361331ee94fc014295e5acdcdfe2bb1b42f1dede8a166ea47f5eb7b`.
      Payload SHA-256
      `a4582e51cb9f0f3a37a04ea066a03f9d6ccb5043074145610cf21e99acedf8c6`.
      All static checks passed, including unchanged original Linux kernel/DT,
      exact tested rescue ramdisk, source-end bound, USB probe marker and
      ufsrw=0/rootrw=0. No root filesystem is selected before the stop.
      **User authorized** this exact boot-only test and brought the phone
      to fastboot. Product/partition/hash checks passed, flash/reboot passed;
      evidence `research/nord2/usb/stop13-test/flash.log`. ADB rescue returned
      and log verification passed as recorded above.
      Rescue log collector now supports `tools/nord2-pull-logs.sh --rescue LABEL`;
      hardware read-only check passed and saved
      `backup/nord2/logs/20261020-115541-standalone-rescue/`.
      Its boot reason confirms PMIC_cold_reboot, explaining empty pstore.
      Uses shell-v2 exit status and bounded wait-for-recovery, no su/getprop.

- **Completed point12: original Android boot restored and verified.**
  User authorized the exact boot-only test/restore. Flash and reboot succeeded;
  saved pmsg is `backup/nord2/logs/20261020-110041-ufs-root-stop12/pstore/pmsg-ramoops-0`,
  15051 bytes, SHA-256 `b37d2e694b395d3ade3e62966c65f099ffea5d777b2a5c4a598463f6ab2eb4b0`.
  Observed correct GPT CRCs/extents, `userdata` on LUN2 read-only, `BSD root:
  ufsroot0`, `root probe host restored, ready=1 writable=0`, the pre-mount
  stop and `RETURN TO STOCK ANDROID KERNEL`. No filesystem was mounted.
  Original boot then flashed back; Android `sys.boot_completed=1` and full boot
  SHA-256 `04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01`.
  Evidence: `research/ci-39388874797/stop12/{flash.log,restore.log,restore-verified.json}`.
  This fifth test authorization is completed. No userdata changes.
  Source/tools through commit `0401f81` pushed to the fork. Full Apple-clang
  CI rebuild dispatched: https://github.com/frxncyyy/ios-sweet/actions/runs/39398655023
  (head0401f81; build PASSED, evidence `research/ci-39398655023/`).
  This CI is a source-reproducibility check; the prepared candidate below uses
  the already tested local point12 relink. Its bytes/hash will not be replaced.
  **Earlier device state: HFS/PID1 passed; original Android boot restored and verified.**
  User authorized this exact boot-only test/restore. Saved pmsg:
  `backup/nord2/logs/20261020-111243-hfs-pid1/pstore/pmsg-ramoops-0`, 15759 bytes,
  SHA-256 `ec60ff389f0868f4b68b52a91f2349d84e6dc972f83d3bb75475caab8f118a44`.
  Observed `BSD root: md0`, `hfs: mounted Nord2Probe`, diagnostic success
  `HFS read verified; getpid=1; EL0 syscalls OK`, voluntary exit66, expected
  initproc panic (prints exit status66, not hex0x4200), then Android return.
  Original boot restored; full partition SHA matches `04e84c4a…6e1f01`,
  `sys.boot_completed=1`. Evidence in `research/ci-39388874797/hfs-pid1/`
  (`flash.log`, `restore.log`, `restore-verified.json`). Sixth test completed.
  **NEW USER AUTHORIZATION (this Nord2, 2026-10-20):** asked which constraint
  applies before placing iOS on userdata; user answered exactly:
  "Possono essere cancellati; non serve il backup dei dati".
  Android userdata replacement is now in scope; no full data backup requested.
  No userdata write has happened yet. Preserve bootloaders/recovery/modem and
  existing backups. Prepare concrete iOS boot/recovery/log path before flashing.
  Historical next steps after PID1: prepare ADB rescue and generic UFS cleanup
  before replacing userdata. Both were implemented later (see above).
  Rescue and combined chain-back are now hardware-tested; generic cleanup
  with an owned UFS root is only host-tested so far. RAM-root diagnostic was
  unaffected because UFS was never selected.
  Existing recovery is backed up (128MiB); parsed locally under
  `research/nord2/recovery-unpacked/` without changing the device. It contains
  Lineage Android15 recovery init/adbd, configfs USB scripts and all shared libs.
  Android currently reports FBE + dm-13 data filesystem; do not assume raw
  userdata can be used as a filesystem while Android is mounted.
  Tested candidate:
  `shim/out/nord2-xnu/nord2-hfs-pid1.img`, 30,584,832 bytes,
  SHA-256 `c143559d225622b2b96fbdf14ff830cfd513ec0c468b199e6ee4161a6692145d`.
  Same point12 kernel; new 8MiB HFS RAM root, `rd=md0 nord2stopat=0
  ufsrw=0 rootrw=0` and existing single-CPU/debug/VM/display args.
  `tools/nord2-init-probe.S` is a bounded diagnostic PID1, NOT Apple launchd.
  It requires getpid=1 and exact HFS file contents, prints success then exits66.
  Expected final PID1-exit panic (status0x4200) invokes the existing Android
  return hook. Failures exit70..74. Require HFS mount + success text + exit66
  and Android return; a reboot or panic alone does not prove success.
  UFS root init is selected only by rd=ufsroot0; this RAM-root test skips it.
  Image helper now accepts --ramdisk; checker validates ADT/segment/range/no
  overlap. Negative checks reject wrong RAM address/size/root/kernel overlap.
  Embedded RAM disk exactly matches the fsck-clean local image. All static
  image/layout checks passed, including actual loader source-end limit.
  Evidence: `research/ci-39388874797/hfs-pid1/`; HFS generation/read-back:
  `research/nord2/rootfs-vm/{run-pid1.py,pid1.log,nord2-hfs-pid1.img}`.
  RAM disk SHA-256 `f2085fc34e65f5e3728dcf72308702578e84884cda4afc7f6a3ad68aefa77e3b`,
  PA0x144280000; payload SHA-256
  `c01bed5a551e6aa169b24175e9e07df30a85c0029b8cb57e2877defaa20966d7`.
  HFS folder-count repair applied after Linux copy; final fsck/read-back pass.
  Full Apple userland has NOT run on this Nord2.
  `shim/out/nord2-xnu/nord2-ufs-root-stop12.img`, 30,572,544 bytes,
  SHA-256 `c02eadfece4ff731f3234d13132ffb823280e5f3f12ef430f9135b69d6398d72`.
  Cmdline: `debug=0xc cpus=1 keepsyms=1 -noprogress nord2stop=0 nord2vm=1
  nord2fb=1 nord2stopat=12 rd=ufsroot0 ufsrw=0 rootrw=0`.
  - Root driver now selects LUN2, checks capacity/4096-byte blocks and both
    GPT CRCs/bounds before publishing `ufsroot0`; Nord 2 accepts only the
    known MMIO region and `ufsroot=userdata`. The ADT adds that UFS node.
    Qualcomm input/USB and background persistent log/sample startup are
    excluded on Nord 2. Point 12 forces read-only even if ufsrw=1 is supplied.
  - After `setconf`, before mounting any filesystem, point 12 restores the
    inherited host and chain-backs. Expected success is
    `root probe host restored, ready=1 writable=0` and the publication stop.
    Initialization failures also restore/quiesce; failed restoration halts
    rather than returning with active DMA. The image checker requires
    matching UFS ADT/kernel support and explicit ufsrw=0/rootrw=0 for point12.
  - Exact-code ASan/UBSan harness now includes root init/publication,
    forced read-only, actual userdata extents, failure cleanup and refusal
    of other partitions, in addition to all point11 tests.
    C compilation, patch regeneration and all image checks passed.
    Artifacts: `research/ci-39388874797/stop12/`.
    Same new-CI base archive; only BSD init/UFS and point10 early C replaced.
  - **Host iOS HFS image built, NOT written to the phone:**
    `research/nord2/rootfs-vm/ios16-root-20H392-nord2-v1.img`, 16 GiB,
    about12 GiB used, case-sensitive journaled HFS+, source16.7.16/20H392.
    SHA-256 `8b1d9de6775674afe790a161419ea23e824c5e045f21a66c6fc886125c991cb3`;
    adjacent `.json`/`.sha256` manifests saved. Nord2 userdata replacement
    was newly authorized above; upstream Oct15 permission was for Sweet.
    Base builder operations plus `tools/sweet-root-tweaks.py` applied inside
    a local KVM guest; final `fsck.hfsplus -f -n` reports clean.
    launchd/dyld original hashes, root:wheel0755,44 dyld cache hard links,
    logd markers and version checked. All47 critical files read back with hashes/owners/modes matching the
    source; `verify.log` reports `ALL_CRITICAL_FILES_MATCH 47`.
    No mobileactivationd container exists yet, so the tweak script reported
    skipped data_ark; retain that limitation until first userland startup.
  - VM tooling/evidence: `research/nord2/rootfs-vm/{prepare.py,run.py,
    smoke.log,build.log,finish.log,verify-content.sh}`. Host kernel/modules
    7.2.9-arch1-1, KVM, 2GiB guest, readonly9p inputs. No host sudo/install.
    Local Debian packages (URLs+hashes in `../toolchains/rootfs-tools/downloads.json`):
    busybox-static1.37.0-6+b9, hfsprogs540.1.linux3-6.
    Root APFS trees were archived with GNU tar/PAX/numeric owners:
    OS8,830,801,920 bytes, SystemOS3,363,665,920, AppOS9,768,960.
    Direct9p tree copy was slow and aborted; `incomplete-slow-9p.img` is NOT
    an installation image. Tar-based copy finished in ~93s; initial build
    missed /dev/stdin, then `finish` added it and repaired the same32 Unicode
    names and HFS folder counts known from upstream. Final checks passed.

- **Completed point-10 test — original Android boot restored
  and verified.** `sys.boot_completed=1`; full boot partition SHA-256
  `04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01`.
  User explicitly authorized the third boot-only test and original-boot
  restore. Flash/reboot and restore evidence:
  `research/ci-39388874797/stop10/{flash.log,restore.log,restore-verified.json}`.
  This third test's authorization is completed. No userdata changes.
  - Saved pmsg: `backup/nord2/logs/20261020-101745-ufs-snapshot-stop10/`,
    15235 bytes, SHA-256
    `1850793489a325bcf584e57dc9d9cb620aba4bc3529b8caa3fcf1e2f049602ee`.
  - Observed UFSHCI: CAP `0x3587071f`, version `0x200`, IS `0`, IE `0x30e75`,
    HCS `0x10f`, HCE `1`; UTRLBA `0x483ba000`, upper `0`, doorbell `0`,
    run/stop `1`; UTMRLBA/upper/doorbell all `0`, run/stop `1`.
    Controller enabled, device and request lists ready, 64-bit addressing
    advertised, no pending transfer/task requests. This proves MMIO access;
    DMA and SCSI transport were untested at point10 (verified at point11).
  - Exact final markers: `UFS snapshot end`,
    `stop at after read-only UFS host snapshot`,
    `RETURN TO STOCK ANDROID KERNEL`. Android log collector completed normally.

- **Completed point-11 test — original Android boot restored and verified.**
  User authorized this image and boot-only flash/restore. Test flash/reboot
  succeeded; Android returned and log collection completed. Original boot
  subsequently restored via fastboot; full boot hash matches the original
  `04e84c4a…6e1f01`, Android `sys.boot_completed=1`. Evidence in
  `research/ci-39388874797/stop11/{restore.log,restore-verified.json}`.
  This fourth test authorization is completed. No userdata changes.
  - Pmsg: `backup/nord2/logs/20261020-103234-ufs-read-stop11/`,
    15057 bytes, SHA-256
    `cfc0e9098dc842d4c315e71f00d4b06eb68e412c8a15dd5f6fbc2f3f8256ea89`.
    DMA at `0x145024000` accepted; capacity31,240,192 x4096;
    boot309504/8192 and userdata3153920/28082168; GPT128 entries and CRCs
    `ab8324bc`/`97de707e` exactly match Android baseline.
    `inherited request list and interrupts restored`, `read probe result=0`,
    point11 stop and Android return markers all present. UFS DMA and
    read transport are VERIFIED; publication later verified at point12; mount still untested.
  `shim/out/nord2-xnu/nord2-ufs-read-stop11.img`, 30,572,544 bytes,
  SHA-256 `2027c467d8162966d9ba7cbb20bd8b1cf550960d48ebec94d4fa53619b00f8f6`.
  Cmdline uses `cpus=1 nord2stopat=11`; all image checks pass.
  - Adds a pre-root probe in `bsd/dev/sweet_ufs.c`, active only at point 11:
    takes over the idle inherited UFS request list, submits TUR,
    READ CAPACITY(10), READ(10) of primary GPT metadata on LUN2, validates
    header/entry CRC32 and partition bounds, prints boot/userdata extents.
    The command guard allows no media writes and no reads outside LBAs 1..63.
    No disk publication, root mount, persistent log or Qualcomm input/USB.
  - Before chain-back, waits for idle, restores original UTRL base, run/stop
    and interrupt-enable registers with read-back checks. A timed-out slot
    gets a bounded clear. If DMA cannot be quiesced or state restored, stays
    halted with DMA memory intact and requires a physical reboot; never
    chain-backs into Android in that state.
  - Source compiled and patch 0082 regenerated/verified. Exact-code host
    harness with ASan/UBSan passes success, inactive gate, non-idle host,
    missing 64-bit DMA, unit attention, corrupt GPT CRCs, bad capacity/tag,
    rejected request-list address, command timeout cleanup and unsafe-DMA
    halt. Rejects all CDB opcodes other than TUR/RC10/READ10 and all writes.
    Harness and logs: `research/ci-39388874797/stop11/`.
    Relink replaces only `bsd_init.o`, `sweet_ufs.o`, `sweet_early.o` in a
    copy of the new CI archive; original archive hash unchanged.
  - Android baseline read-only evidence: `backup/nord2/ufs-baseline/`.
    `/sys/.../0:0:0:2`, 31,240,192 blocks of 4096 bytes; GPT 128 x128 entries
    at LBA2, header CRC `ab8324bc`, entries CRC `97de707e`.
    Boot starts 309504, length8192 blocks; userdata starts3153920,
    length28082168 blocks. Expected XNU readout must match this evidence.
  - Host APFS reader built locally: `../toolchains/apfs-fuse`, upstream
    commit `66b86bd525e8cb90f9012543be89b1f092b75cf3` plus one local
    `#include <cstdint>` in `ApfsLib/PList.h` required by GCC16.
    CMake `-DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Release`.
    Read-only user FUSE mounts currently under
    `research/nord2/ipsw/root-images/mounts/{OS,SystemOS,AppOS}`.
    launchd, dyld, SystemVersion and all 44 dyld cache files read fully,
    hashes/ownership/modes in `critical-files.json`; version16.7.16/20H392.
    HFS image subsequently built as recorded above. No system installation or privileged host mounts.

- **Completed point-9 test — original Android boot restored and verified.**
  Point 9 **passed**, including `SWEET CPU: requested=1 selected=1`,
  `cpu=0 mpidr=0x0 came alive; SGI1 OK`,
  `1/1 CPUs initialized, SGI1 verified`, BSD initialization, network threads
  being scheduled, `calling inittodr`, then
  `stop at before root filesystem selection` → `RETURN TO STOCK ANDROID KERNEL`.
  Android took longer than 90 seconds; user confirmed it was up, and ADB
  `sys.boot_completed=1` confirmed. Do not classify these verbose probes
  as hangs at 90 seconds.
  - Pmsg: `backup/nord2/logs/20261020-100725-cpu-match-stop9/`, 14,519 bytes,
    SHA-256 `25c55ce36efbc2d01f283a30e9a698d2214dca4f2a42031840994fad4f22435e`.
  - This second test was explicitly authorized, completed, and restored via
    fastboot. Full boot hash again
    `04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01`,
    Android boot completed. Evidence in
    `research/ci-39388874797/stop9/{flash.log,restore.log,restore-verified.json}`.
    No userdata modification authorized or done.
  - Point-10 image (test and restoration completed above):
    `shim/out/nord2-xnu/nord2-ufs-snapshot-stop10.img`, 30,570,496 bytes,
    SHA-256 `0a50a1ae5c5548ab716d5093478c67beb99c8380f49621c37eb05eaa8f91db1e`.
    All image checks passed. Read-only UFSHCI register snapshot
    at the same pre-root location. Base `0x11270000`, extent `0x2300` from
    live DT; standard offsets from the user's Linux `drivers/scsi/ufs/ufshci.h`.
    No controller reset, register writes, DMA or SCSI commands. Only runs
    with `nord2stopat=10`. Host check of the exact helper with an mmap page
    protected `PROT_READ`: zero accesses at point 9, exact 14 register reads
    at point 10, mapping-failure path still reaches the return hook.
    Evidence and local C-only relink (new CI archive, only `bsd_init.o` and
    `sweet_early.o` replaced): `research/ci-39388874797/stop10/`.
    The working new-CI buildtree's `sweet_early.h` was updated with the helper
    declaration; its original tar archive is unchanged.
    Storage transport and root mounting remain unported.
  - IPSW OS/SystemOS/AppOS images extracted on the HOST only, using the
    selected d22ap BuildIdentity, ZIP CRC verified while streaming and SHA-256
    recorded in `research/nord2/ipsw/root-images/extraction.json`.
    Files: `OS.dmg` (4,730,814,044 bytes), `SystemOS.dmg` (2,743,074,816),
    `AppOS.dmg` (10,485,760). Full HFS root has NOT been built. Host lacks
    apfs-fuse/hfsprogs and `sudo -n` requires a password; no system changes
    or privileged host mounts attempted.

- **Completed point-8 test — Android restored and verified afterwards.**
  Stop 8 **passed**: the saved log contains `irq intid 0x...0001`, then
  `stop at before bsd_init` → `RETURN TO STOCK ANDROID KERNEL` →
  `entry=0000000040280000`. Android reached `sys.boot_completed=1`.
  This proves the kernel returned from `kperf_init_early()` and reached the
  genuine point-8 hook. The anticipated `SWEET CPU: 1/1...` text is absent
  from this pmsg; do not quote it as an observation. The SGI1 IRQ and passed
  wait are the actual evidence.
  - Log: `backup/nord2/logs/20261020-095916-cpu-match-stop8/`, pmsg
    10,553 bytes, SHA-256
    `f7f18d47b769cf437651bb42410175a58f7ca43faf903205617b9b05959e7576`.
    Android reports the original LK `PMIC_cold_reboot` boot reason even on
    this successful chain-back; nonempty pmsg + markers prove preservation.
  - Original `backup/nord2/boot.img` flashed back, rebooted, full partition
    SHA-256 `04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01`
    and `sys.boot_completed=1` verified. Evidence:
    `research/ci-39388874797/stop8-restore{.log,-verified.json}`.
    This test's flash/restore authorization is completed.
  - Point-9 image (test and restore completed above):
    `shim/out/nord2-xnu/nord2-cpu-match-stop9.img`, 30,570,496 bytes, SHA-256
    `d57e8f097f005e6e13533f9d94bd40923f6797015b22302bc3a61cc4c639ae13`.
    Adds point 9 after `inittodr(0)`, before the root filesystem mount loop
    in `bsd_init`. Uses the **new CI archive**, replacing only `bsd_init.o`;
    all Apple-compiled C++ objects including the CPU matching fix are retained.
    `cpus=1 nord2stopat=9`; all image checks passed.
    Source comparison against the new CI tree shows only the point-9 hook
    added to `bsd_init.c`; original CI archive hash unchanged after relinking.
    Build files, exact link argv, link warnings and checker output are in
    `research/ci-39388874797/stop9/`. Patch 0082 regenerated/verified.

- Session 8 stop-8 test authorization and artifacts: user explicitly
  authorized a temporary boot-only flash and subsequent original-boot
  restore, and put the phone in fastboot. Preflight: expected Nord 2,
  `product=oplus6893`, boot size `0x2000000`, not userspace fastboot; image and
  original backup hashes verified. Flash and reboot both returned OK.
  Test and restore completed as recorded above.
  - Candidate: `shim/out/nord2-xnu/nord2-ci-cpu-match-stop8.img`,
    30,676,992 bytes, SHA-256
    `7348e1782171947a6f39019c71d6a3b8b9adc65f136913afa8966021d75f7509`.
    Cmdline: `debug=0xc cpus=1 keepsyms=1 -noprogress nord2stop=0 nord2vm=1
    nord2fb=1 nord2stopat=8`. All image checks passed.
  - CI artifact kernel SHA-256:
    `97af2e32c7e2d86f850ecab81e19b9a8048c8e81105de3d69b5d719250cb4cca`
    (verified against CI's `SHA256SUMS`).
    Logs/manifests: `research/ci-39388874797/{run.json,build.log,
    stop8-image-check.log,stop8-flash.log}`.
  - The boot-argument defect below was reproduced by compiling the exact
    `getval()`/`isargsep()` source on the host:
    `research/nord2/bootarg-parser-check.{c,log}`. `0008` is STR;
    `8` and `0x08` are NUM 8; `0010` is NUM 8, not decimal 10.

- Session 8 build **passed**: fix commit `d550a21`, pushed to
  `frxncyyy/ios-sweet:codex/nord2-bringup`; macOS workflow
  https://github.com/frxncyyy/ios-sweet/actions/runs/39388874797.
  Local checks passed: patch 0082 applies and reproduces all edited files;
  C++ configuration syntax; serialized ADT CPU identities/affinities; all
  eight CPU nodes match exactly one personality. The image checker now also
  verifies CPU matches against the **embedded kernel**, excluding the ADT
  itself from its search. Negative checks reject the old CI kernel with the
  new ADT, one missing boot-CPU binding, and an absent CPU subtree.
  `shim/nord2-xnu/README.md` and `DISPLAY-LOG.md` have been updated to reflect
  the verified late stops, live display, CI build requirement, and actual
  payload address `0x43744000`. See the current device state above for testing.

- **Second source-confirmed defect in the old diagnostics:** XNU's
  `pexpert/gen/bootargs.c:getval()` interprets a leading zero followed by a
  digit as octal. `nord2stopat=0008` is classified as a string, copied into
  the uint32 argument as the first three characters plus NUL, not point 8.
  `0007` and `0005` happened to work. Thus the old stop-8 run is NOT a valid
  after-kperf checkpoint; the successful stop 7 and failed stop 5 still
  bound the failure before BSD. The new test uses **`nord2stopat=8`**.
  The checker now rejects padded decimal stop arguments, and the image
  helper's default is `nord2stopat=2`. The legacy `patch_gate_arg()` in
  `loader.c` also emits padded decimal: do not use automatic `nord2vmstop`
  stepping until that is fixed. It is inactive in the new test image.

- 2026-10-20 (session 8, Codex): **CPU driver matching mismatch found in the
  source; corrected stop 8 now verified above.** This supersedes the final session-7
  explanation below. `ml_wait_max_cpus()` does NOT spin until eight CPUs check
  in: it sleeps on `max_cpus_initialized`, waiting for `ml_set_max_cpus()`.
  `ml_parse_cpu_topology()` honors `cpus=1`; `IODTPlatformExpert::createNubs()`
  only adds `logical-cpu-id` to CPUs selected by that topology. Eight ADT
  nodes therefore do not require eight online CPUs.
  - The real mismatch: Nord 2 CPU nodes had only `arm,cortex-a55/a78` and
    `ARM,v8`, but the only `SweetCPU` personality in `KernelConfigTables.cpp`
    matched `qcom,kryo470`. No Nord 2 CPU could match, and the saved deep-boot
    logs contain no `SWEET CPU:` lines. `SweetCPU::startAll()` normally calls
    `ml_set_max_cpus()` after testing SGI1 on every selected CPU.
  - Fix: add `mediatek,mt6893-cpu` to Nord 2 CPU compatible lists and a separate
    `SweetCPU` personality for that token (patch 0082). Keep the actual Cortex
    types, Sweet's existing Qualcomm match, `cpus=1`, and the normal kperf wait.
    `tools/nord2-mkpatch.sh` now includes `iokit/KernelConfigTables.cpp`.
    Build with Apple clang via CI; do not use fake `__builtin_xnu_*` fallbacks.
  - Next test: CI kernel + new ADT, `nord2stopat=8` (after kperf, before
    `bsd_init`). Expected evidence: `SWEET CPU: requested=1 selected=1`,
    `1/1 CPUs initialized, SGI1 verified`, stop 8, chain-back to Android.
    A failure may expose a further CPU/GIC issue; do not claim a working fix
    until the log proves it. Do not try `cpus=8` as a workaround or skip kperf.
  - Phone checked at session start: Android `sys.boot_completed=1`, rooted,
    full boot partition SHA-256
    `04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01`,
    matching `backup/nord2/boot.img`; pstore empty. This was the state before
    the session-8 test authorized above. Older per-test approvals were
    completed.
  - `gh` is now installed/authenticated for `frxncyyy/ios-sweet`; branch
    `codex/nord2-bringup`, starting commit `168c6b4`. Existing untracked `kmod/`
    belongs to the previous work and has been left untouched.

- 2026-10-19 (session 6, Claude, handoff to Opencode): **diagnostic test of
  `nord2-xnu-late2.img` `4a2614ce…` (authorized, run twice) done; Android did
  not come back.** Photo of the final screen:
  `/home/frxncy/Downloads/materialgram/IMG_20261019_214059_2.jpg`. User: the
  phone resets about 1 s after the last printed line. Analysis of the photo is
  left to Opencode (not done in this session).
  **Phone state at handoff: restored.** `boot` read back as
  `04e84c4a…6e1f01`, `sys.boot_completed=1` (verified after the test).

- 2026-10-19 (session 6, Claude): **test of the io_map console fix (image
  `3721f971…`, authorized): still stops after `SWEET BATTERY`, then reboot
  loop.** pstore empty (`PMIC_cold_reboot`), log
  `backup/nord2/logs/20261019-213101-late2-console-fix`. Restore verified
  (`boot` = `04e84c4a…6e1f01`, `sys.boot_completed=1`). Checked: the Sweet
  Qualcomm drivers (keys/touch/USB via `sweet_ufs.c:660-662`, UFS) are NOT
  started — the UFS root path needs `rd=ufsroot0` and the ADT has no
  `/sweet-ufs`; `SweetWatchdog` never fires; battery is the static fallback.
  They are still needed later and must be ported to MediaTek (MT6359 PMIC,
  FT3518 on I2C0, MediaTek UFS LUN2, SSUSB), not removed. Working hypothesis:
  the kernel stalls once the scheduler starts waiting (timer IRQ not
  delivered?) and the MediaTek hardware watchdog left armed by LK resets the
  phone. New diagnostics (patch 0082, sleh.c and startup.c added to the build
  and to `nord2-mkpatch.sh`): every `kernel_bootstrap_log` /
  `kernel_bootstrap_thread_log` step and every `bsd_init` step is printed on
  the console; `sleh_irq` reports timer ticks (1-3, then every 256th up to
  2048) and the first 8 other INTIDs. Image `nord2-xnu-late2.img` SHA-256
  `4a2614cebea6594c2da932ca97319953e4f5c9f4e6eabed08724691a48712a72`.

- 2026-10-19 (session 6, Claude): **root cause of the silence after
  `SWEET BATTERY` identified in the source (not yet tested on the phone).**
  `pmap_switch_user_ttb()` (osfmk/arm/pmap/pmap.c:7577-7587) loads
  `invalid_ttep` into TTBR0 when it switches to a kernel thread, so the
  bootstrap V=P map that the console used (pmsg 0x48160000, framebuffer
  0x76e10000, DSI/MUTEX MMIO) disappears with the first context switch — that is
  when the log stopped. It also explains why the late stop at the end of
  `arm_init` worked (no switch yet). The bootstrap table itself is NOT reused:
  `bootstrap_pagetables` is read-only-late data and `cpu.c:928-935` cleans it to
  PoC to start secondary CPUs, so it stays a valid V=P table for the whole boot
  (the earlier "XNU reuses the bootstrap tables" note was wrong).
  Fix (patch 0082): `nord2_console_remap()` right after `arm_vm_init()` maps
  pmsg and framebuffer with `ml_io_map_wcomb` and DSI0/DISP_MUTEX with
  `ml_io_map` (TTBR1 aliases, valid in every thread); `nord2_phys()` and the
  frame push use them. `nord2_leave_to()` now masks DAIF first, uses the cached
  chain-back values before any device-tree lookup, and reloads TTBR0 with
  `kvtophys(bootstrap_pagetables)` + TLB flush before the trampoline
  (verified in the linked kernel's disassembly). Expected on screen after the
  test: `console remapped to kernel VA`, then the log continues past the
  battery line to the real failure, or to hook 2 and back to Android.
  - New `tools/nord2-mkpatch.sh`: regenerates 0082 from the buildtree and checks
    it applies to the pristine CI sources and reproduces the buildtree.
  - Images rebuilt (all `FAILURES: none`, 30.568.448 bytes). Test candidate
    `nord2-xnu-late2.img` SHA-256
    `3721f9717986935adb90456f319a592e2768aa0372a58d30c71c51d5c84b3a7e`
    (`nord2stopat=0002`, on-screen log). Previous outputs moved to
    `shim/out/nord2-xnu.prev-*`.

- 2026-10-19 (session 4, Claude): logs after a hang are lost because the reset
  is a PMIC cold reboot (`androidboot.bootreason=PMIC_cold_reboot`; pstore
  empty). `tools/nord2-pull-logs.sh [label]` saves pstore + boot reason +
  dmesg right after each attempt. User chose an on-screen early log instead of
  the gate walk: researched spec in `shim/nord2-xnu/DISPLAY-LOG.md` (DSI0
  0x1400e000, DISP_MUTEX 0x14117000, OVL 0x14000000/0x14001000, framebuffer
  carve-out 0x76e10000+0x1df0000; command-mode panel needs MUTEX_EN + DSI_START
  per frame). Phase 1 = read-only register dump at the cpu_init stop.
  NOT implemented. Opencode's uncommitted loader/build changes (nord2vmloop)
  were left untouched.
- 2026-10-19 (session 5, Claude): **LATE STOP TEST SUCCEEDED (image
  `e3f3613f…`, authorized): the full early bring-up runs and the kernel
  returns to Android with the complete pmsg log**
  (`backup/nord2/logs/20261019-192352-late-arminit/pstore/pmsg-ramoops-0`,
  3621 bytes): loader lines, markers 1..20 including
  `16: calling serial_init`, `19: serial_init returned`,
  `20: kprintf startup complete`, `chain-back cached`, then
  `stop at end of arm_init` → `RETURN TO STOCK ANDROID KERNEL` →
  `entry=0x40280000`; Android reached `sys.boot_completed=1`. The cached
  chain-back parameters work after the page-table switch. Also note DSI
  `INTSTA=0x80000010` (BUSY + FRAME_DONE) while the on-screen console is
  driving frames.
  - Restore verified after the test: `boot` = `04e84c4a…6e1f01`,
    `sys.boot_completed=1`.
  - So: **the whole `arm_init` path (arm_vm_init, TTBR0 switch, physmap,
    serial_init, PE_init_printf) is now proven on MT6893 with a clean return.**
    The next stop to investigate is in the IOKit/driver init after
    `SWEET BATTERY`, i.e. hook point 2 (planned in `SweetBattery.cpp`).
- 2026-10-19 (session 5, Claude): **late stop-with-return added** (built, not
  flashed): `nord2_stop_at(point, label)` (boot argument `nord2stopat=<n>`)
  logs a marker and chains back with the cached parameters; hook 1 sits right
  after `PE_init_printf(FALSE)` at the end of `arm_init()`. New image
  `nord2-xnu-late.img` (cmdline `… nord2stop=0 nord2vm=1 nord2fb=1
  nord2stopat=0001`), 30.566.400 bytes, SHA-256
  `e3f3613f761a4f6e706982f0ea1d792e922004fddfaa4a074ac9bc1083b6c06a`; six of
  six verify; patch dry-run OK. Expected: full early bring-up, then Android
  returns and the pmsg holds the complete log (no photo needed).
- 2026-10-19 (session 5, Claude): **MILESTONE — XNU RUNS PAST `arm_vm_init`
  INTO PLATFORM/IOKIT INIT ON MT6893** (image `59d642c9…`, console fix,
  authorized). The panel showed (verified from the photo) the kernel's own
  output well beyond `cpu_init`:
  `OSLog stream configured`, `Log queues configured`, `Firehose configured`,
  `mig_table_max_displ = 54 mach_kobj_count = 374`,
  `debug_log_init: Error!! gPanicBase is still not initialized` (expected: the
  ADT has no panic-base), `kdp_core zlib memory`, `iBoot version: nord2-probe`,
  `SWEET AMFI: built-in interface registered`,
  `Security policy loaded: Guarded vnode policy`,
  `SWEET: starting device-tree platform expert`, `SWEET: platform expert
  registered`, `SWEET AKS: AppleKeyStore registered`, `SWEET WDT: IOWatchdog
  registered`, `SWEET SURF`, `SWEET HID: buttons/touch registered`,
  `SWEET BATTERY: static fallback, 100% 4200mV external=1 charging=0`.
  **So `arm_vm_init` completes on the Nord 2 and the kernel reaches the Sweet
  platform/IOKit bring-up.** The last visible line is the battery fallback;
  the boot stops (no Android) somewhere after it, in driver init. Both earlier
  "arm_vm_init panics" were the console/boot-args bug, not arm_vm_init.
  - The pmsg of this boot is lost (no return, forced fastboot), but the screen
    log is sufficient: the on-screen console works for real now.
  - Next: add a return/stop point after the platform expert (or a boot-arg
    driven stop) so the milestone boots back into Android with the log in
    pmsg, then bisect the driver init that follows the battery fallback.
  - Restore verified after the test: `boot` = `04e84c4a…6e1f01`,
    `sys.boot_completed=1`.
- 2026-10-19 (session 5, Claude): **ROOT CAUSE FOUND: the phase-2 panic was
  our own console, not arm_vm_init.** Symbolising the panic photo:
  `pc = PE_parse_boot_argn_internal`, `lr = nord2_putc`. `nord2_fb_on()`
  called `PE_parse_boot_argn("nord2fb")` on the **first marker (stage 1)**,
  before `PE_init_platform()` initialised the boot arguments → data abort
  (translation fault, `esr = 0x96000006`) → panic with
  `Synchronous exception taken while/without SP selected` → `/chosen`
  unavailable (ADT not initialised yet) → `cannot chain back`. So nothing in
  `arm_vm_init` has been observed hanging yet; both earlier "arm_vm_init
  panic" tests were hitting this console bug.
  - Fix: `nord2_fb_on()` no longer parses anything (it only reads a cached
    flag). New `nord2_fb_init()` is called at the stage-6 stop (boot args and
    DT valid) from `nord2_display_dump()` and `nord2_maybe_return()`, sets the
    flag from `nord2fb=1` and draws `on-screen log active`.
  - New image `nord2-xnu-vmfb.img`, 30.566.400 bytes, SHA-256
    `59d642c90cfd236a0fae57a580d011da134e6210b0d2cebae78a5bbed5a51ee9`; five of
    five verify; patch regenerated and dry-run OK. Not flashed.
- 2026-10-19 (session 5, Claude): **bottom band widened to 32 lines and the
  chain-back cache now logs its own result**, because the previous 20-line band
  scrolled the `NORD2-XNU` stage markers and the panic header out of view (the
  photo only kept the register tail and the final
  `no /chosen` / `cannot chain back`). It also proved the cache fallback had
  **not** engaged (`using cached chain-back parameters` absent), cause still
  unknown; the new `NORD2-XNU: chain-back cached` / `cache: …` lines will say
  whether the lookup or the properties fail. New image
  `nord2-xnu-vmfb.img` 30.566.400 bytes SHA-256
  `0a498b8e90db2820c197d36ed68e193e458ac5d798c92e55ce7b75fcb0a74bfa`; all five
  verify; patch dry-run OK. Not flashed.
- 2026-10-19 (session 5, Claude): **on-screen log moved to the bottom band**
  (user request: easier to photograph). The console now uses the last
  `20 * 32 = 640` rows of the panel (`NORD2_FB_ORIGIN_Y = 1760`), scrolls one
  cell up when full and always leaves the newest line at the very bottom.
  New image `nord2-xnu-vmfb.img`, 30.566.400 bytes, SHA-256
  `a50ab6cfc05563ceb0195daebb10fe67d5ec0fb2d3515f80ec2986f6cc18f735`
  (payload `d4c4fecb…`); all five images verify; patch regenerated and
  dry-run OK. Not flashed.
- 2026-10-19 (session 5, Claude): **PANIC-CACHE TEST FAILED to return (image
  `905a1431…`, authorized).** The phone stayed in the crash (user: "continua a
  ritornare sul crash e non andare in Android", then forced fastboot). So the
  cached-parameter fallback did **not** get the panic path back to Android.
  The pmsg log of that boot was lost with the reset, so the last screen line
  is unknown. Hypotheses (unverified): by then the CPU/stack state is already
  unusable (XNU's own handler panicked with "without SP selected"), and/or the
  bootstrap V=P table is no longer usable after `pmap_bootstrap()` (it may be
  reused), so both the pmsg write and the trampoline fault again.
  - Restore verified after the test: `boot` = `04e84c4a…6e1f01`,
    `sys.boot_completed=1`.
  - Proposed next step (NOT implemented): a **self-contained fault trap** that
    depends on no kernel page table. Install our own `VBAR_EL1` before
    `arm_vm_init` with a handler that, with the MMU already off, (a) writes
    `ESR_EL1/ELR_EL1/FAR_EL1` to the pmsg ring **at its physical address**,
    (b) reads a {stock entry, dtb} pair that the **loader** would store at a
    fixed physical address (e.g. `0x142008000`), and (c) branches there with
    `x0 = dtb`, `x1..x3 = 0`. That returns to Android from *any* fault,
    including the one at `far = 0x4003c5`.
- 2026-10-19 (session 5, Claude): **PHASE-2 TEST DONE (image
  `f8213c7c…`, authorized). Verified from the panel photo:**
  - **The on-screen console works.** The panel showed the kernel log in white
    on black — phase 2's goal (seeing the log during a boot that never returns
    to Android) is met.
  - The boot ends in an XNU panic, not a silent hang:
    `NORD2-XNU PANIC: Synchronous exception taken without SP selected …`
    followed by a register dump whose values are partly 32-bit truncated
    (`0x7a9258c` is `0xfffffe0007a9258c`, `x15 = 0xffffffff96000006` is the
    ESR): the exception handler could not build a proper frame.
  - The original exception is a **data abort**: `esr = 0x96000006`
    (EC 0x25, DFSC 6 = translation fault level 2) with **`far = 0x4003c5`**,
    a low, unmapped address — the kernel dereferenced a bad pointer.
  - The chain-back then failed with `NORD2-XNU: no /chosen; cannot chain back`,
    because after `arm_vm_init()` switches the kernel page tables the ADT
    (placed just past the kernel image) is no longer reachable **at the VA
    stored in `boot_args.deviceTreeP`**.
  - Fix implemented and built (not flashed): `nord2_cache_chain_back()` reads
    the three `/chosen` properties at the `cpu_init` stop and again just
    before `arm_vm_init`, and `nord2_leave_to()` falls back to that copy
    (`using cached chain-back parameters`) when the ADT lookup fails. A panic
    after the table switch can then still return to Android, so the complete
    panic text lands in pmsg and no photo is needed.
  - Image: `shim/out/nord2-xnu/nord2-xnu-vmfb.img`, **30.566.400 bytes**,
    SHA-256 `905a1431716f490ef329c0aec905288423fc502a39a20be4a88285264a35701f`
    (payload `6574e058…`); all five images verify with
    `tools/nord2-check-xnu.py`; patch regenerated (1094 lines, dry-run OK).
  console for the `arm_vm_init` hang, per `shim/nord2-xnu/DISPLAY-LOG.md`:
  - `start.s` also maps `0x76000000` and `0x78000000` (32 MiB each, Normal
    Non-Cacheable) for the framebuffer carve-out, next to the Device block
    for the display MMIO.
  - `sweet_early.c`: `nord2_fb_*` draws into the phase-1-active buffer
    `0x76e10000` (pitch **4352**, 1080x2400, RGBA 32 bpp, 16x32 cells) and
    pushes one frame per line: bounded wait for `DSI_BUSY`, `MUTEX_EN(0)=1`,
    `DSI_START` 0 then 1, bounded wait for `FRAME_DONE` (bit 4), then W1C it.
    All polls are bounded; nothing can block forever. Enabled only by boot
    argument `nord2fb=1` (absent = off, so `boot`/`vm` behave as before). It is
    fed by `nord2_putc`, so every pmsg line is also drawn on the panel.
  - Image: `shim/out/nord2-xnu/nord2-xnu-vmfb.img`, **30.566.400 bytes**,
    SHA-256 `f8213c7c13d44865e1ddc3d9acd0bb4911c4f789b7fe124e4c9d91a145688c18`
    (payload `9d9c45fd…`, cmdline `… nord2stop=0 nord2vm=1 nord2fb=1`).
    `tools/nord2-check-xnu.py`: all checks pass. `nord2-xnu-boot.img` rebuilt
    too (`c4e3f527…`, FB off).
  - Expected: the phone boots into XNU, draws the log, hangs in
    `arm_vm_init`; the panel keeps the last frame, so a **photo** gives the
    last line reached. pmsg is lost (PMIC cold reboot) by design here.
  - Note: the phase-1 dump's string literals do not appear in the binary
    because clang inlines `sweet_early_string` for constant strings into
    `putc` calls; the dump is present and was verified on hardware (an
    earlier `strings` check was a false alarm).
- 2026-10-19 (session 5, Claude): **PHASE-1 TEST DONE (image
  `595ea593ddc66622a9ee538bf565c9df2e3e634f2adeb36c140fb3f90ba4747a`,
  authorized, boot-only). Android came back in 53 s and the pmsg log
  survived; collected with `tools/nord2-pull-logs.sh display-phase1` into
  `backup/nord2/logs/20261019-151659-display-phase1/`. Verified results:**
  ```
  NORD2-DISP DSI START=00000001 INTSTA=00000004 MODE=00000000
  NORD2-DISP MUTEX 0 EN=00000000 SOF=00000000 MOD0=261f7bf7 MOD2=00000003
  NORD2-DISP MUTEX 1..9 EN=SOF=MOD0=MOD2=0
  NORD2-DISP OVL OVL0    EN=1 ROI=0960021c SRC=00000002
  NORD2-DISP LAY OVL0/1    CON=000021ff SIZE=0960021c PITCH=000010e0 ADDR=77806000
  NORD2-DISP OVL OVL0_2L EN=1 ROI=0960021c SRC=00000001
  NORD2-DISP LAY OVL0_2L/0 CON=000021ff SIZE=0960021c PITCH=00001100 ADDR=76e10000
  NORD2-DISP OVL OVL1    EN=1 ROI=0960021c SRC=00000002
  NORD2-DISP LAY OVL1/1    CON=000021ff SIZE=0960021c PITCH=000010e0 ADDR=77806870
  NORD2-DISP OVL OVL1_2L EN=1 ROI=0960021c SRC=00000001
  NORD2-DISP LAY OVL1_2L/0 CON=000021ff SIZE=0960021c PITCH=00001100 ADDR=76e10870
  ```
  Reading: `ROI/SIZE = 0x0960021c` = **2400 rows x 540 columns**; `CON =
  0x21ff` = format index 2 (32 bpp) with the low bits as in the driver; two
  **full-screen buffers** `0x76e10000` (pitch 4352) and `0x77806000` (pitch
  4320), each split by x into 540-px halves (right half at +0x870 = 2160 B =
  540 px), blended by **two pipes**: pipe 0 = OVL0 + OVL0_2L, pipe 1 = OVL1 +
  OVL1_2L (`SRC` bit0 = 2L layer, bit1 = 1L layer). **All four ADDRs are
  inside the framebuffer carve-out `0x76e10000..0x78c00000`, so LK uses
  physical addresses, not an IOMMU** — the phase-2 plan holds. DSI is idle
  with `FRAME_DONE` (INTSTA bit 4) set and `START=1`. **No mutex is enabled**
  (all `EN=0`) while `MUTEX 0` still holds `MOD0=0x261f7bf7`/`MOD2=3`: LK
  appears to disable `MUTEX_EN` after its last frame (hypothesis; phase 2
  verifies by enabling mutex 0).
  - Consequences for phase 2: draw into `0x76e10000` with **pitch 4352**,
    RGBA 32 bpp, treat it as one 1080x2400 surface (the second pipe reads its
    right half at `+2160`); trigger with `MUTEX_EN(0)=1` + `DSI_START` 0/1 +
    bounded `FRAME_DONE` poll. Map buffer and MMIO Normal-NC / Device
    respectively, with a kernel VA alias after `arm_vm_init` drops TTBR0.
- 2026-10-19 (session 5, Claude): **phase 1 implemented, built, NOT flashed.**
  - The uncommitted `nord2vmloop` gate-walk experiment (loader.c, build.sh,
    `tools/nord2-vmstep-walk.sh`) was **discarded** (`git checkout`, file
    removed): the gate walk is abandoned in favour of the on-screen log, and
    the tree is clean at commit `392da40` for the new work.
  - `start.s`: new 32 MiB **Device-nGnRnE** block `0x14000000..0x141fffff`
    in the existing V=P L2 page (L1 index 0, L2 index 10 — pmsg is 36, the RAM
    window starts at 161), covering DSI0, both OVL pipes and DISP_MUTEX.
  - `sweet_early.c`: `nord2_display_dump()`, read-only, called from
    `arm_init.c` at the stage-6 stop just before `nord2_maybe_return()`. It
    logs `NORD2-DISP …` lines to pmsg: DSI `START/INTSTA/MODE`; MUTEX 0..9
    `EN/SOF/MOD0/MOD2`; OVL0, OVL0_2L, OVL1, OVL1_2L `EN/ROI/SRC`; layers 0..3
    `CON/SIZE/PITCH/ADDR`. It performs **no display writes** and has no
    unbounded loops. `nord2_leave_to()` (entry override) was split out of
    `nord2_return_to_stock()` while doing this.
  - Image: `shim/out/nord2-xnu/nord2-xnu-boot.img`, **30.566.400 bytes**,
    SHA-256 `595ea593ddc66622a9ee538bf565c9df2e3e634f2adeb36c140fb3f90ba4747a`
    (payload `5c6b4c3fae171c8ec9dc705d4e1b235dc5368a2140c9adb44455fa50dfafa7d9`,
    cmdline `… nord2stop=1`). `tools/nord2-check-xnu.py`: all checks pass.
    Patch `xnu/patches/0082-nord2-early-probe.patch` regenerated (871 lines)
    and verified with `patch --dry-run` against the pristine buildtree.
  - Phase-1 acceptance criteria from `DISPLAY-LOG.md`: (1) which mutex is
    active (`EN`/`MOD0` non-zero, `SOF` = DSI0 SOF); (2) which OVL/2L layer is
    enabled and whether its `ADDR` is inside `0x76e10000..0x78c00000` (if not,
    it is an IOMMU address and the phase-2 plan changes); (3) pitch/size/format
    of the active layer (expect 1080x2400, pitch 4352, 32 bpp — unverified for
    LK); (4) single vs dual pipe (are OVL1/1_2L layers enabled?).
  - Next: flash the image boot-only, let Android come back, pull the log with
    `tools/nord2-pull-logs.sh display-phase1`, analyse, restore, then phase 2.

- 2026-10-19 (session 3): **first Nord2 XNU probe image built and statically
  verified; NOT flashed.** Artifact `shim/out/nord2-xnu/nord2-xnu-boot.img`,
  30560256 bytes, SHA-256
  `9c537406ee9a0a48229230ce90090af4f2d890df2d769cda86aad2e5b206edc2`
  (payload `3564f764fd7fe2ac409813b15c55704a7a115a4bf260d7ff7ce45a156d7f69d5`,
  14140432 bytes). `shim/nord2-xnu/build.sh` reproduces that hash end to end.
  Design and layout in `shim/nord2-xnu/README.md`; the XNU side is
  `xnu/patches/0082-nord2-early-probe.patch` (five files, four objects).
  - What it does: `start.s` maps the 32 MiB block at `0x48000000` in the
    existing V=P L2 page (Normal Non-Cacheable) for NORD2 instead of writing
    the Sweet framebuffer band at `0x9c000000`; `sweet_early.c` logs to the
    pmsg ring at `0x48160000`; `arm_init.c` calls `nord2_maybe_return()`
    after stage 6 (`cpu_init`), before `processor_bootstrap()`/`arm_vm_init`;
    `arm_vm_init.c` drops Sweet's `sweet_early_map_final/relocate`.
  - The loader (18705-byte Image at `0x40080000`) validates the live FDT
    (MT6893, `mblock-16-pstore` reservation, `ramoops.pmsg_size=0x10000`,
    MMU/D-cache off), logs, copies the payload segments into
    `0x1_42000000 + 0x3e000000`, places the ADT (9336 bytes) and boot_args
    just after the kernel, patches `/chosen`
    `nord2-stock-entry/dtb/xnu-region`, and enters `__start`.
  - Payload: SWEETXNU v1, 9 segments, virt_base `0xfffffe0006000000`, entry
    `0xfffffe0007a94488` (`__start`), kernel_end `0xfffffe000826c000`;
    symbol table stripped (`strip -S -x`, 14.1 MiB). `start.s` identity-maps
    30 x 32 MiB from `phys_base` (`0x142000000..0x17e000000`), so the
    trampoline page is `0x142004000`.
  - Chain-back: `nord2_leave_xnu(entry, dtb, region_start, region_end,
    region_start+0x4000)` — verified against the verified CPU probe: the
    stock Image's first instruction is the `code0` branch, so entry is
    `0x40280000`; trampoline disables the MMU with the probe-observed
    `SCTLR_EL1=0x30d00800`, zeroes x1..x3, `x0=dtb`. `nord2stop=1` default.
  - Boot image: stock boot v2 header/ramdisk/DT reused byte-for-byte; kernel
    gzip = `[loader 2 MiB][stock Image padded to image_size][payload]`,
    decompressing to 71566352 bytes ending at `0x444c0410`, below the ramdisk
    at `0x51100000`. Static checks in `tools/nord2-check-xnu.py` all pass.
  - Sizes/limits: `tools/nord2-pack-xnu.py` previously demanded
    `len(stock Image) == image_size` and a 16 MiB gzip: both wrong on this
    dump (43896848 vs 55316480; 26.2 MiB gz) and fixed. LK's decompression
    bound is `KERNEL_DECOMPRESS_SIZE`; public MediaTek LK `rules.mk` sets
    `0xc800000` (200 MiB) and the verified probe already decompressed 57.4 MiB,
    so 68 MiB (71.6 MB) is inferred safe, not read out of this device's LK.
  - Known limits of the probe: the kernel console (`PE_putc`) is not routed to
    pmsg, so only stage marks and `sweet_early_panic` text are visible; if XNU
    faults before `debug.c`'s panic path there will be no output past the last
    mark. A "no output / Android back up" result still needs the log pulled.
  - NOT flashed, and no permission has been asked yet. A test needs an
    explicitly authorized **boot-partition-only** flash of the image above,
    then restoration from `backup/nord2/boot.img` with hash and
    `sys.boot_completed` checks.
- 2026-10-19 (session 3, continued): **FIRST FLASH TESTED — it hangs at the
  OnePlus logo and never reaches Android.** Flash of `boot` only was
  authorized, done (`fastboot flash boot`, OKAY) and reverted; the read-back
  boot SHA-256 matched the original backup and the follow-up Android boot
  reached `sys.boot_completed=1`. **The pmsg log was lost**: the phone had to
  be reset by hand into fastboot, and `/sys/fs/pstore` was empty afterwards
  (no `pmsg-ramoops-0`), so it was a cold enough reset to clear the ramoops
  RAM (or the loader never wrote anything). No conclusion can be drawn about
  how far the probe got — the hang is consistent with (a) LK failing to
  decompress the 71.6 MB blob, (b) a loader `fatal()`/fault with the old
  "spin" behaviour, or (c) an XNU fault/hang.
  - Fix built for the next attempt: **every failure path now returns to
    Android** instead of spinning, so the log always survives. Loader:
    log ring initialised *first*; `fatal()` and the exception vectors log and
    then `chain_stock()` (jump to `0x40280000` with `x0 = dtb`); XNU:
    `sweet_early_panic()` logs the panic and calls `nord2_return_to_stock()`
    (guarded against recursion). Three escalation images from one build
    (`shim/nord2-xnu/build.sh`), all statically verified, all with the same
    71.6 MB blob: `nord2-xnu-loaderonly.img` (validate only, no copy, no XNU;
    SHA-256 `97e98f6332598a82da601bab1d4453a6663b2f8cf0a802c9d1950734936f87b6`),
    `nord2-xnu-copy.img` (+ segment copy; `7c56bd30f058719ef6d20d14f298fa2ab4a932513a3d0640d3995c3d65cd3854`),
    `nord2-xnu-boot.img` (full probe; `663a2a18029f74f7190ff2afe65022dac51dc9fb98724094aa23a3f948eb1fa8`).
    Payload `36877c87d08c56b0f67f178667041305b1f83d3af64200a9c9194c136eb3334d`.
  - Next: flash `loaderonly` first. If Android boots with the loader log in
    pmsg, LK decompression and the loader zig are fine and the fault is in the
    copy or XNU; if it still hangs at the logo, the prime suspect is LK's
    `KERNEL_DECOMPRESS_SIZE` (< 71.6 MB) and the blob must shrink to the
    verified 57.4 MB (payload overlapping the stock BSS, dropping the
    `__LINKEDIT` segment and/or putting part of it in the loader's 2 MiB
    reserve). Each image needs its own explicit flash authorization.
- 2026-10-19 (session 3, test 2 — full probe, authorized): **Android booted
  and the pmsg log survived** (`backup/nord2/probe-v3-pmsg.txt`, 249 bytes):
  ```
  NORD2-XNU LOADER: early probe, no storage writes
  dtb=0x0000000047c80000
  NORD2-XNU LOADER: live FDT and cache state ok
  stock_image_size=0x00000000034c1000
  payload=0x0000000043744000
  NORD2-XNU: LOADER FATAL: no SWEETXNU payload after the stock kernel
  ```
  **Verified on hardware:** LK decompresses the 71.6 MB blob (the loader ran,
  so `KERNEL_DECOMPRESS_SIZE` was never the problem); the loader's live-FDT
  checks (mediatek,MT6893 / mblock-16-pstore / ramoops.pmsg_size / SCTLR) all
  pass; the stock Image header is readable at 0x40280000 with
  `image_size = 0x34c1000`; the log ring is readable by Android as
  `/sys/fs/pstore/pmsg-ramoops-0`; and the loader's chain-back works — Android
  reached `sys.boot_completed=1` and boot was restored afterwards.
  - **Root cause of the logo hang (a loader bug, not LK):** `same()` compares
    up to a NUL, but `payload.magic` is 8 bytes *not* NUL-terminated and is
    followed by `version = 1`, so `same(pl->magic, "SWEETXNU")` returned false
    always → `fatal("no SWEETXNU payload")` → the old fatal spun in `wfe`
    (first flash) / chained back (second flash). Fixed with a fixed-width
    `magic8()` comparison. The ADT property-name uses of `same()` are fine
    (32-byte NUL-padded fields).
  - Rebuilt images (all statically verified, same payload
    `36877c87d08c56b0f67f178667041305b1f83d3af64200a9c9194c136eb3334d`):
    `nord2-xnu-boot.img` `5b1ec736638e57844187ec2f951f0ecba9f77ac38b6260a7c69142aa5538f9a0`,
    `nord2-xnu-loaderonly.img` `04d4f0b1b10e0a8072a7a882ae51a4aace12ea0513bfa22e919039f738233764`,
    `nord2-xnu-copy.img` `04cc460c7fbbb09d3d3eb68d2c7aa8736e122887c6050699ac0c13a9c9e430b2`.
  - Next: flash the fixed full probe and read how far XNU gets (the log shows
    the loader stages, then `NORD2-XNU <stage>: ...` markers and either the
    return-to-stock messages or a panic captured by `sweet_early_panic`).
- 2026-10-19 (session 3, test 3 — fixed probe, authorized): **MILESTONE —
  XNU RUNS ON THE NORD 2 UP TO `cpu_init()` AND RETURNS TO ANDROID.**
  Authorized flash of `nord2-xnu-boot.img`
  `5b1ec736638e57844187ec2f951f0ecba9f77ac38b6260a7c69142aa5538f9a0`; Android
  reached `sys.boot_completed=1` ~54 s after `fastboot reboot`. Full pmsg log
  (`backup/nord2/probe-v4-pmsg.txt`, 725 bytes):
  ```
  NORD2-XNU LOADER: early probe, no storage writes
  dtb=0x0000000047c80000
  NORD2-XNU LOADER: live FDT and cache state ok
  stock_image_size=0x00000000034c1000
  payload=0x0000000043744000
  NORD2-XNU LOADER: payload ok
  nsegs=0x0000000000000009
  entry_va=0xfffffe0007a94488
  NORD2-XNU LOADER: copying kernel
  kernel_end=0x000000014426c000
  adt=0x000000014426c000
  boot_args=0x0000000144270000
  entry=0x0000000143a94488
  NORD2-XNU LOADER: JUMP TO XNU
  NORD2-XNU 1: arm_init entered (MMU on)
  NORD2-XNU 2: PE_init_platform done
  NORD2-XNU 3: ml_parse_cpu_topology done
  NORD2-XNU 4: cpu_bootstrap done
  NORD2-XNU 5: kernel_startup_bootstrap done
  NORD2-XNU 6: cpu_init done
  NORD2-XNU: RETURN TO STOCK ANDROID KERNEL
  NORD2-XNU: entry=0000000040280000
  ```
  **Verified:** the ADT is accepted (platform identity + 8 CPU nodes +
  `ml_parse_cpu_topology` on MT6893 affinities), `PE_init_platform` and
  `cpu_init` (MIDR/cache id/coresight path) complete, the 992 MiB window and
  the V=P map work, the pmsg block mapping at `0x48000000` works with the MMU
  on, and the trampoline returns to the stock kernel with the observed
  MMU-off `SCTLR_EL1` state (Android boots). This is XNU kernel code running
  on MediaTek silicon — not an iOS boot: no display, touch, UFS, GIC or timer
  take-over, and nothing past `arm_vm_init`.
  - Next milestone options: (1) route the kernel console to pmsg so the
    kernel's own `dprintf`/kprintf output becomes visible; (2) push the stop
    point from stage 6 past `processor_bootstrap()` and `arm_vm_init`
    (GIC/arch-timer/PMAP bring-up, the real porting work).
  - **Boot partition restored** (same session, after a manual fastboot
    entry): `fastboot flash boot backup/nord2/boot.img`, read-back SHA-256
    `04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01`
    (matches the backup) and `sys.boot_completed=1`. Phone is back on the
    untouched stock boot image.
- 2026-10-19 (session 3, console work — built, NOT flashed): **kernel console
  now routed to pmsg.** `osfmk/console/serial_console.c` `_cnputs()` (which
  `console_write()` uses directly before `console_init()`, and for printf,
  kprintf, IOLog and panic output) now calls `sweet_early_console_write()` on
  NORD2 instead of the Sweet UFS log / `_cnputc`, so the early kernel's own
  text lands in the same `pmsg` ring as the stage markers. The Sweet path is
  untouched for non-NORD2 builds. New objects: five rebuilt
  (`serial_console.c` added), relinked and stripped; entry moved to
  `0xfffffe0007a93488` (`__start`, now checked dynamically by
  `tools/nord2-check-xnu.py` in segment 2 instead of a hardcoded VA).
  - Flashed and read back (authorized): log identical to probe-v4 plus the
    new `entry` value — **no kernel `printf` output exists before `cpu_init`**,
    so the hook is armed for the next stage (`backup/nord2/probe-v5-pmsg.txt`).
- 2026-10-19 (session 3, escalation built, NOT flashed): **two more stop
  points.** `nord2_maybe_return()` (stage 6) already continues when
  `nord2stop=0`; a new `nord2_before_vm_init()` hook was added in
  `arm_init.c` between `processor_bootstrap()` and `arm_vm_init()` and stops
  (chaining back to Android, console still live) unless the boot argument
  `nord2vm=1` is given. `shim/nord2-xnu/build.sh` now also builds the payload
  variants and their images:
  - `nord2-xnu-boot.img` `5324d9d8595bc8497ccdaa160c4bf79f4303a56431d62857beec6a108deee0a8`
    (`nord2stop=1`, stop after `cpu_init`; payload `5f8942ea…`);
  - `nord2-xnu-continue.img` `bbe528024cfc22e10cae91c98ffebf1568528a453828fa0c2856b34c15218a2b`
    (`nord2stop=0`, run `processor_bootstrap()` then stop before
    `arm_vm_init`; payload `d7b9c56a…`);
  - `nord2-xnu-vm.img` `5b967b4beda0517993231e9beb1cbcc3adade0da4404981ccc7f85afb956b66a`
    (`nord2stop=0 nord2vm=1`, enter `arm_vm_init`; payload `4b7f1ba2…`).
  All three statically verified. Any panic in the entered code is logged and
  chains back to Android (`sweet_early_panic`).
  - **Test 4 (`nord2-xnu-vm.img`, authorized): Android booted and the panic+`
    chain-back chain worked end to end** — XNU's `sleh` handler caught the
    fault, `sweet_early_panic` dumped the CPU state to pmsg and
    `nord2_return_to_stock()` brought Android back
    (`backup/nord2/probe-v6-vm-pmsg.txt`, 1839 bytes). The panic was a
    **self-inflicted `brk #1` in `nord2_maybe_return`**: the two stop hooks
    were declared `__attribute__((noreturn))` but return when
    `nord2stop=0`/`nord2vm=1`, so the compiler put an unreachable `brk #1`
    at the end of the returning path (pc `0xfffffe0007a92f44`, ESR
    `0xf2000001` = BRK, ISS comment 1). The `nord2stop=0` path itself had
    worked (it printed "continuing past early stop"), and
    `PE_parse_boot_argn` read the cmdline correctly. Fixed by dropping the
    `noreturn` attribute from both hooks; the rebuilt kernel has no `brk #1`
    in that region.
  - Rebuilt images (30.562.304 bytes each, `FAILURES: none`):
    `nord2-xnu-vm.img` `69ababae0a8ea795677feb9b2c4e2b0fb98f07e38db28b64a50ca180e640cfd9`
    (payload `b08fa0eb…`),
    `nord2-xnu-continue.img` `6d19735ef61c2ba6cc343216ac48fc47212235fa62aa85fce898d4314c9c68f2`,
    `nord2-xnu-boot.img` `77f1ba34164238cb42b7d1132ff100b82c71534edb0f6c545ce80581fc32debe`.
  - Next: flash `nord2-xnu-vm.img` again. It should now enter `arm_vm_init`
    and either print its progress (console hook) or panic (logged, chained
    back). Expect the first real memory-map/GIC work to fail: that is the
    next porting task.
- 2026-10-19 (session 3, test 5 — `nord2-xnu-vm.img` fixed, authorized):
  **`arm_vm_init` HANGS** (no panic): Android never came back, the user saw a
  bootloop and had to force fastboot. Two lessons, both verified:
  - The loader now *preserves* an existing ring instead of resetting it
    (`previous log preserved` line), and **DRAM does survive a normal
    reboot** — after `adb reboot` the file held the earlier Android pmsg data
    plus both probe runs (`backup/nord2/probe-v8-reboot-pmsg.txt`, 2158
    bytes). But **DRAM does not survive the reset that follows a hang**
    (cold/WDT): at the next boot the loader found no previous ring, so the
    hung run's log is unrecoverable. Any future step that can hang must
    therefore be *bounded by a stop hook that returns to Android*, never run
    to a hang.
  - Note: `/sys/fs/pstore/pmsg-ramoops-0` is a *shared* zone — Android writes
    its own "TENS" records into it through `/dev/pmsg0`, so the file contains
    Android records followed by our appended probe log.
  - Next plan (agreed shape): instrument `arm_vm_init` with a sequence of
    numbered gates (`nord2_vm_gate(stage)`, controlled by a boot argument)
    that each log `NORD2-NEXT=<stage+1>` and chain back to Android; the
    loader reads that value out of the preserved previous log and rewrites
    the `nord2vmstop=` field of the payload command line in RAM before XNU
    starts. One flash, then each plain reboot advances the gate by one step
    until a boot hangs: the last `NORD2-NEXT` seen is the failing step.
    Rebuild images (30.562.304 bytes, all verified):
    `nord2-xnu-boot.img` `b23c76acb1265822275629ac035affd120e74a8e845dd1a28337a021127e046b`
    (loader preserves the ring), `nord2-xnu-continue.img` `69a1dd03…`,
    `nord2-xnu-vm.img` `292f8a5c50785f88dbb9288c0db3606208f402da7d12dc7d2817c678b13fce79`.
  - **Gates implemented (2026-10-19, built, NOT flashed):** ten
    `nord2_vm_gate()` calls inside `arm_vm_init` (entry, before the bootstrap
    table setup, before the page-table allocation, after the segment lookup,
    before `arm_set_user_tbi`, after the TTBR0-off/physmap rebind, before and
    after `pmap_bootstrap`, after the early I/O tables, and at the end). A gate
    stops only when `stage >= nord2vmstop` and logs
    `NORD2-XNU: gate 0x… stop; NORD2-NEXT=0x…`. The loader scans the
    *preserved* ring for the last `NORD2-NEXT=0x…` and rewrites the four
    digits after `nord2vmstop=` in the payload command line (in RAM, before
    `boot_args` are filled) — so one image walks one gate per reboot. Images:
    `nord2-xnu-vmstep.img`
    `4e943109a6824ff19d4bd1b9f0842318bea4ff24c897080d0c126983fe6ef3b8`
    (payload `41f93f50…`, cmdline `… nord2stop=0 nord2vm=1 nord2vmstop=0001`),
    plus refreshed `-boot` `07a2755d…`, `-continue` `5334b698…`, `-vm`
    `40d8f90b…` (all 30.564.352 bytes, `FAILURES: none`).
    Caveat: the ring is shared with Android, whose own pmsg records grow it;
    the "last tag wins" rule assumes the ring does not wrap (64 KiB against a
    few KiB of logs).
  - **Phone state:** `boot` was restored to
    `backup/nord2/boot.img` after the hang (`read-back` SHA-256
    `04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01`,
    `sys.boot_completed=1`). Nothing of the probe is left on the device.
  - **Phone state:** `boot` currently holds the console probe
    (`217c377d…`, verified boot: Android starts normally through the
    chain-back). It must be restored from `backup/nord2/boot.img` at the end
    of the session.
- 2026-10-19 (session 2): `tools/nord2-xnu-cc.py BUILDTREE COMPONENT SRC OUT.o`
  rebuilds the CI compile flags from makedefs (PLATFORM=MacOSX:
  ARM_LARGE_MEMORY=1, link 0xfffffe0007004000, -march=armv8.2-a+crypto,
  DEFINES + CONFIG_DEFINES, osfmk meta_features.h, INCFLAGS). Checked against
  the CI archive objects (`research/nord2/orig-objs`): unmodified start.s gives
  an identical start.o except alignment fill (zeros instead of NOPs, never
  executed; load commands and relocations identical); sweet_early.c gives the
  same symbols and data/bss/cstring sizes (code differs: clang 23 vs Apple
  clang). Support libs (firehose, profiling) come from separate Apple projects
  and were not re-audited. No Nord2 XNU patch, ADT or image exists yet.
- Proton Clang 13.0.0 in `../toolchains/proton-clang`, GNU bc in
  `../toolchains/bc`; `../nord2-current-kernel` now a full checkout (+ KSU-Next
  submodule 3c352ee); `olddefconfig` from device config.gz reproduces it
  exactly (out dir `../nord2-kout`). Local README line added there only to get
  the `-dirty` release string.

This checkout now targets a connected OnePlus Nord 2 / MediaTek MT6893.
All Sweet phone state and approvals below this section belong to upstream,
not to this user. **iOS has NOT booted on the Nord 2.** Android and userdata
are preserved; no permission to erase userdata has been given.

- Branch `codex/nord2-bringup`, base `fdad0de8dc0e75c5ea84762adc03f7232dc91f19`.
- Private backups: `backup/nord2/manifest.json` and 19 partition dumps, each
  originally checked against the device. Original boot SHA-256:
  `04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01`.
- Both boot-only diagnostic tests and their restoration were explicitly
  authorized. Test 1 had no surviving log after a cold reset. Test 2
  (`shim/nord2/build.sh --chain-stock`) succeeded: 454-byte pmsg log,
  all diagnostics COMPLETE, then Android boot completed. Logs saved as
  `backup/nord2/probe-v2-*`. **Original boot restored and read-back SHA-256
  matched**, and the subsequent Android boot reached `sys.boot_completed=1`.
- Tested chain image SHA-256:
  `43108d50e95688c3275f6b16093f2d13c08f438396adfaf6ccece3a575b64318`.
  `/proc/iomem` confirms original Android kernel relocated by 2 MiB.
  See `shim/nord2/README.md` for CPU register results and build procedure.
- `fastboot boot` fails even on the exact original image. `adb reboot
  bootloader` returns to Android despite kernel requesting bootloader.
  User manually enters fastboot. Do not reuse Sweet flashing commands.
- Phone is unlocked, rooted (KernelSU). Android properties spoof OPPO;
  hardware/live FDT confirm MT6893 and OnePlus Nord 2. ADB/fastboot are in
  `/home/frxncy/Android/Sdk/platform-tools`. Quote a single combined remote
  command as `su -c <shell-quoted-command>` so all commands run as root.
- Live DT: `dtb/nord2/live.dtb`, `.dts`; original boot unpacked under
  `dtb/nord2/boot-unpacked`. Boot v2, kernel 0x40080000, ramdisk 0x51100000,
  FDT 0x47c80000. DTB payload is an Android DT table. Compressed Android
  kernel has appended DTBs; do not drop them.
- RAM 0x40000000 + 8 GiB with carveouts. GICD 0x0c000000,
  GICR 0x0c040000; virtual timer IRQ 27, physical IRQ 30, 13 MHz.
  PSCI and SMCCC 1.1. CPU affinity 0,0x100,...,0x700.
  UFS 0x11270000, IRQ137, partitioned disk is LUN2 (Sweet assumes LUN0).
  Screen Samsung AMS643YE05, 1080x2400; Android stride4352. Live packed
  atag,videolfb gives 0x76e10000 + 0x1df0000. Touch FT3518 at I2C0/0x38.
  None of Sweet's Qualcomm display/USB/PMIC/touch addresses are safe here.
- Ramoops reservation 0x48090000+0xe0000; pmsg last64KiB at0x48160000,
  header sig/start/size (12 bytes), ECC0. Chainload avoids the cold reset
  that previously lost the log.
- IPSW in Downloads: iPhone10,3,iPhone10,6_16.7.16_20H392_Restore.ipsw.
  Identity and DeviceTree only extracted so far under research/nord2/ipsw.
- CI artifacts from upstream run39158169212 downloaded to
  `research/ci-39158169212`: patched XNU source, generated headers,
  complete object archive and linked kernel. Latest patch0081 included.
  No gh/auth here; Linux host root unavailable. Do not upload artifacts.
- cctools-port built at `../toolchains/cctools`; ld64.lld cannot link this
  kernel's static segment layout. cctools ld can, but needs support libs.
  Official libdispatch1415.140.2 compiled successfully for kernel arm64:
  `research/nord2/firehose.o`, command in firehose-command.json.
  LLVM16 profiling support and a Darwin chkstk implementation were subsequently
  built; link now succeeds (details below). Downloaded Windows chkstk.S is
  NOT appropriate and was not used.
- User supplied current Android source: `https://github.com/frxncyyy/android_kernel_oplus_mt6893/tree/KSUN-RUI-susfs-v2.2.0-manual-hook`, cloned at `../nord2-current-kernel`; HEAD `04d00c2565764c3922c9b4826b5acff1106e533f` matches the running kernel build hash.
- Local XNU relink now succeeds with cctools, firehose and LLVM16 profiling support plus a Darwin stack probe. Initial output `research/nord2/kernel.relink` is not yet a Nord2 kernel and has NOT been booted.
- User requested a harness handoff because usage is running out; no further
  phone test is pending. Final read-only check again confirmed Android
  `sys.boot_completed=1` and original full-partition boot SHA-256.
- Reproduction commands: `research/nord2/firehose-command.json`,
  `profile-command.json`, `link-command.json`. The latter is a complete argv
  list executable with Python subprocess from the repo root. Compiler-rt
  files live in `research/nord2/compiler-rt`; libdispatch in research/libdispatch.
  `chkstk.S`/`.o` in research/nord2 implement the observed Darwin x9/x10 ABI;
  evidence in chkstk-evidence.txt from cctools nm + LLVM C API. REA provider
  unavailable (no Ghidra/Hopper/IDA); ordinary shell evidence, not REA output.
- Relink SHA-256 `5913361f2f39f492bf1154779b0512fe4252ebaed80cf320240102224b79f914`,
  21979536 bytes, no undefined symbols, no chained-fixups command. This is
  still the Sweet platform kernel and is unsafe to boot on Nord2.
- Source compile smoke checks only: sweet_early.c compiled; start.s failed
  because VM_KERNEL_LINK_ADDRESS was not supplied. Commands saved as
  start-command.json / sweet_early-command.json. Both source files remain
  UNMODIFIED. Reconcile all compile flags with MakeInc.def before using
  output: CFLAGS_ARM64 include ARM_LARGE_MEMORY=1, PAGE_SIZE_FIXED,
  VM_KERNEL_LINK_ADDRESS=0xfffffe0007004000, arm64/ARM64/__ARM64__/LP64,
  __API__=v4 and fno-strict-aliasing. Missing flags can affect ABI; successful
  linking alone does not validate the support libraries or kernel layout.
- Next (from session 3): (1) XNU now stops cleanly after `cpu_init` and
  returns to Android (log `backup/nord2/probe-v4-pmsg.txt`); the next
  milestones are a real early console (`PE_putc` → pmsg) and then
  `processor_bootstrap()`/`arm_vm_init` with GIC and arch-timer bring-up;
  (2) the boot partition is restored and verified (SHA-256 `04e84c4a…6e1f01`,
  `sys.boot_completed=1`). Display, touch and UFS takeover still need
  implementation and validation.
  A working CPU probe is not a working iOS port. Display, touch and UFS
  takeover still need implementation and validation.


## Agent handoff / active work (2026-10-15, latest)

### >>> UPDATE 2026-10-18 (read first, then the 10-17 handoff below) <<<

**STATE at end of 2026-10-18 (evening):**
- On the phone: userdata = **image v15** (`/run/media/dipa/android/ios-sweet-ipsw/ios16-root-20H392-v15.img/.simg`; = v14 + `/private/var/preferences/SystemConfiguration/preferences.plist` with one network service "USB Ethernet": Interface DeviceName ue0 Type Ethernet, IPv4 Manual 192.168.7.2/24 router 192.168.7.1, DNS 1.1.1.1 8.8.8.8, set "Automatic" as CurrentSet). Fallback image v14 (v12/v13 deleted).
- Kernels: **0078** = all 8 cores (`cpus=8`, cpu6/7 = A76, homogeneous SMP; mkadt now emits 8 CPU nodes; ADT `shim/out/sweet-0078hi.adt`); **0079** = display off/on via IOMFB selector 12 kern_RequestPowerChange(state) (fade over 6 vsyncs, then black; verified: power button gives "SWEET FB: display off/on"); **0080** = `SWEET NET: <proc> route add/delete/change ...` and `... add/delete address ...` trace (built, `shim/out/shim-boot-0080s.img`, NOT booted yet).
- **Internet over USB: not working yet.** Host side is ready (temporary iptables: FORWARD accept for 192.168.7.0/24 + MASQUERADE, comment "ios-sweet"; lost on host reboot). With v15 the phone sent no DNS and no outbound traffic (only mDNS/ARP) -> configd probably did not apply the service (or did, without default route/DNS). Next: boot 0080s on v15, watch `SWEET NET` lines (does configd add 192.168.7.2 / default route on ue0?). If configd ignores ue0 (not an IOKit IONetworkInterface; configd 1241.140.6 source in `research/configd-1241`, SCNetworkInterface createInterface needs IONetworkInterface -> controller (IOMACAddress, SelfNamed) -> bus), add IOKit stand-ins and/or rename the ifnet to en*.
- Live log listener (`nc -ukl 5140`) may still be running, writing `logs/live-0078s.txt`.

**LIVE LOG over USB (0077, boot 0077s): no more recovery round-trips.** Boot an image with `sweetusb=2`, run `tools/sweet-usbnet.sh logs/live-NAME.txt` on the PC: it waits for the phone's CDC-ECM interface, sets 192.168.7.1/24 (sudo) and prints/saves the kernel console the phone streams every 100 ms as UDP to 192.168.7.1:5140 (starts with the last 64 KiB). `ping 192.168.7.2` checks the link. Current image: `shim/out/shim-boot-0077s.img` (kernel 0077, ADT sweet-0069hi, cpus=6, sweetfb_noconsole=1, sweetfb_hz=30, sweetusb=2). The recovery pull (`tools/sweet-log-pull.sh`) is still the way after a panic/hang before USB comes up.

**🎉 USB networking works (boot 0076s, 18:24 local): `ping 192.168.7.2` from the PC answers in ~1 ms, 0 % loss (200-packet burst, 1500-byte frames, 512/1024-byte frames).** 0075: CDC-ECM function (bulk 0x81/0x01, interrupt 0x82 NETWORK_CONNECTION; host MAC 02:00:5e:77:ee:01) + kernel ethernet ifnet `ue0` (legacy output, header strip like if_fake) with 192.168.7.2/24 set from the kernel (proto_plumb + SIOCAIFADDR on an in-kernel socket). 0076: received frames into one cluster mbuf (mbuf_allocpacket gave a first mbuf of length 0 -> panic "Failed mbuf validity check: len -14"). Host side (Linux cdc_ether, interface `enp0s20f0u3u1`): `nmcli dev set IF managed no; sudo ip addr add 192.168.7.1/24 dev IF; sudo ip link set IF up`. Boot image `shim/out/shim-boot-0076s.img` (`sweetusb=2`). iOS itself does not see ue0 yet (configd needs an IONetworkInterface in IOKit).

**USB device mode works (boot 0074s, 17:45 local): the host enumerates the phone at high speed as `1209:0001 ios-sweet "Sweet iOS (USB bring-up)"`.** 0072 read-only probe: ABL leaves the USB30_PRIM GDSC and all 7 USB clocks on, DWC3 v3.20a (GSNPSID 0x5533320a) in device mode (GCTL 0x00112000), halted (DCTL RUN_STOP 0, DSTS 0x00d3de04), event buffer still at ABL's 0xfdfc8000, QUSB2 PHY alive. 0073 (`sweetusb=2`): DCTL soft reset, own event buffer (polled, interrupt masked), ep0 DEPSTARTCFG/SETEPCONFIG/SETTRANSFRESOURCE, setup TRB, RUN_STOP; control-transfer state machine as Linux ep0.c; descriptors: vendor class, no data endpoints. It ran but no event came: **0074** forces the VBUS session in the Qualcomm wrapper (QSCRATCH 0xa6f8800: HS_PHY_CTRL |= UTMI_OTG_VBUS_VALID(20)|SW_SESSVLD_SEL(28), SS_PHY_CTRL |= LANE0_PWR_PRESENT(24), as Linux dwc3_qcom_vbus_override_enable) -> connect + enumeration. Image `shim/out/shim-boot-0074s.img`. Black Phone app: main thread waits in -[VMVoicemailManager fetchAccounts] (sync XPC; voicemail needs the modem).

**Navigation works (boot 0071s on v14, log `logs/boot-0071s.txt`):** swipe up from the bottom edge returns to the home screen; app switcher works. Two fixes: **0070** pid_hibernate (syscall 435 was nosys on the macOS config -> SIGSYS killed SpringBoard in -[SBToAppsWorkspaceTransaction _didComplete] when an app left the foreground; the only CONFIG_EMBEDDED-only entry in syscalls.master); **0071** touches starting within 48 px of an edge carry kIOHIDDigitizerEventFromEdgeTip (UIKit `_UIEventHIDTouchEdgeTypeForHIDEvent` reads the digitizer EventMask 0xb0007 bits 0x800/0x1000/0x2000/0x40000). Diagnosed, not fixed: Settings (Preferences) SIGTRAP in PodcastsUI func_1cdc47c58: force-unwrap of `URL.volumeTotalCapacity` (nil on our HFS+ root). Phone/Photos/Messages show black (MobilePhone launches, no crash) - next: stack samples via boot-arg `sweetsample=MobilePhone,MobileSlideShow,MobileSMS,SpringBoard` (image `shim/out/shim-boot-0071s.img`).

**🎉🎉 MILESTONE 2026-10-18 ~15:30 local: iOS 16.7 SPRINGBOARD HOME SCREEN ON THE PHONE** (boot `shim/out/shim-boot-0069s.img` = kernel 0069 + ADT `shim/out/sweet-0069hi.adt`, image **v14**; log `logs/boot-0069s-v14-HOMESCREEN.txt`). Home screen with a few apps, Settings opens, Control Center / lock screen / notification center work, touch + buttons work, 6 A55 cores (`cpus=6`), `sweetfb_noconsole=1 sweetfb_hz=30`. Known issue: cannot leave Settings (home gesture / AssistiveTouch home do nothing) - to investigate.
What it took today (in order): 0058 HID services (Apple IOHIDFamily-1915 user client), 0059 iPhone X identity (hw.product etc.), 0060 SPMI keys + GENI touch, 0061 1125x2436 presented display, 0062 RPMh L11C touch power + IOPMPowerSource battery, 0063 GENI FIFO packing (Goodix GT9896), 0064 PRIV_WORK_INTERVAL grant (SpringBoard "Failed to create work interval"), 0065 SMP via PSCI (6 A55), 0066 crash map regions, 0067 console mute without cons_ops write, 0068 IOSurface lookup of an already-mapped surface returns base -1 (backboardd snapshot crash), 0069 UFS log flush 10 s. Image v13: CommCenter removed from launchd.plist, BKNoWatchdogs. **Image v14** = v13 + Setup skipped (`/var/mobile/Library/Preferences/com.apple.purplebuddy.plist` SetupDone/SetupFinishedAllSteps true, SetupVersion 11, daemonizeNeedsToRun false; purplebuddy.notbackedup mini-buddy flags false; managedconfiguration.notbackedup LockdownActivationIndicatesCloudConfigurationAvailable false; springboard MCForceShowCloudConfigurationUI false) + mobileactivationd data ark seeded (`/private/var/containers/Data/System/D90C94C4-F94E-4FCF-8A28-A93BE3846D8E/Library/internal/data_ark.plist`, -ActivationState Activated etc.) + MediaToolbox D22/D221/Default SubServerConfiguration StartRecorderServer=false (mediaserverd crash loop: portrait-filter prewarm called NULL GL). **Activation:** ADT /chosen certificate-production-status 0, effective-production-status-ap 0, certificate-security-mode 1, effective-security-mode-sep 1 -> mobileactivationd's native Hactivation branch (Codex `codex/activation/report.md`; alternative 4-byte daemon patch documented there). Fallback image v13; v12 deleted.

**🎉 TOUCH WORKS (boot 0063u on image v13, 2026-10-18 ~13:15 local):** tapping (i) on Setup's Hello opens the device-info sheet; swipe-up is recognised (spinner, then back to Hello - under investigation). Chain: RPMh L11C vote (0062, `bsd/dev/sweet_rpmh.c`) -> GT9896 @0x5d on GENI SE7 (0063 fixed FIFO packing: register address went out byte-swapped) -> sweet_hid_touch -> SweetHIDTouch -> backboardd. Battery icon OK (0062 SweetBattery IOPMPowerSource, static 100 %). **Image v13** (= v12 + CommCenter jobs removed from launchd.plist + `BKNoWatchdogs`=true and `FBLaunchWatchdogExceptions{com.apple.purplebuddy:100}` in /var/mobile/Library/Preferences/com.apple.springboard.plist): Setup no longer killed by the scene-create watchdog (it blocked in CoreTelephony getSubscriptionInfo -> CommCenter, Codex `codex/setup/report.md`; that report also lists how to skip Setup: SetupDone/SetupFinishedAllSteps/SetupVersion=11 in com.apple.purplebuddy.plist + activation check). v9 deleted, fallback = v12. Input latency ~1 min with 1 CPU -> Codex task 11 = SMP via PSCI (`codex/smp/`).

**INPUT WORKS (boot 0060u on a freshly flashed v12, log `logs/boot-0060u.txt`):** power, volume up, volume down reach iOS (PMIC -> sweet_keys.c -> SweetHIDButtons -> backboardd). Power locks/unlocks (screen "off" shows the kernel console because the panel never really turns off). **Device identity fixed: iPhone X** (home indicator, Face-ID-style Setup). Earlier test (0059u, `sweethidtest=15` fake Home presses) advanced Setup from Hello to the language list.
- **0058** HID: `iokit/Kernel/arm/hid/`: IOHIDEventServiceUserClient + IOHIDEventServiceQueue + IOHIDEvent compiled UNCHANGED from Apple IOHIDFamily-1915.140.3 (= exact version of the iOS 16.7 IOHIDLib, clone in `research/iohid-1915`) over our minimal IOHIDEventService (SweetHID.cpp: SweetHIDButtons consumer page 0x0C power 0x30 / vol 0xE9/0xEA / menu 0x40; SweetHIDTouch digitizer collections). C hooks `sweet_hid_key(key, down)` (0 power, 1 vol+, 2 vol-, 3 home) and `sweet_hid_touch(finger, down, x, y)`. HID logs go to IOLog. Boot-arg `sweethidtest=N` = fake Home press every N s.
- **0059** identity: hw.machine/hw.model/hw.target/hw.product from the ADT (macOS-config kernel used to answer "iPad8,6"/"Redmi Note 10 Pro"). mkadt: root compatible `D22AP|iPhone10,3|xiaomi,sweet|...`, model iPhone10,3, target-type D22, `/product` copied from the real D22AP device tree (`research/ipsw-x/d22ap.adt`, extracted from the IPSW, NOT in git), `/buttons home-button-type 2`. Codex report `codex/mg/report.md`: MG ProductType = sysctl **hw.product** (unrecognized -> iPod), HWModelStr = hw.target, HomeButtonType = `/buttons/home-button-type`; disk cache `/private/var/containers/Shared/SystemGroup/systemgroup.com.apple.mobilegestaltcache/Library/Caches/com.apple.MobileGestalt.plist` (keyed by build + UUID only -> stale iPod answers survived; reflashed clean v12 to clear it).
- **0060** drivers by Codex (`codex/input/hw-report.md`): `bsd/dev/sweet_keys.c` (SPMI arbiter v5 observer reads, PON 0x810 bits KPDPWR/RESIN SID0, PM6150L GPIO2 SID4 0xc110; works), `bsd/dev/sweet_touch.c` (GENI SE7 I2C @0xa84000, starts 45 s after boot). Touch result: SE7 fw ok (proto 3) but **0x5d and 0x38 both time out -> touch IC unpowered** (L11C LDO needs an RPMh vote). Codex task 8 = RPMh client (`codex/rpmh/`), task 7 = battery (`codex/battery/`).
- **0061** (CI 39117078444): red status bar "rdar:45025538" = UIKit "No status bar visual provider for screen" (360x800 pt is no real iPhone). AppleCLCD now presents **1125x2436** (iPhone X) and shows the centered 1080x2400; touch mapped the same; ADT canvas 1125x2436. `sweetfb_native=1` reverts.
- Images: `shim/out/shim-boot-0060b.img` (0060 + /buttons ADT `sweet-0060hi.adt`). Next: 0061 image with ADT from current mkadt.

### >>> HANDOFF 2026-10-17 late (read this first) <<<

**STATE: iOS 16.7 Setup.app "Hello" screen is ON THE PHONE** (boot `shim/out/shim-boot-0057u.img` on image v12). Photo `/run/media/dipa/android/ios-sweet-work/MILESTONE-setup-hello-2026-10-17.jpg`. SpringBoard + backboardd + Setup run; nothing crashes in the GUI path. Remaining crash/relaunch loops: installcoordinationd (should be fixed by 0057, unverified), mediaserverd (VirtualAudio removed in v12, unverified), mobileactivationd (~100/boot, not analysed), bluetoothd + fairplayd.H2 relaunch constantly (no hardware).

**What is on the phone now:** userdata = image **v12** (`/run/media/dipa/android/ios-sweet-ipsw/ios16-root-20H392-v12.img` + `.simg` + `.sha256`; v9 kept as fallback, v8/v10/v11 deleted or superseded). Kernel **0057** (`xnu/out-0057`, CI `39067532732`). ADT **`shim/out/sweet-0057hi.adt`** (QEMU twin `research/q0057-hi.adt`). Boot image **`shim/out/shim-boot-0057u.img`**. Boot args: `debug=0xc serial=3 cpus=1 keepsyms=1 -enable_kprintf_spam -noprogress rd=ufsroot0 unentitled_ios_sim_launch=1 ignition_level=1 wdt_test=no_user_checkin ufsrw=1 rootrw=1 launchd_unsecure_cache=1 wdt=-1`.

**Build a phone image** (kernel K, ADT A): `python3 tools/mkpayload.py $K $A shim/out/payload-X.bin --cmdline "$B" && (cd shim && ./build.sh ../shim/out/payload-X.bin && cp out/shim-boot.img out/shim-boot-X.img)` then check `python3 tools/unpack_bootv2.py shim/out/shim-boot-X.img shim/out/check-X && cmp shim/out/check-X/ramdisk shim/out/payload-X.bin`. QEMU smoke test: `python3 tools/mkpayload.py $K research/q0057-hi.adt $S/csq/payloadX.bin --ramdisk $S/csq/root-fork.img --cmdline "debug=0xc serial=3 cpus=1 keepsyms=1 -enable_kprintf_spam -noprogress rd=md0"` + `PATH=~/swift/usr/bin:$PATH python3 tools/qemu-sweet.py ... --secs 40 --kernel $K --fail-on-panic` (S = scratchpad; root-fork.img lives in the session scratchpad csq/ dir and may be gone in a new session).

**Kernel workflow:** edit `research/bt28/xnu/...` (keep the pre-edit copy in `research/preNNNN/`), syntax check `python3 tools/xnu-cc.py research/run28.log research/bt28 <file>`, make `xnu/patches/NNNN-*.patch` with `diff -u pre bt28 | sed` path rewrite, add new/changed files to `research/check-patch-order.py` list, run it (must say applied + MATCH), commit + push -> GitHub Actions `xnu` builds (~8 min) -> `gh run download <id> -R dipa-studio/ios-sweet -n xnu-kernel -D xnu/out-NNNN`. **Repo is PUBLIC since 2026-10-17** (private macOS runner minutes ran out: "recent account payments have failed"); public = free runners. Commit trailer: only `Co-Authored-By: dipa-studio <183933817+dipa-studio@users.noreply.github.com>`.

**Phone workflow (user does buttons, Claude runs commands):**
- Boot test kernel: phone in fastboot -> `fastboot boot shim/out/shim-boot-X.img`. NEVER `fastboot getvar`.
- **Read logs WITHOUT filming:** after a test the user holds Power+VolDown -> fastboot -> `fastboot boot recovery/recovery.img` (OrangeFox, temp) -> wait for `adb shell true` -> `tools/sweet-log-pull.sh /run/media/dipa/android/ios-sweet-work/logs/<name>.txt` (reads the 4 MiB console ring the kernel keeps in the last 1025 blocks of userdata; patch 0047). Then `adb reboot bootloader` brings the phone back to fastboot without the user.
- **Flash a root image:** `fastboot flash -S 512M userdata <img>.simg` (7 min, 25 chunks). After a big flash the Xiaomi ABL fastboot hangs: next `fastboot boot` stalls at "Sending" -> the USER must re-enter fastboot by hand (Power+VolDown). Do not retry commands against the hung ABL. A flash right after `adb reboot bootloader` once failed on chunk 1 (USB protocol error): just retry. User must NOT unplug during flash (once interrupted at 25/25 -> reflash).
- **Edit the phone root without reflashing (same-length only):** find the file's raw offset in the local image (it is one extent; search the image for its first 4 KiB), compare with `adb exec-out dd if=/dev/block/sda34 ...`, then `adb push` changed 4 KiB blocks and `dd of=/dev/block/sda34 seek=... conv=notrunc,fsync`, read back. Recovery kernel has no hfsplus.
- **Pull the whole phone root** (12 min): `adb exec-out "dd if=/dev/block/sda34 bs=1048576 count=16384" > img`; mount ro with udisksctl or `sudo mount -t hfsplus -o loop,ro`. To write into a pulled image: `sudo fsck.hfsplus -fy` first (volume is never cleanly unmounted -> Linux mounts ro even with -o force), then `sudo mount -t hfsplus -o loop,force`.
- Making images: copy latest img with `cp --sparse=always`, mount rw with force, change, `sync; umount`, `sha256sum`, `img2simg`. Keep only the latest image + one fallback on the SSD (/home is ~98% full; big work files live in `/run/media/dipa/android/ios-sweet-work/`, old kernels/images archived in `.../ios-sweet-work/archive/`).

**Diagnostics built into the kernel (all SWEET-only):** persistent console log (0047, boot-arg sweetlog=0 off; flush every ~1 s); `|name| text` copies of /dev/null writes for processes in `sweetnull` (default backboardd,SpringBoard; 0048); `IOK name:` IOKit lookup/open trace for `sweetiolog` (default backboardd,SpringBoard; 0049/0054, 400 lines max); `SWEET Sandbox: proc op N` (0050); `STR ...` heap text dump on SIGABRT/SIGTRAP for `sweetscan` (default backboardd,SpringBoard; 0051/0053) - this is how NSException/BSAssert reasons were found; `SMP name tN pc .. bt ..` user stack sampler for `sweetsample` (default SpringBoard,backboardd; first at 90 s then every 120 s, 40 samples, 16 frames; 0054/0055); `SWEET FB: N swaps` every 1000 swaps; `CRASH name ...` crash reports (0043/0044). Symbolicate cache addresses with `ipsw dyld a2s research/dsc/dyld_shared_cache_arm64 <addr - slide>`; main-binary addresses: addr - mh + 0x100000000.

**Patches added today (0046-0057, all CI-built):** 0046 IOSurface (IOCoreSurfaceRoot) + IOMobileFramebuffer (AppleCLCD on /arm-io/disp0) + arm-io nubs; 0047 persistent log; 0048 /dev/null tee; 0049 IOKit trace; 0050 permissive "Sandbox" MAC policy; 0051/0053 heap text on crash; 0052 RENAME_SWAP via exchangedata on HFS; 0054/0055 sampler, swap counter, AKS selectors 35 (extended device state = 17) and 101; 0056 panel copy from the vsync timer at 10 Hz (sweetfb_hz) instead of every swap; 0057 no config_ext_resolver (ACL group entries resolve without memberd). ADT (tools/mkadt.py): /arm-io/disp0, /chosen/display-scale 3, /chosen/unique-chip-id (synthetic 0x5357454554000001) + chip-id 0x8015 -> IOPlatformUUID A9683613-3704-53F2-95D0-98A8FB574E78. Image v9-v12: /private/var/containers/{Bundle,Data/System,Shared/SystemGroup,Temp}; launchd.plist StandardOut/ErrorPath renamed to ...PatX (pointing jobs at /dev/console stops them launching: EACCES); v12: LastSystemVersion=20H392 in plain /var/mobile/Library/Preferences/com.apple.migration.plist (stops the 1-hour data migration that ran every boot), VirtualAudio.plugin moved to /Library/Audio/Plug-Ins/HAL.disabled/.

**Codex helper** (`codex exec --skip-git-repo-check -m gpt-6.1-sol -c model_reasoning_effort=high -s workspace-write -C /run/media/dipa/android/ios-sweet-work/codex -o OUT "$(cat task.md)" < /dev/null`, run detached with setsid nohup; config already defaults to gpt-6.1-sol high). Reports in `/run/media/dipa/android/ios-sweet-work/codex/`: mediaserverd-report.md, installcoordinationd-report.md (-> 0057), ufs-performance.md (ranked UFS speed plan), **ufs/0058-sweet-ufs-faster.patch** (READY, not yet applied: ufslogsecs default 10 s log flush batching, optional ufscache=1 directional cache ops, optional ufswait=1 spin-then-sleep; syntax-checked, host-stub verified; see ufs/README.md). Apply it as patch 0058 after review.

**Claude safety-classifier note:** several messages about QuartzCore renderer selection / writing an HFS reader tool were cut off by an automatic classifier (false positives, user agrees). Keep those topics short or let them be; they are not blockers now (rendering already works).

**NEXT STEPS (in order):**
1. **Input** (Setup stuck on Hello): touch + power + volume as IOHIDEventService/IOHIDDevice services backboardd matches (log shows backboardd matching IOHIDDevice with DeviceUsagePairs and IOService with HIDServiceSupport=true). Phone hardware: Goodix/Focaltech touch over I2C/SPI (check live DT `dtb/`), buttons via PMIC (pon) and GPIO keys. Start with a fake HID service (e.g. power button via a timer or boot-arg) to learn the ABI, then real drivers.
2. **Device identity**: MobileGestalt reports iPod + Home button. Add /product node / product-type (iPhone10,3, D22AP), device class iPhone, home-button-less (Face ID) properties in mkadt; find exact MG keys (MGIOMFBSupport.c, `IODeviceTree:/product` lookups seen in backboardd trace).
3. **Battery**: AppleSmartBattery/IOPMPowerSource style service (status bar shows empty red battery).
4. Apply Codex 0058 (UFS speed), consider cpus>1, check whether installcoordinationd/mediaserverd loops are gone, analyse mobileactivationd.
5. Hide kernel console over the GUI when not debugging: boot-arg sweetfb_noconsole=1 (but then panics are not on screen).

- **🎉 MILESTONE: iOS GUI ON THE PHONE - Setup.app "Hello" screen (2026-10-17 ~21:30 UTC, boot 0057u on image v12).** Photo: `/run/media/dipa/android/ios-sweet-work/MILESTONE-setup-hello-2026-10-17.jpg` (animated "dobar d(an)" hello lettering, status bar "iPod" + battery icon, bottom "Pritisnite Home za otvaranje" = "Press Home to open" (locale hr), (i) button). Rendered by iOS 16.7 SpringBoard/Setup through CoreAnimation -> backboardd -> kernel IOSurface (IOCoreSurfaceRoot) -> kernel AppleCLCD swaps -> ABL panel memory. What made it: data migration finally skipped (v12: `LastSystemVersion` 20H392 in the plain `/var/mobile/Library/Preferences/com.apple.migration.plist`; ADT `/chosen/unique-chip-id` + `chip-id` for IOPlatformUUID), 0057 (no ext resolver -> ACL containers work), plus everything before (0046 display/IOSurface, 0050 Sandbox policy, display-scale 3, containers dirs, RENAME_SWAP, AKS 35/101, 10 Hz present).
  **Missing (next work, in rough order):**
  - **Input: no touch, no power button, no volume buttons** (no HID drivers; backboardd looks for IOHIDDevice / HIDServiceSupport services). Setup is stuck on "Hello".
  - **Device identity: it thinks it is an iPod with a Home button** (status bar "iPod", "Press Home to open"): MobileGestalt has no product/model identity (no /product node, product-type, etc.) - give it iPhone10,3 / home-button-less (Face ID class) identity.
  - **No battery detection** (no battery/charger/PMIC service; status bar battery shows empty/red).
  - Locale came up Croatian (hr) - harmless.
  - Performance: cpus=1, polled UFS (Codex UFS patch in progress: `codex/ufs/`), backboardd presents at 60 Hz.
- **README refreshed (2026-10-17):** read the full notes and updated the public overview through patch 0045, v7 hardware observations and prepared v8 diagnostics. Documents real Apple userspace, the missing display/IOSurface GUI work, limited security services, highmem, persistent userdata root, current tooling and build examples. Documentation only; no kernel build, tests, flashing or phone boot performed.

- **v11 still migrated (boot 0056u, 20:32 UTC, `logs/boot-0056u-v11.txt`).** On iOS kCFPreferencesCurrentHost lands in the plain plist: all DMEnvironment setters (setLastBuildVersionPref, setUserDataDispositionPref, ...) use CurrentHost and the phone's `/var/mobile/Library/Preferences/com.apple.migration.plist` holds DMUserDataDisposition; datamigrator's XPC service runs with `_RoleAccount: mobile`. **Image v12** = v11 + `LastSystemVersion` = 20H392 added to that plain `com.apple.migration.plist` (keeps DMUserDataDisposition, 501:501 0600) + `/Library/Audio/Plug-Ins/HAL/VirtualAudio.plugin` moved to `/Library/Audio/Plug-Ins/HAL.disabled/` (Codex: stops the mediaserverd SIGSEGV loop; untested). **installcoordinationd loop explained (Codex `codex/installcoordinationd-report.md`):** os_crash "Failed to create data directory" EACCES for `/private/var/containers/Shared/SystemGroup/systemgroup.com.apple.installcoordinationd/Library/InstallCoordination`: `Library`/`Library/Caches` in the container are ACL-only (uid/gid 0xfffffffe, mode 0000, ACE for group GUID ...012B = gid 299 systemusers); with `config_ext_resolver` the kernel asks an identity resolver (memberd/opendirectoryd) that iOS does not run -> EWOULDBLOCK -> ACE skipped -> EACCES. **Patch 0057**: drop config_ext_resolver from FILESYS_BASE (compatibility UID/GID<->GUID mapping is used instead), likely fixes other ACL-protected containers too. Codex task 3: UFS driver performance design (`codex/ufs-performance.md`).
- **mediaserverd SIGSEGV loop explained (Codex report `/run/media/dipa/android/ios-sweet-work/codex/mediaserverd-report.md`):** the fault (FAR 0x390) is in the HAL plug-in `/Library/Audio/Plug-Ins/HAL/VirtualAudio.plugin/VirtualAudio` (ApplyBeamFormerPreference <- VirtualAudio_PlugIn::SetPropertyData <- HALS_AHPPlugIn::ObjectSetPropertyData <- AudioObjectSetPropertyData <- MediaExperience vaemVADSerializationListener): the default virtual audio device ('vdef', VirtualAudioDevice_Default) does not exist (no audio hardware/driver, no /product/audio). Proposed workaround for GUI bring-up: move VirtualAudio.plugin out of /Library/Audio/Plug-Ins/HAL/ in the next root image (untested). v11 reflashed OK after an interrupted flash (USB unplugged at chunk 25/25). Codex task 2: installcoordinationd crash loop.
- **ROOT CAUSE of the endless migration: no IOPlatformUUID (2026-10-17).** datamigrator's `-[DMMigrator isMigrationNeededWithBuildVersion:lastBuildVersion:clientPID:]` gets lastBuildVersion from `-[DMEnvironment lastBuildVersionPref]` = `CFPreferencesCopyValue("LastSystemVersion", "com.apple.migration", kCFPreferencesCurrentUser, kCFPreferencesCurrentHost)` -> a ByHost file. The phone had `/var/mobile/Library/Preferences/com.apple.migration.plist` (only DMUserDataDisposition) and NO `Preferences/ByHost` directory at all. On arm XNU publishes IOPlatformUUID (gethostuuid) only from `/chosen/unique-chip-id` (8 bytes) + `/chosen/chip-id` (4 bytes) (IOPlatformExpertDevice::generatePlatformUUID, called from IOPlatformExpert::start); the SWEET ADT had neither, so every CurrentHost preference write failed. Fix: mkadt adds `unique-chip-id` = 0x5357454554000001 (synthetic) and `chip-id` = 0x8015 -> UUID `A9683613-3704-53F2-95D0-98A8FB574E78` (same SHA1 namespace computation as the kernel). ADT `shim/out/sweet-0057hi.adt`, boot image `shim/out/shim-boot-0056u.img` (0056 kernel + 0057hi ADT, QEMU OK). **Image v11** = v10 + `Preferences/ByHost/com.apple.migration.<UUID upper and lower>.plist` {LastSystemVersion: 20H392} (501:501, dir 0755 files 0600). Flashing v11 2026-10-17 ~20:10 UTC. Codex (gpt-6.1-sol high) analysing the mediaserverd SIGSEGV loop in `/run/media/dipa/android/ios-sweet-work/codex/` (report `mediaserverd-report.md`).
- **v10 did NOT stop the migration (boot 0056n on v10, 19:18 UTC, `logs/boot-0056n-v10.txt`):** datamigrator + migrationpluginwrapper ran again, backboardd drew the progress UI (13000+ swaps in ~5 min). So `com.apple.DataMigration.plist`/LastSystemVersion is not the deciding input. What is known about the decision (datamigrator XPC service `DataMigration.framework/XPCServices/com.apple.datamigrator.xpc/com.apple.datamigrator`, strings): `Migration sentinel present. Migration necessary.` (sentinel `/private/var/datamigrator/migrate`, created and removed by datamigrator itself; absent in v10), `System build version changed from %@ to %@. Migration necessary.` / `...unchanged... not necessary`, `Disposition dictionary with build version %@ is out of date relative to current build version %@`, `SyntheticDidUpgrade`, DMUserDataDispositionManager; previous build source not found yet (not in the binary's plain strings; likely in DataMigration.framework's DMUserDataDispositionManager / preferences domain `com.apple.migration`). backboardd calls `DMPerformMigrationIfNeeded` and holds SpringBoard in `checkInAndWaitForDataMigration` until it returns. NEXT: find where the previous build version / disposition dictionary is stored (disassemble DataMigration.framework DMUserDataDispositionManager + datamigrator's isMigrationNeeded path), write it into the image; or get datamigrator's os_log out (OS_ACTIVITY_DT_MODE needs a launchd env change).
- **Data migration never records completion -> skip it (image v10, 2026-10-17).** Boot 0056n (18:16 UTC, `logs/boot-0056n.txt`, 30 min): 10 Hz panel copies, backboardd still presents at ~60 Hz (107k swaps), SpringBoard still in `BKSSystemShellService _checkIn` at sample 14. Every migration run takes 1+ hour on cpus=1 and repeats each boot. DataMigration decides from `/var/mobile/Library/Preferences/com.apple.DataMigration.plist` key `LastSystemVersion` (vs ProductBuildVersion 20H392); the pulled phone root (21:01 local) had NO such file. **v10** = that pulled phone state + fsck.hfsplus -fy (volume was marked not cleanly unmounted -> Linux mounts it read-only even with -o force; fsck found only hard-link counts + minor volume header, repaired) + `com.apple.DataMigration.plist` {LastSystemVersion: 20H392} (501:501 0600). `/run/media/dipa/android/ios-sweet-ipsw/ios16-root-20H392-v10.img` (+ .sha256, .simg); v8 files deleted, v9 kept as fallback.
- **Repo made PUBLIC (2026-10-17, user request) so GitHub-hosted macOS runners are free:** the 0056 CI run was refused ("recent account payments have failed or your spending limit needs to be increased"). Checked before: no device dumps, serials, IMEI or Apple binaries in the history (.gitignore keeps backup/, dtb/, recovery/, research/ out); commit author emails are public now. 0056 rerun `39056660104` passed -> `xnu/out-0056`, QEMU OK, image `shim/out/shim-boot-0056n.img` booted 18:16 UTC.
- **Boot 0055n (17:16 UTC, ~65 min, `logs/boot-0055n.txt`): migration bar reached 99% in ~30 min (was ~60), then crawled.** Root cause of the slowness: backboardd presents the progress UI at ~58 Hz (**167,000 swaps in ~48 min**) and AppleCLCD copied the full 10 MB frame into the panel on every swap -> ~600 MB/s memcpy on the single CPU (cpus=1), starving everything (polled UFS I/O too). Sampler (16 frames): **SpringBoard waits in `-[BKSSystemShellService _checkIn]`** (synchronous BSXPC call to backboardd, from FBSystemShellInitialize <- SBSystemAppMain) - backboardd holds the system shell check-in until the data migration ends, so the migration is the gate to SpringBoard UI. **Patch 0056**: swap only records the latest swap record; the vsync timer copies it to the panel every 60/sweetfb_hz ticks (default 10 Hz); `SWEET FB: N swaps` every 1000.
- **Boot 0054n (16:53 UTC, ~12 min, `logs/boot-0054n.txt`, patch 0054 sampler):** the data migration runs AGAIN on every boot (it completed once in the 0053s run) - completion is not being recorded. backboardd keeps swapping the progress UI (3700+ swaps in ~10 min, `SWEET FB: N swaps`). Sample during migration: SpringBoard main thread blocked in a synchronous XPC call (`mach_msg2_trap <- dispatch_mach_send_and_wait_for_reply <- xpc_connection_send_message_with_reply_sync <- -[BSXPCServiceConnectionMessage _sendSynchronously:] <- BSXPCServiceConnectionProxy ...`). Unsupported AKS selectors seen: 101 `aks_assert_consume`, 93 `aks_get_internal_info`, 34 `_aks_stash_load` (datamigrator), **35 `aks_get_extended_device_state`** (SpringBoard, chronod, remindd, searchd, symptomsd...). **Patch 0055**: AKS 35 answered like 17 (same DER device state), 101 succeeds; sampler backtraces 16 frames, 40 samples (~80 min).
- **MILESTONE: FIRST iOS PIXELS ON THE PANEL (boot 0053s long run, 15:33 UTC, ~60 min; photos IMG_20261017_183029/183222/183419; log `/run/media/dipa/android/ios-sweet-work/logs/boot-0053s-long.txt`, 407 KB, 1428 flushes).** The iOS data-migration boot screen (Apple logo + progress bar) was shown and the bar advanced to full in ~60 minutes. It is drawn by **backboardd through our kernel display path**: `SWEET SURF: backboardd create id 2/3 1080x2400 fmt 0x42475241 (BGRA) bpe 4 bpr 4352 size 10452992 keys 11`, IOSurface sels 9, 27, 5, 34 and traps 2/3 (lock/unlock), IOMFB SwapBegin (4) / SwapEnd (5, 1132-byte struct) / GetLayerDefaultSurface (3) / 68 / 55, then `SWEET FB: swap N layer 0 surface 2|3 1080x2400 ... a 0,0 1080x2400 b 0,0 1080x2400` alternating (double buffering; only the first 16 swaps are logged). Triggered by `com.apple.datamigrator` (AKS sels 101/93/34 return unsupported) and `com.apple.migrationpluginwrapper`. Probably ProgressUI-style CoreGraphics drawing into IOSurfaces (no GPU used). After the migration: SpringBoard and backboardd stay up (exec once each, no crash), nothing new drawn; crash loops only for mediaserverd, installcoordinationd, mobileactivationd (~108 each in the hour); bluetoothd and fairplayd.H2 relaunch constantly (no hardware). Kernel console text overlays the image (sweetfb_noconsole=1 would hide it).
- **MILESTONE: SpringBoard and backboardd both stay up (boot 0053s on v9, 15:29 UTC, `logs/boot-0053s.txt`).** With /chosen/display-scale = 3: `exec SpringBoard` once, `exec backboardd` once, no CRASH for either; MobileGestaltHelper reads IOMFB size/dot pitch; backboardd opens AppleCLCD (sels 57,15,22,29,33,79,74 as before). No IOSurface creates and no swaps: backboardd keeps polling `IOProviderClass IOAcceleratorES` (-> 0 found) and AGXAccelerator/IOAcceleratorESLegacy - no GPU, so no CoreAnimation renderer yet. Log goes quiet after ~47 s (only 47 flushes): the system idles. NEXT WALL: CoreAnimation rendering without a GPU (CPU path) so the window server produces frames.
- **SpringBoard's invalid mode = scale 0 (boot 0053n, 15:19 UTC, `logs/boot-0053n.txt`, heap scan):** `FBSDisplayMode cannot be configured as requested : <FBSDisplayMode: ...; infxinf@0x (1080x2400/0) 60Hz sRGB SDR>` - pixel size and 60 Hz are fine, scale is 0. libMobileGestalt (MGIOMFBSupport.c, sub_19c560a54) reads `display-scale` (u32, default 0) and `display-rotation` from **IODeviceTree:/chosen**, pixel size via IOMobileFramebufferGetDisplaySize (MobileGestaltHelper sel 8/28 in the log). mkadt now adds `/chosen/display-scale` = 3 (1080x2400 @3x = 360x800 pt). ADT `shim/out/sweet-0054hi.adt` (QEMU `research/q0054-hi.adt`), image `shim/out/shim-boot-0053s.img` = 0053 kernel + 0054hi ADT. Possible catch: MobileGestalt may have cached scale 0 in `/private/var/containers/Shared/SystemGroup/systemgroup.com.apple.mobilegestaltcache/...`.
- **MILESTONE: backboardd survives and opens the display (boot 0052n on v9, 15:07 UTC, `logs/boot-0052n.txt`).** No `CRASH backboardd`. It walks `IODeviceTree:/arm-io` children, matches IOMobileFramebuffer, `open AppleCLCD type 0 -> 0x0`, calls IOMFB selectors 57, 15, 22, 29, 33, 79 (GetBlock, out 1580/40), 74 (SupportedFrameInfo), 12, 6, 27, 17, 84, 50; opens IOCoreSurfaceRoot (sel 13); looks for AGXAccelerator / IOAcceleratorES / IOAcceleratorESLegacy (none, repeatedly), `IODeviceTree:/product` (none, repeatedly), `/buttons`, aop-smart-cover, IOPMrootDomain. No swaps yet. **SpringBoard now gets past display discovery** and dies (sig 5) in `-[FBSDisplayMode _initWithWidth:height:preferredScale:scaleOverride:refreshRate:gamut:hdr:rotation:virtual:validityCheck:]+580` <- `_initWithCADisplayMode:scale:rotation:` <- `FBSDisplayConfiguration _initWithImmutableDisplay:` <- `FBSDisplaySource initWithDisplay:` <- `FBSDisplayMonitor` (slide 0x14504000): "FBSDisplayMode cannot be configured as requested". Checks: width/height >= 2, preferredScale > 0, scaleOverride >= 0, refreshRate non-zero, valid gamut/HDR/rotation. Suspects: refresh rate 0 (IOMFB timing blocks answered with zeros) and/or scale 0 (MobileGestalt main-screen-scale; no /product node in the ADT). **Patch 0053**: heap text scan also on SIGTRAP and for SpringBoard (default sweetscan list backboardd,SpringBoard) to read the exact mode description.
- **Image v9 (2026-10-17) = v8 + `/private/var/containers/{Bundle,Data/System,Shared/SystemGroup,Temp}` (root:wheel 0755; on iOS these live on the data volume, absent from the rootfs) + launchd.plist with StandardOutPath/StandardErrorPath renamed to ...PatX (the in-place phone fix; daemons' stdout reaches the log via the 0048 /dev/null tee).** `/run/media/dipa/android/ios-sweet-ipsw/ios16-root-20H392-v9.img` SHA256 aa9f9139...6e67, `.simg` 12,145,303,864 bytes; written through `sudo mount -t hfsplus -o loop,force`. **Patch 0052** (`xnu/patches/0052-sweet-hfs-rename-swap.patch`, CI `39042761826` -> `xnu/out-0052`, QEMU OK, image `shim/out/shim-boot-0052n.img`): vn_rename falls back to VNOP_EXCHANGE when VNOP_RENAMEX(RENAME_SWAP) returns ENOTSUP for two regular files on one volume (HFS exchange swaps data and cnode descriptors = swap semantics). Flashing v9 started 2026-10-17 ~15:00 UTC.
- **backboardd EXCEPTION FOUND (boot 0051n, 14:41 UTC, `logs/boot-0051n.txt`, patch 0051 heap scan):** `*** Terminating app due to uncaught exception 'NSInvalidArgumentException', reason: '*** -[NSURL initFileURLWithPath:]: nil string parameter'`. First-throw stack (slide 0x337f0000): `+[NSURL fileURLWithPath:]` <- `-[BSSystemContainerForCurrentProcessPathProvider libraryPath]` <- `cachesPath` <- backboardd main+0x1d858 (StartWindowServer, shader cache path). BaseBoard: libraryPath = `BSSystemSharedResourcesDirectoryForIdentifier` = `BSSystemSharedDirectoryForIdentifier(id)` + "/Library"; that calls `container_system_path_for_identifier` (libsystem_containermanager -> `_container_legacy_lookup` -> containermanagerd XPC), which returned NULL (BaseBoard logs "Error getting shared system container ..."). With 0050 the sandbox denials are gone. containermanagerd.log.0 in the pulled (pre-0050) image: "Did not receive any persona attributes", "Expected to find at least one persona", notify_register_dispatch failed 9; `/private/var/containers` does NOT exist in the root image (on iOS it lives on the data volume). usermanagerd does create kernel personas 99/199/1000, but fails to save `/private/var/keybags/usersession.kb` and `persona.kb`: "Failed to atomically swap writing file: Operation not supported (45)" (HFS has no RENAME_SWAP). NEXT: create `/private/var/containers` (and check what containermanagerd needs for system group containers), fix or emulate RENAME_SWAP on HFS, then re-check backboardd.
- **Disk housekeeping (2026-10-17):** /home was at 4 GiB free. Moved (not deleted) `xnu/out-0010..0044` and old `shim/out/*.img`/`payload-*.bin` (everything before 0046) to `/run/media/dipa/android/ios-sweet-work/archive/{xnu,shim-out}` (6.3 GiB); /home now 8.7 GiB free. Large work files (phone-root.img 16 GiB, logs, tracev3 dumps) live in `/run/media/dipa/android/ios-sweet-work/`. **Patch 0051** (`xnu/patches/0051-sweet-crash-heap-strings.patch`, CI `39041440935`): on SIGABRT of a process in boot-arg `sweetscan` (default backboardd) the crash log prints heap text runs as `STR ...` (to find the NSException reason). Boot 0051n 14:41 UTC.
- **Boot 0049n (14:11 UTC, `logs/boot-0049n.txt`, patch 0049 trace): backboardd's IOKit lookups before it dies** (each run): `IODeviceTree:/arm-io/pearl-sep` -> none, matching ApplePearlSEPDriver -> none, `IOService:/IOResources/AppleKeyStore` open OK, matching IOHIDDevice (DeviceUsagePairs) / `HIDServiceSupport` notifications, `usb-drd,t8027` notification; **never any IOMobileFramebuffer / display lookup** - the exception happens before display detection. launchd (tracev3) shows backboardd's mach lookups denied with `error = 159: Sandbox restriction`: com.apple.mobilegestalt.xpc, com.apple.iohideventsystem, com.apple.system.notification_center, com.apple.logd, containermanagerd, ACCHWComponentAuthService, oscard. libsystem_sandbox uses `__mac_syscall("Sandbox", op, arg)` (ops: 1 init, 2 check, 3 note, 4 container path, 5/6/7/8 extensions, 0x15 check_bulk, ...); with no policy named "Sandbox" every call gets ENOPOLICY. **Patch 0050** (`xnu/patches/0050-sweet-sandbox-policy.patch`): built-in permissive MAC policy "Sandbox" in sweet_amfi.c (mpo_policy_syscall returns 0, container/extension ops ENOENT; logs first use of each op as `SWEET Sandbox: proc op N`).
- **Phone root pulled (16 GiB, 12 min, `adb exec-out dd if=/dev/block/sda34`) to `/run/media/dipa/android/ios-sweet-work/phone-root.img` (loop-mounted ro at /run/media/dipa/iOSRoot).** No .ips crash reports. logd tracev3 (Persist, LZ4 `bv41` blocks, decompressed strings in `ios-sweet-work/tracev3/all.txt`): /private/var/db/uuidtext is EMPTY so only launchd's own messages are readable. Findings: v8 launch failure = `Service could not initialize: posix_spawn(/usr/libexec/backboardd), error 0xd - Permission denied` (xpcproxy exit 78) - /dev/console not openable for the mobile user; backboardd[98] spawned `com.apple.MTLCompilerService` before SIGABRT; **launchd denies many mach lookups with `error = 159: Sandbox restriction`** (mediaserverd -> com.apple.backboard.display.services, remoted, mobiletimerd -> sleepd, identityservicesd -> apsd) and **tccd exits OS_REASON_SANDBOX "profile not found"** - no Sandbox kext/profiles on SWEET; likely cause of many daemon crash loops (next wall after the display). CF's uncaught-exception text never reaches stderr for daemons (no CF handler; libc++abi prints only the type). **Patch 0049** (`xnu/patches/0049-sweet-iokit-trace.patch`): `IOK name: ...` console lines for registry path lookups, child iterators, matching (with result count/class), matching notifications and IOServiceOpen of the processes in boot-arg `sweetiolog` (default backboardd), max 400 lines.
- **CONFIRMED: launchd StandardOut/ErrorPath=/dev/console stops backboardd from launching.** After the in-place rename (boot 0047n-b, log `logs/boot-0047n-b.txt`) backboardd launches again and dies as on v7: sig 6, `___pthread_kill <- abort <- demangling_terminate_handler <- _objc_terminate <- std::terminate <- _dispatch_client_callout <- _dispatch_once_callout <- main+0x9334` (window-server dispatch_once). It never opened AppleCLCD or IOCoreSurfaceRoot (no `SWEET FB/SURF: backboardd` lines) -> throws before/at display detection. **Patch 0048** (`xnu/patches/0048-sweet-devnull-tee.patch`, CI `39037749668` -> `xnu/out-0048`, image `shim/out/shim-boot-0048n.img`): writes to /dev/null by processes named in boot-arg `sweetnull` (default `backboardd,SpringBoard`) are printed as `|name| text`. Boot 0048n (13:48 UTC, log `logs/boot-0048n.txt`): only `|backboardd| libc++abi: terminating due to uncaught exception of type NSException` (CF's reason line goes to os_log, not stderr, despite CFLOG_FORCE_STDERR/OS_ACTIVITY_DT_MODE). Next: pull the whole 16 GiB root from recovery (`adb exec-out dd if=/dev/block/sda34`) to `/run/media/dipa/android/ios-sweet-work/phone-root.img` and read ReportCrash .ips files / logd tracev3 for the exception reason.
- **Persistent log WORKS (boot 0047n on v8, 13:31 UTC; pulled from OrangeFox: 70 KB, 111 flushes, `/run/media/dipa/android/ios-sweet-work/logs/boot-0047n.txt`).** In 3 min **backboardd was never exec'd** (v7 ran it), ~70 bare `exec xpcproxy`, SpringBoard loops on FBSDisplayMonitor (no display), mediaserverd sig 11 loop, locationd, lockdownd sig 5. Suspect the v8 plist (StandardOut/ErrorPath=/dev/console on backboardd). **In-place plist edit from recovery (no reflash):** launchd.plist is one extent at image/partition byte offset 0x1e025f000 (195 blocks; verified bytes identical on phone vs local v8 mount); renamed StandardOutPath->StandardOutPatX and StandardErrorPath->StandardErrorPatX (same length, both jobs) in blocks 68 and 105 via `adb push` + `dd of=/dev/block/sda34 seek=... conv=notrunc,fsync`, read back VERIFIED. Phone now differs from local v8 image in those 2 blocks (files in `/run/media/dipa/android/ios-sweet-work/`). Test boot 0047n 13:38:42 UTC. Recovery kernel has no hfsplus (raw block edits only).
- **Patch 0047: persistent console log (CI `39036478663` -> `xnu/out-0047`, QEMU no panic; phone image `shim/out/shim-boot-0047n.img` = 0047 + sweet-0046hi.adt + 0045n cmdline).** `_cnputs` (all console output: kernel printf + daemons' /dev/console) also appends to a 4 MiB ring in sweet_ufs.c; after the UFS partition is ready and writable (ufsrw=1) a thread writes new ring blocks + header to the LAST 1025 blocks (4 KiB) of userdata every ~1 s and SYNCHRONIZE CACHE (prints `SWEET LOG: console log to the last 4100 KiB ...`). Read: phone to fastboot -> `fastboot boot recovery/recovery.img` (OrangeFox) -> `tools/sweet-log-pull.sh OUT.txt` (adb dd of that area + `tools/sweet-log.py`). Not yet verified on the phone. Panics after the last flush are not in it (screen still shows them). 2x font attempt reverted: XNU's v_scale only scales the progress icon.
- **Boot 0046n on v8 (13:05 UTC, VID_20261017_150522_HSR_120):** phone prints `SWEET SURF: IOCoreSurfaceRoot registered` and `SWEET FB: AppleCLCD 1080x2400 stride 4320 at 0x9c000000, default surface 1` (nubs + match work on hardware). Visible userspace: early boot tasks, launchd_cache_loader, `exec SpringBoard`; no `exec backboardd` / `SWEET FB: backboardd` line seen in the readable frames (video blurry). Frame 00:47: lower half of the screen shows OLDER console text (16:01:12) below newer lines -> unconfirmed hint that a swap copied an old snapshot of the panel (CA reads the default/displayed surface = panel memory). SpringBoard still dies in `-[FBSDisplayMonitor _initWithBookendObserver:transformer:]+1160` (sig 5, slide 0x30960000). **Shim: 2x console font** (`shim/loader.c` Video.v_depth scale field = 1) so phone videos are readable; image `shim/out/shim-boot-0046b.img` = 0046n + 2x font.
- **Patch 0046: built-in IOSurface + IOMobileFramebuffer (2026-10-17, offline, NOT phone-tested).** `xnu/patches/0046-sweet-iosurface-iomfb.patch`: new `iokit/Kernel/arm/SweetSurface.{h,cpp}` (IOCoreSurfaceRoot + IOSurfaceRootUserClient: sel 13/0/4/34/35, traps 0-5, wired buffers mapped into the caller, per-client refs) and `SweetFramebuffer.cpp` (AppleCLCD : IOMobileFramebuffer on /arm-io/disp0: sizes, swap = CPU copy of enabled layers into the ABL panel memory, 60 Hz vsync via sendAsyncResult64WithOptions(CanDrop), panel memory published as the default IOSurface, kernel text console paused on the first swap only with `sweetfb_noconsole=1` (default keeps the console: pausing switches it to serial and would hide panics)); SweetPlatformExpert creates nubs for /arm-io children (not gic) attached to the platform expert (IOPlatformDevice::compareName casts its provider to IOPlatformExpert, so nubs under the arm-io nub never matched - first CI build showed that in QEMU). mkadt: /arm-io/disp0 {device_type display-subsystem, compatible sweet,display, canvas-width 1080, canvas-height 2400}. CI `39021572169` -> `xnu/out-0046`. **QEMU** (`research/q0046-hi.adt`, RAM disk init, msgbuf): `SWEET SURF: IOCoreSurfaceRoot registered`, `SWEET FB: AppleCLCD 1080x2400 stride 4320 at 0x9c000000, default surface 1`, init alive, no panic. Phone image **`shim/out/shim-boot-0046n.img`** (SHA256 9c4d2fe9..., CI `39024108588` with the review fixes: swap bounds check, zeroed 64-bit async ref, port release, 1.5 GiB wired budget, ref leak/race fixes; QEMU re-checked) = 0046 kernel + `shim/out/sweet-0046hi.adt` + the 0045n cmdline; boot it on image v8 (backboardd/SpringBoard log to console) and film. Expected new console lines: `SWEET SURF: <proc> create id ...`, `SWEET FB: backboardd sel N ...`, `SWEET FB: swap ...`. Open question for the next wall: QuartzCore's IOMFB server builds its renderer from Metal first (`CAMetalContextCreate`), GLES only when enabled; how the CPU renderer gets selected on a device without a GPU is not yet established.
- **IOSurface values ABI (offline RE, `/run/media/dipa/android/ios-sweet-work/iosurface-values.md`) - TODO in SweetSurface (0046 only accepts and drops values, so IOSurfaceCopyValue always returns NULL):** sel 9 SetValue in = {u32 id, u32 pad, IOCFSerialize([value, key]) or ([dict] = merge)}, sel 0xb RemoveValue in = {u32 id, u32 pad, key\0} (empty key = remove all); both out 4 bytes = new generation, client accepts only (gen+2)|1. Kernel after each change: gen = (gen+2)|1 (odd, never 0) -> state+0x20, serialized size (>=1) -> state+0x28. sel 0xa in {u32 id, u32 0, u8 0}, out {u32 ?, u32 gen, serialized non-bulk dict}; too small -> 0xe00002db + needed size (client retries x16). sel 0x1b in 0x88: bulk struct 0x70 @0, u64 valid size @0x70, u64 changed mask @0x78, u32 id @0x80 -> copy masked fields into state+0x30 (bit0 32B @0, bit1 16B @0x20, bit2 8B @0x30, bits3-10 1B @0x38..0x3f, bits11-14 24B @0x40/4B @0x58/8B @0x5c/1B @0x64).
- **IOMFB display matching confirmed (offline RE, `/run/media/dipa/android/ios-sweet-work/iomfb-match.md`):** the framework never matches on "AppleCLCD"; any registered IOMobileFramebuffer subclass works. `iomfb_match_callback` reads only `dfr`/`AppleTV`/`external` (CFBoolean, service itself, no parents); internal main display = all absent/false (type 0, name "primary"). Display count = /arm-io children whose device_type starts with "display-subsystem" (count 0 -> CreateDisplayList returns NULL; more nodes than services -> 30 s wait). IOMobileFramebufferOpen fails only on IOServiceOpen failure; canvas-width/height read as 4-byte CFData from the IOService-plane parent (optional). CopyProperty = IORegistryEntrySearchCFProperty on the service only. Patch 0046 (disp0 node, `external`=false, AppleCLCD : IOMobileFramebuffer) fits all of this.
- **IOMobileFramebuffer swap + notification ABI (2026-10-17, offline).** Swap record = fb+0x18, **0x46c bytes**, zeroed by SwapBegin and sent whole by `kern_SwapEnd` = IOConnectCallStructMethod(sel 5, rec, 0x46c, no output). **SwapBegin** = sel 4, no input, 1 scalar out = swap id (stored at rec+0x50). `kern_SwapSetLayer(fb, layer<4, IOSurfaceRef, CGRect a, CGRect b, flags)`: rec+0x54+4*layer = **IOSurface id**; rec+0x104 bit layer = layer enabled (rec+0x108 bit cleared); rect a (int x,y,w,h, scaled) at rec+0x64+16*layer, rect b at rec+0xc4+16*layer; rec+0x114+4*layer = transform from flags&0xf; flag bits 5/31/6/7/8/9 set bytes rec+0x2ca/0x2cc/0x2e3/0x2e5/0x2e6/0x467+layer. A NULL surface only sets the enable bit with zero id. SwapWait = sel 6 scalars [swap id, timeout, 0].
  - **Notifications**: `IOMobileFramebufferScheduleWithDispatchQueue`/`GetRunLoopSource` create an IONotificationPort and `IOConnectSetNotificationPort(conn, type, port, 0)` (kernel `registerNotificationPort`). `kern_EnableNotifications(fb, type, cb, ctx)` = **sel 72 scalars [NotifyCallbackFuncs[type], fb, type, 0]** -> the kernel keeps an async reference {port, func, refcon=fb} per type and fires it with `sendAsyncResult64`. Types: 0 HotPlug, 1 Power, 3 FrameInfo (args[0] = mask of 31-bit keys, args[1] u32, then values), 4 CRC, **5 Vsync**: 6 args, each a 32-bit half in a u64 slot: (ts lo, ts hi), (t2 lo, hi) passed through the time-ingest helper, (t3 lo, hi); the framework then calls the client's vsync callback(fb, ts, t2', t3, ctx). Vsync callbacks are dropped while the notification port has queued messages.

- **IOSurface.framework user-client ABI reversed (2026-10-17, offline, step 2 of the GUI plan).** `ipsw dyld disass research/dsc/dyld_shared_cache_arm64 --image IOSurface` (selector scan script: scratchpad `selscan.py`).
  - Connect (`_iosConnectInitalize`, pthread_once): `IOServiceGetMatchingService(IOServiceNameMatching("IOCoreSurfaceRoot"))`, retried with sleep(1) up to 241 times (a missing service stalls every client ~4 min, no crash); `IOServiceOpen(type 0)`; then **sel 13** (no inputs, struct out 0x18 bytes = `_ioSurfaceLimits`) and requires u16 at out+0x16 == 1 (ABI version); out+0 = offset/plane-base alignment mask, out+4 = bytes-per-row alignment mask (used as (x+m)&~m), +0..+0x14 also copied into IOSurface remote-method init, otherwise logs and closes the connection.
  - **Create = sel 0**: scalar in [parent id or 0], struct in = `IOCFSerialize(props, kIOCFSerializeToBinary)` (OSUnserializeBinary format; keys IOSurfaceWidth/Height/BytesPerRow/BytesPerElement/PixelFormat/AllocSize/PlaneInfo/CacheMode/IsGlobal/...), struct out **0xa68 bytes** ("lock result"). **Lookup = sel 4**: scalar in [id], same struct out. LookupFromMachPort = sel 0x22 (scalar in [port name]); CreateMachPort = sel 0x23 (scalar in [id, options], scalar out [port name]).
  - Lock result (copied to client+0x68, size 0x68 + planes*0x80, min 0xe8): 0x00 u64 base address **mapped in the caller's task**; 0x08 u64 task VA of a per-surface shared state page (+0 u64 registry id, +8/+0xa u16 bound accel ids, +0xc u32 seed, +0x14 u8 tiled, +0x20 u32 value generation, +0x24 YCbCr matrix, +0x28 u32 non-bulk values size, +0x2d u8 displayable, +0x2e u8 sysmem-only, +0x30 0x70 bytes bulk attachments); 0x10 u64 task VA of timestamp page (4 u64 indexed timestamps, +0x20 detach code); 0x18 u32 surface id; 0x1c u32 alloc size; 0x20 width; 0x24 height; 0x28 bytes per row; 0x2c buffer offset; 0x30 pixel format; 0x34 u32 (dcache flush length); 0x38 plane count; 0x40 u16 bytes/element; 0x42 u8 element width; 0x43 u8 element height; 0x44 cache mode; 0x4c u8 subsampling; 0x4d u8 prohibit-use count; 0x50 parent id; 0x54 slice count; 0x58 u64 trace id; 0x60 u64 protection options; planes[n] at 0x68+n*0x80 (+0 width, +4 height, +8 bytes/row, +0xc offset, +0x14 size, +0x18 u16 bpe, +0x1a/+0x1b u8 element w/h, +0x1c u8 component count, +0x1d address format, +0x54 u8 compression type (non-zero -> no CPU base address)).
  - **Traps** (IOConnectTrapN -> `IOUserClient::getTargetAndTrapForIndex`): 0 IncrementUseCount(id, category), 1 DecrementUseCount(id, category), 2 **Lock**(id, options, &seed_out), 3 **Unlock**(id, options, &seed_out), 4 Release(id), 5 Release after a failed create/lookup setup (id).
  - Other selectors: 5 SetYCbCrMatrix, 6 WrapClientImage, 7 WrapClientMemory, 9 SetValue (serialized), 0xa UpdateNonBulkValueCache (struct in 9), 0xb RemoveValue, 0xc BindAccel, 0x11 SetSurfaceNotify (async), 0x12 RemoveSurfaceNotify, 0x13 Log, 0x14 SetPurgeable, 0x15 SetOwnership, 0x16 SetTiled, 0x18 SetTimestamp, 0x19 GetTileFormat, 0x1a CopyDataValueBytes, 0x1b SetBulkAttachments (struct 0x88), 0x1e/0x2e-0x31 (via outlined helpers), 0x1f SetCompressedTileDataRegionMemoryUsed, 0x20 GetGraphicsCommPageAddress, 0x21 SetIndexedTimestamp, 0x24-0x26/0x2d IOSurfaceAccelerator, 0x27-0x29 shared events, 0x2a SignalEvent, 0x2b Set/InitDetachModeCode, 0x2c SetOwnershipIdentity, 0x32-0x34 data properties.
  - => SWEET kernel service: class/name **IOCoreSurfaceRoot** + IOSurfaceRootUserClient; minimum = sel 13, 0, 4, 9/0xa/0xb (values), traps 0-5, 0x22/0x23 later for cross-process surfaces (CA sends surfaces to backboardd by mach port). Memory: IOBufferMemoryDescriptor per surface, mapped into each task with createMappingInTask; ids from a global table.

- **Image v8: daemon logs on the console.** `tools/patch-launchd-plist.py IN OUT LABEL...` rewrites launchd.plist (binary, plistlib): renames every `_PanicOnCrash` to `_PanicOnCrasX` and gives the listed jobs `StandardOutPath`/`StandardErrorPath` `/dev/console` (devfs console is 0622, writable by mobile) plus `OS_ACTIVITY_DT_MODE=YES` (os_log mirrored to stderr) and `CFLOG_FORCE_STDERR=1`. v8 = v7 + that plist for com.apple.backboardd and com.apple.SpringBoard, written through the mount (796310 bytes, root:80 0644), SHA256 in `ios16-root-20H392-v8.img.sha256`, `.simg` ready; boot with `shim/out/shim-boot-0045n.img`. Goal: see backboardd's uncaught exception text and the window server's display probing. ODDITY (unresolved): the v5 raw-image rename at offset 0x1e025f000 is present in the v5-v7 image bytes, but the plist read through a Linux mount of v7 still had the original `_PanicOnCrash` keys; filefrag shows the file as one extent. Never patch file contents through raw offsets again; write through the mount.
- **IOMobileFramebuffer.framework reversed (step 1 of the GUI plan).** `ipsw dyld disass research/dsc/dyld_shared_cache_arm64 --image IOMobileFramebuffer`.
  - Display discovery (`IOMobileFramebufferCreateDisplayList` -> `getDisplayListNumber`): iterates children of `IODeviceTree:/arm-io`, counts nodes whose `device_type` is "display-subsystem" or "ext-display-subsystem" (or `compatible` "paravirtualizedgraphics,gpu"); then `iomfb_populate_display_infos` adds an IOServiceAddMatchingNotification(IOServiceFirstMatch, **IOServiceMatching("IOMobileFramebuffer")**) and runs the run loop up to 30 s until that many services matched. `iomfb_match_callback` reads service properties `dfr`, `AppleTV`, `external` (CFBoolean) and names displays (external ones "%s-%d").
  - `IOMobileFramebufferOpen(service, task, type, &fb)`: `__ioMobileFramebufferAlloc` (CFPreferences com.apple.iokit.IOMobileGraphicsFamily: canvas_width/height, override_display_width/height, override_panel_width/height, benchmark, enable_ktrace), `IOServiceOpen(service, task, type)` (GetMainDisplay passes type 0), `getEDTProperty` reads `canvas-width`/`canvas-height` from the service's IODeviceTree parent, then fills a table of `kern_*` function pointers.
  - User client selectors (`kern_*` -> IOConnectCall*): 3 _kern_GetLayerDefaultSurface[ScalarMethod_stub]; 4 _kern_SwapBegin[ScalarMethod_stub]; 5 _kern_SwapEnd[StructMethod_stub]; 6 _kern_SwapWaitWithTimeout[ScalarMethod_stub] _kern_SwapWait[ScalarMethod_stub]; 7 _kern_GetID[ScalarMethod_stub]; 8 _kern_GetDisplaySize[ScalarMethod_stub]; 10 _kern_SetTVOutMode[ScalarMethod_stub]; 11 _kern_SetWSSInfo[ScalarMethod_stub]; 12 _kern_RequestPowerChange[ScalarMethod_stub]; 13 _kern_EnableStatistics[ScalarMethod_stub]; 14 _kern_SetFlags[ScalarMethod_stub]; 15 _kern_SetDebugFlags[ScalarMethod_stub]; 16 _kern_SetTVOutSignalType[ScalarMethod_stub]; 17 _kern_SetGammaTable[StructMethod_stub]; 18 _IOMobileFramebufferIsMainDisplay[ScalarMethod_stub]; 19 _kern_SetWhiteOnBlackMode[ScalarMethod_stub]; 20 _kern_SwapSignal[ScalarMethod_stub]; 22 _kern_SetDisplayDevice[ScalarMethod_stub]; 23 _kern_SetDigitalOutMode[ScalarMethod_stub]; 24 _kern_GetHDCPDownstreamState[StructMethod_stub]; 25 _kern_GetDigitalOutState[ScalarMethod_stub]; 26 _kern_printDisplayRegs[ScalarMethod_stub]; 27 _kern_GetGammaTable[StructMethod_stub]; 28 _kern_GetDotPitch[ScalarMethod_stub]; 29 _kern_GetDisplayArea[StructMethod_stub]; 30 _kern_EnableDisableDithering[ScalarMethod_stub]; 31 _kern_SetUnderrunColor[ScalarMethod_stub]; 33 _kern_EnableDisableVideoPowerSavings[ScalarMethod_stub]; 34 _kern_SetVideoDACGain[ScalarMethod_stub]; 35 _kern_SetLine21Data[ScalarMethod_stub]; 41 _IOMobileFramebufferEnableMirroring[ScalarMethod_stub]; 42 _IOMobileFrameBufferGetMirroringCapability[StructMethod_stub]; 43 _IOMobileFramebufferSetRenderingAngle[StructMethod_stub]; 44 _IOMobileFramebufferSetMirrorContentRegion[StructMethod_stub]; 47 _kern_HDCPSendRequest[ScalarMethod_stub]; 48 _kern_HDCPGetReply[ScalarMethod_stub]; 49 _kern_SurfaceIsReplaceable[ScalarMethod_stub]; 50 _kern_SetBrightnessCorrection[ScalarMethod_stub]; 51 _kern_SetColorRemapMode[ScalarMethod_stub]; 52 _kern_SwapCancel[ScalarMethod_stub]; 53 _IOMobileFramebufferSetCanvasSize[ScalarMethod_stub]; 54 _IOMobileFramebufferGetMatrix[Method_stub]; 55 _IOMobileFramebufferSetMatrix[Method_stub]; 56 _IOMobileFramebufferKernelTests[StructMethod_stub]; 57 _kern_GetColorRemapMode[ScalarMethod_stub]; 66 _kern_SPLCSetBrightness[ScalarMethod_stub]; 67 _kern_SPLCGetBrightness[ScalarMethod_stub]; 68 _kern_SetParameter[Method_stub]; 69 _kern_SwapSetColorMatrix[StructMethod_stub]; 72 _kern_DisableNotifications[ScalarMethod_stub] _kern_EnableNotifications[ScalarMethod_stub]; 73 _kern_ChangeFrameInfo[ScalarMethod_stub]; 74 _kern_SupportedFrameInfo[Method_stub]; 75 _kern_FactoryPortal[StructMethod_stub]; 77 _kern_GetCurrentAbsoluteTime[ScalarMethod_stub]; 78 _kern_SetBlock[Method_stub]; 79 _kern_GetBlock[Method_stub]; 80 _kern_GetLinkQuality[ScalarMethod_stub]; 81 _kern_SwapCancelAll[ScalarMethod_stub]; 82 _kern_GetProtectionOptions[ScalarMethod_stub]; 83 _kern_CopyLayerDisplayedSurface[ScalarMethod_stub]; 84 _kern_SetContrast[StructMethod_stub]; 85 _kern_GetBufBlock[Method_stub]; 86 _IOMFBGainEncoderFinishEncoding[Method_stub]; UNKNOWN {}
  - => Kernel needs: an `/arm-io` child node with device_type "display-subsystem" (+ canvas-width/height) and an IOService of class **IOMobileFramebuffer** (subclass name match) attached under it, opened with type 0, implementing at least GetDisplaySize(8), IsMainDisplay(18), GetID(7), GetLayerDefaultSurface(3), SwapBegin(4)/SwapEnd(5 struct)/SwapWait(6), Enable/DisableNotifications(72), frame info(73/74), CopyLayerDisplayedSurface(83), plus IOSurface for the swapped buffers. NEXT: CoreAnimation's AppleDisplay/IOMFBDisplay usage of these (constructor, first swap, vsync), then IOSurface.framework's IOSurfaceRoot selectors.
- **GUI PLAN (decided 2026-10-16): CPU rendering + kernel IOMobileFramebuffer.** User: CPU rendering is fine for now (goal = prove the GUI runs), may later research an Apple-style GPU driver for the Adreno 618 -> keep display output and renderer separable. QuartzCore findings (symbols from the cache's .symbols subcache: `ipsw dyld symaddr research/dsc/dyld_shared_cache_arm64 --image QuartzCore`; strings: `ipsw dyld macho ... QuartzCore --strings`):
  - Renderers: `sw_new_context` (CA::OGL::SWContext, full CPU renderer incl. blur, images, IOSurface-backed surfaces, make_buffer_current(WindowServer::Surface)), `gles_new_context`, `metal_new_context`, `new_null_context`. Env knobs include CA_ENABLE_OGL, CA_DISPLAY_TYPE, CA_FORCE_LOCAL_SERVER, CA_FORCE_TIMER, CA_CANVAS_WIDTH/HEIGHT, CA_MAIN_DISPLAY_SCALE/ORIENTATION, CA_DISABLE_RENDER, CA_ALWAYS_RENDER (selection logic not traced yet).
  - `-[CAWindowServer _detectDisplays]` (0x1884e3150) iterates static tables `open_funcs`/`count_funcs` = AppleCLCDDisplay, AppleRGBOutDisplay, AppleWirelessDisplay only; each `count()` = `display_count_of_type(IOMFBDisplayType)` over **`IOMobileFramebufferCreateDisplayList()`**, `open(i)` = `create_display_of_type` -> `CA::WindowServer::AppleDisplay(CFString name, ..., IOMFBDisplayType)` (an IOMFBDisplay driven by IOMFBServer: vsync_callback, frame_info_callback, swaps). `CA::WindowServer::VirtualDisplay` exists but is never auto-detected (explicit API only). => the main display MUST come from an IOMobileFramebuffer kernel service.
  - NEXT: reverse IOMobileFramebuffer.framework: IOMobileFramebufferCreateDisplayList / IOMobileFramebufferOpen (service matching, display type property), the user-client selectors used by IOMFBServer (display size/modes, swap begin/set-layer/end with IOSurface ids, vsync notifications), then the IOSurface kernel side (IOSurfaceRoot user client: create/lookup/lock, memory mapping) that both IOMFB swaps and CA surfaces need. Kernel: SWEET IOMobileFramebuffer service that copies the swapped IOSurface to the 1080x2400 framebuffer at 0x9c000000.
- **logd fixed; backboardd = DISPLAY WALL (boot 0045n on v7, 21:08:57 UTC, frames VID_20261016_230850 00:55-01:28, slide 0x4c3ec000, `research/crash-0045n-v7.txt`).** No `CRASH logd` any more; new daemons run (runningboardd, WirelessRadioManagerd, networkserviceproxy, dasd, cloudpaird, magicswitchd, atc, askpermissiond, ManagedSettingsAgent, containermanagerd uses AKS 0/17). New: locationd sig 11 at CFRunLoopAddSource+132 (NULL source) from its main+0x2af138 in a dispatch_once. **backboardd** sig 6: abort <- std::terminate <- _objc_terminate <- dispatch_once <- main+0x9334 = the "start %{public}@..." wrapper (0x100009264) for **"window server"** (block 0x10000a770 -> dispatch_once body 0x10001d7f0 = StartWindowServer): MTLSetShaderCachePath, `[CAWindowServer serverWithOptions:@{kCAWindowServerDisableUpdatesOnMainDisplay: @YES}]`, `setRendererFlags:`, **`mainDisplay`** -> `displayId` -> `[CADisplay displayWithDisplayId:]`, setWindowServerDisplay:, bounds, pointScale, currentMode, preferredScale, nativeOrientation, touchScales... With no display, mainDisplay is nil and it throws. (HID system and GS start fine before it.) SpringBoard dies on the same missing display (FBSDisplayMonitor). QuartzCore display backends (from the cache's .symbols subcache, `ipsw dyld str`): `CA::WindowServer::AppleCLCDDisplay` (IOMobileFramebuffer hardware) and **`CA::WindowServer::VirtualDisplay`** (open(), new_server, clone_update). `CA_DISPLAY_TYPE` and `CA_FORCE_LOCAL_SERVER` strings exist in QuartzCore but have no direct xref (likely read via a table of CA_* env names). NEXT: find how CA::WindowServer::Server enumerates displays (IOMobileFramebuffer services vs VirtualDisplay / CA_DISPLAY_TYPE) and choose: (a) kernel IOMobileFramebuffer service + IOSurface, or (b) a VirtualDisplay rendered by CA's CPU path (CA::OGL::SW) and copied to the framebuffer.
- **watchdogd + CommCenter + identityservicesd fixed (boot 0045n on v6, 20:41:30 UTC, frames VID_20261016_224133 00:39-01:13, slide 0x20fec000).** `SWEET WDT: watchdogd sel 0 in 0/0 out 1/0 -> ok`, no watchdogd/CommCenter/identityservicesd crashes (CommCenter/identityservicesd now call AKS 0/17/7). usermanagerd boot task runs (UMD_INIT creates primary session uid 501 "mobile", personas 99 System and 199 Default registered with the kernel); `Failed to atomically swap writing file /private/var//keybags/usersession.kb: Operation not supported (45)` (HFS lacks the atomic swap rename; non-fatal so far). Remaining: SpringBoard (FBSDisplayMonitor, no display), backboardd (uncaught exception), mediaserverd, lockdownd (crash inside obfuscated code, main+0x4c1b4?), logd. **logd**: main+0x18160 = `BUG IN LIBTRACE: Failed to migrate directory` from 0x10001801c(dir 0): 0x100016538 requires the dir to exist with uid/gid/mode from the table at 0x10002df14 (0: /private/var/db/diagnostics 272:272 0751, 1: /private/var/db/uuidtext 272:272 0755, 2: /private/var/db/diagnostics/timesync 272:272 0755) plus a `.migrated` file, else XPC "operation 6" to a helper and crash. **Image v7** = v6 + those dirs/markers (also in mkiosroot.sh), SHA256 in `ios16-root-20H392-v7.img.sha256`, `.simg` ready. Boot with `shim/out/shim-boot-0045n.img`.
- **Exit-reason text works (boot 0044n, frames VID_20261016_221726 00:32-00:53, slide 0x22d5c000).** CommCenter/identityservicesd: `couldn't dlopen libobjc-trampolines.dylib: dlopen(/usr/lib/libobjc-trampolines.dylib, 0x0106): tried: '/usr/lib/libobjc-trampolines.dylib' (no such file, not in dyld cache)`: the file only exists in the OS cryptex and dyld does not search cryptexes at ignition_level=1. 19 cryptex files have no root counterpart (OS: DriverKit dyld cache + .symbols, VisualTestKit, libffi-trampolines, libobjc-trampolines, libstdc++ 6.0.9 + 2 symlinks; App: SafariSupport.bundle, AuthenticationServicesAgent, com.apple.Safari.History, passwordbreachd, safarifetcherd, webbookmarksd, webinspectord, webpushd) -> hard-linked (symlinks copied: Linux hfsplus cannot hard-link symlinks); `tools/mkiosroot.sh` does it generically. watchdogd still crashed: the `-wdt_no_pmu` / `/arm-io/wdt no-pmu` check (0x10000a01c) only runs when os_variant_has_internal_diagnostics; otherwise 0x100009b18 looks up IOServiceMatching("IOService") + IOPropertyMatch {IOPMUPrimary: 1} (NSConstantDictionary) and crashes if absent, then IORegistryEntrySetCFProperty(boot stage 0xff) (failure only logged). Patch **0045**: IOWatchdog sets IOPMUPrimary = 1 and accepts setProperties; CI `38961814194` -> `xnu/out-0045`. **Image v6** = v5 + the 19 cryptex links, SHA256 7c610c6a...297a, `ios16-root-20H392-v6.simg` (full flash needed). Boot image `shim/out/shim-boot-0045n.img` (0045 + sweet-0044hi.adt). Still open: SpringBoard (no display), backboardd (uncaught exception, reason none), mediaserverd (Fig NULL dict), logd (main+0x18160), lockdownd (main+0x4c1b4).
- **System survives daemon crashes (boot 0043p, video frames VID_20261016_215006 00:25/00:42/00:54).** With the v5 plist patch no panic: launchd keeps launching (runningboardd, powerd, tccd, healthd, peopled, cryptexd, routined, aksd, mediaremoted, familynotificationd, nanoprefsyncd, navd, cloudpaird, passd, amsaccountsd, accessoryupdaterd, wcd ...). Shared cache slide that boot 0x406b4000 (`research/crash-0043p.txt`). watchdogd still crashed at main+0xb82c from the PMU step: `-wdt_no_pmu` is only honored when os_variant_has_internal_diagnostics; customer builds read `no-pmu` from IODeviceTree:/arm-io/wdt (IORegistryEntrySearchCFProperty) -> mkadt adds /arm-io/wdt {no-pmu}. **cryptexd** sig 6: abort_with_payload <- os_crash_fmt <- `__darwin_arm_secure_boot_chip.cold.7` ("panic: failed to get crypto algorithm") <- `_img4_chip_select_personalized_ap`: reads /chosen use-ddi-secure-boot, allow-ecid-mismatch, crypto-hash-method ("sha1" | "sha2-384"), /chosen/asmb lp-smb0/1 -> mkadt adds crypto-hash-method "sha2-384". **identityservicesd** sig 6 = same `_objc_fatal` in imp_implementationWithBlock as CommCenter (block trampolines; message needed). lockdownd sig 5 at main+0x4c1b4. Patch **0044**: crash report adds `CRASH name reason ns N code X: text` (exit reason description = abort_with_payload text). CI `38959096811` -> `xnu/out-0044`, QEMU OK. Image `shim/out/shim-boot-0044n.img` = 0044 + `shim/out/sweet-0044hi.adt` (wdt no-pmu, crypto-hash-method) + cmdline (wdt=-1, no -wdt_no_pmu).
- **First symbolicated daemon crashes (boot 0043u on journaled v4, 19:38:53 UTC; video frame chat image 36; transcribed in `research/crash-0043u.txt`; shared cache slide 0x40d60000 for every process).** (Gemini's video transcription invented values, use frames only.) `tools/sweet-crash.py research/dsc/dyld_shared_cache_arm64 x research/crash-0043u.txt`:
  - **watchdogd** pc main+0xb82c (`_os_crash` trap helper) from 0x100009bb0 = set PMU boot stage via IORegistry (no PMU) -> fix: boot-args `wdt=-1 -wdt_no_pmu` (Apple's own test mode writes "%s wdt=-1 -wdt_no_pmu"; watchdogd logs "skipping setting PMU boot stage because -wdt_no_pmu found in boot-args or no-pmu found in EDT"). Kernel effect of wdt=-1: sched hygiene debug off, zone_exhausted_timeout -1 (harmless).
  - **SpringBoard** sig 5 in `-[FBSDisplayMonitor _initWithBookendObserver:transformer:]+1160` <- `+[FBDisplayManager sharedInstance]` <- `FBSystemShellInitialize` <- `SBSystemAppMain`: no main display. GUI WALL: display service (IOMobileFramebuffer / CoreAnimation display).
  - **backboardd** sig 6: abort <- std::terminate <- _objc_terminate (uncaught exception; message not shown).
  - **CommCenter** sig 6: `_objc_fatal` in `imp_implementationWithBlock` (from `+[_PFRoutines initialize]`, CoreData): objc block trampolines cannot be allocated -> likely kernel gap (trampoline page remap); will hit many processes.
  - **mediaserverd** sig 11: CFDictionaryGetValue(NULL) in FigNotificationListenerStartNotifications (routing session manager).
  - **logd** sig 5 at its main+0x18160 (bt main+0x9dfc); lockdownd sig 5 (not yet symbolicated).
  - Image v5 = v4 + launchd.plist (`/System/Library/xpc/launchd.plist`, contiguous at image offset 0x1e025f000, 1456980 bytes) with the 7 `_PanicOnCrash` keys renamed `_PanicOnCrasX` in place (raw image patch, no metadata change) -> `diff-v4-v5.simg` = 7 blocks; v5 SHA256 78f96974...185a. Boot image `shim/out/shim-boot-0043p.img` = 0043 + `wdt=-1 -wdt_no_pmu`.
- **0043 console crash reports (QEMU verified); image `shim/out/shim-boot-0043u.img`; NEXT: flash v4 + boot 0043u.** Boot 0042u (video photo IMG_20261016_211646): short exec lines work; watchdogd crashes with NO `SWEET WDT:` line (fails before its first IOWatchdog call: lookup or IOServiceOpen, or elsewhere); lockdownd, SpringBoard, mediaserverd, CommCenter, identityservicesd, backboardd, watchdogd all crash repeatedly ("Too many corpses being created" = corpse limit, no crash reports). AKS callers: lockdownd (0, 7), imagent/sleepd (0, 17, 7), backboardd (0, 17), UserEventAgent (0, 7, 102, 103 -> unsupported). Patch **0043**: `sweet_crash_log` (osfmk/kern/exception.c, called from proc_prepareexit for crash exits on the current proc) prints `CRASH name[pid] sig S code (exc<<32|code) sub X slide SR mh MH`, `CRASH name pc lr sp fp far`, `CRASH name bt` (10 fp frames); AKS sel 103 notify_state_is_set -> 0. CI `38954520419` -> `xnu/out-0043`. QEMU test: init forks, child `brk #1`: `CRASH launchd[2] sig 5 code 0x600000001 sub 0x1000004f0 slide 0x0`, pc correct, parent keeps running (pid 1 crashes panic before proc_prepareexit, so test with a child). `tools/sweet-crash.py CACHE ROOT [report]` symbolicates typed-in CRASH lines (`ipsw dyld a2s` with addr - slide; main binary offsets = addr - mh + 0x100000000).
- **v3 reflashed 18:50-18:55 UTC (`research/flash-userdata-v3b.log`, 24/24 OKAY); 0042u booted 19:01:17 UTC on the clean disk (`research/boot0042u-2.log`), SLOW MO VID pending (watchdogd failure reason).** ABL hangs on the first `fastboot boot` after every big flash; user re-enters fastboot. User gave standing approval to reflash/boot without asking (memory standing-flash-approval). Commits now end with `Co-Authored-By: dipa-studio <183933817+dipa-studio@users.noreply.github.com>` (user request; never a Claude trailer).
  - watchdogd flow: connect_to_kext (0x100005408): IOServiceGetMatchingService("IOWatchdog") fails -> "failed to discover" + _os_crash; IOServiceOpen(type 1) fails -> "IOServiceOpen failed with error : 0x%x" + _os_crash. Then sel 0 (out 1 scalar): 0 -> "userspace monitoring disabled", 0x100009bb0, sleep 2, exit(0); nonzero -> "userspace monitoring enabled", normal operation.
  - **Image v4 (JOURNALED):** `tools/mkiosroot.sh` now does `mkfs.hfsplus -J`, mounts with `-o loop,force`, adds the /System/Library/Caches/com.apple.dyld hard links (44) and /private/var/keybags/systembag.kb (0700/0600 root). Built from the IPSW dmgs (OS 087-86698-044, SystemOS 087-86593-047, AppOS 087-86715-047) -> `/run/media/dipa/android/ios-sweet-ipsw/ios16-root-20H392-v4.img` SHA256 fba4aeae...ec0f9 + `.simg`. Header: attr 0x80002100 (unmounted, journaled), journal info block 129, journal 8 MiB at 532480 flagged need-init. Linux `fsck.hfsplus -n` reports "Volume header needs minor repair" on any fresh journaled volume even after -y (quirk). QEMU: 64 MB journaled test root (`rd=md0 rootrw=1`, 0042 kernel): `hfs: Initializing the journal (joffset 0x3000 sz 0x80000)` -> mounted -> init runs, no panic. NEXT: flash v4 (full), boot 0042u.
- **SPRINGBOARD STARTS (boot 0041u 18:39:53 UTC, photo IMG_20261016_204142).** backboardd, bluetoothd, fairplayd.H2, SpringBoard (pid 111, crashed: "Too many corpses"), xpcproxy run; `SWEET AKS: backboardd sel 0 -> 0`, `sel 17 in 1/0 out 0/4096 -> 0`. Panic: watchdogd[113] exited, namespace 2 subcode 5 (SIGTRAP = its _os_crash): watchdogd crashes on purpose when IOServiceGetMatchingService("IOWatchdog") fails ("connect_to_kext() unexpectedly failed") or IOServiceOpen(type 1) fails ("IOServiceOpen failed with error"); IOWatchdog DOES register (QEMU msgbuf: `SWEET WDT: IOWatchdog registered`), cause not on screen yet -> need video. Note `wdt_test=no_user_checkin` makes watchdogd log "detected testing boot-args to not run, exiting" (path not reached here?). **HFS CORRUPTION:** `hfs_swap_BTNode: offset #0 invalid (0xBC1D) (blockSize 0x2000 numRecords 81)`, `node=19677 fileID=4` (catalog), `hfs_mark_inconsistent` repeated. Likely cause: non-journaled HFS + unclean panics while writing (0038e wrote a 2.7 GB core then panicked; every boot since ends in panic). Plan: reflash v3 (clean), longer term HFS journaling. Patch **0042**: one short `exec NAME` console line instead of the 3-line "Allowing spawn of iOS binary" notice; CI `38949451276` -> `xnu/out-0042`; image `shim/out/shim-boot-0042u.img` (same cmdline as 0041u).
- **LAUNCHD DAEMONS RUN (boot 0039u, 18:29:13 UTC, photo IMG_20261016_203052).** With `launchd_unsecure_cache=1` all boot tasks pass and launchd starts LaunchDaemons (pids > 100: identityservicesd 114 and lockdownd 106 crash, "Too many corpses being created"). Fatal: `watchdogd: ERROR: failed to discover watchdog KEXT service` -> exit (namespace 2 = signal, subcode 5) -> `_PanicOnCrash` panic. watchdogd: IOServiceMatching("IOWatchdog"), IOServiceOpen type 1, IOConnectCallScalarMethod sel 0/1 (check in)/3/4 (disable/enable userspace monitoring)/5, IOConnectCallStructMethod sel 2 (1 KiB in) and 9 (small out); VM path ("detected virtual machine environment and no watchdog KEXT found, exiting...") only when it detects a VM. Console also flooded with `hfs: set hardlink bit on vol=iOSRoot cnid=321318` (Linux-made hard links lack kHFSHasLinkChainBit; repaired only in memory on each lookup). Patches **0040** (`iokit/Kernel/arm/SweetWatchdog.cpp`: IOWatchdog + IOWatchdogUserClient, all selectors succeed with zeroed outputs, each logged once as `SWEET WDT: ...`) and **0041** (sweet_hfs hfs_catalog.c: the three link-repair printfs limited to 8 per boot); CI `38948245372` PASSED -> `xnu/out-0041` (QEMU regression OK). Image `shim/out/shim-boot-0041u.img` (0041 + sweet-0038hi.adt + ufsrw rootrw + launchd_unsecure_cache=1) booted 18:39:53 UTC (`research/boot0041u.log`), photo pending.
- **v3 FLASHED + keybag passes; next wall AMFI (launchd_cache_loader).** User said "flash, nothing to lose": `fastboot -S 512M flash userdata ios16-root-20H392-v3.simg` 17:51:52-17:57:01 UTC, 24/24 chunks OKAY (`research/flash-userdata-v3.log`). userdata = v3 (SHA256 fa84274c...22df) until the next rw boot. ABL fastboot then hung ("Sending" forever, `reboot bootloader` timed out) until the user re-entered fastboot by hand (common Xiaomi ABL crash). Boot 0038e 18:06:15 UTC (`research/boot0038e-3.log`), photos IMG_20261016_200812/200838: keybag OK, then boot tasks tzinit (`Bad zoneinfo link`, resets to system partition, OK), finish-restore, finish-demo-restore, sysstatuscheck, prng_seedctl (`failed to load kernel prng seed`, `failed to load virtual random (-147)`, AllowCrash), auearlyboot, then **launchd_cache_loader: `abort: Unable to find AMFI`** -> 2.7 GB core written to /cores in 91 s -> launchd panic `launchd_cache_loader[22] exited -- exit reason namespace 7 subcode 0x1`. launchd_cache_loader: SecStaticCodeCheckValidity of /System/Library/xpc/launchd.plist with detached .sig, then IOServiceMatching("AppleMobileFileIntegrity") + IOServiceOpen type 0 + IOConnectCallMethod sel 6 (1 scalar = 1, struct in = cdhash) = trust the cdhash, then sends the cache to launchd. Boot-arg `launchd_unsecure_cache=1` skips signature + AMFI ("Using unsecure cache"); the value is parsed with strtoul base 10 (helper 0x100002da8), so a path = 0 = off: boot 0039w (18:17:13 UTC, `=/System/Library/xpc/launchd.plist`, photo IMG_20261016_202008) still aborted `Unable to find AMFI` (core dump now skipped: `Failed to generate core file ... error 14`, so 0039 works). Image `shim/out/shim-boot-0039u.img` uses `launchd_unsecure_cache=1`. Patch **0039**: do_coredump = 0 on SWEET (kern.coredump sysctl still works); CI `38945267341` -> `xnu/out-0039` (_do_coredump now in __common). Image `shim/out/shim-boot-0039w.img` = 0039 + sweet-0038hi.adt + ufsrw rootrw + `launchd_unsecure_cache=/System/Library/xpc/launchd.plist`. TODO later: real AppleMobileFileIntegrity IOService (sel 6 trust cdhash etc.).
- **Image v3 ready, awaiting user "flash".** Boot 0038w 17:38:10 UTC, photo (chat image 30): `SWEET AKS: keybagd sel 0 -> 0x0`, `sel 14 out 1/0 -> 0x0` (x2), `MKB_INIT: mount is with NO CPROTECT ... does not support Data Protection`, `System Keybag loaded`; then keybagd still opens /private/var/keybags/systembag.kb (0x100012d3c, for BackupKeyBagKeys) -> ENOENT, `no-effaceable-storage` not set -> `Can't load the keybag. tears in rain... Time to die` -> `FATAL KEYBAG ERROR: kb_load` -> reboot -> Halt/Restart Timed Out panic. Fix: ADT `/defaults/no-effaceable-storage` (mkadt.py; `shim/out/sweet-0038hi.adt`, `research/q0038-hi.adt`, QEMU OK) + file `/private/var/keybags/systembag.kb` (0600 root, dir 0700; XML plist {KeyBagKeys: data}). **v3 image** `/run/media/dipa/android/ios-sweet-ipsw/ios16-root-20H392-v3.img` = v2 + that file, fsck OK, SHA256 fa84274c...22df; `ios16-root-20H392-v3.simg` (img2simg: zero blocks become FILL chunks, so a full flash overwrites every block the rw boots changed). Flash: `fastboot -S 512M flash userdata ios16-root-20H392-v3.simg`, then boot `shim/out/shim-boot-0038e.img` (0038 + sweet-0038hi.adt + ufsrw=1 rootrw=1).
- **0038 AppleKeyStore (CI 38938744130 PASSED -> `xnu/out-0038`; QEMU msgbuf: `SWEET AKS: AppleKeyStore registered (no passcode, unlocked)`; phone image `shim/out/shim-boot-0038w.img` booted 17:38:10 UTC, `research/boot0038w.log`, photo pending).** QEMU full kernel log trick (fb.txt is only the last screen): `llvm-nm KERNEL | grep _msgbufp` -> `--hmp "x /1gx ADDR"` gives struct msgbuf address (static, 0xfffffe0007b56678 on 0038) -> `--hmp "x /3gx STRUCT"` gives bufc (3rd quad) -> `--hmp "memsave BUFC 0x20000 \"FILE\""` (quote the file name), `tr -d "\0" < FILE`. Same inputs give the same addresses. Boot 0037w 17:08:35 UTC, photo IMG_20261016_191052: bindfs fixed mount; mount-phase-1/2 pass; `cc_fips_test` boot task: all corecrypto FIPS POST tests PASSED in userspace; then keybag boot task `keybagd --init`: `MKB_INIT: FATAL KEYBAG ERROR: failed to load system bag`, `Could not get options entry from the device tree` (x2, IODeviceTree:/options = NVRAM, non-fatal), `REBOOTING INTO RECOVERY MODE` -> reboot -> panic `Halt/Restart Timed Out @IOPlatformExpert.cpp:884` (no restart path on SWEET).
  - keybagd (static libaks): load_devicebag (0x100012c30) calls aks_get_system (sel 14, 0-1 scalar in, 1 out) and loads /private/var/keybags/systembag.kb only if the result is 0xe00002f0 (NotFound); any other error -> fatal; SUCCESS -> skip the file (userspace-reboot path). systembag.kb is a plist {KeyBagKeys}; decrypted with effaceable storage unless IODeviceTree:/defaults has `no-effaceable-storage`. Image has no /private/var/keybags. has_data_protection: statfs of /private/var; no cprotect on our HFS -> device-bag volume mapping skipped.
  - libaks connection: IORegistryEntryFromPath / IOServiceMatching("AppleKeyStore"), IOServiceOpen type 0, then selector 0 (no args) must succeed.
  - **AppleKeyStore selector map (iOS 16.7 AppleKeyStore.framework, via `ipsw dyld disass`):** 0 open, 2 create_bag, 3 save_bag, 4 unload_bag, 5 set_system_with_passcode, 6 load_bag, 7 get_lock_state (1 in, 1 out; bit 2 = unlocked since boot), 9 unlock_device, 10 wrap_key, 11 unwrap_key, 12 unlock_bag/test_stashed_kek, 13 lock_bag, 14 get_system, 15 change_secret_se, 16 internal_state, 17 get_device_state (1 in, struct out <=4K: DER SET of SEQUENCE{UTF8 key, INTEGER}: ss state, sls lock_state (MKBGetDeviceLockState returns it), sb backoff, sbo, sfa failed attempts, sfao, sgs generation, sas assertions, sgpe grace, srcd countdown, sr reserved), 18 recover_with_escrow_bag, 19 obliterate_class_d, 20 drain_backup_keys, 21/22 set/clear_backup_bag, 23 get_bag_uuid, 24 rewrap_key_for_backup, 26/27 assert_hold/drop, 30/31 set/get_configuration, 33-37 stash, 38 auth_token_create, 40 generation, 41 fdr_hmac_data, 42 verify_password, 43 _aks_operation, 44 remote_session, 49/50 signing keys, 51/53/54 stash enable/persist/escrow, 55 unload_session_bags, 57/58 remote peer, 59-62 smartcard, 63 migrate_s_key, 65 set_keybag_for_volume_with_cookie_persona, 66/67 sync bags, 68 copy_volume_cookie_persona, 69-73 se_*, 74-79/84/85/99 fv_* (FileVault/VEK), 80/81 absinthe, 82 make_public_backup_bag, 83 drop_auxiliary_auth, 86 kext get/set options, 87 invalidate_bag, 88-91/94/95/106/107 se_*, 92 lower_iteration_count, 93 get_internal_info, 96-98/117 keybag_persona create/list/delete, 100/101 assert promote/consume, 102/103 notify_state set/is_set, 104 fairplay, 105 change_secret_epilogue, 108 delete_xart_leak, 110-113 backup volume, 114 sealed_hashes_set, 115 prewarm_sps, 118-128 AKSIdentity*/AKSGetLastUser.
  - Patch **0038**: `iokit/Kernel/arm/SweetKeyStore.cpp` (class AppleKeyStore on IOResources, IOUserClientClass AppleKeyStoreUserClient; personality in KernelConfigTables.cpp): 0/4/5/9/12/13/26/27/55/102 success, 14 -> handle 0, 7 -> 0x4, 17 -> DER state (ss 4, sls 0), others kIOReturnUnsupported; logs `SWEET AKS: <proc> sel N arg0 in a/b out c/d -> ret` for every call.
  - Tools: shared cache copied to `research/dsc/` (git-ignored; ipsw needs a writable dir for its .a2s symbol cache); `ipsw dyld disass research/dsc/dyld_shared_cache_arm64 --image AppleKeyStore|MobileKeyBag`; scratchpad `annot2.py BIN DIS FIXUPS` annotates llvm-objdump output with strings and import names (`ipsw macho info --fixups`).
- **0037 bindfs (phone boot 0037w pending photo).** Boot 0036mw 16:57:47 UTC, photo IMG_20261016_190102: WRITABLE ROOT WORKS: `/dev/ufsroot0 on / (hfs, local, noatime)` after mount_hfs update (GetMasterBlock EBUSY harmless), coredump of mount written to /cores (rw). Then `mount: failed to mount /private/preboot/5CBE3601.../usr/standalone/firmware -> ...: Operation not supported by device (19)` -> exit 74 -> launchd panic `mount[2] exited`. Cause: the macOS-derived config had nullfs but no bindfs (ENODEV = unknown fs type). Caller passes env 0 -> bind not required, ENOENT (missing source dir) tolerated. Patch **0037** adds `bindfs` to FILESYS_BASE in MASTER.arm64.sweet; CI `38936550483` -> `xnu/out-0037` (bindfs_vfsops present; QEMU probe regression OK). Image `shim/out/shim-boot-0037w.img` (0037 + sweet-0036hi.adt + ufsrw=1 rootrw=1) booted 17:08:35 UTC (`research/boot0037w.log`). NOTE: userdata has now been modified by rw boots; it no longer equals the v2 image, so sparsediff with v2 as OLD is not safe any more (full reflash or a new baseline needed).
- **0036 phone: launchd boot tasks run; mount -P 1 needs /chosen/boot-manifest-hash.** Boot 0036hr 16:15:32 UTC, photo IMG_20261016_185236: platform check passes; fsck boot task runs (`fsck: not booting main or diagnostic OS. Skipping fsck on OS container`); mount-phase-1: `failed to get filesystem info` x2 (no IODeviceTree:/filesystems/fstab, non-fatal), `mount: data volume missing, but not required in env: 0`, mount_hfs update of /: `GetMasterBlock: Error 16 opening /dev/ufsroot0` (EBUSY, mounted block dev, non-fatal) then mount() EROFS (read-only boot) -> `mount: / failed with 1`; then `failed to get boot-manifest-hash` -> errx exit 66 -> launchd panic `mount[3] exited`. /sbin/mount: fn 0x100005f90 reads IODeviceTree:/chosen boot-manifest-hash (hex), bindfs-mounts /private/preboot/<HEX>/usr/standalone/firmware (ENOENT tolerated unless required). `tools/mkadt.py` now adds `boot-manifest-hash` = SHA-384("sweet boot manifest") (5CBE3601...5205). ADTs `shim/out/sweet-0036hi.adt`, `research/q0036-hi.adt` (QEMU probe OK, csops 0x26000001). Image `shim/out/shim-boot-0036mw.img` (0036 + new ADT + ufsrw=1 rootrw=1), booted 16:57:47 UTC (`research/boot0036mw.log`): FIRST WRITABLE BOOT of userdata (restore source: v2 image on SSD).
- **0036 CS_SIGNED (phone boot 0036hr pending photo).** Phone boot 0035hr 2026-10-16 15:57:02 UTC (`research/boot0035hr.log`, photo IMG_20261016_180249): highmem works on hardware; shared cache maps (trust-cache warnings only), Apple launchd runs (`Darwin Bootstrapper Version 7.0.0`, `entering ondemand mode`), starts boot task fsck, then userspace panic `boot task is not a platform binary (fsck, /sbin/fsck)`. launchd+0x3fea0: `csops(pid, CS_OPS_STATUS)` must have bit 0x04000000. Cause: nothing set CS_SIGNED (AMFI does it in vnode_check_signature), so csproc_get_blob() returns NULL for every process and csops never reports CS_PLATFORM_BINARY (also breaks per-process cdhash/entitlement queries). QEMU probe init (`csops` status printer, scratchpad csq/) on 0035: `0x02000001`; patch **0036** (`*cs_flags |= CS_SIGNED` in the SWEET AMFI hook), CI `38929635791` -> `xnu/out-0036`: `0x26000001`. Images `shim/out/shim-boot-0036hr.img` / `-0036hw.img` (same cmdlines as 0035). 0036hr booted 16:15:32 UTC (`research/boot0036hr.log`). Next launchd boot tasks (embedded __config plist): fsck -qL and mount -P 1/2 (RequireSuccess); no /etc/fstab on the image.
- **0035 ENTITLEMENTS + PLATFORM BINARIES (QEMU verified).** `xnu/patches/0035-sweet-amfi-entitlements-platform-binaries.patch`: new `iokit/Kernel/arm/SweetEntitlements.cpp` (OSObject subclass: OSDictionary from OSUnserializeXML of the signature's XML entitlements + original XML for CS_OPS_ENTITLEMENTS_BLOB); `bsd/kern/sweet_amfi.c` answers queryEntitlementBoolean/String(+WithProc, string also matches array members), copyEntitlementAsOSObject(+WithProc, retained), asdict (retained), get_xml (kalloc_data copy); built-in MAC policy "SWEET AMFI" (registered from bsd_init before vfs/exec) with mpo_vnode_check_signature: every signed binary on the MNT_ROOTFS mount gets CS_PLATFORM_BINARY (stand-in for AMFI's static trust cache: iOS system binaries are ad-hoc, cd_flags 0x2, platform byte 0), and attaches entitlements ONLY for the writable copy (cs_blob != csb_ro_addr; revalidation passes the RO blob). LESSON: first version attached in adjustContextWithoutMonitor -> QEMU `unable to adjust OSEntitlements context without monitor: 53` because ubc_cs_convert_to_multilevel_hash (4K-page signatures on 16K kernel) sets csb_entitlements_blob = NULL; ALL iOS 16 binaries (launchd, backboardd, SpringBoard) use 4K code pages, so this would have broken launchd on the phone. Boot-arg `sweet_amfi_debug=1` logs `SWEET AMFI: <ident>: N entitlements (test: ...)`. `tools/mkinit.py OBJ OUT [ENTS.plist]` adds an XML entitlements blob (special slot -5; default output byte-identical). QEMU `research/q0035-ent` (RAM disk `research/root-hfs-ent.img`, init with 3 test entitlements, highmem): `org.sweet.init: 3 entitlements (test: bool 1 string 1 array 1)` then init PID 1. MAC policies loaded: 0034 = 1, 0035 = 2 (QMP `mac_policy_list`). CI `38909807230` -> `xnu/out-0035`. Phone images: `shim/out/shim-boot-0035hr.img` (highmem, ro), `-0035hw.img` (highmem, +ufsrw=1 rootrw=1), `-0035r.img` (low mem fallback).
- **PHONE TEST ORDER (updated):** 0035hr -> if instant reset, 0035r -> then 0035hw.

- **HIGHMEM: XNU in the upper DDR bank, ~3.95 GiB (was 1.2 GiB).** Live FDT memory = <0x80000000 2G> + <0x1_00000000 0xfcc00000>; all fixed reserved-memory carve-outs are in the low bank (highest: ramdump_fb 0xaf000000, rmtfs ~0xfde01000), dynamic pools have alloc-ranges <= 0xffffffff, UFS node has no `iommus` (Linux DMAs anywhere). `tools/mkadt.py --highmem` sets /chosen dram-base 0x100000000 dram-size 0xfcc00000; `tools/mkpayload.py` now takes phys_base/mem_size from the ADT (default payloads byte-identical, verified); `tools/qemu-sweet.py` sizes -m from the payload (8G for highmem). Patch **0034** (`0034-sweet-ufs-dma-above-4g.patch`): UFS refuses DMA memory above 4G without UFSHCI CAP.64AS (bit 24), logs `DMA at 0x...`. CI `38905889883` success -> `xnu/out-0034`. **QEMU verified** (`research/q0034-hi`, ADT `research/q0034-hi.adt` --qemu --highmem, HFS RAM disk): phys_base 0x100000000, init PID 1 heartbeat, no panic; `max_mem` = `mem_size` = 0xfcc00000 read via QMP (`research/q0034-hi-mem`). Phone images: `shim/out/shim-boot-0034hr.img` (read-only) / `-0034hw.img` (+ufsrw=1 rootrw=1), ADT `shim/out/sweet-0034hi.adt` (asmb SIP + sweet-ufs + highmem). PHONE RISK: unknown runtime TZ/XPU protection in the upper bank would show as an instant reset; fall back to 0033r/w. Watchdog ruled out for the overnight reset: msm-watchdog bark is 20 s (qcom,bark-time 0x4e20) yet panic screens stay up for many minutes.

- **Offline research 2026-10-16 (user away, phone unavailable). Images rebuilt** `shim/out/shim-boot-0033r.img` / `-0033w.img` now also carry `wdt_test=no_user_checkin`. NEXT ON PHONE: boot 0033r (expect past in6_nigroup), then 0033w (first writes: rw root fixes HFS link-chain bits itself and gives /var for launchd sockets /var/run/syslog, /var/run/lockdown.sock).
- **launchd.plist** (`/System/Library/xpc/launchd.plist`, compiled; 532 daemons, 66 RunAtLoad/KeepAlive, list in scratchpad `early-daemons.txt`). `_PanicOnCrash` daemons: containermanagerd/usermanagerd (InternalOnly), lockdownd/remoted/trustd/remotepairingdeviced (AutomationOnly), **watchdogd unconditional** (KeepAlive SuccessfulExit=false). watchdogd needs IOKit `IOWatchdog` kext service; if absent it exits only when `kern.hv_vmm_present` (ADT `/defaults/vmm-present`, hvg_sysctl.c) or with boot-arg `wdt_test=no_user_checkin` ("detected testing boot-args to not run, exiting"; reads kern.bootargs). Using the boot-arg (vmm-present would change many daemons; keep as an option).
- **GUI path research (QuartzCore in the shared cache, `ipsw dyld str CACHE STR`, ~10 s/search):** CA::WindowServer has IOMFBServer/IOMFBDisplay (Apple DCP/IOMobileFramebuffer), AppleCLCDDisplay, AppleRGBOutDisplay, **VirtualServer/VirtualDisplay** and **MemorySurface**, EAGLServer. Renderers: CA::OGL::MetalContext, GLContext and a **CPU software rasterizer CA::OGL::SW** (samplers/blend modes; "image is too large for software renderer"). Env knobs: `CA_ENABLE_OGL`, `CA_DISPLAY_TYPE`, `CA_FORCE_LOCAL_SERVER` (249 CA_* vars). Hypothesis (unverified): backboardd's render server could run a VirtualServer/MemorySurface with the SW renderer and we copy the surface to the ABL framebuffer -> needs disassembly of CA::WindowServer server creation. Expected other GUI walls: IOSurface (closed-source kext; needs a reimplementation), IOMobileFramebuffer user client, IOHID touch events, AppleKeyStore/keybag (SEP) for lock state, MobileGestalt answers (AppleCLCD etc.).

- **0032r RESULT (boot 05:07:50 UTC, `research/boot0032r.log`, photo IMG_20261016_071200): SHARED CACHE MAPPED, LAUNCHD RUNS.** With SIP relaxed (ADT asmb) the `not in trust cache` lines are only warnings for every subcache (.31 ... .42.dyldlinkedit); no SIP rejection; libSystem loads; launchd configures networking (`skip attaching fsw to lo0 using legacy TX model`) -> kernel data abort far 0x28 in `SHA1Init` (symbolized as sha1_init) from `in6_nigroup` (IPv6 on lo0): g_crypto_funcs NULL. Kernel addresses are unslid: `~/swift/usr/bin/llvm-symbolizer --obj=KERNEL ADDR` works directly.
- **0032** BearSSL SHA-1 (`0032-sweet-bearssl-sha1.patch`), **0033** BearSSL SHA-384/512 (`0033-sweet-bearssl-sha384-sha512.patch`, BearSSL ctx stored in SHA512_CTX, static-asserted to fit; ubc_subr.c uses SHA-384 for code directories). `0020-stage-bearssl.sh` now also stages hash/sha1.c, hash/sha2big.c, codec/{dec,enc}{32,64}be.c. CI `38854909053` success (0032-only run `38854802073` superseded). Kernel `xnu/out-0033` (SHA1Init -> _br_sha1_init verified by disassembly; nm symbols have a leading underscore). Images (ADT sweet-0032): `shim/out/shim-boot-0033r.img` (read-only), `shim-boot-0033w.img` (+ufsrw=1 rootrw=1). Still NULL g_crypto_funcs users: des, chacha20poly1305, rsa, aesxts, rand, flow_divert.
- `research/check-patch-order.py` now skips patches touching CI-staged sweet_hfs sources (0031) instead of stopping.

- **0031r RESULT (boot 05:03:52 UTC 2026-10-16, `research/boot0031r.log`, photo IMG_20261016_070508): SHARED CACHE FOUND.** dyld now opens `dyld_shared_cache_arm64` (v2 image hard links), kernel refuses to map it: `shared_region: ... map(...'dyld_shared_cache_arm64'): not in trust cache` -> `vnode is not SIP-protected` (vm_unix.c: macOS arm64 path needs trust cache, else root-owned + SF_RESTRICTED unless CSR_ALLOW_UNRESTRICTED_FS) -> `dyld cache '(null)' not loaded: syscall to map cache into shared region failed` -> libSystem not loaded -> coredump to /cores fails error 30 (EROFS) -> panic initproc failed -> "MACH Reboot" (spins; screen kept). Lots of `hfs: set hardlink bit on vol=iOSRoot cnid=... linkid=...` spam (Linux-made hard links lack the link-chain bit; read-only mount cannot persist the fix). FIX: ADT `/chosen/asmb` with `lp-sip0`=0x6f (u64; incl. CSR_ALLOW_UNRESTRICTED_FS) and `lp-sip1`=1 (unauthenticated root), `tools/mkadt.py` -> `shim/out/sweet-0032.adt`. Images with kernel 0031: `shim/out/shim-boot-0032r.img` (read-only) and `shim-boot-0032w.img` (+ufsrw=1 rootrw=1).

- **2026-10-16 morning:** userdata now = **v2** image (`diff-v1-v2.simg` flashed 21:06:50 UTC 2026-10-15, OKAY, `research/flash-userdata-diff-v2.log`); future diffs use `ios16-root-20H392-v2.img` as OLD. Booted `shim-boot-0029k.img` (ignition_level=1) right after (`research/boot0029k-v2.log`); **no photo** (user asleep). Next morning the phone was in PixelOS recovery = some hardware reset (ABL then booted Android, which cannot use userdata). OBSERVATION: our kernel cannot reset the SoC itself (`PE_halt_restart` is never set; reboot/halt print "MACH Reboot"/"CPU halted" and spin), so the reset came from outside XNU: Qualcomm watchdog or a TZ/XPU violation. Unknown which; re-boot 0029k and watch/video until the reset. Do NOT tap factory reset/format data in recovery (would wipe the iOS image).
- **0030** (`xnu/patches/0030-sweet-ufs-writes.patch`, CI `38852655542`): UFS WRITE(10) + read-modify-write for partial 4 KiB blocks + SYNCHRONIZE CACHE (DKIOCSYNCHRONIZE), only with boot-arg `ufsrw=1`; `ufs_write_blocks` re-checks partition bounds before every write (only place data goes out).
- **0031** (`xnu/patches/0031-sweet-read-write-root.patch`, CI `38852837176`): boot-arg `rootrw=1` clears MNT_RDONLY before the root mount (no data volume on sweet: /private/var is on the root; image has no /etc/fstab); Apple HFS accepts a dirty non-journaled ROOT read-write on SWEET (logs "mounting dirty non-journaled root read-write"). Later: journaled HFS for crash safety. Kernel `xnu/out-0031` (CI `38852837176` success, 5m31s; 0030 run cancelled by the newer push). Boot images (phone ADT `sweet-0028.adt`, ramdisk byte-matched): `shim/out/shim-boot-0031r.img` = 0029k args (read-only, ignition_level=1) and `shim/out/shim-boot-0031w.img` = same + `ufsrw=1 rootrw=1`. PLAN: boot 0031r first and film until the reset (diagnose overnight reset), then 0031w (first ever writes to userdata; v2 image on the SSD is the restore source). `research/check-patch-order.py` cannot check 0031 (staged sweet_hfs files are not in its index); CI stage scripts run first, so CI is the check.

- **ignition_level=1 result** (boot 21:01:39 UTC, `research/boot0029k.log`, photo IMG_20261015_230308): preboot/cryptex stages skipped (need >= 2/3), `dylib_cache: finding shared cache on system volume: ignition level = 1, search root fd = 4, subpath = System/Library/Caches/com.apple.dyld/` -> not found -> `ignition boot failed: 19` -> no shared cache -> libSystem not loaded. FIX IN PROGRESS: image v2 = copy of the flashed image + `/System/Library/Caches/com.apple.dyld/` with HARD LINKS to the 44 cryptex cache files (no extra space; avoids symlink-following doubts), then `tools/sparsediff.py ios16-root-20H392.img ios16-root-20H392-v2.img diff.simg` and flash only the diff. Boot with `ignition_level=1` (shim-boot-0029k.img).
- **ignition_level=0 result** (boot 20:58:07 UTC, `research/boot0029j.log`, photo IMG_20261015_230037): all stages skipped as "ignition level insufficient" (hello/preboot/cryptex1 sniff >= 2, select os cryptex path/graft >= 3, **dyld_cache >= 1**), `ignition boot failed: 89`, "ignition partially disabled", then same `Library not loaded: /usr/lib/libSystem.B.dylib (no such file, no dyld cache)`. => dyld gets the shared cache directory from ignition's dyld_cache stage. Next: `ignition_level=1` (`shim/out/shim-boot-0029k.img`) to skip preboot/cryptex stages but run dyld_cache; its log "finding shared cache on %s ... subpath = %s" will show where it searches. If it searches rootfs /System/Library/Caches/com.apple.dyld, add a symlink there (image rebuild; consider a diff-only sparse flash tool to avoid rewriting 12 GB).
- **APPLE dyld/launchd RUNNING (libignition) with `unentitled_ios_sim_launch=1`** (boot 20:53:24 UTC, `research/boot0029i.log`, photo IMG_20261015_225617): kernel prints "Allowing spawn of iOS binary ... boot-arg set to true"; `shared_region: ... check_np ... vm_shared_region_start_address() failed` (no region yet, normal); dyld's built-in **libignition** (Darwin Ignition Sequence 1.0.0, libignition-51~1974; prints rd=ufsroot0, hardware target sweet, ignition level 0x5, platform 0x2, boot spec local, 7 stages): `hello from launchd.1` -> `preboot: mounting preboot` -> `failed to lookup preboot: 25` (ENOTTY: looks for a separate Preboot volume by role) -> ignition fails -> dyld `Library not loaded: /usr/lib/libSystem.B.dylib ... (no such file, no dyld cache)` -> launchd exit -> panic initproc failed. dyld boot-args found via strings: `ignition_level`, `ignition_force_dylib_root` (forced rootfs/livefs/cryptex fallback), `ignition_halt_after`, `ignition_live_app_graft`; dyld cache search paths include /System/Cryptexes/OS, /System/Volumes/Preboot/Cryptexes/OS/, /private/preboot/Cryptexes/OS/ (present in our image). Next try `ignition_level=0` (`shim/out/shim-boot-0029j.img`).
- **APPLE launchd EXECUTED FROM UFS (2026-10-15).** Flashed iOS image: `img2simg` -> `ios16-root-20H392.simg` (12.1 GB) then `fastboot -S 512M flash userdata ...simg` (24 chunks, 307 s, `research/flash-userdata-ios16.log`; a raw-image flash was killed by earlyoom at 2.6 GB RSS - always flash the sparse image). ABL fastboot hung on getvar after the flash; power+vol-down fixed it. Boot 0029u 20:49:04 UTC (`research/boot0029u-ios16.log`), photo IMG_20261015_225134: `hfs: mounted iOSRoot` -> load_init_program /sbin/launchd -> panic `unexpected SIGKILL of launchd (CS_KILLED) with reason -- namespace 9 code 0xe` = OS_REASON_EXEC / EXEC_EXIT_REASON_WRONG_PLATFORM: our kernel is XNU_TARGET_OS_OSX, iOS-platform binaries are "alien" and refused unless spawn attr platform set or DEVELOPMENT TUNABLE `unentitled_ios_sim_launch`. Next try: same kernel, cmdline + `unentitled_ios_sim_launch=1` (`shim/out/shim-boot-0029i.img`). LONGER TERM: macOS-config kernel marks iOS maps alien (vm_map_mark_alien) and differs from iOS in many policies; an iPhoneOS-config kernel build may eventually be needed.
- **iOS 16 ROOT IMAGE BUILT:** `/run/media/dipa/android/ios-sweet-ipsw/ios16-root-20H392.img` (16 GiB case-sensitive HFS+, label iOSRoot, 12 GB used, fsck OK after `fsck.hfsplus -y` rewrote 32 non-ASCII localized WorkflowKit Shortcut names Linux decomposes differently). Built by `tools/mkiosroot.sh OS.dmg SYSTEMOS.dmg APPOS.dmg OUT.img [SIZE]` (needs sudo; apfs-fuse as root; OS dmg is UDIF -> link must end .dmg). Contents: iPhone X 20H392 OS root + SystemOS cryptex at /private/preboot/Cryptexes/OS (so /System/Cryptexes/OS -> cache, 44 files, dyld_v1 arm64) + AppOS at .../App. /sbin/launchd and /usr/lib/dyld root:wheel 0755. NEXT: (confirm) `fastboot flash userdata ios16-root-20H392.img` (auto-sparse), boot 0029u (rd=ufsroot0) -> Apple launchd attempt.
- **MILESTONE ON HARDWARE: BOOT FROM INTERNAL FLASH.** 0029 CI `38797924912` PASSED, `xnu/out-0029` SHA256 `a68bc847...2a37`. Image `shim/out/shim-boot-0029u.img` (payload-0029u = 0029 kernel + `shim/out/sweet-0028.adt` + `rd=ufsroot0`, NO RAM disk). Boots 20:01:34 and 20:12:37 UTC (`research/boot0029u*.log`), photo IMG_20261015_221403: `SWEET UFS: "userdata" on LUN 0: first block 0x2b8c00, 109643 MiB, read-only` -> `BSD root: ufsroot0, major 1, minor 0` -> `hfs: mounted SweetRoot on device b(1, 0)` -> `SWEET: root volume is not authenticated (error 25); continuing unsealed` -> `load_init_program ... /sbin/launchd` -> `*** SWEET userspace: init running as PID 1 ***` -> heartbeat (ran for minutes unattended). userdata currently holds the 8 MB `research/root-hfs.img`. NEXT: build a multi-GB HFS+ image with the iOS 16 (iPhone X 20H392) userland (OS rootfs + SystemOS cryptex contents incl. dyld shared cache at /System/Library/Caches/com.apple.dyld; preserve modes/owners/symlinks; decompress LZBITMAP via apfs-fuse), flash it to userdata (sparse, needs user OK), then try Apple's dyld/launchd. Driver is read-only (root mounts RDONLY), fine for first launchd attempts.
- **userdata FLASHED with the 8 MB HFS test volume** (user said "flash it", 2026-10-15 19:53:17 UTC, `research/flash-userdata-hfs-test.log`: `fastboot flash userdata research/root-hfs.img` OKAY; pre-checks: one sweet device, partition-size:userdata 0x1AC4BFB000, partition-type f2fs, image fsck OK, SHA256 3626ea69...). Android userdata is now destroyed (expected/approved). Boot 0028u at 19:53:20 (`research/boot0028u-hfs.log`), photo IMG_20261015_215541: `BSD root: ufsroot0` -> **`hfs: mounted SweetRoot on device b(1, 0)` from internal UFS** -> panic `rootvp not authenticated after mounting @bsd_init.c:931` (arm64 macOS-config requires FSIOC_KERNEL_ROOTAUTH sealed root for non-ramdisk roots). Patch **0029** (`dd1c284`): SWEET logs `SWEET: root volume is not authenticated (error N); continuing unsealed` instead of panicking (Apple configs keep the panic; pivot_root still requires auth). CI **`38797924912`** running. Next: download out-0029, phone image with sweet-0028.adt + rd=ufsroot0 (no RAM disk), boot -> expect init from UFS.
- **UFS ROOT DRIVER VERIFIED ON HARDWARE (read-only).** 0028 CI `38796139570` PASSED, `xnu/out-0028` SHA256 `8b981640...9da6`; QEMU HFS RAM-disk regression `research/q0028-hfs` still runs init. Phone boot 2026-10-15 19:50:24 UTC (`research/boot0028u.log`, image `shim/out/shim-boot-0028u.img`, phone ADT `shim/out/sweet-0028.adt` with /sweet-ufs, NO RAM disk, `rd=ufsroot0`), photos IMG_20261015_215151/215220: `SWEET UFS: "userdata" on LUN 0: first block 0x2b8c00, 109643 MiB, read-only` -> `BSD root: ufsroot0, major 1, minor 0` -> `hfs_ValidateHFSPlusVolumeHeader: unknown Volume Signature : f188` (Android-encrypted data, as expected) -> `cannot mount root, errno = 19`, kernel retries mountroot (no panic). Nothing written to flash. NEXT (destructive, needs user confirmation right before): `fastboot flash userdata research/root-hfs.img` (8 MB HFS with test init) then boot rd=ufsroot0 -> expect init from internal flash.
- **PATCH 0028 (`33aceac`): XNU read-only UFS root device**, CI **`38796139570`** running (next artifacts xnu/out-0028, research/bt29, run29.log). `bsd/dev/sweet_ufs.c`: reuses ABL's UFSHCI state (like the shim probe), takes over UTRL at a kmem_alloc_contig region (DMA layout: UTRL 0x0, UCD 0x1000, 1 MiB bounce 0x4000), verifies UTRLBA readback before any doorbell, IE=0 polled, one SCSI cmd at a time on LUN0 (TUR x3, READ(10) via bounce, PRDT 256 KiB chunks), flush_dcache64 by PA around each command (host not dma-coherent per DT), finds GPT partition by name (`ufsroot=` boot-arg, default `userdata`), exposes `/dev/ufsroot0`/`rufsroot0` (512-byte logical sectors, physical 4 KiB, DKIOC* ioctls), refuses writes (EROFS). IOKitBSDInit: `rd=ufsroot0` -> sweet_ufs_rootdev() (lazy init) else panic. ADT: `tools/mkadt.py` adds `/sweet-ufs` reg <0x1d84000 0x3000> for the PHONE only (not --qemu). Local compile OK, imports resolve. Sandbox research/bt28 (pre0028 originals). FIRST PHONE TEST PLAN (non-destructive): boot 0028 with phone ADT + `rd=ufsroot0` and NO RAM disk: expect `SWEET UFS: "userdata" on LUN 0: first block 0x2b8c00, 109643 MiB` + `BSD root: ufsroot0`, then root mount failure (userdata is still Android-encrypted) -> panic. That validates the driver before flashing an HFS image into userdata (needs explicit user confirmation at that time).
- **SUPERSEDES the 64 GiB iosroot GPT plan: user said "u can delete android" (2026-10-15).** Use the existing `userdata` partition (LUN0 sda34, 4K LBA 0x2B8C00, 0x1AC4B MiB ~107 GiB) directly as the iOS root: NO GPT changes. Keep bootloaders, recovery (OrangeFox), modem/persist/fsg/modemst untouched. Write the HFS image later with `fastboot flash userdata` (sparse, auto-split under the 768 MiB limit). Android will no longer boot normally (expected; reinstallable). Order: XNU UFS driver first, read-only against the current GPT; flash only once it can locate userdata. Nothing erased yet.
- **DECIDED (user, 2026-10-15): 64 GiB `iosroot` on LUN0 (sda), taken from userdata (wipes Android user data; Android/super kept).** Planned GPT (4K LBAs): userdata 0x2B8C00..0xD7D6FF (~43 GiB, keep type/unique GUIDs and attributes); iosroot 0xD7D700..0x1D7D6FF (0x1000000 blocks = 64 GiB, 1 MiB aligned); backup GPT area unchanged. Procedure: user backs up phone data -> boot OrangeFox recovery (adb) -> back up GPT (sgdisk --backup + raw dd of LBA 0-5 and last 6 blocks) to research/ -> sgdisk edit -> Format Data -> verify Android boots to setup. NOT EXECUTED YET.
- **INTERNAL UFS WORKS FROM THE SHIM (read-only), user authorized internal-UFS approach incl. flashing (2026-10-15, "i allow everything").** Shim probe `shim/ufs.c` (commit `12ed3ed` + filter): ufshc@1d84000 is UFSHCI 2.1 (ver 0x210, cap 0x1587031F, 32 slots), ABL leaves HCE=1, HCS=0x10F (device present, lists ready, UIC ready); probe takes over UTRL at its own buffer (verifies UTRLBA readback before any doorbell), TEST UNIT READY/READ CAPACITY/READ(10) all succeed. LUN0 = 0x1D7D800 x 4 KiB (~126.6 GB), valid GPT (64 entries at LBA 2). Photos IMG_20261015_212916/213325/213429. Big LUN0 partitions (start in 4K LBAs / MiB): super 0xCA00/0x2200 (8.5 GiB), logdump 0x22CA00/64, minidump 0x230A00/128, rawdump 0x238A00/128, recovery 0x240C00/128, cache 0x248C00/384 ext4, exaid 0x260C00/384, cust 0x278C00/1024 ext4, **userdata 0x2B8C00/0x1AC4B MiB (~107 GiB), fs unreadable = encrypted (metadata encryption)**. cache..cust contiguous = only 1.75 GiB (too small for iOS ~10+ GiB). Plan needing user consent: shrink userdata in GPT (wipes Android data) and add an `iosroot` partition at the end of LUN0. LUNs 1-5 not listed yet (sdb..sdf backed up in backup/lun).
- **fastboot limit measured (2026-10-15, read-only getvar):** `max-download-size: 805306368` (768 MiB); `max-fetch-size` unsupported; `is-userspace: no` (ABL fastboot). A `fastboot boot` image (shim+kernel+RAM disk) must be < 768 MiB, so the 2.5 GiB iOS shared cache cannot ride in the boot RAM disk even if XNU gets all RAM. Real userland needs storage access (UFS driver, or USB host) or a much smaller cache. Unlocking full RAM is still needed later (GUI).
- **iOS 16 USERLAND SOURCE (Claude, 2026-10-15).** Chose **iPhone X (iPhone10,3/10,6, D22/D221AP)** iOS **16.7.16 (20H392)** over iPhone 8: same A11 (no PAC -> plain arm64, runs on Kryo 470/ARMv8.2), last iOS for A11; X userland is home-button-less and tall-screen (1125x2436 vs sweet 1080x2400), better fit for later GUI. Note the X has its OWN SystemOS cryptex (different digest from iPhone 8's). IPSW URL in `research/ipsw-x/URL.txt` (+ BuildManifest). Images on the user's external SSD (NOT in git, do not format that disk: it holds the user's photos): `/run/media/dipa/android/ios-sweet-ipsw/iPhone10,3_16.7.16_20H392/` — `087-86593-047.dmg` = Cryptex1,SystemOS (2,743,074,816 B), `087-86698-044.dmg` = OS rootfs (4,730,814,044 B), both zip CRC-32 verified, plus Firmware/*.root_hash/.trustcache/.mtree. AppOS cryptex is 087-86715-047.dmg (10 MB, same as iPhone 8).
  - Images are raw APFS; files use LZBITMAP (decmpfs type 14). 7z cannot decode. Tools: `ipsw` 3.1.738 (AUR ipsw-bin, user-approved) needs apfs-fuse on Linux; **apfs-fuse** built in user space (system cmake broken: cmake 4.4.4 wants libjsoncpp.so.27, jsoncpp 1.9.6 installed = partial upgrade, NOT fixed) with pip cmake + `-DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_CXX_FLAGS="-include cstdint"`, installed to `~/.local/bin/apfs-fuse` (+apfs-dump-quick). Mount via a comma-free symlink (apfs-fuse splits options on ',' in `iPhone10,3`; exFAT has no symlinks -> put link elsewhere): `apfs-fuse LINK/087-86698-044.dmg MNT` -> `MNT/root/...`; unmount `fusermount3 -u MNT`.
  - Verified from headers: `dyld_shared_cache_arm64` magic `dyld_v1   arm64`; `/sbin/launchd`, `/usr/lib/dyld` = cputype 0x100000c subtype 0 (plain arm64). libSystem exists only inside the cache.
  - **BLOCKER for real userland via RAM disk:** shared cache = 3.10 GiB in 44 files (~2.5 GiB without the 592 MB `.symbols`), but the kernel manages only 1.20 GiB (0xb0000000..0xfd000000). Options: (A) give XNU the phone's full ~6 GB (banks 0x80000000+2G and 0x100000000+0xfcc00000, carve-out holes need pmap support) AND check fastboot max-download-size for a multi-GB boot image; (B) storage driver (Qualcomm UFS) to read a root from flash (no flashing allowed: would need a file/partition strategy); (C) run against individual dylibs extracted from the cache (uncertain).
- **MILESTONE ON HARDWARE: HFS+ ROOT FILESYSTEM -> PID 1.** Boot 2026-10-15 18:22:12 UTC (`research/boot0027b.log`, image `shim/out/shim-boot-0027b.img`), photo `IMG_20261015_202334.jpg`: `BSD root: md0` -> `hfs: mounted SweetRoot on device b(1, 0)` -> `load_init_program: attempting to load /sbin/launchd` -> `*** SWEET userspace: init running as PID 1 ***` -> repeated `SWEET init: still alive`, no panic. Kernel 0027 (built-in Apple OSS HFS + BearSSL MD5), init with SP alignment fix. NEXT: real iOS 16 userland on the HFS image (dyld, libSystem/dyld shared cache, launchd). Known gaps: entitlements (none granted), SHA-1/SHA-384/512 providers, no storage driver (RAM disk only), root mounted read-only.
- **PHONE HFS ROOT (0027) + SIGBUS ROOT-CAUSED.** Phone boot 2026-10-15 17:49:06 UTC (`research/boot0027.log`, photo `IMG_20261015_200031.jpg`): `hfs: mounted SweetRoot on device b(1, 0)` (HFS works on hardware), init printed its first line, then `pid 1 exited -- exit reason namespace 2 subcode 0xa` = SIGNAL/SIGBUS -> `initproc failed to start` panic. Diagnostic boot 18:08:13 UTC (`research/boot0027d.log`, image `shim/out/shim-boot-0027d.img`, diag init `research/diag-init` from scratch diag-init.S with [a][b][c] markers, `cs_debug=1`; photo `IMG_20261015_201547.jpg`): output `PID [a][b]` then SIGBUS -> fault at first `strb w21,[sp]`; no code-signing messages. Cause: kernel enters a static binary with sp at argc, not necessarily 16-aligned (depends on arg/apple string lengths, differs mockfs vs HFS); EL0 SP alignment check -> EXC_ARM_SP_ALIGN -> SIGBUS (sleh handle_sp_align). QEMU does not enforce SP alignment. NOT a kernel bug: real binaries start in dyld which aligns. Fix `1a8fc15`: sweet-init aligns sp at entry. Rebuilt `research/root-hfs.img`, QEMU `research/q0027-hfs2` OK. Phone image `shim/out/shim-boot-0027b.img` SHA256 `098ba46c...c5c` (payload-0027b, ramdisk match check-0027b) ready, awaiting FASTBOOT. Lesson: Assembly `;` is a comment in Apple arm64 asm (use .macro, not ;-joined #define).
- **HFS ROOT WORKS (QEMU).** 0027 CI `38779441895` PASSED, `xnu/out-0027` SHA256 `a8643886...5f6a`. QEMU `research/q0027-hfs` (RAM disk = `research/root-hfs.img`, rd=md0): `BSD root: md0` -> `hfs: mounted SweetRoot on device b(1, 0)` -> `load_init_program: attempting to load /sbin/launchd` -> `*** SWEET userspace: init running as PID 1 ***` + heartbeat, no panic. Phone image `shim/out/shim-boot-0027.img` (payload-0027 = 0027 kernel + sweet-0021.adt + root-hfs.img + rd=md0, ramdisk byte-match check-0027) ready; awaiting FASTBOOT for phone test.
- **HFS ROOT WORK (Claude), patches 0026-0027.**
  - 0026 (`f0e990c` + link fix `64c96f4`): Apple OSS **hfs-627.100.6** (commit `31d5ec238dfab485e78a9a30b8b92d13e8914f03`, paired with xnu-8796.141.3 in macos-135 release.json) staged by `xnu/patches/0026-stage-hfs.sh` into `bsd/sweet_hfs` (core + hfs_encodings; AppleKeyStore include/key prototypes guarded by CONFIG_PROTECT; SDK TargetConditionals copied). New config option `sweet_hfs` (MASTER + MASTER.arm64.sweet); 40 `optional sweet_hfs` files; Makefile.template foreach adds `_CFLAGS_ADD = -include sweet_hfs_compat.h -I...` (MUST come after kernel -D flags; compat undefs BSD/XNU_KERNEL_PRIVATE and the CONFIG_* HFS tests so kext-config.h decides: macOS profile, no content protection). `sweet_hfs_iokit.c` replaces hfs_iokit.cpp/hfs_encodings_kext.cpp: `sweet_hfs_register()` called in bsd_init after vfsinit (encodings init, vfs_fsadd VFS_TBLCANMOUNTROOT, zones, sysctls); is_ejectable 0, iterate_media none, serial KERN_NOT_SUPPORTED, OSKextGetCurrentLoadTag -> kOSKextInvalidLoadTag (renamed via compat macro). hfs_encodings' own `hfs_allocated` renamed per object (separate kexts on Apple; first CI `38777325964` failed on that duplicate at link). mockfs now validates Mach-O magic (its TODO) so non-Mach-O roots fall through to HFS (new fs entries come AFTER mockfs in vfsconf). CI `38778133720` PASSED, `xnu/out-0026` SHA256 `0b49170c...701a`, prints `SWEET HFS: hfs-627.100.6 registered`.
  - HFS+ image: `research/root-hfs.img` (8 MB, mkfs.hfsplus from AUR hfsprogs installed with user approval; loop-mounted with sudo; /sbin/launchd = research/sweet-init, /dev; fsck.hfsplus OK). QEMU `research/q0026-hfs`: `BSD root: md0` then data abort in MD5Init from hfs_getvoluuid (mount computes volume UUID).
  - 0027 (`14bf2b7`): 0020 stager also copies BearSSL hash/md5.c + codec/dec32le.c/enc32le.c; `corecrypto_md5.c` SWEET branch converts MD5_CTX <-> br_md5_context around br_md5_* calls; wrappers sweet_md5.c/sweet_dec32le.c/sweet_enc32le.c. Host test 40/40 match md5sum. CI **`38779441895`** running.
  - Local tooling: `tools/xnu-cc.py` has `XNU_CC_POST` (flags after logged ones). Sandbox `research/bt28/xnu` = 0025 CI tree + 0026/0027 edits; originals `research/pre0026`, `research/pre0027`. HFS local objects `research/hfs-objs`. Checker entries updated to bt28.
  - SHA-1 / SHA-384/512 still have no provider.
- **MILESTONE ON HARDWARE: USERSPACE PID 1 RUNS ON THE PHONE.** Boot 2026-10-15 17:16:00 UTC (`research/boot0025.log`: send OKAY0.410s, boot OKAY0.135s), image `shim/out/shim-boot-0025.img` SHA256 `9dbfa0b1...89db0`. Photo `IMG_20261015_191733.jpg`: `BSD root: md0` -> `load_init_program: attempting to load /sbin/launchd` -> `*** SWEET userspace: init running as PID 1 ***` -> repeated `SWEET init: still alive` (5 s heartbeat), no panic. Kernel 0025 + mockfs RAM disk + built-in no-entitlements AMFI. No flashing performed.
- **NEXT (toward real iOS userland):** real root filesystem with real binaries. Options: (a) build Apple OSS `hfs` (macOS 13.5 tag `hfs-627.100.6`) into SWEET like libpthread, then an HFS RAM disk image; (b) iOS 16 dyld + libSystem/dyld shared cache from the IPSW (needs APFS or repacking onto HFS). Expected hurdles: entitlements (sweet_amfi grants none), SHA-1/MD5/SHA-384/512 providers (BearSSL), missing kexts/daemons. Current init is a static test program, not launchd.
- **MILESTONE: USERSPACE RUNS (QEMU).** 0025 CI `38774788048` PASSED, `xnu/out-0025` SHA256 `5496069b...4877`. QEMU `research/q0025-rd` (150 s, no panic): `BSD root: md0` -> `load_init_program: attempting to load /sbin/launchd` -> `*** SWEET userspace: init running as PID 1 ***` -> `SWEET init: still alive` every 5 s. Exec, ad-hoc SHA-256 code signature, syscalls, devfs /dev/console, select timeouts all work. Phone image prepared: `shim/out/shim-boot-0025.img` 16,515,072 bytes SHA256 `9dbfa0b15140c0bd733a16c4f727f6a7df44a8741f98d8e266d5f2fa4f689db0` (payload-0025 = 0025 kernel + sweet-0021.adt + research/sweet-init + rd=md0; ramdisk byte-match check-0025). Waiting for user to put phone in FASTBOOT.
- **0024 CI `38773277811` PASSED**, `xnu/out-0024` SHA256 `a0ba964b...4348`. QEMU `research/q0024-rd` and **PHONE (boot 2026-10-15 17:05:55 UTC, `research/boot0024.log`, image `shim/out/shim-boot-0024.img` SHA256 `cf3dac86...6964`, 16,510,976 bytes, payload `shim/out/payload-0024.bin` = 0024 kernel + `shim/out/sweet-0021.adt` + RAM disk `research/sweet-init` + rd=md0; ramdisk byte-match `check-0024`)** agree: `Added memory device md0`, `BSD root: md0`, `load_init_program: attempting to load /sbin/launchd`, then data abort in accelerate_entitlement_queries (PC 0x...77d2458, FAR 0x1a0) = NULL AMFI interface. Photo `IMG_20261015_190715.jpg`. RAM disk root is physically verified.
- **0025 (`2518c79`)**: new `bsd/kern/sweet_amfi.c` registers a built-in AMFI interface at EARLY_BOOT/MIDDLE: no entitlements (adjustContextWithoutMonitor succeeds without creating a context, queries KERN_DENIED, copies NULL/KERN_NOT_FOUND, get_xml/transmuted false); WithMonitor KERN_NOT_SUPPORTED; OSEntitlements_query and query_context_to_object panic explicitly (CoreEntitlements closed; no kernel caller); TrustCache version 0 (0015 keys on trust_cache_rt==NULL, img4 absent -> still unavailable); CoreEntitlements table zero (PMAP_CS only). Compiles locally; checker updated (bt27 entries). CI **`38774788048`** running. Next artifacts `xnu/out-0025`, `research/bt29`, `research/run29.log`. Real iOS binaries will need real entitlements later (no OSS CoreEntitlements exists).
- **0021-0024 RAM-disk-root iteration (Claude), QEMU only so far, no new phone boot since 0020.** Each kernel tested with `tools/mkpayload.py KERNEL research/q0021.adt OUT --ramdisk research/sweet-init --cmdline "debug=0xc serial=3 cpus=1 keepsyms=1 -enable_kprintf_spam -noprogress rd=md0"` then `tools/qemu-sweet.py OUT DIR --secs 150 --kernel KERNEL --fail-on-panic` (PATH needs ~/swift/usr/bin for llvm-nm). Fresh ADTs: `research/q0021.adt` (QEMU), `shim/out/sweet-0021.adt` (phone).
  - 0021 CI `38769532000` PASSED, `xnu/out-0021` SHA256 `2b7500a1...962e`, `research/bt25`/`run25.log`. QEMU `research/q0021-rd`: md0 root, then assert `iomediaClass` in IOService::publishHiddenMedia.
  - 0022 (`20ece9c`, publishHiddenMedia without IOMedia class) CI `38770666158` PASSED, `xnu/out-0022` SHA256 `a4d0e787...30c5`, `research/bt26`/`run26.log`. QEMU `research/q0022-rd`: data abort in SHA256_Init (NULL g_crypto_funcs) from vm_kernel_addrhash_internal.
  - 0023 (`c8089d1`, libkern SHA256_Init/Update/Final use built-in ccsha256_ltc_di + ccdigest_init/update on SWEET; also serves code-signing SHA-256; SHA-1/MD5/SHA-384/512 still have NO provider - BearSSL from 0020 could supply them later) CI `38771876423` PASSED, `xnu/out-0023` SHA256 `d763fed7...1003`, `research/bt27`/`run27.log`. QEMU `research/q0023-rd`: **mockfs mounted md0, `load_init_program: attempting to load /sbin/launchd`**, then panic `mockfs_fsnode_vnode failed to create fictitious pages ... rvalue = 5` (mockfs.c:351).
  - 0024 (`3293c80`): mockfs used memdev mi_base (4K page number of a VIRTUAL kernel address for IOKit RAMDisk, truncated to 32 bits) << PAGE_SHIFT as a physical address. Now zero-copy pager mapping only when mi_phys (with 4K units); virtual md uses pagein/blockmap/strategy -> memdev bcopy. memdev's own mdPhys path assumes 4K ppnums (unsafe on 16K arm64), so do NOT switch the RAMDisk to physical. CI **`38773277811`** running. Next artifacts `xnu/out-0024`, `research/bt28`, `research/run28.log`.
  - Sandboxes: bt25/bt26/bt27 contain the next patch's edited files; pristine copies in `research/pre0022`..`pre0024`.
- **PATCH 0021 (Claude): RAM disk root via mockfs -> first userspace program.** Commits `9c37771` (initial; CI `38768500280` FAILED: mockfs_fsnode.c kfree_type(mockfs_fsnode_t) typed-alloc static assert; local xnu-cc stubs `__builtin_xnu_types_compatible` so it can't catch this) and `193772f` (fix), CI **`38769532000`** running: https://github.com/dipa-studio/ios-sweet/actions/runs/38769532000. Next artifacts: `xnu/out-0021`, `research/bt25`, `research/run25.log`.
  - Kernel: `config/MASTER.arm64.sweet` FILESYS_BASE += `mockfs` (XNU's built-in FS: root device is itself a Mach-O, served as /sbin/launchd, plus /dev for devfs). Upstream mockfs bit-rot fixed: kalloc_type/kfree_type used the POINTER typedef mockfs_fsnode_t (nodes were 8 bytes) -> `struct mockfs_fsnode`. `IOService::setRootMedia` asserted IOBlockStorageDriver class exists (IOStorageFamily absent): SWEET-only, a missing class means root=NULL (same as IOSetImageBoot); Apple path keeps assert. All 21 patches apply (`research/check-patch-order.py`); 5 TUs compile locally with -DMOCKFS (`research/mockfs-objs`), imports resolve vs 0020 kernel. Sandbox `research/bt24/xnu` now holds 0021 versions of those 3 files; pristine 0020 copies in `research/pre0021`.
  - Tools: `tools/mkpayload.py --ramdisk IMAGE` appends IMAGE as an extra payload segment right after the kernel, moves kernel_end past it (shim unchanged: ADT+boot_args follow it, image stays below topOfKernelData), and adds ADT `/chosen/memory-map` `RAMDisk`=<phys,size>. Boot with `rd=md0`. Without --ramdisk output is byte-identical to existing 0020 QEMU and phone payloads (verified). `tools/sweet-init.S` + `tools/mkinit.py`: PIC static init (open /dev/console, print PID, heartbeat every 5 s via select, never exits) wrapped into a static arm64 Mach-O (__PAGEZERO, __TEXT@0x100000000, LC_UNIXTHREAD, ad-hoc SHA-256 CodeDirectory v0x20400, 4K pages). Build: `~/swift/usr/bin/clang -target arm64-apple-ios16.0 -c tools/sweet-init.S -o X.o && tools/mkinit.py X.o research/sweet-init` (ld64.lld cannot link static iOS). Signature not verified by a real codesign; the kernel will tell.
  - QEMU with 0020 kernel + RAM disk (`research/q0020-rd.bin`, rd=md0, 150 s `research/q0020-rd150`): `Added memory device md0`, **`BSD root: md0, major 1, minor 0`** (plumbing works), then the setRootMedia assert (now fixed in 0021). NOTE: QEMU needs >60 s to reach IOFindBSDRoot (60 s run showed nothing after stf0; likely RTC/inittodr wait), so use --secs 150.
  - NEXT: download 0021, QEMU 150 s with ramdisk; expect mockfs mount then init exec -> `*** SWEET userspace: init running as PID 1 ***`. Then phone boot with same payload (phone ADT shim/out/sweet-0020.adt pattern + --ramdisk + rd=md0).
- **0020 PHONE RESULT (user transcription of screen, 2026-10-15): KERNEL INIT COMPLETE, WAITING FOR ROOT.** After Skywalk self-tests PASSED, kminit, ifnet_attach lo0/gif0/stf0, screen prints `BSD`, then `Waiting on <dict><key>IOProviderClass</key><string>IOMedia</string><key>Content</key><string>Apple_HFS</string></dict>` and `Failed to write boot-picker-bringup-reason to NVRAM.` No panic. This is IOFindBSDRoot waiting for a root device (none exists); NVRAM warning is expected (no NVRAM driver). QEMU `cpu_idle` snapshot is presumably the same wait (not GDB-confirmed; milestone GDB run was cancelled by user since phone answered it). Draft `research/check-milestones20.gdb` (hbreaks inittodr/vfs_mountroot/IOFindBSDRoot/bsd_utaskbootstrap) left uncommitted/ignored.
- **NEXT MILESTONE: root filesystem -> launchd.** Plan: (1) RAM disk: shim places an image in RAM, ADT /chosen/memory-map `RAMDisk` entry, boot-arg `rd=md0` (XNU md driver). (2) HFS is a separate kext on Apple; build Apple OSS `hfs` (tag paired with macOS 13.5 in distribution-macOS macos-135 release.json) into SWEET like libpthread 0019. APFS is closed. (3) Minimal HFS image with iOS 16 launchd (later: dyld/shared cache; AMFI/code-signing absent will be a later hurdle). User's end goal is full iOS GUI.
- **0020 (AES) VERIFIED + PHONE BOOTED (Claude took over after Codex usage limit).** Actions `38765422261` (`ab9e23f`) PASSED; kernel `xnu/out-0020/kernel.development.sweet` checksum OK; buildtree `research/bt24`, log `research/run24.log`. Actual-kernel AES checks PASS (`research/check-aes20.gdb` / `.log`, 12 extra CBC cases match OpenSSL). 45s QEMU `research/q0020`: no panic, zero unknown glyphs, past old TCP/AES panic -> Skywalk self-tests PASSED -> bsd_autoconf kminit -> ifnet_attach lo0/gif0/stf0; final snapshot PC in `cpu_idle` (idle or waiting; NOT yet diagnosed, could be root-device wait). Image `shim/out/shim-boot-0020.img` 16,461,824 bytes SHA256 `775200cb02d95fc750981516b79d91dcdc96184bbe67a38f9d57c59b4a391828`, ramdisk byte-matches `payload-0020.bin` (`check-0020`). **Phone temporarily booted 2026-10-15 16:10:00 UTC**: one FASTBOOT product sweet, send OKAY0.407s, boot OKAY0.134s, total0.549s, exit0 (`research/boot0020.log`). Awaiting user photo; do not reboot the phone before it. NEXT: read photo; diagnose what QEMU waits on after stf0 (GDB backtrace of threads / longer run). Codex's STOP request below is complete.
- User requests STOP after completing the current0020 AES iteration (Actions/QEMU/phone), then leave a precise committed handoff for another AI. User says FASTBOOT ready again after0019 photo. Do not start a new blocker fix after this iteration.
- Goal: iOS16-generation XNU on Redmi Note10Pro sweet/SM7150. Always commit, use gh/Actions for kernel builds, keep this handoff current. Temporary fastboot boot only; preserve Android/backups. No userland/root filesystem boot yet.
- **Patch0020 update:** initial `e71098d` / Actions `38765012466` superseded by scalar-only compile flags. Added -mgeneral-regs-only to all six provider/BearSSL TUs and corecrypto_aes.c; object disassembly now confirms no SIMD. Updated implementation `ab9e23f`, Actions `38765422261` queued/running: https://github.com/dipa-studio/ios-sweet/actions/runs/38765422261. Do not launch another run. portable BearSSL AES CBC and legacy one-block ECB wired directly into SWEET corecrypto_aes.c. Genuine bitslice implementation, no CPU AES/SIMD requirement, heap or fake global crypto table. Source fetched by `0020-stage-bearssl.sh` from official BearSSL git at immutable `7bea48e5e850ab4cafbe68d3765cdaba13a86d6f`; MIT license retained, downloaded source stays out of git. Kernel limits include adapter only. All128/192/256-bit keys, CBC encrypt/decrypt, copied contexts, in-place/out-of-place; zero block count and size overflow handled, invalid keys clear context. Seven TUs compile to arm64 objects (`research/aes-objs`), imports resolve, all20 patches apply and sources match (`research/patch0020-check.log`). Sandbox `research/bt23/xnu`, originals `research/pre0020`. NEXT: Actions then actual-kernel NIST/random CBC/ECB checks and >=45s QEMU before another phone boot. Next artifacts out-0020 / bt24 / run24.log. 0020 updated CI pending. Local sources/checks match the scalar-only patch.
- **Latest built kernel0019**, implementation `256d3f0`, patches0001-0019. Actions `38763072907` PASSED (4m5s): https://github.com/dipa-studio/ios-sweet/actions/runs/38763072907. Kernel `xnu/out-0019/kernel.development.sweet`, SHA256 `a27df7568f03fb541cf0e8024c0a594fa178a85054fcf9eba29158cbd4c5ecc0`; checksum verified. Log/source `research/run23.log` / `research/bt23` (no nested .git). Seven pthread source/build/header files and staged adaptations byte-match local compile inputs.
- **0019 hardware photo received** `IMG_20261015_175306.jpg`: pthread registration visible, passes mbinit and DLIL checksum/scheduling, same AES fault as QEMU: PC`0xfffffe00079174e0` (`aes_encrypt_key128`), LR`0xfffffe000767c3a4` (`tcp_init`), FAR0x78, x8=0. Disassembly proves NULL g_crypto_funcs->ccaes_cbc_encrypt. Photo processed; no more0019 observation pending. General crypto table remains absent, other hash/mode operations may still fault after0020. DO NOT bypass TCP or install fake crypto. XNU already contains SHA256/HMAC implementations; AES source there is a dispatch wrapper. Full future provider interface libkern/libkern/crypto/register_crypto.h. Apple/corecrypto default README describes evaluation-only licensing, so no external corecrypto was fetched or staged; BearSSL selected for this portable AES integration.
- **0019 pthread FIXED in QEMU and observed beyond its init on phone**: 45s QEMU `research/q0019`, zero unknown glyphs, then AES panic. Actual-kernel API checks PASS `research/check-pthread19.gdb` / `.log`, guest `research/q0019-api`: function/callback table identity, actual provider pthread_init returns, locks/global hash/threadcall/typed zones initialized, thread_selfid via syscall shim + buffer bounds, process hash init/delete via shims restores NULL. Monitor quit prints expected remote-disconnect after PASS. All fixture guests stopped.
- **0019 provider**: genuine Apple libpthread-514.100.2 commit `a6c70ca0e544347a0377babf748f831577ec24f9`, exact paired release with xnu8796.141.3 in distribution-macOS/macos-135/release.json (`research/macos-135-release.json`). 0019 shell stage runs before patches (workflow copies *.sh); three SWEET wrapper TUs use kext BSD header view, vm_fault_external, renamed provider-side pthread_kern/current_uthread; register real function table at EARLY_BOOT SECOND, real pthread_init later. Stager applies bounded address casts, zero user addresses, proper panic format. SWEET opaque ksyn storage enlarged48->56 for trailing kwe_uth, checked with provider static assertion. Local pre-CI TargetConditionals fixture was syntax-only; artifact bt23 carries actual SDK header and CI is authoritative.
- **Phone0019 temporarily booted** 2026-10-15 15:51:36 UTC: exactly one FASTBOOT/product sweet verified; send OKAY0.406s, boot OKAY0.134s, total0.546s, exit0 (`research/boot0019.log`). Image `shim/out/shim-boot-0019.img`, 16,457,728 bytes, SHA256 `19429a57e486b856880df9e27327ed488e0139345311fdb0350974e72103a9a1`. Phone ADT/payload `shim/out/sweet-0019.adt` / `payload-0019.bin` timer19, unpacked ramdisk byte-match (`check-0019`). EntryPA0xb146b488, kernelendPA0xb2000000, boot_argsPA0xb2004000/top0xb2008000. Last user FASTBOOT+USB reply preceded this boot; do not assume phone is still in FASTBOOT.
- **CPU0018 physically verified** by `IMG_20261015_173951 (2).jpg`: boot CPU0 registered, real SGI1 verified, initialization complete -> BSD/MAC/mbinit -> old pthread-table panic. SweetCPU uses legacy IOCPU, ml_processor_register/processor_start/initCPU, real SGI1 clears SIGPdisabled before max CPUs completion; no forced flags/skipped waits. CPU APIs and two-CPU rejection passed (`research/check-cpu18.gdb`, `q0018-api`, `q0018-smp`). Single core; SMP/suspend unsupported. CURRENT SWEET has NO USE_APPLEARMSMP.
- QEMU next run: generate fresh `tools/mkadt.py research/q0020.adt --qemu` (timer27), mkpayload with `debug=0xc serial=3 cpus=1 keepsyms=1 -enable_kprintf_spam -noprogress`, then `tools/qemu-sweet.py PAYLOAD OUT --secs 45 --kernel KERNEL --fail-on-panic`. 12s can miss later panics because IORTC waits30s. Runner snapshot only: panic exit2, blank/incomplete exit3; absence of panic does not mean complete boot. Qualcomm XPU/ABL/panel and phone SMC PSCI require hardware checks.
- Local checker `tools/xnu-cc.py research/run23.log research/bt23 FILTER --per-file`, XNU_CC_OBJDIR for objects. For new BearSSL TUs add XNU_CC_EXTRA with -I bt23/xnu/libkern/crypto/sweet_bearssl/{inc,src}. Patch checker `research/check-patch-order.py` uses temporary upstream index; source fixtures/artifacts ignored under research. GDB mask registers to unsigned64, use function-entry manual-call return traps, discard manipulated guests. No QEMU guest intentionally left running.

## Session log and earlier handoffs (superseded by the active summary above)

### Prior 0019 transition notes (superseded)

- Goal: iOS 16 generation XNU on sweet. Always commit completed work, use gh/Actions for kernel builds, keep this handoff current. Temporary fastboot boot only; preserve Android/backups.
- **0019 build PASSED** Actions `38763072907` (4m5s), implementation `256d3f0`. Kernel `xnu/out-0019/kernel.development.sweet`, SHA256 `a27df7568f03fb541cf0e8024c0a594fa178a85054fcf9eba29158cbd4c5ecc0`; checksum verified. Log/source `research/run23.log` / `research/bt23`; all seven patched files and staged provider adaptations match local compile inputs. Prefer bt23 as the next baseline.
- **0019 pthread FIXED in QEMU**: 45s run `research/q0019` registers provider, passes mbinit, DLIL SUM16 checks and DLIL thread scheduling. Actual-kernel GDB checks `research/check-pthread19.gdb` / `.log` PASS: function/callback table identity, real _pthread_init returns, locks/global hash/cleanup threadcall/typed zones non-NULL, thread_selfid via actual syscall shim with output bounds, process hash init/delete through actual shims restores NULL. Guest stopped after tests; no fixture left running. GDB monitor quit prints expected remote disconnect after PASS.
- **Current next blocker:** QEMU0019 panic data abort FAR0x78 at `aes_encrypt_key128` PC`0xfffffe00079174e0`, called by `tcp_init` LR`0xfffffe000767c3a4`. Disassembly confirms `g_crypto_funcs == NULL`, loading ccaes_cbc_encrypt at+0x78. General crypto registration is still absent; DO NOT bypass TCP or supply fake crypto. Next integrate a genuine compatible crypto implementation/provider (AES first needed here, other digest/mode APIs also used). XNU contains SHA256/HMAC but no AES implementation; its corecrypto_aes.c is a dispatch wrapper. Full crypto table is in libkern/libkern/crypto/register_crypto.h.
- **0019 phone image prepared and verified**, user reports FASTBOOT+USB again; exactly one device/product sweet verified. `shim/out/shim-boot-0019.img`, 16,457,728 bytes, SHA256 `19429a57e486b856880df9e27327ed488e0139345311fdb0350974e72103a9a1`; unpacked ramdisk matches phone payload (`shim/out/check-0019`, `payload-0019.bin`, `sweet-0019.adt`, timer19). EntryPA0xb146b488; kernelendPA0xb2000000; boot_argsPA0xb2004000/top0xb2008000. Usual debug cmdline cpus=1/-noprogress. **Temporarily booted 2026-10-15 15:51:36 UTC**: send OKAY0.406s, boot OKAY0.134s, total0.546s, exit0 (`research/boot0019.log`). User asked for photo after45s. Await the photo; DO NOT reboot again before observation. Bootloader acceptance only; no0019 physical progress observed yet.
- **Hardware photo confirms 0018 progress** (`IMG_20261015_173951 (2).jpg`): real phone prints `SWEET CPU: boot CPU 0 registered, SGI1 verified, initialization complete`, enters BSD/MAC, reaches `mbinit: done`, then the same missing-pthread-function-table panic as QEMU at pthread_shims.c:318. CPU completion/interrupt delivery is now physically observed. The photo was received; no further phone boot performed yet.
- **Patch0019 committed/pushed `256d3f0`; Actions `38763072907` queued/running:** https://github.com/dipa-studio/ios-sweet/actions/runs/38763072907. Do not launch a duplicate. genuine Apple OSS libpthread-514.100.2 provider (immutable commit `a6c70ca0e544347a0377babf748f831577ec24f9`), paired with xnu-8796.141.3 by Apple's `distribution-macOS/macos-135/release.json` (`research/macos-135-release.json`). `0019-stage-libpthread.sh` fetches/stages it before patches; workflow now copies shell scripts too. SWEET-only wrappers compile all three real kernel sources, rename provider-side symbols, use kext header view and vm_fault_external, register the real function table at EARLY_BOOT SECOND. Existing pthread_init then runs the actual provider initialization. No pthread wait/syscall is bypassed. Source staging includes bounded explicit address casts/zero user addresses and panic-format fixes. SWEET kernel opaque ksyn storage enlarged from48 to56 bytes to contain this provider's trailing kwe_uth, with a provider size assertion. Local compile of three provider TUs plus pthread_shims/kern_fork passed; imports resolve and provider exports do not collide with existing kernel symbols. All19 patches apply and changed sources match (`research/patch0019-check.log`). Source sandbox `research/bt22/xnu`, originals `research/pre0019`, objects `research/pthread-objs`. Local TargetConditionals fixture only has target macros for syntax; CI stages the real macOS SDK header and is authoritative. Next Actions artifacts: out-0019 / bt23 / run23.log; run QEMU >=45 seconds. 0019 CI is pending; no 0019 QEMU run yet.
- **Latest kernel implementation `e7d9550`, patches0001-0018; CI `38760004398` PASSED (3m56s)**: https://github.com/dipa-studio/ios-sweet/actions/runs/38760004398. No build is pending. Kernel `xnu/out-0018/kernel.development.sweet`, SHA256 `b3c8d59acc88f93a2c836eb2527a05ed4fbaac7c580a2ea8c3b795137f4aadf1`; checksum verified. Source/log `research/bt22` / `research/run22.log` (baseline for patch0019; no nested .git). New driver/config sources match CI exactly.
- **CPU wait FIXED in QEMU**: SweetCPU uses legacy IOCPU, validates single boot CPU, ml_processor_register -> processor_start -> initCPU real SGI1 -> scheduler/IOKit completion -> ml_set_max_cpus. It does NOT force a flag or skip a wait. CPU APIs passed: Running CPU0, processor_array[0] association, SGI signalling enabled, name, unsupported secondary-start error (`research/check-cpu18.gdb` / `.log`, `research/q0018-api`). Single-core only; SMP and suspend unsupported. Two-CPU topology fails explicitly (`research/q0018-smp`).
- **Next blocker reproduced in 45s QEMU** (`research/q0018-long`, zero unknown glyphs): BSD initialization reaches MAC framework, network mcache and `mbinit: done`, then **`pthread kernel extension not loaded (function table is NULL). @pthread_shims.c:318`**. Do not bypass pthread_init: find/integrate the matching open-source libpthread kernel implementation and register its real function table. A 12s run `research/q0018` looked panic-free before the 30s IORTC wait elapsed; use >=45s or GDB checkpoints for the next iteration. No userland/root filesystem boot yet.
- **Phone 0018 TEMPORARILY BOOTED** 2026-10-15 **15:31:15 UTC**: exactly one FASTBOOT device verified product sweet; send OKAY0.400s, boot OKAY0.134s, total0.539s, exit0. Local log `research/boot0018.log`. This confirms bootloader acceptance only. User was asked for a photo; await it, DO NOT reboot the phone again before their observation. No flashing performed.
- Current hardware image `shim/out/shim-boot-0018.img`, **16,437,248 bytes**, SHA256 **`b7a23595d143a76c72c9fe2cfcc5d77a8f16987b5af1a2703bbffdfa6922908c`**. Phone ADT/payload `shim/out/sweet-0018.adt` / `shim/out/payload-0018.bin` (timer19), unpacked ramdisk exactly matches payload (`shim/out/check-0018`). EntryPA0xb146b488; kernelendPA0xb1ffc000; boot_argsPA0xb2000000/top0xb2004000. Command line `debug=0xc serial=3 cpus=1 keepsyms=1 -enable_kprintf_spam -noprogress`.
- QEMU payload/ADT `research/q0018.bin` / `research/q0018.adt` (timer27). Run `python3 tools/qemu-sweet.py research/q0018.bin research/NEW-OUT --secs 45 --kernel xnu/out-0018/kernel.development.sweet --fail-on-panic`; fresh ADT seed for a new payload. Output fb.png/fb.txt/regs.txt/qemu.log/result.json. Visible panic exit2; blank/incomplete decode exit3; no_panic_observed is snapshot-only, not successful boot. -noprogress prevents the graphical indicator corrupting console text. Qualcomm XPU/bootloader/panel and SMC PSCI still need physical validation.
- Patches0014-0017 fixed PRNG registration/direct SHA256, explicit unavailable trust-cache service with rejected loads/no matches, built-in SweetPlatformExpert, and timer PPI from ADT (phone19/default19, QEMU27). All their checks/artifacts are preserved in the history below. CURRENT SWEET has NO USE_APPLEARMSMP; do not assume gCPUIC/gAIC/gPMGR AppleARMSMP globals exist.
- Local single-file checker: tools/xnu-cc.py research/run22.log research/bt22 FILTER --per-file, output via XNU_CC_OBJDIR. Patch checker `research/check-patch-order.py` applies all18 via temporary index, matches sandbox sources. Sandbox for0018 `research/bt21/xnu`, originals `research/pre0018`, objects `research/cpu-objs`. First compiler issues were missing machine/atomic.h and nonexistent getNameSymbol; fixed and both TUs compiled successfully before CI. Fixtures/artifacts under research are local-only/ignored.
- GDB: mask int(register)&((1<<64)-1) for unsigned kernel pointers; use function-entry manual-call return traps, discard manipulated guests. No QEMU fixture is intentionally left running.

### Earlier active handoff snapshot (superseded)


- **0018 milestone:** Actions `38760004398` PASSED (3m56s), checksums passed, new CPU/build files byte-match CI tree. Kernel `xnu/out-0018/kernel.development.sweet`, SHA256 `b3c8d59acc88f93a2c836eb2527a05ed4fbaac7c580a2ea8c3b795137f4aadf1`; log/tree `research/run22.log` / `research/bt22`. QEMU `research/q0018` 12s: boot CPU registered, SGI1 verified, CPU initialization complete, **BSD initialization entered** (copyright, MAC framework, buffer headers), no panic/zero unknown glyphs. Direct CPU API checks PASS (`research/check-cpu18.gdb`, `.log`, `research/q0018-api`): CPU0 Running, processor matches processor_array[0], real SGI enabled, CPU name correct, secondary-start returns KERN_NOT_SUPPORTED. Two-CPU ADT correctly panics before registration (`research/q0018-smp`). Longer 45s run `research/q0018-long` still running; next BSD wait/fault not yet identified.
- **Prepared 0018 phone image:** `shim/out/shim-boot-0018.img`, 16,437,248 bytes, SHA256 `b7a23595d143a76c72c9fe2cfcc5d77a8f16987b5af1a2703bbffdfa6922908c`. Phone ADT/payload `shim/out/sweet-0018.adt` / `shim/out/payload-0018.bin`, usual debug args with cpus=1 and -noprogress. Unpacked ramdisk byte-compared (`shim/out/check-0018`). Entry PA remains 0xb146b488; kernel end PA 0xb1ffc000, boot_args 0xb2000000/top0xb2004000. User reports FASTBOOT+USB ready; read-only check found exactly one product sweet. NEXT: temporary fastboot boot of this verified image, record send/boot results, then await hardware photo without rebooting again.
- Patch 0018 committed/pushed **`e7d9550`**; Actions **`38760004398`** queued/running: https://github.com/dipa-studio/ios-sweet/actions/runs/38760004398. Do not launch a duplicate run.
- Active continuation: patch **0018** implemented in `iokit/Kernel/arm/SweetCPU.cpp` + arm64 source list + exact qcom,kryo470 personality. Keeps legacy IOCPU, accepts ONLY the single boot CPU, fills ml_processor_info from parsed topology with CPU object as cpu_id, calls ml_processor_register and processor_start. initCPU sends SGI1 via ICC_SGI1R_EL1 and waits (1-second deadline) for real cpu_signal_handler to clear SIGPdisabled. Only after successful startup calls ml_cpu_init_completed/cpusRunning and publishes completion/max CPUs. Secondary startup/suspend are explicitly unsupported. Local C++ compile passed for both TUs (`research/cpu-objs`), all imports resolve, object contains dsb ishst / ICC_SGI1R / isb; all18 patches apply in order and match sources. Sandbox `research/bt21/xnu`, originals `research/pre0018`; next CI artifacts should use `xnu/out-0018`, `research/bt22`, `research/run22.log`. User has the phone available; first run QEMU, then prepare/temporarily boot useful hardware progress.
- Goal: iOS 16 generation XNU on Redmi Note 10 Pro / sweet. User wants persistent progress, commits, kernel builds via `gh` Actions, and this handoff kept current across agent switches. Temporary `fastboot boot` only; preserve Android and backups.
- Latest kernel implementation: **`aa3f991`**, patches **0001-0017**. All four new Actions builds PASSED: 0014 `38748543058` (4m0s), 0015 `38749549779` (4m3s), 0016 `38750733996` (4m3s), 0017 **`38751507437` (4m7s)**. Latest: https://github.com/dipa-studio/ios-sweet/actions/runs/38751507437. No build is pending.
- **Current blocker: CPU initialization has no SWEET CPU driver.** QEMU registers SweetPlatformExpert, then kernel_bootstrap_thread waits in **kperf_init_early -> ml_wait_max_cpus**. Confirmed via actual-kernel GDB call stack (`research/locate-cpu-wait2.log` / `.symbols`) and normal-run `max_cpus_initialized == 2` (MAX_CPUS_WAIT). Do not force the flag or remove the wait. Next implement real CPU registration and completion using XNU's CPU/processor interfaces. CURRENT SWEET uses legacy IOCPU (`USE_APPLEARMSMP` is absent); enabling AppleARMSMP is a separate architectural change requiring GIC/PMGR services. No userland/root filesystem boot yet.
- Fixed this session: PC=0 unregistered PRNG callback (0014, built-in HMAC-SHA256 provider + direct entropy/write_random hashing); absent Image4/AMFI trust-cache service (0015, unavailable runtime, loads rejected and queries no-match); missing platform driver (0016, SweetPlatformExpert exact personality); incorrect QEMU timer IRQ (0017, validated ADT PPI property, phone default19/QEMU27).
- **Latest kernel** `xnu/out-0017/kernel.development.sweet`, SHA256 `055a80190ab998b4df9203f1102072dcc78faad1779a537a514e522b682b582f`. Buildtree/log **`research/bt21` / `research/run21.log`**; all nine changed C/C++/header/config files byte-match local checked sources. Checksums passed. Entry PA `0xb146b488`, kernel end PA `0xb1ff4000`. Preferred baseline for patch 0018: `research/bt21/xnu` (no nested .git).
- **QEMU verified**: `research/q0017-noprogress` ran **30 seconds**, no visible panic, **zero unknown glyphs**, IRQ exceptions returned normally (4 IRQs, tickless idle). It still waits for CPU completion, so this is NOT a completed boot. Earlier 15s run `research/q0017` captured 122 IRQs, timer value27, GICR_ISENABLER0 `0x08000002`, and CPU WAIT; graphical progress overlay corrupted 13 glyphs. Add **`-noprogress`** to keep the console readable. New runner classifies incomplete/blank decoding explicitly (exit3 with --fail-on-panic), visible panic exits2. Snapshot-only; use regs/GDB to diagnose stalls.
- Current QEMU payload/ADT: **`research/q0017-noprogress.bin` / `research/q0017.adt`**. Run: `python3 tools/qemu-sweet.py research/q0017-noprogress.bin research/NEW-OUT --secs 30 --kernel xnu/out-0017/kernel.development.sweet --fail-on-panic`. Generate fresh seed with mkadt for a new payload. QEMU virt models generic ARM execution, not Qualcomm XPU/bootloader/panel or the phone's SMC PSCI conduit.
- **Prepared phone image**: **`shim/out/shim-boot-0017.img`**, 16,416,768 bytes, SHA256 `51ff7fe30b4a6668c3452ffe1da922c5ac30cb8de60625c2a652587ecbd0ec92`. Phone ADT/payload `shim/out/sweet-0017.adt` / `shim/out/payload-0017.bin` (timer19). Unpacked ramdisk exactly matches payload (`shim/out/check-0017`). Command line **`debug=0xc serial=3 cpus=1 keepsyms=1 -enable_kprintf_spam -noprogress`**. NOT sent to phone this session: known CPU-driver wait should be addressed in QEMU first. Last actual phone boot remains 0013; user's latest photo `IMG_20261015_160033.jpg` confirmed the old PRNG PC=0 panic.
- Direct QEMU/GDB API checks: PRNG 600-byte chunking, buffer bounds, changing output, zero-length reads/writes, user reseed, contended refresh, generator1 init/output PASS again on latest0017 (`research/check-prng17.gdb` / `.log`, guest `research/q0017-api`). Trust-cache unavailable-service/argument/output cases PASS on0015 (`research/check-trustcache.gdb` / `.log`). Platform registration + target/product normal/truncated/zero/null/bounds PASS on0016 (`research/check-platform.gdb`, `research/check-platform3.log`). Different platform ID correctly retains IOPanicPlatform fallback (`research/q0016-negative`). Timer PPI32 and size2 correctly panic (`research/q0017-badppi`, `research/q0017-badsize`); omitted property selects19 (`research/q0017-default`). These are API/bring-up checks, not crypto certification or SMP scheduling validation.
- Local checker **`research/check-patch-order.py`** verifies all17 patches and changed source equality using a temporary upstream git index. Fixtures/logs/artifacts under research are local-only and ignored. For GDB unsigned kernel pointers, mask `int(register_value) & ((1<<64)-1)`; x30/x29 may be rendered signed. Stop at a function entry for manual-call return traps, and discard each manipulated guest after API tests. All fixture QEMU guests have been stopped.


- User wants persistent progress toward iOS 16, commits for completed changes, builds through `gh` / GitHub Actions.
- Keep this file current at every milestone so another agent can resume immediately, including when usage limits interrupt a session.
- Patch 0017 committed/pushed `aa3f991`, Actions `38751507437` running: https://github.com/dipa-studio/ios-sweet/actions/runs/38751507437. Next artifacts: xnu/out-0017, research/bt21, research/run21.log. Do not duplicate.
- Patch 0016 kernel downloaded/checksum verified: SHA256 `d06336c54baa06e0df6561a8bd7df98a9b9f7502c9381c62d1a44c045cd1ce46`, `xnu/out-0016/kernel.development.sweet`; buildtree/log `research/bt20` / `research/run20.log`. QEMU `research/q0016` (10s) prints platform expert registered, zero unknown glyphs, no panic observed BUT stalls (timer mapping still wrong until 0017). Direct actual-kernel GDB tests passed for registered instance and target/product names, including maxLength=1/0 and NULL, sentinels intact (`research/check-platform.gdb`, `research/check-platform3.log`, `research/q0016-api3`). First two fixture attempts used premature/wrong return breakpoints; discarded, corrected fixture uses start-return observation then a function-entry return trap. No phone boot yet.
- Patch 0016 CI `38750733996` PASSED (4m3s); artifacts not yet downloaded/booted. Patch 0017 prepared to fix QEMU timer fidelity BEFORE interpreting future no-panic/stall runs: actual `qemu-system-aarch64 -M virt,gic-version=3,dumpdtb=research/qemu-virt.dtb ...` timer property has virtual PPI 11 => INTID 27, while phone uses INTID 19. Kernel formerly hardcoded 19. New optional /arm-io/gic virtual-timer-intid property selects the PPI (validate exact u32 and 16..31); missing property retains phone default 19. One RO-late global drives GIC enable and sleh_irq dispatch. tools/mkadt.py emits phone19/QEMU27. Both C TUs compile (`research/timer-objs`), ADT serialize/parse checks confirm values/addresses, all 17 patches apply and match. Sandbox `research/bt19/xnu`, originals `research/pre0017`; initial local sleh compile failed due read-only exported header not updated, fixed export and BOTH recompiled successfully. CI exports normally.
- Architecture correction for next agent: CURRENT SWEET board_config does NOT define USE_APPLEARMSMP; it compiles the legacy IOCPU path, not cpu_boot_thread/IOPMGR AppleARMSMP. Existing roadmap mentions AppleARMSMP as a future approach. Do not look for gCPUIC/gAIC/gPMGR globals in the current kernel: they are absent. Determine the actual CPU-init wait after platform registration before choosing the next driver architecture.
- Patch 0016 committed/pushed as `0dcdf5c`; Actions `38750733996` running: https://github.com/dipa-studio/ios-sweet/actions/runs/38750733996. Next artifacts: `xnu/out-0016`, `research/bt20`, `research/run20.log`.
- Patch 0016 implemented: new `iokit/Kernel/arm/SweetPlatformExpert.cpp`, arm64 source list, built-in personality in KernelConfigTables.cpp guarded by ARM64_BOARD_CONFIG_SWEET. Matches only xiaomi,sweet at score 10000, keeps IOPanicPlatform fallback for other hardware. Uses IODTPlatformExpert base initialization/power domain and createNubs CPU logical-ID annotation; overrides top-level publishing because base requires nonexistent Apple NVRAM /options. Publishes real CPU/top-level nodes, provides bounded target/product names, and registers the platform service. GIC/PMGR and arm-io child-bus drivers remain separate work. Both C++ TUs compile (`research/platform-objs`), all imports resolve against 0015/own objects, all 16 patches apply and sources match. Sandbox `research/bt19/xnu`; originals `research/pre0016`. NEXT: full Actions/QEMU; inspect CPU boot waits after the platform registers, do not equate absence of panic with successful boot.
- Patch 0015 CI `38749549779` PASSED (4m3s), checksum verified. Kernel `xnu/out-0015/kernel.development.sweet`, SHA256 `ca1b424a596c7058550ad1659080a333e7643270f019656401d80bf2b876645f`; buildtree/log `research/bt19` / `research/run19.log`. QEMU `research/q0015` (10s, zero unknown glyphs) passes trust-cache handling and machine lockdown, then panics `Unable to find driver for this platform: "xiaomi,sweet" @IOPlatformExpert.cpp:2319` (IOPanicPlatform fallback). Next concrete work: add a built-in SWEET IODTPlatformExpert personality/class; keep the fallback for mismatched platforms. No phone boot yet.
- Direct trust-cache API checks PASSED in the actual 0015 kernel under QEMU/GDB (`research/check-trustcache.gdb`, `research/check-trustcache.log`, `research/q0015-api`): runtime pointers NULL, hash/UUID queries return NOT_FOUND, typed/untyped/legacy loads return NOT_SUPPORTED, capabilities outputs zero and bounds preserved, invalid-argument cases still reject. Second PRNG check also verifies contended refresh returns immediately (`research/check-prng2.log`, `research/q0014-api2`).
- Patch 0015 committed/pushed `8f851d4`, Actions run `38749549779` running: https://github.com/dipa-studio/ios-sweet/actions/runs/38749549779. Do not duplicate.
- Direct PRNG API checks passed in the REAL 0014 kernel under QEMU/GDB, stopped before trust-cache init (`research/check-prng.gdb`, log `research/check-prng.log`, guest `research/q0014-api`). Invoked read_random for 600 bytes (chunking and buffer sentinel intact), repeated output differed, zero-length read/write succeeded, write_random(64-byte input) succeeded, secondary generator 1 initialized and generated distinct output. QEMU log has no exceptions during these checks; guest was discarded after the tests. This tests API behavior, not a cryptographic certification or SMP scheduling.
- Patch 0015 implemented in `bsd/kern/kern_trustcache.c`: SWEET with missing Image4/AMFI marks both runtime pointers NULL and logs service unavailability. Existing static loader handles NULL. Public load APIs return KERN_NOT_SUPPORTED; hash/UUID queries return KERN_NOT_FOUND; capabilities outputs are zero with KERN_NOT_SUPPORTED. Invalid-input checks remain, and Apple paths/available-provider initialization retain existing behavior. No trust is granted. Local arm64 compile passed (`research/trustcache-objs`), all 15 patches apply and sources match. Sandbox `research/bt18/xnu`, baseline `research/pre0015`. Next: Actions/QEMU and direct unavailable-service API checks using GDB.
- Patch 0014 CI `38748543058` PASSED (4m0s), artifact checksum passed. Kernel `xnu/out-0014/kernel.development.sweet`, SHA256 `741de20c851755d333759d7e668346f3edb4f325efac4cc7c142d80d1f5ff75c`; buildtree/log `research/bt18` / `research/run18.log`. QEMU run `research/q0014` (10s, zero unknown glyphs) prints `SWEET PRNG: HMAC-SHA256 registered`, passes commpage initialization, loads vnguard policy, THEN panics `image4 interface not available @kern_trustcache.c:253`. PC=0 fault FIXED IN QEMU. No new phone boot performed.
- Next work: SWEET currently has neither AppleImage4 nor AMFI. `trust_cache_runtime_init` unconditionally requires both; unlike BCM2837, SWEET enters this startup path. Investigate an explicitly unavailable trust-cache runtime with rejected loads/no query matches while providers are absent; preserve all Apple-platform checks. Merely skipping the startup call would leave later AMFI/IMG4 dereferences unsafe. Existing load_static_trust_cache already handles trust_cache_rt == NULL.
- QEMU runner now supports `--kernel MATCHING_KERNEL --fail-on-panic`: runtime font extraction from the payload produces exact `fb.txt`, plus `result.json`; a visible panic returns 2. Baseline 0013 re-run `research/q0013-text` detects the same NULL callback panic with zero unknown glyphs. Synthetic glyph roundtrip (including literal `?`) and invalid-VA rejection passed. Screen rows remain circular, not a full log; `no_panic_observed` does not prove successful boot. README has the invocation.
- Patch 0014 committed/pushed as `be61f99`; Actions run `38748543058` is running: https://github.com/dipa-studio/ios-sweet/actions/runs/38748543058. Do not start a duplicate build.
- 2026-10-15 active: hardware photo `IMG_20261015_160033.jpg` confirms the same PC=0 instruction-fetch abort as QEMU (LR `0xfffffe0007443fe4` = read_random). Independently reproduced with kernel 0013 in `research/q0013` (8-second run); prior allocator fix works. Latest tooling commit reviewed: `8d66d2d`.
- Patch 0014 implemented: registers a built-in HMAC-SHA256 DRBG provider at EARLY_BOOT/FIRST, before commpage creation. A shared mutex serializes output/reseed across CPUs (correctness first; no per-CPU scaling yet), initgen validates CPU identities, refresh uses try-lock and the existing entropy filter/health tests. SWEET entropy accumulation and write_random now use the built-in SHA256 directly; no incomplete g_crypto_funcs table is registered. The existing bootloader-seed requirement remains enforced.
- Local validation: both PRNG translation units compile to arm64 objects in `research/prng-objs`; no SHA256 wrapper imports remain. All 14 patches apply in CI order and resulting PRNG sources match local inputs. Sandbox `research/bt17/xnu`, originals `research/pre0014`, checker `research/check-patch-order.py`. NEXT: Actions kernel build, download to `xnu/out-0014`, run the QEMU payload before requesting another phone boot. Baseline QEMU ADT/payload are `research/q0013.adt` / `research/q0013.bin`.
- 2026-10-15 08:15 QEMU EMULATION WORKS (`tools/qemu-sweet.py`, see "QEMU emulation" below). Patch 0013 VERIFIED in QEMU: boot passes kmem_crypto_init and ulock_initialize. NEXT BLOCKER (reproduced in QEMU, expected on hardware too): `pmap_create_commpages -> random() -> RandomULong -> read_random -> prng_funcs.refresh == NULL` (instruction fetch abort at pc 0, lr read_random+0x34). Nobody calls `register_and_init_prng` (closed corecrypto kext does it on Apple). Fix = patch 0014: SWEET-registered cckprng_funcs provider (osfmk/prng/prng_random.c); note random_cpu_init also panics while !prng_ready. The 07:26 hardware boot of 0013 will most likely show this same panic.
- Previous hardware boot: patch 0013 (`a3a46e2`) CI `38686345689` PASSED (3m44s), image `shim/out/shim-boot-0013.img` was temporarily booted on 2026-10-15 at 05:26 UTC: send OKAY (0.407s), boot OKAY (0.134s), total 0.546s. Superseded by the hardware/QEMU PC=0 observation and patch 0014 work above.
- Latest kernel implementation commit: `8f851d4`. Latest successful kernel CI: `38749549779` (patch 0015), https://github.com/dipa-studio/ios-sweet/actions/runs/38749549779.
- CURRENT image: `shim/out/shim-boot-0013.img`, 16,412,672 bytes, SHA256 `110d1c46aee4b60253db24b6535e9781c4cf0fd25ca2a70e9016cf81378cea25`. Kernel `xnu/out-0013/kernel.development.sweet`, SHA256 `f0b1d375995316c550ce7014ff234d8271e9b9ac2857795ceff41b76ba2cd740`, plain arm64. CI log confirms 0013 applied; checksums passed. Entry PA `0xb146b488`, kernel end PA `0xb1ff4000`.
- Current log/buildtree: `research/run17.log`, `research/bt17` (extracted, no nested .git; preferred baseline for patch 0014). Payload/ADT: `shim/out/payload-0013.bin`, `shim/out/sweet-0013.adt` (includes /defaults). Unpacked ramdisk byte-compared with payload in `shim/out/check-0013`.
- Patch-0012 original image: `shim/out/shim-boot-0012.img`, 16,412,672 bytes, SHA256 `cf4b8ec4a65a51888d71c17d09cd0831d6b761b90e2531fdb8e575c32f4fd24a`. Kernel `xnu/out-0012/kernel.development.sweet`, SHA256 `69b78f49f6459cc63a0af44e7cf73c1779ef72130ef72b12ae86c88183ffc742`; checksums passed and sweet_early_relocate symbol present. Entry PA remains `0xb146b488`, kernel end PA `0xb1ff4000`.
- Patch-0012 log/tree: `research/run16.log`, `research/bt16` (extracted, no nested .git; preferred baseline for next patch). CI log confirms 0012 applied. Payload `shim/out/payload-0012.bin`, ADT `shim/out/sweet-0012.adt`; packed ramdisk byte-compared with payload in `shim/out/check-0012`.
- Active payload command line: `debug=0xc serial=3 cpus=1 keepsyms=1 -enable_kprintf_spam`.
- Latest observed photo: `/home/dipa/Downloads/IMG_20261015_071346.jpg` (local only). Stages 9 (kernel framebuffer VA), 10 (TTBR0 off/TLB flush), 11 (physmap table pointers) and 12 (calling pmap_bootstrap) all visible. Persistent diagnostics are VERIFIED ON HARDWARE.
- pmap.c:1987 is the assert after SecureDTLookupEntry(NULL, "/defaults", &entry). The pmap-max-asids property itself is optional; an empty /defaults uses XNU's MAX_ASIDS. Do not bypass the assertion or guess an ASID limit.
- Previous payload-only fix image: `shim/out/shim-boot-0012-defaults.img`, SHA256 `3ff61a268853c6ea7cbf284833f1346a1d791980ca366b6e1a215a86e040512b` (16,412,672 bytes). Same kernel/CI as 0012; ADT 2516 bytes (`shim/out/sweet-0012-defaults.adt`), root /defaults present with no ASID override. Payload `shim/out/payload-0012-defaults.bin`; unpacked ramdisk exactly matches payload (`shim/out/check-0012-defaults`). This boot reached the allocator crypto NULL-pointer fault below.
- Latest hardware photo: `/home/dipa/Downloads/IMG_20261015_071608.jpg` (local only). Normal serial framebuffer console works. VM bootstrap/zone initialization, logging, scheduler, IPC, KDP and ulock_initialize print before the kmem_crypto_init data abort. gPanicBase warnings are expected because PLATFORM_PANIC_LOG_DISABLED is configured, and are not the faulting call.
- Faulting instruction: `ldr x0, [x8, #0x150]` in crypto_random_kmem_ctx_size; x8 = NULL g_crypto_funcs. Source: libkern/crypto/corecrypto_rand.c and osfmk/vm/vm_kern.c:2448. rand.h explicitly permits NON-cryptographic kmem RNGs, but requires no locking in generation and context size <=256. Do not substitute read_random here: the global PRNG provider is also unregistered and kmem generation must remain lock-free. early_random may seed contexts during single-CPU bootstrap; it uses the built-in HMAC-SHA256 DRBG with the host-generated ADT seed.
- Patch 0013 keeps the replacement entirely private to vm_kern.c: SplitMix64 state is 8 bytes per CPU/sizeclass, initialized using early_random. Uniform selection uses rejection sampling to avoid modulo bias. Other platforms retain the original corecrypto calls; SWEET's generic crypto APIs/registration remain untouched. Reference: https://prng.di.unimi.it/splitmix64.c (Sebastiano Vigna, public domain).
- Validation: vm_kern.c compiles to arm64 (`research/kmem-rng-objs`, using `research/run16.log` + `research/bt16`). Undefined symbols include early_random and NO crypto_random_* references. All 13 patches apply in CI order and match the checked source. Original saved as `research/vm_kern.pre0013.c`.
- RNG host checks: extracted actual new helper in `research/kmem-rng-check.c`; 16,384 outputs match the author's downloaded reference (`research/splitmix64-reference.c`), 147,456 bounded outputs satisfy their limits (including 1 and UINT64_MAX), and a forced rejection advances correctly. Full kernel crypto services and the registered kernel PRNG provider remain future work; this patch only supplies the allocator RNG contract.
- Patch-0011 artifacts: kernel `xnu/out-0011/kernel.development.sweet`, log `research/run15.log`, extracted buildtree `research/bt15` (no nested .git). CI log confirms patch 0011 applied; artifact checksums passed. Kernel is plain arm64, entry VA `0xfffffe000746b488` -> PA `0xb146b488`, kernel end PA still `0xb1ff4000`.
- Patch-0011 image: 16,412,672 bytes, SHA256 `29ad5cbfc752473de4af2c471a24a8b34f92da4bff5afec65c614b90cd3ad53a`. Kernel SHA256 `8ac3a956894b09f7bd0d890de8c5de38c65513287feb1e1bcd12db8ca772ee5c`. Payload/ADT in `shim/out/payload-0011.bin`, `shim/out/sweet-0011.adt`. Header unpacked/checked in `shim/out/check-0011`.
- First patch-0011 attempt timed out waiting for FASTBOOT; superseded by the successful send/boot recorded above. Do not repeat the boot before the user reports the current screen.
- Patch-0011 photo: `/home/dipa/Downloads/IMG_20261015_070218.jpg` (local only). Visible stages 1-8 and old red shim text; entry PA B146B488, boot args PA B1FF8000 / top B1FFC000. No evidence yet locating execution after stage 8.
- Patch-0012 sandbox: `research/bt15/xnu` (NO nested .git). Proposed permanent debug VA: VM_MIN_KERNEL_ADDRESS + ARM_TT_L2_SIZE, in the reserved gap below gVirtBase and above low globals; map the same PA framebuffer as Normal-NC in the final TTBR1 before arm_vm_physmap_init changes phystokv(). Keep the early panic sink until normal kprintf startup.
- Patch 0012 validation: all 4 changed C translation units compiled to arm64 objects in `research/post-ttbr0-objs`; internal header export added to osfmk/arm64/Makefile and recreated locally in EXPORT_HDRS for pe_serial.c. All 12 patches apply in CI order and match all 6 edited files. Baselines are saved under `research/pre0012/`.
- Actual mapping-helper host model (`research/early-map-check.c`, barrier stubbed only for host compilation) confirms: VA `0xfffffe0002000000`, L1 index 2016, L2 index 1; descriptor `0x006000009c000605` = framebuffer PA, Normal-NC, outer-shareable, EL1 RW, PXN+UXN. All other L2 entries preserved. The gap is excluded by ARM_LARGE_MEMORY's pmap_virtual_region(0).
- Next boot command line MUST add `-enable_kprintf_spam`: DEVELOPMENT disables kprintf_output by default even when DB_KPRT is set (`pexpert/arm/pe_kprintf.c`). Planned command line: `debug=0xc serial=3 cpus=1 keepsyms=1 -enable_kprintf_spam`.

### Patch 0012 screen legend (after original stages 1-8)
- 9: kernel framebuffer VA live; 10: TTBR0 off / TLB flushed; 11: physmap pointers rebound.
- 12: calling pmap_bootstrap; 13: pmap_bootstrap returned; 14: early I/O page tables ready; 15: arm_vm_init returned.
- 16: calling serial_init; 17: fbcon config read / mapping; 18: fbcon mapping returned; 19: serial_init returned; 20: kprintf startup complete.
- Normal serial initialization clears the screen, so earlier stages may disappear before 19. If serial exists, early console disables after 20; otherwise early panic output stays enabled.
- Local source sandbox: `research/bt13/xnu` (git baseline `623b060`, patches 0001-0008 already in baseline; uncommitted export edits correspond to patch 0009). Preserve those edits when producing patch 0010.
- Previous compile log/tree: `research/run13.log`, `research/bt13`; previous kernel: `xnu/out/kernel.development.sweet`. New successful log/tree: `research/run14.log`, `research/bt14`; new kernel: `xnu/out-0010/kernel.development.sweet`.
- Current device access: patch-0012 image was temporarily started from FASTBOOT; no adb connection is expected. Never flash partitions for bring-up; use `fastboot boot` with the shim image.
- Patch `0010` lives in `xnu/patches/0010-sweet-early-framebuffer-debug.patch`. Its exact compiled sources are under `research/bt13/xnu`; generated objects are in `research/early-objs`.
- Local checks passed: all 5 changed/new translation units compiled to arm64 Mach-O objects; new console only imports `_doprnt` and `iso_font` (both exist in the previous kernel). All 10 patches applied to a temporary upstream git index in CI order, and all 7 resulting changed source files matched the local compile inputs.
- Reproduce changed-file compilation: `XNU_CC_OBJDIR="$PWD/research/early-objs" python3 tools/xnu-cc.py research/run13.log research/bt13 arm_init.c arm_vm_init.c kern/debug.c arm64/start.s osfmk/arm64/sweet_early.c --per-file` (create the output directory first). The helper now accepts explicit new source paths absent from the old artifact's Makefiles and rejects empty selections.
- Device command result: `adb reboot bootloader`, then `timeout 30s fastboot boot shim/out/shim-boot-0010.img`: send OKAY (16028 KB), boot OKAY, total 0.541s. This confirms bootloader acceptance only, not XNU progress.
- The successful boot above was on 2026-10-14. On 2026-10-15, `timeout 45s fastboot boot ...` timed out waiting for a device; no image was sent. Boot image SHA256 rechecked and unchanged. No new build is needed to repeat the trial.
- Earlier retry: `timeout 90s fastboot boot shim/out/shim-boot-0010.img` timed out (exit 124). This is superseded by the successful 2026-10-15 repeat boot recorded above; do not repeat it until the user reports the screen.
- User observation confirms MMU-on C entry, platform expert, CPU topology/bootstrap, startup bootstrap and cpu_init all complete. Failure is within arm_vm_physmap_init (non-XNU_MONITOR branch), before stage 8 / TTBR0 teardown. Old red shim text remains behind the early console; this is expected because only written rows are cleared.
- New edit/compile sandbox is `research/bt14/xnu` (extracted artifact, NO nested .git; `git diff` there would act on the parent project). Use a saved baseline file / generated unified diff, or explicitly initialize a local baseline before editing. `research/bt13/xnu` does have its own baseline git repo.
- Exact user-reported panic: `arm_vm_init.c:1606 Assertion failed: (temp_ptov_table[ptov_index - 1].va + temp_ptov_table[ptov_index - 1].len) <= physmap_end`.
- Root cause: MacOSX build sets `ARM_LARGE_MEMORY=1`; `XNU_MONITOR` is off. Block alignment in arm_vm_physmap_slide can add gaps, but the non-PPL large-memory physmap_end omitted the padding already present in the small-memory branch. Patch 0011 reserves PTOV_TABLE_SIZE*ARM_TT_TWIG_SIZE (8*32MB) in both the aperture bound and its L1 allocation; monitor/PPL builds retain zero padding. The assertion is preserved.
- Validation: `arm_vm_init.c` compiled to an arm64 object using `research/run14.log` + `research/bt14` (object in `research/physmap-objs`). All 11 patches apply in CI order and produce exactly that source. Host harness `research/physmap-check.c` extracts the real XNU slide function with page-table-write stubs: all 65,536 page-aligned slides within 1GB checked against the current kernel memory layout; old bound fails 65,504 cases, padded bound fails zero; worst overrun 0x1ffc000 bytes. Baseline source saved as `research/arm_vm_init.pre0011.c`.
- CI: https://github.com/dipa-studio/ios-sweet/actions/runs/38638702083 (success; artifacts and full log downloaded).
- New image: `shim/out/shim-boot-0010.img` (16,412,672 bytes, SHA256 `ed2c270e0081bad5ff517be415f9bfe8805463aea7866732203fa97ac737e64e`). Previous image preserved as `shim/out/shim-boot-pre0010.img`.
- New kernel SHA256: `3ee05270cc291c49589a101678f904b1b69cb2f2575e05398bce8a4e052f409a`. Artifact checksums passed; arm64 (not arm64e), early-console symbols present. Payload entry VA `0xfffffe000746a488` -> PA `0xb146a488`; kernel end PA `0xb1ff4000`. Command line remains `debug=0xc serial=3 cpus=1 keepsyms=1`.

### Patch 0010 screen legend
- Bottom-left white 64x32 tile: assembly entered `start_first_cpu`, before page tables/MMU.
- Text `SWEET EARLY 1`: entered `arm_init` with MMU on; 2: platform expert done; 3: CPU topology done; 4: CPU bootstrap done; 5: startup bootstrap done; 6: CPU init done; 7: entering `arm_vm_init`; 8: leaving bootstrap TTBR0.
- `SWEET EARLY PANIC:` prints a copy of panic arguments before normal panic handling needs per-CPU/thread state.
- In patches 0010/0011 the console disabled before TTBR0 invalidation; patch 0012 replaces that with a persistent kernel VA alias and diagnostics through normal serial/kprintf startup (see its legend above).
- Screen contents require a physical observation from the user: adb cannot capture a framebuffer after Android has been replaced by a bare-metal kernel.

## QEMU emulation (2026-10-15)
- `tools/mkadt.py OUT.adt --qemu` = same ADT with QEMU virt GICD 0x08000000 / GICR 0x080a0000 / CNTFRQ 62.5 MHz.
- `tools/qemu-sweet.py PAYLOAD OUTDIR [--secs N] [--hmp "x /8gx ADDR"] [--gdb]`: host-side copy of shim/loader.c
  (segments + ADT + boot_args at their PAs, 6-instruction stub sets x0 and jumps), `-M virt,gic-version=3 -cpu cortex-a76`
  (same ID_AA64MMFR0 as Kryo 470), 4G RAM so 0x9c000000 fb and 0xb0000000 kernel are plain RAM.
  Outputs `fb.png` (the screen), `regs.txt`, `qemu.log` (exceptions with ESR/FAR/ELR). `--gdb` halts with a stub on :1234
  (system `gdb` + `set architecture aarch64`, `hbreak *VA` works with the MMU on).
- Not emulated: Qualcomm XPUs, ABL, real panel, PSCI conduit (QEMU virt uses hvc). Hardware still decides those.
- Example: `python3 tools/mkadt.py /tmp/q.adt --qemu && python3 tools/mkpayload.py xnu/out-0013/kernel.development.sweet /tmp/q.adt /tmp/pq.bin --cmdline 'debug=0xc serial=3 cpus=1 keepsyms=1 -enable_kprintf_spam' && python3 tools/qemu-sweet.py /tmp/pq.bin /tmp/q`

## Layout
- `backup/`  partition dumps + full sdb..sdf LUNs, `SHA256SUMS` (all verified on-device == local)
- `dtb/`     live FDT (from OrangeFox), PixelOS boot.img dtb, dtbo, dts versions
- `shim/`    bare-metal boot shim (`./build.sh` -> `out/shim-boot.img`)
- `research/` mainline sm7150 dts, XNU pe_fiq.c etc, PocketDarwin patches
- `tools/unpack_bootv2.py`

## Device access
- No root in PixelOS 17 (user build, adb root disabled). Root = `fastboot boot recovery/recovery.img` (OrangeFox R12.0_2, temp boot, not flashed).
- `fastboot boot` works for stock boot.img, OrangeFox, and our shim.
- Shim resets itself via PSCI SYSTEM_RESET; IMEM restart_reason 0x77665500 is seen
  (bootreason=bootloader) but ABL still boots Android, so it does not stop in fastboot.

## Hardware facts (live DT + shim measurements)
- Framebuffer (ABL cont splash): 0x9c000000, 1080x2400, stride 4320, a8r8g8b8, panel is DSC *video* mode (no kick needed).
- RAM: 0x80000000+2G, 0x100000000+0xfcc00000. Live reserved-memory = PixelOS base dtb + sweet dtbo fixups.
- GICv3: GICD 0x17a00000, GICR 0x17a60000 (stride 0x20000, 8 PEs). GICD_TYPER 0x007B041D: SecurityExtn=1 (DS=0).
  ABL leaves GICD_CTLR = 0x10 (ARE_NS on, groups off). Writing 0x13 reads back 0x12: EnableGrp1A (bit1) is the one that matters.
- GIC Group 1 VERIFIED (shim 2c): vtimer INTID 19 = 30 IRQs/300ms, ptimer INTID 18 = 30, SGI 1 to self = 10/10.
  ICC_SRE=1 already, ICC_CTLR 0x8440, priority/enable writes stick. GICR_IGROUPR0 and GICD_IGROUPRn read 0 from NS
  even though Group 1 delivery works (RAZ in NS view) - do not use them as a health check.
  INTID 28 shows pending constantly (unknown PPI 12).
- Timer 19.2 MHz. Qualcomm PPI numbering: secure-phys/phys/virt/hyp = PPI 1/2/3/0 -> vtimer INTID 19, ptimer 18 (NOT 27/30).
- PSCI 1.1 via SMC, SMCCC 1.1. CPU_ON ok; SYSTEM_SUSPEND and SYSTEM_RESET2 unsupported.
- SMP: CPU_ON works for all 7 secondaries (MPIDR 0x81000n00).
- Entry EL1, MMU off, x0 = DTB (magic ok). ID_AA64MMFR0 = ...101122: 16K granule YES, 4K YES.
- No serial console: debug UART SE is in SPI mode on sweet. Screen is the only debug channel.

## XNU implications
- XNU GICv3 path (pexpert/arm/pe_fiq.c, VMAPPLE) puts the timer (PPI 27) in Group 0 = FIQ. With SecurityExtn=1,
  Group 0 is secure -> impossible from NS EL1. Need: Group 1 IRQ, ICC_IGRPEN1, INTID 19, and the timer handled in the IRQ path.
- XNU VMAPPLE PSCI uses `hvc`; sweet needs `smc`.
- PocketDarwin NonAppleGenericCpu.patch only swaps -mcpu=apple-h7 for -march=armv8-a.

## Milestone 3 (2026-10-14): 16K MMU verified
- ABL loads the Image at 0x80080000 (base 0x80000000 + 0x80000) regardless of text_offset=0.
- 16K granule, T0SZ=28 (L2 start, 32MB blocks), MAIR 0x44ff00, TCR 0x1409CB51C, PARange=2 (40-bit PA).
- Only own image pages WB (Qualcomm XPUs: never map carve-outs cacheable); fb Normal-NC; rest Device.
- AT S1E1R checks all as expected; unmapped -> translation fault L2.
- Fill fb: 110 ms (MMU off) -> 3.5 ms (NC). Memory loop 70 ms -> 0.7 ms. vtimer + SGI IRQs still fine with MMU on.

## XNU loader design notes (xnu-8796)
- Entry: `_start` -> `start_first_cpu`, x0 = boot_args (PA), MMU off, EL1. boot_args: Revision/Version 2,
  virtBase/physBase/memSize/topOfKernelData, Boot_Video (fb 0x9c000000, rowBytes 4320, 1080x2400, depth 32),
  deviceTreeP/Length (ADT), CommandLine[608], bootFlags, memSizeActual.
- start.s maps [kernel mach header .. physBase+memSize) with cacheable BLOCKS. On Qualcomm every XPU carve-out
  must therefore lie below the kernel: use physBase = 0xb0000000 (above ramdump_fb 0xaf000000+16M,
  cdsp_sec, qseecom), end before rmtfs 0xfde01000 -> memSize ~0x4d000000 for bring-up.
- Base ADT: iPhone 8 d20ap DeviceTree from iOS 16.7.16 IPSW (tools/remotezip.py + tools/adt.py; LZFSE payload).
  Template values are empty: the loader must fill chosen/*, memory-map, cpu*/reg etc. like iBoot.
- XNU build: GitHub Actions (dipa-studio/ios-sweet, xnu.yml) via blacktop/darwin-xnu-build, MACOS_VERSION=13.5.
  New-clang fixes in xnu/patches; Xcode 16 iig is incompatible (IORPC.kernelContent) -> macos-14 runner, oldest Xcode.

## Kernel architecture plan (SWEET machine config)
- First stock build (VMAPPLE) is arm64e (PAC). Kryo 470 = ARMv8.2, no PAC -> SWEET must build plain arm64
  (HAS_APPLE_PAC is only defined for __arm64e__, so arm64 drops PAC automatically).
- Template: VMAPPLE (16K pages, GICv3 code in pe_fiq.c) minus APPLEVIRTUALPLATFORM hypercalls/paravirt PAC,
  plus BCM2837 (generic ARM: no APPLE_ARM64_ARCH_FAMILY Apple sysregs). Do not link the KDK lib<cfg>.os.*.a.
- Timer: XNU expects it on FIQ (sleh_fiq -> rtclock_intr). SWEET: GIC Group 1, so sleh_irq must ack ICC_IAR1,
  route INTID 19 to the same timer path, SGIs to cpu_signal_handler, EOI via ICC_EOIR1.
- SMP/interrupts: open-source AppleARMSMP (USE_APPLEARMSMP) needs an IOInterruptController (AIC role) and an
  IOPMGR (CPU power -> PSCI CPU_ON via SMC). Plan: built-in IOKit classes (SweetGIC, SweetPMGR, platform
  expert) compiled into the kernel and matched via iokit/KernelConfigTables.cpp -> no closed kexts needed.
- Debug: xnu patch 0003 sweet-fbcon = kprintf drawn on the framebuffer (ADT node "sweet-fbcon").
- ADT /arm-io ranges must not map to parent 0 (pe_arm_get_soc_base_phys()==0 disables serial): use parent 0x1000000.

## Status 2026-10-14 late: first XNU boot attempt
- SWEET kernel builds (CI run 38635343732, patches 0001-0009). Verified offline: arm64, no PAC, no >v8.2 code,
  70300 classic relocs all in range (slide 0 = no loader work), entry = __start.
- Boot: shim loader worked (initrd 0x846F0000-0x855FE3E0, BOOTARG 0xB1FF8000, JUMP 0xB146A488) but screen stayed
  red -> xnu died/hung before serial_init (fbcon). kprintf is a no-op before STARTUP_SUB_KPRINTF.
- Historical next-step plan (patch 0010, now implemented and built; see active handoff above): early fb debug channel
  1. start.s start_first_cpu (after mov x20,x0 / mov x21,#0): PA marker band (MMU off, direct store to 0x9c000000).
  2. start.s after both create_bootstrap_mapping calls: add ONE block entry to the existing V=P L2 table
     (x25 + 2*PGBYTES; do NOT call create_bootstrap_mapping again: fb and RAM share L1 entry 0, it would replace
     the L2 table). Entry = TYPE_BLOCK|VALID|SH(OUTER)|ATTRINDX(CACHE_ATTRINDX_WRITECOMB=1)|AF|PNX|NX|0x9c000000,
     index = (va & ARM_TT_L2_INDEX_MASK) >> ARM_TT_L2_SHIFT.
  3. new osfmk/arm64/sweet_early.c (+files.arm64 entry): sweet_early_mark(n) colored bands at screen bottom and
     sweet_early_putc using iso_font via VA=PA while the bootstrap TTBR0 is live; disable before
     arm_vm_init's set_mmu_ttb(invalid_ttep) (arm_vm_init.c:2148).
  4. markers in arm_init after PE_init_platform, ml_parse_cpu_topology, cpu_bootstrap, kernel_startup_bootstrap,
     cpu_init, before arm_vm_init.
  5. panic hook at top of panic_trap_to_debugger: va_copy + _doprnt(fmt, &copy, sweet_early_putc, 16).
  Verify with tools/xnu-cc.py (compile+symbols) and patch-order simulation before the single CI run.

- 2026-10-19 (session 5, Claude): **hook 2 plan pinned down (NOT implemented).**
  `iokit/Kernel/arm/SweetBattery.cpp:280` logs
  `SWEET BATTERY: timer unavailable; static fallback` — the last line the deep
  boot showed. Add after it, under `ARM64_BOARD_CONFIG_SWEET` + `NORD2`:
  `extern "C" void nord2_stop_at(unsigned int, const char *);` then
  `nord2_stop_at(2, "after SweetBattery fallback");`.
  Build: compile `iokit/Kernel/arm/SweetBattery.cpp` (clang selects C++ from the
  extension; `nord2-xnu-cc.py` needs no change) and extend the archive replace
  step to handle `.cpp` (`${base%.cpp}.o` -> `SweetBattery.o`; today only
  `%.c`/`%.s` are handled), then add the object to the `ar r` line.
  Test image: the `late` cmdline with `nord2stopat=0002`.
  Also tried and reverted: hook 2 in `bsd/kern/bsd_init.c` — `nord2-xnu-cc.py`
  cannot rebuild bsd objects (component-specific generated flags), file restored
  pristine.

- 2026-10-19 (session 5, Claude): **hook 2 implemented (late2), tool extended.**
  `tools/nord2-xnu-cc.py` now reproduces non-osfmk components: every component
  Makefile's `-include meta_features.h` (CONFIG_DTRACE etc.), all `-D` markers
  from the component Makefile (`BSD_BUILD`, `BSD_KERNEL_PRIVATE`,
  `IOKIT_KERNEL_PRIVATE`, …), `-I<xnu>/osfmk` (so the *patched* osfmk headers
  win over the CI-exported copies) and `-x c++` for `.cpp`. Verified by
  compiling `bsd/kern/bsd_init.c` cleanly and seeing `U _nord2_stop_at`.
  `shim/nord2-xnu/build.sh` now passes the right component per source
  (`comp="${src%%/*}"`) — it previously compiled everything as osfmk.
  Hook 2 lives after `IOKitBSDInit()` in `bsd_init.c` (call in a small block so
  the return value survives) and triggers on `nord2stopat=0002`.
  New image `nord2-xnu-late2.img`, 30.566.400 bytes, SHA-256
  `4180e651c734f02b0fd8ec4dde28f42a3a000baea5df06e28c1b93988f3e1ff6`
  (payload `4034044e…`); 7 images built, all checks pass; patch dry-run OK.

- 2026-10-19 (session 5, Claude): **late2 test: silence right after
  `SWEET BATTERY: static fallback`, then reboot loop** (authorized). The hook 2
  stop was never reached. Diagnosis: the console still addressed the pmsg ring
  and the framebuffer through the identity (V=P) map, which stops being
  reliable once `arm_vm_init`/`pmap` take over the bootstrap tables — so the
  log goes silent while the kernel keeps running into a fault (reset loop).
  Fix: `nord2_phys()` now uses `phystokv()` once `nord2_physmap_active` is set
  by `nord2_vm_gate()` at gate 10 (the first gate after `set_mmu_ttb()`); the
  ring and the framebuffer are then reached through the kernel physmap. The
  restore after the test was verified (`boot` = `04e84c4a…6e1f01`).
  New `nord2-xnu-late2.img`, 30.566.400 bytes, SHA-256
  `855534e238fa091436da5cb72e14704c67f73f1dce194b2e2eaa2e3f09010bb2`;
  7 images, all checks pass; patch dry-run OK.

- 2026-10-19 (session 5, Claude): **identity tables instead of the physmap
  alias.** The physmap-alias test stopped right after marker 10
  (`TTBR0 off; TLB flushed`): XNU's physmap only covers
  `[physBase, physBase+memSize)` = `0x142000000..0x180000000`, so
  `phystokv(0x48160000)` / `phystokv(0x76e10000)` point outside it and fault.
  Fix: `sweet_early.c` builds its **own** L1+L2 tables in BSS
  (`nord2_identity_init()` at the stage-6 stop) covering L2[10] display
  (Device), L2[36] pmsg (Normal-NC), L2[59]/[60] framebuffer (Normal-NC) and
  L2[161..190] the XNU RAM window (Normal WB), and `arm_vm_init.c` puts
  `nord2_l1_pa` in TTBR0 instead of the reusable bootstrap table. pmsg, the
  framebuffer, the trampoline and the cache-clean loop keep working for the
  whole boot.
  New `nord2-xnu-late2.img`, 30.566.400 bytes, SHA-256
  `b2c0977deafc87d1403b9fbbb01e58002934c1003ff3a6d14769678b1e0231c6`;
  7 images, all checks pass; patch dry-run OK.

- 2026-10-19 (session 5, Claude): **identity-table attempt failed, reverted.**
  The private identity tables (`nord2_identity_init`) made the boot stop early
  in `arm_vm_init` (user: last line `NORD2-XNU 7: entering arm_vm_init`), i.e.
  before marker 10, so they were worse than the bootstrap table, which reached
  the battery. Reverted to `set_mmu_ttb(boot_ttep)`, identity-table code left
  dormant (not called). Also learned why hook 2 never fired: it sat in the
  `else` branch of `if (fReal)` in `SweetBattery::start`, and `fReal` is false
  without `sweetbattery=1`; the correct place is right after
  `registerService()` in the same function. `SweetBattery.cpp` still cannot be
  rebuilt: `nord2-xnu-cc.py` gets 70 errors from `IOLib.h`
  (`'T' does not refer to a value`), i.e. the CI's C++/kext flags are not
  reproduced yet; `-fapple-kext` alone does not help. All Sweet IOKit drivers
  match unconditionally on `IOResources` (`KernelConfigTables.cpp`), so they
  cannot be disabled through the ADT.
  - Best-known-good image rebuilt after the revert:
    `nord2-xnu-late2.img` 30.566.400 bytes SHA-256
    `1c99b8192b786b8f5c3d5eff8e80d2172eb65ef93d09647b012fd5b048c443ac`
    (bootstrap V=P table, console through the identity map, hook 2 present in
    `bsd_init.c` but not reachable). 7 images, all checks pass.

- 2026-10-19 (session 7, Opencode): **photo of the session-6 diagnostic image
  analysed (facts).** `/home/frxncy/Downloads/materialgram/IMG_20261019_214059_2.jpg`
  shows the whole `kernel_bootstrap` sequence (`PE_init_iokit`, `early_boot`,
  `vnguard_policy_init`, `dtrace_early_init`, `trust_cache_init`,
  `OSKextRemoveKextBootstrap`, `machine_lockdown`, `kernel_do_post - done`,
  `PE_lockdown_iokit`), then `SWEET: starting device-tree platform expert`,
  platform expert registered, AKS, WDT (logged twice), HID buttons/touch and
  finally `SWEET BATTERY: static fallback, 100% 4200mV external=1 charging=0`.
  **The timer ticks are delivered** (`NORD2-XNU: timer tick 0x1/0x2/0x3`), so
  the "timer IRQ not delivered" hypothesis is disproved. No panic text and no
  `after IOKit BSD init` line: the reset (~1 s later) happens inside
  `IOKitBSDInit()`, after the last Sweet driver, without a printed panic —
  consistent with a hardware reset (watchdog) rather than an XNU panic.
- 2026-10-19 (session 7, Opencode): **WDT + tick-rate diagnostic built (not
  flashed).** `nord2_console_remap()` now maps the MediaTek toprgu watchdog
  (`0x10007000`) and logs `wdt_mode/status/interval`, then pets it
  (`WDT_RESTART = 0x1971`); `nord2_irq_seen()` pets it on every interrupt and
  prints the timer tick every 64 up to 2048 (was every 256), so we can see
  whether ticks continue right up to the reset (hardware WDT) or stop earlier
  (CPU fault/hang). Image `nord2-xnu-late2.img` 30.568.448 bytes SHA-256
  `d12f20e59aca04a6615d8bafd1bd1b0a3fcb0850affdafa63ff4cac69a7f3736`;
  7 images build with `FAILURES: none`; patch regenerated with
  `tools/nord2-mkpatch.sh` (1483 lines, applies cleanly, matches the
  buildtree).

- 2026-10-19 (session 7, Opencode): **WDT-pet test result (image `d12f20e5…`,
  authorized): reset still happens.** Photo: ticks `0x1,0x2,0x3,0x40,0xc0`
  and **no `0x100`** before the reset; last line still
  `SWEET BATTERY: static fallback`. Facts: the timer keeps interrupting until
  at least tick 192, then **interrupts stop** and the SoC resets ~0.5-1 s
  later; petting the MediaTek toprgu (`WDT_RESTART=0x1971`) did **not**
  prevent it, and no panic text appears. Working hypothesis: the kernel enters
  a state with IRQs masked (spinlock/exception) after `SweetBattery` registers,
  and the reset comes from the watchdog/PMIC with no console output.
  - Next diagnostic (built, not flashed): `sleh_irq` now passes the interrupted
    PC to `nord2_irq_seen()`, and every 64th timer tick prints
    `tick 0x… pc=0x…`. The last printed PC names the code path that leads to
    the masked state. Image `nord2-xnu-late2.img` 30.568.448 bytes SHA-256
    `b4699780061b5b9b41e1552b4e1997798239b59c0658bd53d359e4a86c859185`;
    7 images, `FAILURES: none`; patch regenerated with `nord2-mkpatch.sh`.

- 2026-10-19 (session 7, Opencode): **tick-PC test result (image `b4699780…`,
  authorized).** Every logged tick carries `pc=0x…`: ticks 2, 3, 0x40 and 0xc0
  all show `pc=0xfffffe0007a8eca8`, the **idle loop**, while the bootstrap
  thread keeps running (the driver lines continue after the ticks stop). So:
  interrupts stop being delivered after tick 192 although the kernel keeps
  executing; the last pet of the toprgu was then ~1 s before the reset, which
  matches a toprgu timeout. New hypothesis: the timer IRQ stops being
  delivered/re-armed while the system is still alive; because our watchdog pet
  only ran in the IRQ path, the toprgu then reset the SoC.
  - Next (built, not flashed): also pet the toprgu (`WDT_RESTART=0x1971`) on
    **console writes**, so it stays fed even when interrupts stop. Image
    `nord2-xnu-late2.img` 30.568.448 bytes SHA-256
    `e1eb6b1ea91b84489c94884b777a7fa194b8b8f04e05df56676a97916910acf0`;
    7 images, `FAILURES: none`; patch regenerated.

- 2026-10-19 (session 7, Opencode): **pet-on-console test (image `e1eb6b1e…`,
  authorized): reset unchanged.** Same picture: ticks `0x1,0x2,0x3,0x40,0xc0`
  with `pc=0xfffffe0007a8eca8/0xecc0` (the **idle loop**), then the driver lines
  continue while **no more timer ticks arrive**, and ~1 s after
  `SWEET BATTERY: static fallback` the SoC resets with no panic and no pstore.
  Feeding the MediaTek toprgu (`WDT_RESTART=0x1971`) from both the IRQ path and
  every console write does **not** prevent it, so the reset is not the toprgu.
  `androidboot.bootreason=PMIC_cold_reboot` points at the PMIC reset path
  (PMIC watchdog MT6359 fed over pwrap, or a PMIC write from the power stack).
  Verified facts: (a) timer interrupts stop after tick 192 although the kernel
  keeps executing and logging; (b) the reset follows the last Sweet driver;
  (c) no panic is printed (`gPanicBase` is not initialised, but our console
  hook still prints panics, so this is not a plain XNU panic).
  - Open questions for next session: why the timer IRQ stops after 0xc0
    (rtclock re-arm vs lost GIC EOI); whether the PMIC watchdog (MT6359 over
    pwrap) is the reset source; whether the power stack started by
    `SweetBattery::registerService()` writes a PMIC/reset register.
  - Candidates: port a minimal pwrap/MT6359 watchdog pet (or read its status),
    and/or stop `SweetBattery` before `registerService()` (needs the CI C++
    flags in `nord2-xnu-cc.py`, still unresolved), and/or keep the timer alive
    (EOI diagnostics in `sleh_irq`).

- 2026-10-19 (session 7, Opencode): **timer-stall probe built (not flashed).**
  Every 256 console characters, `nord2_putc()` compares the tick counter with a
  snapshot; if it did not advance it prints
  `NORD2-XNU: timer stalled, ctl=0x… tval=0x…` (up to 6 times). Reading
  `CNTV_CTL_EL0`/`CNTV_TVAL_EL0` tells whether the architected timer is still
  armed when the ticks vanish: armed + ISTATUS set → the GIC stops delivering;
  not armed → `rtclock` stopped re-arming. Image `nord2-xnu-late2.img`
  30.568.448 bytes SHA-256
  `280518b9e965c01198cd68a5f3238ca5c938dbc9b1c13df7ce9c84245f9632b9`;
  7 images, `FAILURES: none`; patch regenerated.

- 2026-10-19 (session 7, Opencode): **timer-stall probe result (image
  `280518b9…`, run with user "vai"): dead earlier than before, and no
  `timer stalled` line.** The boot stopped right after
  `SWEET HID: touch registered` (the battery line never printed), i.e. at a
  *different* log position than the previous runs, and it died "while
  printing". Since the probe only reports when 256 console chars pass with an
  unchanged tick counter, the absence of a report means the ticks were still
  arriving shortly before the reset. Conclusion: the failure is
  **time-based and nondeterministic relative to the log**, not tied to a
  specific driver line — consistent with a watchdog expiring after a fixed
  time from boot. The toprgu pet does not stop it, so the remaining candidate
  is the **PMIC (MT6359) watchdog fed over pwrap**, or a PMIC reset write from
  the power stack. `bootreason=PMIC_cold_reboot` supports the PMIC path.
  - Next: give the PMIC watchdog attention over pwrap (needs the MT6359/pwrap
    registers from the live DT) or read its status before the reset; also
    consider a heartbeat/timestamp line each second to measure the exact time
    from boot to reset across runs.

- 2026-10-19 (session 7, Opencode): **WDT registers repeated on ticks (built,
  not flashed).** The startup `wdt_mode/status/interval` lines scrolled out of
  the 32-line band in every photo; they are now reprinted at ticks 0x40, 0x80
  and 0xc0 (i.e. inside the visible tail), so the last screen shows whether
  the MediaTek toprgu is armed and with what interval. Live DT facts:
  `pwrap@10026000` (pwrap + spi_mst at 0x10028000 with spi/wrap/ulposc clocks)
  and the MT6359 node has **`apwdtrst_en = <1>`**, i.e. the PMIC is allowed to
  reset the AP on an AP-watchdog event — consistent with the
  `PMIC_cold_reboot` reason. A real pwrap pet needs a pwrap/SPI driver
  (clocks + command framing), so the cheap next measurement is the toprgu
  state. Image `nord2-xnu-late2.img` 30.568.448 bytes SHA-256
  `2361ded64af5d30914868b46881db2ddcf27420bfd716d6daa38cf5cfc743518`;
  7 images, `FAILURES: none`; patch regenerated.

- 2026-10-19 (session 7, Opencode): **toprgu registers measured (image
  `2361ded6…`, authorized).** On screen: `wdt_mode=0x0000007d` (bit0 = 1, the
  **AP watchdog is enabled**), `wdt_interval=0x00000000`,
  `wdt_status=0x00000000`, repeated at ticks 0x40/0x80/0xc0. Petting the
  toprgu (`WDT_RESTART=0x1971`) both on IRQs and on every console byte still
  does **not** stop the reset, so the AP watchdog either is not the reset
  source or the MT6893 pet path differs. Combined with
  `apwdtrst_en = <1>` in the MT6359 node and `bootreason=PMIC_cold_reboot`,
  the remaining candidate is the **PMIC-side reset path** (MT6359 AP watchdog
  over pwrap, or the power stack started by `SweetBattery::registerService()`).
  - Next session: implement minimal pwrap access (0x10026000 pwrap +
    0x10028000 spi_mst, clocks spi/wrap/ulposc) to read/pet the MT6359 AP
    watchdog, or stop `SweetBattery` before `registerService()` (needs the CI
    C++ flags in `nord2-xnu-cc.py`).

- 2026-10-19 (session 7, Opencode, handoff): **pwrap next-step pointers.**
  The vendor kernel is at
  `/home/frxncy/Documents/Codex/2026-10-19/vedi-questa-repo-https-github-com/work/nord2-current-kernel`
  (NOT `~/nord2-current-kernel`). `pwrap_read`/`pwrap_write` and the reg map
  are in `drivers/soc/mediatek/mtk-pmic-wrap.c`; the PMIC watchdog/AP-reset
  registers are not under `drivers/mfd` or `drivers/watchdog` in this tree, so
  grep the whole tree for `apwdtrst`/`WDTRSTB` (likely
  `drivers/power/supply/` or the MT6359 RTC) before writing the kernel-side
  pet. Nothing was changed in the vendor tree.
  - Correction from the grep: `apwdtrst_en`/`apwdtrst_sel` live in the **MT6360**
    PMU driver (`drivers/misc/mediatek/pmic/mt6360/pmu/mt6360_pmu_core.c`), so
    the DT node with `apwdtrst_en = <1>` is an MT6360 (companion PMIC), not the
    MT6359. That is the most likely AP-reset path: read that driver's register
    addresses/values and its pwrap transport before writing the kernel-side pet.
  - **Concrete MT6360 facts found (session 7, end):**
    `drivers/misc/mediatek/pmic/mt6360/inc/mt6360_pmu.h`:
    `MT6360_PMU_CORE_CTRL1 = 0x01`, `MT6360_PMU_CORE_CTRL2 = 0x06`.
    `mt6360_pmu_core.c`: `apwdtrst_en` = CTRL2 **bit7 (0x80)**,
    `apwdtrst_sel` = CTRL2 bit6 (0x40); `i2cstmr_rst_en` / `i2cstmr_rst_tmr`
    are in CTRL1 (bit fields in the same `mt6360_val_props` table). There is an
    IRQ `ap_wdtrst_evt` with `mt6360_pmu_ap_wdtrst_evt_handler()`, i.e. the
    PMIC deliberately resets the AP on an AP-watchdog/I2C-timeout event, and
    the Nord 2 DT sets `apwdtrst_en = <1>`. **Most likely reset path: the
    MT6360's I2C/AP watchdog timing out because the kernel never talks to the
    PMIC over pwrap — time-based and log-position independent, exactly what we
    observed.**
    Next: implement minimal pwrap read/write (mtk-pmic-wrap.c), read CTRL1/
    CTRL2, clear `apwdtrst_en` (CTRL2 bit7) and the CTRL1 I2C-timeout enable,
    then re-run the deep boot.

- 2026-10-19 (session 7, Opencode): **ROOT CAUSE of the reset: the toprgu pet was
  writing the wrong register.** The MediaTek toprgu layout (vendor
  `aee/hangdet/aee_hangdet.c` and `watchdog/mtk_wdt.c`) is
  `WDT_MODE = 0x00` (bit0 = enable), `WDT_LENGTH = 0x04`,
  **`WDT_RST = 0x08` with magic `0x1971`**, `WDT_STATUS = 0x0c`. Our code wrote
  `0x1971` to **0x04 (LENGTH)** and read "status"/"interval" at 0x08/0x0c, so
  the AP watchdog was **never kicked** — and the Nord 2 DT enables the MT6360
  AP-watchdog reset input (`apwdtrst_en = <1>`, MT6360 on I2C, not pwrap).
  That explains every observation: `wdt_mode = 0x7d` (enabled), `wdt_length`
  read from the wrong register showed 0, petting had no effect, the reset is
  time-based and independent of the log, and `bootreason = PMIC_cold_reboot`.
  Fix: kick `WDT_RST` (offset 0x08) and log `MODE/LENGTH/STATUS` from their
  real offsets. Image `nord2-xnu-late2.img` 30.568.448 bytes SHA-256
  `dadfea1d8709c96e4b246991c9139744278286b3b6ad635be48e38c869849ee4`;
  7 images, `FAILURES: none`; patch regenerated.

- 2026-10-19 (session 7, Opencode): **WDT pet fix works; the C++ path is
  impossible on this host.** Test `dadfea1d…` (authorized): ticks now go past
  `0xc0` (seen `0x100`), `wdt_mode=0x7d`, `wdt_length=0x000f0000`,
  `wdt_status=0` — the AP watchdog is fed. The reset still comes, but **about
  10 s after** `SWEET BATTERY: static fallback` (user), matching the ~0x000f0000
  timeout: after the battery line the kernel **hangs** (no further console
  writes → no pets) and the watchdog then resets it. So the remaining bug is a
  genuine hang in `SweetBattery::registerService()` / the IOKit C++ path.
  - **Blocker confirmed (not a flag problem):** `IOLib.h` uses Apple-only
    builtins `__builtin_xnu_type_summary()` and
    `__builtin_xnu_types_compatible()` (preprocessed expansion checked). They
    exist only in Apple's clang, which is not available on this Linux host, so
    **no Sweet `.cpp` object can be rebuilt here** — instrumenting or gating
    `SweetBattery.cpp` is not possible with the current toolchain.
  - Consequences: the next step must stay in C (osfmk/bsd/pexpert/sleh) or
    avoid the C++ drivers entirely. Options: (a) find the hang from the CPU
    side (keep the timer alive so the tick handler can print the PC during the
    hang; the timer currently stops before the battery line); (b) a C-side
    workaround that prevents the power stack from being started by
    `registerService()` (e.g. an IOKit boot-arg/`IOKitBSDInit` pre-check);
    (c) port a minimal MT6360 I2C driver and clear `apwdtrst_en` (the MT6360 is
    an I2C device, so pwrap is not the path).

- 2026-10-19 (session 7, Opencode): **C++ BLOCKER SOLVED.** `IOLib.h`'s
  `__builtin_xnu_type_summary/__builtin_xnu_types_compatible/...` can be
  satisfied with three preprocessor fallbacks
  (`-D__builtin_xnu_type_summary(x)=0xffff`,
  `-D__builtin_xnu_types_compatible(a,b)=1`,
  `-D__builtin_xnu_type_signature(x)="NORD"`): the templates that use them are
  parsed but not instantiated by `SweetBattery.cpp`, so the object is clean.
  `shim/nord2-xnu/build.sh` now builds `iokit/Kernel/arm/SweetBattery.cpp`
  (component `iokit`, C++ via `-x c++`) and replaces the archive member
  **`SweetBattery.cpo`** (not `.o` — it is a `.cpp`, so `ar d` needed the
  `.cpo` name; 30 duplicate symbols proved it). `SweetBattery::start()` now has
  two hooks: `nord2_stop_at(2, "before SweetBattery registerService")` and
  `nord2_stop_at(3, "after …")`. Image `nord2-xnu-late2.img`
  (cmdline `nord2stopat=0002`) 30.568.448 bytes SHA-256
  `5a3aed311685bcf109a0369923995c587d7a4c857aeee86170385a9b2bf85c1e`;
  7 images, `FAILURES: none`; patch regenerated.
  - Expected: stop **before** `registerService` → Android returns. If it does,
    the post-battery hang is at/after `registerService` and a `nord2stopat=0003`
    run will tell whether it is inside it.

- 2026-10-19 (session 7, Opencode): **the local C++ build path was reverted on
  purpose.** The project README (§1 "Build the kernel") is explicit: the
  kernel is built on GitHub's macOS runners with blacktop/darwin-xnu-build,
  applying `xnu/patches/`; local `tools/xnu-cc.py` is only for single-file
  checks. The `__builtin_xnu_*` fallbacks made SweetBattery.cpp compile with
  upstream clang, but the resulting `__kalloc_type` metadata (signature/flags)
  would not be what Apple clang emits, and the kernel validates those views at
  boot — an unsanctioned and potentially unsafe shortcut. Reverted:
  `SweetBattery.cpp` is pristine, `build.sh`/`nord2-mkpatch.sh` no longer
  reference it, and the rebuild reproduces the known-good image
  `dadfea1d…` exactly. C++ instrumentation (the hook around
  `SweetBattery::registerService()`) must go through the **CI build**
  (`xnu/patches/` + `gh workflow run xnu.yml`), not the local relink.

- 2026-10-19 (session 7, Opencode): **CI route prepared (README §1).** Patch
  0082 now also contains (a) the two `nord2_stop_at()` hooks around
  `SweetBattery::registerService()` (CI-only instrumentation, Apple clang) and
  (b) `#define NORD2 1` in `pexpert/pexpert/arm64/SWEET.h`, because the CI build
  does not pass `-DNORD2` and would otherwise compile the Nord2 code out.
  Patch regenerated (1552 lines, applies cleanly, matches the buildtree),
  commit `28904d9`. `gh` is not installed and not authenticated here, so the
  workflow must be run by the user:
  1. push this branch to a fork (public forks get free macOS minutes);
  2. Actions -> `xnu.yml` -> Run workflow (or `gh workflow run xnu.yml -R FORK`);
  3. download the artifact (`out-XXXX` with the built kernel and buildtree);
  4. hand it over: the Nord 2 image is then
     `strip -S -x kernel.development.sweet` -> `tools/mkpayload.py` with
     `tools/mkadt-nord2.py` -> `tools/nord2-pack-xnu.py` -> `tools/nord2-check-xnu.py`.
  Nothing has been pushed from this machine.

- 2026-10-20 (session 7, Opencode): **CI BUILD PASSED and the CI kernel is
  packaged.** Run `39331965998` on `frxncyyy/ios-sweet` (branch
  `codex/nord2-bringup`, macOS runner, 6m58s) with patch 0082 (NORD2 in
  `SWEET.h`, SweetBattery hooks, unconditional `sweet_early.h` include in
  `bsd_init.c`). Artifacts in `research/ci-39331965998/`; kernel
  `kernel.development.sweet` 17.223.720 bytes, contains `nord2_stop_at`,
  `nord2_console_remap`, `nord2_vm_gate` and the SweetBattery globals.
  Image built with `tools/nord2-image-from-kernel.sh` (strip, ADT, payload,
  pack, check): `shim/out/nord2-xnu/nord2-ci.img` **30.676.992 bytes**,
  SHA-256 `dd39cbd46a14d4efba75897d4ccaaade7aac774a63d62ed62c90f8282366ab49`,
  payload `9cca1181…`, cmdline `nord2stopat=0002`; `FAILURES: none`.
  - Expected: stop **before** `SweetBattery::registerService()` (the CI hook)
    and return to Android. If it does, the post-battery hang is at/after
    `registerService`; a run with `nord2stopat=0003` will say whether it is
    inside it.

- 2026-10-20 (session 7, Opencode): **CI hook works: stop BEFORE
  `registerService` returns to Android.** Image `nord2-ci.img` (`dd39cbd4…`,
  authorized) booted, printed `chain-back cached` then
  `stop at before SweetBattery registerService`, chained back and Android
  reached `sys.boot_completed=1` in ~71 s; log
  `backup/nord2/logs/20261020-001810-ci-before-register` (10.5 KiB, full deep
  boot). So the CI-built (Apple clang) C++ kernel and the stop hook are sound,
  and the post-battery hang is **at or after `registerService()`**.
  - Next: `nord2-ci-after.img` (same kernel, `nord2stopat=0003`, stop *after*
    `registerService`) 30.676.992 bytes SHA-256
    `7d73673620c91123a46c8da41f6b6c75eb88a5697cb611c8a93fa989268b5e70`,
    payload `e23bd7cf…`, `FAILURES: none`. If it returns, the hang is later;
    if it hangs, it is inside `registerService()`.

- 2026-10-20 (session 7, Opencode): **the after-hook also returns: the hang is
  NOT in `registerService()`.** `nord2-ci-after.img` (`7d736736…`, authorized)
  printed `stop at after SweetBattery registerService` → `RETURN TO STOCK
  ANDROID KERNEL` → Android up (~71 s); log
  `backup/nord2/logs/*ci-after-register`. So `SweetBattery::start()` completes
  and the hang happens **after it returns**, inside the same
  IOKit/STARTUP entry: no further `kernel_bootstrap` step is logged after
  `PE_lockdown_iokit` (the Sweet drivers run inside that step).
  - `IOKitBSDInit()` is only `IOKitBSDInit.cpp:publishResource("IOBSD")`, so a
    hook there is useless. The driver matching happens inside the
    STARTUP_SUB_IOKIT startup entry (C++), which is now CI-buildable.
  - Next: add `nord2_stop_at(4, …)` at the end of the IOKit
    matching/start loop (or right after `kernel_startup_initialize_upto(
    STARTUP_SUB_IOKIT)` in `startup.c`, C, cheaper) and bisect with
    `nord2stopat=0004`; then the next driver/step after `SweetBattery` will
    name the actual hang.

- 2026-10-20 (session 7, Opencode): **point-4 hook built via CI.** Run
  `39336045043` (7m52s, after one infrastructure flake: `hdiutil couldn't eject
  disk4`) includes `nord2_stop_at(4, "after StartIOKitMatching")` in
  `iokit/Kernel/IOStartIOKit.cpp`. Kernel
  `research/ci-39336045043/xnu-kernel/kernel.development.sweet` 17.240.104
  bytes; image `shim/out/nord2-xnu/nord2-ci-4.img` **30.676.992 bytes** SHA-256
  `787aa7548df2a5c5dbbbc8fab1388a0e38591f49d45bde53a2a234be60322d70`, payload
  `d7692637…`, `FAILURES: none`.
  - Expected: if it returns to Android, the IOKit matching completes and the
    hang is later (`bsd_init`); if it hangs, the hang is inside
    `StartIOKitMatching()` and the next step is a per-driver log (class name) in
    the matching loop.

- 2026-10-20 (session 7, Opencode): **point 4 returns: the IOKit matching
  completes.** `nord2-ci-4.img` (`787aa754…`, authorized) printed
  `stop at after StartIOKitMatching` → Android up (~71 s). So the post-battery
  hang is **after** the matching, in `bsd_init()` (C, buildable locally).
  Added local hooks (no CI needed): point 5 = `bsd_init` entry, point 6 =
  right after `printf(copyright)`. Local image with `nord2stopat=0005`:
  `shim/out/nord2-xnu/nord2-l5.img` **30.570.496 bytes** SHA-256
  `be5ee58d9b8c562d9dbd526a66c5fe2728ec460d47b9c311e3114f70766ba99c`,
  payload `3a8dcb28…`, `FAILURES: none`.
  - Expected: if 0005 returns, the hang is inside `bsd_init` and 0006 will say
    whether it is before or after the copyright print; if 0005 hangs, the hang
    is between `StartIOKitMatching()` and `bsd_init` entry
    (`vm_set_restrictions`/`kperf_init_early`).

- 2026-10-20 (session 7, Opencode): **point 5 (bsd_init entry) HANGS → the hang
  is before `bsd_init`, between `StartIOKitMatching()` and bsd_init.** Restore
  verified (`boot` = `04e84c4a…6e1f01`, Android up). Remaining region in
  `kernel_bootstrap` (startup.c:786-796):
  ```c
  vm_set_restrictions(machine_info.max_cpus);   /* trivial, no hang possible */
  #if KPERF
      kperf_init_early();                        /* <- only nontrivial call left */
  #endif
      bsd_init();
  ```
  `vm_set_restrictions` (vm_pageout.c:4974) only reads a boot arg and sets a
  flag. So the hang is either `kperf_init_early()` (if KPERF is enabled) or the
  very first instructions of `bsd_init()` before the point-5 hook (prologue /
  stack), though the latter is only initialisation.
  - Next: build with a point 7 hook **after `vm_set_restrictions`** and a point 8
    **after `kperf_init_early`** (or simply after `kperf_init_early` before
    `bsd_init`) to split the last two candidates; both are C and local.

- 2026-10-20 (session 7, Opencode): **hooks 7/8 built (local).** `startup.c` got
  (unconditional) `<arm64/sweet_early.h>`, `nord2_stop_at(7, "after
  vm_set_restrictions")` and `nord2_stop_at(8, "before bsd_init")`. Image
  `shim/out/nord2-xnu/nord2-l7.img` **30.570.496 bytes** SHA-256
  `8013ad20dac68c58d9bf8a0a69492b4900884ba1ff67d70e8fbe2900f1318d59`,
  payload `555aa0e0…`, `FAILURES: none`.
  - Expected: if 0007 hangs, the stall is in `PE_lockdown_iokit`'s return path
    (after `StartIOKitMatching` returned) or `vm_set_restrictions`; if it
    returns, the stall is `kperf_init_early()` and 0008 pins it.

- 2026-10-20 (session 7, Opencode): **point 7 returns to Android (~92 s)** →
  `vm_set_restrictions` and the `PE_lockdown_iokit` return path are fine; the
  only remaining call before `bsd_init` is **`kperf_init_early()`**. Point 8
  (right after it, before `bsd_init`) is in the same kernel, so only a new
  payload is needed: `shim/out/nord2-xnu/nord2-l8.img` (`nord2stopat=0008`) —
  see below for size/SHA. If 0008 hangs, `kperf_init_early()` is the stall; if
  it returns, the stall is in `bsd_init`'s prologue before point 5.

- 2026-10-20 (session 7, Opencode): **ROOT CAUSE OF THE POST-"SWEET BATTERY"
  HANG FOUND: `kperf_init_early()` → `ml_wait_max_cpus()`.**
  - Point 7 (`after vm_set_restrictions`) returns to Android (~92 s); point 8
    (`before bsd_init`, right after `kperf_init_early()`) hangs →
    `kperf_init_early()` is the stall.
  - Source (`osfmk/kperf/kperf.c:77`): `kperf_init_early()` starts with
    `ml_wait_max_cpus();` — it **spins until `machine_info.max_cpus` CPUs have
    checked in**. Our ADT advertises **8 CPU nodes** and the probe boots with
    `cpus=1` (only CPU0 online), so the wait never completes: that is the
    silent, timer-starving stall right after the IOKit matching, and the AP
    watchdog (`WDT_RST` now correctly kicked, ~10 s timeout) resets the SoC
    with `PMIC_cold_reboot`.
  - Fixes to try next (cheap, no CI):
    1. boot with `cpus=8` (PSCI bring-up of all 8 cores is already supported by
       the Sweet port, patch 0065) so `ml_wait_max_cpus()` completes; or
    2. for `cpus=1` probes, stop the spin: gate `kperf_init_early()` (or
       `ml_wait_max_cpus()`) on NORD2 when only one CPU is requested, e.g.
       skip the wait when `machine_info.max_cpus > ml_get_cpu_count()`, or
    3. add `kperf` to the probe cmdline? (not needed: the wait is unconditional).
  - **Phone restored** after the test (`boot` = `04e84c4a…6e1f01`), Android up.
  - Images/tests of this batch: `nord2-l7.img` (`8013ad20…`) returns;
    `nord2-l8.img` (`0feb4942…`) hangs; CI runs 39331965998 (before/after
    SweetBattery hooks), 39336045043 (after StartIOKitMatching). All committed.
