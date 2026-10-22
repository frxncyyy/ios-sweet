#!/usr/bin/env python3
"""Strict bounded numeric data diagnostics. Observations are never delivery/IP proof."""
import argparse,json,re,sys
U32=(1<<32)-1;U64=(1<<64)-1
SCHEMAS={
 'DATAHDR':'n g dir dst eth bytes dhcp parse',
 'DATATXS1':'n part g pid wlan status ambiguous last_kind last_pub',
 'DATATXS2':'n part seq tx_count rate flags adv tid flush',
 'DATACOUNT':'g tx_sec tx_data rx_uni rx_group tx_dhcp rx_dhcp txs_ok txs_fail ambiguous',
 'DATATYPE':'g dir uni group ipvfour arp ipvsix eapol other',
 'DHCPSTAT':'g dir discover offer request ack nak other'}
SUMMARY=(('DATATYPE',1),('DHCPSTAT',1),('DATATYPE',2),('DHCPSTAT',2))
NUM=r'(0|[1-9][0-9]*)'
PAT={k:re.compile('^NORD2 WIFI '+('DATATXS' if k.startswith('DATATXS') else k)+': '+' '.join(f+'='+NUM for f in v.split())+'$') for k,v in SCHEMAS.items()}
def require(value,why):
 if not value:raise ValueError(why)
