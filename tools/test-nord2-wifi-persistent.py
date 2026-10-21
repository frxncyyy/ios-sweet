#!/usr/bin/env python3
"""Exercise the production persistent radio core, transport and PMIC helper.

The PMIC helper lives in the XNU patch; --touch can instead select an applied
source file. Host fixtures never execute native MMIO or SMC instructions.
"""
import argparse
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
HELPERS = ROOT / 'shim/nord2-xnu'
FIXTURES = ROOT / 'tools/fixtures'


def function(source, name):
    start = source.index(name + '(')
    start = source.rfind('static ', 0, start)
    brace = source.index('{', start)
    end, depth = brace + 1, 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--touch', type=Path)
    args = parser.parse_args()
    if args.touch:
        touch = args.touch.read_text()
    else:
        patch = (ROOT / 'xnu/patches/0082-nord2-early-probe.patch').read_text()
        touch = '\n'.join(line[1:] for line in patch.splitlines()
                          if line.startswith('+') and not line.startswith('+++'))
    fixture = (FIXTURES / 'nord2-wifi-persistent-pmic.c').read_text()
    assert function(touch, 'npa_pmic_source_enable') == function(fixture, 'npa_pmic_source_enable')
    with tempfile.TemporaryDirectory(prefix='nord2-wifi-persistent-') as directory:
        cases = [('core', ['wifi_persistent.c']),
                 ('adapter', ['wifi_persistent.c', 'wifi_persistent_adapter.c']),
                 ('pmic', [])]
        for case, files in cases:
            binary = Path(directory) / case
            command = ['cc', '-std=c11', '-Wall', '-Wextra', '-Werror', '-O1',
                       '-g', '-fsanitize=address,undefined', '-I' + str(HELPERS),
                       str(FIXTURES / ('nord2-wifi-persistent-' + case + '.c'))]
            command += [str(HELPERS / name) for name in files]
            if files:
                command.append(str(HELPERS / 'wifi_common.c'))
            subprocess.run(command + ['-o', str(binary)], check=True)
            subprocess.run([str(binary)], check=True)
    print('PASS production persistent core, native transport and exact PMIC helper')


if __name__ == '__main__':
    main()
