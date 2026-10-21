#include "wifi_main_fw_place.h"
#define WINDOW_PA UINT32_C(0x18500000)
#define WINDOW_BYTES UINT32_C(0x10000)
#define CONN_BASE UINT32_C(0x8fb00000)
#define CONN_BYTES UINT32_C(0x450000)
#define OWNER_CHUNK UINT32_C(4096)
static int fail(struct nfp_state *s,int e){s->error=e;s->halt=!!s->dirty;return e;}
static int own(const struct nfp_io *o,void *c,struct nfp_state *s){return o->owner(c)?fail(s,-2):0;}
static const struct nmt_span *span(const struct nmt_layout *l,unsigned i){
 if(i==0)return &l->patch;
 if(i==1)return &l->patch_date;
 if(i>=2&&i<=6)return &l->main[i-2];
 return 0;
}
static int exact(const struct nmt_span *s,unsigned index){
 static const uint32_t len[7]={5696,16,5744,5168,29408,675456,257520};
 static const uint32_t target[7]={0x00100000,0xf027f0d0,0x00104000,
  0x00201c00,0x00404000,0xf031e000,0xf03c3800};
 static const uint32_t pa[7]={0,0x8fd7f0d0,0,0,0,0x8fe1e000,0x8fec3800};
 enum nmt_space space=(index==1||index>=5)?NMT_CONN_EMI:NMT_AP2WF_WINDOW;
 if(!s||index>=7||!s->data||s->length!=len[index]||
    s->firmware_target!=target[index]||s->host_physical!=pa[index]||
    s->space!=space||(s->length&3u)||(s->firmware_target&3u))return -1;
 if(space==NMT_CONN_EMI){
  if(s->host_physical<CONN_BASE||s->length>CONN_BYTES-(s->host_physical-CONN_BASE))return -1;
 }else if(s->length>WINDOW_BYTES)return -1;
 return 0;
}
static uint32_t le32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|
 (uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int nfp_span(const struct nfp_io *o,void *c,const struct nmt_layout *l,
 struct nfp_state *s,unsigned index){
 const struct nmt_span *t;if(!o||!o->owner||!o->remap||!o->write32||
  !o->read32||!o->barrier||!l||!s||index>=7||s->halt||
  (s->entered&&s->next!=index))return -1;
 t=span(l,index);if(exact(t,index))return -1;
 if(!s->entered){if(index)return -1;s->entered=1;}
 s->stage=index+1;s->attempted|=1u<<index;
 if(own(o,c,s))return s->error;
 if(t->space==NMT_AP2WF_WINDOW){
  s->dirty=1;s->last_target=t->firmware_target;
  if(o->remap(c,t->firmware_target))return fail(s,-3);
  s->remaps++;
  if(own(o,c,s)||o->barrier(c))return fail(s,-4);
  s->barriers++;
 }
 for(uint32_t off=0;off<t->length;off+=4){
  uint32_t physical=(t->space==NMT_AP2WF_WINDOW?WINDOW_PA:t->host_physical)+off;
  uint32_t value=le32(t->data+off);
  if((off&(OWNER_CHUNK-1u))==0&&own(o,c,s))return s->error;
  s->dirty=1;s->last_address=physical;s->last_expected=value;
  if(o->write32(c,t->space,physical,value))return fail(s,-5);
  s->words_written++;
 }
 if(own(o,c,s)||o->barrier(c))return fail(s,-6);
 s->barriers++;
 for(uint32_t off=0;off<t->length;off+=4){
  uint32_t physical=(t->space==NMT_AP2WF_WINDOW?WINDOW_PA:t->host_physical)+off;
  uint32_t value=0,expected=le32(t->data+off);
  if(((off&(OWNER_CHUNK-1u))==0&&own(o,c,s))||
     o->read32(c,t->space,physical,&value))return fail(s,-7);
  s->words_read++;s->last_address=physical;s->last_expected=expected;s->last_observed=value;
  if(value!=expected)return fail(s,-8);
 }
 if(own(o,c,s)||o->barrier(c))return fail(s,-6);
 s->barriers++;
 s->completed|=1u<<index;s->next=index+1;
 if(index==6)s->stage=NFP_COPIED;
 return 0;
}
