#!/usr/bin/env python3
"""Offline strict connection-log/source-format regressions; no secrets/device IO."""
from pathlib import Path
import argparse,ast,hashlib,importlib.util,json,re,subprocess,sys,tempfile
sys.dont_write_bytecode=True
R=next(p for p in Path(__file__).resolve().parents if(p/'AGENTS.md').exists())
ap=argparse.ArgumentParser();ap.add_argument('--check-log',type=Path,action='append',default=[],help='Additional immutable real logs that must remain INVALID');ap.add_argument('--report',type=Path);args=ap.parse_args()
s=importlib.util.spec_from_file_location('connection_log',R/'tools/nord2-wifi-connection-log.py');m=importlib.util.module_from_spec(s);s.loader.exec_module(m)
positive=negative=0

def row(kind,d):return 'NORD2 WIFI '+kind+': '+' '.join(str(k)+'='+str(v)for k,v in d.items())+'\n'
def station(**kw):
 d={k:0 for k in m.STATION};d.update(generation=1,ticket=1,service=5,transaction=8,action=5,associated=1,t_ms=100);d.update(kw);return d
def ready(**kw):
 d=station(service=6,result=1,linked=1,transaction=15,action=0,outcome=1,associated=1,sta=1,ptk=1,gtk=1,port=1,t_ms=300);d.update(kw);return d
def event(**kw):
 d=dict(event=9,published=1,notified=1,status=0,producer=48,consumer=0,generation=1,ticket=1,epoch=2,t_ms=301);d.update(kw);return d
def notice(**kw):
 d=dict(notice=1,current=1,phase=6,result=1,authorized=1,generation=1,ticket=1,epoch=2,t_ms=302);d.update(kw);return d
def link(**kw):
 d=dict(generation=1,authorized=1,changed=1,error=0);d.update(kw);return d
def complete(r=None,e=None,n=None,l=None):return row('IFNET LINK',link()if l is None else l)+row('IOS EVENT',event()if e is None else e)+row('IOS JOINNOTICE',notice()if n is None else n)+row('IOS STATION',ready()if r is None else r)
def yes(text,claim=None,value=True):
 global positive
 r=m.parse(text.encode()if isinstance(text,str)else text);assert r['valid'],r
 if claim:assert r[claim]is value,r
 for k in ('dhcp_exchange_proven','ip_acquisition_proven','internet_connectivity_proven'):assert r[k]is False
 positive+=1;return r
def no(text):
 global negative
 r=m.parse(text.encode()if isinstance(text,str)else text);assert not r['valid'],r
 for k in ('association_observed','native_authorized_observed','authorized_link_observed','current_authorized_link_observed','join_notification_observed','data_air_tx_success_observed','bsd_input_success_observed','ip_acquisition_proven'):assert not r[k],r
 for attempt in r['attempts']:
  for k in ('association_observed','native_authorized_observed','authorized_link_observed','current_authorized_link_observed','join_notification_observed','data_air_tx_success_observed','bsd_input_success_observed','ifnet_up_observed','join_snapshot_observed'):assert not attempt[k],r
 negative+=1
