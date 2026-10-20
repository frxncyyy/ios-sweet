/* Original isolated observation helper. ABI references: selected Android
 * drivers/memory/mediatek/emimpu_old.c:43–50,80–110; include/memory/mediatek/emi.h;
 * include/linux/soc/mediatek/mtk_sip_svc.h. Native SMC preserves x0..3 as Linux
 * arch/arm64/kernel/smccc-call.S does. No hardware call occurs at load time. */
#include "wifi_emi_snapshot.h"
static int valid_args(const uint64_t a[8])
{
 return a && a[0]==NWES_SMC_ID && a[1]==2 && a[2]<=2 && a[3]<32 &&
  a[4]<(a[2]==2?2u:1u) && !a[5] && !a[6] && !a[7];
}
int nwes_native_call(void *context,const uint64_t a[8],uint64_t result[4])
{
 (void)context;
 if(!result || !valid_args(a)) return -1;
#if defined(__aarch64__)
 register uint64_t x0 __asm__("x0")=a[0],x1 __asm__("x1")=a[1];
 register uint64_t x2 __asm__("x2")=a[2],x3 __asm__("x3")=a[3];
 register uint64_t x4 __asm__("x4")=a[4],x5 __asm__("x5")=0;
 register uint64_t x6 __asm__("x6")=0,x7 __asm__("x7")=0;
 /* x0..7 are read/write operands; x8..17 conservatively clobbered, matching
  * the existing audited XNU PSCI pattern. x18 is platform-reserved. */
 __asm__ volatile("smc #0" : "+r"(x0),"+r"(x1),"+r"(x2),"+r"(x3),
  "+r"(x4),"+r"(x5),"+r"(x6),"+r"(x7) : :
  "x8","x9","x10","x11","x12","x13","x14","x15","x16","x17","cc","memory");
 result[0]=x0;result[1]=x1;result[2]=x2;result[3]=x3;
 return 0;
#else
 return -1;
#endif
}
static void classify(struct nwes_field *f,unsigned field)
{
 uint64_t raw=f->raw[0]; uint32_t v=(uint32_t)raw; f->value=v;
 /* Preserve both common sign-extended and zero-extended32 SiP errors. */
 for(unsigned e=1;e<=4;e++) {
  if(raw==(uint64_t)(0u-e) || raw==UINT64_MAX-(e-1)) {
   f->known_error=-(int32_t)e;f->status=NWES_KNOWN_ERROR;return;
  }
 }
 /* ABI's register values are32-bit. Do not silently discard unknown upper
  * bits. SA/EA are64KiB units within this device's0x40000000..0x23fffffff.
  * APC has8 three-bit fields and optional group0 lock bit31. */
 uint32_t allowed=field<2?0x1ffffu:(field==2?0x80ffffffu:0x00ffffffu);
 f->status=(raw>UINT32_MAX || (v&~allowed))?NWES_INVALID_VALUE:NWES_VALUE;
}
int nwes_collect(nwes_call_fn call,void *ctx,const uint32_t *regions,size_t count,
 struct nwes_region *output,size_t output_count)
{
 if(!call || !regions || !output || !count || count>32 || output_count<count) return -1;
 for(size_t i=0;i<count;i++) {
  if(regions[i]>=32) return -1;
  for(size_t j=0;j<i;j++) if(regions[i]==regions[j]) return -1;
 }
 for(size_t i=0;i<count;i++) {
  struct nwes_region zero={0};zero.region=regions[i];output[i]=zero;
 }
 for(size_t i=0;i<count;i++) for(unsigned f=0;f<4;f++) {
  uint64_t a[8]={NWES_SMC_ID,2,f<2?f:2,regions[i],f==3?1:0,0,0,0};
  struct nwes_field *v=&output[i].field[f];uint64_t raw[4]={0};
  if(call(ctx,a,raw)) { v->status=NWES_TRANSPORT_ERROR;return 1; }
  for(unsigned j=0;j<4;j++) v->raw[j]=raw[j];
  classify(v,f);
  if(v->known_error==-1) return 1;
 }
 return 0;
}
int nwes_decode_range(const struct nwes_region *r,uint64_t base,uint64_t limit,
 unsigned shift,uint64_t *start,uint64_t *end)
{
 if(!r || !start || !end || start==end || base>limit || shift>=64 ||
  r->field[0].status!=NWES_VALUE || r->field[1].status!=NWES_VALUE) return -1;
 uint64_t a=r->field[0].value,b=r->field[1].value;
 if(a>b || b>(UINT64_MAX>>shift)) return -1;
 uint64_t tail=shift?((UINT64_C(1)<<shift)-1):0;
 a<<=shift;b<<=shift;
 if(a>UINT64_MAX-base || b>UINT64_MAX-base || b+base>UINT64_MAX-tail) return -1;
 a+=base;b+=base+tail;
 if(b>limit) return -1;
 *start=a;*end=b;return 0;
}
int nwes_domain(const struct nwes_region *r,unsigned domain,uint32_t *permission)
{
 if(!r || !permission || domain>=16) return -1;
 const struct nwes_field *f=&r->field[2+domain/8];
 if(f->status!=NWES_VALUE) return -1;
 *permission=(f->value>>(3*(domain%8)))&7u;return 0;
}
