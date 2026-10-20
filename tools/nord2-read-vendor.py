#!/usr/bin/env python3
"""Read vendor/odm out of Android super through the tested RAM ADB rescue.

No phone files are written or mounted. Does not reboot or flash. Output stays
private: it can contain proprietary firmware and device configuration.
Reads AOSP liblp v10 metadata, checks SHA-256 and primary/backup agreement, then
streams only the requested logical partition, with bounded memory use.

Format reference (Apache-2.0, Android Open Source Project):
https://android.googlesource.com/platform/system/core/+/main/fs_mgr/liblp/include/liblp/metadata_format.h
"""
import argparse
import hashlib
import io
import json
from pathlib import Path
import re
import selectors
import shutil
import struct
import subprocess
import tempfile
import time

SUPER = "/dev/block/by-name/super"
SECTOR = 512


def require(ok, message):
    if not ok:
        raise ValueError(message)


def name(raw):
    text = raw.rstrip(b"\0").decode("ascii")
    require(re.fullmatch(r"[A-Za-z0-9_]+", text), "Invalid LP name")
    return text


def geometry(data):
    require(len(data) == 4096, "Truncated geometry")
    magic, size = struct.unpack_from("<II", data)
    require(magic == 0x616c4467 and size == 52, "Unknown LP geometry")
    require(hashlib.sha256(data[:8] + bytes(32) + data[40:size]).digest()
            == data[8:40], "Geometry checksum mismatch")
    maximum, slots, block = struct.unpack_from("<III", data, 40)
    require(128 <= maximum <= 1024 * 1024 and maximum % SECTOR == 0,
            "Unsupported metadata size")
    require(1 <= slots <= 3 and block in (512, 1024, 2048, 4096),
            "Unsupported LP geometry")
    return {"maximum": maximum, "slots": slots, "block": block}


def metadata(data, geo, super_bytes):
    require(len(data) == geo["maximum"], "Truncated metadata")
    magic, major, minor, hs = struct.unpack_from("<IHHI", data)
    require(magic == 0x414c5030 and major == 10 and minor in (0, 1, 2),
            "Unsupported LP version")
    require(hs == (256 if minor == 2 else 128), "Unsupported header size")
    require(hashlib.sha256(data[:12] + bytes(32) + data[44:hs]).digest()
            == data[12:44], "Header checksum mismatch")
    ts = struct.unpack_from("<I", data, 44)[0]
    require(hs + ts <= len(data), "Tables exceed metadata")
    tables = data[hs:hs + ts]
    require(hashlib.sha256(tables).digest() == data[48:80], "Tables checksum mismatch")
    if hs == 256:
        require(struct.unpack_from("<I", data, 128)[0] == 0,
                "Virtual A/B or overlays require separate snapshot handling")
    entries, intervals = [], []
    for i, expected in enumerate((52, 24, 48, 64)):
        off, count, size = struct.unpack_from("<III", data, 80 + 12 * i)
        end = off + count * size
        require(size == expected and end <= ts and count < 4096, "Invalid table bounds")
        require(not count or all(end <= a or off >= b for a, b in intervals),
                "Overlapping tables")
        if count:
            intervals.append((off, end))
        entries.append([tables[off+j*size:off+(j+1)*size] for j in range(count)])
    partitions, extents, groups, devices = entries
    require(len(devices) == 1, "Only single-super layouts are supported")
    first_sector, _, _, dev_size, dev_name, flags = struct.unpack("<QIIQ36sI", devices[0])
    require(name(dev_name) == "super" and flags == 0 and dev_size == super_bytes,
            "Super identity or size mismatch")
    minimum = 12288 + geo["maximum"] * geo["slots"] * 2
    require(minimum <= first_sector * SECTOR < super_bytes, "Invalid first data sector")
    result, allocated, used_extents = {}, [], set()
    for raw in partitions:
        pn, attr, first, count, group = struct.unpack("<36sIIII", raw)
        pn = name(pn)
        require(pn not in result and attr & ~5 == 0, "Duplicate/disabled/slot-suffixed partition")
        require(first + count <= len(extents) and group < len(groups), "Invalid extent/group index")
        mapped = []
        for index in range(first, first + count):
            require(index not in used_extents, "Shared extent entry")
            used_extents.add(index)
            sectors, kind, target, source = struct.unpack("<QIQI", extents[index])
            require(sectors > 0 and kind in (0, 1), "Invalid extent")
            length = sectors * SECTOR
            if kind == 0:
                start, end = target * SECTOR, (target + sectors) * SECTOR
                require(source == 0 and target >= first_sector and end <= super_bytes,
                        "Extent outside super")
                require(all(end <= a or start >= b for a, b in allocated),
                        "Overlapping physical extents")
                allocated.append((start, end))
                mapped.append({"offset": start, "bytes": length})
            else:
                require(target == source == 0, "Invalid zero extent")
                mapped.append({"offset": None, "bytes": length})
        result[pn] = mapped
    return result, hashlib.sha256(data[:hs + ts]).hexdigest()


