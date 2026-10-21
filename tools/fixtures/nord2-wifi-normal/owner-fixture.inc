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
static unsigned normal_enabled,normal_pubs,normal_no_ddone,normal_wrong_didx,scan_flag=1;
static void full_deliver(const unsigned char*);
static unsigned late_ena,completion_custom;static uint32_t completion_raw[4];
static uint64_t clock_step=1000;static unsigned completion_didx_override=UINT32_MAX;
static unsigned event_type=0xe000,event_cid_override=256,event_status,event_badseq;
static struct {uint32_t address,value;}reg[2048];
static unsigned registers,mmio_reads,mmio_writes,delays,barriers,manual_halts,main_flag=1,gic_active,clock_fail,ack_fail,inherited;
static int nrpc_receipt_ok(struct nrb_native*n){return n==&nrpa_native.rom&&n->receipt;}
static unsigned cap_flag=1,basic_flag=0,resource_missing,basic_pending,basic_pubs;
static void PE_parse_boot_argn(const char*s,unsigned*v,unsigned bytes){assert(bytes==4);*v=!strcmp(s,"nord2wifiios")?0:!strcmp(s,"nord2wificap")?cap_flag:!strcmp(s,"nord2wifimain")?main_flag:!strcmp(s,"nord2wifibasic")?basic_flag:!strcmp(s,"nord2wifiscan")?scan_flag:1;}
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
static unsigned sleep_cap,sleep_pre,sleep_post,sleep_state,sleep_eid=7,sleep_bytes=40,sleep_outer=0x3800,sleep_stream,sleep_pushed,sleep_mixed,sleep_normal,sleep_delay;
static unsigned sleepy_didx(void){for(unsigned j=0;j<registers;j++)if(reg[j].address==0x1802550c)return reg[j].value;return nwfn_native.init.transport.rx_index;}
static void sleepy_at(unsigned index){struct nwt_state*t=&nwfn_native.init.transport;uint8_t*d=pool_memory+(t->rx_ring-NDP_POOL_BASE)+16*index,*p=pool_memory+(t->rx_buffers-NDP_POOL_BASE)+4096*index;
 memset(p,0,sleep_bytes);p[0]=p[1]=p[24]=p[25]=255;p[2]=(uint8_t)sleep_outer;p[3]=(uint8_t)(sleep_outer>>8);p[26]=0x34;p[27]=0x12;p[28]=(uint8_t)sleep_eid;p[29]=255;p[36]=(uint8_t)sleep_state;p[37]=p[38]=p[39]=255;if(sleep_bytes>40)memset(p+40,0xab,sleep_bytes-40);
 if(sleep_mixed&&(sleep_pushed&1)){p[28]=0x27;p[38]=3;p[44]=p[45]=0;}
 put32(d,(uint32_t)(t->rx_buffers+4096*index));put32(d+4,0xc0000000u|(sleep_bytes<<16));put32(d+8,0);put32(d+12,0);}
