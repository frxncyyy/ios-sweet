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
are synthetic firmware tests. Repeated joins and DHCP on the physical Nord 2
must still be verified after installing the prepared image.
