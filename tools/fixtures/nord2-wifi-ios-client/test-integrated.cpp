#define NIWL_HOST_TEST 1
#include "LifecycleShell.cpp"
#include <thread>
static task ownerTask,otherTask;
static nss_completion callback;static void*cookie;static uint64_t ticket=1,gen;static bool canceled,active;static unsigned releases;static nsb_scan scan;
extern "C" int nord2_wifi_scan_info(nss_info*out){*out={};out->mac[0]=2;out->count=1;out->channels[0]={1,6};return 0;}
extern "C" int nord2_wifi_scan_enqueue(const nss_request*r,void*c,uint64_t g,nss_completion f,uint64_t*t){assert(!r->count&&!active);active=true;canceled=false;cookie=c;gen=g;callback=f;*t=++ticket;return 0;}
extern "C" int nord2_wifi_scan_cancel(uint64_t t){assert(t==ticket);canceled=true;return 0;}
extern "C" int nord2_wifi_scan_view(uint64_t t,nss_view*out){assert(t==ticket);if(canceled)return 1;*out={&scan,400,gen,NSS_COMPLETE};return 0;}
extern "C" int nord2_wifi_scan_release(uint64_t t){assert(t==ticket);releases++;return 0;}
static void finish(nss_status status=NSS_COMPLETE){assert(active);callback(cookie,gen,ticket,status);active=false;}
static void put(uint8_t*p,uint32_t n){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(n>>(8*i));}
static IOReturn call(IOUserClient*c,unsigned s,IOExternalMethodArguments&a){a.selector=s;return c->externalMethod(s,&a,nullptr,nullptr,nullptr);}
static IOUserClient*make(){test_current=&ownerTask;NiwlLifecycleService*p=new NiwlLifecycleService;IOUserClient*c=nullptr;assert(!p->newUserClient(&ownerTask,nullptr,0,nullptr,&c));p->release();return c;}
static void completeScan(){scan={};scan.entered=scan.done=1;scan.sequence=1;scan.started=100;scan.last=200;scan.deadline=1000;scan.completion.sequence=1;scan.completion.state=7;}
static void envelope(uint8_t*p,uint32_t operation,uint32_t request,uint32_t bytes){memset(p,0,NIW_SMALL_BYTES);put(p,operation);put(p+4,bytes);memcpy(p+15368,"en0",4);put(p+15384,request);}
static uint8_t carrier[NIW_SMALL_BYTES];
int main(){IOUserClient*c=make();IOExternalMethodArguments a={};uint8_t one=0;
 a.structureOutput=&one;a.structureOutputSize=1;assert(!call(c,1,a)&&one==1);a.structureOutputSize=1;assert(!call(c,5,a)&&one==1);
 a={};test_current=&otherTask;assert(call(c,3,a)==kIOReturnNotPrivileged);test_current=&ownerTask;assert(!call(c,3,a));
 OSAsyncReference64 ref={1};rights++;a={};a.asyncReference=ref;a.asyncReferenceCount=8;a.asyncWakePort=1;assert(!call(c,2,a));
 IOOptionBits options;IOMemoryDescriptor*d,*s;assert(!c->clientMemoryForType(1,&options,&d));assert(!c->clientMemoryForType(2,&options,&s));auto*ring=(uint8_t*)static_cast<IOBufferMemoryDescriptor*>(d)->getBytesNoCopy();auto*state=(uint32_t*)static_cast<IOBufferMemoryDescriptor*>(s)->getBytesNoCopy();
 envelope(carrier,0x802869c8,10,5456);put(carrier+8,1);put(carrier+12,3);put(carrier+60,1);put(carrier+64,1);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;
 carrier[24]=1;assert(call(c,0,a)==kIOReturnUnsupported&&!active);carrier[24]=0; /* nonzero SSID length is directed */
 carrier[100]=1;put(carrier+8+4880,0x80000000u); /* inactive channel storage plus caller RSSI default */
 completeScan();assert(!call(c,0,a)&&active);assert(call(c,0,a)==kIOReturnBusy);finish();assert(state[1]==52&&ring[0]==87&&ring[28]==10&&!ring[48]&&notifications==1&&releases==1);
 uint8_t row[1164];envelope(carrier,NIW_GET,11,1164);a.structureOutput=row;a.structureOutputSize=sizeof row;assert(call(c,0,a)==(IOReturn)0xe0820446); /* genuine completed empty */
 state[0]=state[1];completeScan();scan.count=scan.frames=1;auto&r=scan.records[0];r.observed_us=150;r.bssid[0]=2;r.subtype=8;r.channel=r.hw_channel=6;r.ssid_present=1;r.interval=100;r.rxv_valid=1;r.group_valid=4;r.rxv[4]=100;r.rxv[5]=90;r.ie_total=r.ie_bytes=3;r.ies[0]=1;r.ies[1]=1;r.ies[2]=2;
 envelope(carrier,0x802869c8,10,5456);put(carrier+8,1);put(carrier+12,3);put(carrier+60,1);put(carrier+64,1);a.structureOutput=nullptr;a.structureOutputSize=0;assert(!call(c,0,a));finish();
 envelope(carrier,NIW_GET,11,1164);a.structureOutput=row;a.structureOutputSize=sizeof row;assert(!call(c,0,a));niws_result decoded;assert(!niws_decode_result(row,sizeof row,&decoded)&&decoded.rates[0]==1);a.structureOutputSize=sizeof row;assert(call(c,0,a)==(IOReturn)0xe0820446);
 envelope(carrier,NIW_GET,43,0);memcpy(carrier+15368,"bad",4);a.structureOutputSize=sizeof row;assert(call(c,0,a)!=kIOReturnSuccess); /* identity cannot override interface check */
 envelope(carrier,NIW_GET,12,13);a.structureOutputSize=13;assert(!call(c,0,a));for(unsigned i=0;i<13;i++)assert(!row[i]);
 state[0]=state[1];envelope(carrier,0x802869c8,10,5456);put(carrier+8,1);put(carrier+12,3);put(carrier+60,1);put(carrier+64,1);a.structureOutput=nullptr;a.structureOutputSize=0;assert(!call(c,0,a));
 assert(!c->clientClose()&&canceled);c->release();unsigned count=notifications;finish(NSS_CANCELED);assert(notifications==count&&rights==0&&ownerTask.refs==1&&locks==0&&malloc_count==0);d->release();s->release();assert(buffer_count==0);
 /* Notification failure preserves the already published immutable completion
  * and real empty cache; a full ring never fabricates an empty success. */
 c=make();a={};assert(!call(c,3,a));rights++;a={};a.asyncReference=ref;a.asyncReferenceCount=8;a.asyncWakePort=1;assert(!call(c,2,a));
 envelope(carrier,0x802869c8,10,5456);put(carrier+8,1);put(carrier+12,3);put(carrier+60,1);put(carrier+64,1);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;completeScan();assert(!call(c,0,a));sendFail=true;finish();sendFail=false;
 envelope(carrier,NIW_GET,11,1164);a.structureOutput=row;a.structureOutputSize=sizeof row;assert(call(c,0,a)==(IOReturn)0xe0820446);assert(!c->clientClose());c->release();assert(!rights&&!malloc_count);
 c=make();a={};assert(!call(c,3,a));rights++;a={};a.asyncReference=ref;a.asyncReferenceCount=8;a.asyncWakePort=1;assert(!call(c,2,a));IOMemoryDescriptor*fullState;assert(!c->clientMemoryForType(2,&options,&fullState));auto*fullIndices=(uint32_t*)static_cast<IOBufferMemoryDescriptor*>(fullState)->getBytesNoCopy();fullIndices[0]=4;
 envelope(carrier,0x802869c8,10,5456);put(carrier+8,1);put(carrier+12,3);put(carrier+60,1);put(carrier+64,1);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;completeScan();assert(!call(c,0,a));count=notifications;finish();assert(notifications==count&&fullIndices[1]==0);
 envelope(carrier,NIW_GET,11,1164);a.structureOutput=row;a.structureOutputSize=sizeof row;assert(call(c,0,a)==kIOReturnNoResources);assert(!c->clientClose());c->release();fullState->release();assert(!rights&&!malloc_count&&!buffer_count);
 /* Actual completion publication holds gate through retained async send.
  * Close must block there, then release mappings/right exactly once. */
 c=make();a={};assert(!call(c,3,a));rights++;a={};a.asyncReference=ref;a.asyncReferenceCount=8;a.asyncWakePort=1;assert(!call(c,2,a));
 envelope(carrier,0x802869c8,10,5456);put(carrier+8,1);put(carrier+12,3);put(carrier+60,1);put(carrier+64,1);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;completeScan();assert(!call(c,0,a));
 enteredSend=false;allowSend=false;sendBlock=true;std::thread worker([]{finish();});while(!enteredSend)std::this_thread::yield();
 std::atomic<bool>closing{false},closed{false};std::thread closer([&]{test_current=&ownerTask;closing=true;assert(!c->clientClose());closed=true;});while(!closing)std::this_thread::yield();assert(!closed&&rights==1);allowSend=true;worker.join();closer.join();sendBlock=false;c->release();assert(rights==0&&malloc_count==0&&ownerTask.refs==1&&buffer_count==0);
 puts("PASS integrated actual IOUC: owner/ABI/default passive queue/cache-before-event/empty EOF/populated rates/CARD/bad-name/close-inflight retained callback/no late publication");
}
