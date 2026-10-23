# Nord 2 speaker playback: Android parity review

Reviewed 2026-10-23 before the next device test. The scope is 48 kHz stereo
speaker playback, including repeated start/stop, HAL delivery and failure
handling. It does not include microphone, telephony or Android SmartPA DSP.

## Source selection

- Android kernel: `frxncyyy/android_kernel_oplus_mt6893`, commit
  `04d00c2565764c3922c9b4826b5acff1106e533f`, branch
  `KSUN-RUI-susfs-v2.2.0-manual-hook`.
- Selected audio platform: MT6885 AFE; the MT6893 board binds I2S3 to TFA9874
  slaves `0x34` and `0x37`, reset GPIO111/148.
- Selected codec sources are the four top-level files under
  `sound/soc/codecs/audio/tfa98xx`, as selected by its Makefile. The parallel
  `src/tfa98xx.c` implementation is not built and is not the startup contract.
- Installed HAL: HackiOS commit `a79ff0772a3454fd1ef127e181e5b3bce6c9ce18`;
  the compared Topaz commit is `e4772911e116ab4ab6c4ac1f38e6106fefe8b483`.
  The existing plugin package is reused without changes.
- The private Nord2 A16 speaker container is hash-pinned. Its 104 board
  bitfields and five silicon tables were checked against their source. It
  has not been established as a byte-identical dump of this handset's ODM.

## Corrections collected into one candidate

| Area | Evidence and correction |
|---|---|
| Inherited I2C state | AP START bit1 is not transaction-busy. V2 DMA_FSM_DEBUG bit15 participates in an optional high-speed branch above 1 MHz; the selected 100 kHz path preserves it. Retain the measured START2/CONTROL28/IRQ1/FIFO0/DEBUG8800 evidence. |
| Controller programming | Follow selected `i2c-mtk.c:963–1213`: timeout/divider, control/timing/address, FIFO, interrupt mask, lengths, data, MCU_INTR, START. Verify completion W1C before restoring inherited configuration. |
| Completion notification | `i2c-mtk.c:1068–1081` produces AP mask `0x129`. Poll with that peripheral mask and verify CPU delivery disabled for DTS SPI120 / GIC INTID152. The read-only PE helper reuses the existing distributor mapping; no GIC writes or extra mapping. Mask0 was not proven to guarantee completion latching. |
| Amplifier defaults | `tfa_dsp.c:611–633` issues I2CR before silicon initialization. Add the same literal reset, wait for self-clear, verify PWDN1/AMPE0, then apply settings. General register writes cannot assert I2CR; the dedicated operation requires fresh evidence from the same cold amplifier. |
| Container preamble | Replay the selected container's volatile `0f/a1/a0` key sequence before its bitfields, following `tfa_container.c:1210–1242`. No nonvolatile programming command is added. |
| Power protection | `clk-mt6885-pg.c:3351–3353` ignores release ACK. Verify request register EN2 instead of requiring ACK to clear. Before powerdown, verify both request SET and ACK1, so a stale ACK cannot hide a dropped SET. Preserve unrelated bits. |
| Normal stop | Mute both amplifiers before waiting for either to power down. Use Android's 100 ms MANSTATE wait, with a final boundary read. The former sequence could leave the second output playing an old SRAM ring during the first wait. |
| Runtime faults | Lost DMA timing/pointer, failed I/O and runtime clock/TDM faults immediately assert the two owned reset lines before blocking amplifier operations. Close the owned AFE path and retain a sticky error; do not restart automatically. |
| HAL burst size | The HAL advertises a maximum 16,384-frame period. An 8,192-frame producer could discard half of a valid write. Use a bounded 32,768-frame producer, without changing hardware DMA size or blocking writes. |
| Idle and recovery | Track queued meaningful PCM, drain the hardware tail, and let zero-only HAL writes expire. Old accepted audio is not discarded solely due to startup delay. Live ENABLE reports unavailable while a hardware error remains latched. |

## Deliberate differences retained

The port uses the source's supported 100 kHz I2C mode, rather than the DT's
400 kHz default. It polls instead of registering an I2C interrupt handler.
Every transfer checks current controller and shared-master ownership; it does
not reset the shared I2C controller or take over APDMA/CCU.

The PCM frontend is signed 16-bit stereo, while I2S3 uses 32-bit serial slots,
matching Android's frontend/backend distinction. One 4 KiB SRAM block at
`0x11212000` lies inside the board's declared audio SRAM. The existing 16 KiB
AFE mapping covers this block. A two-millisecond worker services this ring;
scheduler latency is measured and late ownership is treated as a fault.

The running ADSP and valid 196.608 MHz APLL2 are preserved. Optional MCLK,
other I2S gates, PMIC rail writes and non-selected DSP initialization were
not added without a board/source requirement. Both outputs initially use the
existing low digital ceiling; Android speaker processing and protection are
not implemented by these changes.

