#include "wifi_main_fw_place.h"
#include "wifi_main_phy_force.h"
#include "wifi_main_phy_backup.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

static unsigned char *file(const char *path,size_t *n){
 FILE *f=fopen(path,"rb");long z;unsigned char *p;
 assert(f&&fseek(f,0,SEEK_END)==0&&(z=ftell(f))>=0&&fseek(f,0,SEEK_SET)==0);
 p=malloc((size_t)z);assert(p&&fread(p,1,(size_t)z,f)==(size_t)z&&fclose(f)==0);
 *n=(size_t)z;return p;
}
static void be32(unsigned char *p,unsigned x){p[0]=x>>24;p[1]=x>>16;p[2]=x>>8;p[3]=x;}
static void put32(unsigned char *p,unsigned x){p[0]=x;p[1]=x>>8;p[2]=x>>16;p[3]=x>>24;}
static unsigned char fdt[256];
static size_t build_fdt(int fem){
 const char root[]="mediatek,MT6893";
 const char other[]="mediatek,connfem";
 size_t off=56;
 memset(fdt,0,sizeof(fdt));be32(fdt,0xd00dfeed);be32(fdt+8,56);
 be32(fdt+12,176);be32(fdt+20,17);be32(fdt+24,16);
 be32(fdt+off,1);off+=8;/* root begin + aligned empty name */
 be32(fdt+off,3);be32(fdt+off+4,(unsigned)sizeof(root));be32(fdt+off+8,0);off+=12;
 memcpy(fdt+off,root,sizeof(root));off=(off+sizeof(root)+3)&~(size_t)3;
 if(fem){be32(fdt+off,3);be32(fdt+off+4,(unsigned)sizeof(other));be32(fdt+off+8,0);off+=12;
  memcpy(fdt+off,other,sizeof(other));off=(off+sizeof(other)+3)&~(size_t)3;}
 be32(fdt+off,2);off+=4;be32(fdt+off,9);off+=4;
 assert(off<=176);memcpy(fdt+176,"compatible",11);be32(fdt+4,187);
 be32(fdt+32,11);be32(fdt+36,(unsigned)(off-56));return 187;
}
static void phy_test(const char *wifi_path){
 size_t n;unsigned char *wifi=file(wifi_path,&n),frame[NFA_FRAME_BYTES],rx[NFA_RX_BYTES]={0};
 unsigned char digest[32];struct nfa_board b={0};struct nfa_result r={0};
 static const unsigned char expected[32]={0x2a,0xc3,0x2f,0x86,0x72,0xac,0x11,0xa7,
  0x1c,0x09,0xb8,0x8d,0xf3,0x89,0x09,0x3d,0xbc,0xe3,0xc2,0x34,
  0xae,0x23,0xa8,0x61,0xbd,0x5e,0xd1,0xf7,0x23,0x30,0xb8,0x8c};
 assert(nfa_prepare(wifi,n,fdt,build_fdt(0),&b)==0);
 assert(b.verified&&b.connfem_absent&&b.coant==0);
 assert(!memcmp(b.nvram,wifi+3972,225)&&b.nvram[225]==0&&b.nvram[227]==0);
 assert(nfa_force_frame(&b,7,frame)==0);
 assert(frame[36]==8&&frame[37]==0xa0&&frame[39]==7);
 SHA256(frame+64,NFA_TLV_BYTES,digest);assert(!memcmp(digest,expected,32));
 assert(nfa_force_frame(&b,0,frame)!=0);
 wifi[0]^=1;assert(nfa_prepare(wifi,n,fdt,187,&b)!=0);wifi[0]^=1;
 assert(nfa_prepare(wifi,n,fdt,build_fdt(1),&b)!=0);
 build_fdt(0);
 rx[26]=0;rx[27]=0xe0;rx[28]=5;rx[29]=7;
 put32(rx+32,0x556789aa);rx[36]=1;rx[37]=1;rx[38]=20;
 rx[40]=1;rx[42]=16;rx[44]=0x81;put32(rx+48,0x100);put32(rx+52,0x800);put32(rx+56,25);
 assert(nfa_force_response(rx,sizeof(rx),7,0x450000,&r)==0);
 assert(r.valid&&r.force_success&&r.backup_needed&&r.emi_offset==0x100&&r.emi_bytes==0x800);
 rx[29]=8;assert(nfa_force_response(rx,sizeof(rx),7,0x450000,&r)!=0);rx[29]=7;
 rx[45]=2;assert(nfa_force_response(rx,sizeof(rx),7,0x450000,&r)!=0);rx[45]=0;
 put32(rx+48,0x44ffff);put32(rx+52,2);assert(nfa_force_response(rx,sizeof(rx),7,0x450000,&r)!=0);
 assert(nfa_force_response(rx,sizeof(rx)-1,7,0x450000,&r)!=0);
 free(wifi);
}
struct fake {unsigned writes,reads,remaps,barriers,fail_at,owner_checks,fail_owner_at;
 unsigned target;unsigned char window[65536],*conn;};
