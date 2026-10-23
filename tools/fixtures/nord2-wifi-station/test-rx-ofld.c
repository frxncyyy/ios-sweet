/* Actual native RX through the synthetic DMA model. No hardware or DHCP claim. */
#define NST_TEST_NO_MAIN 1
#include "transport-hardware.inc"

/* Build a translated CCMP unicast-to-self data frame identical to the canonical
 * accepted template, with optional OFLD and optional hard ICV error. groups=9
 * (GROUP1 CCMP-PN @ +40, GROUP4 802.11 hdr @ +24); Ethernet payload at +56. */
static unsigned uc_ccmp(struct test*t,uint8_t*p,unsigned pn,unsigned ofld,unsigned icv){
 unsigned n=116;memset(p,0,n);put16(p,n);put16(p+2,2u<<11);
 put32(p+4,5u|(9u<<11)|(4u<<16)|(0u<<21)|(icv?(1u<<25):0u)); /* wlan5,sec CCMP,key0, opt ICV err (DW1 bit25) */
 put32(p+8,1u<<13);           /* translated */
 put32(p+12,6u<<8);           /* header length word */
 put32(p+16,ofld?(3u<<11):0u);/* DW4 OFLD=3 when requested */
 put16(p+24,0x4208);memcpy(p+26,t->s.peer.ap,6);p[40]=(uint8_t)pn; /* fc + A2 + CCMP PN low */
 uint8_t*m=p+56;memcpy(m,t->s.peer.self,6);m[6]=2;m[11]=7;m[12]=8;m[13]=0; /* DA=self, EtherType 0x0800 */
 return n;
}
static void ready(struct test*t){
 setup(t);t->s.controlled_port=1;t->s.ptk.installed=1;t->s.ptk.key_id=0;
 t->s.gtk.installed=1;t->s.gtk.key_id=2;t->s.gtk_keys[2]=t->s.gtk;t->s.gtk_wlan[2]=t->s.peer.group_wlan;t->s.gtk_mapped=1u<<2;
}

static void ofld_discrimination(void){
 struct test*t=calloc(1,sizeof *t);assert(t);
 uint8_t p[128];unsigned err=0;

 /* A clean frame validates the template before exercising OFLD and errors. */
 ready(t);unsigned eth=t->events[NST_ETHERNET];
 unsigned n=uc_ccmp(t,p,1,0,0);deliver(t,0,p,n);step(t);
 C(t->events[NST_ETHERNET]==eth+1);C(t->s.data_diag.rx_unicast>=1);cleanup(t);

 /* A real ICV error must still be rejected. */
 ready(t);err=t->s.data_diag.rejected[NSDD_RX_ERROR];eth=t->events[NST_ETHERNET];
 n=uc_ccmp(t,p,2,0,1);deliver(t,0,p,n);step(t);
 C(t->events[NST_ETHERNET]==eth);C(t->s.data_diag.rejected[NSDD_RX_ERROR]==err+1);cleanup(t);

 /* OFLD metadata alone must not drop valid protected data. The bounded
  * descriptor observation remains separate from the error-drop counter. */
 ready(t);err=t->s.data_diag.rejected[NSDD_RX_ERROR];eth=t->events[NST_ETHERNET];
 unsigned rxe=t->s.data_diag.rx_error_logs;
 n=uc_ccmp(t,p,3,1,0);deliver(t,0,p,n);step(t);
 C(t->s.data_diag.rx_error_logs==rxe+1); /* bounded descriptor observation */
 C(t->events[NST_ETHERNET]==eth+1);
 C(t->s.data_diag.rejected[NSDD_RX_ERROR]==err); /* not an error drop any more */
 printf("NORD2 OFLD-RX: candidate_routed=1 delivered=1 rx_error_drop_delta=0 breadcrumb=1\n");

 cleanup(t);

 /* The same OFLD frame must fail when its installed key does not match. */
 ready(t);t->s.ptk.key_id=1;eth=t->events[NST_ETHERNET];
 unsigned keyrej=t->s.data_diag.rejected[NSDD_RX_KEY];
 n=uc_ccmp(t,p,4,1,0);deliver(t,0,p,n);step(t);
 C(t->events[NST_ETHERNET]==eth);
 C(t->s.data_diag.rejected[NSDD_RX_KEY]==keyrej+1); /* reached and rejected by key gate */

 cleanup(t);

 free(t);
 printf("PASS ofld-rx: clean-unicast delivered, ICV-error dropped, OFLD %s, key gate preserved (synthetic model, no hardware/DHCP claim)\n",
        "routed-through-secure-decode");
}

