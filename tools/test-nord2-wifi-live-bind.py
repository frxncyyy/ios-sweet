#!/usr/bin/env python3
from pathlib import Path
import argparse,subprocess,tempfile,hashlib,json
R=next(p for p in Path(__file__).resolve().parents if(p/'AGENTS.md').is_file());T=Path(__file__).resolve().parent
p=argparse.ArgumentParser();p.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');p.add_argument('--log-dir',type=Path,required=True);a=p.parse_args();a.log_dir.mkdir(parents=True,exist_ok=True);S=a.candidate.resolve();F=T/'fixtures/nord2-wifi-live-control/binder.c';names=['wifi_live_control.c','wifi_live_control.h','wifi_live_control_bind.inc','wifi_station_data_diag.c','wifi_station_data_diag.h','wifi_station_transport.h'];pins={n:hashlib.sha256((S/n).read_bytes()).hexdigest() for n in names}
with tempfile.TemporaryDirectory(prefix='nwl-bind-') as td:
 exe=Path(td)/'test';c=subprocess.run(['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-pthread','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(S),'-I'+str(R/'tools/fixtures/nord2-wifi-data-diag'),str(F),str(S/'wifi_station_data_diag.c'),'-o',str(exe)],capture_output=True,text=True);(a.log_dir/'compile.log').write_text(c.stdout+c.stderr);assert c.returncode==0,c.stderr
 r=subprocess.run([str(exe)],capture_output=True,text=True,timeout=60);(a.log_dir/'binder.log').write_text(r.stdout+r.stderr);assert r.returncode==0,r.stderr;assert r.stdout.splitlines()[-1].startswith('PASS ')
 rows=[x for x in r.stdout.splitlines() if x.startswith('NORD2 WIFI LIVEFRAME:')];assert len(rows)==130 and all(len(x.encode())+1<=255 for x in rows)
assert pins=={n:hashlib.sha256((S/n).read_bytes()).hexdigest() for n in names}
report={'passed':True,'source_pins':pins,'fixture_sha256':hashlib.sha256(F.read_bytes()).hexdigest(),'last':r.stdout.splitlines()[-1],'actual_binder':True,'native_operations':'typed host doubles; no device/firmware proof'};(a.log_dir/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