static void sleepy_enqueue(unsigned count){unsigned index=sleepy_didx();while(count--){sleepy_at(index);index=(index+1)%16;sleep_pushed++;}hardware_register(0x1802550c,index);}
static void sleepy_pre_basic(void){sleepy_enqueue(sleep_pre);}
static void simulated_completion(unsigned next){
 struct nwt_state *t=&nwfn_native.init.transport;
 unsigned char *tx=pool_memory+(t->tx_ring-NDP_POOL_BASE)+16*t->tx_index;
 unsigned char *rx=pool_memory+(t->rx_ring-NDP_POOL_BASE)+16*t->rx_index;
 unsigned char *cmd=pool_memory+(t->command-NDP_POOL_BASE);
 if(normal_enabled&&t->completed>=5){
  normal_pubs++;assert(cmd[37]==0xa0&&cmd[39]==(uint8_t)(6+normal_pubs-1));
  put32(tx,0xffffffffu);put32(tx+4,normal_no_ddone?0x7fffffffu:0xffffffffu);put32(tx+8,0xdeadbeefu);put32(tx+12,0x12345678u);
  hardware_register(0x1802541c,normal_wrong_didx?next+1:next);full_deliver(cmd);return; /* no fabricated ACK */
 }
 if(cmd[36]==2){
  assert(get32(cmd)==(76u|(2u<<23))&&cmd[37]==0xa0&&cmd[38]==1&&cmd[39]==5);
  for(unsigned k=64;k<76;k++)assert(!cmd[k]);
  assert(get32(tx+4)==0x404c0000u);put32(tx+4,0x80000000u);if(completion_custom)for(unsigned k=0;k<4;k++)put32(tx+4*k,completion_raw[k]);hardware_register(0x1802541c,completion_didx_override==UINT32_MAX?next:completion_didx_override);basic_pubs++;
  if(basic_pending)hardware_register(0x18025200,0x20000000u);
  sleepy_enqueue(sleep_post);
  return; /* no fabricated RX or firmware ACK for CID2 */
 }
 unsigned bytes=cmd[36]==0x8a?116:cmd[36]==8?60:36;
 if(cmd[36]==0x41)hardware_register(0x18025204,0x20000000u);
 if(late_ena&&cmd[36]==0x41)hardware_register(0x18025204,0x20000000);
 unsigned char *event=pool_memory+(t->rx_buffers-NDP_POOL_BASE)+4096*t->rx_index;
 put32(tx+4,get32(tx+4)|0x80000000u);put32(rx+4,(bytes<<16)|0xc0000000u);
 memset(event,0,bytes);event[26]=(uint8_t)event_type;event[27]=(uint8_t)(event_type>>8);event[28]=cmd[36]==8?5:1;event[29]=(uint8_t)(cmd[39]+event_badseq);event[32]=(uint8_t)event_status;event[33]=(uint8_t)(event_cid_override<256?event_cid_override:cmd[36]);
 if(cmd[36]==0x8a){event[3]=0x38;event[26]=0;event[27]=0x38;event[28]=0xec;event[36]=3;
  /* Synthetic quotas and regulatory/PHY metadata, NOT actual hardware values. */
  put32(event+40,0);put32(event+44,28);put32(event+48,0x80000001u);
  put32(event+52,17);put32(event+56,resource_missing?0:3);put32(event+60,23);put32(event+64,7);put32(event+68,0x01000080);event[72]=3;
  put32(event+76,99);put32(event+80,12);put32(event+84,0x5553);event[88]=1;
  put32(event+96,8);put32(event+100,12);event[106]=1;event[107]=2;event[108]=2;event[114]=15;
 }
 hardware_register(0x1802541c,next);hardware_register(0x1802550c,(t->rx_index+1)%16);
 if(cmd[36]==0x8a&&sleep_cap){unsigned dst=(t->rx_index+sleep_cap)%16;uint8_t*dd=pool_memory+(t->rx_ring-NDP_POOL_BASE)+16*dst,*dp=pool_memory+(t->rx_buffers-NDP_POOL_BASE)+4096*dst;memcpy(dp,event,bytes);put32(dd,(uint32_t)(t->rx_buffers+4096*dst));put32(dd+4,0xc0000000u|(bytes<<16));put32(dd+8,0);put32(dd+12,0);for(unsigned k=0;k<sleep_cap;k++)sleepy_at((t->rx_index+k)%16);hardware_register(0x1802550c,(dst+1)%16);}

}
static void ni_wr(uintptr_t b,unsigned o,uint32_t v){uint32_t a=(uint32_t)b+o;mmio_writes++;if(a==0x18025508&&basic_pubs&&sleep_pushed<sleep_stream)sleepy_enqueue(1);
 assert(a!=0x18024228&&a!=0x18025228);if(a==0x18025418&&nwfn_native.stage==NWFN_RETAINED&&nwfn_native.init.transport.submit_attempted)simulated_completion(v);if(a==0x18060010){assert(v==2);v=0;}if(a==0x18060000&&!clock_fail)v|=6;
 for(unsigned i=0;i<registers;i++)if(reg[i].address==a){reg[i].value=v;return;}
 assert(registers<2048);reg[registers].address=a;reg[registers++].value=v;}
