/* Native owned IOUC: scan and bounded WPA2 join bridge. Native station owner
 * establishes association, keys, data admission and real ifnet carrier; client
 * never infers success from enqueue or a cached beacon. */
#ifdef NIWL_HOST_TEST
#include "test-xnu.h"
#else
#include <IOKit/IOUserClient.h>
#include <IOKit/IOBufferMemoryDescriptor.h>
#include <IOKit/IOLib.h>
#include <libkern/c++/OSBoolean.h>
#include <kern/task.h>
#include <kern/clock.h>
#include <libkern/c++/OSString.h>
#endif
extern "C" {
#include "../../../bsd/dev/wifi_ios_binding.h"
#include "../../../bsd/dev/wifi_ios_control.h"
#include "../../../bsd/dev/wifi_ios_event.h"
#include "../../../bsd/dev/wifi_ios_lifecycle.h"
#include "../../../bsd/dev/wifi_scan_service.h"
#include "../../../bsd/dev/wifi_ios_card.h"
#include "../../../bsd/dev/wifi_ios_policy.h"
#include "../../../bsd/dev/wifi_ios_scalar.h"
#include "../../../bsd/dev/wifi_ios_join.h"
#include "../../../bsd/dev/wifi_ios_station_view.h"
}
static uint32_t niw_u32(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
/* Monotonic time is diagnostic only, never scan freshness or a timeout. */
static unsigned long long niw_ms(void){
#ifdef NIWL_HOST_TEST
 return 0;
#else
 uint64_t now=0,ns=0;clock_get_uptime(&now);absolutetime_to_nanoseconds(now,&ns);return ns/1000000u;
#endif
}
static int niw_root_busy(void){
#ifdef NIWL_HOST_TEST
 return -1; /* No real registry exists in the host harness. */
#else
 IOService*root=IOService::getServiceRoot();return root?(int)root->getBusyState():-1;
#endif
}
/* Same-IPSW RawSet preserves IOReturn in errno; ScanAsync returns that errno.
 * wifid retries raw EBUSY16, not Mach kIOReturnBusy (0xe00002d5). */
static const IOReturn kNiwScanBusy=16;
static void niwj_free(void*p,size_t n){if(p){niwj_clear(p,n);IOFree(p,n);}}
class NiwlLifecycleClient : public IOUserClient {
 OSDeclareDefaultStructors(NiwlLifecycleClient)
 task_t owner;IOLock *gate;
 enum Phase {Closed,Open,Allocated,Armed};Phase phase;
 IOBufferMemoryDescriptor *data,*state;
 OSAsyncReference64 reference;bool ownsReference;
 uint64_t generation,scanGeneration,pendingTicket,pendingEpoch;unsigned unsupportedLogs,pipeLogs,scanLogs,queueLogs,completionLogs,eventLogs,resultLogs,outputLogs;
 struct {uint32_t request,operation,count;} controlTrace[40];
 char interfaceName[16];struct nils_state cacheLife;struct nils_ticket cacheTicket;
 struct njs_snapshot *station;uint64_t joinTicket,joinEpoch;unsigned joinLogs;
 struct niwa_snapshot *snapshot;uint32_t producer;IOReturn lastScanError;
 static void completed(void*,uint64_t,uint64_t,enum nss_status);
 static void joinCompleted(void*,uint64_t,uint64_t,enum njs_notice);
 IOReturn joinLocked(const uint8_t*,size_t);
 bool stationEventLocked(unsigned);
 IOReturn scanLocked(const uint8_t*,size_t);
 IOReturn getLocked(const niw_request*,const void*,size_t,IOExternalMethodArguments*);
 bool eventLocked(uint32_t);

 void discardLocked(void);
 void closeOwned(void);
 IOReturn dispatchLocked(uint32_t,IOExternalMethodArguments *,bool *);
 bool traceControl(uint32_t,uint32_t);
 IOReturn outputLocked(uint32_t,IOExternalMethodArguments *,bool *);
public:
 void bindInterface(const char*);
 bool initWithTask(task_t,void *,UInt32,OSDictionary *) override;
 IOReturn externalMethod(uint32_t,IOExternalMethodArguments *,IOExternalMethodDispatch *,OSObject *,void *) override;
 IOReturn clientMemoryForType(UInt32,IOOptionBits *,IOMemoryDescriptor **) override;
 IOReturn clientClose(void) override;
 IOReturn clientDied(void) override;
 void stop(IOService *) override;
 void free(void) override;
};
OSDefineMetaClassAndStructors(NiwlLifecycleClient,IOUserClient)
bool NiwlLifecycleClient::initWithTask(task_t task,void *security,UInt32 type,OSDictionary *props){
 station=nullptr;joinTicket=joinEpoch=0;joinLogs=0;pipeLogs=scanLogs=queueLogs=completionLogs=eventLogs=resultLogs=outputLogs=unsupportedLogs=0;bzero(controlTrace,sizeof controlTrace);snapshot=nullptr;pendingTicket=0;pendingEpoch=0;scanGeneration=0;producer=0;lastScanError=kIOReturnNotReady;bzero(&cacheLife,sizeof cacheLife);bzero(interfaceName,sizeof interfaceName);
 owner=TASK_NULL;gate=nullptr;phase=Closed;data=state=nullptr;ownsReference=false;generation=0;bzero(reference,sizeof reference);
 if(!task||current_task()!=task||type)return false;
 OSObject *entitlement=IOUserClient::copyClientEntitlement(task,"com.apple.wifi.manager-access");
 OSBoolean *allowed=OSDynamicCast(OSBoolean,entitlement);bool authorized=allowed&&allowed->isTrue();
 if(entitlement)entitlement->release();
 if(!authorized||!IOUserClient::initWithTask(task,security,type,props))return false;
 gate=IOLockAlloc();if(!gate)return false;
 snapshot=(niwa_snapshot*)IOMalloc(sizeof(*snapshot));if(!snapshot)return false;bzero(snapshot,sizeof(*snapshot));
 station=(njs_snapshot*)IOMalloc(sizeof(*station));if(!station)return false;bzero(station,sizeof(*station));
 task_reference(task);owner=task;generation=1;phase=Open;return true;
}
void NiwlLifecycleClient::discardLocked(void){
 /* All cache/event publication and reference sending is under gate. Close
  * invalidates scan generation first; pending callback retains this client and
  * stable snapshot until retirement. Mappings own descriptor refs independently. */
 nils_close(&cacheLife);
 if(ownsReference){IOUserClient::releaseAsyncReference64(reference);ownsReference=false;bzero(reference,sizeof reference);}
 if(data){data->release();data=nullptr;}if(state){state->release();state=nullptr;}
}
void NiwlLifecycleClient::closeOwned(void){
 if(!gate)return;IOLockLock(gate);phase=Closed;if(generation!=UINT64_MAX)generation++;uint64_t ticket=pendingTicket,jt=joinTicket;discardLocked();IOLockUnlock(gate);if(ticket)nord2_wifi_scan_cancel(ticket);if(jt)nord2_wifi_join_cancel(jt);
}
bool NiwlLifecycleClient::traceControl(uint32_t request,uint32_t operation){
 for(unsigned i=0;i<40;i++)if(controlTrace[i].count&&controlTrace[i].request==request&&controlTrace[i].operation==operation){if(controlTrace[i].count<8){controlTrace[i].count++;return true;}return false;}
 for(unsigned i=0;i<40;i++)if(!controlTrace[i].count){controlTrace[i].request=request;controlTrace[i].operation=operation;controlTrace[i].count=1;return true;}return false;
}
IOReturn NiwlLifecycleClient::dispatchLocked(uint32_t selector,IOExternalMethodArguments*a,bool*adopted){
 if(phase==Closed)return kIOReturnNotOpen;
 if(current_task()!=owner)return kIOReturnNotPrivileged;
 if(a&&a->structureOutputSize&&!a->structureOutput)return kIOReturnBadArgument;
 if(!a||a->version!=kIOExternalMethodArgumentsCurrentVersion||a->selector!=selector||a->scalarInputCount||a->scalarOutputCount||a->structureOutputDescriptor)return kIOReturnBadArgument;
 if(selector==0||selector==6){
  if(a->asyncReferenceCount||a->asyncWakePort)return kIOReturnBadArgument;
  size_t bytes=selector==0?NIW_SMALL_BYTES:NIW_LARGE_BYTES;
  const void*carrier=a->structureInput;void*copy=nullptr;
  if(a->structureInputDescriptor){
   if(a->structureInputSize||a->structureInputDescriptor->getLength()!=bytes)return kIOReturnBadArgument;
   copy=IOMalloc(bytes);if(!copy)return kIOReturnNoMemory;
   IOReturn prepared=a->structureInputDescriptor->prepare(kIODirectionOut);
   if(prepared!=kIOReturnSuccess){niwj_free(copy,bytes);return prepared;}
   IOByteCount copied=a->structureInputDescriptor->readBytes(0,copy,bytes);
   IOReturn completed=a->structureInputDescriptor->complete(kIODirectionOut);
   if(copied!=bytes||completed!=kIOReturnSuccess){niwj_free(copy,bytes);return completed!=kIOReturnSuccess?completed:kIOReturnBadArgument;}
   carrier=copy;
  }else if(a->structureInputSize!=bytes)return kIOReturnBadArgument;
  struct niw_request q={};IOReturn result=kIOReturnBadArgument;
  if(niwi_scan_envelope(selector,carrier,bytes,interfaceName)==0){if(a->structureOutputSize){if(copy)niwj_free(copy,bytes);return kIOReturnBadArgument;}result=scanLocked((const uint8_t*)carrier+8,5456);if(copy)niwj_free(copy,bytes);return result;}

  uint32_t unsupportedSet=0;
  if(!niwi_set_shape(selector,carrier,bytes,interfaceName,&unsupportedSet)){
   const uint8_t*power=(const uint8_t*)carrier+8;size_t tail=selector==0?15368:245768;
   if(unsupportedSet==20||unsupportedSet==22){
    uint32_t extent=niw_u32((const uint8_t*)carrier+4),value=niw_u32((const uint8_t*)carrier+tail+20);
    if(a->structureOutputSize||value)result=kIOReturnBadArgument;
    else if(unsupportedSet==20)result=joinLocked((const uint8_t*)carrier+8,extent);
    else if(extent)result=kIOReturnUnsupported;
    else{int x=nord2_wifi_join_disconnect();result=!x?kIOReturnSuccess:x==2?kNiwScanBusy:kIOReturnNotReady;}
    if(traceControl(unsupportedSet,0x802869c8))printf("NORD2 WIFI IOS JOINCONTROL: request=%u bytes=%u status=%#x t_ms=%llu\n",unsupportedSet,extent,(unsigned)result,niw_ms());
    if(copy)niwj_free(copy,bytes);return result;
   }
   if(unsupportedSet==19&&niw_u32((const uint8_t*)carrier+4)==24&&traceControl(19,0))printf("NORD2 WIFI IOS POWERSET: version=%u radios=%u state=%u\n",niw_u32(power),niw_u32(power+4),niw_u32(power+8));
   if(unsupportedSet==19&&!a->structureOutputSize&&niw_u32((const uint8_t*)carrier+4)==24&&!niw_u32((const uint8_t*)carrier+tail+20)&&!niwi_power_on_admit(power,24)){
    nss_info info={};result=nord2_wifi_scan_info(&info)?kIOReturnNotReady:kIOReturnSuccess;
    if(traceControl(19,0x802869c8))printf("NORD2 WIFI IOS CONTROL: selector=%u request=19 operation=0x802869c8 bytes=24 status=%#x\n",selector,(unsigned)result);
    if(copy)niwj_free(copy,bytes);return result; /* already physically retained ON; no event/write */
   }
   if(traceControl(unsupportedSet,0x802869c8))printf("NORD2 WIFI IOS CONTROL: selector=%u request=%u operation=0x802869c8 unsupported=1\n",selector,unsupportedSet);
   if(copy)niwj_free(copy,bytes);return kIOReturnUnsupported;
  }
  if(!niw_decode(selector,carrier,bytes,&q)){
   if(selector==0&&q.request==43&&q.value==0&&q.payload_bytes==0&&!memcmp(q.interface_name,interfaceName,16)){
    static const char identity[]="Nord2 MT6893 passive scan controller";size_t used=0;
    if(!niwb_driver_version(carrier,bytes,identity,sizeof identity,a->structureOutput,a->structureOutputSize,&used)){
     a->structureOutputSize=(uint32_t)used;result=kIOReturnSuccess;
    }else result=kIOReturnBadArgument;
   }else result=getLocked(&q,carrier,bytes,a);
  }
  if(traceControl(q.request,q.operation))printf("NORD2 WIFI IOS CONTROL: selector=%u request=%u operation=%#x bytes=%u status=%#x\n",selector,q.request,q.operation,q.payload_bytes,(unsigned)result);
  if(copy)niwj_free(copy,bytes);
  /* Unimplemented request IDs are explicitly unsupported; no security or link
   * state is inferred from successful scan operations. */
  return result;
 }
 if(a->structureInputDescriptor||a->structureInputSize||a->scalarInputCount||a->scalarOutputCount)return kIOReturnBadArgument;
 if(selector==1||selector==5){
  if(a->asyncReferenceCount||a->asyncWakePort)return kIOReturnBadArgument;
  struct niwb_dispatch_view view={NIWB_OPEN,NIWB_CONTROL_ALL,NIWB_PIPE_ALL};size_t used=0;
  if(niwb_selector(&view,selector,0,a->structureOutputSize,0,0,0,(uint8_t*)a->structureOutput,&used))return kIOReturnBadArgument;
  a->structureOutputSize=(uint32_t)used;return kIOReturnSuccess; /* implemented control/event transport shape */
 }
 if(a->structureOutputSize)return kIOReturnBadArgument;
 if(selector==2){
  if(phase!=Allocated)return kIOReturnNotReady;
  if(a->asyncReferenceCount!=8||!a->asyncReference||!a->asyncWakePort)return kIOReturnBadArgument;
  for(unsigned i=0;i<8;i++)reference[i]=a->asyncReference[i];ownsReference=true;*adopted=true;phase=Armed;return kIOReturnSuccess;
 }
 if(a->asyncReferenceCount||a->asyncWakePort)return kIOReturnBadArgument;
 if(selector==3){
  if(phase!=Open||generation==UINT64_MAX)return kIOReturnBusy;
  struct niwe_view empty={0,0,4096};struct niwe_space space;
  if(niwe_plan(&empty,4,0,&space)!=1)return kIOReturnBadArgument; /* bounds only, no event publication */
  IOBufferMemoryDescriptor*d=IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task,kIOMemoryKernelUserShared,4096,4096);
  if(!d)return kIOReturnNoMemory;
  IOBufferMemoryDescriptor*s=IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task,kIOMemoryKernelUserShared,4096,4096);
  if(!s){d->release();return kIOReturnNoMemory;}
  bzero(d->getBytesNoCopy(),4096);bzero(s->getBytesNoCopy(),4096);producer=0;data=d;state=s;phase=Allocated;generation++;return kIOReturnSuccess;
 }
 if(selector==4){if(scanGeneration!=UINT64_MAX)scanGeneration++;discardLocked();phase=Open;return kIOReturnSuccess;}
 return kIOReturnUnsupported;
}
IOReturn NiwlLifecycleClient::outputLocked(uint32_t selector,IOExternalMethodArguments*a,bool*adopted){
 if(!a||!a->structureOutputDescriptor)return dispatchLocked(selector,a,adopted);
 if(phase==Closed)return kIOReturnNotOpen;
 if(current_task()!=owner)return kIOReturnNotPrivileged;
 IOMemoryDescriptor*md=a->structureOutputDescriptor;size_t capacity=a->structureOutputDescriptorSize;
 if(a->version!=kIOExternalMethodArgumentsCurrentVersion||a->selector!=selector||a->scalarInputCount||a->scalarOutputCount||a->asyncReferenceCount||a->asyncWakePort)return kIOReturnBadArgument;
 if((selector!=0&&selector!=6)||a->structureOutputSize||!capacity||capacity>NIW_LARGE_PAYLOAD||md->getLength()!=capacity)return kIOReturnBadArgument;
 void*buffer=IOMalloc(capacity);if(!buffer)return kIOReturnNoMemory;bzero(buffer,capacity);
 IOReturn prepared=md->prepare(kIODirectionIn);if(prepared!=kIOReturnSuccess){IOFree(buffer,capacity);return prepared;}
 IOExternalMethodArguments copy=*a;copy.structureOutputDescriptor=nullptr;copy.structureOutputDescriptorSize=0;copy.structureOutput=buffer;copy.structureOutputSize=capacity;
 unsigned savedNext=snapshot->cursor.next;
 IOReturn result=dispatchLocked(selector,&copy,adopted);
 if(result==kIOReturnSuccess&&(copy.structureOutputSize>capacity||md->writeBytes(0,buffer,copy.structureOutputSize)!=copy.structureOutputSize))result=kIOReturnBadArgument;
 IOReturn completed=md->complete(kIODirectionIn);if(result==kIOReturnSuccess&&completed!=kIOReturnSuccess)result=completed;
 if(result!=kIOReturnSuccess)snapshot->cursor.next=savedNext;
 if(outputLogs++<64)printf("NORD2 WIFI IOS OUTPUT: status=%#x requested=%u used=%u cursor_rollback=%u\n",(unsigned)result,(unsigned)capacity,(unsigned)copy.structureOutputSize,(unsigned)(result!=kIOReturnSuccess));
 if(result==kIOReturnSuccess)a->structureOutputDescriptorSize=copy.structureOutputSize;
 IOFree(buffer,capacity);return result;
}
IOReturn NiwlLifecycleClient::externalMethod(uint32_t selector,IOExternalMethodArguments*a,IOExternalMethodDispatch*d,OSObject*t,void*r){
 /* XNU passes an owned wake-port send reference on async calls. Balance every
  * rejected/replayed call too, not only the accepted stored registration. */
 OSAsyncReference64 incoming={0};bool incomingRight=a&&a->asyncReference&&a->asyncReferenceCount;
 if(incomingRight)incoming[0]=a->asyncReference[0];bool adopted=false;uint64_t cancel=0,cancelJoin=0;IOReturn result=kIOReturnNotReady;
 if(gate){IOLockLock(gate);if(selector==4){cancel=pendingTicket;cancelJoin=joinTicket;}result=(d||t||r)?kIOReturnBadArgument:outputLocked(selector,a,&adopted);if(selector>=1&&selector<=5&&pipeLogs++<32)printf("NORD2 WIFI IOS PIPE: selector=%u phase=%u status=%#x output=%u refs=%u t_ms=%llu\n",selector,(unsigned)phase,(unsigned)result,a?(unsigned)a->structureOutputSize:0,a?a->asyncReferenceCount:0,niw_ms());IOLockUnlock(gate);}
 if(cancel&&result==kIOReturnSuccess)nord2_wifi_scan_cancel(cancel);
 if(cancelJoin&&result==kIOReturnSuccess)nord2_wifi_join_cancel(cancelJoin);
 if(incomingRight&&!adopted)IOUserClient::releaseAsyncReference64(incoming);return result;
}
IOReturn NiwlLifecycleClient::clientMemoryForType(UInt32 type,IOOptionBits*options,IOMemoryDescriptor**memory){
 if(!gate||!options||!memory)return kIOReturnBadArgument;IOLockLock(gate);
 IOReturn result=kIOReturnNotReady;
 if(phase==Closed)result=kIOReturnNotOpen;
 else if(current_task()!=owner)result=kIOReturnNotPrivileged;
 else if(type!=1&&type!=2)result=kIOReturnBadArgument;
 else if(phase==Allocated||phase==Armed){IOMemoryDescriptor*m=type==1?data:state;m->retain();*memory=m;*options=type==1?kIOMapReadOnly:0;result=kIOReturnSuccess;}
 if(pipeLogs++<32)printf("NORD2 WIFI IOS MAP: type=%u phase=%u status=%#x bytes=%u\n",type,(unsigned)phase,(unsigned)result,result==kIOReturnSuccess?(unsigned)(*memory)->getLength():0);
 IOLockUnlock(gate);return result;
}
IOReturn NiwlLifecycleClient::clientClose(void){closeOwned();terminate();return kIOReturnSuccess;}
IOReturn NiwlLifecycleClient::clientDied(void){closeOwned();terminate();return kIOReturnSuccess;}
void NiwlLifecycleClient::stop(IOService*p){closeOwned();IOUserClient::stop(p);}
void NiwlLifecycleClient::free(void){closeOwned();if(owner){task_deallocate(owner);owner=TASK_NULL;}if(gate){IOLockFree(gate);gate=nullptr;}if(snapshot){IOFree(snapshot,sizeof(*snapshot));snapshot=nullptr;}if(station){niwj_free(station,sizeof(*station));station=nullptr;}IOUserClient::free();}
#ifdef NIWL_HOST_TEST
class NiwlLifecycleService : public IOService {
 OSDeclareDefaultStructors(NiwlLifecycleService)
public:
 IOReturn newUserClient(task_t,void*,UInt32,OSDictionary*,IOUserClient**) override;
};
OSDefineMetaClassAndStructors(NiwlLifecycleService,IOService)
IOReturn NiwlLifecycleService::newUserClient(task_t task,void*security,UInt32 type,OSDictionary*props,IOUserClient**handler){
 if(!handler||isInactive())return kIOReturnNotReady;
 NiwlLifecycleClient*c=new NiwlLifecycleClient;
 if(!c)return kIOReturnNoMemory;
 if(!c->initWithTask(task,security,type,props)){c->release();return kIOReturnNotPrivileged;}
 c->bindInterface("en0");
 if(!c->attach(this)){c->release();return kIOReturnNotReady;}
 if(!c->start(this)){c->detach(this);c->release();return kIOReturnNotReady;}
 *handler=c;return kIOReturnSuccess;
}

