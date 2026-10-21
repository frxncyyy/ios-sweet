#!/usr/bin/env python3
"""Exercise the production AP wake core, transport and actual native binding.

Fixtures inject register I/O and faults; no physical MMIO or SMC is executed.
"""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
HELPERS = ROOT / 'shim/nord2-xnu'
FIXTURES = ROOT / 'tools/fixtures'


def main():
    with tempfile.TemporaryDirectory(prefix='nord2-wifi-wake-') as directory:
        for case in ('core-adapter', 'bind'):
            binary = Path(directory) / case
            command = ['cc', '-std=gnu11', '-Wall', '-Wextra', '-Werror',
                       '-O1', '-g', '-fsanitize=address,undefined',
                       '-I' + str(HELPERS),
                       str(FIXTURES / ('nord2-wifi-wake-' + case + '.c')),
                       str(HELPERS / 'wifi_wake.c'),
                       str(HELPERS / 'wifi_wake_adapter.c'), '-o', str(binary)]
            subprocess.run(command, check=True)
            subprocess.run([str(binary)], check=True)
    print('PASS production AP wake core, transport and native binding')


if __name__ == '__main__':
    main()
