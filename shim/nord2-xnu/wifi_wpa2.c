/* Private provider replacement. State machine unchanged from candidate135660fe;
 * all original Redwood arithmetic removed. Existing BearSSL-backed provider
 * supplies SHA1/HMAC/PBKDF2/PRF/AES-CT unwrap, with exact MIT provenance. */
#include "wifi_wpa2.h"
#include "wifi_wpa_crypto.h"
#if defined(KERNEL) || defined(_KERNEL)
#include <sys/systm.h>
#else
#include <string.h>
#endif
static void wipe(void*v,size_t n){volatile uint8_t*p=v;while(n--)*p++=0;}
static void raw_pmk(const uint8_t*p,unsigned pn,const uint8_t*s,unsigned sn,uint8_t out[32]){(void)nwpc_pmk(p,pn,s,sn,out);}
static void hmac_sha1(const uint8_t*k,unsigned kn,const uint8_t*const*p,const unsigned*n,unsigned count,uint8_t out[20]){
 size_t sizes[4];if(count>4){wipe(out,20);return;}for(unsigned i=0;i<count;i++)sizes[i]=n[i];(void)nwpc_hmac(k,kn,p,sizes,count,out);
}
static void prf(const uint8_t*k,unsigned kn,const char*label,const uint8_t*p,unsigned n,uint8_t*out,unsigned on){
 if(kn!=32||n!=76||on!=48||strcmp(label,"Pairwise key expansion")){wipe(out,on);return;}(void)nwpc_ptk(k,p,out);
}
static int aes_unwrap(const uint8_t*k,const uint8_t*p,unsigned n,uint8_t*out){size_t used=0;return nwpc_unwrap(k,p,n,out,NWP_FRAME,&used);}
/* ---- strict first WPA2-PSK/CCMP transaction, no rekey/roam/PMF ---- */
static unsigned be16(const uint8_t*p){return (unsigned)p[0]<<8|p[1];}
static void put16(uint8_t*p,unsigned v){p[0]=(uint8_t)(v>>8);p[1]=(uint8_t)v;}
static int equal(const uint8_t*a,const uint8_t*b,size_t n){unsigned d=0;for(size_t i=0;i<n;i++)d|=a[i]^b[i];return !d;}
static int nonzero(const uint8_t*a,size_t n){unsigned d=0;for(size_t i=0;i<n;i++)d|=a[i];return !!d;}
static int rsn_valid(const uint8_t*p){
 static const uint8_t rsn[22]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};
 return equal(p,rsn,sizeof rsn); /* exactly one CCMP pairwise/group, PSK; no PMF */
}
/* Bounded WPA2 selection from a full AP RSN IE. This driver selects PSK/CCMP,
 * never SAE or PMF. MFPC is an AP capability, MFPR requires unsupported PMF. */
