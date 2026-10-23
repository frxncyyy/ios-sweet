# Nord 2 live Wi-Fi development controls

The development kernel can expose a fixed USB command mailbox with
`nord2wifilive=1`. It must be installed once before these commands work.
The existing USB log reader remains available. This is not a shell or a kernel
patch loader: changes to executable driver code still require a new image.

Run commands from the repository with Python 3 and libusb available:

```sh
python3 tools/nord2-wifi-live.py status
python3 tools/nord2-wifi-live.py status --watch 30
python3 tools/nord2-wifi-live.py capture dhcp
python3 tools/nord2-wifi-live.py capture headers
python3 tools/nord2-wifi-live.py capture off
python3 tools/nord2-wifi-live.py rx-ofld allow
python3 tools/nord2-wifi-live.py rx-ofld drop
python3 tools/nord2-wifi-live.py disconnect
```

`status` reports public driver states, queue counters, receive rejection counts,
DHCP message counts and `join_ready`. Neither an authorized WPA port nor a DHCP
message counter proves a lease. Check the assigned IP, router and an actual
request in Safari on the phone.

Capture commands enable bounded numeric frame summaries in the USB log, without
packet contents, addresses, SSIDs or credentials. `capture` in the snapshot is
the configured/armed mode, not proof that a frame has been emitted. Arming it
before a Join reserves a window for that attempt. Each subsequent attempt also
consumes a window. There are at most 32 windows per boot, each limited to 64 TX
and 64 RX summaries. A restart of capture never resets security state, original
counters, deadlines or packet ownership.

`rx-ofld allow` selects the corrected receive policy: OFLD classification alone
does not drop a packet. `drop` reinstates that additional legacy filter for a
live comparison. Both modes retain all header, peer, key, replay and controlled
port checks. The option applies to the current active station; a fresh attempt
defaults to `allow`.

`disconnect` requests the existing asynchronous teardown and waits for closed
queues plus `join_ready`. A timeout leaves the state intact and reports the
incomplete operation. It does not force a radio reset. Start the next Join from
iOS, supplying the password only on the phone and keeping Private Wi-Fi Address
off. The host never stores or replays a password or a previous Join request.

## Command transport

The Nord 2 USB interrupt handler only queues fixed requests. The retained Wi-Fi
owner executes them under the driver's existing locking rules, with an idle
wakeup interval of 100 ms. The transport uses a boot nonce, monotonic command
tokens and an exact expected station generation. An identical command can be
retried after a lost USB response without executing it twice. Rebooted,
conflicting, stale, exhausted or busy sessions are reported explicitly.

Protocol version 1 uses vendor requests `0x60` (64-byte capabilities), `0x61`
(zero-data OUT command), and `0x62` (256-byte result). Public words are defined
in `wifi_live_control.h`; word 57 is `join_ready`, and words 58–63 are reserved
zero. Time is in milliseconds. A USB acknowledgement means queued, not executed.
The host matches the completed token before reporting an operation result.

The CLI serializes command clients with a local lock. Use only one USB log
collector. It cannot execute arbitrary code, access memory/register addresses,
write storage, expose keys or perform an automatic Join.

## Repeated attempts and limits

Reuse requires completed native teardown, PTK and GTK deletion submissions,
STA/BSS/channel cleanup, real DMA/token retirement, client callback retirement
and a fresh completed native scan. Historical `used`/`attempted` markers and
boot-wide packet IDs stay intact. The new station preserves physical ring/token
positions and uses fresh authentication material. Incomplete or uncertain
cleanup refuses another attempt.

This follows the selected MediaTek driver's ordering and completion contract.
The firmware key-ready event has no host generation, and token returns have a
finite wrapping namespace; the implementation cannot distinguish an arbitrary
late duplicate after wire identifiers wrap. It does not claim a new firmware
epoch barrier or firmware acknowledgement of key deletion. Management packet
IDs are never reused, so their finite boot-wide pool can eventually require a
restart. Data/security correlation falls back to explicitly ambiguous PID 127
after its distinct diagnostic IDs are exhausted; packet transmission continues.

