#include "wifi_ios_control.h"
static uint32_t le32(const uint8_t *p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
int niw_decode(unsigned selector,const void *input,size_t bytes,struct niw_request *out){
 const uint8_t *p=input;struct niw_request q={0};size_t cap;
 if(!p||!out)return -1;
 if(selector==0&&bytes==NIW_SMALL_BYTES)cap=NIW_SMALL_PAYLOAD;
 else if(selector==6&&bytes==NIW_LARGE_BYTES)cap=NIW_LARGE_PAYLOAD;
 else return -1;
 q.operation=le32(p);q.payload_bytes=le32(p+4);q.payload_offset=8;q.payload_capacity=cap;
 if(q.operation!=NIW_GET||q.payload_bytes>cap)return -1;
 size_t tail=8+cap;unsigned terminated=0;
 for(unsigned i=0;i<16;i++){q.interface_name[i]=(char)p[tail+i];if(!p[tail+i])terminated=1;}
 if(!terminated||!q.interface_name[0])return -1;
 q.request=le32(p+tail+16);q.value=le32(p+tail+20);
 /* Tail+24/+32 are not trusted as kernel lengths/pointers. No dereference. */
 *out=q;return 0;
}
int niw_call_shape(unsigned selector,size_t input,size_t output,unsigned async_refs){
 if(selector==0)return input==NIW_SMALL_BYTES&&!async_refs;
 if(selector==6)return input==NIW_LARGE_BYTES&&!async_refs;
 if(selector==1||selector==5)return !input&&output==1&&!async_refs;
 if(selector==3||selector==4)return !input&&!output&&!async_refs;
 if(selector==2)return !input&&!output&&async_refs==8;
 return 0;
}
int niw_unbound_request(const struct niw_request *request){(void)request;return -1;}
