#!/usr/bin/env python3
"""Exercise canonical DMA claim/preflight/native binding without device access."""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / 'shim/nord2-xnu'
FIX = ROOT / 'tools/fixtures/nord2-wifi-dma'
with tempfile.TemporaryDirectory(prefix='nord2-dma-test-') as directory:
    for fixture, sources, args in [
        ('test-fdt.c', ['dma_pool_fdt.c', 'dma_pool_contract.c', 'wifi_rom_fdt.c'],
         [str(ROOT / 'dtb/nord2/live.dtb')]),
        ('test-preflight.c', ['wifi_dma_preflight.c'], []),
        ('test-bind.c', ['pool_probe.c', 'pool_adapter.c', 'wifi_dma_preflight.c',
                         'dma_pool_contract.c'], []),
        ('test-guards.c', ['pool_probe.c', 'pool_adapter.c', 'wifi_dma_preflight.c',
                           'dma_pool_contract.c'], []),
    ]:
        executable = Path(directory) / fixture.removesuffix('.c')
        subprocess.run(['cc', '-std=gnu11', '-O1', '-g', '-Wall', '-Wextra',
                        '-Werror', '-fsanitize=address,undefined',
                        '-fno-omit-frame-pointer', '-I' + str(SRC),
                        str(FIX / fixture), *[str(SRC / s) for s in sources],
                        '-o', str(executable)], check=True)
        subprocess.run([str(executable), *args], check=True)

    # Guard fixtures must keep using the actual ROM receipt predicates.
    real = (SRC / 'wifi_rom_powered.c').read_text()
    fixture = (FIX / 'test-guards.c').read_text()
    for name in ['policy_receipt', 'nrpc_receipt_ok']:
        def body(text):
            start = text.index(name + '(')
            begin = text.index('{', start)
            depth = 1
            end = begin + 1
            while depth:
                depth += (text[end] == '{') - (text[end] == '}')
                end += 1
            return ''.join(text[begin:end].split())
        assert body(real) == body(fixture), name
print('PASS canonical DMA tests; no native hardware or device access')
