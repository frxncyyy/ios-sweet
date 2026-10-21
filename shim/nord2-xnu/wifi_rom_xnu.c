/* PRIVATE native candidate, no boot hook or invocation in active tree. */
#include "wifi_rom_xnu.h"
#ifdef NRB_NATIVE_HOST_TEST
#include "nord2-wifi-rom-host-platform.h"
#else
#include <sys/errno.h>
#include <sys/systm.h>
#include <kern/kalloc.h>
#include <kern/zalloc.h>
#include <pexpert/pexpert.h>
#include <pexpert/device_tree.h>
#include <pexpert/arm64/boot.h>
#endif
#ifndef NRB_DEVICE_BARRIER
#define NRB_DEVICE_BARRIER() __asm__ volatile("dsb sy":::"memory")
#endif
extern int sweet_ufs_read_vendor(uint64_t,void *,uint32_t);
extern vm_offset_t ml_io_map(vm_offset_t,vm_size_t);
static int layout(void){
 const boot_args *b=(const boot_args *)PE_state.bootArgs;
 /* Loader/XNU physical window pinned by selected build. No currentCONNPA
  * alias may be introduced by other code; closed-build aliasaudit inREADME. */
 DTEntry entry;const void *base=0,*size=0;unsigned nb=0,ns=0;
 if(!b||b->physBase!=UINT64_C(0x142000000)||b->memSize!=UINT64_C(0x3e000000)||b->memSizeActual!=b->memSize)return -1;
 if(SecureDTLookupEntry(NULL,"/chosen",&entry)!=kSuccess||
    SecureDTGetProperty(entry,"dram-base",&base,&nb)!=kSuccess||
    SecureDTGetProperty(entry,"dram-size",&size,&ns)!=kSuccess||nb!=8||ns!=8||!base||!size)return -1;
 uint64_t db=0,ds=0;memcpy(&db,base,8);memcpy(&ds,size,8);
 return db!=b->physBase||ds!=b->memSize;
}
static int claim_read(struct nrc_claim *claim){
 DTEntry entry;const void *p=0;unsigned n=0;
 if(SecureDTLookupEntry(NULL,"/chosen",&entry)!=kSuccess||SecureDTGetProperty(entry,"nord2-conn-claim",&p,&n)!=kSuccess)return -1;
 return nrb_claim_decode(p,n,claim);
}
static int vendor(void *cookie,uint64_t offset,void *data,uint32_t bytes){
 struct nrb_native *n=cookie;int e=sweet_ufs_read_vendor(offset,data,bytes);if(e)n->supply_error=e;return e;
}
static void erase(void *p,size_t size){volatile unsigned char *v=p;if(v)while(size--)*v++=0;}
int nrb_xnu_prepare(struct nrb_native *n){
 unsigned opt=0;
 if(!PE_parse_boot_argn("nord2wifirom",&opt,sizeof(opt))||opt!=1)return EPERM;
 if(!n||n->attempted)return EALREADY;
 n->attempted=1;
 if(layout()||claim_read(&n->claim))return EINVAL;
 unsigned char *decoder=0;int e=ENOMEM;
 n->output=kalloc_data(NWF_OUTPUT_BYTES,Z_WAITOK|Z_NOPAGEWAIT);
 decoder=kalloc_data(NWF_SCRATCH_BYTES,Z_WAITOK|Z_NOPAGEWAIT);
 n->backup=kalloc_data(NRB_BACKUP_BYTES,Z_WAITOK|Z_NOPAGEWAIT);
 n->scratch=kalloc_data(NRB_CHUNK,Z_WAITOK|Z_NOPAGEWAIT);
 if(!n->output||!decoder||!n->backup||!n->scratch)goto fail;
 if(nwf_load(vendor,n,n->output,NWF_OUTPUT_BYTES,decoder,NWF_SCRATCH_BYTES,n->blobs)){e=n->supply_error?n->supply_error:EIO;goto fail;}
 /* Actual authenticated loader output order: 3=WMMCU,2=WiFi. */
 if(nrb_prepare(&n->state,1,&n->claim,n->blobs[3].data,n->blobs[3].size,
    n->blobs[2].data,n->blobs[2].size,n->backup,NRB_BACKUP_BYTES,n->scratch,NRB_CHUNK)){e=EINVAL;goto fail;}
 erase(decoder,NWF_SCRATCH_BYTES);kfree_data(decoder,NWF_SCRATCH_BYTES);
 n->enabled=1;return 0;
fail:
 if(decoder){erase(decoder,NWF_SCRATCH_BYTES);kfree_data(decoder,NWF_SCRATCH_BYTES);}
 if(n->scratch){erase(n->scratch,NRB_CHUNK);kfree_data(n->scratch,NRB_CHUNK);n->scratch=0;}
 if(n->backup){erase(n->backup,NRB_BACKUP_BYTES);kfree_data(n->backup,NRB_BACKUP_BYTES);n->backup=0;}
 if(n->output){erase(n->output,NWF_OUTPUT_BYTES);kfree_data(n->output,NWF_OUTPUT_BYTES);n->output=0;}
 return e;
}
static int owner(void *cookie,enum nrb_owner_stage stage){
 struct nrb_native *n=cookie;struct nrc_claim now;
 if(!n||!n->enabled||!n->fresh_owner||layout()||claim_read(&now)||now.source!=n->claim.source||now.base!=n->claim.base||now.bytes!=n->claim.bytes)return -1;
 return n->fresh_owner(n->owner_cookie,stage);
}
static int smc(void *cookie,const uint64_t a[8],uint64_t out[4]){
 struct nrb_native *n=cookie;struct nrc_smc_request expected[2];
 if(!n||!a||!out||n->state.phase!=2||n->state.issued<1||n->state.issued>2||nrc_region27_set_intent(&n->claim,expected))return -1;
 for(unsigned i=0;i<8;i++)if(a[i]!=expected[n->state.issued-1].x[i])return -1;
#ifdef NRB_NATIVE_HOST_TEST
 return nrb_host_smc(a,out);
#elif defined(__aarch64__)
 /* Captured request only, logged immediately before this one-way monitor call. */
 printf("NORD2 WIFI ROM SMC ARM: group=%u claim=%#llx/%#llx x0=%#llx x1=%#llx x2=%#llx x3=%#llx x4=%#llx\n",
  n->state.issued==1?1u:0u,(unsigned long long)n->claim.base,
  (unsigned long long)n->claim.bytes,(unsigned long long)a[0],
  (unsigned long long)a[1],(unsigned long long)a[2],
  (unsigned long long)a[3],(unsigned long long)a[4]);
 register uint64_t x0 __asm__("x0")=a[0],x1 __asm__("x1")=a[1];
 register uint64_t x2 __asm__("x2")=a[2],x3 __asm__("x3")=a[3];
 register uint64_t x4 __asm__("x4")=a[4],x5 __asm__("x5")=0;
 register uint64_t x6 __asm__("x6")=0,x7 __asm__("x7")=0;
 __asm__ volatile("smc #0" : "+r"(x0),"+r"(x1),"+r"(x2),"+r"(x3),
  "+r"(x4),"+r"(x5),"+r"(x6),"+r"(x7) : :
  "x8","x9","x10","x11","x12","x13","x14","x15","x16","x17","cc","memory");
 out[0]=x0;out[1]=x1;out[2]=x2;out[3]=x3;return 0;
#else
 return -1;
#endif
}
static int map(void *cookie,uint64_t base,uint64_t size){struct nrb_native *n=cookie;
 if(!n||n->io||n->state.accepted!=2||n->state.raw_valid!=3||base!=NRC_CONN_BASE||size!=NRC_CONN_BYTES||owner(n,NRB_POWERED_CPU_HELD))return -1;
 n->io=(uintptr_t)ml_io_map((vm_offset_t)base,(vm_size_t)size);
 return n->io?0:-1;
}
static int span(struct nrb_native *n,uint64_t pa,uint32_t bytes){
 if(!n||!n->io||!bytes||bytes>NRB_CHUNK||(pa&3)||(bytes&3)||n->state.accepted!=2||!n->state.mapped||n->state.halt)return -1;
 for(unsigned i=0;i<NWF_ROM_SPANS;i++){const struct nwf_rom_span *s=&n->state.plan.spans[i];
  if(pa>=s->physical&&pa-s->physical<=s->bytes&&bytes<=s->bytes-(pa-s->physical))return 0;
 }
 return -1;
}
static int read(void *cookie,uint64_t pa,void *data,uint32_t bytes){struct nrb_native *n=cookie;
 if(!data||span(n,pa,bytes)||owner(n,NRB_POWERED_CPU_HELD))return -1;
 unsigned char *p=data;volatile const uint32_t *io=(volatile const uint32_t *)(n->io+(uintptr_t)(pa-NRC_CONN_BASE));
 for(uint32_t i=0;i<bytes;i+=4){uint32_t v=*io++;p[i]=(unsigned char)v;p[i+1]=(unsigned char)(v>>8);p[i+2]=(unsigned char)(v>>16);p[i+3]=(unsigned char)(v>>24);}
 NRB_DEVICE_BARRIER();return 0;
}
static int write(void *cookie,uint64_t pa,const void *data,uint32_t bytes){struct nrb_native *n=cookie;
 if(!data||span(n,pa,bytes)||owner(n,NRB_POWERED_CPU_HELD))return -1;
 const unsigned char *p=data;volatile uint32_t *io=(volatile uint32_t *)(n->io+(uintptr_t)(pa-NRC_CONN_BASE));
 for(uint32_t i=0;i<bytes;i+=4)*io++=(uint32_t)p[i]|(uint32_t)p[i+1]<<8|(uint32_t)p[i+2]<<16|(uint32_t)p[i+3]<<24;
 NRB_DEVICE_BARRIER();return 0;
}
static int barrier(void *cookie){struct nrb_native *n=cookie;if(owner(n,NRB_POWERED_CPU_HELD))return -1;
 NRB_DEVICE_BARRIER();return 0;
}
static const struct nrb_ops ops={owner,smc,map,read,write,barrier};
int nrb_xnu_policy(struct nrb_native *n,int (*check)(void *,enum nrb_owner_stage),void *c){
 if(!n||!n->enabled||!check||n->fresh_owner)return -1;
 n->fresh_owner=check;n->owner_cookie=c;return nrb_policy(&ops,n,&n->state);
}
int nrb_xnu_place(struct nrb_native *n){if(!n||!n->enabled)return -1;return nrb_place(&ops,n,&n->state);}
