#include "wifi_main_phy_xnu.h"
#include "wifi_main_wfdma_native.h"
#include "wifi_main_halt.h"
#include "wifi_rom_xnu.h"
#include <sys/errno.h>
#include <libkern/libkern.h>
#include <kern/kalloc.h>
#include <kern/zalloc.h>
#include <pexpert/pexpert.h>
#include <pexpert/device_tree.h>
#include <pexpert/arm64/boot.h>
extern vm_offset_t ml_io_map(vm_offset_t,vm_size_t);
struct nfa_native nfa_native;
static int nfa_native_halt(void){
 nfa_native.exchange.halt=1;
 return nwm_lifetime_halt(&nwfn_native);
}
static uint32_t be32(const volatile uint8_t *p){
 return (uint32_t)p[0]<<24|(uint32_t)p[1]<<16|(uint32_t)p[2]<<8|p[3];
}
static int overlaps(uint64_t a,uint64_t n,uint64_t b,uint64_t m){
 return a<b+m&&b<a+n;
}
int nfa_xnu_prepare(struct nfa_native *n){
 unsigned main_flag=0,rom_flag=0,dma_flag=0;
 DTEntry chosen;const void *p=0,*wifi=0;
 unsigned pn=0,wn=0;uint64_t pa=0;
 if(!PE_parse_boot_argn("nord2wifimain",&main_flag,sizeof(main_flag))||main_flag!=1||
    !PE_parse_boot_argn("nord2wifirom",&rom_flag,sizeof(rom_flag))||rom_flag!=1||
    !PE_parse_boot_argn("nord2wifidma",&dma_flag,sizeof(dma_flag))||dma_flag!=1)return EPERM;
 if(n!=&nfa_native||n->attempted)return EALREADY;
 n->attempted=1;
 if(SecureDTLookupEntry(NULL,"/chosen",&chosen)!=kSuccess||
    SecureDTGetProperty(chosen,"nord2-stock-dtb",&p,&pn)!=kSuccess||
    pn!=8||!p||
    SecureDTGetProperty(chosen,"nord2-wifi-calibration",&wifi,&wn)!=kSuccess||
    wn!=NFA_WIFI_BYTES||!wifi)return EINVAL;
 memcpy(&pa,p,8);
 if((pa&3)||pa<UINT64_C(0x40000000)||
    pa>UINT64_C(0x100000000)-UINT64_C(0x100000)||
    overlaps(pa,UINT64_C(0x100000),UINT64_C(0x8fb00000),UINT64_C(0x450000))||
    overlaps(pa,UINT64_C(0x100000),UINT64_C(0x90000000),UINT64_C(0xf20000)))return EINVAL;
 uintptr_t mapped=(uintptr_t)ml_io_map((vm_offset_t)pa,(vm_size_t)0x100000);
 if(!mapped)return ENOMEM;
 n->original_fdt_io=mapped;n->original_fdt_pa=pa;
 volatile const uint8_t *f=(volatile const uint8_t *)mapped;
 if(be32(f)!=UINT32_C(0xd00dfeed))return EINVAL;
 uint32_t bytes=be32(f+4);
 if(bytes<40||bytes>0x100000||
    overlaps(pa,bytes,UINT64_C(0x8fb00000),UINT64_C(0x450000))||
    overlaps(pa,bytes,UINT64_C(0x90000000),UINT64_C(0xf20000)))return EINVAL;
 n->original_fdt_bytes=bytes;
 if(nfa_prepare(wifi,wn,(const void *)mapped,bytes,&n->board))return EINVAL;
 n->backup_memory=kalloc_data(UINT32_C(0x450000),Z_WAITOK|Z_NOPAGEWAIT);
 if(!n->backup_memory)return ENOMEM;
 n->backup_capacity=UINT32_C(0x450000);
 n->prepared=1;return 0;
}
static int cal_owner(void *c){
 struct nfa_native *n=c;
 return n!=&nfa_native||!n->prepared||!n->backup_memory||
    !nwfn_native.rom||!nwfn_native.rom->io||
    nwfn_native.init.transport.finish_phase!=1||
    nwfn_native.init.transport.completed>2||
    nwfn_owner(&nwfn_native);
}
static int cal_read(void *c,uint32_t off,void *dst,uint32_t bytes){
 if(cal_owner(c)||!dst||!bytes||off>=UINT32_C(0x450000)||
    bytes>UINT32_C(0x450000)-off)return -1;
 uint8_t *out=dst;
 for(uint32_t i=0;i<bytes;){
  uint32_t at=off+i,aligned=at&~3u;
  uint32_t word=*(volatile const uint32_t *)
   (nwfn_native.rom->io+(uintptr_t)aligned);
  do {
   out[i]=(uint8_t)(word>>((at&3u)*8u));
   i++;at++;
  }while(i<bytes&&(at&3u));
 }
 __asm__ volatile("dsb sy":: :"memory");
 return 0;
}
static int cal_barrier(void *c){
 if(cal_owner(c))return -1;
 __asm__ volatile("dsb sy":: :"memory");return 0;
}
static const struct nfa_backup_io cal_io={cal_owner,cal_read,cal_barrier};
int nfa_xnu_run(void){
 if(!nfa_native.prepared||!nwfn_native.init.initialized||
    nwfn_native.stage!=NWFN_RETAINED||nwfn_owner(&nwfn_native))return nfa_native_halt();
 int result=nfa_run(&nwfn_io,&nwfn_native,&nwfn_native.init,&nfa_native.board,
                &cal_io,&nfa_native,nfa_native.backup_memory,
                nfa_native.backup_capacity,&nfa_native.exchange);
 return result?nfa_native_halt():0;
}
