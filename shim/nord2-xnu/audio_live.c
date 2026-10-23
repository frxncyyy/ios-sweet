#include "audio_live.h"
/* Single USB producer / single audio worker. No allocator, wait, lock,
 * logging, MMIO, driver invocation or secret storage in this transport. */
static struct {
 uint32_t enabled,boot[2],pending,token,op,generation,sequence,reply[NAL_WORDS];
} live;
static uint32_t get(const uint32_t*p){return __atomic_load_n(p,__ATOMIC_ACQUIRE);}
static void put(uint32_t*p,uint32_t n){__atomic_store_n(p,n,__ATOMIC_RELEASE);}
static void le32(uint8_t*p,uint32_t n){for(unsigned k=0;k<4;k++)p[k]=(uint8_t)(n>>(k*8));}
static uint32_t le16(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8;}
int nal_enabled(void){return get(&live.enabled)==1;}
int nal_initialize(uint64_t nonce){
 if(!nonce||get(&live.enabled))return -1;
 put(&live.boot[0],(uint32_t)nonce);put(&live.boot[1],(uint32_t)(nonce>>32));
 put(&live.enabled,1);return 0;
}
unsigned nal_gain_value(unsigned index){
 static const unsigned steps[NAL_GAIN_STEPS]={0,128,256,362,512,724,1024,2048,2896,4096,5793,8192,11585,16384,23170,32768};
 return index<NAL_GAIN_STEPS?steps[index]:UINT32_MAX;
}
int nal_gain_index_valid(unsigned encoded){
 return !(encoded&~255u) && (encoded&15u)<NAL_GAIN_STEPS &&
  ((encoded>>4)&15u)<NAL_GAIN_STEPS;
}
int nal_submit(unsigned value,unsigned index){
 if(!nal_enabled()||value>65535||index>65535)return 0;
 unsigned token=value&NAL_MAX_TOKEN,op=value>>12;
 if(!token||op<NAL_STATUS||op>NAL_TONE_BOTTOM)return 0;
 if(op==NAL_GAIN ? !nal_gain_index_valid(index) : index!=0)return 0;
 unsigned previous=get(&live.token);
 if(token==previous)return op==get(&live.op)&&index==get(&live.generation);
 if(previous==NAL_MAX_TOKEN||token!=previous+1||get(&live.pending))return 0;
 put(&live.op,op);put(&live.generation,index);put(&live.token,token);
 put(&live.pending,1);return 1;
}
int nal_take(struct nal_request*q){
 if(!q||!nal_enabled()||get(&live.pending)!=1)return 0;
 *q=(struct nal_request){get(&live.token),get(&live.op),get(&live.generation)};
 put(&live.pending,2);return 1;
}
int nal_complete(const struct nal_request*q,unsigned result,const struct nal_snapshot*s){
 if(!q||!s||result>NAL_LIMIT||get(&live.pending)!=2||
 q->token!=get(&live.token)||q->op!=get(&live.op)||q->generation!=get(&live.generation))return -1;
 uint32_t version=get(&live.sequence);put(&live.sequence,version+1);
 /* Strong word ordering also prevents reader loads crossing the version
  * checks; no plain concurrent memcpy or unbounded seqlock retry. */
 for(unsigned k=0;k<NAL_WORDS;k++)__atomic_store_n(&live.reply[k],s->word[k],__ATOMIC_SEQ_CST);
 const uint32_t prefix[]={NAL_MAGIC,NAL_VERSION,NAL_BYTES,get(&live.boot[0]),get(&live.boot[1]),q->token,q->op,q->generation,result};
 for(unsigned k=0;k<sizeof prefix/sizeof prefix[0];k++)__atomic_store_n(&live.reply[k],prefix[k],__ATOMIC_SEQ_CST);
 __atomic_store_n(&live.sequence,version+2,__ATOMIC_SEQ_CST);
 put(&live.pending,0);return 0;
}
size_t nal_caps(uint8_t*out,size_t size){
 if(!out||size<NAL_CAP_BYTES)return 0;
 uint32_t v[NAL_CAP_WORDS]={NAL_MAGIC,NAL_VERSION,NAL_CAP_BYTES,NAL_BYTES,
 NAL_COMMANDS,NAL_MAX_TOKEN,0u,0,0,0,0,0,0,0,0,0};
 /* Additive range metadata: last-op/argument remain at12/13. */
 v[14]=NAL_GAIN_MAX;v[15]=NAL_GAIN_STEPS;
 v[9]=nal_enabled();if(v[9]){v[7]=get(&live.boot[0]);v[8]=get(&live.boot[1]);
 v[10]=get(&live.pending);v[11]=get(&live.token);v[12]=get(&live.op);v[13]=get(&live.generation);}
 for(unsigned k=0;k<NAL_CAP_WORDS;k++)le32(out+k*4,v[k]);return NAL_CAP_BYTES;
}
size_t nal_reply(uint8_t*out,size_t size){
 if(!out||size<NAL_BYTES||!nal_enabled())return 0;
 uint32_t before=__atomic_load_n(&live.sequence,__ATOMIC_SEQ_CST);
 if(!before||(before&1))return 0;
 uint32_t v[NAL_WORDS];for(unsigned k=0;k<NAL_WORDS;k++)v[k]=__atomic_load_n(&live.reply[k],__ATOMIC_SEQ_CST);
 if(before!=__atomic_load_n(&live.sequence,__ATOMIC_SEQ_CST))return 0;
 for(unsigned k=0;k<NAL_WORDS;k++)le32(out+k*4,v[k]);return NAL_BYTES;
}
int nal_usb(const uint8_t setup[8],uint8_t*out,size_t capacity,uint32_t*length){
 if(!setup||!out||!length)return 0;unsigned code=setup[1];
 if(code<NAL_USB_CAP||code>NAL_USB_REPLY)return -1;
 *length=0;unsigned value=le16(setup+2),index=le16(setup+4),wanted=le16(setup+6);
 if(code==NAL_USB_COMMAND){if(setup[0]!=0x40||wanted)return 0;return nal_submit(value,index)?2:0;}
 if(setup[0]!=0xc0||value||index||!wanted)return 0;
 size_t n=code==NAL_USB_CAP?nal_caps(out,capacity):nal_reply(out,capacity);
 if(!n)return 0;*length=(uint32_t)(n<wanted?n:wanted);return 1;
}
