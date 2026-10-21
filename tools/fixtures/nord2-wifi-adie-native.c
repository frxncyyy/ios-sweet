#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <setjmp.h>
#include "wifi_common_scope.h"
#include "wifi_adie_strap.h"
#include "wifi_cold_snapshot.h"
typedef uintptr_t vm_offset_t;
static struct {vm_offset_t spm,infra,gals;bool saved,domain_touched;} nw;
static bool ncc_fresh_owned,ncc_latched_dirty;
static vm_offset_t ni_gpio=0x10005000;
static unsigned commonopt,idopt,maps,reads,writes,logs,failwrite,guarddrop,hang,semdeny,owned,triggers,releases,delays,outerrestores,clockfail,irqfail;
static uint32_t chipid;
static uint32_t regs[40],before[40],strap[4],original[4],topaddr;
static jmp_buf trap;
static uint32_t ni_rd(vm_offset_t,unsigned);
static void ni_wr(vm_offset_t,unsigned,uint32_t);
static int PE_parse_boot_argn(const char*k,unsigned*v,unsigned n){assert(n==4);if(!strcmp(k,"nord2wificommon"))*v=commonopt;else{assert(!strcmp(k,"nord2wifiadieid"));*v=idopt;}return 1;}
static vm_offset_t ml_io_map(uint64_t a,unsigned n){assert(n==4096&&(a==0x18000000||a==0x18004000||a==0x18005000||a==0x18008000||a==0x18072000));maps++;return a;}
static void delay(unsigned us){assert(us==1||us==10||us==50);delays++;}
static int logline(const char*f,...){char b[256];va_list a;va_start(a,f);int n=vsnprintf(b,sizeof(b),f,a);va_end(a);assert(n>=0&&n<255);logs++;return 0;}
static void panic(const char*s){assert(strstr(s,"dirty restoration"));assert(ncc_latched_dirty&&ncc_fresh_owned&&nw.saved&&nw.domain_touched);longjmp(trap,1);}
#define printf logline
#define NCC_DEVICE_BARRIER() ((void)0)
#define NCC_TICKS() UINT64_C(100)
#define NCC_FREQUENCY() UINT64_C(13000000)
#include "wifi_common_adapter.inc"
#undef printf
static int indexof(uint32_t a){for(unsigned i=0;i<40;i++)if(ncc_table[i].address==a)return (int)i;return -1;}
static uint32_t ni_rd(vm_offset_t b,unsigned o){uint32_t a=b+o;reads++;
 switch(a){case 0x18060000:return clockfail?0:6;case 0x180602d4:return irqfail?0:1;case 0x10006304:return guarddrop&&triggers?0:0xd;case 0x1000616c:case 0x10006170:return 2;case 0x1020e504:return 1;case 0x10001228:return 0xe70c500e;case 0x10001724:case 0x18060184:return 0;case 0x1807202c:if(semdeny)return 0;assert(!owned);owned=1;return 1;case 0x18004050:return topaddr;case 0x18004058:assert(owned&&triggers==1&&!hang);return chipid;case 0x18004000:return hang&&triggers?0x20:0;}
 for(unsigned i=0;i<4;i++)if(nis_fields[i].address==a)return strap[i];
 for(unsigned i=0;i<14;i++)if(ncc_predicates[i].address==a)return 0;
 int i=indexof(a);assert(i>=0);return regs[i];
}
static void ni_wr(vm_offset_t b,unsigned o,uint32_t v){uint32_t a=b+o;if(a==0x18060000){assert((v&1)&&!owned&&!triggers);return;}
 writes++;
 if(a==0x1807222c){assert(v==1&&owned&&(!triggers||!hang));owned=0;releases++;return;}
 if(a==0x18004050){assert(owned&&(v==0xb02c||v==0x12345678));topaddr=v^(writes==failwrite?1:0);return;}
 if(a==0x18004054){assert(v==0&&owned&&topaddr==0xb02c);for(unsigned i=0;i<40;i++)assert(regs[i]==((before[i]&~ncc_table[i].mask)|(ncc_table[i].value&ncc_table[i].mask)));for(unsigned i=0;i<4;i++)assert(strap[i]==((original[i]&~nis_fields[i].mask)|nis_fields[i].value));assert(!triggers);triggers++;if(writes==failwrite)hang=1;return;}
 for(unsigned i=0;i<4;i++){
  uint32_t target=nis_fields[i].address;
  if(i<2&&(a==target+4||a==target+8)){assert(v==nis_fields[i].mask);if(a==target+4)strap[i]|=v;else strap[i]&=~v;if(writes==failwrite)strap[i]^=v;return;}
  if(i>=2&&a==target){strap[i]=v;if(writes==failwrite)strap[i]^=nis_fields[i].mask&-nis_fields[i].mask;return;}
 }
 int i=indexof(a);assert(i>=0);
 /* No common restore until reset/strap AND semaphore cleanup complete. */
 if(triggers){assert(!owned);for(unsigned j=0;j<4;j++)assert(strap[j]==original[j]);}
 regs[i]=v^(writes==failwrite?1:0);
}
static void reset(void){memset(&ncc_native,0,sizeof(ncc_native));memset(&nis_native,0,sizeof(nis_native));nw.spm=0x10006000;nw.infra=0x10001000;nw.gals=0x1020e000;nw.saved=nw.domain_touched=ncc_fresh_owned=true;ncc_latched_dirty=false;commonopt=idopt=1;maps=reads=writes=logs=failwrite=guarddrop=hang=semdeny=owned=triggers=releases=delays=outerrestores=clockfail=irqfail=0;chipid=0x66358a11;topaddr=0x12345678;for(unsigned i=0;i<40;i++)regs[i]=before[i]=0x12340000+i;strap[0]=original[0]=0x100;strap[1]=original[1]=0x200;strap[2]=original[2]=0x01111111;strap[3]=original[3]=0x1200;}
static int run(void){if(setjmp(trap))return -1;ncc_power_window(0x18060000,0x18001000,0x20010101);assert(!ncc_latched_dirty);outerrestores++;return 0;}
int main(void){unsigned total;
 reset();commonopt=0;assert(!run()&&!maps&&!reads&&!writes&&!logs);
 reset();idopt=0;assert(!run()&&!triggers&&maps==4&&writes==80);
 reset();assert(!run()&&triggers==1&&!owned&&releases==1&&nis_native.result.clean&&ncc_native.result.clean&&outerrestores==1);assert(!memcmp(regs,before,sizeof(regs))&&!memcmp(strap,original,sizeof(strap)));total=writes;
 /* Source addr+WD+addrrestore,1semaphore release,4strap apply+restore,
  *40common apply+restore =>92writes. Inject all except semaphore release,
  *whose nativewrite has no recoverable error channel/readback authority. */
 assert(total==92);
 for(unsigned i=1;i<=total;i++){
  if(i==48)continue; /* native semaphore release has no recoverable fault channel */
  reset();failwrite=i;int rc=run();if(!rc){assert(ncc_native.result.clean);assert(!memcmp(regs,before,sizeof(regs))&&!memcmp(strap,original,sizeof(strap)));}else assert(ncc_latched_dirty&&!outerrestores);
 }
 reset();chipid=0x99999999;assert(!run()&&!owned&&nis_native.result.id.id_valid&&!nis_native.result.id.known_chip&&ncc_native.result.clean);
 reset();clockfail=1;assert(!run()&&!triggers&&!owned&&nis_native.result.clean&&!nis_native.clock_ok&&nis_native.clock_polls==1600);
 reset();irqfail=1;assert(!run()&&!triggers&&!owned&&nis_native.result.clean&&!nis_native.clock_ok);
 reset();semdeny=1;assert(!run()&&!triggers&&!owned&&delays==1001&&nis_native.result.clean);
 reset();hang=1;assert(run()==-1&&owned&&!releases&&!outerrestores&&nis_native.retained&&!ncc_native.result.clean);assert(!ncc_native.result.restored);
 reset();guarddrop=1;assert(run()==-1&&owned&&!releases&&!outerrestores&&!ncc_native.result.restored);
 reset();strap[3]|=1;assert(!run()&&!writes&&!triggers); /* initially active A-die refused */
 puts("PASS exactnative scoped lifetime/order;optout;91writepoint corruptions of92writes;semaphore timeout;ambiguousSPI+guardloss retain allownership/halt;pre-reset refusal;clockdetect/IRQ refusal;bounded logs");return 0;}
