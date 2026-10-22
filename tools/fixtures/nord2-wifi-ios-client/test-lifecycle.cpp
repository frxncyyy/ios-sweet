#define NIWL_HOST_TEST 1
#include "LifecycleShell.cpp"
#include <thread>
#include <cstdio>
static task owner_task,other_task;static unsigned checks; /* Added station snapshot is a second owned allocation. */
static IOReturn call(IOUserClient*c,unsigned selector,IOExternalMethodArguments&a){a.selector=selector;checks++;return c->externalMethod(selector,&a,nullptr,nullptr,nullptr);}
static IOExternalMethodArguments args(){return {};}
static IOUserClient*client(){NiwlLifecycleService*s=new NiwlLifecycleService;IOUserClient*c=nullptr;assert(!s->newUserClient(&owner_task,nullptr,0,nullptr,&c));s->release();return c;}
static void w32(uint8_t*p,uint32_t n){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(n>>(8*i));}
int main(){test_current=&owner_task;
 test_current=&other_task;{auto*c=new NiwlLifecycleClient;assert(!c->initWithTask(&owner_task,nullptr,0,nullptr));c->release();assert(owner_task.refs==1);}test_current=&owner_task;checks++;
 authorized=false;{auto*c=new NiwlLifecycleClient;assert(!c->initWithTask(&owner_task,nullptr,0,nullptr));c->release();assert(owner_task.refs==1);}authorized=true;fail_lock=true;{auto*c=new NiwlLifecycleClient;assert(!c->initWithTask(&owner_task,nullptr,0,nullptr));c->release();assert(owner_task.refs==1);}fail_lock=false;
 IOUserClient*c=client();assert(owner_task.refs==2&&locks==1);uint8_t byte=0xaa;auto a=args();a.structureOutput=&byte;a.structureOutputSize=1;assert(!call(c,1,a)&&byte==1);byte=0xaa;a.structureOutputSize=1;assert(!call(c,5,a)&&byte==1);checks+=2;
 test_current=&other_task;a=args();assert(call(c,3,a)==kIOReturnNotPrivileged&&buffer_count==0);test_current=&owner_task;
 a=args();a.scalarInputCount=1;assert(call(c,3,a)==kIOReturnBadArgument&&buffer_count==0);a=args();a.structureInputSize=1;assert(call(c,3,a)==kIOReturnBadArgument);
 fail_buffer_at=allocation_calls+1;a=args();assert(call(c,3,a)==kIOReturnNoMemory&&buffer_count==0);
 fail_buffer_at=allocation_calls+2;a=args();assert(call(c,3,a)==kIOReturnNoMemory&&buffer_count==0);fail_buffer_at=0;assert(!call(c,3,a)&&buffer_count==2);assert(call(c,3,a)==kIOReturnBusy&&buffer_count==2);
 IOOptionBits opt=0x55;IOMemoryDescriptor*data=nullptr,*state=nullptr;assert(!c->clientMemoryForType(1,&opt,&data)&&opt==kIOMapReadOnly);assert(!c->clientMemoryForType(2,&opt,&state)&&opt==0);for(size_t i=0;i<4096;i++)assert(((uint8_t*)static_cast<IOBufferMemoryDescriptor*>(data)->getBytesNoCopy())[i]==0);checks+=3;
 opt=0x55;IOMemoryDescriptor*sentinel=data;assert(c->clientMemoryForType(3,&opt,&sentinel)==kIOReturnBadArgument&&sentinel==data&&opt==0x55);test_current=&other_task;assert(c->clientMemoryForType(1,&opt,&sentinel)==kIOReturnNotPrivileged);test_current=&owner_task;
 OSAsyncReference64 ref={7,0xdeadbeef,0x12345678,4,5,6,7,8};a=args();a.asyncWakePort=7;a.asyncReference=ref;a.asyncReferenceCount=7;rights++;assert(call(c,2,a)==kIOReturnBadArgument&&rights==0);a.asyncReferenceCount=8;rights++;assert(!call(c,2,a)&&rights==1);rights++;assert(call(c,2,a)==kIOReturnNotReady&&rights==1);test_current=&other_task;rights++;assert(call(c,2,a)==kIOReturnNotPrivileged&&rights==1);test_current=&owner_task;
 a=args();assert(!call(c,4,a)&&rights==0&&buffer_count==2);assert(!call(c,4,a));assert(!call(c,3,a)&&buffer_count==4);IOMemoryDescriptor*fresh=nullptr;assert(!c->clientMemoryForType(1,&opt,&fresh)&&fresh!=data);fresh->release();data->release();state->release();assert(buffer_count==2);
 uint8_t carrier[NIW_SMALL_BYTES]={0},output[256];w32(carrier,NIW_GET);std::memcpy(carrier+15368,"en0",4);w32(carrier+15384,43);auto*md=IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task,0,sizeof carrier,1);std::memcpy(md->getBytesNoCopy(),carrier,sizeof carrier);a=args();a.structureInputDescriptor=md;a.structureOutput=output;a.structureOutputSize=256;std::memset(output,0xaa,sizeof output);assert(!call(c,0,a)&&a.structureOutputSize<256&&!std::strcmp((char*)output,"Nord2 MT6893 passive scan controller")&&output[a.structureOutputSize]==0xaa&&malloc_count==2);
 assert(md->prepares==1&&md->completes==1&&md->reads==1&&!md->depth);
 for(unsigned failure=0;failure<3;failure++){
  unsigned pre=md->prepares,complete=md->completes,read=md->reads;
  md->failPrepare=failure==0;md->failRead=failure==1;md->failComplete=failure==2;
  a.structureOutputSize=256;std::memset(output,0xaa,sizeof output);
  assert(call(c,0,a)!=kIOReturnSuccess&&a.structureOutputSize==256&&!md->depth&&malloc_count==2);
  for(auto byte:output)assert(byte==0xaa);
  assert(md->prepares==pre+1&&md->reads==read+(failure!=0)&&md->completes==complete+(failure!=0));
 }
 md->failPrepare=md->failRead=md->failComplete=false;
 w32(carrier+15384,12);w32(carrier+4,13);std::memcpy(md->getBytesNoCopy(),carrier,sizeof carrier);a.structureOutputSize=13;std::memset(output,0xaa,sizeof output);assert(!call(c,0,a)&&output[0]==0&&a.structureOutputSize==13&&malloc_count==2);md->release();
 a=args();a.asyncWakePort=7;a.asyncReference=ref;a.asyncReferenceCount=8;rights++;assert(!call(c,2,a)&&rights==1);test_current=&other_task;assert(!c->clientDied()&&rights==0&&buffer_count==0);test_current=&owner_task;assert(!c->clientClose());a=args();assert(call(c,3,a)==kIOReturnNotOpen);c->release();assert(owner_task.refs==1&&locks==0);
 c=client();a=args();assert(!call(c,3,a));std::atomic<unsigned>done{0};std::thread thread([&]{test_current=&owner_task;for(unsigned i=0;i<1000;i++){uint8_t v=0xaa;auto b=args();b.selector=1;b.structureOutput=&v;b.structureOutputSize=1;auto rc=c->externalMethod(1,&b,nullptr,nullptr,nullptr);assert(rc==0||rc==kIOReturnNotOpen);if(!rc)assert(v==1);}done=1;});assert(!c->clientClose());thread.join();assert(done==1&&buffer_count==0);c->release();assert(owner_task.refs==1&&locks==0&&rights==0&&malloc_count==0);checks++;
 /* Close must wait behind the actual gate while an OOL read is prepared. */
 c=client();auto*blocked=IOBufferMemoryDescriptor::inTaskWithOptions(kernel_task,0,NIW_SMALL_BYTES,1);
 w32(carrier+15384,43);w32(carrier+4,0);std::memcpy(blocked->getBytesNoCopy(),carrier,sizeof carrier);blocked->blockRead=true;
 std::atomic<bool>closeStarted{false},closeDone{false};IOReturn readResult=kIOReturnNotReady;
 std::thread reader([&]{test_current=&owner_task;auto b=args();b.selector=0;b.structureInputDescriptor=blocked;uint8_t reply[256];b.structureOutput=reply;b.structureOutputSize=256;readResult=c->externalMethod(0,&b,nullptr,nullptr,nullptr);});
 while(!blocked->enteredRead)std::this_thread::yield();
 std::thread closer([&]{test_current=&owner_task;closeStarted=true;assert(!c->clientClose());closeDone=true;});
 while(!closeStarted)std::this_thread::yield();assert(!closeDone);blocked->releaseRead=true;reader.join();closer.join();
 assert(readResult==kIOReturnSuccess&&closeDone&&blocked->prepares==1&&blocked->completes==1&&!blocked->depth&&malloc_count==2);
 blocked->release();c->release();assert(owner_task.refs==1&&locks==0&&buffer_count==0);checks++;
 for(unsigned i=0;i<32;i++){c=client();a=args();assert(!call(c,3,a));assert(!c->clientClose());c->release();assert(owner_task.refs==1&&locks==0&&buffer_count==0);}checks+=32;
 std::printf("PASS %u actual shell lifecycle/owner/async-right/prepared-OOL/mapping/replay/allocation/close/death checks plus1000 concurrent close-admission probes; host IOKit primitives mocked, no service or backend\n",checks);
}
