/* Original staged pre-root experiment. Selected Linux ufshcd.c:4125,4449;
 * ufs-mediatek.c:1462; unipro.h attributes/timeouts. No live-kernel hook. */
#include "ufs_two_lane.h"
#define IS 0x20u
#define HCS 0x30u
#define CMD 0x90u
#define ARG1 0x94u
#define ARG2 0x98u
#define ARG3 0x9cu
#define UCC 0x400u
#define UPM 0x10u
#define SNAP 18u
struct run { const struct n2l_ops *o;void *c;struct n2l_result *r;uint64_t start; };
/* RXgear,activeRX,terminationRX,TXgear,activeTX,terminationTX,HSseries,
 * PWRMODE,TXHSADAPT, six PA user-data timeouts, three local timeouts. */
static const uint16_t attrs[SNAP]={0x1583,0x1580,0x1584,0x1568,0x1560,0x1569,
 0x156a,0x1571,0x15d4,0x15b0,0x15b1,0x15b2,0x15b3,0x15b4,0x15b5,0xd041,0xd042,0xd043};
static const uint16_t caps[6]={0x1540,0x1520,0x1581,0x1561,0x15a9,0x1587};
static const uint32_t timeout_values[9]={8191,65535,32767,8191,65535,32767,8191,65535,32767};
static uint32_t rd(struct run *s,uint32_t a){return s->o->read32(s->c,a);}
static void wr(struct run *s,uint32_t a,uint32_t v){s->o->write32(s->c,a,v);}
static int expired(struct run *s,uint64_t begin,uint64_t span){return s->o->ticks(s->c)-begin>=span;}
static int healthy(struct run *s)
{
 if(!s->o->owned_preroot(s->c) || !(rd(s,0x34)&1) ||
    (rd(s,HCS)&0xb)!=0xb || (rd(s,HCS)&0x30) || rd(s,0x58) || rd(s,0x78) || rd(s,0x24) || rd(s,0x18))return 0;
 for(uint32_t a=0x38;a<=0x48;a+=4)if(rd(s,a))return 0;
 return 1;
}
static int allowed(uint16_t attr,int set)
{
 for(unsigned i=0;i<SNAP;i++)if(attrs[i]==attr)return 1;
 if(!set)for(unsigned i=0;i<6;i++)if(caps[i]==attr)return 1;
 return 0;
}
/* Pending commands are never retried/aborted. Unexpected IS, timeout, UIC error
 * or ownership loss is ambiguous and forbids subsequent rollback/handoff. */
