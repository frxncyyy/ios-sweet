#define NWFN_NATIVE_HOST_TEST 1
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <setjmp.h>
static jmp_buf parked_exit;
#include "wifi_main_wfdma_native.h"
#define NRPC_RUNNING 16
struct nrb_native {unsigned receipt;};
struct nrpc_state {unsigned entered,stage,rom_ready,cpu_release_attempted,cpu_released,power,protection,balanced,halt,dirty,unverified,ap_vote,wf_vote,sem20,clock_vote,rom_done;};
static struct {struct nrb_native rom;struct {struct nrpc_state state;unsigned asleep;}diag;unsigned active;uintptr_t local,rom_done;}nrpa_native;
static struct {uintptr_t host,cfg,rgu;unsigned outer_retained;struct {unsigned retained,fatal,hw_handoff,sema11,sema19,spi_inflight;struct {unsigned phase,retained,halt;}result;}state;}npa_native;
static struct {uintptr_t spm,infra,gals;unsigned saved,domain_touched;}nw;
static struct {unsigned active;struct{struct{unsigned entered;}result;}diag;}nawa_native;
static struct {unsigned active;struct{struct{unsigned entered;}state;}diag;}nwsa_native;
static unsigned ni_saved,ncc_fresh_owned,ncc_latched_dirty,npa_persistent_lifetime;
static struct {struct{struct npp_state state;unsigned asleep;}probe;struct ndp_claim claim;uintptr_t pool,wfdma;unsigned prepared,attempted,active;}nppa_native;
static unsigned char pool_memory[NDP_POOL_BYTES];
static unsigned late_ena;
static unsigned event_type=0xe000,event_cid_override=256,event_status,event_badseq;
static struct {uint32_t address,value;}reg[2048];
static unsigned registers,mmio_reads,mmio_writes,delays,barriers,manual_halts,main_flag=1,gic_active,clock_fail,ack_fail,inherited;
static int nrpc_receipt_ok(struct nrb_native*n){return n==&nrpa_native.rom&&n->receipt;}
static unsigned cap_flag=1,basic_flag=0,resource_missing,basic_pending,basic_pubs;
static void PE_parse_boot_argn(const char*s,unsigned*v,unsigned bytes){assert(bytes==4);*v=!strcmp(s,"nord2wificap")?cap_flag:!strcmp(s,"nord2wifimain")?main_flag:!strcmp(s,"nord2wifibasic")?basic_flag:1;}
static uintptr_t ml_io_map(uintptr_t p,unsigned n){assert(n==0x1000||n==0x2000);return p;}
static uint32_t ni_rd(uintptr_t b,unsigned o){uint32_t a=(uint32_t)b+o;assert(a!=0x180251f0);mmio_reads++;
 for(unsigned i=0;i<registers;i++)if(reg[i].address==a)return reg[i].value;
 switch(a){case 0x10006304:return 0xd;case 0x1000616c:case 0x10006170:return 2;case 0x1020e504:return 1;
 case 0x0c000130:return gic_active?(1u<<23):0;
 case 0x18060184:return ack_fail?0x20:0;
 case 0x18060000:return clock_fail?0:0x4006;
 case 0x180602d4:return 1;case 0x18001000:return 0x20010101;
 case 0x18000010:return 1;case 0x18000000:return 0x80;case 0x180602cc:return 4;
 case 0x184b0010:return 0x20010000;case 0x184c1604:return 0x1d1e;
 case 0x18024208:case 0x18025208:return inherited?1:0;
 default:return 0;}}
