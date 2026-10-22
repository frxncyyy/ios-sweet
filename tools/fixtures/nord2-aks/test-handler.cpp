#include "test-xnu.h"
#include AKS_SOURCE
static unsigned checks;
#define CHECK(x) do{checks++;if(!(x)){std::fprintf(stderr,"check failed at line%d: %s\n",__LINE__,#x);std::abort();}}while(0)
static bool bytes(const uint8_t*p,size_t n,uint8_t v){while(n--)if(*p++!=v)return false;return true;}
struct Call {
 AppleKeyStoreUserClient client;uint64_t in[2]={0,7},scalar=UINT64_C(0xabcdef0123456789);
 uint8_t key[80],out[128];IOExternalMethodArguments a;
 Call(unsigned selector=10){for(unsigned i=0;i<sizeof(key);i++)key[i]=(uint8_t)i;memset(out,0xa5,sizeof(out));
  a.selector=selector;a.scalarInput=in;a.scalarInputCount=2;a.scalarOutput=&scalar;a.scalarOutputCount=selector==10?1:0;
  a.structureInput=key;a.structureInputSize=32;a.structureOutput=out;a.structureOutputSize=sizeof(out);}
 IOReturn run(){return client.externalMethod(a.selector,&a,nullptr,nullptr,nullptr);}
};
int main(){
 Call known;
#if defined(AKS_EXPECT_UNSUPPORTED)
 CHECK(known.run()==kIOReturnUnsupported);CHECK(bytes(known.out,sizeof(known.out),0xa5));
 CHECK(known.scalar==UINT64_C(0xabcdef0123456789));
 std::printf("PASS baseline/default-off actual handler: %u checks\n",checks);return 0;
#else
 static const uint8_t expected[40]={
  0x48,0x4b,0x77,0x72,0x07,0,0,0,
  0x5a,0x7e,0xa6,0xca,0xea,0x16,0x3e,0x5a,0x8a,0xae,0xc6,0xfa,0x1a,0x36,0x6e,0x8a,
  0xba,0xde,0xe6,0x0a,0x2a,0x76,0x9e,0xba,0xca,0xee,0x06,0x5a,0x7a,0x96,0xae,0xca};
 CHECK(known.run()==0);CHECK(known.a.structureOutputSize==40);CHECK(known.scalar==7);
 CHECK(!memcmp(known.out,expected,40));CHECK(bytes(known.out+40,sizeof(known.out)-40,0xa5));
 Call un(11);memcpy(un.key,expected,40);un.a.structureInputSize=40;
 CHECK(un.run()==0);CHECK(un.a.structureOutputSize==32);for(unsigned i=0;i<32;i++)CHECK(un.out[i]==i);
 for(unsigned n=1;n<=64;n++)for(unsigned cls=0;cls<256;cls++){
  Call c;c.in[1]=cls;c.a.structureInputSize=n;CHECK(c.run()==0);CHECK(c.scalar==cls);
  CHECK(c.a.structureOutputSize==n+8);CHECK(c.out[4]==cls);CHECK(bytes(c.out+5,3,0));
  Call u(11);u.in[1]=cls;memcpy(u.key,c.out,n+8);u.a.structureInputSize=n+8;
  CHECK(u.run()==0);CHECK(u.a.structureOutputSize==n);CHECK(!memcmp(u.out,c.key,n));
  u.a.structureInputSize=n+8;u.a.structureOutputSize=128;u.in[1]^=1;memset(u.out,0xa5,sizeof(u.out));
  CHECK(u.run()==kIOReturnBadArgument);CHECK(bytes(u.out,sizeof(u.out),0xa5));
 }
 Call upper;upper.in[1]=UINT64_C(0x123456789abcdef0);CHECK(upper.run()==0);CHECK(upper.scalar==upper.in[1]);CHECK(upper.out[4]==0xf0);
 Call upperu(11);upperu.in[1]=upper.in[1];memcpy(upperu.key,upper.out,40);upperu.a.structureInputSize=40;CHECK(upperu.run()==0);
 Call noecho;noecho.a.scalarOutputCount=0;noecho.a.scalarOutput=nullptr;CHECK(noecho.run()==0);CHECK(noecho.a.scalarOutputCount==0);
 for(unsigned sel:{10u,11u})for(unsigned fault=0;fault<25;fault++){
  Call c(sel);if(sel==11){memcpy(c.key,expected,40);c.a.structureInputSize=40;}
  IOMemoryDescriptor md;OSObject*variable=nullptr;
  switch(fault){
   case 0:c.a.version++;break;case 1:c.a.scalarInput=nullptr;break;case 2:c.a.scalarInputCount=1;break;
   case 3:c.a.scalarInputCount=3;break;case 4:c.in[0]=1;break;case 5:c.a.scalarOutputCount=sel==10?2:1;break;
   case 6:c.a.scalarOutputCount=1;c.a.scalarOutput=nullptr;break;
   case 7:c.a.asyncReferenceCount=1;break;case 8:c.a.asyncWakePort=1;break;
   case 9:c.a.structureInputDescriptor=&md;break;case 10:c.a.structureInput=nullptr;break;
   case 11:c.a.structureInputSize=0;break;case 12:c.a.structureInputSize=sel==10?65:73;break;
   case 13:c.a.structureOutput=nullptr;break;case 14:c.a.structureOutputSize=0;break;
   case 15:c.a.structureOutputDescriptorSize=1;break;
   case 16:c.a.structureOutputDescriptor=&md;c.a.structureOutputDescriptorSize=128;break;
   case 17:c.a.structureOutputDescriptor=&md;c.a.structureOutput=nullptr;c.a.structureOutputDescriptorSize=128;break;
   case 18:c.a.structureOutputDescriptor=&md;c.a.structureOutput=nullptr;c.a.structureOutputSize=0;c.a.structureOutputDescriptorSize=127;break;
   case 19:c.a.structureVariableOutputData=&variable;break;
   case 20:c.a.structureInputSize=sel==10?UINT32_MAX:8;break;
   case 21:if(sel==10)c.a.structureInputSize=0;else c.key[0]^=1;break;
   case 22:if(sel==10)c.a.scalarInputCount=0;else c.key[5]=1;break;
   case 23:if(sel==10)c.a.scalarOutputCount=3;else c.key[6]=1;break;
   case 24:if(sel==10)c.a.structureOutputSize=39;else c.key[7]=1;break;
  }
  auto a=c.a;uint64_t scalar=c.scalar;CHECK(c.run()!=0);CHECK(bytes(c.out,sizeof(c.out),0xa5));
  CHECK(c.scalar==scalar);CHECK(c.a.scalarOutputCount==a.scalarOutputCount);CHECK(c.a.structureOutputSize==a.structureOutputSize);
  CHECK(c.a.structureOutputDescriptorSize==a.structureOutputDescriptorSize);CHECK(!md.prepares&&!md.writes&&!md.completes);
 }
 for(unsigned sel:{10u,11u})for(unsigned cap=0;cap<(sel==10?40u:32u);cap++){
  Call c(sel);if(sel==11){memcpy(c.key,expected,40);c.a.structureInputSize=40;}c.a.structureOutputSize=cap;
  CHECK(c.run()==kIOReturnNoSpace);CHECK(bytes(c.out,sizeof(c.out),0xa5));CHECK(c.scalar==UINT64_C(0xabcdef0123456789));CHECK(c.a.structureOutputSize==cap);
 }
 for(unsigned sel:{10u,11u})for(unsigned fault=0;fault<5;fault++){
  Call c(sel);if(sel==11){memcpy(c.key,expected,40);c.a.structureInputSize=40;}
  IOMemoryDescriptor md;md.fail_prepare=fault==1;md.short_write=fault==2||fault==4;md.fail_complete=fault==3||fault==4;
  /* Real MIG keeps this inband pointer nonnull with size0 for OOL output. */
  c.a.structureOutput=c.out;c.a.structureOutputSize=0;c.a.structureOutputDescriptor=&md;c.a.structureOutputDescriptorSize=128;
  uint8_t scratch[72];memset(scratch,0xde,sizeof(scratch));IOReturn status=sweet_aks_compat_key(sel,&c.a,scratch);
  CHECK(bytes(scratch,sizeof(scratch),0));CHECK(bytes(c.out,sizeof(c.out),0xa5));CHECK(!md.depth);CHECK(md.prepares==1);
  CHECK(md.writes==(fault==1?0u:1u));CHECK(md.completes==(fault==1?0u:1u));
  if(!fault){CHECK(status==0);CHECK(c.a.structureOutputDescriptorSize==(sel==10?40u:32u));
   CHECK(sel==10?!memcmp(md.data,expected,40):!memcmp(md.data,known.key,32));CHECK(c.scalar==(sel==10?7u:UINT64_C(0xabcdef0123456789)));}
  else{CHECK(status==(fault==1?kIOReturnNotReady:fault==2?kIOReturnIOError:kIOReturnError));
   CHECK(c.a.structureOutputDescriptorSize==128);CHECK(c.scalar==UINT64_C(0xabcdef0123456789));}
 }
 {Call c;IOMemoryDescriptor md;md.length=39;c.a.structureOutput=nullptr;c.a.structureOutputSize=0;c.a.structureOutputDescriptor=&md;c.a.structureOutputDescriptorSize=39;CHECK(c.run()==kIOReturnNoSpace);CHECK(!md.prepares);}
 {uint8_t scratch[72];memset(scratch,0x1f,sizeof(scratch));CHECK(sweet_aks_compat_key(10,nullptr,scratch)==kIOReturnBadArgument);CHECK(bytes(scratch,72,0));Call c;CHECK(c.client.externalMethod(10,nullptr,nullptr,nullptr,nullptr)==kIOReturnBadArgument);}
 {Call c;uint8_t scratch[72];memset(scratch,1,72);CHECK(sweet_aks_compat_key(11,&c.a,scratch)==kIOReturnBadArgument);CHECK(bytes(scratch,72,0));}
 {Call c;c.a.structureOutput=c.key;c.a.structureOutputSize=80;CHECK(c.run()==0);CHECK(!memcmp(c.key,expected,40));}
 {Call again;CHECK(again.run()==0);CHECK(!memcmp(again.out,expected,40));}
 {Call c;c.a.selector=kAKSGetSystem;c.a.structureInputSize=0;c.a.scalarInputCount=0;CHECK(c.run()==0);CHECK(c.scalar==0);}
 std::printf("PASS actual generated AKS10/11 handler: %u checks; original vector, all64 lengths/256 classes, atomic preflight, descriptor faults, scratch wipe\n",checks);
#endif
}