## Verification and evidence limits

Durable regressions are registered in `tools/nord2-audio-check.py`:

- actual inherited register values, repeated playback and restoration;
- all 65,536 inherited V2 debug values under the 100 kHz branch;
- controller ordering, lost/sticky completion clears, GIC owner/race failures;
- stale protection ACK, dropped requests and mandatory powerdown ACK;
- reset defaults, exact key order, unsafe reset rejection and 99/100 ms bounds;
- both-output stop ordering and immediate containment of runtime faults;
- full HAL bursts, queue wrap, delayed startup, ring-tail drain and zero tails;
- native publication, live control, packaging and recovery faults.

Tests execute the actual source with register models or explicitly identified
kernel-wrapper stubs, under ASan/UBSan. Synthetic register values remain labeled
as such. They establish code behavior, not physical amplifier ACK, revision,
clock lock, DMA progress or audible output. Those are still required on-device
checks after the complete candidate and its rollback image are verified.

Private audit receipts live under
`research/nord2/audio/playback-20261023/{hardware,install,profile}-sol/android-parity-20261023`.
The combined image pipeline is `research/nord2/audio/playback-20261023/ios-parity`.
The earlier `ios-i2c-legacy` candidate was superseded before packing and testing.

## First hardware test and routing correction

The authorized parity image (`d0677caa…adb3a0c`, source `23ebee5`, CI
`40015584829`) initialized both physical amplifiers as revision `0c74`.
The one-second silent DMA and tone tests completed without hardware or stop
errors. During iOS media playback, the HAL delivered 955,392 frames with no
producer drops; the driver serviced 904,332 cumulative PCM frames. The user
heard no media output. These counters establish data movement, not sound.

The follow-up comparison found an error missed by the initial audit:
`mt6885-dai-i2s.c:407–432` binds I2S3 to `AFE_CONN0/1`, while the port used
`AFE_CONN28/29`. The latter belong to I2S1 in the same Android source at
lines 461–483. Correct clocks and advancing DL1 therefore did not establish
that samples reached the physical I2S3 output. The route regression must
anchor sink selection to Android's DAPM arrays, not merely reproduce the
port's register constants.

The port was stopped cleanly through USB after this test. The follow-up
candidate routes and disconnects DL1 through CONN0/1, removes CONN28/29
from the write allowlist, and preserves the historical observation addresses.
Source-derived tests reproduce the old defect and verify three 0c74 playback
cycles, stereo sample packing and failure cleanup without any I2S1 writes.
The full sanitizer suite and ten native compilation units pass.

The candidate also records nine fixed amplifier registers per chip once after
both enables, before PCM delivery. It preserves partial evidence on failed
reads and acts on clear-on-read NOCLK rather than hiding that fault. Saved
values are printed only after stop or failed startup, with no new ongoing
reads or playback-time console output. Neither the HAL nor live USB ABI changes.

The initial digital ceiling remains 256/32768 (about −42.1 dB); the half-scale
test tone reaches about −48.2 dBFS. No physical output success or amplifier
gain in dB is inferred from the container's raw gain fields.


## Audible hardware result (2026-10-23)

The route image `3b8cb873…4bec4c46`, source `6ee28295`, CI `40019831363`,
was flashed after exact authorization. HAL reuse succeeded (mode1/phase2/error0).
The user heard the diagnostic tone and then confirmed iOS media from **both
speakers**, with responsive volume and pause/resume. The bottom speaker was
reported extremely quiet; speaker balance and normal-volume output remain
unresolved. No gain or speaker-protection setting was changed for this test.

Both 0c74 amplifiers supplied complete startup snapshots (valid mask1ff):
SYS0018, STATUS10=0016, STATUS11=e2c0, STATUS13=850f, STATUS14=00c8,
MUTE58=1614, TDM20=2890, TDM23=000b and TDM26=1001/0010. The selected Android
bitfield definitions decode these as PLL/clocks locked, amps engaged,
MANOPER1/MANSTATE9, no manual/hard mute, and enabled TDM/speaker inputs.
These observations were captured before feeding PCM, not sampled continuously.

After media and pause, live STATUS11 showed 3,249,152 accepted frames, zero
producer drops, 3,205,820 cumulative PCM frames, 14 stream starts and a maximum
worker gap of3,976us. Playback had stopped with an empty queue and error,
uncertain and stop_error all zero. The underrun counter includes intentionally
zero-fed silent tests and stream tails; it is not an audible-glitch count.
The test did not establish microphones, telephony, full speaker protection,
or calibrated loudness.

Private immutable evidence is under
`research/nord2/audio/playback-20261023/ios-route/hardware-review`.
