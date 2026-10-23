#!/usr/bin/env python3
from pathlib import Path
import argparse,subprocess,tempfile,json,hashlib,re
R=next(p for p in Path(__file__).resolve().parents if (p/'AGENTS.md').exists());F=Path(__file__).resolve().parent/'fixtures/nord2-wifi-open';a=argparse.ArgumentParser();a.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');a.add_argument('--board',type=Path);a.add_argument('--bearssl',type=Path,default=R/'research/ci-39158169212/buildtree/xnu/libkern/crypto/sweet_bearssl');a.add_argument('--output',type=Path,required=True);a.add_argument('--only',choices=['codec','actual','owner','owner-reverse','regressions','legacy-codec'],default='codec');x=a.parse_args();x.output.mkdir(parents=True,exist_ok=False);S=x.candidate
before={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in (S).iterdir()}
with tempfile.TemporaryDirectory() as z:
 T=Path(z)
 for S in [R/'shim/nord2-xnu',S]:
  for p in S.iterdir():
   if p.suffix in ('.c','.h','.inc'):(T/p.name).write_bytes(p.read_bytes())
 if x.only=='codec':
  names=['wifi_ios_join','wifi_ios_station_view','wifi_ios_policy','wifi_ios_mapping','wifi_ios_rates','wifi_ios_scan_contract','wifi_station_wire','wifi_station_tx'];fixture=F/'test-codec.c';cmd=['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(T),str(fixture),*[str(T/(n+'.c')) for n in names],'-o',str(T/'test')]
  c=subprocess.run(cmd,capture_output=True,text=True);(x.output/'compile.log').write_text(c.stdout+c.stderr);c.check_returncode();r=subprocess.run([str(T/'test')],capture_output=True,text=True);(x.output/'test.log').write_text(r.stdout+r.stderr);r.check_returncode();print(r.stdout,end='')
  fmt=re.search(r'printf\(("NORD2 WIFI IOS STATIONPART: sample=%u part=5[^"\n]*")', (T/'wifi_station_service_bind.inc').read_text()).group(1)
  code='#include <stdio.h>\n#include <stdint.h>\n#include <assert.h>\nint main(void){char s[512];int n=snprintf(s,sizeof s,'+fmt+',UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX,UINT32_MAX,(unsigned long long)UINT64_MAX,UINT32_MAX,UINT32_MAX);assert(n>0&&n<255);printf("PASS compiled PART5 max-width %d bytes including newline\\n",n);}'
  (T/'bound.c').write_text(code);subprocess.run(['clang','-Wall','-Wextra','-Werror',str(T/'bound.c'),'-o',str(T/'bound')],check=True);b=subprocess.run([str(T/'bound')],capture_output=True,text=True,check=True);(x.output/'printf-bound.log').write_text(b.stdout);print(b.stdout,end='')
 elif x.only=='regressions':
  r=subprocess.run(['python3',str(R/'tools/test-nord2-wifi-station.py'),'--candidate',str(S),'--bearssl',str(x.bearssl),'--log-dir',str(x.output/'station')],capture_output=True,text=True);(x.output/'runner.log').write_text(r.stdout+r.stderr);r.check_returncode();print(r.stdout,end='')
 elif x.only=='actual':
  B=x.bearssl;A=R/'tools/fixtures/nord2-wifi-station';fixture=F/'test-actual.c';names=['wifi_scan_bss','wifi_wpa_diag','wifi_station_data_diag','wifi_station_diag','wifi_station_transaction','wifi_station_wire','wifi_wpa2','wifi_wpa_owner','wifi_wpa_crypto','wifi_wpa_nst','wifi_station_tx','wifi_station_transport','wifi_normal_transport','wifi_capability','wifi_main_transport','wifi_main_command'];extra=[B/'src'/n for n in ['hash/sha1.c','codec/dec32be.c','codec/enc32be.c','symcipher/aes_ct.c','symcipher/aes_ct_dec.c','symcipher/aes_ct_cbcdec.c']];cmd=['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-Wno-unused-function','-Wno-unused-const-variable','-fsanitize=address,undefined','-fno-sanitize-recover=all',*['-I'+str(p) for p in [T,F,A,B/'inc',B/'src']],str(fixture),*[str(T/(n+'.c')) for n in names],*map(str,extra),'-o',str(T/'test')]
  c=subprocess.run(cmd,capture_output=True,text=True);(x.output/'compile.log').write_text(c.stdout+c.stderr);c.check_returncode();r=subprocess.run([str(T/'test')],capture_output=True,text=True);(x.output/'test.log').write_text(r.stdout+r.stderr);r.check_returncode();print(r.stdout.splitlines()[-1])
 elif x.only in ('owner','owner-reverse'):
  if not x.board:raise SystemExit('--board explicit retained-owner input is required; no calibration is bundled')
  r=subprocess.run(['python3',str(F/'run-owner.py'),'--candidate',str(S),'--log-dir',str(x.output/'owning'),'--board',str(x.board),'--bearssl',str(x.bearssl)]+(['--initial-open','0'] if x.only=='owner-reverse' else []),capture_output=True,text=True);(x.output/'runner.log').write_text(r.stdout+r.stderr);r.check_returncode();print(r.stdout,end='')
 elif x.only=='legacy-codec':
  for group,names in [('join',['wifi_ios_join']),('station-view',['wifi_ios_station_view','wifi_ios_join','wifi_ios_policy','wifi_ios_mapping','wifi_ios_rates','wifi_ios_scan_contract'])]:
   cmd=['clang','-std=c11','-O1','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(T),str(R/'tools/fixtures/nord2-wifi-ios-contract'/('test-'+group+'.c')),*[str(T/(n+'.c')) for n in names],'-o',str(T/group)]
   c=subprocess.run(cmd,capture_output=True,text=True);(x.output/(group+'-compile.log')).write_text(c.stdout+c.stderr);c.check_returncode();r=subprocess.run([str(T/group)],capture_output=True,text=True);(x.output/(group+'.log')).write_text(r.stdout+r.stderr);r.check_returncode();print(r.stdout,end='')
 else:raise SystemExit('fixture not yet prepared')
 assert before=={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in (S).iterdir()},'candidate source changed during test'
 (x.output/'report.json').write_text(json.dumps({'passed':True,'hardware_claim':False,'group':x.only,'sources':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in (S).iterdir()}},indent=2)+'\n')
