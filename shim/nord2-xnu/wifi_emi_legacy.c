#include "wifi_emi_legacy.h"
int wifi_emi_legacy_offset(unsigned index,uint32_t *offset)
{
 if(!offset||index>=128)return -1;
 unsigned region=index/4,field=index%4;
 *offset=(field+1)*0x100+4*region;return 0;
}
int wifi_emi_legacy_args_valid(const uint64_t a[8])
{
 if(!a||a[0]!=WIFI_EMI_LEGACY_FID)return 0;
 for(unsigned j=2;j<8;j++)if(a[j])return 0;
 for(unsigned i=0;i<128;i++){uint32_t off;wifi_emi_legacy_offset(i,&off);if(a[1]==off)return 1;}
 return 0;
}
int wifi_emi_legacy_native_call(void *context,const uint64_t a[8],uint64_t result[4])
{
 (void)context;if(!result||!wifi_emi_legacy_args_valid(a))return -1;
#if defined(__aarch64__) && !defined(WIFI_EMI_LEGACY_NO_SMC)
 register uint64_t x0 __asm__("x0")=a[0],x1 __asm__("x1")=a[1];
 register uint64_t x2 __asm__("x2")=0,x3 __asm__("x3")=0;
 register uint64_t x4 __asm__("x4")=0,x5 __asm__("x5")=0;
 register uint64_t x6 __asm__("x6")=0,x7 __asm__("x7")=0;
 __asm__ volatile("smc #0" : "+r"(x0),"+r"(x1),"+r"(x2),"+r"(x3),
 "+r"(x4),"+r"(x5),"+r"(x6),"+r"(x7) : :
 "x8","x9","x10","x11","x12","x13","x14","x15","x16","x17","cc","memory");
 result[0]=x0;result[1]=x1;result[2]=x2;result[3]=x3;return 0;
#else
 return -1;
#endif
}
int wifi_emi_legacy_collect(wifi_emi_legacy_call_fn call,void *ctx,struct wifi_emi_legacy_snapshot *out)
{
 if(!call||!out)return -1;
 /* Avoid a6KiB temporary aggregate on kernel stack. */
 for(unsigned j=0;j<4;j++)out->valid[j]=0;
 out->issued=0;
 for(unsigned i=0;i<128;i++){
  struct wifi_emi_legacy_field *f=&out->field[i];
  for(unsigned j=0;j<4;j++)f->raw[j]=0;
  wifi_emi_legacy_offset(i,&f->offset);f->status=WIFI_EMI_LEGACY_NOT_CALLED;f->error=0;
 }
 for(unsigned i=0;i<128;i++){
  struct wifi_emi_legacy_field *f=&out->field[i];
  uint64_t a[8]={WIFI_EMI_LEGACY_FID,f->offset,0,0,0,0,0,0},raw[4]={0};
  out->issued++;
  if(call(ctx,a,raw)){f->status=WIFI_EMI_LEGACY_TRANSPORT;return 1;}
  for(unsigned j=0;j<4;j++)f->raw[j]=raw[j];
  for(unsigned e=1;e<=4;e++)if(raw[0]==UINT64_MAX-(e-1)||raw[0]==(uint64_t)(0u-e)){
   f->error=-(int32_t)e;f->status=WIFI_EMI_LEGACY_ERROR;return 1;
  }
  if(raw[0]>UINT32_MAX){f->status=WIFI_EMI_LEGACY_INVALID;return 1;}
  f->status=WIFI_EMI_LEGACY_VALUE;out->valid[i/32]|=UINT32_C(1)<<(i%32);
 }
 return 0;
}
