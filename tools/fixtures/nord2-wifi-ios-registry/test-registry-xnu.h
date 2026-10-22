#ifndef NIWI_REGISTRY_TEST
#define NIWI_REGISTRY_TEST
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <map>
#include <vector>
#include <string>
#include <cerrno>
#include <functional>
#include <algorithm>
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
struct ifnet;using ifnet_t=ifnet*;
struct mbuf{std::vector<uint8_t> storage=std::vector<uint8_t>(2048);size_t off=0,len=0,headerlen=0;unsigned flags=1;void*header=nullptr;mbuf *next=nullptr,*nextpkt=nullptr;ifnet_t rcvif=nullptr;};using mbuf_t=mbuf*;
static unsigned packetsFreed;static bool failPacket,failCopyData,failSetRcvif,failSetData,smallPacket;
constexpr unsigned MBUF_PKTHDR=1,MBUF_DONTWAIT=0,DLT_EN10MB=1,IFNAMSIZ=16,IF_MINMTU=72;
static void mbuf_freem(mbuf_t p){while(p){auto*n=p->next;packetsFreed++;delete p;p=n;}}
static void mbuf_freem_list(mbuf_t p){while(p){auto*n=p->nextpkt;mbuf_freem(p);p=n;}}
static mbuf_t mbuf_nextpkt(mbuf_t p){return p->nextpkt;}
static unsigned mbuf_flags(mbuf_t p){return p->flags;}
static size_t mbuf_pkthdr_len(mbuf_t p){return p->headerlen;}
static int mbuf_copydata(mbuf_t p,size_t off,size_t n,void*out){if(failCopyData)return EINVAL;auto*b=(uint8_t*)out;while(p&&n){if(off>=p->len){off-=p->len;p=p->next;continue;}size_t k=std::min(n,p->len-off);memcpy(b,p->storage.data()+p->off+off,k);b+=k;n-=k;off=0;p=p->next;}return n?EINVAL:0;}
static int mbuf_getpacket(unsigned,mbuf_t*out){if(failPacket){*out=nullptr;return ENOBUFS;}*out=new mbuf;if(smallPacket)(*out)->storage.resize(8);return 0;}
static size_t mbuf_maxlen(mbuf_t p){return p->storage.size();}
static void*mbuf_data(mbuf_t p){return p->storage.data()+p->off;}
static void mbuf_setlen(mbuf_t p,size_t n){p->len=n;}
static void mbuf_pkthdr_setlen(mbuf_t p,size_t n){p->headerlen=n;}
static int mbuf_pkthdr_setrcvif(mbuf_t p,ifnet_t net){if(failSetRcvif)return EINVAL;p->rcvif=net;return 0;}
static void mbuf_pkthdr_setheader(mbuf_t p,void*ptr){p->header=ptr;}
static int mbuf_setdata(mbuf_t p,void*ptr,size_t n){if(failSetData)return EINVAL;auto off=(uint8_t*)ptr-p->storage.data();assert(off>=0&&off+n<=p->storage.size());p->off=(size_t)off;p->len=n;return 0;}
struct ifnet_stat_increment_param{uint64_t packets_in=0,bytes_in=0;};
static unsigned sentCalls,txTaps,rxTaps,inputCalls,bpfAttached;static int sendResult,inputError,eventError;static uint64_t sentGeneration;static std::vector<uint8_t>sentFrame,inputFrame,tappedFrame;static std::function<void()> eventHook;
extern "C" int nord2_wifi_link_send(const uint8_t*frame,size_t n,uint64_t gen){sentCalls++;sentGeneration=gen;sentFrame.assign(frame,frame+n);return sendResult;}
static int bpf_tap_out(ifnet_t,unsigned,mbuf_t p,void*,size_t){txTaps++;tappedFrame.resize(p->headerlen);assert(!mbuf_copydata(p,0,p->headerlen,tappedFrame.data()));return 0;}
static int bpf_tap_in(ifnet_t,unsigned,mbuf_t p,void*h,size_t n){rxTaps++;tappedFrame.assign((uint8_t*)h,(uint8_t*)h+n);auto*b=(uint8_t*)mbuf_data(p);tappedFrame.insert(tappedFrame.end(),b,b+p->len);return 0;}
static int ifnet_stat_increment_out(ifnet_t,unsigned,size_t,unsigned){return 0;}
static int ifnet_input(ifnet_t net,mbuf_t p,ifnet_stat_increment_param*stats){inputCalls++;assert(p->rcvif==net&&p->header==p->storage.data()&&p->off==14&&p->headerlen==p->len);assert(stats->packets_in==1&&stats->bytes_in==p->len+14);inputFrame.assign((uint8_t*)p->header,(uint8_t*)p->header+14+p->len);mbuf_freem(p);return inputError;}
static void bpfattach(ifnet_t,unsigned type,unsigned hlen){assert(type==DLT_EN10MB&&hlen==14);bpfAttached++;}
struct kern_event_msg{uint32_t total_size,vendor_code,kev_class,kev_subclass,id,event_code,event_data[1];};
#define KEV_MSG_HEADER_SIZE offsetof(kern_event_msg,event_data[0])
static_assert(KEV_MSG_HEADER_SIZE==24&&sizeof(kern_event_msg)==28,"selected XNU event ABI");
constexpr unsigned KEV_VENDOR_APPLE=1,KEV_NETWORK_CLASS=1,KEV_DL_SUBCLASS=2,KEV_DL_LINK_ON=13,KEV_DL_LINK_OFF=12;
struct TestLinkEvent{kern_event_msg header;uint32_t family,unit;char name[IFNAMSIZ];};static std::vector<TestLinkEvent>linkEvents;
static int ifnet_event(ifnet_t,kern_event_msg*event){
 /* Match dlil.c's posted payload span; do not reinterpret the producer's
  * enclosing allocation, which previously concealed a one-word offset bug. */
 assert(event->total_size==KEV_MSG_HEADER_SIZE+8+IFNAMSIZ);
 TestLinkEvent captured={};memcpy(&captured.header,event,KEV_MSG_HEADER_SIZE);
 memcpy(&captured.family,event->event_data,event->total_size-KEV_MSG_HEADER_SIZE);
 linkEvents.push_back(captured);if(eventHook)eventHook();return eventError;
}

