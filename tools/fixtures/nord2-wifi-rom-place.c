#include "wifi_rom_place.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct ctx {struct nrb_state *s;unsigned calls,fail,failed,after,smcs,maps,readcalls,writecalls,barriers,powered,smcerr_group,badread;unsigned char *memory;};
static int step(struct ctx *c){if(c->failed)c->after++;if(++c->calls==c->fail){c->failed=1;return -1;}return 0;}
static int owner(void *p,enum nrb_owner_stage stage){struct ctx *c=p;if(step(c))return -1;assert((stage==NRB_COLD_BEFORE_POWER&&!c->powered)||(stage==NRB_POWERED_CPU_HELD&&c->powered));return 0;}
static int smc(void *p,const uint64_t a[8],uint64_t raw[4]){struct ctx *c=p;if(step(c))return -1;struct nrc_smc_request q[2];assert(c->smcs<2&&!c->powered&&!c->maps);assert(!nrc_region27_set_intent(&c->s->claim,q));assert(!memcmp(a,q[c->smcs].x,sizeof(q[0].x)));c->smcs++;raw[0]=(c->smcerr_group==c->smcs)?0x200c:0;raw[1]=0x123;raw[2]=0x456;raw[3]=0x789;return 0;}
static int map(void *p,uint64_t base,uint64_t bytes){struct ctx *c=p;if(step(c))return -1;assert(c->smcs==2&&c->s->accepted==2&&c->powered&&base==NRC_CONN_BASE&&bytes==NRC_CONN_BYTES);c->maps++;return 0;}
static size_t offset(struct ctx *c,uint64_t pa,uint32_t n){assert(c->s->accepted==2&&c->s->raw_valid==3&&c->maps==1&&c->powered&&n&&n<=NRB_CHUNK);assert(pa>=NRC_CONN_BASE&&pa-NRC_CONN_BASE+n<=NRC_CONN_BYTES);return (size_t)(pa-NRC_CONN_BASE);}
static int read(void *p,uint64_t pa,void *b,uint32_t n){struct ctx *c=p;if(step(c))return -1;memcpy(b,c->memory+offset(c,pa,n),n);if(c->badread&&c->s->phase==6)((unsigned char *)b)[0]^=1;c->readcalls++;return 0;}
static int write(void *p,uint64_t pa,const void *b,uint32_t n){struct ctx *c=p;if(step(c))return -1;assert(c->s->backup_complete&&c->readcalls==88);memcpy(c->memory+offset(c,pa,n),b,n);c->writecalls++;return 0;}
static int barrier(void *p){struct ctx *c=p;if(step(c))return -1;c->barriers++;return 0;}
static const struct nrb_ops ops={owner,smc,map,read,write,barrier};
static unsigned char *file(const char *p,size_t n){FILE *f=fopen(p,"rb");assert(f);unsigned char *b=malloc(n);assert(b&&fread(b,1,n,f)==n&&fgetc(f)==EOF);fclose(f);return b;}
int main(int argc,char **argv){assert(argc==3);unsigned char *m=file(argv[1],49868),*w=file(argv[2],296916),*backup=malloc(NRB_BACKUP_BYTES),*scratch=malloc(NRB_CHUNK),*mem=malloc(NRC_CONN_BYTES);assert(backup&&scratch&&mem);
 struct nrc_claim claim={NRC_CLAIM_IOS_FIXED,NRC_CONN_BASE,NRC_CONN_BYTES};struct nrb_state s={0};struct ctx c={.s=&s,.memory=mem};
 unsigned char wire[24]={0};for(unsigned i=0;i<8;i++){wire[i]=(unsigned char)(NRC_CONN_BASE>>(i*8));wire[8+i]=(unsigned char)(NRC_CONN_BYTES>>(i*8));}wire[16]=3;wire[20]=1;struct nrc_claim decoded;assert(!nrb_claim_decode(wire,24,&decoded));wire[20]=0;assert(nrb_claim_decode(wire,24,&decoded));wire[20]=1;wire[16]=2;assert(nrb_claim_decode(wire,24,&decoded));wire[16]=3;
 assert(nrb_prepare(&s,0,&claim,m,49868,w,296916,backup,NRB_BACKUP_BYTES,scratch,NRB_CHUNK)&&!c.calls);
 assert(!nrb_prepare(&s,1,&claim,m,49868,w,296916,backup,NRB_BACKUP_BYTES,scratch,NRB_CHUNK));assert(!nrb_policy(&ops,&c,&s));unsigned policycalls=c.calls;c.powered=1;memset(mem,0xa5,NRC_CONN_BYTES);assert(!nrb_place(&ops,&c,&s));assert(s.placed&&s.phase==7&&!s.halt&&s.backup_complete&&c.writecalls==88&&c.readcalls==176&&c.barriers==2);unsigned total=c.calls;
 for(unsigned i=0;i<NWF_ROM_SPANS;i++){const struct nwf_rom_span *t=&s.plan.spans[i];const unsigned char *src=t->blob?w:m;assert(!memcmp(mem+t->emi_offset,src+t->source_offset,t->bytes));}for(unsigned i=0;i<NRB_BACKUP_BYTES;i++)assert(backup[i]==0xa5);
 unsigned before=c.calls;assert(nrb_policy(&ops,&c,&s)&&nrb_place(&ops,&c,&s)&&c.calls==before);
 for(unsigned fail=1;fail<=total;fail++){
  s=(struct nrb_state){0};c=(struct ctx){.s=&s,.memory=mem,.fail=fail};assert(!nrb_prepare(&s,1,&claim,m,49868,w,296916,backup,NRB_BACKUP_BYTES,scratch,NRB_CHUNK));
  int e=nrb_policy(&ops,&c,&s);if(!e){c.powered=1;e=nrb_place(&ops,&c,&s);}assert(e&&!c.after);
  if(s.issued)assert(s.halt);
  if(s.accepted<2)assert(!c.maps&&!c.writecalls);
 }
 for(unsigned group=1;group<=2;group++){
  s=(struct nrb_state){0};c=(struct ctx){.s=&s,.memory=mem,.smcerr_group=group};assert(!nrb_prepare(&s,1,&claim,m,49868,w,296916,backup,NRB_BACKUP_BYTES,scratch,NRB_CHUNK));assert(nrb_policy(&ops,&c,&s));assert(c.smcs==group&&s.halt&&s.raw[group-1][0]==0x200c&&!c.maps);
 }
 s=(struct nrb_state){0};c=(struct ctx){.s=&s,.memory=mem,.badread=1};assert(!nrb_prepare(&s,1,&claim,m,49868,w,296916,backup,NRB_BACKUP_BYTES,scratch,NRB_CHUNK));assert(!nrb_policy(&ops,&c,&s));c.powered=1;assert(nrb_place(&ops,&c,&s)&&s.halt&&s.backup_complete&&s.write_attempted&&!s.placed);
 printf("PASS authenticatedrealROM fourpersistent spans/346720backup/88writes/readback/barriers; every%ucallbackfault (%upolicy); group1/0positiveerrorstopbeforemap, invalidcurrentclaim, defaultoff, no retries, mismatchretainedhalt\n",total,policycalls);
 free(m);free(w);free(backup);free(scratch);free(mem);
}
