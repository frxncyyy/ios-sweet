#define SUBMISSION_MAIN submission_baseline
#include "../nord2-wifi-station/test-security-submission.c"
static void token_report(struct test*h,unsigned token){uint8_t p[12]={0};put16(p,12);put16(p+2,(6u<<11)|1);put32(p+4,2u<<16);put32(p+8,token);deliver(h,3,p,sizeof p);}
static void raw_capture(struct test*h,const uint8_t*eth,size_t n){
 const uint8_t*d=h->capture,*f=d+32;C(h->capture_n==n+50&&(get32(d)&65535)==n+50&&h->cidx==1&&h->init.transport.completed==6);
 C(((get32(d)>>23)&3)==0&&((get32(d+4)>>16)&3)==2&&((get32(d+4)>>11)&31)==12);
 C(f[0]==8&&f[1]==65&&!memcmp(f+4,h->s.peer.ap,6)&&!memcmp(f+10,h->s.peer.self,6)&&!memcmp(f+16,eth,6));
 C(!f[2]&&!f[3]&&!f[22]&&!f[23]&&!memcmp(f+24,"\xaa\xaa\x03\0\0\0",6)&&!memcmp(f+30,eth+12,n-12));
 C((get32(d+12)&2)&&!(get32(d+12)&1)&&((get32(d+8)&63)==32)&&((get32(d+28)>>20)&3)==2);
}
static void page_present(struct test*h){C(!memcmp(at(h,h->s.normal.buffer,4096),h->s.normal.page,4096));C(h->s.normal.page[33]&128);C(h->s.wire_busy&&!h->s.dma_done);}
int main(void){submission_baseline();struct test*h=calloc(1,sizeof *h);assert(h);uint8_t eth[60]={0};memset(eth,255,6);eth[6]=2;eth[11]=1;eth[12]=8;
 for(unsigned profile=0;profile<=NST_DATA_TODS;profile++){
 /* Header DDONE alone never frees the separate TXP payload; foreign, malformed,
  * duplicate, host TXS, cancellation and key submissions cannot bypass this. */
 setup(h);h->s.data_profile=profile;h->s.controlled_port=1;h->defer_token=1;C(!nst_data(&h->s,1,eth,sizeof eth));step(h);C(h->s.normal.dma_done&&!h->s.normal.report_done&&h->s.dma_completed==1&&!h->events[NST_DATA_SUBMITTED]);page_present(h);if(profile)raw_capture(h,eth,sizeof eth);unsigned tok=h->s.normal.token;
 token_report(h,(tok+1)%2048);step(h);page_present(h);uint8_t bad[8]={0};put16(bad,8);put16(bad+2,(6u<<11)|1);put32(bad+4,2u<<16);deliver(h,3,bad,sizeof bad);step(h);page_present(h);txdone(h,127,5,0);step(h);page_present(h);nst_close_port(&h->s);C(!h->s.controlled_port&&h->s.wire_busy);uint8_t key[64]={0};C(nst_command(&h->s,2,7,key,sizeof key)==1);C(!nstx_idle(&h->s.normal));token_report(h,tok);step(h);C(h->events[NST_DATA_SUBMITTED]==1&&!h->s.wire_busy&&nstx_idle(&h->s.normal));for(unsigned i=0;i<4096;i++)C(!at(h,h->s.normal.buffer,4096)[i]);token_report(h,tok);step(h);C(h->events[NST_DATA_SUBMITTED]==1&&h->s.normal.completed==1);cleanup(h);
 /* Token may arrive first, but descriptor still owns its header. */
 setup(h);h->s.data_profile=profile;h->s.controlled_port=1;h->defer_dma=1;C(!nst_data(&h->s,1,eth,sizeof eth));step(h);C(h->s.normal.report_done&&!h->s.normal.dma_done&&!h->events[NST_DATA_SUBMITTED]);page_present(h);dma_retire(h);step(h);C(nstx_idle(&h->s.normal)&&h->events[NST_DATA_SUBMITTED]==1);cleanup(h);
 /* Missing token is uncertain payload ownership even after header DDONE. */
 setup(h);h->s.data_profile=profile;h->s.controlled_port=1;h->defer_token=1;C(!nst_data(&h->s,1,eth,sizeof eth));step(h);page_present(h);h->now=h->s.operation_deadline;C(nst_step(&h->s,&io,h,16,callback,h)<0);C(h->s.halt&&h->s.error==3&&h->s.normal.published&&h->s.normal.dma_done&&!h->s.normal.report_done&&!h->events[NST_DATA_SUBMITTED]);C(!memcmp(at(h,h->s.normal.buffer,4096),h->s.normal.page,4096));cleanup(h);
 /* Exercise both ring1024 and source token2048 wrap, keeping management domain
  * independent. Source token reports have no generation tag: reuse is not a
  * proof against a delayed duplicate beyond this completed lifetime. */
 setup(h);h->s.data_profile=profile;h->s.controlled_port=1;unsigned management_pid=h->s.next_pid;for(unsigned k=0;k<2200;k++){C(!nst_data(&h->s,UINT64_MAX,eth,sizeof eth));step(h);C(h->s.normal.next==(k+1)%1024&&h->s.normal.token_next==(k+1)%2048&&h->s.normal.completed==k+1&&h->s.normal.dma_completed==k+1&&h->s.normal.submitted==k+1);C(!h->s.halt&&!h->s.wire_busy&&!h->s.air_pending&&h->s.next_pid==management_pid&&!h->events[NST_FRAME_RESULT]);C(h->s.normal.credit_acquired==k+1&&h->s.normal.credit_released==k+1&&h->s.normal.credit_available==1&&!h->s.normal.credit_reserved);C(h->prior.completed==1&&h->init.transport.completed==6);}
 C(h->events[NST_DATA_SUBMITTED]==2200);cleanup(h);}
 /* Reject unsupported encapsulation before PID admission/IO, and revoke a
  * copied-but-unpublished raw payload without touching hardware. */
 setup(h);h->s.data_profile=NST_DATA_TODS;h->s.controlled_port=1;unsigned calls=h->calls;
 eth[12]=0x80;eth[13]=0xf3;C(nst_data(&h->s,3,eth,sizeof eth)<0&&!h->s.wire_busy&&!h->s.submitted&&h->calls==calls);
 eth[12]=8;eth[13]=6;C(!nst_data(&h->s,3,eth,sizeof eth));nst_close_port(&h->s);C(!h->s.wire_busy&&!h->s.submitted&&!h->s.normal.published);step(h);C(!h->published&&!h->events[NST_DATA_SUBMITTED]);cleanup(h);
 /* Original Ethernet classification remains public metadata after raw build. */
 setup(h);h->s.data_profile=NST_DATA_TODS;h->s.controlled_port=1;C(!nst_data(&h->s,4,eth,sizeof eth));step(h);raw_capture(h,eth,sizeof eth);C(h->s.data_diag.tx_data==1&&h->s.data_diag.tx_ether[1]==1&&h->s.data_diag.tx_group==1);cleanup(h);
 /* Idle metadata proof does not grant active IO allowlists and refuses each
  * outstanding operation/lifetime/credit/clock-owner invariant. */
 setup(h);h->s.controlled_port=h->s.sta_active=h->s.ptk_ready=h->s.ptk.installed=h->s.gtk.installed=1;C(nst_idle_window(&h->s,&h->init)&&!nst_lease_window(&h->s,&h->init));
 C(!nst_memory_range(&h->s,h->s.normal.buffer,4096,0)&&!nst_register_write(&h->s,0x18025308,1));
 unsigned*bad[]={&h->s.active,&h->s.wire_busy,&h->s.air_pending,&h->s.callback_active,&h->s.peer_kind,&h->s.cleanup_started,&h->s.link_lost,&h->s.pm_pending,&h->s.activation_pending,&h->s.halt,&h->s.bss_absent,&h->s.normal.published,&h->s.error};
 for(unsigned k=0;k<sizeof bad/sizeof bad[0];k++){*bad[k]=1;C(!nst_idle_window(&h->s,&h->init));*bad[k]=0;}
 h->init.transport.dirty=1;C(nst_idle_window(&h->s,&h->init));h->init.transport.dirty=0;
 h->s.ptk.installed=0;C(!nst_idle_window(&h->s,&h->init));h->s.ptk.installed=1;
 h->s.data_profile=2;C(!nst_idle_window(&h->s,&h->init));h->s.data_profile=0;
 h->s.normal.credit_available=0;C(!nst_idle_window(&h->s,&h->init));h->s.normal.credit_available=1;
 h->init.transport.tx_index++;C(!nst_idle_window(&h->s,&h->init));h->init.transport.tx_index--;C(nst_idle_window(&h->s,&h->init));cleanup(h);free(h);printf("PASS %u normal HW0/TXP actual-source lifetime assertions (synthetic DMA/firmware)\n",checks);return 0;}
