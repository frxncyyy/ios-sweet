/* Compatible replacement registry classes: IOService ABI only. No proprietary
 * IO80211 vtable; a native-owner-gated BSD Ethernet path. Explicit software ADT provider. */
#ifdef NIWI_REGISTRY_HOST_TEST
#include "test-registry-xnu.h"
#else
#include <IOKit/IOService.h>
#include <IOKit/IOUserClient.h>
#include <IOKit/IOCatalogue.h>
#include <IOKit/IOLib.h>
#include <libkern/c++/OSArray.h>
#include <libkern/c++/OSDictionary.h>
#include <libkern/c++/OSString.h>
#include <libkern/c++/OSData.h>
#include <libkern/c++/OSNumber.h>
#include <kern/task.h>
#include <kern/clock.h>
#include <net/kpi_interface.h>
#include <sys/kpi_mbuf.h>
#include <sys/errno.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/if_media.h>
#include <net/if_ether.h>
#include <net/bpf.h>
#include <sys/kern_event.h>
#include <sys/sockio.h>
#include <sys/systm.h>
#endif
extern "C" {
#include "../../../bsd/dev/wifi_scan_service.h"
#include "../../../bsd/dev/wifi_ifnet_link.h"
#ifndef NIWI_REGISTRY_HOST_TEST
#include <pexpert/pexpert.h>
#endif
IOReturn niwl_create_client(IOService*,task_t,void*,UInt32,OSDictionary*,IOUserClient**);
}
static_assert(sizeof(ifmediareq)==44&&SIOCGIFMEDIA==0xc02c6938,"selected 64-bit ifmedia ABI");
/* SystemConfiguration enumerates IONetworkInterface descendants and classifies
 * their IO80211Controller parent. A Wi-Fi-only class chain is insufficient for
 * configd to discover the same BSD interface that wifid already opens. */
class IONetworkInterface : public IOService {
 OSDeclareDefaultStructors(IONetworkInterface)
};
OSDefineMetaClassAndStructors(IONetworkInterface,IOService)
class IO80211Interface : public IONetworkInterface {
 OSDeclareDefaultStructors(IO80211Interface)
public:
 IOReturn newUserClient(task_t t,void*s,UInt32 n,OSDictionary*p,IOUserClient**o) override {return niwl_create_client(this,t,s,n,p,o);}
};
OSDefineMetaClassAndStructors(IO80211Interface,IONetworkInterface)
class IO80211SkywalkInterface : public IO80211Interface {
 OSDeclareDefaultStructors(IO80211SkywalkInterface)
};
OSDefineMetaClassAndStructors(IO80211SkywalkInterface,IO80211Interface)
/* Original bounded adapter for an interface already attached by the BSD KPI.
 * Contract constants are corroborated by Apple's published IONetworkStack and
 * the selected configd consumer. This is not a copy of either implementation.
 * No ifnet is created, renamed, brought up or given a working data path here. */
