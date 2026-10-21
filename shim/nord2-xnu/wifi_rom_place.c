#include "wifi_rom_place.h"
#ifdef NRB_HOST_TEST
#include <string.h>
#else
#include <libkern/libkern.h>
#endif
static uint32_t le32(const unsigned char *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static uint64_t le64(const unsigned char *p){return le32(p)|(uint64_t)le32(p+4)<<32;}
int nrb_claim_decode(const void *wire,size_t n,struct nrc_claim *out){
 if(!out)return -1;
 *out=(struct nrc_claim){0};
 if(!wire||n!=24)return -1;
 const unsigned char *p=wire;uint32_t src=le32(p+16);
 if(le64(p)!=NRC_CONN_BASE||le64(p+8)!=NRC_CONN_BYTES||src!=NRC_CLAIM_IOS_FIXED||le32(p+20)!=1)return -1;
 *out=(struct nrc_claim){(enum nrc_claim_source)src,NRC_CONN_BASE,NRC_CONN_BYTES};return 0;
}
static int overlap(const void *a,size_t an,const void *b,size_t bn){
 uintptr_t x=(uintptr_t)a,y=(uintptr_t)b;
 if(an>UINTPTR_MAX-x||bn>UINTPTR_MAX-y)return 1;
 return x<y+bn&&y<x+an;
}
int nrb_prepare(struct nrb_state *s,unsigned opt,const struct nrc_claim *claim,
 const unsigned char *m,size_t mn,const unsigned char *w,size_t wn,
 unsigned char *backup,size_t bn,unsigned char *scratch,size_t sn){
 if(opt!=1||!s||s->prepared||s->policy_attempted||!claim||claim->source!=NRC_CLAIM_IOS_FIXED||claim->base!=NRC_CONN_BASE||claim->bytes!=NRC_CONN_BYTES||!backup||bn<NRB_BACKUP_BYTES||!scratch||sn<NRB_CHUNK)return -1;
 const void *ptr[]={s,claim,m,w,backup,scratch};size_t len[]={sizeof(*s),sizeof(*claim),mn,wn,NRB_BACKUP_BYTES,NRB_CHUNK};
 for(unsigned i=0;i<6;i++)for(unsigned j=0;j<i;j++)if(overlap(ptr[i],len[i],ptr[j],len[j]))return -1;
 struct nwf_rom_plan p;
 if(nwf_rom_make_plan(m,mn,w,wn,&p))return -1;
 *s=(struct nrb_state){0};s->claim=*claim;s->plan=p;
 s->mcu=m;s->mcu_bytes=mn;s->wifi=w;s->wifi_bytes=wn;s->backup=backup;s->scratch=scratch;s->prepared=1;s->phase=1;return 0;
}
int nrb_policy(const struct nrb_ops *o,void *c,struct nrb_state *s){
 if(!s||!s->prepared||s->policy_attempted||s->halt||!o||!o->owner||!o->smc)return -1;
 struct nrc_smc_request q[2];
 if(nrc_region27_set_intent(&s->claim,q)||o->owner(c,NRB_COLD_BEFORE_POWER))return -1;
 s->phase=2;s->policy_attempted=1;
 for(unsigned i=0;i<2;i++){
  if(o->owner(c,NRB_COLD_BEFORE_POWER)){s->error=-2;goto fail;}
  s->issued++;for(unsigned j=0;j<4;j++)s->raw[i][j]=UINT64_MAX;
  if(o->smc(c,q[i].x,s->raw[i])){s->error=-3;goto fail;}
  s->raw_valid|=1u<<i;
  if(s->raw[i][0]){s->error=-4;goto fail;}
  s->accepted++;
 }
 s->phase=3;return 0;
fail:s->halt=1;return -1;
}
static int owned(const struct nrb_ops *o,void *c){return o->owner(c,NRB_POWERED_CPU_HELD);}
int nrb_place(const struct nrb_ops *o,void *c,struct nrb_state *s){
 if(!s||!s->prepared||s->halt||s->phase!=3||s->accepted!=2||s->raw_valid!=3||s->mapped||!o||!o->owner||!o->map||!o->read||!o->write||!o->barrier)return -1;
 struct nwf_rom_plan p;
 if(nwf_rom_make_plan(s->mcu,s->mcu_bytes,s->wifi,s->wifi_bytes,&p)||memcmp(&p,&s->plan,sizeof(p))||owned(o,c)){s->error=-5;goto fail;}
 s->phase=4;
 if(o->map(c,s->claim.base,s->claim.bytes)){s->error=-6;goto fail;}
 s->mapped=1;
 /* No destination mutation until ALL original bytes exist in backup. */
 size_t base=0;
 for(unsigned i=0;i<NWF_ROM_SPANS;i++){
  const struct nwf_rom_span *t=&s->plan.spans[i];
  for(uint32_t off=0;off<t->bytes;){uint32_t n=t->bytes-off;if(n>NRB_CHUNK)n=NRB_CHUNK;
   if(owned(o,c)||o->read(c,t->physical+off,s->backup+base+off,n)){s->error=-7;goto fail;}
   s->reads++;off+=n;
  }base+=t->bytes;
 }
 if(base!=NRB_BACKUP_BYTES){s->error=-8;goto fail;}s->backup_complete=1;s->phase=5;
 for(unsigned i=0;i<NWF_ROM_SPANS;i++){
  const struct nwf_rom_span *t=&s->plan.spans[i];const unsigned char *src=t->blob?s->wifi:s->mcu;
  for(uint32_t off=0;off<t->bytes;){uint32_t n=t->bytes-off;if(n>NRB_CHUNK)n=NRB_CHUNK;
   if(owned(o,c)){s->error=-9;goto fail;}
   s->write_attempted=1;s->writes++;
   if(o->write(c,t->physical+off,src+t->source_offset+off,n)){s->error=-10;goto fail;}off+=n;
  }
 }
 if(owned(o,c)||o->barrier(c)){s->error=-11;goto fail;}s->barriers++;s->phase=6;
 for(unsigned i=0;i<NWF_ROM_SPANS;i++){
  const struct nwf_rom_span *t=&s->plan.spans[i];const unsigned char *src=t->blob?s->wifi:s->mcu;
  for(uint32_t off=0;off<t->bytes;){uint32_t n=t->bytes-off;if(n>NRB_CHUNK)n=NRB_CHUNK;
   if(owned(o,c)||o->read(c,t->physical+off,s->scratch,n)){s->error=-12;goto fail;}
   s->reads++;if(memcmp(s->scratch,src+t->source_offset+off,n)){s->error=-13;goto fail;}off+=n;
  }
 }
 if(owned(o,c)||o->barrier(c)){s->error=-14;goto fail;}s->barriers++;
 s->placed=1;s->phase=7;return 0;
fail:s->halt=1;return -1;
}
