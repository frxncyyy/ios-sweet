#!/usr/bin/env python3
"""Exact source injected-SMC tests; never executes a native SMC or device I/O."""
from pathlib import Path
import subprocess
import tempfile
ROOT=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='nord2-emi-snapshot-') as td:
    d=Path(td); c=d/'test.c'; exe=d/'test'
    c.write_text(r'''
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include "wifi_emi_snapshot.h"
static unsigned calls,mode,at;
static int fake(void *ctx,const uint64_t a[8],uint64_t r[4]) {
 assert(ctx==&mode); assert(a[0]==0xc200050b && a[1]==2 && a[2]<=2 && a[3]<32);
 assert(a[4]<(a[2]==2?2u:1u) && !a[5] && !a[6] && !a[7]);
 unsigned index=calls++; if(mode==1 && index==at)return -1;
 r[0]=a[2]==0?0x4fb0:a[2]==1?0x4ff4:a[4]?0x654321:0x80000600;
 r[1]=0x1111111122222222ull;r[2]=0x3333333344444444ull;r[3]=0x5555555566666666ull;
 if(mode==2 && index==at)r[0]=UINT64_MAX;
 if(mode==3 && index==at)r[0]=0xfffffffeu;
 if(mode==4 && index==at)r[0]=0x100000000ull;
 if(mode==5 && index==at)r[0]=UINT64_MAX-3;
 if(mode==6 && index==at)r[0]=0x40000000;
 if(mode==7 && index==at)r[0]=UINT64_MAX-2;
 return 0;
}
static int collect(const uint32_t *list,size_t n,struct nwes_region *out) {
 calls=0;return nwes_collect(fake,&mode,list,n,out,n);
}
int main(void) {
 uint32_t list[32]={27,31};struct nwes_region out[32];uint64_t start=9,end=9;
 assert(!collect(list,2,out) && calls==8 && out[0].region==27 && out[1].region==31);
 for(unsigned i=0;i<2;i++)for(unsigned f=0;f<4;f++) {
  assert(out[i].field[f].status==NWES_VALUE);
  assert(out[i].field[f].raw[1]==0x1111111122222222ull);
  assert(out[i].field[f].raw[2]==0x3333333344444444ull);
  assert(out[i].field[f].raw[3]==0x5555555566666666ull);
 }
 assert(!nwes_decode_range(out,0x40000000,0x23fffffff,16,&start,&end));
 assert(start==0x8fb00000 && end==0x8ff4ffff);
 for(unsigned dom=0;dom<16;dom++) {
  uint32_t v=99;assert(!nwes_domain(out,dom,&v));
  assert(v==((out[0].field[2+dom/8].value>>(3*(dom%8)))&7));
 }
 for(mode=1;mode<=7;mode++)for(at=0;at<8;at++) {
  int rc=collect(list,2,out);struct nwes_field *f=&out[at/4].field[at%4];
  if(mode<=2) {
   assert(rc==1 && calls==at+1);
   assert(f->status==(mode==1?NWES_TRANSPORT_ERROR:NWES_KNOWN_ERROR));
   for(unsigned j=at+1;j<8;j++)assert(out[j/4].field[j%4].status==NWES_NOT_CALLED);
  }else {
   assert(!rc && calls==8);
   assert(f->status==((mode==3||mode==5||mode==7)?NWES_KNOWN_ERROR:NWES_INVALID_VALUE));
   if(mode==3)assert(f->known_error==-2);if(mode==5)assert(f->known_error==-4);if(mode==7)assert(f->known_error==-3);
  }
 }
 mode=0;for(unsigned i=0;i<32;i++)list[i]=i;
 assert(!collect(list,32,out) && calls==128);
 memset(out,0xa5,sizeof(out));struct nwes_region before[32];memcpy(before,out,sizeof(out));
 list[0]=32;calls=0;assert(nwes_collect(fake,&mode,list,32,out,32)==-1 && !calls);
 assert(!memcmp(before,out,sizeof(out)));list[0]=1;
 assert(nwes_collect(fake,&mode,list,32,out,32)==-1 && !calls);list[0]=0;
 assert(nwes_collect(fake,&mode,list,33,out,32)==-1);
 assert(nwes_collect(fake,&mode,list,32,out,31)==-1);
 assert(nwes_collect(fake,&mode,list,0,out,32)==-1);
 assert(nwes_collect(0,&mode,list,1,out,32)==-1);
 assert(nwes_collect(fake,&mode,0,1,out,32)==-1);
 assert(nwes_collect(fake,&mode,list,1,0,32)==-1);
 assert(!collect(list,1,out));
 start=end=9;assert(nwes_decode_range(out,UINT64_MAX-1,UINT64_MAX,16,&start,&end)==-1);
 assert(start==9 && end==9);
 assert(nwes_decode_range(out,0,UINT64_MAX,64,&start,&end)==-1);
 assert(nwes_decode_range(out,0,UINT64_MAX,63,&start,&end)==-1);
 assert(nwes_decode_range(out,0x40000000,0x80000000,16,&start,&end)==-1);
 out[0].field[0].value=0x5000;
 assert(nwes_decode_range(out,0,UINT64_MAX,16,&start,&end)==-1);
 uint32_t p=99;assert(nwes_domain(out,16,&p)==-1 && p==99);
 out[0].field[2].status=NWES_INVALID_VALUE;
 assert(nwes_domain(out,0,&p)==-1 && p==99);
 /* Native wrapper compiled but deliberately never called by this test. */
 return 0;
}
''')
    source=ROOT/'shim/nord2-xnu/wifi_emi_snapshot.c'
    subprocess.run(['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(source.parent),str(c),str(source),'-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
    subprocess.run(['clang','--target=aarch64-none-elf','-std=c11','-O2','-Wall','-Wextra','-Werror','-ffreestanding','-c',str(source),'-o',str(d/'snapshot.o')],check=True)
print('PASS: exact injected SMC ABI/128 bounded queries/raw x0..3/errors/no retry/decoding; ASan+UBSan; AArch64 native wrapper compile (not executed)')
