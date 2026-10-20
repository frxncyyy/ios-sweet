#!/usr/bin/env python3
"""Exact-source bridge lifecycle tests; no device IO or firmware activation."""
from pathlib import Path
import subprocess
import tempfile
ROOT=Path(__file__).resolve().parents[1]
def main():
    with tempfile.TemporaryDirectory() as directory:
        d=Path(directory)
        (d/'wifi_firmware_probe_test_platform.h').write_text(r'''
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#define Z_WAITOK 0
#define Z_NOPAGEWAIT 2
#define os_atomic_cmpxchg(p,a,b,o) __sync_bool_compare_and_swap(p,a,b)
static int arg_present=1,arg=1,fail_alloc,allocs,frees,reads,loads,logs,io_error,loader_error;
static unsigned char *buffers[2];
static size_t sizes[2];
static uint64_t ticks;
static int PE_parse_boot_argn(const char *name,void *value,int size) {
 assert(!strcmp(name,"nord2wififw") && size==sizeof(int));
 *(int *)value=arg;return arg_present;
}
static void *kalloc_data(size_t size,unsigned flags) {
 assert(flags==(Z_WAITOK|Z_NOPAGEWAIT));assert(allocs<2);
 int index=allocs++;
 if(fail_alloc==index+1)return NULL;
 sizes[index]=size;buffers[index]=malloc(size);assert(buffers[index]);
 memset(buffers[index],0xa5,size);return buffers[index];
}
static void kfree_data(void *ptr,size_t size) {
 int index=ptr==buffers[0]?0:1;
 assert(ptr==buffers[index] && size==sizes[index]);
 for(size_t i=0;i<size;i++)assert(((unsigned char *)ptr)[i]==0);
 free(ptr);buffers[index]=NULL;frees++;
}
static void clock_get_uptime(uint64_t *time) {*time=++ticks;}
static int probe_test_log(const char *fmt,...) {
 assert(strstr(fmt,"NORD2 WIFI FW"));logs++;return 0;
}
typedef int kern_return_t;
typedef int wait_result_t;
typedef unsigned long thread_t;
typedef void (*thread_continue_t)(void *,wait_result_t);
#define KERN_SUCCESS 0
static int start_error,starts,deallocs,terminations;
static thread_continue_t continuation;
static kern_return_t kernel_thread_start(thread_continue_t fn,void *arg,thread_t *thread) {
 starts++;assert(!arg);if(start_error)return start_error;
 continuation=fn;*thread=42;return KERN_SUCCESS;
}
static void thread_deallocate(thread_t thread) {assert(thread==42);deallocs++;}
static void thread_terminate_self(void) {terminations++;}
#define printf probe_test_log
''')
        (d/'test.c').write_text(r'''
#include "wifi_firmware_probe.c"
int sweet_ufs_read_vendor(uint64_t offset,void *buffer,uint32_t length) {
 assert(offset==4096 && length==4096);reads++;
 if(io_error)return io_error;
 memset(buffer,0xc3,length);return 0;
}
int nwf_load(nwf_read_fn read,void *ctx,unsigned char *out,size_t cap,
 unsigned char *scratch,size_t scratchcap,struct nwf_blob blobs[4]) {
 loads++;assert(cap==NWF_OUTPUT_BYTES && scratchcap==NWF_SCRATCH_BYTES);
 memset(out,0xc4,cap);memset(scratch,0xc5,scratchcap);
 if(read(ctx,4096,scratch,4096) || loader_error)return -1;
 for(unsigned i=0;i<4;i++){blobs[i].name="test-firmware";blobs[i].size=i+1;blobs[i].data=out;}
 return 0;
}
static void reset(void) {
 assert(!buffers[0] && !buffers[1]);
 nwf_probe_attempted=nwf_probe_start_attempted=0;arg_present=arg=1;
 start_error=starts=deallocs=terminations=0;continuation=NULL;
 fail_alloc=allocs=frees=reads=loads=logs=io_error=loader_error=0;
}
int main(void) {
 for(int value=-2;value<=3;value++)if(value!=1){
  reset();arg=value;assert(nord2_wifi_firmware_probe()==EPERM);
  assert(!allocs && !reads && !loads && !logs && !nwf_probe_attempted);
 }
 reset();arg_present=0;assert(nord2_wifi_firmware_probe()==EPERM);assert(!allocs && !nwf_probe_attempted);
 for(int which=1;which<=2;which++){
  reset();fail_alloc=which;assert(nord2_wifi_firmware_probe()==ENOMEM);
  assert(allocs==which && frees==which-1 && !reads && !loads && logs==1);
  assert(nord2_wifi_firmware_probe()==EALREADY);
 }
 reset();loader_error=1;assert(nord2_wifi_firmware_probe()==EIO);
 assert(allocs==2 && frees==2 && loads==1 && reads==1 && logs==1);
 reset();io_error=ENXIO;assert(nord2_wifi_firmware_probe()==ENXIO);
 assert(frees==2 && logs==1);
 reset();assert(!nord2_wifi_firmware_probe());assert(frees==2 && loads==1 && reads==1 && logs==5);
 assert(nord2_wifi_firmware_probe()==EALREADY && allocs==2 && reads==1);
 reset();nwf_probe_attempted=1;assert(nord2_wifi_firmware_probe()==EALREADY && !allocs);
 for(int value=-2;value<=3;value++)if(value!=1){
  reset();arg=value;assert(nord2_wifi_firmware_start()==EPERM);
  assert(!starts && !allocs && !deallocs && !terminations && !nwf_probe_start_attempted);
 }
 reset();arg_present=0;assert(nord2_wifi_firmware_start()==EPERM && !starts);
 reset();start_error=5;assert(nord2_wifi_firmware_start()==EAGAIN);
 assert(starts==1 && !deallocs && !terminations && !allocs);
 assert(nord2_wifi_firmware_start()==EALREADY && starts==1);
 reset();assert(!nord2_wifi_firmware_start());
 assert(starts==1 && deallocs==1 && continuation && !allocs && !terminations);
 continuation(NULL,0);
 assert(terminations==1 && frees==2 && reads==1 && logs==5);
 assert(nord2_wifi_firmware_start()==EALREADY && starts==1 && deallocs==1);
 reset();loader_error=1;assert(!nord2_wifi_firmware_start());continuation(NULL,0);
 assert(terminations==1 && frees==2 && logs==1 && deallocs==1);
 return 0;
}
''')
        exe=d/'test'
        subprocess.run(['cc','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-omit-frame-pointer','-DNWF_PROBE_HOST_TEST','-I'+str(d),'-I'+str(ROOT/'shim/nord2-xnu'),str(d/'test.c'),'-o',str(exe)],check=True)
        subprocess.run([str(exe)],check=True)
        print('PASS: exact bridge ASan/UBSan; strict gate; once guard; allocation failures; loader/disk errors; full erase/free; metadata-only logs; spawn failures/reference cleanup/thread termination; no activation')
if __name__=='__main__':main()
