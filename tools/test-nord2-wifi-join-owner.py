#!/usr/bin/env python3
"""Canonical retained NSS/first-join deferral test; offline synthetic firmware only."""
from pathlib import Path
import tempfile,subprocess,json,argparse
a=argparse.ArgumentParser();a.add_argument("--log-dir",type=Path);a.add_argument("--board",type=Path);a.add_argument("--candidate",type=Path);args=a.parse_args()
P=Path(__file__).resolve().parent;R=next(p for p in P.parents if (p/'AGENTS.md').exists());J=P/'fixtures/nord2-wifi-station';B=R/'research/ci-39158169212/buildtree/xnu/libkern/crypto/sweet_bearssl';S=R/'shim/nord2-xnu';F=R/'tools/fixtures/nord2-wifi-normal'
names=('wifi_wpa_diag','wifi_station_data_diag','wifi_station_diag','wifi_default_feature','wifi_rx_readback','wifi_normal_backend','wifi_normal_config','wifi_normal_init','wifi_normal_rx','wifi_normal_transport','wifi_regulatory_policy','wifi_scan_bss','wifi_scan_plan','wifi_basic_config','wifi_basic_resources','wifi_basic_metadata','wifi_basic_primitives','wifi_capability','wifi_capability_dispatcher','wifi_main_initializer','wifi_main_dmashdl','wifi_main_init_plan','wifi_main_transport','wifi_main_layout','wifi_dma_preflight','wifi_main_command','dma_pool_contract','wifi_ios_join','wifi_station_wire','wifi_station_transport','wifi_wpa_nst','wifi_wpa_owner','wifi_station_transaction','wifi_wpa2','wifi_wpa_crypto')
with tempfile.TemporaryDirectory(prefix='nss-native-') as td:
 d=Path(td)
 for f in S.iterdir():
  if f.suffix in ('.h','.inc','.c'):(d/f.name).write_bytes(f.read_bytes())
 if args.candidate:
  for f in args.candidate.iterdir():
   if f.suffix in ('.h','.inc','.c'):(d/f.name).write_bytes(f.read_bytes())
 (d/'nss_host.h').write_bytes((J/'join-owner-host.h').read_bytes())
 for f in F.iterdir():
  if f.suffix=='.inc':(d/f.name).write_bytes(f.read_bytes())
 owner=(d/'owner-fixture.inc').read_text().replace('static unsigned cap_flag=1,','static unsigned ios_flag=1;\nstatic unsigned cap_flag=1,').replace('*v=!strcmp(s,"nord2wificap")?', '*v=!strcmp(s,"nord2wifiios")?ios_flag:!strcmp(s,"nord2wificap")?')
 owner=owner.replace('!strcmp(s,"nord2wifiios")?0:', '!strcmp(s,"nord2wifiios")?ios_flag:')
 (d/'owner-fixture.inc').write_text(owner)
 # Existing full native fixture's probes and source-owned board/auth init remain.
 test=(F/'test-full.c').read_text();test=test[:test.index('int main(')]
 test=test.replace('assert(!delivered++&&nct_native.session.cursor==(NCB_COMMANDS-1)&&nct_native.session.record[NCB_COMMANDS-1].publication_attempted&&!nct_native.session.record[NCB_COMMANDS-1].published);','delivered++;unsigned ix=nct_native.session.continuation?0:NCB_COMMANDS-1;assert(nct_native.session.cursor==ix&&nct_native.session.record[ix].publication_attempted&&!nct_native.session.record[ix].published);')
 test=test.replace('p[36]=1;p[40]', 'p[36]=cmd[64];p[40]')
 test=test.replace('static unsigned mode,delivered,probe_memory,probe_write,delayed_trigger;', 'static unsigned refill_drain;\nstatic uint64_t cancel_inflight;\nstatic unsigned mode,delivered,probe_memory,probe_write,delayed_trigger;').replace('if(cmd[36]!=3)return;', 'if(cmd[36]!=3)return; if(cancel_inflight){assert(!nord2_wifi_scan_cancel(cancel_inflight));cancel_inflight=0;}')
 test=test.replace('probe_write|=1u<<r->current;', 'probe_write|=1u<<r->current;if(refill_drain&&nct_native.session.phase==NCT_DRAIN&&r->current==0){unsigned future=(q->next+1)%q->ring.count;uint8_t*dd=pool_memory+(q->ring.descriptors-NDP_POOL_BASE)+16*future;put32(dd+4,0xc0000000u);hardware_register(q->ring.base+12,(future+1)%q->ring.count);}')
 test+='''
static unsigned callbacks,last_status,join_notices[5];
static void completed(void*c,uint64_t gen,uint64_t ticket,enum nss_status status){assert(c==&callbacks&&gen==99);callbacks++;last_status=status;assert(!nord2_wifi_scan_release(ticket));}
static void joined(void*c,uint64_t epoch,uint64_t ticket,enum njs_notice notice){assert(c==&join_notices&&epoch==7&&ticket);join_notices[notice]++;if(notice==NJS_RETIRED){struct njs_snapshot state;assert(!nord2_wifi_join_copy_state(&state)&&state.phase==NJS_FAILED);}}
int nord2_wifi_ifnet_link(uint64_t g,int a){assert(g&&!a);return 0;}
int nord2_wifi_ifnet_receive(uint64_t g,const uint8_t*p,size_t n){assert(g&&p&&n>=14);return 0;}
static struct niwj_request request(void){struct niwj_request q={0};static const uint8_t rsn[22]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};q.ssid_length=4;memcpy(q.ssid,"test",4);memset(q.pmk,0x42,32);memcpy(q.station_rsn,rsn,22);q.security=1;return q;}
static void drain(void){worker_thread=2;if(!setjmp(parked_exit))nss_owner_loop();}
int main(int argc,char**argv){assert(argc==2);FILE*f=fopen(argv[1],"rb");assert(f&&fread(authenticated_board,1,sizeof authenticated_board,f)==sizeof authenticated_board);fclose(f);
 setup(0);run_worker();assert(nss_native.ready&&!ncd_native.parked&&service_ready_hooks==1&&nct_native.session.completed==26);
 unsigned pubs=normal_pubs;struct nss_request scan={0};struct niwj_request join=request();uint64_t st[4],jt;
 for(unsigned k=0;k<4;k++)assert(!nord2_wifi_scan_enqueue(&scan,&callbacks,99,completed,&st[k]));
 assert(!nord2_wifi_join_enqueue(&join,&join_notices,7,joined,&jt));memset(&join,0,sizeof join);unsigned reads=mmio_reads,writes=mmio_writes;
 assert(njs_owner_work()==0&&!nst_native.attempted&&reads==mmio_reads&&writes==mmio_writes);uint64_t rejected=123;assert(nord2_wifi_scan_enqueue(&scan,&callbacks,99,completed,&rejected)==2&&rejected==123);
 drain();assert(callbacks==4&&last_status==NSS_COMPLETE&&normal_pubs==pubs+4&&!nst_native.attempted&&join_notices[NJS_JOIN_FAILED]==1&&join_notices[NJS_RETIRED]==1&&njs_native.cookie_retired&&!njs_native.cookie&&!njs_native.completion);struct niwj_request zero={0};assert(!memcmp(&zero,&njs_native.request,sizeof zero));
 /* A canceled deferred join retires before the already-queued real scan,
  * leaves packet/RX cursor continuation to that scan, and never prepares NST. */
 delivered=0;assert(!nord2_wifi_scan_enqueue(&scan,&callbacks,99,completed,&st[0]));join=request();assert(!nord2_wifi_join_enqueue(&join,&join_notices,7,joined,&jt));assert(!nord2_wifi_join_cancel(jt));reads=mmio_reads;writes=mmio_writes;assert(njs_owner_work()==1&&reads==mmio_reads&&writes==mmio_writes&&!nst_native.attempted&&join_notices[NJS_RETIRED]==2);drain();assert(callbacks==5&&normal_pubs==pubs+5&&!njs_native.used);
 /* Deadline while queued: no authority callback, no late prepare, one retire. */
 delivered=0;assert(!nord2_wifi_scan_enqueue(&scan,&callbacks,99,completed,&st[0]));join=request();assert(!nord2_wifi_join_enqueue(&join,&join_notices,7,joined,&jt));drain_now=njs_native.deadline_us*1000;reads=mmio_reads;writes=mmio_writes;assert(njs_owner_work()==1&&njs_native.snapshot.result==9&&reads==mmio_reads&&writes==mmio_writes&&!nst_native.attempted&&join_notices[NJS_RETIRED]==3);drain();assert(callbacks==6&&normal_pubs==pubs+6&&!manual_halts&&!njs_native.used);
 assert(!memcmp(&zero,&njs_native.request,sizeof zero));assert(delivered&&nrd_calls&&nrd_calls==nrd_restored&&nrd_checked==nrd_calls);IOLock*notice=njs_native.notice_lock;assert(!pthread_mutex_destroy(&notice->m));free(notice);assert(!pthread_mutex_destroy(&nss_native.lock->m));free(nss_native.lock);puts("PASS actual retained native owner: boot26 +6 real queued scans, deferredjoin admission, no newscan admission, scan callbacks/5ringcontinuation beforejoin, cancel/deadline zeroNST/zeroMMIO/oneRetired/secretwipe; no RF claim");return 0;}
'''
 (d/'test.c').write_text(test)
 # Same existing readback fixture seam; owner/checked IO are not replaced.
 rx=(d/'wifi_normal_rx.c').read_text().replace('#include "wifi_normal_rx.h"','#include "wifi_normal_rx.h"\nextern int nrd_host_probe(const struct ncd_io*,void*,struct ncr_state*,struct nrd_snapshot*,unsigned);\n#define nrd_capture nrd_host_probe');(d/'wifi_normal_rx.c').write_text(rx)
 basic=(d/'wifi_basic_config_bind.inc').read_text().replace(' int result=nbc_prepare',' sleepy_pre_basic();\n int result=nbc_prepare');(d/'wifi_basic_config_bind.inc').write_text(basic)
 cap=(d/'wifi_capability_bind.inc').read_text().replace('static int ncd_native_mread(void*c,uint64_t p,void*v,uint32_t n)\n{','static int ncd_native_mread(void*c,uint64_t p,void*v,uint32_t n)\n{full_test_probe(0);').replace('static int ncd_native_write(void*c,uint32_t p,uint32_t v)\n{','static int ncd_native_write(void*c,uint32_t p,uint32_t v)\n{nrd_forbid_write();full_test_probe(1);').replace('static int ncd_native_mwrite(void*c,uint64_t p,const void*v,uint32_t n)\n{','static int ncd_native_mwrite(void*c,uint64_t p,const void*v,uint32_t n)\n{nrd_forbid_write();');(d/'wifi_capability_bind.inc').write_text(cap)
 cmd=['clang','-std=c11','-DNSS_HOST_TEST','-DNJS_HOST_TEST','-DNCFG_HOST_TEST','-O1','-g','-Wall','-Wextra','-Werror','-pthread','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(d),'-I'+str(B/'inc'),'-I'+str(B/'src'),str(d/'test.c'),*[str(d/(name+'.c')) for name in names],*[str(B/'src'/x) for x in ['hash/sha1.c','codec/dec32be.c','codec/enc32be.c','symcipher/aes_ct.c','symcipher/aes_ct_dec.c','symcipher/aes_ct_cbcdec.c']],'-lcrypto','-o',str(d/'test')];subprocess.run(cmd,check=True);result=subprocess.run([str(d/'test'),str(args.board or R/'research/nord2/wifi/calibration/WIFI')],capture_output=True,text=True);
 if args.log_dir:args.log_dir.mkdir(parents=True,exist_ok=True);(args.log_dir/'test-native.log').write_text(result.stdout+result.stderr)
 print(result.stdout.splitlines()[-1:] if result.stdout else result.stderr);result.check_returncode()
