#!/usr/bin/env python3
"""Strict completed client timing pairs; no claim from absent/sampled-out calls."""
import argparse,json,re
from pathlib import Path
MAX_INPUT=16*1024*1024
U64=(1<<64)-1;U32=(1<<32)-1
RELEVANT={1,2,4,5,6,8,9,10,12,13,14,15,16,17,18,19,27,43,51,57,96,97,103,106,115,207}
BEGIN=re.compile(r'NORD2 WIFI IOS QUERYBEGIN: client=([0-9]+) call=([0-9]+) selector=([0-9]+) entered_ms=([0-9]+) locked_ms=([0-9]+)')
END=re.compile(r'NORD2 WIFI IOS QUERYEND: client=([0-9]+) call=([0-9]+) request=([0-9]+) status=(0x[0-9a-fA-F]+|0) done_ms=([0-9]+) wait_ms=([0-9]+) work_ms=([0-9]+) clock_ok=([0-9]+) sample=([0-9]+)')
def parse(text):
    issues=[];begins={};ends={};pairs=[];budgets={}
    def bad(line,why):issues.append({'line':line,'reason':why})
    if len(text.encode('utf-8'))>MAX_INPUT:return {'valid':False,'issues':[{'line':0,'reason':'input exceeds bounded limit'}],'pairs':[],'absence_proves_no_calls':False}
    for number,line in enumerate(text.splitlines(),1):
        if 'NORD2 WIFI IOS QUERY' not in line:continue
        # Exact full rows only: no unknown fields or untrusted echo in errors.
        m=BEGIN.fullmatch(line);e=END.fullmatch(line)
        if not(m or e):bad(number,'unknown or incomplete timing row');continue
        try:
            values=[int(v,16)if i==3 and e else int(v)for i,v in enumerate((m or e).groups())]
        except ValueError:bad(number,'invalid numeric field');continue
        client,call=values[:2];key=(client,call)
        if not 0<client<=U64 or not 0<call<=U32:bad(number,'invalid timing identity');continue
        if m:
            _,_,selector,entered,locked=values
            if selector not in (0,6) or entered>U64 or locked>U64:bad(number,'begin field outside source domain');continue
            if key in begins or key in ends:bad(number,'duplicate or reused timing identity');continue
            begins[key]=(number,selector,entered,locked)
        else:
            _,_,request,status,done,wait,work,ok,sample=values
            if request>U32 or status>U32 or any(x>U64 for x in (done,wait,work)) or ok not in (0,1) or sample not in (1,2):bad(number,'end field outside source domain');continue
            if key not in begins or key in ends:bad(number,'missing begin or duplicate end');continue
            ends[key]=number;first,selector,entered,locked=begins[key]
            monotonic=entered<=locked<=done
            if ok!=int(monotonic):bad(number,'clock validity contradicts timestamps');continue
            if (ok and(wait!=locked-entered or work!=done-locked))or(not ok and(wait or work)):bad(number,'duration contradicts timestamps');continue
            slow=bool(ok and(wait>=50 or work>=50))
            if (sample==2)!=slow:bad(number,'sample class contradicts slow threshold');continue
            if sample==1 and status==0 and request not in RELEVANT:bad(number,'successful request excluded by source normal policy');continue
            used=budgets.setdefault(client,{1:0,2:0});used[sample]+=1
            if used[sample]>(64 if sample==1 else 16):bad(number,'per-client sample budget exceeded');continue
            pairs.append({'client':client,'call':call,'selector':selector,'request':request,'status_bits':status,'entered_ms':entered,'locked_ms':locked,'done_ms':done,'wait_ms':wait,'work_ms':work,'clock_ok':ok,'sample':sample})
    for key,(line,*_)in begins.items():
        if key not in ends:bad(line,'missing completed timing end')
    return {'valid':not issues,'issues':issues,'pairs':pairs if not issues else [],'observed_clients':[{'client':k,'normal_pairs':v[1],'slow_pairs':v[2],'normal_limit_observed':v[1]==64,'slow_limit_observed':v[2]==16}for k,v in sorted(budgets.items())]if not issues else [],'absence_proves_no_calls':False,'limits':['Completed sampled selector0/6 calls only.','No absence/freshness/UI-cause/connection/IP proof.','Raw32-bit status is not converted to errno.']}
def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('log',type=Path);ap.add_argument('--output',type=Path);a=ap.parse_args()
    with a.log.open('rb')as f:data=f.read(MAX_INPUT+1)
    try:result=parse(data.decode('utf-8'))
    except UnicodeDecodeError:result={'valid':False,'issues':[{'line':0,'reason':'input is not UTF8'}],'pairs':[],'absence_proves_no_calls':False}
    out=json.dumps(result,indent=2)+'\n'
    if a.output:a.output.write_text(out)
    else:print(out,end='')
    return 0 if result['valid']else 1
if __name__=='__main__':raise SystemExit(main())
