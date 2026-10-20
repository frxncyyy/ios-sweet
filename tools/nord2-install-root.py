#!/usr/bin/env python3
"""Install the frozen Nord2 iOS root through authenticated ADB RAM recovery.

Default: read-only preflight. --write-userdata requires prior human approval.
Only the image-sized prefix of userdata is written; GPT and other partitions
are never written. A full device readback hash is required before boot testing.
Keep the phone in ADB rescue after installation; ordinary Android cannot use
the replacement HFS filesystem. This tool never reboots or flashes boot.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import subprocess
import time

ROOT_SIZE = 17179869184
ROOT_SHA = "8b1d9de6775674afe790a161419ea23e824c5e045f21a66c6fc886125c991cb3"
PART_START = 25231360  # Linux sysfs uses 512-byte sectors.
PART_SECTORS = 224657344
DEVICE = "/dev/block/sdc57"
BY_NAME = "/dev/block/by-name/userdata"
RESCUE_MARKER = "ADB RAM recovery; userdata is not mounted"
BACKUPS = {
    "backup/nord2/boot.img":
        "04e84c4a14e5c3cb77d0e587da93b5a7e720e8c5b2a3a51ab3ef4cda746e1f01",
    "research/nord2/rescue/nord2-adb-rescue.img":
        "d78bf9fe720a1d293b863ea0a8b30e0df4b5eafb097b1343a3c30e7c3d03bac5",
}


def digest(file):
    h = hashlib.sha256()
    for chunk in iter(lambda: file.read(4 << 20), b""):
        h.update(chunk)
    return h.hexdigest()


def shell(adb, serial, command, timeout=30):
    # Shell v2 (-T) preserves stdin bytes and propagates remote exit status.
    return subprocess.run([adb, "-s", serial, "shell", "-T", command],
                          capture_output=True, text=True, check=True,
                          timeout=timeout).stdout


def preflight(adb, serial):
    if shell(adb, serial, "id -u").strip() != "0":
        raise RuntimeError("ADB must run as root in the tested RAM recovery")
    if shell(adb, serial, "cat /nord2-rescue").strip() != RESCUE_MARKER:
        raise RuntimeError("Tested RAM recovery marker is missing")
    if shell(adb, serial, f"readlink -f {BY_NAME}").strip() != DEVICE:
        raise RuntimeError("Unexpected userdata device")
    start = int(shell(adb, serial, "cat /sys/class/block/sdc57/start").strip())
    sectors = int(shell(adb, serial, "cat /sys/class/block/sdc57/size").strip())
    size = int(shell(adb, serial, f"blockdev --getsize64 {DEVICE}").strip())
    if (start, sectors, size) != (PART_START, PART_SECTORS, PART_SECTORS * 512):
        raise RuntimeError("Unexpected userdata geometry")
    mounts = shell(adb, serial, "cat /proc/mounts")
    if any(line.startswith("/dev/block") or " /data " in line
           for line in mounts.splitlines()):
        raise RuntimeError("A block filesystem or /data is mounted")
    shell(adb, serial, "set -o pipefail; command -v dd; command -v sha256sum")
    return dict(userdata_device=DEVICE, start_sector=start, sectors=sectors,
                partition_bytes=size, block_filesystems_mounted=False)


def write_root(adb, serial, file, output):
    # No dd count: a pipe may produce short reads. EOF is the exact verified
    # image end. nocreat/notrunc prevent creating a wrong path or truncation.
    command = (f"dd of={BY_NAME} bs=1048576 "
               "conv=nocreat,notrunc,fsync status=none")
    file.seek(0)
    with (output / "write.stdout").open("xb") as stdout, \
         (output / "write.stderr").open("xb") as stderr:
        proc = subprocess.Popen([adb, "-s", serial, "shell", "-T", command],
                                stdin=file, stdout=stdout, stderr=stderr)
        last = -1
        deadline = time.monotonic() + 1800
        while proc.poll() is None:
            if time.monotonic() >= deadline:
                proc.terminate()
                try:
                    proc.wait(timeout=10)
                except subprocess.TimeoutExpired:
                    proc.kill()
                    proc.wait()
                raise RuntimeError("userdata transfer timed out; image may be partial")
            sent = os.lseek(file.fileno(), 0, os.SEEK_CUR)
            gib = sent // (1 << 30)
            if gib != last:
                print(f"ADB consumed {sent}/{ROOT_SIZE} image bytes", flush=True)
                last = gib
            time.sleep(1)
        if proc.returncode:
            raise RuntimeError("userdata write failed; retain RAM recovery and inspect write.stderr")
        if os.lseek(file.fileno(), 0, os.SEEK_CUR) != ROOT_SIZE:
            raise RuntimeError("ADB did not consume the complete image")


def verify_device(adb, serial, output):
    # SHA match also rejects short reads. pipefail exposes a dd I/O failure.
    command = (f"set -o pipefail; dd if={BY_NAME} bs=1048576 "
               f"count={ROOT_SIZE // 1048576} status=none | sha256sum")
    result = shell(adb, serial, command, timeout=1800)
    (output / "device-readback.sha256").write_text(result)
    if result.split()[:1] != [ROOT_SHA]:
        raise RuntimeError("Full userdata readback hash mismatch; do not boot iOS")


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("image", type=Path)
    ap.add_argument("output", type=Path, help="new private evidence directory")
    ap.add_argument("--serial", required=True)
    ap.add_argument("--adb", default="/home/frxncy/Android/Sdk/platform-tools/adb")
    mode = ap.add_mutually_exclusive_group()
    mode.add_argument("--write-userdata", action="store_true")
    mode.add_argument("--verify-only", action="store_true")
    args = ap.parse_args()
    if args.output.exists():
        ap.error("Evidence output already exists")
    for name, expected in BACKUPS.items():
        with Path(name).open("rb") as file:
            if digest(file) != expected:
                raise RuntimeError(f"Recovery backup hash mismatch: {name}")
    with args.image.open("rb") as file:
        if os.fstat(file.fileno()).st_size != ROOT_SIZE or digest(file) != ROOT_SHA:
            raise RuntimeError("Not the frozen, verified iOS root image")
        report = preflight(args.adb, args.serial)
        args.output.mkdir(parents=True)
        report.update(image=str(args.image), image_bytes=ROOT_SIZE,
                      image_sha256=ROOT_SHA, write_requested=args.write_userdata,
                      userdata_write_started=False, userdata_write_completed=False,
                      full_readback_verified=False)
        evidence = args.output / "result.json"
        evidence.write_text(json.dumps(report, indent=2) + "\n")
        if args.write_userdata:
            # Repeat live checks immediately before opening the target.
            preflight(args.adb, args.serial)
            report["userdata_write_started"] = True
            evidence.write_text(json.dumps(report, indent=2) + "\n")
            write_root(args.adb, args.serial, file, args.output)
            report["userdata_write_completed"] = True
            evidence.write_text(json.dumps(report, indent=2) + "\n")
        if args.write_userdata or args.verify_only:
            verify_device(args.adb, args.serial, args.output)
            report["full_readback_verified"] = True
        evidence.write_text(json.dumps(report, indent=2) + "\n")
        print(json.dumps(report, indent=2))


if __name__ == "__main__":
    main()
