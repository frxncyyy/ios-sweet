#!/usr/bin/env python3
"""Bounded numeric connection receipts; carrier/traffic are never DHCP/IP proof."""
from pathlib import Path
import argparse,json,re
LIMIT=4*1024*1024
MAX_ROWS=8192
U32=(1<<32)-1
U64=(1<<64)-1
STATION='generation ticket service result linked retired transaction action outcome owner_halt native_phase error halt peer_step cid sequence submitted dma air_pending associated sta ptk gtk port event status events tx_accept tx_result tx_ok tx_fail queued_drop rx_delivered rx_raw rx_raw_eapol rx_raw_drop t_ms'.split()
STATION_LOCAL=STATION[:32]+['tx_local']+STATION[32:]
STATION_SECURITY=STATION_LOCAL+['security','rx_raw_data']
STATION_PARTS=[STATION[:7],STATION[7:14],STATION[14:24],STATION[24:32],STATION[32:]]
def expand_station_parts(text):
 """Only a complete, ordered, boot-unique five-part snapshot can be evidence."""
 output=[];issues=[];pending=None;last_sample=0;saw_legacy=False;saw_parts=False
 for line in text.splitlines(keepends=True):
  marker='NORD2 WIFI IOS STATIONPART:'
  if marker not in line:
   saw_legacy|='NORD2 WIFI IOS STATION:' in line
   output.append(line);continue
  saw_parts=True;body=line[line.index(marker):]
  match=re.fullmatch(r'NORD2 WIFI IOS STATIONPART: sample=([0-9]+) part=([1-5]) (.*)\n',body)
  if not match or len(body.encode())>255 or line.count('NORD2 WIFI ')!=1:
   issues.append('invalid station part framing');continue
  sample,part=int(match[1]),int(match[2]);fields=match[3].split(' ');pairs=[];valid=True
  for field in fields:
   m=re.fullmatch(r'([a-z_]+)=([0-9]+)',field)
   if not m:valid=False;break
   key,value=m[1],int(m[2]);maximum=U64 if key in ('generation','ticket','t_ms') else U32
   if value>maximum:valid=False;break
   pairs.append((key,value))
  schemas=[STATION_PARTS[part-1]]
  if part==4:schemas.append(STATION_PARTS[3]+['tx_local'])
  if part==5:schemas.append(STATION_PARTS[4]+['security','rx_raw_data'])
  if not valid or [k for k,v in pairs] not in schemas or not 1<=sample<=112:
   issues.append('invalid station part schema');continue
  if part==1:
   if pending is not None or sample<=last_sample:issues.append('duplicate or incomplete station sample')
   pending=[sample,1,pairs];last_sample=max(last_sample,sample)
  elif pending is None or pending[0]!=sample or pending[1]+1!=part:
   issues.append('unbound or reordered station part');continue
  else:pending[1]=part;pending[2].extend(pairs)
  if part==5 and pending is not None:
   output.append('NORD2 WIFI IOS STATION: '+' '.join(k+'='+str(v) for k,v in pending[2])+'\n');pending=None
 if pending is not None:issues.append('incomplete station sample at end')
 if saw_legacy and saw_parts:issues.append('mixed station trace formats in one boot')
 return ''.join(output),issues
