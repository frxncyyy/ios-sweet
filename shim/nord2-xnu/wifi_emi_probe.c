/* Isolated boot bridge. Nothing is activated until the explicit start hook. */
#include "wifi_emi_snapshot.h"
#include "wifi_emi_probe.h"
#include "wifi_emi_legacy.h"
#ifdef NWEP_HOST_TEST
#include "wifi_emi_probe_test_platform.h"
#else
#include <sys/errno.h>
#include <sys/systm.h>
#include <kern/clock.h>
#include <kern/sched_prim.h>
#include <pexpert/pexpert.h>
#include <os/atomic_private.h>
extern kern_return_t kernel_thread_start(thread_continue_t,void *,thread_t *);
extern void thread_deallocate(thread_t);
extern void thread_terminate_self(void);
#endif
static unsigned int nwep_attempted,nwep_start_attempted;
static struct wifi_emi_legacy_snapshot wifi_emi_legacy_result;
/* mode0 refused,1 original observer,2 legacy. Conflict always refuses. */
static int wifi_emi_legacy_mode(void)
{
 int old=0,legacy=0;
 int old_present=PE_parse_boot_argn("nord2wifiemi",&old,sizeof(old));
 int new_present=PE_parse_boot_argn("nord2wifiemilegacy",&legacy,sizeof(legacy));
 if(new_present && legacy==1)return old_present && old==0?2:0;
 return old_present && old==1?1:0;
}
int wifi_emi_legacy_probe(void)
{
 if(wifi_emi_legacy_mode()!=2)return EPERM;
 if(!os_atomic_cmpxchg(&nwep_attempted,0u,1u,relaxed))return EALREADY;
 uint64_t start,end;clock_get_uptime(&start);
 printf("NORD2 WIFI EMI LEGACY begin queries_max=128 deadline=none effective=unknown\n");
 int status=wifi_emi_legacy_collect(wifi_emi_legacy_native_call,NULL,&wifi_emi_legacy_result);
 for(unsigned i=0;i<128;i++){
  const struct wifi_emi_legacy_field *f=&wifi_emi_legacy_result.field[i];
  printf("NORD2 WIFI EMI LEGACY index=%u offset=%#x status=%u error=%d raw=%llx/%llx/%llx/%llx\n",
   i,f->offset,(unsigned)f->status,f->error,(unsigned long long)f->raw[0],
   (unsigned long long)f->raw[1],(unsigned long long)f->raw[2],(unsigned long long)f->raw[3]);
 }
 clock_get_uptime(&end);
 printf("NORD2 WIFI EMI LEGACY end collector=%d issued=%u valid=%#x/%#x/%#x/%#x elapsed_ticks=%llu effective=unknown\n",
  status,wifi_emi_legacy_result.issued,wifi_emi_legacy_result.valid[0],wifi_emi_legacy_result.valid[1],
  wifi_emi_legacy_result.valid[2],wifi_emi_legacy_result.valid[3],(unsigned long long)(end-start));
 return status?EIO:0;
}
int nord2_wifi_emi_probe(void)
{
 int error=0;
 uint64_t start,end;
 const uint32_t regions[2]={27,31};
 struct nwes_region result[2]={0};
 if(wifi_emi_legacy_mode()==2)return wifi_emi_legacy_probe();
 if(wifi_emi_legacy_mode()!=1)return EPERM;
 if(!os_atomic_cmpxchg(&nwep_attempted,0u,1u,relaxed)) return EALREADY;
 clock_get_uptime(&start);
 printf("NORD2 WIFI EMI begin queries_max=8 deadline=none observation_only=1\n");
 int status=nwes_collect(nwes_native_call,NULL,regions,2,result,2);
 if(status) error=EIO;
 unsigned issued=0;
 for(unsigned i=0;i<2;i++) {
  for(unsigned f=0;f<NWES_FIELDS;f++) {
   const struct nwes_field *v=&result[i].field[f];
   if(v->status!=NWES_NOT_CALLED) issued++;
   if(v->status!=NWES_VALUE) error=EIO;
   printf("NORD2 WIFI EMI region=%u field=%u status=%u known_error=%d raw=%llx/%llx/%llx/%llx\n",
    result[i].region,f,(unsigned)v->status,v->known_error,
    (unsigned long long)v->raw[0],(unsigned long long)v->raw[1],
    (unsigned long long)v->raw[2],(unsigned long long)v->raw[3]);
  }
  uint64_t first=0,last=0;
  uint32_t ap=0,conn=0,scp=0;
  int range=nwes_decode_range(&result[i],UINT64_C(0x40000000),
      UINT64_C(0x23fffffff),16,&first,&last);
  int domains=nwes_domain(&result[i],0,&ap) |
      nwes_domain(&result[i],2,&conn) | nwes_domain(&result[i],3,&scp);
  if(range || domains) error=EIO;
  printf("NORD2 WIFI EMI decoded region=%u range_valid=%u first=%llx last=%llx domains_valid=%u ap=%u conn=%u scp=%u effective=unknown\n",
   result[i].region,range==0,(unsigned long long)first,(unsigned long long)last,
   domains==0,ap,conn,scp);
 }
 clock_get_uptime(&end);
 printf("NORD2 WIFI EMI end status=%d collector=%d issued=%u elapsed_ticks=%llu effective=unknown\n",
  error,status,issued,(unsigned long long)(end-start));
 return error;
}
static void nwep_thread(void *argument,wait_result_t wait_result)
{
 (void)argument;(void)wait_result;
 (void)nord2_wifi_emi_probe();
 thread_terminate_self();
}
int nord2_wifi_emi_start(void)
{
 thread_t thread;
 if(!wifi_emi_legacy_mode())return EPERM;
 if(!os_atomic_cmpxchg(&nwep_start_attempted,0u,1u,relaxed)) return EALREADY;
 kern_return_t status=kernel_thread_start(nwep_thread,NULL,&thread);
 if(status!=KERN_SUCCESS) {
  printf("NORD2 WIFI EMI thread status=%d\n",status);return EAGAIN;
 }
 thread_deallocate(thread);return 0;
}