#endif

void NiwlLifecycleClient::bindInterface(const char*n){strlcpy(interfaceName,n,sizeof interfaceName);}
extern "C" IOReturn niwl_create_client(IOService*provider,task_t task,void*security,UInt32 type,OSDictionary*props,IOUserClient**handler){
 if(!provider||!handler||provider->isInactive())return kIOReturnNotReady;
 OSString*name=OSDynamicCast(OSString,provider->getProperty("IOInterfaceName"));if(!name||name->getLength()>=16)return kIOReturnNotReady;
 NiwlLifecycleClient*c=new NiwlLifecycleClient;if(!c)return kIOReturnNoMemory;
 if(!c->initWithTask(task,security,type,props)){c->release();return kIOReturnNotPrivileged;}
 c->bindInterface(name->getCStringNoCopy());
 if(!c->attach(provider)){c->release();return kIOReturnNotReady;}
 if(!c->start(provider)){c->detach(provider);c->release();return kIOReturnNotReady;}
 *handler=c;printf("NORD2 WIFI IOS CLIENT: opened=1 type=0 root_busy=%d t_ms=%llu\n",niw_root_busy(),niw_ms());return kIOReturnSuccess;
}
IOReturn NiwlLifecycleClient::scanLocked(const uint8_t*p,size_t n){
 bool trace=scanLogs++<64;
 unsigned peer=0;if(n>=14){bool zero=true,broadcast=true;for(unsigned i=8;i<14;i++){zero=zero&&!p[i];broadcast=broadcast&&p[i]==255;}peer=zero?0:broadcast?1:2;}
 if(trace)printf("NORD2 WIFI IOS SCANREQ: bytes=%u phase=%u version=%u bss=%u scan_type=%u phy_mode=%u dwell=%u rest=%u channels=%u ssid_length=%u names=%u flags_lo=%u flags_hi=%u rssi_floor=%d short_ssid=%u peer=%u t_ms=%llu\n",(unsigned)n,(unsigned)phase,n>=4?niw_u32(p):0,n>=8?niw_u32(p+4):0,n>=56?niw_u32(p+52):0,n>=60?niw_u32(p+56):0,n>=62?(unsigned)(p[60]|(uint16_t)p[61]<<8):0,n>=68?niw_u32(p+64):0,n>=72?niw_u32(p+68):0,n>=20?niw_u32(p+16):0,n>=4888?niw_u32(p+4884):0,n>=4876?niw_u32(p+4872):0,n>=4880?niw_u32(p+4876):0,n>=4884?(int32_t)niw_u32(p+4880):0,n>=5452?(unsigned)(niw_u32(p+5448)!=0):0,peer,niw_ms());
 if(phase!=Armed||!ownsReference)return kIOReturnNotReady;
 if(scanGeneration==UINT64_MAX)return kIOReturnNoResources;
 if(pendingTicket)return kNiwScanBusy;
 nss_info info={};nss_request request={};if(nord2_wifi_scan_info(&info))return kIOReturnNotReady;
 int admission=niwi_scan_admit(p,n,&info,&request);
 if(trace){
  uint32_t listed=n>=72?niw_u32(p+68):0,flags=0,low=0,high=0;
  for(uint32_t j=0;j<listed&&j<400&&n>=84+12*(size_t)j;j++){
   uint32_t ch=niw_u32(p+76+12*j),f=niw_u32(p+80+12*j);flags|=f;
   if(!j||ch<low)low=ch;if(ch>high)high=ch;
   if(scanLogs<=4)printf("NORD2 WIFI IOS CHANNEL: index=%u version=%u number=%u flags=%#x\n",j,niw_u32(p+72+12*j),ch,f);
  }
  printf("NORD2 WIFI IOS ADMISSION: status=%d owner_channels=%u requested_channels=%u channel_flags_or=%#x channel_min=%u channel_max=%u t_ms=%llu\n",admission,(unsigned)info.count,admission?0:(unsigned)request.count,flags,low,high,niw_ms());
 }
 if(admission)return kIOReturnUnsupported;
 nils_close(&cacheLife);bzero(snapshot,sizeof(*snapshot));scanGeneration++;
 if(nils_open(&cacheLife,scanGeneration)||nils_begin(&cacheLife,&cacheTicket,snapshot))return kIOReturnNotReady;
 retain();uint64_t ticket=0;int result=nord2_wifi_scan_enqueue(&request,this,scanGeneration,completed,&ticket);
 if(result){nils_finish(&cacheLife,&cacheTicket,nullptr);release();return result==2?kNiwScanBusy:kIOReturnNotReady;}
 if(queueLogs++<64)printf("NORD2 WIFI IOS ENQUEUE: status=%d generation=%llu ticket=%llu t_ms=%llu\n",result,(unsigned long long)scanGeneration,(unsigned long long)ticket,niw_ms());
 pendingTicket=ticket;pendingEpoch=generation;lastScanError=kIOReturnNotReady;return kIOReturnSuccess;
}
void NiwlLifecycleClient::completed(void*cookie,uint64_t gen,uint64_t ticket,enum nss_status status){
 NiwlLifecycleClient*c=(NiwlLifecycleClient*)cookie;
 IOLockLock(c->gate);nss_view view={};bool current=c->phase==Armed&&c->scanGeneration==gen&&c->pendingTicket==ticket&&c->pendingEpoch==c->generation;
 if(current){
  int result=-1;
  if(status==NSS_COMPLETE&&!nord2_wifi_scan_view(ticket,&view)&&view.client_generation==gen){
   size_t count=0;const niwp_rate_map*map=niwi_rate_map(&count);
   result=nils_prepare(&c->cacheTicket,view.scan,view.captured_us,view.captured_us,map,count,c->snapshot);
  }
  if(result==0&&nils_finish(&c->cacheLife,&c->cacheTicket,c->snapshot)==0)c->lastScanError=kIOReturnSuccess;
  else{nils_finish(&c->cacheLife,&c->cacheTicket,nullptr);c->lastScanError=kIOReturnNotReady;}
  if(!c->eventLocked(c->lastScanError==kIOReturnSuccess?0:5)){nils_close(&c->cacheLife);c->lastScanError=kIOReturnNoResources;}
 }else if(c->cacheLife.inflight)nils_finish(&c->cacheLife,&c->cacheTicket,nullptr);
 if(c->completionLogs++<64)printf("NORD2 WIFI IOS COMPLETE: generation=%llu ticket=%llu backend=%u current=%u cache_status=%#x rows=%u omitted=%u omit_mask=%#x t_ms=%llu\n",(unsigned long long)gen,(unsigned long long)ticket,(unsigned)status,(unsigned)current,(unsigned)c->lastScanError,current?c->snapshot->count:0,current?c->snapshot->omitted:0,current?c->snapshot->omit_mask:0,niw_ms());
 if(c->pendingTicket==ticket)c->pendingTicket=0;
 IOLockUnlock(c->gate);
 nord2_wifi_scan_release(ticket); /* backend defers reuse until this callback returns */
 c->release();
}
bool NiwlLifecycleClient::eventLocked(uint32_t status){
 if(phase!=Armed||!ownsReference||!data||!state)return false;
 uint32_t*indices=(uint32_t*)state->getBytesNoCopy();uint32_t consumer=__atomic_load_n(indices,__ATOMIC_ACQUIRE);
 niwe_view view={consumer,producer,4096};niwe_space space={};if(niwe_plan(&view,4,0,&space)!=1)return false;
 uint8_t record[52];if(niwe_scan_done(record,sizeof record,(const uint8_t*)interfaceName,status))return false;
 uint8_t*ring=(uint8_t*)data->getBytesNoCopy();
 for(uint32_t i=0;i<space.padding;i+=4){uint32_t pad=80;memcpy(ring+producer+i,&pad,4);}
 for(unsigned i=0;i<52;i++)ring[(space.record_offset+i)%4096]=record[i];
 producer=space.next;__atomic_store_n(indices+1,producer,__ATOMIC_RELEASE);
 IOReturn result=sendAsyncResult64(reference,kIOReturnSuccess,nullptr,0);
 if(eventLogs++<128)printf("NORD2 WIFI IOS EVENT: event=10 published=1 notified=%u status=%#x producer=%u consumer=%u t_ms=%llu\n",(unsigned)(result==kIOReturnSuccess),(unsigned)result,producer,consumer,niw_ms());
 return true; /* immutable event is published; notify failure never retracts its real cache */
}
IOReturn NiwlLifecycleClient::getLocked(const niw_request*q,const void*carrier,size_t bytes,IOExternalMethodArguments*a){
 if(memcmp(q->interface_name,interfaceName,16)||q->value)return kIOReturnBadArgument;
 if(q->request==27||q->request==254){nss_info info={};size_t used=0;if(nord2_wifi_scan_info(&info))return kIOReturnNotReady;if(niws_channels(q,interfaceName,&info,a->structureOutput,a->structureOutputSize,&used))return kIOReturnBadArgument;a->structureOutputSize=(uint32_t)used;return kIOReturnSuccess;}
 if(q->request==43||q->request==44){if(q->payload_bytes!=256||a->structureOutputSize<256)return kIOReturnBadArgument;
  static const char driver[]="Nord2 MT6893 passive scan controller";static const char hardware[]="Nord2 MT6893 Wi-Fi";bzero(a->structureOutput,256);
  if(q->request==43)memcpy(a->structureOutput,driver,sizeof driver);else memcpy(a->structureOutput,hardware,sizeof hardware);a->structureOutputSize=256;return kIOReturnSuccess;}
 if(q->request==15){nss_info info={};size_t used=0;if(nord2_wifi_scan_info(&info))return kIOReturnNotReady;
  if(niws_reply(q,interfaceName,info.scanning?NIWS_SCANNING:NIWS_IDLE,a->structureOutput,a->structureOutputSize,&used))return kIOReturnBadArgument;a->structureOutputSize=(uint32_t)used;return kIOReturnSuccess;}
 if(q->request==13||q->request==21||q->request==1||q->request==9||q->request==2||q->request==4||q->request==16||q->request==103){
  nss_info info={};size_t used=0;int native=nord2_wifi_join_copy_state(station);
  if(native){if(q->request==21)return kIOReturnNotReady;bzero(station,sizeof(*station));station->phase=NJS_IDLE;}
  if(q->request==13&&nord2_wifi_scan_info(&info))return kIOReturnNotReady;
  int result=niwj_state_reply(station,q->request,q->payload_bytes,info.scanning,niw_ms()*1000,a->structureOutput,a->structureOutputSize,&used);
  if(result==1)return (IOReturn)6; /*ENXIO: no authenticated current association*/
  if(result)return result==-2?kIOReturnUnsupported:kIOReturnBadArgument;
  a->structureOutputSize=(uint32_t)used;return kIOReturnSuccess;
 }
 if(q->request==12){niwc_manifest empty={};size_t used=0;if(niwc_card_reply(carrier,bytes,&empty,a->structureOutput,a->structureOutputSize,&used))return kIOReturnBadArgument;a->structureOutputSize=(uint32_t)used;return kIOReturnSuccess;}
 if(q->request==11){if(q->payload_bytes!=1164||a->structureOutputSize<1164)return kIOReturnBadArgument;int result=nils_take(&cacheLife,scanGeneration,(uint8_t*)a->structureOutput,a->structureOutputSize);if(resultLogs++<256)printf("NORD2 WIFI IOS RESULT: status=%d next=%u count=%u generation=%llu\n",result,snapshot->cursor.next,snapshot->cursor.count,(unsigned long long)scanGeneration);if(result==0)return (IOReturn)0xe0820446;if(result<0)return lastScanError==kIOReturnSuccess?kIOReturnNotReady:lastScanError;a->structureOutputSize=1164;return kIOReturnSuccess;}
 if(q->request==19){nss_info info={};if(q->payload_bytes!=24||a->structureOutputSize<24||nord2_wifi_scan_info(&info))return kIOReturnNotReady;uint8_t power[24]={1,0,0,0,1,0,0,0,1};memcpy(a->structureOutput,power,sizeof power);a->structureOutputSize=sizeof power;return kIOReturnSuccess;}
 if(unsupportedLogs++<64)printf("NORD2 WIFI IOS REQUEST: request=%u bytes=%u unsupported=1\n",q->request,q->payload_bytes);
 return kIOReturnUnsupported;
}

