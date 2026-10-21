# AP wake diagnostic after persistent radio initialization

`nord2wifi=2 nord2wifipersist=1 nord2wifiwake=1` enables one bounded AP wake
and balanced sleep after the retained phase10 initialization succeeds.
Without the new flag this path adds no MMIO, mappings or result logs.
It does not load firmware, release a radio CPU or create a network interface.

The binding runs in the existing sole input/power owner thread. It first checks
the persistent owner's software state, then only the always-on AP registers.
It does not reuse the earlier generic guard that reads the sleeping CONN
domain. A separate one-shot state owns the HOST1a0 wake reference; the old
phase10 state, terminal handoff and retained power lifetime stay unchanged.
An inherited nonzero HOST vote causes refusal without a write.

The source sequence asserts HOST1a0, polls its acknowledgement, resets and
reads the clock detector, checks the IRQ and CONN ID, then verifies that both
radio processors remain off/reset. Its last hardware operation clears the
owned HOST1a0 vote. There are no callbacks or readbacks after that sleep write;
only saved result fields are logged before normal input initialization.

The wake reference is recorded before the write can take effect. An uncertain
failure after that point retains ownership and panics for manual reset instead
of attempting an unverified rollback. Software timeout limits cannot interrupt
a stalled MMIO access. ACK polling stops at the first timeout; deterministic
clock polling is bounded but is not timing-equivalent to the vendor's jitter.

`wifi_wake.c` and `wifi_wake_adapter.c` are separate guarded BSD translation
units. `wifi_wake_bind.inc` reuses the existing exact AP/HOST/CFG/RGU mappings.
No PMIC, TOP-SPI, semaphore, DRAM, MPU or firmware operations are added.

Success evidence must include the old phase10 retained/handoff result plus
`status=0 phase=7 balanced=1 ref=0 halt=0`, ID `0x20010101`, clock bits 1 and 2,
IRQ bit 0, all five cold observations and their cleared CPU/power masks.
`balanced=1` describes this diagnostic's wake vote; common power remains
retained. These records do not prove networking or future memory permissions.

The host tests exercise the actual production core, adapter and native bind
with injected I/O: every successful-path callback failure, bounded polling,
foreign votes, default-off behavior, no I/O after sleep, unchanged old owner
state and retained ownership on uncertain failure. Run:

```sh
python3 tools/test-nord2-wifi-wake.py
```

Selected Android source is commit
`04d00c2565764c3922c9b4826b5acff1106e533f` in
[android_kernel_oplus_mt6893](https://github.com/frxncyyy/android_kernel_oplus_mt6893/tree/04d00c2565764c3922c9b4826b5acff1106e533f).
The wake path is in `drivers/misc/mediatek/connectivity/conninfra/platform/`
`mt6893/mt6893_pos.c`; clock readability polling is in `consys_reg_util.h`.
