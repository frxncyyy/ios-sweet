/* Derivative of MediaTek mt6893_pos.c GPL-2.0, selected04d00c25.
 * Vendor order/values preserved; stronger fail-stop checked transport.
 * Porting scope and source references: WIFI-PERSISTENT.md. */
#include "wifi_persistent.h"
#include "wifi_common.h"
struct core {const struct npc_ops *o;void *c;struct npc_result *r;};
static int ck(struct core *s){return s->o->check(s->c,NPC_REQUIRED);}
static int rd(struct core *s,uint32_t a,uint32_t *v){return ck(s)||s->o->read(s->c,a,v);}
static int wr(struct core *s,uint32_t a,uint32_t v){
 if(ck(s))return -1;
 s->r->mutated=s->r->retained=1;s->r->writes++;
 return s->o->write(s->c,a,v);
}
static int mask(struct core *s,uint32_t a,uint32_t m,uint32_t v){uint32_t old;
 if(rd(s,a,&old))return -1;
 return wr(s,a,(old&~m)|(v&m));
}
static int waitus(struct core *s,unsigned n){return ck(s)||s->o->delay_us(s->c,n);}
static int take(struct core *s){if(ck(s))return -1;
 /* Callback failure conservatively may have acquired: retain semaphore. */
 s->r->sema_held=1;return s->o->sema(s->c,11,1);
}
static int release(struct core *s){if(ck(s)||s->o->sema(s->c,11,0))return -1;
 s->r->sema_held=0;return 0;
}
static int xfer(struct core *s,unsigned a,int w,uint32_t *v){
 if(ck(s)||!s->r->sema_held)return -1;
 s->r->transfers++;
 if(w)s->r->mutated=s->r->retained=1;
 return s->o->spi(s->c,a,w,v);
}
static int sr(struct core *s,unsigned a,uint32_t *v){return xfer(s,a,0,v);}
static int sw(struct core *s,unsigned a,uint32_t v){return xfer(s,a,1,&v);}
static int sm(struct core *s,unsigned a,uint32_t m,uint32_t v){uint32_t old;
 if(sr(s,a,&old))return -1;
 return sw(s,a,(old&~m)|(v&m));
}
static int sem19(struct core *s,int on){
 if(ck(s))return -1;
 if(on)s->r->sema19_held=1;
 if(s->o->sema(s->c,19,on))return -1;
 if(!on)s->r->sema19_held=0;
 return 0;
}
/* Exact source macro initialread + while(cnt>100) + reread/delay5. */
static int topclock(struct core *s,int on){uint32_t v;unsigned count=0;
 if(mask(s,0x18001900,2,on?2:0)||rd(s,0x18005008,&v))return -1;
 while(v&(1u<<26)){
  if(count>100)return -1;
  if(rd(s,0x18005008,&v)||waitus(s,5))return -1;
  count++;s->r->clock_polls++;
 }
 return 0;
}
#define DO(x) do{if((x)){r->error=-(int)r->phase;goto fail;}}while(0)
int npc_run(const struct npc_ops *o,void *c,unsigned opt,unsigned buildmode,struct npc_result *r){
 struct core s={o,c,r};uint32_t v,e[4]={0};
 if(!r)return -1;
 *r=(struct npc_result){0};
 if(opt!=1||buildmode>3||!o||!o->check||!o->read||!o->write||!o->delay_us||!o->sema||!o->spi){r->error=-100;return -1;}
 r->phase=1;DO(ck(&s));
 /* D-die handoff precedes hardware retention/SPI, even though vendor
  * A-die later clears entire SRAM. Do not move these words after clear. */
 DO(rd(&s,0x18001818,&v));DO(wr(&s,0x18052820,v));
 if(buildmode)DO(wr(&s,0x18052838,buildmode));
 for(unsigned i=0;i<NCC_COUNT;i++)DO(mask(&s,ncc_table[i].address,ncc_table[i].mask,ncc_table[i].value));
 r->phase=2;
 for(unsigned i=0;i<0x4000;i+=4)DO(wr(&s,0x18050000+i,0));
 /* GPIO180 SET aliases; MODE22 actual mask bits18:16 (70000). */
 DO(wr(&s,0x10005054,0x100000));DO(wr(&s,0x10005154,0x100000));
 DO(mask(&s,0x10005460,0x70000,0));DO(mask(&s,0x18001900,1,1));
 DO(take(&s));DO(sr(&s,0x02c,&r->chip));DO(release(&s));
 if(r->chip!=0x66358a11){r->error=-101;goto fail;}
 DO(wr(&s,0x18052800,r->chip));
 DO(take(&s));DO(sw(&s,0x064,0x7700));DO(release(&s));
 DO(take(&s));DO(sw(&s,0x0bc,0x21));DO(release(&s));
 DO(mask(&s,0x1800400c,0x80ff,0));DO(wr(&s,0x18002018,0x50305a00));
 r->phase=3;DO(take(&s));
 DO(sr(&s,0x108,&v));
 /* Initial busy => state not ours; never clear/retrigger an active read. */
 if(v&0x40000000){r->error=-102;goto fail;}
 DO(sw(&s,0x108,v&~0x40000000u));
 DO(sr(&s,0x108,&v));DO(sw(&s,0x108,(v&~0x43ff00c0u)|0x40000000));
 DO(sr(&s,0x108,&v));
 while((v&0x40000000)&&r->efuse_polls<70){r->efuse_polls++;DO(waitus(&s,30));DO(sr(&s,0x108,&v));}
 if(v&0x40000000){r->error=-103;goto fail;}
 DO(sr(&s,0x108,&v));r->efuse_valid=(v>>29)&1;
 /* Vendor invalid-efuse branch zeroes software thermal data then continues.
  * Private core records validity only; never publishes/logs efuse payload. */
 if(r->efuse_valid){
  DO(sr(&s,0x130,&e[0]));DO(wr(&s,0x18052804,e[0]));
  DO(sr(&s,0x134,&e[1]));DO(wr(&s,0x18052808,e[1]));
  if(e[1]&0x80){DO(sm(&s,0x034,0xf000,((e[1]>>3)&15)<<12));DO(sm(&s,0x038,0x03800000,(e[1]&7)<<23));}
  if(e[1]&0x8000)DO(sm(&s,0x038,0x0c000000,((e[1]>>13)&3)<<26));
  DO(sr(&s,0x138,&e[2]));DO(wr(&s,0x1805280c,e[2]));
  DO(sr(&s,0x13c,&e[3]));DO(wr(&s,0x18052810,e[3]));
 }
 DO(release(&s));r->phase=4;
 const uint32_t tail[][2]={{0x070,0x80000000},{0x380,0xe8002},{0x390,0xe8002},{0xa18,0xf6e8fff5},{0xa1c,0x55555fff},{0x384,0x2008},{0x394,0x2008}};
 for(unsigned i=0;i<7;i++){DO(take(&s));DO(sw(&s,tail[i][0],tail[i][1]));DO(release(&s));}
 DO(mask(&s,0x10005460,0x70000,0x10000));
 r->phase=5;
 const uint32_t afe[][2]={{0x10,0},{0x18,0x144b0160},{0x40,0x10990c13},{0x100,0x10990c13},{0x58,0xcd258051},{0x78,0xc5258251},{0x94,0xc5258251}};
 for(unsigned i=0;i<7;i++)DO(wr(&s,0x18003000+afe[i][0],afe[i][1]));
 DO(mask(&s,0x18003000,1,1));DO(waitus(&s,60));DO(mask(&s,0x18003000,1,0));
 DO(mask(&s,0x18003008,1u<<21,1u<<21));DO(waitus(&s,30));
 DO(mask(&s,0x18003008,1u<<20,1u<<20));DO(waitus(&s,60));
 for(int bit=21;bit>=17;bit--)DO(mask(&s,0x18003000,1u<<bit,1u<<bit));
 DO(waitus(&s,800));
 for(int bit=21;bit>=17;bit--)DO(mask(&s,0x18003000,1u<<bit,0));
 DO(mask(&s,0x18003008,1u<<21,0));DO(mask(&s,0x18003008,1u<<20,0));
 DO(take(&s));DO(sw(&s,0x070,5));DO(release(&s));
 r->phase=6;DO(mask(&s,0x180030f4,0x7fff7fff,0x03140521));DO(mask(&s,0x18003004,0xcf,0x4f));
 r->phase=7;
 /* WiFi radio3. Source voltage callback returns0 for drv_type!=0;
  * NO invented VCN13 raise/PMIC write in this WiFi-only path. */
 DO(sem19(&s,1));DO(mask(&s,0x18052834,8,8));DO(sem19(&s,0));
 r->phase=8;
 /* First-subsystem-on RC path, positive RC mode must be in entry proof. */
 DO(mask(&s,0x18060184,0xe00,0xe00));DO(mask(&s,0x18001860,0xf000,0x1000));
 const uint32_t rc[][2]={{0x183c,0x1838},{0x1844,0x1840},{0x184c,0x1848},{0x1854,0x1850}};
 for(unsigned i=0;i<4;i++){DO(wr(&s,0x18000000+rc[i][0],0x02080706));DO(mask(&s,0x18000000+rc[i][1],0x8000,0));}
 DO(mask(&s,0x18001834,0x10,0x10));DO(mask(&s,0x18001834,0x20,0x20));DO(mask(&s,0x18001834,0x40,0x40));
 DO(mask(&s,0x18001c00,1,1));DO(mask(&s,0x18001834,1,0));
 DO(mask(&s,0x18001c00,0x40000,0x40000));DO(mask(&s,0x18001c00,0x7ff0,0x3ff0));
 /* E3 uses e2_default. HOST_CONNINFRA topclock type4, mask7f. */
 DO(sem19(&s,1));DO(rd(&s,0x18052830,&v));
 if(!(v&0x7f))DO(topclock(&s,1));
 DO(mask(&s,0x18052830,0x10,0x10));DO(sem19(&s,0));
 DO(take(&s));DO(sm(&s,0xa18,0xc180,0xc180));DO(sm(&s,0xa1c,0xf0ff0080,0x50550080));DO(release(&s));
 DO(sem19(&s,1));DO(rd(&s,0x18052830,&v));
 if(v&0x10){DO(mask(&s,0x18052830,0x10,0));DO(rd(&s,0x18052830,&v));if(!(v&0x7f))DO(topclock(&s,0));}
 DO(sem19(&s,0));
 DO(mask(&s,0x18001a00,1u<<28,1u<<28));DO(mask(&s,0x180010f0,0x1b,0x1b));
 /* WiFi-only skips BGF power workaround. Source config/debug sequence. */
 DO(wr(&s,0x1800f000,0x32c8001c));DO(mask(&s,0x18001b00,7,0));DO(wr(&s,0x18000090,6));
 DO(mask(&s,0x18001860,0x200,0x200));
 r->phase=9;
 /* No extra CONN read/check after source switches bus control toHW.
  * Last guarded operation below; then ONLY source1860read/write. */
 DO(mask(&s,0x18001a00,1u<<23,0));
 DO(o->read(c,0x18001860,&v));r->writes++;
 DO(o->write(c,0x18001860,v|1));
 /* Selected platform PMIC ops has no common_power_low_power_mode hook:
  * pmic_mng wrapper returns0, no transport/callback is emitted. */
 r->phase=10;return 0;
fail:
 r->halt=r->mutated||r->sema_held||r->sema19_held;return -1;
}
