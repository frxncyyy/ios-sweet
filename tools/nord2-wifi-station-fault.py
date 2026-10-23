#!/usr/bin/env python3
"""Strict bounded terminal diagnostics, not a networking or clean-DMA receipt."""
import argparse,json,re
from pathlib import Path
SCHEMAS={1:'source generation native error halt transaction owner in_kind in_ticket',2:'last deadline operation_deadline cleanup_deadline_ms',3:'submit dma busy pub ddone air_pending air_done air_status cleanup associated_scan'}
U64={'generation','in_ticket','last','deadline','operation_deadline','cleanup_deadline_ms'}

def parse(raw):
 issues=[];parts={};order=[]
 if len(raw)>8*1024*1024:return {'valid':False,'complete':False,'issues':['input>8MiB']}
 try:text=raw.decode('utf8')
 except UnicodeDecodeError:return {'valid':False,'complete':False,'issues':['invalid UTF8']}
 for line in text.splitlines(keepends=True):
  pos=line.find('NORD2 WIFI IOS STATIONFAULT')
  if pos<0:continue
  body=line[pos:]
  try:
   if not line.endswith('\n') or len(body.encode())>230 or line.count('NORD2 WIFI ')!=1:raise ValueError()
   m=re.fullmatch(r'NORD2 WIFI IOS STATIONFAULT: part=([123]) (.*)\n',body)
   if not m:raise ValueError()
   part=int(m[1]);tokens=m[2].split(' ');names=SCHEMAS[part].split()
   if part in parts or len(tokens)!=len(names):raise ValueError()
   row={}
   for token,name in zip(tokens,names):
    v=re.fullmatch(name+r'=([0-9]{1,20})',token)
    if not v:raise ValueError()
    n=int(v[1]);bits=64 if name in U64 else 32
    if n>=1<<bits:raise ValueError()
    row[name]=n
   if part==1 and row['source'] not in range(1,7):raise ValueError()
   parts[part]=row;order.append(part)
  except ValueError:issues.append('malformed/duplicate STATIONFAULT')
 if order and order!=[1,2,3]:issues.append('incomplete/out-of-order STATIONFAULT')
 return {'valid':not issues,'complete':bool(not issues and order==[1,2,3]),'observed':bool(order),'parts':parts,'issues':issues,'network_ready':False,'safe_dma_retirement_claim':False,'scope':'One boot terminal failure observation; no observed fault does not prove no failure. last/deadline/operation_deadline are microseconds; cleanup_deadline_ms is milliseconds. Invalid internal counters may be the reason for the failure and remain raw numeric evidence.'}
if __name__=='__main__':
 a=argparse.ArgumentParser();a.add_argument('log',type=Path);x=a.parse_args();r=parse(x.log.read_bytes());print(json.dumps(r,indent=2));raise SystemExit(0 if r['valid'] else 2)
