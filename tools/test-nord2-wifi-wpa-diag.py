#!/usr/bin/env python3
"""Actual-source crypto/transaction diagnostics and strict reader; offline only."""
from pathlib import Path
import argparse,subprocess,tempfile,json,hashlib,runpy,re
R=next(p for p in Path(__file__).resolve().parents if (p/'AGENTS.md').exists());P=Path(__file__).parent/'fixtures/nord2-wifi-wpa-diag';S=R/'shim/nord2-xnu';J=R/'tools/fixtures/nord2-wifi-station'
a=argparse.ArgumentParser();a.add_argument('--candidate',type=Path);a.add_argument('--bearssl',type=Path);a.add_argument('--log-dir',type=Path);a.add_argument('--only',choices=['crypto','transaction','gate','reader']);z=a.parse_args();B=z.bearssl or R/'research/ci-39158169212/buildtree/xnu/libkern/crypto/sweet_bearssl';records=[]
with tempfile.TemporaryDirectory(prefix='wpa-diag-') as td:
 d=Path(td);W=z.log_dir or d;W.mkdir(parents=True,exist_ok=True)
 for f in S.iterdir():
  if f.suffix in ('.c','.h'):(d/f.name).write_bytes(f.read_bytes())
 if z.candidate:
  for f in z.candidate.iterdir():
   if f.suffix in ('.c','.h'):(d/f.name).write_bytes(f.read_bytes())
 for kind in ['crypto','transaction','gate']:
  if z.only and z.only!=kind:continue
  files=[P/('test-'+kind+'.c')]+[d/(x+'.c') for x in ['wifi_wpa2','wifi_wpa_diag','wifi_wpa_crypto']+(['wifi_station_transaction','wifi_station_wire'] if kind!='crypto' else [])]+[B/'src'/x for x in ['hash/sha1.c','codec/dec32be.c','codec/enc32be.c','symcipher/aes_ct.c','symcipher/aes_ct_dec.c','symcipher/aes_ct_cbcdec.c']]
  cmd=['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-Wno-unused-const-variable','-Wno-unused-function','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(d),'-I'+str(J),'-I'+str(B/'inc'),'-I'+str(B/'src'),*map(str,files),'-o',str(d/kind)]
  subprocess.run(cmd,check=True)
  for mode in (range(3) if kind=='gate' else (None,)):
   command=[str(d/kind)]+([str(mode)] if mode is not None else [])
   v=subprocess.run(command,capture_output=True,text=True);label=kind+str(mode) if mode is not None else kind;(W/(label+'.log')).write_text(v.stdout+v.stderr);v.check_returncode();records.append({'suite':label,'exit':0,'source_pins':{str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in files}});print(label,'PASS')
 if not z.only or z.only=='reader':
  parse=runpy.run_path(str(R/'tools/nord2-wifi-wpa-diag.py'))['parse'];rows=(P/'source-rows.log').read_text().splitlines();assert len(parse('\n'.join(rows))['records'])==4;negative=0
  for key,value in json.loads((P/'reader-cases.json').read_text())['field_mutations']:
   bad=re.sub(r'\b'+key+r'=\d+',key+'='+value,rows[0])
   try:parse(bad)
   except ValueError:negative+=1
   else:raise AssertionError(key)
  for bad in [rows[0]+' secret=1',rows[0][:-1],rows[0]+'\n'+rows[0],rows[-1],rows[0].replace('failure=0','failure=1')]:
   try:parse(bad)
   except ValueError:negative+=1
   else:raise AssertionError('accepted invalid row')
  if (W/'transaction.log').exists():assert len(parse((W/'transaction.log').read_text())['records'])==4
  worst='NORD2 WIFI WPAHDR: n=40 g='+str(2**64-1)+' t='+str(2**64-1)+' bucket=2 state=5 bytes=640 version=255 type=255 declared=65535 info=65535 class=5 result=5 failure=14\n';assert len(worst)<=255
  gates=json.loads((P/'reader-gate-cases.json').read_text())['valid_rows']
  for i,line in enumerate(gates):
   assert len(parse(line)['preengine_gate_records'])==1
   actual=W/('gate'+str(i)+'.log')
   if actual.exists():assert len(parse(actual.read_text())['preengine_gate_records'])==1
  gate_negative=0
  badrows=[gates[0]+'\n'+gates[0],gates[0]+' extra=1',gates[0][:-1],gates[0].replace('g=42','g=0'),gates[0].replace('state=1','state=6'),gates[0].replace('present=0','present=2'),gates[0].replace('reason=1','reason=2'),gates[1].replace('reason=2','reason=3'),gates[2].replace('bytes=18446744073709551615','bytes=640'),gates[2].replace('bytes=18446744073709551615','bytes=18446744073709551616')]
  for line in badrows:
   try:parse(line)
   except ValueError:gate_negative+=1
   else:raise AssertionError('invalid gate row')
  gate_worst='NORD2 WIFI WPAGATE: n=1 g='+str(2**64-1)+' t='+str(2**64-1)+' state=5 present=1 bytes='+str(2**64-1)+' reason=3\n';assert len(gate_worst)<=255
  records.append({'suite':'reader','source_rows':4,'negative_mutations':negative,'gate_rows':3,'gate_negatives':gate_negative,'max_line_bytes':len(worst),'gate_max_line_bytes':len(gate_worst)});print('reader PASS',negative,'header and',gate_negative,'gate negative cases')
 (W/'report.json').write_text(json.dumps({'status':'PASS_OFFLINE_ONLY','records':records},indent=2)+'\n')
