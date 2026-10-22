#!/usr/bin/env python3
"""Offline real-source GTK rekey tests; synthetic hardware and public keys only."""
from pathlib import Path
import argparse,tempfile,subprocess,json,hashlib
R=next(p for p in Path(__file__).resolve().parents if (p/'AGENTS.md').exists())
a=argparse.ArgumentParser();a.add_argument('--candidate',type=Path);a.add_argument('--hardware-fixture',type=Path,help='Optional reviewed normal TX hardware model');a.add_argument('--only',choices=['engine','native']);a.add_argument('--log-dir',type=Path);args=a.parse_args()
P=Path(__file__).resolve().parent/'fixtures/nord2-wifi-gtk-rekey';F=R/'tools/fixtures/nord2-wifi-station';S=R/'shim/nord2-xnu';B=R/'research/ci-39158169212/buildtree/xnu/libkern/crypto/sweet_bearssl'
base=['wifi_wpa_diag','wifi_station_data_diag','wifi_station_diag','wifi_station_transaction','wifi_station_wire','wifi_wpa2','wifi_wpa_owner','wifi_wpa_crypto']
plan={'engine':('test-engine.c',[]),'native':('test-native.c',['wifi_wpa_nst','wifi_station_transport','wifi_normal_transport','wifi_capability','wifi_main_transport','wifi_main_command'])};report=[]
with tempfile.TemporaryDirectory(prefix='wifi-gtk-') as td:
 d=Path(td)
 for folder in [S]+([args.candidate] if args.candidate else []):
  for f in folder.iterdir():
   if f.suffix in ('.c','.h','.inc'):(d/f.name).write_bytes(f.read_bytes())
 for f in P.iterdir():
  if f.suffix in ('.c','.h'):(d/f.name).write_bytes(f.read_bytes())
 actual=(F/'actual-transport-test.c').read_text();needle='C(nwn_pump(&x->n,x->h->now/1000)>=0);';assert actual.count(needle)==1
 actual=actual.replace(needle,'int pr=nwn_pump(&x->n,x->h->now/1000);if(pr<0)fprintf(stderr,"pump phase%u out%u group%u nhalt%u err%u cid%u added%u serial%u/%u\\n",x->n.owner.transaction.phase,x->n.owner.transaction.outcome,x->n.owner.transaction.group_active,x->h->s.halt,x->h->s.error,x->h->s.wire_cid,x->h->s.gtk_command_added,x->h->s.gtk_submission_serial,x->h->s.gtk_replay_serial);C(pr>=0);')
 (d/'actual-transport-test.c').write_text(actual)
 # Add read-only observation to the existing hardware model, preserving actual
 # nst/nwn/nwo/stc guards and callbacks. No mock permission or direct receipt.
 hardware=(args.hardware_fixture or F/'transport-hardware.inc').read_text();needle='consume(t);if(t->s.wire_type)';assert hardware.count(needle)==1
 hardware=hardware.replace(needle,'gtk_test_publish(t);'+needle)
 normal_needle='deliver(t,3,b,sizeof b);}return 0;}if(a==0x18025418)'
 if '#include \"wifi_station_tx.h\"' in (d/'wifi_station_transport.h').read_text():
  assert hardware.count(normal_needle)==1;hardware=hardware.replace(normal_needle,'deliver(t,3,b,sizeof b);}gtk_test_publish(t);return 0;}if(a==0x18025418)')
 (d/'transport-hardware.inc').write_text(hardware)
 for name,(fixture,extra) in plan.items():
  if args.only and name!=args.only:continue
  normal_extra=['wifi_station_tx'] if name=='native' and (d/'wifi_station_tx.c').exists() else []
  files=[d/fixture]+[d/(n+'.c') for n in base+extra+normal_extra]+[B/'src'/x for x in ['hash/sha1.c','codec/dec32be.c','codec/enc32be.c','symcipher/aes_ct.c','symcipher/aes_ct_dec.c','symcipher/aes_ct_cbcdec.c']]
  command=['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-Wno-unused-const-variable','-Wno-unused-function','-fsanitize=address,undefined','-fno-sanitize-recover=all','-pthread','-I'+str(d),'-I'+str(F),'-I'+str(B/'inc'),'-I'+str(B/'src'),*map(str,files),'-o',str(d/name)]
  subprocess.run(command,check=True);result=subprocess.run([str(d/name)],text=True,capture_output=True);print(name,result.returncode,result.stdout.splitlines()[-1:] if result.stdout else result.stderr[-2000:])
  if args.log_dir:args.log_dir.mkdir(parents=True,exist_ok=True);(args.log_dir/(name+'.log')).write_text(result.stdout+result.stderr)
  report.append({'suite':name,'exit':result.returncode,'source_sha256':{f.name:hashlib.sha256(f.read_bytes()).hexdigest() for f in files}});result.check_returncode()
if args.log_dir:(args.log_dir/'report.json').write_text(json.dumps({'synthetic_firmware_only':True,'hardware_claim':False,'suites':report},indent=2)+'\n')
