#ifndef NIWI_REGISTRY_TEST
#define NIWI_REGISTRY_TEST
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <map>
#include <vector>
#include <string>
#include <cerrno>
using UInt32=uint32_t;using IOReturn=int;struct task {int refs=1;bool admin=true;};using task_t=task*;using user_addr_t=uintptr_t;using errno_t=int;using protocol_family_t=unsigned;using u_int32_t=uint32_t;using u_long=unsigned long;
constexpr int kIOReturnNotReady=-1,kIOReturnSuccess=0,kIOReturnNotPrivileged=-2,kIOReturnBadArgument=-3,kIOReturnUnsupported=-4,kIOReturnNoDevice=-5,kIOReturnNoSpace=-6,kIOReturnNotOpen=-7,kIOReturnNoMemory=-8,kIOReturnNoResources=-9;
#define TASK_NULL nullptr
static task testTask;static task_t testCurrent=&testTask;static task_t current_task(){return testCurrent;}static void task_reference(task_t t){t->refs++;}static void task_deallocate(task_t t){assert(t->refs>1);t->refs--;}
static const char*kIOClientPrivilegeAdministrator="admin";
#include <mutex>
static unsigned locks,lockCalls,failLockAt;
struct IOLock{std::mutex m;};static IOLock*IOLockAlloc(){if(++lockCalls==failLockAt)return nullptr;locks++;return new IOLock;}static void IOLockFree(IOLock*p){locks--;delete p;}static void IOLockLock(IOLock*p){p->m.lock();}static void IOLockUnlock(IOLock*p){p->m.unlock();}
class OSObject{int refs=1;public:virtual~OSObject(){}virtual void free(){}void retain(){refs++;}void release(){assert(refs);if(!--refs){free();delete this;}}virtual bool init(){return true;}};
class OSNumber:public OSObject{public:uint64_t value;explicit OSNumber(uint64_t v):value(v){}uint64_t unsigned64BitValue(){return value;}};
class OSString:public OSObject{public:std::string value;OSString(const char*v):value(v){}static OSString*withCString(const char*s){return new OSString(s);}};
class OSData:public OSObject{public:std::vector<uint8_t>bytes;OSData(const void*p,size_t n):bytes((const uint8_t*)p,(const uint8_t*)p+n){}size_t getLength(){return bytes.size();}const void*getBytesNoCopy(){return bytes.data();}};
class OSBoolean:public OSObject{public:bool value;explicit OSBoolean(bool v):value(v){}};
class OSDictionary:public OSObject{public:std::map<std::string,OSObject*>values;OSObject*getObject(const char*k){auto i=values.find(k);return i==values.end()?nullptr:i->second;}void removeObject(const char*k){auto i=values.find(k);if(i!=values.end()){i->second->release();values.erase(i);}}static OSDictionary*withCapacity(unsigned){return new OSDictionary;}bool setObject(const char*k,OSObject*v){v->retain();if(values[k])values[k]->release();values[k]=v;return true;}~OSDictionary(){for(auto&v:values)v.second->release();}};
class OSArray:public OSObject{public:std::vector<OSObject*>values;static OSArray*withCapacity(unsigned){return new OSArray;}void setObject(OSObject*v){v->retain();values.push_back(v);}~OSArray(){for(auto*v:values)v->release();}};
#define OSDynamicCast(t,p) dynamic_cast<t*>(p)
#define OSDeclareDefaultStructors(t) public:t()=default;const char*className()const override{return #t;}
#define OSDefineMetaClassAndStructors(t,b)
class IOUserClient;
static unsigned starts,stops,attaches,detaches,failStart,failAttach;static std::string failProperty;static std::vector<class IOService*>registered;
class IOService:public OSObject{bool inactive=false;public:OSDictionary properties;IOService*opened=nullptr;static IOService*getResourceService(){static IOService r;return &r;}uint64_t getRegistryEntryID(){return (uintptr_t)this;}void setName(const char*){}bool open(IOService*p){if(opened)return false;opened=p;return true;}void close(IOService*p){assert(opened==p);opened=nullptr;}void removeProperty(const char*k){properties.removeObject(k);}virtual IOReturn setProperties(OSObject*){return kIOReturnUnsupported;}virtual const char*className()const{return "IOService";}virtual bool start(IOService*){return ++starts!=failStart;}virtual void stop(IOService*){stops++;}bool attach(IOService*){return ++attaches!=failAttach;}void detach(IOService*){detaches++;}bool isInactive(){return inactive;}void terminate(){inactive=true;}void registerService(){registered.push_back(this);}OSObject*getProperty(const char*k){return properties.values.count(k)?properties.values[k]:nullptr;}bool setProperty(const char*k,const char*v){if(failProperty==k)return false;auto*s=new OSString(v);properties.setObject(k,s);s->release();return true;}bool setProperty(const char*k,uint64_t n,unsigned){if(failProperty==k)return false;auto*v=new OSNumber(n);properties.setObject(k,v);v->release();return true;}bool setProperty(const char*k,bool v){if(failProperty==k)return false;auto*b=new OSBoolean(v);properties.setObject(k,b);b->release();return true;}bool setProperty(const char*k,const void*v,unsigned n){if(failProperty==k)return false;auto*d=new OSData(v,n);properties.setObject(k,d);d->release();return true;}virtual IOReturn newUserClient(task_t,void*,UInt32,OSDictionary*,IOUserClient**){return -1;}};
class IOUserClient:public IOService{public:static IOReturn clientHasPrivilege(void*p,const char*){auto*t=(task_t)p;return t&&t->admin?0:kIOReturnNotPrivileged;}virtual bool initWithTask(task_t,void*,UInt32,OSDictionary*){return true;}virtual IOReturn clientClose(){return 0;}virtual IOReturn clientDied(){return clientClose();}};
class IOCatalogue{public:unsigned drivers=0;bool addDrivers(OSArray*a,bool){assert(a->values.size()==1);drivers++;return true;}};static IOCatalogue catalogue;static IOCatalogue*gIOCatalogue=&catalogue;
static unsigned bootOpt=1;static void PE_parse_boot_argn(const char*,unsigned*v,size_t){*v=bootOpt;}
struct mbuf{};using mbuf_t=mbuf*;static unsigned packetsFreed;static void mbuf_freem(mbuf_t p){packetsFreed++;delete p;}
struct sockaddr{};struct sockaddr_dl{uint8_t sdl_len,sdl_family,sdl_index,sdl_type,sdl_nlen,sdl_alen;char sdl_data[24];};struct ifnet_demux_desc{};
struct ifnet;using ifnet_t=ifnet*;
struct ifnet_init_eparams{uint32_t ver,len,flags,unit,family,subfamily,type;const char*name;errno_t(*output)(ifnet_t,mbuf_t);errno_t(*ioctl)(ifnet_t,u_long,void*);errno_t(*demux)(ifnet_t,mbuf_t,char*,protocol_family_t*);errno_t(*add_proto)(ifnet_t,protocol_family_t,const ifnet_demux_desc*,uint32_t);errno_t(*del_proto)(ifnet_t,protocol_family_t);errno_t(*check_multi)(ifnet_t,const sockaddr*);void(*detach)(ifnet_t);const void*broadcast_addr;unsigned broadcast_len;};
struct ifnet{int refs=1;bool attached=false;ifnet_init_eparams params;unsigned addrlen=0,hdrlen=0,flags=0;uint8_t mac[6];};static ifnet_t lastNet;static unsigned allocated,released,netDetached,attachFailures;static bool failLL;
constexpr unsigned IFNET_INIT_CURRENT_VERSION=1,IFNET_INIT_LEGACY=1,IFNET_FAMILY_ETHERNET=2,IFNET_SUBFAMILY_WIFI=3,IFT_ETHER=6,AF_LINK=18,IFF_BROADCAST=2,IFF_MULTICAST=0x8000,IFF_SIMPLEX=0x800,IFF_RUNNING=0x40,IFF_UP=1;
constexpr unsigned IFM_IEEE80211=0x80,IFM_AUTO=0,IFM_AVALID=1,IFM_ACTIVE=2;
constexpr u_long SIOCGIFMEDIA=0xc02c6938,SIOCSIFFLAGS=0x80206910;

