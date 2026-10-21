#include "wifi_ios_cursor.h"
#include "wifi_ios_scan_contract.h"
int niwc_begin(struct niwc_cursor*c,uint64_t generation){
 if(!c||c->phase!=NIWC_NEW||!generation)return -1;
 c->phase=NIWC_WAITING;c->generation=generation;c->records=0;c->count=0;c->next=0;return 0;
}
int niwc_complete(struct niwc_cursor*c,uint64_t generation,const uint8_t*p,size_t n,unsigned count){
 struct niws_result decoded;
 if(!c||c->phase!=NIWC_WAITING||generation!=c->generation||count>NIWC_MAX_RECORDS||n!=(size_t)count*NIWC_RECORD_BYTES||(n&&!p))return -1;
 for(unsigned i=0;i<count;i++)if(niws_decode_result(p+(size_t)i*NIWC_RECORD_BYTES,NIWC_RECORD_BYTES,&decoded))return -1;
 c->records=p;c->count=count;c->phase=NIWC_READY;return 0;
}
int niwc_take(struct niwc_cursor*c,uint64_t generation,uint8_t*out,size_t n){
 if(!c||c->phase!=NIWC_READY||generation!=c->generation||!out||n!=NIWC_RECORD_BYTES||c->next>c->count||c->count>NIWC_MAX_RECORDS||(c->count&&!c->records))return -1;
 if(c->next==c->count)return 0;
 const uint8_t*p=c->records+(size_t)c->next*NIWC_RECORD_BYTES;
 /* A real caller retains snapshot immutability/owner serialization; parsing
  * again bounds consumer fields but is not authentication of changed data. */
 struct niws_result decoded;if(niws_decode_result(p,NIWC_RECORD_BYTES,&decoded))return -1;
 for(size_t i=0;i<n;i++)out[i]=p[i];
 c->next++;return 1;
}
int niwc_cancel(struct niwc_cursor*c,uint64_t generation){
 if(!c||!generation||generation!=c->generation||c->phase==NIWC_NEW||c->phase==NIWC_CANCELLED)return -1;
 c->phase=NIWC_CANCELLED;c->records=0;return 0;
}