static unsigned long long niws_ms(void){
#ifdef NIWI_REGISTRY_HOST_TEST
 return 0;
#else
 uint64_t now=0,ns=0;clock_get_uptime(&now);absolutetime_to_nanoseconds(now,&ns);return ns/1000000u;
#endif
}
class IONetworkStackUserClient;
class IONetworkStack : public IOService {
 OSDeclareDefaultStructors(IONetworkStack)
 IOLock *gate=nullptr;
 IOService *ownedInterface=nullptr;
 ifnet_t ownedNet=nullptr;
 uint64_t entryID=0;
 uint32_t unit=0;
 char bsdName[16]={};
 bool active=false;unsigned openLogs=0,propertyLogs=0;
 bool liveLocked(){
  if(!active||!ownedInterface||ownedInterface->isInactive()||!ownedNet)return false;
  ifnet_t found=nullptr;
  errno_t e=ifnet_find_by_name(bsdName,&found);
  bool same=!e&&found==ownedNet&&ifnet_unit(found)==unit;
  if(found)ifnet_release(found);
  return same;
 }
public:
 bool initOwned(IOService *interface,ifnet_t net){
  if(!interface||!net||!IOService::init())return false;
  gate=IOLockAlloc();if(!gate)return false;
  if(interface->isInactive()||strcmp(ifnet_name(net),"en")||ifnet_unit(net)>=32)return false;
  unit=ifnet_unit(net);snprintf(bsdName,sizeof bsdName,"en%u",unit);
  ifnet_t found=nullptr;errno_t e=ifnet_find_by_name(bsdName,&found);
  if(e||found!=net){if(found)ifnet_release(found);return false;}
  ownedNet=found;ownedInterface=interface;interface->retain();entryID=interface->getRegistryEntryID();
  return entryID!=0;
 }
 bool start(IOService *provider) override {
  if(!gate||!ownedInterface||!ownedNet||!IOService::start(provider))return false;
  if(!setProperty("IOUserClientClass","IONetworkStackUserClient")){IOService::stop(provider);return false;}
  setName("IONetworkStack");active=true;return true;
 }
 void deactivate(){if(gate){IOLockLock(gate);active=false;IOLockUnlock(gate);}}
 void stop(IOService *provider) override {deactivate();IOService::stop(provider);}
 void free(void) override {
  if(ownedInterface){ownedInterface->release();ownedInterface=nullptr;}
  if(ownedNet){ifnet_release(ownedNet);ownedNet=nullptr;}
  if(gate){IOLockFree(gate);gate=nullptr;}
  IOService::free();
 }
 IOReturn setProperties(OSObject *properties) override {
  if(IOUserClient::clientHasPrivilege(current_task(),kIOClientPrivilegeAdministrator)!=kIOReturnSuccess)return kIOReturnNotPrivileged;
  OSDictionary *dict=OSDynamicCast(OSDictionary,properties);
  if(!dict)return kIOReturnBadArgument;
  OSNumber *command=OSDynamicCast(OSNumber,dict->getObject("IONetworkStackUserCommand"));
  if(!command)return kIOReturnBadArgument;
  uint64_t cmd=command->unsigned64BitValue(),target=0,requested=0;
  if(cmd>2)return kIOReturnUnsupported;
  if(cmd==2){
   /* Global operation, not an individual target. Owned pending set is empty:
    * the sole owned interface was attached by the BSD KPI before publication. */
   if(dict->getObject("IORegistryEntryID")||dict->getObject("IOInterfaceUnit"))return kIOReturnBadArgument;
  }else{
   OSNumber *requestedUnit=OSDynamicCast(OSNumber,dict->getObject("IOInterfaceUnit"));
   OSData *id=OSDynamicCast(OSData,dict->getObject("IORegistryEntryID"));
   /* Selected caller supplies all three. No path fallback/numeric truncation. */
   if(!requestedUnit||!id||id->getLength()!=sizeof(uint64_t))return kIOReturnBadArgument;
   requested=requestedUnit->unsigned64BitValue();if(requested>UINT32_MAX)return kIOReturnBadArgument;
   memcpy(&target,id->getBytesNoCopy(),sizeof target);
  }
  if(!gate)return kIOReturnNotReady;
  IOLockLock(gate);IOReturn result=kIOReturnNotReady;bool live=liveLocked();
  if(live){
   if(cmd==2)result=kIOReturnSuccess;
   else if(target!=entryID)result=kIOReturnNoDevice;
   /* Already attached interfaces cannot be re-reserved by Apple's state
    * machine. Preserve NoSpace on repeat; do not claim rename/new attach. */
   else result=kIOReturnNoSpace;
  }
  if(propertyLogs<16){propertyLogs++;printf("NORD2 WIFI IOS STACK SET: command=%u unit=%u live=%u status=%#x t_ms=%llu\n",(unsigned)cmd,(unsigned)requested,(unsigned)live,(unsigned)result,niws_ms());}
  IOLockUnlock(gate);return result;
 }
 IOReturn newUserClient(task_t,void*,UInt32,OSDictionary*,IOUserClient**) override;
 void opened(){IOLockLock(gate);if(openLogs<8){openLogs++;printf("NORD2 WIFI IOS STACK OPEN: type=0 unit=%u ifstate=1 t_ms=%llu\n",unit,niws_ms());}IOLockUnlock(gate);}
 bool ready(){if(!gate)return false;IOLockLock(gate);bool r=liveLocked();IOLockUnlock(gate);return r;}
};
OSDefineMetaClassAndStructors(IONetworkStack,IOService)