static int owner(void *v){struct fake *f=v;f->owner_checks++;return f->owner_checks==f->fail_owner_at?-1:0;}
static int remap(void *v,uint32_t t){struct fake *f=v;f->remaps++;f->target=t;return f->fail_at==1?-1:0;}
static int wr(void *v,enum nmt_space s,uint32_t pa,uint32_t x){
 struct fake *f=v;unsigned char *p;
 if(s==NMT_AP2WF_WINDOW){assert(pa>=0x18500000&&pa<0x18510000);p=f->window+(pa-0x18500000);}
 else{assert(pa>=0x8fb00000&&pa<0x8ff50000);p=f->conn+(pa-0x8fb00000);}
 if(f->fail_at==f->writes+2)return -1;
 put32(p,x);f->writes++;return 0;
}
static int rd(void *v,enum nmt_space s,uint32_t pa,uint32_t *x){
 struct fake *f=v;unsigned char *p=s==NMT_AP2WF_WINDOW?
  f->window+(pa-0x18500000):f->conn+(pa-0x8fb00000);
 *x=(unsigned)p[0]|(unsigned)p[1]<<8|(unsigned)p[2]<<16|(unsigned)p[3]<<24;
 f->reads++;return 0;
}
static int barrier(void *v){((struct fake *)v)->barriers++;return 0;}
static void place_test(const char *main_path,const char *patch_path){
 size_t mn,pn;unsigned char *m=file(main_path,&mn),*p=file(patch_path,&pn);
 struct nmt_layout layout;struct nfp_state state={0};
 struct nfp_io io={owner,remap,wr,rd,barrier};
 struct fake *f=calloc(1,sizeof(*f));assert(f);f->conn=calloc(1,0x450000);assert(f->conn);
 assert(nmt_parse_fixed_layout(m,mn,p,pn,&layout)==0);
 assert(nfp_span(&io,f,&layout,&state,1)!=0);
 for(unsigned i=0;i<7;i++)assert(nfp_span(&io,f,&layout,&state,i)==0);
 assert(state.stage==NFP_COPIED&&state.completed==0x7f&&state.remaps==4&&
        state.words_written==state.words_read&&state.words_written>200000&&
        state.barriers==18&&!state.halt);
 assert(nfp_span(&io,f,&layout,&state,0)!=0);
 memset(&state,0,sizeof(state));f->writes=0;f->fail_at=2;
 assert(nfp_span(&io,f,&layout,&state,0)!=0&&state.halt&&state.dirty);
 assert(nfp_span(&io,f,&layout,&state,0)!=0);/* no replay */
 memset(&state,0,sizeof(state));f->writes=f->fail_at=f->owner_checks=0;f->fail_owner_at=4;
 assert(nfp_span(&io,f,&layout,&state,0)!=0&&state.halt&&state.dirty&&state.words_written==1024);
 assert(nfp_span(&io,f,&layout,&state,0)!=0&&f->owner_checks==4);
 free(f->conn);free(f);free(m);free(p);
}
static int backup_owner(void *c){(void)c;return 0;}
struct backup_ctx {unsigned count,fail;};
static int backup_read(void *c,uint32_t off,void *dst,uint32_t n){
 struct backup_ctx *x=c;uint8_t *p=dst;x->count++;
 if(x->count==x->fail)return -1;
 for(uint32_t i=0;i<n;i++)p[i]=(uint8_t)(off+i);
 return 0;
}
static int backup_barrier(void *c){(void)c;return 0;}
static void backup_test(void){
 struct nfa_backup_io io={backup_owner,backup_read,backup_barrier};
 struct nfa_result r={.valid=1,.force_success=1,.backup_needed=1,
                      .emi_offset=0x123,.emi_bytes=8193};
 struct nfa_backup_state s={0};struct backup_ctx c={0};
 uint8_t b[8193];
 assert(nfa_backup(&io,&c,&r,b,sizeof(b),&s)==0);
 assert(s.valid&&s.bytes_copied==8193&&s.reads==3&&c.count==3);
 for(unsigned i=0;i<sizeof(b);i++)assert(b[i]==(uint8_t)(r.emi_offset+i));
 memset(&s,0,sizeof(s));c=(struct backup_ctx){.fail=2};
 assert(nfa_backup(&io,&c,&r,b,sizeof(b),&s)!=0&&s.halt&&s.attempted&&s.bytes_copied==4096);
 assert(nfa_backup(&io,&c,&r,b,sizeof(b),&s)!=0&&c.count==2);
}
int main(int argc,char **argv){
 assert(argc==4||argc==5);phy_test(argv[1]);place_test(argv[2],argv[3]);backup_test();
 if(argc==5){size_t wn,dn;unsigned char *w=file(argv[1],&wn),*d=file(argv[4],&dn);
  struct nfa_board b={0};assert(nfa_prepare(w,wn,d,dn,&b)==0);
  free(w);free(d);}
 puts("PASS: board hash/FDT/TLV/response, seven ordered spans, sticky write fault");
}
