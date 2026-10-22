#!/usr/bin/env python3
"""Host-only checks of canonical iOS Wi-Fi admission and scalar contracts."""

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cc', default='clang', help='ASan/UBSan-capable C compiler')
    parser.add_argument('--only', choices=('all', 'policy', 'scalar'), default='all')
    parser.add_argument('--report', type=Path, help='Optional JSON hashes and summaries')
    args = parser.parse_args()
    root = Path(__file__).resolve().parents[1]
    source = root / 'shim/nord2-xnu'
    fixtures = root / 'tools/fixtures/nord2-wifi-ios-contract'
    suites = {
        'policy': ('wifi_ios_policy.c', 'wifi_ios_rates.c', 'wifi_ios_mapping.c'),
        'scalar': ('wifi_ios_scalar.c',),
    }
    selected = tuple(suites) if args.only == 'all' else (args.only,)
    # Record the complete local include closure used by these canonical TUs.
    dependencies = set()

    def collect(path):
        if path in dependencies:
            return
        dependencies.add(path)
        for name in re.findall(r'^\s*#\s*include\s*"([^"]+)"', path.read_text(), re.M):
            header = path.parent / name
            if not header.is_file():
                header = source / name
            if not header.is_file():
                raise RuntimeError('Missing local dependency: ' + str(header))
            collect(header)

    for name in selected:
        for filename in suites[name]:
            collect(source / filename)
        collect(fixtures / ('test-' + name + '.c'))
    dependencies.add(Path(__file__).resolve())
    dependencies.add(fixtures / 'README.md')

    def hashes():
        return {str(p.relative_to(root)): hashlib.sha256(p.read_bytes()).hexdigest()
                for p in sorted(dependencies)}

    before = hashes()
    report = {'status': 'PASS_SYNTHETIC_ONLY', 'sanitizers': ['address', 'undefined'],
              'files': before, 'summaries': [], 'commands': []}
    env = os.environ.copy()
    env['ASAN_OPTIONS'] = 'halt_on_error=1:abort_on_error=1'
    env['UBSAN_OPTIONS'] = 'halt_on_error=1:print_stacktrace=1'
    with tempfile.TemporaryDirectory(prefix='nord2-ios-contract-') as temporary:
        for name in selected:
            executable = Path(temporary) / name
            command = [args.cc, '-std=c11', '-O1', '-g', '-Wall', '-Wextra', '-Werror',
                       '-Wno-misleading-indentation', '-fsanitize=address,undefined',
                       '-fno-sanitize-recover=all', '-fno-omit-frame-pointer',
                       '-I' + str(source), str(fixtures / ('test-' + name + '.c'))]
            command += [str(source / f) for f in suites[name]]
            command += ['-o', str(executable)]
            subprocess.run(command, check=True, env=env)
            run = subprocess.run([str(executable)], env=env,
                                 text=True, capture_output=True)
            print(run.stdout, end='')
            if run.stderr:
                print(run.stderr, end='')
            run.check_returncode()
            report['summaries'].append(run.stdout.strip())
            report['commands'].append([part.replace(str(root), '$REPO').replace(
                temporary, '$TEMP') for part in command])
    if before != hashes():
        raise RuntimeError('Source or fixture changed during test')
    if args.report:
        args.report.write_text(json.dumps(report, indent=2) + '\n')


if __name__ == '__main__':
    main()
