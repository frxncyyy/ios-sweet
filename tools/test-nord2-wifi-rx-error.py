#!/usr/bin/env python3
"""Actual helper sanitizers, strict reader negatives and unchanged RX-path replay."""
from pathlib import Path
import argparse,hashlib,importlib.util,json,re,subprocess,tempfile
R=next(p for p in Path(__file__).resolve().parents if(p/'AGENTS.md').is_file())
HERE=Path(__file__).resolve().parent;private=HERE.parent/'shim/nord2-xnu'
S=private if private.is_dir() else R/'shim/nord2-xnu'
a=argparse.ArgumentParser();a.add_argument('--log-dir',type=Path,required=True);v=a.parse_args();v.log_dir.mkdir(parents=True,exist_ok=True)
def run(cmd):
 p=subprocess.run(cmd,text=True,capture_output=True);assert p.returncode==0,p.stdout+p.stderr;return p.stdout
with tempfile.TemporaryDirectory(prefix='rx-error-') as t:
 out=Path(t)/'pure';cmd=['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(S),str(HERE/'fixtures/nord2-wifi-rx-error/test-rx-error.c'),str(S/'wifi_station_data_diag.c'),'-o',str(out)]
 run(cmd);stdout=run([str(out)]);(v.log_dir/'helper.log').write_text(stdout)
 spec=importlib.util.spec_from_file_location('reader',HERE/'nord2-wifi-rx-error.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
 positive=negative=0
 def yes(s):
  global positive
  r=m.parse(s);assert r['valid'] and not r['air_delivery_proven'] and not r['dhcp_lease_proven'] and not r['absence_proves_zero_errors'];positive+=1;return r
 def no(s):
  global negative
  try:m.parse(s)
  except ValueError:negative+=1;return
  raise AssertionError('accepted malformed '+repr(s))
 lines=[l+'\n' for l in stdout.splitlines() if l.startswith('NORD2 WIFI RXERROR:')];assert len(lines)==8
 yes('');yes('unrelated console\n');yes(''.join(lines));yes(lines[3]);yes(''.join(l.replace('g=18446744073709551615','g=1') for l in lines));yes(lines[0].replace('\n','\r\n'))
 yes(lines[0]+lines[0].replace('g=18446744073709551615','g=2')+lines[1])
 base=lines[0]
 for k in range(1,len(base)):no(base[:k]) if base[:k].startswith('NORD2 WIFI RXERROR') else None
 for key in m.FIELDS:
  no(re.sub(r' '+key+r'=\d+','',base));no(re.sub(key+r'=\d+',key+'=-1',base));no(re.sub(key+r'=\d+',key+'=00',base));no(re.sub(key+r'=\d+',key+'='+str(2**64 if key=='g' else 2**32),base))
 for s in (base.rstrip()+' secret=1\n',base.rstrip()+' mac=FAKESECRET\n',base.rstrip()+' payload=0\n',base.replace('RXERROR:','RXERRORX:'),base+' '+base,base.replace('\n','\r\r\n'),base+base,lines[1]+lines[0]):no(s)
 for key,limit in {'n':8,'wlan':1023,'sec':31,'key':3,'ht':1,'groups':31,'a1':3,'dst':2,'tid':15}.items():no(re.sub(key+r'=\d+',key+'='+str(limit+1),base))
 for key in ('g','n'):no(re.sub(key+r'=\d+',key+'=0',base))
 for key,bit in (('d1',1),('d2',1),('d4',4)):no(re.sub(key+r'=\d+',key+'='+str(bit),base))
 no(re.sub(r'd[124]=\d+',lambda x:x.group().split('=')[0]+'=0',base));no(re.sub(r'type=\d+','type=0',base));no(re.sub(r'dst=\d+','dst=1',base))
 for a1 in range(4):
  for ht in range(2):
   s=re.sub(r'a1=\d+',f'a1={a1}',base);s=re.sub(r'dst=\d+',f'dst={0 if not a1 else 1 if a1==1 else 2}',s);s=re.sub(r'ht=\d+',f'ht={ht}',s);yes(s)
 canonical=(R/'shim/nord2-xnu/wifi_station_transport.c').read_text();candidate=(S/'wifi_station_transport.c').read_text();hook='nsdd_rx_error(&s->data_diag,s->generation,p,n);'
 assert candidate.count(hook)==1
 assert candidate==canonical if hook in canonical else candidate.replace(hook,'')==canonical
 fmt=re.search(r'printf\("(NORD2 WIFI RXERROR: .*?)\\n"',(S/'wifi_station_data_diag.c').read_text()).group(1)
 assert re.findall(r' ([a-z0-9]+)=',fmt)==list(m.FIELDS) and '%s' not in fmt and '%p' not in fmt
 assert max(len(l.encode()) for l in lines)<=255
 # Replay augmented actual NST RX fixture on baseline and candidate source.
 names=['wifi_station_data_diag','wifi_station_diag','wifi_wpa_diag','wifi_station_transaction','wifi_station_wire','wifi_wpa2','wifi_wpa_owner','wifi_wpa_crypto','wifi_wpa_nst','wifi_station_tx','wifi_station_transport','wifi_normal_transport','wifi_capability','wifi_main_transport','wifi_main_command']
 B=R/'research/ci-39158169212/buildtree/xnu/libkern/crypto/sweet_bearssl';crypto=[B/'src'/x for x in ('hash/sha1.c','codec/dec32be.c','codec/enc32be.c','symcipher/aes_ct.c','symcipher/aes_ct_dec.c','symcipher/aes_ct_cbcdec.c')]
 for label in ('baseline','candidate'):
  d=Path(t)/label;d.mkdir();src=R/'shim/nord2-xnu'
  for f in src.iterdir():
   if f.suffix in('.c','.h','.inc'):(d/f.name).write_bytes(f.read_bytes())
  for f in S.iterdir():
   if f.suffix in('.c','.h','.inc'):(d/f.name).write_bytes(f.read_bytes())
  if label=='baseline':
   p=d/'wifi_station_transport.c';p.write_text(p.read_text().replace(hook,''))
  includes=[d,R/'tools/fixtures/nord2-wifi-data-diag',R/'tools/fixtures/nord2-wifi-station',B/'inc',B/'src']
  cmd=['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-Wno-unused-function','-Wno-unused-const-variable','-fsanitize=address,undefined','-fno-sanitize-recover=all',*['-I'+str(p) for p in includes],str(HERE/'fixtures/nord2-wifi-rx-error/test-actual.c'),*[str(d/(n+'.c')) for n in names],*map(str,crypto),'-o',str(d/'run')]
  run(cmd);actual=run([str(d/'run')]);(v.log_dir/label).mkdir(exist_ok=True);(v.log_dir/label/'rx.log').write_text(actual)
 b=(v.log_dir/'baseline/rx.log').read_text();c=(v.log_dir/'candidate/rx.log').read_text();new=[l+'\n' for l in c.splitlines() if l.startswith('NORD2 WIFI RXERROR:')]
 assert new and ''.join(l for l in c.splitlines(keepends=True) if not l.startswith('NORD2 WIFI RXERROR:'))==b
 yes(''.join(new));assert all('FAKESECRET' not in l for l in lines+new)
 report={'passed':True,'reader_positive':positive,'reader_negative':negative,'pure_last_line':stdout.splitlines()[-1],'actual_rx_output_equal_except_new_rows':True,'actual_rx_detail_rows':len(new),'max_row_bytes':max(len(l.encode()) for l in lines+new),'single_observation_hook_verified':True,'source_pins':{str(p.relative_to(R)):hashlib.sha256(p.read_bytes()).hexdigest() for p in S.iterdir() if p.is_file()},'runtime_changes':'observation only; error gate expression/reject/return unchanged','hardware_claim':False}
 (v.log_dir/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
