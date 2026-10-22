#undef printf
static std::atomic<unsigned> checks;
#define CHECK(x) do{assert(x);checks++;}while(0)
struct Timing {uint64_t client;unsigned call,selector,request,status,ok,sample;uint64_t entered,locked,done,wait,work;};
static std::vector<Timing> pairs(){std::lock_guard<std::mutex>g(log_lock);std::vector<Timing> out;
 for(const auto&b:captured){unsigned long long id,entered,locked,done,wait,work;unsigned call,selector,request,status,ok,sample;int used=0;
  if(sscanf(b.c_str(),"NORD2 WIFI IOS QUERYBEGIN: client=%llu call=%u selector=%u entered_ms=%llu locked_ms=%llu%n",&id,&call,&selector,&entered,&locked,&used)!=5)continue;
  CHECK(b.substr(used)=="\n");unsigned matches=0;Timing row{};
  for(const auto&e:captured){unsigned long long eid;unsigned ec;int n=0;
   if(sscanf(e.c_str(),"NORD2 WIFI IOS QUERYEND: client=%llu call=%u request=%u status=%x done_ms=%llu wait_ms=%llu work_ms=%llu clock_ok=%u sample=%u%n",&eid,&ec,&request,&status,&done,&wait,&work,&ok,&sample,&n)==9&&eid==id&&ec==call){
    CHECK(e.substr(n)=="\n");matches++;row={id,call,selector,request,status,ok,sample,entered,locked,done,wait,work};}}
  CHECK(matches==1);out.push_back(row);
 }CHECK(out.size()*2==captured.size());return out;
}
static void clear_logs(){std::lock_guard<std::mutex>g(log_lock);captured.clear();}
static IOReturn query(IOUserClient*c,uint8_t*payload=nullptr,size_t outsize=64){
 uint8_t in[NIW_SMALL_BYTES],out[64];envelope(in,NIW_GET,43,0);if(payload)memcpy(in+100,payload,16);
 IOExternalMethodArguments a{};a.structureInput=in;a.structureInputSize=sizeof in;a.structureOutput=out;a.structureOutputSize=outsize;return call(c,0,a);
}
static void destroy(IOUserClient*c){CHECK(!c->clientClose());c->release();CHECK(ownerTask.refs==1&&!locks&&!malloc_count&&!buffer_count);}
int main(){
 CHECK(!niwt_relevant(11,kIOReturnSuccess));CHECK(niwt_relevant(11,(IOReturn)0xe0820446));CHECK(niwt_relevant(0,kIOReturnBadArgument));CHECK(niwt_relevant(51,0));
 auto*c=make();clear_logs();CHECK(!query(c));auto p=pairs();CHECK(p.size()==1&&p[0].client&&p[0].call==1&&p[0].request==43&&p[0].sample==1&&p[0].ok&&p[0].wait==0&&p[0].work==0);
 uint8_t marker[16];memset(marker,'Q',sizeof marker);clear_logs();CHECK(!query(c,marker));p=pairs();CHECK(p.size()==1&&p[0].call==2);for(auto&l:captured)CHECK(l.find("QQQQ")==std::string::npos);
 clear_logs();script_clock=true;script_pos=0;script_times[0]=2000;script_times[1]=1999;script_times[2]=2001;CHECK(!query(c));script_clock=false;p=pairs();CHECK(p.size()==1&&!p[0].ok&&!p[0].wait&&!p[0].work&&p[0].done==2001&&p[0].status==0);
 clear_logs();test_current=&otherTask;CHECK(query(c)==kIOReturnNotPrivileged);test_current=&ownerTask;p=pairs();CHECK(p.size()==1&&p[0].request==0&&p[0].status==(unsigned)kIOReturnNotPrivileged);destroy(c);
 /* Actual OOL read blocks while holding the real client's gate; second call
  * times that gate wait separately. Clock signal precedes lock acquisition. */
 c=make();clear_logs();auto*md=new IOBufferMemoryDescriptor(NIW_SMALL_BYTES);envelope((uint8_t*)md->getBytesNoCopy(),NIW_GET,43,0);md->blockRead=true;fake_ms=1000;
 std::thread first([&]{test_current=&ownerTask;uint8_t out[64];IOExternalMethodArguments a{};a.structureInputDescriptor=md;a.structureOutput=out;a.structureOutputSize=sizeof out;CHECK(!call(c,0,a));});
 while(!md->enteredRead)std::this_thread::yield();clock_entered=false;
 std::thread second([&]{test_current=&ownerTask;signal_clock=true;CHECK(!query(c));signal_clock=false;});
 while(!clock_entered)std::this_thread::yield();fake_ms=1120;md->releaseRead=true;first.join();second.join();p=pairs();CHECK(p.size()==2&&p[0].client==p[1].client&&p[0].call!=p[1].call);
 unsigned workCount=0,waitCount=0;for(auto&r:p){CHECK(r.ok&&r.sample==2&&r.request==43&&!r.status);workCount+=r.work==120&&r.wait==0;waitCount+=r.wait==120&&r.work==0;}CHECK(workCount==1&&waitCount==1);for(const auto&line:captured)std::fputs(line.c_str(),stdout);CHECK(md->prepares==1&&md->reads==1&&md->completes==1&&!md->depth);md->release();destroy(c);
 /* Normal boot queries exhaust only their normal pool; slow calls retain a
  * separate reserve. Completed slow samples stop exactly at16 without wrap. */
 c=make();clear_logs();fake_ms=3000;for(unsigned i=0;i<100;i++)CHECK(!query(c));p=pairs();CHECK(p.size()==64);clear_logs();
 for(unsigned i=0;i<20;i++){script_clock=true;script_pos=0;script_times[0]=3000;script_times[1]=3000;script_times[2]=3100;CHECK(!query(c));script_clock=false;}p=pairs();CHECK(p.size()==16);for(auto&r:p)CHECK(r.sample==2&&r.work==100&&r.wait==0);clear_logs();CHECK(!query(c));CHECK(captured.empty());destroy(c);
 /* Real simultaneous dispatches still return original results and produce
  * unique client/call tuples even if lines interleave after unlock. */
 c=make();clear_logs();fake_ms=5000;std::vector<std::thread>threads;for(unsigned i=0;i<8;i++)threads.emplace_back([&]{test_current=&ownerTask;for(unsigned j=0;j<16;j++)CHECK(!query(c));});for(auto&t:threads)t.join();p=pairs();CHECK(p.size()==64);std::set<unsigned>ids;for(auto&r:p)CHECK(ids.insert(r.call).second);destroy(c);
 /* Diagnostic IDs are atomic, bounded and never reused on saturation. */
 uint32_t counter=0;std::set<uint32_t> all;std::mutex m;threads.clear();for(unsigned i=0;i<8;i++)threads.emplace_back([&]{std::vector<uint32_t>local;for(unsigned j=0;j<1024;j++){auto id=niwt_call_id(&counter);if(id)local.push_back(id);}std::lock_guard<std::mutex>g(m);for(auto id:local)CHECK(all.insert(id).second);});for(auto&t:threads)t.join();CHECK(!all.empty()&&all.size()<=8192);CHECK(counter==all.size());
 counter=UINT32_MAX-1;CHECK(niwt_call_id(&counter)==UINT32_MAX);CHECK(!niwt_call_id(&counter)&&counter==UINT32_MAX);niwt_client_sequence=UINT64_MAX-1;CHECK(niwt_client_id()==UINT64_MAX);CHECK(!niwt_client_id()&&niwt_client_sequence==UINT64_MAX);
 c=make();clear_logs();CHECK(!query(c));CHECK(captured.empty());destroy(c);
 /* Actual source printf widths at maximum values, including raw error bits. */
 clear_logs();timing_printf(QUERYBEGIN_FORMAT,~0ull,~0u,~0u,~0ull,~0ull);
 timing_printf(QUERYEND_FORMAT,~0ull,~0u,~0u,~0u,~0ull,~0ull,~0ull,1u,2u);CHECK(captured[0].size()==154);CHECK(captured[1].size()==213);
 CHECK(!rights&&!malloc_count&&!buffer_count&&!locks&&ownerTask.refs==1);std::printf("PASS timing actual client: %u checks; gate/work contention, OOL balance, IDs/concurrency,64+16 limits, backward clock, no payload, max widths154/213\n",checks.load());
}
