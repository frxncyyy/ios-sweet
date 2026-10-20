#!/usr/bin/env python3
"""Guard the image checker's distinct RW-userland and RO-stop-only contracts."""
from pathlib import Path
import importlib.util
p=Path(__file__).with_name('nord2-check-xnu.py')
spec=importlib.util.spec_from_file_location('check_xnu',p)
m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
base=['cpus=8','rd=ufsroot0','ufsroot=userdata']
rw=base+['ufsrw=1','rootrw=1']
ro=base+['ufsrw=0','rootrw=0','nord2ufs2lane=1','nord2input=0','nord2wifi=0',
         'nord2wififw=0','nord2wifiemi=0','nord2fbdiag=0','nord2ufsprofile=1']
markers=[b'NORD2 UFS X2 STOP:',b'NORD2 UFS X2 STATE:',
         b'root_published=0 return_blocked=1',b'NORD2 UFS X2 SNAP C:']
kernel=b'\0'.join(markers)
def valid(args,k=kernel):return all(ok for _,ok in m.point23_root_checks(args,k))
assert valid(rw,b'') and valid(ro)
assert not valid(base+['ufsrw=0','rootrw=0'])
for args in (ro,rw):
 for i,token in enumerate(args):
  assert not valid(args[:i]+args[i+1:])
  assert not valid(args+[token])
  key,value=token.split('=',1)
  assert not valid(args[:i]+[key+'=unexpected']+args[i+1:])
for missing in markers:assert not valid(ro,b'\0'.join(x for x in markers if x!=missing))
assert not valid(rw+['nord2ufs2lane=1'])
assert not valid(ro+['ufsrw=1']) and not valid(ro+['rootrw=1'])
print('PASS: RW default preserved; explicit RO lane mode; missing/duplicate/mixed/invalid args and absent stop markers refused')
