#include "wifi_ios_lifecycle.h"
int nils_open(struct nils_state*s,uint64_t generation){
 if(!s||s->open||s->inflight||!generation||generation<=s->generation)return -1;
 s->generation=generation;s->open=1;s->cache=0;return 0;
}
int nils_begin(struct nils_state*s,struct nils_ticket*t,struct niwa_snapshot*out){
 if(!s||!t||!out||out->entered||!s->open||s->inflight||s->serial==UINT64_MAX)return -1;
 s->cache=0;s->pending=out;++s->serial;s->inflight=1;t->generation=s->generation;t->serial=s->serial;t->output=out;return 0;
}
int nils_prepare(const struct nils_ticket*t,const struct nsb_scan*scan,uint64_t captured,uint64_t reference,
 const struct niwp_rate_map*m,size_t n,struct niwa_snapshot*out){
 if(!t||!t->generation||!t->serial||!scan||!out||out!=t->output||out->entered)return -1;
 if(scan->done&&!scan->partial&&!scan->timed_out&&!scan->dropped)
  return niwaa_snapshot_prepare(scan,t->generation,captured,reference,m,n,out);
 return niwa_snapshot_prepare(scan,t->generation,0,0,m,n,out);
}
int nils_finish(struct nils_state*s,const struct nils_ticket*t,struct niwa_snapshot*out){
 if(!s||!t||!s->inflight||t->serial!=s->serial||t->generation!=s->generation||t->output!=s->pending||(out&&out!=s->pending))return -1;
 s->inflight=0;s->pending=0;
 if(!s->open)return 2;
 if(!out||!out->entered||out->generation!=t->generation)return -1;
 if(out->status!=NIWA_COMPLETE)return 1;
 s->cache=out;return 0;
}
int nils_take(struct nils_state*s,uint64_t generation,uint8_t*out,size_t bytes){
 if(!s||!s->open||s->inflight||!s->cache||s->generation!=generation)return -1;
 return niwc_take(&s->cache->cursor,generation,out,bytes);
}
void nils_close(struct nils_state*s){if(s){s->open=0;s->cache=0;}}

int nils_prepare_history(const struct nils_ticket*t,struct niwa_history*h,const struct nsb_scan*scan,
 uint64_t captured,uint64_t reference,const struct niwp_rate_map*m,size_t n,unsigned bss,struct niwa_snapshot*out){
 if(!t||!t->generation||!t->serial||!scan||!out||out!=t->output||out->entered)return -1;
 return niwa_history_prepare(h,scan,t->generation,captured,reference,m,n,bss,out);
}
int nils_take_timed(struct nils_state*s,struct niwa_history*h,uint64_t generation,uint64_t reference,uint8_t*out,size_t bytes){
 if(!s||!s->open||s->inflight||!s->cache||s->generation!=generation)return -1;
 return niwa_snapshot_take_timed(s->cache,h,generation,reference,out,bytes);
}
