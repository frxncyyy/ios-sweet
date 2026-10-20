/* Opt-in XNU diagnostic: load/verify in RAM, log metadata, erase and free.
 * No firmware execution, EMI access, radio CPU, PMIC or MMIO operations. */
#include "wifi_firmware.h"
#include "wifi_firmware_probe.h"
#ifdef NWF_PROBE_HOST_TEST
#include "wifi_firmware_probe_test_platform.h"
#else
#include <sys/errno.h>
#include <sys/systm.h>
#include <kern/kalloc.h>
#include <kern/zalloc.h>
#include <kern/clock.h>
#include <kern/sched_prim.h>
#include <pexpert/pexpert.h>
#include <os/atomic_private.h>
#endif

#ifndef NWF_PROBE_HOST_TEST
/* Kernel-private declarations used by existing BSD sweet_ufs.c as well. */
extern kern_return_t kernel_thread_start(thread_continue_t, void *, thread_t *);
extern void thread_deallocate(thread_t);
extern void thread_terminate_self(void);
#endif
extern int sweet_ufs_read_vendor(uint64_t, void *, uint32_t);
extern void sweet_ufs_vendor_report(void);
static unsigned int nwf_probe_attempted;
struct nwf_probe_read_context { int error; };
static int
nwf_probe_read(void *context, uint64_t offset, void *buffer, uint32_t length)
{
 struct nwf_probe_read_context *state=context;
 int error=sweet_ufs_read_vendor(offset,buffer,length);
 if(error) state->error=error;
 return error;
}
static void
nwf_probe_erase(unsigned char *data,size_t size)
{
 volatile unsigned char *p=data;
 while(size--) *p++=0;
}
int
nord2_wifi_firmware_probe(void)
{
 int enabled=0,error=ENOMEM;
 unsigned char *output=NULL,*scratch=NULL;
 struct nwf_blob blobs[NWF_FILE_COUNT]={{0}};
 struct nwf_probe_read_context context={0};
 uint64_t start,end;
 if(!PE_parse_boot_argn("nord2wififw",&enabled,sizeof(enabled)) || enabled!=1)
  return EPERM;
 if(!os_atomic_cmpxchg(&nwf_probe_attempted,0u,1u,relaxed)) return EALREADY;
 clock_get_uptime(&start);
 /* kalloc.c:kalloc_large rejects Z_NOWAIT; NOPAGEWAIT allows lock contention
  * but avoids waiting for VM pages. Never use Z_NOFAIL for these large sizes. */
 output=kalloc_data(NWF_OUTPUT_BYTES,Z_WAITOK | Z_NOPAGEWAIT);
 if(!output) goto done;
 scratch=kalloc_data(NWF_SCRATCH_BYTES,Z_WAITOK | Z_NOPAGEWAIT);
 if(!scratch) goto done;
 if(nwf_load(nwf_probe_read,&context,output,NWF_OUTPUT_BYTES,
     scratch,NWF_SCRATCH_BYTES,blobs)) {
  error=context.error?context.error:EIO;
  goto done;
 }
 error=0;
 for(unsigned i=0;i<NWF_FILE_COUNT;i++)
  printf("NORD2 WIFI FW verified name=%s bytes=%u\n",blobs[i].name,blobs[i].size);
done:
 /* Always erase: also covers alloc failure, failed loader and future changes
  * to the loader's own failure cleanup. Nothing is retained or activated. */
 if(scratch) { nwf_probe_erase(scratch,NWF_SCRATCH_BYTES); kfree_data(scratch,NWF_SCRATCH_BYTES); }
 if(output) { nwf_probe_erase(output,NWF_OUTPUT_BYTES); kfree_data(output,NWF_OUTPUT_BYTES); }
 nwf_probe_erase((unsigned char *)blobs,sizeof(blobs));
 clock_get_uptime(&end);
 if(context.error) sweet_ufs_vendor_report();
 printf("NORD2 WIFI FW probe status=%d elapsed_ticks=%llu\n",error,
     (unsigned long long)(end-start));
 return error;
}

static unsigned int nwf_probe_start_attempted;
static void
nwf_probe_thread(void *argument,wait_result_t wait_result)
{
 (void)argument; (void)wait_result;
 (void)nord2_wifi_firmware_probe();
 thread_terminate_self();
}
int
nord2_wifi_firmware_start(void)
{
 int enabled=0;
 thread_t thread;
 if(!PE_parse_boot_argn("nord2wififw",&enabled,sizeof(enabled)) || enabled!=1)
  return EPERM;
 if(!os_atomic_cmpxchg(&nwf_probe_start_attempted,0u,1u,relaxed))
  return EALREADY;
 kern_return_t status=kernel_thread_start(nwf_probe_thread,NULL,&thread);
 if(status!=KERN_SUCCESS) {
  printf("NORD2 WIFI FW thread status=%d\n",status);
  return EAGAIN;
 }
 thread_deallocate(thread);
 return 0;
}