#pragma pack(push,4)
struct ifmediareq{char ifm_name[16];int ifm_current,ifm_mask,ifm_status,ifm_active,ifm_count;int*ifm_ulist;};
#pragma pack(pop)
static bool failCopyout;static unsigned copied;
static int copyout(const void*p,user_addr_t a,size_t n){if(failCopyout)return EFAULT;assert(a&&n==4);memcpy((void*)a,p,n);copied++;return 0;}
static int ifnet_allocate_extended(const ifnet_init_eparams*p,ifnet_t*out){*out=new ifnet;(*out)->params=*p;lastNet=*out;allocated++;return 0;}static int ifnet_release(ifnet_t p){assert(p->refs);if(!--p->refs){released++;if(lastNet==p)lastNet=nullptr;delete p;}return 0;}static int ifnet_find_by_name(const char*name,ifnet_t*out){*out=nullptr;char actual[16];if(!lastNet||!lastNet->attached)return ENXIO;snprintf(actual,sizeof actual,"%s%u",lastNet->params.name,lastNet->params.unit);if(strcmp(actual,name))return ENXIO;lastNet->refs++;*out=lastNet;return 0;}static int ifnet_attach(ifnet_t p,const sockaddr_dl*a){assert(p->addrlen==6&&p->hdrlen==14&&a->sdl_alen==6);if(attachFailures){attachFailures--;return EEXIST;}p->attached=true;return 0;}static int ifnet_detach(ifnet_t p){netDetached++;p->attached=false;p->params.detach(p);return 0;}
static int ifnet_set_addrlen(ifnet_t p,unsigned n){p->addrlen=n;return 0;}static int ifnet_set_hdrlen(ifnet_t p,unsigned n){p->hdrlen=n;return 0;}static int ifnet_set_lladdr(ifnet_t p,const void*a,unsigned n){if(failLL)return EINVAL;assert(n==6);memcpy(p->mac,a,n);return 0;}static const char*ifnet_name(ifnet_t p){return p->params.name;}static unsigned ifnet_unit(ifnet_t p){return p->params.unit;}static int ifnet_set_mtu(ifnet_t,unsigned n){assert(n==1500);return 0;}static int ifnet_set_flags(ifnet_t p,unsigned v,unsigned){p->flags=v;return 0;}
#endif
