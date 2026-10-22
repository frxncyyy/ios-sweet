#!/usr/bin/env python3
"""Offline actual retained-owner scan return regression; synthetic hardware only.
Uses canonical source, existing authenticated-board fixture and checked owner IO.
No optional candidate overlay, RF claim, or device access.
"""
from pathlib import Path
import argparse, subprocess, tempfile
P=Path(__file__).resolve().parent
R=next(p for p in P.parents if (p/'AGENTS.md').exists())
a=argparse.ArgumentParser();a.add_argument('--log-dir',type=Path);a.add_argument('--board',type=Path);args=a.parse_args()
with tempfile.TemporaryDirectory(prefix='wifi-scan-return-') as tmp:
 W=args.log_dir.resolve() if args.log_dir else Path(tmp);W.mkdir(parents=True,exist_ok=True)
 s=(P/'test-nord2-wifi-join-owner.py').read_text()
 start=s.index(" test+=\'\'\'");end=s.index(" (d/\'test.c\').write_text(test)",start)
 s=s[:start]+" test+="+repr((P/'fixtures/nord2-wifi-scan-return/owner-extra.c').read_text())+'\n'+s[end:]
 old="P=Path(__file__).resolve().parent;R=next(p for p in P.parents if (p/'AGENTS.md').exists());"
 assert old in s;s=s.replace(old,'R=Path('+repr(str(R))+');P=R/"tools";')
 insertion=" owner=(d/'owner-fixture.inc').read_text().replace('static unsigned normal_enabled,','static unsigned station_hardware;static void station_completion(unsigned);\\nstatic unsigned normal_enabled,').replace('if(normal_enabled&&t->completed>=5){','if(normal_enabled&&t->completed>=5){if(station_hardware){station_completion(next);return;}')\n (d/'owner-fixture.inc').write_text(owner)\n # Existing full native fixture's probes"
 assert " # Existing full native fixture's probes" in s;s=s.replace(" # Existing full native fixture's probes",insertion)
 f=W/'runner.py';f.write_text(s)
 cmd=['python3',str(f),'--log-dir',str(W)]
 if args.board:cmd+=['--board',str(args.board.resolve())]
 subprocess.run(cmd,check=True)
