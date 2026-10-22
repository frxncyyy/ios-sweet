#!/usr/bin/env python3
from pathlib import Path
import argparse,subprocess,tempfile,json,hashlib
R=next(p for p in Path(__file__).resolve().parents if (p/'AGENTS.md').exists());P=Path(__file__).resolve().parent;F=P/'fixtures/nord2-wifi-normal-tx';a=argparse.ArgumentParser();a.add_argument('--candidate',type=Path);a.add_argument('--log-dir',type=Path,required=True);a.add_argument('--only',choices=['pure','actual']);v=a.parse_args();v.log_dir.mkdir(parents=True,exist_ok=False);records=[]
with tempfile.TemporaryDirectory() as z:
 d=Path(z)
 for source in [R/'shim/nord2-xnu']+([v.candidate] if v.candidate else []):
  for f in source.iterdir():
   if f.suffix in ('.c','.h','.inc'):(d/f.name).write_bytes(f.read_bytes())
 for group in ['pure','actual']:
  if v.only and v.only!=group:continue
  names=['wifi_station_tx','wifi_station_wire'];includes=[d,F,R/'tools/fixtures/nord2-wifi-station'];extra=[]
  if group=='actual':names+=['wifi_station_transport','wifi_station_transaction','wifi_wpa2','wifi_wpa_owner','wifi_wpa_crypto','wifi_wpa_nst','wifi_station_diag','wifi_station_data_diag','wifi_wpa_diag','wifi_normal_transport','wifi_capability','wifi_main_transport','wifi_main_command'];B=R/'research/ci-39158169212/buildtree/xnu/libkern/crypto/sweet_bearssl';includes += [B/'inc',B/'src'];extra=[B/'src'/x for x in ['hash/sha1.c','codec/dec32be.c','codec/enc32be.c','symcipher/aes_ct.c','symcipher/aes_ct_dec.c','symcipher/aes_ct_cbcdec.c']]
  files=[F/('test-'+group+'.c')]+[d/(n+'.c') for n in names]+extra;cmd=['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-Wno-unused-function','-Wno-unused-const-variable','-fsanitize=address,undefined','-fno-sanitize-recover=all',*['-I'+str(x) for x in includes],*map(str,files),'-o',str(d/group)];compile=subprocess.run(cmd,capture_output=True,text=True);(v.log_dir/(group+'-compile.log')).write_text(compile.stdout+compile.stderr);compile.check_returncode();result=subprocess.run([str(d/group)],capture_output=True,text=True);(v.log_dir/(group+'.log')).write_text(result.stdout+result.stderr);result.check_returncode();records.append({'group':group,'passed':True,'last_line':result.stdout.splitlines()[-1],'sources':{str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in files}});print(records[-1]['last_line'],flush=True)
(v.log_dir/'report.json').write_text(json.dumps({'passed':True,'groups':records,'hardware_claim':False},indent=2)+'\n')
