#!/usr/bin/env python3
"""Reject the RCpc2 instructions that faulted in the Nord2 two-CPU probe.

This is a targeted regression guard, not a complete CPU compatibility test.
Disassemble instruction sections, with RCpc2 decoding explicitly enabled:
older objdump defaults otherwise print these instructions as <unknown>.
"""
import argparse
from pathlib import Path
import re
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('kernel', type=Path)
    parser.add_argument('--objdump', type=Path, default=(
        Path(__file__).resolve().parents[2] / 'toolchains/proton-clang/bin/llvm-objdump'))
    args = parser.parse_args()
    result = subprocess.run([
        str(args.objdump), '-d', '--no-show-raw-insn', '--mattr=+rcpc,+rcpc-immo',
        str(args.kernel),
    ], capture_output=True, text=True, check=True)
    # Fail if the decoder did not accept the requested feature instead of
    # silently treating unsupported instructions as unknown.
    if result.stderr.strip():
        sys.exit(result.stderr)
    instructions = re.findall(r'^\s*[0-9a-f]+:\s+\S.*$', result.stdout, re.M)
    if not instructions:
        sys.exit('No instructions decoded; RCpc2 check cannot run')
    bad = [line for line in instructions if re.search(
        r':\s+(?:ldapur(?:b|h|sb|sh|sw)?|stlur(?:b|h)?)\s', line)]
    if bad:
        print(f'FAIL: {len(bad)} RCpc2 instructions in {args.kernel}', file=sys.stderr)
        print('\n'.join(bad[:20]), file=sys.stderr)
        return 1
    print(f'ok: no LDAPUR/STLUR in {len(instructions)} decoded instructions')
    return 0


if __name__ == '__main__':
    sys.exit(main())
