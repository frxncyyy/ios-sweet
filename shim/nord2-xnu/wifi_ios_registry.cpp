/* Compatible replacement registry classes: IOService ABI only. No proprietary
 * IO80211 vtable or data/link implementation. Explicit software ADT provider. */
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
#include <net/kpi_interface.h>
#include <sys/kpi_mbuf.h>
#include <sys/errno.h>
#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/if_media.h>
#include <sys/sockio.h>
#include <sys/systm.h>
#endif
extern "C" {
#include "../../../bsd/dev/wifi_scan_service.h"
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
class IO80211Controller : public IOService {
 OSDeclareDefaultStructors(IO80211Controller)
 ifnet_t net=nullptr;IO80211SkywalkInterface*interface=nullptr;
 static errno_t output(ifnet_t,mbuf_t p){mbuf_freem(p);return ENETDOWN;}
 static errno_t demux(ifnet_t,mbuf_t,char*,protocol_family_t*){return EOPNOTSUPP;}
 static errno_t add(ifnet_t,protocol_family_t,const ifnet_demux_desc*,u_int32_t){return EOPNOTSUPP;}
 static errno_t del(ifnet_t,protocol_family_t){return 0;}
 static errno_t multi(ifnet_t,const sockaddr*){return EOPNOTSUPP;}
 static void detached(ifnet_t n){ifnet_release(n);}
 static errno_t ioctl(ifnet_t,u_long cmd,void*data){
  if(cmd==SIOCSIFFLAGS)return 0; /* administrative flags only; never link/carrier */
  if(cmd!=SIOCGIFMEDIA)return EOPNOTSUPP;
  if(!data)return EINVAL;struct ifmediareq*r=(struct ifmediareq*)data;
  int requested=r->ifm_count;int media=IFM_IEEE80211|IFM_AUTO;
  r->ifm_current=media;r->ifm_active=media;r->ifm_status=IFM_AVALID;r->ifm_mask=0;r->ifm_count=1;
  if(requested<0)return EINVAL;
  if(requested>0&&r->ifm_ulist)return copyout(&media,(user_addr_t)r->ifm_ulist,sizeof(media));
  return 0;
 }
public:
 bool start(IOService*) override;
 void stop(IOService*) override;
 IOReturn newUserClient(task_t t,void*s,UInt32 n,OSDictionary*p,IOUserClient**o) override {return niwl_create_client(this,t,s,n,p,o);}
};
OSDefineMetaClassAndStructors(IO80211Controller,IOService)
bool IO80211Controller::start(IOService*p){
 net=nullptr;interface=nullptr;unsigned opt=0;PE_parse_boot_argn("nord2wifiios",&opt,sizeof opt);
 OSData*compatible=OSDynamicCast(OSData,p->getProperty("compatible"));
 static const char expected[]="nord2,mt6893-wifi";
 if(opt!=1||!compatible||compatible->getLength()!=sizeof expected||memcmp(compatible->getBytesNoCopy(),expected,sizeof expected))return false;
 nss_info info={};if(nord2_wifi_scan_info(&info)||!info.count||!IOService::start(p))return false;
 static const uint8_t broadcast[6]={255,255,255,255,255,255};
 ifnet_init_eparams params={};params.ver=IFNET_INIT_CURRENT_VERSION;params.len=sizeof params;params.flags=IFNET_INIT_LEGACY;params.name="en";params.family=IFNET_FAMILY_ETHERNET;params.subfamily=IFNET_SUBFAMILY_WIFI;params.type=IFT_ETHER;params.output=output;params.ioctl=ioctl;params.demux=demux;params.add_proto=add;params.del_proto=del;params.check_multi=multi;params.detach=detached;params.broadcast_addr=broadcast;params.broadcast_len=6;
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
 /* Never signal link up, carrier, association, supported encryption or IP readiness. */
 /* IOInterfaceName is only on actual interface: one bind target. */
 interface=new IO80211SkywalkInterface;
 if(!interface||!interface->init()){printf("NORD2 WIFI IOS REGISTRY: registered=0 failure=3\n");if(interface)interface->release();interface=nullptr;ifnet_detach(net);net=nullptr;IOService::stop(p);return false;}
 if(!interface->attach(this)){printf("NORD2 WIFI IOS REGISTRY: registered=0 failure=4\n");interface->release();interface=nullptr;ifnet_detach(net);net=nullptr;IOService::stop(p);return false;}
 if(!interface->start(this)){printf("NORD2 WIFI IOS REGISTRY: registered=0 failure=5\n");interface->detach(this);interface->release();interface=nullptr;ifnet_detach(net);net=nullptr;IOService::stop(p);return false;}
 if(!interface->setProperty("IOInterfaceName",name)||!interface->setProperty("BSD Name",name)||!interface->setProperty("IOInterfaceUnit",(uint64_t)ifnet_unit(net),32)||!interface->setProperty("IOInterfaceType",(uint64_t)IFT_ETHER,32)||!interface->setProperty("IO80211InterfaceRole","Infrastructure")||!interface->setProperty("IOInterfaceNamePrefix","en")||!interface->setProperty("IOBuiltin",true)||!interface->setProperty("IOPrimaryInterface",true)){
  printf("NORD2 WIFI IOS REGISTRY: registered=0 failure=7\n");interface->stop(this);interface->detach(this);interface->release();interface=nullptr;ifnet_detach(net);net=nullptr;IOService::stop(p);return false;
 }
 interface->registerService();registerService();printf("NORD2 WIFI IOS REGISTRY: registered=1 unit=%u media=%#x carrier=0 network_class=1 builtin=1\n",ifnet_unit(net),IFM_IEEE80211|IFM_AUTO);return true;
}
void IO80211Controller::stop(IOService*p){
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
