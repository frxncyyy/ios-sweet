#!/usr/bin/env python3
"""Pack the Nord 2 XNU early-probe boot image.

  nord2-pack-xnu.py STOCK_BOOT LOADER_IMAGE PAYLOAD OUT

STOCK_BOOT    the verified original boot.img (backup/nord2/boot.img)
LOADER_IMAGE  shim/nord2-xnu Image (arm64 Linux Image, linked at 0)
PAYLOAD       SWEETXNU payload from tools/mkpayload.py

The result is an Android boot v2 image with the same header, ramdisk and DT
table as the stock image. Its kernel is a gzip stream containing:

  0x000000                 loader, zero-padded to 2 MiB
  0x200000                 the unchanged stock Android kernel Image
  0x200000 + image_size    the SWEETXNU payload (16 KiB aligned)

so LK decompresses everything to 0x40080000 and jumps to the loader, while
the stock kernel stays byte-for-byte at 0x40280000 for the chain-back.

No device access. The packer refuses to overwrite an existing output and
checks every size against the partition and the original layout.
"""
import argparse
import gzip
import hashlib
import struct
import subprocess
import sys
import tempfile
import zlib
from pathlib import Path

PARTITION_SIZE = 33554432
LOADER_RESERVE = 0x200000
PAYLOAD_ALIGN = 0x4000
RAMDISK_PA = 0x51100000
KERNEL_PA = 0x40080000
LOADER_SOURCE_END = KERNEL_PA + LOADER_RESERVE + 0x6000000


def u32(buf, off):
    return struct.unpack_from('<I', buf, off)[0]


def stock_parts(stock):
    if len(stock) < 1660 or stock[:8] != b'ANDROID!' or u32(stock, 40) != 2:
        raise SystemExit('expected an Android boot image v2')
    page = u32(stock, 36)
    if page != 2048 or u32(stock, 12) != KERNEL_PA or u32(stock, 20) != RAMDISK_PA:
        raise SystemExit('not the verified Nord 2 boot layout')
    if u32(stock, 24) or u32(stock, 1632):
        raise SystemExit('second/recovery_dtbo payload unsupported')
    align = lambda n: (n + page - 1) // page * page
    rstart = page + align(u32(stock, 8))
    dstart = rstart + align(u32(stock, 16))
    if dstart + u32(stock, 1648) > len(stock):
        raise SystemExit('truncated original boot image')
    return page, stock[page:page + u32(stock, 8)], \
        stock[rstart:rstart + u32(stock, 16)], \
        stock[dstart:dstart + u32(stock, 1648)]


def unpack_stock_kernel(kernel_gz):
    dec = zlib.decompressobj(31)
    image = dec.decompress(kernel_gz) + dec.flush()
    if not dec.eof:
        raise SystemExit('stock kernel gzip is truncated')
    if image[0x38:0x3c] != b'ARM\x64':
        raise SystemExit('stock kernel is not an arm64 Linux Image')
    return image, dec.unused_data


def payload_span(payload):
    magic, version, nsegs = struct.unpack_from('<8sII', payload, 0)
    if magic != b'SWEETXNU' or version != 1 or not 0 < nsegs <= 64:
        raise SystemExit('not a SWEETXNU v1 payload')
    phys_base, mem_size = struct.unpack_from('<2Q', payload, 0x38)
    virt_base, entry_va, adt_off, adt_size, kernel_end = \
        struct.unpack_from('<5Q', payload, 0x10)
    return dict(nsegs=nsegs, phys_base=phys_base, mem_size=mem_size,
                virt_base=virt_base, entry_va=entry_va, adt_off=adt_off,
                adt_size=adt_size, kernel_end=kernel_end)


