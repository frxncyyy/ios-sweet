/******************************************************************************
 *
 * This file is provided under a dual license.  When you use or
 * distribute this software, you may choose to be licensed under
 * version 2 of the GNU General Public License ("GPLv2 License")
 * or BSD License.
 *
 * GPLv2 License
 *
 * Copyright(C) 2016 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See http://www.gnu.org/licenses/gpl-2.0.html for more details.
 *
 * BSD LICENSE
 *
 * Copyright(C) 2016 MediaTek Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  * Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/
/* Derived from MediaTek dual-licensed selected gen4m source; BSD chosen.
 * See LICENSES.md for copyright, terms and exact extraction anchors. */
#include "wifi_normal_config.h"
#ifdef NCFG_HOST_TEST
#include <openssl/sha.h>
#else
#include <libkern/crypto/sha2.h>
#endif
struct cfg_domain {const uint16_t *cc;unsigned count;uint8_t sub[6][6];};
struct cfg_default {uint8_t cc[2];int8_t power[5];uint8_t unit;};
struct cfg_ac {uint8_t cc[2],channel;int8_t power[9];};
struct cfg_he {uint8_t cc[2],channel;int8_t power[18];};
struct cfg_group {const uint8_t (*cc)[2];unsigned count;};
#include "wifi_normal_config_tables.inc"
#define COUNT(a) (sizeof(a)/sizeof((a)[0]))
static void zero(void *v,size_t n){uint8_t *p=v;while(n--)*p++=0;}
static void put16(uint8_t *p,unsigned x){p[0]=(uint8_t)x;p[1]=(uint8_t)(x>>8);}
static void put32(uint8_t *p,uint32_t x){put16(p,x);put16(p+2,x>>16);}
static unsigned cc(const uint8_t *p){return (unsigned)p[0]*256+p[1];}
static int digest(const uint8_t *p,size_t n){
 static const uint8_t expected[32]={0x61,0xac,0x33,0x00,0xaa,0xd6,0xf6,0x53,0x64,0xf8,0x45,0x89,0x4b,0xb9,0x82,0xb7,0xc5,0xd0,0x77,0x34,0x6f,0x78,0x8c,0x29,0xdf,0x01,0x9c,0x93,0xc4,0xf3,0xf8,0x9f};
 uint8_t sum[32];unsigned d=0;
#ifdef NCFG_HOST_TEST
 if(!SHA256(p,n,sum))return -1;
#else
 SHA256_CTX h;SHA256_Init(&h);SHA256_Update(&h,p,n);SHA256_Final(sum,&h);
#endif
 for(unsigned j=0;j<32;j++)d|=sum[j]^expected[j];
 volatile uint8_t *v=sum;for(unsigned j=0;j<32;j++)v[j]=0;
 return d?-1:0;
}
static unsigned select_domain(const struct cfg_domain *a,unsigned n,unsigned country,unsigned fallback,unsigned *used){
 for(unsigned i=0;i<n;i++)for(unsigned j=0;j<a[i].count;j++)if(a[i].cc[j]==country){*used=0;return i;}
 *used=1;return fallback;
}
static unsigned select_power(const struct cfg_default *a,unsigned n,unsigned country){
 for(unsigned i=0;i<n;i++)if(cc(a[i].cc)==country)return i;
 for(unsigned g=0;g<COUNT(cfg_power_groups);g++){
  unsigned found=0;for(unsigned j=0;j<cfg_power_groups[g].count;j++)if(cc(cfg_power_groups[g].cc[j])==country){found=1;break;}
  if(found){country=0x4730+g;break;}
 }
 for(unsigned i=0;i<n;i++)if(cc(a[i].cc)==country||!cc(a[i].cc))return i;
 return n;
}
int ncfg_prepare(const struct ncfg_input *in,struct ncfg_plan *out){
 struct ncfg_plan p={0};struct nrp_board b;
 if(!in||!out||!in->board||in->board_bytes!=6146||in->project!=20827||
 in->caps.sku_present||!in->caps.phy_present||in->caps.band5!=1||in->caps.max_bw!=2||in->caps.nss!=2||in->caps.path!=15||
 digest(in->board,in->board_bytes)||nrp_board_decode(in->board,in->board_bytes,&b)||
 nni_nvram_plan(in->board,in->board_bytes,&p.nvram)||p.nvram.fragments!=12)return -1;
 unsigned country=cc(b.country);p.count=NCFG_COMMANDS;p.band5=b.support5&&b.enable5&&in->caps.band5;
 p.active_index=select_domain(cfg_active,COUNT(cfg_active),country,COUNT(cfg_active)-1,&p.active_fallback);
 p.passive_index=select_domain(cfg_passive,COUNT(cfg_passive),country,1,&p.passive_fallback);
 p.initial_power_index=select_power(cfg_initial_default,COUNT(cfg_initial_default),country);
 p.project_power_index=select_power(cfg_project_default,COUNT(cfg_project_default),country);
 if(p.initial_power_index>=COUNT(cfg_initial_default)||p.project_power_index>=COUNT(cfg_project_default))return -1;
 *out=p;return 0;
}
static int same(const struct ncfg_plan *a,const struct ncfg_plan *b){
 if(a->count!=b->count||a->band5!=b->band5||a->active_index!=b->active_index||a->passive_index!=b->passive_index||a->active_fallback!=b->active_fallback||a->passive_fallback!=b->passive_fallback||a->initial_power_index!=b->initial_power_index||a->project_power_index!=b->project_power_index)return 0;
 const struct nni_nvram *x=&a->nvram,*y=&b->nvram;
 if(x->fragments!=y->fragments||x->source_bytes!=y->source_bytes||x->used_bytes!=y->used_bytes||x->terminator!=y->terminator||x->ignored_bytes!=y->ignored_bytes)return 0;
 for(unsigned j=0;j<x->fragments;j++)if(x->span[j].offset!=y->span[j].offset||x->span[j].bytes!=y->span[j].bytes||x->span[j].tag!=y->span[j].tag)return 0;
 return 1;
}
int ncfg_address(const struct ncfg_input *in,const struct ncfg_plan *p,struct nni_address *out){
 struct ncfg_plan check;
 if(!p||!out||ncfg_prepare(in,&check)||!same(p,&check))return -1;
 return nni_address_select(in->board,in->board_bytes,0,out);
}
int ncfg_entry(const struct ncfg_plan *p,unsigned i,struct ncfg_entry *out){
 if(!p||!out||p->count!=NCFG_COMMANDS||i>=p->count)return -1;
 *out=(struct ncfg_entry){i<2?0x0f:(i>=5&&i<17?0x48:0x49),1,i<2?120:(i>=5&&i<17?1564:1536)};return 0;
}
static void header(uint8_t *p,unsigned bytes,unsigned cid){zero(p,bytes);put32(p,bytes|(2u<<23));put32(p+4,1u<<16);put16(p+32,bytes-32);p[36]=(uint8_t)cid;p[37]=0xa0;p[38]=1;}
static int channel_in(const struct cfg_domain *d,unsigned band,unsigned channel,unsigned *dfs){
 for(unsigned j=0;j<6;j++){const uint8_t *s=d->sub[j];if(s[1]!=band)continue;
  for(unsigned k=0;k<s[4];k++)if((unsigned)s[3]+k*s[2]==channel){if(dfs)*dfs|=s[5]!=0;return 1;}}
 return 0;
}
static int clamp(int x){return x>63?63:(x< -64?-64:x);}
static void power(uint8_t *p,const struct cfg_default *def,unsigned type){
 static const uint8_t lo[5]={1,36,52,100,149},hi[5]={14,50,64,144,165};
 if(type!=7){put16(p,cc(def->cc));}
 p[4]=type!=0;p[6]=(uint8_t)type;p[7]=1;
 if(type==7)return; /* initialized empty lists/default empty fields: no ANT entries */
 unsigned stride=type==2?21:12,rates=type==2?18:9,n=0;
 for(unsigned band=0;band<5;band++)for(unsigned ch=lo[band];ch<=hi[band];ch+=band?2:1){
  uint8_t *r=p+8+n*stride;r[0]=(uint8_t)ch;int base=def->power[band]>63?63:def->power[band];
  for(unsigned rate=0;rate<rates;rate++){
   unsigned shift=type==2?(rate<12?0:(rate<15?6:12)):(rate<3?0:(rate<5?6:(rate<7?12:18)));
   int v=base;if(def->unit&(1u<<band))v=clamp(base+(int)shift);r[1+rate]=(uint8_t)v;
  }
  unsigned country=cc(def->cc);
  { /* source power validity uses full subband center-channel table, not regulatory permission */
   if(type==2){for(unsigned k=0;k<COUNT(cfg_project_he);k++){const struct cfg_he *x=&cfg_project_he[k];if(!cc(x->cc))break;if(cc(x->cc)!=country||x->channel!=ch)continue;for(unsigned rix=0;rix<rates;rix++){int v=(int8_t)r[1+rix];if(v>x->power[rix])v=x->power[rix];r[1+rix]=(uint8_t)clamp(v);}}}
   else{for(unsigned k=0;k<COUNT(cfg_project_ac);k++){const struct cfg_ac *x=&cfg_project_ac[k];if(!cc(x->cc))break;if(cc(x->cc)!=country||x->channel!=ch)continue;for(unsigned rix=0;rix<rates;rix++){int v=(int8_t)r[1+rix];if(v>x->power[rix])v=x->power[rix];r[1+rix]=(uint8_t)clamp(v);}}}
  }
  n++;
 }
 p[3]=(uint8_t)n;
}
int ncfg_build(const struct ncfg_input *in,const struct ncfg_plan *p,unsigned i,uint8_t *out,size_t capacity,size_t *used){
 struct ncfg_plan check;struct ncfg_entry e;
 if(!p||!out||!used||ncfg_prepare(in,&check)||!same(p,&check)||ncfg_entry(p,i,&e)||capacity<e.bytes)return -1;
 /* All refusal checks complete before any caller output is modified. */
 if(e.cid==0x48){if(nni_nvram_fragment(in->board,in->board_bytes,&p->nvram,i-5,0,out,capacity))return -1;}
 else{header(out,e.bytes,e.cid);uint8_t *body=out+64;
  if(i<2){const struct cfg_domain *d=i==0?&cfg_passive[p->passive_index]:&cfg_active[p->active_index];put16(body,cc(in->board+10));put16(body+2,i==0);
   for(unsigned j=0;j<6;j++){const uint8_t *s=d->sub[j];uint8_t *t=body+4+j*8;t[0]=s[0];t[1]=s[1];if(s[1]>0&&s[1]<3){t[2]=s[2];t[3]=s[3];t[4]=s[4];}}
   /* source default bandwidth modes both20/40 (CONFIG_BW_20_40M=0). */
  }else{unsigned type=(i<5?i-2:i-17);type=type==0?0:(type==1?2:7);
   const struct cfg_default *d=i<5?&cfg_initial_default[p->initial_power_index]:&cfg_project_default[p->project_power_index];
   power(body,d,type);}
 }
 *used=e.bytes;return 0;
}
int ncfg_channels(const struct ncfg_input *in,const struct ncfg_plan *p,struct ncfg_channel *out,size_t capacity,size_t *used){
 static const uint8_t ch5[25]={36,40,44,48,52,56,60,64,100,104,108,112,116,120,124,128,132,136,140,144,149,153,157,161,165};
 struct ncfg_plan check;if(!p||!out||!used||capacity<39||ncfg_prepare(in,&check)||!same(p,&check))return -1;
 for(unsigned j=0;j<39;j++){unsigned band=j<14?1:2,ch=j<14?j+1:ch5[j-14],dfs=0;
  unsigned allow=channel_in(&cfg_active[p->active_index],band,ch,&dfs)&&(band==1||p->band5);
  out[j]=(struct ncfg_channel){(uint8_t)band,(uint8_t)ch,(uint8_t)allow,(uint8_t)dfs,(uint8_t)channel_in(&cfg_passive[p->passive_index],band,ch,0)};
 }
 *used=39;return 0;
}
