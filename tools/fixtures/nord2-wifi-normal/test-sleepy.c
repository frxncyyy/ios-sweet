#define main full_reference_main
#include "test-full.c"
#undef main
static void scenario(unsigned cap,unsigned pre,unsigned post,unsigned state){
 setup(0);sleep_cap=cap;sleep_pre=pre;sleep_post=post;sleep_state=state;
 sleep_eid=7;sleep_bytes=40;sleep_outer=0x3800;sleep_stream=0;sleep_pushed=0;sleep_mixed=0;sleep_normal=0;sleep_delay=0;
}
void success(unsigned cap,unsigned pre,unsigned post,unsigned state){
 scenario(cap,pre,post,state);run_worker();struct nbc_state*b=&nbc_native.state;struct ncd_state*c=&ncd_native.diag;
 assert(!manual_halts&&ncd_native.parked&&b->completed&&b->tx_done&&b->publications==1&&basic_pubs==1&&normal_pubs==22&&nwfn_native.init.transport.completed==27);
 assert(c->events==cap+1&&c->rearmed==cap+1&&c->matched&&c->history[c->events-1].kind==1&&!c->debug_events&&c->sequence_echo==1&&!c->sequence_mismatches);
 for(unsigned k=0;k<cap;k++)assert(c->history[k].kind==3&&c->history[k].eid==7&&c->history[k].debug_type==state&&c->history[k].sequence==255);
 assert(b->sleepy_events==pre+post&&!b->debug_events&&b->rearmed==pre+post&&b->last_sleepy==(pre+post?state:0));
 assert(b->budget.reserved&&b->budget.pse_remaining==2&&nct_native.session.acquired==22&&nct_native.session.released==22);
 if(pre+post)assert(b->last_debug.kind==3&&b->last_debug.debug_type==state&&!b->last_debug.debug_id&&!b->last_debug.debug_bytes);
}

#ifndef SLEEPY_FAULTS
int main(int argc,char**argv){assert(argc==2);FILE*f=fopen(argv[1],"rb");assert(f&&fread(authenticated_board,1,sizeof(authenticated_board),f)==sizeof(authenticated_board));fclose(f);
 success(1,1,1,0);success(2,2,3,1);success(14,0,0,255);success(0,7,8,255);
 /* Larger actual body/opaque header lengths never become address authority. */
 scenario(1,1,1,2);sleep_bytes=48;run_worker();assert(!manual_halts&&nbc_native.state.sleepy_events==2&&!nbc_native.state.last_debug.debug_id&&!nbc_native.state.last_debug.debug_bytes);
 /* Advice cannot manufacture TX completion. A later DDONE transition and
  * another unsolicited notification are observed only through the real loop. */
 scenario(1,1,0,1);completion_custom=1;completion_raw[0]=0x90005000;completion_raw[1]=0x404c0000;completion_raw[2]=completion_raw[3]=0;sleep_delay=1;run_worker();assert(!manual_halts&&nbc_native.state.completed&&nbc_native.state.polls==2&&nbc_native.state.sleepy_events==2&&nbc_native.state.rearmed==2);
 scenario(0,1,1,1);completion_custom=1;completion_raw[0]=0x90005000;completion_raw[1]=0x404c0000;completion_raw[2]=completion_raw[3]=0;run_worker();assert(manual_halts==1&&nbc_native.state.error==18&&!nbc_native.state.tx_done&&!nbc_native.state.completed&&nbc_native.state.sleepy_events==2&&nbc_native.state.rearmed==2&&!normal_pubs&&nwfn_native.init.transport.completed==4);
 /* Seventeenth event refuses before touching/rearming it;16 is the whole
  * BASIC debug+sleepy budget, not a per-kind allowance. */
 scenario(0,0,1,1);sleep_stream=17;run_worker();assert(manual_halts==1&&nbc_native.state.error==21&&nbc_native.state.sleepy_events==16&&nbc_native.state.rearmed==16&&basic_pubs==1&&!normal_pubs&&nwfn_native.init.transport.completed==4&&nwm_drain_entered);
 scenario(0,0,1,1);sleep_stream=17;sleep_mixed=1;sleep_bytes=48;run_worker();assert(manual_halts==1&&nbc_native.state.error==21&&nbc_native.state.sleepy_events==8&&nbc_native.state.debug_events==8&&nbc_native.state.rearmed==16&&!normal_pubs);
 scenario(1,1,1,255);sleep_normal=1;run_worker();assert(!manual_halts&&ncb_native.rx.sleepy==1&&ncb_native.rx.events==2&&ncb_native.rx.scan.done&&ncb_native.rx.history[1].kind==3&&ncb_native.rx.history[1].status==255&&ncb_native.rx.history[1].sequence==255);
 for(unsigned phase=0;phase<2;phase++)for(unsigned bad=0;bad<5;bad++){
  scenario(phase?0:1,0,phase?1:0,1);
  if(bad==0)sleep_bytes=39;if(bad==1)sleep_outer=0x3801;if(bad==2)sleep_eid=0xf0;if(bad==3)sleep_eid=0xed;if(bad==4)sleep_eid=0x28;
  run_worker();assert(manual_halts==1&&nwm_drain_entered&&!normal_pubs);
  if(phase)assert(nbc_native.state.halt&&!nbc_native.state.completed&&!nbc_native.state.rearmed&&basic_pubs==1&&nwfn_native.init.transport.completed==4);
  else assert(ncd_native.diag.halt&&!ncd_native.diag.completed&&!ncd_native.diag.rearmed&&!basic_pubs&&nwfn_native.init.transport.completed==3);
 }
 puts("PASS real-owner unsolicited sleepy before CAP/preCID2/postCID2, rawstate0/1/2/255, opaque metadata, total16budget, malformed/unknown/fatal refusal and unchanged softwarecredits/noACK. Synthetic only.");return 0;
}

#endif
