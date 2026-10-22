#define NST_TEST_NO_MAIN 1
#include "transport-hardware.inc"
static void scan_admission(struct test*t){
 setup(t);struct nsb_scan good=t->rx.scan;struct nsb_scan bad;
 C(nst_scan_admissible(&good));C(!nst_scan_admissible(NULL));
 good.partial=1;good.dropped=1;good.count=NSB_RESULTS;good.frames=NSB_RESULTS+1;
 C(nst_scan_admissible(&good));
 t->rx.scan=good;struct nst_peer peer=t->s.peer;struct sw_txd txd=t->s.txd;struct ncs_cap cap={.bss=4,.wtbl=32,.bmc=4,.own_mac=1};memset(&t->s,0,sizeof t->s);
 C(!nst_prepare(&t->s,&t->init,&t->prior,&t->rx,&cap,&peer,&txd,17,t->now,t->now+30000000));
 for(unsigned k=0;k<14;k++){
  bad=good;switch(k){case 0:bad.entered=0;break;case 1:bad.done=0;break;case 2:bad.active=1;break;case 3:bad.timed_out=1;break;case 4:bad.completion.state=6;break;case 5:bad.completion.sequence++;break;case 6:bad.count++;break;case 7:bad.frames=0;break;case 8:bad.started=1;break;case 9:bad.deadline=bad.started;break;case 10:bad.dropped=0;break;case 11:bad.dropped=2;break;case 12:bad.last=bad.deadline;break;default:bad.partial=0;}
  C(!nst_scan_admissible(&bad));t->rx.scan=bad;memset(&t->s,0,sizeof t->s);struct nst before=t->s;
  C(nst_prepare(&t->s,&t->init,&t->prior,&t->rx,&cap,&peer,&txd,17,t->now,t->now+30000000)<0);C(!memcmp(&before,&t->s,sizeof before));
 }
 bad=good;bad.count=bad.frames=NSB_RESULTS-1;C(!nst_scan_admissible(&bad));cleanup(t);
}
static void actual_scan_overflow(void){
 struct nsb_scan s={0};uint8_t p[65]={0},done[NSB_SCAN_DONE_BYTES]={0};
 C(!nsb_begin(&s,42,100,100000));put16(p,sizeof p);put16(p+2,0x3801);put32(p+8,12u<<8);put32(p+12,6u<<8);uint8_t*m=p+24;
 put16(m,0x80);m[10]=m[16]=2;put16(m+32,100);m[36]=0;m[37]=3;memcpy(m+38,"lab",3);
 for(unsigned k=0;k<NSB_RESULTS;k++){m[15]=m[21]=(uint8_t)(k+1);C(!nsb_add(&s,p,sizeof p,101+k));}
 struct nsb_record retained[NSB_RESULTS];memcpy(retained,s.records,sizeof retained);
 m[15]=m[21]=99;C(nsb_add(&s,p,sizeof p,120)==1);C(s.partial&&s.dropped==1&&s.count==NSB_RESULTS);C(!memcmp(retained,s.records,sizeof retained));
 C(!nst_scan_admissible(&s));done[0]=42;done[5]=7;C(!nsb_complete(&s,done,sizeof done,130));C(nst_scan_admissible(&s));
 s.completion.state=6;C(!nst_scan_admissible(&s));s.completion.state=7;s.timed_out=1;C(!nst_scan_admissible(&s));
}
static unsigned raw_eapol(struct test*t,uint8_t*p,unsigned payload){
 unsigned n=24+24+8+4+payload;memset(p,0,n);put16(p,n);put16(p+2,2u<<11);put32(p+4,t->s.peer.wlan);put32(p+8,12u<<8);
 uint8_t*m=p+24;put16(m,0x0208);memcpy(m+4,t->s.peer.self,6);memcpy(m+10,t->s.peer.ap,6);memcpy(m+16,t->s.peer.ap,6);
 const uint8_t snap[8]={0xaa,0xaa,3,0,0,0,0x88,0x8e};memcpy(m+24,snap,8);m[32]=2;m[33]=3;m[34]=(uint8_t)(payload>>8);m[35]=(uint8_t)payload;return n;
}
static void raw_tests(struct test*t){
 setup(t);uint8_t p[1600],good[1600];unsigned n=raw_eapol(t,good,4),accepted=0;
 deliver(t,0,good,n);step(t);C(t->events[NST_EAPOL]==++accepted);C(t->s.deferred.bytes==22);C(!memcmp(t->s.deferred.body,t->s.peer.self,6));C(!memcmp(t->s.deferred.body+6,t->s.peer.ap,6));C(t->s.deferred.body[12]==0x88&&t->s.deferred.body[13]==0x8e);C(!memcmp(t->s.deferred.body+14,good+56,8));
 C(t->s.rx_untranslated==1&&t->s.rx_untranslated_eapol==1&&!t->s.rx_untranslated_drop);C(!t->s.controlled_port);
 for(unsigned k=0;k<19;k++){
  memcpy(p,good,n);switch(k){case 0:p[28]^=1;break;case 1:p[34]^=2;break;case 2:p[40]^=2;break;case 3:p[4]=4;break;case 4:p[25]|=0x40;break;case 5:p[24]|=0x80;break;case 6:p[25]|=4;break;case 7:p[46]|=1;break;case 8:p[25]|=0x80;break;case 9:p[25]&=~2;break;case 10:p[25]|=1;break;case 11:p[48]^=1;break;case 12:p[54]^=1;break;case 13:p[59]++;break;case 14:p[6]=4;break;case 15:p[9]=11;break;case 16:p[7]|=2;break;case 17:p[11]|=8;break;case 18:p[16]|=1;}
  deliver(t,0,p,n);step(t);C(t->events[NST_EAPOL]==accepted);C(!t->s.controlled_port&&!t->s.ptk.installed&&!t->s.gtk.installed);C(!memcmp(t->s.rx_frame,p,n));
 }
 for(unsigned len=0;len<n;len++){deliver(t,0,good,len);step(t);C(t->events[NST_EAPOL]==accepted);}
 n=raw_eapol(t,p,1496);C(n==1556);deliver(t,0,p,n);step(t);C(t->events[NST_EAPOL]==++accepted);C(t->s.deferred.bytes==NST_MAX_ETHERNET);
 n=raw_eapol(t,p,1497);deliver(t,0,p,n);step(t);C(t->events[NST_EAPOL]==accepted);
 /* Retry, power-management and MoreData bits do not change the bounded shape. */
 n=raw_eapol(t,p,0);p[25]|=0x38;deliver(t,0,p,n);step(t);C(t->events[NST_EAPOL]==++accepted);
 C(t->s.rx_untranslated_eapol==accepted);C(t->s.rx_untranslated>=t->s.rx_untranslated_eapol+t->s.rx_untranslated_drop);cleanup(t);
}
static void group_tests(struct test*t){
 setup(t);t->s.controlled_port=1;t->s.gtk.installed=t->s.ptk.installed=1;t->s.gtk.key_id=2;unsigned received=0;
 for(unsigned k=0;k<12;k++){
  uint8_t p[116]={0};put16(p,sizeof p);put16(p+2,2u<<11);put32(p+4,5u|(9u<<11)|(4u<<16)|(2u<<21));put32(p+8,1u<<13);put32(p+12,6u<<8);put16(p+24,0x4208);memcpy(p+26,t->s.peer.ap,6);p[40]=(uint8_t)(k+1);uint8_t*m=p+56;memset(m,255,6);m[6]=2;m[11]=7;m[12]=8;
  switch(k){case 1:p[4]=4;break;case 2:p[4]=6;break;case 3:memcpy(m,t->s.peer.self,6);p[4]=4;break;case 4:p[26]^=2;break;case 5:p[6]^=0x20;break;case 6:p[40]=2;break;case 7:p[25]&=~0x40;break;case 8:p[7]|=2;break;case 9:p[6]^=1;break;case 10:p[24]|=0x80;p[10]=3;break;case 11:p[24]|=0x80;p[10]=3;break;}
  uint64_t before=t->s.gtk.pn[16];deliver(t,0,p,sizeof p);step(t);
  if(k<2||k>=10)received++;else C(t->s.gtk.pn[16]==before);
  C(t->events[NST_ETHERNET]==received);C(t->s.gtk.installed&&t->s.controlled_port);
 }
 C(t->s.gtk.pn[16]==2&&t->s.gtk.pn[3]==12);cleanup(t);
}
static void actual_data_route(struct test*t){
 setup(t);t->s.controlled_port=1;uint8_t eth[60]={0};memcpy(eth+6,t->s.peer.self,6);memset(eth,255,6);eth[12]=8;
 C(!nst_data(&t->s,77,eth,sizeof eth));t->defer_air=1;step(t);C(t->published==1&&t->s.dma_done&&!t->events[NST_FRAME_RESULT]);
 C(((get32(t->capture)>>23)&3)==2);C((get32(t->capture)>>25)==1);C((get32(t->capture+4)&0x8400)==0x8400);C(!(get32(t->capture+4)&(3u<<16)));C(get32(t->capture+12)&2);C(!memcmp(t->capture+32,eth,sizeof eth));
 txdone(t,t->s.wire_pid,t->s.peer.wlan,0);step(t);C(t->events[NST_FRAME_RESULT]==0&&t->events[NST_DATA_SUBMITTED]==1&&!t->last_status);cleanup(t);
}
int main(void){struct test*t=calloc(1,sizeof *t);assert(t);scan_admission(t);actual_scan_overflow();raw_tests(t);group_tests(t);actual_data_route(t);free(t);printf("PASS %u focused assertions: overflow-only prepare atomicity, raw EAPOL bounds/ownership, group AP/BMC key/PN and actual data route\n",checks);return 0;}
