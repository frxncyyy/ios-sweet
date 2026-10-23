#!/usr/bin/env python3
"""Extract current audio_device.c start() and run a strict sanitized mock test.

The fixture mocks only kernel context/time and low-level hardware start/stop.
No copied implementation of start() is used. No device/CI action occurs.
"""
import argparse
from pathlib import Path
import re
import subprocess
import tempfile

ROOT = next(p for p in Path(__file__).resolve().parents
            if (p / 'shim/nord2-xnu/audio_device.c').is_file()
            and (p / 'tools/nord2-audio-check.py').is_file())


def extract_start(source):
    matches = list(re.finditer(r'^static\s+int\s+start\s*\([^;{}]*\)\s*\{', source, re.M))
    if len(matches) != 1:
        raise ValueError('expected exactly one static int start() definition')
    begin = matches[0].start()
    at = matches[0].end() - 1
    depth = 0
    state = 'code'
    while at < len(source):
        ch = source[at]
        pair = source[at:at + 2]
        if state == 'code':
            if pair == '/*':
                state = 'comment'
                at += 2
                continue
            if pair == '//':
                state = 'line'
                at += 2
                continue
            if ch in ('"', "'"):
                state = ch
            elif ch == '{':
                depth += 1
            elif ch == '}':
                depth -= 1
                if depth == 0:
                    return source[begin:at + 1] + '\n'
        elif state == 'comment':
            if pair == '*/':
                state = 'code'
                at += 2
                continue
        elif state == 'line':
            if ch == '\n':
                state = 'code'
        elif ch == '\\':
            at += 2
            continue
        elif ch == state:
            state = 'code'
        at += 1
    raise ValueError('unterminated start() definition')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, default=ROOT / 'shim/nord2-xnu/audio_device.c')
    parser.add_argument('--cc', default='clang')
    args = parser.parse_args()
    fixture = Path(__file__).with_name('test-device-deadline.c')
    extracted = extract_start(args.source.read_text())
    with tempfile.TemporaryDirectory(prefix='nord2-test-deadline-') as temp:
        temp = Path(temp)
        (temp / 'audio-start-under-test.inc').write_text(extracted)
        exe = temp / 'test'
        command = [args.cc, '-std=c11', '-O1', '-g', '-Wall', '-Wextra', '-Werror',
                   '-fsanitize=address,undefined', '-fno-sanitize-recover=all',
                   '-I' + str(ROOT / 'shim/nord2-xnu'), '-I' + str(temp),
                   str(fixture), '-o', str(exe)]
        subprocess.run(command, check=True, cwd=temp)
        subprocess.run([str(exe)], check=True, cwd=temp)
    print('PASS current-source diagnostic deadline regression:', args.source)


if __name__ == '__main__':
    main()
