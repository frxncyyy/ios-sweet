#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "wifi_wpa2.h"
#include "wifi_wpa_diag.h"
#include "vectors.h"
static unsigned checks;
static int rng(void*c,uint8_t*p,size_t n){(void)c;assert(n==32);memcpy(p,sn,n);return 0;}
static void authorized(struct nwp*w){struct nwp_result r;assert(!nwp_begin(w,42,pmk_vector,own,peer,rsn,rsn,rng,0));assert(nwp_receive(w,m1,sizeof m1,&r)==NWP_TX);assert(!nwp_security_submitted(w,42,r.ticket,1));assert(nwp_receive(w,m3,sizeof m3,&r)==NWP_TX);uint64_t t=r.ticket;assert(nwp_security_submitted(w,42,t,1)==NWP_INSTALL);assert(nwp_keys_ready(w,42,t,7,1)==NWP_AUTH);checks+=6;}
static void wiped(const struct nwp*w){struct nwp z={0};assert(!memcmp(w,&z,sizeof z));checks++;}
int main(void){struct nwp w;struct nwp_result r;uint8_t p[sizeof m3];struct nwpd_budget b={0};struct nwpd_record d;
 authorized(&w);assert(nwp_receive(&w,m3,sizeof m3,&r)==NWP_TX&&!r.failure);assert(!nwp_security_submitted(&w,42,r.ticket,1)&&w.state==NWP_AUTHORIZED&&w.installed==1);checks+=2;
 authorized(&w);assert(nwp_receive(&w,m1,sizeof m1,&r)==NWP_FAIL&&r.failure==NWP_F_M1_STATE_REPLAY_NONCE);wiped(&w);checks++;
 authorized(&w);memcpy(p,m3,sizeof p);p[5]=0x13;p[6]=0x82;assert(nwp_receive(&w,p,sizeof p,&r)==NWP_FAIL&&r.failure==NWP_F_KEYINFO_STATE);wiped(&w);checks++;
 authorized(&w);memcpy(p,m3,sizeof p);p[81]^=1;assert(nwp_receive(&w,p,sizeof p,&r)==NWP_FAIL&&r.failure==NWP_F_MIC);wiped(&w);checks++;
 for(unsigned n=0;n<99;n++){authorized(&w);assert(nwp_receive(&w,m3,n,&r)==NWP_FAIL&&r.failure==NWP_F_ENVELOPE);wiped(&w);checks++;}
 /* Early flood and arbitrary external data diagnostics cannot spend late/fail reservations. */
 for(unsigned i=0;i<1000;i++)assert(nwpd_take(&b,&d,42,1,1,m1,sizeof m1,1,0)==(i<8));checks+=1000;
 for(unsigned i=0;i<1000;i++)assert(nwpd_take(&b,&d,42,1,5,m3,sizeof m3,1,0)==(i<24));checks+=1000;
 for(unsigned i=0;i<1000;i++)assert(nwpd_take(&b,&d,42,1,5,p,sizeof p,-1,7)==(i<8));checks+=1000;
 assert(b.serial==40&&b.early==8&&b.late==24&&b.failure==8);checks++;
 for(unsigned n=0;n<7;n++){struct nwpd_budget z={0};assert(nwpd_take(&z,&d,42,1,5,m1,n,0,0));assert(d.bytes==n);checks++;}
 struct nwpd_budget z={0};memset(&d,0xa5,sizeof d);struct nwpd_record old=d;assert(!nwpd_take(&z,&d,42,1,5,0,1,0,0)&&!memcmp(&old,&d,sizeof d));assert(!nwpd_take(&z,&d,42,1,5,m1,641,0,0));checks+=2;
 puts("PASS late-WPA real-vector retransmit/rekey/failure-zeroization and reserved diagnostic bounds");printf("checks=%u\n",checks);return 0;}
