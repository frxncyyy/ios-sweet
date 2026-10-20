#!/usr/bin/env python3
"""Exact guarded vendor-reader C with private LP/GPT fixtures and real SHA256.

No device access. Private fixture binaries stay outside git. The backup GPT
header is synthesized from the saved primary using the standard mirror offsets;
a differing actual backup fails closed on hardware.
"""
from pathlib import Path
import re
import subprocess
import tempfile

root=Path(__file__).resolve().parent.parent
s=(root/'research/ci-39158169212/buildtree/xnu/bsd/dev/sweet_ufs.c').read_text()
a=s.index('/* Immutable read-only vendor mapping.')
b=s.index('\nstatic bool\nnord2_ufs_wait(uint32_t off, uint32_t mask, uint32_t value)\n{',a)
production=s[a:b]
a=s.index('static uint32_t\nnord2_ufs_crc32(')
b=s.index('/* Inspect only GPT metadata;',a)
helpers=s[a:b]
defines='\n'.join(re.findall(r'^#define (?:REG_\w+|HCS_\w+|UFS_BLOCK|BOUNCE_SIZE|DMA_\w+)\s+.*$',s,re.M))
assert 'nord2_ufs_vendor_enabled = (firmware == 1);' in s
assert 'PE_parse_boot_argn("nord2wififw"' in s
# The reader deliberately cannot call writes, root selection, or mutate bindings.
for forbidden in ('ufs_write_blocks(', 'ufs_wr(', 'nord2_ufs_read_gpt(',
                  'su.first_block =','su.nblocks =','su.writable =','su.lun ='):
    assert forbidden not in production
