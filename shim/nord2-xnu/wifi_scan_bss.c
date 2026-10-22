#include "wifi_scan_bss.h"
static uint16_t le16(const uint8_t *p){return (uint16_t)(p[0]|(uint16_t)p[1]<<8);}
static uint32_t le32(const uint8_t *p){return le16(p)|(uint32_t)le16(p+2)<<16;}
static void cp(void *d,const void *s,size_t n){uint8_t *a=d;const uint8_t *b=s;while(n--)*a++=*b++;}
static int eq(const void *a,const void *b,size_t n){const uint8_t *x=a,*y=b;while(n--)if(*x++!=*y++)return 0;return 1;}
static int mac_ok(const uint8_t *a){unsigned v=0;for(unsigned i=0;i<6;i++)v|=a[i];return v&&!(a[0]&1);}
int nsb_decode(const uint8_t *p,size_t n,uint64_t now,struct nsb_record *out)
{
 struct nsb_record r={0};size_t off=24,rxv=0,bytes,hdr,ie;uint32_t d1,d2;unsigned groups,seen_ds=0,seen_ht=0;
 if(!p||!out||n<24||n>3840)return -1;
 if((le16(p+2)&0x380f)!=0x3801)return -1;
 bytes=le16(p);d1=le32(p+4);d2=le32(p+8);groups=(d1>>11)&31;
 /* Restrictive unprotected, unfragmented management subset. No reorder,
  * decrypt, translated Ethernet or offloaded packet interpretation. */
 if((d1&0x0fff0000u)||
    (d2&((1u<<13)|(1u<<23)|(1u<<24)|(1u<<25)|(1u<<27)))||
    (le32(p+16)&(3u<<11)))return -1;
 if(groups&8)off+=16;
 if(groups&1)off+=16;
 if(groups&2)off+=8;
 if(groups&4){rxv=off;off+=8;}
 if(groups&16)off+=72;
 off+=((d2>>14)&3)*2;hdr=((d2>>8)&31)*2;
 if(bytes>n||off>bytes||hdr!=24||bytes-off<36)return -1;
 const uint8_t *m=p+off;uint16_t fc=le16(m);
 if((fc&0x00ff)!=0x80&&(fc&0x00ff)!=0x50)return -1;
 if((fc&0xc700)|| (le16(m+22)&15)||!mac_ok(m+16)||!mac_ok(m+10))return -1;
 cp(r.bssid,m+16,6);cp(r.transmitter,m+10,6);r.subtype=(uint8_t)((fc>>4)&15);
 r.interval=le16(m+32);r.capability=le16(m+34);r.privacy=(r.capability>>4)&1;
 r.hw_channel=(uint8_t)(le32(p+12)>>8);r.group_valid=(uint8_t)groups;r.observed_us=now;
 if(rxv){cp(r.rxv,p+rxv,8);r.rxv_valid=1;}
 ie=off+36;r.ie_total=(uint16_t)(bytes-ie);
 /* Validate all IEs, including bytes beyond the retained snapshot. */
 for(size_t pos=ie;pos<bytes;){
  if(bytes-pos<2)return -1;
  unsigned id=p[pos],len=p[pos+1];pos+=2;if(len>bytes-pos)return -1;
  if(id==0){
   if(r.ssid_present||len>32)return -1;
   r.ssid_present=1;r.ssid_len=(uint8_t)len;cp(r.ssid,p+pos,len);
   unsigned nonzero=0;for(unsigned k=0;k<len;k++)nonzero|=p[pos+k];r.hidden=!nonzero;
  }else if(id==3){if(seen_ds++||len!=1)return -1;r.ds_channel=p[pos];}
  else if(id==61){if(seen_ht++||len!=22)return -1;r.ht_channel=p[pos];}
  else if(id==48)r.rsn_present=1;
  else if(id==221&&len>=4&&p[pos]==0&&p[pos+1]==0x50&&p[pos+2]==0xf2&&p[pos+3]==1)r.wpa_present=1;
  pos+=len;
 }
 r.channel=r.ds_channel?r.ds_channel:r.ht_channel?r.ht_channel:r.hw_channel;
 r.channel_mismatch=(r.ds_channel&&r.ht_channel&&r.ds_channel!=r.ht_channel)||
  (r.hw_channel&&r.channel&&r.hw_channel!=r.channel);
 r.ie_bytes=r.ie_total>NSB_IE_BYTES?NSB_IE_BYTES:r.ie_total;
 r.ies_truncated=r.ie_total>NSB_IE_BYTES;cp(r.ies,p+ie,r.ie_bytes);
 *out=r;return 0;
}
int nsb_decode_done(const uint8_t *p,size_t n,struct nsb_done *out)
{
 struct nsb_done d={0};if(!p||!out||n<NSB_SCAN_DONE_BYTES||n>3840)return -1;
 /* Body only after normal WIFI_EVENT EID0d validation; sequence here is
  * the scan payload sequence, not the command-header sequence. */
 d.sequence=p[0];d.sparse_valid=p[1];d.sparse_band=p[2];d.sparse_channel=p[3];
 d.completed_channels=p[4];d.state=p[5];d.version=p[6];d.beacons=le32(p+8);d.pno=p[12];
 d.channel_count=p[16];if(d.channel_count>64)return -1;
 for(unsigned i=0;i<64;i++){d.channels[i]=p[20+i];d.idle[i]=le16(p+84+2*i);
  d.beacon_probe_counts[i]=p[212+i];d.mdrdy_counts[i]=p[276+i];d.scan_time[i]=le16(p+348+2*i);}
 d.beacons_2g=le32(p+340);d.beacons_5g=le32(p+344);*out=d;return 0;
}
int nsb_begin(struct nsb_scan *s,uint8_t seq,uint64_t now,uint64_t timeout)
{
 if(!s||s->entered||!timeout||timeout>UINT64_MAX-now)return -1;
 s->entered=s->active=1;s->sequence=seq;s->started=s->last=now;s->deadline=now+timeout;return 0;
}
int nsb_expire(struct nsb_scan *s,uint64_t now)
{
 if(!s||!s->entered||!s->active)return -1;
 if(now<s->last){s->active=0;s->partial=1;return -1;}s->last=now;
 if(now>=s->deadline){s->active=0;s->timed_out=s->partial=1;return 1;}return 0;
}
int nsb_add(struct nsb_scan *s,const uint8_t *p,size_t n,uint64_t now)
{
 struct nsb_record r;if(nsb_expire(s,now)!=0)return -1;
 if(nsb_decode(p,n,now,&r))return -1;
 s->frames++;
 for(unsigned i=0;i<s->count;i++)if(eq(s->records[i].bssid,r.bssid,6)&&
   s->records[i].ssid_present==r.ssid_present&&s->records[i].ssid_len==r.ssid_len&&eq(s->records[i].ssid,r.ssid,r.ssid_len)){
  /* Do not replace a coherent on-channel observation with adjacent-channel
   * leakage. Preserve that observation's RSSI/IEs/timestamp as one real row. */
  if(!r.channel_mismatch||s->records[i].channel_mismatch)s->records[i]=r;
  return 0;
 }
 if(s->count==NSB_RESULTS){s->dropped++;s->partial=1;return 1;}
 s->records[s->count++]=r;return 0;
}
int nsb_complete(struct nsb_scan *s,const uint8_t *p,size_t n,uint64_t now)
{
 struct nsb_done d;if(nsb_expire(s,now)!=0||nsb_decode_done(p,n,&d))return -1;
 if(d.sequence!=s->sequence)return 1;
 s->completion=d;s->active=0;s->done=1;if(d.state!=7)s->partial=1;return 0;
}

int nsb_complete_event(struct nsb_scan *s,const uint8_t *p,size_t n,uint64_t now)
{
 if(!p||n<36+NSB_SCAN_DONE_BYTES||n>3840||
    (le16(p+2)&0x380f)!=0x3800||p[28]!=0x0d)return -1;
 int rc=nsb_complete(s,p+36,n-36,now);
 if(rc==0){s->completion.rxd_bytes=le16(p);s->completion.event_bytes=le16(p+24);s->completion.event_sequence=p[29];}
 return rc;
}
