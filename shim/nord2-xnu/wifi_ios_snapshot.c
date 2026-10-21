#include "wifi_ios_snapshot.h"
#include "wifi_ios_bss.h"
static int row(const struct nsb_record*s,uint32_t age,const struct niwp_rate_map*m,size_t n,struct niws_result*out){
 struct niwb_bss b;struct niws_result r={0};size_t rates=0;
 if(niwb_bss_metadata(s,&b)||!b.rssi_valid||b.rate_count>NIWS_MAX_RATES)return -1;
 if(b.rate_count&&niwp_rates(m,n,b.ie_rates,b.rate_count,r.rates,NIWS_MAX_RATES,&rates))return -1;
 r.channel=b.channel;r.channel_flags=b.band==1?8u:16u;r.age=age;r.rssi=b.rssi_dbm;
 r.beacon_interval=b.interval;r.capabilities=b.capabilities;r.rate_count=(uint8_t)rates;
 r.ssid_bytes=b.ssid_bytes;r.ie_bytes=b.ie_bytes;
 for(unsigned i=0;i<6;i++)r.bssid[i]=b.bssid[i];
 for(unsigned i=0;i<b.ssid_bytes;i++)r.ssid[i]=b.ssid[i];
 for(unsigned i=0;i<b.ie_bytes;i++)r.ie[i]=b.ies[i];
 *out=r;return 0;
}
int niwa_snapshot_prepare(const struct nsb_scan*s,uint64_t generation,const uint32_t*age,size_t ages,
 const struct niwp_rate_map*m,size_t n,struct niwa_snapshot*out){
 if(!s||!out||out->entered||!generation||!s->entered||s->count>NSB_RESULTS||s->count>s->frames||
    s->last<s->started||s->deadline<=s->started||(s->active&&s->done)||(s->timed_out&&!s->partial))return -1;
 enum niwa_status state=NIWA_PENDING;
 if(s->timed_out)state=NIWA_TIMEOUT;
 else if(s->partial||s->dropped)state=NIWA_PARTIAL;
 else if(s->done){if(s->completion.sequence!=s->sequence||s->completion.state!=7)return -1;state=NIWA_COMPLETE;}
 if(state==NIWA_COMPLETE){
  if(ages!=s->count||(ages&&!age))return -1;
  for(unsigned i=0;i<s->count;i++){
   struct niws_result r;uint8_t encoded[NIWS_SCAN_RESULT_BYTES];
   if(row(&s->records[i],age[i],m,n,&r)||niws_encode_result(&r,encoded,sizeof encoded))return -1;
  }
 }
 out->entered=1;out->status=state;out->generation=generation;out->backend_count=s->count;out->dropped=s->dropped;
 out->completion_state=s->completion.state;out->completed_channels=s->completion.completed_channels;
 if(state!=NIWA_COMPLETE)return 1; /* never mark partial/timeout/pending cache READY */
 out->count=s->count;
 for(unsigned i=0;i<s->count;i++){
  struct niws_result r;/* Immutable input validated above; no backend callbacks. */
  if(row(&s->records[i],age[i],m,n,&r)||niws_encode_result(&r,out->rows[i],NIWS_SCAN_RESULT_BYTES))return -1;
 }
 if(niwc_begin(&out->cursor,generation)||niwc_complete(&out->cursor,generation,&out->rows[0][0],
    (size_t)out->count*NIWS_SCAN_RESULT_BYTES,out->count))return -1;
 return 0;
}