Local tests exercise actual owner code through two complete WPA2 handshakes,
teardown and fresh scan, then reject an old-MIC attempt. They also cover mailbox
concurrency, lost responses, capture quotas and receive-policy guards. These
are synthetic firmware tests. Repeated disconnect/rejoin has now been observed on the physical Nord 2.
DHCP still fails there; the v2 peer/profile additions require a new device test.


## Version2 peer metadata and next-join profile extension

This tool requires a kernel that explicitly enables `nord2wifilive=1`. It is not a shell, memory reader, key dump or network-success test. Password entry remains on the phone. The CLI holds one host lock, checks the boot nonce and obtains fresh STATUS before every non-STATUS operation. An ambiguous timeout never causes a new disconnect token to be sent automatically.

ABI v2 adds these commands:

- `python3 tools/nord2-wifi-live.py peer`: fixed WLAN5 metadata probe while the actual station is healthy, authorized and between DMA/pump operations. Reports FROM_DS, TO_DS, MUAR, cipher and QoS only. It does not expose MAC/key/table words. At most16 eligible attempts per boot; STATUS and idempotent command replay consume none. Wrong selector group, changing group, absent maps or invalid owner refuse without changing any table.
- `python3 tools/nord2-wifi-live.py data-profile ethernet`: select the existing Ethernet-conversion path for the next fresh join.
- `python3 tools/nord2-wifi-live.py data-profile tods`: select the separately reviewed raw legacy ToDS ordinary-data path for the next fresh join. EAPOL retains its existing transport. This is a diagnostic compatibility profile, not evidence of DHCP success.

Profiles require `join_ready=1`, no active/queued/retiring client and the owner's real closed-key/peer/fresh-scan proof after any previous attempt. Disconnect and wait for readiness before switching. The active station/profile/page is never rewritten. `next_data_profile` is the intended next constructor setting; `data_profile` is the most recently prepared station setting. They may differ while idle. Only the next constructor applies selection.

The original STATUS, capture, OFLD and disconnect commands retain their semantics. The updated CLI can use ABI v1 for those original commands; new commands refuse on v1. A v1 host rejects v2's capability/version before sending a command. Reply words0..57 retain their old positions; v2 uses58 next profile,59 prepared profile,60 peer valid,61 packed metadata,62 probe error;63 stays zero. Packed metadata is FD bit0,TD bit1,MUAR bits2..7,cipher bits8..12,QoS bit13. It is decoded into named public fields by the CLI. Probe error0 none,1 prerequisite,2 maps absent,3 owner,4 initial selector,5 final selector changed. Result codes distinguish refused/unavailable/stale/limit.

The source-backed fixed mapping is selected SOC3 `_HIF_AXI`: chip WTBLON0x820d0000 maps bus0x430000, added to DT CSRbase0x18000000. Only two retained4KiB pages are mapped: selector0x18434000 and entry page0x18438000. The only table reads are WLAN5 DW0 at page+0x500 and DW2 at+0x508, bounded by group0 selector reads before/after. No selector/MMIO write is implemented. The temporary probe window extends only the existing outer physical owner check; ordinary RX/memory/register-write allowlists and active flags remain unchanged.

Reproducible offline checks (no USB):

```
python3 tools/test-nord2-wifi-live-control.py --log-dir /tmp/live-pure
python3 tools/test-nord2-wifi-live-bind.py --log-dir /tmp/live-binder
python3 tools/test-nord2-wifi-live.py --log-dir /tmp/live-host
python3 tools/test-nord2-wifi-live-peer.py --log-dir /tmp/live-peer
```

The last test runs actual retained NSS/NST/NWL owner and guard code with a synthetic AP/MMIO fixture and a read-only pinned board file. It never prints board bytes. Host simulation/compilation does not prove hardware register readability, raw-profile interoperability, air delivery, a DHCP lease or Internet access.
