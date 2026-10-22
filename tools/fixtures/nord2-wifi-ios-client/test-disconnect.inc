int main(){IOUserClient*c=make();IOExternalMethodArguments a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;unsigned checks=0;
 envelope(carrier,0x802869c8,22,8);put(carrier+8,0xaaaaaaaa);put(carrier+12,0x80000000u);assert(!call(c,0,a)&&jdisconnects==1&&!jenqueues);checks++;
 /* All other small extents refuse before native; no partial output. */
 for(unsigned size=1;size<=64;size++){if(size==8)continue;envelope(carrier,0x802869c8,22,size);unsigned n=jdisconnects;assert(call(c,0,a)==kIOReturnUnsupported&&jdisconnects==n);checks++;}
 /* Compiler-fill prefix is inactive; signed32 reason is opaque metadata. */
 for(uint32_t prefix:{0u,1u,0xaaaaaaaau,0xffffffffu,0x12345678u})for(uint32_t reason:{0u,1u,2u,0x7fffffffu,0x80000000u,0xffffffffu}){
  envelope(carrier,0x802869c8,22,8);put(carrier+8,prefix);put(carrier+12,reason);unsigned n=jdisconnects;assert(!call(c,0,a)&&jdisconnects==n+1&&!jenqueues&&!jactive);checks++;
 }
 envelope(carrier,0x802869c8,22,0);unsigned n=jdisconnects;assert(!call(c,0,a)&&jdisconnects==n+1);checks++;
 envelope(carrier,0x802869c8,22,8);n=jdisconnects;put(carrier+15388,1);assert(call(c,0,a)==kIOReturnBadArgument&&jdisconnects==n);checks++;put(carrier+15388,0);
 uint8_t output=0xa5;a.structureOutput=&output;a.structureOutputSize=1;assert(call(c,0,a)==kIOReturnBadArgument&&jdisconnects==n&&output==0xa5);checks++;a.structureOutput=nullptr;a.structureOutputSize=0;
 memcpy(carrier+15368,"bad",4);assert(call(c,0,a)!=0&&jdisconnects==n);checks++;envelope(carrier,0x802869c8,22,8);
 test_current=&otherTask;assert(call(c,0,a)==kIOReturnNotPrivileged&&jdisconnects==n);checks++;test_current=&ownerTask;
 a.structureInputSize=sizeof carrier-1;assert(call(c,0,a)==kIOReturnBadArgument&&jdisconnects==n);checks++;a.structureInputSize=sizeof carrier;
 /* Genuine OOL copied carrier has balanced prepare/complete; failures never
  * reach native. Payload is never kept by native leave API. */
 auto*md=new IOBufferMemoryDescriptor(sizeof carrier);memcpy(md->getBytesNoCopy(),carrier,sizeof carrier);a.structureInput=nullptr;a.structureInputSize=0;a.structureInputDescriptor=md;
 assert(!call(c,0,a)&&jdisconnects==n+1&&md->prepares==1&&md->completes==1&&!md->depth);n=jdisconnects;checks++;
 md->failPrepare=true;assert(call(c,0,a)==kIOReturnNotReady&&jdisconnects==n&&md->completes==1);md->failPrepare=false;checks++;
 md->failRead=true;assert(call(c,0,a)==kIOReturnBadArgument&&jdisconnects==n&&md->prepares==3&&md->completes==2&&!md->depth);md->failRead=false;checks++;
 md->failComplete=true;assert(call(c,0,a)==kIOReturnNotReady&&jdisconnects==n&&!md->depth);checks++;md->failComplete=false;md->release();
 assert(!c->clientClose());c->release();assert(!malloc_count&&!buffer_count&&!rights&&!locks&&ownerTask.refs==1);
 printf("PASS %u actual IOUC DISASSOC checks: exact0/8/opaque-prefix/signedreason/task/name/value/output/short/OOL faults; no join enqueue\n",checks);return 0;}