static int uic(struct run *s,uint16_t attr,int set,uint32_t *value)
{
 struct n2l_result *r=s->r;
 if(!allowed(attr,set) || !healthy(s) || rd(s,IS) ||
    expired(s,s->start,s->o->frequency*20)) {r->uncertain=1;return -1;}
 int power=set && attr==0x1571;uint32_t mask=UCC|(power?UPM:0);
 r->last_attr=attr;wr(s,ARG1,(uint32_t)attr<<16);wr(s,ARG2,0);wr(s,ARG3,set?*value:0);
 s->o->barrier(s->c);wr(s,CMD,set?2:1);s->o->barrier(s->c);
 r->commands++;if(set)r->sets++;
 uint64_t begin=s->o->ticks(s->c);uint64_t span=(s->o->frequency+1)/2;
 uint32_t status=0;unsigned i;
 for(i=0;i<500000;i++) {
  status=rd(s,IS);
  if(status&~mask){r->uncertain=1;break;}
  if((status&mask)==mask && (rd(s,HCS)&8))break;
  if(expired(s,begin,span) || expired(s,s->start,s->o->frequency*20)){r->uncertain=1;break;}
  s->o->delay_us(s->c,1);
 }
 r->last_is=status;r->last_hcs=rd(s,HCS);
 if(i==500000 || r->uncertain || (status&mask)!=mask){r->uncertain=1;return -1;}
 r->last_result=rd(s,ARG2)&255;uint32_t observed=rd(s,ARG3);
 /* Only the clean, newly completed transaction's flags are acknowledged. */
 wr(s,IS,mask);s->o->barrier(s->c);
 if(!healthy(s) || rd(s,IS)){r->uncertain=1;return -1;}
 if(power && ((r->last_hcs>>8)&7)!=1){r->uncertain=1;return -1;}
 if(r->last_result)return -2;
 if(!set)*value=observed;
 return 0;
}
static int capture(struct run *s,uint32_t values[SNAP])
{for(unsigned i=0;i<SNAP;i++)if(uic(s,attrs[i],0,&values[i]))return -1;return 0;}
static int verify(struct run *s,const uint32_t saved[SNAP],unsigned lanes,unsigned adapt)
{
 uint32_t actual[SNAP];if(capture(s,actual))return -1;
 for(unsigned i=0;i<SNAP;i++) {
  uint32_t expected=(i==1||i==4)?lanes:i==8?adapt:saved[i];
  if(actual[i]!=expected)return -2;
 }
 return 0;
}
static int set_value(struct run *s,unsigned index,uint32_t value)
{return uic(s,attrs[index],1,&value);}
static int negotiate(struct run *s,const uint32_t saved[SNAP],unsigned lanes)
{
 if(set_value(s,8,1))return -1; /* G4/UniPro1.8 initial adaptation */
 for(unsigned i=0;i<7;i++)if(set_value(s,i,(i==1||i==4)?lanes:saved[i]))return -1;
 for(unsigned i=9;i<SNAP;i++)if(set_value(s,i,saved[i]))return -1;
 if(set_value(s,7,saved[7]))return -1; /* negotiated async PA_PWRMODE */
 return verify(s,saved,lanes,1);
}
static int digest(struct run *s,unsigned char hash[32])
{
 if(!healthy(s)||rd(s,IS)){s->r->uncertain=1;return -1;}
 s->r->reads++;int error=s->o->read_digest(s->c,1,5,hash);
 if(error && error!=N2L_READ_COMPLETED_ERROR)s->r->uncertain=1;
 if(!healthy(s)||rd(s,IS)){s->r->uncertain=1;return -1;}
 return error;
}
static int equal(const unsigned char a[32],const unsigned char b[32])
{unsigned d=0;for(unsigned i=0;i<32;i++)d|=a[i]^b[i];return d==0;}
int n2l_diagnostic(const struct n2l_ops *o,void *ctx,unsigned optin,struct n2l_result *r)
{
 if(!r)return -1;struct n2l_result zero={0};*r=zero;
 if(optin!=1||!o||!o->read32||!o->write32||!o->ticks||!o->delay_us||!o->barrier||
  !o->owned_preroot||!o->read_digest||!o->expected_gpt_digest||!o->frequency||o->frequency>UINT64_MAX/20){r->error=-1;return -1;}
 struct run s={o,ctx,r,0};s.start=o->ticks(ctx);uint32_t saved[SNAP],limits[6];
 unsigned char baseline[32],observed[32];r->phase=1;
 if(!healthy(&s)||rd(&s,IS)){r->error=-2;return -1;}
 for(unsigned i=0;i<SNAP;i++) {
  if(uic(&s,attrs[i],0,&saved[i])){r->error=-3;return -1;}
  r->snapshot[i]=saved[i];r->snapshot_mask|=1u<<i;
 }
 for(unsigned i=0;i<6;i++) {
  if(uic(&s,caps[i],0,&limits[i])){r->error=-3;return -1;}
  r->limits[i]=limits[i];r->limits_mask|=1u<<i;
 }
 /* Unknown limits/AH8/version/timeout policy fail closed without any SET. */
 for(unsigned i=0;i<4;i++)if(limits[i]!=2){r->error=-4;return -1;}
 if((limits[4]&15)!=5||limits[5]!=4||saved[0]!=4||saved[3]!=4||
  saved[1]!=1||saved[4]!=1||saved[2]!=1||saved[5]!=1||saved[6]!=2||
  saved[7]!=0x11||saved[8]>1){r->error=-4;return -1;}
 for(unsigned i=0;i<9;i++)if(saved[9+i]!=timeout_values[i]){r->error=-4;return -1;}
 if(digest(&s,baseline)||!equal(baseline,o->expected_gpt_digest)){r->error=-5;return -1;}
 r->phase=2;int forward=negotiate(&s,saved,2);
 if(!forward && (digest(&s,observed)||!equal(baseline,observed)))forward=-1;
 if(r->uncertain){r->error=-6;return -1;}
 /* Always restore by another negotiated transition, even if preparatory SET
  * failed. No raw-active-attribute rollback masquerades as a restored link. */
 r->phase=3;r->rollback_attempted=1;
 if(negotiate(&s,saved,1)||set_value(&s,8,saved[8])||verify(&s,saved,1,saved[8])||
  digest(&s,observed)||!equal(baseline,observed)){
  r->error=-7;r->uncertain=1;return -1;
 }
 r->original_verified=1;r->handoff_allowed=1;r->phase=4;
 if(forward){r->error=-8;return -1;}
 r->root_allowed=1;return 0;
}
