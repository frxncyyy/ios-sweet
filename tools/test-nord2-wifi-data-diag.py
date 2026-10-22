#!/usr/bin/env python3
"""Diagnostic-only pure/actual-source checks. No device or firmware access."""
from pathlib import Path
import argparse,hashlib,json,re,subprocess,tempfile
R=next(p for p in Path(__file__).resolve().parents if (p/'AGENTS.md').is_file());P=Path(__file__).resolve().parent/'fixtures/nord2-wifi-data-diag';F=Path(__file__).resolve().parent/'fixtures/nord2-wifi-station';S=R/'shim/nord2-xnu'
a=argparse.ArgumentParser();a.add_argument('--candidate',type=Path);a.add_argument('--log-dir',type=Path);a.add_argument('--only',choices=('pure','actual','extended','rx'));args=a.parse_args();records=[]
with tempfile.TemporaryDirectory(prefix='wifi-data-diag-') as td:
 d=Path(td)
 for f in S.iterdir():
  if f.suffix in ('.c','.h','.inc'):(d/f.name).write_bytes(f.read_bytes())
 if args.candidate:
  for f in args.candidate.iterdir():
   if f.suffix in ('.c','.h','.inc'):(d/f.name).write_bytes(f.read_bytes())
 for group in ('pure','actual','extended','rx'):
  if args.only and group!=args.only:continue
  files=[P/('test-'+group+'.c'),d/'wifi_station_data_diag.c'];includes=[d,P,F];extra=[]
  if group in ('actual','rx'):
   names=['wifi_station_diag','wifi_wpa_diag','wifi_station_transaction','wifi_station_wire','wifi_wpa2','wifi_wpa_owner','wifi_wpa_crypto','wifi_wpa_nst','wifi_station_tx','wifi_station_transport','wifi_normal_transport','wifi_capability','wifi_main_transport','wifi_main_command'];files += [d/(n+'.c') for n in names]
   B=R/'research/ci-39158169212/buildtree/xnu/libkern/crypto/sweet_bearssl';includes += [B/'inc',B/'src'];files += [B/'src'/x for x in ('hash/sha1.c','codec/dec32be.c','codec/enc32be.c','symcipher/aes_ct.c','symcipher/aes_ct_dec.c','symcipher/aes_ct_cbcdec.c')];extra=['-Wno-unused-function','-Wno-unused-const-variable']
  cmd=['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-misleading-indentation',*extra,'-fsanitize=address,undefined','-fno-sanitize-recover=all',*['-I'+str(p) for p in includes],*map(str,files),'-o',str(d/group)]
  subprocess.run(cmd,check=True);result=subprocess.run([str(d/group)],capture_output=True,text=True)
  if args.log_dir:args.log_dir.mkdir(parents=True,exist_ok=True);(args.log_dir/(group+'.log')).write_text(result.stdout+result.stderr)
  result.check_returncode();lines=[l for l in result.stdout.splitlines() if l.startswith(('NORD2 WIFI DATAHDR:','NORD2 WIFI DATATXS:','NORD2 WIFI DATACOUNT:','NORD2 WIFI DATATYPE:','NORD2 WIFI DHCPSTAT:','NORD2 WIFI RXREJECT:','NORD2 WIFI RXTOTAL:','NORD2 WIFI RXCOUNTS:','NORD2 WIFI TXPHY:','NORD2 WIFI DHCPMATCH:','NORD2 WIFI DHCPTOTAL:'))]
  assert lines and all(len(l.encode())+1<255 for l in lines);assert all(re.fullmatch(r'NORD2 WIFI (?:DATAHDR|DATATXS|DATACOUNT|DATATYPE|DHCPSTAT|RXREJECT|RXTOTAL|RXCOUNTS|TXPHY|DHCPMATCH|DHCPTOTAL): (?:[a-z_]+=[0-9]+)(?: [a-z_]+=[0-9]+)*',l) for l in lines)
  assert all('%s' not in f.read_text() and '%p' not in f.read_text() for f in (d/'wifi_station_data_diag.c',));assert 'FAKESECRET' not in result.stdout
  if group=='extended':
   phy=[l for l in lines if l.startswith('NORD2 WIFI TXPHY:')]
   assert len(phy)==1 and 'response=2 table=3 width=1 power=254 delay=305419896 applied=2309737967' in phy[0]
   assert len([l for l in lines if l.startswith('NORD2 WIFI DHCPMATCH:')])==12
   assert len([l for l in lines if l.startswith('NORD2 WIFI RXREJECT:')])==2
   assert len([l for l in lines if l.startswith('NORD2 WIFI RXCOUNTS:')])==5
  records.append({'group':group,'exit':result.returncode,'numeric_lines':len(lines),'max_line_bytes':max(len(l.encode())+1 for l in lines),'sources':{str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in files},'last_line':result.stdout.splitlines()[-1]});print(group,result.stdout.splitlines()[-1])
if args.log_dir:(args.log_dir/'report.json').write_text(json.dumps({'passed':True,'hardware_claim':False,'lease_or_IP_claim':False,'groups':records},indent=2)+'\n')
