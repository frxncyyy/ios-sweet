#!/usr/bin/env python3
"""Pure exact-source ROM planning tests; private bytes stay local."""
from pathlib import Path
import subprocess
import tempfile
ROOT=Path(__file__).resolve().parents[1]
def main():
    with tempfile.TemporaryDirectory() as directory:
        d=Path(directory); exe=d/'test'; source=d/'test.c'
        source.write_text(r'''
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wifi_rom_plan.h"
static unsigned char *load(const char *path,size_t n) {
 FILE *f=fopen(path,"rb");assert(f);unsigned char *p=malloc(n);assert(p);
 assert(fread(p,1,n,f)==n && fgetc(f)==EOF);fclose(f);return p;
}
static void rejected(unsigned char *m,size_t mn,unsigned char *w,size_t wn) {
 struct nwf_rom_plan p;memset(&p,0xa5,sizeof(p));
 assert(nwf_rom_make_plan(m,mn,w,wn,&p));
 unsigned char *b=(unsigned char *)&p;for(size_t i=0;i<sizeof(p);i++)assert(!b[i]);
}
int main(int argc,char **argv) {
 assert(argc==3);unsigned char *m=load(argv[1],49868),*w=load(argv[2],296916);
 struct nwf_rom_plan p;assert(!nwf_rom_make_plan(m,49868,w,296916,&p));
 const uint64_t addresses[4]={0x8fd30000,0x8fd7f0e0,0x8fdb0000,0x8fd7f0f0};
 const uint32_t lengths[4]={49820,16,296868,16};
 for(unsigned i=0;i<4;i++) {
  struct nwf_rom_span *s=&p.spans[i];
  assert(s->blob==i/2 && s->bytes==lengths[i] && s->physical==addresses[i]);
  assert(s->source_offset==(i%2?0:48));
  assert(s->physical==NWF_ROM_EMI_BASE+s->emi_offset);
  assert(s->bytes<=NWF_ROM_EMI_BYTES-s->emi_offset);
  for(unsigned j=0;j<i;j++)assert(s->physical>=p.spans[j].physical+p.spans[j].bytes || p.spans[j].physical>=s->physical+s->bytes);
 }
 assert(nwf_rom_make_plan(m,49868,w,296916,NULL));
 rejected(NULL,49868,w,296916);rejected(m,49868,NULL,296916);
 rejected(w,296916,m,49868);
 size_t invalid[]={0,1,47,48,49867,49869,SIZE_MAX};
 for(unsigned i=0;i<sizeof(invalid)/sizeof(invalid[0]);i++)rejected(m,invalid[i],w,296916);
 rejected(m,49868,w,296915);rejected(m,49868,w,SIZE_MAX);
 /* Every byte of both48-byte headers, including version/date/CRC, is pinned.
  * A changed undocumented CRC is rejected by SHA; no CRC validity claimed. */
 for(unsigned i=0;i<48;i++){
  m[i]^=1;rejected(m,49868,w,296916);m[i]^=1;
  w[i]^=1;rejected(m,49868,w,296916);w[i]^=1;
 }
 m[48]^=1;rejected(m,49868,w,296916);m[48]^=1;
 w[296915]^=1;rejected(m,49868,w,296916);w[296915]^=1;
 /* Overflow/out-of-reservation/wrong-target attempts fail before publication. */
 unsigned char save[4];memcpy(save,m+24,4);
 const unsigned targets[]={0xffffffff,0xf044ffff,0xf0000000,0xf02b0000};
 for(unsigned i=0;i<4;i++){
  for(unsigned j=0;j<4;j++)m[24+j]=(unsigned char)(targets[i]>>(j*8));
  rejected(m,49868,w,296916);
 }
 memcpy(m+24,save,4);
 assert(!nwf_rom_make_plan(m,49868,w,296916,&p));
 free(m);free(w);return 0;
}
''')
        subprocess.run(['cc','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-Wno-deprecated-declarations','-fsanitize=address,undefined','-fno-omit-frame-pointer','-DNWF_ROM_HOST_TEST','-I'+str(ROOT/'shim/nord2-xnu'),str(source),str(ROOT/'shim/nord2-xnu/wifi_rom_plan.c'),'-lcrypto','-o',str(exe)],check=True)
        firmware=ROOT/'research/nord2/wifi/firmware'
        subprocess.run([str(exe),str(firmware/'soc3_0_ram_wmmcu_1a_1_hdr.bin'),str(firmware/'soc3_0_ram_wifi_1a_1_hdr.bin')],check=True)
        print('PASS: real ROM plans/hash/4bounded nonoverlapping spans;96 header mutations; payload corruption; null/size/target/overflow rejection; exact-C ASan/UBSan')
if __name__=='__main__':main()
