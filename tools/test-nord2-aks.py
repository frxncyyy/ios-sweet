#!/usr/bin/env python3
"""Exercise the actual patched keystore TU, without Apple binary artifacts."""
from pathlib import Path
import os
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
FIXTURES = ROOT / "tools/fixtures/nord2-aks"
TARGET = "iokit/Kernel/arm/SweetKeyStore.cpp"
COMPAT = "0083-nord2-keystore-compat.patch"


def selected_patch(path):
    blocks, current = [], []
    # Other patch sections contain legacy byte comments. Decode reversibly;
    # the selected keystore sections are ordinary ASCII source.
    for line in path.read_text(encoding="latin-1").splitlines(keepends=True):
        if line.startswith("--- ") and current:
            blocks.append(current)
            current = []
        current.append(line)
    if current:
        blocks.append(current)
    return "".join("".join(block) for block in blocks
                   if "+++ b/" + TARGET + "\n" in block)


def main():
    with tempfile.TemporaryDirectory(prefix="nord2-aks-") as directory:
        temp = Path(directory)
        source = temp / TARGET
        source.parent.mkdir(parents=True)
        for patch in sorted((ROOT / "xnu/patches").glob("*.patch")):
            if patch.name == COMPAT:
                continue
            fragment = selected_patch(patch)
            if fragment:
                subprocess.run(["patch", "--batch", "--fuzz=0", "-s", "-p1"],
                               input=fragment, text=True, cwd=temp, check=True)
        old = temp / "SweetKeyStore.old.cpp"
        old.write_bytes(source.read_bytes())
        subprocess.run(["patch", "--batch", "--fuzz=0", "-s", "-p1"],
                       input=selected_patch(ROOT / "xnu/patches" / COMPAT),
                       text=True, cwd=temp, check=True)
        includes = temp / "include"
        for header in ("pexpert/arm64/board_config.h", "IOKit/IOService.h",
                       "IOKit/IOUserClient.h", "IOKit/IOKitKeys.h", "IOKit/IOLib.h",
                       "IOKit/IOMemoryDescriptor.h", "libkern/c++/OSString.h", "sys/proc.h"):
            path = includes / header
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text('#include "test-xnu.h"\n')
        for name, actual, defines in (
            ("nord2", source, ["-DNORD2=1"]),
            ("old", old, ["-DNORD2=1", "-DAKS_EXPECT_UNSUPPORTED=1"]),
            ("sweet", source, ["-DAKS_EXPECT_UNSUPPORTED=1"]),
        ):
            executable = temp / name
            subprocess.run([
                "clang++", "-std=c++17", "-O1", "-g", "-Wall", "-Wextra", "-Werror",
                "-fsanitize=address,undefined", "-fno-sanitize-recover=all",
                "-DSWEET=1", *defines, "-I" + str(includes), "-I" + str(FIXTURES),
                '-DAKS_SOURCE="' + str(actual) + '"', str(FIXTURES / "test-handler.cpp"),
                "-o", str(executable),
            ], check=True)
            env = dict(os.environ, ASAN_OPTIONS="detect_leaks=1:halt_on_error=1",
                       UBSAN_OPTIONS="halt_on_error=1:print_stacktrace=1")
            subprocess.run([str(executable)], env=env, check=True)


if __name__ == "__main__":
    main()
