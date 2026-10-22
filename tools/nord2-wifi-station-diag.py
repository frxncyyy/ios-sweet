#!/usr/bin/env python3
"""Numeric diagnostic receipt only. Never authorizes association/key/data readiness."""
import re,json,argparse
from pathlib import Path
FIELDS=['n','g','t','k','a','b','c','d','e','f','h','i']
PAT=re.compile(r'^NORD2 WIFI NSTDIAG: '+r' '.join(x+r'=(\d+)' for x in FIELDS)+r'$')
def parse(text):
 records=[]
 for line in text.splitlines():
  if 'NORD2 WIFI NSTDIAG:' not in line:continue
  m=PAT.fullmatch(line)
  if not m:raise ValueError('malformed diagnostic row')
  d=dict(zip(FIELDS,map(int,m.groups())))
  if any(v>((1<<64)-1 if k in ('g','t') else (1<<32)-1) for k,v in d.items()):raise ValueError('width')
  if not 1<=d['n']<=96 or d['n']!=(records[-1]['n']+1 if records else 1) or not 1<=d['k']<=8:raise ValueError('count/kind')
  if d['k']==4 and (not 1<=d['h']<=7 or any(d[k]>256 for k in ['d','e','f']) or d['i']>255):raise ValueError('txs field')
  if d['k']==5 and (d['b']>255 or d['c']>255 or d['d']>65535 or d['e'] not in (0,1,3) or d['f']>65535 or d['h']>1 or d['i']):raise ValueError('eapol field')
  if d['k']==7 and (d['a']>4 or d['b']>32 or d['h'] or d['i']):raise ValueError('queue type field')
  records.append(d)
 return {'rows':records,'m1_public_class':sum(x['k']==5 and x['e']==1 for x in records),'m3_public_class':sum(x['k']==5 and x['e']==3 for x in records),'txs_match_predicate':sum(x['k']==4 and x['h']==7 for x in records),'native_readiness_proven':False}
if __name__=='__main__':
 a=argparse.ArgumentParser();a.add_argument('log',type=Path);z=a.parse_args();print(json.dumps(parse(z.log.read_text()),indent=2))
