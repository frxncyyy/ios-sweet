#!/usr/bin/env python3
"""Supplemental fixed-RXD error observations; no payload/admission/air/IP proof."""
import argparse,json,re
from pathlib import Path
FIELDS=('g','n','d1','d2','d4','wlan','sec','key','ht','groups','type','a1','dst','tid')
PAT=re.compile(r'NORD2 WIFI RXERROR: '+r' '.join(k+r'=(0|[1-9][0-9]*)' for k in FIELDS))
def parse(text):
 if len(text.encode())>16*1024*1024:raise ValueError('input bound')
 rows=[];last={}
 for line in text.splitlines(keepends=True):
  if not line.lstrip().startswith('NORD2 WIFI RXERROR'):continue
  if not line.endswith('\n') or len(line.encode())>255:raise ValueError('row extent')
  body=line[:-1];body=body[:-1] if body.endswith('\r') else body
  m=PAT.fullmatch(body)
  if not m:raise ValueError('schema')
  r=dict(zip(FIELDS,map(int,m.groups())))
  if not 0<r['g']<2**64 or not 1<=r['n']<=8:raise ValueError('identity/budget')
  if any(r[k]>2**32-1 for k in FIELDS if k!='g'):raise ValueError('uint32')
  if r['d1']&~0x0f800000 or r['d2']&~0x0b800000 or r['d4']&~0x1803 or not(r['d1'] or r['d2'] or r['d4']):raise ValueError('error masks')
  if any(r[k]>v for k,v in {'wlan':1023,'sec':31,'key':3,'ht':1,'groups':31,'a1':3,'dst':2,'tid':15}.items()) or r['type'] not in(2,7):raise ValueError('metadata')
  if r['dst']!=(0 if r['a1']==0 else 1 if r['a1']==1 else 2):raise ValueError('A1 correlation')
  if r['g'] not in last and len(last)>=64:raise ValueError('generation bound')
  if r['n']<=last.get(r['g'],0):raise ValueError('duplicate/order')
  last[r['g']]=r['n'];rows.append(r)
 return {'valid':True,'records':rows,'observed_records':len(rows),'budget_reached_generations':[g for g,n in last.items() if n==8],'absence_proves_zero_errors':False,'destination_is_hardware_metadata_only':True,'payload_inspected':False,'authenticated_destination_proven':False,'rx_is_bsd_delivery':False,'air_delivery_proven':False,'dhcp_lease_proven':False,'pid_attribution_proven':False}
def main():
 a=argparse.ArgumentParser();a.add_argument('log',type=Path);v=a.parse_args()
 try:print(json.dumps(parse(v.log.read_text()),indent=2))
 except(ValueError,UnicodeError) as e:print(json.dumps({'valid':False,'error':str(e)}));raise SystemExit(1)
if __name__=='__main__':main()