static unsigned le16(const uint8_t*p){return p[0]|(unsigned)p[1]<<8;}
static int suite(const uint8_t*p,unsigned type){return p[0]==0&&p[1]==15&&p[2]==172&&p[3]==type;}
int nwp_ap_rsn(const uint8_t*p,size_t n){
 if(!p||n<20||n>257||p[0]!=48||p[1]+2u!=n||le16(p+2)!=1||!suite(p+4,4))return -1;
 size_t at=8;unsigned count=le16(p+at),ccmp=0,psk=0;at+=2;
 if(!count||count>(n-at)/4)return -1;
 for(unsigned i=0;i<count;i++,at+=4)ccmp|=suite(p+at,4);
 if(n-at<2)return -1;count=le16(p+at);at+=2;
 if(!count||count>(n-at)/4)return -1;
 for(unsigned i=0;i<count;i++,at+=4)psk|=suite(p+at,2);
 if(!ccmp||!psk)return -1;
 if(at==n)return 0;
 if(n-at<2)return -1;unsigned caps=le16(p+at);at+=2;if(caps&(64u|2u|8192u))return -1;
 if(at==n)return 0;
 if(n-at<2)return -1;count=le16(p+at);at+=2;
 if(count>(n-at)/16)return -1;at+=(size_t)count*16;
 /* Optional group-management cipher is metadata only for non-PMF selection. */
 if(n-at==4){if(!(caps&128u)||p[at]||p[at+1]!=15||p[at+2]!=172)return -1;at+=4;}
 return at==n?0:-1;
}
void nwp_cancel(struct nwp*w){if(w)wipe(w,sizeof(*w));}
static int fail(struct nwp*w,struct nwp_result*r){nwp_cancel(w);if(r)wipe(r,sizeof(*r));return NWP_FAIL;}
int nwp_pmk(const uint8_t*p,size_t pn,const uint8_t*s,size_t sn,uint8_t out[32]){
 if(!out)return -1;
 if(!p||!s||pn<8||pn>63||sn<1||sn>32){wipe(out,32);return -1;}
 raw_pmk(p,(unsigned)pn,s,(unsigned)sn,out);return 0;
}
int nwp_begin_ap(struct nwp*w,uint64_t gen,const uint8_t pmk[32],const uint8_t own[6],const uint8_t peer[6],const uint8_t rsn[22],const uint8_t *ap,size_t ap_bytes,nwp_rng rng,void*ctx){
 if(!w)return -1;
 nwp_cancel(w);
 if(!gen||!pmk||!own||!peer||!rsn||!ap||!rng||!rsn_valid(rsn)||nwp_ap_rsn(ap,ap_bytes)||
    !nonzero(own,6)||!nonzero(peer,6)||(own[0]&1)||(peer[0]&1)||equal(own,peer,6))return -1;
 w->generation=gen;memcpy(w->pmk,pmk,32);memcpy(w->own,own,6);memcpy(w->peer,peer,6);memcpy(w->rsn,rsn,22);memcpy(w->ap_rsn,ap,ap_bytes);w->ap_rsn_bytes=(unsigned)ap_bytes;
 if(rng(ctx,w->snonce,32)||!nonzero(w->snonce,32)){nwp_cancel(w);return -1;}
 w->state=NWP_M1;return 0;
}
int nwp_begin(struct nwp*w,uint64_t g,const uint8_t p[32],const uint8_t o[6],const uint8_t a[6],const uint8_t r[22],const uint8_t ar[22],nwp_rng rng,void*c){return nwp_begin_ap(w,g,p,o,a,r,ar,22,rng,c);}
static void derive_keys(struct nwp*w){
 uint8_t data[76],keys[48];int m=memcmp(w->own,w->peer,6)<0,n=memcmp(w->snonce,w->anonce,32)<0;
 memcpy(data,m?w->own:w->peer,6);memcpy(data+6,m?w->peer:w->own,6);
 memcpy(data+12,n?w->snonce:w->anonce,32);memcpy(data+44,n?w->anonce:w->snonce,32);
 prf(w->pmk,32,"Pairwise key expansion",data,76,keys,48);
 memcpy(w->kck,keys,16);memcpy(w->kek,keys+16,16);memcpy(w->tk,keys+32,16);wipe(keys,48);wipe(data,76);
}
static void mic(const struct nwp*w,const uint8_t*p,unsigned n,uint8_t*out){
 const uint8_t z[16]={0},*pieces[3]={p,z,p+97};unsigned sizes[3]={81,16,n-97};uint8_t d[20];
 hmac_sha1(w->kck,16,pieces,sizes,3,d);memcpy(out,d,16);wipe(d,20);
}
static int reply(struct nwp*w,const uint8_t*p,unsigned kind,struct nwp_result*r){
 if(w->serial==UINT64_MAX)return fail(w,r);
 unsigned n=kind==1?121:99;memset(r,0,sizeof(*r));r->frame[0]=p[0];r->frame[1]=3;put16(r->frame+2,n-4);r->frame[4]=2;
 put16(r->frame+5,kind==1?0x010a:0x030a);memcpy(r->frame+9,p+9,8);
 if(kind==1){memcpy(r->frame+17,w->snonce,32);put16(r->frame+97,22);memcpy(r->frame+99,w->rsn,22);}
 mic(w,r->frame,n,r->frame+81);r->action=NWP_TX;r->bytes=n;r->ticket=++w->serial;w->pending=r->ticket;w->tx_kind=kind;return NWP_TX;
}
static int kde(struct nwp*w,const uint8_t*p,unsigned n,const uint8_t*rsc){
 unsigned at=0,have_rsn=0,have_gtk=0;
 while(at<n){
  if(p[at]==0xdd&&(at+1==n||p[at+1]==0)){
   for(unsigned j=at+1;j<n;j++)if(p[j])return -1;
   break;
  }
  if(n-at<2||p[at+1]>n-at-2)return -1;
  unsigned len=p[at+1]+2;const uint8_t*e=p+at;
  if(e[0]==48){if(have_rsn++||len!=w->ap_rsn_bytes||!equal(e,w->ap_rsn,len))return -1;}
  else if(e[0]==221){
   static const uint8_t oui[4]={0,15,172,1};
   if(have_gtk++||len!=24||!equal(e+2,oui,4)||(e[6]&~7u)||e[7])return -1;
   w->gtk_index=e[6]&3;memcpy(w->gtk,e+8,16);memcpy(w->rsc,rsc,8);
  }else return -1;
  at+=len;
 }
 return have_rsn==1&&have_gtk==1?0:-1;
}
int nwp_receive(struct nwp*w,const uint8_t*p,size_t n,struct nwp_result*r){
 if(!r)return NWP_FAIL;
 memset(r,0,sizeof(*r));
 if(!w)return NWP_FAIL;
 if(w->state==NWP_OFF)return NWP_FAIL;
 if(!p||n<99||n>NWP_FRAME||p[0]<1||p[0]>2||p[1]!=3||be16(p+2)!=n-4||p[4]!=2||be16(p+97)!=n-99)return fail(w,r);
 unsigned info=be16(p+5);
 if(be16(p+7)!=16)return fail(w,r);
 if(info==0x008a){
  if(!nonzero(p+17,32)||be16(p+97))return fail(w,r);
  if(w->state!=NWP_M1){
   if(w->state!=NWP_M3||memcmp(p+9,w->m1_replay,8)<0||!equal(p+17,w->anonce,32))return fail(w,r);
   if(w->pending)return NWP_BUSY;
   memcpy(w->m1_replay,p+9,8);
   return reply(w,p,1,r); /* Same ANonce, same SNonce/PTK; monotonic retransmit counter. */
  }
  if(w->pending)return NWP_BUSY;
  memcpy(w->anonce,p+17,32);memcpy(w->m1_replay,p+9,8);derive_keys(w);w->state=NWP_M3;
  return reply(w,p,1,r);
 }
 if(info!=0x13ca||w->state<NWP_M3)return fail(w,r);
 uint8_t signature[16];mic(w,p,(unsigned)n,signature);int ok=equal(signature,p+81,16);wipe(signature,16);
 if(!ok||!equal(p+17,w->anonce,32))return fail(w,r);
 if(w->m3_bytes){
  /* AP retries may use a higher replay counter. All authenticated key/RSN/RSC
   * contents must remain identical; never reinstall/reset any packet number. */
  if(n!=w->m3_bytes||memcmp(p+9,w->m3+9,8)<0)return fail(w,r);
  for(unsigned i=0;i<n;i++)if(!((i>=9&&i<17)||(i>=81&&i<97))&&p[i]!=w->m3[i])return fail(w,r);
  if(w->pending)return NWP_BUSY;
  memcpy(w->m3,p,n);
  return reply(w,p,3,r);
 }
 if(w->state!=NWP_M3||memcmp(p+9,w->m1_replay,8)<=0)return fail(w,r);
 if(w->pending)return NWP_BUSY;
 uint8_t plain[NWP_FRAME];unsigned wrapped=be16(p+97);
 if(aes_unwrap(w->kek,p+99,wrapped,plain)||kde(w,plain,wrapped-8,p+65)){wipe(plain,sizeof plain);return fail(w,r);}
 wipe(plain,sizeof plain);memcpy(w->m3,p,n);w->m3_bytes=(unsigned)n;w->state=NWP_M4_TX;
 return reply(w,p,2,r);
}
int nwp_tx_done(struct nwp*w,uint64_t gen,uint64_t ticket,int success){
 if(!w||!ticket||gen!=w->generation||ticket!=w->pending)return NWP_DROP;
 unsigned kind=w->tx_kind;w->pending=0;w->tx_kind=0;
 if(!success){nwp_cancel(w);return NWP_FAIL;}
 if((kind==2||kind==3)&&w->state==NWP_M4_TX&&!w->installed){w->installed=1;w->install_ticket=ticket;w->state=NWP_KEYS;return NWP_INSTALL;}
 return NWP_DROP;
}
int nwp_keys_ready(struct nwp*w,uint64_t gen,uint64_t ticket,unsigned mask,int success){
 if(!w||gen!=w->generation||!ticket||ticket!=w->install_ticket||w->state!=NWP_KEYS||!w->installed)return NWP_DROP;
 if(!mask||(mask&~7u)||success!=1){nwp_cancel(w);return NWP_FAIL;}
 w->ready_mask|=mask;
 if(w->ready_mask!=7)return NWP_DROP;
 w->state=NWP_AUTHORIZED;return NWP_AUTH;
}
