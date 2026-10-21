#include "wifi_wake_adapter.h"
static int proof(void *c){struct nawa *a=c;uint32_t v;
 if(a->asleep||a->hw->owner(a->c))return -1;
 /* Always-on AP only. NEVER HOST184/ncc_ap_check before source wake. */
 const uint32_t address[]={0x10006304,0x1000616c,0x10006170,0x1020e504,0x10001228,0x10001724};
 const uint32_t mask[]={0x1f,2,2,1,0x2000,4},want[]={0xd,2,2,1,0,0};
 for(unsigned i=0;i<6;i++){
  if(a->hw->read32(a->c,address[i],&v)||(v&mask[i])!=want[i])return -1;
 }
 return 0;
}
static int read(void *c,uint32_t p,uint32_t *v){struct nawa *a=c;unsigned phase=a->result.phase;
 if(a->asleep||!v||proof(a))return -1;
 if(p==0x180601a0){if(phase!=1)return -1;}
 else if(p==0x18060184){if(phase<2||phase>3)return -1;}
 else if(p==0x18060000||p==0x180602d4){if(phase!=3)return -1;}
 else if(p==0x18001000){if(phase!=4||!a->result.awake)return -1;}
 else if(p==0x18000010||p==0x18000000||p==0x180602cc||p==0x18000014||p==0x18000008){if(phase!=5||!a->result.awake||a->result.chip!=0x20010101)return -1;}
 else return -1;
 int e=a->hw->read32(a->c,p,v);
 if(!e&&p==0x18060000){a->last_clock=*v;a->clock_seen=1;}
 return e;
}
static int write(void *c,uint32_t p,uint32_t v){struct nawa *a=c;
 if(a->asleep||proof(a))return -1;
 if(p==0x180601a0){
  if((a->result.phase==2&&v==1)||(a->result.phase==6&&v==0)){
   int e=a->hw->write32(a->c,p,v);
   if(!e&&v==0)a->asleep=1;
   return e;
  }
 }else if(p==0x18060000&&a->result.phase==3&&a->clock_seen&&v==(a->last_clock|1u)){
  return a->hw->write32(a->c,p,v);
 }
 return -1;
}
static int delay(void *c,unsigned us){struct nawa *a=c;
 if(us!=1&&us!=10&&us!=1000)return -1;
 return proof(a)||a->hw->delay_us(a->c,us);
}
static const struct naw_ops ops={proof,read,write,delay};
int nawa_start(struct nawa *a,const struct nawa_hw *hw,void *c,unsigned opt){
 if(opt!=1||!a||a->result.entered||!hw||!hw->owner||!hw->read32||!hw->write32||!hw->delay_us)return -1;
 a->hw=hw;a->c=c;return naw_run(&ops,a,1,&a->result);
}
