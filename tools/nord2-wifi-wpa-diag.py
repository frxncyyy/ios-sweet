#!/usr/bin/env python3
"""Strict numeric diagnostic reader. No handshake/peer/key/IP success proof."""
import re,json,sys
KEYS='n g t bucket state bytes version type declared info class result failure'.split()
GKEYS='n g t state present bytes reason'.split()
GPAT=re.compile(r'^NORD2 WIFI WPAGATE: '+r' '.join(k+r'=([0-9]+)' for k in GKEYS)+r'$')
PAT=re.compile(r'^NORD2 WIFI WPAHDR: '+r' '.join(k+r'=([0-9]+)' for k in KEYS)+r'$')
def parse(text):
 rows=[];gates=[];counts=[0,0,0];last=0
 for line in text.splitlines():
  if 'NORD2 WIFI WPAGATE:' in line:
   m=GPAT.fullmatch(line)
   if not m:raise ValueError('malformed gate row')
   x=dict(zip(GKEYS,map(int,m.groups())))
   if gates or x['n']!=1 or not 0<x['g']<=2**64-1 or max(x['t'],x['bytes'])>2**64-1 or x['state']>5 or x['present']>1:raise ValueError('gate bound/budget')
   expected=1 if not x['present'] else 2 if not x['bytes'] else 3 if x['bytes']>640 else 0
   if not expected or x['reason']!=expected:raise ValueError('gate reason')
   gates.append(x);continue
  if 'NORD2 WIFI WPAHDR:' not in line:continue
  m=PAT.fullmatch(line)
  if not m:raise ValueError('malformed diagnostic row')
  x=dict(zip(KEYS,map(int,m.groups())))
  if x['n']!=last+1 or not 1<=x['n']<=40 or max(x['g'],x['t'])>2**64-1:raise ValueError('sequence/width')
  if x['bucket']>2 or x['state']>5 or x['bytes']>640 or x['version']>255 or x['type']>255 or max(x['declared'],x['info'])>65535 or x['class'] not in (0,1,3,5) or x['result']>5 or x['failure']>14:raise ValueError('field bound')
  bucket=2 if x['result']==0 else 1 if x['state']==5 else 0
  if x['bucket']!=bucket or bool(x['failure'])!=(x['result']==0):raise ValueError('result correlation')
  if x['bytes']<4 and any(x[k] for k in ('version','type','declared')):raise ValueError('short header')
  expected=0
  if x['bytes']>=7 and x['type']==3:expected=1 if x['info']==138 else 3 if x['info']==5066 else 5 if not x['info']&8 else 0
  elif x['info']:raise ValueError('missing keyinfo')
  if x['class']!=expected:raise ValueError('public classification')
  counts[bucket]+=1
  if counts[bucket]>(24 if bucket==1 else 8):raise ValueError('budget')
  rows.append(x);last=x['n']
 return {'records':rows,'preengine_gate_records':gates,'crypto_success_inferred':False,'key_ready_inferred':False,'ip_inferred':False}
if __name__=='__main__':
 try:print(json.dumps(parse(open(sys.argv[1]).read())))
 except (ValueError,OSError) as e:print(str(e),file=sys.stderr);sys.exit(1)