def sat(values):return min(U32,sum(values))
def parse(text,require_complete=True):
 require(len(text)<=16*1024*1024,'log size limit')
 streams={};pending=None;summarizing=None;lines=0
 for raw in text.splitlines(keepends=True):
  line=raw.rstrip('\r\n')
  if not re.search(r'NORD2 WIFI (?:DATA|DHCPSTAT)',line):continue
  require(raw.endswith('\n'),'unterminated diagnostic row')
  lines+=1;key=None;x=None
  for k,p in PAT.items():
   m=p.fullmatch(line)
   if m:key=k;x=dict(zip(SCHEMAS[k].split(),map(int,m.groups())));break
  require(x is not None,'unknown/malformed diagnostic schema')
  require(all(v<=(U64 if k=='g' else U32) for k,v in x.items()),'integer overflow')
  if key=='DATATXS1':require(x['part']==1,'wrong TXS first part')
  if key=='DATATXS2':
   require(pending is not None and x['part']==2 and x['n']==pending[1]['n'],'missing/mismatched TXS first part')
   g,first=pending;s=streams[g];require(summarizing is None,'TXS during summary')
   require(x['seq']<=65535 and x['rate']<=65535 and all(x[k]<=255 for k in ('tx_count','flags','tid','flush')) and x['adv'] in (0,1),'TXS field bound')
   require(x['adv']==(x['flags']&1),'advanced flag mismatch')
   require(x['adv'] or not any(x[k] for k in ('seq','tx_count','rate','tid','flush')),'inactive advanced fields')
   s['txs'].append({**first,**{k:v for k,v in x.items() if k not in ('n','part')}});pending=None
   continue
  require(pending is None,'interleaved TXS parts')
  g=x['g'];require(g>0,'zero generation')
  require(g in streams or len(streams)<64,'generation resource limit')
  s=streams.setdefault(g,{'generation':g,'last':0,'headers':[],'txs':[],'summary':None,'types':{},'dhcp':{},'budgets':[0,0,0,0]})
  if key in ('DATAHDR','DATATXS1'):
   require(summarizing is None and s['summary'] is None,'record after/during summary')
   require(x['n']==s['last']+1 and x['n']<=56,'record sequence/bound');s['last']=x['n']
   if key=='DATAHDR':
    require(x['dir'] in (1,2,3) and x['dst']<=2 and x['eth']<=65535 and 14<=x['bytes']<=1518 and x['dhcp']<=8 and x['parse']<=15,'header field bound')
    require((x['parse']==1)==bool(x['dhcp']),'DHCP parse correlation')
    require(x['eth']==2048 or not(x['dhcp'] or x['parse']),'non-IPv4 DHCP parsing')
    require(not x['dhcp'] or x['bytes']>=285,'short DHCP classification')
    require(x['parse']!=2 or x['bytes']<34,'short-IP size mismatch')
    require(x['parse']<3 or x['bytes']>=34,'parsed-IP size mismatch')
    budget=(2 if x['dir']==3 else 0)+bool(x['dhcp']);s['budgets'][budget]+=1
    require(s['budgets'][budget]<=(12 if x['dhcp'] else 4),'header budget')
    s['headers'].append(x)
   else:
    require(all(x[k]<=255 for k in ('pid','wlan','status')) and x['ambiguous']==int(x['pid']==127) and x['last_kind'] in (0,2,3),'TXS metadata bound/correlation')
    require(x['last_kind'] or x['last_pub']==0,'uninitialized publication context')
    require(len(s['txs'])<24,'TXS pair budget');pending=(g,x)
   continue
  if key=='DATACOUNT':
   require(summarizing is None and s['summary'] is None,'duplicate/interleaved summary');s['summary']=x;summarizing=(g,0)
  else:
   require(summarizing is not None and summarizing[0]==g,'missing/wrong-generation summary')
   i=summarizing[1];require((key,x['dir'])==SUMMARY[i],'summary order/duplicate part')
   s['types' if key=='DATATYPE' else 'dhcp'][x['dir']]=x
   summarizing=(g,i+1) if i<3 else None
 complete=bool(streams) and pending is None and summarizing is None
 for s in streams.values():
  c=s['summary'];covered=c is not None and len(s['types'])==2 and len(s['dhcp'])==2
  if covered:
   tx,rx=s['types'][1],s['types'][2]
   require(rx['uni']==c['rx_uni'] and rx['group']==c['rx_group'],'RX summary mismatch')
   for d,t,total,dhcp in ((1,tx,sat([c['tx_sec'],c['tx_data']]),c['tx_dhcp']),(2,rx,sat([c['rx_uni'],c['rx_group']]),c['rx_dhcp'])):
    require(sat([t['uni'],t['group']])==total==sat(t[k] for k in ('ipvfour','arp','ipvsix','eapol','other')),'type partition mismatch')
    require(dhcp<=t['ipvfour'] and sat(s['dhcp'][d][k] for k in ('discover','offer','request','ack','nak','other'))==dhcp,'DHCP summary mismatch')
   for direction,kind in ((1,'tx'),(2,'rx')):
    observed_headers=[h for h in s['headers'] if (h['dir']!=3)==(direction==1)]
    t=s['types'][direction];d=s['dhcp'][direction]
    require(sum(h['dst']==0 for h in observed_headers)<=t['uni'] and sum(h['dst']!=0 for h in observed_headers)<=t['group'],'destination observations exceed summary')
    require(sum(h['eth'] not in (2048,2054,34525,34958) for h in observed_headers)<=t['other'],'other EtherType observations exceed summary')
    for ether,name in ((2048,'ipvfour'),(2054,'arp'),(34525,'ipvsix'),(34958,'eapol')):
     require(sum(h['eth']==ether for h in observed_headers)<=t[name],'EtherType observations exceed summary')
    for typ,name in ((1,'discover'),(2,'offer'),(3,'request'),(5,'ack'),(6,'nak')):
     require(sum(h['dhcp']==typ for h in observed_headers)<=d[name],'DHCP observations exceed summary')
    require(sum(h['dhcp'] in (4,7,8) for h in observed_headers)<=d['other'],'DHCP other mismatch')
   require(c['ambiguous']<=sat([c['txs_ok'],c['txs_fail']]),'ambiguous total mismatch')
   observed=[sum(h['dir']==1 for h in s['headers']),sum(h['dir']==2 for h in s['headers']),sum(h['dir']==3 and h['dst']==0 for h in s['headers']),sum(h['dir']==3 and h['dst']!=0 for h in s['headers'])]
   require(all(a<=c[k] for a,k in zip(observed,('tx_sec','tx_data','rx_uni','rx_group'))),'header observations exceed summary')
   require(sum(t['status']==0 for t in s['txs'])<=c['txs_ok'] and sum(t['status']!=0 for t in s['txs'])<=c['txs_fail'] and sum(t['pid']==127 for t in s['txs'])<=c['ambiguous'],'TXS observations exceed summary')
   # Header reservations are deterministic; saturated subtraction is unknown.
   expected=[]
   for total,dhcp in ((c['tx_sec']+c['tx_data'],c['tx_dhcp']),(c['rx_uni']+c['rx_group'],c['rx_dhcp'])):
    expected.extend([min(4,total-dhcp) if total<U32 and dhcp<U32 else None,min(12,dhcp)])
   covered=all(e is None or got==e for got,e in zip(s['budgets'],expected)) and len(s['txs'])>=min(8,c['txs_ok']+c['txs_fail'])
  s['summary_complete']=c is not None and len(s['types'])==2 and len(s['dhcp'])==2
  s['observation_complete']=covered
  complete=complete and covered
  for k in ('last','budgets'):s.pop(k)
 require(not require_complete or complete,'incomplete diagnostic capture/parts/summary')
 return {'valid':True,'complete':complete,'attempts':list(streams.values()),'observed_lines':lines,'pid127_ambiguous':True,'last_publication_is_correlation':False,'rx_is_bsd_delivery':False,'dhcp_is_lease_proof':False,'internet_proven':False,'air_delivery_proven':False,'txs_authorization_bucket_available':False,'capture_integrity_proven':False}
def main():
 a=argparse.ArgumentParser(description=__doc__);a.add_argument('log');a.add_argument('--allow-prefix',action='store_true');v=a.parse_args()
 try:
  with open(v.log) as f:text=f.read(16*1024*1024+1)
  print(json.dumps(parse(text,not v.allow_prefix),indent=2))
 except (OSError,ValueError) as e:print(str(e),file=sys.stderr);return 1
 return 0
if __name__=='__main__':sys.exit(main())
