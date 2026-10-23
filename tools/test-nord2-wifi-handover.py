#!/usr/bin/env python3
"""Offline actual retained-owner first-switch ordering. No device or RF IO."""
from pathlib import Path
import argparse,subprocess,json,hashlib
R=next(p for p in Path(__file__).resolve().parents if(p/'AGENTS.md').exists());F=Path(__file__).resolve().parent/'fixtures/nord2-wifi-handover'
a=argparse.ArgumentParser();a.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');a.add_argument('--board',type=Path,required=True);a.add_argument('--bearssl',type=Path,default=R/'research/ci-39158169212/buildtree/xnu/libkern/crypto/sweet_bearssl');a.add_argument('--log-dir',type=Path,required=True);x=a.parse_args();x.log_dir.mkdir(parents=True,exist_ok=False)
def pins():return {p.name:hashlib.sha256(p.read_bytes()).hexdigest()for p in x.candidate.iterdir()if p.is_file()}
before=pins();v=subprocess.run(['python3',str(F/'run-owner.py'),'--candidate',str(x.candidate),'--board',str(x.board),'--bearssl',str(x.bearssl),'--initial-open','0','--log-dir',str(x.log_dir/'owning')],capture_output=True,text=True);(x.log_dir/'runner.log').write_text(v.stdout+v.stderr);v.check_returncode();assert before==pins(),'candidate changed during owning test';print(v.stdout,end='');(x.log_dir/'report.json').write_text(json.dumps({'passed':True,'hardware_claim':False,'sources':before},indent=2)+'\n')