JOINREQ='bytes reason mode auth_lower auth_upper rsn_bytes key_kind key_bytes native_flags assoc_flags assoc_reason cached_pmk_bytes blacklist_count scope_id conditional_option transition_disable band_preference t_ms'.split()
JOIN_PARTS={
 'IOS JOINREQ':'trace bytes reason mode auth_lower auth_upper rsn_bytes key_kind key_bytes t_ms'.split(),
 'IOS JOINOPTS':'trace native_flags assoc_flags assoc_reason cached_pmk_bytes blacklist_count scope_bytes conditional_option'.split(),
 'IOS JOINIDENT':'trace private_present owner_valid same_identity transition_disable band_preference'.split(),
}
NOTICE='notice current phase result authorized generation ticket epoch t_ms'.split()
EVENT='event published notified status producer consumer t_ms'.split()
# Scan EVENT retains its old shape; station EVENT carries actual callback identity.
EVENT_BOUND='event published notified status producer consumer generation ticket epoch t_ms'.split()
SCHEMAS={
 'IOS STATION':[STATION,STATION_LOCAL,STATION_SECURITY],
 'IOS STATIONGUARD':['reason source stage error valid addr raw mask expected'.split()],
 'IOS LINK':['generation up error'.split()],
 'IFNET LINK':['generation authorized changed error'.split()],
 'IOS JOINREQ':[JOINREQ,JOIN_PARTS['IOS JOINREQ']],
 'IOS JOINOPTS':[JOIN_PARTS['IOS JOINOPTS']],
 'IOS JOINIDENT':[JOIN_PARTS['IOS JOINIDENT']],
 'IOS JOINOWNER':['trace ready status'.split()],
 'IOS DISASSOC':['bytes reason status'.split()],
 'IOS JOINCONTROL':['request bytes status t_ms'.split()],
 'IOS JOINNOTICE':[NOTICE],
 'IOS EVENT':[EVENT,EVENT_BOUND],
}
SIGNED={('IOS LINK','error'),('IFNET LINK','error'),('IOS DISASSOC','reason')}
def authorized(row):
 # Legacy receipts describe WPA2 only. OPEN must explicitly identify itself;
 # absence of keys never silently downgrades an incomplete WPA2 handshake.
 security=row.get('security',1)
 keys=(row['ptk']==1 and row['gtk']==1) if security==1 else (row['ptk']==0 and row['gtk']==0) if security==2 else False
 return bool(keys and row['service']==6 and row['result']==1 and row['linked']==1 and row['transaction']==15 and row['outcome']==1 and not(row['owner_halt']or row['halt']or row['error'])and all(row[k]==1 for k in ('associated','sta','port')))
def rekey_transition(row):
 # STC_GTK/REKEY_GTK and STC_EAPOL_TX/TX_GROUP preserve an already opened
 # controlled port. They are not a new handshake or proof of a new GTK ACK.
 return bool('tx_local' in row and row.get('security',1)==1 and
             (row['transaction'],row['action']) in ((13,16),(10,15)) and
             authorized(dict(row,transaction=15)))
