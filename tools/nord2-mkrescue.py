#!/usr/bin/env python3
"""Build an ADB-only RAM recovery from the Nord2's existing boot/recovery backups.

No device access. Uses the original Android kernel/DT and the recovery's
init/adbd, their library closure and SELinux policy. No recovery UI, fstab
mounts, formatting or userdata access are started. ADB retains public-key
authentication; the supplied host public key is embedded only in the output.

  nord2-mkrescue.py STOCK_BOOT RECOVERY_BOOT OUT --adb-key ~/.android/adbkey.pub
"""
import argparse
import gzip
import hashlib
import json
from pathlib import Path, PurePosixPath
import re
import stat
import struct
import subprocess
import tempfile


def parts(b):
    assert b[:8] == b'ANDROID!' and struct.unpack_from('<I', b, 40)[0] == 2
    page = struct.unpack_from('<I', b, 36)[0]
    assert page == 2048 and not struct.unpack_from('<I', b, 24)[0]
    sizes = [struct.unpack_from('<I', b, o)[0] for o in (8, 16, 1632, 1648)]
    pos, result = page, []
    for size in sizes:
        assert pos + size <= len(b)
        result.append(b[pos:pos + size])
        pos += (size + page - 1) // page * page
    return b[:page], (result[0], result[1], result[3])


def cpio_read(data):
    out, pos = {}, 0
    while pos < len(data):
        assert data[pos:pos + 6] == b'070701', 'expected newc archive'
        f = [int(data[pos + 6 + 8*i:pos + 14 + 8*i], 16) for i in range(13)]
        name = data[pos + 110:pos + 110 + f[11] - 1].decode()
        off = (pos + 110 + f[11] + 3) & ~3
        if name == 'TRAILER!!!':
            return out
        assert not PurePosixPath(name).is_absolute() and '..' not in PurePosixPath(name).parts
        assert name not in out and off + f[6] <= len(data)
        out[name] = (f, data[off:off + f[6]])
        pos = (off + f[6] + 3) & ~3
    raise ValueError('missing cpio trailer')


def cpio_write(entries):
    out = bytearray()
    all_entries = dict(entries)
    all_entries['TRAILER!!!'] = ([0] * 13, b'')
    for ino, (name, (fields, data)) in enumerate(all_entries.items(), 1):
        f = list(fields)
        f[0], f[6], f[11] = ino, len(data), len(name.encode()) + 1
        f[4] = 2 if stat.S_ISDIR(f[1]) else 1
        f[5] = f[12] = 0
        out += b'070701' + b''.join(f'{x:08x}'.encode() for x in f)
        out += name.encode() + b'\0'
        out += bytes(-len(out) % 4)
        out += data
        out += bytes(-len(out) % 4)
    return bytes(out)


def replace(entries, name, data, mode=None):
    fields = list(entries[name][0]) if name in entries else [0] * 13
    if mode is not None:
        fields[1], fields[2], fields[3] = mode, 0, 0
    entries[name] = (fields, data)


def strip_symbols(data, directory):
    """Remove non-runtime symbols, verifying every loaded byte except ELF metadata."""
    src, dst = Path(directory) / 'input.elf', Path(directory) / 'stripped.elf'
    assert data[:6] == b'\x7fELF\x02\x01' and struct.unpack_from('<H', data, 18)[0] == 183
    src.write_bytes(data)
    # Host binutils supports the generic ELF64 container. Restore e_machine,
    # which its generic backend otherwise sets to EM_NONE; no relocations run.
    subprocess.run(['objcopy', '-I', 'elf64-little', '-O', 'elf64-little',
                    '--strip-unneeded', str(src), str(dst)], check=True)
    result = bytearray(dst.read_bytes())
    result[18:20] = data[18:20]
    assert result[:40] == data[:40] and result[48:60] == data[48:60]
    phoff = struct.unpack_from('<Q', data, 32)[0]
    phent, phnum = struct.unpack_from('<HH', data, 54)
    assert phent == 56
    assert result[phoff:phoff + phent*phnum] == data[phoff:phoff + phent*phnum]
    for i in range(phnum):
        kind, _, off, _, _, size, _, _ = struct.unpack_from('<II6Q', data, phoff + i*phent)
        if kind == 1:  # PT_LOAD: only section-header metadata in ELF header may change.
            assert result[max(off, 64):off + size] == data[max(off, 64):off + size]
    return bytes(result)


