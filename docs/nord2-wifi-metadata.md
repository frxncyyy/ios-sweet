# Nord 2 Wi-Fi identity and service prerequisites

Audit date: 2026-10-22. Baseline: source `938cb89`, executed image
`6e4ce860`, CI `39790703398`, iOS 16 userland from the selected D22 IPSW.
The bounded audit covers all 53 direct MobileGestalt calls and 15 direct
registry/matching calls in the selected `wifid`, its embedded WiFiManager,
the relevant MobileGestalt providers, and the deployed ADT/IOService sources.
It is not a claim about every daemon, indirect call or future association path.

Eight recovered wifid crash reports share the same Objective-C exception:
insertion of a nil cached `WifiAddressData` into the dictionary entry
`PRIVATE_MAC_ADDRESS_VALUE`. The matching binary's manager constructor can
cache the answer before the native Wi-Fi controller is ready. Publishing only
when the controller starts does not establish correct ordering.

## Property inventory

| Consumer contract | Existing state | Action |
|---|---|---|
| `IODeviceTree:/chosen/mac-address-wifi0`, binary six-byte CFData | Missing in the executed image. MobileGestalt checks this first. | Add before kernel startup, from the same pinned private board calibration used by the native driver. |
| `WifiAddress` string | Derived from `WifiAddressData`. | The same six bytes supply it; no independent string property. |
| `local-mac-address` on wlan/sdio/marconi-wifi | Alternative MobileGestalt fallback; absent. | No additional alias needed when the primary chosen property is present. |
| Controller `IOMACAddress`, BSD link address | Already use the validated native board address. | Preserve and verify equality with chosen before exposing services. |
| Network class ancestry, name, unit, type, Infrastructure role, built-in/primary flags | Already published with the expected types after attaching the real ifnet. | Preserve real state; do not claim carrier before association/key authorization. |
| `/arm-io/wlan` and native compatible | Already satisfy launchd and driver matching. | No copied Qualcomm or Apple hardware nodes. |
| Root model/compatible and filtered `/product` | Selected D22/iPhone10,3 compatibility profile is present. | Preserve this userland profile; it does not identify the physical MTK chip. |
| Chosen chip-id and unique-chip-id | Stable synthetic compatibility values with the required four/eight-byte extents; kernel derives platform UUID. | Preserve current identity and preferences. |
| `WifiChipset` | D22 product metadata present; all four selected wifid consumers tolerate nil before Apple-model optimizations. | No fabricated MTK-to-Broadcom replacement. |
| Wi-Fi vendor, revision, firmware strings | No additional mandatory selected wifid consumer found. | Do not invent Apple driver metadata. |
| Board-id, platform-name, serial | Extra fields in another port; no mandatory selected Wi-Fi consumer established. | Do not copy another device's identity. |
| `/options` and Apple NVRAM proxy | Absent; the selected kernel explicitly handles missing options. | No fake NVRAM backend or calibration-as-Apple-NVRAM substitution. |
| Android Wi-Fi calibration | Existing pinned private 6146-byte chosen property. | Remains native calibration; source of the factory address, not Apple NVRAM. |
| BuildVersion | Its provider reads `ProductBuildVersion` through `_CFCopySystemVersionDictionary`; the selected SystemVersion.plist supplies it. | No DeviceTree lookup on this path. |
| Diagnostics boot policy | A separate query checks chosen `osenvironment="diagnostics"`, otherwise **presence** of product `boot-ios-diagnostics`. | Keep the latter absent: even a zero/empty placeholder would enable the diagnostics bypass. Do not use this to hide an unlock problem. |

## Boot packaging and runtime contract

After the existing calibration injector, run:

```sh
python3 tools/nord2-wifi-identity.py calibrated.adt identity.adt
```

The tool accepts the already pinned board calibration, extracts the exact native
factory-address field, and adds only `chosen/mac-address-wifi0`. It rejects
wrong provenance, invalid/multicast/zero addresses, missing or duplicate nodes,
placeholder properties and conflicting values. Output is private and exclusive;
no address value appears in source or logs. Default ADT generation is unchanged.
The image verifier must check both the calibration and address, and prove that
removing this one addition yields the previous calibrated ADT exactly.

The controller independently obtains its address from the retained native owner,
checks the existing chosen property's type, length and bytes, and refuses a
conflict before interface publication. Its fallback installation is useful for
diagnostics but does not replace the pre-userland packaging requirement.

Tests execute the actual registry class, compare the offline selector with the
actual compiled native address selector, and check the executed payload in the
private audit. ASan/UBSan registry tests include refusal/lifetime paths. These
are static/host results, not a captured live MobileGestalt answer or proof of
WPA2 association, DHCP or Internet access.

## Separate keychain prerequisite

The executed log also contains 157 unsupported `securityd` wrap-key requests.
Patch `0083-nord2-keystore-compat.patch` implements NORD2-only operations 10/11
with the existing upstream `HKwr` format. This is a separate demonstrated API
gap, not the cause established by the recovered nil-MAC crashes.

**This is insecure bring-up compatibility:** the public fixed XOR format has no
encryption, authentication, SEP or passcode protection. It preserves upstream
persistent-format compatibility; it does not erase/reset the keychain, invent a
per-boot secret or guess foreign blob formats. A future secure implementation
will need an explicit migration policy. Tests cover the actual patched handler,
known upstream vector, descriptor failures, bounds and non-NORD2 regression.

## Upstream attribution and retained evidence

- [HackiOS loader](https://github.com/oosexgang/hackios-xnu-kernel/blob/8e61187098f2c26c5daee8f9f72b6416aa64b9ab/xnuloader/loader.c)
  independently publishes a raw six-byte chosen MAC before XNU. Its fixed
  address and other device identity values are not reused.
- [HackiOS MiniAppleKeyStore](https://github.com/oosexgang/hackios-xnu-kernel/blob/8e61187098f2c26c5daee8f9f72b6416aa64b9ab/xnu/files/iokit/Kernel/MiniAppleKeyStore.cpp)
  provides the APSL-2.0 prototype format, also used by
  [Topaz at e4772911](https://github.com/MakrSas/ios-topaz/tree/e4772911e116ab4ab6c4ac1f38e6106fefe8b483).
  The new handler adds bounded ABI validation and error handling without claiming
  cryptographic integrity for that format. The license is retained in
  [licenses/APSL-2.0.txt](licenses/APSL-2.0.txt).

Private reports, original-byte verification and hashes are retained under
`research/nord2/wifi/claude-opus-security-failure/review-device-tree/`.
No Apple binaries, private calibration, MAC, device serial or crash payloads
belong in Git. MobileGestalt manifest `e3123a12`, upstream audit `db1e5580`,
and independent AKS review `1a020f6f` identify the completed reviews.
