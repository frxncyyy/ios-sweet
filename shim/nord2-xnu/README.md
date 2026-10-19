# Nord 2 XNU early probe

This is a diagnostic boot image for the OnePlus Nord 2 (MediaTek MT6893), not
an iOS boot. It contains the Sweet XNU kernel built for MT6893, a small
Linux-Image loader and a preserved copy of the stock Android kernel. It
writes a log to the Android ramoops `pmsg` ring, runs XNU as far as
`cpu_init()`, then returns to the untouched stock kernel. It never writes
storage, the framebuffer or a reserved carve-out, and it does not load a
ramdisk of its own.

## Artifact layout

`tools/nord2-pack-xnu.py` reuses the stock Android boot v2 header, ramdisk
and DT table, and replaces only the gzip kernel stream:

```
0x40080000  loader                      (this shim, zero-padded to 2 MiB)
0x40280000  stock Android Image         (byte-identical, BSS zero-padded to
                                         the header's image_size)
0x436c1000  SWEETXNU payload            (16 KiB aligned)
```

LK decompresses the whole stream to `0x40080000` and, because the loader's
Image header branches from its first instruction, runs the loader. The
loader finds the stock Image header at `0x40280000`, so the stock kernel
stays byte-for-byte where a normal boot puts it, and the chain-back needs no
copying. The decompressed blob ends at `0x444c0410`, below LK's decompression
limit (`KERNEL_DECOMPRESS_SIZE = 0xc800000` on this mediaTek generation) and
well below the ramdisk at `0x51100000`.

The loader places the payload's Mach-O segments in a 992 MiB window in the
upper DDR bank:

```
phys_base 0x1_42000000   mem_size 0x3e000000   virt_base 0xfffffe0006000000
```

`start.s` identity-maps the 32 MiB blocks from `phys_base` for `mem_size`
bytes (30 blocks, `0x1_42000000..0x1_7e000000`); the ADT and `boot_args` sit
just past the kernel image, and the chain-back trampoline is copied to
`0x1_42004000`, the first free page of the mapped window.

## Chain-back

The boot args carry `/chosen` with three zero placeholders that the loader
patches in RAM:

| property | size | value |
| --- | --- | --- |
| `nord2-stock-entry` | 8 | `0x40280000` (stock Image entry) |
| `nord2-stock-dtb` | 8 | the live FDT physical address |
| `nord2-xnu-region` | 16 | first mapped / end of the identity window |

`nord2_maybe_return()`, called from `arm_init.c` after `cpu_init()`, reads
them and, unless the boot argument `nord2stop=0` was given, calls
`nord2_leave_xnu()` in `start.s`. That routine copies its trampoline into the
window, cleans the whole window to the point of coherency, turns the MMU off
with the `SCTLR_EL1` value the CPU probe observed at stock entry
(`0x30d00800`), zeroes `x1..x3`, and branches to the stock entry with
`x0 = dtb`. `nord2stop=1` is also the kernel default.

Because the early stop is before `arm_vm_init()`, the kernel never touches
the GIC, the arch timer or UFS: only the ADT is parsed.

## Reading the result

After Android starts, the log is `/sys/fs/pstore/pmsg-ramoops-0` (read it
before the next reboot). It shows the loader stages, then for each XNU stage
`NORD2-XNU N/64 <label>` up to `6/64 cpu_init done`, then either
`RETURN TO STOCK ANDROID KERNEL` or a panic with the stage it reached. Logs
from the earlier CPU probe are in `backup/nord2/probe-v2-*`.

## Building

```sh
shim/nord2-xnu/build.sh
```

It needs the extracted CI buildtree (`research/ci-39158169212/buildtree`, from
`xnu-buildtree.zip`), the relinked object archive next to it, the cctools
`ar`/`ranlib`/`strip` in `../toolchains/cctools`, and the backup
`backup/nord2/boot.img`.

The XNU-side changes are `xnu/patches/0082-nord2-early-probe.patch` against
the CI buildtree (six files; five of them become objects). The script
rebuilds those objects with `-DNORD2=1`, swaps them into a copy of the CI
archive, relinks with the saved `research/nord2/link-command.json` argv,
strips the symbol table, builds the ADT, the payload, this loader, the boot
image, and finally runs `tools/nord2-check-xnu.py` on the result. Outputs
land in `shim/out/nord2-xnu/`; the relink and the packer refuse to overwrite.

## Flashing

This directory builds an image; it is not permission to flash it. `fastboot
boot` fails on this phone even for the original image, so a test means
flashing **boot only**, entering fastboot manually, and afterwards restoring
`backup/nord2/boot.img` (33554432 bytes, SHA-256
`04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01`) and
checking the read-back hash and `sys.boot_completed`. Bootloader, recovery,
modem, persist and userdata are never touched.
