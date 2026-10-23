#!/usr/bin/env python3
"""Compile actual current source with focused stop ordering fixture, no device."""
import argparse,resource,subprocess,tempfile
from pathlib import Path
R=next(p for p in Path(__file__).resolve().parents if (p/'shim/nord2-xnu/audio_hw.c').exists())
p=argparse.ArgumentParser(description=__doc__);p.add_argument('--source',type=Path,default=R/'shim/nord2-xnu/audio_hw.c');p.add_argument('--expect-failure',action='store_true');a=p.parse_args()
resource.setrlimit(resource.RLIMIT_CORE,(0,0))
with tempfile.TemporaryDirectory(prefix='nord2-stop-test-') as temp:
 exe=Path(temp)/'test'
 subprocess.run(['clang','-DNORD2','-std=c11','-Wall','-Wextra','-Werror','-O1','-g','-fsanitize=address,undefined','-fno-sanitize-recover=all','-ffunction-sections','-fdata-sections','-Wl,--gc-sections','-I'+str(R/'shim/nord2-xnu'),str(a.source.resolve()),str(Path(__file__).with_name('test-stop.c')),'-o',str(exe)],check=True)
 r=subprocess.run([str(exe)],capture_output=True,text=True)
 if a.expect_failure:
  assert r.returncode!=0,'baseline unexpectedly passes';print('PASS baseline reproduces stop-order failure:',r.stderr.strip().splitlines()[-1])
 else:
  print(r.stdout,end='');assert r.returncode==0,r.stderr
