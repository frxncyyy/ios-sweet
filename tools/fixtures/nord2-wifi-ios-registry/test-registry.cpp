#define NIWI_REGISTRY_HOST_TEST 1
#include "wifi_ios_registry.cpp"
extern "C" int nord2_wifi_scan_info(nss_info*out){*out={};out->count=1;out->mac[0]=2;return 0;}
extern "C" IOReturn niwl_create_client(IOService*,task_t,void*,UInt32,OSDictionary*,IOUserClient**){return 0;}
static IOService provider;
static void reset(){starts=stops=attaches=detaches=failStart=failAttach=0;registered.clear();failProperty.clear();}
int main(){static const char compatible[]="nord2,mt6893-wifi";auto*d=new OSData(compatible,sizeof compatible);provider.properties.setObject("compatible",d);d->release();
 reset();bootOpt=0;IO80211Controller off;assert(!off.start(&provider)&&!allocated);bootOpt=1;
 reset();attachFailures=1;IO80211Controller c;assert(c.start(&provider));assert(registered.size()==2&&!strcmp(c.className(),"IO80211Controller"));IOService*i=registered[0];assert(dynamic_cast<IO80211Interface*>(i));assert(dynamic_cast<IONetworkInterface*>(i));
 assert(!strcmp(OSDynamicCast(OSString,i->getProperty("IOInterfaceNamePrefix"))->value.c_str(),"en"));
 assert(OSDynamicCast(OSBoolean,i->getProperty("IOBuiltin"))->value);
 assert(OSDynamicCast(OSBoolean,i->getProperty("IOPrimaryInterface"))->value);
 assert(OSDynamicCast(OSBoolean,c.getProperty("IOBuiltin"))->value);
 auto*mac=OSDynamicCast(OSData,c.getProperty("IOMACAddress"));assert(mac&&mac->getLength()==6&&!memcmp(mac->getBytesNoCopy(),lastNet->mac,6));assert(!strcmp(OSDynamicCast(OSString,i->getProperty("IO80211InterfaceRole"))->value.c_str(),"Infrastructure"));assert(!strcmp(OSDynamicCast(OSString,i->getProperty("IOInterfaceName"))->value.c_str(),"en1"));assert(!c.getProperty("IOInterfaceName"));assert(!(lastNet->flags&(IFF_UP|IFF_RUNNING))&&lastNet->mac[0]==2);
 ifmediareq q={};assert(!lastNet->params.ioctl(lastNet,0xc02c6938,&q));assert((q.ifm_current&0xe0)==0x80&&q.ifm_status==IFM_AVALID&&!(q.ifm_status&IFM_ACTIVE)&&q.ifm_count==1&&!copied);
 int media=0;q.ifm_count=1;q.ifm_ulist=&media;assert(!lastNet->params.ioctl(lastNet,SIOCGIFMEDIA,&q)&&media==(IFM_IEEE80211|IFM_AUTO)&&copied==1);failCopyout=true;q.ifm_count=1;assert(lastNet->params.ioctl(lastNet,SIOCGIFMEDIA,&q)==EFAULT);failCopyout=false;
 assert(lastNet->params.ioctl(lastNet,123,&q)==EOPNOTSUPP);assert(lastNet->params.output(lastNet,new mbuf)==ENETDOWN&&packetsFreed==1);c.stop(&provider);assert(allocated==released);
 reset();failStart=2;IO80211Controller bad;unsigned before=allocated;assert(!bad.start(&provider)&&allocated==before+1&&allocated==released&&detaches==1&&stops==1);
 reset();failAttach=1;IO80211Controller noAttach;assert(!noAttach.start(&provider)&&allocated==released&&stops==1);
 reset();failLL=true;IO80211Controller noMAC;assert(!noMAC.start(&provider)&&allocated==released&&stops==1);failLL=false;
 for(const char*property:{"IOMACAddress","IOBuiltin","IOInterfaceNamePrefix","IOPrimaryInterface"}){
  reset();failProperty=property;IO80211Controller noProperty;assert(!noProperty.start(&provider));assert(registered.empty()&&allocated==released);
 }
 puts("PASS actual registry/ifnet: Infrastructure/name/class hierarchy, actual media ioctl0xc02c6938 count0/1+copyout fault, carrierdown/TXrefusal, collision and attachment failures balanced");
}
