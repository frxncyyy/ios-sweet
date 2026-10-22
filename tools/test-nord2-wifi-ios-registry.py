#!/usr/bin/env python3
"""Test actual registry code against the SystemConfiguration discovery contract."""
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
SOURCE = ROOT / "shim/nord2-xnu"
FIXTURE = ROOT / "tools/fixtures/nord2-wifi-ios-registry"
with tempfile.TemporaryDirectory(prefix="nord2-ios-registry-") as temporary:
    root = Path(temporary)
    arm = root / "iokit/Kernel/arm"
    bsd = root / "bsd/dev"
    arm.mkdir(parents=True)
    bsd.mkdir(parents=True)
    for header in SOURCE.glob("*.h"):
        shutil.copy2(header, bsd / header.name)
    shutil.copy2(SOURCE / "wifi_ios_registry.cpp", arm)
    for fixture in FIXTURE.iterdir():
        shutil.copy2(fixture, arm / fixture.name)
    executable = root / "test"
    subprocess.run([
        "clang++", "-std=c++17", "-Wall", "-Wextra", "-Werror",
        "-fsanitize=address,undefined", "-fno-sanitize-recover=all",
        str(arm / "test-registry.cpp"), "-o", str(executable),
    ], check=True)
    subprocess.run([str(executable)], check=True)
