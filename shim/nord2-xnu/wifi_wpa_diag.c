#include "wifi_wpa_diag.h"
int nwpd_take(struct nwpd_budget*b,struct nwpd_record*r,uint64_t g,uint64_t t,unsigned state,const uint8_t*p,size_t n,int result,unsigned failure){
 if(!b||!r||n>640||(!p&&n)||state>5||result< -1||result>4||failure>14||b->serial>=40)return 0;
 unsigned bucket=result<0?2:state==5?1:0;unsigned*used=bucket==2?&b->failure:bucket==1?&b->late:&b->early;unsigned limit=bucket==1?24:8;if(*used>=limit)return 0;
 struct nwpd_record v={0};v.generation=g;v.ticket=t;v.n=++b->serial;v.bucket=bucket;v.state=state;v.bytes=(unsigned)n;v.result=(unsigned)(result+1);v.failure=failure;
 if(n>=4){v.version=p[0];v.type=p[1];v.declared=(unsigned)p[2]<<8|p[3];}
 if(n>=7&&v.type==3){v.info=(unsigned)p[5]<<8|p[6];v.class_id=v.info==0x008a?1:v.info==0x13ca?3:!(v.info&8)?5:0;}
 ++*used;*r=v;return 1;
}

#if defined(KERNEL) || defined(_KERNEL)
#include <sys/systm.h>
#else
#include <stdio.h>
#endif
void nwpd_observe(uint64_t g,uint64_t t,unsigned state,const uint8_t*p,size_t n,int result,unsigned failure){
 static struct nwpd_budget budget;struct nwpd_record r;
 if(nwpd_take(&budget,&r,g,t,state,p,n,result,failure))
 printf("NORD2 WIFI WPAHDR: n=%u g=%llu t=%llu bucket=%u state=%u bytes=%u version=%u type=%u declared=%u info=%u class=%u result=%u failure=%u\n",r.n,(unsigned long long)r.generation,(unsigned long long)r.ticket,r.bucket,r.state,r.bytes,r.version,r.type,r.declared,r.info,r.class_id,r.result,r.failure);
}

int nwpd_gate_record(struct nwpd_gate*r,uint64_t g,uint64_t t,unsigned state,unsigned present,size_t n){
 if(!r||!g||state>5||present>1)return 0;
 unsigned reason=!present?1:!n?2:n>640?3:0;if(!reason)return 0;
 *r=(struct nwpd_gate){g,t,(uint64_t)n,state,present,reason};return 1;
}
void nwpd_gate_observe(uint64_t g,uint64_t t,unsigned state,unsigned present,size_t n){
 static unsigned emitted;struct nwpd_gate r;if(emitted||!nwpd_gate_record(&r,g,t,state,present,n))return;emitted=1;
 printf("NORD2 WIFI WPAGATE: n=1 g=%llu t=%llu state=%u present=%u bytes=%llu reason=%u\n",(unsigned long long)r.generation,(unsigned long long)r.ticket,r.state,r.present,(unsigned long long)r.bytes,r.reason);
}
