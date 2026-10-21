#include "wifi_ios_age.h"
int niwaa_age_plan(const struct nsb_scan*s,uint64_t captured,uint64_t reference,struct niwaa_ages*out){
 struct niwaa_ages a={0};
 if(!s||!out||!s->entered||!s->done||s->active||s->timed_out||s->partial||s->dropped||
    s->count>NSB_RESULTS||s->count>s->frames||s->completion.state!=7||s->completion.sequence!=s->sequence||
    s->last<s->started||s->deadline<=s->last||captured<s->last||reference<captured)return -1;
 a.captured_us=captured;a.reference_us=reference;a.count=s->count;
 for(unsigned i=0;i<s->count;i++){
  uint64_t observed=s->records[i].observed_us;
  if(observed<s->started||observed>s->last)return -1;
  uint64_t milliseconds=(reference-observed)/1000u;
  if(milliseconds>INT32_MAX)return -1;
  a.milliseconds[i]=(uint32_t)milliseconds;
 }
 *out=a;return 0;
}
int niwaa_snapshot_prepare(const struct nsb_scan*s,uint64_t generation,uint64_t captured,uint64_t reference,
 const struct niwp_rate_map*m,size_t n,struct niwa_snapshot*out){
 struct niwaa_ages a;
 if(niwaa_age_plan(s,captured,reference,&a))return -1;
 return niwa_snapshot_prepare(s,generation,a.milliseconds,a.count,m,n,out);
}
