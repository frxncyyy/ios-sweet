#!/usr/bin/env python3
"""Actual-source offline power/deauth regression. No device or RF operations."""
from pathlib import Path
import argparse,subprocess
P=Path(__file__).resolve().parent;R=next(p for p in P.parents if(p/'AGENTS.md').exists());F=P/'fixtures/nord2-wifi-power-cleanup'
a=argparse.ArgumentParser();a.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');a.add_argument('--log-dir',type=Path,required=True);a.add_argument('--only',choices=['all','owner','transport','client'],default='all');x=a.parse_args();W=x.log_dir.resolve();W.mkdir(parents=True,exist_ok=True)
def run(name,script,arguments):
 d=W/name;d.mkdir(exist_ok=True);p=d/'run.py';p.write_text(script);subprocess.run(['python3',str(p),*arguments],check=True)
if x.only in ('all','owner'):
 s=(R/'tools/test-nord2-wifi-lifecycle.py').read_text();s=s.replace("P=Path(__file__).resolve().parent;R=next(p for p in P.parents if (p/'AGENTS.md').exists());F=P/'fixtures/nord2-wifi-lifecycle';P=R/'tools'",'R=Path('+repr(str(R))+');F=Path('+repr(str(F))+');P=R/"tools"')
 run('owner',s,['--candidate',str(x.candidate.resolve()),'--log-dir',str(W/'owner')])
if x.only in ('all','transport'):
 s=(R/'tools/test-nord2-wifi-station.py').read_text().replace("R=next(p for p in Path(__file__).resolve().parents if (p/'AGENTS.md').exists())",'R=Path('+repr(str(R))+')').replace("P=Path(__file__).resolve().parent/'fixtures/nord2-wifi-station'",'P=R/"tools/fixtures/nord2-wifi-station"').replace("'airtimeout':('test-native-air-timeout.c',","'airtimeout':("+repr(str(F/'test-deauth.c'))+",")
 run('transport',s,['--candidate',str(x.candidate.resolve()),'--only','airtimeout','--log-dir',str(W/'transport')])
if x.only in ('all','client'):
 s=(R/'tools/test-nord2-wifi-ios-client.py').read_text().replace("R=next(x for x in Path(__file__).resolve().parents if(x/'AGENTS.md').exists());P=Path(__file__).resolve().parent/'fixtures/nord2-wifi-ios-client';",'R=Path('+repr(str(R))+');P=R/"tools/fixtures/nord2-wifi-ios-client";')
 inject=''' source=source.replace('static unsigned logical_power=1,power_calls;', 'static unsigned logical_power=1,power_calls,power_unavailable;').replace('power_calls++;if(on!=logical_power)', 'if(power_unavailable)return 1;power_calls++;if(on!=logical_power)').replace('extern "C" int nord2_wifi_scan_info(nss_info*out){*out={};', 'extern "C" int nord2_wifi_scan_info(nss_info*out){if(power_unavailable)return 1;*out={};')
'''
 anchor=" source+=(P/'test-join-stubs.inc').read_text();";assert anchor in s;s=s.replace(anchor,inject+anchor).replace("(P/'test-disconnect.inc').read_text()",repr((F/'test-power.inc').read_text()))
 run('client',s,['--candidate',str(x.candidate.resolve()),'--only','disconnect','--log',str(W/'client/client.log')])
