#!/usr/bin/env python3
"""Synthetic multiple-generation connection observations; no device/identity data."""
from pathlib import Path
import argparse,hashlib,json,runpy
R=next(p for p in Path(__file__).resolve().parents if (p/'AGENTS.md').exists())
a=argparse.ArgumentParser();a.add_argument('--reader',type=Path,default=R/'tools/nord2-wifi-connection-log.py');a.add_argument('--log-dir',type=Path,required=True);z=a.parse_args();z.log_dir.mkdir(parents=True,exist_ok=True);m=runpy.run_path(str(z.reader));parse=m['parse'];checks=0

def row(kind,d):return 'NORD2 WIFI '+kind+': '+' '.join(f'{k}={v}'for k,v in d.items())+'\n'
def station(g,ready=False,tx=0,rx=0):
 d={k:0 for k in m['STATION_LOCAL']};d.update(generation=g,ticket=g,service=6 if ready else 3,result=int(ready),linked=int(ready),transaction=15 if ready else 1,outcome=int(ready),associated=int(ready),sta=int(ready),ptk=int(ready),gtk=int(ready),port=int(ready),tx_accept=tx,tx_local=tx,rx_delivered=rx,t_ms=100*g)
 return row('IOS STATION',d)
def notice(g,current=1,ready=True):return row('IOS JOINNOTICE',dict(notice=1 if ready else 2,current=current,phase=6 if ready else 8,result=1 if ready else 2,authorized=int(ready),generation=g,ticket=g,epoch=g+10,t_ms=100*g))
def complete(g,tx=0,rx=0):
 return row('IFNET LINK',dict(generation=g,authorized=1,changed=1,error=0))+row('IOS EVENT',dict(event=9,published=1,notified=1,status=0,producer=48,consumer=0,generation=g,ticket=g,epoch=g+10,t_ms=100*g))+notice(g)+station(g,True,tx,rx)
def good(text):
 global checks
 r=parse(text.encode());assert r['valid'],r
 assert not any(r[k]for k in ('dhcp_exchange_proven','ip_acquisition_proven','internet_connectivity_proven'));checks+=1;return r

first=complete(1,5,8)
r=good(first+station(2,False,5,8));assert r['authorized_link_observed']and not r['current_authorized_link_observed'];assert not r['attempts'][0]['current_authorized_link_observed']
second=r['attempts'][1];assert not second['data_local_submission_observed']and not second['bsd_input_success_observed']
r=good(first+station(2,False,5,8)+complete(2,5,8));assert r['current_authorized_link_observed']and r['attempts'][1]['current_authorized_link_observed']and not r['attempts'][0]['current_authorized_link_observed'];assert not r['attempts'][1]['data_local_submission_observed']
r=good(first+station(2,False,5,8)+complete(2,6,9));assert r['attempts'][1]['data_local_submission_observed']and r['attempts'][1]['bsd_input_success_observed']
r=good(complete(2,9,12));assert r['current_authorized_link_observed']and not r['data_local_submission_observed']and not r['bsd_input_success_observed']
r=good(complete(2,9,12)+station(2,True,10,13));assert r['data_local_submission_observed']and r['bsd_input_success_observed']
r=good(first+notice(99,current=0));assert r['current_authorized_link_observed'] # stale metadata does not displace current
r=good(first+notice(2,ready=False));assert not r['current_authorized_link_observed']
r=good(first+row('IFNET LINK',dict(generation=2,authorized=0,changed=1,error=0)));assert not r['current_authorized_link_observed']
r=parse((station(2)+station(1)).encode());assert not r['valid']and not r['native_authorized_observed'];checks+=1
# Per-model command counters may reset; boot traffic observations remain scoped.
s=first+station(2,False,5,8);s=s.replace('submitted=0 dma=0','submitted=90 dma=90',1);good(s)
# Existing RX/extended readers key generations; WPA and PID budgets are boot-static.
log=R/'research/nord2/wifi/live-control-20261023/rejoin-candidate/proposed/verification/durable/test-native.log'
actual={}
if log.is_file():
 text=log.read_text();r=good(text);assert len(r['attempts'])>=3
 for rel in ('tools/nord2-wifi-wpa-diag.py','tools/nord2-wifi-data-diag.py','research/nord2/wifi-ios-live/ios/extended-reader/proposed/tools/nord2-wifi-data-extended.py','tools/nord2-wifi-rx-error.py','tools/nord2-wifi-tx-correlation.py'):
  mod=runpy.run_path(str(R/rel));result=mod['parse'](text,**({'require_complete':False}if 'data-diag' in rel else {}));assert result.get('valid',True);actual[rel]='PASS';checks+=1
 (z.log_dir/'multi-generation-numeric.log').write_text(first+station(2,False,5,8)+complete(2,6,9))
report={'passed':True,'checks':checks,'reader_sha256':hashlib.sha256(z.reader.read_bytes()).hexdigest(),'actual_rejoin_log_sha256':hashlib.sha256(log.read_bytes()).hexdigest() if log.is_file() else None,'other_readers':actual,'no_hardware_claim':True,'no_lease_or_IP_claim':True}
(z.log_dir/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report))