r=yes('');assert r['status']=='NO_NATIVE_JOIN_RECEIPT'
yes(row('IOS STATION',station()),'association_observed');yes(row('IOS STATION',station()),'native_authorized_observed',False)
r=yes(complete(),'authorized_link_observed');assert r['current_authorized_link_observed']and r['join_notification_observed']and not r['data_air_tx_success_observed']and not r['bsd_input_success_observed']
yes(complete(ready(retired=1)),'current_authorized_link_observed') #cookie retirement alone is not disconnect
r=yes(complete()+row('IOS STATION',ready(tx_accept=3,tx_result=2,tx_ok=1,tx_fail=1,rx_delivered=2,t_ms=400)));assert r['data_air_tx_success_observed']and r['bsd_input_success_observed']
last=station(service=7,result=2,transaction=16,outcome=3,linked=0,associated=1,sta=1,ptk=1,gtk=1,port=0,t_ms=500)
r=yes(complete()+row('IFNET LINK',link(authorized=0))+row('IOS STATION',last));assert r['authorized_link_observed']and not r['current_authorized_link_observed']
yes(complete()+row('IOS STATION',station(service=8,result=9,transaction=21,outcome=4,associated=0)),'current_authorized_link_observed',False)
yes(row('IOS STATION',station(service=8,result=2,transaction=0,action=0,associated=0)),'association_observed',False)
yes(complete()+row('IOS LINK',dict(generation=1,up=1,error=5)),'current_authorized_link_observed',False)
guard=dict(reason=1,source=1,stage=1,error=1,valid=0,addr=0,raw=0,mask=0,expected=0)
r=yes(complete()+row('IOS STATIONGUARD',guard),'current_authorized_link_observed',False);assert r['fatal_native_guard_observed']
yes(complete()+row('IOS STATIONGUARD',guard)+row('IOS STATION',ready()),'current_authorized_link_observed',False)
yes(complete(e=event(notified=0,status=0xe00002bc)),'join_notification_observed',False)
yes(complete(l=link(error=5)),'authorized_link_observed',False)
yes(complete(l=link(error=5)),'join_notification_observed',False)
yes(complete(station()),'join_notification_observed',False)
# An old current0 callback may report stale stored metadata, with zero generation.
yes(complete()+row('IOS JOINNOTICE',notice(current=0,generation=0,epoch=1)),'current_authorized_link_observed')
yes(complete()+row('IOS JOINNOTICE',notice(current=0,epoch=1)),'current_authorized_link_observed')
# Missing trace identity does not manufacture an attribution across clients.
event_unbound={k:event()[k]for k in m.EVENT};r=yes(complete(e=event_unbound),'join_notification_observed',False);assert r['uncorrelated_station_events']==1
for omission in ('IFNET LINK','IOS EVENT','IOS JOINNOTICE','IOS STATION'):
 text=''.join(x for x in complete().splitlines(keepends=True)if not x.startswith('NORD2 WIFI '+omission+':'))
 r=yes(text);assert not(r['authorized_link_observed']and r['join_notification_observed'])
# No PMK-profile admission or accepted enqueue alone becomes association.
req=dict(bytes=592,reason=0,mode=2,auth_lower=1,auth_upper=8,rsn_bytes=22,key_kind=6,key_bytes=32,native_flags=4,assoc_flags=0,assoc_reason=2,cached_pmk_bytes=0,blacklist_count=0,scope_id=0,conditional_option=0,transition_disable=0,band_preference=0,t_ms=50)
yes(row('IOS JOINREQ',req)+row('IOS JOINCONTROL',dict(request=20,bytes=592,status=0,t_ms=51)),'authorized_link_observed',False)
for key in ('mode','auth_lower','auth_upper','rsn_bytes','key_kind','key_bytes','native_flags','assoc_flags','assoc_reason','cached_pmk_bytes','blacklist_count','scope_id','conditional_option','transition_disable','band_preference'):
 q=req.copy();q[key]=99;no(row('IOS JOINREQ',q))
q=req.copy();q.update(reason=5,assoc_flags=2048,band_preference=99);yes(row('IOS JOINREQ',q),'association_observed',False)
for key in m.STATION:
 q=ready();q[key]=m.U64+1 if key in('generation','ticket','t_ms')else m.U32+1;no(complete(q))
for key in ('linked','retired','owner_halt','halt','air_pending','associated','sta','ptk','gtk','port'):
 q=ready();q[key]=2;no(complete(q))
for key in ('associated','sta','ptk','gtk','port'):
 q=ready();q[key]=0;no(complete(q))
for key,value in [('generation',0),('ticket',0),('service',9),('result',4),('transaction',23),('action',15),('outcome',6),('native_phase',10),('event',15),('cid',256),('sequence',256),('error',1),('owner_halt',1),('halt',1)]:
 q=ready();q[key]=value;no(complete(q))
no(complete(ready(tx_result=1)))
no(complete(ready(tx_accept=10)))
yes(complete(ready(tx_accept=9)),'authorized_link_observed')
yes(complete(ready(tx_accept=1,queued_drop=1)),'authorized_link_observed')
# Counter width is source uint32: modulo sum is valid, no fake monotonic constraint.
yes(complete(ready(tx_accept=0,tx_result=m.U32,tx_ok=m.U32-1,tx_fail=1,rx_delivered=m.U32)),'data_air_tx_success_observed')
for key,value in [('epoch',3),('generation',0),('ticket',0),('notified',2),('published',0),('producer',4096),('consumer',4096),('producer',1),('event',99)]:
 q=event();q[key]=value;no(complete(e=q))
