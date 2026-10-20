#!/usr/bin/env python3
"""Canonical preflight observations against frozen pre-observability predicate.
No active-source copying, device access or generated private test dependency.
"""
from pathlib import Path
import subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
HERE=ROOT/'shim/nord2-xnu'
# Differential first-gate test: compare EXACT old short-circuit access trace to
# new implementation, including HCS double read, every failure and final IS.
old='static int original(struct run *s)\n{\n if(!s->o->owned_preroot(s->c) || !(rd(s,0x34)&1) ||\n    (rd(s,HCS)&0xb)!=0xb || (rd(s,HCS)&0x30) || rd(s,0x58) || rd(s,0x78) || rd(s,0x24) || rd(s,0x18))return 0;\n for(uint32_t a=0x38;a<=0x48;a+=4)if(rd(s,a))return 0;\n return 1;\n}\n'
reference=old.replace('original(', 'reference(').replace('for(uint32_t a=0x38;a<=0x48;a+=4)if(rd(s,a))return 0;', 'for(uint32_t a=0x38;a<=0x48;a+=4){uint32_t v=rd(s,a);if(v!=0 && !(a==0x38 && v==0x80000010u))return 0;}')
new=(HERE/'ufs_two_lane.c').read_text()
def extract(s,a,b):return s[s.index(a):s.index(b,s.index(a))]
new=extract(new,'static int gate(','static int allowed(')
with tempfile.TemporaryDirectory() as td:
 p=Path(td); c=p/'d.c';exe=p/'d'
 c.write_text('''#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "ufs_two_lane.h"
#define HCS 0x30u
struct run{const struct n2l_ops *o;void*c;struct n2l_result*r;uint64_t start;int observing;};
static unsigned trace[32],count,fail;static uint32_t pa;
static uint32_t readcb(void*c,uint32_t a){(void)c;trace[count++]=a;
 unsigned i=count;uint32_t v=a==0x34?1:a==0x30?0x10f:0;
 if(a==0x38 && pa)v=pa;
 if(i==fail){if(a==0x34)v=0;else if(a==0x30)v=i==2?0:0x30;else v=1;}return v;}
static int owned(void*c){(void)c;trace[count++]=0xffff;return fail!=1;}
static uint32_t rd(struct run*s,uint32_t a){return s->o->read32(s->c,a);}
'''+old+reference+new+'''
int main(void){for(fail=0;fail<=14;fail++){
 struct n2l_ops o={.read32=readcb,.owned_preroot=owned};struct n2l_result r={0};struct run s={&o,0,&r,0,1};
 count=0;int before=original(&s);if(before)before=!rd(&s,0x20);unsigned n=count, saved[32];memcpy(saved,trace,sizeof(trace));
 count=0;int after=healthy(&s);if(after){uint32_t v=rd(&s,0x20);after=gate(&s,13,v,v==0);}
 assert(before==after && n==count && !memcmp(saved,trace,n*sizeof(unsigned)));
 assert(r.preflight_valid==((1u<<count)-1));assert(r.preflight_fail==fail);
 }
 fail=0;
 for(unsigned bit=0;bit<33;bit++){
 pa=bit==32?0x80000010u:0x80000010u|(1u<<bit);
 struct n2l_ops o={.read32=readcb,.owned_preroot=owned};struct n2l_result r={0};struct run s={&o,0,&r,0,1};
 count=0;int before=reference(&s);if(before)before=!rd(&s,0x20);unsigned n=count,saved[32];memcpy(saved,trace,sizeof(trace));
 count=0;int after=healthy(&s);if(after){uint32_t v=rd(&s,0x20);after=gate(&s,13,v,v==0);}
 assert(before==after && n==count && !memcmp(saved,trace,n*sizeof(unsigned)));
 assert(r.preflight[8]==pa&&r.preflight_valid==((1u<<count)-1));
 if(bit==32||bit==4||bit==31)assert(after&&count==14&&!r.preflight_fail);
 else assert(!after&&count==9&&r.preflight_fail==9);
 }
 return 0;}
''')
 subprocess.run(['clang','-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(HERE),str(c),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
print('PASS differential initial gate: unchanged short-circuit reads, first failure, explicit validity for all14 evaluations')
