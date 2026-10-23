#!/usr/bin/env python3
"""Test current cold-reset/profile source; optional private --board enables tuning tests."""
import argparse,subprocess,tempfile
from pathlib import Path
R=next(p for p in Path(__file__).resolve().parents if (p/'shim/nord2-xnu/audio_hw.c').exists()and(p/'tools/nord2-audio-check.py').exists())
D=Path(__file__).resolve().parent
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--source-dir',type=Path,default=R/'shim/nord2-xnu');p.add_argument('--board',type=Path);a=p.parse_args();S=a.source_dir.resolve()
cases=[('test-i2cr.c',['audio_i2c8.c'])]
if a.board:cases += [('test-amp-parity.c',['audio_amp.c','audio_board.c']),('test-amp-model.c',['audio_i2c8.c','audio_amp.c','audio_board.c'])]
with tempfile.TemporaryDirectory(prefix='nord2-amp-parity-')as temp:
 for name,sources in cases:
  exe=Path(temp)/name
  subprocess.run(['clang','-DNORD2','-std=c11','-Wall','-Wextra','-Werror','-O1','-g','-fsanitize=address,undefined','-fno-sanitize-recover=all','-include',str(S/'audio_i2c8.h'),'-I'+str(R/'shim/nord2-xnu'),'-I'+str(R/'tests/nord2-audio'),*[str(S/x)for x in sources],str(D/name),'-o',str(exe)],check=True)
  subprocess.run([str(exe)]+([str(a.board.resolve())]if a.board else[]),check=True)
