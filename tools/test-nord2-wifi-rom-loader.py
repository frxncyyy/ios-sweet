from pathlib import Path
import subprocess,tempfile,sys,importlib.util
root=Path(__file__).resolve().parents[1];P=root/"shim/nord2-xnu"
s=(P/'loader.c').read_text()
opt=s[s.index('static int nrc_loader_optin'):s.index('/* ---- ADT:',s.index('static int nrc_loader_optin'))]
patch=s[s.index('static uint32_t adt_le32'):s.index('typedef void (*xnu_entry_t)')]
head=r'''#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <string.h>
#include <setjmp.h>
#include <stdio.h>
#include "wifi_rom_contract.h"
static struct nrc_claim nrc_loader_owner;
static jmp_buf jb;
static void fatal(const char *s){(void)s;longjmp(jb,1);}
static int same(const char*a,const char*b){return !strcmp(a,b);}
'''
test=r'''
static void put32(unsigned char*p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(unsigned char)(v>>(i*8));}
static uint64_t get64(unsigned char*p){uint64_t v=0;for(unsigned i=0;i<8;i++)v|=(uint64_t)p[i]<<(i*8);return v;}
static unsigned add(unsigned char*b,unsigned off,const char*n,unsigned len){memcpy(b+off,n,strlen(n));put32(b+off+32,len);return off+36+len;}
static unsigned tree(unsigned char*b,int claim,int duplicate,unsigned claimlen){
 memset(b,0,512);put32(b+4,1);put32(b+8,3+claim+duplicate);unsigned n=16;
 n=add(b,n,"nord2-stock-entry",8);n=add(b,n,"nord2-stock-dtb",8);n=add(b,n,"nord2-xnu-region",16);
 if(claim)n=add(b,n,"nord2-conn-claim",claimlen);
 if(duplicate)n=add(b,n,"nord2-conn-claim",24);return n;
}
int main(void){
 assert(nrc_loader_optin("debug=1",sizeof("debug=1"))==0);
 assert(nrc_loader_optin("nord2wifirom=1",sizeof("nord2wifirom=1"))==1);
 assert(nrc_loader_optin(" debug=1 nord2wifirom=1 cpus=8 ",sizeof(" debug=1 nord2wifirom=1 cpus=8 "))==1);
 assert(nrc_loader_optin("nord2wifirom=0",sizeof("nord2wifirom=0"))==-1);
 assert(nrc_loader_optin("nord2wifirom=10",sizeof("nord2wifirom=10"))==-1);
 assert(nrc_loader_optin("nord2wifirom=1 nord2wifirom=1",sizeof("nord2wifirom=1 nord2wifirom=1"))==-1);
 assert(nrc_loader_optin("xnord2wifirom=1",sizeof("xnord2wifirom=1"))==0);
 assert(nrc_loader_optin("nord2wifirom=1",sizeof("nord2wifirom=1")-1)==-1);
 unsigned char b[512];unsigned len=tree(b,1,0,24);unsigned claimoff=16+44+44+52+36;
 nrc_loader_owner=(struct nrc_claim){NRC_CLAIM_IOS_FIXED,NRC_CONN_BASE,NRC_CONN_BYTES};
 assert(!setjmp(jb));patch_chosen(b,len,0x40280000,0x49000000,0x142000000,0x144000000);
 assert(get64(b+claimoff)==NRC_CONN_BASE&&get64(b+claimoff+8)==NRC_CONN_BYTES);
 assert(adt_le32(b+claimoff+16)==3&&adt_le32(b+claimoff+20)==1);
 assert(get64(b+52)==0x40280000&&get64(b+96)==0x49000000);
 for(int which=0;which<3;which++){
  len=tree(b,which!=0,which==1,which==2?20:24);
  if(!setjmp(jb)){patch_chosen(b,len,1,2,3,4);assert(0);}
 }
 nrc_loader_owner=(struct nrc_claim){0};len=tree(b,1,0,24);
 assert(!setjmp(jb));patch_chosen(b,len,1,2,3,4);
 assert(get64(b+claimoff)==0&&get64(b+claimoff+8)==0&&adt_le32(b+claimoff+16)==0&&adt_le32(b+claimoff+20)==0);
 puts("PASS actual loader ROM opt-in and current-boot ADT claim encoding; defaultoff/malformed/duplicate/missing contract");
}
'''
with tempfile.TemporaryDirectory() as d:
 source=Path(d)/'test.c';binary=Path(d)/'test';source.write_text(head+opt+patch+test)
 subprocess.run(['cc','-std=c11','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-fsanitize=address,undefined','-I'+str(P),str(source),'-o',str(binary)],check=True)
 subprocess.run([str(binary)],check=True)
sys.path.insert(0,str(root/'tools'))
spec=importlib.util.spec_from_file_location('candidate_adt',root/'tools/mkadt-nord2.py');m=importlib.util.module_from_spec(spec);spec.loader.exec_module(m)
chosen=m.build().children[0];values=[v for k,_,v in chosen.props if k=='nord2-conn-claim'];assert values==[bytes(24)]
print('PASS candidate ADT first-child chosen carries unique zero 24-byte CONN claim')
