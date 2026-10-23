#!/usr/bin/env python3
"""Actual retained-owner peer/profile test. Synthetic radio/MMIO only, no USB."""
from pathlib import Path
import argparse,subprocess,json,hashlib
R=next(p for p in Path(__file__).resolve().parents if(p/'AGENTS.md').is_file());T=R/'tools';F=T/'fixtures/nord2-wifi-live-control'
a=argparse.ArgumentParser(description=__doc__);a.add_argument('--log-dir',type=Path,required=True);a.add_argument('--board',type=Path);v=a.parse_args();W=v.log_dir.resolve();W.mkdir(parents=True,exist_ok=True)
owner=(T/'fixtures/nord2-wifi-rejoin/owner.c').read_text();owner=owner[:owner.index(' /* Third real Join')]
owner=owner.replace(',clock_expire;', ';');owner='\n'.join(l for l in owner.splitlines() if not l.startswith('static void test_prepare_clock'))+'\n'
owner=owner.replace('static uint8_t*station_packet',(F/'peer-owner.inc').read_text()+'\nstatic uint8_t*station_packet')
owner=owner.replace('  uint8_t data[60]={0};','  peer_probe_actual();\n  assert(nst_native.state.data_profile==cycle);\n  uint8_t data[60]={0};')
anchor='  /* Foreign origin/callback/key/DMA/halts refuse reuse without any IO. */'
owner=owner.replace(anchor,'  struct nwl_snapshot changed=peer_command(NWL_DATA_TODS,(unsigned)njs_native.native_generation);assert(changed.word[NWL_F_RESULT]==NWL_OK&&nst_native_next_data_profile()==1);\n'+anchor)
# Epoch2 intentionally retains selected raw profile, so active refusal preserves1.
owner=owner.replace('assert(!nst_native_owner_window','assert(!nst_native_owner_window')
owner=owner.replace('&&!nst_native_next_data_profile());','&&nst_native_next_data_profile()==s->data_profile);')
owner+=''' assert(delivered&&nrd_calls&&nrd_calls==nrd_restored&&nrd_checked==nrd_calls&&station_pubs>station_frames&&link_up_calls==2&&link_down_calls>=2);assert(notices[NJS_JOINED]==2&&notices[NJS_LEFT]==2&&notices[NJS_RETIRED]==2&&!manual_halts);IOLock*notice=njs_native.notice_lock;assert(!pthread_mutex_destroy(&notice->m));free(notice);assert(!pthread_mutex_destroy(&nss_native.lock->m));free(nss_native.lock);puts("PASS actual retained owner peer: two WPA2 epochs, fixed WTBL reads, physical-owner failure, stale/busy/lease/selector refusal, zero writes and active profile immutable; synthetic hardware only");return 0;}\n'''
before={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in (R/'shim/nord2-xnu').iterdir() if p.suffix in('.c','.h','.inc')}
q=W/'fixture';q.mkdir(exist_ok=True);(q/'owner.c').write_text(owner)
s=(T/'test-nord2-wifi-rejoin.py').read_text().replace("F=P/'fixtures/nord2-wifi-rejoin'",'F=Path('+repr(str(q))+')')
s='\n'.join(l for l in s.splitlines() if not l.strip().startswith("owner='static void test_prepare_clock"))+'\n'
s=s.replace('random_calls++;memset(p,0x40+random_calls,n);','if(n==8){memset(p,0x77,n);return;}random_calls++;memset(p,0x40+random_calls,n);')
old="P=Path(__file__).resolve().parent;R=next(p for p in P.parents if (p/'AGENTS.md').exists());"
s=s.replace(old,'R=Path('+repr(str(R))+');P=R/"tools";',1)
needle=" (d/'owner-fixture.inc').write_text(owner)"
insert=""" owner=owner.replace('static uint32_t ni_rd(uintptr_t b,unsigned o){','static unsigned peer_hw_reads,peer_selector,peer_changed,peer_selector_reads;\\\\nstatic uint32_t ni_rd(uintptr_t b,unsigned o){if(b==0x18434000||b==0x18438000){assert(nst_peer_model_allowed);peer_hw_reads++;if(b==0x18434000){assert(!o);return peer_selector_reads++&&peer_changed?1:peer_selector;}assert(o==0x500||o==0x508);return o==0x500?(1u<<27):(5u<<16);}')
 owner='static unsigned nst_peer_model_allowed=1;\\\\n'+owner
 owner=owner.replace('static void clock_interval_to_deadline(unsigned ms,unsigned scale,uint64_t*d){','static uint64_t peer_idle_deadline;static void clock_interval_to_deadline(unsigned ms,unsigned scale,uint64_t*d){if(ms==100&&scale==NSEC_PER_MSEC){*d=peer_idle_deadline=drain_now+100000000;return;}').replace('if(d==scheduler_deadline){','if(d==peer_idle_deadline)return THREAD_WAITING;if(d==scheduler_deadline){')
"""
s=s.replace(needle,insert+needle)
# Enable actual live binder in the canonical owning fixture; keep sources exact.
needle="f=W/'runner.py';f.write_text(s)"
s=s.replace(needle,"s=s.replace(\"names=('wifi_wpa_diag'\",\"names=('wifi_live_control','wifi_wpa_diag'\").replace(\"'-DNSS_HOST_TEST'\",\"'-DNSS_HOST_TEST','-DNWL_HOST_TEST'\")\n"+needle)
f=W/'compose.py';f.write_text(s);cmd=['python3',str(f),'--log-dir',str(W/'run')]
s=s.replace("a.add_argument('--candidate',type=Path","a.add_argument('--board',type=Path);a.add_argument('--candidate',type=Path").replace("'--log-dir',str(W)],check=True)","'--log-dir',str(W)]+(['--board',str(x.board.resolve())] if x.board else []),check=True)")
f.write_text(s)
if v.board:cmd+=['--board',str(v.board.resolve())]
r=subprocess.run(cmd,capture_output=True,text=True);(W/'runner.log').write_text(r.stdout+r.stderr);assert r.returncode==0,r.stdout+r.stderr
assert before=={p.name:hashlib.sha256(p.read_bytes()).hexdigest() for p in (R/'shim/nord2-xnu').iterdir() if p.suffix in('.c','.h','.inc')},'canonical source changed during composition'
(W/'dependency-pins.json').write_text(json.dumps(before,indent=2)+'\n')
names=['wifi_live_control.c','wifi_live_control.h','wifi_live_control_bind.inc','wifi_station_transport_bind.inc','wifi_station_transport_forward.inc','wifi_basic_config_bind.inc','wifi_station_transport.c']
report={'passed':True,'actual_native_owner':True,'copied_sources':len(before),'before_after_pins_equal':True,'device_access':False,'sources':{n:hashlib.sha256((R/'shim/nord2-xnu'/n).read_bytes()).hexdigest() for n in names},'limits':['Synthetic AP/MMIO/board DT fixture; no hardware probe or connectivity proof.','Fixedread hardware words are typed model values; native window/physical guard not mocked.','Injected fatal physical-owner state is explicitly restored only by host fixture.']};(W/'report.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))
