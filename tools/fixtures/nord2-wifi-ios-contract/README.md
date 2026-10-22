# iOS Wi-Fi pure contract fixtures

Run `python3 tools/test-nord2-wifi-ios-contract.py` from any directory.
Clang with AddressSanitizer and UndefinedBehaviorSanitizer is required by default;
`--cc` may select a compatible compiler. Executables live in a temporary directory.
The runner compiles the current canonical `shim/nord2-xnu` helpers and records
source/fixture SHA-256 hashes with optional `--report PATH`.

The policy fixture uses an independent typed 5456-byte request structure, with
compile-time offset assertions for the selected iOS/Redwood scan ABI. It covers
raw modes 1 and 2 translated to passive discovery, all/native-subset channels,
ACTIVE permission flags without probes, ordered subsets, duplicate/ambiguous
channels, every truncation, unused bytes, directed filters, timing and advanced
options. Refusals must preserve output. The accepted raw-mode-1 translation is
this port's explicit compatibility policy, not proof of active probing.

The scalar fixture preserves the reviewed v2 matrix: GET13 state and GET15 STA
mode, invalid requests/states/names/capacities, null arguments and atomic refusal.
GET27/254 produce the exact 4824-byte allowed-channel layout, zero reserved bytes,
no ACTIVE/DFS flags, capacity and malformed metadata refusal. Synthetic channel
metadata is not runtime country evidence.

These fixtures need no board file, device, private artifact, firmware or IOKit
model. They check pure encoders and admission only. They do not authenticate an
owner, establish IPC/Settings behavior, prove association, or grant a native
transport lease. Native callers must supply authoritative retained state and
serialize lifetime and ownership separately.
