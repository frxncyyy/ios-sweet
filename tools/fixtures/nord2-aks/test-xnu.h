#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <initializer_list>
using IOReturn=uint32_t;
using IOByteCount=uint64_t;
constexpr IOReturn kIOReturnSuccess=0,kIOReturnError=0xe00002bc,
 kIOReturnNoSpace=0xe00002db,kIOReturnBadArgument=0xe00002c2,
 kIOReturnUnsupported=0xe00002c7,kIOReturnIOError=0xe00002ca,
 kIOReturnNotReady=0xe00002d8;
constexpr unsigned kIOExternalMethodArgumentsCurrentVersion=2;
enum IODirection { kIODirectionIn=1,kIODirectionOut=2 };
class OSObject {};
class IOExternalMethodDispatch {};
class IOMemoryDescriptor {
public:
 uint8_t data[128];size_t length=128;unsigned prepares=0,writes=0,completes=0,depth=0;
 bool fail_prepare=false,short_write=false,fail_complete=false;
 IOMemoryDescriptor(){memset(data,0xa5,sizeof(data));}
 IOByteCount getLength(){return length;}
 IOReturn prepare(IODirection d){if(d!=kIODirectionIn||depth)abort();prepares++;if(fail_prepare)return kIOReturnNotReady;depth=1;return 0;}
 IOByteCount writeBytes(IOByteCount at,const void*p,IOByteCount n){if(depth!=1||at||n>length||n>sizeof(data))abort();writes++;size_t used=(size_t)n-(short_write?1:0);memcpy(data,p,used);return used;}
 IOReturn complete(IODirection d){if(d!=kIODirectionIn||depth!=1)abort();depth=0;completes++;return fail_complete?kIOReturnError:0;}
};
struct IOExternalMethodArguments {
 uint32_t version=2,selector=0,asyncWakePort=0;uint64_t*asyncReference=nullptr;uint32_t asyncReferenceCount=0;
 const uint64_t*scalarInput=nullptr;uint32_t scalarInputCount=0;
 const void*structureInput=nullptr;uint32_t structureInputSize=0;IOMemoryDescriptor*structureInputDescriptor=nullptr;
 uint64_t*scalarOutput=nullptr;uint32_t scalarOutputCount=0;
 void*structureOutput=nullptr;uint32_t structureOutputSize=0;IOMemoryDescriptor*structureOutputDescriptor=nullptr;
 uint32_t structureOutputDescriptorSize=0,__reservedA=0;OSObject**structureVariableOutputData=nullptr;uint32_t __reserved[30]={};
};
class IOService : public OSObject {
public:
 virtual ~IOService()=default;
 virtual bool start(IOService*){return true;}
 void setProperty(const char*,const char*){}
 void registerService(){}
};
class IOUserClient : public IOService {
public:
 virtual IOReturn clientClose(){return 0;}
 virtual IOReturn externalMethod(uint32_t,IOExternalMethodArguments*,IOExternalMethodDispatch*,OSObject*,void*){return kIOReturnUnsupported;}
 void terminate(){}
};
#define OSDeclareDefaultStructors(C)
#define OSDefineMetaClassAndStructors(C,P)
#define APPLE_KEXT_OVERRIDE override
#ifndef __unused
#define __unused __attribute__((unused))
#endif
constexpr const char*kIOUserClientClassKey="IOUserClientClass";
inline void IOLog(const char*,...){/* Intentionally suppress key-free numeric diagnostics in exhaustive tests. */}
inline void proc_selfname(char*p,size_t n){if(n)std::snprintf(p,n,"securityd");}
