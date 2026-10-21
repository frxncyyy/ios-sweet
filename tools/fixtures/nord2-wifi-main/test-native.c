#define NWFN_NATIVE_HOST_TEST 1
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
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
static void PE_parse_boot_argn(const char*s,unsigned*v,unsigned bytes){assert(bytes==4);*v=!strcmp(s,"nord2wifimain")?main_flag:1;}
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
 unsigned bytes=cmd[36]==8?60:36;
 if(late_ena&&cmd[36]==0x41)hardware_register(0x18025204,0x20000000);
 unsigned char *event=pool_memory+t->rx_buffers-NDP_POOL_BASE+4096*t->rx_index;
 put32(tx+4,get32(tx+4)|0x80000000u);put32(rx+4,(bytes<<16)|0xc0000000u);
 memset(event,0,bytes);event[26]=(uint8_t)event_type;event[27]=(uint8_t)(event_type>>8);event[28]=cmd[36]==8?5:1;event[29]=(uint8_t)(cmd[39]+event_badseq);event[32]=(uint8_t)event_status;event[33]=(uint8_t)(event_cid_override<256?event_cid_override:cmd[36]);
 hardware_register(0x1802541c,next);hardware_register(0x1802550c,(t->rx_index+1)%16);
}
static void ni_wr(uintptr_t b,unsigned o,uint32_t v){uint32_t a=(uint32_t)b+o;mmio_writes++;
 assert(a!=0x18024228&&a!=0x18025228);if(a==0x18025418&&nwfn_native.stage==NWFN_RETAINED&&nwfn_native.init.transport.submit_attempted)simulated_completion(v);if(a==0x18060010){assert(v==2);v=0;}if(a==0x18060000&&!clock_fail)v|=6;
 for(unsigned i=0;i<registers;i++)if(reg[i].address==a){reg[i].value=v;return;}
 assert(registers<2048);reg[registers].address=a;reg[registers++].value=v;}
static void delay(unsigned us){assert(us==1||us==10||us==500||us==1000);delays++;}
static void nwfn_test_barrier(void){barriers++;}
static void panic(const char *message){assert(strstr(message,"manual reset"));manual_halts++;}
#include "wifi_main_wfdma_bind.inc"
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
/* Host scheduler model for the real terminal-drain helper. This fixture
 * checks coordinator failure lifetime; detailed wake/context tests are in
 * test-drain.c. No wall-clock sleep or device operation occurs here. */
#define NSEC_PER_MSEC 1000000u
#define THREAD_UNINT 0
#define THREAD_WAITING (-1)
#define THREAD_CONTINUE_NULL NULL
static uint64_t drain_now,drain_deadline;
static unsigned drain_blocks;
static int preemption_enabled(void){return 1;}
static void clock_interval_to_deadline(unsigned ms,unsigned scale,uint64_t *d){assert(ms==30000&&scale==NSEC_PER_MSEC);*d=drain_deadline=drain_now+ms;}
static void clock_get_uptime(uint64_t *n){*n=drain_now;}
static int assert_wait_deadline(void *channel,int mode,uint64_t d){assert(channel&&mode==THREAD_UNINT&&d==drain_deadline);return THREAD_WAITING;}
static void thread_block(void *cont){assert(!cont&&nwfn_native.halt&&nwfn_native.init.halt&&nwfn_native.init.transport.halt);drain_blocks++;drain_now=drain_deadline;}
#include "wifi_main_bind.inc"
static void fixture(void){late_ena=0;event_type=0xe000;event_cid_override=256;event_status=event_badseq=0;nwm_drain_entered=0;drain_now=drain_deadline=drain_blocks=0;memset(&nwfn_main,0,sizeof(nwfn_main));memset(&nfp_native,0,sizeof(nfp_native));memset(&nfa_native,0,sizeof(nfa_native));child_calls=child_fail=child_failed=child_after=span_next=0;memset(&nwfn_native,0,sizeof(nwfn_native));memset(&nppa_native,0,sizeof(nppa_native));memset(reg,0,sizeof(reg));registers=mmio_reads=mmio_writes=delays=barriers=manual_halts=gic_active=clock_fail=ack_fail=inherited=0;main_flag=1;
 nrpa_native.rom.receipt=1;nrpa_native.diag.state=(struct nrpc_state){.entered=1,.stage=16,.rom_ready=1,.cpu_release_attempted=1,.cpu_released=1,.power=1,.protection=1,.balanced=1,.rom_done=0x1d1e};nrpa_native.diag.asleep=1;nrpa_native.local=0x184b0000;nrpa_native.rom_done=0x184c1000;
 npa_native.host=0x18060000;npa_native.cfg=0x18001000;npa_native.rgu=0x18000000;npa_native.outer_retained=1;npa_native.state.retained=1;npa_native.state.hw_handoff=2;npa_native.state.result.phase=10;npa_native.state.result.retained=1;
 nw.spm=0x10006000;nw.infra=0x10001000;nw.gals=0x1020e000;nw.saved=nw.domain_touched=1;ni_saved=ncc_fresh_owned=npa_persistent_lifetime=1;ncc_latched_dirty=0;
 nppa_native.prepared=nppa_native.attempted=1;nppa_native.claim=(struct ndp_claim){NDP_POOL_BASE,NDP_POOL_BYTES,3,1};nppa_native.pool=(uintptr_t)pool_memory;nppa_native.wfdma=0x18024000;nppa_native.probe.asleep=1;nppa_native.probe.state=(struct npp_state){.entered=1,.stage=NPP_DONE,.claim_ready=1,.live_valid=31,.preflight_complete=1,.verified=1,.zero_complete=1,.balanced=1,.asleep=1,.set_attempted=3,.set_accepted=2,.raw_valid=3,.words_written=32768,.words_read=32768,.mapped=1};}