static unsigned expanded(struct test*t,uint8_t*p,unsigned groups,unsigned pad){
 memset(p,0,512);unsigned off=24,g4=0,g1=0;
 if(groups&8){g4=off;off+=16;}if(groups&1){g1=off;off+=16;}
 if(groups&2)off+=8;if(groups&4)off+=8;if(groups&16)off+=72;off+=pad*2;
 unsigned n=off+60;put16(p,n);put16(p+2,2u<<11);
 put32(p+4,5u|(groups<<11)|(4u<<16));put32(p+8,(1u<<13)|(pad<<14));put32(p+12,6u<<8);put32(p+16,3u<<11);
 if(g4){put16(p+g4,0x4208);memcpy(p+g4+2,t->s.peer.ap,6);}if(g1)p[g1]=1;
 uint8_t*m=p+off;memcpy(m,t->s.peer.self,6);m[6]=2;m[11]=7;m[12]=8;m[13]=0;return n;
}
static unsigned direct(struct test*t,const uint8_t*p,unsigned n){unsigned before=t->events[NST_ETHERNET];deliver(t,0,p,n);step(t);return t->events[NST_ETHERNET]>before?NST_ETHERNET:0;}
int main(void){ofld_discrimination();struct test*t=calloc(1,sizeof*t);assert(t);uint8_t p[512];unsigned total=0;
 /* Every group mask and legal header-offset value. Required group4 AP header
  * and group1 PN must both exist; actual mask15 and extended mask31 included. */
 for(unsigned g=0;g<32;g++)for(unsigned pad=0;pad<4;pad++){
  ready(t);unsigned n=expanded(t,p,g,pad);C((direct(t,p,n)==NST_ETHERNET)==((g&9)==9));total++;cleanup(t);
 }
 /* Actual receive/rearm/callback path for observed groups15 and every offset. */
 for(unsigned pad=0;pad<4;pad++){
  ready(t);unsigned n=expanded(t,p,15,pad),before=t->events[NST_ETHERNET];deliver(t,0,p,n);step(t);
  C(t->events[NST_ETHERNET]==before+1);total++;cleanup(t);
 }
 /* Physical truncations under a valid full declared length never deliver. */
 ready(t);unsigned n=expanded(t,p,31,3);cleanup(t);
 for(unsigned k=0;k<n;k++){ready(t);expanded(t,p,31,3);C(direct(t,p,k)!=NST_ETHERNET);total++;cleanup(t);}
 /* Security/header guards reached after OFLD. No candidate bypass. */
 for(unsigned mode=0;mode<12;mode++){
  ready(t);n=expanded(t,p,15,2);unsigned off=n-60;
  if(mode==0)t->s.controlled_port=0;
  if(mode==1)t->s.ptk.installed=0;
  if(mode==2)t->s.ptk.key_id=1;
  if(mode==3)put32(p+4,6u|(15u<<11)|(4u<<16));
  if(mode==4)p[26]^=1; /* group4 AP identity */
  if(mode==5)p[off]^=2; /* destination ownership */
  if(mode==6)put16(p+24,0x0208); /* missing Protected flag */
  if(mode==7)put32(p+4,5u|(15u<<11)); /* plaintext ordinary data */
  if(mode==8)p[40]=0; /* missing PN */
  if(mode==9)t->s.ptk.pn[16]=1; /* replay */
  if(mode==10)put32(p+8,(1u<<13)|(2u<<14)|(1u<<25)); /* TRANS_FAIL */
  if(mode==11)put16(p+32,1); /* fragment */
  C(direct(t,p,n)!=NST_ETHERNET);total++;cleanup(t);
 }
 free(t);printf("PASS independent OFLD %u group-layout/truncation/security cases\n",total);return 0;}
