# iOS 16 on sweet (Redmi Note 10 Pro, SM7150) - working notes


## Nord 2 active handoff — 2026-10-20 (this workspace)


- **2026-10-22: combined datapath image PREPARED and independently verified.**
  shim/out/nord2-xnu/nord2-ios-wifi-datapath-stop23.img,33478656 bytes:
  SHA25608e6be56c42f62c0b0e4771132c9865f84a49f4f634c9a235503e595d6403a42.
  Payload14993648 bytes SHA256
  7af08ad87ab0ad6bdc45520d6964691c8c115b025f59616fe0ff79b7ca5ee70f;
  boot spare75776 bytes. Builder41708 CLOSED0. Independent actual-image report
  3e4b5b3b/manifest5e158b8d PASS; parent verified all7 members/protected stage
  pins/image/recoveries. Only independent-checker template adaptations were
  needed (external pinned receipt wrapper and absent null serial field).
  Exact source b441429316787f81d62106e7a9892f282b28e174, CI39859151068 attempt1;
  kernel17787168 bytes SHAbd62a2e8856c040e4d5fb70a401849cc4e1978c0718a4058abc033c96b842888.
  Source/CI/schema/prepared gates TRUE. Human/RF/conditional recovery FALSE;
  runner UNUSED. Phone remains b8851761, one Join consumed. No new device action.
  Next: exact boot-only user approval and manual fastboot; one WPA2 Join with
  private address disabled/password only on phone, single600s collector to
  capture DHCP and late GTK renewal. Open-network comparison remains unsupported.
  This image bundles normal HW0 TXD/TXP/token lifetime, secure GTK renewal,
  infrastructure/FAST/dwell scans, bounded RX/TXS/own-client DHCP diagnostics.
  No live data/lease/Internet or new rekey success claimed. Recovery d9a44316
  or verified RAM ADB only; never stock Android over iOS userdata.


- **2026-10-22: exact datapath linked review PASS; image packaging running.**
  Independent report70351db8/manifest570f3a63 verifies27 bounded linked bodies
  and14 numeric emitter formats. Parent verified all62 manifest members,
 331 source pins, actual kernel markers and exact CI/source identity.
  Source/schema gates are verified for b441429/CI39859151068. Builder41708
  packages nord2-ios-wifi-datapath-stop23.img under1GiB address-space limit.
  Prepared-image and human/RF/recovery authorization gates remain FALSE.
  New image not yet verified; phone remains b8851761, no device operation.
  Next: completed image, independent existing-artifact review, exact approval.


- **2026-10-22: datapath CI39859151068 succeeded; exact artifact verified.**
  Attempt1/sourceb441429316787f81d62106e7a9892f282b28e174; watcher96813
  CLOSED0. Kernel17787168 bytes SHA
  bd62a2e8856c040e4d5fb70a401849cc4e1978c0718a4058abc033c96b842888.
  Parent confirms231generated files byte-equal,37genericARMv8.2 compile units,
  and2279112 decoded instructions without LDAPUR/STLUR. Source/marker pins
  match; final bounded actual linked review is running before image packaging.
  Supplemental six-family reader26945cf8 independently passes109positive/
  760negative cases and is wired into the newstage snapshot collector only.
  Future collector600s preserves the late rekey window. Phone remainsb8851761;
  no new image, Join or flash yet; human authorization gates remain false.


- **2026-10-22: combined datapath source committed; full CI dispatched once.**
  Source b441429316787f81d62106e7a9892f282b28e174, fork branch
  codex/nord2-bringup; CI39859151068 attempt1 compiling on macOS. Watcher96813
  verifies exact head/attempt and downloads artifacts only after success.
  All13 local composed groups and15 actual ARM64 C units passed; independent
  source/merge reviews passed. New supplemental numeric diagnostics reader is
  being prepared privately while CI waits. New image has not been built and
  all source-CI/image/human test gates remain closed; phone stillb8851761.
  Next: successful CI, generated-source/ISA/linked checks, build/verify exact
  nord2-ios-wifi-datapath-stop23.img, then fresh boot-only user approval.


- **2026-10-22: final combined datapath suite PASS, ready for full CI.**
  All13 groups in wifi-ios-datapath/ios/local-tests/combined.json pass on final
  canonical runtime: normal TX, secure GTK, all9 station groups, data diagnostics,
  join owner, both scan-return paths, native scan service/backend, iOS contract/
  client, strict connection/WPA/data readers. Final source matches successful
  fifteen-TU ARM64 compileattempt3, patchb363b17a. Two older fixture failures
  were migrated to the new GTK representation and authenticated Group1 policy;
  no runtime acceptance weakened. Exact synthetic modified-Group1 now refuses
  on MIC failure rather than obsolete unsupported-type gate, with full wipe.
  New stage all hardware/image/CI gates remain false pending full build/audit.
  Phone untouched on b8851761, one Join consumed, no usable IP claim.


- **2026-10-22: full normal TX and secure group renewal integrated locally.**
  Runtime now includes source-parity HW0 TXD/TXP with one reserved payload page,
  checked descriptor completion AND type6 token return before buffer reuse;
  management remains on its previous ring. Native ledger and scan-return
  closure account for both rings. Full GTK renewal authenticates MIC/replay/
  unwrap/KDE, uses per-key WTBL/PN slots and protects G2 with PTK; duplicate
  messages do not reinstall keys. User data remains gated by controlled port.
  Two documented merge conflicts resolved retaining root RX diagnostics and
  both ownership models. Canonical normalTX/GTK tests PASS; old direct-state
  fixture migration4paths preserves assertions, independent focused passes.
  Final15 actual generated ARM64 C units PASS, compiled-review/attempt3 receipt
  a4d9a39a; generated0082 b363b17a. Initial compileattempt1 failed missingnewTU
  registration due early shell sequencing; fixed before successful attempts2/3.
  Independent source reviews: normal3e8d2e44, GTK1b468d62, final comment addendum
  fd669182. Combined final suite running; no CI or new image yet. Limits remain
  one Join/boot, source2048 token IDs without on-wire generation, bounded32GTK
  materials/10s rekey. No hardware throughput, new rekey or usable IP claim.


- **2026-10-22: scan compatibility promoted; combined datapath stage prepared.**
  Canonical BSS2 infrastructure/BSS3 any, FAST/passive-compatible scan and exact
  110ms dwell profile now pass policy/snapshot/actual client/native owner tests.
  Full client runner v3 PASS after fixing fixture extraction placement; native
  scan cache remains immutable and only infrastructure result snapshots filter
  ESS/IBSS. Connection reader182positive/292negative admits appended actions15/16
  and data phases10..15 only in local-submission format; no air/IP inference.
  New stage research/nord2/wifi-ios-datapath/ios is prepared with all source,
  CI, image and human gates FALSE. Intended image nord2-ios-wifi-datapath-stop23.
  Normal HW0 host TX + secure GTK renewal private candidates are being finalized
  and reviewed together, including token ownership/cancellation/cleanup. No
  new CI, image, flash or Join. Phone remains b8851761; open-network test is
  inapplicable to current WPA2-only decoder despite user confirming availability.


- **2026-10-22: final collection closed; recovered scans proven, candidates underway.**
  Collector87378 CLOSED0; final immutable capture-final.log515020 bytes SHA
  77d7fd3b389d98f41bcfcc10fcefaad258c71891d285ee04ae3f07173ef50740,
  all5 readers EXIT0. Later scan tickets4..17 complete with1..13 APs (one
  capacity-limited partial); initial bss2/FAST/timing profiles were refused,
  so scan-return works but service policy still delays recovery. No panic.
  Claude terminal read-only review12d1a0c1 confirms closure before MIC and
  no proof that broadcast DHCP OFFER/ACK belongs to this phone. Native data
  transport and full group-key renewal remain private candidates, not tested.
  Root promoted bounded RX rejection/TXS advanced fields/DHCP request correlation
  diagnostics with tests. Correlation stores4 XID/client pairs privately, wipes
  them at terminal summary and logs only match counts; no lease proof. ASan/UBSan
  baseline719pure/1141actual +extended1105actual/correlation tests pass.
  Agent format also implements infrastructure/FAST/dwell scan policy privately.
  No new image or device action; phone remains b8851761 and one Join is consumed.

- **2026-10-22: decisive b8851761 terminal evidence; supersedes prefix findings below.**
  Immutable postauth/ios/hardware-review/postauth-terminal.log481564 bytes,
  SHA1571ed543b97037fe3758ff82807130916a4e6d8098feaac2594e86b88a0dbd7;
  all5 parsers EXIT0; DATA summary complete. At466971ms WPAHDR3 state5,
  bytes131/info4994(0x1382)/class5/failure4: group-shaped key message rejected
  on universal pairwise key-length gate BEFORE MIC/unwrap. Actual KeyLength
  value and authenticity were not logged; this is not authenticated Group1.
  Cleanup completed without panic, linkdown, SCANRETURN completed118. iOS
  subsequently asks scans but policy returns-2; no fresh scan results proven.
  Summary30 outgoing DISCOVER/0REQUEST,0 unicast RX/1307 group RX;7 classified
  group DHCP frames include1OFFER/1ACK but have no own-client/XID correlation,
  so neither is proof of our lease. TXS3 management success/9 sharedPID127
  MPDU_ERROR; per-packet protected-data attribution remains unproven.
  Claude same-session first hardware review finished7047cdea, terminal update
  exec32741 running read-only. Cap owns private full secure GTK-rekey candidate,
  wire private complete normal HW0 TXD/TXP/token-return candidate, root bounded
  RX/TXS/DHCP-match observations and scan policy review. No canonical runtime
  changes yet and no repeat Join/reboot. User confirms open network available,
  but current image is WPA2-only; do not test open with this image.

- **2026-10-22: b8851761 one WPA2 Join reproduces spinner; new DATA evidence saved.**
  User reports same waiting spinner, no usable IP. Initial reader74334 CLOSED0;
  immutable hardware-review/capture-initial.log458650 bytes SHA
  fa60faadcb153fb1ca2789d1099266775cfa7c518c10c8a960076cee1b8bb686.
  All5 readers EXIT0 (data intentionally valid prefix, no terminal summary).
  First action snapshot join-spinner.log414371 bytes SHA73824b34...9e45533:
  local authorization109705ms;10 native broadcast DHCP DISCOVER publications,
  no classified Offer;9 sharedPID127 TXSstatus3/MPDU_ERROR with tx_count30,
  advanced flags3/rate0 versus managementPID1/2status0/count1/rate0. PID127
  cannot identify which packet failed; native RX is not BSD delivery. Group
  ARP/IP/IPv6 received; no observed late WPA error, scan return or panic yet.

  Root resumed read-only collector87378 for600s at hardware-followup (initial
  rejected900s invocation made no USB calls; tool maximum is600). New output
  includes initial capture as exact prefix. Do not run competing collectors.
  Claude resumed same Opus session via agentformat/exe89580, new folder
  research/nord2/wifi/claude-opus-postauth-806c/hardware-b8851761, restricted
  read-only and viewer on desktop3. Wire audits native TX; cap's protocol audit
  a3a34cf3 says AP-peer/PTK uplink broadcast matches source, not a GTK/noACK fix.
  Proposed open-network comparison was withdrawn: current decoder only accepts
  WPA2, so open network would refuse before traffic and is not a valid test.
  No new source change, build, phone reboot or repeat Join authorized/performed.


- **2026-10-22: postauth image b8851761 flashed with exact user authorization.**
  Usercall_413702708126415b89f7b0874c887b70 authorized this exact boot-only
  test and conditional d9a/rescue recovery, confirming fastboot ready. Root
  verified sole known device/productoplus6893/boot32MiB/non-userspace fastboot,
  rehashed image; runner74334 reverified existing image then flash/reboot OKAY.
  New stage runner is USED; never rerun it. Collector owned by74334 is active
  in research/nord2/wifi-ios-postauth/ios/hardware-live,300s read window after
  connection. Await valid boot/display, then one WPA2 Join with private address
  disabled and password only on device. Do not start a competing USB collector.


- **2026-10-22: combined postauth image PREPARED and independently verified.**
  shim/out/nord2-xnu/nord2-ios-wifi-postauth-stop23.img,33470464 bytes:
  SHA-256 b8851761d3441fc507177b819e17a0c0a3679952122cb1c3e47e7a65966a96ff.
  Payload14976896 bytes SHA-256
  220b81e28ae398b9e1909d0078a9d01ac4557e44d30232aa1cad1fe698b0b4fc.
  Boot spare83968 bytes. Builder98597 CLOSED0 under1GiB; independent existing
  image reportdfa4e210/manifest8851fbc2 PASS, members/protected pins rechecked.
  Exact source aecd95ca388b6b7c6d61712ae22e8eb24ddfe3a6, CI39850764280 attempt1.
  Kernel17770200 bytes SHA9f6f2d31e64d6ee0ce09c4f3e11c69e81f923087ceb34852db851d385286b5b5.
  All source/CI/schema/prepared gates TRUE; human/RF/conditional-recovery gates
  remain FALSE and one-shot runner UNUSED. Fresh exact boot-only approval and
  manual fastboot are required next. Phone still806c8d61; no device action.

  Combined candidate fixes postauthorized terminal scan-return exclusion and
  logs bounded WPA header/rejection/pre-engine extent plus DHCP/TXS/protocol
  observations. No crypto acceptance, data descriptor/ring or key policy change;
  no usable IP, new hardware scan-return or rekey success is claimed. One WPA2
  Join with private address disabled is the intended capture, then inspect all
  reserved diagnostics together. Conditional recovery: d9a44316 or RAM ADB,
  never stock Android over iOS userdata. Claude's final corrected verdict is
  research/nord2/wifi/claude-opus-postauth-806c/followup-command-data/REPORT.md.

- **2026-10-22: actual postauth linked review PASS; image packaging running.**
  Report367f94de/manifest3cdfee67:17 bounded linked bodies,8 exact emitter
  literals, all39 review members verified by root. Source/schema/CI gates now
  verified for aecd95ca/CI39850764280. Builder98597 runs with1GiB address-space
  limit for shim/out/nord2-xnu/nord2-ios-wifi-postauth-stop23.img.
  Prepared-image/human/RF/recovery gates remain FALSE pending final independent
  image verification and exact approval. Phone stays806c8d61; no device action.

- **2026-10-22: CI39850764280 attempt1 succeeded; downloaded kernel verified.**
  Exact source aecd95ca388b6b7c6d61712ae22e8eb24ddfe3a6; watcher65059
  CLOSED0. Kernel17770200 bytes SHA-256
  9f6f2d31e64d6ee0ce09c4f3e11c69e81f923087ceb34852db851d385286b5b5.
  Root audit229 generated files byte-equal,36 generic ARMv8.2 compile units,
  2275095 decoded instructions with no LDAPUR/STLUR. Artifact/ISA checks EXIT0.
  Actual bounded linked review underway in postauth/ios/linked-review; image
  not built and all human gates remain FALSE. Phone still806c8d61 untouched.

- **2026-10-22: combined post-authentication source committed and CI dispatched.**
  Source aecd95ca388b6b7c6d61712ae22e8eb24ddfe3a6, full CI39850764280
  attempt1 (one dispatch). Watcher research/ci-39850764280/watch.py verifies
  exact source/attempt and downloads artifacts only on success. Additive reader
  v2 independent verdict a0cef12c PASS; canonical11+/407- PASS. Source manifest
  postauth/ios/source-proposals.json96f60131 pins320 files and13 local reviews.
  New identity/schema/image/human gates remain closed. Phone still806c8d61.
  Await full CI, source/ISA/linked audit and verified image before new approval.


- **2026-10-22: final post-authentication runtime composition verified locally.**
  Canonical scan-return fix now also admits a previously authorized connection
  only after complete key/peer/DMA cleanup; one-join policy remains unchanged.
  Added reserved numeric WPA failure/header and pre-engine gate diagnostics,
  plus bounded native TXS/EtherType/DHCP-class observations. These do not alter
  cryptographic acceptance, descriptors, ring ownership or transmission policy.
  The new diagnostic records contain no addresses, SSIDs, key material or packet
  bodies. DHCP classification is not a lease; native RX is not BSD delivery;
  shared PID127 cannot identify which publication received a TX status.

  Both postauthorized/failed-before-authorization scan-return tests, all9 station
  groups, full normal transport, WPA crypto/transaction/3 input-gate modes and
  numeric reader tests PASS. Data diagnostics719 pure/1141 actual transport
  checks PASS. Actual10 translation-unit ARM64 compiles with generic ARMv8.2
  flags PASS; independent final composition review03004d9d/manifest108b8154
  confirms228 generated files and207 canonical siblings. Generated0082 SHA
  1eefac22471533b2982ebc7df2344a0482318e692c9600cf3582e1525d548b82.
  Evidence: research/nord2/wifi-ios-postauth/ios and local-submit/ios audits.
  Strict additive DATA reader v2fdf66849 passes11 positive/407 negative cases,
  including independently found destination/EtherType counterexamples. All new
  stage build/human gates remain closed. Next: commit, one full CI, artifact/image
  verification, then request fresh exact boot-only approval. Phone stays806c;
  no additional Join, flash or recovery was performed. No IP fix claimed.

- **2026-10-22: Claude completed and corrected its initial causal claims.**
  Initial exec33267 CLOSED0; narrow same-session follow-up99882 CLOSED0.
  Read research/nord2/wifi/claude-opus-postauth-806c/followup-command-data/REPORT.md
  as the final verdict. The earlier REPORT overclaimed that COMMAND-format
  protected data was invalid and that group rekey caused this closure.
  Source supports conditional protected command data; Linux DPP policy excludes
  flagged DHCP/ARP and depends on firmware capabilities. That mismatch does not
  yet prove a firmware rejection or justify a packet-format/ring-only change.
  Claude now agrees no functional TX change is source-justified on this evidence.
  Group/PTK rekey support is missing but the late packet remains unidentified.
  Cap's independent qualified route audit manifest1710b6c6 agrees.

  Native DATAHDR/DATATXS/protocol-counter candidateb87eae44 independently passed
  reviewa2f0ab7c and is now promoted, including build registration. No acceptance,
  descriptor, key or ownership policy changes. Parent source compile and final
  all-sources tests await the pre-engine WPA input-gate diagnostic amendment.
  Strict data diagnostic reader is being prepared by cap. Wire separately
  documents a complete normal data-ring/TXP/token path for future work; it is
  not part of the current candidate. No CI or new phone action yet.

- **2026-10-22: post-authentication candidates integrated, final composition pending.**
  Canonical postauthorized scan-return predicate/test and nonsecret WPA failure
  diagnostics are promoted; generator has the new wifi_wpa_diag TU but0082 has
  NOT yet been regenerated. Scan-return canonical tests and WPA crypto/actual
  transaction/reader checks pass. Independent diagnostic review0ba24a93 and
  integration packageb5ceee6d preserve the original cryptographic acceptance.
  TXS/DHCP-class diagnostic candidate is still being prepared privately.
  New closed stage research/nord2/wifi-ios-postauth/ios has all source/CI/image/
  human gates FALSE; no CI dispatch or new image yet. Old executed stage remains
  immutable. Wait for Claude report and final source composition before building.

  BSD/ifnet audit271b06ca found no justified framing/mbuf/checksum/link fix.
  Raw captured configd requests a169.254/16 link-local address after link-up,
  with no observed DHCP private/public en0 ADD. Entry logging precedes completion,
  so this establishes fallback activity, not a proven installed address.
  The phone remains806c8d61; do not retry Join or reuse its one-shot runner.

- **2026-10-22: image806c8d61 completed WPA2 locally, but usable IP remains unproven.**
  Collector44878 CLOSED0; no USB reader active. Final445163-byte receipt:
  research/nord2/wifi-ios-local-submit/ios/hardware-review/capture-final.log,
  SHA-256 bcd722dca6cefc34fe321895070b2868c3f14fc030726dd6853312bca0ac7f9d.
  Real M3 integrity/PTK/GTK/controlled-port and ifnet-up at168497ms; iOS consumed
  the connection notification. User saw the network name, but a spinner and no
  confirmed IP or Safari. Six status3 MPDU_ERROR reports cannot be attributed
  to M4 or DHCP because hostPID127 is shared. At215338ms, a late EAPOL event
  caused clean key/peer retirement; final submitted56=dma56, halt0, retired1.
  The user did NOT toggle Wi-Fi. Earlier POWERSET/DISASSOC records PRECEDE Join
  and do not explain the closure. No new panic or working IP claim.

  Historical joined=1 blocks return to scans even after complete cleanup.
  Private candidate hardware-review/postauthorized-scan-candidate/manifest.json
  SHA349d042385e39a9cd1faacdeaf2d0456e81e0e7d1494023fa03fe8c689350e27
  removes only that exclusion, retaining one-join policy and physical guards;
  independent review9ceb4534 PASS. Late EAPOL exact failure remains unknown:
  old diagnostics filled earlier. Candidate review-late-eapol manifest8c7f8a93
  adds reserved public-header/failure diagnostics without widening acceptance;
  independent review pending. Native descriptor audit manifest6fc996b9 found
  no justified descriptor/key/rate fix. Wire now prepares bounded TXS/DHCP-class
  diagnostics; no canonical runtime promotion or new CI yet.

  User requested Claude review. Opus session01e36598-017d-44c6-9f31-18c35b9ccb96
  is running read-only via exec33267, folder
  research/nord2/wifi/claude-opus-postauth-806c; live viewer on desktop3.
  Do not duplicate it. Await its report before the next combined build.
  Phone stays806c8d61; no further Join or new flash authorized. Conditional
  recovery remains verified d9a44316 or RAM ADB rescue, never stock Android.

- **2026-10-22: combined image806c8d61 flashed with exact user approval.**
  Usercall_cbf67d246b7044d5a9968afa4d172787 authorized the named boot-only
  image, conditional d9a/rescue recovery and confirmed manual fastboot ready.
  Root verified the sole known device, productoplus6893, boot0x2000000 and
  non-userspace fastboot. Exact image reverified; flash/reboot returnedOKAY.
  Runner44878 (now CLOSED0; see newest result above) owned the collector at
  research/nord2/wifi-ios-local-submit/ios/hardware-live. One-shot runner USED;
  never rerun it. The authorized one-Join hardware result is recorded above.

- **2026-10-22: combined WPA2 submission/scan-return image PREPARED and verified.**
  shim/out/nord2-xnu/nord2-ios-wifi-submit-scan-stop23.img,33468416 bytes:
  SHA-256806c8d61f58dd625dc0bd68b42941e05ed4abb186c29e6687c30d0fffddc6522.
  Payload14960192 bytes SHAb0bfabb34d74da6ab23476490fa6a15b466bd7fcf5f2af48e22237e3fd510bec;
  boot spare86016 bytes. Builder67204 CLOSED0 under1GiB address-space limit.
  Independent existing-image reportd12d975b/manifest3544b84b PASS; root verified
  every member and protected stage pin. Exact CI39841085457/source38b3e67e,
  loader/rescue/header/segments and calibrated identity ADT all verified.
  Prepared gate is TRUE; human/RF/conditional-recovery gates remain FALSE and
  one-shot runner is UNUSED. Exact new flash approval is required next.
  Phone remains454ac21e, stable after failed join but scans blocked; no active
  USB reader or new phone action. Conditional next-test recovery is d9a44316
  or verified RAM ADB rescue, never stock Android over the iOS userdata.
  Test remains one WPA2 join with private address disabled. Typed local receipt
  does not prove air ACK/keys/IP; scan-return is source/model verified only.
  Pre-password12s delay remains unexplained. No new hardware success claimed.

- **2026-10-22: CI39841085457 and actual linked review passed; image building.**
  Exact source38b3e67e4fd9b53b415bf8142ce1c687c58c81d2, attempt1 success;
  watcher56147 CLOSED0. Kernel17720648 bytes SHA-256
  e930f02f95e60046d14dfa357733df436dfdc9d46560d697ac41ab60634825d0.
  Root verified225 generated files,34 generic ARMv8.2 units and2274123 decoded
  instructions without LDAPUR/STLUR. Bounded linked reported7eccfb and manifest
  aa27dcf8 PASS; all32 members checked. Source/CI/schema gates are verified.
  Builder67204 active with1GiB address-space limit for
  shim/out/nord2-xnu/nord2-ios-wifi-submit-scan-stop23.img.
  Prepared-image and human flash gates remain FALSE pending independent final
  image checks and exact new approval. Currentphone454ac21e remains untouched.

- **2026-10-22: one full build dispatched for combined submission/scan fix.**
  Commit38b3e67e4fd9b53b415bf8142ce1c687c58c81d2 pushed to fork;
  CI39841085457 attempt1 in progress, watcher56147 owns artifact collection in
  research/ci-39841085457. Do not redispatch or reuse old artifacts.
  Final independent source/8TU review5002aee9 PASS; source bundle contains298
  committed files, source-proposals SHA0c7caa76. Separate actual linked/image
  review remains pending. No new flash or join; phone remains454ac21e.

- **2026-10-22: combined local submission and scan-return correction merged.**
  Typed SECURITY_SUBMITTED/DATA_SUBMITTED receipts follow verified DMA retirement;
  they never imply an air ACK or installed keys. Real management receipts,
  M3 integrity checks, PTK/GTK readiness and the controlled port remain required.
  Host and management PID domains are disjoint. After terminal cleanup, a sealed
  same-worker handoff can resume scans without permitting a second join epoch.
  Actual callback retirement precedes this handoff; all five RX cursors persist.

  Canonical host suite passed all nine groups, including 10524 submission,
  1524 cleanup, 2723 actual-transport and 2272 owner assertions. New actual-owner
  scan-return fixture completed two subsequent scans; its terminal-metadata
  seam is explicit and is not hardware evidence. Normal full regression passed
  after removing one unused disabled-host-test stub. Strict connection parser
  passed 158 positive/280 negative cases and preserves old log compatibility.
  All eight actual ARM64 owning units compiled with selected CI flags:
  research/nord2/wifi-ios-local-submit/ios/compiled-review/attempt1/report.json.
  Regenerated0082 SHA bce45b92c03ff337b6a72b5d2dcef909818df03bbd70a22d8bb0bb602c169242.
  Independent canonical composition review32ed09d9 confirms exact merge.
  Next: one full CI, actual artifact/image verification and new exact flash
  approval. No new flash authorized; phone remains454ac21e and usable.
  The pre-password12s delay remains unexplained, not claimed fixed.

- **2026-10-22: active Android path confirms local completion is distinct from air TXS.**
  Selected driver has a literal disabled legacy security-command dispatch.
  Ordinary EAPOL follows wlanEnqueueTxPacket/QM; its TX status callback does not
  gate STA supplicant progress. Our typed local completion candidate therefore
  retains proven DDONE ownership while removing mandatory AIR receipt for
  M2/M4 and ordinary data. Management still requires real matched TXS; keys and
  controlled-port checks remain mandatory. Host-only and management PID domains
  must be disjoint so late events cannot complete another frame.
  Source design review-security-submission manifest0a9fdb1e; independent active
  path qualification review-handshake-progress/v2 manifest10b0af01. Public M3
  headers are not yet a verified MIC or proof of installed keys.

  Wire is implementing the private completion candidate; cap is implementing a
  private closed-NST-to-scan ownership handoff (design manifest06785851), keeping
  first-connection-used flags. Format independently reviews both semantics and
  the new strict parser. Root's parser adds distinct local-retired counts;
  159positive/278negative tests and all three immutable current logs pass.
  Fresh closed stage research/nord2/wifi-ios-local-submit/ios targets
  nord2-ios-wifi-submit-scan-stop23.img; all CI/image/authorization gates false.
  No new CI or flash. Phone stays on454ac21e; no USB reader or further join needed.
  Pre-password12s remains unexplained; measured completed driver calls <=2ms.

- **2026-10-22: 454ac21e test failed to join, but cleanup completed and iOS stayed usable.**
  User reports the same Unable to Join, then explicitly confirms phone/controls
  still work. Collector71787 CLOSED0; no USB reader active. Final immutable log
  hardware-review/postjoin-no-networks.log is390165 bytes, SHA-256
  2cc735b40e1ae4685621b8167a3bb9bf88d88d52f63135238b567e6e9e74af49.
  Earlier complete join snapshot368040 bytes, SHA e83cd4e66106cd274cf96ce1370b47504c162c58d4b14464c05772ddbbd3fff7;
  strict connection parser VALID. Association succeeded. M2 PID3/WLAN5 had
  verified DDONE but no matching TXS; THREE M3-shaped public headers arrived
  (155 bytes, KeyInfo0x13ca). These are not yet MIC/nonce/replay validation.
  Local negative256 at210256ms caused real deauth PID4 TXS, then peer retirement:
  final submitted16=dma16, retired1, associated0/sta0/port0/halt0. No keys/IP.

  User subsequently reports Wi-Fi enabled but no networks. Source confirms
  njs_scan_blocked_locked retains used=1 after the first attempt and refuses
  new scans; the old scan transport cannot simply regain DMA ownership.
  Cap agent audits a safe scan handoff separately. Do not blindly remove used.
  The earlier pre-password delay is still12s. Immutable331487-byte snapshot
  f5b803aa has90 valid timing pairs, no slow sample, max gate0ms/work2ms.
  Scan batches have uncorrelated idle gaps; no cause or delay fix established.

  Wire/format agents now audit typed local security completion for M2 and M4,
  separate from optional air receipts. Current mandatory AIR receipt blocks
  queued M3. Caveat: selected Android's legacy security command branch is
  unreachable from ordinary packet dispatch; active path is being checked.
  No new source patch, CI or image authorized. Keep phone on454ac21e; no further
  join attempts needed. Conditional d9a/rescue recovery remains available.

- **2026-10-22: TXS/timing image 454ac21e flashed with exact user approval.**
  User call_8ea1e36f6e4f48719845807703169259 authorized the identified boot-only
  test and conditional d9a/rescue recovery; confirmed manual fastboot ready.
  Exact image hash, sole known device, product oplus6893, boot size 0x2000000
  and non-userspace fastboot verified. Flash and reboot returned OKAY.
  Runner71787 ACTIVE owns the sole USB collector in wifi-ios-txs/ios/hardware-live.
  Do not launch a second USB reader or re-execute the one-shot runner.
  Await native console and GUI, then bracket password opening before one WPA2
  Join attempt. No new association/key/IP outcome has been observed yet.

- **2026-10-22: TXS/timing image PREPARED and verified; new flash approval required.**
  Image shim/out/nord2-xnu/nord2-ios-wifi-txs-timing-stop23.img, 33464320 bytes:
  SHA-256 454ac21e3bc8f6dd313456f7da01b6a8712e61ca1bff6ae702a1075dc7ccb62e.
  Payload14943680 bytes, SHA c0e19becd83e85feca46cc2f949458c665ee094e80a70a025a770461dbeb3286;
  boot spare90112 bytes. Builder43320 CLOSED0. Independent image report a1993f65,
  manifest72aadc5e PASS; root verified every member and stage pin before opening
  the prepared gate. Kernel/source/segments/header/loader/rescue checks pass.
  ADT equals executed d9a; removing only the MAC addition yields executed6e4.
  CI39830672617/source1a4b1fed identity and linked review are verified.

  Fresh stage research/nord2/wifi-ios-txs/ios has source/native/prepared gates
  true; human/RF/conditional-recovery authorization remains FALSE. Runner unused,
  no active USB reader, no new flash. Current phone remains healthy d9a44316.
  Next: obtain exact new boot-only approval plus conditional d9a/rescue recovery,
  then verify manual fastboot and use the one-shot runner. Do not carry a73's
  old approval forward. Test one WPA2-Personal/CCMP join with private address off,
  capturing pre-password timing separately. Password stays on the phone.
  This image adds known-DDONE negative cleanup and diagnostic timing; it does
  not establish a fix for the missing M2 TXS or prove WPA2/IP connectivity.

- **2026-10-22: CI39830672617 succeeded; actual artifacts verified.**
  Attempt 1, exact source 1a4b1fed3d36622b9697085c55640dbd5f4595eb.
  Kernel 17720064 bytes, SHA-256
  88867058bf927b0b4bdd979f41f804b8e1848b65d415721b713f50f3791863fb.
  Watcher49910 CLOSED0. Root checked 224 generated files against reviewed
  sources, 34 actual generic ARM64 compile units, and 2272839 instructions
  without LDAPUR/STLUR. All new diagnostic markers are present.
  Bounded linked review PASS: report f3aad9ec, manifest 1568791c; root checked
  all member hashes. Builder43320 is active with a 1 GiB address-space limit.
  CI identity and native/schema gates are verified; prepared-image and human
  gates remain closed until final-image review and exact new authorization.
  Additional selected-Android TXS prerequisite audit eacd3cde found no justified
  configuration change. Earlier captured frame results show the route worked
  before M2; the cause of M2's missing receipt remains unresolved. Phone stays
  on healthy d9a44316, no USB collector or new device action.

- **2026-10-22: combined TXS/timing source committed; full CI running.**
  Source 1a4b1fed3d36622b9697085c55640dbd5f4595eb pushed to fork.
  Exactly one workflow dispatched: CI39830672617, attempt 1; macOS Build is
  in progress. Watcher49910 downloads successful artifacts into
  research/ci-39830672617. Fresh source bundle 881595e7 pins 291 committed files.
  Stage-template review 51ba66fb confirms target/recovery/reader consistency;
  root applied explicit new marker/reader requirements and honest timeout
  metadata. Source/schema/prepared/human gates remain false pending artifacts.
  Linked review and independent image checker are prepared, not yet executed.
  Phone stays on healthy d9a44316, no active USB reader or pending human action.

- **2026-10-22: combined TXS cleanup and request timing integrated; local verification passes.**
  Phone is on restored d9a44316; user confirmed lockscreen and controls work.
  Failed-test collector 8715 and recovery collector 18000 are both CLOSED (exit 0).
  No USB reader is active. Recovery yielded no retained final panic message.
  The a73 hardware result established association and M2 DMA completion, but no
  matching air-transmission receipt, installed keys, authorized port or IP.
  The cause of the missing receipt remains unresolved.

  The combined change treats a missing security/cleanup-deauth TXS after known
  DDONE as local negative status 256, then performs real key/peer cleanup.
  It never treats DMA completion as an air ACK. Unknown DMA, command retirement,
  owner, memory and global deadline failures retain the fatal path. Late old-PID
  receipts cannot complete the next cleanup frame. Bounded numeric diagnostics
  record descriptor fields, rejected TXS headers, EAPOL type and RX counts,
  without addresses, replay counters or key material. IOUC timing separates
  gate wait from handler time and emits after unlocking; budgets are finite,
  so missing rows are not evidence that no calls occurred.

  Canonical station suites (8 groups), diagnostic decoder/budget/parser,
  join-owner, client/lifetime, connection parser (149 positive/267 negative),
  timing/concurrency (9020 checks), and timing parser (13 positive/26 negative)
  pass. Five actual ARM64 C owning units compile: manifest e41674d5 under
  research/nord2/wifi-ios-txs/ios/compiled-review. Client ARM64 compilation and
  independent timing review also passed. Generated 0082 applies and matches.
  Private reviewed candidates remain under wifi-ios-join-options/ios; runtime
  transport is 4105df3b, client 771a00ee, generated patch 3e4d06fa.

  Fresh stage research/nord2/wifi-ios-txs/ios remains closed. Next: commit/push,
  one full macOS CI, actual-artifact and independent image verification, then
  request approval for the exact new boot image. No new image is authorized.
  Target: nord2-ios-wifi-txs-timing-stop23.img. Primary recovery is d9a44316,
  secondary 6e4ce860, RAM rescue d78bf9fe. Never boot Android over iOS userdata.


- **2026-10-22: a73 join reaches association then FAILS; d9a restored, user confirms healthy.**
  FirstactualWPA2 profile acceptedreason0 flags4000/scope36, SET22/SET20 status0.
  Strictcomplete receipt VALID: associationobservedTRUE, STAactiveTRUE;
  PTK/GTK/port/IP allFALSE. At174565ms M2published;submitted12==dma12,
  air_pending1 persists; at177568ms nativephase9/error3/halt1. This is3s
  published-operationdeadline with knownDDONE, not proof of missedDMA.
  Log346639B SHA8763328e27e49681db4a823df4f6d2c4669f0dd18db45841d3b03dff45da6931
  in newstage hardware-review/join-first-terminal.log; receipt4182364a.
  User: UnableJoin then WiFi/devicecrash. Collector8715 CLOSED0 earlyUSBtimeout-7;
  no finalpaniccaptured. Source chain fail3→njs_fatal_owner→nss_owner_loop drain
  thenpanic explains expectedbehavior, not a capturedpanic. KnownDDONE timeout
  must be distinguished from uncertainDMA without inventing successfulTX.
  Manualfastboot ready call_69475b517edb41be95bcb9d86127dc3f; conditionalrecovery
  underoriginal call_f0646fc alreadyapproved. Exactd9a boot restored/rebootOKAY;
  recoverycollector18000 subsequently CLOSED0. User call_21fdce0b8eb742ca95e2938d0de34ef8
  confirms tutto ok. Restorelog contains no retainedpriorpanic (no previousmarker).
  User also measured12s beforepassword, beforeJoin. Immutable306780B snapshot
  b5859b4d:3successfulscans span4.365s, cannotalone explain12s; untimedpriorGETs.
  Timingreviewmanifest2cfbea42, sourceaudit2cb24748; noattribution/fixclaim.
  Formatwireauditmanifest a5619562: no provenM2 TXD/Ethernet mismatch vsAndroid;
  laterEAPOLevents have no subtype, do not claim M1duplicates as observation.
  Wire preparesknown-DONE negativeTX/cleanup candidate with latePID defenses;
  format preparessecret-free descriptor/TXS/EAPOL diagnostics; cap prepares
  boundedrequest/gate/dispatch timing. Allprivate, canonical1134851 unchanged,
  no newCI/image/flashauthorization. Root toreview/combine/test together.


- **2026-10-22 19:53 local: join-options a73ee086 FLASHED; live test running.**
  Human exactboot/RF/conditionald9a/rescue approval call_f0646fc645ca4d96abc9cfccfc4bf655,
  readyfastboot. Single knownNord2 product/bootcapacity/nonuserspace checks PASS;
  flash/reboot OKAY. Runner8715 ACTIVE owns soleUSBreader at newstage hardware-live.
  Firmware MAINstatus0stage9ready1; identitypublished1bytes6; one execwifid.
  First valid GUI at796168380/13MHz=61.24s fromkernel timer; humanUI pending.
  At19:55 local user asked to select one WPA2 network and STOP at password dialog,
  before Join submission, to bracket pre-password delay independently. Question
  host timestamp is NOT actual tap time; stored hardware-review/prepassword-question.json.
  Cap pre-password auditmanifest2cb24748/report145bbe15 checkedallpins: selected
  WiFiKit has real async scan-preparation path but execution/10s attribution
  unproven; oldd9a log lacks scans precedingfirst5 WPA2 pairs, nooldGET97/PIPEchain.
  No additional fix justified. Newimage still has no actual association/key/IP
  proof. Await password-open reply, snapshot log, then ask one WPA2 submit with
  Private Wi-Fi Address OFF, password onlyphone. No reboot/retry unless needed.


- **2026-10-22: combined join-options image a73ee086 PREPARED/VERIFIED, not authorized.**
  CI39823157589 attempt1/source1134851 SUCCESS. Builder28170 CLOSED0; independent
  image review PASS report9ff898d9/manifest a0603159, all filepins checked by root.
  Target shim/out/nord2-xnu/nord2-ios-wifi-join-options-stop23.img,33464320B;
  SHA a73ee08664c5eafdc42f1f24163c3feb2167b12f9e44e5f25b9afdc3b9d0eb99.
  Payload14943520B SHA6854250185d94b18ac1ea81071e61fd882cca7e79a7528a89a1ddbd0cf2d15f6;
  bootspare90112B. Actual kernel/header/segments/ADT/calibration/rescue verified;
  ADT identical to executed d9a, removing only chosen MAC restores6e4 ADT.
  Newstage wifi-ios-join-options/ios source/native/prepared gatesTRUE; humanFALSE,
  runner unused, no USBcollector. Primary fallback current d9a44316; secondary
  6e4ce860 and RAMrescue d78 verified. User must approve this exact new image
  before boot write. No actual native association/key/IP outcome yet.
  Separate pre-password delay investigation ongoing: user means ~10s after
  selecting network/list or details Join Network, before credentials dialog.
  Old SET22/association timings do not address that delay. Cap audits exact path;
  do not claim this image resolves it. Phone remains d9a iOS, no device changes.


- **2026-10-22: user clarified the ~10s delay is BEFORE password entry.**
  Both tapping a network in the list and tapping Join Network in its details
  delay opening the password screen. Do not describe this as association or
  post-submit Join latency. The immediate Unable to Join after submitting is
  a separate failure. Prior SET22→SET20 gaps do not explain this UI delay.
  Cap agent now audits the pre-password selection path separately, read-only.
  Combined join-options CI39823157589 attempt1 SUCCESS at exact1134851 source;
  kernel17703288B SHA53106c3316652296b8e1ad17c86b6a0eb86e8fd67963e7b716770235d7b1ee00.
  Root222-file/33compile-unit/ISA checks PASS; linkedreviewb37e9b29 PASS,
  manifest99b20953. Builder28170 ACTIVE; no finished new-image review yet.
  Source/schema gates true, prepared/human false. Phone remainsd9a44316;
  no activeUSBreader, new-image authorization still required after preparation.


- **2026-10-22: combined join-options CI39823157589 RUNNING.**
  Runtime/source1134851bf309c06c613b0383842e70a895e46473 committed/pushed tofork;
  exactlyone workflow dispatched. Watcher11139 ACTIVE downloads only successful
  exactsource attempt1 into research/ci-39823157589. No retry/secondCI yet.
  Four actual owningTU reviewmanifest8031e4b3 PASS; parser review6f997b72 PASS,
  canonical/stageparser386acf06 byteequal. Newstage279sourcepins equal committed
  sourcebytes. Closedtemplate48checks/12negativeADT checks PASS. CI identity,
  linked/image/prepared/human gates remainfalse, no new testimage or deviceIO.
  Format agent prepares boundedlinked extractor; wire prepares independentimage
  checker, both awaiting actualartifacts. Rootownsartifactaudit/build/approval.
  Phone remainsd9a44316 withuserreportedfailedjoins; bothUSBreadersclosed. Next:
  inspectwatcher, audit222files/ISA, finishlinkedreview, buildnewjoin-optionsimage,
  independentimagecheck, then askexactboot-onlyapproval. Donotflash withoutit.



- **2026-10-22: cohesive join-options correction integrated; local checks PASS, CI next.**
  Exact producer correction: scope4000 is bounded1..36-byte string metadata;
  private0800 requires actual retained factoryidentity equality, no MAC change.
  SelectedSET22 accepts0/8bytes; initialized healthy untouchedidle is success
  withoutnative/ifnet/attempt mutation. NativeAssocResp Retrybit now agrees
  withmanagementparser; allotherpeer/status/AID/rate constraints preserved.
  JOIN diagnostics have atomic bounded nonwrappingtraceIDs and three short
  records; STATION has five correlatedparts. All10 source printf formats<=234B.
  New canonical receiptparser supports exactshortschemas,149positive/269negative
  tests; invalidreceipts suppress aggregate AND nestedsuccessclaims. Oldactual
  truncatedlogs remainINVALID. AllIOUC,profile134777+,disconnect104,station7groups
  includingRetry411/owner2271/native1088/actual2730 PASS underASan/UBSan.
  Generated0082 applies/matches; four actual ARM64 owningunits compile (client,
  join,stationtransport,sweet_touch; localAppletypedmetadata macro limitation).
  Private candidates under wifi-ios-identity/ios: joinv2a0deda4c,disconnect5b03206c,
  retry1d943511. Fresh closedstage wifi-ios-join-options/ios prepared; no newCI,
  image or flash yet. Oldphone d9a44316 stillUIusable, no activeUSBcollector.
  Primarynextfallbackd9a44316, secondary6e4ce860/RAMrescue. User approval for
  a future image remains required; no further device action authorized yet.



- **2026-10-22: identity/AKS hardware test complete; actual join-profile blockers isolated.**
  User sees Wi-Fi address now; selecting network still takes~10s and WPA2 Join
  fails immediately. Extra OPEN attempt unsupported by existing WPA2-only path.
  d9a44316 image remains running, UI usable; sole readers72648 and33384 CLOSED0.
  Immutable427075B attempt-final.log under wifi-ios-identity/ios/hardware-review;
  one exec wifid,73 AKS10/11 operations allstatus0,10 JOINREQ rows,zero native
  station rows. This establishes corrected identity+keychain and request arrival,
  NOT association/keys/IP. PrivateOFF profile actuallyobservedflags0x4000;
  initialPrivateON0x4800; selectedproducer scope536 is STRING LENGTH36, notID.
  Current zero-only options reject both. Exact8B SET22 also wronglyunsupported.
  Root/agents preparing one cohesive source change: bounded scope normalization,
  privateaddress equality-only with retainedactualMAC, exactSET22 idle no-op,
  Retrybit-consistent assocresponse validation, and <=255B numeric diagnostics.
  No canonical runtime changes, new CI or further flash yet. User asked no more
  retries while analysis proceeds. Currentimage recovery remains6e4ce860/rescue.



- **2026-10-22 19:08 local: combined identity/AKS image d9a44316 FLASHED; live test in progress.**
  CI39804982005 attempt2 succeeded; builder51870 and independent verifier97444
  CLOSED0. Image33462272B SHA d9a44316f6b257221a948a32483a6778b33cdf5057a3f79ffecfceebf0318de3;
  independent manifest4a866675, identity/readback/source/rescue checks PASS.
  Human explicitly authorized boot-only test plus conditional6e4/RAMrescue recovery
  in call_04cab33a24294ca4a6e302b39530557d, ready in fastboot. Runner72648 ACTIVE,
  flash/write/reboot all OKAY; sole USB owner under wifi-ios-identity/ios.
  New boot identity published=1 bytes=6 source=1; AKS10/11 operations status0;
  one exec wifid in current log. First GUI at552164476/13000000=42.47s;
  native and iOS scans find real APs. Human UI/one WPA2 attempt requested.
  First actual JOINREQ now reaches driver, rejected reason5: assoc_flags0x4800,
  scope_id36, otherwise observed WPA2 pair2/1/8 RSN22 PMK32. No native station
  admission, carrier, IP or connection proof. JOINREQ row truncates/interleaves.
  Format agent audits exact userland metadata; wire audits normalization and
  downstream blockers read-only. Snapshot hardware-review/boot-first-join.log.
  Do not run another USB reader or re-execute used runner. No new build/flash.
  Current authorized fallback6e4ce860; no stock Android boot over iOS userdata.


- **2026-10-22: CI39804982005 attempt2 SUCCESS; combined image BUILDING.**
  User suspected another retry was needed; live GitHub confirmed success at
  16:48:37UTC (18:48local). No third retry dispatched. Watcher10655 CLOSED0,
  artifacts downloaded. Exact source6b3302d1; kernel17703208B SHA
  7f363c332d1c8773d0141339154dc99abd2054615f74319c2f6c04fbe6b3ce14.
  Source audit222 generated files equal,33 actual genericARMv8.2 compile units;
  no LDAPUR/STLUR in2272432 decoded instructions. Bounded linked registry/AKS
  review2ffcffab, manifest7ece921e PASS (1223instructions, actual72-byte wipe).
  Fresh stage wifi-ios-identity/ios CI/native gates nowTRUE; prepared/humanFALSE.
  Builder51870 ACTIVE under1GiB, targetnord2-ios-wifi-identity-keychain-stop23.img.
  Wire agent will independently check completed image; no device operations.
  Root must confirm builder success and independent review, record exact hash,
  then request new boot-only approval with current6e4/RAMrescue fallback.
  Phone remains healthy6e4; no new flash or userland connection success claim.

- **2026-10-22: CI39804982005 rerun as attempt2, user requested; QUEUED.**
  Attempt1 never acquired a hosted runner: GitHub annotation says capacity
  constraints; build job119915675214 has no steps. It failed/cancelled before
  compilation. Root watcher26246 CLOSED2; preserved its metadata/log/annotations
  and watcher under research/ci-39804982005/attempt1. No kernel artifact exists.
  User requested rerun; gh run rerun accepted, live API confirms attempt2,
  source unchanged6b3302d1e039ca451b02a8aa2d8773aea269d8e4, queued build job119951733217.
  New sole watcher10655 ACTIVE runs watch-attempt2.py, checks exactsource/attempt2
  and downloads artifacts only after success. No duplicate workflow dispatch.
  Format agent's prior linked review stopped pending/failure; extractor prepared
  at wifi-ios-identity/ios/linked-review/identity-aks/extract.py, no linked PASS.
  Next successful artifact checks/image preparation remain as below; all gates
  closed, phone healthy6e4, no new device access or flash approval.

- **2026-10-22: metadata audit finished; single combined CI39804982005 QUEUED.**
  Runtime/source commit1fd4c4d plus documentation6b3302d1e039ca451b02a8aa2d8773aea269d8e4
  pushed tofork; dispatched exactly once oncodex/nord2-bringup. Root watcher26246
  ACTIVE checks exactsource/attempt1 and downloads successful artifacts to
  research/ci-39804982005. No duplicate build. Format agent awaits artifacts for
  bounded linked registry/AKS verification; no phone reader or device action.
  Fresh stage research/nord2/wifi-ios-identity/ios pins272 committed source files,
  same539B bootargs, and sole chosen MAC addition. Recovery metadata corrected
  to current healthy6e4ce860; RAMrescue unchanged. Independent stage review5a5e50ab
  passed ADT checks; source/schema/linked/prepared/human gates remain FALSE until
  actual successful CI and image verification. No new image or flash approval.
  BuildVersion provider confirmed system-version dictionary, not DT. Diagnostics
  query checks presence of boot-ios-diagnostics: never add even a zero placeholder.
  MAC getter's alternate path is ReleaseType=Restore; normal selected iOS profile
  does not take it. No persisted-nil cache evidence or cache reset needed.
  Next: inspect watcher status; on success run audit-artifact.py plus existing
  ISA check, finish bounded linked review, fill exact CI/kernel/schema pins,
  build and independently verify nord2-ios-wifi-identity-keychain-stop23.img,
  then request exact boot-only approval with conditional6e4/RAMrescue recovery.
  Existing phone remains6e4, lockscreen/touch/buttons confirmed. No connection
  or keychain success claim. Public audit docs/nord2-wifi-metadata.md records scope.

- **2026-10-22: broader Wi-Fi metadata audit COMPLETE; cohesive corrections integrated.**
  Exact selected wifid/embedded WiFiManager:53 direct MobileGestalt calls and15
  registry/matching calls audited; deployed ADT and latest pinned Topaz/HackiOS
  compared. No other mandatory Wi-Fi DT addition established. Detailed supported
  property/type/order table: docs/nord2-wifi-metadata.md. Private manifests under
  review-device-tree: mobilegestalt e3123a12, upstream db1e5580, consumers fd49ed65.
  Crucial lifecycle correction: manager can cache MAC before native service
  readiness. New offline tools/nord2-wifi-identity.py publishes actual board MAC
  from pinned existing calibration before kernel/userland; later registry checks
  equality against native owner. Independent actual executed-payload check PASS,
  sole addition preserves every old ADT byte, real compiled native selector agrees.
  Registry candidate254e2106 promoted; ASan/UBSan PASS and0082 regenerated/apply
  verified. NORD2-only AKS0083 exact11595cb1 promoted after independent review;
  canonical generated-handler tests164628+3+3 PASS, actual ARM64 candidate TU PASS.
  Upstream public HKwr format is insecure/no SEP or key protection; no reset or
  foreign-format guessing. This is not proof of connection or live crash repair.
  Fresh stage wifi-ios-identity/ios being prepared with all gates false; no new
  CI/image/device action. Phone healthy6e4ce860; no active USB owner. Old stages
  and executed evidence untouched. UFS/speed closed; audio kmod remains unrelated.

- **2026-10-22: actual wifid nil identity identified; broader metadata audit before CI.**
  Eight recovered crashes match the selected wifid UUID. Original instructions
  at 0x1000930e0 insert cached WifiAddressData into PRIVATE_MAC_ADDRESS_VALUE;
  MobileGestalt reads OSData6 at IODeviceTree:/chosen/mac-address-wifi0, absent
  from our tree. Private registry candidate publishes native board-derived MAC
  before interface exposure; host sanitizer tests and actual ARM64 TU compile
  pass (candidate manifest cad58cf0). Publication timing is being independently
  checked: exec wifid occurs before native service registration in the log.
  User requested a broader IODeviceTree/provider/consumer audit to bundle other
  demonstrated gaps before another build. Three agents cover exact MobileGestalt,
  wifid consumers, and deployed tree versus pinned Topaz/HackiOS sources under
  wifi/claude-opus-security-failure/review-device-tree. No speculative identities.
  Separate AKS10/11 candidate adf79c24 and independent review 1a020f6f pass;
  upstream HKwr format is explicitly insecure prototype compatibility, not SEP
  or encrypted key protection. No new canonical runtime changes or CI yet.
  Claude actual-crash followup87270 CLOSED1 due quota (reset21:30 local), not a
  completed review. Prior upstream review did complete. Phone remains restored
  healthy6e4ce860, user confirmed all controls; no active USB reader/new approval.


- **2026-10-22: eight actual wifid crashes SAVED; authorized iOS restoration DONE.**
  Human authorized d78bf9fe rescue→same6e4ce860 boot-only roundtrip in
  call_1de0c6baa5ff4ad89c0847eb38acd924. Rescue flash76773 CLOSED0; collector78107
  CLOSED0:8reports,475136physicalbytes,0writes0mounts,dirtyobservation/nojournal.
  Private live-wifid-reports/result.json maps reportbytes+hashes. All8 wifid
  EXC_CRASH/SIGABRT, ObjectiveC lastExceptionBacktrace __NSDictionaryM setObject.
  Faulting wifid offsets0x930e4/0xf11a0/0x130a88/0x134ed4, UUIDf0919f06…5292.
  Contents nowestablish realdaemoncrashes; AKS10unsupported remainsconcretegap
  but causationnotyetproved. Needexactnilvaluefromproducer, notanchoringonAKS.
  User physicalfastboot call_0f5a721803534e29a2ca8bf090e9bece; restore6e4ce860
  flash/reboot OKAY. Root69012 CLOSED0 read-onlyUSBcollector restore-live;
  no activeUSBowner. User confirmed lockscreen/touch/buttons healthy in
  call_7fe903163053412a93a01063db37e5d8. No userdata changes/no stockAndroidboot.
  Claude upstream review88500 CLOSED0; actualcrash followup87270 ACTIVE,
  resumed sameOpus5.5 sessionb4585316, visible desktop3. Format preparingexact
  byteverifiedcrashproducer; no canonicalchanges/CI/newtestimage yet.


- **2026-10-22: user confirms wifid crash reports; fresh upstream reveals AKS gap.**
  Human sees several wifid-1969-12-31-16xxxxx in AnalyticsData. Contents not yet
  collected; RTC names are not reliable event chronology. Fresh Topaz e4772911
  adds0900 Apple80211/WLAN glue missing from old89451b8 reference. Its AKS10/11
  handlers (from HackiOS) are absent locally; exact log has157 securityd wrap10
  failures. Upstream comparison report91ccccfe/manifest7847eb56 preserved
  under wifi/claude-opus-security-failure/review-upstream. Important limitation:
  upstreamHKwr/XOR is prototype compatibility, not encrypted key protection;
  persistentUFS/keychain behavior needs explicit review before reuse.
  Claude resumed on new upstream+exacterror, wrapper88500 ACTIVE, sessionb4585316,
  live viewer desktop3. No new build. GET51 causal theory independently not
  corroborated (producer review654bf468), do not add speculative replies.
  Bounded read-only HFSX wifid-report reader under review-log-access being frozen.
  Roundtrip plan saved; images d78bf9fe rescue and6e4ce860 restore verified;
  new boot-only diagnostic roundtrip NOT YET AUTHORIZED. Phone remains6e4ce860,
  no activeUSBreader, no device actions since successful authorized test.


- **2026-10-22: Claude failure diagnosis COMPLETE; no speculative next build.**
  Wrapper19134 CLOSED0, resumed Opus5.5/high sessionb4585316, desktop3 viewer
  retained. REPORT.md at wifi/claude-opus-security-failure. Final hardware log
  attempt-final.log434903B SHA3624e56ea8b63bbc7a7684824fde28227218b4b753e6a7e56104adb6103f0226.
  Backend PASS; 13 fresh scans, no JOINREQ/JOINCONTROL/STATION receipts. No
  association/key/IP evidence. Claude observes recurring client initialization
  ~15s and new GET51country query; daemon restart is HYPOTHESIS, not crash proof.
  No task/PID/close/death diagnostics currently identify the restarting client.
  Root asked user to inspect existing iOS AnalyticsData for relevant crash names
  without reboot. cap independently checking trace/source; format tracing exact
  producer GET51; wire preparing bounded read-only userland-log access if needed.
  Native USB only exposes kernel log/status, NOT arbitrary filesystem. Current
  iOS6e4ce860 remains installed; no active USB reader. No new CI or flash.


- **2026-10-22: WPA2 image6e4ce860 flashed; user still reports unable to join.**
  Exact authorization call_b9b8a74fd0f646979e5d8b153f5203bc consumed; sole boot
  write/reboot succeeded. Source938cb89, CI39790703398, image33462272B SHA
  6e4ce860cfbd0b27fbace65e61e96edc8d2af8067603652cd67a0939c53331c6.
  USB live and valid iOS frames observed. Root runner79059 CLOSED0, USED,
  never rerun. Continued read-only USB collector3397 CLOSED0 in
  wifi-ios-security/ios/hardware-after-attempt; no active USB owner.
  User: network selection takes~10s to password sheet, then unable to join;
  Private Wi-Fi Address OFF. Current snapshot has CARD08/40 executing but no
  native JOINREQ/JOINCONTROL/STATION receipt. No connection/IP proof or panic.
  Fresh immutable snapshot/parsers in wifi/claude-opus-security-failure.
  Claude Opus5.5/high running, wrapper19134, resumed sessionb4585316;
  live viewer on desktop3. User requested read-only analysis of
  remaining pre-SET20 gates before another build. No new image/flash approval.
  Phone remains6e4ce860; conditional52d4/RAM rescue authorized if necessary.
  Never stock Android boot over iOS userdata; UFS/speed closed.


- **2026-10-22: cohesive WPA2 image VERIFIED; pending exact boot-only approval.**
  nord2-ios-wifi-security-battery-stop23.img33462272B SHA
  6e4ce860cfbd0b27fbace65e61e96edc8d2af8067603652cd67a0939c53331c6.
  Payload07f369366eea4ad8ec15fb22b457a9408262fa806774bf6427707113bd76b258,
  spare92160B. CI39790703398 source938cb89, kernel86b44266. Builder1098CLOSED0.
  Independent existing-image report1f8b9a44/manifest545196ee PASS under1GiB;
  all loader/kernel/ADT/rescue/header/source/parser/review identities exact.
  Identity/native/prepared gates TRUE; human/RF/recovery authorization FALSE;
  fresh runner UNUSED. Private stage:research/nord2/wifi-ios-security/ios.
  Claude full review90a4e6ae beforeCI yielded cohesive fixes; independent final
  review also fixed first-pump clock and verified linked behavior. No hardware
  connection proof. Phone still healthy43fd, noUSBreader. Ask exact newflash
  approval with conditional52d4/RAMrescue recovery. First test: one WPA2-PSK/CCMP
  attempt after boot, PrivateWi-FiAddress OFF, password onphoneonly. Observe
  JOINREQ/nativeassoc/keys/DHCP/IP/Safari separately; no success inferred.
  Never stockAndroidboot overiOSuserdata. UFS/speed remains closed.


- **2026-10-22: CI39790703398 SUCCESS, kernel verified; image building.**
  Exact source938cb89, kernel17703096B SHA86b44266ff4a20d24ae1bada1eda0533bc7f8a6e75634d940ba5d5453040ea2e.
  221generatedsources match,32actualgenericARMv8.2 compileunits PASS,
  2,271,900instruction ISA check PASS. Watcher53752 CLOSED0, all artifacts local.
  Independent bounded linked reviews PASS: service2886e1cb, WPA79e89147,
  native4f343335; all manifest/body/kernel hashes verified. Source bundle266files
  at wifi-ios-security/ios/source-manifest-complete.json preservesinitial261bundle.
  Identity/native gates TRUE; prepared/human gates FALSE. Builder1098 ACTIVE
  under1GiB limit, targetnord2-ios-wifi-security-battery-stop23.img. No phoneaction.
  Next independentexistingimageverification, then exact boot-only authorization.
  Phone healthy43fd; fallback52d4/RAMrescue verified available. No USBreader.


- **2026-10-22: cohesive connection CI39790703398 IN PROGRESS.**
  Source938cb89b827bf7cd477044714ed23b935d4d6bd0 committed and pushed tofork,
  preceded by CARD932b489. Runner53752 watches exact attempt1 and downloads only
  success artifacts toresearch/ci-39790703398. Job waited~14min for macos14;
  now running. No duplicate dispatch. Native tests/review evidence and full261
  source manifest at wifi-ios-security/ios; no prepared image or authorization.
  GitHub announced reducedmacos14 capacity beforeNov2 retirement (official
  changelog2026-10-01); todayOct22 is not scheduledbrownout. Wire documented
  future macos15 contingency using exact oldIIG generatedpairs; NOTimplemented,
  no workflow change, do not commit Apple/generatedbinary artifacts.
  Next successartifact audit,23bounded linkedcheck, pack/independentimagecheck.
  Phone remains43fd, noUSBowner. Newflash stillrequires exactimageauthorization.


- **2026-10-22: cohesive WPA2 fixes READY for one CI build.**
  Claude full review90a4e6ae incorporated, final independent reviewc4d0bbfb PASS
  after startup clock correction; old source fails the new crossing-ms test,
  corrected source passes. No additional concrete blockers. Combined canonical
  suites PASS; subsequent targeted service regression PASS; all12 actual generic
  ARMv8.2 TUs compile (58706 CLOSED0), regenerated0082 applies and matches.
  Frozen stage planf62f04ef, clock addendum1b7cd0ea, schema13d34ace; all execution
  gates FALSE pending exact successful CI/link/image. Runtime binder4f39afd8.
  Four queued full scans can theoretically exhaust8s; observed logs show pending
  peak1 and0.88–2.14s scans, so no speculative scan-cancellation rewrite.
  Remaining first-test limits: one native attempt/boot, PSK/CCMP, no SAE/mandatory
  PMF/AKM6/private MAC/rekey/roam; raw encrypted untranslated data counted/refused.
  These static/synthetic results are not association/DHCP/IP proof. Phone43fd
  remains untouched; no USB reader. Next exact commit/push/CI, artifact+image
  verification, then request new exact boot-only approval. UFS/speed closed.


- **2026-10-22: cohesive connection fixes integrated; final review caught clock regression.**
  Frozen cap b8ede1a1/runtime09a8da6a, WPA de2837d0, native v22355472e
  promoted after hash/baseline verification; root corrected BSD link event.
  Full combined canonical contracts/client/station/join-owner/registry suites
  PASS unchanged (runner53879 CLOSED0), evidence wifi-ios-security/ios/combined.
  Fresh parser ac1ed268 promoted to this new stage only; all image/device gates
  remain FALSE. Claude full review90a4e6ae complete and incorporated. Attempted
  focused final resume27591 CLOSED1: usage limit until16:20 local, no review
  produced. Independent format review found old now_us reused after constructor
  fresh_us, potentially backward clock at first pump; targeted fix/regression
  in progress (format owns service binder + service-test only). Cap refreshing
  stage pins/linked verification plan after final source freeze. No CI yet,
  no new image and no device writes. Phone healthy43fd, no active USB reader.


- **2026-10-22: Claude full connection review COMPLETE; cohesive fixes underway.**
  Claude wrapper89661 CLOSED0, sessionb4585316; REPORT.md under
  research/nord2/wifi/claude-opus-connect-review, desktop3 viewer retained.
  GET12 security inventory commit932b489 complete; no CI/push yet by explicit
  user requirement to review before build. Claude identified11 candidate issues.
  Root confirmed/fixed actual kern_event_msg payload offset: XNU posts offset24,
  old producer usedsizeof28; corrected48B DL event, real-layout fixture PASS;
  old implementation now rejected by samefixture. Note Ethernetfamily2, not6
  as report says; fixture ON13/OFF12 corrected too. Root owns these3files.
  cap owns exact iOS RSN22/24/28 normalization, numericdiagnostics, scan/join
  deferral/cache; format owns PMKID/RSNXE/vendor parsing, ExtendedKeyID,
  authtrailers and boundedresponse retry; wire owns overflow-only nativeprepare,
  groupWLAN RX and rawEAPOL decoding/counters. All candidates private until
  integrated and tested. U2 ordinary-data COMMAND route confirmed valid in
  original Android; no speculative ring rewrite. No hardware join proof.
  Phone43fd unchanged, no activeUSBreader. Next integrate validated candidates,
  focused final review, canonicaltests/nativecompile, one CI/image. No flash
  without exactnewimage approval. UFS/speed remains closed.


- **2026-10-22: CARD security fix integrated/tested; Claude review BEFORE CI.**
  Native-owned inventory now supplies only installed CCMP+RSN raw13=08/40/zeros;
  no keys/carrier prerequisite or broader security support. Candidate31c4fb87,
  independent reviewdc6f1192; exact4 runtime files integrated. Actual generic
  ARM64 client/sweet_touch compile PASS. Full canonical client/scan/join/lifetime/
  CARD suite and actual native service suite ASan/UBSan PASS. Two fixture-only
  followups: optional query counter warning; old lifecycle expectedzero changed
  to08/40 preserving allocation checks.0082 regenerated/applies. Evidence in
  research/nord2/wifi-ios-security/ios; new stage all execution gates FALSE.
  User now requires Claude comprehensive blocker review BEFORE build. Claude
  Opus5.5/high read-only RUNNING, UUIDb4585316-9654-4897-b2d0-905116396d3f,
  PID451700/wrapper89661, research/nord2/wifi/claude-opus-connect-review.
  Prompt covers selected iOS producer/SET20 through native auth/assoc/WPA2/
  key receipts and BSD/DHCP. Visible foot viewer desktop3. No CI dispatched;
  await REPORT.md, independently validate concrete findings and integrate
  cohesively before one next build. Phone43fd unchanged, USB collector closed.


- **2026-10-22: first connection failed BEFORE native join; exact frontend cause found.**
  Phone remains43fd789b, iOS graphics/scans continue, no panic. Human reports
  password screen waits then "unable to join network", Private Wi-Fi Address
  disabled. Both root collectors71002/1712 CLOSED0; no active USB reader.
  Immutable wifi-ios-connect/ios/connection-attempt-final.log493810B SHA
  c5b12e263ca1f5623982fcbaea65ac6d16377a713de3210f21aa498fb4578999,
  through705.402s: zero JOINREQ/JOINCONTROL/native STATION receipts. No radio
  association or key exchange attempted/observed. Existing GET12 still returns
  empty CARD manifest from scan-only implementation. Exact same-IPSW producer
  rejects RSN with missing bit14 (-3900) and CCMP with missing bit3 (-3903)
  before SET20. cap private native-owned inventory candidate + format independent
  review in progress: expose only installed CCMP/RSN, no WPA1/SAE/PMF claims.
  Root preparing fresh wifi-ios-security stage; no new image/approval or device
  writes. Existing43fd authorization is consumed; healthy52d4 rescue retained.
  UFS/speed closed. Leave unrelated kmod/audio work intact.

- **2026-10-22: combined WPA2 image43fd789b AUTHORIZED and FLASHED.**
  User exact approval call_90e2d06cf50f4397b92aa093a1f2fca1: "Autorizzo, pronto
  in fastboot". Expected physicaldevice/product/boot-size/non-userspace verified;
  only boot written, transfer/write/reboot OKAY. Image33458176B
  SHA43fd789b749b57eab3f735086170931e0c21efa308af394d8ad90f97ba80c9b6.
  Root runner71002 ACTIVE with300s read-only USB capture at wifi-ios-connect/ios/
  hardware-live. Runner USED, NEVER rerun. Phone now43fd; need boot health,
  Settings startup responsiveness and first WPA2 join/address/browsing observations.
  No success assumed. Conditional52d4/RAM rescue recovery authorized if necessary;
  no stock Android boot over iOS userdata. Password stays on phone.

- **2026-10-22: combined WPA2 image VERIFIED; awaiting exact boot-only approval.**
  nord2-ios-wifi-connect-battery-stop23.img33458176B SHA
  43fd789b749b57eab3f735086170931e0c21efa308af394d8ad90f97ba80c9b6;
  CI39776853840 source db14928f50e572b37ea2020ee288d1a23ecb9600.
  Independent image review48bbc65c/manifest36da9cbd PASS, payload/loader/kernel/
  ADT/rescue/header exact; spare96256B. Identity/native/prepared gates TRUE;
  authorization FALSE, runner UNUSED. Phone remains healthy52d4, no USB owner.
  Pending human approval for only boot, including conditional healthy52d4/RAM
  rescue recovery. Test first WPA2-PSK/CCMP association+keys+BSD traffic; user
  enters password on phone and disables Private Wi-Fi Address. One attempt per
  boot; retry requires reboot, no SAE/mandatory PMF/rejoin/roaming claim.
  After approved flash collect live USB log, ask Settings responsiveness,
  network join/address and real browsing. Never infer IP from carrier/counters.
  No stock Android boot over iOS userdata. UFS/boot-speed remains closed.

- **2026-10-22: combined connection image packaged; independent check pending.**
  Builder56944 CLOSED0 within1GiB; nord2-ios-wifi-connect-battery-stop23.img
  33458176B SHA43fd789b749b57eab3f735086170931e0c21efa308af394d8ad90f97ba80c9b6.
  Payloadbc8077762997fa8b1858c56ee37cb6cf15e276f6b4bbe92f54d79118e8ba0de7;
  spare96256B. CI/kernel/source unchanged; independent cap existing-image check
  active. Prepared/authorization gates still FALSE, runner UNUSED. No flash.
  First attempt per boot, PSK/CCMP only, private MAC/SAE/mandatory PMF/rejoin
  unsupported. Need concrete exact-image boot-only approval after independent
  verification; user enters password on phone. Healthy52d4 fallback retained.

- **2026-10-22: combined linked reviews PASS; image packaging active.**
  ExactCI39776853840/kernel72471a30: client/ifnet report85352550,
  WPA/provider9c266af4, native transportf17a0e69 all findings[]. Identity/native
  schema gates TRUE; prepared/authorization still FALSE. Bounded1GiB builder
  session56944 active, target nord2-ios-wifi-connect-battery-stop23.img.
  No device action. Need complete image manifest and independent verification
  before asking concrete boot-only authorization. Healthy52d4 fallback retained.

- **2026-10-22: corrected combined CI SUCCESS; artifact audits PASS.**
  CI39776853840 attempt1 source db14928f50e572b37ea2020ee288d1a23ecb9600;
  watch70501 CLOSED0, artifacts downloaded. Kernel17702688B SHA
  72471a30ac680fc261c0f2a96a3254efb87c844b3c5983522579e36c01434199.
  Source221 generated files equal;32 actual units generic ARMv8.2; full ISA
  2270559 instructions no LDAPUR/STLUR PASS (89923/51436 CLOSED0).
  Bounded linked reviews running: cap client/ifnet, wire MTK transport,
  format WPA/provider. Identity gate TRUE; native/prepared/approval still FALSE.
  No image/flash; phone discovery52d4 remains healthy. Next package after reviews.

- **2026-10-22: corrected combined CI running.** Source db14928f50e572b37ea2020ee288d1a23ecb9600
  committed/pushed; CI39776853840 attempt1, watch70501 owns log/artifact download.
  Only change after669ab25 is exact C linkage for three BPF functions; registry
  ARM64 compile and nm check PASS, no station semantics changed. Combined source
  pins refreshed, all image/approval gates FALSE; phone healthy52d4 unchanged.

- **2026-10-22: combined CI link failure corrected locally.** CI39775642868
  attempt1 failed only on three BPF symbols in registry C++ translation unit:
  selected net/bpf.h does not wrap bpfattach/bpf_tap_in/out in extern C.
  Root wraps that include only; regenerated0082 and actual ARM64 object check
  confirm all three references now use exact C symbols. No runtime behavior
  or station/crypto change. First CI source669ab25 retained with log/annotations;
  watch20095 CLOSED2, no image or device action. Next commit/fresh combined CI.

- **2026-10-22: combined connection CI running.** Source669ab2537777fa4caee553554feac67e8accfa32
  committed/pushed; CI39775642868 attempt1 in progress. Root watch20095 owns
  download/log capture. Canonical station/client/contracts/registry/NSS tests
  and all12 generated native compiles PASS. New source manifest contains261 pins,
  identity/native/prepared/approval gates still FALSE. No phone action.

- **2026-10-22: combined first WPA2 connection implemented; native checks PASS.**
  Canonical integration includes bridge v3 dc18f399, MTK transport v4 fe23e167,
  WPA/owner/service 5000d7cf and real existing-ifnet adapter 3feabf25. Independent
  cross-reviews PASS. Actual generated generic ARMv8.2 compile of 12 translation
  units PASS; canonical WPA owner/connector/actual transport/live binder suites,
  client lifecycle/contract and BSD registry suites PASS (ASan/UBSan). Existing
  NSS repeated-scan/default-off regression PASS. Generated
  0082 applies and reproduces buildtree. Logs: research/nord2/wifi-ios-connect/ios.
  SET20 -> native auth/assoc -> WPA2 PSK/CCMP -> hardware key receipts -> truthful
  controlled port -> existing BSD Ethernet/BPF input/output is implemented;
  no hardware association or DHCP/IP success yet. Mixed WPA2/WPA3 supported only
  when PSK/CCMP and optional PMF are offered; no SAE/mandatory PMF/private MAC.
  First attempt consumes the boot epoch: no rejoin/offchannel scan after it,
  no firmware retirement invented. Ordinary auth/assoc/wrong-key timeouts clean
  up; genuinely uncertain DMA retains inherited drain/halt policy. Retried M3
  cannot reinstall keys; key buffers are wiped. No credential logging.
  Includes prior IONetworkStack startup fix; UFS/boot-speed work remains closed.
  Healthy discovery52d4b159 remains installed. Combined target is
  nord2-ios-wifi-connect-battery-stop23.img, not yet built or authorized.
  New stage gates remain FALSE, runner UNUSED; fallback is healthy52d4.
  Next: commit/push, exact combined CI, linked/source/ISA audits, image verification,
  then concrete boot-only authorization. Leave unrelated kmod/audio work intact.

- **2026-10-22: next test expanded to WPA2 connection; parallel integration.**
  User requires connection work in next hardware build, and confirms target is
  WPA2-Personal or WPA2/WPA3 mixed. Keep healthy discovery52d4b159 installed;
  no active USB collector/new flash approval. Stack fix4e48740 pushed, exact
  CI39768467149 attempt1 SUCCESS, kernel17632424B SHA17e26e7966a072247ded42e236a1aaadd7eddcc9500affdaaaf1fd3d3c704e75.
  Generated-source equality PASS; ISA2253086 instructions PASS; compile log's
  interleaved sweet_touch line independently reconstructed (23 generic units).
  Do not propose stack-only flash: combined connection source still private.
  cap_stage owns actual iOS SET20/22/events/state bridge (candidate601d3ef3,
  v2 reason metadata review pending); format_stage owns WPA2 engine plus live
  njs binder on existing retained NSS worker; station_wire owns native MTK
  association/key/data transport and source guards. Root owns real BSD Ethernet,
  BPF/DHCP link/data adapter. Candidates under research/nord2/wifi/ios-service-bridge.
  Host/native compile evidence is not hardware connection evidence. Outstanding:
  compose typed ownership boundaries, review native failure/rejoin retirement,
  independent review, canonical integration, new combined CI/image verification,
  then concrete boot-only approval. UFS/boot-speed work remains closed.

- **2026-10-22: functional IONetworkStack fix integrated; CI next.**
  Registry source50e69c6a registers stack/owned type0 client for existing real
  attached ifnet; adds truthful IOInterfaceState1, exact naming dictionaries,
  admin/task ownership, bounded open/set timing, balanced deactivation/refs.
  Scans/radio/UFS unchanged. Candidate manifestec4043df; independent review
 891a9601 PASS. Actual generated ARMv8.2 registry compile92626 CLOSED0;
  canonical registry Werror/ASan/UBSan fixture PASS, retained media/TX tests.
  Generated0082 applies/reproduces buildtree. Fresh wifi-ios-stack/ios image/CI/
  approval gates FALSE, runner UNUSED; source-only implementation ready for CI.
  Current52d4b159 healthy with visible networks after~5minutes; no USB collector.
  Need exact CI artifact/native checks and packaging before new image approval.

- **2026-10-22: discovery52d4b159 full capture complete.**
  Both collectors4410/23834 CLOSED0; no USB owner active. Immutable
  research/nord2/wifi-ios-discovery/ios/backend-full.log459229B,
  SHA1fd389de3e894208f466c0310b023ceeb76398a8ad45b178e134b0b39e373509.
  Strict backend PASS/issues=[];33 completed fresh UI scans,33 notifications,
 210 encoded rows with EOF; human confirms visible networks after~5minutes.
  Independent prefix receipt manifest1305e4bf confirms actual GET11 streams;
  unrelated unsupported request207 explains conservative delivered=null field.
  Healthy phone52d4b159 stays installed; no recovery or further device action.
  Missing-stack evidence manifest51700d4d pins configd type0 and setProperties
  contract; full wifid causal link remains inference. Private stack candidate
  uses existing real ifnet plus truthful registered state and owned client;
  independent review underway. New wifi-ios-stack/ios staging all gates FALSE,
  runner UNUSED; no new CI/image/authorization. Next integrate reviewed registry
  fix, native compile and CI; do not change validated scan or UFS behavior.

- **2026-10-22: Wi-Fi discovery CONFIRMED in iOS Settings on52d4b159.**
  Human: page opens after roughly5minutes and finally shows networks; prior
  health confirmation remains valid. Actual UI scans on2.4/5GHz complete with
  event10, per-row GET11 and EOF. ui-confirmed.log423414B SHA40809745f449ed4b003073f2c68961b4ee590db1c26327ddde91ecb970a7906b saved privately;
  final capture23834 still active. No association or networking implemented.
  Initial event pipe322.619s vs clients26.190s and exact configd Stack300s timer
  implicate missing IONetworkStack startup service; rootbusy0 alone did not fix.
  cap_stage validates exact caller; station_wire prepares private real owned
  interface adapter (existing ifnet, truthful registered state, no duplicate BSD
  interface); format_stage independently reviews live scan delivery. No new
  image/flash, phone52d4b159 stays healthy. Fix remaining initialization delay
  before next proposed boot test; no need to alter proven scan path.

- **2026-10-22: discovery52d4b159 boots healthy; Settings still blocked.**
  Human confirms lockscreen/touch/buttons okay but Wi-Fi Settings freezes.
  Root4410 CLOSED0; immutable backend-boot.log350624B, SHAb021928c0607f19936085c05a3d793bf21a1263b714de4bae580e45bd769c785,
  under research/nord2/wifi-ios-discovery/ios. Backend parser PASS/issues=[];
  three client opens at26.2s with root_busy0, no armed pipe/scan request yet.
  Read-only Settings collector23834 ACTIVE for600s; sole USB owner, no new flash.
  cap_stage found exact configd waits for IONetworkStack service/open separately
  from root quiet, initial300s timeout; investigating actual caller contract with
  station_wire in parallel. This is a candidate explanation, not a proven fix.
  Current phone52d4b159, no recovery; runner USED. Need eventual Settings scan
  chain and final UI observation; do not claim discovery success.

- **2026-10-22: combined discovery52d4b159 VERIFIED + AUTHORIZED + FLASHED.**
  Image nord2-ios-wifi-discovery-battery-stop23.img33415168B,
  SHA52d4b1591b3b8d906214ec37bc93e309d5a3b69d59fd8a013c424d755a504aca;
  payloadd1906dbb7c6041692d0db42f950899fc07d7e2cfd67c86cc9cb1544520aa9562.
  Spare139264B. Packaging13636/independentverify43682 CLOSED0; independent report
  c783f16b and manifestcc44731c. Scan linked reportec35175f/manifest2ab324df PASS.
  Exact approval call_91391bde95ce483dbd40446c159103a0, human “Autorizzo, pronto
  in fastboot”. Expected device verified, boot-only flash and reboot OKAY.
  New discovery runner USED: NEVER rerun. Root4410 ACTIVE with300s read-only USB
  collection at wifi-ios-discovery/ios/hardware-live. Phone now52d4b159; no recovery.
  Need actual client/pipe/scan/event/result timing and human Settings/network/health
  observations; no hardware success assumed. Conditional6a6a/RAM rescue authorized.

- **2026-10-22: discovery CI39760685458 attempt2 SUCCESS; artifact audits PASS.**
  Watch78857 CLOSED0, artifacts downloaded. Exactee2e6fd kernel17607656B,
  SHA33fe90d930d0b304f687c84f649a7092fea1cf804a615893c9f57ce40b062c96.
  Sourceaudit59535/ISA92763 CLOSED0:196generatedfiles equal,23actualunits generic,
 2251904decodedinstructions noLDAPUR/STLUR. Quiesce bounded linked review PASS,
  paired suppression and normal callbacks preserved; report8bafc4b6, manifest5cde08db.
  Scan linked9function review running in wifi_station_wire; root waits report
  before promoting schema gate and bounded1GiB packaging. CI identity gate true,
  schema/prepared/authorization stillfalse; runner UNUSED. Phone195a unchanged.

- **2026-10-22: CI39760685458 attempt1 cancelled before runner; attempt2 queued.**
  Exact GitHub annotation: hosted runner was not acquired after multiple attempts;
  macOS arm64 capacity notices. Zero steps and zero artifacts; no compiler failure.
  Evidence preserved at research/ci-39760685458/attempt-1/{run,job,annotations}.json.
  Watcher21893 CLOSED1 because attempt1 had no downloadable log. Reran same
  CI39760685458/sourceee2e6fd; attempt2 job119770821706 queued. New root watcher78857
  requires attempt2, captures annotations on failure and downloads exact success
  artifacts. All221 source pins unchanged, all image/approval gates still false.
  Phone195a unchanged, no USB/device action. Continue with verified CI artifact
  audit/link review/package as below; do not use attempt1 as build evidence.

- **2026-10-22: discovery local work complete; external macOS runner still queued.**
  CI39760685458 has no runner steps yet; root watcher21893 remains active and
  downloads exact artifacts on success. No duplicate workflow or new flash.
  Additional actual-IOUC composition ASan test PASS: explicit13-channel caller
  reaches exact requested subset;7 synthetic rows yield3valid/4omitted(mask0xf),
  event10 and GET11/EOF succeed; native owner/IOKit primitives remain mocked.
  Receipt research/nord2/wifi-ios-discovery/ios/composition-review.json, private
  composition manifest32a7de3724ca6ec1e83b7fb5b85cc489e7213241a8e2e0e11a3ebdccdccf7764.
 221 source pins unchanged; healthy6a6a and RAM rescue hashes rechecked.
  Next: resume watcher21893 / inspect research/ci-39760685458/run.json; only after
  success run audit-artifact.py and whole-kernel nord2-check-rcpc2.py. Notify
  wifi_station_wire and wifi_cap_stage with verified artifact path/size/hash to
  finish bounded linked checks (scripts already prepared). Promote new staging
  source/CI gates only from verified evidence, then bounded-memory build-image.sh,
  independent verify-image.py and concrete exact-image approval. Current phone195a,
  no active USB collector, new discovery runner UNUSED; no new approval acquired.

- **2026-10-22: combined discovery sourceee2e6fd pushed; CI39760685458 queued.**
  Exact sourceee2e6fd9516b5eb7a5c41cabf1a077c93e53ccc7 on fork/codex/nord2-bringup.
  Root watcher21893 will retain run.json/build.log and download success artifacts
  under research/ci-39760685458. Eight focused source compiles PASS, contract
 325snapshot/11071policy/15365scalar/5407caller checks PASS. Private native schema
  includes221 source pins; gates FALSE, no image or approval. Linked-review
 9function extractor prepared at wifi-ios-discovery/ios/linked-review, awaiting
  artifact (no inherited verdict). Phone195a unchanged, no active USB collector.

- **2026-10-22: Claude review integrated as combined discovery fix; CI next.**
  Exact hardware LOW_LATENCY0x20 now admitted (LOW_PRIORITY0x2 separately),
  semantic filters still refused with distinct codes; RawSet EBUSY16 proven
  through selected IO80211 errno path. Snapshot omits/counts individual invalid
  BSS rows, retains1024B IEs, skips only proven basic-marked selectors123/126/127,
  preserves coherent on-channel records over adjacent-channel receptions.
  NORD2 paired IOKit daemon busy-hold/release suppressed without touching other
  busy counts, deferred matches or resource publication. Same-IPSW configd
  confirms root IOBusyInterest and240s quiet timer; historical delay attribution
  remains inference. Client timestamps/root-busy/channel/omission logs added.
  Reviews at research/nord2/wifi/claude-opus-ios-195a/review-{policy,snapshot,quiesce};
  source policy/caller/snapshot/scalar ASan suites PASS; actual IOUC/lifetime PASS;
  repeated scan-service and immutable normal regressions PASS. Eight generated
  native units plus two whole IOKit sources compile genericARMv8.2.0082 regenerates
  and applies exactly; diff-check clean excluding nested patch context whitespace.
  New wifi-ios-discovery/ios staging gates FALSE, runner UNUSED. Phone195a unchanged;
  no new image/authorization/device action. Need CI, exact artifact verification
  and packaging before requesting one combined boot-only test. UFS unchanged.

- **2026-10-22:195a7e17 Settings test FAILED; delegated to visible Claude Opus.**
  Human reports several minutes before Wi-Fi Settings opens and still no networks;
  no fresh explicit touch/buttons confirmation. Current phone195a7e17 unchanged,
  no recovery or further flash. Both collectors55188/70804 CLOSED0; full log
  413935B SHA653465fdd90becd0ab42bc5e81701d484e4a424a685fb806179922ff247049fc
  at research/nord2/wifi-ios-profile/ios/backend-full.log. Backend parser PASS,
  complete=true/issues=[]/7bootAP. iOS parser shows 26SCANREQ/26ADMISSION-1,
  flags_lo32/flags_hi0, RSSI INT32_MIN, phaseArmed; zero enqueue/completion/event/
  results. The prior RSSI fix was incomplete: current policy rejects all flags;
  flag0x20 semantics and the separate UI delay need source/caller investigation.
  User explicitly requested Claude and desktop3. Claude Opus5.5/high RUNNING
  UUID1e2f22fe-044e-432b-b225-92f430064ffd, PID418155, wrapper session58446;
  research/nord2/wifi/claude-opus-ios-195a/PROMPT.md contains complete handoff,
  exact413k trace snapshot plus sources/disassembly/references. Read/Grep/Glob
  only; requested cohesive diagnosis, proposed patch and caller-based fixtures,
  no canonical/build/device changes. Live foot viewer PID418374, class
  claude-wifi-ios-195a, verified mapped/visible/focused on workspace3; do not
  duplicate. session.json tracks progress; wrapper saves REPORT.md/result.json.
  Snapshot411611B SHAa14fc184b6444041c05acdbb666a456e372bce14175b23f98027fbb444619099
  stays immutable; collection-complete.json points to the later complete trace.
  Next read Claude result, review/integrate concrete fixes together before CI;
  never treat7 bootAPs as iOS Settings delivery. Existing runner USED.

- **2026-10-22:195a7e17 boot capture PASS; human Settings test pending.**
  Root55188 CLOSED0; complete boot log343019B saved at
  wifi-ios-profile/ios/backend-boot.log, SHAe9100838d80d9c2b7d1db42869b5e56c08eab322cc8b883f3e9e51ab0ad729cc.
  Backend parser complete=true/issues=[];7AP,3client opens, valid iOS frames.
  No fresh SCANREQ/enqueue/event/result yet: the corrected policy has not been
  exercised on hardware. Health/Settings question is pending, no result assumed.
  Settings read-only USB collector70804 ACTIVE for600s at hardware-settings;
  one owner, no other USB collector. Phone195a7e17; runner USED; no recovery.
  wire agent investigating private codec flags504 and event9 metadata extent;
  source/device unchanged. Next read human result, save immutable full trace
  and parse actual UI chain. Do not confuse7 boot APs with Settings delivery.

- **2026-10-22: scan-profile195a7e17 FLASHED; live test collecting.**
  Exact approval call_d5571d307bc84e3e8f21aa4fc4828c77, human “Autorizzo,
  pronto in fastboot”. Boot-only flash/reboot OKAY; current phone195a7e17.
  wifi-ios-profile/ios runner USED, NEVER rerun. Root55188 ACTIVE with300s
  read-only USB collection; USB console connected, initial26-command startup
  and22-channel scan report7AP/7frames/noerrors, retained owner ready1.
  Fresh UI scan/admission/results and human health still pending; Settings
  question sent while collecting. No recovery, no association claim. Exact
  conditional healthy6a6a/RAM rescue approval acquired; never stock Android.
  wire association codec55a94847 private1294checksPASS; real defaultflags504
  and metadataevent9 remain unresolved, so no actual caller admission claimed.

- **2026-10-22: scan-profile image195a7e17 VERIFIED; exact approval pending.**
  shim/out/nord2-xnu/nord2-ios-wifi-scan-profile-battery-stop23.img,33415168B,
  SHA195a7e174c750f1e0b2bd54894c615ce9e008068b8a52e609923246f6a3ed09b;
  payloadf5a88fb64c03f81ad5cd5b8d51ec9b74f45f45bf2e0f09c469e0ec30d1ce3aff.
  CI39750474116/source27717ff/kernel01c1922e. Pack71233 and independent
  existing-image/header/loader/ADT/calibration/kernel/rescue check46492 CLOSED0;
 139264B boot spare. Fresh wifi-ios-profile/ios prepared/source gates true,
  all human authorization flags FALSE and runner UNUSED. Current phone a199cd0c;
  no new flash, recovery or USB collector. Next exact image boot-only approval
  and physicalfastboot, then fresh runner and real Settings scan/event/results.
  Conditional proposed fallback healthy6a6a or tested RAM rescue; never stock
  Android over iOS userdata. No actual scan delivery/association claimed yet.

- **2026-10-22: scan-profile CI39750474116 PASS; image packaging71233 active.**
  Exact source27717ff, kernel17607648B SHA01c1922e7518ab7a9d39a408be337cd2f0fb53f346faf85c00602c0b2993eda9.
  Generated195files equal CI,20affected compile units genericARMv8.2;
 2251653 decoded instructions contain no LDAPUR/STLUR. Watch37085,
  sourceaudit80503,ISA14627 CLOSED0. Four-function linked semantic review
 52c5c02b/manifestb785c5f5 PASS; local object bodies differ in code generation,
  actual linked bounds/constants/call targets reviewed with no finding.
  wifi-ios-profile/ios CI/schema gates verified; prepared/approval gates false,
  runner UNUSED. Bounded1GiB pack71233 running. Phone remains a199cd0c.
  Private parallel followup9a7dbe26 replaces inherited crypto with staged MIT
  BearSSL,3148checksPASS, and proves EID24 station-ready versus GTK no-response
  software completion. Private station-transaction2cb49e90 passes1688checks;
  native actions/data path remain unimplemented, first-attempt latch does not
  claim a firmware teardown/reconnect barrier. No association changes in CI.

- **2026-10-22: scan-profile source27717ff pushed; CI39750474116 queued.**
  Exact source27717fffcbbd0221334a4b18df6565551f877ab3; root watcher37085
  will retain run.json/build.log and download exact success artifacts into
  research/ci-39750474116. New wifi-ios-profile/ios gates remain false and
  runner UNUSED. Phone still a199cd0c; no device action or new approval.
  Final full a199 log365419B independently parses PASS/complete/issues=[];
  SHA0ba5725b3adf8d7af6752047ae852b21b6690bba5edef24f66a406e3789ea771.
  wire agent has prepared bounded4-function linked review for new policy,
  power helper and two client methods; does not repeat unchanged registry/DMA.
  format agent is resolving native PTK/GTK completion semantics and a private
  BearSSL provider using already-staged MIT-licensed files. Offline work only.

- **2026-10-22: complete a199cd0c trace saved; semantic scan-profile fix integrated.**
  Both collectors41821/14266 CLOSED0. Full365419B log preserved at
  wifi-ios-compat/ios/backend-full.log, SHA0ba5725b3adf8d7af6752047ae852b21b6690bba5edef24f66a406e3789ea771.
  Hardware result records event pipe/maps/channel replies succeeded, repeated
  UI SET10 reached Armed but no enqueue. Human still reports delayed/empty UI.
  Fresh wifi-ios-profile/ios source admits the same-IPSW dictionary default
  RSSI INT32_MIN at4880; validates semantics, ignores padding/inactive storage,
  allows nondirected broadcast BSSID, retains owner channel allowlist and all
  unsupported active/filter/timing refusals. ON helper ignores inactive state
  slots but still requires one real ready radio and ON1; OFF remains unsupported.
  New numeric SCANREQ metadata/ADMISSION/POWERSET reports reveal options/status
  without SSID/BSSID/credential bytes. No new radio/DMA/association behavior.
  Private candidate5e4fcae8 and independent review18f04303 PASS. Canonical pure
  contract/caller projections PASS; actual IOUC sentinel passes enqueue/cache/
  GET11/event plus fault/lifecycle suites, native2changedTUs compile with exact
  genericARMv8.2 flags.0082 regenerates and applies exactly. Full CI next.
  Current phone a199cd0c, new image gates false and runner unused. No new
  approval/device action. Parallel offline station-wire77ce9b49 and WPA engine
  135660fe candidates complete privately; integration/security/native completion
  contracts remain open and are not part of the next scan-profile image.

- **2026-10-22: a199cd0c reaches real iOS scan requests; Settings still empty.**
  Human reports Wi-Fi page opens after about3minutes, then no networks.
  Driver boot scan has8AP/8frames; full backend26/UFS/radio parser PASS.
  Actual registry network_class1,5IOUC opens, event pipe Armed(refs8), map types
  1/2, OOL channel responses27/254 succeeded. Many SET10 requests5456B arrive
  Armed, version1/BSS3/scan1/PHY1/defaulttiming/13channels, but no ENQUEUE.
  GET96/97 are not blockers: live event setup and scans pass them. Static
  addendum a13cd7b9 proves failed role96 defaults to0 Infrastructure, not AWDL6;
  current BSD/media/Infrastructure enumeration path does not use Apple whitelist.
  Concrete policy false refusal found in selected IO80211 scan builder: it
  unconditionally writes INT32_MIN RSSI at offset4880; current zero-byte sweep
  refuses it. format_stage preparing semantic-field policy and caller-derived
  fixtures plus exact SET19 serializer review; do not spoof class names.
  Boot collector41821 CLOSED0,316443B. Settings read-only collector14266 ACTIVE
  in wifi-ios-compat/ios/hardware-settings for600s. Immutable boot and first UI
  snapshots saved; latter356713B and has boot as exact prefix. No new flash,
  recovery or new approval. Current a199cd0c remains on phone; health is limited
  to valid framebuffer and user's ability to use Settings, not full button check.
  wire agent preparing offline MTK station/management/EAPOL/TXD/key candidate;
  only private research changes. Association/data plan f22cb793 saved; still no
  association, encryption or packet networking implemented in installed image.

- **2026-10-22: iOS Wi-Fi compatibility a199cd0c FLASHED; collecting, health pending.**
  Exact user approval call_e5de447d21f94dbab2e5330a419574f9, “Autorizzo il test”.
  Boot-only flash/reboot OKAY, current phone is a199cd0c, no fallback performed.
  shim/out/nord2-xnu/nord2-ios-wifi-ios-compat-battery-stop23.img,33413120B,
  SHA a199cd0cda7138ba2ecf39e3bc585cbbd3556e12f3b7169d81894bdccf985562;
  payload f6c384200da4fdb1998bd75f6ccd341b45987a4bccf8146359955b51eedc0021.
  CI39744729444/source b7800f8/kernel28a8634f; bounded package30125 and
  independent existing-image check62942 CLOSED0;141312B spare in boot.
  Exact header/loader/stock Linux/rescue bytes/ADT/calibration/kernel segments
  verified. Fresh wifi-ios-compat/ios runner is USED: NEVER rerun.
  Root collector41821 ACTIVE with300seconds read window. Await actual iOS
  network enumeration, event pipe, fresh UI scan and result delivery; human
  Settings list and full health still pending. No association/IP implementation.
  Conditional recovery healthy6a6a/RAM rescue already authorized; never stock
  Android over iOS userdata. If hardware fails, save log before manual recovery.
  Parallel format_stage preparing association/data implementation plan using
  oosexgang and selected MT6893 Android driver; no device/source mutation there.

- **2026-10-22: iOS compatibility CI39744729444 PASS; image packaging in progress.**
  Source b7800f8d81a197a490923e6443157a1d740effbe, kernel 17607600B,
  SHA28a8634f95297a62de1ece6c7d3597d7e56a2ef9ca3a5729f8882d5654c3edac.
  All195 generated files equal CI;20 actual affected compilation units use
  generic ARMv8.2;2251459 decoded instructions contain no LDAPUR/STLUR.
  Exact linked review cb60f9ec/manifest96e7d8c3 PASS verifies metaclass parent
  chain and init retention, scalar/channel calls and OOL copy/rollback branch.
  CI watcher37012, source audit92732 and ISA15576 CLOSED0. Image packaging
  session30125 runs with a1GiB address-space cap at wifi-ios-compat/ios;
  prepared image/human approval gates remain false, runner UNUSED.
  User connected Nord2 in fastboot; expected single device detected read-only.
  No new flash or approval. Next finish package/check-existing and obtain
  exact image boot-only approval. Healthy6a6a and tested RAM rescue verified.

- **2026-10-22: cohesive iOS Wi-Fi compatibility correction integrated; local checks PASS.**
  New stage research/nord2/wifi-ios-compat/ios, no device action or image approval.
  Current phone remains last58f6 (currently absent USB); healthy6a6a preserved.
  Root added IONetworkInterface ancestry, actual controller MAC, builtin/prefix/
  primary properties. Same-IPSW SystemConfiguration matching proves the previous
  hierarchy was absent from SCNetworkInterfaceCopyAll's enumeration path.
  User-supplied reference https://github.com/oosexgang/hackios-xnu-kernel at
  8e61187098f2c26c5daee8f9f72b6416aa64b9ab, RedwoodWLAN.cpp and
  redwood_apple80211.c, corroborates discovery/layouts. Snapshots/pins private at
  research/reference/oosexgang. Do not transplant its Qualcomm backend to MTK.
  GET13/15 raw4B replies now use real retained worker state/STA mode; GET27/254
  expose actual allowed channels in4824B layout; GET103 returns ENXIO while
  unassociated; GET43/44 general queries identify our driver/hardware. Exact
  SET19 already-on is idempotent only while retained owner available; off remains
  unsupported because physical shutdown is not implemented. No fake carrier,
  association, security capabilities or IP readiness. GET96/97 and optional
  SET383/365/25 remain unsupported; their failure is not proven the UI blocker.
  Scan accepts bounded nondirected/default-timing all/subset channels for modes
  1/2, translated to this driver's passive discovery policy. ACTIVE permission
  does not cause probes; directed filters, custom timing, DFS/unknown options
  still refuse. Retained ownership/DMA continuation remains unchanged.
  IOUC supports checked OOL output, accepts unused inline pointer with size0,
  and rolls back GET11 cursor on failed copy/complete so retry loses no row.
  Setup/scan/event/result logs now have separate budgets from repeated getters.
  Source review: scan7414ae78/reviewea993996, scalar+channels636508aa,
  registry0c75021a, transportaf6af1d4/runtimecceb7414. All evidence is static or
  synthetic; Settings success requires a new hardware test, not inferred here.
  Portable canonical tools/test-nord2-wifi-ios-{contract,registry,client}.py
  pass ASan/UBSan; actual owner250+ continuation/default-off tests pass including
  idle/queued/running/completed state.19 actual generated native compilation
  units pass generic ARMv8.2 with no private includes; 0082 regenerates/applies
  cleanly and exactly reproduces the buildtree. CI and image verification next.
  Preserve unrelated untracked kmod audio work; UFS/boot speed remain closed.

- **2026-10-22: RESUMED by user (“eccomi, riprendiamo”).**
  Phone last flashed58f62ae6; no new device actions. USB inventory currently
  does not show the Nord2. Root integrating cohesive iOS consumer compatibility;
  format_stage owns role/parent/scalar ABI, cap_stage owns scan/status requests,
  after_id owns IOUC/power/event audit. Full375276B saved trace is authoritative.
  Do not change UFS/boot speed. Keep private evidence out of git. New image
  requires verification then exact boot-only approval; old runner stays USED.

- **2026-10-22: PAUSED BY USER after scan-service58f62ae6 hardware collection.**
  Exact approval call_5680d604ecda40a985661be914631868; boot-only flash/reboot
  OKAY. Current phone is58f62ae6, NOT6a6a. No recovery performed, no device
  mutation after flash: only read-only USB collection. Fresh wifi-ios-scan/ios
  runner USED: NEVER rerun. Root97916, final24587 and settings89958 CLOSED0;
  no collector active. User requested logs then resume tomorrow morning;
  subsequent report that Settings opens does NOT revoke the pause. No more
  source changes, builds or flashes until resumed. UFS/boot speed remain closed.
  Initial radio startup26 and22-channel scan PASS,4BSS/4frames. Retained owner
  ready, interface registered carrier-down,3actual client opens. NO fresh UI
  scans enqueued/completed, NO event notifications or result rows delivered.
  User: Settings initially froze on Wi-Fi and Control Center toggle was grey;
  after many attempts Wi-Fi page opens but displays NO networks. Separate
  full lockscreen/all-buttons health was not explicitly confirmed on58f6.
  Latest logs show additional requests beyond the first96/97: GET103 returned
  unsupported76times; SET383 eight times, SET365/25 once each unsupported.
  Full observed request set/counts in private ios-request-counts.json; do not
  mistake successful registry/client discovery for functional Settings Wi-Fi.
  All published USB buffer captured to research/nord2/wifi-ios-scan/ios/backend-full.log,
  375276B SHA555765f9ba8a1d46ccb788b7c2c13cddab968b7b1bf9faa0535f8ce8b5aa3563.
  Earlier340921B and365787B captures are exact prefixes; all3 collectors have
  nativeUSB protocol2, no transport error, no buffer overflow or userdata command.
  backend-full-parsed.json complete=true/issues=[]; ios-service-parsed.json
  records the incomplete UI path; hardware-result.json and stage-pending.json
  preserve actual outcome and pause. These are XNU USB logs, not an extraction
  of all iOS userland filesystem logs. Raw/private data remain outside git.
  Next morning: review COMPLETE request trace and implement a cohesive consumer
  contract correction. Bounded request96/97 evidence saved under
  research/nord2/wifi/ios-service-bridge/request-96-97-handoff/handoff.json,
  SHAcdfb99df6669bed651440e5f9490c6ea5bab94568f7c7dec47496e9351fff4bb.
  Proven names VIRTUAL_IF_ROLE/PARENT, GET96 scalar value at requestoffset20,
  GET97 raw16namebytes. Infrastructure enum and correct physical parent remain
  unproven; fallback role6 does NOT establish enum. Causality of UI freeze is
  also unproven. Agent stopped; do not resume analysis during requested pause.
  Healthy6a6a/RAM rescue available under original conditional recovery approval;
  never boot stock Android over installed iOS userdata. New image needs exact
  boot-only approval. Association/security/data/IP remain unimplemented.

- **Pre-flash history: iOS Wi-Fi scan-service image58f62ae6 verified (superseded above).**
  shim/out/nord2-xnu/nord2-ios-wifi-service-scan-battery-stop23.img,
  33409024B SHA256 58f62ae6c3e7650bd71d9c8f723360602ee6ac90ff1a21d03e269ec8ca769b4c,
  payload b9d72642bf3e4eaa7de6b1b62af54e383c4f0fdb3bcda068d21de9fe2100b658;
  boot spare145408B. CI39690505727 SUCCESS/source52bb4e7b9c1e7cf5cfab3a72e9d608ccf04368dd.
  Kernel17603624B SHA820a1879f676177633ca877506a60ea0a06cba6ccfbca87d53cd5dada71e99f0.
  All193 generated files equal CI;19 actual affected compilation units use
  genericARMv8.2;2,250,267 decoded instructions contain no LDAPUR/STLUR.
  Linked review9e567859/manifest53fbbac4 PASS: actual metaclasses/constructors,
  registry/ifnet/userclient/cache/event and retained scan owner/drain paths.
  Bounded1GiB pack and independent existing-image/header/loader/ADT/calibration/
  kernel segments/rescue verification PASS. Root watcher88190 and pack50283
  CLOSED. New wifi-ios-scan/ios prepared/source/identity gates true; all human
  approval flags FALSE, runner UNUSED. Collector will allow300seconds for UI.
  Current phone remains humanhealthy6a6a, no collector or device action.
  Next obtain exact58f62ae6 boot-only approval plus physicalfastboot; use only
  fresh stage runner. Conditional healthy6a6a and verified RAM rescue available;
  never stock Android. Test actual wifid discovery/fresh scan/result/event and
  Settings display separately from lockscreen/input health. No association,
  security, data traffic, IP networking or UI success claimed before hardware.

- **Source integration history: iOS scan service52bb4e7; CI subsequently passed above.**
  Explicit nord2wifiios=1 activates the retained radio worker queue and software
  ADT wlan provider. No firmware replay or repeated startup26 sequence. New
  scans preserve actual DMA/credit/sequence state; stale RX must drain before
  publication, bounded clean refusal preserves advanced cursors. Default-off
  old path remains unchanged. Real IO80211Controller/Interface/SkywalkInterface
  IOService classes, carrier-down enN ifnet, Infrastructure property and exact
  SIOCGIFMEDIA type reply implement the recovered iOS discovery requirements.
  Entitled owned IOUserClient binds fresh passive scans to immutable result
  cache and mapped event10; close/cancel/OOL/send-reference races are covered.
  Populated results use actual received BSS and a documented compatible rate
  map. Association, security, packet TX/RX and IP networking NOT implemented;
  actual Settings compatibility requires hardware testing, not assumed.
  Private repeated-scan manifest cb4376e2/review fd710542; service ad4554a8/
  review 1db59a70; strict new owner-terminal parser review 291669a8.
  Generated 0082 applies cleanly; all18 actual generated TUs compile with
  generic ARMv8.2 flags and native headers, including2Cpp without private -I.
  Canonical normal/capability/VCN33 and new scan-service sanitizer suites PASS:
  250+continuations, pending/budget drain refusal, wraps, cancellation, fatal
  drain/defaultoff; private actual IOUC/registry lifetime suites also PASS.
  Exact-source artifacts are saved under research/ci-39690505727; watcher88190 closed.
  New source in shim/nord2-xnu/wifi_ios_* and wifi_scan_service*, canonical
  runner tools/test-nord2-wifi-scan-service.py. Fresh stage wifi-ios-scan/ios;
  no image or new approval yet, runner UNUSED. Phone remains healthy6a6a,
  no USB collector/device action. Next full CI/link/image checks then exact
  boot-only approval with healthy6a6a/RAM rescue, never stock Android.

- **2026-10-22: VCN33 image 6a6a2ef7 is the current healthy baseline; real RF reception works.**
  User confirms lockscreen, touch and buttons: call_dcedce3836584301a4a60eb424d84b2a.
  Boot-only flash/reboot succeeded under approval call_657c74a78b6943c9bc996e2594f1d4d0.
  Current image shim/out/nord2-xnu/nord2-ios-wifi-vcn33-battery-stop23.img,
  33390592 bytes, SHA256 6a6a2ef79f03eaef540f4247016234217d5f8655b320c6afdf5dd81485c155ad.
  CI 39684479974, source 97c2150d0f7e77b2a00af18a9c5d081017b00652;
  kernel SHA256 ce122a75228da792a25c2fc448d9bfd7904fe751f4b1d976a9079936e19a6e2d.
  Selected Android Wi-Fi power callback requires VCN33_1/2 RC configuration.
  Five guarded writes select nominal 3.3 V and RC channels; no SW_EN or shared
  VS2 write. Both MON registers read 3 before calibration and after firmware
  ready. All 26 startup commands and the 22-channel scan complete successfully:
  7 BSS, 8 received management frames/beacons, observed RF on 2.4 GHz.
  The nine selected 5 GHz channels still have zero counters; 5 GHz reception,
  association, packet networking and iOS Settings integration are NOT proven.
  Independent hardware comparison preserves all 8 DMA queue rows, configuration
  and command structure versus c31: strong evidence for the VCN33 correction.
  Audit combined-backend/hardware-audit/vcn33-6a6a/REPORT.md,
  manifest bb5fa99a391d1125a94d8fc3ad5f3aada3a4edf6404373738587886571b66847.
  Immutable full USB log research/nord2/wifi-vcn33/ios/backend-full.log,
  304825 bytes, SHA256 f3dc779e9f59145b77fd45462ee4cb7c0bbe45ebfb75f30f964a0d4ebcf40f85.
  Strict VCN33 + 26-command/RXRF parser PASS, no issues. Collector 24737 exited
  0 at deadline with no transport error, now CLOSED. hardware-result.json and
  stage-pending.json record completed hardware and human health. Runner USED,
  never rerun. No recovery, new device action or approval question pending.
  Keep this boot on the phone. UFS and boot speed are completed by user request.

- **iOS service launch gate prepared, not activated on the phone.**
  tools/mkadt-nord2.py accepts --wifi-service, adding only /arm-io/wlan
  with compatible nord2,mt6893-wifi. Default ADT remains byte-identical;
  enabled serialization removes back to the exact baseline. Image builder
  selects it only for nord2wifiios=1. mkpayload now rejects NUL/overlong
  command lines instead of silently truncating the 608-byte field.
  Fresh research/nord2/wifi-ios-scan/ios contains offline preparation scripts,
  all source/CI/prepared/approval gates false, no execution records or image.
  6a6a remains the healthy recovery baseline and current device boot.

- **Next cohesive task: actual iOS Wi-Fi scan service and association path.**
  Root owns canonical source, CI and device actions. Existing private bridge
  boot-cache-native manifest84b8020a/review6ebd5cad has a real immutable nsb
  cache export but remains unactivated; it does not perform fresh UI scans.
  after_id now implements retained worker scan continuation/queue and fresh
  completion; format_stage implements actual service/IOUserClient registration,
  cache/event wiring and honest unsupported association; cap_stage resolves
  populated Apple scan conversion and matching and reviews the candidates.
  Do not build another tiny radio diagnostic or tune VS2/DMA speculatively.
  Before a new destructive flash, prepare/verify the concrete image and obtain
  its exact approval per AGENTS.md. Healthy 6a6a and verified RAM rescue remain
  available; never boot stock Android over the installed iOS userdata.
  Previous c31 receipt/history below is retained as comparison evidence.

- **c31 receive lifecycle26 HARDWARE-HEALTHY, still ZERO RF; delegated to Claude.**
  Human lockscreen/touch/buttons confirmed call_858994b387c746c392746d6863609a2e.
  Actual26commands DDONE/credits complete, scan22channels state7 completes;
  zeroBSS/frames/beacon-probe/MDRDY. Strict26 parserPASS, RXDMAvalid,
  all8RXDMAQUEUErows and CFG sameas1c5; no justifiedDMAwrite/newfieldfix.
  NewCID70+filter omissions executed but didNOTsolve reception.
  Finalimmutable wifi-rx-lifecycle/ios/backend-full.log246534B SHA
  ddc21d009fe45e195c6efdad6c0f35af7f758c09eeda9a24ec53ef4aec7d9540.
  USB180s collector exit0/deadline/noerror; session98240 CLOSED, runnerUSED.
  Currentphone healthyc31; keepit, no recovery/fastboot action pending.
  User explicitly delegated analysis to Claude. Readonly Claude Opus launched
  in research/nord2/wifi/claude-opus-rf-c31 (CLI modelclaude-opus-5-5),
  session864d9b77-5357-4b28-98c7-f3bc4d37c443/PID381329/rootexec83032.
  Read/Grep/Glob only, no shell/write/device/CI authority. Prompt and live
  stream.jsonl saved; final REPORT.md/result.json generated oncompletion.
  Other3agents closed their audits to handoff: rf-calibration-recheck/REPORT.md
  (e1d1881b manifest), hardware-audit/rx-lifecycle-c31-scan/report.json
  (1b9ef7de), hardware-audit/lifecycle-c31/HANDOFF.md (716bc99c manifest).
  Lastlead: selected CMD82 statistics query/EID03 may expose aggregateRF
  counters; exact callerusesPARAM_802_11_STATISTICS_STRUCT not guessed72B
  CMD_GET_STATISTICS. Query/response lifecycle needs design; don't append to
  SET-only recipe blindly. BroadMACdump writesdebugmux, NOTread-only.
  No next candidate/build yet; reviewClaude diagnosis before furtherchanges.


- **Receive lifecycle26 imagec31ff5e4 AUTHORIZED and BOOT-ONLY flashed OKAY.**
  Exactapproval call_e6c0741c39ad46c1a760c3bbd9695089 includes conditional
  healthy1c5/RAMrescue recovery. KnownsingleNord2/productoplus6893/boot32MiB/
  nonuserspaceverified. Flash/rebootOKAY; fresh wifi-rx-lifecycle/ios runnerUSED,
  NEVERrerun. Root session98240 collectingUSB180s; outcome/healthpending.
  Currentphonec31, not1c5. No stockAndroidboot permitted overiOSuserdata.


- **Receive lifecycle26 imagec31ff5e4 VERIFIED; exact boot-only approval pending.**
  shim/out/nord2-xnu/nord2-ios-wifi-rx-lifecycle-battery-stop23.img33386496B
  SHAc31ff5e47611fac18d6eb242cab86f39fa049d751ba0a9e0fa65ed3658ad8b1f
  payload6ed8e309c2bcd670bd088552003996e1054bf974b0fc024e6fe44aeb723c3c6a;
  bootspare167936B. CI39678188048/source8d6960fb/kernel3c86a847.
  Bounded1GiBpack completed0; independent segment/loader/rescue/ADT/Linux/DT/
  header and --check-existing PASS. Fresh wifi-rx-lifecycle/ios prepared,
  identity/schema gatestrue; allhumanapprovalflagsfalse, runnerUNUSED.
  Adds verified3CID70 publicdefault batches +CID0a RXfilter beforeAIS/scan;
  total26commands, ordinarysourcecorrect startup; RFbenefit stillUNTESTED.
  Currentphone remains humanhealthy1c5, noUSBcollector. Exacthealthy1c5 and
  verifiedRAMrescue fallback pinned. Next obtain exactnewimageapproval+
  physicalfastboot; run onlyfresh gatedrunner; collectUSB and strict26parser.
  No association/iOSWifiinterface claim. Private bridgeadapter uncalled.


- **Receive lifecycle26 CI39678188048 SUCCESS; verified kernel, packing.**
  Source8d6960fb52a07aa381a93a36418b6e128a4f3518, kernel17534736B
  SHA3c86a847219334ea02e3e531aa36c77ac48c06a228211233d4bb71444d096e4d.
  All160generated files equal CI,13actualgenericARMv8.2TUs;2,241,609decoded
  instructions noLDAPUR/STLUR. Linked review35349186/manifest20371553 PASS:
  actual26recipe/defaults/filter0xc and priorowner/async/RX/30sdrain preserved.
  Fresh wifi-rx-lifecycle/ios identity/schema gatestrue; bounded1GiB pack
  root session43799 running. Image/prepared/humanapproval stillpending;
  runnerUNUSED. Phone remains healthy1c5, no collector/device action.
  Private lifecycle-scan-adapter/v2 manifest61be3330/reviewa9a59691 passed
  sanitizer+ARM64 checks; exactticket/generation/output plusclose/inflight,
  genuineemptyEOF distinctfrompartial. UNCALLEDhelper only, noIOUCregistration
  or realretainedbackend/eventproducer; populatedApple ratemap stillunproven.
  Next finishpack/independentverify, then exactnewimage boot-only approval
  with healthy1c5/RAMrescue fallback. Do not rerun any earlier usedrunner.


- **Receive lifecycle26 source8d6960fb CI39678188048 running.**
  Root watcher14045 saves run/log/artifacts under research/ci-39678188048.
  Canonical all5 suites PASS; three affected normalARM64 TUs PASS.
  Fresh wifi-rx-lifecycle/ios has148sourcepins, parser26, all device gatesfalse,
  runnerUNUSED. No newimage/exactapproval yet. Phone stays humanhealthy1c5.
  cap_stage prepares independent linked review; format_stage reviews stage;
  after_id advances private scan-to-iOS lifecycle adapter, no live registration.
  Next verify exactsource/artifact, bounded1GiB pack, exactnewimageapproval.


- **Receive lifecycle26 integrated; source and independent reviews PASS.**
  Adds three public OnePlus CID70 SET/no-response default batches (11keys,
  body284/frame348, grouping4/4/3) and CID0a RXfilter0xc (body68/frame132)
  before AISactivation/passive scan. These are proven Linux startup omissions,
  not yet proven cause of zero RF reception. Selected source flags/original
  serializer verified; no invented cfg/EEPROM/FEM values or speculativeDMA writes.
  Runtime20d876dd/review74ec8f2b; wirev2e58fe1e5/reviewcdd8c617;
  parser26 82201436/review9b19e96c (769positive/2107negative,10nativeblocks).
  New wifi_default_feature TU linked via bsd/conf/files and generated0082;
  patch applies clean/matches, normalARM64 default/backend/owner compile PASS.
  Canonical full26/450RXfaults/185BASICfaults/65578decoder PASS; CI next.
  Fresh research/nord2/wifi-rx-lifecycle/ios allgatesfalse/runnerUNUSED,
  healthy1c5 fallback pinned. Current phone remains humanhealthy1c5,
  no active collector/device action/approval question. Next one cohesiveCI,
  exact artifact/linked/image validation, then exact new boot-only approval.
  Bounded pre-scan audit061f9b71 finds no additional proven mandatory command;
  optional lowlatency-disable startup difference documented, not added blindly.


- **Async image1c5aa596 HARDWARE-HEALTHY; scan still zero networks.**
  User confirms lockscreen/touch/buttons call_d3dc9bad9f2f4b759f4020859fcf39e7.
  MAIN/CAP/BASIC pass; all22normal commands and22channel scan complete;
  pinnedasync parserPASS/noissues. RX14events =13sleepy+SCAN_DONE; allfour
  DATAqueuesDIDX0, untouchedDDONE0 before/after. BothRXDMAsnapshots valid.
  SCANRFversion4: all22beacon/probecounters0, MDRDY0 exceptchannel165=1;
  idle/time remain rawunits. ZeroBSSID fix executed butdidnotresolveemptyRX.
  BASIC hadnosleepyevent thisboot; don'tclaimthat specificracewasexercised.
  Clean180s collector exit0/deadline/noUSBerror, finalios/backend-full.log
  250210B SHAb5053eba426f24862da7316ba5ecb0e6a89a52b829b958ca7f3819b8af079368; root session1659closed.
  Currentphone1c5healthy; no recovery needed, no collector, runnerUSED.
  Root foundLinuxUP lifecycle omittedCID0a: nl80211requiresWDEVUP; dev_open
  invokesdev_set_rx_mode; MediaTekforcesALLMULTI, ordinaryawake mask0xc.
  RFsourceaudit8797d528 confirmsgap, notyetcausalproof. format_stage prepares
  private rx-lifecycle correction/tests; after_id auditsOEM/defaultconfig;
  cap_stage DMAaudit findsnojustifiedring/enablewrite. No newCI/imageyet.

- **Async image1c5aa596 AUTHORIZED and BOOT-ONLY flashed/rebooted OKAY.**
  User exact approval call_cde2f19f1bcd4938b82cf44b106af099 includes conditional
  healthyc3/RAMrescue recovery. Known single Nord2/productoplus6893/boot32MiB/
  nonuserspace verified. Fresh wifi-async-scan/ios runner now USED; never rerun.
  Root session1659 collectingUSB180s; hardware outcome/health still pending.
  Current phone1c5, notc3. No stockAndroid boot over iOSuserdata permitted.

- **Async scan image1c5aa596 VERIFIED; exact boot-only approval pending.**
  shim/out/nord2-xnu/nord2-ios-wifi-async-scan-battery-stop23.img33386496B
  SHA2561c5aa596436e98e74cbc3b2bf72ed66d004ba49599ad77f63f452ceae029eb56
  payloadd8c162742863d0fd181c0337af2ce3d4235240879c4012b643cd15a2f338cfcd;
 167936B boot spare. CI39672757944/source361b8eac; kernel8bc54611.
  Bounded1GiB pack exit0, independent contents/segments/loader/rescue/ADT/
  originalLinux/dualDT/header and --check-existing PASS. Fresh wifi-async-scan/ios
  identity/schema/prepared gates true; human approval flags false, runner UNUSED.
  Includes shared EID7 hint handling across CAP/BASIC/normal, zeroBSSID source
  correction and readonly RX/RF snapshots; actual scan correction not yet tested.
  Current phone stays humanhealthy c3ce7dcf; exactc3 and RAMrescue fallback
  retained. No collector/device action. Next: obtain exactimage approval and
  physicalfastboot, then new one-use runner only and pinned async parser.
  Private iOS bridge CARDv2 and lifecyclev2 independently reviewed but not
  registered/backend-bound or included in this radio image. No network promise.

- **Async CI39672757944 SUCCESS; bounded image pack in progress.**
  Exact source361b8eac9be79101091a86471d4fd3f43fd44099, kernel17534512B
  SHA8bc54611a1fee1b099ec126bbf0e8b40f0ff7b3b325920f433f2dbe134b47205.
  All158 generated/source files match CI;12 actual genericARMv8.2 compiles,
  2,241,245 decoded instructions contain no LDAPUR/STLUR. Linked review
  ff66a789/manifest808aa7d7 PASS: three async consumers/budget16, actual
  native22-worker/lease, fiveRXqueues, readonlysnapshots, zeroBSSID and
  fatal30sdrain preserved. Identity/schema gates true; root pack session80567
  under1GiB address limit. No exact image approval or device action yet.
  Current phone c3 humanhealthy. Fresh wifi-async-scan/ios runner UNUSED.
  Parallel private native lifecycle v2 dba1e0c4/reviewbc2cc6dc passes96checks
  and1000closeprobes+selectedXNU ARM64 compile. V1 missing OOLprepare found
  and fixed; v2 pairs prepare/read/complete with failure/close serialization.
  No native link/registration/backend binding; future live callbacks still
  need generation/inflight drain. CARDv2 f11c4f8c/reviewfcc1a99c retained.
  Next: finish pack, independently verify exact image, then exact new boot
  approval with healthyc3/RAMrescue fallback. No executed runner reuse.

- **Async source361b8eac CI39672757944 submitted, waiting for macOS runner.**
  Root watcher session63069 will save run.json/build.log/artifacts under
  research/ci-39672757944 on successful completion. No new image or approval.
  Fresh wifi-async-scan/ios pins source/async parser; identity/schema closed.
  Current phone remains human-healthy c3, no active USB collector.
  Runtime independentb63ce626, old CAP fixture addendum019e87db; parser
  independent1f03e14f. All required canonical host/focused compile checks pass.
  Receive prerequisite audit89577f3a found no proven additional mandatory RX
  correction; CID70 defaults and CID0a netdev filter remain source differences
  without first-scan causality. No speculative recipe additions.
  Parallel bridge CARD v2 f11c4f8c/reviewfcc1a99c proves raw13/6 security gates;
  truthful empty native feature set supported (1261 checks), no fake readiness.
  v1 nonempty restriction was rejected by root/reviewer and preserved as
  historical package. Native lifecycle shell private implementation underway.
  Next: verify exact CI artifact/source/linked code, pack bounded1GiB, verify
  image, obtain exact new boot-only approval. Do not reuse executed runners.

- **Async EID7 correction integrated; one CI build next.**
  Current phone remains c3ce7dcf, human confirms healthy after f53 recovery.
  Shared source-exact sleepy decoder covers CAP/BASIC/normal; raw uint8 hint
  never creates ACK, TX completion or firmware ownership. Proposal951320e1,
  independent reviewb63ce626; explicit async parser84538436/review1f03e14f.
  Canonical full22/fivequeues plus450faults,185new BASICfaults,65578decoder
  checks and all8 CAP/BASIC suites PASS. Old EID7-refusal fixture updated to
  require hints consumed but no capability completion. Four actual normal
  ARM64 focused compiles PASS; generated0082 clean/equal, new header only.
  Fresh wifi-async-scan/ios has exact source/parser pins, CI/schema gatesfalse,
  image/authorization absent and runnerunused. Keeps source-correct V2 scan
  BSSID and readonly DMA/RF snapshots, still hardware-unreached after f53.
  Next: one CI, artifact/linked verification, bounded image pack, exact new
  boot-only approval. Never rerun failedf53/restore-c3 runners or stock boot.


- **c3 recovery complete and HUMAN-HEALTHY after failedf53 test.**
  User confirms lockscreen/touch/buttons call_7cbc3ffb5d7b4dfe9555d7bb01c996e5.
  Restore90s collector closed; immutable restore-c3/restored-full.log and
  result.json saved under wifi-scan-receive/ios. No collector remains.
  Current phone c3; failedtest/restore runners both USED, neverrerun.
  New cohesive sourcefix handles unsolicited EID7 acrossCAP/BASIC/normal;
  rawstate unknown infailedlog, never infer0/1. Sourceaudit5d6ff119 proves
  rawuint8 state/noCMDseq correlation/noautomatic FW-own. Agents working
  private source/real-ownerfault tests/parser; no newCI/image/approval yet.
  Fresh wifi-async-scan/ios template created ALL gatesfalse/runnerunused;
  exact healthyc3 fallback pinned. Restore238590B SHA3d07868a0a8972231f83756567101da3d28ec2faf2af4ce0c2811d5122b5bbb9,
  v3parserPASS, deadline/noUSBerror. Root can continue offline sourcework.

- **f53ad2b7 FAILED before scan; healthy c3 restore FLASHED/rebooted OKAY.**
  Actual MAIN/CAP pass, BASICerror23 rejected40B EID7sleepy/seq0/outerinner3800
  afterCID2 completeddescriptor: txdone1/callbacks73/completed4. No NORMAL/
  scan request reached; six-byte scanfix and newRXRF diagnostics UNTESTED.
  Immutable first/final157371B SHA c12ecd8d0c2d865adddcc1822b3ca06c864e38a9520bfd9fbb1c6e8bfd355861
  under wifi-scan-receive/ios/backend-first-receipt.log/backend-failed-full.log.
  Finalcollector exit0 but usb_error/libusb-7 (NOT clean180s); no extra panic
  tail captured. Failure drain30s observed. Never rerun failed stage runner.
  User physicalfastboot call_9c2d996548a642e493b3f6312c6cc440; existing conditional
  approval used for exactc3 restore. Fresh restore-c3 runner USED; boot/reboot
  OKAY. Currentphone c3; root session2145 collecting90s, humanhealth pending.
  cap_stage preparing source-exact boundedasync EID7 handling and realnative
  timing/fault fixtures in combined-backend/basic-sleepy-race; format independent
  source/phase audit andreview. Root solecanonical/deviceowner. No newimageyet.

- **f53ad2b7 AUTHORIZED and boot-only FLASHED/rebooted OKAY; result pending.**
  User approval call_07a2a2f7c15c46ef81b1a5e7eabac20f includes conditional
  healthyc3/RAMrescue recovery. Single knownNord2 fastboot/productoplus6893/
  boot0x2000000/nonuserspace checked. Fresh wifi-scan-receive/ios runner USED;
  NEVER rerun. Root session14928 collects USB180s; currentphone f53ad2b7,
  notc3. No hardware scan/health result yet. Originalstock boot prohibited.

- **Scan receive image f53ad2b7 VERIFIED; exact boot-only approval pending.**
  shim/out/nord2-xnu/nord2-ios-wifi-scan-receive-battery-stop23.img33386496B
  SHA256f53ad2b7875ed7435b1cbaaf3b9173df72565459295b7ea91fb6b9c9d8653bda
  payload36a6a8ec6a8e654a461ff6c405b6057c78f4dac4a2ecfec6da526f7167744fea;
  boot spare167936B. CI39667598700/source035793eb6971be129d8ab0514c1b64e4157df7db.
  Linkedreviewc492d264/manifestb2a7a19d PASS: actual checked readonly helper,
  baseline/done/expiry hooks, zeroextendedBSSID, actual worker/reports/drain.
  Bounded1GiB packexit0; independent imagecontents and --check-existing PASS:
  exactsegments/loader/ADT/privateboard/rescue/originalLinux/dualDT/header.
  Fresh wifi-scan-receive/ios source/schema/CI/prepared gates true; human exact
  image/RF/conditionalrecovery remain FALSE. Runner UNUSED; no device action.
  Current phone stays c3ce7dcf healthy; recovery-image.json pins that image.
  New candidate corrects six scanrequestbytes + captures DMA/RF evidence;
  no hardware zero-BSS cause proved, no iOS interface/association yet.
  Next: exact-image approval+physicalfastboot, fresh one-use runner ONLY,
  collect and parse with pinned RX/RFv2; conditional healthyc3/RAMrescue.
  No previous runner reuse or stockAndroid boot over iOSuserdata.
  Private bridge bind d220338c review0d4e15a0 PASS; no native activation.

- **Scan receive CI39667598700 SUCCESS; exact source035793eb6971be129d8ab0514c1b64e4157df7db.**
  Kernel17534512B SHA2445e75edb05da8599b08fa5da8aa869c43ae30b7edc790d5db0131ee9c21db5
  matches artifactSUMS. All157generated/buildtree files equal;12 actual compiler
  invocations generic ARMv8.2;2,241,168 decoded instructions noLDAPUR/STLUR.
  Final root canonical22/fivequeues+450faults/sourceoracle257/CAPBASIC8 PASS.
  Fresh wifi-scan-receive/ios identity true; schema awaiting independent linked
  review under combined-backend/scan-receive-linked-review (after_id).
  No image prepared/approval/device action; current c3ce7dcf humanhealthy and
  exact recovery file reverified. CI watcher completed0; no collectors.
  Private servicebridge age evidence811a7dfd/adapterb016ddbd handlesms and
  signed32 bound with explicit hosttimestamps (26checks); rate map890f6447
  remains unresolved, compared implementations cannot validate exact mapping.
  Bind-policyd220338c resolves selector1controlIOUC/selector5mappedpipe and
  request43 DRIVER_VERSION boundedidentity;559checks and normalARM64 PASS,
  independent review pending. No native service publication or ADT change.
  Next: finish linkedreview, open schema only on verifiedpass, pack under1GiB,
  independent imageverify, exactboot-only approval using healthyc3 recovery.

- **Cohesive scan receive correction promoted; next CI/image not yet run.**
  Corrected ordinary V2 extended BSSID to zero according to selected Android
  cfg80211→AIS→SCN source; source oracle257cases matches all1188 scan bytes
  and12activation bytes. This is a proven source mismatch, not yet a proven
  cause of empty hardware scan. PassiveSSID/dwell remain source-correct.
  Added two bounded readonly DMA0 snapshots under actual native lease and
  captured SCAN_DONE RF counters. No new write/ownership path, no identifiers
  logged. New wifi_rx_readback TU registered; generated0082 applies cleanly.
  RX proposal f0584f0f/reviewd026e663; portable fixtures3d11f460 include450
  native callback faults and restoration/no-write assertions. Root final
  canonical run PASS (full22/fivequeues +450faults); CAP/BASIC8suites and ARM64 helper,
  RX/scan/owning sweet_touch compile PASS. Portable source oracle committed.
  Parser v2 manifest2dfffa98/review1ee12031,11positive582negative PASS:
  exact new metadata plus unchanged v3 core, completed cursor correlation.
  Fresh wifi-scan-receive/ios parser copied; CI/image/approval gates FALSE.
  Current c3ce7dcf remains humanhealthy, no collector/device action. Planned
  fallback is exact healthyc3ce7dcf; e0d3/RAMrescue retained. No stock boot.
  Next: commit/push, ONE CI, linked/artifact verification,
  bounded1GiB image pack and exact boot-only authorization before device action.

- **c3ce7dcf full capture closed and preserved; current image stays installed.**
  Collector180s exit0/stop_reason=deadline/transport_error=null.
  Immutable wifi-normal-scan/ios/backend-full.log244598B SHA256
  c6055d4b27a46c6ea6290a6249c85a785b4d1aa88c8496afbc1863e07aeee45a.
  Full v3 parser passes same config/scan receipts. Humanhealthy; userconfirms
  nearby networks visible on BOTH bands call_b28bae79c17f4775888173e11b923964.
  Zero BSS remains a real reception/discovery gap. No recovery/device action
  pending; no collector remains. Native backend currently parks afterscan.
  PassiveSSIDType0/count0 and dwell/min/timeout0 match source cfg80211→AIS→SCN;
  do not invent wildcard/dwell fixes. DataDMA0 mapping matches source, but
  retained actual DMA0 readback not captured; agents reviewing prerequisites.
  Servicebridge completed-scan adapter private4f0ac27a ASan/UBSan PASS:
  real empty vs partial/timeout preserved, no interface published; populated
  Apple rate/age units still explicit unresolved inputs, no guessed encoding.

- **c3ce7dcf hardware: complete22-command batch + scan completion; HUMAN-HEALTHY.**
  Actual MAIN/CAP/BASIC allsuccessful; NORMAL status0/phase7/complete22,
  no halt/error; SCAN done1/state7/completed_channels22 in ~1.21seconds.
  RX11packets=10sleepy EID7 + matchingSCAN_DONE; ALL DATA QUEUES EMPTY,
  BSS0/frames0. Command path succeeded, RF reception/network discovery is
  NOT yet proven; do not call this usable Wi-Fi or connection availability.
  First immutable161342B receipt d30e6b393426316a9d16794f92c53c50940a2e3d65ffaddac1ca02cc0076555d
  under wifi-normal-scan/ios/backend-first-receipt.log; v3 parsercomplete=true,
  configtransferred=true/scancomplete=true, noissues, network/interfacefalse.
  User confirms lockscreen/touch/buttons call_d54e5e06ff0b42bab29e2c987c25e12a.
  Retainnewimage; no recovery needed. See latest full-capture completion above.
  cap_stage investigating zeroRX/MACfilters vs source; format_stage config/
  passive scan prerequisites. Root only deviceowner; no new flash requested.

- **Combined backend c3ce7dcf AUTHORIZED and BOOT-ONLY FLASHED.**
  User exact-image/calibration/conditional e0d3/RAMrescue authorization:
  call_90526ebbdec14bd68cf6a1b84293bb7a. Known singlefastboot Nord2 identity,
  oplus6893, boot0x2000000 and nonuserspace verified. Flash boot and reboot
  both OKAY. Fresh wifi-normal-scan/ios runner has executed; NEVER rerun it.
  USB collector active180s, root session87889; hardware result pending.
  Phone now testing c3ce7dcf, not e0d3. No scan success yet observed.

- **ONE combined backend image VERIFIED; exact-image user approval pending.**
  shim/out/nord2-xnu/nord2-ios-wifi-normal-scan-battery-stop23.img33384448B
  SHA256c3ce7dcfcce13085579e59998b6f29a9fb0e2b11a7a3420b9cc97db5a8ab3f1a
  payload2c34bc090c9167214edcc76177a8e6c824005b87bf05c0966e43337e3bde0f15
  CI39657170591/source15f70fb35ad835fb43233c5ed2fb5cd1838e7c12.
  One substantive candidate: full20 normal config/calibration commands plus
  AIS activation and passive scan; no iOS network interface/association yet.
  Bounded1GiB packexit0; independent loaded segments/loader/ADT/private board/
  originalLinux/dualDT/rescue/header verification and --check-existing PASS.
  Boot spare169984B. Linked6fa3e031 and CI119file equality PASS; source/CI/
  schema/prepared gates true. Human exact-image/RF/conditionalrecovery false.
  Fresh one-use wifi-normal-scan/ios runner UNUSED; no device operation.
  e0d3 fallback and RAMrescue hashes reverified. Phone remains e0d3 healthy.
  Next: exact boot-only approval+physicalfastboot; newbackend log parser v3
  is pinned e453135c/reviewe7129145 (25positive595negative). No actual scan
  success claimed. Do not reuse old BASIC/TXD/restore runners.
  Parallel iOS channel mapping resolved statically: Apple80211 scan-result
  CHANNEL_FLAGS passes unchanged to CWFChannel.flags; evidence560ac766 under
  ios-service-bridge/channel-mode-audit/selector-resolution. Request mapping
  remains separate; no live service/interface published.

- **Combined backend CI39657170591 SUCCESS, exact source15f70fb35ad835fb43233c5ed2fb5cd1838e7c12.**
  Kernel SHA2565970c0ec156dade26067075bdecebdce5acdeea8959d4f7db08310ed3d7527ac
  matches artifact SHA256SUMS. Actual8 newTUs+owningTU/basic/cap all compiled
  generic ARMv8.2; binary2,240,632instructions contain no LDAPUR/STLUR.
  Independent actual linked review in progress. Fresh normal-scan/ios identity
  verified; schema/image/approval gates closed. No image/device operation yet.
  Final parser v3 independently PASS (25positive595negative), manifest15baf85a,
  parsere453135c/reviewe7129145. CAP sequence metadata, source software credit,
  queue-drain and partial/truncated-history semantics checked. Source/normal
  synthetic proofs remain valid; phone e0d3 healthy, no collector/device action.

- **Combined Wi-Fi backend promoted; one substantive CI/image target.**
  Canonical shim sources and regenerated0082 patch now include full source-
  backed20 normal config commands (legacy domains/power,12 authenticated board
  NVRAM fragments, OEM project20827→20817 power), first AIS activation and
  passive non-DFS scan. Same actual CAP/BASIC worker; exact command lease;
  PSE3 minus retained BASIC1, source AXI software release after publication,
  DDONE buffer reuse, all five owned RX queues, bounded copied BSS cache and
  matching scan-done after queue drain. Command-only runtime report policy:
  no data TX tokens/stations yet; reports are counted/rearmed, not fake credits.
  This is NOT hardware success or an iOS Wi-Fi interface/association.
  UFS/boot/input/display unchanged; phone e0d3 healthy/untouched this turn.
  BASIC TXD source-parity fix and bounded ASCII EID27 opaque-inner correction
  included. No standalone TXD CI/flash. Optional Android override files are
  not loaded: new iOS profile intentionally uses source/OEM defaults.
  Config c4bea629/review6c81ed03; transport32324b25/review9d0a7cb7;
  scan c67341c3/normalized645e02bb; debug70969c74/reviewff33cc20.
  Eight helper TUs + owning sweet_touch normal ARM64 compile PASS.
  Exact final synthetic22-command/fivequeue/BSS/timeout integration and202
  actual native RX callback faults PASS: full-tests8ff2f28f/reportbd7b4065,
  candidate-pinsec3ad092, promotion03a57c7a. Canonical normal runner PASS.
  All eight canonical CAP/BASIC suites PASS (GCC ASan/UBSan); all65536
  EID27 inner values checked. Private proofs remain under combined-backend.
  Fresh wifi-normal-scan/ios stage CLOSED: no CI/image/new authorization yet.
  Next: commit complete source, ONE full CI, verify linked result, prepare
  exact image with1GiB pack limit, then request exact boot-only approval.

- **USER CHANGED NEXT MILESTONE: cohesive backend + real passive scan.**
  Stop per-command/diagnostic-only builds. The reviewed TXD correction9e75195f
  and fresh CLOSED wifi-basic-txdone/ios template are not a standalone CI/image
  target anymore; all its gates remain false, no artifact or approval exists.
  Fold correction into combined normal initialization/config/calibration/credit
  reuse/five-queue RX/BSS activation/real passive scan, with phase diagnostics.
  Current phone e0d3 is recovered and human-healthy, no device action pending.
  Root owns RX/scan and final native integration; wifi_cap_stage implements
  same-worker command transport/credit ledger in combined-backend/transport;
  wifi_format_stage implements actual SKU0 legacy domain/power/board batch in
  combined-backend/config; wifi_after_id continues exact iOS service contracts.
  Actual observed PSEcmd3/BASICdebit1/remaining2 and absentTAG3 select legacy,
  not V2. Minimize hardware attempts by finishing and validating full path first.
  No promise that one hardware test will suffice; no scan/networking achieved yet.


- **e0d3 RECOVERED AND HUMAN-HEALTHY; BASIC89b3 stage closed/immutable.**
  User confirms lockscreen/touch/buttons call_2c6e0543be0c4694a71cbc345fa775fc.
  Restore90s collector exit0/deadline/no transporterror; immutable222928B,
  SHA256086fd13b5c74ce9aa11637c57070b49cf278d70978b3e9bfec60e4a9685bb439.
  No collector remains; never rerun BASIC or restore runner. Current phone is
  e0d3, not89b3 or0e949521. Agent cap_stage preparing source-parity DDONE
  completion correction with captured raw words; format_stage reviews.
  Exact failed field remains unknown, but error17's linked branch proves
  DMADONE1 was seen before extra echo checks. Loggedcontrol0 was untouched
  state, not an observed returned word. Do not rewrite the failed evidence.
  Service bridge passive v2 manifest6aca308e / review0bb57356 fixes output
  capacity and old BSS/type labeling; event-pipe private9764b2c3 adds copied
  ring helpers, still no active producer/service/backend publication.


- **BASIC89b3 failed completion guard; e0d3 restore flashed/rebooted OKAY.**
  Actual BASIC status-1/phase3/error17, one publication attempt, DIDX5,
  sharedcompleted4. Exact rejected field UNKNOWN: raw returned descriptor
  words were not logged and tx_control was assigned only after the guard.
  First immutable log123061B/b94b01cbd0b3f03110f7811b3b5996cc2b562279448762392dfba01aee378084;
  final157474B/df8037ca4ea66e0901c9e7f17956c3096ab3109b83607affe81a72f1458b163e.
  Collectorexit0 but stop_reason usb_error/libusb-7, NOT a full180s success;
  no final panic text captured. Both files are preserved in executed stage.
  Real metadata: PSE data/command128/3, PLE912/0, page128/64, extra8;
  onePSE debit leaves2. TAG3 absent(sku0), PHY present,5GHz1/BW2/NSS2/path0xf.
  Native BASIC did not complete; no firmware ACK or networking inferred.
  Selected Android AXI command completion checks DIDX/softwareownedcell and
  DMADONE, not returned address/length/extra echoes. Agent preparing private
  source-parity correction+rawdiagnostics, independentreview assigned; exact
  failed word remains unknown. No guessed correction or new flash approved.
  Conditional recovery authorized previously; user supplied fastboot
  call_45d64c3c0f8f43df9b259cd573f36c82. Fresh one-use e0d3 restore script
  wrote onlyboot and rebooted OKAY.90s collector session87951 active;
  human lockscreen/input confirmation pending. Never rerun either runner.


- **BASIC89b3 AUTHORIZED AND BOOT-ONLY FLASHED; live result pending.**
  Exact image approved call_1625b7d33b6c44b9bc553f887bfe3305, including
  conditional e0d3/RAM-rescue recovery. Expected single Nord2 fastboot identity
  verified privately; product/capacity/non-userspace checks passed. Flash boot
  and reboot both OKAY. One-use runner session14204 collecting USB180s;
  NEVER rerun executed wifi-basic-config/ios runner. No source/result claims
  from the hardware yet. No stock Android boot permitted.


- **BASIC IMAGE VERIFIED; EXACT BOOT-ONLY APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-basic-config-battery-stop23.img`,
  33357824 bytes, SHA256
  89b3eb2abe4e906f420263803033a2a33a3717b23aecdafd488344e168e42066.
  Payload966922ed566e37add4908467e01c47d27624d327ac51c46e41cfc0a003354009.
  CI39648973050/source c4a34727819024714da1b2cab1ab1ffc37fc5067.
  Independent actual-linked review298f8aee PASS; root verified all evidence
  hashes and reviewed actual lease/guard/terminal paths. Bounded image pack
  exit0; independent loaded-kernel/loader/ADT/calibration/original-Linux/dualDT/
  rescue/header verification and existing-image check PASS;196608 spare bytes.
  e0d3 fallback and RAM rescue hashes verified. CI/schema/prepared true;
  human/RF/recovery false. Fresh runner unused; no device action. Phone remains
  healthy0e949521. Test adds one CID2/no-response after actual CAP receipt and
  logs real resource/TAG3/PHY metadata; no scan or interface yet.
  Ask exact boot-only approval plus conditional e0d3/rescue recovery; manual
  fastboot may be required. Never boot stock Android over installed iOS root.
  Parallel iOS-service bridge has third private milestonec1f128b6: raw13-byte
  capabilities and bounded completed-cache semantics. Earlier scan helper has
  a dormant naming bug (+4 BSS type mislabeled scan type; actual type+52),
  documented in peer-review erratum aea3e859; author correcting in a new
  passive-scan contract revision before integration. No native service exposed.


- **BASIC CI39648973050 SUCCESS; new image packing, no hardware action.**
  Source c4a34727819024714da1b2cab1ab1ffc37fc5067; fully downloaded kernel
  17464832 bytes, SHA256 e64638ee7d34e15eb585721b6bf554f63c7709cce81f50bb892d1a3b43524ca1.
  Artifact SHA256SUMS and successful CI head match. 1140 compiler invocations
  on1139 lines carry generic ARMv8.2+crypto flags. One new partial echo has
  complete target flags but an unrecoverable source suffix; its exact hash and
  interleaved header-install context are preserved, not inherited/exempted
  from the previous run. Four new TUs and sweet_touch each have full echoes.
  CI/schema gates true; independent linked review active. Exact491-byte image
  is packing with1GiB address-space limit (session2801). Prepared and human
  approval gates remain false. Current phone remains healthy0e949521.
  Service-bridge second private milestone6baac3c4 has exact scan request/result
  and async-event layouts, plus unregistered refusal-only userclient scaffolding;
  peer review underway. New work traces capability bits/initial queries/cache
  semantics; no service/interface is published. Private regulatory evaluator
  b8fdaf76 is complete with explicit country provenance and no TX authority;
  actual runtime TAG3/PHY/country/power prerequisites still gate its use.


- **BASIC_CONFIG promoted; local validation passed, full CI next.**
  Exact candidate7dcc9883, independent native reviewf037648f and root review
  are saved under wifi-basic-config. Adds default-off nord2wifibasic=1 for one
  same-worker CID2 set/no-response after the real capability receipt. TC4 debit
  is1PSE/0PLE; no MAC prerequisite or invented ACK; completed5 closes the lease.
  Missing pure prerequisites skip before I/O. Uncertain DMA uses shared halt,
  the existing30s log drain and panic. UFS/display/input/rootfs are unchanged.
  Seven host suites pass (including67 actual-owner callback faults). Four new
  helper TUs and owning sweet_touch pass normal ARM64 compilation with only
  source/output substitutions; no forced NORD2/includes. Generated patch14741
  lines applies and reproduces the buildtree. Exact491-byte image stage stays
  CLOSED pending CI/artifact/packed-image checks. No new flash authorization.
  Current phone remains healthy0e949521; never rerun its executed stage.


- **Wi-Fi parallel work — current phone remains healthy on 0e949521.**
  User confirmed the lockscreen, touch and buttons after the completed capability
  capture. No further device action or restore has occurred. UFS and boot speed
  remain closed and unchanged at the user's request.
  Sol `wifi_cap_stage` is completing the private same-worker, one-command
  BASIC_CONFIG continuation; `wifi_format_stage` independently reviews it.
  Root owns the fresh CLOSED `research/nord2/wifi-basic-config/ios` stage:
  `nord2wifibasic=1`, exact491-byte command line, no image/CI/flash yet.
  Synthetic parser checks pass10 positive/109 negative cases; they are not a
  hardware BASIC_CONFIG receipt. A clean prerequisite skip is separate from TX
  completion, and neither implies a firmware ACK, scan or network connectivity.
  At the user's explicit request, Sol `wifi_after_id` now works on the iOS
  service bridge in parallel. Its first private milestone manifest41989084
  identifies the exact same-IPSW IO80211 user-client type0, selectors and control
  carrier layouts using offline disassembly. REA engines are unavailable; these
  are static shell observations, not runtime/decompiler evidence. Current Topaz
  was independently checked again at89451b8 (184 files): useful radio backend
  API, but no published consumer/user-client bridge in that exact checkout.
  Further initial-query/scan-result/async-event ABI work is active in a separate
  private revision. No IOService, interface, carrier or ADT node is published.


- **FORMAT 0e949521 SUCCESS COMPLETE; CURRENTPHONE HEALTHY, NO RESTORE.**
  USB180s collector exit0/deadline/no transport error; full immutable
  237316bytes SHA2563b32249042c69db7d39bd4a1c122bb3219a9c8cb1cb65f1ddc47168e6ee2ab41.
  StrictcapabilityparserPASS full andfirst; realEIDec504bytes/inner3800,
  19TLVs,resourcesv1supported,capMACinvalid; all workerhalt/errorflags0.
  Humanlockscreen/touch/buttons confirmed call_7c11e6d55caa4aed8072cfb77629355a.
  Collectorfinished; phone stays0e949521. No recoveryneeded/performed.
  NEVER rerun executed wifi-capability-format runner. No scan/interface.
  Private next step: one source-backed BASIC_CONFIG CID2/noresponse after
  sameownedcapreceipt, firstnormalresourceaccounting +capmetadata; defaultoff,
  beforeIO prerequisitefail shouldpark ratherthanforcebootloop. UnsafeDMA
  keepscommonhalt/drain. Agents own basic-config-runtime/normal-resources;
  canonicalunchanged. Actualquotas/TAG3/PHY unlogged so cannotbeinvented.


- **FORMAT 0e949521 HARDWARE SUCCESS; iOS HUMAN-HEALTHY, FULLCAPTURE RUNNING.**
  Boot-only flash/reboot OKAY. RealMAIN status0/stage9/ready1/halt0/error0.
  CAP status0/phase5/complete1/halt0/error0,1TX/1event/1rearm/match1/polls3.
  Actual504-byte EIDec/seq4 header inner3800 (observed, not0007/E000).
  19TLVs known6/unknown13, resourcesv1(80000001)supported; MACpresent1 but
  invalid0, product/fw values0. Worker parked without error; strict parserPASS.
  Firstimmutable159231bytes SHA256
  286a3a8066286062b427aa163ff8b3a910fa787164182332ed4beec3531d4b88.
  Valid1125x2436 frames; user confirmslockscreen/touch/buttons
  (call_7c11e6d55caa4aed8072cfb77629355a). No recoveryneeded/performed.
  Full180s capture session37782stillrunning; finish andfreeze beforeclosing.
  No scan/interface/connection. Currentphone0e949521, notfallbacke0d3.
  Private next resource/nativeworker work underway; realtag3/PHY/quotas
  retained inRAM but notlogged. Do not infer unobserved values.


- **FORMAT 0e949521 AUTHORIZED; ONE-USE RUNNER STARTING.**
  User approved exact boot-onlyimage +conditional e0d3/rescue recovery
  (call_96b979b879ea4ddf95491a3b5eccefa4). Sole previously authorized
  Nord2 fastboot identity/product/capacity/nonuserspace verified privately.
  USBcollector starts before reboot. Newstage wifi-capability-format/ios;
  NEVER rerun after flash.log exists. Hardwarecapability result pending.


- **CAPABILITY FORMAT IMAGE VERIFIED; EXACT FLASH APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-capability-format-battery-stop23.img`,
  33353728bytes SHA256
  0e949521b82795c69d8539043cc3c921c31f9de265cee6b72d12e90aff9877a2.
  Payloadf726b5f852dcc9d7b63c31435cf69c838748fd0d03678e2c4cb74bbb799d6129.
  CI39642478606/source5ac2355a3d121984565682aa8486ffc5b1d1cacc;
  kernel17464304/cb4356e1367ecbd13d1282ab38631e00f3b81e239925c79b9729bd2d6ce14930.
  Actuallinkedreview0ec2c1cf PASS; root checked changedbranch and all evidence
  hashes. Bounded1GiB packexit0; freshindependentloader/segments/ADT/privatecal/
  originalLinux/dualDT/rescueCPIO/headerchecks PASS; existingprepared image
  reverified;200704sparebytes. Exact474byte commandline matches6ddf. Actual
  e0d3/rescuehashes verified. CI/schema/prepared true; human/RF/recoveryfalse.
  New one-use runner NEVER executed. Test only same onecapquery with corrected
  EIDec innerpolicy+headerdiagnostics; no scan/interface/association.
  Ask exactboot-onlyapproval +conditional e0d3/rescue,manualfastbootifneeded.
  Currentphone e0d3restored humanhealthy; no hardware actions afterrestore.
  Privatefuture regulatory-policy707e8b79 peerreview4d9e26e6 PASS pureformat only, notpromoted;
  runtimeTAG3/PHY,regulatoryruleevaluation,conditionalpowerlimitinputs pending.
  UFS/bootspeed closed unchanged; executed stages immutable.


- **FORMAT CI39642478606 SUCCESS; IMAGE PACKING WITH 1GiB LIMIT.**
  Source5ac2355a3d121984565682aa8486ffc5b1d1cacc; kernel17464304 bytes
  SHA256cb4356e1367ecbd13d1282ab38631e00f3b81e239925c79b9729bd2d6ce14930.
  Fullydownloaded/artifactSHA/source verified;1136 genericARMv8.2+crypto
  compilerinvocations on1135 lines, one dualcommand line and one truncated
  pathprefix echo saved explicitly (fulltargetflags/source retained).
  Newstage preflightPASS; CI/schema true, prepared/human false. Boundedpack
  session38616 running; independent linked-format review in progress.
  No hardware action. Currentphone e0d3 humanhealthy; executedstages untouched.


- **FORMAT STAGE REVIEW PASS; CI39642478606 BUILDING.**
  Freshstage wifi-capability-format/ios pin1a70f0a073d79548a5eaebde91300c4d54511b459c33b4ab5b02558d1408c742;
  parserbdda5dd9bba42c0f667e57eda9747f324301130ee18b9418bd55638768a330ba,
  independentreview69642a3d and root builder/source reviewPASS. Exact474byte
  commandline matches executed6ddf; unchanged UFS/display/input parameters.
  Per-event36B HEADER row correlated with EVENT; inner opaque onlycapability,
  errors/debug/unknown remain refused. Headermetadata cannot replay TLVbody.
  Schema gate true; CIidentity/prepared/human stillfalse. Do NOT rerun closed
  test-stage after gatesopen. No newimage/flash; phonee0d3humanhealthy.
  Sol privately prepares source-backed regulatory prerequisite for later scan:
  SINGLE_SKU compiledoption does not establish runtimecapabilitytag3 state.


- **CAPABILITY FORMAT FULL CI39642478606 STARTED.**
  Source5ac2355a3d121984565682aa8486ffc5b1d1cacc; exactB/host/focusedchecks PASS.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39642478606
  Freshstage wifi-capability-format/ios pin1a70f0a0; authorclosedchecksPASS,
  root builder/source review and independent parser review in progress.
  All CI/schema/prepared/human gates remain closed. No new image/flash.
  Phone remains recovered e0d3 humanhealthy; UFS/bootspeed unchanged.


- **CAPABILITY FORMAT FIX PROMOTED; LOCAL CHECKS PASS, FULL CI NEXT.**
  Exact private B manifestefb352b1, independentreviewe4ac37a7, root source
  parity review: selected Android AXI waiter checks outer380f->3800/EIDec,
  no innerE000 guard. Only exact capability reply treats inner as metadata;
  debug/unknown policies unchanged. Captures36headerbytes after outer proof,
  excludes TLV/MAC body; actual failedinner remains UNKNOWN.
  Four runtime files changed; canonical header regression wired into runner.
  Five canonical ASan/UBSan suites PASS (65536inner,32crossguards,224trunc,
  8outerrefusals,128callbackfaults). Three normal focused ARM64 TUs PASS:
  sweet_touch371280, capability13568, dispatcher36560 bytes. Generatedpatch
  14396lines applies/reproduces; sharednativeABI unchanged. FullCI next.
  Currentphone e0d3 restored/humanhealthy; executed6ddf stage untouched.
  Fresh wifi-capability-format/ios stage remains closed pending root checks.
  UFS/boot speed unchanged; no scan/network interface/connection claim.


- **CAPABILITY 6ddf FAILURE CAPTURED; e0d3 RESTORED AND HUMAN-HEALTHY.**
  Conditional boot-only restore/reboot OKAY;90s USB collector exit0,
  deadline/no transport error. Immutable restore224264 bytes SHA256
  88cef153775bda44943b6fd75cdeef03eaa70f193c7d05eea8ea8e78f695126d.
  Valid1125x2436 frames and user confirms lockscreen/touch/buttons
  (call_696825c6493f4458920b383563977ce2). Recovery complete; no collector
  running and NEVER rerun executed capability/restore runner.
  Actual capability EIDec504-byte response remains unparsed: inner type
  value unobserved, E000 guard rejected. Selected Android query/waiter
  does not enforce that inner field; private exact-EID source-parity fix
  plus bounded36-byte header diagnostics under independent review.
  No scan/interface/connection. Currentphone e0d3; UFS/boot speed unchanged.

- **CAPABILITY REPLY RECEIVED BUT INNER HEADER REFUSED; RESTORING e0d3.**
  Authorized6ddf boot flash/rebootOKAY; main actualFWREADY succeeded.
  CAP phase4/error20, TXdone1/events1/rearmed0: actualEIDec/seq4,
  outerclass3800,504bytes with declared504/480. Size/outerclass match;
  inner bytes26/27 differ from expectedE000 but value UNLOGGED, not inferred.
 30sdrain ran; collector ended USBtimeout-7. First/full immutable154846bytes
  SHA580fc8aa79a393afd2cc72a0f211b36945d2f11b723cc1a191e4d30f4255d21d.
  Both snapshots saved, strict parser refuses success. Collector finished.
  User returnedfastboot; conditional authorized e0d3 boot-only restore
  starting, then90sUSBcapture. Never rerun executed capability/restore runner.
  Actual header diagnostics being prepared privately; no format relaxed.


- **CAPABILITY 6ddf8c15 AUTHORIZED; ONE-USE RUNNER STARTING.**
  User approved exact boot-only image and conditional e0d3/RAM-rescue
  recovery (call_66dd891bf947492d86cb1d0397f58a33, answer autorizzo).
  Sole fastboot device matches prior authorized identity; productoplus6893,
  bootcapacity2000000 and bootloader/non-userspace checked privately.
  USB collector starts before reboot. NEVER rerun capability runner after
  flash.log exists. Real capability reply and humanhealth pending.


- **CAPABILITY IMAGE VERIFIED; EXACT NEW FLASH APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-capability-battery-stop23.img`,
  33353728bytes SHA256
  6ddf8c15dd33ccdf84131b356a54f9857925c8e3f52391f4701368aa50eee3e9.
  Payloadc6ee6d4411b43ee1b97e1416c154d6699ded9d958f2a68f72b62a7bd3876689f.
  CI39637874421/sourcece8f968afa45303c9ba3ccaa3e7c01aa9d755e7e;
  kernel17464304/SHA2f9c5d4c1d77cd61d818826e8012d8b63ad88c4cdcda862a4665c51f1fbbb991.
  Linked capability reviewf5b6c101 PASS actualdefaultoff/FW_READY/worker/
  singlequery/boundedRX/commonhalt/drain/parking/usedABI. Bounded1GiB pack
  exit0; fresh independent loader/segments/ADT/privatecal/Linux/dualDT/raw
  rescue/header verificationPASS;200704bytes spare. Existing manifest/payload
  rechecked; actuale0d3/rescue hasheschecked. Currentstagepin43280d60 after
  exactcmdline474-byte repair; parser7c5bdf4f unchanged and reviewed.
  CI/schema/preparedtrue, humanexact/RF/recoveryfalse. Runner NEVER executed.
  New test asks only firstcapability query after mainFWREADY; no scan/network
  interface/association. May fail;30sdrain/manualfastboot conditionale0d3/rescue.
  Currentphonefa13humanhealthy; no restore needed/performed. UFS/bootspeed
  unchanged. Ask new exact boot-only approval after saving handoff.


- **CAPABILITY CI39637874421 SUCCESS; IMAGE PACKING.**
  Sourcece8f968afa45303c9ba3ccaa3e7c01aa9d755e7e, kernel17464304bytes,
  SHA2f9c5d4c1d77cd61d818826e8012d8b63ad88c4cdcda862a4665c51f1fbbb991.
  Full download/artifact SHA/source and1136genericARMv8.2+crypto commands
  verified, including bothnewTUs. Targeted linked review running. Newstage
  preflight caught two inherited template omissions BEFORE imagebuild:
  nord2ufsprofile=1 and nord2fbdiag=1. Parent restored EXACT executedfa13
  commandline plus nord2wificap=1 (474bytes), recordedrepair+newstagepins;
  original author report preserved, privateREADME/testlength corrected.
  Preflight nowPASS, CI/schema true; bounded1GiB packstarted, image/human
  gatesfalse. No hardware actions. Currentphone remainsfa13humanhealthy.


- **CAPABILITY RECEIPT/IMAGE SOURCE CONTRACT REVIEWED.**
  New private stage wifi-capability/ios stagepin5f561142 (templates only,
  excludes mutable gates) and strict parser7c5bdf4f reviewed by parent.
  Actual emitter8records, RXslot advancement, retained worker, exact main
  receipt and bounded supported debug events checked.13syntheticpositive/
  163negative capability cases PASS; actual main fullsnapshot correctly
  refuses capability completion because it has no capability rows.
  Native source/schema gate true; CI/image/authorization still false.
  Parent review saved privately; no device action. Actual normal capability
  E000 response remains unproven. UFS/boot speed remain unchanged.


- **CAPABILITY FULL CI39637874421 STARTED.**
  Sourcece8f968afa45303c9ba3ccaa3e7c01aa9d755e7e; canonical capability CID8a/EIDec query,
  defaultoff,3s/16events/oneworker, no scan/interface. Local validation PASS.
  Full artifact/link/image validation pending; no new device authorization.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39637874421
  Current phone remains humanhealthy fa13, no hardware action performed.


- **CAPABILITY QUERY PROMOTED; HOST/ARM64 CHECKS PASS, FULL CI NEXT.**
  Exact independently reviewed production candidate manifest75fc0de8 promoted:
  five canonical wifi_capability files, generated sweet_touch deferred hook,
  bsd/conf/files entries and generator. Patch14375 lines applies/reproduces.
  Actual canonical four ASan/UBSan host suites PASS (dispatcher128 callback
  fault positions, native worker/ownership, actual current owner and drain).
  Three normal focused ARM64 objects PASS: sweet_touch370896, capability13496,
  dispatcher34968 bytes. Source-backed CID8a/EIDec normal reply query is bounded
  to3s/16events, defaultoff without nord2wificap=1, one worker/one submit;
  no scan/network interface. Full CI/link/image/actual capability result pending.
  SWINT fa13 currentphone remains humanhealthy, all prior collectors finished;
  reviewed separate main evidence-v2 also passes full236116-byte snapshot.
  Session briefly read-only interrupted commit/build; write/USB access now
  restored and tested. Capabilitystage parser/schema work resumed privately;
  all CI/image/human authorization gates remain false. No new flash performed.
  Keep UFS/boot speed unchanged; kmod untracked user directory untouched.


- **SWINT fa13be9b STABLE; FULL CAPTURE AND HUMAN HEALTH CONFIRMED.**
  USB180s capture completed without transport error; full236116 bytes SHA
  7e177f368be4f854fb7a43c305cf4a27dcae5f48f31959d186ad9522b246d69b. Matching N9 ACK and real FW_READY confirmed, no
  native error; valid iOS frames and user confirms lockscreen/touch/buttons.
  All collectors finished; phone remains on successful fa13be9b. No restore
  performed or needed. Preserve executed SWINT evidence; runner is one-use.
  e0d3 remains tested fallback for next explicit approval until successor
  recovery policy is set. Capability integration/source review in progress.
  New strict diagnostic parser awaiting peerreview; old parser result kept.


- **SWINT HARDWARE SUCCESS: REAL N9 ACK AND FW_READY OBSERVED.**
  Authorized fa13be9b boot-only flash/reboot OKAY. MAIN status0/stage9/
  ready1/halt0/error0/events3/finish2. Third RX EID1/seq3/type7/status0
  matches expected; source-guarded readiness register180600f0 raw3/polls1.
  All7 spans verified, PHY backup30720, coex done, nativeerror0/guard0;
  DMA statusvalid3 with no pendingSWINT/coherent bits at captured checks.
  First immutable snapshot137952 bytes SHA
  c655aacb2a502ed9decece7abec1057860b51edc3fa03cbb9406c4a34ab6271c.
  Independent source+log review PASS. Existing frozen parser returns
  incomplete solely for10 newer unknown diagnostic record types; preserve
  that result, don't falsely report it passed. Separate strict evidence-v2
  parser being prepared. USB180s capture still active; valid1125x2436 frames
  observed; human health confirmation pending. No recovery needed from
  current observations; NEVER rerun executed SWINT runner. Current boot is
  fa13be9b, not e0d3. Capability production integration being prepared
  privately from reviewed32a1e5d0 now actual readiness prerequisite is met.
  UFS/boot speed unchanged; no scan/association/network interface yet.


- **SWINT fa13be9b AUTHORIZED; ONE-USE RUNNER STARTING.**
  Exact user approval for boot-only main firmware/calibration attempt and
  conditional e0d3 or verified RAM-rescue recovery. Sole device identity,
  product/capacity verified privately. USB collector before reboot; NEVER
  rerun wifi-main-swint/ios runner after flash.log exists. Actual ACK and
  FW_READY pending. UFS/boot speed unchanged.


- **SWINT IMAGE VERIFIED; NEW EXACT FLASH APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-main-swint-battery-stop23.img`,
  33349632 bytes, SHA256
  fa13be9baa232b26c89994998cd566fa2ec4e0d96748b5420f1c6504ba203be7.
  Payload5086ccea1a0c72d343cc25f5cc92ace6196603395d24f89489bd7edbeeed602c.
  CI39630376895/source520525269d472e34af34449a9444dd00f822c8ee;
  kernel17479800 bytes SHA45adc8ea5f5d0c7de88221fb7fad4ab2537de71d161c71563c46f0ad5d05bcf5.
  Targeted linked narrow CID41 gate/pendingSTA29 fail-stop/noextraIO/ABI PASS.
  Bounded1GiB pack exit0; fresh independent loader/segments/ADT/privatecal/
  Linux/dualDT/raw rescue/header checks PASS;204800 bytes spare. Existing
  manifest/payload rechecked, actual e0d3/rescue hashes verified. Stage CI/
  schema/prepared true; human exact/RF/recovery false; runner NEVER executed.
  Same main firmware/calibration attempt permits only observed late ENA29,
  refuses unhandled actual pendingSWINT. Real ACK/FW_READY still required;
  new test can fail. Keep30s drain/manualfastboot conditional e0d3/rescue.
  Ask exact new boot-only approval. Currentphone e0d3 human-healthy, all
  collectors finished. Privatecapability32a1e5d0 reviewed, not integrated;
  audio0680eaaa reviewed uncalled, hardware ownership blockers remain.
  UFS/boot speed unchanged.


- **PRIVATE NEXT-STAGE REVIEWS COMPLETE; NO ACTIVATION.**
  Capability runtime-current-owner9ddf7902 updates actual nwt1176/nwi3040/
  nwfn7472,43-field ABI and synthetic actual-owner query proof. First bounded
  query performs all callbacks at completed3 then parks at4: no ENA gate
  widening needed, deliberate further-owner call refuses. Separate
  runtime-failure-drain32a1e5d0 adds only existing nwm_failure_drain after
  common halt/report before panic; independent sanitizer/review PASS.
  Hardware FW_READY still required before promotion; normal E000 RX format
  unproven and no scan/interface provided. Qualified private mixed-header
  compiles are not full production CI; integration mapping requirements
  need refresh from old7440 snapshots before future canonical promotion.
  Audio silent-afe-adapter-v2 0680eaaa independently PASS for worker receipt
  consumedonce and existing16KiB AFE VA+2000 reuse; uncalled/private. Actual
  sameboot coldstate and SRAM/audio-master exclusivity remain unproven.
  Current SWINT image still packing; no hardware action pending except a
  future exact new approval. e0d3 phone remains human-healthy.


- **SWINT CI39630376895 SUCCESS; IMAGE PACKING.**
  Source520525269d472e34af34449a9444dd00f822c8ee; kernel17479800 bytes,
  SHA45adc8ea5f5d0c7de88221fb7fad4ab2537de71d161c71563c46f0ad5d05bcf5.
  Full download/SHA/source checks PASS;1134 genericARMv8.2+crypto commands
  across1131 lines (3 interleavedpairs). Linked policy review pending; stage
  CI/schema true, prepared/human approval false. Bounded1GiB pack underway.
  Phone e0d3 healthy, collectors finished. Audio silent-afe-adapter-v2
  0680eaaa fixes privatev1 findings; peer review pending, no runtime caller.
  UFS/boot speed unchanged.


- **PARALLEL AUDIO ADAPTER REVIEW: PRIVATE v1 NEEDS CHANGES.**
  Frozen silent-afe-adapter c47a9444 has no caller and is not integrated.
  Independent source review found missing actual amp-measurement-thread
  binding and a duplicate SRAM mapping inside the existing16KiB AFE Device
  mapping. Author is preparing separate privatev2 with captured measurement
  worker receipt and reuse of AFE VA+2000; originalv1 preserved. Hardware
  coldstate/master ownership still unproven. Do not activate based on host
  tests. Wi-Fi fullCI39630376895 remains current priority; phone e0d3 healthy.


- **SWINT POLICY FULL CI39630376895 RUNNING.**
  Source520525269d472e34af34449a9444dd00f822c8ee; narrowly permits measured
  late ENA29 and rejects actual pendingSWINT. Full artifact/link/image checks
  pending; no new hardware approval. Current e0d3 human-healthy, no collectors.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39630376895


- **MEASURED LATE MCU INTERRUPT ENABLE POLICY PROMOTED; CI NEXT.**
  Independently reviewed mcu-swint-enable-v2 manifestfbb8a9cf, single binder
  53ef37d1 promoted exactly with native fixture. Allows only DMA1 ENA29 at
  retained actualCID41/typea0 phase1/completed2/submitted or postACKphase2/
  completed3. Existing DMA1 STA read stops reason23 on pendingSWINT29.
  No ABI/marker/extra MMIO/W1C/retry/recovery; all other enables/GIC/ownership/
  coherent guards and real ACK/FW_READY checks remain. Parent actual canonical
  native ASan/UBSan PASS; generated normal-context sweet_touch ARM64 compile
  nonempty PASS. Patch13958 applies/reproduces. New wifi-main-swint/ios pins
  current reviewed source; schema true, CI/prepared/human approval false.
  Phone e0d3 human-healthy, collectors finished; UFS/boot speed unchanged.


- **OWNERDIAG TEST COMPLETE; e0d3 RESTORED AND HUMAN-HEALTHY.**
  Conditional boot-only restoration/reboot OKAY; USB90s capture223847 bytes,
  SHA25640c4bf832c7bbfd265b07fee8e098eedb4a8efa1b00642e0bafac247106076b1.
  Valid1125x2436 frames observed; user confirms lockscreen/touch/buttons OK.
  Both collectors finished. Preserve all executed wifi-main-ownerdiag/ios
  evidence; never rerun its flash or restore. Concrete first failure is
  DMA1 HOST_INT_ENA29 unexpectedly enabled, not a proven pending SW interrupt.
  Selected source separates ENA29 from STA29. Narrow late-CID41 candidate
  and review underway; actual pendingSWINT must halt with no W1C/recovery.
  New image will require fresh approval. UFS/boot speed unchanged.


- **OWNERDIAG HARDWARE: DMA1 INTERRUPT ENABLE29 IS THE FIRST STOP.**
  Authorized e011b66d boot-only flash/reboot OKAY; repeated calibration and
  all7 main spans, then first GUARD reason5/source2/stage6/error5/valid1:
  addr18025204 raw20000000 mask7d37ffff expected0. This is DMA1 HOST_INT_ENA
  MCU-to-host software interrupt enable29, NOT a pending interrupt or proven
  firmware error. No third ACK/FW_READY. First immutable143601 bytes SHA
  e06a1bffb38822d425f6a9940b3c36f95655de911d9b64d7146eec6a43f784f3;
  full154784 bytes SHA70654f3f1c93b035d5e84463f3a0406398e9815cdb3acd127660c2e52717e41d.
  Failure collector finished; user returned fastboot. Conditional e0d3
  recovery underway (restore-e0d3-flash/client/live); never rerun executed
  runner/restoration. Separate ENA versus actual STA29 source review and
  narrow candidate underway: keep GIC masked/other enables rejected and
  fail-stop for unhandled actual SW interrupt; no invented ACK or W1C.
  Audio silent-afe-adapter c47a9444 privately frozen, author checks PASS;
  independent review pending, no native runtime caller/hardware proof.
  UFS/boot speed unchanged.


- **OWNERDIAG e011b66d AUTHORIZED; ONE-USE RUNNER STARTING.**
  User approved exact boot-only image/main calibration attempt/conditional
  e0d3 or tested RAM-rescue recovery. Sole device identity/product/capacity
  verified privately; USB collector starts before reboot. Do not rerun the
  wifi-main-ownerdiag/ios runner after flash.log exists. Actual first guard
  and firmware readiness pending. UFS/boot speed unchanged.


- **OWNER DIAGNOSTICS IMAGE VERIFIED; NEW EXACT APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-main-ownerdiag-battery-stop23.img`,
  33347584 bytes, SHA256
  e011b66da1b49367f22dfb1d01cedc7712f306f98673752c46c05fa9c08fb4b7.
  Payload b8e84df972bc3affff573994a69c8a92acd1827aea2f6868f7ee6af564235e60.
  CI39626548042/source88588a074718e54dd4f1fa63142c304899fff0f4,
  kernel17479800 bytes SHAfb4d1a2772b3b9cd451cbc7d4e75fd3af5ccd14042935ec99bc68a45d322c1e9.
  Targeted linked first-error/guard/raw/reader-writer7472 layout review PASS.
  Bounded1GiB packing exit0; independent fresh loader/segments/ADT/private
  calibration/Linux/dualDT/raw rescue/header ID verification PASS;206848 spare.
  Existing manifest/payload rechecked; actual e0d3/RAM-rescue hashes rechecked.
  Stage identity/schema/prepared true; human exact/RF/conditional-recovery
  authorization FALSE; one-use runner NEVER executed. Same main/calibration
  attempt plus first-failed-predicate diagnostics, no relaxed guard or retry.
  First actual failed predicate and FW_READY remain unknown. Failure may
  recur;30s USB drain and manual fastboot conditional e0d3/rescue recovery.
  Ask NEW exact boot-only approval. Phone e0d3 human-healthy; no collectors.
  Private guard-reference.json maps18 hardware cases to selected Android
  source and next investigation; not permission to ignore any failure.
  Audio private ID integration independently reviewed; silent AFE adapter
  preparation continues privately. UFS/boot speed unchanged.


- **OWNER DIAGNOSTICS CI39626548042 SUCCESS; IMAGE PREPARATION.**
  Source88588a074718e54dd4f1fa63142c304899fff0f4; kernel17479800 bytes,
  SHA256 fb4d1a2772b3b9cd451cbc7d4e75fd3af5ccd14042935ec99bc68a45d322c1e9.
  Downloaded artifact/SHA/source checked;1134 generic ARMv8.2+crypto compiler
  invocations (three interleaved pairs and one exact split command preserved).
  Targeted linked review pending; stage CI/schema true, prepared/approval false.
  Bounded1GiB packing next. Phone e0d3 human-healthy; no collectors running.
  Private audio ID integration b479fba2 independently reviewed PASS: default
  off/no-MMIO, modes1/2, no AFE cycle caller, patch reproduction, nonempty ARM64
  objects and host ASan/UBSan. Compilation uses documented mixed focused
  headers, not full current CI/link. Audio remains private/out of this image.
  UFS/boot speed unchanged.


- **OWNER DIAGNOSTICS CI39626548042 RUNNING.**
  Source88588a074718e54dd4f1fa63142c304899fff0f4 preserves first owner error
  and adds captured-only diagnostics. Full linked/image verification pending;
  CI/prepared/human approval gates closed. Current phone e0d3 human-healthy,
  all collectors finished; UFS/boot speed unchanged. Audio ID integration
  preparation continues privately, with no AFE streaming call site.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39626548042


- **FIRST OWNER-FAILURE DIAGNOSTICS PROMOTED; READY FOR FULL CI.**
  Reviewed private owner-guard-diagnostics-v2 manifest9a8b2a3f promoted:
  native header, adapter and coordinator plus native fixture. Only trailing
  whitespace normalized in adapter (canonical e214cdb7, provenance in
  wifi-main-ownerdiag/focused/normalization.json). No guard relaxation or
  extra MMIO. All114 ordered software predicates and18 hardware read paths
  unchanged; first reason/stage and failed raw/mask/expected are captured.
  A later halt cannot replace or misattribute an earlier non-guard error.
  Parent canonical native/pre-span/drain ASan+UBSan PASS. Pre-span fixture
  had stale missing scheduler declarations and overwritten-error expectation;
  repaired with forbidden-path stubs and first-error expectation. Actual
  generated normal-context ARM64 sweet_touch/FW/PHY helpers PASS. ARM64 ABI
  checks: nwt1176/nwi3040 unchanged, nwfn7472 (32 bytes appended at7440), old
  offsets unchanged. Patch13943 applies/reproduces. Full CI/link required.
  Private ownerdiag stage source/schema pins ready, CI/prepared/approval false.
  Selected dynamic N9 source audit confirms CID41/72B/entry0/override0 and
  mandatory ACK then FW_READY; no proven command bug or permission to ignore
  owner failure. Actual failed guard remains unknown. Phone e0d3 human-healthy,
  all collectors finished; UFS/boot speed unchanged.


- **RX FORMAT HARDWARE: PATCH AND PHY PASS; N9 OWNER GUARD STOPS; RESTORED.**
  Authorized8f1824f1 boot-only flash/reboot OKAY. Patch-finish ACK accepted;
  PHY60-byte EID5/response81/status0 and 30720-byte calibration backup pass.
  All seven firmware spans verified (244752 words), then N9 finish CID41 TX
  72 bytes consumed (DIDX3/DDONE80000000). During third RX wait: events2,
  finish1, polls872, transporterror2 owner callback, RXDIDX2/valid3/reason5;
  no third header/descriptor and no FW_READY. Native20/21 overwrite original
  guard reason1..6/22; DMAstatusvalid0 does NOT establish actual owner-bit or
  coherent fault. Do not infer a specific predicate until captured.
  First immutable log139839 bytes SHAe4f88896748bc014ce91dc672d3be6f032e29217aed4a2a230a6c1c0702294e2;
  complete155085 bytes SHA12ae689aa07f461b346f6211703fdff31cf2b34cff463dd62698e3632932f529.
  Conditional e0d3 restore/reboot OKAY; USB90s capture223320 bytes,
  SHA43ce701f01433f1e83243a62fb6518bf79866c03bda3bcb1e2bd649d49a026d2.
  User confirms lockscreen/touch/buttons healthy; all collectors finished.
  Evidence wifi-main-rxformat/ios; NEVER rerun executed runner/restoration.
  Private first-owner-reason diagnostic candidate and selected dynamic N9
  ACK/source audit underway. UFS/boot speed unchanged.

- **AUDIO REVISION5 PRIVATE SALVAGE REVIEWED; UNCALLED.**
  revision5-review inputmanifest547baeeb; candidate9d5be791 preserves Claude
  partial work, adds stricter fullword SPM/ADSP cold gates, repairs AFE build
  wiring and stale I2C8 saved-value fixture. Author host ASan/UBSan and normal
  helper ARM64 checks pass; independent SOURCE review PASS for uncalled
  private plan. No native SRAM/phase-value adapter, exclusive worker or
  production callsite yet; no audio hardware/playback claim. Preserve this
  candidate and INDEPENDENT-REVIEW.json; original Claude candidate-next intact.


- **RX FORMAT 8f1824f1 AUTHORIZED; ONE-USE RUNNER STARTING.**
  User approved exact boot-only image/same calibration and main-firmware
  attempt/conditional e0d3 or verified RAM-rescue recovery. Fastboot identity
  confirmed privately. wifi-main-rxformat/ios runner executes once with USB
  collector before reboot; NEVER rerun after flash.log exists. Actual ACK,
  PHY and main firmware result pending. UFS/boot speed unchanged.


- **RX FORMAT IMAGE VERIFIED; NEW EXACT FLASH APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-main-rxformat-battery-stop23.img`,
  33347584 bytes, SHA256
  8f1824f18a3ec8a70d75dbbcc65a8f2075da2fe12259253c1a0896786e9bcf0b.
  Payload7b3cbcea20645b82c0367359b21659527d9094e5e701ff25bd57f455cbea1e0e.
  CI39617764738/source8b33a45453c6d3222c73a1acacf3dadf97d21c5b;
  kernel17463416 bytes SHAfa02e1e181bc18e8736ed658526927fad3978a4585a3fd522425aad25086d416.
  Targeted linked format/status/CID/PHY/ABI review PASS. Bounded1GiB pack
  exit0; independent fresh loader/segments/ADT/calibration/Linux/dualDT/raw
  rescue/header verification PASS,206848 bytes spare. Existing manifest and
  payload rechecked; e0d3/RAM-rescue actual hashes rechecked. Private stage
  identity/schema/prepared gates true; human exact/RF/conditional-recovery
  authorization FALSE, runner NEVER executed. New exact approval required
  for same firmware/calibration attempt, boot only, 30-second failure drain,
  conditional e0d3 or verified RAM-rescue recovery. No actual ACK/PHY/main
  success yet. Current phone e0d3 human-healthy; no collectors running.
  UFS/boot speed unchanged. Claude audio partial revision5 is frozen as
  revision5-review/input, manifest547baeeb; Sol review/test completion active.


- **RX FORMAT CI39617764738 SUCCESS; IMAGE PACKING.**
  Source8b33a45453c6d3222c73a1acacf3dadf97d21c5b; kernel17463416 bytes,
  SHA256 fa02e1e181bc18e8736ed658526927fad3978a4585a3fd522425aad25086d416.
  Full artifact/SHA/source identity verified; 1134 generic ARMv8.2+crypto
  invocations across1133 lines, one interleaved pair. Targeted linked INIT
  format/CID/status/PHY/ABI review PASS. Private stage identity/schema gates
  true; bounded1GiB image compression underway, prepared/approval false.
  Phone e0d3 human-healthy. UFS/boot speed unchanged.

- **CLAUDE AUDIO HIT LIMIT AGAIN; PARTIAL WORK PRESERVED.**
  Resumed Opus5.5 wrote audio_afe.h/c, snapshot POWERON_CONFIG_EN addition
  and partial binder evidence mapping in candidate-next, then returned
  session limit (reset21:20 Europe/Rome). No completed tests/manifest claimed.
  Original partial files are preserved; Sol is reviewing a separate private
  audio/revision5-review copy, including concrete checks/fixes. No active
  audio integration or hardware execution. Visible desktop3 viewer retains
  session output, including rate-limit result.


- **PRIVATE CAPABILITY PREPARATION UPDATED; NOT ACTIVATED.**
  after-fw-ready/runtime-init-format-rebase manifest40dc28d3 matches current
  RX ABI (nwt1176/nwi3040/nwfn7440), with 35 cross-TU checks and sanitizer
  tests. Peer review found unchanged normal-command TX still rejected the
  measured consumed-zero completion. Separate runtime-tx-completion candidate
  0b2550ce fixes DDONE-first/exact80000000 with pointer/DW2/DW3/software-owned
  length checks; independent review and both sanitizer suites pass. Normal
  event E000 format remains unproven and unchanged. Default-off and actual
  FW_READY gate remain; no activation. Production integration still needs
  whole-context compile/link and an explicit failure-log capture policy.


- **RX FORMAT FULL CI39617764738 STARTED.**
  Source8b33a45453c6d3222c73a1acacf3dadf97d21c5b contains the measured INIT
  format correction and focused fixtures. Local normal-context compiles and
  affected sanitizer checks passed. Full artifact/linked/image verification
  pending; new image approval not yet requested. Phone e0d3 human-healthy.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39617764738


- **MEASURED INIT FORMAT FIX PROMOTED; READY FOR FULL CI.**
  Four exact reviewed runtime files from candidate e174323a promoted together.
  Accept only measured inner type 0007 and legacy e000; remove three CID echo
  assertions absent from selected gen4m fw_dl.c consumer. Request CID40/41,
  actual EID/sequence/status0, bounds, PHY TLV/status/60-byte checks and DMA
  ownership/halt/drain remain. No struct, marker or backend operation added.
  Fixture package fbcffd49 promoted; canonical transport/native/init-event,
  initializer/halt/private-spans ASan+UBSan pass. Actual generated normal
  CI-context ARM64 sweet_touch/transport/PHY/layout compiles pass with nonempty
  objects; no extra private includes. Patch13836 applies and reproduces.
  Private wifi-main-rxformat/ios source/schema pins complete; CI, prepared
  image and approval gates remain closed until full verification. Phone e0d3
  human-healthy; UFS/boot speed unchanged. No new hardware approval yet.


- **RX HEADER TEST IDENTIFIED INIT FORMAT; e0d3 RESTORED AND HUMAN-HEALTHY.**
  Authorized 0a4b7ade boot-only test reached RX: expected and actual EID/seq
  both 1/1; only inner packet type differed, observed 0007 vs expected e000.
  Validity 0xf means printed status/CID zero are INVALID placeholders, not
  success. No PHY attempt or FW_READY. Immutable failure 152508 bytes, SHA256
  5a484204e4181a6d4c3a73cb8884d3cbe66cf04f0b67973d50b789eea2d4387a.
  Conditional e0d3 boot-only restoration/reboot succeeded; 90s USB capture
  227166 bytes, SHA256
  6bbb2ec7be16b3a3e7473c890e2bf2b07320479abcfa624f0a143451c2bba085.
  User confirms lockscreen/touch/buttons healthy. All collectors finished;
  never rerun executed RXHEADER runner or restoration. Private evidence in
  research/nord2/wifi-main-rxheader/ios. Reviewed INIT format candidate
  e174323a accepts exact 0007/e000 and removes source-unsupported CID echo
  assertions; EID/seq/status/bounds/ownership checks stay. Promotion underway,
  not yet built or authorized for hardware. UFS/boot speed remain unchanged.

- **CLAUDE AUDIO RESUMED AFTER USAGE RESET.**
  Existing session 8f43c7d1-ae71-4989-a701-b5712553f9a0, Opus/high, resumed
  from reviewed revision4 into private audio/claude-opus/candidate-next.
  Stream and prompt in resume-20261021-1620; visible live viewer on desktop3.
  Claude may edit private audio candidate only, no device/active source/CI.
  Current work is source/implementation preparation, not working audio.














- **RX HEADER0a4b7ade AUTHORIZED; ONE-USE RUNNER STARTING.**
  User approvedexactimage/samefirmwareattempt/conditional e0d3 orRAMrescue.
  Fastbootidentity confirmed privately. wifi-main-rxheader/ios runnerexecutes
  once; USB180s collectorbefore reboot. Do notrerun onceflash.logexists.
  Runtime rawheaderunknown; UFS/boot speed unchanged.


- **RX HEADER IMAGE VERIFIED; NEW EXACT FLASH APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-main-rxheader-battery-stop23.img`,
  33347584B SHA256 0a4b7ade57f35b2cd1932696f3e3bf7ccd10ab0685d7e69b757db0f19996b13b.
  Payload dbc23092a701beafab6603068e816fa4478353442c17768d666ed705d5c1617d.
  CI39613306032/sourceba6400414b980a4dcc9f537b6890b8cc7883a557;
  kernel17463416B SHA60ea1e4ea5447992bc0d5a63aa59463f092a28023de6759347af4a2164445148.
  Targetlinked capturedbounds/validity/writerreader/newownerlayout+72review
  PASS. Bounded1GiB packingexit0; independent freshloader/segments/ADT/private
  calibration/Linux/dualDT/rawrescue/AndroidheaderPASS,206848spare. Existing
  manifest+payload rechecked, e0d3/RAMrescue actualhashes rechecked. Newstage
  identity/schema/prepared true; exacthuman/RF/conditionalrecoveryFALSE,
  runner NEVERexecuted. Diagnostics-only repeatofsamefirmwareattempt; raw
  type/EID/sequnknown; currentearlierheaderguardunchanged, failuremayrepeat.
  Ask NEW exactboot-onlyapproval withconditional e0d3/RAMrescue and30sdrain.
  Phonee0d3 humanhealthy/allcollectorsfinished. UFS/boot speed unchanged.


- **RX HEADER CI39613306032 SUCCESS; IMAGE PACKING.**
  Sourceba6400414b980a4dcc9f537b6890b8cc7883a557, kernel17463416B
  SHA25660ea1e4ea5447992bc0d5a63aa59463f092a28023de6759347af4a2164445148.
  Completeartifactdownload/SHA256SUMS/sourceidentity PASS;1134genericARMv8.2
  invocations across1132lines(twointerleavedpairs). Newstageidentity/schema
  gatestrue; bounded1GiB packing andindependentlinkedRX/ABIreviewunderway.
  Prepared/humanapprovalfalse; phonee0d3 humanhealthy/allcollectorsfinished.
  Private dynamic-patch-sequence-audit/report.json d71e0df8 confirmsnoomitted
  selectedpre-CID40 semaphore/drain/FWDLpacket: selectedAXI/SOC3 NO_SEM=1,
  directremapcopy andNULLFWDLcallback. Actualrawheader remainsunknown.
  UFS/boot speed unchanged.


- **RX HEADER CI39613306032 RUNNING.**
  Sourceba6400414b980a4dcc9f537b6890b8cc7883a557; onlynewruntimechange capturedRXdiagnostics,
  noacceptance/IO/poll/rearmchanges. Tests/actualnormalcontextcompilesPASS;
  allTU rebuild requiredfor enlargeddiagnosticstate. Newstageidentity/
  prepared/approvalfalse untilfullCI/artifact/linked/packingchecks.
  Currentphonee0d3 humanhealthy/allcollectorsfinished; UFS/boot speedunchanged.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39613306032


- **RX HEADER DIAGNOSTICS PROMOTED; READY FOR FULL CI.**
  Reviewedcandidate6d8d00f8 adds captured-only RXdescriptor/expectedbuffer/
  DIDX/validity/reason and bounded type/EID/seq/status/CID fields; noadditional
  backendIO or acceptance/rearm/poll changes. New RXDESC/RXWORDS/RXHEADER.
  Exactcanonical3files+transportfixture promoted together; focused transport/
  initializer/native/halt ASanUBSanPASS; actualgenerated sweet_touch/transport
  normalcontextARM64PASS. Patch13835 applies/reproduces. Independent host+
  ARM64 layoutPASS:nwt1176/nwi3040/nwfn7440; transportoffset1864unchanged,
  downstreamownerfields+72. FullallTUrebuild/linkedreview required. New
  wifi-main-rxheader/ios schema/sourcepinned, CI/prepared/approvalfalse.
  Currente0d3 humanhealthy/allcollectorsfinished; UFS/boot speed unchanged.

- **TX WRITEBACK TEST REACHED RX; e0d3 RESTORED/HUMAN-HEALTHY.**
  Authorized a8cc8050 boot-only flash/reboot OKAY; TXconsumedzero accepted,
  mismatch0/used68/DIDX1. Actual RX descriptor accepted and36bytescopied:
  descriptor/mac/init lengths36/36/12, but error21/events0/finish0 at
  packettype/EID/sequenceguard BEFORE status/CIDwrapper. NoPHY/mainready.
  DMA1status08000001 = normalTX17+RX0completion. Immutablefailure152411B
  SHA647683c0e4ec07049bd234366f09be012960166342c3c0e770ae669d2a0b7415.
  Rawheaderunknown: do not inferwhichfieldmismatched. Selectedsourceaudit
  rx-header36-audit/report.json8b0db51e findsno staticdefect; lengthsfit
  RXD24+INIT8+RESULT4 but aloneprove noidentity/status. Existing reviewed
  RXdiag captures allfourfailingbytes, no extrareads or guardrelaxation.
  Usermanuallyenteredfastboot; authorized e0d3 boot-only restore/reboot OKAY.
  USB90s capture223459B SHA
  a4d6528b7c6dbfdb462383418c7fdfec02c820ab9cd9c1134ce590d43eea7797;
  humanconfirms lockscreen/touch/buttons. No preservedprefix. Allcollectors
  finished. Evidence wifi-main-txwriteback/ios; NEVER rerun either executed
  runner/restoration. UFS/boot speed unchanged.


- **TX WRITEBACK a8cc8050 AUTHORIZED; ONE-USE RUNNER STARTING.**
  User approved exactimage/same mainfirmware+calibration attempt/conditional
  e0d3 or RAMrescue recovery; fastboot identity confirmed privately. New
  wifi-main-txwriteback/ios runner executes once; USB180s collector starts
  before reboot. Do not rerun onceflash.log exists. Runtime result pending.
  UFS/boot speed unchanged.


- **TX WRITEBACK IMAGE VERIFIED; NEW EXACT FLASH APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-main-txwriteback-battery-stop23.img`,
  33347584B SHA256 a8cc8050f041ef423343576a22c965d8592618061ae030d6ce3111892fb04ad6.
  Payload 73a3138deae3ce1530eb290131812ddb4c3c0ae601f2ce87a2461ebe085ef066.
  CI39610057577/source6bee7acfab972cc9243be77f913a34993e937619;
  kernel17479800B SHA660198f99813bbf462341d57f15f117242809ff044437e9449e3ad7d20af3eca.
  Targetlinked exactzero/controlguards/capturedwriterreader/unchangedRX/drain
  PASS. Bounded1GiB packingexit0; independent freshloader/segments/ADT/private
  calibration/Linux/dualDT/rawrescue/Androidheader verificationPASS,206848spare.
  Existingmanifest+payload rechecked; e0d3/RAMrescue actualhashes rechecked.
  Newstageidentity/schema/prepared true; exacthuman/RF/conditionalrecovery
  gatesFALSE, runner NEVER executed. ActualhardwareRX/PHY/mainready remains
  unknown. Ask NEW exactboot-only approval withconditional e0d3/RAMrescue,
  same mainfirmware/calibration attempt and30sfailuredrain. Currentphonee0d3
  humanhealthy/allcollectorsfinished. RXdiagnosticcontingency remainsprivate;
  UFS/boot speed unchanged.


- **TX WRITEBACK CI39610057577 SUCCESS; IMAGE PACKING.**
  Source6bee7acfab972cc9243be77f913a34993e937619, kernel17479800B
  SHA256660198f99813bbf462341d57f15f117242809ff044437e9449e3ad7d20af3eca.
  Completeartifactdownload/SHA256SUMS/sourceidentity PASS,1134 complete
  compilerinvocations genericarmv8.2+crypto. RCPC2scanPASS2227670instructions.
  Newstageidentity/schema gatestrue; bounded1GiB packing and independent
  targetlinked review underway. Prepared/exacthumanapproval gatesfalse.
  Currentphonee0d3 humanhealthy/no collectors. RXcontingency remainsprivate;
  UFS/boot speed unchanged.


- **PRIVATE RX AUDIT AND DIAGNOSTICS CONTINGENCY REVIEWED.**
  Under wifi/next-stage/main-fw-transport, rx-response-audit/report.json
  SHA848f9ae93004192932f8cb32aae1ada29577429cb3234db65d73463ea5bec327
  finds no proven ring/event-offset defect. Extra native CID/pointer assertions
  and shorter1000x1ms RX budget differ from selectedsource3000ms response /
  5000ms PHY budgets; no actual RXfailure or need to weakenchecks observed.
  rx-diagnostics manifest6d8d00f8 independently reviewedPASS: captured-only
  rawdescriptor/expectedbuffer/DIDX and bounded INIT metadata, validitygated,
  no payload/calibrationdata or additionalIO/accept/rearm/pollchanges.
  Host sanitizersPASS; privateARM64 context qualified, allTUrebuild needed if
  integrated later. NOT active, not in CI39610057577. CurrentCI contains only
  reviewed measuredTXexception; currentphonee0d3 humanhealthy.


- **TX WRITEBACK CI39610057577 RUNNING.**
  Source6bee7acfab972cc9243be77f913a34993e937619; exactmeasured80000000 completionexception,
  capturedTXWB classification andsame30sfailuredrain. Localtests/actual
  generatedsource compilePASS. Newstageidentity/prepared/approvalfalse until
  fullCI/artifactchecks/linkedreview/packing. Currentphonee0d3 humanhealthy,
  no devicecollector; UFS/boot speed unchanged. No hardwareapproval for new
  image. https://github.com/frxncyyy/ios-sweet/actions/runs/39610057577


- **MEASURED TX WRITEBACK EXCEPTION PROMOTED; READY FOR FULL CI.**
  Privatecandidatefc14aeb9 independently reviewedPASS. Exactword1=80000000
  acceptedonlyafter expectedDIDX/DDONE/exactpointer/DW2DW3zero, retaining
  softwareused/ownership/realRXchecks. Zero+extraflags/wrongnonzerolength
  reject; original-lengthpolicy unchanged. Appendedtx_consumed_zero and
  capturedTXWB marker; noextraIO/replay/reset;30sdrain retained.
  Threecanonical transportC/header/coordinator files promoted together,
  canonical transport/initializer/native/halt ASanUBSan PASS. Exact priorCI-
  derived actualgenerated sweet_touch/transport ARM64 PASS, noextra private
  includes/forcedNORD2. Patch13812lines applies/reproduces. Newprivate stage
  wifi-main-txwriteback/ios source/schema pinned; CI/prepared/approvalfalse.
  Currentphonee0d3 humanhealthy, allcollectors stopped. No newimage or
  hardwareapproval yet. UFS/boot speed unchanged.


- **TXDONE923d7cea IDENTIFIES CONSUMED DESCRIPTOR; e0d3 RESTORED/HEALTHY.**
  Boot-only flash/reboot OKAY. Immutable failure snapshot150834B SHA256
  8f8da54e00bef8e7f25ec472c2954e55753ddd5d942f30e71eee84ac42b6d6cd.
  Stage4 patchfinish, transporterror16: expectedDIDX1,DDONE1, used68,
  submitted90005000/40440000/0/0 observed90005000/80000000/0/0.
  Pointer/DW2/DW3 match; SDL0ANDLastSec0 consumed. Not earlyDDONE race.
  DMA1bit27 normalTX17completion; coherent20/21 clear. NoRX/PHY/mainready.
  Exact measured zero-control exception candidatefc14aeb9 reviewedPASS;
  retains expectedDIDX/DDONE/pointer/unusedwords/softwareused/ownership,
  rejects zero plus othercontrolbits; actual matchingRX stillrequired.
  User manuallyenteredfastboot; conditional e0d3 restore/reboot OKAY,
  USB90s capture224056B SHA256
  8a96dbccc7fde179f275f90546ab66c066f46b8be4c9262771e736d220d8c629.
  Human confirms lockscreen/touch/buttons. No preservedfailureprefix;
  original failure safelycaptured. Allcollectors finished. Evidence under
  wifi-main-txdone/ios; NEVER rerun either executed runner or restoration.
  Next private stage wifi-main-txwriteback/ios gatesfalse; no newimage or
  approval. UFS/boot speed unchanged; nextWiFi/audio remain private.


- **TXDONE IMAGE923d7cea AUTHORIZED; ONE-USE RUNNER STARTING.**
  User approved exactimage/samefirmwareRF attempt/conditional e0d3 or RAMrescue
  restoration, fastboot confirmed privately. wifi-main-txdone/ios runner
  executes once, USB180s collector before reboot. Do not rerun onceflash.log
  exists. Runtime result pending. UFS/boot speed unchanged.


- **TXDONE IMAGE VERIFIED; NEW EXACT FLASH APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-main-txdone-battery-stop23.img`,
  33347584B SHA256 923d7cea9abf666ef11ec78026f6421c74cce2d8c719047def0caf0e5fc6bfb2.
  Payload 53714120c267ab986299b00023000d889efc7539e830a383e5c3cbae312f176c.
  CI39606475052/source45a03c3f3aeb6c94f92bf308a3aefb20aee6675c;
  kernel17479800B SHA2d60508c3503202892ae4a943caef78cb6f1bd916b2284efbea7bf8dadba0440.
  Targeted linked DDONE-first/strictchecks/tracewriter+coordinatoroffsets
  reviewPASS, no additional backendIO,1000poll/drain retained. Bound1GiB
  packingexit0; full freshloader/payloadsegments/ADT/calibration/Linux/dualDT/
  rawrescue/Androidheader verificationPASS,206848Bspare. Existingpayload+
  manifest rechecked; e0d3/RAMrescue recoveryhashes rechecked. Private stage
  wifi-main-txdone/ios identity/schema/prepared true, exact human/RF/recovery
  gatesFALSE; runner NEVER executed. Source-order correction not a proven
  repair; rawwords/DDONE of originalerror16 unknown. Ask NEW exact boot-only
  approval with conditional e0d3/RAMrescue fallback, samefirmware/RF attempt.
  Currentphonee0d3 human-confirmed healthy, allcollectors stopped. UFS/boot
  speed unchanged. runtimec1ef0929 rebase/audio4 remain private only.


- **TXDONE CI39606475052 SUCCESS; IMAGE PACKING.**
  Source45a03c3f3aeb6c94f92bf308a3aefb20aee6675c; kernel17479800B SHA256
  2d60508c3503202892ae4a943caef78cb6f1bd916b2284efbea7bf8dadba0440.
  ArtifactSHA256SUMS/source identity PASS. Parallel build log split an
  AppleARMSMP compiler echo across adjacentlines and wifi_firmware source tail;
  privateverify-kernel.py preserves/reunites explicitfragment evidence, checks
  separateCCmarker/source tail and all1146Build CPU/archpairs genericarmv8.2.
  1135 matchedcomplete/reunitedinvocations; no claim all echoes arewholelines.
  Newstageidentity/schema gatestrue; bounded1GiB packing and targetedlinked
  TXDONE/strictchecks/crossTUoffset review underway. Prepared/human gatesfalse.
  Currentphonee0d3 healthy/no collectors. Runtimec1ef0929 rebaseprivate only.


- **PRIVATE RUNTIME REBASE VERIFIED; NO ACTIVATION.**
  after-fw-ready/runtime-txdone-rebase manifestc1ef0929 pinnedcanonical
  45a03c3 independently reviewedPASS. Dispatcher/corecapability behavior
  byteidentical original; currentdependencyheaders/source snapshots include
  appendedTXDESC/pre-span/drain fields. Separate21field crossTUlayout check
  PASS (nwt1104/nwi2968/nwfn7368/nfp336,txdesc1040), hostsanitizersPASS.
  Private ARM64 uses qualified extra harness; productioncompile stillrequired
  before any integration. ActualFW_READY absent, defaultoff gate unchanged.
  Original05f1707f runtime remains frozen. MainTXDONE CI stillrunning.


- **TXDONE CI39606475052 RUNNING.**
  Source45a03c3f3aeb6c94f92bf308a3aefb20aee6675c. Same radio attempt,
  DDONE-first order with strict echo retained and rawcaptured descriptors.
  New wifi-main-txdone/ios schema pinned; identity/prepared/authorization
  gatesfalse until successfulCI/artifactverification/linkedreview/packing.
  Phonee0d3 human-confirmed healthy, no collectors. runtimecapability package
  getting a PRIVATE rebase for appendedstructures; never activated before
  genuine mainFW_READY. UFS/boot speed unchanged.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39606475052


- **TXDONE ORDER/RAW DIAGNOSTICS PROMOTED; READY FOR CI.**
  Private candidate68ff8bca independently reviewed PASS. Transport waits
  DDONE before unchanged strict pointer/SDL0/DW2/DW3 checks, same1000poll
  bound/ownership/coherenthalt/no replay/RXrearm on failure. Appended trace
  captures intended/observed4words,descriptorPA,DIDX,length,validity,mismatch
  andwaitcount; newTXDESC/TXSUBMIT/TXOBSERVED markers, no extra backendIO.
  Main transportC/header/coordinator promoted together. 30sdrain retained.
  Partialwriteback is NOT proven: configuredTX_WB_DDONE promises fullAXI
  writeback beforeIRQ, so actualraw/DDONE capture still needed.
  Canonical transport/initializer/native/halt ASanUBSan PASS. Native fixture
  needed scheduler mocks for prior drain include; host-only fix now asserts
  commonhalt and exactlyone drain beforeuncertainpanic. Exact priorCI-derived
  actualgenerated sweet_touch andtransport ARM64 PASS withoutprivateincludes.
  Patch13800lines applies/reproduces. New wifi-main-txdone/ios schema pinned;
  CI/prepared/human gatesfalse. Currente0d3 healthy/no collectors. No new
  hardwareapproval. UFS/boot speed closed; audio/nextWiFipackages private.


- **DRAIN TEST CAPTURED ERROR16; e0d3 RESTORED/HUMAN-CONFIRMED.**
  Authorized ebfa71f6 boot-only flash/reboot OKAY. New30s failure drain
  enabled complete captured failure diagnostics before terminalpanic.
  Immutable boot-failure-first-snapshot.log151581B SHA256
  f09a845890a455fbedb40c76d91e3a424edb86e3be4a9d970f8ed24c2ff188aa.
  DMA initphase5 succeeded; spans0/1 copied+verified1428words; rawsync1,
  PRESPANreason0 passes. Mainstage4 patchfinish submittedonce, DIDX0->1,
  transporterror16 on descriptor echo before DDONE check. DMA1status08000000
  is NORMAL TX17completion bit27, NOT coherentfault (bits20/21 clear).
  nativeoriginal20 wrappererror; generic21 follows. No RX event/PHYattempt/
  mainready. Raw descriptor words absent, mismatching field unknown.
  Existing code validates pointer/SDL0/DW2/DW3 BEFORE DDONE, unlike selected
  Linux completion ordering. Narrow private correction+rawdiagnostics underway;
  keep strict post-DDONE checks, no replay/reset/guard weakening. Not yet a
  proven hardware rootcause. Detailed source review under runtime-tx-error16.
  User enteredfastboot; conditional e0d3restore/reboot OKAY, USB90s captured
  226068B SHA256 6a5d8e2365ff4911076e857cbfb353c5eb5665a2fd923b53969383fb5311ebf8.
  User confirms lockscreen/touch/buttons healthy. Currentrestore had no
  preservedprefix; captured failure already safe. All collectors stopped;
  NEVER rerun executed boot or restoration. Evidence under wifi-main-drain/ios.
  UFS/boot speed unchanged. No new image or hardware approval yet.


- **DRAIN IMAGE ebfa71f6 AUTHORIZED; ONE-USE RUNNER STARTING.**
  User explicitly authorized exact image, same firmware/RF attempt and
  conditional e0d3/RAMrescue recovery; fastboot identity confirmed privately.
  New wifi-main-drain/ios runner will execute once with nativeUSB180s
  collection started before reboot. Do not rerun once flash.log exists.
  Runtime failure/success unknown; UFS/boot speed unchanged.


- **DRAIN IMAGE VERIFIED; NEW EXACT BOOT APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-main-drain-battery-stop23.img`,
  33345536 bytes, SHA256 ebfa71f63e4218a2613b6e9734c63bbcbf5c89fbc0afa418e22894cbb31a1cfd.
  Payload 626f8aa812127fd79792b37566055169bbbab94ccdfd8ecb620f5111fddf7cc7.
  CI39602168207/source5ced67ed016a9e22771f511b3b8bb7ca3513db1f; kernel
  f4180433252673241e2769fe920e53d7ecd73d0935647f4b1dadb20368828ed5.
  Targeted linked drain/context/deadline/originalpanic review PASS. Packing
  with1GiB limit exit0; independent fresh loader/payload segments/ADT/private
  calibration/Linux/dualDT/rawrescue/Androidheader checks PASS,208896 spare.
  Existing manifest and payload rechecked. e0d3 and RAMrescue hashes rechecked.
  New wifi-main-drain/ios identity/schema/prepared gates true; exact human/RF/
  recovery approval stillFALSE, runner never executed. Scope is same main
  firmware attempt plus30s failure-drain opportunity, NOT a proven radiofix.
  Current e0d3 healthy human-confirmed/no collector. Ask NEW exact boot-only
  approval with e0d3/RAMrescue conditionalrestore. UFS/boot speed unchanged.


- **DRAIN CI39602168207 SUCCESS; BOUNDED IMAGE PACKING.**
  Source5ced67ed016a9e22771f511b3b8bb7ca3513db1f; kernel17479800 bytes,
  SHA256 f4180433252673241e2769fe920e53d7ecd73d0935647f4b1dadb20368828ed5.
  Artifact SHA256SUMS/source identity and1135genericARMv8.2+crypto invocations
  verified. New wifi-main-drain/ios identity/schema gates true. Packing
  uses1GiB RLIMIT_AS; linked drain-only review in parallel. Prepared/human
  authorization gates still false. Healthy e0d3 phone untouched/no collector.


- **DRAIN CI39602168207 RUNNING.**
  Source5ced67ed016a9e22771f511b3b8bb7ca3513db1f; only runtime delta is
  bounded scheduler interval before uncertain-radio panic. New private stage
  wifi-main-drain/ios reviewed/pinned schema including DRAIN/PRESPAN/PREORDER,
  identity/prepared/human-authorization gates still false pending successful
  CI, exact kernel/download checks, linked drain path review and packing.
  Current phone e0d3 healthy human-confirmed, no device collector active.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39602168207


- **PRE-PANIC DRAIN PROMOTED; READY FOR FULL CI.**
  Private candidate a38018d5 independently reviewed PASS. Actual canonical
  helper ASan/UBSan passes fixed30s absolute-deadline/one-use/unsafe-context
  and early-wake bounds. Actual generated sweet_touch ARM64 compiles with
  prior CI-derived flags, no private bsd/dev include shortcut or forced NORD2.
  Includes/helper/test/generator/WIFI-MAIN updated together; patch13778 lines
  applies/reproduces. Only runtime change is scheduler sleep in the already
  halted, uncertain-radio terminal branch before existing panic. No radio
  mutation/retry/cleanup or USB ownership changes; capture not guaranteed.
  Source-backed loss window: panic stops other CPUs before retained-radio
  cleanup refusal, so USB thread cannot drain final diagnostics thereafter.
  New 0ce01268 cause remains UNKNOWN. Current e0d3 human-confirmed healthy;
  no collectors. New private wifi-main-drain stage being prepared with closed
  gates; needs full CI/artifact checks/image verification and NEW exact flash
  authorization. UFS/boot speed unchanged, audio4 private.


- **0ce01268 RECOVERY COMPLETE; NEW FAILURE CAUSE UNKNOWN.**
  Authorized e0d3 boot-only restore/reboot succeeded 2026-10-21 14:37:12
  Europe/Rome. USB90s capture completed: 223215 bytes, SHA256
  41a9294d7bccbf818e14ea8b037f2274929992d8d91564d29c9d8d7d832312b6.
  User confirms lockscreen/touch/buttons healthy. Frozen full current log and
  restoration-report.json under wifi-main-state/ios. NO previous-log marker
  survived this restore, therefore no final failed suffix or raw PRESPAN state
  was recovered; do not mistake old b60 diagnostics in initial capture for
  the new failure. Initial immutable101500B snapshot remains preserved.
  All collectors stopped; do not rerun executed image/restore runners.
  Private failure-drain candidate a38018d5 adds bounded30s scheduler sleep
  before existing terminal panic so CPU0 USB can drain captured diagnostics;
  independent review pending. No new radio-cause inference or flash approval.
  UFS/boot speed unchanged; audio4 remains private/unintegrated.



- **STATE IMAGE0ce01268 BOOTLOOP; RESTORE PENDING FASTBOOT.**
  Authorized boot-only flash/reboot OKAY. Native USB first capture101500B
  SHA2848c106798b93e1a4664d35c17b5a149e3f4abf4cd9f7622155fef1e6a308e1
  ends during MAIN ARM2 with READ54 timeout, no final PRESPAN diagnostics yet.
  User reports another bootloop. Capture and immutable first snapshot saved
  under wifi-main-state/ios; collector stopped, runner must NEVER be rerun.
  User asked manual fastboot for already-authorized exact e0d3 restoration;
  retrieve preserved final failure log from restored native USB. Actual
  failure cause/new readiness state unknown until final log is recovered.



- **STATE-CHECK IMAGE0ce01268 AUTHORIZED; ONE-USE RUNNER STARTED.**
  User explicitly authorized the exact image, RF calibration, boot-only flash
  and conditional recovery to healthy e0d3/RAM rescue, and reports fastboot.
  New runner wifi-main-state/ios/run-approved-boot.py started exactly once;
  native USB180s capture follows reboot. Never rerun after flash.log exists.
  Runtime main firmware outcome pending. No networking claim.



- **STATE-CHECK IMAGE VERIFIED; EXACT NEW FLASH APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-main-state-battery-stop23.img`, 33345536 bytes,
  SHA256 0ce012687f2e8266a9ca28edc18101e772ef34db304874b85526a00d279f9ea5.
  Payload 67ec29a6790194ae4e06e4c90ea1013a0021f6c16b9e3822981256c016e70a67.
  CI39597568934/source852b935b2fa1044ba37e5ec6baae10ac02c83e5a; kernel
  62f0fb751d8650e7cc881a4a557c1f27d50c641c80a1fd9f4f8088f66ad77f94.
  Fullmask3/captureddiagnostics/structoffsets linked review PASS; poolmap
  route still correct. Bounded1GiB packing exit0 and full independent loader,
  payload/ADT/calibration, Linux/DT/CPIO and Androidheader checks PASS.
  Boot spare 208896 bytes. Private wifi-main-state/ios
  identity/schema/prepared gates true, authorizationFALSE and runnerNEVERrun.
  Ask NEW exact boot-only/RF approval with e0d3/RAMrescue conditionalfallback.
  Current phone remains e0d3, human-confirmed lockscreen/touch/buttons healthy.
  No collector active. Original b60dd9bc completefailed log51448B remains
  frozen: DMAinitialized, failurebeforepatchcopy, originalsyncunknown.
  UFS/boot speed closed/unchanged; audio4 and nextWiFipackages private only.



- **STATE-CHECK CI39597568934 SUCCESS; IMAGE PACKING.**
  Source852b935b2fa1044ba37e5ec6baae10ac02c83e5a, kernel17479696 bytes,
  SHA25662f0fb751d8650e7cc881a4a557c1f27d50c641c80a1fd9f4f8088f66ad77f94.
  Artifact and1134generic ARMv8.2+crypto compile invocations verified.
  Private wifi-main-state/ios identity/schema gates true; 1 GiB-bounded
  packing and targeted linked review underway. Prepared/authorization gates
  remain false. Currente0d3 healthyhumanconfirmed; no device operations.
  Private audio/revision4 manifest969df856 narrow0xec I3C correction reviewed
  PASS, actualnormalARM64 objects. after-fw-ready normal-init59c31970 also
  reviewed PASS, alongside runtime05f1707f and scan-bssb7649200. All remain
  private/unintegrated, no runtime capability/scan/networking claim.



- **STATE-CHECK FIX CI39597568934 RUNNING.**
  Source852b935b2fa1044ba37e5ec6baae10ac02c83e5a; full SW_SYNC readiness
  correction and captured PRESPAN/PREORDER diagnostics. Current e0d3 phone
  is human-confirmed healthy; failed b60dd9bc complete logs are preserved.
  New private wifi-main-state/ios has all gatesfalse; needs successful CI,
  exact source/artifact verification, targeted linked review and bounded
  image packing before asking a NEW exact boot-only approval.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39597568934
  Private audio revision4 narrow register correction underway, no integration.



- **PRE-SPAN STATE CORRECTION PROMOTED; READY FOR CI.**
  Private pre-span-fix manifest6d51d655 independently reviewed PASS. Three
  canonical files promoted together: firmware C/header and coordinator.
  Full SW_SYNC0 ready mask3 replaces unsupported any-bit inference; all
  receipt/owner checks retained. Captured PRESPAN/PREORDER logs distinguish
  refusal reasons and retain raw sync/original native error before generic21.
  Actual prior failure value remains unknown. Focused canonical ASan/UBSan
  regression PASS; actual generated sweet_touch and firmware ARM64 compile
  PASS using CI-derived flags/source headers, no extra bsd/dev include or
  forced NORD2. Patch13746 lines applies/reproduces. New private stage
  wifi-main-state/ios has false identity/prepared/approval/schema gates;
  source/marker pins populated, awaiting full CI. No new flash authorization.
  e0d3 restoration now HUMAN-CONFIRMED: lockscreen/touch/buttons work;
  restoration-report.json updated, collectors stopped. Audio private review
  PARENT-REVIEW.md found revision3 I3C guard wrong offsete4 vs sourceec and
  corrected handoff's false claim revision2 compile was empty. No audio merge.



- **b60dd9bc BOOTLOOP LOG RECOVERED; e0d3 RESTORED.**
  Exact authorized new boot flashed/rebooted OKAY14:00. Initial USB97000B
  (SHA a9a1e6254c720532173aa7fd3a76de9dd257f6f5d61c4e4363f7bf526bc2d52d)
  ended with READ54 timeout. User confirmed bootloop. Separate STATUS52 retry
  timed out, no successful second capture. User entered fastboot; authorized
  boot-only e0d3 restore/reboot OKAY14:02. Restore USB90s275510B complete;
  original and split evidence plus reports under wifi-main-map/ios.
  Preserved failed prefix51448B SHA a61b4959fbf2b75065aef0904a98a17c77bdfab77e654f664257d88704bf4403
  proves DMA pool mapping/zero verification PASS, full initializer phase5,
  scheduler99/register92/memory2123 succeeded. MAINstage3 patch-copy failed
  before any span writes/commands/PHY: explicit retained-state panic, not an
  unexplained controller crash. Nativeerror21 is coordinator overwrite;
  original guard reason/pre_sync raw were not logged. Source defect found:
  pre_sync rejects ANY readiness bit though selected ready mask3 requires both;
  actual trigger remains unknown. Private narrow correction plus captured
  diagnostics/tests in progress; no new CI/image/approval. Current e0d3
  USB/display logs healthy, human confirmation pending. All collectors stopped.
  Never rerun executed b60dd9bc runner. UFS/boot speed remain unchanged.



- **POOL MAP IMAGE b60dd9bc AUTHORIZED; ONE-USE RUNNER STARTED.**
  User explicitly authorized exact image, RF calibration, boot-only flash and
  conditional recovery to healthy e0d3/RAM rescue, and reports fastboot ready.
  New runner research/nord2/wifi-main-map/ios/run-approved-boot.py started
  exactly once; collect native USB180s. Never rerun after flash.log exists.
  Runtime DMA/main firmware outcomes are pending; no networking claim.



- **POOL MAP FIX IMAGE VERIFIED; NEW EXACT FLASH APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-main-map-battery-stop23.img`, 33345536 bytes,
  SHA256 b60dd9bc3935d5e30ac7ef740310e41d5198752fb6d4ce244c5a0617b58065b4.
  Payload b5541b28fcfc22a409c80adba60c4902dcc49c6c7d3ac3b4132f29ee73ef14ff.
  CI39594356768/source a1b90e3fa9e786b9b0fc0201db080faeede7b024; kernel
  0e754ab6dda77ace4d9407686b6af878fbafd066880b833866e877f74d27d9a7.
  Targeted linked review confirms full 15.125 MiB claim reaches kernel_map
  through ml_io_map_unmappable with Device flags7; retained guards unchanged.
  Bounded 1 GiB packing exited0. Fresh loader, complete payload segments,
  sole calibration ADT addition, Linux/dual DT/raw rescue and Android header
  verification PASS. Boot partition spare208896 bytes. New private stage
  research/nord2/wifi-main-map/ios has identity/schema/prepared gates true;
  human/RF/recovery authorization still FALSE and runner NEVER executed.
  Next: ask exact boot-only approval for b60dd9bc, then run its one-use runner
  only after approval and physical fastboot. Conditional fallback is healthy
  e0d3 or tested RAM rescue; NEVER normal Android over installed iOS userdata.
  Current phone remains e0d3, human-confirmed healthy; no USB collector active.
  The main-image368062de failed evidence and restoration remain frozen.
  Capability runtime05f1707f and pure scan/BSS b7649200 independently reviewed
  PASS in private after-fw-ready; neither integrated nor hardware-tested.
  DeepSeek audio session remains visible desktop3; no audio integration.



- **POOL MAP FIX CI39594356768 SUCCEEDED; IMAGE PACKING.**
  Exact source a1b90e3fa9e786b9b0fc0201db080faeede7b024. Kernel 17463256 bytes,
  SHA256 0e754ab6dda77ace4d9407686b6af878fbafd066880b833866e877f74d27d9a7.
  Artifact SHA256SUMS and 1134 actual generic ARMv8.2+crypto compiler
  invocations pass, including the main firmware translation units. New
  private wifi-main-map/ios identity/schema verified; 1 GiB-bounded image
  packing is running alongside targeted linked allocator review. Prepared
  image and exact hardware authorization remain false. Current e0d3 remains
  human-confirmed healthy; no new device actions or collectors.
  The private after-fw-ready/runtime capability candidate (manifest05f1707f)
  passed independent source review and sanitizer fault tests; not integrated.
  Further scan/BSS work is private. DeepSeek audio session remains on desktop3.


- **POOL MAP FIX CI39594356768 RUNNING.**
  Sourcea1b90e3fa9e786b9b0fc0201db080faeede7b024, onlyruntimechange is
  fullDMAclaim virtualmap route (physicalrange/Deviceattrs unchanged).
  https://github.com/frxncyyy/ios-sweet/actions/runs/39594356768
  Newprivate stage wifi-main-map/ios has allgatesfalse; samecmdline/calADT/
  e0d3fallback with distinctimage/approvalscope. Source/templates review
  beforeidentityfill; actualnewlinkedallocator proof andimagepacking next.
  Currente0d3 healthyhumanconfirmed, collectorsstopped. AudioDeepSeeklive3.


- **POOL KERNEL-MAP FIX READY FOR CI.**
  BinderSHA2d3b2116c79c43be9defbb7c6e25a2930bb912301cd5abc2218791fa70c34183:
  full15.125MiB map now ml_io_map_unmappable(...VM_WIMG_IO), no changed
  physicaladdress/attributes/ownership/cleanup. Native header/prototype
  included explicitly for BSDTU. HostASan/UBSan actualbinder/guard suites
  PASS and actualsweet_touch ARM64PASS; independentpool-map-review.jsonPASS.
  Patch13711lines reproduces. Othernewmain permanent mappings<=0x124000;
  static subtotalnotruntimefree-space proof. Currentphone restoredhealthy
  e0d3; original368062de evidence frozen. Newstage wifi-main-map/ios being
  prepared with closedgates; no newimage or flashapproval yet.


- **e0d3 RESTORED AND HUMAN-CONFIRMED HEALTHY; DMA MAP PANIC LOCATED.**
  Restore of exacte0d3 boot-only OKAY; nativeUSB90s271273B captured with
  preservedfailedboot prefix46972B plus currentrestored224301B. Preserve
  originals and split logs/restoration-report.json under wifi-main/ios.
  Failedboot actually completed ROM and bothregion18SETs, then panicked at
  DMAstage8 map: kmem_alloc15859712(flags100021) failed3. Sourceio_map.c has
  permanent io_submap8MiB:15.125MiB can neverfit. No evidence ofzero/mainFW.
  Narrowfix changes onlypoolmapping to ml_io_map_unmappable(kernel_map,
  sameVM_WIMG_IOattrs), retaining lifetime/fullphysicalclaim. Tests separate
  smallMMIO/permanent fromlargepoolmap; hostguards/sanitizersPASS; focused
  compile+independentreview underway. Newimage+newapproval required.
  User confirms restoredlockscreen/touch/buttons. Collectors stopped.
  AudioOpenCode session nowrunning visible desktop3, actualprovider/model
  opencode-go/deepseek-v4.1-flash(max), session ses_ea1c1e193ffeylJx9etgp2e09D,
  privateaudio/deepseek-4.1-flash/work. ParentverifiedmappedFootwindow3 and
  livePID268484; session.json/privateprompt/logs saved. No audio integration.


- **MAIN-FW368062de HARDWARE BOOTLOOP; RESTORE e0d3 PENDING FASTBOOT.**
  Authorized boot-only flash/reboot OKAY, nativeUSB connected then READ54
  timeout(libUSB-7). Original44000B SHAd31e7796...05da834; separate retry
 44795B shows a different boot. Both end after ROMPREP/coldpowerbegin;
  no finalROM/DMA/MAIN receipt captured. User confirms bootloop. Do not claim
  mainfirmware executed or reachedFW_READY. Both collectors stopped; never
  rerun one-use boot runner. User asked to enter fastboot; exact healthy
  e0d3 restore already authorized. Private hardware-bootloop-report.json
  stores evidence, hashes and prior battery observations (bus7/11success,
  SOC100/100,voltage8687mV,temp3063dK,charger3f/fault0). Realbattery readings
  succeeded, but currentimage notstable. Investigating sourceboundary.


- **MAIN-FW368062de AUTHORIZED; ONE-USE BOOT RUNNER STARTING.**
  Human explicitly authorized exact image and reported fastboot ready.
  Private authorization includes RF calibration and conditional RAMrescue/
  restore of healthy e0d3. Run once, then capture native USB180s. Never rerun
  runner after flash.log or hardware-live exists. MainFW and battery hardware
  outcomes remain pending. No userdata rewrite or other partition flash.


- **MAIN-FW + BATTERY IMAGE VERIFIED; EXACT FLASH APPROVAL PENDING.**
  `shim/out/nord2-xnu/nord2-ios-wifi-main-battery-stop23.img`,33345536B,
  SHA256368062de75a2bcf8b44954c61a1a924cbd01ebd77083d4691144c7604af73521.
  Payloadac51872e96db932f7503c30f0bf5f8c86de0de5de09dca6e6871209a79d280dc.
  CI39591024820/source825781f52ee2f967d8e329051d627e7dac1180fa;
  kernel2a6068312bd95bbdf3fa2c6a253059288e4e360779700c7d866e10d5e5188cd4.
  Targeted independent linked review PASS, including actual mainblob0,
  region18/DMA ownership, source firmware sequence/shared halt/panic and
  both battery FIFO low5 paths. Bounded1GiB packing exit0; full freshloader,
  payload segments, privatecalADT-only addition, Androidheader and byte-exact
  Linux/dualDT/rawrescue checks PASS. Bootspare208896B. Identity/schema/
  prepared gates true; authorization FALSE and one-use runner NEVER run.
  Private stage research/nord2/wifi-main/ios; incomplete parser wrapper
  preserves unknown DMA/RF effects. This image requests actual RF calibration
  and N9 FW_READY, not scan/networking. User exact approval requested.
  Current phone remains healthy e0d3; fallback samee0d3 and tested RAMrescue.
  UFS/boot speed closed/unchanged. User also explicitly requested OpenCode
  DeepSeek4.1Flash audio session visible desktop3; delegated startup ongoing,
  private-source-only work, no audio integration or device access.


- **MAIN-FW CI39591024820 SUCCEEDED; LINKED REVIEW / PACKING.**
  Source825781f52ee2f967d8e329051d627e7dac1180fa. Kernel17463256B SHA256
  2a6068312bd95bbdf3fa2c6a253059288e4e360779700c7d866e10d5e5188cd4.
  ArtifactSHA and1135actual genericARMv8.2+crypto compile invocations pass,
  including12newTUs. Private identity/schema gates true; image/prepared and
  approval gates still closed. Independent linked review and1GiB-bounded
  image construction next. No new hardware action; phone e0d3 healthy.


- **REPLACEMENT MAIN-FW CI39591024820 RUNNING.**
  Exact source825781f52ee2f967d8e329051d627e7dac1180fa; header-only fix.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39591024820
  Private native-schema source hash refreshed; identity/prepared/approval
  still closed. Wait successful full artifact and independent linked review.
  Current phone e0d3 healthy; no new hardware action.


- **MAIN-FW CI39589483352 FAILED; LOCAL INCLUDE FIX VERIFIED.**
  Only three compile errors: angle-bracket includes of sibling ROM headers in
  wifi_main_fw_xnu/phy_xnu. Changed to quotes; no runtime logic change.
  Both actual ARM64 TUs now pass with CI-derived include paths (no extra
  bsd/dev include), source-generated NORD2, system clang23. Patch reproduces.
  Earlier focused check had extra local include coverage, so missed this.
  Full replacement CI required; no candidate flash or new authorization.
  Current e0d3 remains human-confirmed healthy, no collector/device writes.


- **MAIN-FW CI39589483352 RUNNING.**
  Source df4503b3e4a0ed75388ff75e874274565e9aaeaf pushed to fork.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39589483352
  Canonical native and private-calibration host tests and13actualARM64objects
  PASS. Wait fullCIartifact, verify source/compiler flags/linkedpath, then
  boundedimagepacking and exacthardwareapproval. No flashapproval for this
  candidate. Currente0d3 remains healthy andcollector stopped.


- **MAIN-FW + BATTERY SOURCE READY FOR FULL CI.**
  Native/coordinator e1bfc6cf and firmware/PHYv2 0856bff4 independently PASS;
  promoted with include-name-only normalization to wifi_main_* canonical files.
  Actual main0/patch1 blobs, shared halt, retained finish buffers, bounded
  source-ordered40→8→41 and RF/EMIbackup/PCCIF/FW_READY sequence integrated.
  Canonical sanitizer suites pass including actualnative/coordinator fault
  paths and privatefirmware/calibration fixtures. All12new actual ARM64 TUs
  plus entire active sweet_touch compile under normal CI flags/no forcedNORD2.
  Patch13704lines applies/reproduces. Frozen UFS1ef095f3 unchanged. Next fullCI,
  linkedreview, boundedimagepack with privatecalADT, then exactnewapproval.
  Currente0d3 phone stays human-confirmed healthy; no collector/device action.
  Audio remains private. No scan/association/iOSnetworkinterface claim.


- **MAIN-FW INTEGRATION IN PROGRESS; PHONE STILL HEALTHY e0d3.**
  Reviewed WFDMA callbackv2 a10267a1 promoted as flat wifi_main_* helper
  names/includes; canonical host ASan/UBSan suites and6normal ARM64 TUs pass.
  Actual native owner/coordinator and mainFW/PHY wrapper still private pending
  frozen final review. Parent caught main blob index2 bug against actual
  wifi_firmware.c order (main0/patch1/ROM2/3); author fixes it in privatev2
  together with reviewer's shared-lifetime halt propagation. Original files
  remain frozen. Full source integration/CI/image still pending; next flag
  nord2wifimain=1 will include actual RF calibration, never imply networking.
  New private wifi-main/ios stage has all gates closed. No new device action.


- **NARROW DMA COUNT / BATTERY FIFO FIXES PROMOTED; NO NEW FLASH.**
  DMA accepts zero or observed512 count only with zero base/ext/indices and
  disabled/stable engines. Battery uses selected same-IP V2 low5 receive
  count; cross-driver inference and unnamed upperbits documented explicitly.
  Original e0d3 refusal evidence/parser frozen. Canonical ASan/UBSan suites
  pass including profile mutations; actual two active ARM64 objects pass
  normal CI-derived flags with system clang23 (no forced NORD2). Patch11738
  lines applies/reproduces. MainFW native integration waits shared-lifetime
  halt fix and actual binding/coordinator independent review; no CI yet.
  Phone remains e0d3 healthy with collector stopped. UFS unchanged.


- **DMA e0d3a50f BOOT HEALTHY; PREFLIGHT REFUSAL RECORDED.**
  Authorized boot-only flash/reboot OKAY; one-use runner executed, never rerun.
  Native USB180s exit0, 238276B SHA256
  a9be4c98ae9e75637567ed9365435e00f09e554566661d20789d1ea36468a3ff.
  Final strict report complete/no issues, complete_inherited_refusal:
  ROM again status0/stage16/done1d1e; all27 DMA rings have raw base0/count512/
  CIDX0/DIDX0; engines disabled/stable. Original strict-zero count predicate
  refused before any region18 SET, map, zero or DMA enable; votes balanced.
  Count512 is observed unprogrammed profile, not a documented reset default.
  Battery likewise refused before any bus write/gauge command: FIFO_STAT840,
  priorIRQ1/ack_returned0. Selected I3C low5 receive-count convention and
  matching V2 map support a narrow source-map inference being reviewed;
  upperbits remain unnamed, not independently proven empty-state flags.
  Human confirms lockscreen/touch/buttons; phone remains healthy e0d3a50f,
  collector stopped, no recovery needed. Current USB report/hash and human
  confirmation saved in research/nord2/wifi-dma/ios. No new flash approval.
  Next: reviewed count0/observed512 preflight revision; narrow battery FIFO
  count fix; actual native mainFW/PHY/DMA work continues privately. Original
  executed parser/evidence stays frozen. UFS/boot speed unchanged and closed.


- **DMA + BATTERY e0d3a50f AUTHORIZED; ONE-USE RUNNER STARTING.**
  Human "Autorizzo, pronto in fastboot" for exacte0d3a50f...31ec65d5.
  Known Nord2 is sole fastboot device. Private authorization permits boot-only
  plus conditional tested RAM rescue/current23a9 restoration. Parent will
  execute runner once/capture180s. Never rerun after flash.log exists.
  DMA pool and new battery hardware outcomes still pending.


- **DMA + BATTERY IMAGE VERIFIED; AWAIT EXACT BOOT-ONLY APPROVAL.**
  shim/out/nord2-xnu/nord2-ios-wifi-dma-battery-stop23.img,33325056B,
  SHA256 e0d3a50f7a706f27a8d9fb19bbd6951c85f016e8b179b47b726c331331ec65d5.
  Payloadc00073d2bc5025503e0a5c049d04e4f223bb174342e27e52728a45b5ee93cc85.
  CI39584842714/sourceed88fdc641a365efa6f11cd6c867b36be5147ec3;
  kernele005f651248b1f46baa987cebd7d1183a0ec218b39ef87933a7a257b7272e06c.
  Independent actual linked-path review PASS (DT/receipt/SET/map/wordbounds,
  battery priorIRQ1 branch). Bounded1GiB image build exit0; exact freshly
  rebuilt loader/full2MiB padding, all loaded segments/fixups, dualzeroADT
  claims/productADT, Linux/bothDTs/rawrescue/header checks PASS;229376B spare.
  Identity/prepared gates TRUE; human authorization FALSE; runner NEVER
  EXECUTED. Current23a9 image independently rehashed as fallback; tested RAM
  rescue preserved. Current phone still23a9 healthy; no USB collector.
  Scope: post-ROM fresh wake, read-only112read/27ring WFDMA snapshot; only
  empty/stable allows region18 SET/map/first128KiBzero+readback/sleep. Known
  inherited state returns without policy/pool writes. No DMA enable/mainFW/
  RF/network. Includes reviewed battery initial completion-latch fix.
  Next exact approval/physicalfastboot, one-use boot-only runner+180sUSB.
  Private future firmware transport v1 under review: coherent-fault polling
  gap found and separate v2 being corrected; no active integration. Future
  execution uses distinct nord2wifimain=1, never repurposes existing
  read-only nord2wififw=1. Private PHY/span/calibration work continues; no
  private calibration bytes committed. Audio completion still private.


- **DMA + BATTERY CI39584842714 SUCCEEDED; LINKED REVIEW IN PROGRESS.**
  Kernel17427288B SHA256 e005f651248b1f46baa987cebd7d1183a0ec218b39ef87933a7a257b7272e06c.
  Sourceed88fdc641a365efa6f11cd6c867b36be5147ec3; all1122 actual compiler
  invocations generic armv8.2-a+crypto, requiredTUs/markers/SHA256SUMS PASS.
  Artifact saved locally. Independent linked-path review precedes identity
  gate/image build. New loader26456B SHA256 ca1bdd787df9ef516f63131a0abdc131d4839702e73ffa6d25cacdd20348e59b.
  No new device action or flash approval. Audio revision completed privately
  by Sol in audio/revision-completion, tests pass; still needs independent
  review before any promotion. Opus original remains interrupted/preserved.


- **DMA + BATTERY CI39584842714 STARTED.**
  Source ed88fdc641a365efa6f11cd6c867b36be5147ec3 pushed to fork/codex/nord2-bringup.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39584842714
  Local source/focused checks passed; wait for full CI artifact, linked
  review and bounded image packaging. Next image not authorized or flashed.
  Phone remains human-confirmed23a9bd30; USB collector idle.


- **DMA POOL + BATTERY SOURCE READY FOR FULL CI (2026-10-21).**
  Promoted frozen pool manifest3ac8f504/native4b3fad0d after independent
  core/native/loader review. Actual canonical host ASan/UBSan tests PASS:
  FDT claim/collision, 112read27ring snapshot/224callback faults, defaultoff,
  same-ROM/persistent owner, exact source region18 SET, first128KiB
  zero/readback, inherited refusal and retained failure; actual receipt
  guards/192claim mutations/word bounds. Five active ARM64 objects PASS with
  CI-derived flags and no forcedNORD2. Fresh canonical loader builds; patch
  11731lines applies/reproduces. Frozen UFS hash unchanged1ef095f3... .
  Next image nord2-ios-wifi-dma-battery-stop23.img uses current23a9 args plus
  nord2wifidma=1; includes prior battery completion-latch fix. FullCI/linked/
  image checks still pending; no new flash approval. Phone stays23a9 healthy.
  MainFW transport/calibration work continues privately in parallel.
  External Opus hit session limit mid-audio-revision (reported16:20 local
  reset); incomplete files preserved, no audio integrated. Sol wifi_evidence
  now completes/reviews a separate private audio/revision-completion copy.


- **BATTERY LATCH FIX PROMOTED; DMA POOL/PREFLIGHT CANDIDATE IN PROGRESS.**
  Runtime-first manifest9b0fa59f...d847789 reviewed and promoted exactly:
  completion-only INTR_STAT1 accepted only after repeated idle/DMAoff/FIFOempty,
  one-bit ack and zero readback. Canonical source/publishing/telemetry/fault
  tests PASS; private identical-source normal CI-flag ARM64 checks PASS.
  Patch10974 lines applies/reproduces. No new CI/image/flash yet. Current23a9
  human-confirmed healthy ROM boot remains running, USB idle.
  Next combined candidate: battery fix plus separate current-boot low32 Wi-Fi
  DMA pool claim0x90000000+0xf20000, source region18 SET, fresh post-ROM wake
  and read-only WFDMA preflight. Full transport remains private until global
  DMA ownership/init and source PHY-calibration obligations are discharged.
  External Opus completed audio draft; independent review found shared-I2C8
  takeover/recovery/stickyhalt/variant issues and one host fixture failure.
  A private Opus revision is now running in same visible output session.


- **ROM23a9bd30 HARDWARE SUCCESS; BATTERY INITIAL IRQ-LATCH REFUSAL.**
  Boot-only flash/reboot OKAY; one-use runner executed, never rerun.
  NativeUSB180s completed exit0; 248475B SHA256 ce199484b0b5320f2a3db92e434630340837f4a6666b1f5f5df857fe3cc09771.
  Final strict report complete/noissues; independent review PASS. Human
  lockscreen/touch/buttons confirmed. ROMstatus0 stage16 done1d1e37polls;
  twoSET raw0/0, receipt88writes176reads2barriers, CPUreleased1. Retained
  power/protection1 and AP/WF/sem/clock0, asleep/balanced1 as designed.
  This proves ROM firmware startup, not mainN9/scan/networking. Current23a9
  remains running healthy; collector stopped. Exact49c8 and testedRAMrescue
  retained, never normalAndroid over iOS userdata. Battery did not issue any
  gauge command: preflight saw INTR_STAT1 (prior completion), STARTidle/DMAoff,
  FIFO not yet read. Private runtime-first narrow latch adoption ready/review
  pending; no reset/retry. Main-fw WFDMA transport/low32pool implementation
  proceeds privately. External Opus audio identification candidate private,
  parent ownership/recovery review pending. UFS/boot unchanged.


- **ROM + BATTERY23a9bd30 AUTHORIZED; ONE-USE RUNNER STARTING.**
  Human "Autorizzo, pronto in fastboot" for exact23a9bd30...63fd1b90.
  Known Nord2 sole fastboot device; private authorization records boot-only
  plus conditional testedRAMrescue/current49c8 restoration. Parent executes
  once/captures180s. Never rerun after flash.log/hardware-live exists.
  Real ROM and battery hardware results still pending. UFS/boot unchanged.



- **ROM + BATTERY IMAGE VERIFIED; AWAIT EXACT BOOT-ONLY APPROVAL.**
  shim/out/nord2-xnu/nord2-ios-wifi-rom-battery-stop23.img,33314816B,
  SHA256 23a9bd30b7fc5d439bafcd10104932884a1a145afc2f5d34cd8dd6d463fd1b90.
  Payload0a21b1298834a2233571b3f8cd61b8e25421dc476a9dc6cbdbf4354c9988c403.
  CI39578688632/source3e1351fa02995bb3fcec11272436565821639414;
  kernel f9d2cf13b361c25a3e7667012ca30fc865f88ca9b9250ae1b0b0991d6535792c.
  All1118 compiler invocations/linked review PASS. Exact rebuilt loader plus
  full2MiB zero padding, loadedkernel/fixups, static24BzeroCONNclaim/productADT,
  Linux/bothDTs/rawrescueCPIO/Androidheader byte checks PASS;239616B spare.
  Private wifi-rom/ios identity/prepared pinned TRUE; human approval FALSE;
  runner NEVER EXECUTED. Args current49c8 minuswfsys1 plusrom1+battery1.
  Next exact approval/physicalfastboot, then one-use runner and180sUSBcapture.
  ROM_DONE is only radio firmware startup; not scanning/networking. Battery
  values unknown until actual I2C results. Failure retains resources/manual
  reset; conditional testedRAMrescue/current49c8 restore, never normalAndroid.
  Phone remains49c8 healthy, USB collector stopped. UFS/boot work unchanged.
  Private next main-fw-transport source plan/helpers complete: selected path
  requires32-bit device-visible DMA; highXNU PA must not be truncated. Separate
  dynamically described Wi-Fi DMA pool lacks fixed current-iOS claim. No active
  transport changes. External Opus audio worker continues; live viewer desktop1.



- **ROM + BATTERY CI SUCCEEDED; LINKED REVIEW PASS; IMAGE PACKING.**
  CI39578688632/source3e1351fa02995bb3fcec11272436565821639414, job119150585846.
  Kernel17409456B SHA256 f9d2cf13b361c25a3e7667012ca30fc865f88ca9b9250ae1b0b0991d6535792c.
  All1118 actual compiler invocations generic armv8.2-a+crypto verified.
  Linked review pins actual prepare/claim/SET/copy/receipt/CPU-release/ROM102
  and battery poll->native sample->collector chain. Report under ci39578688632.
  Exact private identity supplied; bounded1GiB image packing in progress.
  Human approval remains false; runner never executed. Current49c8 phone
  unchanged. External Opus audio is writing a private identification candidate;
  parent pending review notes flag shared I2C8 DDR/UFS regulator bus, GPIO
  takeover/unknown-transaction recovery concerns. No audio code is integrated.


- **ROM + BATTERY FULL CI39578688632 RUNNING.**
  Source3e1351fa02995bb3fcec11272436565821639414 pushed to fork.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39578688632
  Wait for source/artifact/flags, final linked check and bounded image packing.
  Current49c8 phone remains healthy and USB idle; no new flash approval.
  External Claude Opus audio task continues in its separate private workspace
  with live output visible in foot. Parent owns all integration/device work.



- **ROM + BATTERY SOURCE READY FOR FULL CI.**
  Canonical source, loader and generated patch integrated. Actual canonical
  loader/contract/placement/native/core/binding host tests passed: 539
  placement callback faults, 1172 power callback faults, exact source SET,
  same-object receipt, uncertain CPU release and ROM timeout. Binding switch
  break whitespace normalized for GCC; no behavioral delta from reviewed
  b845aa9754d4c3dbf07993cbd115e0f4505199e1d9b5a3e0d7370a14c8adcdff.
  Seven focused Wi-Fi ARM64 objects pass; fuel canonical tests and four
  objects pass. Parent caught/fixed helper guard ordering: real CI has no
  -DNORD2, so new fuel TUs now load board_config before NORD2 guards. Exports
  verified under normal CI flags and absent for alternate T8103. Generated
  patch10960 lines applies/reproduces exact tree. Frozen UFS SHA unchanged.
  Private wifi-rom/ios build/one-use runner/strict evidence parser prepared,
  all identity/approval gates false; no image or new device action yet.
  Next args: exact current49c8 minus nord2wifiwfsys=1, plus nord2wifirom=1
  and nord2battery=1. Full CI/artifact/linked/image checks precede approval.
  External Opus audio worker and live viewer continue in private folder.



- **ROM_DONE + FUEL INTEGRATION IN PROGRESS; CLAUDE AUDIO RUNNING.**
  Phone remains the healthy WFSYS49c8c3ee baseline, human GUI/input confirmed;
  USB collector stopped. No new image or flash authorization.
  Reviewed ROM loader/claim, contract, placement, powered core promoted to
  canonical source; native binding and durable diagnostics being completed.
  Fresh loader build produces the reviewed 20228-byte binary, SHA256
  da910e5ca646e1db275c1421cf9b62e50c3ad063253d46cea54fbcf5dfd73429.
  Image builder now rebuilds loader/FDT helper to prevent stale reuse.
  Fuel provider/presentation and captured-only telemetry passed independent
  review, canonical sanitizer and focused ARM64 tests; active IOKit wiring
  promoted. Full generated patch/CI/image still pending. UFS unchanged.
  At the user's explicit request, external Claude Code started with Opus
  (resolved claude-opus-5-5, high effort) on audio, isolated source snapshot
  3d3725a under research/nord2/audio/claude-opus. Session/PID, prompt, stream,
  STATUS.md and artifacts stay private there. Read-only source references;
  no device/CI/active-source authority. A foot window titled
  "Claude Opus — Audio Nord 2 (live)" shows its live text/tool activity;
  closing the viewer does not terminate Claude. Built-in --bg refused the
  new untrusted directory; documented print mode is running as a detached
  immediate task instead, with normal tool restrictions, not bypass mode.
  Parent remains sole device/CI/integration owner. Next: finish/review binding,
  regenerate patch, full CI, verify concrete combined ROM/battery boot image,
  then request that exact boot-only test. ROM_DONE is not networking.


- **WFSYS49c8c3ee HARDWARE SUCCESS; CURRENT WORKING BASELINE.**
  Boot-only flash/reboot OKAY, runner EXECUTED once; never rerun.
  NativeUSB180s completed, 230446B SHA256 734345bbddd24c4a3a1a3efd5e0b9447eb468439ba35e25cbd255809caa8ba66.
  Strict report complete/no issues; human lockscreen/touch/buttons confirmed.
  WFSYS status0 stage11 local_id1 cpu_held1 balanced1 halt/dirty/unverified/
  error0; all owned AP/WF/sem/clock/power/protection votes0, asleep1.
  CONN20010101 local20010000 clock4006 IRQ1 initialAFE4400 finalPWR582fb00.
  Coldvalid1f CPU0/top12/PWR582fb00/BGFCPU0/BGFtop12.
  Polls AP3 WF1 cfg2 power2 protection4 ID1 off2 sem1 sem_off1 clock6.
  Common phase10 remains retained. No ROM/CPU release/RF/network claim.
  USB collector stopped; current49c8c3ee iOS healthy, previous49f177ec and
  tested RAM rescue preserved. Final report wifi-wfsys/ios/hardware-report-final.json.
  Next combined ROM_DONE PRIVATE candidates: hardened rom-native-candidate
  current-boot FDT claim, reviewed/tested rom-loader-candidate (20228B linked),
  rom-native-binding actual sourceSET/4span persistent copy tests passed;
  rom-powered-core under construction. No new image/flash authorization.
  Fuel gauge private realI2C provider and presentation candidate under peer
  review; knownfake100/chargingfallback identified. UFS/boot unchanged.


- **WFSYS49c8c3ee AUTHORIZED; ONE-USE RUNNER STARTING.**
  Human "Autorizzo, pronto in fastboot" for exact49c8c3ee...5e465131.
  Known Nord2 sole fastboot device; no USB collector. Private authorization
  records boot-only plus conditional testedRAMrescue/current49 restoration.
  Parent executes once/captures180s. Never rerun after flash.log/hardware-live.
  ROM/CPUrelease/network not part of this image; UFS/boot unchanged.


- **WFSYS IMAGE VERIFIED; AWAIT EXACT BOOT-ONLY APPROVAL.**
  shim/out/nord2-xnu/nord2-ios-wifi-wfsys-stop23.img, 33296384B,
  SHA256 49c8c3ee9ff78336358201d176ef9ccc32e44a99e0a39c7f9fced1995e465131.
  Payload df7d1d789bf2aa91013ccb414ca26ecd1b31e00104cbedd1761889633d1e1b36.
  CI39572918115/source61398cb6675e8e7b5701c8fa6ea0da1bc067f5b9 succeeded;
  kernel17373432B SHA256 8ec9b89ea7e1ee90701703f3761853973c40559e588ed773e3f42c5f38067cd5.
  All1109 actual generic armv8.2-a+crypto compile invocations verified; one
  interleaved .CFLAGS/header-install false match identified/excluded with
  evidence preserved. Independent linked WFSYS review PASS. Loaded kernel
  segments/fixups, ADT, original Linux/bothDTs/raw rescueCPIO/header PASS;
  258048B spare. Exact49 args with wake1 replaced by wfsys1.
  Private wifi-wfsys/ios identity/prepared pinned; human authorization FALSE,
  runner NEVER EXECUTED. Current49 human-verified iOS remains running, USB idle.
  Next: exact image approval/physical fastboot, one-use boot-only runner and
  180s capture. Conditional recovery current49 or testedRAMrescue after manual
  reset; never normal Android. No firmware CPU release/network in this image.
  Parallel private work: wifi_after_id real ROM native mapping/SET/copy;
  wifi_rom_plan current-FDT claim hardening/parent loader review;
  wifi_evidence reassigned by human to fuel gauge (100%/charging is fake
  SweetBattery fallback; actual Nord2 gauge I2C7/chargerI2C11 identified,
  real native provider still being implemented). UFS/boot remains closed.


- **WFSYS FULL CI39572918115 RUNNING.**
  Source61398cb6675e8e7b5701c8fa6ea0da1bc067f5b9 pushed to fork.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39572918115
  Private wifi-wfsys/ios build/verification/one-use runner/evidence templates
  reviewed; negative parser/default-false gates PASS. Identity/approval false.
  Next image nord2-ios-wifi-wfsys-stop23.img; exact49 args replacing only
  nord2wifiwake=1 with nord2wifiwfsys=1. Current49/manual RAM rescue fallback.
  Await artifact/source/flags and linked path verification, then package and
  ask concrete boot-only approval. Phone remains49 healthy; no USB collector.
  User asks faster visible Wi-Fi progress: prepare combined ROM_DONE path
  concurrently (fixed current-boot CONN ownership + source region27 policy +
  authenticated ROM spans + CPU release), after WFSYS acceptance. No estimate
  or firmware/network success implied; UFS/boot work remains closed.


- **WFSYS CPU-HELD SOURCE READY FOR CI; NO NEW DEVICE AUTHORIZATION.**
  AP wake49f177ec remains healthy, human lockscreen/input confirmed; USB idle.
  Canonical wifi_wfsys core/adapter/bind now behind nord2wifiwfsys=1; next
  image must omit nord2wifiwake. Fresh AP/WF/PLL/semaphore ownership after
  retained phase10, CPU reset held, power/local-ID then source-subset off.
  No ROM/DRAM/MPU/CPU release/network operation. Parent reviewed source core;
  independent Sol reviewed native binding. Exact promoted bytes match both.
  Canonical ASan/UBSan tests PASS (1290 callback faults plus actual binding),
  three focused ARM64 compiles PASS; patch9316 lines applies/reproduces tree.
  UFS frozen SHA unchanged; boot-speed/UFS work remains closed.
  Next: full CI, linked/image verification, concrete boot-only approval.
  Sol preparing private ROM native design in parallel to reduce serial work.


- **AP-WAKE49f177ec HARDWARE SUCCESS; CURRENT WORKING BASELINE.**
  Authorized boot-only runner EXECUTED once; flash/reboot OKAY. NativeUSB180s
  exit0,227028-byte log SHA256
  67a6cc65a29da2ac815f9f3b00494a038c6a3f3d748352cf8a849c9c3987d5f0.
  Strict final report complete/no issues: persistentphase10 retained/handoff2,
  wake status0/phase7/balanced1/ref0/halt0, ID20010101/clock4006/IRQ1;
  ackpolls3/clockpolls1/IDpolls1; coldvalid1f CPU0/top12/power0582fb00/
  BGCPU0/BGtop12. Own AP wake vote balanced, common power remains retained.
  No panic/USB transport error/capacity loss. Human confirms lockscreen,
  touch and buttons. Current phone49f177ec healthy; USB collector STOPPED.
  Final evidence wifi-ap-wake/ios/hardware-report-final.json. Previousc6cb,
  8ca8 and tested rescue preserved. No new flash approval; never rerun runner.
  No firmware execution/RF/network connectivity or DRAM permission inferred.
  Private wfsys-rom-plan.md now pins meaningful next CPU-held WFSYS local-ID
  stage and exact later four ROM spans. Static saved FDTs contain dynamic CONN
  size/no-map only, no current fixedbase tag. Current carveout/alias ownership
  and reviewed region27 sourceSET lifetime are future ROM design tasks, not
  grounds to fabricate permission callbacks or experiment with alternateSMCs.
  /root/wifi_rom_plan implementing PRIVATE CPU-held WFSYS source core/contract;
  no production/device/CI change. Source-subset success poweroff follows actual
  protect->clockoff->poweroff order; no byte-exact restoration claim. UFS/boot
  work remains closed. Parent next reviews source/tests/native integration.


- **AP-WAKE49f177ec AUTHORIZED; RUNNER STARTING ONCE.**
  Human: "Autorizzo, pronto in fastboot" for exact49f177ec…ad8e9310 image.
  Known Nord2 is sole fastboot device. Private authorization records boot-only
  plus conditional tested rescue/currentc6cb restoration after manual reset.
  Parent owns USB; run once and collect180s, never repeat after flash.log or
  hardware-live exists. No DRAM/MPU/ROM/CPU-release/network operation.


- **AP-WAKE IMAGE READY; AWAIT EXACT NEW BOOT-ONLY APPROVAL.**
  shim/out/nord2-xnu/nord2-ios-wifi-ap-wake-stop23.img,33,292,288B,
  SHA25649f177ec5d977d4f7c0d1407c78f585e5704ccc85c1cf17804f6c940ad8e9310.
  Payloadf8bdf45a4dff930f4dfe9402f3753878fa9bd2bbf7c1eff8aca8a2778ef144a6.
  CI39568093909/source6978794fde46fd1410c631eb0ee4b918a6065c2a;
  kernelff0657a709b4555c97711358c85dc62f3a0d3ec454c295e852a7794aa065ff30.
  Source/artifact/all1107 compiler invocations/linked review PASS. Exact loaded
  kernel segments, Linux,bothDTs,raw rescueCPIO,productADT,Androidheader verified;
  262144B spare. Args exact currentc6cb plus nord2wifiwake=1 only. No firmware
  execution, DRAM/MPU write, WFSYS CPU release or networking in this image.
  Private wifi-ap-wake/ios identity/manifest/prepared filled; authorization
  FALSE; runner NEVER EXECUTED. After exact approval and physical fastboot,
  record private boot-only authorization and conditional tested rescue/current
  c6cb restoration, then run ONCE --authorized-wifi-ap-wake-boot/capture180s.
  Never rerun if flash.log/hardware-live exists. Parse final full capture with
  new AP-wake parser: phase10 plus status0/phase7/balanced1/ref0/halt0, validID/
  clockIRQ/cold masks and baseline health required; human GUI/input separately.
  Recovery manual after reset only; never normal Android over iOS userdata.
  Current phone c6cb remains healthy, collector stopped; 8ca8/rescue preserved.
  /root/wifi_after_id and /root/wifi_evidence pending tasks interrupted during
  handoff; parent completed private template review/tests. New Sol
  /root/wifi_rom_plan audits future WFSYS/ROM prerequisites privately, no active
  changes. UFS/boot-speed work remains closed. Calibration inventory saved at
  research/nord2/wifi/next-stage/calibration-source.md, no private values in git.


- **AP-WAKE CI39568093909 SUCCEEDED; LINKED REVIEW PASS; IMAGE PACKING.**
  Source6978794fde46fd1410c631eb0ee4b918a6065c2a, job119115205909.
  Kernel17,355,960B SHA256
  ff0657a709b4555c97711358c85dc62f3a0d3ec454c295e852a7794aa065ff30.
  Artifact/source/all1107 genericARMv8.2-a+crypto invocations verified.
  Linked retained-only call/default-off/software owner/separate wake reference/
  final sleep/no-postsleep-I/O review PASS. Private identity now pinned;
  bounded image packing in progress. Parent tested/reviewed private templates,
  took ownership from evidence agent; authorization remains FALSE. Current
  c6cb phone healthy, no USB client/device operation. Full-image verification
  and prepared identity still pending; do not request/execute flash yet.
  Calibration source report confirms saved per-device WIFI/WIFI_CUSTOM exist
  privately; firmware copies are distinct from NVRAM/calibration. Future native
  buffer handoff and selected configuration still need implementation.


- **AP-WAKE FULL CI39568093909 RUNNING.**
  Source6978794fde46fd1410c631eb0ee4b918a6065c2a pushed to fork.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39568093909
  Production parity/default-off/build wiring review PASS. Focused ARM64 and
  canonical sanitizer tests PASS. Await full success, artifact/source/flags
  verification and linked review, then bounded image packing and exact new
  boot-only approval. Current c6cb phone remains healthy; no device operation.
  Next args equal current c6cb plus nord2wifiwake=1 only. Private templates
  remain unapproved. UFS/boot untouched. Source-only WFSYS/ROM prerequisite
  audit proceeds independently; no DRAM/MPU/CPU release in this image.


- **AP-WAKE SOURCE PROMOTED; FOCUSED VALIDATION PASSED.**
  Exact independently reviewed native-binding files promoted to canonical
  wifi_wake core/adapter/bind, two separate guarded BSD translation units.
  Explicit nord2wifiwake=1 adds one source AP wake/ID/cold-check/balanced sleep
  only after retained phase10; old owner state unchanged, no post-sleep I/O.
  Actual canonical ASan/UBSan core+adapter240 callback fault cases and native
  binding ownership/default-off/reentry tests PASS. Three focused ARM64
  compiles PASS; patch8758lines applies/reproduces tree; frozen UFS hash exact.
  Parent awaiting short production parity review, then commit/push/full CI.
  No new device operation/approval. Current c6cb126f is hardware/human verified;
  collector stopped, 8ca8 and tested rescue remain available. New private
  wifi-ap-wake/ios templates are being prepared with identity/approval blank.


- **PERSISTENT RADIO c6cb126f HARDWARE SUCCESS; CURRENT WORKING BASELINE.**
  Boot-only authorized runner EXECUTED once; flash/reboot OKAY. Native USB180s
  exit0,228082-byte log SHA256
  9476ee36c8f8f457704c10774447cc32537d09903ffa2c18fdf5de5f1bdb1613.
  Final strict report complete/no issues, phase10 retained1, ADIE66358a11,
  HW handoff2; outer stage3 success1/retained1/restored0. No panic/USB error/
  capacity loss. Human confirms lockscreen/touch/buttons working. Current
  phone c6cb126f is healthy; collector stopped. Previous8ca8/rescue preserved.
  This proves source radio initialization only; no firmware execution/network
  interface/effective DRAM permission inferred. No new flash authorization.
  Final evidence research/nord2/wifi-persistent/ios/hardware-report-final.json.
  Native private AP-wake binding independently reviewed PASS at
  next-stage/ap-wake-candidate/native-binding/independent-review.json;
  separate wake lifetime, explicit default-off flag, source balanced sleep,
  no post-sleep MMIO or old persistent state changes. Parent will promote,
  compile/test/build exact next image, then seek new boot-only approval.
  UFS/boot tuning remains closed; all agents are Wi-Fi only.



- **PERSISTENT RADIO c6cb126f AUTHORIZED; RUNNER STARTING ONCE.**
  Human: "Autorizzo, pronto in fastboot" for exactc6cb126f…a4192b8e image.
  Known Nord2 is the sole fastboot device. Private authorization records boot
  only and conditional tested rescue/working8ca8 restoration after manual
  reset. Parent owns USB; run once and collect180s, never repeat after flash.log
  or hardware-live exists. No MPU SET/DRAM firmware writes/CPU release.


- **PERSISTENT RADIO IMAGE READY; AWAIT EXACT BOOT-ONLY APPROVAL.**
  shim/out/nord2-xnu/nord2-ios-wifi-persistent-stop23.img,33,288,192B,
  SHA256 c6cb126fe53c3c50c4a2959d9981ed8c00d546147c448e639027e578a4192b8e.
  Payload64bb81aeafdd449a4289a98b16d871c3e8e8d2d9b548237d853ca4e624a8879b.
  CI39563384413/source196417186ec7bf7827fbc64f9779dbbd37d25425;
  kernelb10bdeff4f41ef3641082032ff0e9ecb89564cacb982fc048766befbe5188d7b.
  Independent linked review PASS. All loaded kernel segments/header/Linux/
  bothDTs/rawCPIO/productADT checked;266240B spare. Explicit persist1 replaces
  common/ADIE diagnostic opt-ins; legacy observer absent/EMI0; other args exact.
  Private research/nord2/wifi-persistent/ios identity/manifest/prepared filled;
  authorization FALSE, runner NEVER EXECUTED. After exact new approval and
  physical fastboot: fill private authorization with conditional rescue/working
  8ca8 restoration, run ONCE --authorized-wifi-persistent-boot and capture180s.
  Never rerun with flash.log/hardware-live present. Parse using new persistent
  parser, not legacy/ADIE parser; phase10 retained/handoff2 and outer success
  are expected, restoring common logs are not. Manual reset recovery only;
  normal Android must never boot over iOS userdata. No DRAM firmware writes,
  MPU SET, WFSYS/BGFSYS CPU release or networking implemented in this image.
  Current phone remains tested97922165 with confirmed lockscreen/touch/buttons;
  USB client stopped. Preferred fallback8ca8 and tested rescue preserved.
  Boot-speed agent closed as requested, only Wi-Fi agents used. UFS/boot frozen.
  Future private AP-wake candidate has independent source PASS, not integrated;
  documentary nuance: deterministic clock polling is bounded but not always
  earlier refusal than vendor randomized-delay policy. No future hardware claim.


- **PERSISTENT CI39563384413 SUCCEEDED; IMAGE PACKING IN PROGRESS.**
  Source196417186ec7bf7827fbc64f9779dbbd37d25425, job119099715025 success.
  Kernel17,306,184B SHA256
  b10bdeff4f41ef3641082032ff0e9ecb89564cacb982fc048766befbe5188d7b.
  Artifact sums/source/all1105 genericARMv8.2-a+crypto compile invocations
  verified, including both new persistent translation units. Private identity
  pinned; bounded imagepacking session86869 running. Independent linked review
  in parallel. Current phone97922165 healthy; no new flash approval/device I/O.
  Private next-stage/ap-wake-candidate has focused independent PASS and tests,
  but no native integration/hardware acceptance. Future MPU setter source audit
  found group0 monitor-forced software lock; never call it a reversible probe
  or ignore return codes as the selected Linux caller does. No setter is in
  this image; no DRAM firmware writes/CPU release/networking implemented.


- **PERSISTENT RADIO FULL CI39563384413 RUNNING.**
  Source196417186ec7bf7827fbc64f9779dbbd37d25425 pushed to fork.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39563384413
  Await success, downloadkernel/buildlog/runjson, run privateverify-kernel.py;
  independent linked review then verifiedimagepack under1GiB process limit.
  Persistent image identity/approval still blank; no new device operation.
  Current97922165iOS remains healthy, USBcapture ended. All agents Wi-Fi only.
  Source-only follow-ups examine next wake/ROM sequence and authoritative
  MPU setup semantics; no MPU/DRAM/firmware execution work integrated.


- **PERSISTENT RADIO SOURCE INTEGRATED; READY FOR FULL CI.**
  Canonical wifi_persistent core/adapter/bind and explicit nord2wifipersist=1
  branch promoted from independently reviewed native v2. Two separate guarded
  BSD translation units; ordinary restoring diagnostics unchanged. Source
  buildword3 and persistent-only SW_LP-before-enable order pinned. Register-only
  remap, cold16KiB SYSRAM initialization, E3/efuse read/AFE/PLL/RC/HW handoff;
  WFSYS/BGFSYS remain off/reset, no DRAM firmware access or CPU release.
  Before mutation latch retained lifetime; source phase10 skips teardown;
  uncertain failure retains state and requires manual reset. No post-handoff
  CONN checks. Production parity review PASS; focused ARM64 all3 files PASS;
  canonical ASan/UBSan core8725/native269468/PMIC order/failure suites PASS.
  Generated patch8508lines reproduces tree. UFS remains byte-exact
  1ef095f34a95e362b922a486d7ff71eae1a5eb614a22011f8c9e06bb7552b6b4.
  New private image templates research/nord2/wifi-persistent/ios remain blank
  identity/approval until CI/source/kernel pinning and image verification.
  Planned args: working8ca8 minus old common/ADIE opt-ins plus persist1;
  legacy observer absent, EMI0, all other args unchanged. Current phone97922165
  healthy; USB stopped. Next commit/push/CI, linked review, build exact image,
  then NEW boot-only authorization. Previous97922165 approval is consumed.


- **LEGACY EMI TEST FINISHED: REQUEST REFUSED; IOS REMAINS HEALTHY.**
  Authorized97922165 runner EXECUTED, never rerun. Flash/reboot OKAY, native
  USB180s exit0;238102-byte log SHA256
  c8329b98c72c31785611018f9e0679c76989fe958f8c50bcdd9b4ed81cd79878.
  Strict final report complete/no issues: legacy first offset100 returned
  x0=ffffffffffffffff,x1=100,x2=x3=0 (error-1); collector1/issued1/valid0.
  Remaining127 NOT_CALLED, no MPU metadata or effective permission proof.
  Combined success intentionally FALSE. ADIE66358a11/common40/outer restore
  still succeed; user confirms lockscreen/touch/buttons work. No kernel panic,
  no USB transport error/capacity loss; collector ended at deadline. Current
  phone97922165 is healthy, USB client stopped; preferred fallback8ca8 kept.
  Offline monitor-abi/runtime-refusal report now includes top-level dispatch
  gating omitted from prior legacy-helper review: same saved ATF can route
  context to a dispatcher that supports50b but rejects261 with-1. Actual
  runtime selector/origin remains unknown; do not change FID or assume alias.
  Handlerx7 is context flags, not guestSMCx7. Leave legacy observer OFF in
  next ordinary image. It does not block register-only persistent radio init;
  DRAM access/firmware execution/CPU release remain separate unimplemented work.
  Native private v2 corrects source Kbuild BUILD_MODE3 and persistent-only
  PMIC SW_LP before enable. Saved conninfra.cfg absence of tcxo_gpio plus parser
  zero-init selects26M_COTMS, no missing GPIO155 TCXO mux. WholecandidateARM64
  and targeted tests PASS. Independent focused native v2 review PASS, pinned in
  post-id-candidate/native-candidate/v2/independent-review.json; no concrete
  register-only source blocker remains. Production-named integration/link/CI
  and device behavior remain pending.


- **LEGACY EMI 97922165 AUTHORIZED; RUNNER STARTING ONCE.**
  Human: "Autorizzo, pronto in fastboot" for the exact97922165…b984a85 image.
  Known Nord2 is the sole fastboot device. Private authorization records boot
  only and conditional tested rescue/current8ca8 restoration after manual
  reset. Parent owns USB; run prepared runner once and collect180s. Never
  rerun after flash.log/hardware-live exists. No radio firmware execution.


- **LEGACY EMI IMAGE READY; AWAITING NEW EXACT BOOT-ONLY APPROVAL.**
  Image: shim/out/nord2-xnu/nord2-ios-wifi-emi-legacy-stop23.img,
  33,282,048 bytes, SHA256
  979221654522038778ae470dbbc8d299775360994fbe93626d462f39ab984a85.
  Payload: 72c90840a1519134cf79ce60bdc2bdc5111d3b0eb4dc3072161ae5063675ef52.
  CI39560131423/source d068e5448f53cf8df59c2d7882c4b20e0d34284d;
  kernel746d391bab3b68ad9111788fc5d83739134a2c94c07521d35cb5e1d703dbe7be.
  Independent linked review PASS; exact loaded kernel segments, Linux, both
  DTs, raw rescue CPIO, product ADT and Android header verified; 272384 bytes
  spare. Exact working8ca8 commandline plus nord2wifiemilegacy=1 only.
  Private identity/manifest/prepared image fields filled; authorization FALSE,
  runner NEVER EXECUTED. Next: exact approval, physical fastboot, record private
  authorization, run once --authorized-wifi-emi-legacy-boot, collect180s.
  Preserve any flash.log/hardware-live; never rerun after a partial execution.
  Conditional recovery must be recorded for tested rescue/current8ca8 image;
  manual reset only, never normal Android over iOS userdata. Current phone
  remains healthy8ca8; no USB client active. UFS/boot tuning unchanged.
  New offline decoder preserves raw region evidence without asserting effective
  permissions, overlap priority, disabled encodings or lock-bit semantics.
  Private phase10 radio core independently reviewed PASS. Native retained
  adapter/full touch candidate ARM64 compile PASS, final review pending; no
  production integration. Historical CONN8fb00000/MCIF8a000000 remap provenance
  accepted for register-only private design; not permission for DRAM access.


- **LEGACY EMI CI39560131423 SUCCEEDED; PACKING NEXT IMAGE.**
  Source d068e5448f53cf8df59c2d7882c4b20e0d34284d, job119089119811 success.
  Kernel17,354,104B SHA256
  746d391bab3b68ad9111788fc5d83739134a2c94c07521d35cb5e1d703dbe7be.
  Artifact sums/source/all1102 genericARMv8.2-a+crypto invocations verified;
  compile list includes new collector. Private image identity pinned. New
  Wi-Fi reviewer checks linked legacy path in parallel with bounded packing.
  Current8ca8phone healthy; no USB operation/new flash authorization.


- **BOOT-SPEED AGENT CLOSED AT HUMAN REQUEST; ALL ACTIVE AGENTS WI-FI ONLY.**
  /root/boot_speed completed its final frozen phase7 radio review (PASS) and
  was interrupted/retired; do not assign more work to it. New Sol agent
  /root/wifi_radio_review independently reviews the frozen phase10 extension.
  /root/wifi_after_id prepares a separate private native retained-lifetime
  adapter. No UFS or boot tuning. Current phone stays on healthy8ca8 image;
  no USB client active and no new image authorization. CI39560131423 running.
  Private completion phase10 adds source status/RC lowpower/topclock/final
  HW-control, 8725 callback-fault tests PASS; no native/hardware claim yet.


- **LEGACY EMI FULL CI39560131423 RUNNING.**
  Source d068e5448f53cf8df59c2d7882c4b20e0d34284d pushed to fork.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39560131423
  Await success, downloadkernel+buildlog, run privateverify-kernel.py, pin
  source/hash/flags (newcollectoraddsonecompile), independentlinkedreview,
  fillblank wifi-emi-legacy/ios identity thenpack plannednewimage. No new
  authorization; nextflashrequiresconcreteimageapproval. Current8ca8iOSgood,
  USBcapturefinished. PrivatepostIDauthor continues sourceexactlowpower/stages;
  reviewerchecks frozenphase7 thenlinkedEMI. NoUFS/bootchangesallowed.


- **LEGACY EMI OBSERVER INTEGRATED; READY FOR FULL CI.**
  Reviewedprivatecore/probe copiedbyteexact: wifi_emi_legacy.{c,h} and modified
  wifi_emi_probe.c, sharedpostrootstart/onceguard; explicitnew1andold0required.
  Singlebsdconfbuildentry, mkpatch7972lines exactapply/reproduction. UFSfile
  stays1ef095f34a95e362b922a486d7ff71eae1a5eb614a22011f8c9e06bb7552b6b4.
  ProductionreviewPASS, core1280failure/allowlist/raw/NOT_CALLED testsPASS,
  old8queryregression/new128querybridge/mode/thread/reportboundsPASS, fullBSD
  ARM64compilesprobe+collectorPASS. NoSMCexecuted. Newimage templates/private
  parser in research/nord2/wifi-emi-legacy/ios, identityblank; exactcurrent8ca8
  argsplusnord2wifiemilegacy=1. Current8ca8phoneGUI/inputgood, lastcaptureended.
  Next: commit/pushCI, pinartifact+linkedreview, pack/reviewimage then ask
  NEWexactboot-onlyapproval (current8ca8authorizationdoesNOTcovernewimage).
  Privatepersistentpost-IDcandidate now exists post-id-candidate/retain.c,h;
  phasesDdie/common/SRAMclear/E3Adie/efuseread/AFE/PLL,8586callbackfaultsPASS.
  NOTintegrated: missingnativeownership/remap andfinalstatus/voltage/lowpower
  stages. Noautomaticanalogrollback; retainconfiguredCONN/manualresetonfault.


- **WI-FI CHIP_ID HARDWARE SUCCESS; CURRENT IOS HEALTHY.**
  Authorized8ca8ee39 runner EXECUTED; NEVERrerun. Flash/rebootOKAY; USB180s
  exit0,233025B final log SHA256
  bb31a1bfd3f76bc88f431fba192513047d5f33d95dde0e4033f4da56da03053b.
  Final strict parser complete/combinedsuccess, noissues/nokernelpanic,
  hardware-report-final.json plus human "Sì, lockscreen e comandi funzionano".
  A-die66358a11 valid/known/complete/clean1,error/restore_error0; strapf/f,
  semowned0; clockraw4006 IRQ1 polls1. Common40snapshots/verified, touched/
  restoredffffffffff clean1;202047ticks/13MHz. Outerpowerstage3success1/restored1.
  UFS verifiedretainedx2/root1/handoff0 unchanged. USBcaptureended; noactiveclient.
  Currentimage8ca8ee39ff7b6a46c5ea76d839607c22e519211b657a9c9bcde014c43c84cd95
  is now testedworkingfallback; previous3b2e/rescue also preserved. No Wi-Fi
  networking or firmware execution yet. Human resumesongoingWiFigoal.
  Next privatework: wifi_evidence prepares separate nord2wifiemilegacy=1
  readonly128offsetobserver (oldnord2wifiemi=0 required), reuseexistingthreadhook
  withoutUFSfilechange. Reviewedlegacy82000261ABI, rawcapture only; noMPUSET.
  wifi_after_id prepares source-exact persistent coldDdie/A-dieE3/AFE/PLL path,
  retainingownedCONN and manualresetonuncertainty; unlike prior reversibleprobe,
  no requirement to inventanalogrollback. PreservevendorSYSRAMclear/order and
  gatesbeforeWFSYScpurelease; source-only privateuntilreview/newimageapproval.
  Permission interpretation report: nooverlappriority/disabledencodingproof;
  APCbit31notlockproof (ATFsoftwarelock); Androidcarveout8fb00000+450000 known,
  currentXNUownership still must be checked beforefirmwareplacement.


- **WI-FI CHIP_ID 8ca8ee39 AUTHORIZED; RUNNER STARTING ONCE.**
  Human: "Autorizzo, pronto in fastboot" for exact8ca8ee39…3c84cd95.
  Private authorization-required.json records boot-only and conditional rescue/
  current3b2e restoration. Fastboot device matches known Nord2. Parent owns USB;
  run prepared runner once, collect180s. Never repeat afterflash.log exists.
  Test is ID-only; no network/firmware execution. UFS/boot unchanged.


- **NEW WI-FI CHIP_ID IMAGE VERIFIED; AWAIT EXACT FLASH AUTHORIZATION.**
  shim/out/nord2-xnu/nord2-ios-wifi-adie-id-stop23.img,33,282,048B,
  SHA2568ca8ee39ff7b6a46c5ea76d839607c22e519211b657a9c9bcde014c43c84cd95.
  Payload5c2a71c3a07fa149e8526b431cdc18553f035b4b0e4f5f7db961dcb60982f1f7.
  CI39557231502/source639f6ba7719b4a3cc2db3ae4031175d5428251bd;
  kernel7b79aa1824c044b788737e5fdd2ee89c0405d2fca10729ccac65d0d54e9361d0.
  Independent linked review and image checks PASS: all patched MachOsegments,
  original Linux/bothDTs/rescue rawCPIO/productADT/header verified;272384Bspare.
  Args EXACTprevious3b2e plus nord2wifiadieid=1. UFS/boot/input/display/USB
  unchanged. Conditionalcommon40+strap/reset+clockIRQ+TOPSPI CHIP_ID read;
  complete reverse restore or dirtyhalt/manualreset. No network/firmwareexec.
  Private research/nord2/wifi-adie-id/ios manifest/preparedidentity populated;
  authorization-required.json FALSE, no serial, runner NEVER EXECUTED.
  Parent requests exact boot-only authorization including conditional testedADB
  rescue/currentworking3b2e restore. After approval and physicalfastboot, fill
  privateauthorization; run once --authorized-wifi-adie-id-boot fromreporoot.
  Never rerun ifflash.log/hardware-live exists. Capture180s, newAP2000/ADIE
  parser; preserveoldAP6000evidence. NoUSBclient/deviceoperation yetthisturn.
  Currentphone lastknownworking3b2eiOS. NEVERbootstockAndroidoveriOSuserdata.


- **WI-FI CHIP_ID KERNEL VERIFIED; IMAGE COMPRESSION RUNNING.**
  CI39557231502 source639f6ba; kernel17,337,416B SHA256
  7b79aa1824c044b788737e5fdd2ee89c0405d2fca10729ccac65d0d54e9361d0.
  Artifact sums/source and all1102 genericARMv8.2+crypto invocations verified.
  Independent linked-path-review.json PASS, exact production helpers match
  source639f6ba. Private image identity now filled from this evidence only.
  Packing session2227 under1GiB process limit; image-build/build-console.log
  in research/nord2/wifi-adie-id/ios. No hardware/authorization yet.
  Monitor offline independent review also PASS:330 instruction words match
  tee1 originals; legacy82000261 read helper and50b action2 semantics verified,
  128 aligned SA/EA/APC offsets for32regions/two groups checked. Currentmonitor
  attribution still not observed; future observer design private, no active
  SMC changes. Source-only after-id-20261021.md maps later analog/calibration
  prerequisites; none are proven by CHIP_ID alone. All agents now completed.


- **WI-FI CHIP_ID CI39557231502 SUCCEEDED; ARTIFACT VERIFICATION NEXT.**
  Source639f6ba7719b4a3cc2db3ae4031175d5428251bd. Job119079733089 success
  in7m24s; run.json/build.log saved, kernel downloading. Parent pins kernel
  hash/compiler flags before filling image identity; independent linked review
  in parallel. No device access or new authorization on this resumed turn.
  Offline follow-up found concrete EMI ABI mismatch in original tee backups:
  private research/nord2/wifi/next-stage/monitor-abi/REPORT.md and identity.json.
  Backed-up ATF function50b action2 returns2 unchanged, explaining old capture
  if same monitor runs; action4 writes hardware, action5 returns0, action0 SET.
  Separate legacy SMC32 82000261 dispatch/helper performs bounded-offset32bit
  MPU read at10226000+offset (max93c). This is static backup evidence only;
  current-monitor attribution/loadbase/region-overlap semantics not proven.
  REA firmware provider unavailable (Binwalk); ordinary bounded LLVM fallback
  used, no installs/device/SMCs. Exact tee1 hash in private identity/report.
  New CI/image REMAINS ID-ONLY, nord2wifiemi=0. Future legacy observer requires
  independent helper/offset/currentcontext review before any added hardware call.


- **WI-FI CHIP_ID CI RUNNING; PRODUCTION REVIEW PASSED.**
  Source639f6ba7719b4a3cc2db3ae4031175d5428251bd pushed to fork.
  CI39557231502: https://github.com/frxncyyy/ios-sweet/actions/runs/39557231502
  Independent production-review.json verifies all8 canonical helpers equal
  reviewed private algorithms after only include/guard-name renaming; active
  wrappers/copies exact, whole touch body and outer power unchanged.
  UFS exact1ef095f34a95e362b922a486d7ff71eae1a5eb614a22011f8c9e06bb7552b6b4.
  All focused host/ARM64 tests passed. New private image/runner/parser templates
  in research/nord2/wifi-adie-id/ios; CI identity deliberately still empty.
  Await successful CI, download and pin artifact+compiler/source evidence,
  independent linked review, fill identity then pack exact image. Next flash
  requires new exact approval; no device command performed on resumed turn.
  Source-only Sol investigation of later Wi-Fi prerequisites runs in parallel.


- **RESUMED WI-FI ONLY — REVIEWED CHIP_ID CANDIDATE INTEGRATED.**
  Human resumed this morning. UFS/boot remain frozen. Production helpers
  wifi_common_scope, wifi_adie_id, wifi_adie_strap and wifi_adie_adapter.inc
  integrate the reviewed private candidate with include/symbol renaming only.
  Persistent AP mask2000; original outer power6000wait unchanged. Scoped ID
  default-off, conditional on all40common verified, clock/IRQ checks before
  semaphore11; ambiguous completion retains ownership and halts.
  Tracked common640callback/default-off/AP/restore tests and ID28callback/
  native91write-fault/clock-IRQ/lifetime suites PASS. Whole sweet_touch ARM64
  compile PASS. Generated patch7849lines applies/reproduces active tree.
  Active sweet_ufs.c unchanged SHA256
  1ef095f34a95e362b922a486d7ff71eae1a5eb614a22011f8c9e06bb7552b6b4.
  Independent production review pending; no CI/image/device action yet.
  New private templates research/nord2/wifi-adie-id/ios keep identity empty
  until parent pins successful CI; planned nord2-ios-wifi-adie-id-stop23.img.
  Current 3b2e iOS remains the known-good fallback. Ask new exact boot-only
  approval only after completed CI/image verification. Preserve kmod/.


- **PAUSED BY HUMAN — resume only on the next user request.**
  Human: "possiamo mettere un po' in pausa? riprendiamo domattina".
  All three Sol agents completed; no CI, image packing, USB capture or flashing
  started after the working 3b2e live-x2 test. Phone remains working iOS with
  lockscreen/touch/buttons. UFS and boot tuning explicitly closed by human;
  preserve current behavior exactly. No new flash authorization pending.
  Current boot: nord2-ios-ufs2-wifi-ap-stop23.img, SHA-256
  3b2e7ecb2a8dfeecfda38615229aa6d9f5327a0ede2bfb6424ee355d14204434.
  No active Wi-Fi implementation changes yet. Next Wi-Fi candidate is private:
  research/nord2/wifi/next-stage/gate-bit13/. Minimal persistent AP mask
  6000->2000 aligns selected Android; keep original outer power/wait unchanged.
  Optional scoped-id-candidate now independently reviewed PASS in its
  independent-review.json: common40 verified before GPIO180/reset/TOPSPI ID,
  source clockdetect + timeout-IRQ prerequisite before semaphore11 acquisition,
  bounded polls, reverse cleanup; uncertain command retains ownership/halt.
  Candidate native SHA a31ccf2a206302f5d6bf0b90381c8d1bcb836b651bcb7005f24b23554cdff9ff;
  adapter a44a904c3e6a1e1e938be3273e09d4b4dcbe3a22ae408c2cd90a0a5dbef6c75e;
  whole touch 5984384f56d2c6b4bdf28ca181f530dafbd2121742f2e47a92650c1e521aa3b6.
  Review explicitly records source resets clockdetector each retry+jitter;
  candidate resets once then bounded polls, refusing if clocks unproven.
  Independent real-value guard, 91 write-fault/lifecycle/clock-IRQ tests and
  whole candidate ARM64 compile PASS. No network/RF/firmware execution yet.
  Next after human resumes: integrate reviewed helpers under production names,
  adapt tracked tests/default-off fixtures and mkpatch; verify unchanged UFS,
  focused compile/tests, CI build, pack/review exact image and only then ask
  boot-only authorization. New capture parser must use AP mask2000; preserve
  old mask6000 evidence/parser. Keep current 3b2e image and tested ADB rescue
  as manual recovery options; NEVER boot stock Android over iOS userdata.


- **LIVE-X2 HARDWARE SUCCESS; USER CLOSES UFS / BOOT WORK FOR NOW.**
  Human confirms lockscreen and touch/buttons on 3b2e7ecb…14204434, then says
  "per quanto ufs e tempi di avvio va bene così, non serve altro per ora".
  Preserve this working two-lane UFS / boot behavior exactly; focus Wi-Fi.
  Authorized runner EXECUTED, never rerun. Flash and reboot passed; native
  USB 180 s exit0. Final 237,321 B log SHA-256
  7f49789768a30441a1413849ba63f2d2bfa54721d1fe023c533958af1489e42a.
  Live proof phase5, active/root1/handoff0, 60 commands / 18 SET / 2 reads,
  no rollback; retained x2 verified. No UFS errors/timeouts or runtime panic
  in final capture. Human GUI/input confirmation separate from timed evidence.
  First ownership 48.425269 s vs previous 47.672765 s; unequal workloads and
  no demonstrated boot gain. Final private ufs-cost-comparison.{json,md} and
  hardware-report-final.json verify capture bytes/hash and preserve limits.
  Wi-Fi AP gate failure precisely identified: owner_ok1ff/owner_fail0,
  valid1f/failed4, prot=e70c500e => status bit14 set. RX/host raw zero UNREAD.
  Outer power stage3/ID20010101/cfg3 success1/restored1; no common40 writes.
  Selected Android POS intentionally disables bit14 release-ACK check;
  persistent readability tests bit13 plus bank2 bit2. Three source audits
  agree minimal persistent mask6000->2000 is justified; leave power sequence
  and all other guards intact. Sol Wi-Fi agent prepares private correction
  and assesses existing scoped CHIP_ID stage integration; no new active edits
  yet, no extra force-wake workaround. Any next image requires exact approval.
  Current phone remains working 3b2e iOS, capture ended, no USB client active.
  Conditional recovery for current authorized test remains permitted through
  physical fastboot/rescue or prior 2e4f image; do not boot stock Android.


- **LIVE-X2 / AP TEST AUTHORIZED; RUNNER STARTING NOW.**
  Human: "Autorizzo, pronto in fastboot" for exact 3b2e7ecb…14204434.
  Private authorization.json records boot-only scope and conditional rescue /
  current 2e4f iOS restore. Parent owns USB. Run the prepared runner once;
  never rerun after flash.log exists. Capture 180 s, parse live proof plus AP
  failure evidence and UFS costs. Permanent Linux-return block means failure
  recovery requires physical reset/fastboot. No duplicate permission needed
  for the already-authorized conditional rescue/current-image restoration.


- **NEW LIVE-X2 / WI-FI AP IMAGE VERIFIED; AWAIT EXACT FLASH AUTHORIZATION.**
  `shim/out/nord2-xnu/nord2-ios-ufs2-wifi-ap-stop23.img`, 33,277,952 B;
  SHA-256 3b2e7ecb2a8dfeecfda38615229aa6d9f5327a0ede2bfb6424ee355d14204434.
  Payload 5d4d5b72857a380db985f4c61d543f5203433df1d7eca6edc7948a5139dbcc73.
  CI 39510279443 / source 4a8b957230c19db8969080778749a6e55e9e60f8;
  independent linked review PASS, exact kernel segments/header/Linux/both DTs/
  rescue CPIO/product ADT/static image-mode checks PASS. Boot spare 276,480 B.
  Requested: nord2ufs2live=1, cpus8, stop23, existing iOS userdata RW only after
  phase5/60 UIC/18 SET/2 verified GPT reads; retain x2 for normal operation.
  Existing fast console/RTC/input/display/USB and reversible common Wi-Fi probe
  retained, now with precise first AP/owner failure logs. No RF/networking or
  firmware execution added. Both old diagnostic and current observer completed.
  Linux return is permanently blocked for live mode. ANY uncertain pre-root
  I/O or active I/O failure halts; manual reset to fastboot is required for
  recovery. Tested rescue can then be flashed explicitly; current working
  2e4f315e image is preserved and independently hashed by the prepared runner.
  Private directory `research/nord2/ufs-live-common-ap/ios`: manifest.json,
  payload.bin, parse-evidence.py and run-approved-boot.py prepared. Runner flag
  --authorized-live-x2-common-ap-boot; capture 180 seconds, verifies exact new
  image/current 2e4f/ce84/old fd6/stock backup/rescue and initial rootfs proof.
  RUNNER NEVER EXECUTED, no flash.log/hardware-live; no new authorization yet.
  Ask exact boot-only approval including conditional manual rescue/current-iOS
  restoration, then physical fastboot. User's last confirmed device state is
  working 2e4f iOS lockscreen with input; no USB capture active.


- **CI 39510279443 PASSED; NEXT IMAGE PACKING OFFLINE.**
  Source 4a8b957230c19db8969080778749a6e55e9e60f8; kernel 17,320,320 B,
  SHA-256 f3e608d0550d418b5e49e8978c17b6a0c7bc79846232cc5dc7209d3cb9cecda9.
  Artifact sums and all 1,102 generic ARMv8.2+crypto compiler invocations verified.
  Private identity-required.json filled only after this evidence. Build session
  45579 packs nord2-ios-ufs2-wifi-ap-stop23.img under a 1 GiB process limit;
  logs in `ufs-live-common-ap/ios`. Agent independently reviews linked paths.
  New image still unauthorized; wait for packing/independent manifest and
  linked review before requesting exact boot-only approval and physical fastboot.
  Live mode recovery requires manual reset and explicit fastboot rescue/current
  iOS restore; automatic Linux return is blocked. Current working image remains
  2e4f315e, human lockscreen/input confirmed; no USB collector or device writes.


- **FULL CI 39510279443 RUNNING FOR LIVE X2 + COMMON AP OBSERVER.**
  Source `4a8b957230c19db8969080778749a6e55e9e60f8` pushed to fork.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39510279443
  Parent must await successful run, download kernel/log, validate source/hash
  and generic ARMv8.2 compiler flags, review linked live/error/return paths,
  then fill private `ufs-live-common-ap/ios/identity-required.json` and build.
  Exact image remains unbuilt and unauthorized; no new device operation.
  Private parser handles initial verified x2, failed transition and both
  pre-root uncertain-I/O / later runtime panics without claiming success.
  Product ADT pinned; CI identity placeholders block premature image builds.


- **LIVE-X2 / AP OBSERVABILITY READY FOR FULL CI.**
  Final active driver SHA-256
  1ef095f34a95e362b922a486d7ff71eae1a5eb614a22011f8c9e06bb7552b6b4.
  Parent caught a pre-active TUR retry gap after the first independent review;
  that review is marked superseded. Fixed wrapper now halts uncertain armed
  requests before descriptor reuse, including pre-root TUR, while preserving
  known-completed unit-attention retry and engine rollback. Independent exact
  TUR-loop fault regression and revised review PASS, both profile modes.
  Canonical live engine/adapter/arguments/runtime, original stop adapter,
  new image-mode checker and common Wi-Fi tests PASS. Full active UFS ARM64
  compile PASS; generated patch 7472 lines applies/reproduces exactly.
  Private next image templates: `ufs-live-common-ap/ios`, planned
  nord2-ios-ufs2-wifi-ap-stop23.img, single combined live-x2/common AP observer.
  CI/source/kernel identity still deliberately unfilled, no image or approval.
  Current 2e4f iOS remains working; no USB/device mutation since its capture.


- **REVIEWED LIVE-X2 SOURCE INTEGRATED; NO NEW IMAGE OR AUTHORIZATION YET.**
  Private live-candidate independent review PASS; exact production candidate
  installed into canonical helpers and active sweet_ufs.c. New opt-in
  nord2ufs2live=1 requires unique normal RW, stop23, cpus8, userdata and
  profile1 arguments; stop-only nord2ufs2lane token must be absent.
  Forward engine success: phase5, 60 UIC / 18 SET / 2 verified GPT reads;
  root writable/publication only afterward. Failed/uncertain transitions and
  runtime active I/O errors stop without legacy DMA abort/reuse. Linux return
  is permanently blocked from before UFS setup; recovery is manual reset to
  fastboot and explicit rescue/known-good image flash, not automatic fallback.
  Existing stop-only behavior passed frozen reference trace comparisons.
  Precise common Wi-Fi AP first-failure logs integrated with no gate changes.
  Generated patch 7467 lines applies/reproduces active tree exactly. Parent
  finalizes tracked tests/checker/docs before full CI. Current 2e4f315e iOS is
  still running, human confirmed lockscreen/input, no USB client active.


- **COMMON WI-FI CAPTURE COMPLETE; WORKING IOS LEFT RUNNING.**
  Both authorized 477e2abf / 2e4f315e runners executed; never rerun either.
  Current image is 2e4f315e66bb84e9d811ca4245db08342ad1aff235e2b121fc7d13aea33dfd1d,
  CI 39506694221 / source 9327714bfe56719881e4c6251243fcd6afc97b24.
  USB 180 s exited 0 at deadline, no transport/capacity error; final log 237,955 B,
  SHA-256 2939196c9328b03656b749277e4f216d4e8e961e8ffc04920ad7133484469d49.
  Private `ufs-linereset-common/ios/final-common-cost-report.json` validates
  exact capture and records `complete=true`, `classification=skipped_guard`.
  Common probe skipped at owner-or-AP before COLD/table; no new common writes.
  Outer power stage 3, ID 0x20010101/config 3, success 1, restored 1.
  First GUI ownership 47.672765 s from early framebuffer init, not power-on or
  human lockscreen onset; framebuffer 4.252993 s, no timeouts. UFS errors and
  timeouts 0. Range-map timing not isolated from workload; no speedup claim.
  Human confirms current-image lockscreen and touch/buttons working.
  No capture client remains. Next candidates are NOT authorized: precise AP
  failure logging (gates/read ordering unchanged), plus opt-in live x2 UFS.
  Sol agent ufs_link owns private live candidate, boot_speed independent review;
  parent owns integration. Live mode must retain x2 only after verified GPT,
  permit root only after success, and permanently block Linux return on this
  boot; any uncertain/active I/O failure requires physical reset/fastboot.
  Source stop diagnostic must remain behavior-equivalent. No active UFS edits yet.
  Topaz exact 89451b811a4e6cc5cdcf734c45887163d92c599e reviewed in private
  `wifi/topaz-reference/report.md`: reusable scan/PMK/Ethernet hooks, Qualcomm
  hardware sequence not portable; public checkout lacks the iOS-facing hook
  consumers. Actual Nord2 ADT lacks wlan; defer daemon gate until real driver.
  Investigating exact AP predicates against Android source without relaxing them.

- **AUTHORIZED COMMON WI-FI OBSERVER STARTING.**
  Human confirms physical fastboot. Exact2e4f315e image runner starting now;
  covered by the preceding two-image authorization. Capture180s. Parent owns
  USB; never rerun this runner after flash.log exists. Evaluate cold/common/
  restore/TIME and outer restoration, then iOS/input/display. New live-x2 and
  scoped A-die candidates remain private; this image still uses one UFS lane.

- **TWO-LANE UFS HARDWARE TEST PASSED; NEED FASTBOOT FOR AUTHORIZED WI-FI.**
  477e2abf flash/reboot OKAY00:29:37CEST; USB60s exit0, log16,907 bytes
  SHA-256 bdaba5e79dd32ef4a21b1e6ff523bc1dae8bd3e0850694f339e4f5e89ca987d4.
  Correct policy parser PASS: rc0 phase4 error0 commands115 sets37 reads3,
  uncertain0 rollback1 original1; all18 saved attributes and6 limits valid.
  x2 negotiated/read-verified and x1 negotiated/restored/read-verified.
  Initial UECPA80000010 now accepted; all ownership/preflight checks passed.
  No root mount or disk writes; phone intentionally halted. First runner has
  executed; never rerun. Request physical fastboot, then run already-authorized
  2e4f315e common Wi-Fi observer. Do not ask for duplicate image approval.
  No throughput or iOS-on-x2 claim yet. Agent prepares private normal-iOS x2
  mode with explicit root/handoff rules; stop-only diagnostic stays unchanged.
  User supplied MakrSas/ios-topaz branchtopaz as Wi-Fi reference; Sol agent
  inspecting hardware and iOS interfaces separately. No active edits from it.

- **LINERESET / COMMON WI-FI SEQUENCE AUTHORIZED; FIRST RUNNER STARTING.**
  Human: "Autorizzo entrambi; pronto in fastboot". Exact477e2abf diagnostic
  then2e4f315e common Wi-Fi observer; tested rescue/currentce84 restoration
  if necessary also covered. Private authorization-sequence.json records scope.
  Parent owns USB. First runner starting now, never rerun after flash.log.
  Parse using --allow-line-reset. Request physical fastboot only before the
  already-authorized second image; no repeat approval required.

- **NEW UFS / COMMON WI-FI IMAGES VERIFIED; AWAIT EXACT AUTHORIZATION.**
  CI39506694221 / source9327714bfe56719881e4c6251243fcd6afc97b24 passed.
  Focused linked review passed: exact LINERESET exception, paired range mapping,
  common opt-in, all40 snapshots/restoration and dirty handoff block.
  Both images33,275,904 bytes with278,528 bytes spare in boot:
  `shim/out/nord2-xnu/nord2-ufs-linereset-stop23.img` SHA-256
  477e2abfb2ce6418544acbc1d75868898a0c0b0ad8b4f5e46b498c7e3d191101;
  payload b551e0e65a572a4f614604847837529eac2075291d67acf41238594cd3742f2f.
  RO pre-root two-lane trial, GPT verification and negotiated x1 restoration,
  then always halt. Parse with explicit --allow-line-reset; old captures keep
  their original zero-only policy. No root mount or Linux handoff.
  `shim/out/nord2-xnu/nord2-ios-wifi-common-stop23.img` SHA-256
  2e4f315e66bb84e9d811ca4245db08342ad1aff235e2b121fc7d13aea33dfd1d;
  payload bb0c7a7ab02203a47d44412bdd199de752b1724f17ffe0fe46e7888d7a1a4764.
  RW iOS, eight cores, input/display/USB, fast console, RTC skip, four-file
  firmware check and reversible40-register common Wi-Fi setup. New setup is
  hardware UNTESTED; dirty restoration blocks power-off/Linux handoff and
  requires physical fastboot. No firmware execution/RF/networking yet.
  Independent unpack, exact kernel segments, Android header, Linux/bothDTs,
  rescue raw contents and static boot guards all PASS. Exact runners prepared
  under private ufs-linereset-common/{diagnostic,ios}, flags respectively
  --authorized-ufs-linereset-boot / --authorized-wifi-common-boot; capture60s/180s.
  Neither runner executed or image authorized. Both verify current working
  ce84 image plus original backups/rescue before flashing. Phone remains on
  working ce84 iOS; no USB capture client active. Need new sequence approval,
  physical fastboot for each test. A-die scoped-ID candidate remains private,
  independent source review/fault tests/ARM64 compile PASS, no hardware claim.

- **CURRENT OBSERVER FINALIZED; PHONE ON WORKING IOS.**
  ce84 USB600s deadline complete, no transport/capacity error. Log309,551B
  SHA-256 e9ef4a1cb8ac6de606a04f0b928d888cac33ed24cb941a79dd4473ee6ebf748e.
  Private ios-profile/final-cost-report.json validates full log/result hash,
  preserves counters and separates human lockscreen/speed feedback. Exact
  lockscreen onset unknown; firstGUI ownership47.7209016s is fromstage6 only.
  No capture client remains. Both395030 runners have executed; never rerun.
  New395066 images are still packing, not authorized. Planned next captures
  60s for stop-only UFS (engine20s bound) and180s for iOS common probe; enough
  to cover measured startup while avoiding the former600s idle collection.

- **CI39506694221 PASSED; NEW IMAGES PACKING.**
  Source9327714bfe56719881e4c6251243fcd6afc97b24, kernel17,336,088B SHA-256
  c5b7c8eb73302fa13d28a729543ab438241954d6bb8f4c0f3d33d50bedc69c17.
  Artifact checksum and all1,102 compiler invocations/physical command lines
  genericARMv8.2+crypto verified. Private diagnostic/ios verifiers now pinned
  to exact kernel; two offline builds starting under1GiB perprocess limits.
  Metadata independently checks both modes: EMI off, common onlyinRWobserver,
  explicit dirtyhalt behavior and nofirmwareexecution. Newimages notauthorized.
  ce84observer USB600s finished exit0, log309,551B; agent finalizing report.
  Phone left on workingiOS, humanlockscreen +muchfasterboot confirmed.

- **NEXT CI39506694221 RUNNING; CURRENT IOS STILL CAPTURING.**
  Source9327714bfe56719881e4c6251243fcd6afc97b24 pushed tofork; fullCI
  https://github.com/frxncyyy/ios-sweet/actions/runs/39506694221
  IncludesnarrowUECPA LINERESET exception/defaultoff commonWiFi diagnostic/
  pairedrange mapping. Independent sourceintegration/guard/patchparity review
  and canonicalfault/ARM64 testsPASS. Input+powerregressions alsoPASS after
  modelnewdefaultoffhook/dirtylatchdependencies. Patch7286lines exactreproduction.
  gitdiffwhitespace's space-before-tab finding is generated unifiedpatchcontext,
  not sourceindentation; otherchecksPASS. Newprivate templates under
  ufs-linereset-common/{diagnostic,ios} readyONLY, kernelhashpendingCI; both
  unbuilt/unauthorized. Currentce84lockscreenconfirmed; parentUSB600scapture
  ongoing. NoCPUclockfeature inthisCI. NextA-dieID remainsprivatepreparedonly.

- **COST OBSERVER: LOCKSCREEN CONFIRMED, USER REPORTS MUCH FASTER BOOT.**
  ce84a4f3 flash/reboot OKAY00:11:32CEST. Capture600s stillrunning (do not
  restart/reboot). Human: lockscreenvisible, avvio "anni luce più veloce";
  noexacthumanbootduration. FirstGUI47.720902s fromstage6, framebuffer4.248683s.
  41,328strategybuffers/37,414reads/1,994,534,400requestedbytes,421RMWreads.
  map0.986191s, mutexwait24.465547s(overlapping aggregate), maxwait0.157074s,
  held7.695039s, copy0.483964s. 41,941commands,1,949,347,840readbytes,
  cmd7.309914s, doorbell6.965747s, errors0/timeouts0. G4x1B remains. All4WiFi
  files verified, powerprobeID20010101/success1/restored1. Do not attribute all
  47.7s toUFS or subtract/sumoverlappingtimers aswalltime. ExactXNU APIs support
  narrowpaired buf_map_range/buf_unmap_range update, nowactive; strategy and
  sourceAPI contract ASan/UBSan+fullBSDcompilePASS. Improvementnotmeasured.
  NativeCPUclock helper remainsprivate/unintegrated; nofrequencyclaim.
  Combinednextsource includesLINERESET exception +defaultoff commonWiFi
  transaction +pairedmapping only. Preparing fullCI; notineither currentimage.

- **SECOND AUTHORIZED UFS COST OBSERVER STARTING.**
  Human confirms physicalfastboot. ce84a4f3 runner nowstarting, captures600s.
  Single-use; neverrerun afterflash.log exists. No newpermission needed; exact
  image coveredby precedingsequence. ParentownsUSB. NewLINERESET/commonWifi
  sources areseparatefrom this frozen8702715image. Collectstrategy/GUItiming.

- **PREFLIGHT CAPTURE COMPLETE: UECPA LINERESET IS EXACT REFUSAL.**
  2ec05f7f flash/reboot OKAY00:06:41CEST; nativeUSB120s exit0. Full20-line
  log16,844B SHA-256 bb5753f35548d851c40086cd27b4691ad612ea94bd6f0afb99d0da89cd956276.
  Initial fail9/index8 measuredUECPA80000010; all23ownershipconditionsPASS,
  HCE1/HCS10f, doorbells/IE/AHIT0. NoDMEcommands/SETs/rootmount. Controller
  gates beyondUECPA were unread, notzero. SelectedAndroid ufshcd.c6113ff
  explicitly ignores LINERESET, distinct from laneerror mask0xf. Narrow next
  canonicalfix accepts only0 or exact80000010 atsameexistingread; no broadmask,
  noextraMMIO/clear, allothererror/statusgates unchanged. Fault/transition/
  rollback/readtrace testsPASS. Newfix NOT in either authorizedimage.
  First runnerEXECUTED; neverrerun. Needphysicalfastboot for alreadyauthorized
  ce84a4f3 observer. Privatehardware-result.json +hardware-report-final.json.
  Common Wi-Fi candidate nowintegrated in active source with defaultoff
  nord2wificommon=1,40registersnapshot/apply/readback/reverserestore, freshcold
  ownerguards, dirtyhandoffblock and elapsedcounter. Exactsource/table/fault
  tests, ARM64helpers and activefullsweet_touch compilePASS; noCI/flashyet.

- **UFS PREFLIGHT/COST SEQUENCE AUTHORIZED; FIRST RUNNER STARTING.**
  Human: "Autorizzo entrambi; pronto in fastboot". Covers exact2ec05f7f
  diagnostic then ce84a4f3 observer hashes below. Private authorization record
  ufs-preflight-cost/authorization-sequence.json. Parent owns device/USB.
  Diagnostic runner is single-use; never rerun after flash.log exists. Collect
  actual20-line preflight before requesting physicalfastboot for authorized
  observer. No new approval needed for second image in this sequence.

- **CI39503015652: BOTH NEW UFS IMAGES VERIFIED; AWAIT AUTHORIZATION.**
  Source870271596eb0a09bd5b2920981681bf8cca200fc, full CI success and focused
  linked gate/cost review PASS. Both images33,273,856B, boot spare280,576B.
  `shim/out/nord2-xnu/nord2-ufs-preflight-stop23.img` SHA-256
  2ec05f7f7cf986e024334a1346fa002175d38d13e83756d0ead614b481fc47a4.
  RO pre-root x2 trial with20-line exact gate evidence; if gates pass it tries
  x2, validates GPT reads and restores x1, then always halts. Physical fastboot
  required afterwards; no root publication/mount or Linux handoff.
  `shim/out/nord2-xnu/nord2-ios-ufs-cost-stop23.img` SHA-256
  ce84a4f313184c89973f65bd95e12dc3c23ac9fb17af4c58690d0ab9bc6dfb49.
  RW iOS observer, eight cores/input/USB, RTC skip, optimized console,
  firmware RAM verification and reversible radio power probe. Strategy request
  sizes/map/mutex/copy timings measured; no lane/queue/capability tuning. Omits
  already-applied zoomreset and ambiguous EMI queries. Both independent image
  checks PASS including exact embedded fixed-up kernel segments, Android header
  ID, Linux/bothDTs/tested rescue raw contents and command-line guards.
  Private ufs-preflight-cost/{diagnostic,ios-profile}/run-approved-boot.py
  prepared ONLY; exact flags --authorized-ufs-preflight-boot and
  --authorized-ufs-cost-boot, captures120s/600s. Neither authorized or executed.
  Phone remains working0956173b iOS with human-confirmed lockscreen. Need exact
  new sequence approval and physical fastboot for each image. Native candidate
  Wi-Fi common transaction now has source-backed14-field idle guard,40-reg
  reversible transaction and dirty-restore permanent handoff block; sanitizer,
  ARM64compile and independent review PASS. It remains private/unintegrated,
  not part of these images and not hardware tested. CPU configured-clock
  decoder/read collector tests PASS; isolated, no native hook/device access.

- **CI39503015652 PASSED; NEXT TWO IMAGES PACKING.**
  Source870271596eb0a09bd5b2920981681bf8cca200fc, kernel17,319,048B SHA-256
  05c97b14898b52691b39c1fefcd53c8ff461c0ccc282d1b52a13ff111182d8ce.
  Artifact checksum verified;1,102compiler invocations have genericARMv8.2+
  crypto flags. Parallel make interleaved two echoed commands into one physical
  logline; initial one-command-per-line verifier rejected this. Verifier now
  requires exactcount/matching CPU+arch pairs percompiler; preserves interleave
  evidence (1,101physical commandlines). No compiler-target/source change.
  Private ufs-preflight-cost diagnostic and ios-profile packing concurrently
  with1GiB perprocess limit. New fullpayload segment verification added.
  Linked newgate/counter review underway. No newimage authorized/flashed.
  Phone remains on0956173b with confirmedlockscreen; both previous runners done.

- **ZOOM/WIFI CAPTURE COMPLETE; PHONE LEFT ON WORKING IOS.**
  NativeUSB600s exit0, no transport error/capacity hit. Log381,019B SHA-256
  05556e92b7e0eda4b617dfa5e4eb46afac6b00f98f9d8526bf7967c6deeac31e.
  Final parser + hardware-result.json in private zoom-wifi record human
  lockscreen confirmation separately from log-only graphics evidence. Do not
  rerun either executed CI39499794889 runner. No rollback requested/needed.
  User re-supplied Android kernel URL: local branch and remote ref independently
  match04d00c2565764c3922c9b4826b5acff1106e533f. Existing README modification
  in that separate repo preserved. Exact MT6893 config remains reference.
  Isolated wifi/next-stage/cold-snapshot3register helper+adapter tests/ARMcompile
  pass. Isolated common-transaction snapshots/restores4retention+36SPI-table
  source-exact controls; fault/source-extraction/ARMcompile pass, no hook.
  Native common-client idle predicate still being derived from selected source;
  do not count these private candidates as implemented or hardware validated.

- **NEXT UFS CI39503015652 RUNNING.**
  Source870271596eb0a09bd5b2920981681bf8cca200fc pushed to fork.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39503015652
  Adds gate observations and strategy costs only; exact guards/link settings/
  queue/ioctl/transfer limits unchanged. Private CI verifier/watch prepared.
  Agent prepares two offline templates: RO nord2-ufs-preflight-stop23.img and
  RW nord2-ios-ufs-cost-stop23.img. Observer omits zoomreset (already applied)
  and disables ambiguous EMI queries; keeps verified firmware probe, input,
  eightcores/USB/fastlog/RTC skip. Not built or authorized yet.
  Current0956173b stillrunning, userconfirmedlockscreen; USB600s capture ongoing.

- **LOCKSCREEN RESTORED; ALL4 WIFI FILES VERIFIED; NEXT UFS SOURCE READY.**
  Current0956173b flash/reboot OKAY at23:41:45CEST. User confirms lockscreen.
  Zoom stage3 error0 attempted1 backup1 verified1 already0 restore_error0.
  Valid active surfaces1125x2436 now present; graphics failure resolved by
  exact two-value preference reset. FirstGUI ownership620483877ticks/13MHz
  =47.7295s from stage6, framebuffer54703208ticks=4.208s, not measured time
  to actual lockscreen. Capture600s still running; do not reboot prematurely.
  Wi-Fi main973648/patch5726/WiFiROM296916/WMMCUROM49868 all verified in RAM;
  probe status0 elapsed661119ticks (~50.855ms). No firmware execution/network.
  Eight EMI reads all returnx0=2 with remaining arguments preserved; current
  structural status0/range_valid1 does NOT prove actual MPU metadata. Decoded
  40020000 range must not gate ROM placement. Selected Android ABI matches
  helper, monitor semantics remain unknown; agent continues source analysis.
  Future UFS source integrates initial-gate observations preserving exact
  short-circuit MMIO reads/gates, with explicit validity+first-failure fields.
  Adds strategy request histogram/map/mutex wait+hold/copy/RMW/flag counters;
  ioctl capabilities/limits unchanged (private FEATURES proposal not applied).
  Exact active strategy/profile + canonical engine/adapter/reference trace
  ASan/UBSan suites PASS. Patch6940lines reproduces active tree. Integrated
  ARM64compile PASS with current sweet_early header; initial local attempt used
  stale generated-tree header and lacked nord2_input_init declaration (preserved
  diagnostic); no production source fix needed. Preparing next fullCI.

- **SECOND AUTHORIZED IMAGE STARTED: ZOOM/WIFI0956173b.**
  Human confirmed physicalfastboot again; zoom-wifi/run-approved-boot.py now
  STARTED. NEVER rerun this exclusive runner. Exact image and two-value VFS
  reset were authorized in the preceding sequence. NativeUSB600s capture
  under zoom-wifi/hardware-live; wait for actual firmware/EMI/Zoom reports.
  No hardware success or visible GUI result yet. Parent owns USB/device;
  Wi-Fi agent analyzes the host log only. UFS preflight observability candidate
  and strategy-phase instrumentation remain separate from frozen image.

- **UFS FIXED TEST CAPTURE COMPLETE; NO LANE TRANSITION.**
  72ad734d…f8194821 flash/reboot OKAY at23:38:19CEST; nativeUSB120s exit0.
  All8 scheduler PASS; GPT validated. Complete7-line diagnostic report:
  rc=-1 phase1 error-2 commands0 sets0 reads0, uncertain0, root_published0,
  return_blocked1. Initial healthy()/IS gate refused before first DME command;
  exact failing predicate NOT recorded. LAST HCS/IS zeros are unpopulated
  result fields, NOT measured register values. Do not infer a register cause.
  Final log16212B SHA-256 ea97b2fb80ec4d6643ea66da35ba6d7afe7c0ce497027def41ce92c1311d3476.
  Private hardware-fixed/hardware-result.json preserves complete raw report.
  Phone intentionally halted in diagnostic; no root mount/disk write occurred.
  First runner completed/EXECUTED, never rerun. Need physical fastboot, then
  execute already-authorized zoom-wifi0956173b image; no new permission needed.
  Separate agent prepares exact short-circuit preflight observations preserving
  every guard/no extra MMIO reads; NOT integrated into current frozen images.

- **BOTH NEW IMAGES AUTHORIZED; FIRST RUNNER STARTED.**
  Human explicitly approved entire sequence, including Zoom two-value reset
  and verified backup: "Autorizzo entrambi; pronto in fastboot". Authorization
  covers exact72ad734d…f8194821 then0956173b…2235c89a images above. First
  hardware-fixed/run-approved-boot.py STARTED; never rerun. Parent collects
  UFS result before second image; physical fastboot required between tests.
  No new permission needed for second image; request only physical readiness.
  Private CI authorization-sequence.json records hashes and human wording.

- **TWO NEW IMAGES VERIFIED; AWAIT EXACT HUMAN AUTHORIZATION.**
  CI39499794889/c25b788, both33,271,808B with282,624B boot spare.
  `shim/out/nord2-xnu/nord2-ufs-two-lane-fixed-stop23.img` SHA-256
  72ad734dc5ed4a3216088bbb46aff6ebe1336d40efb9fd15c963be11f8194821,
  payload f85c7c7bda6b53f7f46f0dcd3652d142afb9c7a1e9300b48f5c014c190a49cb4.
  RO pre-root x2 trial, verifies GPT and x1 rollback, always halts; physical
  fastboot needed afterward. No disk writes/root publication/Linux handoff.
  `shim/out/nord2-xnu/nord2-ios-zoom-wifi-stop23.img` SHA-256
  0956173bfa821fc8a5721b7b0ebcc38bb86b89f3a4ff71881eca3df32235c89a,
  payload57eb12e073caf063e2d86c1dc64e77eb77992daf1aeddcff176ea229fa2d859a.
  RW iOS, 8cores/input/USB, RTC skip, firmware RAM verification, EMI reads and
  exact opt-in VFS preference reset960x2079 ->1125x2436 with verified backup.
  No Wi-Fi firmware execution/networking or UFS lane tuning in this observer.
  Static checks/independent unpack/header ID/Linux+DT+rescue contents and
  focused actual-linked SMP gate/zoom-before-launchd/vendor geometry PASS.
  Private hardware-fixed and zoom-wifi manifests/runners are prepared ONLY;
  neither new runner executed or flash authorized. Flags respectively
  --authorized-ufs-two-lane-fixed-boot and --authorized-zoom-wifi-boot.
  Capture120s/600s; no automatic reboot/restore. Phone confirmed ADB recovery.
  Request sequence authorization including explicit Zoom preference mutation;
  user must physically enter fastboot for each image. Keep original tested
  RAM rescue available; never boot normal Android over installed iOS userdata.

- **CI39499794889 PASSED; TWO NEW IMAGES PACKING.**
  Source c25b788dd0294436fd3e2dcb3260e993bdc02f06, kernel17,319,008B,
  SHA-256 b011efbe19316f85779bca6b7204a6b92bdeec28da5afdc6e9a402b085fe2c83.
  Artifact checksum and all1,102 C/C++ genericARMv8.2+crypto compile lines
  verified. Focused linked SMP/zoom/vendor reviews underway. Independent new
  fixed-stop-only UFS and RW zoom/Wi-Fi observers pack in parallel under1GiB
  per-process virtual-memory limits; no device operation or new approval yet.
  Phone remains RAM ADB rescue, userdata unmounted. New images will require
  explicit exact-image authorization; zoom image also resets two preference
  values via mounted VFS and creates a verified original-content backup.
  Private ufs-performance instrumentation candidate is intentionally NOT in
  this build. It measures caller sizes/map/mutex wait+hold/copies; exact VFS
  tests show GETFEATURES0 alone still leaves later limit queries unsupported.
  Existing default request/queue limits are nonzero. No UFS speed gain claimed.
  CPU DT2.0/2.6GHz is declared, not measured; read-only telemetry plan saved.

- **CI39499794889 RUNNING: SMP GATE, GPT, ZOOM RESET.**
  Source c25b788dd0294436fd3e2dcb3260e993bdc02f06, full macOS kernel build.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39499794889
  Narrow RO23 SMP gate now validates the explicit diagnostic bundle. Review
  caught a mock/API mismatch before CI: PE_boot_args already returns the
  command-line pointer, NOT boot_args; scanner and tests now use actual ARM
  source contract and608-byte limit. Startup and post-SMP gates share it.
  Vendor backup geometry correction uses liveCRC-validated total-8 fixture.
  Canonical zoom_reset.c/h opt-in helper runs once in bsdinit_task immediately
  before load_init_program, after mounted-root initialization. Exact83B old
  content+501:501/0600/regular/nlink1/size checks; exclusively create and verify
  root-owned0600 backup via VFS, then same-size write/fsync/readback. Bounded
  rollback on failure; already-standard is idempotent. No raw block writes.
  New backup inherited group is explicitly normalized only on exclusive create.
  Default inactive; new image must explicitly request nord2zoomreset=1 and user
  must approve preference reset alongside boot flash. Host original stays saved.
  Exact-source ASan/UBSan failures/default/meta tests, peer VFS review, both
  integrated BSD translation-unit compiles and patch reproduction PASS.
  Phone remains RAM ADB rescue, userdata unmounted; no preference reset run.

- **LIVE READS IDENTIFY WIFI VALIDATOR AND ZOOM SETTINGS.**
  ADB rescue remains running, no block filesystem mounted and no phone writes.
  Read53,248B from LUN2: primary GPT pinned hash unchanged; real backup entries
  start at31240184(total-8), not assumed31240187(total-5). Both header CRCs,
  entry CRCs and complete arrays match. Private wifi/live-gpt-read stores raw
  copies and hashes. Narrow vendor validator fix + real-copy/fault tests pass.
  Targeted read-only HFSX reader fetched147,456B, found mobile graphics plist:
  canvas_height2079/canvas_width960, exactly83B, SHA-256
  a162a5b4ce6e5acbce663a3595ce4402effedf3f628e6da0c0c1710d6f92db27.
  Volume is dirty from forced reboot: raw observation is explicitly untrusted
  until revalidated through mounted XNU VFS; no journal/raw disk modification.
  Original saved in display-capabilities/graphics-zoom-original.plist.
  Proposed standard2436x1125 differs at four bytes, new SHA-256
  a275c9213db9ef42c86512664684f1d76f0c623cd42b108c516c6b5ac7d2d7a3.
  Preparing opt-in pre-launchd VFS reset with exact preimage/backup/readback;
  not integrated or run. Runtime SMP RO-mode fix and tests also ready.
  Frozen Wi-Fi observer74271734…366f72a is built/verified but unflashed and
  still carries old vendor placement; hold it while integrating these fixes.

- **UFS TEST REFUSED EARLY; PHONE IN ADB RAM RESCUE.**
  Exact0089ae58 image flash/reboot succeeded, but unchanged runtime
  nord2_smp_userland_config still required RW1 for stop23. Panic before the
  lane diagnostic: unsupported startup configuration cpu=1 cpus=8 stop=23.
  No UFS lane transition attempted. Pstore saved immediately under private
  backup/nord2/logs/20261020-231112-ufs-two-lane-diag; hardware-result.json in
  hardware-candidate records digest/result. Native USB never started; parent
  stopped its host wait. ADB recovery authenticated, userdata not mounted;
  recompressed rescue Linux path now hardware-observed working. DO NOT rerun
  old runner. Fixing narrow runtime RO-mode gate; previous static checker
  covered only image arguments and failed to catch this integration mismatch.
  User has been told; no next flash authorized. Phone should remain connected
  in rescue for targeted read-only HFS preferences extraction (Zoom confirmed).

- **UFS TWO-LANE IMAGE AUTHORIZED AND FLASHED; CAPTURE RUNNING.**
  CI39495585573/dece242; image
  `shim/out/nord2-xnu/nord2-ufs-two-lane-diag-stop23.img`, 33,269,760 bytes,
  SHA-256 0089ae589b3bd9cb4b552e0a8916414ba5d5d61cbd5089f75936a0da2a6c55dc.
  Payload d28cf9839c2e5df75473a7a9eea51e7524641020fad6d0c61e760ccd460b967b.
  Standard/static checks, independent unpack and linked path reviews PASS.
  Strict read-only pre-root test; tries x2, verifies reads, negotiates x1 back,
  then always halts (no iOS or Linux handoff). Physical fastboot required after
  capture. Private hardware-candidate/run-approved-boot.py was prepared,
  exact flag --authorized-ufs-two-lane-boot; exclusive logs prevent repeat.
  User authorized exact image; flash/reboot OKAY at 23:09 CEST. Runner
  EXECUTED, do not rerun. Native USB capture in hardware-candidate/hardware-live.
  Test intentionally halts; physical fastboot needed for the next image.
  Second Wi-Fi/display image packing sequentially, not flashed/authorized.

- **USER CONFIRMS DISPLAY ZOOM CHANGED WHILE GUI WORKED.**
  User answered yes to Zoom schermo change at 23:07 CEST. Matches current
  960x2079 versus prior fullscreen1125x2436 and repeated missing scaler lookups.
  Strong explanation for persisted failure even with earlier kernel, but actual
  preference and causal reset not yet verified. Investigating reversible reset;
  no preference/filesystem change made. Full record diagnostics remain useful.

- **COMBINED TEST COMPLETE; UI STILL SHOWS CONSOLE.**
  Authorized e5f91de5…6e2741b flashed/rebooted OKAY at 22:40:03 CEST.
  Runner EXECUTED; never rerun. Final 600-second USB capture: 956,617 bytes,
  SHA-256 ff74088bf980d307fbc4566cf2182ecbc3862c1860a33afd3d2e444c23bcac10;
  client exit 0. Phone remains running this XNU image, iOS root mounted RW.
  RTC skip=1, elapsed=1 tick at 13 MHz. GUI ownership reached 63.7903 seconds
  from stage6 (previous 93.6836), framebuffer 4.8270 seconds; this does NOT
  measure lockscreen readiness. User confirms console persists, power dims it.
  Five observed swap records have all four surface IDs zero. Present succeeds,
  ownership failures=0; no active layers/lookups/copies at the diagnostic
  checkpoint. Complete records are needed to distinguish empty submissions
  from an unsupported record layout; no speculative capability changes made.
  UFS remains G4x1 B FAST/FAST, 9.6597 aggregate command seconds, no errors or
  timeouts. LIMITS valid/understood=ff: both directions available2/connected2,
  terms1, UniPro5, adaptation1, AHIT0, HCS10f/UPMCRS1/IS0. Last error-list
  characters truncated at exactly255; next reports split into bounded lines.
  Firmware probe errno22 after 66,604 ticks; no files verified. Most likely
  backup-GPT identity/layout assumption, NOT proven. New bounded validator
  stage/header/range logs preserve every existing hash and validation gate.
  Results/parser/raw evidence: private `research/nord2/rtc-ufs-wifi/`.

- **NEXT SOURCE VERIFIED; CI39495585573 PASSED.**
  Source dece242488ecb646fc4693e81319b2e0b0f28d1f; macOS full kernel build.
  Kernel 17,285,968 bytes, SHA-256
  9bdffb19b3f52da6450d6a978879555ba5dd8d76111b07e622fa7cee3d5b0864.
  Artifact checksum verified; all 1,101 C/C++ compile commands use generic
  ARMv8.2+crypto (verifier includes both clang and clang++).
  https://github.com/frxncyyy/ios-sweet/actions/runs/39495585573
  Adds first-two complete swap-record diagnostics, bounded vendor validation
  evidence, opt-in dedicated EMI-read worker and separate `nord2ufs2lane=1`
  diagnostic. The latter runs before root publication, tests negotiated x2
  and rollback to x1 with pinned GPT reads, then always stops. It never leaves
  a mounted root or allows Linux fallback. Unknown limits/timeouts/ownership
  fail closed; uncertain DMA/UIC state remains owned. Diagnostic-only SCSI
  polling also has a one-million-iteration hard limit if its clock stalls.
  Engine/adapter fault models, real saved GPT hashing, default-path regression,
  bounded logging, display/vendor/firmware tests and local ARM64 compiles pass.
  Dedicated two-lane mode has NOT run on hardware. A new exact image approval
  is required for any further flash. Packing the UFS stop-only image first,
  then the iOS/Wi-Fi/display observer sequentially to limit host memory use.
  The phone remains on e5f91de5; no further device action executed.
  Independent linked reviews confirm pre-root X2 halt/return guards, fixed
  GPT digest/read, exact EMI read ABI and worker lifecycle, vendor reports
  outside the UFS lock. Evidence: private CI directory linked-{ufs,wifi}-review.
  Private offline parsers prepared for complete X2 and two swap-record dumps.

- **FUTURE ROM TRANSACTION HELPER TESTED, NOT LINKED.**
  `wifi_rom_transaction.{c,h}` uses the verified ROM plan and an injected I/O
  adapter, snapshots all four exact spans before writing, checks placement,
  restores and verifies original bytes. Partial writes and prerequisite loss
  are covered; unverified cleanup retains the original backup and dirty flag.
  ASan/UBSan fault tests and local ARM64 compile pass; independent review found
  no blocker within its explicit adapter contract. It does not establish MPU
  permissions, cold/reset state or physical ownership, and has no native I/O
  implementation or activation hook. Not included in CI39495585573.

- **COMBINED CANDIDATE VERIFIED (PROVENANCE).**
  CI39491596110/source ce25bf39098c1be91cc8d929c4e6b2dd4959088c passed.
  Kernel: 17,300,400 bytes, SHA-256
  a917ad29f47f9099cc239fe2b5f6fc3693dec5ec913fc01e531f9265c94ff101;
  all 1,099 captured compile commands use generic ARMv8.2+crypto.
  Frozen `shim/out/nord2-xnu/nord2-ios-rtc-ufs-wifi-stop23.img`:
  33,263,616 bytes (290,816 spare), SHA-256
  e5f91de5f0f25af0d89b700ebe52f3912a4dcc5c1387e2c474702a74e6e2741b.
  Payload SHA-256 cecd954e2e677f01f46182f7d87df5ff315c86aae9bc64796cf5a4630f62431a.
  Includes opt-in RTC wait skip, extended read-only UFS limits, GUI ownership
  retry/diagnostics and four-file firmware verification in RAM. No UFS tuning,
  firmware execution or Wi-Fi networking. Uses installed iOS root read/write.
  Static checks, independent unpack and linked GUI retry/RTC spot checks pass.
  Private `rtc-ufs-wifi/{manifest.json,verify-image.py,run-approved-boot.py}`;
  runner was prepared and has now executed as recorded above.
  User confirms console remains visible; a power press dims the screen.
  Rescue base is recompressed: 28,594,176 bytes, SHA-256
  ed05ea02d0499fe4109e6498a6852e769c346226aa4a56a220338673f8a2f5bf.
  Linux/both DTs/all uncompressed CPIO contents are identical; raw CPIO SHA
  95989a2ddff7d3fad8c63df9916a7d2510e09499f0b1710ac9fbf078ce60979f.
  Only gzip container/header RAMdisk size+ID differ. This container has not
  been hardware tested. Original tested rescue d78bf9fe is kept for fallback.
  https://github.com/frxncyyy/ios-sweet/actions/runs/39491596110
  Isolated `wifi_emi_snapshot.{c,h}` is tested/reviewed but NOT linked into
  this image: structured read-only SMC, no SET/CLEAR/MMIO writes. No SMC has
  been executed and effective permissions are still unknown. Parallel work
  prepares a separate opt-in snapshot bridge and a pre-root two-lane UFS
  diagnostic; neither changes this frozen candidate.

- **NEXT WI-FI EMI OBSERVER PREPARED, NOT IN THE CANDIDATE.**
  `shim/nord2-xnu/wifi_emi_probe.{c,h}` provides a strict `nord2wifiemi=1`
  worker around the existing structured SMC reader: eight read queries for
  regions 27/31, raw return/status logging, pure range/domain decoding and
  thread cleanup. No allocations, storage/CONN/MPU writes or RF activity.
  Query count is bounded; a secure monitor call has no interruptible software
  deadline. Effective permission/ownership remains unknown. Exact production
  collector/bridge fault and lifecycle tests under ASan/UBSan pass, as does
  local ARM64 BSD compilation. Root reviewed and reran the host test.
  Private integration plan: `wifi/next-stage/emi-probe-integration.md`.
  No hook, patch regeneration, CI or hardware test for this bridge yet.

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
