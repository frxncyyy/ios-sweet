static void retained_client(){
 for(unsigned failure=1;failure<=3;failure++){
  niwl_malloc_fail=niwl_malloc_calls+failure;auto*provider=new NiwlLifecycleService;IOUserClient*rejected=nullptr;
  assert(provider->newUserClient(&ownerTask,nullptr,0,nullptr,&rejected)!=0&&!rejected);provider->release();
  assert(!malloc_count&&!locks&&!rights&&ownerTask.refs==1);niwl_malloc_fail=0;
 }
 niwl_scan_extended=true;IOUserClient*c=make();IOExternalMethodArguments a={};assert(!call(c,3,a));OSAsyncReference64 ref={1};rights++;a={};a.asyncReference=ref;a.asyncReferenceCount=8;a.asyncWakePort=1;assert(!call(c,2,a));
 auto subset=[&](unsigned channel,unsigned id,uint64_t start,bool timeout=false){
  completeScan();scan.started=start;scan.last=start+200;scan.deadline=start+1000;scan.count=scan.frames=1;
  auto&r=scan.records[0];r.observed_us=start+100;r.bssid[0]=2;r.bssid[5]=(uint8_t)id;r.subtype=8;r.channel=r.hw_channel=(uint8_t)channel;r.capability=1;r.ssid_present=1;r.ssid_len=4;memcpy(r.ssid,"test",4);r.interval=100;r.rxv_valid=1;r.group_valid=4;r.rxv[4]=100;r.rxv[5]=255;r.ie_total=r.ie_bytes=3;r.ies[0]=1;r.ies[1]=1;r.ies[2]=12;
  envelope(carrier,0x802869c8,10,5456);put(carrier+8,1);put(carrier+12,2);put(carrier+60,2);put(carrier+64,1);put(carrier+8+68,1);put(carrier+8+72,1);put(carrier+8+76,channel);put(carrier+8+80,channel==100?0x112:0xa);
  a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;assert(!call(c,0,a));niwl_scan_test_us=start+400;
  if(timeout){scan.partial=scan.timed_out=1;scan.done=0;finish(NSS_PARTIAL);}else finish();
 };
 uint8_t row[1164];auto take=[&](unsigned*channel){envelope(carrier,NIW_GET,11,1164);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;a.structureOutput=row;a.structureOutputSize=sizeof row;IOReturn result=call(c,0,a);if(!result){niws_result decoded;assert(!niws_decode_result(row,sizeof row,&decoded));*channel=decoded.channel;}return result;};
 unsigned channel=0;subset(6,1,1000000);assert(!take(&channel)&&channel==6);assert(take(&channel)==(IOReturn)0xe0820446);
 subset(100,2,2000000);assert(!take(&channel)&&channel==6);assert(!take(&channel)&&channel==100);assert(take(&channel)==(IOReturn)0xe0820446);
 subset(100,2,3000000);niwl_scan_test_us=31000100;assert(!take(&channel)&&channel==100);assert(take(&channel)==(IOReturn)0xe0820446); /* old2.4 entry expired, current5GHz remains */
 subset(6,3,32000000,true);assert(take(&channel)==kIOReturnNotReady); /* failed scan never refreshes history */
 subset(100,2,34000000);assert(!take(&channel)&&channel==100);assert(take(&channel)==(IOReturn)0xe0820446);
 assert(!c->clientClose());c->release();assert(!rights&&!locks&&!malloc_count&&!buffer_count);
 c=make();a={};assert(!call(c,3,a));rights++;a={};a.asyncReference=ref;a.asyncReferenceCount=8;a.asyncWakePort=1;assert(!call(c,2,a));
 completeScan();scan.started=35000000;scan.last=35000200;scan.deadline=35001000;scan.count=scan.frames=0;niwl_scan_test_us=35000400;
 envelope(carrier,0x802869c8,10,5456);put(carrier+8,1);put(carrier+12,2);put(carrier+60,2);put(carrier+64,1);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;assert(!call(c,0,a));finish();assert(take(&channel)==(IOReturn)0xe0820446); /* newclient doesnot inheritoldhistory */
 assert(!c->clientClose());c->release();niwl_scan_extended=false;niwl_scan_test_us=400;assert(!rights&&!locks&&!malloc_count&&!buffer_count);
 puts("PASS actual IOUC retention:2.4/DFS5GHz subset union, original age/30s expiry, failed scan refusal, close/reopen isolation; no fake native join");
}
int main(){assert(!oldmain());IOUserClient*c=make();IOExternalMethodArguments a={};uint8_t reply[1164];
 /* Normal fixed256 identity shape differs from initial zero-body bind. */
 for(unsigned req:{43u,44u}){envelope(carrier,NIW_GET,req,256);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;a.structureOutput=reply;a.structureOutputSize=256;memset(reply,0xaa,sizeof reply);assert(!call(c,0,a)&&a.structureOutputSize==256&&reply[0]=='N');unsigned end=0;while(reply[end])end++;for(unsigned i=end;i<256;i++)assert(!reply[i]);assert(reply[256]==0xaa);}
 /* Polling must not consume event/scan diagnostic budgets. */
 for(unsigned n=0;n<160;n++){envelope(carrier,NIW_GET,19,24);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;a.structureOutput=reply;a.structureOutputSize=24;assert(!call(c,0,a));}
 for(unsigned req:{13u,15u}){envelope(carrier,NIW_GET,req,0);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;a.structureOutput=reply;a.structureOutputSize=4;memset(reply,0xaa,sizeof reply);assert(!call(c,0,a)&&a.structureOutputSize==4&&reply[0]==(req==15)&&!reply[1]&&!reply[2]&&!reply[3]&&reply[4]==0xaa);}
 active=true;envelope(carrier,NIW_GET,13,0);a.structureOutputSize=4;assert(!call(c,0,a)&&reply[0]==1);active=false;
 envelope(carrier,NIW_GET,103,1164);a.structureOutputSize=1164;memset(reply,0xaa,sizeof reply);assert(call(c,0,a)==6);for(auto v:reply)assert(v==0xaa);
 envelope(carrier,0x802869c8,19,24);put(carrier+8,1);put(carrier+12,1);put(carrier+16,1);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;unsigned before=notifications;assert(!call(c,0,a)&&notifications==before&&!active);unsigned powerBefore=power_calls;uint64_t epochBefore=logical_epoch;put(carrier+16,0);assert(!call(c,0,a)&&!logical_power&&logical_epoch==epochBefore+1&&power_calls==powerBefore+1&&notifications==before);put(carrier+16,1);put(carrier+20,1);assert(!call(c,0,a)&&logical_power&&logical_epoch==epochBefore+2&&power_calls==powerBefore+2&&notifications==before&&!active);put(carrier+20,0); /* Open client has no async pipe; armed POWER_CHANGED publication is covered by test-power. */
 envelope(carrier,NIW_GET,19,24);auto*md=new IOBufferMemoryDescriptor(24);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;a.structureOutputDescriptor=md;a.structureOutputDescriptorSize=24;assert(!call(c,0,a)&&md->prepares==1&&md->completes==1&&md->writes==1&&a.structureOutputDescriptorSize==24);auto*p=(uint8_t*)md->getBytesNoCopy();assert(p[0]==1&&p[4]==1&&p[8]==1);
 auto*channels=new IOBufferMemoryDescriptor(4824);envelope(carrier,NIW_GET,27,4824);a.structureOutputDescriptor=channels;a.structureOutputDescriptorSize=4824;assert(!call(c,0,a)&&a.structureOutputDescriptorSize==4824&&channels->writes==1);auto*ch=(uint8_t*)channels->getBytesNoCopy();assert(ch[0]==1&&ch[4]==1&&ch[8]==1&&ch[12]==6&&ch[16]==10);for(unsigned i=20;i<4824;i++)assert(!ch[i]);envelope(carrier,NIW_GET,254,4824);a.structureOutputDescriptorSize=4824;assert(!call(c,0,a));channels->release();envelope(carrier,NIW_GET,19,24);a.structureOutputDescriptor=md;a.structureOutputDescriptorSize=24;
 md->failPrepare=true;assert(call(c,0,a)==kIOReturnNotReady&&md->writes==1&&md->completes==1);md->failPrepare=false;md->failWrite=true;assert(call(c,0,a)==kIOReturnBadArgument&&md->writes==2&&md->completes==2);md->failWrite=false;md->failComplete=true;assert(call(c,0,a)==kIOReturnNotReady&&md->completes==3);md->failComplete=false;
 unsigned preparations=md->prepares;a.structureOutputDescriptorSize=23;assert(call(c,0,a)==kIOReturnBadArgument&&md->prepares==preparations);a.structureOutputDescriptorSize=24;test_current=&otherTask;assert(call(c,0,a)==kIOReturnNotPrivileged&&md->prepares==preparations);test_current=&ownerTask;a.structureOutput=(void*)1;assert(!call(c,0,a));a.structureOutput=reply;a.structureOutputSize=1;preparations=md->prepares;assert(call(c,0,a)==kIOReturnBadArgument&&md->prepares==preparations);a.structureOutput=nullptr;a.structureOutputSize=0;
 envelope(carrier,NIW_GET,103,1164);a.structureOutputDescriptorSize=24;assert(call(c,0,a)==kIOReturnBadArgument&&md->writes==4);md->release();
 /* A failed delivery must not consume an actual populated scan cache row. */
 a={};assert(!call(c,3,a));OSAsyncReference64 ref={1};rights++;a={};a.asyncReference=ref;a.asyncReferenceCount=8;a.asyncWakePort=1;assert(!call(c,2,a));
 completeScan();scan.count=scan.frames=1;auto&r=scan.records[0];r.observed_us=150;r.bssid[0]=2;r.subtype=8;r.channel=r.hw_channel=6;r.ssid_present=1;r.interval=100;r.rxv_valid=1;r.group_valid=4;r.rxv[4]=100;r.rxv[5]=90;r.ie_total=r.ie_bytes=3;r.ies[0]=1;r.ies[1]=1;r.ies[2]=2;
 scan.records[0].capability=1;scan.records[1]=scan.records[0];scan.records[1].bssid[5]=1;scan.records[1].capability=2;scan.records[2]=scan.records[0];scan.records[2].bssid[5]=2;scan.records[2].capability=3;scan.count=scan.frames=3;envelope(carrier,0x802869c8,10,5456);put(carrier+8,1);put(carrier+12,2);put(carrier+60,3);put(carrier+64,1);carrier[8+60]=110;put(carrier+8+4880,0x80000000u);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;assert(!call(c,0,a));assert(capturedRequest.dwell_tu==108&&!capturedRequest.count);finish();
 auto*rowmd=new IOBufferMemoryDescriptor(1164);envelope(carrier,NIW_GET,11,1164);a.structureOutput=(void*)1;a.structureOutputSize=0;a.structureOutputDescriptor=rowmd;a.structureOutputDescriptorSize=1164;
 rowmd->failWrite=true;assert(call(c,0,a)==kIOReturnBadArgument);rowmd->failWrite=false;rowmd->failComplete=true;assert(call(c,0,a)==kIOReturnNotReady);rowmd->failComplete=false;assert(!call(c,0,a));niws_result decoded;assert(!niws_decode_result((uint8_t*)rowmd->getBytesNoCopy(),1164,&decoded)&&decoded.rates[0]==1);a.structureOutputDescriptorSize=1164;assert(call(c,0,a)==(IOReturn)0xe0820446);rowmd->release();assert(!c->clientClose());c->release();assert(!rights&&!locks&&!malloc_count&&!buffer_count);
 retained_client();
 puts("PASS compatibility: real IOUC scalar4/ENXIO-no-association/idempotentON/no-fakeOFF/bounded OOL prepare-write-complete and failures/task/shape; old scan/cache/event/cancel/close tests retained");return 0;}
