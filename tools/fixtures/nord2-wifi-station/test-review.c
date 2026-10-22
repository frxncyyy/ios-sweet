#define main old_model_main
#include "model-test.c"
#undef main
#include "review-vectors.h"
static void start(struct nwp*w){assert(!nwp_begin(w,42,pmk_vector,own,peer,rsn,rsn,rng,0));}
static int first3(struct nwp*w,const uint8_t*p,size_t n,struct nwp_result*r){assert(nwp_receive(w,m1,sizeof m1,r)==NWP_TX);assert(!nwp_tx_done(w,42,r->ticket,1));return nwp_receive(w,p,n,r);}
int main(void){
 struct nwp w;struct nwp_result r,base;start(&w);assert(nwp_receive(&w,m1,sizeof m1,&base)==NWP_TX);
 uint8_t p[160];memcpy(p,m1,sizeof m1);p[2]=0;p[3]=117;p[97]=0;p[98]=22;p[99]=221;p[100]=20;p[101]=0;p[102]=15;p[103]=172;p[104]=4;for(unsigned i=105;i<121;i++)p[i]=(uint8_t)i;
 start(&w);assert(nwp_receive(&w,p,121,&r)==NWP_TX&&r.bytes==base.bytes&&!memcmp(r.frame,base.frame,r.bytes));assert(!nwp_tx_done(&w,42,r.ticket,1));assert(nwp_receive(&w,p,121,&r)==NWP_TX);nwp_cancel(&w);checks+=3;
 for(unsigned n=2;n<22;n++){start(&w);p[3]=(uint8_t)(95+n);p[98]=(uint8_t)n;assert(nwp_receive(&w,p,99+n,&r)==NWP_FAIL&&!w.state);checks++;}p[3]=117;p[98]=22;
 for(unsigned off=100;off<=104;off++){start(&w);p[off]^=1;assert(nwp_receive(&w,p,121,&r)==NWP_FAIL);p[off]^=1;checks++;}
 start(&w);p[121]=221;p[122]=0;p[123]=0;p[3]=120;p[98]=25;assert(nwp_receive(&w,p,124,&r)==NWP_TX);start(&w);p[123]=1;assert(nwp_receive(&w,p,124,&r)==NWP_FAIL);checks+=2;
 start(&w);memcpy(p+121,p+99,22);p[3]=139;p[98]=44;assert(nwp_receive(&w,p,143,&r)==NWP_FAIL);checks++;
 start(&w);assert(!nwp_copy_ap_rsnxe(&w,rxse,sizeof rxse));assert(first3(&w,good_rxse,sizeof good_rxse,&r)==NWP_TX);uint64_t ticket=r.ticket;assert(nwp_tx_done(&w,42,ticket,1)==NWP_INSTALL);assert(nwp_receive(&w,good_rxse,sizeof good_rxse,&r)==NWP_TX);assert(!nwp_tx_done(&w,42,r.ticket,1)&&w.installed==1);checks+=5;
 const uint8_t*bad[]={bad_rxse,duplicate_rxse,unknown_rsn_kde};const size_t lens[]={sizeof bad_rxse,sizeof duplicate_rxse,sizeof unknown_rsn_kde};for(unsigned i=0;i<3;i++){start(&w);assert(!nwp_copy_ap_rsnxe(&w,rxse,sizeof rxse));assert(first3(&w,bad[i],lens[i],&r)==NWP_FAIL&&!w.state);checks++;}
 start(&w);assert(first3(&w,good_rxse,sizeof good_rxse,&r)==NWP_FAIL);start(&w);assert(!nwp_copy_ap_rsnxe(&w,rxse,sizeof rxse));assert(first3(&w,m3,sizeof m3,&r)==NWP_FAIL);start(&w);assert(first3(&w,vendor_m3,sizeof vendor_m3,&r)==NWP_TX);checks+=3;
 for(unsigned z=0;z<sizeof rxse;z++){start(&w);assert(nwp_copy_ap_rsnxe(&w,rxse,z)<0||z==0);checks++;}uint8_t dup[6];memcpy(dup,rxse,3);memcpy(dup+3,rxse,3);start(&w);assert(nwp_copy_ap_rsnxe(&w,dup,6)<0);checks++;
 assert(!nwp_begin_ap(&w,42,pmk_vector,own,peer,rsn,extended_rsn,sizeof extended_rsn,rng,0));assert(first3(&w,extended_m3,sizeof extended_m3,&r)==NWP_TX);checks++;
 uint8_t ap[22];memcpy(ap,rsn,22);ap[21]=32;assert(!nwp_ap_rsn(ap,22));ap[20]|=64;assert(nwp_ap_rsn(ap,22)<0);checks+=2;
 /* Fixed auth fields followed by bounded legal IEs; invalid TLV never advances. */
 for(unsigned variant=0;variant<4;variant++){struct stc s={0};struct stc_arbiter a={0};struct stc_config c=cfg();assert(stc_begin(&s,&a,&c,0)==1);ack(&s);ack(&s);assert(stc_grant(&s,42,0,1,1,1,1,1)==1);ack(&s);uint8_t auth[36]={0};auth[0]=0xb0;memcpy(auth+4,own,6);memcpy(auth+10,peer,6);memcpy(auth+16,peer,6);auth[26]=2;auth[30]=221;auth[31]=4;auth[32]=0;auth[33]=1;auth[34]=2;auth[35]=3;size_t n=36;if(variant==1)n=31;if(variant==2)auth[31]=5;if(variant==3)auth[26]=3;(void)stc_management(&s,42,auth,n,1);assert((s.phase==STC_ASSOC_TX)==(variant==0));assert(stc_cancel(&s,42,1)>=0);cleanup(&s);checks++;}
 /* No retransmit before AIR result; one retry with new ticket within grant. */
 for(unsigned assoc=0;assoc<2;assoc++){struct stc s={0};struct stc_arbiter a={0};struct stc_config c=cfg();assert(stc_begin(&s,&a,&c,0)==1);ack(&s);ack(&s);assert(stc_grant(&s,42,0,1,1,1,1,1)==1);uint64_t old=s.action.ticket;assert(!stc_tick(&s,1)&&s.action.ticket==old);ack(&s);if(assoc){response(&s,0);ack(&s);}uint64_t due=s.response_deadline;assert(!stc_tick(&s,due-1));assert(stc_tick(&s,due)==1&&s.action.ticket!=old&&s.action.kind==(assoc?STC_TX_ASSOC:STC_TX_AUTH));ack(&s);due=s.response_deadline;assert(stc_tick(&s,due)==1&&s.outcome==STC_TIMEOUT&&s.action.kind==STC_ABORT_CHANNEL);cleanup(&s);checks+=5;}
 {struct stc s={0};struct stc_arbiter a={0};struct stc_config c=cfg();c.deadline_ms=520;assert(stc_begin(&s,&a,&c,0)==1);ack(&s);ack(&s);assert(stc_grant(&s,42,0,1,1,1,1,1)==1);ack(&s);assert(stc_tick(&s,520)==1&&s.outcome==STC_TIMEOUT&&s.auth_attempts==1);cleanup(&s);checks++;}
 printf("PASS %u focused WPA PMKID/RSNXE/vendor/ExtendedKeyID/auth-TLV/retry checks; synthetic only\n",checks);return 0;
}
