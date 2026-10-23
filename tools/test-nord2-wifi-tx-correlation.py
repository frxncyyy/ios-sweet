#!/usr/bin/env python3
"""Private/canonical actual-source TX correlation checks; synthetic firmware only."""
from pathlib import Path
import argparse,hashlib,json,subprocess,tempfile,re
R=next(p for p in Path(__file__).resolve().parents if(p/'AGENTS.md').is_file());T=Path(__file__).resolve().parent
private=T.parent/'shim/nord2-xnu';S=private if private.is_dir() else R/'shim/nord2-xnu';P=T/'fixtures/nord2-wifi-tx-correlation'
a=argparse.ArgumentParser();a.add_argument('--log-dir',type=Path,required=True);a.add_argument('--only',choices=('pure','actual','submission'));args=a.parse_args();args.log_dir.mkdir(parents=True,exist_ok=True);reports=[]
with tempfile.TemporaryDirectory(prefix='tx-correlation-') as td:
 d=Path(td);f=d/'fixtures';f.mkdir()
 for p in (R/'shim/nord2-xnu').iterdir():
  if p.suffix in('.c','.h','.inc'):(d/p.name).write_bytes(p.read_bytes())
 for p in S.iterdir():
  if p.suffix in('.c','.h','.inc'):(d/p.name).write_bytes(p.read_bytes())
 for p in (R/'tools/fixtures/nord2-wifi-station').iterdir():
  if p.is_file():(f/p.name).write_bytes(p.read_bytes())
 for p in (T/'fixtures/nord2-wifi-station').iterdir():
  if p.is_file():(f/p.name).write_bytes(p.read_bytes())
 (f/'packets.h').write_bytes((R/'tools/fixtures/nord2-wifi-data-diag/packets.h').read_bytes())
 (f/'txcorr-actual.c').write_bytes((P/'test-actual.c').read_bytes());(f/'txcorr-pure.c').write_bytes((P/'test.c').read_bytes())
 B=R/'research/ci-39158169212/buildtree/xnu/libkern/crypto/sweet_bearssl'
 names=['wifi_station_data_diag','wifi_station_diag','wifi_wpa_diag','wifi_station_transaction','wifi_station_wire','wifi_wpa2','wifi_wpa_owner','wifi_wpa_crypto','wifi_wpa_nst','wifi_station_tx','wifi_station_transport','wifi_normal_transport','wifi_capability','wifi_main_transport','wifi_main_command']
 crypto=[B/'src'/p for p in ('hash/sha1.c','codec/dec32be.c','codec/enc32be.c','symcipher/aes_ct.c','symcipher/aes_ct_dec.c','symcipher/aes_ct_cbcdec.c')]
 for group,fixture in [('pure','txcorr-pure.c'),('actual','txcorr-actual.c'),('submission','test-security-submission.c')]:
  if args.only and args.only!=group:continue
  files=[f/fixture]+([d/'wifi_station_data_diag.c'] if group=='pure' else [d/(n+'.c') for n in names]+crypto)
  cmd=['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-Wno-unused-function','-Wno-unused-const-variable','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(d),'-I'+str(f),'-I'+str(B/'inc'),'-I'+str(B/'src'),*map(str,files),'-o',str(d/group)]
  build=subprocess.run(cmd,text=True,capture_output=True);assert build.returncode==0,build.stderr
  run=subprocess.run([str(d/group)],text=True,capture_output=True);(args.log_dir/(group+'.log')).write_text(run.stdout+run.stderr);assert run.returncode==0,run.stdout[-3000:]+run.stderr
  rows=[l for l in run.stdout.splitlines() if l.startswith(('NORD2 WIFI TXPUB:','NORD2 WIFI TXSTATUS:','NORD2 WIFI TXFINAL:','NORD2 WIFI TXNOTE:','NORD2 WIFI TXLIMIT:'))];assert rows and all(len(l.encode())+1<=255 for l in rows)
  reports.append({'group':group,'exit':run.returncode,'last_line':run.stdout.splitlines()[-1],'numeric_rows':len(rows),'max_row_bytes':max(len(l.encode())+1 for l in rows)});print(group,reports[-1]['last_line'])
 # Actual emitter format strings are compiled at maximum integer width.
 src=(d/'wifi_station_data_diag.c').read_text();formats=re.findall(r'printf\(("NORD2 WIFI TX(?:PUB|STATUS|FINAL|NOTE|LIMIT): [^"]*")',src);assert len(formats)==5
 code='#include <stdint.h>\n#include <stdio.h>\nint main(void){\n'
 for fmt in formats:
  argsfmt=['(unsigned long long)UINT64_MAX' if x=='llu' else 'UINT32_MAX' for x in re.findall(r'%(llu|u)',fmt)]
  code+='printf('+fmt+','+','.join(argsfmt)+');\n'
 code+='return 0;}\n';(d/'width.c').write_text(code)
 w=subprocess.run(['clang','-std=c11','-Wall','-Wextra','-Werror',str(d/'width.c'),'-o',str(d/'width')],capture_output=True,text=True);assert not w.returncode,w.stderr
 width=subprocess.check_output([str(d/'width')],text=True);(args.log_dir/'maximum-width.log').write_text(width);assert len(width.splitlines())==5 and all(len(x.encode())+1<=255 for x in width.splitlines())
 reader=T/'nord2-wifi-tx-correlation.py'
 readtest=subprocess.run(['python3',str(P/'test-reader.py'),str(reader)],capture_output=True,text=True);assert not readtest.returncode,readtest.stdout+readtest.stderr
 (args.log_dir/'reader-tests.json').write_text(readtest.stdout);reader_report=json.loads(readtest.stdout)
 if not args.only or args.only=='actual':
  decoded=subprocess.run(['python3',str(reader),str(args.log_dir/'actual.log')],capture_output=True,text=True);assert not decoded.returncode,decoded.stdout+decoded.stderr
  (args.log_dir/'actual-reader.json').write_text(decoded.stdout)
 # The reset and inspection seams are host-only. Verify preprocessing under
 # both actual kernel macros without pulling unrelated platform headers.
 stub=d/'sys';stub.mkdir();(stub/'systm.h').write_text('int printf(const char *, ...);\n')
 for define in ('KERNEL','_KERNEL'):
  p=subprocess.run(['clang','-E','-P','-D'+define,'-I'+str(d),str(d/'wifi_station_data_diag.c')],text=True,capture_output=True);assert p.returncode==0,p.stderr;assert 'nsdd_test_boot_reset' not in p.stdout and 'nsdd_test_pid' not in p.stdout
 report={'passed':True,'synthetic_firmware_only':True,'new_air_wait':False,'network_success_claim':False,'kernel_reset_seam_absent_both_macros':True,'groups':reports,'reader':reader_report,'maximum_numeric_row_bytes':max(len(x.encode())+1 for x in width.splitlines()),'runtime_pins':{p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in S.iterdir() if p.suffix in('.c','.h','.inc')}}
 (args.log_dir/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
