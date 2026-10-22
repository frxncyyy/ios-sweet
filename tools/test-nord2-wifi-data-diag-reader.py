#!/usr/bin/env python3
"""Portable strict-reader regression using only numeric synthetic-source receipts."""
from pathlib import Path
import argparse,hashlib,json,re,runpy
P=Path(__file__).resolve().parent
R=next((p for p in P.parents if (p/'AGENTS.md').exists()),None)
a=argparse.ArgumentParser();a.add_argument('--emitter-source',type=Path);a.add_argument('--log-dir',type=Path);args=a.parse_args()
module=runpy.run_path(str(P/'nord2-wifi-data-diag.py'));parse=module['parse'];F=P/'fixtures/nord2-wifi-data-diag-reader'
actual=(F/'actual.log').read_text();pure=(F/'pure.log').read_text();rows=actual.splitlines();positive=negative=0

def good(text,complete=True):
 global positive
 text=text.rstrip('\n')+'\n' if text else text
 r=parse(text,require_complete=complete);assert r['complete']==complete
 assert r['pid127_ambiguous'] and not any(r[k] for k in ('last_publication_is_correlation','rx_is_bsd_delivery','dhcp_is_lease_proof','internet_proven','air_delivery_proven','capture_integrity_proven','txs_authorization_bucket_available'))
 positive+=1;return r

def bad(text,prefix=False,unterminated=False):
 global negative
 if text and not unterminated:text=text.rstrip('\n')+'\n'
 try:parse(text,require_complete=not prefix)
 except ValueError:negative+=1
 else:raise AssertionError('accepted malformed/incomplete diagnostic')

def change(index,key,value):
 v=rows.copy();v[index]=re.sub(r'\b'+key+r'=[0-9]+',key+'='+str(value),v[index]);return '\n'.join(v)

bad(actual.rstrip('\n'),unterminated=True)
r=good(actual);assert len(r['attempts'])==1 and r['attempts'][0]['summary']['rx_dhcp']==2
#719 fixture intentionally contains an unfinished generation18 and a seeded
#saturated summary. They remain observations, never a complete native receipt.
first=pure[:pure.index('NORD2 WIFI DATAHDR: n=1 g=18')];good(first);good(pure,False);bad(pure)
for stop in (1,2,5,10,len(rows)-1):bad('\n'.join(rows[:stop]));good('\n'.join(rows[:stop]),False)
good('unrelated NST/WPA/backend text only',False);bad('')
# Every counter bound and exact schema is checked on genuine emitter rows.
for i,line in enumerate(rows):
 for key,val in re.findall(r'([a-z_]+)=([0-9]+)',line):
  bad(change(i,key,(1<<64) if key=='g' else (1<<32)))
 for private in ('ssid','mac','key','nonce','mic','replay','xid','payload'):
  v=rows.copy();v[i]+=' '+private+'=1';bad('\n'.join(v))
 bad('\n'.join(rows[:i]+[line+' trailing']+rows[i+1:]))
# Correlation and field boundaries not detectable by uint32 alone.
h=next(i for i,r in enumerate(rows) if ' dhcp=1 ' in r);host=next(i for i,r in enumerate(rows) if ' pid=127 ' in r)
for i,key,value in [(0,'pid',256),(0,'wlan',256),(0,'status',256),(0,'ambiguous',1),(0,'last_kind',1),(0,'last_pub',1),(1,'seq',65536),(1,'tx_count',256),(1,'rate',65536),(1,'flags',256),(1,'adv',2),(1,'tid',256),(1,'flush',256),(1,'seq',1),(1,'adv',1),(h,'dir',4),(h,'dst',3),(h,'eth',65536),(h,'bytes',13),(h,'bytes',1519),(h,'dhcp',9),(h,'parse',16),(h,'dhcp',0),(h,'eth',2054),(h,'bytes',284),(host,'ambiguous',0),(host+1,'adv',0),(host+1,'flags',0)]:bad(change(i,key,value))
# A TXS has no generation in part2: allow unrelated logs, never another target
#record between its parts. No pub context/PID is used to correlate delivery.
good(actual.replace(rows[0]+'\n'+rows[1],rows[0]+'\nunrelated console message\n'+rows[1]))
for v in [rows[1:],rows[:1]+rows[2:],rows[:1]+[rows[0]]+rows[1:],[rows[1],rows[0]]+rows[2:],rows[:1]+[rows[4]]+rows[1:],rows[:1]+[rows[0].replace('g=17','g=18')]+rows[1:]]:bad('\n'.join(v))
# Missing/duplicated/interleaved summary parts cannot complete.
start=next(i for i,r in enumerate(rows) if ' DATACOUNT:' in r)
for i in range(start,len(rows)):
 bad('\n'.join(rows[:i]+rows[i+1:]));bad('\n'.join(rows[:i]+[rows[i]]+rows[i:]))
 bad(change(i,'g',18))
