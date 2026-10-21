#include "wifi_rom_powered_adapter.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

struct bus {
 uint32_t a[128],v[128],fail_write_addr;unsigned n,io,fail_at,after_sleep;
 unsigned policy,place,release,rom_reads,bad_receipt,bad_policy,bad_place,never_done;
 struct nrpa *adapter;
};
static uint32_t *reg(struct bus *b,uint32_t a) {
 for(unsigned i=0;i<b->n;i++)if(b->a[i]==a)return &b->v[i];
 assert(b->n<128);b->a[b->n]=a;b->v[b->n]=0;return &b->v[b->n++];
}
static int step(struct bus *b) {
 if(b->adapter&&b->adapter->asleep)b->after_sleep++;
 b->io++;return b->fail_at&&b->io==b->fail_at;
}
static int owner(void *p){struct bus *b=p;return step(b);}
static int read32(void *p,uint32_t a,uint32_t *v) {
 struct bus *b=p;if(step(b))return -1;
 if(a==0x180602ccu)*v=(*reg(b,0x18000000u)&0x80u)?4u:0u;
 else if(a==0x18001620u||a==0x18001624u)
  *v=*reg(b,a)&1u?9u:0u;
 else if(a==0x184c1604u){b->rom_reads++;*v=(!b->never_done&&b->release)?0x1d1eu:0u;}
 else if(a==0x18072050u)*v=1;
 else *v=*reg(b,a);
 return 0;
}
static int write32(void *p,uint32_t a,uint32_t v) {
 struct bus *b=p;if(step(b))return -1;
 if(b->fail_write_addr==a&&(a!=0x18000010u||(v&1u)))return -1;
 if(a==0x18000010u&&(v&1u)){
  assert(b->place&&!b->release&&!b->bad_receipt);b->release++;
 }
 *reg(b,a)=v;return 0;
}
static int delay_us(void *p,unsigned us){struct bus *b=p;assert(us==1||us==30||us==50||us==500||us==1000);return step(b)?-1:0;}
static const struct nrpa_hw hw={owner,read32,write32,delay_us};
/* Native policy/placement test doubles execute on the SAME prepared object;
 * the production adapter links actual nrb_xnu_policy/nrb_xnu_place. */