no(complete()+row('IOS JOINNOTICE',notice(epoch=3)))
no(complete(n=notice(authorized=0)))
for err in (-2147483649,2147483648):no(complete()+row('IOS LINK',dict(generation=1,up=0,error=err)))
no(complete().replace(' transaction=15',' transaction=-1'))
no(complete().replace(' associated=1',' associated=1 associated=1'))
no(complete().replace('IOS STATION:','IOS STATIONUNKNOWN:'))
no(complete()+'NORD2 WIFI IOS STATION: generation=1')
no(complete()+'NORD2 WIFI IOS STATION: junk=1\n')
no(complete().replace('ticket=1 service=6','service=6 ticket=1'))
no(complete().replace('ticket=1 service=6','ticket=1 service=6 extraneous=1'))
no(complete().replace('NORD2 WIFI IOS STATION:','NORD2 WIFI IOS STATION: NORD2 WIFI IOS STATION:'))
no(b'\xff'+complete().encode());no(b'x'*(m.LIMIT+1))
no(row('IOS STATION',ready())*113)
no(complete()+row('IFNET LINK',link())*32)
# Presence of arbitrary IP/DHCP text never provides structured network proof.
yes(complete()+'DHCP ACK 192.0.2.1 Internet ready\n','ip_acquisition_proven',False)
# Fatal owner logs after synchronous close but before the old service6 snapshot
# is refreshed; linked0/readiness0 cannot count as authorized.
r=yes(complete()+row('IOS STATION',ready(linked=0,port=0))+row('IOS STATIONGUARD',guard),'current_authorized_link_observed',False)
# Last-attempt result1 may remain briefly while synchronous disconnect publishes
# LEAVING and clears actual authorization flags; result alone grants nothing.
yes(complete()+row('IOS JOINNOTICE',notice(notice=3,phase=7,result=1,authorized=0)),'current_authorized_link_observed',False)
yes(row('IOS STATION',dict(last,result=1)),'native_authorized_observed',False)
# One unreported accepted frame after terminal port closure is source-valid.
r=yes(complete()+row('IOS STATION',dict(last,tx_accept=1)));assert not r['current_authorized_link_observed']
# New fields are exact metadata, never extra readiness or credentials.
for n in (22,24,28):
 q=req.copy();q['rsn_bytes']=n;yes(row('IOS JOINREQ',q),'association_observed',False)
for key in ('mode','auth_lower','auth_upper','rsn_bytes'):
 q=req.copy();q[key]=65536;no(row('IOS JOINREQ',q));q[key]=65535;q['reason']=2;yes(row('IOS JOINREQ',q),'association_observed',False)
for k in ('rx_raw','rx_raw_eapol','rx_raw_drop'):
 d=ready();d[k]=1;no(complete(d))
yes(complete(ready(rx_raw=2,rx_raw_eapol=1,rx_raw_drop=1)),'bsd_input_success_observed',False)
yes(complete(ready(rx_raw=0,rx_raw_eapol=m.U32,rx_raw_drop=1)),'bsd_input_success_observed',False)
no(complete().replace(' rx_raw=0',' pmk=1 rx_raw=0'))
no(row('IOS JOINREQ',req).replace('mode=2','mode=2 ssid=1'))
# The old shorter schema must not silently hide missing new diagnostics.
no(row('IOS JOINREQ',{k:v for k,v in req.items()if k not in ('mode','auth_lower','auth_upper','rsn_bytes')}))
no(complete().replace(' rx_raw=0 rx_raw_eapol=0 rx_raw_drop=0',''))
# Source-supported short request groups: identities bind per request, not row
# adjacency. A different client's complete group can interleave legitimately.
def short(trace=1,**kw):
 d=dict(trace=trace,bytes=592,reason=0,mode=2,auth_lower=1,auth_upper=8,rsn_bytes=22,key_kind=6,key_bytes=32,t_ms=50,native_flags=4,assoc_flags=0x4000,assoc_reason=0,cached_pmk_bytes=0,blacklist_count=0,scope_bytes=36,conditional_option=0,private_present=0,owner_valid=1,same_identity=0,transition_disable=0,band_preference=0);d.update(kw)
 return [row(kind,{k:d[k]for k in fields})for kind,fields in m.JOIN_PARTS.items()]
def bundle(trace=1,**kw):return ''.join(short(trace,**kw))
for flags in (0,0x20,0x4000,0x4020,0x800,0x820,0x4800,0x4820):
 for rsn in (22,24,28):
  for reason in range(3):
   text=bundle(assoc_flags=flags,assoc_reason=reason,rsn_bytes=rsn,scope_bytes=36 if flags&0x4000 else 0,private_present=int(bool(flags&0x800)),same_identity=int(bool(flags&0x800)))
   r=yes(text,'authorized_link_observed',False);assert len(r['join_profile_groups'])==1