static uint32_t get32(const unsigned char *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static void put32(unsigned char *p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(8*i));}
static void hardware_register(uint32_t a,uint32_t v){for(unsigned i=0;i<registers;i++)if(reg[i].address==a){reg[i].value=v;return;}assert(registers<2048);reg[registers].address=a;reg[registers++].value=v;}
static void simulated_completion(unsigned next){
 struct nwt_state *t=&nwfn_native.init.transport;
 unsigned char *tx=pool_memory+t->tx_ring-NDP_POOL_BASE+16*t->tx_index;
 unsigned char *rx=pool_memory+t->rx_ring-NDP_POOL_BASE+16*t->rx_index;
 unsigned char *cmd=pool_memory+t->command-NDP_POOL_BASE;
 if(cmd[36]==2){
  assert(get32(cmd)==(76u|(2u<<23))&&cmd[37]==0xa0&&cmd[38]==1&&cmd[39]==5);
  for(unsigned k=64;k<76;k++)assert(!cmd[k]);
  assert(get32(tx+4)==0x404c0000u);put32(tx+4,0x80000000u);hardware_register(0x1802541c,next);basic_pubs++;
  if(basic_pending)hardware_register(0x18025200,0x20000000u);
  return; /* no fabricated RX or firmware ACK for CID2 */
 }
 unsigned bytes=cmd[36]==0x8a?116:cmd[36]==8?60:36;
 if(cmd[36]==0x41)hardware_register(0x18025204,0x20000000u);
 if(late_ena&&cmd[36]==0x41)hardware_register(0x18025204,0x20000000);
 unsigned char *event=pool_memory+t->rx_buffers-NDP_POOL_BASE+4096*t->rx_index;
 put32(tx+4,get32(tx+4)|0x80000000u);put32(rx+4,(bytes<<16)|0xc0000000u);
 memset(event,0,bytes);event[26]=(uint8_t)event_type;event[27]=(uint8_t)(event_type>>8);event[28]=cmd[36]==8?5:1;event[29]=(uint8_t)(cmd[39]+event_badseq);event[32]=(uint8_t)event_status;event[33]=(uint8_t)(event_cid_override<256?event_cid_override:cmd[36]);
 if(cmd[36]==0x8a){event[3]=0x38;event[26]=0;event[27]=0x38;event[28]=0xec;event[36]=3;
  /* Synthetic quotas and regulatory/PHY metadata, NOT actual hardware values. */
  put32(event+40,0);put32(event+44,28);put32(event+48,0x80000001u);
  put32(event+52,17);put32(event+56,resource_missing?0:9);put32(event+60,23);put32(event+64,7);put32(event+68,0x01000080);event[72]=3;
  put32(event+76,3);put32(event+80,12);put32(event+84,0x5553);event[88]=1;
  put32(event+96,8);put32(event+100,12);event[106]=1;event[107]=2;event[108]=2;event[114]=3;
 }
 hardware_register(0x1802541c,next);hardware_register(0x1802550c,(t->rx_index+1)%16);
}
static void ni_wr(uintptr_t b,unsigned o,uint32_t v){uint32_t a=(uint32_t)b+o;mmio_writes++;
 assert(a!=0x18024228&&a!=0x18025228);if(a==0x18025418&&nwfn_native.stage==NWFN_RETAINED&&nwfn_native.init.transport.submit_attempted)simulated_completion(v);if(a==0x18060010){assert(v==2);v=0;}if(a==0x18060000&&!clock_fail)v|=6;
 for(unsigned i=0;i<registers;i++)if(reg[i].address==a){reg[i].value=v;return;}
 assert(registers<2048);reg[registers].address=a;reg[registers++].value=v;}