PREFIXES=('NORD2 WIFI IOS STATION','NORD2 WIFI IOS LINK','NORD2 WIFI IOS JOIN','NORD2 WIFI IOS DISASSOC','NORD2 WIFI IFNET','NORD2 WIFI IOS EVENT')
def parse(data):
 issues=[];rows=[];counts={}
 def bad(message):
  if len(issues)<64:issues.append(message)
 def result_empty(message):
  return dict(valid=False,issues=[message],status='INVALID',association_observed=False,native_authorized_observed=False,authorized_link_observed=False,current_authorized_link_observed=False,join_notification_observed=False,data_air_tx_success_observed=False,data_local_submission_observed=False,bsd_input_success_observed=False,dhcp_exchange_proven=False,ip_acquisition_proven=False,internet_connectivity_proven=False,fatal_native_guard_observed=False,attempts=[])
 if not isinstance(data,bytes)or len(data)>LIMIT:return result_empty('invalid or oversized log')
 try:text=data.decode('utf8')
 except UnicodeDecodeError:return result_empty('non-UTF8 log')
 text,part_issues=expand_station_parts(text)
 for issue in part_issues:bad(issue)
 for index,line in enumerate(text.splitlines(keepends=True)):
  start=line.find('NORD2 WIFI ')
  if start<0 or not line[start:].startswith(PREFIXES):continue
  body=line[start:].rstrip('\r\n')
  if not line.endswith('\n')or len(body)>1024 or line.count('NORD2 WIFI ')!=1:
   bad('incomplete/interleaved/oversized connection row');continue
  m=re.fullmatch(r'NORD2 WIFI ([A-Z ]+): (.*)',body)
  if not m or m[1]not in SCHEMAS:bad('unknown connection row');continue
  kind,payload=m.groups();parts=payload.split(' ');pairs=[];numeric={};okay=True
  for part in parts:
   m=re.fullmatch(r'([a-z_]+)=(-?(?:0x[0-9a-fA-F]+|[0-9]+))',part)
   if not m:okay=False;break
   key,value=m.groups()
   if key in numeric:okay=False;break
   try:v=int(value,16 if 'x'in value else 10)
   except ValueError:okay=False;break
   if value.startswith('-')and(kind,key)not in SIGNED:okay=False;break
   maximum=(1<<31)-1 if(kind,key)in SIGNED else U64 if key in ('generation','ticket','epoch','trace','t_ms')else U32
   minimum=-(1<<31)if(kind,key)in SIGNED else 0
   if v<minimum or v>maximum:okay=False;break
   pairs.append(key);numeric[key]=v
  if not okay or pairs not in SCHEMAS[kind]:bad('malformed '+kind+' schema/width');continue
  if (kind in ('IOS JOINOPTS','IOS JOINIDENT','IOS JOINOWNER','IOS DISASSOC')or kind=='IOS JOINREQ'and 'trace'in numeric)and len(body.encode())+1>255:
   bad('oversized short diagnostic row');continue
  if len(rows)>=MAX_ROWS:bad('connection row budget exceeded');break
  numeric.update(_kind=kind,_line=index);rows.append(numeric);counts[kind]=counts.get(kind,0)+1
 if sum(counts.get(k,0)for k in ('IOS STATION','IOS STATIONGUARD','IOS LINK'))>112:bad('native trace budget exceeded')
 if counts.get('IFNET LINK',0)>32:bad('ifnet trace budget exceeded')
 attempts={};notices=[];events=[];links=[];join_groups={};join_owner=set();legacy_join=False;station_formats=set();security_by_attempt={};previous_authorized={}
 for row in rows:
  kind=row['_kind']
  if kind=='IOS STATION':
   local_format='tx_local' in row;station_formats.add((local_format,'security' in row))
   if len(station_formats)>1:bad('mixed station counter formats in one boot')
   if local_format and any(row[k]for k in ('tx_result','tx_ok','tx_fail')):bad('local data producer cannot claim air results')
   if row['event'] in (15,16) and row['status'] not in (0,1):bad('local submission status domain')
   if not row['generation']or not row['ticket']or row['service']>8 or row['result']not in (0,1,2,9,21)or row['transaction']>22 or row['action']>(16 if local_format else 14) or row['outcome']>5 or row['native_phase']>(15 if local_format else 9) or row['event']>(16 if local_format else 14) or row['cid']>255 or row['sequence']>255:bad('station enum/identity domain')
   if any(row[k]>1 for k in ('linked','retired','owner_halt','halt','air_pending','associated','sta','ptk','gtk','port')):bad('station boolean domain')
   if 'security' in row:
    security=row['security'];identity=(row['generation'],row['ticket'])
    if security not in (0,1,2):bad('station security domain')
    if security==2 and (row['ptk']or row['gtk']):bad('OPEN station cannot report installed WPA keys')
    if identity in security_by_attempt and security_by_attempt[identity]!=security:bad('station security changed within attempt')
    security_by_attempt[identity]=security
   if row['rx_raw']!=((row['rx_raw_eapol']+row['rx_raw_drop']+row.get('rx_raw_data',0))&U32):bad('raw RX counters disagree')
   if row['tx_result']!=((row['tx_ok']+row['tx_fail'])&U32):bad('data result counters disagree')
   # Bounded residual, not exact outstanding: port closure can discard an
   # already-dequeued unpublished frame without a FRAME_RESULT.
   if ((row['tx_accept']-row['tx_result']-row.get('tx_local',0)-row['queued_drop'])&U32)>9:bad('data queue/residual bound')
   # Fatal closure logs before service snapshot is refreshed: service6/linked0
   # is legitimate transitional metadata, and never an authorization proof.
   key=(row['generation'],row['ticket'])
   continued_rekey=rekey_transition(row) and key in previous_authorized and row['t_ms']>=previous_authorized[key]
   if row['service']==6 and row['linked']and not(authorized(row)or continued_rekey):bad('contradictory authorized station snapshot')
   if authorized(row):previous_authorized[key]=row['t_ms']
   elif not continued_rekey:previous_authorized.pop(key,None)
   attempts.setdefault(key,dict(rows=[],notices=[],events=[],links=[]))['rows'].append(row)
  elif kind=='IOS JOINNOTICE':
   if row['notice']not in (1,2,3,4)or row['current']>1 or row['authorized']>1 or row['phase']>8 or row['result']not in (0,1,2,9,21)or not row['ticket']or not row['epoch']:bad('join notice domain')
   if row['current']and row['notice']!=4 and not row['generation']:bad('current join notice lacks native generation')
   if row['current']and(row['authorized']or row['phase']==6)and not(row['authorized']==1 and row['phase']==6 and row['result']==1):bad('contradictory authorized join notice')
   notices.append(row)
  elif kind=='IOS EVENT':
   if row['event']not in (1,2,3,4,9,10)or row['published']!=1 or row['notified']>1 or row['producer']>=4096 or row['consumer']>=4096 or row['producer']%4 or row['consumer']%4 or row['notified']!=int(row['status']==0):bad('event domain')
   if 'generation'in row and(row['event']in(1,10)or not row['generation']or not row['ticket']or not row['epoch']):bad('bound station event identity')
   events.append(row)
  elif kind=='IFNET LINK':
   if not row['generation']or row['authorized']>1 or row['changed']>1:bad('ifnet link domain')
   links.append(row)
  elif kind=='IOS LINK':
   if not row['generation']or row['up']>1:bad('native link error domain')
  elif kind=='IOS JOINCONTROL':
   if row['request']not in (20,22)or(row['status']==0 and row['bytes']not in ((592,) if row['request']==20 else (0,8))):bad('successful join control shape')
  elif kind=='IOS DISASSOC':
   if row['bytes']not in (0,8)or(row['bytes']==0 and row['reason']!=0):bad('disassociation diagnostic domain')
  elif kind=='IOS JOINOWNER':
   trace=row['trace']
   if not trace or row['ready']!=0 or row['status']==0:bad('join owner unavailable domain')
   if trace in join_owner or trace in join_groups:bad('reused join trace identity')
   join_owner.add(trace)
  elif kind in JOIN_PARTS and 'trace'in row:
   trace=row['trace'];order=list(JOIN_PARTS);position=order.index(kind)
   if not trace or trace in join_owner:bad('invalid or reused join trace identity')
   group=join_groups.setdefault(trace,[])
   if len(group)!=position:bad('duplicate, missing or reordered join fragment')
   group.append(row)
  elif kind=='IOS JOINREQ':
   legacy_join=True
   if row['reason']>6 or any(row[k]>65535 for k in ('mode','auth_lower','auth_upper','rsn_bytes')):bad('join refusal/producer uint16 domain')
   if row['reason']==0 and not(row['bytes']==592 and row['mode']==2 and row['auth_lower']==1 and row['auth_upper']==8 and row['rsn_bytes']in(22,24,28) and row['key_kind']==6 and row['key_bytes']==32 and row['native_flags']in(0,4)and row['assoc_reason']<=2 and row['assoc_flags']in(0,32)and not any(row[k]for k in ('cached_pmk_bytes','blacklist_count','scope_id','conditional_option','transition_disable','band_preference'))):bad('contradictory admitted join profile')
  elif kind=='IOS STATIONGUARD':
   if row['valid']>1:bad('guard validity domain')
 if legacy_join and(join_groups or join_owner):bad('mixed join trace formats in one boot')
 join_profiles=[]
 for trace,group in join_groups.items():
  if len(group)!=3 or [x['_kind']for x in group]!=list(JOIN_PARTS):bad('incomplete join trace group');continue
  q={k:v for part in group for k,v in part.items()if not k.startswith('_')}
  if q['reason']>6 or any(q[k]>65535 for k in ('mode','auth_lower','auth_upper','rsn_bytes','assoc_flags'))or q['assoc_reason']>255 or q['blacklist_count']>255:bad('join diagnostic producer width/domain')
  if any(q[k]>1 for k in ('private_present','owner_valid','same_identity'))or q['same_identity']and not(q['private_present']and q['owner_valid']):bad('join identity diagnostic domain')
  if q['bytes']==592 and q['private_present']!=int(bool(q['assoc_flags']&0x800)):bad('join private-address flag correlation')
  if q['reason']==0:
   wpa2=q['auth_upper']==8 and q['rsn_bytes']in(22,24,28)and q['key_kind']==6 and q['key_bytes']==32
   opened=q['auth_upper']==0 and q['rsn_bytes']==0 and q['key_kind']==0 and q['key_bytes']==0 and q['native_flags']==0
   profile=(q['bytes']==592 and q['mode']==2 and q['auth_lower']==1 and (wpa2 or opened) and q['native_flags']in(0,4)and q['assoc_reason']<=2 and not(q['assoc_flags']&~0x4820)and not any(q[k]for k in ('cached_pmk_bytes','blacklist_count','conditional_option','transition_disable','band_preference')))
   scope=1<=q['scope_bytes']<=36 if q['assoc_flags']&0x4000 else q['scope_bytes']==0
   identity=not q['private_present']or(q['owner_valid']==1 and q['same_identity']==1)
   if not(profile and scope and identity):bad('contradictory admitted join profile')
  join_profiles.append(q)
 # Only the newest observed native generation can be current. Historical
 # authorization remains evidence about its own attempt, never a live link
 # once a later native generation has appeared. current0 callbacks are stale.
 live_generations=[x['generation'] for x in rows if x['_kind'] in ('IOS STATION','IFNET LINK') or x['_kind']=='IOS JOINNOTICE' and x['current']]
 newest_generation=max(live_generations,default=0)
 seen_generation=0
 for x in rows:
  if x['_kind']!='IOS STATION':continue
  if x['generation']<seen_generation:bad('native station generation regressed')
  seen_generation=max(seen_generation,x['generation'])
 # Each native attempt has one immutable callback epoch. Stale current0 notices
 # are retained as observations and cannot establish success or poison a new epoch.
 epoch={}
 for row in notices:
  if not row['current']or not row['generation']:continue
  key=(row['generation'],row['ticket'])
  if key in epoch and epoch[key]!=row['epoch']:bad('conflicting current callback epoch')
  epoch[key]=row['epoch']
  if key in attempts:attempts[key]['notices'].append(row)
 for key,a in attempts.items():
  a['links']=[x for x in links if x['generation']==key[0]]
  a['events']=[x for x in events if x.get('generation')==key[0]and x.get('ticket')==key[1]and x.get('epoch')==epoch.get(key)]
  if any(x.get('generation')==key[0]and x.get('ticket')==key[1]and key in epoch and x.get('epoch')!=epoch[key]for x in events):bad('station event callback epoch mismatch')
 summaries=[]
 for(g,t),a in sorted(attempts.items()):
  r=a['rows'];last=r[-1];ready_rows=[x for x in r if authorized(x)]
  joined=[x for x in a['notices']if x['notice']==1 and x['current']==1 and x['authorized']==1 and x['phase']==6 and x['result']==1]
  ups=[x for x in a['links']if x['authorized']==1 and x['error']==0]
  latestlink=a['links'][-1]if a['links']else None
  latestnotice=a['notices'][-1]if a['notices']else None
  notice_ready=bool(latestnotice and latestnotice['phase']==6 and latestnotice['result']==1 and latestnotice['authorized']==1)
  link_errors=[x for x in rows if x['_kind']=='IOS LINK'and x['generation']==g and x['error']!=0]
  failed_after_ready=bool(link_errors and(max(x['_line']for x in link_errors)>=last['_line']))
  notified=[x for x in a['events']if x['event']==9 and x['published']==1 and x['notified']==1 and x['status']==0]
  # njs traffic counters persist for the whole boot; native DMA counters
  # reset with each new model. For later generations, only a change after
  # their first observed snapshot proves traffic in that generation. A prefix
  # with no start baseline deliberately makes no inherited-counter claim.
  counter_keys=('tx_ok','tx_fail','tx_local','rx_delivered')
  baseline={k:0 if g==1 else r[0].get(k,0) for k in counter_keys}
  changed={k:any(((x.get(k,0)-baseline[k])&U32)>0 for x in r) for k in counter_keys}
  summaries.append(dict(generation=g,ticket=t,callback_epoch=epoch.get((g,t)),station_rows=len(r),last_service=last['service'],last_transaction=last['transaction'],last_result=last['result'],last_outcome=last['outcome'],last_native_error=last['error'],last_owner_halt=last['owner_halt'],last_native_halt=last['halt'],cookie_retired=bool(last['retired']),association_observed=any(x['associated']==1 for x in r),native_authorized_observed=bool(ready_rows),ifnet_up_observed=bool(ups),join_snapshot_observed=bool(joined),join_notification_observed=bool(ready_rows and ups and joined and notified),authorized_link_observed=bool(ready_rows and ups and joined),current_authorized_link_observed=bool(g==newest_generation and authorized(last) and latestlink and latestlink['authorized']==1 and latestlink['error']==0 and joined and notice_ready and not failed_after_ready and not counts.get('IOS STATIONGUARD',0)),data_air_tx_success_observed=changed['tx_ok'],data_air_tx_failure_observed=changed['tx_fail'],data_local_submission_observed=changed['tx_local'],bsd_input_success_observed=changed['rx_delivered'],traffic_counter_baseline=baseline,traffic_baseline_scope='boot-zero for generation1; first observed snapshot for later generations',latest_counters={k:last.get(k,0)for k in ('submitted','dma','events','tx_accept','tx_result','tx_ok','tx_fail','tx_local','queued_drop','rx_delivered')}))
 valid=not issues
 claims=('association_observed','native_authorized_observed','authorized_link_observed','current_authorized_link_observed','join_notification_observed','data_air_tx_success_observed','data_local_submission_observed','bsd_input_success_observed')
 if not valid:
  for summary in summaries:
   for key in claims+('ifnet_up_observed','join_snapshot_observed','data_air_tx_failure_observed'):summary[key]=False
 totals={k:valid and any(s[k]for s in summaries)for k in claims}
 status='INVALID'if not valid else 'AUTHORIZED_LINK_OBSERVED'if totals['current_authorized_link_observed']else 'REVOKED_OR_TERMINAL'if summaries and summaries[-1]['last_service']in(7,8)else 'IN_PROGRESS'if summaries else 'NO_NATIVE_JOIN_RECEIPT'
 return dict(valid=valid,issues=issues,status=status,**totals,dhcp_exchange_proven=False,ip_acquisition_proven=False,internet_connectivity_proven=False,attempts=summaries,row_counts=counts,join_profile_groups=join_profiles,join_owner_unavailable=len(join_owner),uncorrelated_station_events=sum(x['event']in(2,3,4,9)and'generation'not in x for x in events),stale_callback_notices=sum(not x['current']for x in notices),fatal_native_guard_observed=bool(counts.get('IOS STATIONGUARD',0)),scope='Numeric source-bound driver observations. STATION association/key flags and current IOUC snapshot/ifnet generation are separate proofs. tx_ok is a real ordinary-data TX result; tx_local is local buffer retirement after verified DMA only and is never an air ACK, peer receipt or key readiness. rx_delivered is accepted BSD input. None identifies DHCP, IP configuration or connectivity. Traffic counters are boot-cumulative; later-generation traffic requires observed change within that generation. Only the newest observed native generation can be current. Cookie retirement alone does not close an authorized native connection. Strict startup receipt must be checked independently.')
if __name__=='__main__':
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('log',type=Path);a=ap.parse_args();print(json.dumps(parse(a.log.read_bytes()),indent=2))
