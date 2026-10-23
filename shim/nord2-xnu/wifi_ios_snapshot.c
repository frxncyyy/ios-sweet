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
 r.channel=b.channel;r.channel_flags=(b.band==1?8u:16u)|((b.band==2&&b.channel>=52&&b.channel<=144)?0x100u:0u);r.age=age;r.rssi=b.rssi_dbm;
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
 out->retained_view=out->source_overflow=0;
 out->entered=1;out->status=state;out->generation=generation;out->backend_count=s->count;out->dropped=s->dropped;
 out->completion_state=s->completion.state;out->completed_channels=s->completion.completed_channels;
 if(state!=NIWA_COMPLETE)return 1; /* never mark partial/timeout/pending cache READY */
 for(unsigned i=0;i<s->count;i++){unsigned why=row(&s->records[i],age[i],m,n,out->rows[out->count]);
  if(why){out->omitted++;out->omit_mask|=why;}else out->count++;} /* encoder writes nothing on refusal */
 if(niwc_begin(&out->cursor,generation)||niwc_complete(&out->cursor,generation,&out->rows[0][0],
    (size_t)out->count*NIWS_SCAN_RESULT_BYTES,out->count))return -1;
 return 0;
}

int niwa_snapshot_bss_filter(struct niwa_snapshot*s,unsigned bss){
 if(!s||(bss!=2&&bss!=3)||!s->entered||s->status!=NIWA_COMPLETE||
 s->count>NSB_RESULTS||s->cursor.phase!=NIWC_READY||s->cursor.next||
 s->cursor.generation!=s->generation||s->cursor.count!=s->count||
 s->cursor.records!=&s->rows[0][0])return -1;
 if(bss==3)return 0;
 unsigned kept=0;
 for(unsigned i=0;i<s->count;i++){
  unsigned cap=s->rows[i][26]|(unsigned)s->rows[i][27]<<8;
  if((cap&3u)!=1u){s->omitted++;s->omit_mask|=NIWA_OMIT_BSS;continue;}
  if(kept!=i){for(unsigned j=0;j<NIWS_SCAN_RESULT_BYTES;j++)s->rows[kept][j]=s->rows[i][j];s->observed_us[kept]=s->observed_us[i];}
  kept++;
 }
 for(unsigned i=kept;i<s->count;i++){s->observed_us[i]=0;for(unsigned j=0;j<NIWS_SCAN_RESULT_BYTES;j++)s->rows[i][j]=0;}
 s->count=kept;s->cursor.count=kept;return 0;
}

