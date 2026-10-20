#!/usr/bin/env python3
"""Exact ROM transaction with fake bounded EMI only; real authenticated sources."""
from pathlib import Path
import subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
def main():
 with tempfile.TemporaryDirectory() as directory:
  d=Path(directory)
  (d/'test.c').write_text(r'''
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wifi_rom_transaction.h"
static unsigned char memory[0x450000],original[0x450000],backup[NWRT_BACKUP_BYTES],scratch[4096];
static unsigned ops,fail_at,fail_at2,reads,writes,barriers,checks,persistent,corrupt_read,first_write_op,scratch_reads;
static int fail(void){ops++;return ops==fail_at||ops==fail_at2||(persistent && ops>=fail_at);}
static int check(void *ctx,unsigned flags){assert(!ctx && flags==NWRT_PREREQUISITES);checks++;return fail()?-1:0;}
static size_t index(uint64_t p,uint32_t n){assert(p>=NWF_ROM_EMI_BASE && n && n<=4096);size_t at=p-NWF_ROM_EMI_BASE;assert(at+n<=sizeof(memory));const size_t starts[4]={0x230000,0x27f0e0,0x2b0000,0x27f0f0};const size_t lengths[4]={49820,16,296868,16};int allowed=0;for(unsigned i=0;i<4;i++)if(at>=starts[i] && at+n<=starts[i]+lengths[i])allowed=1;assert(allowed);return at;}
static int read_io(void *ctx,uint64_t p,void *out,uint32_t n){assert(!ctx);reads++;size_t at=index(p,n);if(fail())return -1;memcpy(out,memory+at,n);if(out==scratch && ++scratch_reads==corrupt_read)((unsigned char *)out)[0]^=1;return 0;}
static int write_io(void *ctx,uint64_t p,const void *in,uint32_t n){assert(!ctx);if(!writes){assert(reads==88);first_write_op=ops+1;}writes++;size_t at=index(p,n);int error=fail();/* Failed callback can already have changed bytes. */memcpy(memory+at,in,error?n/2:n);return error?-1:0;}
static int barrier_io(void *ctx){assert(!ctx);barriers++;return fail()?-1:0;}
static struct nwrt_ops callbacks={check,read_io,write_io,barrier_io};
static unsigned char *load(const char *path,size_t n){FILE *f=fopen(path,"rb");assert(f);unsigned char *p=malloc(n);assert(p);assert(fread(p,1,n,f)==n&&fgetc(f)==EOF);fclose(f);return p;}
static void reset(void){memcpy(memory,original,sizeof(memory));memset(backup,0xa5,sizeof(backup));memset(scratch,0xa5,sizeof(scratch));ops=reads=writes=barriers=checks=fail_at=fail_at2=persistent=corrupt_read=scratch_reads=0;}
static void zeroed(const unsigned char *p,size_t n){for(size_t i=0;i<n;i++)assert(!p[i]);}
int main(int argc,char **argv){
 assert(argc==3);unsigned char *m=load(argv[1],49868),*w=load(argv[2],296916);
 for(size_t i=0;i<sizeof(original);i++)original[i]=(unsigned char)(i*17+31);
 struct nwrt_result result;
 #define RUN() nwrt_run(&callbacks,NULL,NWRT_PREREQUISITES,m,49868,w,296916,backup,sizeof(backup),scratch,sizeof(scratch),&result)
 reset();assert(!RUN());unsigned total_ops=ops;unsigned first_write=first_write_op;assert(result.write_attempted && result.restore_attempted && result.restore_verified && !result.dirty && !result.backup_retained);assert(!memcmp(memory,original,sizeof(memory)));zeroed(backup,sizeof(backup));zeroed(scratch,sizeof(scratch));
 /* Every one-shot check/read/write/barrier fault. Even partial writes roll back.
  * Faults during restoration must remain dirty even if later readback matches. */
 for(unsigned f=1;f<=total_ops;f++){
  reset();fail_at=f;assert(RUN());assert(result.error||result.restore_error);
  if(result.dirty){assert(result.restore_error && result.backup_retained && !result.restore_verified);}
  else{assert(!memcmp(memory,original,sizeof(memory)));zeroed(backup,sizeof(backup));}
  zeroed(scratch,sizeof(scratch));
 }
 /* Persistent loss of prerequisites after write: no unguarded recovery writes. */
 reset();/* Lose prerequisites immediately after first complete write. */
 fail_at=first_write+2;persistent=1;assert(RUN());assert(result.dirty && result.backup_retained && result.restore_error);zeroed(scratch,sizeof(scratch));
 // Readback mismatch restores originals; restoration mismatch retains backup.
 reset();corrupt_read=1;assert(RUN());assert(result.error==-7 && result.restore_verified && !result.dirty);assert(!memcmp(memory,original,sizeof(memory)));
 reset();corrupt_read=89;assert(RUN());assert(result.restore_error && result.dirty && result.backup_retained && !result.restore_verified);
 /* Verify source rejection has no IO, including modified undocumented CRC. */
 reset();m[32]^=1;assert(RUN());assert(!ops&&!writes);m[32]^=1;
 for(unsigned flags=0;flags<255;flags++){
  reset();assert(nwrt_run(&callbacks,NULL,flags,m,49868,w,296916,backup,sizeof(backup),scratch,sizeof(scratch),&result));assert(!ops);
 }
 reset();assert(nwrt_run(&callbacks,NULL,255,m,49868,w,296916,backup,1,scratch,sizeof(scratch),&result));assert(!ops);
 reset();assert(nwrt_run(&callbacks,NULL,255,m,49868,w,296916,backup,sizeof(backup),backup,4096,&result));assert(!ops);
 /* Allfirst4spans snapshotted before firstwrite asserted bycount at adapter. */
 free(m);free(w);return 0;
}
''')
  exe=d/'test'
  subprocess.run(['cc','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-deprecated-declarations','-fsanitize=address,undefined','-fno-omit-frame-pointer','-DNWRT_HOST_TEST','-DNWF_ROM_HOST_TEST','-I'+str(ROOT/'shim/nord2-xnu'),str(d/'test.c'),str(ROOT/'shim/nord2-xnu/wifi_rom_transaction.c'),str(ROOT/'shim/nord2-xnu/wifi_rom_plan.c'),'-lcrypto','-o',str(exe)],check=True)
  fw=ROOT/'research/nord2/wifi/firmware'
  subprocess.run([str(exe),str(fw/'soc3_0_ram_wmmcu_1a_1_hdr.bin'),str(fw/'soc3_0_ram_wifi_1a_1_hdr.bin')],check=True)
  print('PASS exact-C ASan/UBSan ROM transaction: real hashed sources; all IO/check/barrier one-shot faults; partial writes; persistent prerequisite loss; dirty backup retention; source/optin/buffer gates')
if __name__=='__main__':main()