static void delay(unsigned us){assert(us==1||us==10||us==500||us==1000);delays++;}
static void nwfn_test_barrier(void){barriers++;}
static void panic(const char *message){assert(strstr(message,"manual reset"));manual_halts++;longjmp(parked_exit,2);}
static int ncd_normal_owner_window(const struct nwfn_native *);
#define NWFN_NORMAL_LEASE(n) ncd_normal_owner_window(n)
#include "wifi_main_wfdma_bind.inc"
#undef NWFN_NORMAL_LEASE
#include "wifi_main_fw_xnu.h"
#include "wifi_main_phy_xnu.h"
struct nfp_native nfp_native;
struct nfa_native nfa_native;
static unsigned child_calls,child_fail,child_failed,child_after,span_next;
static int child(void){if(child_failed)child_after++;if(++child_calls==child_fail){child_failed=1;return -10;}return 0;}
int nfa_xnu_prepare(struct nfa_native *n){assert(n==&nfa_native&&!nwfn_native.active);if(child())return -10;n->prepared=1;return 0;}
int nfp_xnu_prepare(uintptr_t cfg,uintptr_t host){assert(cfg==npa_native.cfg&&host==npa_native.host&&!nwfn_native.active);if(child())return -10;nfp_native.prepared=1;return 0;}
int nfp_xnu_span(unsigned span){assert(span==span_next++&&nwfn_native.stage==NWFN_RETAINED);if(span>=2)assert(nfa_native.exchange.backup_valid&&nfp_native.coex_done);return child();}
int nfa_xnu_run(void){if(child())return -10;uint8_t cmd[464]={0};size_t bytes=0;cmd[36]=8;cmd[37]=0xa0;cmd[39]=nwfn_native.init.transport.next_sequence;assert(nwfn_native.init.transport.finish_phase==1);if(nwt_capture(&nwfn_io,&nwfn_native,&nwfn_native.init.transport,cmd,sizeof(cmd),5,nfa_native.exchange.rx,sizeof(nfa_native.exchange.rx),&bytes))return -11;nfa_native.exchange.event_valid=nfa_native.exchange.backup_valid=1;return 0;}
int nfp_xnu_coex(void){assert(nfa_native.exchange.backup_valid);if(child())return -10;nfp_native.coex_done=1;return 0;}
int nfp_xnu_finish(void){if(child())return -10;int r=nwfn_finish_command(0x41);if(!r)nfp_native.ready=1;return r;}
#include <setjmp.h>
static unsigned nwm_drain_entered,drain_blocks,drain_waiting;static uint64_t drain_now,drain_deadline;
static struct {unsigned ready,halt;}nwfn_main;
static void fixture(void){late_ena=0;event_type=0xe000;event_cid_override=256;event_status=event_badseq=0;nwm_drain_entered=0;drain_now=drain_deadline=drain_blocks=0;memset(&nwfn_main,0,sizeof(nwfn_main));memset(&nfp_native,0,sizeof(nfp_native));memset(&nfa_native,0,sizeof(nfa_native));child_calls=child_fail=child_failed=child_after=span_next=0;memset(&nwfn_native,0,sizeof(nwfn_native));memset(&nppa_native,0,sizeof(nppa_native));memset(reg,0,sizeof(reg));registers=mmio_reads=mmio_writes=delays=barriers=manual_halts=gic_active=clock_fail=ack_fail=inherited=0;main_flag=1;
 nrpa_native.rom.receipt=1;nrpa_native.diag.state=(struct nrpc_state){.entered=1,.stage=16,.rom_ready=1,.cpu_release_attempted=1,.cpu_released=1,.power=1,.protection=1,.balanced=1,.rom_done=0x1d1e};nrpa_native.diag.asleep=1;nrpa_native.local=0x184b0000;nrpa_native.rom_done=0x184c1000;
 npa_native.host=0x18060000;npa_native.cfg=0x18001000;npa_native.rgu=0x18000000;npa_native.outer_retained=1;npa_native.state.retained=1;npa_native.state.hw_handoff=2;npa_native.state.result.phase=10;npa_native.state.result.retained=1;
 nw.spm=0x10006000;nw.infra=0x10001000;nw.gals=0x1020e000;nw.saved=nw.domain_touched=1;ni_saved=ncc_fresh_owned=npa_persistent_lifetime=1;ncc_latched_dirty=0;
 nppa_native.prepared=nppa_native.attempted=1;nppa_native.claim=(struct ndp_claim){NDP_POOL_BASE,NDP_POOL_BYTES,3,1};nppa_native.pool=(uintptr_t)pool_memory;nppa_native.wfdma=0x18024000;nppa_native.probe.asleep=1;nppa_native.probe.state=(struct npp_state){.entered=1,.stage=NPP_DONE,.claim_ready=1,.live_valid=31,.preflight_complete=1,.verified=1,.zero_complete=1,.balanced=1,.asleep=1,.set_attempted=3,.set_accepted=2,.raw_valid=3,.words_written=32768,.words_read=32768,.mapped=1};}
