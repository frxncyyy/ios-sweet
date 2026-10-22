int main(){
 std::mutex seenLock;std::vector<uint64_t>seen;std::vector<std::thread>threads;
 for(unsigned t=0;t<16;t++)threads.emplace_back([&]{for(unsigned k=0;k<256;k++){uint64_t id=niwj_trace_id();if(id){std::lock_guard<std::mutex>guard(seenLock);seen.push_back(id);}}});
 for(auto&t:threads)t.join();std::sort(seen.begin(),seen.end());assert(!seen.empty());for(size_t i=1;i<seen.size();i++)assert(seen[i]!=seen[i-1]);
 uint64_t last=__atomic_load_n(&niwj_trace_sequence,__ATOMIC_RELAXED);
 __atomic_store_n(&niwj_trace_sequence,UINT64_MAX-1,__ATOMIC_RELAXED);assert(niwj_trace_id()==UINT64_MAX&&!niwj_trace_id()&&!niwj_trace_id());__atomic_store_n(&niwj_trace_sequence,last,__ATOMIC_RELAXED);
IOMemoryDescriptor*d,*s;IOUserClient*c=armed(&d,&s);IOExternalMethodArguments a={};auto setup=[&]{joinWire();auto*p=carrier+8;p[464]=72;p[508]=2;put(p+536,36);memset(p+540,'x',36);p[576]=0;a={};a.structureInput=carrier;a.structureInputSize=sizeof carrier;};
 setup();ownerUnavailable=1;assert(call(c,0,a)==kIOReturnNotReady&&!jactive&&!jenqueues);ownerUnavailable=0;
 setup();carrier[8+509]=1;assert(call(c,0,a)==kIOReturnUnsupported&&!jactive&&!jenqueues);
 setup();carrier[8+575]=0;assert(call(c,0,a)==kIOReturnUnsupported&&!jactive&&!jenqueues);
 setup();__atomic_store_n(&niwj_trace_sequence,UINT64_MAX,__ATOMIC_RELAXED);assert(!call(c,0,a)&&jactive&&jenqueues==1&&copiedJoin.native_flags==4&&copiedJoin.pmk[31]==31);
 assert(!c->clientClose());c->release();joinNotice(NJS_RETIRED);d->release();s->release();assert(!malloc_count&&!buffer_count&&!rights&&ownerTask.refs==1);
 puts("PASS actual IOUC envelope: unavailable/mismatched owner and malformed scope refuse; exact retained identity enqueues once; close retires owned credential copy");return 0;}