static void delay(unsigned us){if(us==1000&&basic_pubs&&sleep_delay){sleep_delay=0;sleepy_enqueue(1);struct nwt_state*t=&nwfn_native.init.transport;uint8_t*d=pool_memory+(t->tx_ring-NDP_POOL_BASE)+16*t->tx_index;put32(d+4,get32(d+4)|0x80000000u);}assert(us==1||us==10||us==500||us==1000);delays++;}
static void nwfn_test_barrier(void){barriers++;}
static void panic(const char *message){assert(strstr(message,"manual reset"));manual_halts++;longjmp(parked_exit,2);}
static int ncd_normal_owner_window(const struct nwfn_native *);
#define NWFN_NORMAL_LEASE(n) ncd_normal_owner_window(n)
#include "wifi_normal_backend_forward.inc"
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
static unsigned nwm_drain_entered,drain_blocks,drain_waiting,scheduler_wait;static uint64_t scheduler_deadline;static uint64_t drain_now,drain_deadline;
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
static void clock_get_uptime(uint64_t*t){*t=drain_now+=clock_step+full_clock_extra();}
static void absolutetime_to_nanoseconds(uint64_t t,uint64_t*out){*out=t;}
static void assert_wait(event_t c,unsigned mode){assert(c&&mode==THREAD_UNINT);}
static void thread_block(unsigned c){assert(!c);if(drain_waiting){drain_blocks++;drain_waiting=0;drain_now=drain_deadline;return;}if(scheduler_wait){scheduler_wait=0;drain_now=scheduler_deadline+19000000;return;}parks++;longjmp(parked_exit,1);}
#define NSEC_PER_MSEC 1000000u
#define NSEC_PER_USEC 1000u
#define THREAD_WAITING (-1)
static int preemption_enabled(void){return 1;}
static void clock_interval_to_deadline(unsigned ms,unsigned scale,uint64_t*d){if(ms==1000&&scale==NSEC_PER_USEC){*d=scheduler_deadline=drain_now+1000000;return;}assert(ms==30000&&scale==NSEC_PER_MSEC);*d=drain_deadline=30000000000ULL;}
static int assert_wait_deadline(void*c,int mode,uint64_t d){assert(c&&mode==THREAD_UNINT);if(d==scheduler_deadline){scheduler_wait=1;return THREAD_WAITING;}assert(d==drain_deadline);assert(nwfn_native.halt&&nwfn_native.init.halt&&nwfn_native.init.transport.halt&&nfp_native.halt);drain_waiting=1;return THREAD_WAITING;}
#include "failure_drain.inc"
#define NCD_NATIVE_HOST_TEST 1

#include "wifi_capability_bind.inc"
#include "fixture-dt.inc"
#include "wifi_normal_backend_bind.inc"

static void ready_fixture(void){completion_custom=0;completion_didx_override=UINT32_MAX;clock_step=1000;scheduler_wait=0;scheduler_deadline=0;fixture();memset(&ncd_native,0,sizeof(ncd_native));memset(&nbc_native,0,sizeof(nbc_native));basic_flag=basic_pending=resource_missing=basic_pubs=0;cap_flag=1;spawns=parks=0;worker_thread=1;scheduled=0;drain_waiting=0;
 assert(!nwfn_prepare()&&!nwfn_start());assert(!nwfn_finish_command(0x40));
 uint8_t phy[464]={0},event[3840];size_t got=0;phy[36]=8;phy[37]=0xa0;phy[39]=nwfn_native.init.transport.next_sequence;
 assert(!nwt_capture(&nwfn_io,&nwfn_native,&nwfn_native.init.transport,phy,sizeof(phy),5,event,sizeof(event),&got));
 assert(!nwfn_finish_command(0x41));assert(nwfn_native.init.transport.completed==3);
 hardware_register(0x180600f0,3);nwfn_main.ready=1;
 nfp_native=(struct nfp_native){.attempted=1,.prepared=1,.coex_done=1,.finish_attempted=1,.finish_done=1,.ready=1,.ready_raw=3,.ready_polls=1,.host_io=npa_native.host};
 nfp_native.state=(struct nfp_state){.entered=1,.dirty=1,.stage=NFP_COPIED,.completed=127,.next=7};
}
static void run_worker(void){ncd_schedule();assert(spawns==1&&scheduled);worker_thread=2;if(!setjmp(parked_exit))scheduled(0,0);}

_Static_assert(sizeof(struct nwfn_native)==7472,"shared native ABI unchanged");
_Static_assert(sizeof(struct nwt_state)==1176,"transport ABI unchanged");
_Static_assert(sizeof(struct nwi_state)==3040,"initializer ABI unchanged");
_Static_assert(sizeof(struct ncd_state)==5152,"CAP retained layout unchanged");