_Static_assert(NSB_RESULTS==NIWC_MAX_RECORDS,"raw/display bounds must agree");
static void hist_copy(void*d,const void*s,size_t n){uint8_t*a=d;const uint8_t*b=s;while(n--)*a++=*b++;}
static void hist_zero(void*v,size_t n){uint8_t*p=v;while(n--)*p++=0;}
static void hist_put32(uint8_t*p,uint32_t n){for(unsigned j=0;j<4;j++)p[j]=(uint8_t)(n>>(8*j));}
static int hist_key(const uint8_t*a,const uint8_t*b){
 if(a[96]!=b[96])return 0;
 for(unsigned j=28;j<34;j++)if(a[j]!=b[j])return 0;
 for(unsigned j=0;j<a[96];j++)if(a[97+j]!=b[97+j])return 0;
 return 1;
}
void niwa_history_clear(struct niwa_history*h){if(h)hist_zero(h,sizeof(*h));}
static int hist_scan(const struct nsb_scan*s){
 /* Same exact overflow-only proof as niwj_scan_usable, without introducing
  * station-service dependencies or mutating any native completion metadata. */
 return s&&s->entered&&s->done&&!s->active&&!s->timed_out&&s->completion.state==7&&
  s->completion.sequence==s->sequence&&s->count<=NSB_RESULTS&&s->count<=s->frames&&
  s->dropped<=s->frames-s->count&&s->last>=s->started&&s->last<s->deadline&&s->deadline>s->started&&
  (s->partial?(s->dropped&&s->count==NSB_RESULTS):!s->dropped);
}
static void hist_remove(struct niwa_history*h,unsigned index){
 for(unsigned j=index+1;j<h->count;j++){
  h->observed_us[j-1]=h->observed_us[j];hist_copy(h->rows[j-1],h->rows[j],NIWS_SCAN_RESULT_BYTES);
 }
 --h->count;h->observed_us[h->count]=0;hist_zero(h->rows[h->count],NIWS_SCAN_RESULT_BYTES);
}
int niwa_history_prepare(struct niwa_history*h,const struct nsb_scan*s,uint64_t generation,
 uint64_t captured,uint64_t reference,const struct niwp_rate_map*m,size_t n,unsigned bss,struct niwa_snapshot*out){
 struct niws_result decoded;
 if(!h||!out||out->entered||out->cursor.phase!=NIWC_NEW||!generation||generation<=h->generation||
  h->count>NIWC_MAX_RECORDS||!hist_scan(s)||captured<s->last||reference<captured||
  reference<h->reference_us||(bss!=2&&bss!=3)||!m||!n||n>128)return -1;
 for(size_t j=0;j<n;j++){
  if(!m[j].ie_rate||m[j].ie_rate>127)return -1;
  for(size_t k=0;k<j;k++)if(m[j].ie_rate==m[k].ie_rate)return -1;
 }
 for(unsigned j=0;j<h->count;j++){
  if(h->observed_us[j]>h->reference_us||niws_decode_result(h->rows[j],NIWS_SCAN_RESULT_BYTES,&decoded))return -1;
  for(unsigned k=0;k<j;k++)if(hist_key(h->rows[j],h->rows[k]))return -1;
 }
 for(unsigned j=0;j<s->count;j++)if(s->records[j].observed_us<s->started||s->records[j].observed_us>s->last)return -1;
 /* Every fallible input check precedes mutation; no full scan/history local. */
 for(unsigned j=0;j<h->count;)if(reference-h->observed_us[j]>=NIWA_HISTORY_TTL_US)hist_remove(h,j);else j++;
 out->count=out->omitted=out->omit_mask=0;
 for(unsigned j=0;j<s->count;j++){
  uint8_t encoded[NIWS_SCAN_RESULT_BYTES];uint64_t observed=s->records[j].observed_us;
  if(reference-observed>=NIWA_HISTORY_TTL_US)continue;
  unsigned why=row(&s->records[j],(uint32_t)((reference-observed)/1000),m,n,encoded);
  if(why){out->omitted++;out->omit_mask|=why;continue;}
  unsigned at=h->count;
  for(unsigned k=0;k<h->count;k++)if(hist_key(h->rows[k],encoded)){at=k;break;}
  if(at<h->count){if(observed<h->observed_us[at])continue;}
  else if(h->count<NIWC_MAX_RECORDS)h->count++;
  else{
   at=0;for(unsigned k=1;k<h->count;k++)if(h->observed_us[k]<h->observed_us[at])at=k;
   if(observed<h->observed_us[at])continue;
  }
  hist_copy(h->rows[at],encoded,sizeof(encoded));h->observed_us[at]=observed;
 }
 h->generation=generation;h->reference_us=reference;
 for(unsigned j=0;j<h->count;j++){
  unsigned capabilities=h->rows[j][26]|(unsigned)h->rows[j][27]<<8;
  if(bss==2&&(capabilities&3u)!=1u){out->omitted++;out->omit_mask|=NIWA_OMIT_BSS;continue;}
  unsigned at=out->count++;hist_copy(out->rows[at],h->rows[j],NIWS_SCAN_RESULT_BYTES);out->observed_us[at]=h->observed_us[j];
  hist_put32(out->rows[at]+132,(uint32_t)((reference-h->observed_us[j])/1000));
 }
 out->entered=out->retained_view=1;out->status=NIWA_COMPLETE;out->generation=generation;out->reference_us=reference;
 out->backend_count=s->count;out->dropped=s->dropped;out->source_overflow=s->partial;
 out->completion_state=s->completion.state;out->completed_channels=s->completion.completed_channels;
 /* All rows were validated before retention or just encoded above; these calls
  * can only fail on an internal programming error, never grant native state. */
 if(niwc_begin(&out->cursor,generation)||niwc_complete(&out->cursor,generation,&out->rows[0][0],
    (size_t)out->count*NIWS_SCAN_RESULT_BYTES,out->count))return -1;
 return 0;
}
int niwa_snapshot_take_timed(struct niwa_snapshot*s,struct niwa_history*h,uint64_t generation,uint64_t reference,uint8_t*out,size_t bytes){
 if(!h||h->generation!=generation||reference<h->reference_us||!s||s->retained_view!=1||s->status!=NIWA_COMPLETE||!s->entered||s->generation!=generation||
  s->cursor.phase!=NIWC_READY||s->cursor.generation!=generation||s->cursor.records!=&s->rows[0][0]||
  s->cursor.count!=s->count||s->count>NIWC_MAX_RECORDS||s->cursor.next>s->count||reference<s->reference_us||
  !out||bytes!=NIWS_SCAN_RESULT_BYTES)return -1;
 for(unsigned j=s->cursor.next;j<s->count;j++)if(s->observed_us[j]>s->reference_us)return -1;
 while(s->cursor.next<s->count&&reference-s->observed_us[s->cursor.next]>=NIWA_HISTORY_TTL_US)s->cursor.next++;
 unsigned at=s->cursor.next;int result=niwc_take(&s->cursor,generation,out,bytes);
 if(result==1)hist_put32(out+132,(uint32_t)((reference-s->observed_us[at])/1000));
 if(result>=0){
  s->reference_us=reference;h->reference_us=reference;
  for(unsigned j=0;j<h->count;)if(reference-h->observed_us[j]>=NIWA_HISTORY_TTL_US)hist_remove(h,j);else j++;
 }
 return result;
}