int main(void){fixture();main_flag=0;assert(nwfn_prepare()==1&&!mmio_reads&&!mmio_writes);
 fixture();assert(!nwfn_prepare()&&!mmio_reads&&!mmio_writes);int first=nwfn_start();if(first)fprintf(stderr,"start=%d error=%u stage=%u initerr=%u phase=%u sched=%u/%u reg=%u last=%x/%x votes=%u/%u\n",first,nwfn_native.error,nwfn_native.stage,nwfn_native.init.error,nwfn_native.init.phase,nwfn_native.init.scheduler.error,nwfn_native.init.scheduler.operations,nwfn_native.init.reg_writes,nwfn_native.last_address,nwfn_native.last_value,nwfn_native.ap_owned,nwfn_native.wf_owned);assert(!first&&nwfn_native.stage==NWFN_RETAINED&&nwfn_native.ap_owned&&nwfn_native.wf_owned&&nwfn_native.init.initialized&&barriers);assert(!nwfn_owner(&nwfn_native));unsigned old=mmio_reads;assert(nwfn_start()<0&&old==mmio_reads);
 struct nwt_state *transport=&nwfn_native.init.transport;assert(!nwfn_finish_command(0x40));
 unsigned char phy[464]={0},event[3840];size_t received=0;phy[36]=8;phy[37]=0xa0;phy[39]=transport->next_sequence;
 assert(!nwt_capture(&nwfn_io,&nwfn_native,transport,phy,sizeof(phy),5,event,sizeof(event),&received)&&received==60);
 assert(!nwfn_finish_command(0x41)&&transport->completed==3&&transport->finish_phase==2);
 uint8_t byte=0;assert(nwfn_memory_read(&nwfn_native,NDP_POOL_BASE-1,&byte,1));assert(nwfn_memory_write(&nwfn_native,NDP_POOL_BASE+NDP_POOL_BYTES,&byte,1));
 uint64_t original=nwfn_native.init.transport.tx_ring;nwfn_native.init.transport.tx_ring+=16;assert(nwfn_owner(&nwfn_native)<0&&nwfn_native.halt);nwfn_native.init.transport.tx_ring=original;
 fixture();assert(!nwfn_prepare());gic_active=1;assert(nwfn_start()<0&&nwfn_native.halt&&!mmio_writes);
 fixture();assert(!nwfn_prepare());inherited=1;assert(nwfn_start()==1&&nwfn_native.asleep&&nwfn_native.balanced&&!nwfn_native.ap_owned&&!nwfn_native.wf_owned);old=mmio_reads;assert(nwfn_owner(&nwfn_native)<0&&mmio_reads==old);
 fixture();assert(!nwfn_prepare());ack_fail=1;assert(nwfn_start()<0&&nwfn_native.halt&&nwfn_native.ap_owned&&nwfn_native.ap_polls==12);
 fixture();assert(!nwfn_prepare());clock_fail=1;assert(nwfn_start()<0&&nwfn_native.halt&&nwfn_native.ap_owned&&nwfn_native.clock_spent<=16010);
 fixture();nwfn_after_pool();assert(nwfn_main.ready&&!nwfn_main.halt&&nwfn_native.stage==NWFN_RETAINED&&nwfn_native.init.transport.completed==3&&span_next==7);unsigned steps=child_calls;old=mmio_reads;nwfn_after_pool();assert(mmio_reads==old);
 for(unsigned failure=1;failure<=steps;failure++){fixture();child_fail=failure;nwfn_after_pool();assert(nwfn_main.halt&&!nwfn_main.ready&&nwfn_native.halt&&nwfn_native.init.halt&&nwfn_native.init.transport.halt&&!child_after);assert(manual_halts==(failure>=3)&&drain_blocks==(failure>=3));}
 fixture();nppa_native.probe.state.halt=1;nwfn_after_pool();assert(nwfn_main.halt&&!child_calls&&!mmio_reads&&!mmio_writes);
 for(unsigned type=7;type<=0xe000;type=0xe000){
  fixture();assert(!nwfn_prepare()&&!nwfn_start());event_type=type;event_cid_override=0xfe;
  assert(!nwfn_finish_command(0x40)&&nwfn_native.init.transport.finish_phase==1&&nwfn_native.init.transport.rx_cid==0xfe);
  fixture();assert(!nwfn_prepare()&&!nwfn_start());event_type=type;event_status=1;
  assert(nwfn_finish_command(0x40)==-1&&nwfn_native.halt&&!nwfn_native.init.transport.finish_phase);
  fixture();assert(!nwfn_prepare()&&!nwfn_start());event_type=type;event_badseq=1;
  assert(nwfn_finish_command(0x40)==-1&&nwfn_native.init.transport.rx_failure_reason==3&&!nwfn_native.init.transport.completed);
  if(type==0xe000)break;
 }
 fixture();assert(!nwfn_prepare()&&!nwfn_start());assert(nwfn_finish_command(0x42)==-1&&!nwfn_native.init.transport.submit_attempted&&!nwfn_native.init.transport.completed);
 puts("PASS native INIT format: measured0007 and legacye000, real finish helper, CID request validation retained, response CID metadata only, sequence/status negatives; synthetic model.");
 static const struct {uint32_t a,v,mask,want;unsigned reason;} guards[]={
 {0x10006304,0,0x1f,0xd,2},{0x1000616c,0,2,2,2},{0x10006170,0,2,2,2},
 {0x1020e504,0,1,1,2},{0x10001228,0x2000,0x2000,0,2},{0x10001724,4,4,0,2},
 {0x0c000130,1u<<23,(1u<<23)|(1u<<26),0,3},
 {0x180601a0,0,0xffffffffu,1,4},{0x180601a4,0,0xffffffffu,1,4},
 {0x18060184,0x20,0x20,0,4},{0x18060000,0,6,6,4},
 {0x180602d4,0,1,1,4},{0x18001000,0,0xffffffffu,0x20010101,4},
 {0x18024204,1,0x33f800ff,0,5},{0x18025204,1,0x7d37ffff,0,5},
 {0x18060010,4,4,0,6},{0x18024200,1u<<20,0x300000,0,22},
 {0x18025200,1u<<21,0x300000,0,22}};
 for(unsigned k=0;k<sizeof(guards)/sizeof(guards[0]);k++){
  fixture();assert(!nwfn_prepare()&&!nwfn_start());
  hardware_register(guards[k].a,guards[k].v);unsigned writes=mmio_writes,before_reads=mmio_reads;
  assert(nwfn_owner(&nwfn_native)<0&&nwfn_native.error==guards[k].reason);
  assert(mmio_reads-before_reads==k+1); /* exact original short-circuit reads */
  assert(nwfn_native.guard_valid&&nwfn_native.guard_address==guards[k].a&&
   nwfn_native.guard_raw==guards[k].v&&nwfn_native.guard_mask==guards[k].mask&&
   nwfn_native.guard_expected==guards[k].want&&nwfn_native.failure_stage==NWFN_RETAINED);
  unsigned reads=mmio_reads,source=nwfn_native.guard_source;
  nwfn_stop(&nwfn_native,20);nwfn_main_failure(-1);
  assert(nwfn_native.error==guards[k].reason&&nwfn_native.guard_source==source&&
   nwfn_native.guard_address==guards[k].a&&mmio_reads==reads&&mmio_writes==writes);
  assert(nwfn_owner(&nwfn_native)<0&&mmio_reads==reads&&nwfn_native.guard_address==guards[k].a);
 }
 fixture();assert(!nwfn_prepare()&&!nwfn_start());nwfn_native.init.transport.tx_ring+=16;
 old=mmio_reads;assert(nwfn_owner(&nwfn_native)<0&&nwfn_native.guard_reason==1&&
  nwfn_native.guard_source&&!nwfn_native.guard_valid&&mmio_reads==old);
 for(unsigned error=19;error<=20;error++){
  fixture();assert(!nwfn_prepare()&&!nwfn_start());
  nwfn_stop(&nwfn_native,error);unsigned reads=mmio_reads,writes=mmio_writes;
  assert(nwfn_owner(&nwfn_native)<0&&!nwfn_native.guard_reason&&!nwfn_native.guard_valid);
  nwfn_main_failure(-1);
  assert(nwfn_native.error==error&&nwfn_native.failure_stage==NWFN_RETAINED&&
   !nwfn_native.guard_reason&&!nwfn_native.guard_valid&&
   mmio_reads==reads&&mmio_writes==writes);
 }
 for(unsigned pending=0;pending<3;pending++){
  fixture();assert(!nwfn_prepare()&&!nwfn_start());
  nwfn_native.init.transport.completed=2;nwfn_native.init.transport.finish_phase=1;
  nwfn_native.init.transport.submit_attempted=1;nwfn_native.finish_tx[36]=0x41;nwfn_native.finish_tx[37]=0xa0;
  hardware_register(0x18025204,0x20000000u);
  hardware_register(0x18025200,pending?0x20000000u:0x08000001u);
  hardware_register(0x180251f0,pending==2?4:0);
  unsigned reads=mmio_reads,writes=mmio_writes;
  int rc=nwfn_owner(&nwfn_native);
  assert(mmio_reads-reads==18&&mmio_writes==writes);
  if(pending){assert(rc<0&&nwfn_native.error==23&&nwfn_native.guard_reason==23&&
   nwfn_native.guard_address==0x18025200);
   reads=mmio_reads;assert(nwfn_owner(&nwfn_native)<0&&reads==mmio_reads);
  }else{assert(!rc);
   /* Post-ACK/ready owner keeps the same pending-event fail-stop boundary. */
   nwfn_native.init.transport.completed=3;nwfn_native.init.transport.finish_phase=2;
   assert(!nwfn_owner(&nwfn_native));
   hardware_register(0x18025200,0x20000000);assert(nwfn_owner(&nwfn_native)<0&&nwfn_native.error==23);
  }
 }
 fixture();assert(!nwfn_prepare()&&!nwfn_start());
 nwfn_native.init.transport.completed=2;nwfn_native.init.transport.finish_phase=1;
  nwfn_native.init.transport.submit_attempted=1;nwfn_native.finish_tx[36]=0x41;nwfn_native.finish_tx[37]=0xa0;
 hardware_register(0x18025204,0x20000001);assert(nwfn_owner(&nwfn_native)<0&&nwfn_native.error==5);
 fixture();assert(!nwfn_prepare()&&!nwfn_start());
 assert(!nwfn_finish_command(0x40));
 memset(phy,0,sizeof(phy));phy[36]=8;phy[37]=0xa0;phy[39]=nwfn_native.init.transport.next_sequence;
 assert(!nwt_capture(&nwfn_io,&nwfn_native,&nwfn_native.init.transport,phy,sizeof(phy),5,event,sizeof(event),&received));
 late_ena=1;assert(!nwfn_finish_command(0x41)&&nwfn_native.init.transport.completed==3&&nwfn_native.init.transport.finish_phase==2);
 assert(!nwfn_owner(&nwfn_native));
 for(unsigned early=0;early<3;early++){
  fixture();assert(!nwfn_prepare()&&!nwfn_start());
  nwfn_native.init.transport.completed=2;nwfn_native.init.transport.finish_phase=1;
  if(early!=0)nwfn_native.init.transport.submit_attempted=1;
  nwfn_native.finish_tx[36]=early==1?0x40:0x41;nwfn_native.finish_tx[37]=0xa0;
  if(early==2)nwfn_native.init.transport.completed=1;
  hardware_register(0x18025204,0x20000000);
  assert(nwfn_owner(&nwfn_native)<0&&nwfn_native.error==5);
 }
 puts("PASS measured late ENA29 admission; all other enables/GIC unchanged; pending29 always stops including sub0 and postACK, existing raw summary only/no extra MMIO/W1C/replay.");
 puts("PASS captured guard diagnostics: all18 hardware predicates, software identity refusal, first-error/stage retention through finish/coordinator, no MMIO on reporting/overwrite/reentry.");
 puts("PASS native callback model: default off, preparation without MMIO, fresh retained ownership, exact transport identity, memory bounds, GIC refusal, inherited balanced sleep/no post-sleep access, bounded ACK/clock failures. Coordinator source order, all child failure boundaries/common halt and no replay. No hardware execution.");}
