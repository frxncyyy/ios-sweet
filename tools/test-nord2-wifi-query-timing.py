#!/usr/bin/env python3
"""Finite pure timing protocol tests: no user data or hardware."""
import argparse,importlib.util,json,sys
sys.dont_write_bytecode=True
from pathlib import Path
ap=argparse.ArgumentParser();ap.add_argument('--parser',type=Path,default=Path(__file__).with_name('nord2-wifi-query-timing.py'));ap.add_argument('--output',type=Path);a=ap.parse_args()
spec=importlib.util.spec_from_file_location('timing',a.parser);p=importlib.util.module_from_spec(spec);spec.loader.exec_module(p)
counts={'positive':0,'negative':0}
def row(client=1,call=1,selector=0,request=43,status='0',entered=100,locked=100,done=101,wait=0,work=1,ok=1,sample=1):
 return f'NORD2 WIFI IOS QUERYBEGIN: client={client} call={call} selector={selector} entered_ms={entered} locked_ms={locked}\nNORD2 WIFI IOS QUERYEND: client={client} call={call} request={request} status={status} done_ms={done} wait_ms={wait} work_ms={work} clock_ok={ok} sample={sample}\n'
def check(text,valid):
 r=p.parse(text);assert r['valid']==valid,(text,r);assert r['absence_proves_no_calls']is False
 if not valid:assert r['pairs']==[]
 counts['positive'if valid else'negative']+=1;return r
base=row();check(base,True);check('',True);check('other source numeric row\n',True)
check(row(selector=6),True);check(row(request=0,status='0xe00002c7'),True)
check(row(client=p.U64,call=p.U32),True);check(row(entered=p.U64,locked=p.U64,done=p.U64,work=0),True)
check(row(entered=100,locked=99,done=101,work=0,ok=0),True);check(row(locked=150,done=150,wait=50,work=0,sample=2),True)
# Pair IDs, not adjacency, carry concurrent source correlations.
b1,e1=base.splitlines();b2,e2=row(call=2).splitlines();check('\n'.join((b1,b2,e2,e1)),True)
check(base+row(client=2),True)
r=check(''.join(row(call=n)for n in range(1,65)),True);assert r['observed_clients'][0]['normal_limit_observed']
check(''.join(row(call=n,locked=150,done=150,wait=50,work=0,sample=2)for n in range(1,17)),True)
for text in (b1,e1,e1+'\n'+b1,base+base,base+'\n'+e1,base.replace('client=1','client=0'),base.replace('call=1','call=0'),base.replace('selector=0','selector=1'),base.replace('status=0','status=0x100000000'),base.replace('client=1',f'client={p.U64+1}'),base.replace('call=1',f'call={p.U32+1}'),base.replace('done_ms=101',f'done_ms={p.U64+1}'),base.replace('work_ms=1','work_ms=2'),base.replace('clock_ok=1','clock_ok=0'),base.replace('sample=1','sample=2'),base.replace('request=43','request=11'),base.replace('sample=1','sample=3'),base.replace('status=0','status=-1'),base.replace('QUERYEND','QUERYBAD'),base.replace('sample=1','sample=1 secret=9'),base.replace('client=1 call=1 request','client=2 call=1 request'),row(locked=150,done=150,wait=50,work=0,sample=1),row(entered=101,locked=100,done=102,work=1,ok=0)):
 check(text,False)
check(''.join(row(call=n)for n in range(1,66)),False);check(''.join(row(call=n,locked=150,done=150,wait=50,work=0,sample=2)for n in range(1,18)),False)
check('x'*(p.MAX_INPUT+1),False)
result={'verdict':'PASS','tests':counts,'no_absence_proof':True};out=json.dumps(result,indent=2)+'\n';print(out,end='')
if a.output:a.output.write_text(out)
