#define NFP_PRESPAN_HOST_TEST 1
#include "wifi_main_fw_xnu.c"
#include <assert.h>
#include <string.h>
struct nwfn_native nwfn_native;
struct nfa_native nfa_native;
static struct nrb_native rom;
static struct npp_state pool;
static uint32_t host_page[1024];
static unsigned owner_calls,span_calls,owner_error,receipt_valid;
int nrpc_receipt_ok(const struct nrb_native *p){assert(p==&rom);return receipt_valid;}
int nwfn_owner(void *p){assert(p==&nwfn_native);owner_calls++;if(owner_error){nwfn_native.error=owner_error;nwfn_native.halt=1;return -1;}return 0;}
int nfp_span(const struct nfp_io *o,void *ctx,const struct nmt_layout *l,struct nfp_state *s,unsigned index){
 assert(o&&ctx==&nwfn_native&&l==&nfp_native.layout&&s==&nfp_native.state);assert(index<7);span_calls++;return 0;
}
void nfp_test_barrier(void){assert(0);}
static void fresh(unsigned sync){
 memset(&nfp_native,0,sizeof(nfp_native));memset(&nwfn_native,0,sizeof(nwfn_native));memset(&rom,0,sizeof(rom));memset(&pool,0,sizeof(pool));memset(host_page,0,sizeof(host_page));
 owner_calls=span_calls=owner_error=0;receipt_valid=1;
 nfp_native.prepared=1;nfp_native.host_io=(uintptr_t)host_page;host_page[0xf0/4]=sync;
 rom.enabled=1;rom.io=1;nwfn_native.rom=&rom;nwfn_native.pool=&pool;
 pool.stage=NPP_DONE;pool.mapped=pool.verified=pool.zero_complete=pool.balanced=pool.asleep=1;
 nwfn_native.init.pool=&pool;nwfn_native.init.initialized=1;nwfn_native.init.phase=5;nwfn_native.init.driver_owned=1;nwfn_native.init.transport.phase=2;
 nwfn_native.stage=NWFN_RETAINED;nwfn_native.ap_owned=nwfn_native.wf_owned=1;nwfn_native.claim.base=0x90000000;nwfn_native.claim.bytes=0xf20000;
}
static void stopped(unsigned reason){assert(nfp_native.pre_span_reason==reason);assert(nfp_native.halt&&nwfn_native.halt&&nwfn_native.init.halt&&nwfn_native.init.transport.halt&&nwfn_native.init.transport.dirty);assert(!span_calls);}
static struct {uintptr_t cfg,host;}npa_native;
static struct {unsigned active;struct {struct npp_state state;}probe;} nppa_native;
static int nwfn_stop(struct nwfn_native *n,unsigned e){n->error=e;n->halt=1;n->init.halt=n->init.transport.halt=1;return -1;}
void panic(const char *format,...){(void)format;assert(0);}
#include "wifi_main_bind.inc"
int main(void){
 (void)nwfn_after_pool;

 for(unsigned raw=0;raw<4;raw++){
  fresh(raw);int rc=nfp_xnu_span(0);assert(nfp_native.pre_sync_valid&&nfp_native.pre_sync==raw&&owner_calls==1);
  if(raw==3){assert(rc==-1);stopped(NFP_PRE_READY);}else assert(rc==0&&span_calls==1&&!nfp_native.halt);
 }
 fresh(0x40000001);assert(!nfp_xnu_span(0)&&span_calls==1);/* unrelated bits ignored */
 for(unsigned reason=0;reason<7;reason++){
  fresh(0);
  switch(reason){case 0:nfp_native.prepared=0;break;case 1:rom.enabled=0;break;case 2:pool.verified=0;break;case 3:nwfn_native.init.driver_owned=0;break;case 4:nwfn_native.init.transport.phase=1;break;case 5:nwfn_native.wf_owned=0;break;default:nwfn_native.claim.bytes--;break;}
  assert(nfp_xnu_span(0)==-1);stopped(NFP_PRE_SAME);assert(nfp_native.same_bits==(1u<<reason));assert(!owner_calls&&!nfp_native.pre_sync_valid);
 }
 fresh(0);receipt_valid=0;assert(nfp_xnu_span(0)==-1);stopped(NFP_PRE_SAME);assert(nfp_native.same_bits==2);
 fresh(0);owner_error=6;assert(nfp_xnu_span(0)==-1);stopped(NFP_PRE_OWNER);assert(nfp_native.original_native_error==6&&!nfp_native.pre_sync_valid);
 for(unsigned reason=0;reason<3;reason++){
  fresh(0);if(reason==0)nwfn_native.init.transport.completed=1;else if(reason==1)nwfn_native.init.transport.finish_phase=1;else nfp_native.coex_attempted=1;
  assert(nfp_xnu_span(0)==-1);stopped(NFP_PRE_ORDER);assert(nfp_native.pre_sync_valid&&nfp_native.pre_span_index==0);
 }
 fresh(0);assert(nfp_xnu_span(2)==-1);stopped(NFP_PRE_ORDER);assert(!owner_calls&&!nfp_native.pre_sync_valid&&nfp_native.pre_span_index==2);
 /* Shared halt makes a later call refuse before any owner/read/copy. */
 unsigned prior=owner_calls;assert(nfp_xnu_span(0)==-1&&owner_calls==prior&&!span_calls);
 fresh(0);nwfn_native.error=6;nwfn_native.halt=1;
 nwfn_main_failure(-1);assert(nwfn_main.original_native_error==6&&nwfn_main.original_native_halt==1&&nwfn_native.error==21);
 prior=owner_calls;nwfn_main_diagnostics();assert(owner_calls==prior);
 puts("PASS actual native pre-span adapter: raw0/1/2 proceed, full3 refuses; same bit reasons, owner original error, prior event/finish/coex order, shared halt, no copy/remap on refusal. Synthetic model only.");
}
