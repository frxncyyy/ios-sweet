#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <stdlib.h>
#include "wifi_rom_powered_adapter.h"
#include "wifi_persistent_adapter.h"
#include "wifi_wake_adapter.h"
#include "wifi_wfsys_adapter.h"
#include "pool_adapter.h"
typedef uintptr_t vm_offset_t;
typedef size_t vm_size_t;
typedef void *DTEntry;
enum {kSuccess=0};
typedef struct {uint64_t physBase,memSize,memSizeActual;} boot_args;
static boot_args ba={UINT64_C(0x142000000),UINT64_C(0x3e000000),UINT64_C(0x3e000000)};
static struct {boot_args *bootArgs;} PE_state={&ba};
static bool npa_persistent_lifetime,ni_saved,ncc_fresh_owned,ncc_latched_dirty;
static struct {vm_offset_t spm,infra,gals;bool saved,domain_touched;} nw;
static struct {struct npa state;vm_offset_t host,cfg,rgu;bool outer_retained;} npa_native;
static struct {struct nawa diag;bool active;} nawa_native;
static struct {struct nwsa diag;bool active;} nwsa_native;
static struct {struct nrpa diag;struct nrb_native rom;vm_offset_t local,rom_done;bool active,run_attempted;} nrpa_native;
static uint8_t claim_wire[24];
static uint64_t dram_base=UINT64_C(0x142000000),dram_size=UINT64_C(0x3e000000);
static struct {unsigned opt,reads,writes,maps,smcs,panics,logs,prearm,maparm,zeroarm;
 unsigned ap,wf,after_sleep,preflight_busy,smc_bad,map_bad,fail_read;
 uint32_t clock;uint32_t pool[0x20000/4];} m;
static jmp_buf jump;
static int test_printf(const char *fmt,...){m.logs++;if(strstr(fmt,"action=preflight"))m.prearm++;if(strstr(fmt,"action=map"))m.maparm++;if(strstr(fmt,"action=zero"))m.zeroarm++;return 0;}
static void panic(const char *fmt,...){(void)fmt;m.panics++;longjmp(jump,1);}
static void PE_parse_boot_argn(const char *name,unsigned *v,size_t n){assert(n==sizeof(*v));*v=m.opt;if(strcmp(name,"nord2wifidma")&&strcmp(name,"nord2wifirom"))assert(0);}
static int SecureDTLookupEntry(void *x,const char *path,DTEntry *e){(void)x;if(strcmp(path,"/chosen"))return -1;*e=&ba;return 0;}
static int SecureDTGetProperty(DTEntry e,const char *key,const void **p,unsigned *n){assert(e==&ba);
 if(!strcmp(key,"nord2-wifi-dma-claim")){*p=claim_wire;*n=24;return 0;}
 if(!strcmp(key,"dram-base")){*p=&dram_base;*n=8;return 0;}
 if(!strcmp(key,"dram-size")){*p=&dram_size;*n=8;return 0;}
 return -1;}
