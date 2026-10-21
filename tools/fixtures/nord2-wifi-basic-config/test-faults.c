#define main actual_owner_fixture_main
#include "test-current-owner.c"
#undef main
static unsigned calls,fail_at,failed,after;
static int hit(void){if(failed)after++;if(++calls==fail_at){failed=1;return -1;}return 0;}
static int fo(void*c){if(hit())return -1;return ncd_native_owner(c);}
static int fr(void*c,uint32_t a,uint32_t*v){if(hit())return -1;return ncd_native_read(c,a,v);}
static int fw(void*c,uint32_t a,uint32_t v){if(hit())return -1;return ncd_native_write(c,a,v);}
static int fmr(void*c,uint64_t a,void*p,uint32_t n){if(hit())return -1;return ncd_native_mread(c,a,p,n);}
static int fmw(void*c,uint64_t a,const void*p,uint32_t n){if(hit())return -1;return ncd_native_mwrite(c,a,p,n);}
static int fs(void*c,uint64_t a,uint32_t n,unsigned w){if(hit())return -1;return ncd_native_sync(c,a,n,w);}
static int fd(void*c,unsigned us){if(hit())return -1;return ncd_native_delay(c,us);}
static int fn(void*c,uint64_t*t){if(hit())return -1;return ncd_native_now(c,t);}
static const struct nwt_io bus={fo,fr,fw,fmr,fmw,fs,fd};
static const struct ncd_io io={&bus,fn};
static void prepared(void){ready_fixture();run_worker();ncd_native.parked=0;nbc_native.enabled=nbc_native.attempted=1;
 assert(!nbc_prepare(&ncd_native.diag,&nwfn_native.init,&nfp_native,&nbc_native.state));calls=failed=after=0;}
int main(void){fail_at=0;prepared();assert(!nbc_run(&io,&ncd_native,&nbc_native.state));unsigned total=calls;assert(total==nbc_native.state.callbacks&&total>50);
 for(unsigned k=1;k<=total;k++){
  fail_at=k;prepared();assert(nbc_run(&io,&ncd_native,&nbc_native.state)<0);
  struct nbc_state*s=&nbc_native.state;
  assert(failed&&!after&&calls==k&&s->halt&&s->error&&!s->completed&&nwfn_native.init.halt&&nwfn_native.init.transport.halt&&nwfn_native.init.transport.completed==4&&basic_pubs<=1);
  unsigned was=calls;assert(nbc_run(&io,&ncd_native,s)<0&&calls==was); /* no retry */
  if(s->budget.reserved)assert(s->budget.pse_remaining==8);else assert(s->budget.pse_remaining==9);
 }
 /* Clone cannot acquire native authority merely by copying pure prepared state. */
 fail_at=0;prepared();struct nbc_state clone=nbc_native.state;
 assert(nbc_run(&io,&ncd_native,&clone)<0&&!basic_pubs&&nwfn_native.error==5&&nwfn_native.init.transport.completed==4);
 prepared();worker_thread=3;unsigned reads=mmio_reads,writes=mmio_writes;
 assert(nbc_run(&io,&ncd_native,&nbc_native.state)<0&&!basic_pubs&&reads==mmio_reads&&writes==mmio_writes);
 /* Pure CAP/resource refusals never invoke a backend. */
 prepared();memset(&nbc_native.state,0,sizeof(nbc_native.state));ncd_native.diag.frame_bytes=39;
 assert(nbc_prepare(&ncd_native.diag,&nwfn_native.init,&nfp_native,&nbc_native.state)==1&&nbc_native.state.skip_reason==2&&!nbc_native.state.callbacks);
 prepared();memset(&nbc_native.state,0,sizeof(nbc_native.state));
 assert(nbc_prepare(0,&nwfn_native.init,&nfp_native,&nbc_native.state)==1&&nbc_native.state.skip_reason==1&&!nbc_native.state.callbacks);
 printf("PASS %u forwardedcallback fault positions through REAL nativeowner/candidatelease; no postfailurecallback/retry/refund/completed5, atmostonepublication. Synthetic only.\n",total);
 return 0;
}