def build(stock, recovery, key, strip=False):
    assert not struct.unpack_from('<I', stock, 1632)[0], 'stock boot has recovery DTBO'
    header, (kernel, _, dtb) = parts(stock)
    _, (_, ramdisk, _) = parts(recovery)
    entries = cpio_read(gzip.decompress(ramdisk))
    key = key.strip()
    assert key and b'\n' not in key and len(key.split()[0]) > 500, 'expected one ADB public key'

    seeds = ['system/bin/' + n for n in
             ('init', 'adbd', 'linker64', 'sh', 'toybox', 'servicemanager')]
    needed = set()
    with tempfile.TemporaryDirectory() as temp:
        pending = list(seeds)
        while pending:
            name = pending.pop()
            if name in needed:
                continue
            assert name in entries, f'missing {name}'
            fields, data = entries[name]
            assert data.startswith(b'\x7fELF') and stat.S_ISREG(fields[1]), name
            needed.add(name)
            path = Path(temp) / 'elf'
            path.write_bytes(data)
            text = subprocess.check_output(['readelf', '-d', str(path)], text=True)
            pending += ['system/lib64/' + n for n in
                        re.findall(r'\(NEEDED\).*Shared library: \[([^]]+)\]', text)]

    stripped = {}
    if strip:
        with tempfile.TemporaryDirectory() as temp:
            for name in sorted(needed):
                try:
                    stripped[name] = strip_symbols(entries[name][1], temp)
                except AssertionError:
                    # Keep the exact original if generic objcopy changes any
                    # program header or loaded byte beyond ELF section metadata.
                    pass
    kept = {}
    for name, (fields, data) in entries.items():
        # Keep policy, linker/init configuration and directory/symlink layout.
        # Libraries/executables are limited to the dependency closure.
        if name.startswith(('res/', 'vendor/', 'product/', 'odm/', 'system_ext/')):
            continue
        if name.startswith(('system/bin/', 'system/lib64/')) and stat.S_ISREG(fields[1]):
            if name not in needed:
                continue
        if name.startswith('system/lib/'):
            continue
        kept[name] = (fields, stripped.get(name, data))

    # IsRecoveryMode() tests this pathname, even though no recovery service runs.
    replace(kept, 'system/bin/recovery', b'RAM recovery marker\n', stat.S_IFREG | 0o644)
    replace(kept, 'adb_keys', key + b'\n', stat.S_IFREG | 0o600)
    replace(kept, 'nord2-rescue', b'ADB RAM recovery; userdata is not mounted\n',
            stat.S_IFREG | 0o444)

    rc_path = 'system/etc/init/hw/init.rc'
    rc = kept[rc_path][1].decode()
    for name in ('recovery', 'charger', 'fastbootd'):
        rc, n = re.subn(r'^service ' + name + r' [^\n]*\n(?:[ \t]+[^\n]*\n|\n)*',
                        '', rc, flags=re.M)
        assert n == 1, name
    # Use the controller observed on this phone, without a recovery UI to
    # discover/set it. Set root before starting adbd; keep auth enabled.
    rc = rc.replace('on boot\n', '''on boot
    restorecon /adb_keys
    setprop sys.usb.controller usb0
    setprop service.adb.root 1
    setprop sys.usb.config adb
    write /dev/kmsg "NORD2 RESCUE: ADB RAM recovery; no userdata mount"
''', 1)
    assert not re.search(r'^\s*(?:mount_all|format|swapon_all)\b', rc, re.M)
    replace(kept, rc_path, rc.encode())
    assert 'system/etc/init/servicemanager.recovery.rc' in kept
    prop = kept['prop.default'][1].decode()
    for name, value in (('ro.adb.secure', '1'), ('ro.secure', '1'),
                        ('ro.debuggable', '1'), ('persist.sys.usb.config', 'adb')):
        prop = re.sub(r'^' + re.escape(name) + r'=.*\n', '', prop, flags=re.M)
        prop += f'\n{name}={value}\n'
    replace(kept, 'prop.default', prop.encode())
    # All retained links needed for init/ueventd and the shell must resolve.
    for name in ('init', 'system/bin/ueventd'):
        assert name in kept, name
        data = kept[name][1]
        if stat.S_ISLNK(kept[name][0][1]):
            target = data.decode()
            target = target.lstrip('/') if target.startswith('/') else str(PurePosixPath(name).parent / target)
            assert target in kept, (name, target)

    raw = cpio_write(kept)
    check = cpio_read(raw)
    assert {n: d for n, (_, d) in check.items()} == {n: d for n, (_, d) in kept.items()}
    compressed = gzip.compress(raw, compresslevel=9, mtime=0)
    new_header = bytearray(header)
    struct.pack_into('<I', new_header, 16, len(compressed))
    h = hashlib.sha1()
    for data in (kernel, compressed, b'', b'', dtb):
        h.update(data)
        h.update(struct.pack('<I', len(data)))
    new_header[576:608] = h.digest().ljust(32, b'\0')
    image = new_header
    for data in (kernel, compressed, dtb):
        image += data + bytes(-len(data) % 2048)
    assert len(image) <= 33554432, 'rescue does not fit boot partition'
    _, (check_kernel, check_rd, check_dt) = parts(image)
    assert check_kernel == kernel and check_dt == dtb and check_rd == compressed
    manifest = dict(bytes=len(image), sha256=hashlib.sha256(image).hexdigest(),
                    stock_sha256=hashlib.sha256(stock).hexdigest(),
                    recovery_sha256=hashlib.sha256(recovery).hexdigest(),
                    ramdisk_bytes=len(compressed), ramdisk_uncompressed=len(raw),
                    symbols_removed_bytes=sum(len(entries[n][1]) - len(d) for n, d in stripped.items()),
                    elf_files=sorted(needed), adb_authenticated=True, userdata_mount_configured=False,
                    hardware_tested=False)
    return bytes(image), manifest


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('stock', type=Path)
    ap.add_argument('recovery', type=Path)
    ap.add_argument('output', type=Path)
    ap.add_argument('--adb-key', required=True, type=Path)
    ap.add_argument('--strip-symbols', action='store_true')
    args = ap.parse_args()
    assert not args.output.exists(), 'output already exists'
    image, manifest = build(args.stock.read_bytes(), args.recovery.read_bytes(),
                            args.adb_key.read_bytes(), args.strip_symbols)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    with args.output.open('xb') as f:
        f.write(image)
    args.output.with_suffix('.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(json.dumps({k: v for k, v in manifest.items() if k != 'elf_files'}, indent=2))


if __name__ == '__main__':
    main()
