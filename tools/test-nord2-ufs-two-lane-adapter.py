#!/usr/bin/env python3
"""Canonical adapter + active-source integration guards; no device I/O."""
from pathlib import Path
import subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
P=ROOT/'shim/nord2-xnu'
source=(ROOT/'research/ci-39158169212/buildtree/xnu/bsd/dev/sweet_ufs.c').read_text()
adapter=(P/'ufs_two_lane_adapter.inc').read_text()
ACTIVE=ROOT/'research/ci-39158169212/buildtree/xnu/bsd/dev'
for name in ('ufs_two_lane.c','ufs_two_lane.h','ufs_two_lane_adapter.inc'):
 expected=(P/name).read_text()
 if name=='ufs_two_lane.c':
  expected='#include <arm64/sweet_early.h>\n#if defined(NORD2)\n'+expected+'\n#endif /* NORD2 */\n'
 assert (ACTIVE/name).read_text()==expected, 'active helper differs: '+name
assert '#include \"ufs_two_lane.c\"' in source
assert '#include \"ufs_two_lane_adapter.inc\"' in source
assert 'if (nord2_x2_stop || nord2_x2_uncertain) { return false; }' in source
assert 'if (nord2_x2_stop || nord2_x2_uncertain) { return -1; }' in source
assert source.index('nord2_x2_stop = true;',source.index('sweet_ufs_init(void)'))<source.index('SecureDTLookupEntry',source.index('sweet_ufs_init(void)'))
assert source.index('error = nord2_x2_run(')<source.index('su.bmajor = bdevsw_add')
assert 'lck_mtx_unlock(&sweet_ufs_lock);\n\t\tnord2_x2_halt' in source
assert 'status != 1u' in source and 'return ETIMEDOUT; /* preserve outstanding DMA; never abort */' in source
# Production timeout refusal precedes existing request-list clear.
assert source.index('return ETIMEDOUT; /* preserve outstanding DMA; never abort */')<source.index('ufs_wr(REG_UTRLCLR, ~1u);')
with tempfile.TemporaryDirectory(prefix='nord2-x2-adapter-') as td:
 d=Path(td);c=d/'test.c';exe=d/'test'
 c.write_text(r'''
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <setjmp.h>
static jmp_buf stop_jump;static int testing_halt;
#include <openssl/sha.h>
#include "ufs_two_lane.h"
typedef uintptr_t vm_offset_t;
#define REG_IS 0x20u
#define REG_UTRLBA 0x50u
#define REG_UTRLBAU 0x54u
#define REG_UTRLRSR 0x60u
#define N2_UIC_CMD 0x90u
#define N2_UIC_ARG1 0x94u
#define N2_UIC_ARG2 0x98u
#define N2_UIC_ARG3 0x9cu
#define N2_UIC_COMPLETE 0x400u
#define DMA_UTRL 0u
#define DMA_BOUNCE 0x4000u
#define UFS_BLOCK 4096u
#define LCK_MTX_ASSERT_OWNED 1
static struct { bool ready,writable,probe_only;unsigned lun,bdev,bmajor,cmajor;
 uintptr_t regs,dma;uint64_t dma_pa,first_block,nblocks; } su;
static int sweet_ufs_lock,held=1,interrupts=1,irq,preempt,read_error,reads;
static bool nord2_x2_in_progress,nord2_x2_uncertain,nord2_x2_stop,nord2_root_owned,nord2_ufs_uic_pending;
static uint32_t regs[64],pa[65536];static unsigned writes,logs,auto_uic;
static uint64_t counter;
static unsigned char dma[0x9000];
static void lck_mtx_assert(int *p,unsigned flag){assert(p==&sweet_ufs_lock&&flag==1&&held);}
static int ml_get_interrupts_enabled(void){return interrupts;}
static int ml_at_interrupt_context(void){return irq;}
static int get_preemption_level(void){return preempt;}
static uint32_t ufs_rd(uint32_t a){assert(a/4<64);return regs[a/4];}
static void ufs_wr(uint32_t a,uint32_t v){
 assert(a/4<64);writes++;
 if(auto_uic&&a==REG_IS){regs[a/4]&=~v;return;}
 regs[a/4]=v;
 if(auto_uic&&a==N2_UIC_CMD){
  unsigned attr=regs[N2_UIC_ARG1/4]>>16;
  if(v==1)regs[N2_UIC_ARG3/4]=pa[attr];else pa[attr]=regs[N2_UIC_ARG3/4];
  regs[N2_UIC_ARG2/4]=0;regs[0x30/4]=0x10f;
  regs[REG_IS/4]=v==2&&attr==0x1571?0x410:0x400;
 }
}
static uint64_t fake_rsr(const char *name){assert(!strcmp(name,"CNTVCT_EL0")||!strcmp(name,"CNTFRQ_EL0"));if(!strcmp(name,"CNTVCT_EL0"))return counter++;return 13000000;}
#define __builtin_arm_rsr64 fake_rsr
#define __builtin_arm_dsb(x) do{assert(x==15);}while(0)
static void delay(unsigned n){assert(n==1||n==1000000);if(testing_halt)longjmp(stop_jump,1);}
static int ufs_read_blocks(uint64_t lba,uint32_t blocks){assert(lba==1&&blocks==5);reads++;return read_error;}
static int safe_printf(const char *f,...){char text[1024];va_list a;va_start(a,f);int n=vsnprintf(text,sizeof(text),f,a);va_end(a);assert(n>=0&&n<255);logs++;return n;}
#define printf safe_printf
'''+adapter+r'''
static void reset(void){
 memset(&su,0,sizeof(su));memset(regs,0,sizeof(regs));
 su.regs=1;su.dma=(uintptr_t)dma;su.dma_pa=0x145000000ull;su.first_block=3153920;su.nblocks=28082168;su.lun=2;
 nord2_x2_in_progress=nord2_x2_stop=nord2_root_owned=true;
 nord2_x2_uncertain=nord2_ufs_uic_pending=false;interrupts=1;irq=preempt=read_error=reads=writes=0;
 regs[REG_UTRLBA/4]=(uint32_t)su.dma_pa;regs[REG_UTRLBAU/4]=(uint32_t)(su.dma_pa>>32);regs[REG_UTRLRSR/4]=1;
}
int main(int argc,char **argv){
 assert(argc==3);
 reset();struct nord2_x2_context c={su.first_block,su.nblocks,su.dma_pa,su.dma};
 assert(nord2_x2_owned(&c));
 for(unsigned gate=0;gate<20;gate++){
  reset();switch(gate){case 0:su.ready=true;break;case 1:su.writable=true;break;case 2:su.probe_only=true;break;
   case 3:su.lun=0;break;case 4:su.bdev=1;break;case 5:su.bmajor=1;break;case 6:su.cmajor=1;break;
   case 7:su.dma++;break;case 8:su.dma_pa++;break;case 9:su.first_block++;break;case 10:su.nblocks--;break;
   case 11:nord2_ufs_uic_pending=true;break;case 12:interrupts=0;break;case 13:irq=1;break;case 14:preempt=1;break;
   case 15:regs[REG_UTRLBA/4]^=4096;break;case 16:regs[REG_UTRLBAU/4]^=1;break;case 17:regs[REG_UTRLRSR/4]=0;break;
   case 18:nord2_x2_uncertain=true;break;case 19:nord2_root_owned=false;break;}
  assert(!nord2_x2_owned(&c));
 }
 reset();nord2_x2_wr(&c,N2_UIC_CMD,1);assert(nord2_ufs_uic_pending&&writes==1);
 nord2_x2_wr(&c,REG_IS,0x400);assert(!nord2_ufs_uic_pending&&writes==2);
 reset();nord2_x2_wr(&c,0x18,0);assert(nord2_x2_uncertain&&!writes);
 reset();nord2_x2_wr(&c,REG_IS,0xffffffff);assert(nord2_x2_uncertain&&!writes);
 reset();nord2_x2_wr(&c,N2_UIC_CMD,3);assert(nord2_x2_uncertain&&!writes);
 unsigned char hash[32];reset();assert(!nord2_x2_digest(&c,1,5,hash)&&reads==1);
 reset();assert(nord2_x2_digest(&c,2,5,hash)==N2L_READ_UNCERTAIN&&!reads);
 reset();read_error=ETIMEDOUT;assert(nord2_x2_digest(&c,1,5,hash)==N2L_READ_UNCERTAIN);
 reset();read_error=EIO;assert(nord2_x2_digest(&c,1,5,hash)==N2L_READ_COMPLETED_ERROR);
 reset();nord2_x2_uncertain=true;assert(nord2_x2_digest(&c,1,5,hash)==N2L_READ_UNCERTAIN&&!reads);
 assert(nord2_x2_ticks(0)==0);nord2_x2_delay(0,1);nord2_x2_barrier(0);
 /* Keep the compiled run/stop entry points referenced but never execute halt. */

 /* Run exact engine through exact adapter, with real saved GPT bytes/SHA and
  * synchronous model completions. Global UIC ownership must balance. */
 reset();counter=0;auto_uic=1;regs[0x34/4]=1;regs[0x30/4]=0x10f;
 pa[0x1583]=pa[0x1568]=4;pa[0x1580]=pa[0x1560]=1;
 pa[0x1584]=pa[0x1569]=1;pa[0x156a]=2;pa[0x1571]=0x11;pa[0x15d4]=1;
 pa[0x1540]=pa[0x1520]=pa[0x1581]=pa[0x1561]=2;pa[0x15a9]=5;pa[0x1587]=4;
 unsigned t[9]={8191,65535,32767,8191,65535,32767,8191,65535,32767};
 for(unsigned i=0;i<6;i++)pa[0x15b0+i]=t[i];for(unsigned i=0;i<3;i++)pa[0xd041+i]=t[6+i];
 FILE *fixture=fopen(argv[1],"rb");assert(fixture);
 assert(fread(dma+DMA_BOUNCE,1,4096,fixture)==4096);fclose(fixture);
 fixture=fopen(argv[2],"rb");assert(fixture);
 assert(fread(dma+DMA_BOUNCE+4096,1,16384,fixture)==16384);fclose(fixture);
 struct n2l_result integrated;assert(!nord2_x2_run(&integrated));
 assert(integrated.original_verified&&integrated.root_allowed&&reads==3);
 assert(!nord2_ufs_uic_pending&&!nord2_x2_in_progress&&nord2_x2_stop);
 assert(pa[0x1580]==1&&pa[0x1560]==1&&pa[0x15d4]==1);
 (void)&nord2_x2_run;struct n2l_result report;memset(&report,0xff,sizeof(report));
 testing_halt=1;if(!setjmp(stop_jump))nord2_x2_halt(-1,&report);
 assert(logs==7);return 0;
}
''')
 subprocess.run(['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-deprecated-declarations','-fsanitize=address,undefined','-I'+str(P),str(c),str(P/'ufs_two_lane.c'),'-lcrypto','-o',str(exe)],check=True)
 subprocess.run([str(exe),str(ROOT/'backup/nord2/ufs-baseline/header.bin'),str(ROOT/'backup/nord2/ufs-baseline/entries.bin')],check=True)
print('PASS: exact adapter ownership/descriptor/context/writeallowlist/read-error contracts + active hook/fallback/timeout ordering; ASan+UBSan; no hardware')
