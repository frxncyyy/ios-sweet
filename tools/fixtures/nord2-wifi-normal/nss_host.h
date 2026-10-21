#ifndef NSS_HOST_LOCK_H
#define NSS_HOST_LOCK_H
#include <pthread.h>
#include <stdlib.h>
typedef struct {pthread_mutex_t m;}IOLock;
static IOLock*IOLockAlloc(void){IOLock*p=malloc(sizeof(*p));assert(p&&!pthread_mutex_init(&p->m,0));return p;}
static void IOLockLock(IOLock*p){assert(!pthread_mutex_lock(&p->m));}
static void IOLockUnlock(IOLock*p){assert(!pthread_mutex_unlock(&p->m));}
static void thread_wakeup(event_t e){assert(e);}
static unsigned service_ready_hooks;
void nord2_wifi_ios_backend_ready(void){service_ready_hooks++;}
#endif