struct sockaddr{};struct sockaddr_dl{uint8_t sdl_len,sdl_family,sdl_index,sdl_type,sdl_nlen,sdl_alen;char sdl_data[24];};struct ifnet_demux_desc{};
struct ifnet_init_eparams{uint32_t ver,len,flags,unit,family,subfamily,type;const char*name;errno_t(*output)(ifnet_t,mbuf_t);errno_t(*ioctl)(ifnet_t,u_long,void*);errno_t(*demux)(ifnet_t,mbuf_t,char*,protocol_family_t*);errno_t(*add_proto)(ifnet_t,protocol_family_t,const ifnet_demux_desc*,uint32_t);errno_t(*del_proto)(ifnet_t,protocol_family_t);errno_t(*check_multi)(ifnet_t,const sockaddr*);errno_t(*framer_extended)(ifnet_t,mbuf_t*,const sockaddr*,const char*,const char*,uint32_t*,uint32_t*);void(*detach)(ifnet_t);const void*broadcast_addr;unsigned broadcast_len;};
struct ifnet{int refs=1;bool attached=false;ifnet_init_eparams params;unsigned addrlen=0,hdrlen=0,flags=0,mtu=0;uint8_t mac[6];};static ifnet_t lastNet;static unsigned allocated,released,netDetached,attachFailures;static bool failLL;
constexpr unsigned IFNET_INIT_CURRENT_VERSION=1,IFNET_INIT_LEGACY=1,IFNET_FAMILY_ETHERNET=2,IFNET_SUBFAMILY_WIFI=3,IFT_ETHER=6,AF_LINK=18,IFF_BROADCAST=2,IFF_MULTICAST=0x8000,IFF_SIMPLEX=0x800,IFF_RUNNING=0x40,IFF_UP=1;
constexpr unsigned IFM_IEEE80211=0x80,IFM_AUTO=0,IFM_AVALID=1,IFM_ACTIVE=2;
constexpr u_long SIOCGIFMEDIA=0xc02c6938,SIOCSIFFLAGS=0x80206910;

