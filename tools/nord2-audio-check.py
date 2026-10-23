#!/usr/bin/env python3
"""Run source/model checks. --board is a private pinned stock container, never flashed."""
from pathlib import Path
import argparse,subprocess,tempfile,importlib.util,plistlib,struct
R=Path(__file__).resolve().parents[1]
S=R/'shim/nord2-xnu';T=R/'tests/nord2-audio'
INHERITED=['cycles','mixed','refuse-rate','refuse-status','refuse-adsp','refuse-coldmux',
 'drift-audioh','drift-adsp','powerup-timeout','protect-timeout','ampe']
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--board',type=Path);a=ap.parse_args()
 flags=['clang','-std=c11','-DNORD2','-O1','-g','-Wall','-Wextra','-Werror','-Wno-unused-function','-Wno-misleading-indentation','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(S),'-I'+str(T)]
 with tempfile.TemporaryDirectory()as tmp:
  d=Path(tmp)
  spec=importlib.util.spec_from_file_location('pkg',R/'tools/nord2-audio-package.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
  plist=plistlib.dumps({'CFBundleExecutable':'RedwoodAudio','CFBundleIdentifier':'com.redwood.audio','CFPlugInFactories':{m.FACTORY:'RedwoodAudio_Create'}})
  binary=struct.pack('<IIIIIIII',0xfeedfacf,0x100000c,0,8,0,0,0,0)
  (d/'test.pkg').write_bytes(m.encode(binary,plist))
  cases=[('queue',[R/'tests/nord2_audio_stream_test.c',S/'audio_stream.c'],['-lm'],[]),
   ('idle',[T/'test-idle.c',S/'audio_stream.c'],[],[]),
   ('live',[R/'tests/nord2_audio_live_test.c',S/'audio_live.c'],[],[]),
   ('install',[T/'test-flow.c'],['-lcrypto'],[]),
   ('package',[T/'test-package.c'],['-lcrypto'],[d/'test.pkg'])]
  modules=[S/(n+'.c')for n in ['audio_hw','audio_afe','audio_id','audio_i2c8','audio_amp','audio_board']]
  cases.extend([('stream',[T/'test-stream.c']+modules,[],[]),('i2c',[T/'test-i2c.c',S/'audio_i2c8.c',S/'audio_id.c',S/'audio_board.c'],[],[]),
   ('saved-preflight',[T/'test-saved-preflight.c',S/'audio_i2c8.c'],[],[]),
   ('i3c',[T/'test-i3c.c',S/'audio_i2c8.c',S/'audio_board.c'],[],[]),
   ('order',[T/'test-order.c',S/'audio_i2c8.c',S/'audio_board.c'],[],[]),
   ('route-plan',[T/'test-route-plan.c',S/'audio_afe.c',S/'audio_id.c'],[],[]),
   ('plan-dump',[T/'plan-dump.c',S/'audio_afe.c',S/'audio_id.c'],[],[])])
  # Real 8e6697a2 inherited boot snapshot: pure classification and plans.
  for case in ['classify','plan']:
   cases.append(('inherited-'+case,[T/'test-inherited.c']+modules,[],[case]))
  if a.board:
   cases.append(('producer-capacity',[T/'test-producer-capacity.c',S/'audio_stream.c']+modules,[],[a.board.resolve()]))
   cases.append(('hardware',[T/'test-full.c']+modules,[],[a.board.resolve()]))
   cases.append(('route-hardware',[T/'test-route.c']+modules,[],[a.board.resolve()]))
   cases.append(('protection',[T/'test-protection.c']+modules,[],[a.board.resolve()]))
   for case in INHERITED:
    cases.append(('inherited-'+case,[T/'test-inherited.c']+modules,[],[case,a.board.resolve()]))
  built={}
  for name,files,link,args in cases:
   key=tuple(map(str,files))+tuple(link)
   exe=built.get(key)
   if not exe:
    exe=d/name;subprocess.run(flags+[str(p)for p in files]+link+['-o',str(exe)],check=True);built[key]=exe
   subprocess.run([str(exe)]+[str(x)for x in args],check=True,stdout=subprocess.DEVNULL if name=='plan-dump' else None)
 subprocess.run(['python3',str(T/'test_live_cli.py')],check=True)
 subprocess.run(['python3',str(T/'test_ready.py')],check=True)
 subprocess.run(['python3',str(T/'test_device_deadline.py')],check=True)
 subprocess.run(['python3',str(T/'test_producer_policy.py')],check=True)
 subprocess.run(['python3',str(T/'test_stop_order.py')],check=True)
 subprocess.run(['python3',str(T/'test_observed.py')],check=True)
 subprocess.run(['python3',str(T/'test_amp_parity.py')]+(['--board',str(a.board.resolve())]if a.board else[]),check=True)
 print('PASS software models/ABI only; physical sound remains unverified')
if __name__=='__main__':main()
