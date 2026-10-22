#!/usr/bin/env python3
"""Bounded numeric radio diagnostics; no device or firmware access."""
from pathlib import Path
import argparse, json, runpy, subprocess, tempfile

R = Path(__file__).resolve().parents[1]
P = R / 'tools/fixtures/nord2-wifi-station-diag'
S = R / 'shim/nord2-xnu'
ap = argparse.ArgumentParser()
ap.add_argument('--log-dir', type=Path)
args = ap.parse_args()
if args.log_dir:
    args.log_dir.mkdir(parents=True, exist_ok=True)
with tempfile.TemporaryDirectory(prefix='nord2-station-diag-') as td:
    for name in ('test', 'test-budget'):
        cmd = ['clang', '-std=c11', '-Wall', '-Wextra', '-Werror',
               '-fsanitize=address,undefined', '-fno-sanitize-recover=all',
               '-I' + str(S), str(P / (name + '.c'))]
        if name == 'test':
            cmd.append(str(S / 'wifi_station_diag.c'))
        binary = Path(td) / name
        subprocess.run(cmd + ['-o', str(binary)], check=True)
        result = subprocess.run([str(binary)], capture_output=True, text=True, check=True)
        if args.log_dir:
            (args.log_dir / (name + '.log')).write_text(result.stdout + result.stderr)
        print(result.stdout.strip())

parse = runpy.run_path(str(R / 'tools/nord2-wifi-station-diag.py'))['parse']
base = 'NORD2 WIFI NSTDIAG: n=1 g=1 t=2 k=5 a=99 b=2 c=3 d=138 e=1 f=95 h=1 i=0'
assert parse(base)['m1_public_class'] == 1
negative = [base + ' secret=0', base.replace('n=1', 'n=0'),
            base.replace('k=5', 'k=7'), base.replace('e=1', 'e=2'),
            base.replace('h=1', 'h=2'), base.replace('g=1', 'g=18446744073709551616'),
            base + '\n' + base, base.replace('i=0', 'i=1')]
for value in negative:
    try:
        parse(value)
    except ValueError:
        pass
    else:
        raise AssertionError('Invalid diagnostic row accepted')
report = {'parser_positive': 1, 'parser_negative': len(negative),
          'hardware_claim': False, 'readiness_claim': False}
if args.log_dir:
    (args.log_dir / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
print('PASS parser: one positive, eight negative; no readiness inference')