typedef uintptr_t thread_t;typedef int wait_result_t;typedef int kern_return_t;typedef void*event_t;
#define THREAD_NULL 0
#define KERN_SUCCESS 0
#define THREAD_UNINT 0
#define THREAD_CONTINUE_NULL 0
static thread_t worker_thread=1;static unsigned spawns,parks;
static void(*scheduled)(void*,wait_result_t);
static thread_t current_thread(void){return worker_thread;}
static kern_return_t kernel_thread_start(void(*f)(void*,wait_result_t),void*c,thread_t*out){assert(!c);scheduled=f;*out=2;spawns++;return 0;}
static void thread_deallocate(thread_t t){assert(t==2);}
static void clock_get_uptime(uint64_t*t){*t=drain_now+=1000;}
static void absolutetime_to_nanoseconds(uint64_t t,uint64_t*out){*out=t;}
static void assert_wait(event_t c,unsigned mode){assert(c&&mode==THREAD_UNINT);}
static void thread_block(unsigned c){assert(!c);if(drain_waiting){drain_blocks++;drain_waiting=0;drain_now=drain_deadline;return;}parks++;longjmp(parked_exit,1);}
#define NSEC_PER_MSEC 1000000u
#define THREAD_WAITING (-1)
static int preemption_enabled(void){return 1;}
static void clock_interval_to_deadline(unsigned ms,unsigned scale,uint64_t*d){assert(ms==30000&&scale==NSEC_PER_MSEC);*d=drain_deadline=30000000000ULL;}
static int assert_wait_deadline(void*c,int mode,uint64_t d){assert(c&&mode==THREAD_UNINT&&d==drain_deadline);assert(nwfn_native.halt&&nwfn_native.init.halt&&nwfn_native.init.transport.halt&&nfp_native.halt);drain_waiting=1;return THREAD_WAITING;}
#include "failure_drain.inc"
#define NCD_NATIVE_HOST_TEST 1
#include "wifi_capability_bind.inc"

