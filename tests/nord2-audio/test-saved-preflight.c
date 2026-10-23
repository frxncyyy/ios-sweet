/* Tests actual candidate transport; existing parent register model is read-only.
 * No model or parent I2C test is modified. Board allowlist is unloaded here. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "audio_i2c8.h"
#include "test-i2c-model.h"
int nah_board_register_allowed(unsigned r){(void)r;return 0;}
int nah_board_change_allowed(unsigned r,uint16_t a,uint16_t b){(void)r;(void)a;(void)b;return 0;}
static const uint32_t pa[NA8_AP_FIELDS]={0x11d04124,0x11d04110,0x11d0410c,0x11d041f4,0x11d041ec};
static const unsigned failure[NA8_AP_FIELDS]={6,7,8,9,21};
static const uint32_t good[NA8_AP_FIELDS]={2,0x38,0,0,0};
static const uint32_t bad[NA8_AP_FIELDS]={3,0x3c,2,1,0x8000};
struct context{struct i2c8_model model;unsigned reads,writes,delays,ap_reads;int fail_index,failed;uint32_t order[64];};
static struct context c;
static struct na8_state s;
static void reset(void){memset(&c,0,sizeof(c));memset(&s,0,sizeof(s));m_init(&c.model);c.fail_index=-1;for(unsigned i=0;i<NA8_AP_FIELDS;i++)c.model.ch[1][(pa[i]&255)/2]=(uint16_t)good[i];}
static int readio(void *cookie,uint32_t addr,unsigned width,uint32_t *v){
 struct context *x=cookie;assert(!x->failed);x->reads++;
 for(unsigned i=0;i<NA8_AP_FIELDS;i++)if(addr==pa[i]){
  assert(width==2&&x->ap_reads<64);x->order[x->ap_reads++]=addr;
  if(x->fail_index==(int)i){x->failed=1;*v=0xdead;return -1;}
 }
 *v=m_read(&x->model,addr,width);return 0;
}
static int writeio(void *cookie,uint32_t addr,unsigned width,uint32_t v){struct context *x=cookie;assert(!x->failed);x->writes++;m_write(&x->model,addr,width,v);return 0;}
static int delayio(void *cookie,unsigned us){struct context *x=cookie;assert(!x->failed&&us<=10);x->delays++;return 0;}
static const struct na8_io io={readio,writeio,delayio};
static int transfer(void){uint16_t value=0;return na8_read16(&io,&c,&s,0x34,3,&value);}
static void no_mutation(void){assert(!c.writes&&!c.delays&&!c.model.starts&&!s.mutated&&!s.uncertain&&na8_return_ok(&s));}
int main(void){
 /* Every predicate combination retains old priority, but captures all5 reads. */
 for(unsigned mask=1;mask<(1u<<NA8_AP_FIELDS);mask++){
  reset();unsigned first=NA8_AP_FIELDS;
  for(unsigned i=0;i<NA8_AP_FIELDS;i++)if(mask&(1u<<i)){c.model.ch[1][(pa[i]&255)/2]=(uint16_t)bad[i];if(first==NA8_AP_FIELDS)first=i;}
  assert(transfer()==-1&&s.failure==failure[first]&&s.stage==8&&s.offline);
  assert(s.ap_valid==NA8_AP_VALID_ALL&&c.ap_reads==NA8_AP_FIELDS);
  for(unsigned i=0;i<NA8_AP_FIELDS;i++){assert(c.order[i]==pa[i]);assert(s.ap_preflight[i]==((mask&(1u<<i))?bad[i]:good[i]));}
  assert(s.last_addr==pa[first]&&s.last_value==bad[first]&&s.last_valid&&s.last_width==2&&!s.last_write);no_mutation();
 }
 /* Read callback failure stops immediately; only successful earlier slots valid.
  * Callback deliberately writes garbage on failure: invalid saved slot stays0. */
 for(unsigned fail=0;fail<NA8_AP_FIELDS;fail++){
  reset();c.fail_index=(int)fail;assert(transfer()==-1&&s.failure==10);
  assert(c.ap_reads==fail+1&&s.ap_valid==((1u<<fail)-1));
  assert(s.last_addr==pa[fail]&&!s.last_valid&&!s.last_value&&s.last_width==2&&!s.last_write);
  for(unsigned i=0;i<NA8_AP_FIELDS;i++)assert(s.ap_preflight[i]==(i<fail?good[i]:0));
  unsigned reads=c.reads;assert(transfer()==-1&&c.reads==reads);no_mutation();
 }
 /* Preserve earlier ownership guards: never reach AP collection on refusal. */
 for(unsigned guard=0;guard<6;guard++){
  reset();unsigned expected=0;
  if(guard==0){c.model.ch[0][0x24/2]=3;expected=18;}
  if(guard==1){c.model.ch[2][0x24/2]=3;expected=17;}
  if(guard==2){c.model.ch[2][0x10/2]=4;expected=17;}
  if(guard==3){c.model.ch[0][0x10/2]=4;expected=7;}
  if(guard==4){c.model.ch[0][0xf4/2]=1;expected=9;}
  if(guard==5){c.model.ifr1=0;c.model.ifr2=0;c.model.dma[2]=1;expected=7;}
  assert(transfer()==-1&&s.failure==expected&&!c.ap_reads&&!s.ap_valid);no_mutation();
 }
 /* AP START2 remains permitted; no guard or transfer behavior relaxed. */
 reset();assert(transfer()==0&&s.ap_valid==NA8_AP_VALID_ALL&&s.ap_preflight[NA8_AP_START]==2);
 assert(c.model.starts==1);m_check_restored(&c.model);
 /* Snapshot clears before a later call fails in an earlier phase. */
 c.model.mux|=0x8000;assert(transfer()==-1&&s.failure==1&&!s.ap_valid);
 for(unsigned i=0;i<NA8_AP_FIELDS;i++)assert(!s.ap_preflight[i]);
 puts("PASS actualtransport:31predicatecombinations,5callbackfailures,6earlierguards,START2success,stalevalidreset; zeroadditionalwrites/read-after-failure");
}
