#!/usr/bin/env python3
"""Actual canonical IOUC timing concurrency/limit tests, no hardware or credentials."""
from pathlib import Path
import argparse,subprocess,re
R=next(p for p in Path(__file__).resolve().parents if(p/'AGENTS.md').exists());D=Path(__file__).resolve().parent
ap=argparse.ArgumentParser();ap.add_argument('--log-dir',type=Path,required=True);ap.add_argument('--candidate',type=Path);ap.add_argument('--fixture-dir',type=Path,default=D/'fixtures/nord2-wifi-ios-timing');a=ap.parse_args();W=a.log_dir.resolve();W.mkdir(parents=True,exist_ok=True);F=a.fixture_dir.resolve();S=a.candidate.resolve() if a.candidate else R/'shim/nord2-xnu'
source=(S/'wifi_ios_client.cpp').read_text();formats={}
for kind in ('QUERYBEGIN','QUERYEND'):
 found=re.findall(r'printf\(("NORD2 WIFI IOS '+kind+r':[^"\n]*")',source);assert len(found)==1;formats[kind]=found[0]
width_header=''.join('#define '+k+'_FORMAT '+v+'\n' for k,v in formats.items())
script=(R/'tools/test-nord2-wifi-ios-client.py').read_text();script=script[:script.index(' logs=[]')]
script=script.replace("P=Path(__file__).resolve().parent/'fixtures/nord2-wifi-ios-client'","P=R/'tools/fixtures/nord2-wifi-ios-client'")
script=script.replace("R=next(x for x in Path(__file__).resolve().parents if(x/'AGENTS.md').exists())", "R=Path("+repr(str(R))+")")
script=script.replace("(P/'test-extra.inc').read_text()",repr((F/'timing.inc').read_text()))
script=script.replace("source=(B/'test-integrated.cpp').read_text()", "source=(B/'test-integrated.cpp').read_text().replace('#include \\\"LifecycleShell.cpp\\\"','#include \\\"timing-hook.h\\\"\\n#include \\\"LifecycleShell.cpp\\\"')")
# test-xnu guards let hook include the model before printf is redirected.
script=script.replace("(d/'test-xnu.h').write_text(h)","h=h.replace('struct IOLock {','static thread_local unsigned timing_locks_held;\\nstruct IOLock {').replace('p->m.lock();','p->m.lock();timing_locks_held++;').replace('p->m.unlock();','assert(timing_locks_held);timing_locks_held--;p->m.unlock();');(d/'test-xnu.h').write_text(h);(d/'timing-hook.h').write_text("+repr(width_header+(F/'timing-hook.h').read_text())+")")
script+='''\n subprocess.run(['clang++','-std=c++17',*flags,'-Wno-unused-function','-pthread',str(d/'test.cpp'),*objs,'-o',str(d/'test')],check=True)\n result=subprocess.run([str(d/'test')],text=True,capture_output=True);print(result.stdout);print(result.stderr)\n if args.log:args.log.write_text(result.stdout+result.stderr)\n result.check_returncode()\n'''
f=W/'timing-runner.py';f.write_text(script);subprocess.run(['python3',str(f),'--candidate',str(S),'--log',str(W/'timing.log')],check=True)
