#define SUBMISSION_MAIN existing_submission_main
#include "test-security-submission.c"
#undef SUBMISSION_MAIN
#include "packets.h"
static void actual_ether(struct test*h,const uint8_t*eth,size_t n,unsigned group,unsigned pn,unsigned bad){
 uint8_t p[56+1518]={0};assert(n<=1518);put16(p,(unsigned)n+56);put16(p+2,2u<<11);put32(p+4,(group?h->s.peer.group_wlan:h->s.peer.wlan)|(9u<<11)|(4u<<16)|((group?h->s.gtk.key_id:h->s.ptk.key_id)<<21));put32(p+8,1u<<13);put32(p+12,6u<<8);put16(p+24,0x4208);memcpy(p+26,h->s.peer.ap,6);p[40]=(uint8_t)pn;memcpy(p+56,eth,n);if(bad)p[26]^=2;deliver(h,0,p,(unsigned)n+56);
}
int main(void){
 struct test*h=calloc(1,sizeof *h);assert(h);struct composed x;struct stc_arbiter a={0};init_join(h,&x,&a);m2_retired(&x);eapol_rx(&x,m3,sizeof m3);advance(&x);finish_keys(&x);
 uint8_t p[1518];size_t n=dhcp_packet(p,2,1,0);memcpy(p,h->s.peer.self,6);
 actual_ether(h,p,n,0,1,0);advance(&x);C(h->events[NST_ETHERNET]==1&&h->s.data_diag.rx_unicast==1);
 actual_ether(h,p,n,0,1,0);advance(&x);C(h->s.data_diag.rejected[NSDD_RX_REPLAY]==1&&h->events[NST_ETHERNET]==1);
 actual_ether(h,p,n,0,2,1);advance(&x);C(h->s.data_diag.rejected[NSDD_RX_PEER]==1&&h->events[NST_ETHERNET]==1);
 h->s.ptk.installed=0;actual_ether(h,p,n,0,2,0);advance(&x);C(h->s.data_diag.rejected[NSDD_RX_KEY]==1&&h->events[NST_ETHERNET]==1);h->s.ptk.installed=1;
 actual_ether(h,p,n,0,2,0);advance(&x);C(h->events[NST_ETHERNET]==2&&h->s.controlled_port&&h->s.ptk.pn[16]==2);/* Exercise exact current error gate under the actual NST owner/rearm path.
  * Payload length is deliberately zero: only the fixed24B RXD exists. */
 const unsigned masks[3]={0x0f800000,0x0b800000,0x1803};unsigned errors=0;
 for(unsigned w=0;w<3;w++)for(unsigned bit=0;bit<32;bit++)if(masks[w]&(1u<<bit))for(unsigned a1=0;a1<4;a1++)for(unsigned ht=0;ht<2;ht++){
  uint8_t bad[24]={0};put16(bad,24);put16(bad+2,2u<<11);put32(bad+4,h->s.peer.wlan|(4u<<16));put32(bad+8,ht<<13);put32(bad+12,a1<<16);
  put32(bad+(w==0?4:w==1?8:16),get32(bad+(w==0?4:w==1?8:16))|(1u<<bit));
  unsigned before=h->events[NST_ETHERNET],next=h->s.queue[0].next;struct nst_replay pair=h->s.ptk,group=h->s.gtk;
  deliver(h,0,bad,sizeof bad);advance(&x);errors++;
  C(h->s.data_diag.rejected[NSDD_RX_ERROR]==errors&&h->events[NST_ETHERNET]==before&&h->s.queue[0].next==(next+1)%h->s.queue[0].ring.count);
  C(!memcmp(&pair,&h->s.ptk,sizeof pair)&&!memcmp(&group,&h->s.gtk,sizeof group)&&h->s.controlled_port&&!h->s.halt);
 }
 cancel_done(&x);cleanup(h);free(h);
 printf("PASS %u assertions: actual secure RX reject reasons preserve admission, packet numbers and controlled port\n",checks);return 0;
}
