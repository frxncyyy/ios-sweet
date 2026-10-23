#!/usr/bin/env python3
"""Strict sampled-PID diagnostic reader. No ownership/authentication/IP decisions."""
from pathlib import Path
import argparse,json,re
SCHEMAS={
 'TXPUB':('g','pid','wlan','kind','protected','ether','dhcp','parse','bytes','class','pub'),
 'TXSTATUS':('g','pid','wlan','status','count','rate','flags','adv','seq','tid','flush'),
 'TXFINAL':('g','pid','wlan','kind','attempt','pub','local','seen','status'),
 'TXNOTE':('g','original','pid','wlan','reason','count'),
 'TXLIMIT':('g','kind','limit','fallback')}
PAT={k:re.compile('NORD2 WIFI '+k+': '+' '.join(f+r'=(0|[1-9][0-9]*)' for f in fields)) for k,fields in SCHEMAS.items()}
def parse(text):
 if len(text.encode())>16*1024*1024:raise ValueError('input bound')
 slots={};notes={};limits={};last_pub={};observations=0
 for raw in text.splitlines(keepends=True):
  kind=next((k for k in SCHEMAS if raw.lstrip().startswith('NORD2 WIFI '+k)),None)
  if kind is None:continue
  if not raw.endswith('\n') or len(raw.encode())>255:raise ValueError('row extent')
  s=raw[:-1];s=s[:-1] if s.endswith('\r') else s;m=PAT[kind].fullmatch(s)
  if not m:raise ValueError('schema')
  r=dict(zip(SCHEMAS[kind],map(int,m.groups())));observations+=1
  if not 0<r['g']<2**64:raise ValueError('generation')
  if any(v>2**64-1 if k=='original' else v>2**32-1 for k,v in r.items() if k!='g'):raise ValueError('integer domain')
  if kind=='TXLIMIT':
   k=r['kind'];expected={1:(31,0),2:(63,127),3:(126,127)}
   if k not in expected or (r['limit'],r['fallback'])!=expected[k] or k in limits:raise ValueError('limit')
   limits[k]=r;continue
  if kind=='TXNOTE':
   reason=r['reason'];pid=r['pid']
   if not 1<=reason<=6 or pid>255 or pid==127 or r['wlan']>255 or not 1<=r['count']<=2 or r['count']<=notes.get(reason,0):raise ValueError('note')
   if (reason==1 and r['original']) or (reason!=1 and not r['original']) or (reason==3 and r['original']==r['g']) or (reason>=4 and r['original']!=r['g']):raise ValueError('note original')
   notes[reason]=r['count'];continue
  pid=r['pid']
  if not 1<=pid<=126 or r['wlan']>254:raise ValueError('PID/WLAN')
  expected_kind=1 if pid<=31 else 2 if pid<=63 else 3
  slot=slots.setdefault(pid,{'g':r['g'],'wlan':r['wlan'],'kind':expected_kind})
  if slot['g']!=r['g'] or slot['wlan']!=r['wlan'] or ('kind' in r and r['kind']!=expected_kind):raise ValueError('boot identity')
  if kind in slot or 'TXFINAL' in slot:raise ValueError('duplicate/closed')
  if kind=='TXPUB':
   if 'TXSTATUS' in slot or r['protected']>1 or r['ether']>65535 or r['dhcp']>8 or r['parse']>15 or not r['pub'] or r['pub']<=last_pub.get(r['g'],0):raise ValueError('publication domain/order')
   if r['kind']==1 and (r['protected'] or r['ether'] or r['dhcp'] or r['parse'] or r['class'] not in(0,11,12) or not 26<=r['bytes']<=1568):raise ValueError('management')
   if r['kind']==2 and (r['ether']!=34958 or r['dhcp'] or r['parse'] or r['class']>4 or not 18<=r['bytes']<=1518):raise ValueError('security')
   if r['kind']==3 and (r['protected']!=1 or r['ether']<1536 or r['ether']==34958 or r['class'] or not 14<=r['bytes']<=1514):raise ValueError('data')
   if bool(r['dhcp'])!=(r['parse']==1) or (r['dhcp'] and r['ether']!=2048):raise ValueError('DHCP classification')
   last_pub[r['g']]=r['pub']
  elif kind=='TXSTATUS':
   if r['status']>255 or r['count']>255 or r['rate']>65535 or r['flags']>255 or r['adv']>1 or r['seq']>65535 or r['tid']>255 or r['flush']>255:raise ValueError('status domain')
   if r['adv'] != (r['flags']&1):raise ValueError('advanced flag')
   if not r['adv'] and any(r[k] for k in ('count','rate','seq','tid','flush')):raise ValueError('inactive advanced fields')
  else:
   if any(r[k]>1 for k in('attempt','local','seen')) or (not r['seen'] and r['status']!=256) or (r['seen'] and r['status']>255):raise ValueError('terminal domain')
   if (r['pub'] and not r['attempt']) or ((r['local'] or r['seen']) and not r['pub']):raise ValueError('terminal ownership observation')
   if 'TXPUB' in slot and r['pub']!=slot['TXPUB']['pub']:raise ValueError('terminal publication')
   if 'TXSTATUS' in slot and (not r['seen'] or r['status']!=slot['TXSTATUS']['status']):raise ValueError('first status overwritten')
  slot[kind]=r
 rows=[]
 for pid,s in sorted(slots.items()):
  pub=s.get('TXPUB');status=s.get('TXSTATUS');final=s.get('TXFINAL')
  rows.append({'pid':pid,**s,'publication_observed':pub is not None,'status_attributed_to_observed_publication':pub is not None and status is not None,'firmware_mac_success_reported':bool(pub and status and status['status']==0),'terminal_observed':final is not None,'publication_state':('CONFIRMED' if pub or (final and final['pub']) else 'UNCERTAIN' if final and final['attempt'] else 'NOT_PUBLISHED' if final else 'NOT_OBSERVED')})
 return {'valid':True,'observed_records':observations,'samples':rows,'notes':notes,'limits':list(limits.values()),'boot_ids_never_reused':True,'fallback127_never_attributed':True,'absence_proves_zero_traffic':False,'missing_status_is_failure':False,'TXS_releases_DMA_or_token':False,'local_retirement_is_air_success':False,'authentication_proven':False,'DHCP_lease_proven':False,'internet_proven':False,'secret_or_identity_fields_exposed':False}
def main():
 a=argparse.ArgumentParser();a.add_argument('log',type=Path);v=a.parse_args()
 try:print(json.dumps(parse(v.log.read_text()),indent=2))
 except(ValueError,UnicodeError) as e:print(json.dumps({'valid':False,'error':str(e)}));raise SystemExit(1)
if __name__=='__main__':main()
