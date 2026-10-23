#!/usr/bin/env python3
"""Finite strict-reader tests; fixtures contain public numeric metadata only."""
import importlib.util,json,sys
from pathlib import Path
p=Path(sys.argv[1]);spec=importlib.util.spec_from_file_location('reader',p);m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
pos=neg=0
pub='NORD2 WIFI TXPUB: g=1 pid=64 wlan=5 kind=3 protected=1 ether=2048 dhcp=1 parse=1 bytes=342 class=0 pub=7\n'
status='NORD2 WIFI TXSTATUS: g=1 pid=64 wlan=5 status=3 count=30 rate=0 flags=3 adv=1 seq=65535 tid=0 flush=0\n'
final='NORD2 WIFI TXFINAL: g=1 pid=64 wlan=5 kind=3 attempt=1 pub=7 local=1 seen=1 status=3\n'
def yes(s):
 global pos
 r=m.parse(s);assert r['valid'];assert not r['DHCP_lease_proven'] and not r['TXS_releases_DMA_or_token'];pos+=1;return r
def no(s):
 global neg
 try:m.parse(s)
 except (ValueError,UnicodeError):neg+=1;return
 raise AssertionError('accepted contradiction '+s[:180])
yes('');yes(pub);yes(pub+status);yes(pub+status+final);yes(status);yes(final)
r=yes(status);assert not r['samples'][0]['status_attributed_to_observed_publication']
r=yes(pub+status);assert r['samples'][0]['status_attributed_to_observed_publication'] and not r['samples'][0]['firmware_mac_success_reported']
yes(pub+status.replace('status=3','status=0')+final.replace('status=3','status=0'))
r=yes(pub+final.replace('seen=1 status=3','seen=0 status=256'));assert r['samples'][0]['publication_state']=='CONFIRMED'
r=yes(final.replace('pub=7 local=1 seen=1 status=3','pub=0 local=0 seen=0 status=256'));assert r['samples'][0]['publication_state']=='UNCERTAIN'
yes(final.replace('attempt=1 pub=7 local=1 seen=1 status=3','attempt=0 pub=0 local=0 seen=0 status=256'))
yes(pub.replace('g=1','g=18446744073709551615').replace('pub=7','pub=4294967295'))
yes('NORD2 WIFI TXLIMIT: g=1 kind=1 limit=31 fallback=0\nNORD2 WIFI TXLIMIT: g=1 kind=2 limit=63 fallback=127\nNORD2 WIFI TXLIMIT: g=1 kind=3 limit=126 fallback=127\n')
for reason in range(1,7):
 original=0 if reason==1 else 2 if reason==3 else 1
 yes(f'NORD2 WIFI TXNOTE: g=1 original={original} pid=64 wlan=5 reason={reason} count=1\n')
for s in [pub+pub,pub+status+status,pub+status+final+status,status+pub,pub+status+final+final,pub+status+final.replace('status=3','status=0'),pub+final.replace('pub=7','pub=8'),pub+final.replace('attempt=1','attempt=0'),pub+final.replace('pub=7','pub=0'),pub+final.replace('seen=1 status=3','seen=0 status=3'),pub+status.replace('adv=1','adv=0'),pub+status.replace('flags=3','flags=0'),pub+status.replace('flags=3 adv=1','flags=0 adv=0'),pub+status.replace('wlan=5','wlan=6'),pub+status.replace('g=1','g=2'),pub+status.replace('pid=64','pid=127')]:no(s)
for row in (pub,status,final):
 for field,value in [('g','0'),('g','18446744073709551616'),('pid','0'),('pid','127'),('pid','4294967296'),('wlan','255')]:
  import re
  no(re.sub(r'\b'+field+r'=\d+',field+'='+value,row))
 no(row[:-1]);no(row.rstrip('\n')+' extra=1\n');no(row.replace(' g=1',' g=-1'));no(row.replace(' g=1',' g=01'));no(' '+row)
for field,value in [('protected',0),('ether',34958),('class',1),('bytes',1515),('dhcp',9),('parse',16),('pub',0)]:
 import re
 no(re.sub(r'\b'+field+r'=\d+',field+'='+str(value),pub))
for field,value in [('status',256),('count',256),('rate',65536),('flags',256),('adv',2),('seq',65536),('tid',256),('flush',256)]:
 no(re.sub(r'\b'+field+r'=\d+',field+'='+str(value),status))
for key in ('ssid','mac','key','nonce','replay','payload'):
 no(pub.rstrip('\n')+' '+key+'=1\n')
for kind in range(1,4):
 no(f'NORD2 WIFI TXLIMIT: g=1 kind={kind} limit=127 fallback=127\n')
no('NORD2 WIFI TXNOTE: g=1 original=1 pid=64 wlan=5 reason=3 count=1\n')
no('NORD2 WIFI TXNOTE: g=1 original=0 pid=127 wlan=5 reason=1 count=1\n')
no('NORD2 WIFI TXNOTE: g=1 original=0 pid=64 wlan=5 reason=1 count=3\n')
print(json.dumps({'passed':True,'positive':pos,'negative':neg,'secret_material_in_fixtures':False,'partial_rows_never_create_publication_proof':True}))
