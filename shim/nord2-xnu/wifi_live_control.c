#include "wifi_live_control.h"
/* Single USB producer / single retained owner. No allocator, wait, lock,
 * logging, MMIO, driver invocation or secret storage in this transport. */
static struct {
 uint32_t enabled,boot[2],pending,token,op,generation,sequence,reply[NWL_WORDS];
} live;
static uint32_t get(const uint32_t*p){return __atomic_load_n(p,__ATOMIC_ACQUIRE);}
static void put(uint32_t*p,uint32_t n){__atomic_store_n(p,n,__ATOMIC_RELEASE);}
static void le32(uint8_t*p,uint32_t n){for(unsigned k=0;k<4;k++)p[k]=(uint8_t)(n>>(k*8));}
static uint32_t le16(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8;}
int nwl_enabled(void){return get(&live.enabled)==1;}
int nwl_initialize(uint64_t nonce){
 if(!nonce||get(&live.enabled))return -1;
 put(&live.boot[0],(uint32_t)nonce);put(&live.boot[1],(uint32_t)(nonce>>32));
 put(&live.enabled,1);return 0;
}
int nwl_submit(unsigned value,unsigned index){
 if(!nwl_enabled()||value>65535||index>NWL_MAX_GENERATION)return 0;
 unsigned token=value&NWL_MAX_TOKEN,op=value>>12;
 if(!token||op<NWL_STATUS||op>NWL_DATA_TODS||(op==NWL_STATUS&&index))return 0;
 unsigned previous=get(&live.token);
 if(token==previous)return op==get(&live.op)&&index==get(&live.generation);
 if(previous==NWL_MAX_TOKEN||token!=previous+1||get(&live.pending))return 0;
 put(&live.op,op);put(&live.generation,index);put(&live.token,token);
 put(&live.pending,1);return 1;
}
int nwl_take(struct nwl_request*q){
 if(!q||!nwl_enabled()||get(&live.pending)!=1)return 0;
 *q=(struct nwl_request){get(&live.token),get(&live.op),get(&live.generation)};
 put(&live.pending,2);return 1;
}
int nwl_complete(const struct nwl_request*q,unsigned result,const struct nwl_snapshot*s){
 if(!q||!s||result>NWL_LIMIT||get(&live.pending)!=2||
 q->token!=get(&live.token)||q->op!=get(&live.op)||q->generation!=get(&live.generation))return -1;
 uint32_t version=get(&live.sequence);put(&live.sequence,version+1);
 /* Strong word ordering also prevents reader loads crossing the version
  * checks; no plain concurrent memcpy or unbounded seqlock retry. */
 for(unsigned k=0;k<NWL_WORDS;k++)__atomic_store_n(&live.reply[k],s->word[k],__ATOMIC_SEQ_CST);
 const uint32_t prefix[]={NWL_MAGIC,NWL_VERSION,NWL_BYTES,get(&live.boot[0]),get(&live.boot[1]),q->token,q->op,q->generation,result};
 for(unsigned k=0;k<sizeof prefix/sizeof prefix[0];k++)__atomic_store_n(&live.reply[k],prefix[k],__ATOMIC_SEQ_CST);
 __atomic_store_n(&live.sequence,version+2,__ATOMIC_SEQ_CST);
 put(&live.pending,0);return 0;
}
size_t nwl_caps(uint8_t*out,size_t size){
 if(!out||size<NWL_CAP_BYTES)return 0;
 uint32_t v[NWL_CAP_WORDS]={NWL_MAGIC,NWL_VERSION,NWL_CAP_BYTES,NWL_BYTES,
 NWL_COMMANDS,NWL_MAX_TOKEN,NWL_MAX_GENERATION,0,0,0,0,0,0,0,0,0};
 v[9]=nwl_enabled();if(v[9]){v[7]=get(&live.boot[0]);v[8]=get(&live.boot[1]);
 v[10]=get(&live.pending);v[11]=get(&live.token);v[12]=get(&live.op);v[13]=get(&live.generation);}
 for(unsigned k=0;k<NWL_CAP_WORDS;k++)le32(out+k*4,v[k]);return NWL_CAP_BYTES;
}
size_t nwl_reply(uint8_t*out,size_t size){
 if(!out||size<NWL_BYTES||!nwl_enabled())return 0;
 uint32_t before=__atomic_load_n(&live.sequence,__ATOMIC_SEQ_CST);
 if(!before||(before&1))return 0;
 uint32_t v[NWL_WORDS];for(unsigned k=0;k<NWL_WORDS;k++)v[k]=__atomic_load_n(&live.reply[k],__ATOMIC_SEQ_CST);
 if(before!=__atomic_load_n(&live.sequence,__ATOMIC_SEQ_CST))return 0;
 for(unsigned k=0;k<NWL_WORDS;k++)le32(out+k*4,v[k]);return NWL_BYTES;
}
int nwl_usb(const uint8_t setup[8],uint8_t*out,size_t capacity,uint32_t*length){
 if(!setup||!out||!length)return 0;unsigned code=setup[1];
 if(code<NWL_USB_CAP||code>NWL_USB_REPLY)return -1;
 *length=0;unsigned value=le16(setup+2),index=le16(setup+4),wanted=le16(setup+6);
 if(code==NWL_USB_COMMAND){if(setup[0]!=0x40||wanted)return 0;return nwl_submit(value,index)?2:0;}
 if(setup[0]!=0xc0||value||index||!wanted)return 0;
 size_t n=code==NWL_USB_CAP?nwl_caps(out,capacity):nwl_reply(out,capacity);
 if(!n)return 0;*length=(uint32_t)(n<wanted?n:wanted);return 1;
}
