#!/usr/bin/env python3
"""Extract actual startup capture/report functions and test their bounded contract."""
import argparse,subprocess,tempfile
from pathlib import Path
R=next(p for p in Path(__file__).resolve().parents if(p/'shim/nord2-xnu/audio_hw.c').exists()and(p/'tools/nord2-audio-check.py').exists())
D=Path(__file__).resolve().parent
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--source-dir',type=Path,default=R/'shim/nord2-xnu');a=p.parse_args();S=a.source_dir.resolve()
def extract(source,name):
 start=source.index(name);at=source.index('{',start);depth=1;end=at+1
 while depth:
  if source[end]=='{':depth+=1
  elif source[end]=='}':depth-=1
  end+=1
 return source[start:end]+'\n'
hw=(S/'audio_hw.c').read_text();dev=(S/'audio_device.c').read_text()
start=extract(hw,'int nau_hw_start(')
assert start.index('amp_enable(h,i)')<start.index('amp_observe_startup(h)')<start.rindex('lease(h,0)')
assert 'amp_observe_startup('not in extract(hw,'int nau_hw_tick(')
assert dev.count('report_amp_observed();')==1
with tempfile.TemporaryDirectory(prefix='nord2-observed-')as tmp:
 t=Path(tmp);(t/'observed-capture.inc').write_text(extract(hw,'static int amp_observe_startup('));(t/'observed-report.inc').write_text(extract(dev,'static void report_amp_observed('))
 exe=t/'test';subprocess.run(['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-include',str(S/'audio_hw.h'),'-I'+str(R/'shim/nord2-xnu'),'-I'+str(t),str(D/'test-observed.c'),'-o',str(exe)],check=True);subprocess.run([str(exe)],check=True)

with tempfile.TemporaryDirectory(prefix='nord2-observed-i2c-')as tmp:
 exe=Path(tmp)/'test'
 subprocess.run(['clang','-std=c11','-DNORD2','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(R/'shim/nord2-xnu'),'-I'+str(R/'tests/nord2-audio'),str(S/'audio_i2c8.c'),str(D/'test-observed-i2c.c'),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