static void ready_fixture(void){fixture();memset(&ncd_native,0,sizeof(ncd_native));memset(&nbc_native,0,sizeof(nbc_native));basic_flag=basic_pending=resource_missing=basic_pubs=0;cap_flag=1;spawns=parks=0;worker_thread=1;scheduled=0;drain_waiting=0;
 assert(!nwfn_prepare()&&!nwfn_start());assert(!nwfn_finish_command(0x40));
 uint8_t phy[464]={0},event[3840];size_t got=0;phy[36]=8;phy[37]=0xa0;phy[39]=nwfn_native.init.transport.next_sequence;
 assert(!nwt_capture(&nwfn_io,&nwfn_native,&nwfn_native.init.transport,phy,sizeof(phy),5,event,sizeof(event),&got));
 assert(!nwfn_finish_command(0x41));assert(nwfn_native.init.transport.completed==3);
 hardware_register(0x180600f0,3);nwfn_main.ready=1;
 nfp_native=(struct nfp_native){.attempted=1,.prepared=1,.coex_done=1,.finish_attempted=1,.finish_done=1,.ready=1,.ready_raw=3,.ready_polls=1,.host_io=npa_native.host};
 nfp_native.state=(struct nfp_state){.entered=1,.dirty=1,.stage=NFP_COPIED,.completed=127,.next=7};
}
static void run_worker(void){ncd_schedule();assert(spawns==1&&scheduled);worker_thread=2;if(!setjmp(parked_exit))scheduled(0,0);}
int main(void){
 assert(sizeof(struct nwfn_native)==7472&&sizeof(struct nwt_state)==1176&&sizeof(struct nwi_state)==3040&&sizeof(struct ncd_state)==5152);
 ready_fixture();run_worker();assert(parks==1&&ncd_native.parked&&!nbc_native.attempted&&!basic_pubs&&nwfn_native.init.transport.completed==4&&!nwfn_native.halt);
 unsigned reads=mmio_reads,writes=mmio_writes;ncd_schedule();assert(spawns==1&&reads==mmio_reads&&writes==mmio_writes);
 assert(nwfn_owner(&nwfn_native)<0&&nwfn_native.error==5); /* default completed4 stays refused */
 ready_fixture();basic_flag=1;run_worker();assert(parks==1&&!manual_halts&&ncd_native.parked&&nwfn_native.init.transport.completed==5&&basic_pubs==1);
 struct nbc_state*s=&nbc_native.state;assert(s->completed&&s->tx_done&&s->tx_consumed&&s->tx_control==0x80000000u&&s->publications==1&&s->budget.reserved==1&&s->budget.pse_remaining==8&&s->plan.ple_cost==0&&!s->debug_events&&!s->rearmed);
 assert(s->metadata_valid&&s->caps.sku_present&&s->caps.phy_present&&s->caps.nss==2&&!s->verified_cap.mac_present); /* no MAC gate */
 reads=mmio_reads;writes=mmio_writes;assert(nbc_run(&ncd_io,&ncd_native,s)<0&&nbc_prepare(&ncd_native.diag,&nwfn_native.init,&nfp_native,s)<0);assert(reads==mmio_reads&&writes==mmio_writes&&basic_pubs==1);
 assert(!ncd_normal_owner_window(&nwfn_native));assert(nwfn_owner(&nwfn_native)<0&&nwfn_native.error==5); /* completed5 closes lease */
 ready_fixture();basic_flag=1;resource_missing=1;run_worker();s=&nbc_native.state;assert(s->skipped&&s->skip_reason==3&&!s->callbacks&&!s->publications&&!s->budget.reserved&&!basic_pubs&&!manual_halts&&nwfn_native.init.transport.completed==4&&!nwfn_native.halt);
 /* Build real cap receipt, then inspect actual policy's pointer/thread/phase gates before continuation. */
 ready_fixture();run_worker();ncd_native.parked=0;basic_flag=1;nbc_native.enabled=nbc_native.attempted=1;
 assert(!nbc_prepare(&ncd_native.diag,&nwfn_native.init,&nfp_native,&nbc_native.state));s=&nbc_native.state;s->entered=1;s->phase=NBC_LEASED;
 assert(ncd_normal_owner_window(&nwfn_native));struct nwfn_native clone=nwfn_native;assert(!ncd_normal_owner_window(&clone));
 worker_thread=3;assert(!ncd_normal_owner_window(&nwfn_native));worker_thread=2;s->phase=NBC_NEW;assert(!ncd_normal_owner_window(&nwfn_native));s->phase=NBC_LEASED;
 struct ncd_state capclone=ncd_native.diag;s->cap=&capclone;assert(!ncd_normal_owner_window(&nwfn_native));s->cap=&ncd_native.diag;
 s->sequence++;assert(!ncd_normal_owner_window(&nwfn_native));s->sequence--;assert(ncd_normal_owner_window(&nwfn_native));
 hardware_register(0x18025200,0x20000000u);assert(nwfn_owner(&nwfn_native)<0&&nwfn_native.error==23); /* STA29 fatal even lease */
 ready_fixture();basic_flag=1;basic_pending=1;run_worker();s=&nbc_native.state;
 assert(!parks&&manual_halts==1&&nwm_drain_entered&&drain_blocks==1&&s->halt&&!s->completed&&s->budget.reserved&&s->budget.pse_remaining==8&&basic_pubs==1&&nwfn_native.init.transport.completed==4&&ncd_native.halt&&nwfn_native.halt&&nfp_native.halt);
 struct nbc_state invalid={.prepared=1};assert(nbc_run(&ncd_io,&ncd_native,&invalid)<0);
 puts("PASS private BASIC real-owner fixture: defaultoff/refused completed4; exact sameworker lease, clone/wrongthread/replay/STA29 refusal; one76B noresponse CID2/debit1PSE0PLE/DDONE/completed5; noMAC prerequisite; missingresource skip0callbacks; sharedhalt actual30sdrain; ABI7472/1176/3040/5152. SYNTHETIC only.");return 0;
}