for scope in (1,36):yes(bundle(scope_bytes=scope),'association_observed',False)
for native in (0,4):yes(bundle(native_flags=native),'association_observed',False)
for trace in (1,m.U64):yes(bundle(trace),'association_observed',False)
yes(bundle(assoc_flags=0,scope_bytes=0,owner_valid=0)) # no identity override requested
for reason in range(1,7):yes(bundle(reason=reason,auth_upper=0,rsn_bytes=0,assoc_flags=0x4800,private_present=1,owner_valid=0,same_identity=0,band_preference=m.U32))
x,y=short(1),short(2)
yes(x[0]+y[0]+'unrelated scheduling text\n'+x[1]+y[1]+y[2]+x[2])
yes(bundle(5)+bundle(2)) # global allocation order need not equal printf scheduling
for remove in range(3):no(''.join(x[:remove]+x[remove+1:])+complete())
for duplicate in range(3):no(''.join(x[:duplicate+1]+[x[duplicate]]+x[duplicate+1:]))
for badtrace in (0,2,m.U64+1):
 for part in range(3):
  rows=x.copy();rows[part]=rows[part].replace('trace=1 ',f'trace={badtrace} ');no(''.join(rows)+complete())
for order in ((1,0,2),(0,2,1),(2,1,0)):no(''.join(x[i]for i in order))
no(bundle()+bundle());no(bundle()+row('IOS JOINREQ',req))
for part in range(3):
 for key in ('pmk','ssid','credential','unknown'):
  rows=x.copy();rows[part]=rows[part].rstrip('\n')+f' {key}=1\n';no(''.join(rows))
 rows=x.copy();rows[part]=rows[part].replace('trace=1 ','trace=1 trace=1 ');no(''.join(rows))
 rows=x.copy();rows[part]=rows[part].rstrip('\n');no(''.join(rows))
 rows=x.copy();rows[part]=rows[part].rstrip('\n')+x[0];no(''.join(rows))
for key,value in [('mode',1),('auth_lower',0),('auth_upper',6),('key_kind',1),('key_bytes',31),('rsn_bytes',26),('bytes',591),('native_flags',1),('assoc_flags',0x100),('assoc_reason',3),('cached_pmk_bytes',1),('blacklist_count',1),('conditional_option',1),('transition_disable',1),('band_preference',1),('scope_bytes',0),('private_present',1),('same_identity',1),('owner_valid',2)]:no(bundle(**{key:value}))
for scope in (0,37,m.U32):no(bundle(scope_bytes=scope))
no(bundle(assoc_flags=0,scope_bytes=1))
for flags in (0x800,0x4800):
 no(bundle(assoc_flags=flags,scope_bytes=36 if flags&0x4000 else 0,private_present=1,owner_valid=0,same_identity=0))
 no(bundle(assoc_flags=flags,scope_bytes=36 if flags&0x4000 else 0,private_present=1,owner_valid=1,same_identity=0))
for key in ('mode','auth_lower','auth_upper','rsn_bytes','assoc_flags'):
 no(bundle(reason=5,**{key:65536}));yes(bundle(reason=5,**{key:65535},**({'private_present':1}if key=='assoc_flags'else{})))
for key in ('assoc_reason','blacklist_count'):
 no(bundle(reason=5,**{key:256}));yes(bundle(reason=5,**{key:255}))
for key in ('private_present','owner_valid','same_identity'):no(bundle(reason=5,**{key:2}))
for owner in (1,m.U64):
 yes(row('IOS JOINOWNER',dict(trace=owner,ready=0,status=0xe00002d8)))
 no(row('IOS JOINOWNER',dict(trace=owner,ready=1,status=0xe00002d8)))
 no(row('IOS JOINOWNER',dict(trace=owner,ready=0,status=0)))
no(row('IOS JOINOWNER',dict(trace=0,ready=0,status=1)))
no(row('IOS JOINOWNER',dict(trace=1,ready=0,status=1))+bundle())
no(bundle()+row('IOS JOINOWNER',dict(trace=1,ready=0,status=1)))
no(row('IOS JOINOWNER',dict(trace=1,ready=0,status=1))*2)
for extent in (0,8):
 yes(row('IOS JOINCONTROL',dict(request=22,bytes=extent,status=0,t_ms=1)))
 for reason in ((0,)if extent==0 else(-2147483648,-1,0,1,2147483647)):
  yes(row('IOS DISASSOC',dict(bytes=extent,reason=reason,status=0)))
for extent in (1,4,7,9,32):no(row('IOS JOINCONTROL',dict(request=22,bytes=extent,status=0,t_ms=1)))
for q in (dict(bytes=0,reason=1,status=0),dict(bytes=4,reason=0,status=1),dict(bytes=8,reason=2147483648,status=0),dict(bytes=8,reason=-2147483649,status=0)):
 no(row('IOS DISASSOC',q))