# Source-observed destinations and unlisted EtherTypes remain lower bounds
#even when the terminal partitions still sum to the same totals.
bad(change(start+1,'uni',0).replace('dir=1 uni=0 group=2','dir=1 uni=0 group=4'))
bad(change(4,'eth',4660))
bad(change(start+3,'uni',0).replace('dir=2 uni=0 group=1','dir=2 uni=0 group=2'))
bad(change(start+3,'group',0).replace('dir=2 uni=1 group=0','dir=2 uni=2 group=0'))
bad(change(start,'tx_data',0));bad(change(start,'rx_uni',0));bad(change(start,'tx_dhcp',0));bad(change(start,'ambiguous',99));bad(change(start+2,'discover',0));bad(actual+'\n'+rows[4])
# Full rows with omitted final observations also cannot become complete.
bad('\n'.join(rows[:10]+rows[12:]))
# Reservation limits are enforced even without summary/prefix completion.
for dhcp,limit in ((0,4),(1,12)):
 h='NORD2 WIFI DATAHDR: n={n} g=1 dir=2 dst=0 eth=2048 bytes=286 dhcp='+str(dhcp)+' parse='+str(dhcp)
 bad('\n'.join(h.format(n=i+1) for i in range(limit+1)),True)
t=[]
for i in range(25):
 t+=['NORD2 WIFI DATATXS: n='+str(i+1)+' part=1 g=1 pid=127 wlan=255 status=255 ambiguous=1 last_kind=3 last_pub=4294967295','NORD2 WIFI DATATXS: n='+str(i+1)+' part=2 seq=0 tx_count=0 rate=0 flags=0 adv=0 tid=0 flush=0']
bad('\n'.join(t),True)
bad('\n'.join('NORD2 WIFI DATAHDR: n=1 g='+str(g)+' dir=2 dst=0 eth=2054 bytes=14 dhcp=0 parse=0' for g in range(1,66)),True)
# Zero counters can genuinely summarize no observations; full-width generation
#does not weaken correlation. Saturated seeded data alone stays incomplete.
z='\n'.join(re.sub(r'\b([a-z_]+)=[0-9]+',lambda m:m[1]+'='+('18446744073709551615' if m[1]=='g' else m[0].split('=')[1] if m[1]=='dir' else '0'),x) for x in rows[start:]);good(z)
source=args.emitter_source or (R/'shim/nord2-xnu/wifi_station_data_diag.c' if R else None)
assert source and source.is_file(),'provide canonical emitter or --emitter-source'
formats=re.findall(r'printf\("(NORD2 WIFI (?:DATA|DHCPSTAT)[^"\n]+)"',source.read_text());assert len(formats)==6
for fmt in formats:
 sample=fmt.replace('\\n','').replace('%llu','1').replace('%u','1')
 assert any(p.fullmatch(sample) for p in module['PAT'].values()),'source schema drift'
assert not any(x in ''.join(formats) for x in ('%s','%p','nonce=','mic=','replay=','ssid=','mac=','key=','xid='))
report={'passed':True,'positive':positive,'negative':negative,'actual_source_formats':len(formats),'emitter_sha256':hashlib.sha256(source.read_bytes()).hexdigest(),'hardware_or_lease_claim':False}
if args.log_dir:args.log_dir.mkdir(parents=True,exist_ok=True);(args.log_dir/'reader-report.json').write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report))