IOReturn NiwlLifecycleClient::joinLocked(const uint8_t*p,size_t bytes){
 if(phase!=Armed||!ownsReference)return kIOReturnNotReady;
 if(joinTicket)return kNiwScanBusy;
 niwj_request*request=(niwj_request*)IOMalloc(sizeof(*request));if(!request)return kIOReturnNoMemory;
 niwj_diagnostic diag={};int decoded=niwj_decode(p,bytes,request,&diag);
 if(joinLogs++<32)printf("NORD2 WIFI IOS JOINREQ: bytes=%u reason=%u key_kind=%u key_bytes=%u native_flags=%#x assoc_flags=%#x assoc_reason=%u cached_pmk_bytes=%u blacklist_count=%u scope_id=%u conditional_option=%u transition_disable=%#x band_preference=%u t_ms=%llu\n",(unsigned)bytes,diag.reason,diag.key_kind,diag.key_bytes,diag.native_flags,diag.association_flags,diag.association_reason,diag.cached_pmk_bytes,diag.blacklist_count,diag.scope_id,diag.conditional_option,diag.transition_disable,diag.band_preference,niw_ms());
 if(decoded){niwj_free(request,sizeof(*request));return decoded==NIWJ_SHAPE?kIOReturnBadArgument:kIOReturnUnsupported;}
 retain();uint64_t ticket=0;int result=nord2_wifi_join_enqueue(request,this,generation,joinCompleted,&ticket);
 niwj_free(request,sizeof(*request));
 if(result){release();return result==2?kNiwScanBusy:result<0?kIOReturnBadArgument:kIOReturnNotReady;}
 /* Backend promises nonzero ticket, copied input, and no inline callbacks. */
 joinTicket=ticket;joinEpoch=generation;return kIOReturnSuccess;
}
void NiwlLifecycleClient::joinCompleted(void*cookie,uint64_t epoch,uint64_t ticket,enum njs_notice notice){
 NiwlLifecycleClient*c=(NiwlLifecycleClient*)cookie;bool retire=false;
 IOLockLock(c->gate);
 bool owned=c->joinTicket==ticket&&c->joinEpoch==epoch;
 bool current=owned&&c->phase==Armed&&c->generation==epoch;
 if(current&&notice!=NJS_RETIRED&&!nord2_wifi_join_copy_state(c->station)&&c->station->ticket==ticket&&c->station->client_epoch==epoch){
  if(notice==NJS_JOINED&&niwj_authorized(c->station)){
   /* Prioritize terminal completion if the consumer has little ring room.
    * Every query already sees the committed real native snapshot. */
   c->stationEventLocked(9);c->stationEventLocked(2);c->stationEventLocked(3);c->stationEventLocked(4);
  }else if(notice==NJS_JOIN_FAILED&&(c->station->result==2||c->station->result==9||c->station->result==21))c->stationEventLocked(9);
  else if(notice==NJS_LEFT&&!c->station->associated&&!c->station->ptk_ready&&!c->station->gtk_ready&&!c->station->data_ready){
   /* Native already closed controlled port + real ifnet link synchronously. */
   c->stationEventLocked(2);c->stationEventLocked(3);c->stationEventLocked(4);
  }
 }
 if(c->joinLogs++<64)printf("NORD2 WIFI IOS JOINNOTICE: notice=%u current=%u phase=%u result=%u authorized=%u generation=%llu ticket=%llu epoch=%llu t_ms=%llu\n",(unsigned)notice,(unsigned)current,c->station->phase,c->station->result,(unsigned)niwj_authorized(c->station),(unsigned long long)c->station->native_generation,(unsigned long long)ticket,(unsigned long long)epoch,niw_ms());
 if(owned&&notice==NJS_RETIRED){c->joinTicket=0;c->joinEpoch=0;retire=true;}
 IOLockUnlock(c->gate);if(retire)c->release();
}
bool NiwlLifecycleClient::stationEventLocked(unsigned event){
 if(phase!=Armed||!ownsReference||!data||!state)return false;
 uint8_t record[80];size_t bytes=0;if(niwj_notice_record(station,event,interfaceName,record,sizeof(record),&bytes))return false;
 uint32_t*indices=(uint32_t*)state->getBytesNoCopy();uint32_t consumer=__atomic_load_n(indices,__ATOMIC_ACQUIRE);
 niwe_view view={consumer,producer,4096};niwe_space space={};if(niwe_plan(&view,(uint16_t)(bytes-48),0,&space)!=1)return false;
 uint8_t*ring=(uint8_t*)data->getBytesNoCopy();for(uint32_t i=0;i<space.padding;i+=4){uint32_t pad=80;memcpy(ring+producer+i,&pad,4);}
 for(size_t i=0;i<bytes;i++)ring[(space.record_offset+i)%4096]=record[i];
 producer=space.next;__atomic_store_n(indices+1,producer,__ATOMIC_RELEASE);
 IOReturn result=sendAsyncResult64(reference,kIOReturnSuccess,nullptr,0);
 if(eventLogs++<128)printf("NORD2 WIFI IOS EVENT: event=%u published=1 notified=%u status=%#x producer=%u consumer=%u generation=%llu ticket=%llu epoch=%llu t_ms=%llu\n",event,(unsigned)(result==kIOReturnSuccess),(unsigned)result,producer,consumer,(unsigned long long)station->native_generation,(unsigned long long)joinTicket,(unsigned long long)joinEpoch,niw_ms());return true;
}
