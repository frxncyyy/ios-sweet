#!/usr/bin/env python3
"""Offline actual-source WPA2/station tests. No network, device or firmware IO."""
from pathlib import Path
import argparse,tempfile,subprocess,json,hashlib
R=next(p for p in Path(__file__).resolve().parents if (p/'AGENTS.md').exists())
a=argparse.ArgumentParser();a.add_argument('--candidate',type=Path,help='Private wpa2-native-candidate overlay');a.add_argument('--native-candidate',type=Path,help='Optional concurrently reviewed native transport delta');a.add_argument('--bearssl',type=Path);a.add_argument('--only',choices=['owner','connector','actual','service','review','native','retry','airtimeout','submission']);a.add_argument('--log-dir',type=Path);args=a.parse_args()
S=R/'shim/nord2-xnu';P=Path(__file__).resolve().parent/'fixtures/nord2-wifi-station';B=args.bearssl or R/'research/ci-39158169212/buildtree/xnu/libkern/crypto/sweet_bearssl'
base=['wifi_station_diag','wifi_station_transaction','wifi_station_wire','wifi_wpa2','wifi_wpa_owner','wifi_wpa_crypto']
plan={'submission':('test-security-submission.c',['wifi_wpa_nst','wifi_station_transport','wifi_normal_transport','wifi_capability','wifi_main_transport','wifi_main_command']),'airtimeout':('test-native-air-timeout.c',['wifi_wpa_nst','wifi_station_transport','wifi_normal_transport','wifi_capability','wifi_main_transport','wifi_main_command']),'retry':('test-retry.c',['wifi_station_transport','wifi_normal_transport','wifi_capability','wifi_main_transport','wifi_main_command']),'native':('native-regression-test.c',['wifi_station_transport','wifi_normal_transport','wifi_capability','wifi_main_transport','wifi_main_command','wifi_scan_bss']),'review':('test-review.c',[]),'owner':('test.c',[]),'connector':('transport-test.c',['wifi_wpa_nst']),'actual':('actual-transport-test.c',['wifi_wpa_nst','wifi_station_transport','wifi_normal_transport','wifi_capability','wifi_main_transport','wifi_main_command']),'service':('service-test.c',['wifi_wpa_nst','wifi_ios_join'])};report=[]
with tempfile.TemporaryDirectory(prefix='wifi-station-') as td:
 d=Path(td)
 for f in S.iterdir():
  if f.suffix in ('.c','.h','.inc'):(d/f.name).write_bytes(f.read_bytes())
 if args.candidate:
  for folder in [args.candidate]+([args.native_candidate] if args.native_candidate else []):
   for f in folder.iterdir():
    if f.suffix in ('.c','.h','.inc'):(d/f.name).write_bytes(f.read_bytes())
 for name,(fixture,extra) in plan.items():
  if args.only and name!=args.only:continue
  files=[P/fixture]+[d/(n+'.c') for n in base+extra]+[B/'src'/x for x in ['hash/sha1.c','codec/dec32be.c','codec/enc32be.c','symcipher/aes_ct.c','symcipher/aes_ct_dec.c','symcipher/aes_ct_cbcdec.c']]
  cmd=['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-Wno-unused-const-variable','-Wno-unused-function','-fsanitize=address,undefined','-fno-sanitize-recover=all','-pthread','-I'+str(d),'-I'+str(P),'-I'+str(B/'inc'),'-I'+str(B/'src'),*map(str,files),'-o',str(d/name)]
  subprocess.run(cmd,check=True);result=subprocess.run([str(d/name)],text=True,capture_output=True);print(name,result.returncode,result.stdout.splitlines()[-1:] if result.stdout else result.stderr[-1000:])
  if args.log_dir:args.log_dir.mkdir(parents=True,exist_ok=True);(args.log_dir/(name+'.log')).write_text(result.stdout+result.stderr)
  report.append({'suite':name,'exit':result.returncode,'source_sha256':{str(f):hashlib.sha256(f.read_bytes()).hexdigest() for f in files}});result.check_returncode()
if args.log_dir:(args.log_dir/'report.json').write_text(json.dumps({'synthetic_firmware_only':True,'hardware_claim':False,'suites':report},indent=2)+'\n')
