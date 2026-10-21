#!/usr/bin/env python3
"""Offline legacy EMI collector tests; native SMC is never executed."""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
CANON = ROOT / 'shim/nord2-xnu'
ACTIVE = ROOT / 'research/ci-39158169212/buildtree/xnu/bsd/dev'
for name in ('wifi_emi_legacy', 'wifi_emi_probe'):
    assert (ACTIVE / (name + '.h')).read_bytes() == (CANON / (name + '.h')).read_bytes()
    assert (ACTIVE / (name + '.c')).read_text() == (
        '#include <arm64/sweet_early.h>\n#if defined(NORD2)\n' +
        (CANON / (name + '.c')).read_text() + '\n#endif /* NORD2 */\n')
with tempfile.TemporaryDirectory() as directory:
    exe = Path(directory) / 'collector'
    subprocess.run([
        'clang', '-std=c11', '-O1', '-g', '-Wall', '-Wextra', '-Werror',
        '-fsanitize=address,undefined', '-fno-omit-frame-pointer',
        '-DWIFI_EMI_LEGACY_NO_SMC', '-I' + str(CANON),
        str(CANON / 'wifi_emi_legacy.c'),
        str(ROOT / 'tools/fixtures/nord2-wifi-emi-legacy.c'), '-o', str(exe)
    ], check=True)
    subprocess.run([str(exe)], check=True)
    subprocess.run([
        'clang', '--target=aarch64-none-elf', '-std=c11', '-O2', '-Wall',
        '-Wextra', '-Werror', '-ffreestanding', '-c',
        str(CANON / 'wifi_emi_legacy.c'), '-o', str(Path(directory) / 'native.o')
    ], check=True)
print('PASS128-offset allowlist/raw results/1280 first-failure cases/ARM64 compile; no native SMC')
for suite in ('test-nord2-wifi-emi-probe.py', 'test-nord2-wifi-emi-legacy-probe.py'):
    subprocess.run(['python3', str(ROOT / 'tools' / suite)], check=True)