# Station fragments are a locked snapshot, not five independent readiness flags.
def station_parts(d=None,sample=1):
 d=ready()if d is None else d
 return [row('IOS STATIONPART',dict(sample=sample,part=n+1,**{k:d[k]for k in fields}))for n,fields in enumerate(m.STATION_PARTS)]
def completed_parts(d=None,sample=1):return complete().split('NORD2 WIFI IOS STATION:')[0]+''.join(station_parts(d,sample))
yes(completed_parts(),'authorized_link_observed')
yes(completed_parts()+''.join(station_parts(ready(t_ms=400),112)),'current_authorized_link_observed')
yes(completed_parts()+bundle(),'authorized_link_observed')
z=station_parts()
for remove in range(5):no(''.join(z[:remove]+z[remove+1:]))
for duplicate in range(5):no(''.join(z[:duplicate+1]+[z[duplicate]]+z[duplicate+1:]))
for part in range(5):
 for wrong in (0,2,113,m.U32+1):
  p=z.copy();p[part]=p[part].replace('sample=1 ',f'sample={wrong} ');no(''.join(p))
 p=z.copy();p[part]=p[part].rstrip('\n')+' pmk=1\n';no(''.join(p))
 p=z.copy();p[part]=p[part].rstrip('\n');no(''.join(p))
 p=z.copy();p[part]=p[part].rstrip('\n')+z[0];no(''.join(p))
no(''.join(z[1:]+z[:1]));no(''.join(z)+''.join(z));no(''.join(z)+row('IOS STATION',ready()))
no(''.join(z[:2])+''.join(station_parts(sample=2))+''.join(z[2:]))
# Missing/wrong join correlation poisons all authorization claims, even if the
# independent native rows would otherwise look complete.
for omitted in range(3):no(completed_parts()+''.join(x[:omitted]+x[omitted+1:]))
# Source-literal format strings, compiled with actual C printf at maximal
# declared integer widths, reproduce kernel formatting without private data.
formats=[];source_pins={}
for relative in ('shim/nord2-xnu/wifi_ios_client.cpp','shim/nord2-xnu/wifi_station_service_bind.inc'):
 source=(R/relative).read_text();source_pins[relative]=hashlib.sha256(source.encode()).hexdigest()
 for match in re.finditer(r'printf\(("(?:[^"\\]|\\.)*")',source):
  fmt=ast.literal_eval(match[1])
  if any(fmt.startswith('NORD2 WIFI IOS '+kind+':')for kind in ('STATIONPART','JOINREQ','JOINOPTS','JOINIDENT','JOINOWNER','DISASSOC')):
   formats.append(fmt)
assert len(formats)==10,formats
assert sum('STATIONPART:'in f for f in formats)==5
assert any('JOINREQ: trace=%llu'in f for f in formats)
widths=[]
with tempfile.TemporaryDirectory(prefix='connection-format-')as td:
 d=Path(td);calls=[]
 for fmt in formats:
  specs=re.findall(r'%(?:#)?(?:llu|u|x|d)',fmt)
  argv=[('18446744073709551615ULL'if s=='%llu'else'-2147483647-1'if s=='%d'else'4294967295U')for s in specs]
  calls.append('printf('+json.dumps(fmt)+(','+','.join(argv)if argv else'')+');')
 (d/'format.c').write_text('#include <stdio.h>\nint main(void){'+''.join(calls)+'return 0;}\n')
 subprocess.run(['clang','-std=c11','-Wall','-Wextra','-Werror',str(d/'format.c'),'-o',str(d/'format')],check=True)
 output=subprocess.check_output([str(d/'format')]);lines=output.splitlines(keepends=True);assert len(lines)==10
 for fmt,line in zip(formats,lines):
  assert line.endswith(b'\n')and len(line)<=255,(fmt,len(line));widths.append({'format':fmt,'max_bytes':len(line)})
 # Maximum printf widths are deliberately not valid state booleans/enums.
 no(output)
# Original malformed/interleaved historical logs must stay invalid. This option
# is explicit and does not make portable tests depend on private hardware files.
real_logs=[]
for path in args.check_log:
 data=path.read_bytes();no(data);real_logs.append({'path':str(path),'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest(),'expected':'INVALID'})
report={'status':'PASS_SYNTHETIC_AND_SOURCE_FORMATS','positive':positive,'negative':negative,'actual_printf_formats':widths,'source_sha256':source_pins,'additional_logs':real_logs,'no_network_readiness_or_IP_claim':True}
if args.report:args.report.write_text(json.dumps(report,indent=2)+'\n')
print(json.dumps(report,indent=2))
