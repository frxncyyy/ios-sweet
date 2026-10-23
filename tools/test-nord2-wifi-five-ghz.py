#!/usr/bin/env python3
"""Offline passive5GHz and bounded UI scan-history regression; no hardware IO."""
from pathlib import Path
import argparse,hashlib,json,os,subprocess,tempfile
R=next(p for p in Path(__file__).resolve().parents if(p/'AGENTS.md').exists());T=R/'tools';S=R/'shim/nord2-xnu'
a=argparse.ArgumentParser(description=__doc__);a.add_argument('--calibration',type=Path,required=True,help='read-only authenticated6146-byte WIFI file; never embedded/printed');a.add_argument('--candidate',type=Path);a.add_argument('--fixture-overlay',type=Path);a.add_argument('--only',choices=('all','pure','client','owner','contracts','station'),default='all');a.add_argument('--log-dir',type=Path,required=True);x=a.parse_args()
O=x.log_dir.resolve();O.mkdir(parents=True,exist_ok=True);board=x.calibration.resolve();assert board.stat().st_size==6146 and hashlib.sha256(board.read_bytes()).hexdigest()=='61ac3300aad6f65364f845894bb982b7c5d077346f788c29df019c93c4f3f89f','calibration identity/extent'
F=Path(__file__).resolve().parent/'fixtures/nord2-wifi-five-ghz';sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
inputs=[p for p in S.iterdir() if p.is_file()]+[p for p in F.iterdir() if p.is_file()]
if x.candidate:inputs += [p for p in x.candidate.iterdir() if p.is_file()]
if x.fixture_overlay:inputs += [p for p in x.fixture_overlay.rglob('*') if p.is_file()]
before={str(p):sha(p) for p in inputs};summaries=[];env=dict(os.environ,ASAN_OPTIONS='halt_on_error=1',UBSAN_OPTIONS='halt_on_error=1')
def run(name,argv):
 p=subprocess.run(list(map(str,argv)),env=env,text=True,capture_output=True)
 (O/(name+'.log')).write_text(p.stdout+p.stderr)
 if p.returncode:print(p.stdout[-1500:]+p.stderr[-1500:]);p.check_returncode()
 lines=p.stdout.splitlines();summary=lines[-1] if lines else 'PASS '+name;summaries.append(summary);print(summary)
with tempfile.TemporaryDirectory(prefix='wifi-fiveGHz-regression-') as td:
 d=Path(td);C=d/'source';C.mkdir();fixtures=d/'fixtures';fixtures.mkdir()
 for p in S.iterdir():
  if p.is_file():(C/p.name).write_bytes(p.read_bytes())
 if x.candidate:
  for p in x.candidate.iterdir():
   if p.is_file():(C/p.name).write_bytes(p.read_bytes())
 for dirname in ('nord2-wifi-ios-contract','nord2-wifi-ios-client','nord2-wifi-station'):
  target=fixtures/dirname;target.mkdir()
  for p in (T/'fixtures'/dirname).iterdir():
   if p.is_file():(target/p.name).write_bytes(p.read_bytes())
 if x.fixture_overlay:
  for p in x.fixture_overlay.rglob('*'):
   if p.is_file():target=fixtures/p.relative_to(x.fixture_overlay);target.parent.mkdir(parents=True,exist_ok=True);target.write_bytes(p.read_bytes())
 flags=['-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(C)]
 if x.only in ('all','pure'):
  base='wifi_ios_snapshot wifi_ios_bss wifi_ios_rates wifi_ios_mapping wifi_ios_scan_contract wifi_ios_cursor wifi_scan_bss wifi_ios_policy wifi_ios_lifecycle wifi_ios_age'.split()
  for name in ('channels','retention'):
   units=base+('wifi_normal_config wifi_normal_init wifi_regulatory_policy wifi_ios_scalar'.split() if name=='channels' else [])
   run('compile-'+name,['clang',*flags,'-DNCFG_HOST_TEST',F/(name+'.c'),*[C/(u+'.c') for u in units],'-lcrypto','-o',d/name])
   run(name,[d/name,board] if name=='channels' else [d/name])
 if x.only in ('all','contracts'):
  suites={'join':['join'],'station-view':'station_view join policy mapping rates scan_contract'.split(),'snapshot':'snapshot age bss rates mapping scan_contract cursor'.split(),'policy':'policy rates mapping'.split(),'scalar':['scalar'],'caller':'policy rates mapping'.split()}
  for name,units in suites.items():
   extra=[C/'wifi_scan_bss.c'] if name=='snapshot' else []
   run('compile-contract-'+name,['clang',*flags,fixtures/'nord2-wifi-ios-contract'/('test-'+name+'.c'),*[C/('wifi_ios_'+u+'.c') for u in units],*extra,'-o',d/('contract-'+name)])
   run('contract-'+name,[d/('contract-'+name)])
 if x.only in ('all','owner'):
  s=(T/'test-nord2-wifi-join-owner.py').read_text()
  old="P=Path(__file__).resolve().parent;R=next(p for p in P.parents if (p/'AGENTS.md').exists());"
  assert s.count(old)==1;s=s.replace(old,'P=Path('+repr(str(T))+');R=Path('+repr(str(R))+');')
  old='setup(0);run_worker();assert(nss_native.ready';assert s.count(old)==1;s=s.replace(old,'setup(0);run_worker();assert(nss_native.info.count==37&&NSB_RESULTS==64);assert(nss_native.ready')
  runner=d/'owner.py';runner.write_text(s);run('owner',['python3',runner,'--candidate',C,'--board',board,'--log-dir',O/'owner'])
 if x.only in ('all','client'):
  s=(T/'test-nord2-wifi-ios-client.py').read_text();start=s.index('R=next(');end=s.index('\nap=',start)
  s=s[:start]+'R=Path('+repr(str(R))+');P=Path('+repr(str(fixtures/'nord2-wifi-ios-client'))+');B=P;S=R/\'shim/nord2-xnu\'\n'+s[end:]
  runner=d/'client.py';runner.write_text(s);run('client',['python3',runner,'--candidate',C,'--log',O/'client-stdout.log'])
 if x.only in ('all','station'):
  s=(T/'test-nord2-wifi-station.py').read_text();old="P=Path(__file__).resolve().parent/'fixtures/nord2-wifi-station'";assert s.count(old)==1;s=s.replace(old,'P=Path('+repr(str(fixtures/'nord2-wifi-station'))+')')
  old="R=next(p for p in Path(__file__).resolve().parents if (p/'AGENTS.md').exists())";assert s.count(old)==1;s=s.replace(old,'R=Path('+repr(str(R))+')')
  runner=d/'station.py';runner.write_text(s)
  for name in ('native','service'):run('station-'+name,['python3',runner,'--candidate',C,'--only',name,'--log-dir',O/('station-'+name)])
assert before=={str(p):sha(p) for p in inputs},'input changed during tests'
(O/'report.json').write_text(json.dumps({'status':'PASS_SYNTHETIC_ONLY','suite':x.only,'summaries':summaries,'source_inputs_immutable':True,'inputs':before,'calibration_sha256':hashlib.sha256(board.read_bytes()).hexdigest(),'private_values_printed':False,'hardware_tested':False},indent=2)+'\n')
