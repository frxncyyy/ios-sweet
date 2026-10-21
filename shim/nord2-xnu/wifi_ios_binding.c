#include "wifi_ios_binding.h"
#include "wifi_ios_control.h"
int niwb_selector(const struct niwb_dispatch_view*v,unsigned selector,
 size_t in,size_t cap,unsigned si,unsigned so,unsigned ar,uint8_t*out,size_t*used){
 if(!v||!out||!used||v->phase!=NIWB_OPEN||in||cap!=1||si||so||ar||
    (v->control_handlers&~NIWB_CONTROL_ALL)||(v->pipe_handlers&~NIWB_PIPE_ALL)||
    (selector!=1&&selector!=5))return -1;
 unsigned control=v->control_handlers==NIWB_CONTROL_ALL;
 uint8_t result=(uint8_t)(selector==1?control:
    (control&&v->pipe_handlers==NIWB_PIPE_ALL));
 *out=result;*used=1;return 0;
}
int niwb_driver_version(const void*carrier,size_t bytes,const char*identity,
 size_t length,void*out,size_t cap,size_t*used){
 struct niw_request q;
 if(!identity||!out||!used||length<2||length>256||cap<length||
    niw_decode(0,carrier,bytes,&q)||q.request!=43||q.value||q.payload_bytes||
    identity[length-1])return -1;
 for(size_t i=0;i+1<length;i++)
  if((unsigned char)identity[i]<32||(unsigned char)identity[i]>126)return -1;
 /* Native supplies disjoint retained identity/output storage. */
 for(size_t i=0;i<length;i++)((unsigned char*)out)[i]=(unsigned char)identity[i];
 *used=length;return 0;
}
