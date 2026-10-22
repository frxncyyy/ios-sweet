/* Exact actual IOUC GET12 -> actual extracted native-owned inventory -> codec. */
int main(){assertNoJoins();IOUserClient*c=make();IOExternalMethodArguments a={};uint8_t raw[20];unsigned checks=0;
 auto reset=[&](unsigned extent=13,unsigned capacity=13){envelope(carrier,NIW_GET,12,extent);memset(raw,0xa5,sizeof raw);a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;a.structureOutput=raw;a.structureOutputSize=capacity;};
 auto unchanged=[&](){for(auto b:raw)assert(b==0xa5);};
 auto bits=[&](){assert(a.structureOutputSize==13);for(unsigned bit=0;bit<104;bit++){assert(((raw[bit/8]>>(bit%8))&1)==(bit==3||bit==14));checks++;}for(unsigned i=13;i<20;i++)assert(raw[i]==0xa5);};
 reset();unsigned before=card_queries;assert(!call(c,0,a)&&card_queries==before+1);bits();assert(!niwj_authorized(&js)&&!js.data_ready&&!js.ptk_ready&&!js.gtk_ready&&!js.associated);checks++;
 /* Installed-feature inventory must precede real authentication: no cycle. */
 js.phase=NJS_SECURING;reset();assert(!call(c,0,a));bits();assert(!niwj_authorized(&js));checks++;
 card_available=0;reset();assert(call(c,0,a)==kIOReturnNotReady);unchanged();checks++;
 card_available=1;nss_native.failed=1;reset();assert(call(c,0,a)==kIOReturnNotReady);unchanged();checks++;
 nss_native.failed=0;njs_native.notice_lock=nullptr;reset();assert(call(c,0,a)==kIOReturnNotReady);unchanged();checks++;njs_native.notice_lock=&card_notice;
 for(unsigned n=0;n<13;n++){reset(13,n);before=card_queries;assert(call(c,0,a)==kIOReturnBadArgument&&card_queries==before);unchanged();checks++;}
 for(unsigned n:{0u,12u,14u}){reset(n);before=card_queries;assert(call(c,0,a)==kIOReturnBadArgument&&card_queries==before);unchanged();checks++;}
 reset();test_current=&otherTask;before=card_queries;assert(call(c,0,a)==kIOReturnNotPrivileged&&card_queries==before);unchanged();test_current=&ownerTask;checks++;
 reset();put(carrier+15388,1);before=card_queries;assert(call(c,0,a)==kIOReturnBadArgument&&card_queries==before);unchanged();checks++;
 reset();memcpy(carrier+15368,"bad",4);before=card_queries;assert(call(c,0,a)==kIOReturnBadArgument&&card_queries==before);unchanged();checks++;
 /* Offline explicit empty inventory remains supported by unchanged pure codec. */
 reset();struct niwc_manifest empty={};size_t used=99;assert(!niwc_card_reply(carrier,sizeof carrier,&empty,raw,13,&used)&&used==13);for(unsigned i=0;i<13;i++)assert(!raw[i]);checks++;
 assertNoJoins();assert(!c->clientClose());c->release();assert(!rights&&!locks&&!malloc_count&&!buffer_count&&ownerTask.refs==1);
 printf("PASS actual production GET12/native-owned implementation inventory08/40/raw13, no key-readiness cycle, unavailable/failed/owner/shape/capacity atomic guards, offline empty codec retained; checks=%u\n",checks);return 0;
}
