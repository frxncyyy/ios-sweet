#include "wifi_wpa2.h"
#include "wifi_wpa_crypto.h"
#include "vectors.h"
#include "gtk-vectors.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
static unsigned checks;
#define C(x) do{checks++;assert(x);}while(0)
static int rng(void*c,uint8_t*p,size_t n){(void)c;assert(n==32);memcpy(p,sn,n);return 0;}
static void authorized(struct nwp*w){struct nwp_result r;memset(w,0,sizeof *w);C(!nwp_begin(w,17,pmk_vector,own,peer,rsn,rsn,rng,0));C(nwp_receive(w,m1,sizeof m1,&r)==NWP_TX);C(nwp_security_submitted(w,17,r.ticket,1)==NWP_DROP);C(nwp_receive(w,m3,sizeof m3,&r)==NWP_TX);C(nwp_security_submitted(w,17,r.ticket,1)==NWP_INSTALL);C(nwp_keys_ready(w,17,w->install_ticket,7,1)==NWP_AUTH);}
static void zeroed(struct nwp*w){struct nwp z={0};C(!memcmp(w,&z,sizeof z));}
static void sign_frame(uint8_t*p,size_t n){uint8_t digest[20];memset(p+81,0,16);const uint8_t*parts[]={p};size_t bytes[]={n};C(!nwpc_hmac(ptk,16,parts,bytes,1,digest));memcpy(p+81,digest,16);memset(digest,0,sizeof digest);}
static void installed(struct nwp*w,const uint8_t*p,size_t n){struct nwp_result r;C(nwp_receive(w,p,n,&r)==NWP_INSTALL);C(w->group_phase==1&&w->state==NWP_AUTHORIZED&&r.ticket==w->group_ticket);uint64_t ticket=r.ticket;C(nwp_group_ready(w,18,ticket,1,&r)==NWP_DROP);C(nwp_group_ready(w,17,ticket+1,1,&r)==NWP_DROP);C(nwp_group_ready(w,17,ticket,1,&r)==NWP_TX);C(w->group_phase==2&&r.bytes==99&&r.frame[5]==3&&r.frame[6]==2&&!r.frame[7]&&!r.frame[8]);C(nwp_security_submitted(w,17,r.ticket,1)==NWP_DROP);C(w->state==NWP_AUTHORIZED&&!w->group_phase&&!w->pending);}
int main(void){struct nwp w;struct nwp_result r;
 authorized(&w);C(nwp_receive(&w,g1_0,sizeof g1_0,&r)==NWP_INSTALL);uint64_t install=r.ticket;C(nwp_receive(&w,g1_0,sizeof g1_0,&r)==NWP_BUSY&&w.group_ticket==install);C(nwp_group_ready(&w,17,install,1,&r)==NWP_TX);C(!memcmp(r.frame,g2_oracle,sizeof g2_oracle));uint64_t ack=r.ticket;C(nwp_receive(&w,g1_0,sizeof g1_0,&r)==NWP_BUSY);C(nwp_security_submitted(&w,17,ack,1)==NWP_DROP);C(nwp_receive(&w,g1_0,sizeof g1_0,&r)==NWP_TX);C(nwp_security_submitted(&w,17,r.ticket,1)==NWP_DROP);C(w.gtk_history_count==2);
 C(nwp_receive(&w,g1_same_newcounter,sizeof g1_same_newcounter,&r)==NWP_TX);C(w.gtk_history_count==2);C(nwp_security_submitted(&w,17,r.ticket,1)==NWP_DROP);C(nwp_receive(&w,g1_0,sizeof g1_0,&r)==NWP_FAIL);zeroed(&w);
 const uint8_t*bad[]={g1_bad_reserved,g1_bad_duplicate,g1_bad_unknown,g1_bad_badrsn,g1_bad_wrap,g1_bad_rsc};const size_t sizes[]={sizeof g1_bad_reserved,sizeof g1_bad_duplicate,sizeof g1_bad_unknown,sizeof g1_bad_badrsn,sizeof g1_bad_wrap,sizeof g1_bad_rsc};
 for(unsigned k=0;k<6;k++){authorized(&w);C(nwp_receive(&w,bad[k],sizes[k],&r)==NWP_FAIL);zeroed(&w);}
 for(unsigned i=0;i<sizeof g1_0;i++)for(unsigned bit=0;bit<8;bit++){uint8_t p[sizeof g1_0];memcpy(p,g1_0,sizeof p);p[i]^=1u<<bit;authorized(&w);C(nwp_receive(&w,p,sizeof p,&r)==NWP_FAIL);zeroed(&w);}
 for(unsigned i=0;i<sizeof g1_0;i++){authorized(&w);C(nwp_receive(&w,g1_0,i,&r)==NWP_FAIL);zeroed(&w);}
 /* RSN Group1 KeyLength is inactive; authenticated uint16 values do not
  * select a cipher or key size. Exact16-byte KDE still controls it. */
 for(unsigned k=0;k<65536;k++){uint8_t p[sizeof g1_0];memcpy(p,g1_0,sizeof p);p[7]=k>>8;p[8]=k;sign_frame(p,sizeof p);authorized(&w);C(nwp_receive(&w,p,sizeof p,&r)==NWP_INSTALL);C(w.gtk_index==2);nwp_cancel(&w);}
 authorized(&w);installed(&w,g1_0,sizeof g1_0);C(nwp_receive(&w,g1_same_otherindex,sizeof g1_same_otherindex,&r)==NWP_FAIL);zeroed(&w);
 authorized(&w);installed(&w,g1_0,sizeof g1_0);C(nwp_receive(&w,g1_oldkey,sizeof g1_oldkey,&r)==NWP_FAIL);zeroed(&w);
 authorized(&w);C(nwp_receive(&w,g1_0,sizeof g1_0,&r)==NWP_INSTALL);C(nwp_group_ready(&w,17,r.ticket,0,&r)==NWP_FAIL);zeroed(&w);
 authorized(&w);C(nwp_receive(&w,g1_0,sizeof g1_0,&r)==NWP_INSTALL);C(nwp_group_ready(&w,17,r.ticket,1,&r)==NWP_TX);C(nwp_security_submitted(&w,17,r.ticket,0)==NWP_FAIL);zeroed(&w);
 authorized(&w);w.serial=UINT64_MAX;C(nwp_receive(&w,g1_0,sizeof g1_0,&r)==NWP_FAIL);zeroed(&w);
 const uint8_t*all[]={g1_0,g1_1,g1_2,g1_3,g1_4,g1_5,g1_6,g1_7,g1_8,g1_9,g1_10,g1_11,g1_12,g1_13,g1_14,g1_15,g1_16,g1_17,g1_18,g1_19,g1_20,g1_21,g1_22,g1_23,g1_24,g1_25,g1_26,g1_27,g1_28,g1_29,g1_30,g1_31};
 authorized(&w);for(unsigned k=0;k<31;k++)installed(&w,all[k],sizeof g1_0);C(w.gtk_history_count==32);C(nwp_receive(&w,all[31],sizeof g1_0,&r)==NWP_FAIL);zeroed(&w);
 printf("PASS %u GTK engine assertions: real MIC/AES-KW, all inactive lengths, replay/duplicates, integrity/KDE failures, rollback and finite secret history; synthetic only\n",checks);return 0;}