class IONetworkStackUserClient : public IOUserClient {
 OSDeclareDefaultStructors(IONetworkStackUserClient)
 IOLock *gate=nullptr;
 task_t owner=TASK_NULL;
 IONetworkStack *stack=nullptr;
 void closeOwned(){
  if(!gate)return;IOLockLock(gate);IONetworkStack *old=stack;stack=nullptr;
  task_t task=owner;owner=TASK_NULL;IOLockUnlock(gate);
  if(old){old->close(this);old->release();}if(task)task_deallocate(task);
 }
public:
 bool initWithTask(task_t task,void *security,UInt32 type,OSDictionary *properties) override {
  if(!task||current_task()!=task||type||IOUserClient::clientHasPrivilege(security,kIOClientPrivilegeAdministrator)!=kIOReturnSuccess)return false;
  if(!IOUserClient::initWithTask(task,security,type,properties))return false;
  gate=IOLockAlloc();if(!gate)return false;task_reference(task);owner=task;return true;
 }
 bool start(IOService *provider) override {
  IONetworkStack *candidate=OSDynamicCast(IONetworkStack,provider);
  if(!candidate||!candidate->ready()||!IOUserClient::start(provider))return false;
  if(!candidate->open(this)){IOUserClient::stop(provider);return false;}
  candidate->retain();stack=candidate;return true;
 }
 IOReturn setProperties(OSObject *properties) override {
  if(!gate)return kIOReturnNotOpen;
  IOLockLock(gate);IOReturn result=kIOReturnNotOpen;
  if(stack){
   if(current_task()!=owner||IOUserClient::clientHasPrivilege(current_task(),kIOClientPrivilegeAdministrator)!=kIOReturnSuccess)result=kIOReturnNotPrivileged;
   else result=stack->setProperties(properties);
  }
  IOLockUnlock(gate);return result;
 }
 IOReturn clientClose(void) override {closeOwned();if(!isInactive())terminate();return kIOReturnSuccess;}
 IOReturn clientDied(void) override {return clientClose();}
 void stop(IOService *provider) override {closeOwned();IOUserClient::stop(provider);}
 void free(void) override {closeOwned();if(gate){IOLockFree(gate);gate=nullptr;}IOUserClient::free();}
};
OSDefineMetaClassAndStructors(IONetworkStackUserClient,IOUserClient)
IOReturn IONetworkStack::newUserClient(task_t task,void *security,UInt32 type,OSDictionary *properties,IOUserClient **out){
 if(!out)return kIOReturnBadArgument;*out=nullptr;
 if(type)return kIOReturnUnsupported;
 if(!task||current_task()!=task||IOUserClient::clientHasPrivilege(security,kIOClientPrivilegeAdministrator)!=kIOReturnSuccess)return kIOReturnNotPrivileged;
 if(!ready())return kIOReturnNotReady;
 auto *client=new IONetworkStackUserClient;
 if(!client)return kIOReturnNoMemory;
 if(!client->initWithTask(task,security,type,properties)){client->release();return kIOReturnNoResources;}
 if(!client->attach(this)){client->release();return kIOReturnNoResources;}
 if(!client->start(this)){client->detach(this);client->release();return kIOReturnNotReady;}
 *out=client;opened();return kIOReturnSuccess;
}

#include "../../../bsd/dev/wifi_ifnet_link.inc"