class Rescue:
    def __init__(self, adb, serial):
        self.prefix = [adb, "-s", serial]

    def text(self, *args):
        return subprocess.check_output(self.prefix + ["exec-out", *args],
                                       timeout=20).decode().strip()

    def check(self):
        require(self.text("cat", "/nord2-rescue") ==
                "ADB RAM recovery; userdata is not mounted", "Not the tested RAM rescue")
        require(self.text("id", "-u") == "0", "Rescue adbd must be root")
        mounts = self.text("cat", "/proc/mounts")
        require(not any(re.search(r"\s/(data|userdata|vendor|odm)\s", l)
                        or "sdc57" in l or "by-name/userdata" in l
                        for l in mounts.splitlines()), "Unexpected persistent mount")
        size = int(self.text("blockdev", "--getsize64", SUPER))
        require(1024**3 <= size <= 32 * 1024**3 and size % 4096 == 0,
                "Unexpected super size")
        return size

    def read(self, offset, length, sink):
        require(offset >= 0 and length > 0 and (offset | length) % SECTOR == 0,
                "Unaligned read")
        bs = 4096 if (offset | length) % 4096 == 0 else SECTOR
        command = self.prefix + ["exec-out", "dd", f"if={SUPER}", f"bs={bs}",
                                 f"skip={offset//bs}", f"count={length//bs}"]
        # No of=, shell, mount, dmsetup, fsck, or phone write command.
        with tempfile.TemporaryFile() as err:
            proc = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=err, bufsize=0)
            received, progress, last = 0, 0, time.monotonic()
            try:
                with selectors.DefaultSelector() as poll:
                    poll.register(proc.stdout, selectors.EVENT_READ)
                    while True:
                        if not poll.select(30):
                            raise TimeoutError("ADB read stalled for 30 seconds")
                        chunk = proc.stdout.read(1024 * 1024)
                        if not chunk:
                            break
                        received += len(chunk)
                        require(received <= length, "ADB returned excess data")
                        sink.write(chunk)
                        if received - progress >= 64 * 1024**2 and time.monotonic() - last >= 2:
                            print(f"Read {received}/{length} bytes", flush=True)
                            progress, last = received, time.monotonic()
                code = proc.wait(timeout=10)
                err.seek(0)
                diagnostic = err.read(4096).decode(errors="replace")
                require(code == 0 and received == length,
                        f"Incomplete ADB read: {received}/{length}, exit={code}: {diagnostic}")
            finally:
                if proc.poll() is None:
                    proc.kill()
                    proc.wait()
                proc.stdout.close()

    def bytes(self, offset, length):
        require(length <= 1024 * 1024, "Metadata read too large")
        sink = io.BytesIO()
        self.read(offset, length, sink)
        return sink.getvalue()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--adb", default="adb")
    parser.add_argument("--serial", required=True)
    parser.add_argument("--out", type=Path, required=True)
    parser.add_argument("--slot", type=int, default=0)
    parser.add_argument("--partition", choices=("vendor", "odm", "vendor_a", "vendor_b",
                                                "odm_a", "odm_b"))
    args = parser.parse_args()
    rescue = Rescue(args.adb, args.serial)
    super_bytes = rescue.check()
    args.out.mkdir(parents=True, exist_ok=False)
    g0, g1 = (rescue.bytes(off, 4096) for off in (4096, 8192))
    geo = geometry(g0)
    require(geo == geometry(g1), "Geometry copies disagree")
    require(0 <= args.slot < geo["slots"], "Invalid slot")
    metadata_copies, parsed, fingerprints = [], [], []
    for base in (12288, 12288 + geo["slots"] * geo["maximum"]):
        raw = rescue.bytes(base + args.slot * geo["maximum"], geo["maximum"])
        parts, fingerprint = metadata(raw, geo, super_bytes)
        metadata_copies.append(raw)
        parsed.append(parts)
        fingerprints.append(fingerprint)
    require(fingerprints[0] == fingerprints[1], "Metadata copies disagree")
    for label, data in zip(("geometry-primary", "geometry-backup", "metadata-primary",
                            "metadata-backup"), (g0, g1, *metadata_copies)):
        (args.out / (label + ".bin")).write_bytes(data)
    result = {"super_bytes": super_bytes, "geometry": geo, "slot": args.slot,
              "partitions": parsed[0], "metadata_sha256": fingerprints[0],
              "device_writes": False}
    (args.out / "metadata.json").write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps(result, indent=2), flush=True)
    if not args.partition:
        return
    require(args.partition in parsed[0], "Requested partition missing")
    extents = parsed[0][args.partition]
    size = sum(e["bytes"] for e in extents)
    require(0 < size <= 4 * 1024**3, "Vendor/odm size exceeds collection limit")
    require(shutil.disk_usage(args.out).free > size + 256 * 1024**2, "Insufficient host space")
    path = args.out / (args.partition + ".img.partial")
    with path.open("xb") as output:
        for extent in extents:
            if extent["offset"] is None:
                output.seek(extent["bytes"], 1)
            else:
                rescue.read(extent["offset"], extent["bytes"], output)
        output.truncate(size)
    # Recheck metadata so an unexpected state change cannot silently succeed.
    require(rescue.bytes(12288 + args.slot * geo["maximum"], geo["maximum"])
            == metadata_copies[0], "Metadata changed during read")
    with path.open("rb") as data:
        digest = hashlib.file_digest(data, "sha256").hexdigest()
    final = path.with_suffix("")
    path.rename(final)
    result.update(image=final.name, bytes=size, sha256=digest, complete=True)
    (args.out / "result.json").write_text(json.dumps(result, indent=2) + "\n")
    print(json.dumps({"image": str(final), "bytes": size, "sha256": digest}), flush=True)


if __name__ == "__main__":
    main()
