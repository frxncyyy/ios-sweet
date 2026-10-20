# Verified firmware extraction and opt-in XNU probe

`wifi_firmware.c` is a standalone, opt-in library with no boot hook, hardware
mapping, allocation or UFS binding. `nwf_load` reads only the 16 fixed vendor
extents previously verified against the preserved EROFS image. The table
contains offsets and SHA-256 values, never firmware bytes. Kernel builds use
existing exported `SHA256_Init/Update/Final`; host tests use OpenSSL.

The caller supplies 1,326,158 bytes of output and 65,536 bytes of scratch.
Every stored extent is SHA-256 checked before bounded LZ4 decoding/plain copy;
each completed file is SHA-256 checked before any blob pointers are published.
Failures after accepting buffers erase the complete output and scratch.
Arguments rejected before extraction leave buffers alone and clear blob metadata.
The caller must serialize access, keep blob metadata separate from buffers,
and only consume output after success. Buffers remain caller-owned; the caller
should erase output when finished. No target EMI addresses are used.

The read callback accepts a **vendor-relative byte offset** and bounded length,
and must fill the complete range or fail. The guarded UFS adapter exposes:

```c
int sweet_ufs_read_vendor(uint64_t offset, void *buffer, uint32_t length);
```

It must validate GPT identity/CRC/super bounds, confirm the preserved vendor
layout (super-relative offset 7,708,082,176 and length 544,354,304), reject
out-of-range arithmetic, and serialize existing UFS commands under the root
driver mutex. It must neither rebind userdata/root partition state nor expose
writes. Prefer revalidating LP metadata; a hardcoded layout must fail closed
unless storage identity/layout is positively established. The adapter now validates both GPT copies and pinned LP metadata hashes;
its separate model is `tools/test-nord2-ufs-vendor.py`. The backup GPT fixture
is synthesized from the saved primary, so actual backup layout is still an
unverified hardware condition and mismatches fail closed.
The fixed extent hashes reject changed firmware independently of filesystem
metadata. The loader does not hash the entire 544 MiB vendor on every call.

`wifi_firmware_probe.c` provides the opt-in kernel wrapper. Root initialization
starts a dedicated thread only after publishing UFS readiness and releasing its
mutex. Exact `nord2wififw=1` is required; default boots do no firmware work.
The probe permits one attempt, allocates with `Z_WAITOK | Z_NOPAGEWAIT`, logs
only verified names/sizes/status/timing, then erases and frees all buffers and
terminates its thread. It does not access EMI, execute firmware or enable RF.
The adapter and loader still require a hardware validation run.

`tools/nord2-mkpatch.sh` copies these canonical sources into guarded XNU
`bsd/dev/` translation units, adds them to patch0082 and verifies exact patch
reproduction. Focused host tests use these same canonical sources. Local
kernel compiles pass; complete kernel CI and boot-image size remain separate
integration checks. Firmware output consumes RAM, never embedded image space.

Run `python3 tools/test-nord2-wifi-firmware.py` with the private verified vendor
image/manifests present. It tests all real extents/full-file hashes, each extent
read failure and corruption, output corruption, rejected bounds/overlap,
malformed LZ4 and an exact-source ASan/UBSan 100,000-block decoder fuzz run.