class IO80211Controller : public IOService {
 OSDeclareDefaultStructors(IO80211Controller)
 ifnet_t net=nullptr;IO80211SkywalkInterface*interface=nullptr;IONetworkStack *stack=nullptr;
 static void detached(ifnet_t n){ifnet_release(n);}

public:
 bool start(IOService*) override;
 void stop(IOService*) override;
 IOReturn newUserClient(task_t t,void*s,UInt32 n,OSDictionary*p,IOUserClient**o) override {return niwl_create_client(this,t,s,n,p,o);}
};
OSDefineMetaClassAndStructors(IO80211Controller,IOService)
bool IO80211Controller::start(IOService*p){
 net=nullptr;interface=nullptr;stack=nullptr;unsigned opt=0;PE_parse_boot_argn("nord2wifiios",&opt,sizeof opt);
 OSData*compatible=OSDynamicCast(OSData,p->getProperty("compatible"));
 static const char expected[]="nord2,mt6893-wifi";
 if(opt!=1||!compatible||compatible->getLength()!=sizeof expected||memcmp(compatible->getBytesNoCopy(),expected,sizeof expected))return false;
 nss_info info={};if(nord2_wifi_scan_info(&info)||!info.count||!IOService::start(p))return false;
 static const uint8_t broadcast[6]={255,255,255,255,255,255};
 ifnet_init_eparams params={};params.ver=IFNET_INIT_CURRENT_VERSION;params.len=sizeof params;params.flags=IFNET_INIT_LEGACY;params.name="en";params.family=IFNET_FAMILY_ETHERNET;params.subfamily=IFNET_SUBFAMILY_WIFI;params.type=IFT_ETHER;params.output=nwil_output;params.ioctl=nwil_ioctl;params.demux=ether_demux;params.add_proto=ether_add_proto;params.del_proto=ether_del_proto;params.check_multi=ether_check_multi;params.framer_extended=ether_frameout_extended;params.detach=detached;params.broadcast_addr=broadcast;params.broadcast_len=6;
 /* Let ifnet_attach arbitrate real unit collisions; never rename an existing interface. */
 for(unsigned unit=0;unit<32;unit++){
  params.unit=unit;if(ifnet_allocate_extended(&params,&net))continue;
  ifnet_set_addrlen(net,6);ifnet_set_hdrlen(net,14);
  sockaddr_dl address={};address.sdl_len=sizeof address;address.sdl_family=AF_LINK;address.sdl_alen=6;memcpy(address.sdl_data,info.mac,6);
  errno_t error=ifnet_attach(net,&address);if(!error)break;ifnet_release(net);net=nullptr;
 }
 if(!net){printf("NORD2 WIFI IOS REGISTRY: registered=0 failure=1\n");IOService::stop(p);return false;}
 if(ifnet_set_lladdr(net,info.mac,6)){printf("NORD2 WIFI IOS REGISTRY: registered=0 failure=2\n");ifnet_detach(net);net=nullptr;IOService::stop(p);return false;}
 char name[16];snprintf(name,sizeof name,"%s%u",ifnet_name(net),ifnet_unit(net));
 if(ifnet_set_mtu(net,1500)||ifnet_set_flags(net,IFF_BROADCAST|IFF_MULTICAST|IFF_SIMPLEX,IFF_BROADCAST|IFF_MULTICAST|IFF_SIMPLEX|IFF_RUNNING|IFF_UP)){printf("NORD2 WIFI IOS REGISTRY: registered=0 failure=6\n");ifnet_detach(net);net=nullptr;IOService::stop(p);return false;}
 if(!setProperty("IOMACAddress",info.mac,sizeof info.mac)||!setProperty("IOBuiltin",true)){
  printf("NORD2 WIFI IOS REGISTRY: registered=0 failure=8\n");ifnet_detach(net);net=nullptr;IOService::stop(p);return false;
 }
 /* Discovery does not signal carrier. Only native station/key/data readiness can. */
 /* IOInterfaceName is only on actual interface: one bind target. */
 interface=new IO80211SkywalkInterface;
 if(!interface||!interface->init()){printf("NORD2 WIFI IOS REGISTRY: registered=0 failure=3\n");if(interface)interface->release();interface=nullptr;ifnet_detach(net);net=nullptr;IOService::stop(p);return false;}
 if(!interface->attach(this)){printf("NORD2 WIFI IOS REGISTRY: registered=0 failure=4\n");interface->release();interface=nullptr;ifnet_detach(net);net=nullptr;IOService::stop(p);return false;}
 if(!interface->start(this)){printf("NORD2 WIFI IOS REGISTRY: registered=0 failure=5\n");interface->detach(this);interface->release();interface=nullptr;ifnet_detach(net);net=nullptr;IOService::stop(p);return false;}
 if(!interface->setProperty("IOInterfaceName",name)||!interface->setProperty("BSD Name",name)||!interface->setProperty("IOInterfaceUnit",(uint64_t)ifnet_unit(net),32)||!interface->setProperty("IOInterfaceType",(uint64_t)IFT_ETHER,32)||!interface->setProperty("IO80211InterfaceRole","Infrastructure")||!interface->setProperty("IOInterfaceNamePrefix","en")||!interface->setProperty("IOBuiltin",true)||!interface->setProperty("IOPrimaryInterface",true)||!interface->setProperty("IOInterfaceState",(uint64_t)1,32)){
  printf("NORD2 WIFI IOS REGISTRY: registered=0 failure=7\n");interface->stop(this);interface->detach(this);interface->release();interface=nullptr;ifnet_detach(net);net=nullptr;IOService::stop(p);return false;
 }
 /* Publish only after the existing real ifnet and its interface identity are valid.
  * The stack owns an additional lookup reference, never another ifnet. */
 IOService *resources=IOService::getResourceService();stack=new IONetworkStack;
 bool stackAttached=false,stackStarted=false;
 if(stack&&resources&&stack->initOwned(interface,net)){
  stackAttached=stack->attach(resources);if(stackAttached)stackStarted=stack->start(resources);
 }
 if(!stackStarted){
  if(stack){if(stackAttached)stack->detach(resources);stack->release();stack=nullptr;}
  interface->stop(this);interface->detach(this);interface->release();interface=nullptr;
  ifnet_detach(net);net=nullptr;IOService::stop(p);return false;
 }
 if(nwil_bind(net)){
  stack->deactivate();stack->terminate();stack->detach(resources);stack->release();stack=nullptr;
  interface->stop(this);interface->detach(this);interface->release();interface=nullptr;
  ifnet_detach(net);net=nullptr;IOService::stop(p);return false;
 }
 /* DHCP uses BPF on the same Ethernet endpoint; detach tears BPF down. */
 bpfattach(net,DLT_EN10MB,14);
 interface->registerService();stack->registerService();registerService();
 printf("NORD2 WIFI IOS STACK: registered=1 unit=%u ifstate=1 t_ms=%llu\n",ifnet_unit(net),niws_ms());printf("NORD2 WIFI IOS REGISTRY: registered=1 unit=%u media=%#x carrier=0 network_class=1 builtin=1\n",ifnet_unit(net),IFM_IEEE80211|IFM_AUTO);return true;
}
void IO80211Controller::stop(IOService*p){
 if(net)nwil_unbind(net);
 /* Invalidate the stack before removing the identity or detaching BSD. */
 if(stack){stack->deactivate();stack->terminate();stack->detach(IOService::getResourceService());stack->release();stack=nullptr;}
 if(interface)interface->removeProperty("IOInterfaceState");
 if(interface){interface->terminate();interface->detach(this);interface->release();interface=nullptr;}
 if(net){ifnet_detach(net);net=nullptr;}IOService::stop(p);
}
/* Root must call after IOKit catalogue initialization. No service instance or
 * physical address is fabricated here; personality matches the explicit ADT node. */
extern "C" bool nord2_wifi_ios_register(void){
 nss_info actual={};if(nord2_wifi_scan_info(&actual)||!actual.count)return false;
 unsigned opt=0;PE_parse_boot_argn("nord2wifiios",&opt,sizeof opt);if(opt!=1||!gIOCatalogue)return false;
 OSDictionary*d=OSDictionary::withCapacity(5);OSArray*a=OSArray::withCapacity(1);if(!d||!a){if(d)d->release();if(a)a->release();return false;}
 const char*keys[]={"IOClass","IOProviderClass","IONameMatch","IOMatchCategory"};const char*values[]={"IO80211Controller","IOPlatformDevice","wlan","Nord2WiFiController"};
 for(unsigned i=0;i<4;i++){OSString*s=OSString::withCString(values[i]);if(!s){d->release();a->release();return false;}d->setObject(keys[i],s);s->release();}
 a->setObject(d);d->release();bool result=gIOCatalogue->addDrivers(a,true);a->release();return result;
}

extern "C" void nord2_wifi_ios_backend_ready(void){bool submitted=nord2_wifi_ios_register();printf("NORD2 WIFI IOS CATALOGUE: submitted=%u\n",(unsigned)submitted);}
