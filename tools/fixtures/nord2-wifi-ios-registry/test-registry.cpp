#define NIWI_REGISTRY_HOST_TEST 1
#include "wifi_ios_registry.cpp"
extern "C" int nord2_wifi_scan_info(nss_info*out){*out={};out->count=1;out->mac[0]=2;return 0;}
extern "C" IOReturn niwl_create_client(IOService*,task_t,void*,UInt32,OSDictionary*,IOUserClient**){return 0;}
static IOService provider;
static void reset(){
 assert(!locks);starts=stops=attaches=detaches=failStart=failAttach=0;
 lockCalls=failLockAt=0;registered.clear();failProperty.clear();
}
/* Match the selected IOConnectSetCFProperties producer; IDs stay in memory. */
static OSDictionary* request(uint64_t id,uint64_t command=0,uint64_t unit=0,size_t length=8){
 uint8_t bytes[16]={};assert(length<=sizeof bytes);memcpy(bytes,&id,sizeof id);
 auto*d=OSDictionary::withCapacity(3);OSObject*v=new OSData(bytes,length);
 d->setObject("IORegistryEntryID",v);v->release();v=new OSNumber(command);
 d->setObject("IONetworkStackUserCommand",v);v->release();v=new OSNumber(unit);
 d->setObject("IOInterfaceUnit",v);v->release();return d;
}
static void checkStack(IO80211Controller&controller,IOService*interface){
 auto*stack=dynamic_cast<IONetworkStack*>(registered[1]);assert(stack);
 assert(!strcmp(stack->className(),"IONetworkStack"));
 auto*clientClass=OSDynamicCast(OSString,stack->getProperty("IOUserClientClass"));
 assert(clientClass&&clientClass->value=="IONetworkStackUserClient");
 assert(OSDynamicCast(OSNumber,interface->getProperty("IOInterfaceState"))->value==1);
 ifnet_t net=lastNet;unsigned actualUnit=net->params.unit;assert(net->refs==2);
 IOUserClient*out=(IOUserClient*)1;
 assert(stack->newUserClient(&testTask,&testTask,1,nullptr,&out)==kIOReturnUnsupported&&!out);
 assert(stack->newUserClient(&testTask,&testTask,0,nullptr,nullptr)==kIOReturnBadArgument);
 task other,unprivileged;unprivileged.admin=false;testCurrent=&unprivileged;
 assert(stack->newUserClient(&unprivileged,&unprivileged,0,nullptr,&out)==kIOReturnNotPrivileged&&!out);
 testCurrent=&testTask;
 assert(stack->newUserClient(&other,&other,0,nullptr,&out)==kIOReturnNotPrivileged&&!out);
 /* Failed client init/attach/start must balance task and provider ownership. */
 failLockAt=lockCalls+1;
 assert(stack->newUserClient(&testTask,&testTask,0,nullptr,&out)!=0&&!out&&testTask.refs==1);
 failLockAt=0;failAttach=attaches+1;
 assert(stack->newUserClient(&testTask,&testTask,0,nullptr,&out)!=0&&!out&&testTask.refs==1);
 failAttach=0;failStart=starts+1;
 assert(stack->newUserClient(&testTask,&testTask,0,nullptr,&out)!=0&&!out&&testTask.refs==1);
 failStart=0;IOUserClient*client=nullptr;
 assert(!stack->newUserClient(&testTask,&testTask,0,nullptr,&client)&&client&&testTask.refs==2);
 assert(!strcmp(client->className(),"IONetworkStackUserClient"));
 assert(stack->newUserClient(&testTask,&testTask,0,nullptr,&out)!=0&&!out&&testTask.refs==2);
 uint64_t id=interface->getRegistryEntryID();auto*d=request(id);
 for(uint64_t command:{0u,1u})for(uint64_t unit:std::vector<uint64_t>{0,actualUnit,31,UINT32_MAX}){
  auto*q=request(id,command,unit);assert(client->setProperties(q)==kIOReturnNoSpace);q->release();
 }
 auto*q=request(id+1);assert(client->setProperties(q)==kIOReturnNoDevice);q->release();
 for(size_t length:{0u,1u,7u,9u,16u}){
  q=request(id,0,0,length);assert(client->setProperties(q)==kIOReturnBadArgument);q->release();
 }
 q=request(id,0,UINT64_MAX);assert(client->setProperties(q)==kIOReturnBadArgument);q->release();
 q=request(id,3);assert(client->setProperties(q)==kIOReturnUnsupported);q->release();
 for(const char*key:{"IONetworkStackUserCommand","IORegistryEntryID","IOInterfaceUnit"}){
  q=request(id);q->removeObject(key);assert(client->setProperties(q)==kIOReturnBadArgument);
  auto*wrong=new OSString("wrong type");q->setObject(key,wrong);wrong->release();
  assert(client->setProperties(q)==kIOReturnBadArgument);q->release();
 }
 assert(client->setProperties(nullptr)==kIOReturnBadArgument);
 auto*all=OSDictionary::withCapacity(1);auto*command=new OSNumber(2);
 all->setObject("IONetworkStackUserCommand",command);command->release();
 assert(client->setProperties(all)==kIOReturnSuccess);
 q=request(id,2);assert(client->setProperties(q)==kIOReturnBadArgument);q->release();
 testCurrent=&other;assert(client->setProperties(d)==kIOReturnNotPrivileged);testCurrent=&testTask;
 testTask.admin=false;assert(client->setProperties(d)==kIOReturnNotPrivileged);
 assert(stack->setProperties(d)==kIOReturnNotPrivileged);testTask.admin=true;
 net->attached=false;assert(client->setProperties(d)==kIOReturnNotReady);net->attached=true;
 ifnet replacement;replacement.params=net->params;replacement.attached=true;lastNet=&replacement;
 assert(client->setProperties(d)==kIOReturnNotReady&&replacement.refs==1);lastNet=net;
 assert(net->params.unit==actualUnit&&net->refs==2&&!(net->flags&(IFF_UP|IFF_RUNNING)));
 /* A client may keep storage alive across real BSD detach, but cannot use it. */
 controller.stop(&provider);assert(allocated==released+1&&net->refs==1&&!net->attached);
 assert(!interface->getProperty("IOInterfaceState"));
 assert(client->setProperties(d)==kIOReturnNotReady);
 assert(client->setProperties(all)==kIOReturnNotReady);
 assert(stack->newUserClient(&testTask,&testTask,0,nullptr,&out)==kIOReturnNotReady&&!out);
 assert(!client->clientClose()&&!client->clientDied()&&testTask.refs==1);
 assert(client->setProperties(d)==kIOReturnNotOpen);
 client->release();d->release();all->release();assert(allocated==released&&!locks);
}
int main(){
 static const char compatible[]="nord2,mt6893-wifi";auto*d=new OSData(compatible,sizeof compatible);
 provider.properties.setObject("compatible",d);d->release();
 reset();bootOpt=0;IO80211Controller off;assert(!off.start(&provider)&&!allocated);bootOpt=1;
 reset();attachFailures=1;IO80211Controller controller;assert(controller.start(&provider));
 assert(registered.size()==3&&!strcmp(controller.className(),"IO80211Controller"));
 IOService*interface=registered[0];assert(dynamic_cast<IO80211Interface*>(interface));
 assert(dynamic_cast<IONetworkInterface*>(interface));
 assert(OSDynamicCast(OSString,interface->getProperty("IOInterfaceNamePrefix"))->value=="en");
 assert(OSDynamicCast(OSBoolean,interface->getProperty("IOBuiltin"))->value);
 assert(OSDynamicCast(OSBoolean,interface->getProperty("IOPrimaryInterface"))->value);
 assert(OSDynamicCast(OSBoolean,controller.getProperty("IOBuiltin"))->value);
 auto*mac=OSDynamicCast(OSData,controller.getProperty("IOMACAddress"));
 assert(mac&&mac->getLength()==6&&!memcmp(mac->getBytesNoCopy(),lastNet->mac,6));
 assert(OSDynamicCast(OSString,interface->getProperty("IO80211InterfaceRole"))->value=="Infrastructure");
 assert(OSDynamicCast(OSString,interface->getProperty("IOInterfaceName"))->value=="en1");
 assert(!controller.getProperty("IOInterfaceName"));
 assert(!(lastNet->flags&(IFF_UP|IFF_RUNNING))&&lastNet->mac[0]==2);
 ifmediareq mediaRequest={};assert(!lastNet->params.ioctl(lastNet,0xc02c6938,&mediaRequest));
 assert((mediaRequest.ifm_current&0xe0)==0x80&&mediaRequest.ifm_status==IFM_AVALID);
 assert(!(mediaRequest.ifm_status&IFM_ACTIVE)&&mediaRequest.ifm_count==1&&!copied);
 int media=0;mediaRequest.ifm_count=1;mediaRequest.ifm_ulist=&media;
 assert(!lastNet->params.ioctl(lastNet,SIOCGIFMEDIA,&mediaRequest));
 assert(media==(IFM_IEEE80211|IFM_AUTO)&&copied==1);
 failCopyout=true;mediaRequest.ifm_count=1;
 assert(lastNet->params.ioctl(lastNet,SIOCGIFMEDIA,&mediaRequest)==EFAULT);failCopyout=false;
 assert(lastNet->params.ioctl(lastNet,123,&mediaRequest)==EOPNOTSUPP);
 assert(lastNet->params.output(lastNet,new mbuf)==ENETDOWN&&packetsFreed==1);
 checkStack(controller,interface);
 reset();failStart=2;IO80211Controller bad;unsigned before=allocated;
 assert(!bad.start(&provider)&&allocated==before+1&&allocated==released&&detaches==1&&stops==1);
 reset();failAttach=1;IO80211Controller noAttach;
 assert(!noAttach.start(&provider)&&allocated==released&&stops==1);
 reset();failLL=true;IO80211Controller noMAC;
 assert(!noMAC.start(&provider)&&allocated==released&&stops==1);failLL=false;
 for(const char*property:{"IOMACAddress","IOBuiltin","IOInterfaceNamePrefix","IOPrimaryInterface","IOInterfaceState","IOUserClientClass"}){
  reset();failProperty=property;IO80211Controller noProperty;
  assert(!noProperty.start(&provider));assert(registered.empty()&&allocated==released&&!locks);
 }
 reset();failAttach=2;IO80211Controller noStackAttach;
 assert(!noStackAttach.start(&provider)&&registered.empty()&&allocated==released&&!locks);
 reset();failStart=3;IO80211Controller noStackStart;
 assert(!noStackStart.start(&provider)&&registered.empty()&&allocated==released&&!locks);
 reset();failLockAt=1;IO80211Controller noStackLock;
 assert(!noStackLock.start(&provider)&&registered.empty()&&allocated==released&&!locks);
 puts("PASS actual registry/stack: real ifnet/media/TX refusal, type0/admin/task, exact dictionary/repeat naming, publication rollback and detach-before-client-close ownership");
}
