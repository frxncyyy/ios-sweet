from pathlib import Path
import importlib.util,re,json,argparse
P=Path(__file__).resolve().parent;R=next(p for p in P.parents if(p/'AGENTS.md').exists());s=importlib.util.spec_from_file_location('fault',P/'nord2-wifi-station-fault.py');m=importlib.util.module_from_spec(s);s.loader.exec_module(m)
a=argparse.ArgumentParser();a.add_argument('--actual',type=Path);a.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');a.add_argument('--report',type=Path,required=True);x=a.parse_args();yes=no=0

def check(t,ok,complete=None):
 global yes,no
 r=m.parse(t.encode());assert r['valid']==ok,(r,t)
 if complete is not None:assert r['complete']==complete,r
 assert not r.get('network_ready') and not r.get('safe_dma_retirement_claim')
 if ok:yes+=1
 else:no+=1
 return r
source=(x.candidate/'wifi_station_service_bind.inc').read_text();formats=re.findall(r'printf\("(NORD2 WIFI IOS STATIONFAULT:[^"\n]+)',source);assert len(formats)==3
rows=[];widths=[]
for f in formats:
 f=f.replace('\\n','\n');f=re.sub('%llu',str(2**64-1),f);f=re.sub('%u',str(2**32-1),f);widths.append(len(f));assert len(f)<=230;rows.append(f)
rows[0]=rows[0].replace('source=4294967295','source=6');t=''.join(rows);check(t,True,True);check('',True,False);check('unrelated row\n',True,False)
for l in rows:check(t.replace(l,''),False,False);check(t.replace(l,l+l),False,False)
for k,v in [('source=6','source=0'),('source=6','source=7'),('native=4294967295','native=4294967296'),('in_ticket=18446744073709551615','in_ticket=18446744073709551616'),('ddone=4294967295','ddone=-1')]:check(t.replace(k,v),False,False)
check(rows[1]+rows[0]+rows[2],False,False);check(t.rstrip('\n'),False,False);check(t.replace('part=2 ','part=2 key=1 '),False,False);check(t.replace('STATIONFAULT: part=2','STATIONFAULT: part=4'),False,False)
if x.actual:
 raw=x.actual.read_bytes();r=m.parse(raw);assert r['complete'] and not r['issues'],r;assert r['parts'][1]['source']==5 and r['parts'][1]['error']==3 and r['parts'][3]['busy']==1 and r['parts'][3]['ddone']==0;yes+=1
x.report.write_text(json.dumps({'result':'PASS','positive':yes,'negative':no,'maximum_widths':widths,'actual_source_format':True},indent=2)+'\n');print(yes,no,widths)
