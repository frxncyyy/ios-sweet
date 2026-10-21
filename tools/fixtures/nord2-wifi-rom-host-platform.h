#ifndef ROM_HOST_PLATFORM_H
#define ROM_HOST_PLATFORM_H
#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
typedef uintptr_t vm_offset_t;typedef size_t vm_size_t;typedef void *DTEntry;
typedef struct {uint64_t physBase,memSize,memSizeActual;} boot_args;
extern struct host_pe_state {void *bootArgs;} PE_state;
#define kSuccess 0
#define Z_WAITOK 1
#define Z_NOPAGEWAIT 2
#define NRB_DEVICE_BARRIER() do {} while(0)
int PE_parse_boot_argn(const char *,void *,int);
int SecureDTLookupEntry(void *,const char *,DTEntry *);
int SecureDTGetProperty(DTEntry,const char *,const void **,unsigned *);
void *kalloc_data(size_t,int);void kfree_data(void *,size_t);
int nrb_host_smc(const uint64_t[8],uint64_t[4]);
#endif