int nrb_xnu_policy(struct nrb_native *n,int (*own)(void *,enum nrb_owner_stage),void *p){
 struct nrpa *a=p;struct bus *b=a->ctx;
 assert(n==a->rom);if(own(p,NRB_COLD_BEFORE_POWER))return -1;
 b->policy++;
 if(b->bad_policy)return -1;
 n->state.policy_attempted=1;n->state.issued=2;n->state.accepted=2;
 n->state.raw_valid=3;n->state.raw[0][0]=0;n->state.raw[1][0]=0;
 n->state.phase=3;return 0;
}
int nrb_xnu_place(struct nrb_native *n){
 struct nrpa *a=(struct nrpa *)n->owner_cookie;struct bus *b=a->ctx;
 assert(n==a->rom&&a->state.stage==NRPC_ROM_PLACE&&a->state.cpu_held);
 b->place++;
 if(b->bad_place){n->state.halt=1;n->state.error=-12;return -1;}
 n->state.mapped=1;n->state.backup_complete=1;n->state.write_attempted=1;
 n->state.writes=88;n->state.reads=176;n->state.barriers=2;
 n->state.placed=1;n->state.phase=b->bad_receipt?6:7;
 return 0;
}
static void init(struct bus *b,struct nrpa *a,struct nrb_native *n){
 memset(b,0,sizeof(*b));memset(a,0,sizeof(*a));memset(n,0,sizeof(*n));
 b->adapter=a;n->state.prepared=1;n->state.phase=1;n->owner_cookie=a;
 n->state.plan.spans[0].bytes=49820;
 n->state.plan.spans[1].bytes=16;
 n->state.plan.spans[2].bytes=296868;
 n->state.plan.spans[3].bytes=16;
 *reg(b,0x10006304u)=0xdu;*reg(b,0x1000616cu)=2;
 *reg(b,0x10006170u)=2;*reg(b,0x1020e504u)=1;
 *reg(b,0x18001000u)=0x20010101u;
 *reg(b,0x18000008u)=0x12u;*reg(b,0x18000014u)=0;
 *reg(b,0x18003008u)=0x4400u;*reg(b,0x18060000u)=6;
 *reg(b,0x180602d4u)=1;*reg(b,0x184b0010u)=0x20010000u;
 *reg(b,0x18001620u)=1;*reg(b,0x18001624u)=1;
}
static void happy(void){
 struct bus b;struct nrpa a;struct nrb_native n;init(&b,&a,&n);
 assert(nrpa_start(&a,&hw,&b,&n,0)==-1&&b.io==0);
 assert(nrpa_start(&a,&hw,&b,&n,1)==0);
 assert(a.state.stage==NRPC_RUNNING&&a.state.rom_ready&&a.state.balanced);
 assert(a.state.policy_accepted&&a.state.rom_placed&&a.state.cpu_released);
 assert(a.state.cpu_release_attempted);
 assert(a.state.rom_done==0x1d1eu&&a.state.rom_polls==1);
 assert(!a.state.ap_vote&&!a.state.wf_vote&&!a.state.clock_vote&&
  !a.state.sem20&&a.state.power&&a.state.protection&&a.asleep);
 assert(b.policy==1&&b.place==1&&b.release==1&&!b.after_sleep);
 assert(nrpa_start(&a,&hw,&b,&n,1)==-1&&b.after_sleep==0);
}
static void bad_policy(void){
 struct bus b;struct nrpa a;struct nrb_native n;init(&b,&a,&n);b.bad_policy=1;
 assert(nrpa_start(&a,&hw,&b,&n,1)<0);
 assert(a.state.halt&&a.state.ap_vote&&a.state.wf_vote&&
  !a.state.cpu_held&&!a.state.clock_vote&&!b.release&&!b.place);
}
static void bad_receipt(void){
 struct bus b;struct nrpa a;struct nrb_native n;init(&b,&a,&n);b.bad_receipt=1;
 assert(nrpa_start(&a,&hw,&b,&n,1)<0);
 assert(a.state.halt&&a.state.ap_vote&&a.state.wf_vote&&
  a.state.cpu_held&&a.state.clock_vote&&a.state.power&&!b.release);
}
static void bad_place(void){
 struct bus b;struct nrpa a;struct nrb_native n;init(&b,&a,&n);b.bad_place=1;
 assert(nrpa_start(&a,&hw,&b,&n,1)<0);
 assert(a.state.halt&&a.state.ap_vote&&a.state.wf_vote&&
  a.state.cpu_held&&a.state.clock_vote&&a.state.power&&
  !a.state.cpu_release_attempted&&!b.release);
}
static void rom_timeout(void){
 struct bus b;struct nrpa a;struct nrb_native n;init(&b,&a,&n);b.never_done=1;
 assert(nrpa_start(&a,&hw,&b,&n,1)<0);
 assert(a.state.rom_polls==102&&b.rom_reads==102&&a.state.halt);
 assert(a.state.cpu_released&&a.state.power&&a.state.clock_vote&&
  a.state.ap_vote&&a.state.wf_vote&&!a.asleep&&b.release==1);
}
static void release_uncertain(void){
 struct bus b;struct nrpa a;struct nrb_native n;init(&b,&a,&n);
 b.fail_write_addr=0x18000010u;
 assert(nrpa_start(&a,&hw,&b,&n,1)<0);
 assert(a.state.cpu_release_attempted&&!a.state.cpu_released&&
  a.state.cpu_held&&a.state.halt&&a.state.power&&
  a.state.clock_vote&&a.state.ap_vote&&a.state.wf_vote);
}
static void faults(void){
 struct bus b;struct nrpa a;struct nrb_native n;init(&b,&a,&n);
 assert(nrpa_start(&a,&hw,&b,&n,1)==0);unsigned total=b.io;
 for(unsigned i=1;i<=total;i++){
  init(&b,&a,&n);b.fail_at=i;
  int rc=nrpa_start(&a,&hw,&b,&n,1);
  assert(rc<0);
  assert(!b.after_sleep);
  if(b.release)assert(a.state.rom_placed&&n.state.phase==7);
  if(a.state.policy_attempted)assert(a.state.halt);
 }
 printf("fault positions: %u\n",total);
}
int main(void){happy();bad_policy();bad_receipt();bad_place();rom_timeout();release_uncertain();faults();puts("rom-powered PASS");}