#pragma pack(push,4)
struct ifmediareq{char ifm_name[16];int ifm_current,ifm_mask,ifm_status,ifm_active,ifm_count;int*ifm_ulist;};
#pragma pack(pop)
static bool failCopyout;static unsigned copied;
static int copyout(const void*p,user_addr_t a,size_t n){if(failCopyout)return EFAULT;assert(a&&n==4);memcpy((void*)a,p,n);copied++;return 0;}
static int ifnet_allocate_extended(const ifnet_init_eparams*p,ifnet_t*out){*out=new ifnet;(*out)->params=*p;lastNet=*out;allocated++;return 0;}static int ifnet_release(ifnet_t p){assert(p->refs);if(!--p->refs){released++;if(lastNet==p)lastNet=nullptr;delete p;}return 0;}static int ifnet_find_by_name(const char*name,ifnet_t*out){*out=nullptr;char actual[16];if(!lastNet||!lastNet->attached)return ENXIO;snprintf(actual,sizeof actual,"%s%u",lastNet->params.name,lastNet->params.unit);if(strcmp(actual,name))return ENXIO;lastNet->refs++;*out=lastNet;return 0;}static int ifnet_attach(ifnet_t p,const sockaddr_dl*a){assert(p->addrlen==6&&p->hdrlen==14&&a->sdl_alen==6);if(attachFailures){attachFailures--;return EEXIST;}p->attached=true;return 0;}static int ifnet_detach(ifnet_t p){netDetached++;p->attached=false;p->params.detach(p);return 0;}
static int ifnet_set_addrlen(ifnet_t p,unsigned n){p->addrlen=n;return 0;}static int ifnet_set_hdrlen(ifnet_t p,unsigned n){p->hdrlen=n;return 0;}static int ifnet_set_lladdr(ifnet_t p,const void*a,unsigned n){if(failLL)return EINVAL;assert(n==6);memcpy(p->mac,a,n);return 0;}static const char*ifnet_name(ifnet_t p){return p->params.name;}static unsigned ifnet_unit(ifnet_t p){return p->params.unit;}static int ifnet_set_mtu(ifnet_t p,unsigned n){p->mtu=n;return 0;}static int ifnet_set_flags(ifnet_t p,unsigned v,unsigned mask){p->flags=(p->flags&~mask)|(v&mask);return 0;}
static int ifnet_reference(ifnet_t p){if(!p||!p->attached)return ENXIO;p->refs++;return 0;}
static unsigned ifnet_family(ifnet_t p){return p->params.family;}
static unsigned ifnet_mtu(ifnet_t p){return p->mtu;}
static errno_t ether_demux(ifnet_t,mbuf_t,char*,protocol_family_t*){return 0;}
static errno_t ether_add_proto(ifnet_t,protocol_family_t,const ifnet_demux_desc*,uint32_t){return 0;}
static errno_t ether_del_proto(ifnet_t,protocol_family_t){return 0;}
static errno_t ether_check_multi(ifnet_t,const sockaddr*){return 0;}
static errno_t ether_frameout_extended(ifnet_t,mbuf_t*,const sockaddr*,const char*,const char*,uint32_t*,uint32_t*){return 0;}
constexpr u_long SIOCSIFADDR=0x123401,SIOCADDMULTI=0x123402,SIOCDELMULTI=0x123403,SIOCSIFMTU=0x123404,SIOCGIFDEVMTU=0x123405;
struct ifreq{int ifr_mtu;struct{unsigned ifdm_current,ifdm_min,ifdm_max;}ifr_devmtu;};
#endif
