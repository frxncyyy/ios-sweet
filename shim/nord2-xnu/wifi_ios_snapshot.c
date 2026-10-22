#include "wifi_ios_snapshot.h"
#include "wifi_ios_bss.h"
_Static_assert(NSB_IE_BYTES<=NIWS_MAX_IE,"retained IEs fit the 1164-byte result element room");
/* One record that cannot be encoded from observed data is omitted and counted;
 * it never fails the other rows of a completed scan. Off-channel receptions
 * are omitted: INC_OFFCHANNEL_BSS (0x8) is refused at admission. */
static unsigned row(const struct nsb_record*s,uint32_t age,const struct niwp_rate_map*m,size_t n,uint8_t*encoded){
 struct niwb_bss b;struct niws_result r={0};size_t rates=0;
 if(s->channel_mismatch)return NIWA_OMIT_CHANNEL;
 if(s->ies_truncated||s->ie_bytes!=s->ie_total)return NIWA_OMIT_IES;
 if(niwb_bss_metadata(s,&b))return NIWA_OMIT_OTHER;
 if(!b.rssi_valid)return NIWA_OMIT_RSSI;
 if(b.rate_count>NIWS_MAX_RATES||(b.rate_count&&niwp_rates(m,n,b.ie_rates,b.rate_count,r.rates,NIWS_MAX_RATES,&rates)))return NIWA_OMIT_RATES;
 r.channel=b.channel;r.channel_flags=b.band==1?8u:16u;r.age=age;r.rssi=b.rssi_dbm;
 r.beacon_interval=b.interval;r.capabilities=b.capabilities;r.rate_count=(uint8_t)rates;
 r.ssid_bytes=b.ssid_bytes;r.ie_bytes=b.ie_bytes;
 for(unsigned i=0;i<6;i++)r.bssid[i]=b.bssid[i];
 for(unsigned i=0;i<b.ssid_bytes;i++)r.ssid[i]=b.ssid[i];
 for(unsigned i=0;i<b.ie_bytes;i++)r.ie[i]=b.ies[i];
 return niws_encode_result(&r,encoded,NIWS_SCAN_RESULT_BYTES)?NIWA_OMIT_OTHER:0;
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
  if(ages!=s->count||(ages&&!age)||out->cursor.phase!=NIWC_NEW)return -1;
  if(s->count&&(!m||!n||n>128))return -1;
  for(size_t i=0;s->count&&i<n;i++){if(!m[i].ie_rate||m[i].ie_rate>127)return -1;
   for(size_t j=0;j<i;j++)if(m[j].ie_rate==m[i].ie_rate)return -1;
  }
 }
 out->count=out->omitted=out->omit_mask=0;
 out->entered=1;out->status=state;out->generation=generation;out->backend_count=s->count;out->dropped=s->dropped;
 out->completion_state=s->completion.state;out->completed_channels=s->completion.completed_channels;
 if(state!=NIWA_COMPLETE)return 1; /* never mark partial/timeout/pending cache READY */
 for(unsigned i=0;i<s->count;i++){unsigned why=row(&s->records[i],age[i],m,n,out->rows[out->count]);
  if(why){out->omitted++;out->omit_mask|=why;}else out->count++;} /* encoder writes nothing on refusal */
 if(niwc_begin(&out->cursor,generation)||niwc_complete(&out->cursor,generation,&out->rows[0][0],
    (size_t)out->count*NIWS_SCAN_RESULT_BYTES,out->count))return -1;
 return 0;
}
