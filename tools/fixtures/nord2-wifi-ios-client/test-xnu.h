#ifndef NIWL_TEST_XNU_H
#define NIWL_TEST_XNU_H
#include <cstdint>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <strings.h>
#include <cassert>
#include <mutex>
#include <atomic>
#include <thread>
#include <string>
#include <cstdio>
using UInt32=uint32_t;using IOOptionBits=uint32_t;using IOByteCount=size_t;using IODirection=uint32_t;using IOReturn=int;using io_user_reference_t=uint64_t;using OSAsyncReference64=uint64_t[8];
struct task {std::atomic<int> refs{1};};using task_t=task*;
#define TASK_NULL nullptr
static task kernel_task_storage;static task_t kernel_task=&kernel_task_storage;static thread_local task_t test_current;
static task_t current_task(){return test_current;}static void task_reference(task_t t){t->refs++;}static void task_deallocate(task_t t){t->refs--;}
static std::atomic<int> buffer_count{0},malloc_count{0},rights{0},locks{0},allocation_calls{0};static int fail_buffer_at=0;static bool authorized=true,fail_lock=false;
constexpr int kIOReturnSuccess=0,kIOReturnNotReady=-1,kIOReturnNotOpen=-2,kIOReturnNotPrivileged=-3,kIOReturnBadArgument=-4,kIOReturnUnsupported=-5,kIOReturnBusy=-6,kIOReturnNoMemory=-7,kIOReturnNoResources=-8;
constexpr uint32_t kIODirectionOut=2;
constexpr uint32_t kIOMemoryKernelUserShared=1,kIOMapReadOnly=0x1000,kIOExternalMethodArgumentsCurrentVersion=2;
struct IOLock {std::mutex m;};static IOLock*IOLockAlloc(){if(fail_lock)return nullptr;locks++;return new IOLock;}static void IOLockFree(IOLock*p){locks--;delete p;}static void IOLockLock(IOLock*p){p->m.lock();}static void IOLockUnlock(IOLock*p){p->m.unlock();}
static void*IOMalloc(size_t n){malloc_count++;return std::malloc(n);}static void IOFree(void*p,size_t){malloc_count--;std::free(p);}
class OSObject {std::atomic<int> refs{1};public:virtual ~OSObject()=default;virtual void free(){};void retain(){refs++;}void release(){assert(refs>0);if(!--refs){free();delete this;}}};
class OSBoolean:public OSObject {bool value;public:explicit OSBoolean(bool v):value(v){}bool isTrue(){return value;}};
#define OSDynamicCast(t,p) dynamic_cast<t*>(p)
class OSString:public OSObject {std::string value;public:explicit OSString(const char*v):value(v){}size_t getLength(){return value.size();}const char*getCStringNoCopy(){return value.c_str();}};
class OSDictionary:public OSObject{};
class IOMemoryDescriptor:public OSObject {public:virtual IOReturn prepare(IODirection)=0;virtual IOReturn complete(IODirection)=0;virtual size_t getLength()=0;virtual size_t readBytes(size_t,void*,size_t)=0;};
class IOBufferMemoryDescriptor:public IOMemoryDescriptor {void*bytes;size_t length;public:
 unsigned prepares=0,completes=0,reads=0,depth=0;bool failPrepare=false,failRead=false,failComplete=false;
 std::atomic<bool>blockRead{false},enteredRead{false},releaseRead{false};
 IOReturn prepare(IODirection dir)override{assert(dir==kIODirectionOut);prepares++;if(failPrepare)return kIOReturnNotReady;depth++;return kIOReturnSuccess;}
 IOReturn complete(IODirection dir)override{assert(dir==kIODirectionOut&&depth);completes++;depth--;return failComplete?kIOReturnNotReady:kIOReturnSuccess;}
 IOBufferMemoryDescriptor(size_t n):bytes(std::calloc(1,n)),length(n){buffer_count++;}~IOBufferMemoryDescriptor(){assert(!depth);std::free(bytes);buffer_count--;}static IOBufferMemoryDescriptor*inTaskWithOptions(task_t,IOOptionBits,size_t n,size_t){if(++allocation_calls==fail_buffer_at)return nullptr;return new IOBufferMemoryDescriptor(n);}void*getBytesNoCopy(){return bytes;}size_t getLength()override{return length;}size_t readBytes(size_t o,void*p,size_t n)override{assert(depth);reads++;enteredRead=true;while(blockRead&&!releaseRead)std::this_thread::yield();if(failRead)return n?n-1:0;if(o>length||n>length-o)return 0;std::memcpy(p,(char*)bytes+o,n);return n;}};
struct IOExternalMethodDispatch{};
struct IOExternalMethodArguments {uint32_t version=kIOExternalMethodArgumentsCurrentVersion,selector=0,scalarInputCount=0,scalarOutputCount=0,asyncReferenceCount=0;uint32_t asyncWakePort=0;io_user_reference_t*asyncReference=nullptr;const void*structureInput=nullptr;size_t structureInputSize=0;void*structureOutput=nullptr;size_t structureOutputSize=0;IOMemoryDescriptor*structureInputDescriptor=nullptr,*structureOutputDescriptor=nullptr;};
class IOUserClient;
class IOService:public OSObject {bool inactive=false;public:virtual bool start(IOService*){return true;}virtual void stop(IOService*){};virtual IOReturn newUserClient(task_t,void*,UInt32,OSDictionary*,IOUserClient**){return kIOReturnUnsupported;}bool attach(IOService*){return true;}void detach(IOService*){}OSObject*getProperty(const char*){static OSString name("en0");return &name;}bool isInactive(){return inactive;}void terminate(){inactive=true;}};
static unsigned notifications;static bool sendFail;static std::atomic<bool>sendBlock{false},enteredSend{false},allowSend{false};
class IOUserClient:public IOService {public:static IOReturn sendAsyncResult64(OSAsyncReference64,IOReturn,const io_user_reference_t*,uint32_t){notifications++;enteredSend=true;while(sendBlock&&!allowSend)std::this_thread::yield();return sendFail?kIOReturnNotReady:kIOReturnSuccess;}static OSObject*copyClientEntitlement(task_t,const char*){return new OSBoolean(authorized);}static IOReturn releaseAsyncReference64(OSAsyncReference64 r){if(r[0]){assert(rights>0);rights--;}return 0;}virtual bool initWithTask(task_t,void*,UInt32,OSDictionary*){return true;}virtual IOReturn externalMethod(uint32_t,IOExternalMethodArguments*,IOExternalMethodDispatch*,OSObject*,void*){return kIOReturnUnsupported;}virtual IOReturn clientMemoryForType(UInt32,IOOptionBits*,IOMemoryDescriptor**){return kIOReturnUnsupported;}virtual IOReturn clientClose(){return 0;}virtual IOReturn clientDied(){return 0;}};
#define OSDeclareDefaultStructors(t) public: t()=default;
#define OSDefineMetaClassAndStructors(t,b)
#endif
