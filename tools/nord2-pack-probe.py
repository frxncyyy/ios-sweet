#!/usr/bin/env python3
"""Pack a diagnostics-only Linux Image using the backed-up Nord 2 boot v2 header.

No device access. On the tested Nord 2, `fastboot boot` fails even for the
original image. Testing therefore requires separately authorized boot-only
flashing followed by restoration of the verified original partition backup.
Preserves original ramdisk, DTB, addresses, command line and page size.
"""
import argparse
import gzip
import hashlib
import struct
import zlib
from pathlib import Path


def pack(stock: bytes, image: bytes, chain_stock=False) -> bytes:
    def u32(off): return struct.unpack_from('<I', stock, off)[0]
    if len(stock) < 1660 or stock[:8] != b'ANDROID!' or u32(40) != 2:
        raise ValueError('expected Android boot image v2')
    page = u32(36)
    if page != 2048 or u32(12) != 0x40080000 or u32(20) != 0x51100000:
        raise ValueError('not the verified Nord 2 boot layout')
    if u32(24) or u32(1632):
        raise ValueError('second/recovery_dtbo payload unsupported; refusing to discard it')
    if len(image) < 64 or image[56:60] != b'ARM\x64':
        raise ValueError('not an arm64 Linux Image')
    image_size = struct.unpack_from('<Q', image, 16)[0]
    if not len(image) <= image_size <= 1024*1024:
        raise ValueError('unexpected diagnostic Image size')
    align = lambda n: (n+page-1)//page*page
    rstart = page+align(u32(8))
    dstart = rstart+align(u32(16))
    if dstart+u32(1648) > len(stock):
        raise ValueError('truncated original boot image')
    ramdisk = stock[rstart:rstart+u32(16)]
    dtb = stock[dstart:dstart+u32(1648)]
    if dtb.startswith(b'\xd7\xb7\xab\x1e'):
        if len(dtb) < 32:
            raise ValueError('truncated Android DT table')
        _, total, hs, es, count, eo, _, _ = struct.unpack_from('>8I', dtb)
        if total != len(dtb) or hs < 32 or es < 32 or not count or eo+count*es > total:
            raise ValueError('invalid Android DT table')
        for i in range(count):
            size, off = struct.unpack_from('>2I', dtb, eo+i*es)
            if off+size > total or size < 40 or dtb[off:off+4] != b'\xd0\x0d\xfe\xed':
                raise ValueError('invalid FDT entry')
    elif not dtb.startswith(b'\xd0\x0d\xfe\xed'):
        raise ValueError('missing stock DTB')
    if chain_stock:
        # Linux arm64 Image flags bit 3 explicitly permits any 2 MiB-aligned
        # placement (plus text_offset). Keep the original kernel byte-for-byte
        # at +2 MiB, with its full image_size including BSS and appended DTBs.
        dec = zlib.decompressobj(31)
        original = dec.decompress(stock[page:page+u32(8)]) + dec.flush()
        if not dec.eof or original[56:60] != b'ARM\x64':
            raise ValueError('stock kernel is not a gzip arm64 Linux Image')
        offset, original_size, flags = struct.unpack_from('<3Q', original, 8)
        if offset != 0x80000 or not flags & 8 or not len(original) <= original_size < 0x7000000:
            raise ValueError('stock kernel does not permit verified chainload layout')
        prefix = 0x200000
        combined = bytearray(image.ljust(prefix,b'\0'))
        struct.pack_into('<Q', combined, 16, prefix+original_size)
        combined += original.ljust(original_size,b'\0')
        kernel = gzip.compress(combined, mtime=0) + dec.unused_data
    else:
        kernel = gzip.compress(image.ljust(image_size, b'\0'), mtime=0)
    header = bytearray(stock[:page])
    struct.pack_into('<I', header, 8, len(kernel))
    struct.pack_into('<Q', header, 1636, 0)  # no recovery_dtbo
    h = hashlib.sha1()
    for payload in (kernel, ramdisk, b'', b'', dtb):
        h.update(payload); h.update(struct.pack('<I', len(payload)))
    header[576:608] = h.digest().ljust(32, b'\0')
    return bytes(header) + b''.join(x.ljust(align(len(x)),b'\0') for x in (kernel, ramdisk, dtb))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('stock', type=Path)
    parser.add_argument('image', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--chain-stock', action='store_true', help='append original Android kernel at +2 MiB')
    args = parser.parse_args()
    data = pack(args.stock.read_bytes(), args.image.read_bytes(), args.chain_stock)
    with args.output.open('xb') as f:
        f.write(data)
    print(f'{args.output}: {len(data)} bytes, SHA256 {hashlib.sha256(data).hexdigest()}')
