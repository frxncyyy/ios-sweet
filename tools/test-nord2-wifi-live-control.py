#!/usr/bin/env python3
"""Host tests of exact pure live-control runtime; no USB/device or builder."""
from pathlib import Path
import argparse,hashlib,json,subprocess,tempfile
R=next(p for p in Path(__file__).resolve().parents if(p/'AGENTS.md').is_file());T=Path(__file__).resolve().parent
p=argparse.ArgumentParser();p.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');p.add_argument('--log-dir',type=Path,required=True);p.add_argument('--tsan',action='store_true');a=p.parse_args();a.log_dir.mkdir(parents=True,exist_ok=True)
f=T/'fixtures/nord2-wifi-live-control/test.c';S=a.candidate.resolve();before={n:hashlib.sha256((S/n).read_bytes()).hexdigest() for n in ('wifi_live_control.c','wifi_live_control.h')};results=[]
with tempfile.TemporaryDirectory(prefix='nord2-live-test-') as td:
 for mode in (['asan','tsan'] if a.tsan else ['asan']):
  exe=Path(td)/mode;cmd=['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-pthread','-fno-sanitize-recover=all','-fsanitize='+('address,undefined' if mode=='asan' else 'thread'),'-I'+str(S),str(f),'-o',str(exe)]
  c=subprocess.run(cmd,capture_output=True,text=True);(a.log_dir/(mode+'-compile.log')).write_text(c.stdout+c.stderr)
  if c.returncode:
   if mode=='asan':raise SystemExit(c.stderr)
   results.append({'mode':mode,'state':'UNAVAILABLE_COMPILE','exit':c.returncode});continue
  try:r=subprocess.run([str(exe)],capture_output=True,text=True,timeout=90)
  except subprocess.TimeoutExpired:raise SystemExit('bounded host test timed out')
  (a.log_dir/(mode+'.log')).write_text(r.stdout+r.stderr)
  unsupported=mode=='tsan' and any(x in r.stderr for x in ('unexpected memory mapping','personality(old_personality','CHECK failed: tsan_platform_linux'))
  if unsupported:results.append({'mode':mode,'state':'UNAVAILABLE_RUNTIME','exit':r.returncode});continue
  assert r.returncode==0,(mode,r.returncode,r.stderr[-1000:]);assert r.stdout.splitlines()[-1].startswith('PASS ')
  assert not any(x in r.stderr for x in ('runtime error:','AddressSanitizer','WARNING: ThreadSanitizer'))
  results.append({'mode':mode,'state':'PASS','exit':0,'last':r.stdout.splitlines()[-1]})
assert before=={n:hashlib.sha256((S/n).read_bytes()).hexdigest() for n in before}
report={'passed':results[0]['state']=='PASS','results':results,'source_pins':before,'fixture_sha256':hashlib.sha256(f.read_bytes()).hexdigest(),'synthetic_host_only':True,'device_access':False,'binder_semantics_tested':False}
(a.log_dir/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