static vm_offset_t ml_io_map(uintptr_t p,size_t n){m.maps++;
 if(p==0x18024000){assert(n==0x2000);return p;}
 assert(p==NDP_POOL_BASE&&n==NDP_POOL_BYTES);
 return m.map_bad?0:(vm_offset_t)m.pool;
}
static uint32_t ni_rd(vm_offset_t base,unsigned off){assert(!m.after_sleep);m.reads++;if(m.fail_read&&m.reads==m.fail_read)return 0xffffffffu;
 uint32_t p=(uint32_t)base+off;
 switch(p){
 case 0x10006304:return 0xd;
 case 0x1000616c:case 0x10006170:return 2;
 case 0x1020e504:return 1;
 case 0x10001228:case 0x10001724:return 0;
 case 0x180601a0:return m.ap;
 case 0x180601a4:return m.wf;
 case 0x18060184:return 0;
 case 0x18060000:return m.clock;
 case 0x180602d4:return 1;
 case 0x18001000:return 0x20010101u;
 case 0x18000010:return 1;
 case 0x18000000:return 0x92;
 case 0x180602cc:return 0x582fb04;
 case 0x18001620:case 0x18001624:return 0;
 case 0x184b0010:return 0x20010000u;
 case 0x184c1604:return 0x1d1e;
 default:
  if(p>=0x18024000u&&p<0x18026000u)return m.preflight_busy&&p==0x18025300u?1u:0;
  assert(0);return 0;
 }
}
static void ni_wr(vm_offset_t base,unsigned off,uint32_t v){assert(!m.after_sleep);m.writes++;uint32_t p=(uint32_t)base+off;
 if(p==0x180601a0){assert(v<=1);m.ap=v;if(!v){assert(!m.wf);m.after_sleep=1;}return;}
 if(p==0x180601a4){assert(m.ap);m.wf=v;return;}
 if(p==0x18060000){m.clock=v|6u;return;}
 assert(0);
}
static void delay(unsigned us){assert(!m.after_sleep&&(us==1||us==10||us==500||us==1000));}
static int policy_receipt(const struct nrb_native *n) {
 const struct nrb_state *r=&n->state;
 return r->prepared&&r->policy_attempted==1&&
  r->issued==2&&r->accepted==2&&r->raw_valid==3&&
  !r->raw[0][0]&&!r->raw[1][0]&&!r->halt&&!r->error;
}
int nrpc_receipt_ok(const struct nrb_native *n) {
 if(!n)return 0;
 const struct nrb_state *r=&n->state;
 unsigned chunks=0,bytes=0;
 for(unsigned i=0;i<NWF_ROM_SPANS;i++){
  unsigned sz=r->plan.spans[i].bytes;
  if(!sz||sz>NRB_BACKUP_BYTES-bytes)return 0;
  bytes+=sz;chunks+=(sz+NRB_CHUNK-1u)/NRB_CHUNK;
 }
 if(bytes!=NRB_BACKUP_BYTES)return 0;
 return policy_receipt(n)&&r->phase==7&&r->placed==1&&r->mapped==1&&
  r->backup_complete==1&&r->write_attempted==1&&r->barriers==2&&
  r->writes==chunks&&r->reads==2u*chunks;
}

