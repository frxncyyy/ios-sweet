#!/usr/bin/env python3
"""Run mocked-only live mailbox CLI checks, never enumerate or open USB."""
import argparse,hashlib,json,subprocess,re
from pathlib import Path
T=Path(__file__).resolve().parent;R=next(p for p in T.parents if(p/'AGENTS.md').is_file());a=argparse.ArgumentParser(description=__doc__);a.add_argument('--log-dir',type=Path,required=True);v=a.parse_args();v.log_dir.mkdir(parents=True,exist_ok=True)
h=R/'shim/nord2-xnu/wifi_live_control.h';c=R/'shim/nord2-xnu/wifi_live_control.c'
fields=re.search(r'enum nwl_field\s*\{(.*?)\};',h.read_text(),re.S).group(1);fields=re.findall(r'NWL_F_(\w+)',fields);assert fields[-1]=='END' and len(fields)==59
cli=T/'nord2-wifi-live.py';module=cli.read_text();decl=re.search(r"FIELDS=\((.*?)\)\.split\(\)",module,re.S).group(1);strings=re.findall(r"'([^']*)'",decl);assert ''.join(strings).split()==[x.lower() for x in fields[:-1]]
p=subprocess.run(['python3',str(T/'fixtures/nord2-wifi-live/test.py'),str(cli)],capture_output=True,text=True);(v.log_dir/'tests.log').write_text(p.stdout+p.stderr);assert p.returncode==0,p.stdout+p.stderr
r=json.loads(p.stdout);r['source_pins']={str(x.relative_to(R)):hashlib.sha256(x.read_bytes()).hexdigest() for x in(h,c)};r['cli_sha256']=hashlib.sha256(cli.read_bytes()).hexdigest();(v.log_dir/'report.json').write_text(json.dumps(r,indent=2)+'\n');print(json.dumps(r,indent=2))
