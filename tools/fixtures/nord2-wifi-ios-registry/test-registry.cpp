#define NIWI_REGISTRY_HOST_TEST 1
#include "wifi_ios_registry.cpp"
extern "C" int nord2_wifi_scan_info(nss_info*out){*out={};out->count=1;out->mac[0]=2;return 0;}
extern "C" IOReturn niwl_create_client(IOService*,task_t,void*,UInt32,OSDictionary*,IOUserClient**){return 0;}
static IOService provider;
static void reset(){
 assert(locks==(nwil_lock?1u:0u));starts=stops=attaches=detaches=failStart=failAttach=0;
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
 ifnet_t net=lastNet;unsigned actualUnit=net->params.unit;assert(net->refs==3);
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
 assert(net->params.unit==actualUnit&&net->refs==3&&!(net->flags&(IFF_UP|IFF_RUNNING)));
 /* A client may keep storage alive across real BSD detach, but cannot use it. */
 controller.stop(&provider);assert(allocated==released+1&&net->refs==1&&!net->attached);
 assert(!interface->getProperty("IOInterfaceState"));
 assert(client->setProperties(d)==kIOReturnNotReady);
 assert(client->setProperties(all)==kIOReturnNotReady);
 assert(stack->newUserClient(&testTask,&testTask,0,nullptr,&out)==kIOReturnNotReady&&!out);
 assert(!client->clientClose()&&!client->clientDied()&&testTask.refs==1);
 assert(client->setProperties(d)==kIOReturnNotOpen);
 client->release();d->release();all->release();assert(allocated==released&&locks==(nwil_lock?1u:0u));
}
static std::vector<uint8_t> ethernet(unsigned type,size_t n=64){std::vector<uint8_t>b(n,0x5a);if(n>=14){b[12]=type>>8;b[13]=type;}return b;}
static mbuf_t packet(const std::vector<uint8_t>&b,size_t cut=0){auto*p=new mbuf;p->headerlen=b.size();p->len=cut?cut:b.size();memcpy(p->storage.data(),b.data(),p->len);if(cut){p->next=new mbuf;p->next->flags=0;p->next->len=b.size()-cut;memcpy(p->next->storage.data(),b.data()+cut,b.size()-cut);}return p;}
static void checkIfnet(ifnet_t net){
 assert(net->params.demux==ether_demux&&net->params.add_proto==ether_add_proto&&net->params.del_proto==ether_del_proto&&net->params.check_multi==ether_check_multi&&net->params.framer_extended==ether_frameout_extended&&bpfAttached==1);
 assert(nwil_bind(net)==EBUSY&&net->refs==3);
 auto ip=ethernet(0x0800),arp=ethernet(0x0806),ipv6=ethernet(0x86dd);
 assert(nord2_wifi_ifnet_link(0,1)==EINVAL&&nord2_wifi_ifnet_link(1,2)==EINVAL);
 assert(nord2_wifi_ifnet_receive(1,ip.data(),ip.size())==ENETDOWN);
 assert(!nwil_ioctl(net,SIOCSIFFLAGS,nullptr)&&!(net->flags&IFF_RUNNING));
 assert(!nwil_ioctl(net,SIOCSIFADDR,nullptr)&&(net->flags&IFF_UP)&&!(net->flags&IFF_RUNNING));
 ifreq rq={};rq.ifr_mtu=1400;assert(!nwil_ioctl(net,SIOCSIFMTU,&rq)&&net->mtu==1400);
 rq.ifr_mtu=1501;assert(nwil_ioctl(net,SIOCSIFMTU,&rq)==EINVAL);rq.ifr_mtu=71;assert(nwil_ioctl(net,SIOCSIFMTU,&rq)==EINVAL);
 assert(!nwil_ioctl(net,SIOCGIFDEVMTU,&rq)&&rq.ifr_devmtu.ifdm_current==1400&&rq.ifr_devmtu.ifdm_min==72&&rq.ifr_devmtu.ifdm_max==1500);
 assert(!nwil_ioctl(net,SIOCADDMULTI,nullptr)&&!nwil_ioctl(net,SIOCDELMULTI,nullptr));
 unsigned before=sentCalls;
 eventHook=[&]{assert(!nwil_output(net,packet(arp,9)));};
 assert(!nord2_wifi_ifnet_link(10,1));eventHook=nullptr;
 assert(sentCalls==before+1&&sentGeneration==10&&sentFrame==arp&&(net->flags&IFF_RUNNING)&&(net->flags&IFF_UP));
 assert(linkEvents.size()==1&&linkEvents.back().header.event_code==KEV_DL_LINK_ON&&linkEvents.back().unit==net->params.unit&&std::string(linkEvents.back().name)=="en");
 assert(!nord2_wifi_ifnet_link(10,1)&&linkEvents.size()==1);
 assert(nord2_wifi_ifnet_link(9,0)==ESTALE&&nord2_wifi_ifnet_link(11,1)==EBUSY);
 ifmediareq media={};assert(!nwil_ioctl(net,SIOCGIFMEDIA,&media)&&media.ifm_status==(IFM_AVALID|IFM_ACTIVE));
 for(const auto&f:{ip,arp,ipv6}){assert(!nwil_output(net,packet(f,7))&&sentFrame==f&&tappedFrame==f);assert(!nord2_wifi_ifnet_receive(10,f.data(),f.size())&&inputFrame==f&&tappedFrame==f);}
 for(unsigned type:{0x888eu,0x8100u,0x88a8u,0x0050u}){auto bad=ethernet(type);before=sentCalls;assert(nwil_output(net,packet(bad))==EINVAL&&sentCalls==before);assert(nord2_wifi_ifnet_receive(10,bad.data(),bad.size())==EINVAL);}
 auto tooLarge=ethernet(0x0800,1515);assert(nwil_output(net,packet(tooLarge))==EINVAL);assert(nord2_wifi_ifnet_receive(10,tooLarge.data(),tooLarge.size())==EINVAL);
 auto shortFrame=ethernet(0x0800,13);assert(nwil_output(net,packet(shortFrame))==EINVAL);
 auto*p=packet(ip);p->flags=0;assert(nwil_output(net,p)==EINVAL);
 p=packet(ip);p->nextpkt=packet(ip);before=packetsFreed;assert(nwil_output(net,p)==EINVAL&&packetsFreed==before+2);
 failCopyData=true;assert(nwil_output(net,packet(ip))==EINVAL);failCopyData=false;
 for(int result:{NWIL_SEND_NOT_READY,NWIL_SEND_BUSY,NWIL_SEND_INVALID}){sendResult=result;before=txTaps;assert(nwil_output(net,packet(ip))==(result==NWIL_SEND_BUSY?ENOBUFS:result==NWIL_SEND_NOT_READY?ENETDOWN:EINVAL)&&txTaps==before);}sendResult=0;
 for(bool*flag:{&failPacket,&smallPacket,&failSetRcvif,&failSetData}){*flag=true;assert(nord2_wifi_ifnet_receive(10,ip.data(),ip.size())!=0&&net->refs==3);*flag=false;}
 inputError=EIO;assert(nord2_wifi_ifnet_receive(10,ip.data(),ip.size())==EIO&&net->refs==3);inputError=0;
 assert(nord2_wifi_ifnet_receive(9,ip.data(),ip.size())==ENETDOWN);
 eventHook=[&]{assert(nwil_output(net,packet(ip))==ENETDOWN);};
 assert(!nord2_wifi_ifnet_link(10,0));eventHook=nullptr;
 assert(linkEvents.back().header.event_code==KEV_DL_LINK_OFF&&!(net->flags&IFF_RUNNING)&&(net->flags&IFF_UP));
 assert(nord2_wifi_ifnet_receive(10,ip.data(),ip.size())==ENETDOWN);
 eventError=EIO;assert(nord2_wifi_ifnet_link(11,1)==EIO&&!(net->flags&IFF_RUNNING)&&!nwil_active(net));eventError=0;assert(!nord2_wifi_ifnet_link(11,0));
 assert(!nwil_ioctl(net,SIOCGIFMEDIA,&media)&&media.ifm_status==IFM_AVALID);
 assert(!ifnet_set_flags(net,0,IFF_UP));
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
 checkIfnet(lastNet);
 checkStack(controller,interface);
 reset();failStart=2;IO80211Controller bad;unsigned before=allocated;
 assert(!bad.start(&provider)&&allocated==before+1&&allocated==released&&detaches==1&&stops==1);
 reset();failAttach=1;IO80211Controller noAttach;
 assert(!noAttach.start(&provider)&&allocated==released&&stops==1);
 reset();failLL=true;IO80211Controller noMAC;
 assert(!noMAC.start(&provider)&&allocated==released&&stops==1);failLL=false;
 for(const char*property:{"IOMACAddress","IOBuiltin","IOInterfaceNamePrefix","IOPrimaryInterface","IOInterfaceState","IOUserClientClass"}){
  reset();failProperty=property;IO80211Controller noProperty;
  assert(!noProperty.start(&provider));assert(registered.empty()&&allocated==released&&locks==(nwil_lock?1u:0u));
 }
 reset();failAttach=2;IO80211Controller noStackAttach;
 assert(!noStackAttach.start(&provider)&&registered.empty()&&allocated==released&&locks==(nwil_lock?1u:0u));
 reset();failStart=3;IO80211Controller noStackStart;
 assert(!noStackStart.start(&provider)&&registered.empty()&&allocated==released&&locks==(nwil_lock?1u:0u));
 reset();failLockAt=1;IO80211Controller noStackLock;
 assert(!noStackLock.start(&provider)&&registered.empty()&&allocated==released&&locks==(nwil_lock?1u:0u));
 assert(!nwil.net);IOLockFree(nwil_lock);nwil_lock=nullptr;assert(!locks);
 puts("PASS actual registry/stack/controlled Ethernet/BPF: real ifnet/media/TX refusal, type0/admin/task, exact dictionary/repeat naming, publication rollback and detach-before-client-close ownership");
}
