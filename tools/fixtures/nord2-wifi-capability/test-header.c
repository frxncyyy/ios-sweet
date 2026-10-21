#define NCD_NATIVE_FIXTURE_MAIN header_fixture_main
#include "test-native-adapter.c"
#include <stddef.h>
int main(void){
 /* Every u16 inner type is captured byte-exact before the unchanged policy.
  * All inner values are opaque on bounded capability and ASCII EID27 paths. */
 for(unsigned type=0;type<=65535;type++){
  fixture();struct mock m={.txidx=3,.rxidx=3,.mode=100+type};struct ncd_state s={0};
  int rc=ncd_run(&io,&m,&init,&firmware,&s,1);
  assert(s.events==1&&s.history[0].header_valid==1&&s.history[0].inner_type==type);
  struct ncd_event*h=&s.history[0];
  assert(h->bytes==504&&h->rxd_bytes==504&&h->event_bytes==480&&h->rx_class==0x3800&&h->eid==0xec&&h->sequence==4);
  assert(!memcmp(h->header,s.frame,36));
  assert(!rc&&s.completed&&s.rearmed==1&&h->kind==1&&!s.halt&&!s.error);
  unsigned calls=m.calls;assert(ncd_run(&io,&m,&init,&firmware,&s,1)<0&&m.calls==calls);
 }
 /* Opaque inner metadata cannot bypass outer/EID/descriptor/TLV/debug
  * guards, and sequence metadata retains the source-selected semantics. */
 const unsigned types[]={0,7,0xe000,65535};
 for(unsigned ti=0;ti<4;ti++)for(diagnostic_fault=1;diagnostic_fault<=8;diagnostic_fault++){
  fixture();struct mock m={.txidx=3,.rxidx=3,.mode=100+types[ti]};struct ncd_state s={0};
  int rc=ncd_run(&io,&m,&init,&firmware,&s,1);
  if(diagnostic_fault==1){assert(!s.history[0].header_valid&&!s.history[0].inner_type);for(unsigned k=0;k<36;k++)assert(!s.history[0].header[k]);}
  if(diagnostic_fault==8){assert(!rc&&s.completed&&s.rearmed==1&&!s.sequence_echo&&s.sequence_mismatches==1);}
  else{assert(rc<0&&!s.completed&&!s.matched&&!s.rearmed&&s.halt&&init.halt&&init.transport.halt);}
 }
 diagnostic_fault=0;
 /* Redundant inner guard is removed from the pure parser only; wrong EID,
  * wrong supplied sequence and every structurally incomplete extent refuse,
  * preserving the output object atomically. No raw MAC is printed. */
 uint8_t packet[504]={0};packet[3]=0x38;packet[28]=0xec;packet[29]=4;packet[36]=1;put(packet+40,7);put(packet+44,8);packet[48]=2;
 for(unsigned ti=0;ti<4;ti++){
  packet[26]=types[ti]&255;packet[27]=types[ti]>>8;
  struct ncp_result r,before;memset(&r,0xa5,sizeof(r));before=r;
  for(unsigned n=0;n<56;n++){assert(ncp_parse(packet,n,4,&r)<0&&!memcmp(&r,&before,sizeof(r)));}
  packet[2]=1;assert(ncp_parse(packet,sizeof(packet),4,&r)<0&&!memcmp(&r,&before,sizeof(r)));packet[2]=0;
  packet[3]=0;assert(ncp_parse(packet,sizeof(packet),4,&r)<0&&!memcmp(&r,&before,sizeof(r)));packet[3]=0x38;
  packet[28]=0xed;assert(ncp_parse(packet,sizeof(packet),4,&r)<0&&!memcmp(&r,&before,sizeof(r)));packet[28]=0xec;
  assert(ncp_parse(packet,sizeof(packet),3,&r)<0&&!memcmp(&r,&before,sizeof(r)));
  assert(!ncp_parse(packet,sizeof(packet),4,&r)&&r.decoded&&r.mac_valid);
  packet[3]=0x78;assert(!ncp_parse(packet,sizeof(packet),4,&r));packet[3]=0x38;
  packet[2]=0x10;assert(!ncp_parse(packet,sizeof(packet),4,&r));packet[2]=0;
 }
 /* Native success report uses retained header after completion/parking. */
 native_fixture();ncd_schedule();model.mode=107;running_thread=2;
 int end=setjmp(terminal);if(!end)worker_entry(0,0);
 assert(end==1&&!panic_count&&!nwm_drain_entered&&!drain_blocks&&!ncd_native.halt&&!nwfn_native.halt&&!nwfn_native.init.halt&&!nfp_native.halt&&ncd_native.parked);
 assert(ncd_native.diag.history[0].inner_type==7&&ncd_native.diag.history[0].header_valid==1&&!ncd_native.diag.error&&ncd_native.diag.rearmed==1);
 unsigned calls=model.calls;
 /* Poison every body byte in retained copy after completion. Header logging must
  * remain identical and never observe any payload/TLV/MAC body byte. */
 memset(ncd_native.diag.frame+36,0xa5,sizeof(ncd_native.diag.frame)-36);
 ncd_report(0);assert(model.calls==calls&&!model.after);
 printf("PASS header-only diagnostics: all65536 inner types captured; opaque inner for bounded EIDec and ASCII EID27;32crossed guard cases and224pure truncations and8wrongouter atomic refusals; actual-shaped504/480 synthetic match; no after-completion callbacks/body output; eventsize=%zu state=%zu headeroffset=%zu. NO actual inner value inferred.\n",sizeof(struct ncd_event),sizeof(struct ncd_state),offsetof(struct ncd_event,header));
 return 0;
}
