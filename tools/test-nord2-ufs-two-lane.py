#!/usr/bin/env python3
"""Compile canonical two-lane diagnostic; injected MMIO only, no device operations."""
from pathlib import Path
import subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
HERE=ROOT/'shim/nord2-xnu'
with tempfile.TemporaryDirectory(prefix='nord2-x2-') as td:
 p=Path(td);c=p/'test.c';exe=p/'test'
 c.write_text(r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "ufs_two_lane.h"
static uint32_t regs[64],values[65536];
static uint64_t clockticks;
static unsigned issued,fail_at,timeout_at,wrong_status_at,extra_is_at,only_ucc_at,only_upm_at,stalled,reads,read_failure,read_corruption,polls,power;
static int ownership;
static unsigned mismatch_at,loss_at,error_at,read_uncertain,host_error_at,device_error_at;
static uint32_t rd(void *c,uint32_t a) {
 (void)c;assert(a/4<64);
 if(a==0x20 && issued && polls<3 && issued!=timeout_at) {
  polls++;
  if(polls==1 && issued!=only_upm_at)regs[0x20/4]|=0x400;
  if(polls==2 && power && issued!=only_ucc_at)regs[0x20/4]|=0x10;
  if(polls>=2 && issued!=only_upm_at && (!power||issued!=only_ucc_at)) {
   regs[0x30/4]|=8;
  }
  if(issued==extra_is_at)regs[0x20/4]|=2;
  if(issued==error_at)regs[0x3c/4]=1;
  if(issued==host_error_at)regs[0x30/4]|=0x10;
  if(issued==device_error_at)regs[0x30/4]|=0x20;
 }
 return regs[a/4];
}
static void wr(void *c,uint32_t a,uint32_t v) {
 (void)c;assert(a==0x20||a==0x90||a==0x94||a==0x98||a==0x9c);
 if(a==0x20){assert(v==0x400||v==0x410);regs[a/4]&=~v;polls=3;return;}
 regs[a/4]=v;if(a!=0x90)return;
 assert(v==1||v==2);assert(!(regs[0x94/4]&65535));
 uint32_t attr=regs[0x94/4]>>16;issued++;polls=0;power=v==2&&attr==0x1571;
 regs[0x30/4]&=~8u;
 regs[0x98/4]=issued==fail_at?9:0;
 if(v==1)regs[0x9c/4]=values[attr]^(issued==mismatch_at?1u:0u);
 else if(issued!=fail_at)values[attr]=regs[0x9c/4];
 if(power){regs[0x30/4]&=~0x700u;regs[0x30/4]|=(issued==wrong_status_at?0:1)<<8;}
}
static uint64_t ticks(void *c){(void)c;uint64_t old=clockticks;if(!stalled)clockticks+=1000;return old;}
static void delay(void *c,unsigned n){(void)c;assert(n==1);}
static void barrier(void *c){(void)c;}
static int owned(void *c){(void)c;return ownership && (!loss_at || issued<loss_at);}
static int hash(void *c,uint64_t lba,uint32_t n,unsigned char out[32]) {
 (void)c;assert(lba==1&&n==5&&ownership);assert(!regs[0x20/4]);
 assert(values[0x1580]==values[0x1560]);reads++;
 for(unsigned i=0;i<32;i++)out[i]=(unsigned char)i;
 if(reads==read_corruption)out[0]^=1;
 return reads==read_uncertain?N2L_READ_UNCERTAIN:reads==read_failure?N2L_READ_COMPLETED_ERROR:0;
}
static const unsigned char expected[32]={0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};
static struct n2l_ops ops={rd,wr,ticks,delay,barrier,owned,hash,expected,1000000};
static void reset(void) {
 memset(regs,0,sizeof(regs));memset(values,0,sizeof(values));
 regs[0x34/4]=1;regs[0x30/4]=0x10b;clockticks=0;ownership=1;
 mismatch_at=loss_at=error_at=read_uncertain=host_error_at=device_error_at=0;
 issued=fail_at=timeout_at=wrong_status_at=extra_is_at=only_ucc_at=only_upm_at=stalled=reads=read_failure=read_corruption=polls=power=0;
 values[0x1583]=values[0x1568]=4;values[0x1580]=values[0x1560]=1;
 values[0x1584]=values[0x1569]=1;values[0x156a]=2;values[0x1571]=0x11;
 values[0x15d4]=0;
 values[0x1540]=values[0x1520]=values[0x1581]=values[0x1561]=2;
 values[0x15a9]=5;values[0x1587]=4;
 uint32_t defaults[9]={8191,65535,32767,8191,65535,32767,8191,65535,32767};
 for(unsigned i=0;i<6;i++)values[0x15b0+i]=defaults[i];
 for(unsigned i=0;i<3;i++)values[0xd041+i]=defaults[6+i];
}
static void restored(struct n2l_result *r) {
 assert(r->original_verified&&r->handoff_allowed&&!r->uncertain);
 assert(values[0x1580]==1&&values[0x1560]==1&&values[0x15d4]==0);
}
int main(void) {
 struct n2l_result r;reset();assert(!n2l_diagnostic(&ops,0,1,&r));restored(&r);
 assert(r.root_allowed&&reads==3&&r.commands==115&&r.sets==37);
 // Explicit opt-in and every incoming gate must dispatch no SET on rejection.
 reset();assert(n2l_diagnostic(&ops,0,0,&r)&&!issued&&!r.root_allowed);
 for(unsigned gate=0;gate<12;gate++) {
  reset();switch(gate){case 0:ownership=0;break;case 1:regs[0x34/4]=0;break;
   case 2:regs[0x30/4]&=~8u;break;case 3:regs[0x58/4]=1;break;
   case 4:regs[0x78/4]=1;break;case 5:regs[0x24/4]=0x400;break;
   case 6:regs[0x18/4]=1;break;case 7:regs[0x38/4]=1;break;
   case 8:regs[0x20/4]=0x410;break;case 9:regs[0x48/4]=1;break;
   case 10:regs[0x30/4]|=0x10;break;case 11:regs[0x30/4]|=0x20;break;}
  assert(n2l_diagnostic(&ops,0,1,&r)&&!r.sets&&!r.root_allowed);
 }
 uint16_t invalid[]={0x1540,0x1520,0x1581,0x1561,0x15a9,0x1587,0x1583,0x1568,0x1580,0x1560,0x1584,0x1569,0x156a,0x1571,0x15d4,0x15b0,0xd043};
 for(unsigned i=0;i<sizeof(invalid)/sizeof(invalid[0]);i++){
  reset();values[invalid[i]]=99;assert(n2l_diagnostic(&ops,0,1,&r)&&!r.sets&&!r.root_allowed);
 }
 // Each baseline/forward/rollback command faulted independently.
 for(unsigned i=1;i<=115;i++) {
  reset();fail_at=i;assert(n2l_diagnostic(&ops,0,1,&r)&&!r.root_allowed);
  if(i<=24)assert(!r.sets&&!r.rollback_attempted);
  else if(i<=60){restored(&r);assert(r.rollback_attempted);}
  else assert(r.uncertain&&!r.handoff_allowed);
 }
 // Completed GET mismatch never masquerades as successful lane verification.
 for(unsigned i=43;i<=60;i++) {
  reset();mismatch_at=i;assert(n2l_diagnostic(&ops,0,1,&r)&&!r.root_allowed);restored(&r);
 }
 for(unsigned i=79;i<=96;i++) {
  reset();mismatch_at=i;assert(n2l_diagnostic(&ops,0,1,&r)&&r.uncertain&&!r.handoff_allowed);
 }
 for(unsigned i=1;i<=115;i++)for(unsigned fault=0;fault<4;fault++) {
  reset();if(fault==1)loss_at=i;else if(fault==2)host_error_at=i;else if(fault==3)device_error_at=i;else error_at=i;
  assert(n2l_diagnostic(&ops,0,1,&r)&&r.uncertain&&!r.root_allowed&&!r.handoff_allowed);
  assert(issued==i);
 }
 // Every UIC timeout is uncertain: no rollback after ambiguous execution.
 for(unsigned i=1;i<=115;i++) {
  reset();timeout_at=i;assert(n2l_diagnostic(&ops,0,1,&r));
  assert(r.uncertain&&!r.root_allowed&&!r.handoff_allowed&&issued==i);
 }
 // Both ordinary UCC and asynchronous power completion are mandatory.
 unsigned powers[]={42,78};
 for(unsigned i=0;i<2;i++)for(unsigned fault=0;fault<4;fault++) {
  reset();if(fault==0)only_ucc_at=powers[i];if(fault==1)only_upm_at=powers[i];
  if(fault==2)wrong_status_at=powers[i];if(fault==3)extra_is_at=powers[i];
  assert(n2l_diagnostic(&ops,0,1,&r)&&r.uncertain&&!r.root_allowed&&!r.handoff_allowed);
 }
 for(unsigned i=1;i<=3;i++)for(unsigned corruption=0;corruption<2;corruption++) {
  reset();if(corruption)read_corruption=i;else read_failure=i;
  int rc=n2l_diagnostic(&ops,0,1,&r);
  if(corruption&&i==1){assert(rc&&!r.root_allowed);continue;}
  assert(rc&&!r.root_allowed);if(i==2)restored(&r);if(i==3)assert(r.uncertain&&!r.handoff_allowed);
 }
 for(unsigned i=1;i<=3;i++){reset();read_uncertain=i;assert(n2l_diagnostic(&ops,0,1,&r)&&r.uncertain&&!r.handoff_allowed&&!r.root_allowed);}
 reset();timeout_at=25;stalled=1;assert(n2l_diagnostic(&ops,0,1,&r)&&r.uncertain&&issued==25);
 reset();clockticks=UINT64_MAX-5000;assert(!n2l_diagnostic(&ops,0,1,&r));restored(&r);
 return 0;
}
''')
 subprocess.run(['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(HERE),str(c),str(HERE/'ufs_two_lane.c'),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
 subprocess.run(['clang','--target=aarch64-none-elf','-std=c11','-O2','-Wall','-Wextra','-Werror','-ffreestanding','-c',str(HERE/'ufs_two_lane.c'),'-o',str(p/'diagnostic.o')],check=True)
print('PASS: exact canonical two-lane engine; injected gates/limits/each command fault+timeout/async completion/rollback/digests; ASan+UBSan and AArch64 compile; no hardware')
