#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "wifi_ios_age.h"
#include "wifi_ios_rates.h"
/* Synthetic RXD(group4 RXV)+beacon frames through the canonical decoder, dedup,
 * metadata, production rate map, age plan and 1164-byte encoder. Identities
 * are locally administered placeholders; no device frame is used. */
static unsigned checks;static uint8_t buf[3840];static uint64_t now;
static void put32(uint8_t*p,uint32_t v){for(unsigned i=0;i<4;i++)p[i]=(uint8_t)(v>>(8*i));}
static size_t ie(uint8_t*p,unsigned id,const uint8_t*v,size_t n){p[0]=(uint8_t)id;p[1]=(uint8_t)n;if(n)memcpy(p+2,v,n);return n+2;}
static size_t pad(uint8_t*p,size_t bytes){static const uint8_t v[250]={0x00,0x10,0x18};size_t used=0;
 while(used<bytes){size_t left=bytes-used,n=left>252?250:left-2;if(left==253)n=248;used+=ie(p+used,221,v,n);}return used;}
/* HT-required 0xff and SAE-H2E-only 0xfb selectors sit in the rate IEs. */
static size_t frame(unsigned tuned,unsigned ds,uint8_t id,int rxv,size_t extra){
 static const uint8_t ssid[3]={'n','e','t'},rates[5]={0x82,0x84,0x8b,0x96,0xff},ext[9]={0x0c,0x12,0x18,0x24,0x30,0x48,0x60,0x6c,0xfb};
 size_t m=rxv?32:24,at=m+36;uint8_t channel=(uint8_t)ds;memset(buf,0,sizeof buf);
 buf[2]=0x01;buf[3]=0x38;put32(buf+4,rxv?4u<<11:0);put32(buf+8,12u<<8);buf[13]=(uint8_t)tuned;
 if(rxv){buf[28]=100;buf[29]=90;}
 buf[m]=0x80;buf[m+10]=2;buf[m+15]=id;buf[m+16]=2;buf[m+21]=id;buf[m+32]=100;buf[m+34]=1;
 at+=ie(buf+at,0,ssid,3);at+=ie(buf+at,1,rates,5);at+=ie(buf+at,3,&channel,1);at+=ie(buf+at,50,ext,9);
 if(extra)at+=pad(buf+at,extra);
 buf[0]=(uint8_t)at;buf[1]=(uint8_t)(at>>8);return at;
}
static void add(struct nsb_scan*s,unsigned tuned,unsigned ds,uint8_t id,int rxv,size_t extra){size_t n=frame(tuned,ds,id,rxv,extra);now+=10;assert(!nsb_add(s,buf,n,now));checks++;}
static void finish(struct nsb_scan*s){s->active=0;s->done=1;s->completion.sequence=s->sequence;s->completion.state=7;}
int main(void){static struct nsb_scan scan;static struct niwa_snapshot snap;uint8_t out[NIWS_SCAN_RESULT_BYTES];size_t mapn=0;
 const struct niwp_rate_map*map=niwr_production_map(&mapn);
 now=1000;assert(!nsb_begin(&scan,1,now,1000000));
 add(&scan,6,6,1,1,0);                          /* A on-channel, selectors in both rate IEs */
 add(&scan,1,1,2,1,0);uint64_t first=now;        /* B on-channel */
 add(&scan,2,1,2,1,0);                          /* B again through adjacent-channel leakage */
 assert(scan.count==2&&!scan.records[1].channel_mismatch&&scan.records[1].observed_us==first);checks++;
 add(&scan,1,1,2,1,0);assert(scan.records[1].observed_us==now);checks++; /* newer on-channel copy replaces */
 add(&scan,5,6,3,1,0);                          /* C heard only off its channel */
 add(&scan,11,11,4,1,674);                      /* D 700-byte IEs: beyond old 512 retention */
 add(&scan,11,11,5,1,1074);                     /* E 1100-byte IEs: beyond the iOS 1024 room */
 add(&scan,3,3,6,0,0);                          /* F no RX vector: no observed RSSI */
 assert(scan.count==6&&scan.frames==8&&scan.records[3].ie_total==700&&!scan.records[3].ies_truncated&&scan.records[4].ies_truncated);checks++;
 finish(&scan);assert(!niwaa_snapshot_prepare(&scan,1,now,now,map,mapn,&snap));
 assert(snap.status==NIWA_COMPLETE&&snap.backend_count==6&&snap.count==3&&snap.omitted==3&&
   snap.omit_mask==(NIWA_OMIT_CHANNEL|NIWA_OMIT_IES|NIWA_OMIT_RSSI));checks++;
 static const unsigned channel[3]={6,1,11},ies[3]={26,26,700};
 for(unsigned i=0;i<3;i++){struct niws_result r;assert(!niws_decode_result(snap.rows[i],sizeof snap.rows[i],&r));
  assert(r.channel==channel[i]&&r.channel_flags==8&&r.ie_bytes==ies[i]&&r.rssi==-60&&r.rate_count==12&&
   r.rates[0]==1&&r.rates[2]==5&&r.rates[4]==6&&r.rates[11]==54);checks++;}
 for(unsigned i=0;i<3;i++){assert(niwc_take(&snap.cursor,1,out,sizeof out)==1&&!memcmp(out,snap.rows[i],sizeof out));checks++;}
 assert(!niwc_take(&snap.cursor,1,out,sizeof out));checks++;
 /* A completed scan with nothing encodable is a real empty list, not failure. */
 memset(&scan,0,sizeof scan);memset(&snap,0,sizeof snap);now=5000;assert(!nsb_begin(&scan,2,now,1000000));
 add(&scan,5,6,7,1,0);finish(&scan);assert(!niwaa_snapshot_prepare(&scan,2,now,now,map,mapn,&snap));
 assert(snap.status==NIWA_COMPLETE&&!snap.count&&snap.omitted==1&&snap.omit_mask==NIWA_OMIT_CHANNEL&&!niwc_take(&snap.cursor,2,out,sizeof out));checks++;
 /* Partial/timeout scans still never become a ready cache. */
 memset(&scan,0,sizeof scan);memset(&snap,0,sizeof snap);now=9000;assert(!nsb_begin(&scan,3,now,1000000));
 add(&scan,6,6,8,1,0);scan.active=0;scan.partial=1;assert(niwa_snapshot_prepare(&scan,3,0,0,map,mapn,&snap)==1&&snap.status==NIWA_PARTIAL&&!snap.count);checks++;
 /* Known selectors are not rates; every other unknown value stays unknown.
  * The actual RX decoder still retains every original IE byte unchanged. */
 const unsigned legacy[]={2,4,11,22,12,18,24,36,48,72,96,108};
 for(unsigned raw=0;raw<256;raw++){
  memset(&scan,0,sizeof scan);memset(&snap,0,sizeof snap);now=10000+raw*10;assert(!nsb_begin(&scan,4,now,1000000));
  size_t bytes=frame(6,6,9,1,0);buf[32+36+5+2]=(uint8_t)raw;now++;
  assert(!nsb_add(&scan,buf,bytes,now));finish(&scan);assert(!niwaa_snapshot_prepare(&scan,4,now,now,map,mapn,&snap));
  unsigned known=(raw==255||raw==254||raw==251);for(unsigned j=0;j<12;j++)if((raw&127)==legacy[j])known=1;
  assert(snap.count==known&&snap.omitted==!known);checks++;
  if(known){struct niws_result r;assert(!niws_decode_result(snap.rows[0],1164,&r)&&r.ie_bytes==scan.records[0].ie_bytes&&!memcmp(r.ie,scan.records[0].ies,r.ie_bytes));checks++;}
 }
 /* Exact retention boundary, no truncation concealed as a valid IE list. */
 for(unsigned total=1024;total<=1025;total++){
  memset(&scan,0,sizeof scan);memset(&snap,0,sizeof snap);now=20000;assert(!nsb_begin(&scan,5,now,1000000));add(&scan,6,6,10,1,total-26);finish(&scan);
  assert(scan.records[0].ie_total==total&&scan.records[0].ie_bytes==1024&&scan.records[0].ies_truncated==(total>1024));
  assert(!niwaa_snapshot_prepare(&scan,5,now,now,map,mapn,&snap)&&snap.count==(total==1024)&&snap.omitted==(total>1024));checks++;
  if(snap.count){struct niws_result r;assert(!niws_decode_result(snap.rows[0],1164,&r)&&r.ie_bytes==1024&&!memcmp(r.ie,scan.records[0].ies,1024));checks++;}
 }
 /* A malformed IE after byte1024 is still rejected by the real full decoder. */
 memset(&scan,0,sizeof scan);now=30000;assert(!nsb_begin(&scan,6,now,1000000));size_t bytes=frame(6,6,11,1,1074);buf[bytes-1]=1;buf[bytes-2]=221;/* replace last data bytes with opaque data, not headers */
 /* The last element starts at offset32+36+26+1008; deliberately overrun it. */
 buf[32+36+26+1008+1]=255;assert(nsb_add(&scan,buf,bytes,now+1)<0&&!scan.frames&&!scan.count);checks++;
 /* Off-channel-first can be replaced by a genuine on-channel observation. */
 memset(&scan,0,sizeof scan);now=40000;assert(!nsb_begin(&scan,7,now,1000000));add(&scan,5,6,12,1,0);add(&scan,6,6,12,1,0);assert(scan.count==1&&!scan.records[0].channel_mismatch&&scan.records[0].observed_us==now);checks++;
 finish(&scan);
 /* Global contract/map/cursor errors refuse before mutating the output. */
 static struct niwa_snapshot before;uint32_t ages[1]={0};struct niwp_rate_map dup[2]={map[0],map[0]};
 for(unsigned bad=0;bad<5;bad++){memset(&snap,0,sizeof snap);if(bad==4)snap.cursor.phase=NIWC_READY;before=snap;
  const struct niwp_rate_map*m=bad==0?0:bad==1?dup:map;size_t n=bad==1?2:bad==2?129:mapn;size_t count=bad==3?0:1;
  assert(niwa_snapshot_prepare(&scan,7,ages,count,m,n,&snap)<0&&!memcmp(&before,&snap,sizeof snap));checks++;}
 /* Counter garbage cannot index outside bounded rows of a never-entered output. */
 memset(&snap,0,sizeof snap);snap.count=~0u;snap.omitted=~0u;snap.omit_mask=~0u;
 assert(!niwa_snapshot_prepare(&scan,7,ages,1,map,mapn,&snap)&&snap.count==1&&!snap.omitted&&!snap.omit_mask);checks++;
 /* Truly empty completed scan remains distinguishable from omitted rows. */
 memset(&scan,0,sizeof scan);memset(&snap,0,sizeof snap);now=50000;assert(!nsb_begin(&scan,8,now,1000000));finish(&scan);assert(!niwaa_snapshot_prepare(&scan,8,now,now,map,mapn,&snap)&&snap.status==NIWA_COMPLETE&&!snap.count&&!snap.omitted&&!snap.backend_count);checks++;
 memset(&snap,0,sizeof snap);assert(!niwa_snapshot_prepare(&scan,8,0,0,0,0,&snap)&&snap.status==NIWA_COMPLETE&&!snap.count&&!snap.omitted);checks++;
 /* Pending and timed-out inputs do not become READY or get omission counts. */
 memset(&scan,0,sizeof scan);memset(&snap,0,sizeof snap);now=60000;assert(!nsb_begin(&scan,9,now,100));
 assert(niwa_snapshot_prepare(&scan,9,0,0,0,0,&snap)==1&&snap.status==NIWA_PENDING&&snap.cursor.phase!=NIWC_READY&&!snap.count);checks++;
 assert(nsb_expire(&scan,now+100)==1);memset(&snap,0,sizeof snap);assert(niwa_snapshot_prepare(&scan,9,0,0,0,0,&snap)==1&&snap.status==NIWA_TIMEOUT&&snap.cursor.phase!=NIWC_READY&&!snap.count&&!snap.omitted);checks++;
 printf("PASS %u snapshot checks: selectors not rates, on-channel dedup, per-row omission, 1024B IEs, empty-complete, partial-not-ready; synthetic only\n",checks);return 0;}
