#!/usr/bin/env python3
"""Host-only capability dispatcher/native lifetime checks; no device access."""
from pathlib import Path
import argparse,os,subprocess,tempfile
ap=argparse.ArgumentParser()
NAMES=('dispatcher','native-adapter','current-owner','drain','header','basic-owner','basic-faults','debug')
ap.add_argument('--only',choices=NAMES)
args=ap.parse_args()
R=Path(__file__).resolve().parents[1]
S=R/'shim/nord2-xnu';F=R/'tools/fixtures/nord2-wifi-capability'
flags=[os.environ.get('CC','cc'),'-std=c11','-O1','-g','-Wall','-Wextra','-Werror',
       '-fsanitize=address,undefined','-fno-sanitize-recover=all','-fno-omit-frame-pointer','-I'+str(S)]
with tempfile.TemporaryDirectory(prefix='nord2-capability-host-') as temporary:
 for name in NAMES:
  if args.only and args.only!=name:continue
  files=[] if name in ('drain','debug') else [S/'wifi_capability.c',S/'wifi_capability_dispatcher.c',
                                 S/'wifi_main_command.c',S/'dma_pool_contract.c']
  if name=='debug':files=[S/'wifi_capability.c',S/'wifi_main_command.c',S/'dma_pool_contract.c']
  if name in ('native-adapter','current-owner','header','basic-owner','basic-faults'):
   files += [S/('wifi_basic_'+n+'.c') for n in ('config','resources','metadata','primitives')]
  if name in ('current-owner','basic-owner','basic-faults'):
   files += [S/('wifi_main_'+n+'.c') for n in ('initializer','dmashdl','init_plan','transport','layout')]
   files += [S/'wifi_dma_preflight.c']
  output=Path(temporary)/name
  fixture=(R/'tools/fixtures/nord2-wifi-basic-config'/('test-'+('current-owner' if name=='basic-owner' else 'faults')+'.c')) if name.startswith('basic-') else F/('test-'+name+'.c')
  subprocess.run(flags+[str(fixture)]+list(map(str,files))+['-o',str(output)],check=True)
  result=subprocess.run([str(output)],capture_output=True,text=True)
  if result.returncode:
   print(result.stdout);print(result.stderr);result.check_returncode()
  print(result.stdout.splitlines()[-1])
print('PASS canonical capability host checks; synthetic firmware/readiness only, no device or network claim')
