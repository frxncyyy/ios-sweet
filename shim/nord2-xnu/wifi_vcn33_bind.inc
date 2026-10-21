/* Include after persistent binder and failure_drain, before main binder.
 * This is the sole input-thread PMIF owner. Workers only publish one atomic. */
#ifndef NVC_HOST_TEST
#include <kern/clock.h>
#include <kern/thread.h>
#endif
#include "wifi_vcn33.h"
static struct {struct nvc_state state;thread_t owner;unsigned enabled,halt,samples;uint64_t start,next;}nvc_native;
static unsigned nvc_phase;
static void nwm_failure_drain(void);
static void nvc_mark(unsigned phase){__atomic_store_n(&nvc_phase,phase,__ATOMIC_RELEASE);}
static uint64_t nvc_now(void){uint64_t t,ns;clock_get_uptime(&t);absolutetime_to_nanoseconds(t,&ns);return ns/1000;}
static int nvc_owner(void*c){
 if(c!=&nvc_native||!nvc_native.enabled||nvc_native.halt||current_thread()!=nvc_native.owner||
 !npa_persistent_lifetime||!npa_native.outer_retained||!npa_native.state.entered||npa_native.state.fatal||npa_native.state.result.phase!=10||
 !npa_native.proof||npa_native.proof(npa_native.proof_cookie)||
 !nw.saved||!nw.domain_touched||!ni_saved||!ncc_fresh_owned||ncc_latched_dirty)return -1;
 return 0;
}
static int nvc_read(void*c,uint16_t a,uint16_t*v){if(nvc_owner(c))return -1;return ni_pmic_read(a,v)?0:-1;}
static int nvc_rc(void*c,uint32_t*v){if(!v||nvc_owner(c))return -1;*v=ni_rd(nw.spm,0x504);return 0;}
static int nvc_write(void*c,uint16_t a,uint16_t v){
 if(nvc_owner(c))return -1;
 /* Five slots only; reject callback replay, arbitrary values or additional base EN writes. */
 unsigned k=nvc_native.state.writes;
 const uint16_t addr[5]={0x1f98,0x1f9c,0x1be8,0x1be8,0x1c0e};
 uint16_t expected=k<2?(uint16_t)((nvc_native.state.before.value[k?11:5]&~0xf00u)|0xd00):(k==2?0x8040:(k==3?0x8060:0x8040));
 if(k>=5||a!=addr[k]||v!=expected||!nvc_native.state.mutated)return -1;
 if(!nw_wait(ni_pwrap,0xca8,0x800e,0x8000))return -1;
 ni_pwrap_owned=true;ni_wr(ni_pwrap,0xc84,v);ni_wr(ni_pwrap,0xc80,(1u<<29)|a);
 return nw_wait(ni_pwrap,0xca8,0x800e,0x8000)?0:-1;
}
static const struct nvc_io nvc_ops={nvc_owner,nvc_read,nvc_write,nvc_rc};
static void nvc_log(unsigned kind,const struct nvc_snapshot*s){
 const uint16_t*v=s->value;
 printf("NORD2 WIFI VCN33 SNAP: kind=%u phase=%u valid=%#x rc=%#x time_us=%llu values=%#x/%#x/%#x/%#x/%#x/%#x/%#x/%#x/%#x/%#x/%#x/%#x/%#x\n",
 kind,__atomic_load_n(&nvc_phase,__ATOMIC_ACQUIRE),s->valid,s->rc,(unsigned long long)nvc_now(),v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10],v[11],v[12]);
}
static void nvc_stop(void){nvc_native.halt=1;nwm_failure_drain();panic("NORD2 WIFI VCN33: retain lifetime; manual reset required");}
static void nvc_after_persistent(void){
 unsigned opt=0,main=0,persist=0;PE_parse_boot_argn("nord2wifivcn33",&opt,sizeof(opt));if(opt!=1)return;
 PE_parse_boot_argn("nord2wifimain",&main,sizeof(main));PE_parse_boot_argn("nord2wifipersist",&persist,sizeof(persist));
 if(main!=1||persist!=1||nvc_native.enabled){nvc_stop();return;}
 nvc_native.owner=current_thread();nvc_native.enabled=1;nvc_mark(1);
 int result=nvc_run(&nvc_ops,&nvc_native,&nvc_native.state);
 nvc_log(0,&nvc_native.state.before);nvc_log(1,&nvc_native.state.after);
 printf("NORD2 WIFI VCN33: status=%d attempted=%u complete=%u mutated=%u writes=%u error=%u\n",result,nvc_native.state.attempted,nvc_native.state.complete,nvc_native.state.mutated,nvc_native.state.writes,nvc_native.state.error);
 if(result){nvc_stop();return;}
 nvc_native.start=nvc_now();nvc_native.next=nvc_native.start;
}
static void nvc_observe(unsigned kind){
 if(!nvc_native.enabled||nvc_native.halt)return;
 struct nvc_snapshot s;int result=nvc_capture(&nvc_ops,&nvc_native,&s);nvc_log(kind,&s);
 if(result){if(kind==4){nvc_native.next=UINT64_MAX;return;}nvc_stop();}
}
static void nvc_poll(void){
 if(!nvc_native.enabled||nvc_native.halt)return;
 uint64_t now=nvc_now();if(now<nvc_native.start||now-nvc_native.start>=30000000||nvc_native.samples>=300)return;
 if(now<nvc_native.next)return;nvc_native.next=now+100000;nvc_native.samples++;nvc_observe(4);
}
#define NVC_MAIN_OBSERVE(k) nvc_observe(k)
#define NVC_PHASE(k) nvc_mark(k)