static int nppa_host_smc(const uint64_t x[8],uint64_t out[4]){
 assert(x[0]==0xc200050bu&&x[1]==0&&x[2]==0x12009000u);
 assert((m.smcs==0&&x[3]==0x010090f1u&&x[4]==0x00b6db6du)||
        (m.smcs==1&&x[3]==0x000090f1u&&x[4]==0x80b6da28u));
 m.smcs++;out[0]=m.smc_bad?3:0;out[1]=0x123;out[2]=0x456;out[3]=0x789;return 0;
}
#define NCC_DEVICE_BARRIER() ((void)0)
#define NPP_NATIVE_HOST_TEST 1
#define printf test_printf
#include "pool_bind.inc"
#undef printf
static void reset(void){m=(__typeof__(m)){.opt=1,.clock=0x4006};
 for(unsigned i=0;i<0x20000/4;i++)m.pool[i]=0xaaaaaaaau;
 struct ndp_claim claim={NDP_POOL_BASE,NDP_POOL_BYTES,NDP_SOURCE_IOS_FIXED,1};
 assert(ndp_claim_encode(&claim,claim_wire)==0);
 nw=(__typeof__(nw)){.spm=0x10006000,.infra=0x10001000,.gals=0x1020e000,.saved=1,.domain_touched=1};
 npa_native=(__typeof__(npa_native)){.host=0x18060000,.cfg=0x18001000,.rgu=0x18000000,.outer_retained=1};
 npa_native.state.entered=npa_native.state.retained=1;npa_native.state.hw_handoff=2;
 npa_native.state.result.phase=10;npa_native.state.result.retained=1;
 nawa_native=(__typeof__(nawa_native)){0};nwsa_native=(__typeof__(nwsa_native)){0};
 nrpa_native=(__typeof__(nrpa_native)){.local=0x184b0000,.rom_done=0x184c1000,.run_attempted=1};
 nrpa_native.diag.asleep=1;
 nrpa_native.diag.state.entered=1;nrpa_native.diag.state.stage=NRPC_RUNNING;
 nrpa_native.diag.state.rom_ready=nrpa_native.diag.state.cpu_release_attempted=1;
 nrpa_native.diag.state.cpu_released=nrpa_native.diag.state.power=1;
 nrpa_native.diag.state.protection=nrpa_native.diag.state.balanced=1;
 nrpa_native.diag.state.rom_done=0x1d1e;
 nrpa_native.rom.state.phase=7;nrpa_native.rom.state.placed=1;
 nrpa_native.rom.claim=(struct nrc_claim){.source=NRC_CLAIM_IOS_FIXED,.base=NRC_CONN_BASE,.bytes=NRC_CONN_BYTES};
 nppa_native=(__typeof__(nppa_native)){0};
 npa_persistent_lifetime=ni_saved=ncc_fresh_owned=1;ncc_latched_dirty=0;
}
static void full_reset(void){
 reset();ba=(boot_args){UINT64_C(0x142000000),UINT64_C(0x3e000000),UINT64_C(0x3e000000)};dram_base=ba.physBase;dram_size=ba.memSize;
 struct nrb_state *r=&nrpa_native.rom.state;
 r->prepared=r->policy_attempted=r->mapped=r->backup_complete=r->write_attempted=1;
 r->issued=r->accepted=r->barriers=2;r->raw_valid=3;
 unsigned used=0,chunks=0;
 for(unsigned i=0;i<NWF_ROM_SPANS;i++){unsigned n=i+1==NWF_ROM_SPANS?NRB_BACKUP_BYTES-used:NRB_BACKUP_BYTES/NWF_ROM_SPANS;r->plan.spans[i].bytes=n;used+=n;chunks+=(n+NRB_CHUNK-1)/NRB_CHUNK;}
 r->writes=chunks;r->reads=2*chunks;assert(nrpc_receipt_ok(&nrpa_native.rom));
}
static void no_hardware(void){nppa_after_rom();assert(!m.maps&&!m.reads&&!m.writes&&!m.smcs);}
int main(void){
 full_reset();nppa_after_rom();assert(!m.panics&&m.smcs==2&&m.after_sleep&&nppa_native.probe.state.verified);
 for(unsigned i=0;i<24;i++)for(unsigned bit=0;bit<8;bit++){full_reset();claim_wire[i]^=1u<<bit;no_hardware();}
 full_reset();nrpa_native.rom.state.accepted=1;no_hardware();
 full_reset();nrpa_native.rom.state.reads--;no_hardware();
 full_reset();nrpa_native.rom.state.raw[1][0]=1;no_hardware();
 full_reset();nrpa_native.rom.state.backup_complete=0;no_hardware();
 full_reset();nrpa_native.rom.state.plan.spans[0].bytes--;no_hardware();
 full_reset();npa_native.state.fatal=1;no_hardware();
 full_reset();npa_native.state.hw_handoff=1;no_hardware();
 full_reset();nrpa_native.diag.state.rom_done=0;no_hardware();
 full_reset();nrpa_native.diag.state.cpu_released=0;no_hardware();
 full_reset();nrpa_native.diag.state.ap_vote=1;no_hardware();
 full_reset();ba.memSizeActual--;no_hardware();
 full_reset();dram_base--;no_hardware();
 full_reset();nppa_native.active=nppa_native.prepared=1;nppa_native.pool=(vm_offset_t)m.pool;
 nppa_native.probe.state.set_accepted=2;nppa_native.probe.state.raw_valid=3;nppa_native.probe.state.stage=NPP_ZERO;
 uint32_t z=0;assert(!nppa_xnu_word(0,0,&z,1));assert(!nppa_xnu_word(0,NDP_SUBPOOL_BYTES-4,&z,1));
 assert(m.pool[0]==0&&m.pool[32767]==0&&m.pool[1]==0xaaaaaaaa&&m.pool[32766]==0xaaaaaaaa);
 assert(nppa_xnu_word(0,NDP_SUBPOOL_BYTES,&z,1));assert(nppa_xnu_word(0,UINT32_MAX,&z,1));assert(nppa_xnu_word(0,3,&z,1));
 z=1;assert(nppa_xnu_word(0,4,&z,1));assert(m.pool[1]==0xaaaaaaaa);
 nppa_native.probe.state.stage=NPP_VERIFY;assert(!nppa_xnu_word(0,0,&z,0)&&z==0);assert(!nppa_xnu_word(0,NDP_SUBPOOL_BYTES-4,&z,0)&&z==0);
 assert(nppa_xnu_word(0,NDP_SUBPOOL_BYTES,&z,0));
 puts("PASS reviewer actual receipt/body identity guards, all192 claim wire mutations, boot/persistent/ROM refusal before MMIO, first/last 128KiB word bounds");return 0;
}