def pack(stock, loader, payload, zopfli=None):
    page, stock_gz, ramdisk, dtb = stock_parts(stock)
    stock_image, appended = unpack_stock_kernel(stock_gz)
    image_size = struct.unpack_from('<Q', stock_image, 16)[0]
    if not len(stock_image) <= image_size <= 0x7000000:
        raise SystemExit('stock Image image_size is implausible')
    # image_size includes BSS, which the gzip stream does not carry: pad to it
    # so the payload starts after the BSS, exactly like the verified probe.
    stock_image = stock_image.ljust(image_size, b'\0')

    info = payload_span(payload)
    if info['phys_base'] != 0x142000000 or info['mem_size'] != 0x3e000000:
        raise SystemExit('payload window is not the Nord 2 probe window')
    if info['adt_off'] + info['adt_size'] > len(payload):
        raise SystemExit('payload ADT out of range')
    if len(payload) < info['adt_off'] + info['adt_size']:
        raise SystemExit('payload truncated')

    if len(loader) > LOADER_RESERVE:
        raise SystemExit(f'loader is {len(loader)} bytes, over the 2 MiB reserve')
    loader_size = struct.unpack_from('<Q', loader, 16)[0]
    if not len(loader) <= loader_size <= LOADER_RESERVE:
        raise SystemExit('loader Image size header is inconsistent')

    payload_off = LOADER_RESERVE + image_size
    payload_off = (payload_off + PAYLOAD_ALIGN - 1) & ~(PAYLOAD_ALIGN - 1)
    if payload_off + len(payload) > RAMDISK_PA - KERNEL_PA:
        raise SystemExit('payload would reach the ramdisk load address')
    if KERNEL_PA + payload_off + len(payload) > LOADER_SOURCE_END:
        raise SystemExit('payload exceeds the loader source scan bound')

    blob = bytearray(loader.ljust(LOADER_RESERVE, b'\0'))
    blob += stock_image
    blob += b'\0' * (payload_off - len(blob))
    blob += payload
    # Match the stock Image flags: text_offset 0x80000, bit 3 (any 2 MiB
    # aligned placement) is not needed here because LK loads at 0x40080000.
    if struct.unpack_from('<Q', blob, LOADER_RESERVE + 8)[0] != 0x80000:
        raise SystemExit('unexpected stock text_offset')

    if zopfli:
        # Same gzip format and decompressed bytes as the default path. The
        # smaller stream leaves room for the ADB rescue ramdisk in 32 MiB.
        with tempfile.TemporaryDirectory() as temp:
            raw = Path(temp) / 'kernel'
            raw.write_bytes(blob)
            subprocess.run([str(Path(zopfli).resolve()), '--gzip', str(raw)], check=True)
            compressed = raw.with_suffix('.gz').read_bytes()
        if gzip.decompress(compressed) != blob:
            raise SystemExit('compressed kernel round-trip mismatch')
    else:
        compressed = gzip.compress(bytes(blob), 9, mtime=0)
    kernel_gz = compressed + appended
    align = lambda n: (n + page - 1) // page * page
    if page + align(len(kernel_gz)) + align(len(ramdisk)) + align(len(dtb)) > PARTITION_SIZE:
        raise SystemExit('compressed kernel does not fit next to the ramdisk')

    header = bytearray(stock[:page])
    struct.pack_into('<I', header, 8, len(kernel_gz))
    struct.pack_into('<Q', header, 1636, 0)
    h = hashlib.sha1()
    for part in (kernel_gz, ramdisk, b'', b'', dtb):
        h.update(part)
        h.update(struct.pack('<I', len(part)))
    header[576:608] = h.digest().ljust(32, b'\0')

    align = lambda n: (n + page - 1) // page * page
    out = bytes(header) + kernel_gz.ljust(align(len(kernel_gz)), b'\0')
    out += ramdisk.ljust(align(len(ramdisk)), b'\0')
    out += dtb.ljust(align(len(dtb)), b'\0')
    if len(out) > PARTITION_SIZE:
        raise SystemExit(f'boot image {len(out)} exceeds the {PARTITION_SIZE}-byte partition')
    return out, dict(kernel_gz=len(kernel_gz), total=len(out),
                     loader=len(loader), stock_image=image_size,
                     payload_off=payload_off, payload=len(payload),
                     ramdisk=len(ramdisk), dtb=len(dtb))


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('stock', type=Path)
    ap.add_argument('loader', type=Path)
    ap.add_argument('payload', type=Path)
    ap.add_argument('output', type=Path)
    ap.add_argument('--zopfli', type=Path, help='optional local Zopfli executable')
    a = ap.parse_args()
    if a.output.exists():
        raise SystemExit('output already exists')
    data, info = pack(a.stock.read_bytes(), a.loader.read_bytes(), a.payload.read_bytes(), a.zopfli)
    with a.output.open('xb') as f:
        f.write(data)
    for k, v in info.items():
        print(f'{k}: {v}')
    print(f'{a.output}: {len(data)} bytes, SHA256 {hashlib.sha256(data).hexdigest()}')


if __name__ == '__main__':
    main()
