#include "wifi_ios_event.h"
static uint16_t g16(const uint8_t*p){return p[0]|(uint16_t)p[1]<<8;}
static uint32_t g32(const uint8_t*p){return g16(p)|(uint32_t)g16(p+2)<<16;}
static int valid(const struct niwe_view*v){return v&&v->capacity>=52&&v->capacity<=1048576&&!(v->capacity&3)&&v->consumer<v->capacity&&v->producer<v->capacity&&!((v->consumer|v->producer)&3);}
int niwe_peek(const uint8_t*p,size_t n,const struct niwe_view*v,struct niwe_record*out){
 struct niwe_record r={0};uint32_t available,tail;
 if(!p||!out||!valid(v)||n!=v->capacity)return -1;
 available=(v->producer+v->capacity-v->consumer)%v->capacity;if(!available)return 0;
 tail=v->capacity-v->consumer;r.kind=g32(p+v->consumer);
 if(r.kind==80||tail<48){r.total=4;r.next=(v->consumer+4)%v->capacity;*out=r;return 1;}
 if(available<48||r.kind!=87)return -1;
 const uint8_t*h=p+v->consumer;r.primary=g16(h+24);r.secondary=g16(h+26);r.event=g32(h+28);
 r.total=48u+r.primary+r.secondary;
 if(r.primary>4080||r.total>=v->capacity||r.total>available||(r.total&3))return -1;
 r.next=(v->consumer+r.total)%v->capacity;r.primary_offset=(v->consumer+48)%v->capacity;
 r.first_bytes=v->capacity-r.primary_offset;if(r.first_bytes>r.primary)r.first_bytes=r.primary;
 r.second_bytes=r.primary-r.first_bytes;
 for(unsigned i=0;i<16;i++)r.interface_name[i]=h[32+i];*out=r;return 2;
}
int niwe_copy_primary(const uint8_t*p,size_t n,const struct niwe_view*v,const struct niwe_record*r,uint8_t*out,size_t cap){
 struct niwe_record check;if(!r||!out||niwe_peek(p,n,v,&check)!=2||cap<check.primary)return -1;
 /* No caller-controlled offsets: rederive and compare all semantic fields. */
 if(r->kind!=check.kind||r->event!=check.event||r->next!=check.next||r->primary!=check.primary||r->secondary!=check.secondary||r->total!=check.total||r->primary_offset!=check.primary_offset||r->first_bytes!=check.first_bytes||r->second_bytes!=check.second_bytes)return -1;
 for(unsigned i=0;i<16;i++)if(r->interface_name[i]!=check.interface_name[i])return -1;
 for(unsigned i=0;i<check.first_bytes;i++)out[i]=p[check.primary_offset+i];
 for(unsigned i=0;i<check.second_bytes;i++)out[check.first_bytes+i]=p[i];return 0;
}
int niwe_plan(const struct niwe_view*v,uint16_t primary,uint16_t secondary,struct niwe_space*out){
 struct niwe_space r={0};uint32_t used,tail;if(!out||!valid(v)||primary>4080)return -1;
 r.total=48u+primary+secondary;if(r.total&3)return -1;
 used=(v->producer+v->capacity-v->consumer)%v->capacity;r.free_bytes=v->capacity-used-4;
 tail=v->capacity-v->producer;r.padding=tail<48?tail:0;
 if(r.total>r.free_bytes||r.padding>r.free_bytes-r.total)return 0;
 r.record_offset=(v->producer+r.padding)%v->capacity;r.next=(r.record_offset+r.total)%v->capacity;*out=r;return 1;
}

static void p32(uint8_t*p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
int niwe_scan_done(uint8_t*p,size_t n,const uint8_t name[16],uint32_t status){
 if(!p||!name||n!=52)return -1;
 for(unsigned i=0;i<52;i++)p[i]=0;p32(p,87);p[24]=4;p32(p+28,10);
 for(unsigned i=0;i<16;i++)p[32+i]=name[i];p32(p+48,status);return 0;
}