code=r'''
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <openssl/sha.h>
#define bzero(p,n) memset(p,0,n)
'''+defines+r'''
static uint8_t dma[DMA_SIZE];
static struct { uintptr_t dma; uint64_t dma_pa, first_block, nblocks;
 bool ready, probe_only, writable; uint8_t lun; } su;
static bool nord2_root_owned, nord2_ufs_uic_pending;
static int sweet_ufs_lock, held, interrupts, irq_context, preempt;
static unsigned reads, fail_at, mmio_reads;
static uint32_t registers[64];
static uint8_t primary[4096], backup[4096], entries[16384];
static uint8_t geometry0[4096], geometry1[4096], metadata0[65536], metadata1[65536];
static uint64_t capacity_blocks=31240192ull;
static uint32_t capacity_size=4096;
static uint32_t ufs_rd(uint32_t off) { assert(held && off/4<64); mmio_reads++; return registers[off/4]; }
static void lck_mtx_lock(int *l) { assert(l==&sweet_ufs_lock && !held); held=1; }
static void lck_mtx_unlock(int *l) { assert(l==&sweet_ufs_lock && held); held=0; }
static bool ml_get_interrupts_enabled(void) { return interrupts; }
static bool ml_at_interrupt_context(void) { return irq_context; }
static int get_preemption_level(void) { return preempt; }
static void putbe(uint8_t *b,uint32_t v) { b[0]=v>>24;b[1]=v>>16;b[2]=v>>8;b[3]=v; }
static int ufs_scsi(const uint8_t *cdb,unsigned n,uint32_t len,bool write) {
 assert(held && !write && n==10 && cdb[0]==0x25 && len==8);
 for(unsigned i=1;i<10;i++) assert(cdb[i]==0);
 reads++; if(reads==fail_at) return EIO;
 uint8_t *b=(void *)(su.dma+DMA_BOUNCE);
 putbe(b,(uint32_t)(capacity_blocks-1));putbe(b+4,capacity_size);return 0;
}
static int ufs_read_blocks(uint64_t lba,uint32_t blocks) {
 assert(held && su.lun==2 && blocks>0 && blocks<=17);
 reads++; if(reads==fail_at) return EIO;
 uint8_t *out=(void *)(su.dma+DMA_BOUNCE); size_t n=(size_t)blocks*4096;
 if(lba==1 && blocks==1) memcpy(out,primary,n);
 else if(lba==31240191 && blocks==1) memcpy(out,backup,n);
 else if((lba==2 || lba==31240187) && blocks==4) memcpy(out,entries,n);
 else if(lba==344064+1 && blocks==1) memcpy(out,geometry0,n);
 else if(lba==344064+2 && blocks==1) memcpy(out,geometry1,n);
 else if(lba==344064+3 && blocks==16) memcpy(out,metadata0,n);
 else if(lba==344064+35 && blocks==16) memcpy(out,metadata1,n);
 else {
  uint64_t physical=(lba-344064)*4096ull;
  assert(lba>=344064 && physical>=7708082176ull &&
         physical+n<=7708082176ull+544354304ull);
  for(size_t i=0;i<n;i++) out[i]=(uint8_t)(physical+i);
 }
 return 0;
}
'''+helpers+production+r'''
static void crc_header(uint8_t *h) {
 uint32_t zero=0;memcpy(h+16,&zero,4);uint32_t crc=nord2_ufs_crc32(h,92);memcpy(h+16,&crc,4);
}
static void load_file(const char *dir,const char *name,void *buffer,size_t len) {
 char p[4096];assert(snprintf(p,sizeof(p),"%s/%s",dir,name)>0);
 FILE *f=fopen(p,"rb");assert(f);assert(fread(buffer,1,len,f)==len);assert(fgetc(f)==EOF);fclose(f);
}
static void fixtures(const char *lp,const char *gpt) {
 load_file(lp,"geometry-primary.bin",geometry0,sizeof(geometry0));
 load_file(lp,"geometry-backup.bin",geometry1,sizeof(geometry1));
 load_file(lp,"metadata-primary.bin",metadata0,sizeof(metadata0));
 load_file(lp,"metadata-backup.bin",metadata1,sizeof(metadata1));
 load_file(gpt,"header.bin",primary,sizeof(primary));
 load_file(gpt,"entries.bin",entries,sizeof(entries));
 memcpy(backup,primary,sizeof(backup));
 uint64_t v=31240191;memcpy(backup+24,&v,8);v=1;memcpy(backup+32,&v,8);
 v=31240187;memcpy(backup+72,&v,8);crc_header(backup);
}
static void reset(void) {
 memset(dma,0,sizeof(dma)); memset(registers,0,sizeof(registers));
 su.dma=(uintptr_t)dma;su.dma_pa=0x145000000ull;
 su.first_block=3153920;su.nblocks=28082168;su.lun=2;su.writable=true;
 su.ready=true;su.probe_only=false;nord2_root_owned=true;nord2_ufs_uic_pending=false;
 nord2_ufs_vendor_enabled=true;nord2_ufs_vendor_checked=false;nord2_ufs_vendor_error=0;
 interrupts=1;irq_context=preempt=held=0;reads=fail_at=mmio_reads=0;
 capacity_blocks=31240192;capacity_size=4096;
 registers[REG_HCE/4]=1;registers[REG_HCS/4]=HCS_DP|HCS_UTRLRDY;
 registers[REG_UTRLBA/4]=(uint32_t)su.dma_pa;
 registers[REG_UTRLBAU/4]=(uint32_t)(su.dma_pa>>32);registers[REG_UTRLRSR/4]=1;
}
static void unchanged(void) {
 assert(su.first_block==3153920 && su.nblocks==28082168 && su.lun==2 && su.writable);
 assert(held==0);
}
static void output_unchanged(const uint8_t *b,unsigned len) {
 for(unsigned i=0;i<len;i++) assert(b[i]==0xa5);
}
int main(int argc,char **argv) {
 assert(argc==3); fixtures(argv[1],argv[2]);
 uint8_t out[65538];
 reset();memset(out,0xa5,sizeof(out));
 assert(sweet_ufs_read_vendor(123,out+1,65536)==0 && reads==10);
 for(unsigned i=0;i<65536;i++) assert(out[i+1]==(uint8_t)(7708082176ull+123+i));
 assert(out[0]==0xa5 && out[65537]==0xa5);unchanged();
 // Cached validation still serializes ownership, then issues exactly one read.
 unsigned before=reads;assert(sweet_ufs_read_vendor(544354304ull-65536,out,65536)==0);
 assert(reads==before+1);unchanged();
 // Every validation step plus payload transfer can fail without modifying output.
 for(unsigned fail=1;fail<=10;fail++) {
  reset();fail_at=fail;memset(out,0xa5,sizeof(out));
  assert(sweet_ufs_read_vendor(0,out,4096)==EIO);output_unchanged(out,sizeof(out));unchanged();
  if(fail<10) { unsigned n=reads;assert(sweet_ufs_read_vendor(0,out,4096)==EIO && reads==n); }
 }
 // Each immutable copy is independently checked; changes must fail closed.
 uint8_t *copies[]={primary,backup,entries,geometry0,geometry1,metadata0,metadata1};
 unsigned sizes[]={92,92,16384,4096,4096,65536,65536};
 for(unsigned c=0;c<7;c++) {
  for(unsigned j=0;j<16;j++) {
   unsigned at=(j*4051u)%sizes[c];copies[c][at]^=0x80;
   reset();memset(out,0xa5,sizeof(out));
   assert(sweet_ufs_read_vendor(0,out,4096)!=0);output_unchanged(out,sizeof(out));unchanged();
   copies[c][at]^=0x80;
  }
 }
 // Correctly checksummed but incompatible header geometry is still refused.
 reset(); uint64_t changed=3;memcpy(backup+72,&changed,8);crc_header(backup);
 memset(out,0xa5,sizeof(out));assert(sweet_ufs_read_vendor(0,out,4096)!=0);
 output_unchanged(out,sizeof(out));fixtures(argv[1],argv[2]);
 for(unsigned cap=0;cap<2;cap++) {
  reset();if(cap)capacity_size=512;else capacity_blocks--;
  memset(out,0xa5,sizeof(out));assert(sweet_ufs_read_vendor(0,out,1)!=0);
  output_unchanged(out,sizeof(out));unchanged();
 }
 // Offset overflow, endpoint and zero/max length rules; no I/O on invalid calls.
 uint64_t offsets[]={UINT64_MAX,UINT64_MAX-4095,544354304,544354303};
 unsigned lens[]={4096,4096,1,2};
 for(unsigned i=0;i<4;i++) {
  reset();assert(sweet_ufs_read_vendor(offsets[i],out,lens[i])==EINVAL && !reads);unchanged();
 }
 reset();assert(sweet_ufs_read_vendor(0,out,0)==EINVAL && !reads);
 assert(sweet_ufs_read_vendor(0,out,65537)==EINVAL && !reads);
 assert(sweet_ufs_read_vendor(0,0,1)==EINVAL && !reads);
 assert(sweet_ufs_read_vendor(0,(void *)(UINTPTR_MAX-7),16)==EINVAL && !reads);
 assert(sweet_ufs_read_vendor(0,dma+DMA_BOUNCE,4)==EINVAL && !reads);
 // Each runtime ownership/context gate refuses without issuing SCSI.
 for(unsigned gate=0;gate<17;gate++) {
  reset();
  switch(gate) {
   case 0:nord2_ufs_vendor_enabled=false;break;case 1:su.ready=false;break;
   case 2:nord2_root_owned=false;break;case 3:su.lun=0;break;
   case 4:su.probe_only=true;break;case 5:interrupts=0;break;
   case 6:irq_context=1;break;case 7:preempt=1;break;
   case 8:nord2_ufs_uic_pending=true;break;case 9:registers[REG_UTRLDBR/4]=2;break;
   case 10:registers[0x78/4]=1;break;case 11:registers[REG_HCE/4]=0;break;
   case 12:registers[REG_HCS/4]=0;break;case 13:registers[REG_UTRLBA/4]^=4096;break;
   case 14:registers[REG_UTRLBAU/4]^=1;break;case 15:registers[REG_UTRLRSR/4]=0;break;
   case 16:su.dma=0;break;
  }
  memset(out,0xa5,sizeof(out));assert(sweet_ufs_read_vendor(0,out,1)!=0 && !reads);
  output_unchanged(out,sizeof(out));assert(!held);
 }
 // Unaligned one-byte tail is valid; its device block stays inside vendor/super.
 reset();assert(sweet_ufs_read_vendor(544354303,out,1)==0);unchanged();
 // Seeded range fuzz with validation already established, exercising block rounding.
 reset();assert(sweet_ufs_read_vendor(0,out,1)==0);
 uint64_t rng=0x2de310ce;
 for(unsigned i=0;i<10000;i++) {
  rng=rng*6364136223846793005ull+1;
  uint64_t off=rng%544354304ull;unsigned len=(unsigned)((rng>>32)%65536)+1;
  int e=sweet_ufs_read_vendor(off,out,len);
  assert(e==(len<=544354304ull-off?0:EINVAL));unchanged();
 }
}
'''
with tempfile.TemporaryDirectory(prefix='nord2-ufs-vendor-') as tmp:
    c=Path(tmp)/'test.c';exe=Path(tmp)/'test';c.write_text(code)
    subprocess.run(['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror',
                    '-Wno-deprecated-declarations','-fsanitize=address,undefined',
                    str(c),'-lcrypto','-o',str(exe)],check=True)
    subprocess.run([str(exe),str(root/'research/nord2/wifi/vendor-read'),
                    str(root/'backup/nord2/ufs-baseline')],check=True)
print('PASS: exact vendor reader + real SHA256/private fixtures; GPT/LP copies, corruption, faults, bounds/overflow/fuzz, ownership/context, root-state preservation and all-or-nothing output')
