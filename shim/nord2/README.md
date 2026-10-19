# Nord 2 CPU probe

This is a diagnostic for the MediaTek MT6893 OnePlus Nord 2, not an iOS
boot image. It does not access storage controllers. It writes a short log
to the pmsg area of the Android ramoops reservation, after checking the
live device tree and the cache/MMU state.

Build on Linux with clang, ld.lld and Python 3:

```sh
shim/nord2/build.sh --chain-stock
```

The private original partition dump must be at `backup/nord2/boot.img`.
The output is `shim/out/nord2-chain/probe-boot.img`. The packer refuses to
overwrite an existing output. No build command accesses the device.

The chain variant places the unchanged original Android kernel 2 MiB
after the probe. It checks the original Image's placement flag, preserves
its BSS size, appended device trees, ramdisk and boot header addresses,
then branches to that kernel without resetting RAM. The variant without
`--chain-stock` resets instead; a subsequent cold boot may lose its log.

On the tested phone, `fastboot boot` failed even with the exact original
boot image, and `adb reboot bootloader` returned to Android. Each test
therefore used a separately authorized flash of **boot only**, followed
by manual entry into fastboot and restoration of the full original dump.
Never substitute another device's backup. A boot image build is not
permission to flash it.

After Android starts, root can read `/sys/fs/pstore/pmsg-ramoops-0`.
Save it before another reboot. Read back and hash the boot partition
after restoring it; the hash must match the full original backup.

## Verified result, 2026-10-19

The chain variant executed completely, produced a 454-byte pmsg log, and
Android reached `sys.boot_completed=1`. `/proc/iomem` showed Android's
kernel code at `0x40280000` rather than `0x40080000`, confirming the
2 MiB relocation. The observed CPU values were:

| Register | Value |
| --- | --- |
| CurrentEL | `0x4` (EL1) |
| MPIDR_EL1 | `0x81000000` |
| MIDR_EL1 | `0x412fd050` |
| ID_AA64MMFR0_EL1 | `0x101122` |
| ID_AA64PFR0_EL1 | `0x11112222` |
| SCTLR_EL1 | `0x30d00800` |
| CNTFRQ_EL0 | `13000000` |
| ICC_SRE_EL1 | `1` |
| PSCI_VERSION | `0x10001` |
| SMCCC_VERSION | `0x10001` |

This proves bare-metal code execution and return to Android. It does not
prove XNU boot, interrupt delivery, storage takeover, display or touch.
The stock boot partition was restored and its full SHA-256 matched the
private backup. Device dumps and Apple artifacts remain ignored by Git.
