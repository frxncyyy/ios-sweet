#include "wifi_ios_snapshot.h"
#include "wifi_ios_policy.h"
#include "wifi_ios_join.h"
#include "wifi_ios_lifecycle.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static struct niwa_history h,saved;
static struct niwa_snapshot out,before;
static struct nsb_scan scan,seed;
static size_t maps;static const struct niwp_rate_map*map;static unsigned checks;
#define C(x) do{assert(x);checks++;}while(0)
static unsigned get(const uint8_t*p){return p[0]|(unsigned)p[1]<<8|(unsigned)p[2]<<16|(unsigned)p[3]<<24;}
static void record(struct nsb_record*r,unsigned id,unsigned channel,uint64_t observed){
 memset(r,0,sizeof(*r));r->bssid[0]=2;r->bssid[4]=(uint8_t)(id>>8);r->bssid[5]=(uint8_t)id;r->channel=r->hw_channel=(uint8_t)channel;
 r->ssid_present=1;r->ssid_len=4;memcpy(r->ssid,"test",4);r->subtype=8;r->interval=100;r->capability=1;r->observed_us=observed;
 r->rxv_valid=1;r->group_valid=4;r->rxv[4]=100;r->rxv[5]=255;r->ies[0]=1;r->ies[1]=1;r->ies[2]=12;r->ie_bytes=r->ie_total=3;
}
static void source(unsigned count,unsigned id,unsigned channel,uint64_t start){
 memset(&scan,0,sizeof scan);scan.entered=scan.done=1;scan.started=start;scan.last=start+100;scan.deadline=start+1000;scan.sequence=scan.completion.sequence=1;scan.completion.state=7;scan.count=scan.frames=count;
 for(unsigned j=0;j<count;j++)record(scan.records+j,id+j,channel,start+10+j);
}
static int merge(unsigned generation,uint64_t reference,unsigned bss){memset(&out,0,sizeof out);return niwa_history_prepare(&h,&scan,generation,scan.last+10,reference,map,maps,bss,&out);}
static void reject(unsigned generation,uint64_t captured,uint64_t reference,unsigned bss){
 saved=h;memset(&out,0,sizeof out);before=out;
 C(niwa_history_prepare(&h,&scan,generation,captured,reference,map,maps,bss,&out)<0);C(!memcmp(&h,&saved,sizeof h));C(!memcmp(&out,&before,sizeof out));
}
int main(void){
 C(NSB_RESULTS==64&&NIWC_MAX_RECORDS==64);map=niwi_rate_map(&maps);C(map&&maps);
 source(1,1,1,100);C(!merge(1,500,2)&&h.count==1&&out.count==1);
 source(1,2,100,1000);C(!merge(2,1500,2)&&h.count==2&&out.count==2);C(out.source_overflow==0&&out.completed_channels==0);
 C(get(out.rows[0]+8)==1&&get(out.rows[1]+8)==100);C(get(out.rows[1]+12)==0x110);C(h.observed_us[0]==110&&h.observed_us[1]==1010);
 uint8_t row[NIWS_SCAN_RESULT_BYTES],untouched[sizeof row];memset(row,0xa5,sizeof row);memcpy(untouched,row,sizeof row);
 unsigned next=out.cursor.next;C(niwa_snapshot_take_timed(&out,&h,2,1500,row,sizeof row-1)<0&&out.cursor.next==next&&!memcmp(row,untouched,sizeof row));
 C(niwa_snapshot_take_timed(&out,&h,1,1500,row,sizeof row)<0&&out.cursor.next==next);
 C(niwa_snapshot_take_timed(&out,&h,2,1499,row,sizeof row)<0&&out.cursor.next==next);
 C(niwa_snapshot_take_timed(&out,&h,2,5001500,row,sizeof row)==1&&get(row+132)==5001);C(h.observed_us[0]==110);
 C(niwa_snapshot_take_timed(&out,&h,2,5001500,row,sizeof row)==1&&get(row+132)==5000);
 C(niwa_snapshot_take_timed(&out,&h,2,5001500,row,sizeof row)==0);
 /* Full TTL is checked at read, including a cursor left unconsumed for30s. */
 source(0,0,1,6000000);C(!merge(3,6000500,3)&&out.count==2);
 C(niwa_snapshot_take_timed(&out,&h,3,NIWA_HISTORY_TTL_US+110,row,sizeof row)==1&&get(row+8)==100);C(get(row+132)==29999);C(niwa_snapshot_take_timed(&out,&h,3,NIWA_HISTORY_TTL_US+1010,row,sizeof row)==0);
 source(0,0,1,31000000);C(!merge(4,31000500,3)&&!out.count&&!h.count);
 source(1,3,36,32000000);C(!merge(5,32000500,2)&&out.count==1);source(1,3,36,33000000);C(!merge(6,33000500,2)&&out.count==1&&h.count==1&&h.observed_us[0]==33000010);
 /* Every terminal failure refuses atomically; old valid history unchanged. */
 seed=scan;
 for(unsigned bad=0;bad<17;bad++){
  scan=seed;if(bad==0)scan.entered=0;if(bad==1)scan.done=0;if(bad==2)scan.active=1;if(bad==3)scan.timed_out=1;
  if(bad==4)scan.completion.state=6;if(bad==5)scan.completion.sequence=2;if(bad==6)scan.count=65;if(bad==7)scan.frames=0;
  if(bad==8)scan.partial=1;if(bad==9)scan.dropped=1;if(bad==10)scan.last=scan.deadline;if(bad==11)scan.last=scan.started-1;
  if(bad==12)scan.records[0].observed_us=scan.last+1;if(bad==13)scan.records[0].observed_us=scan.started-1;
  reject(bad==14?6:7,scan.last+10,bad==15?1:33001000,bad==16?1:2);
 }
 scan=seed;reject(7,scan.last-1,33001000,2);
 /* Complete empty subsets keep still-fresh other-band rows, no timestamp reset. */
 source(0,0,1,34000000);C(!merge(7,34000500,2)&&out.count==1&&h.observed_us[0]==33000010);
 /* Bad per-row IEs are omissions, not an update of a previously good key. */
 source(1,3,36,35000000);scan.records[0].ie_total=1025;scan.records[0].ies_truncated=1;C(!merge(8,35000500,2)&&out.omitted==1&&h.observed_us[0]==33000010);
 /* Overflow-only genuine completion can supply bounded retained records. */
 niwa_history_clear(&h);source(64,10,100,100);scan.partial=scan.dropped=1;scan.frames=65;C(niwj_scan_usable(&scan));C(!merge(1,500,2)&&out.count==64&&out.source_overflow==1&&out.dropped==1);
 for(unsigned bad=0;bad<4;bad++){seed=scan;if(bad==0)scan.dropped=2;if(bad==1)scan.count=63;if(bad==2)scan.timed_out=1;if(bad==3)scan.done=0;C(!niwj_scan_usable(&scan));reject(2,scan.last+10,600,2);scan=seed;}
 /* Oldest entry drops on capacity, updates keep BSSID+SSID exactly distinct. */
 source(1,200,36,1000);C(!merge(2,1500,2)&&out.count==64&&h.count==64);unsigned found=0;for(unsigned j=0;j<h.count;j++)if(h.rows[j][33]==200)found++;C(found==1);
 source(1,200,36,2000);scan.records[0].ssid[0]='X';C(!merge(3,2500,3)&&h.count==64);found=0;for(unsigned j=0;j<h.count;j++)if(h.rows[j][33]==200)found++;C(found==2);
 source(1,201,36,3000);scan.records[0].capability=2;C(!merge(4,3500,2)&&out.omitted==1&&out.count==63);source(0,0,1,4000);C(!merge(5,4500,3)&&out.count==64);
 /* Existing before-publication BSS filtering keeps the timestamp paired with
  * its moved row, including a non-ESS entry in the middle. */
 C(!niwa_snapshot_bss_filter(&out,2)&&out.count==63);
 for(unsigned j=0;j<out.count;j++){
  unsigned found=0;for(unsigned k=0;k<h.count;k++)if(!memcmp(out.rows[j]+28,h.rows[k]+28,6)&&out.rows[j][96]==h.rows[k][96]&&!memcmp(out.rows[j]+97,h.rows[k]+97,out.rows[j][96])){C(out.observed_us[j]==h.observed_us[k]);found++;}
  C(found==1);
 }
 /* Close/OFF clear wipes every historical encoded byte and timestamp. */
 niwa_history_clear(&h);static const struct niwa_history empty={0};C(!memcmp(&h,&empty,sizeof h));
 /* Public APIs remain bounded and reject corrupted history before mutation. */
 source(1,9,1,1);h.count=65;reject(1,scan.last+10,500,2);niwa_history_clear(&h);
 printf("PASS retention: %u ASan/UBSan checks;64-row union, exact completed-overflow proof, per-read original age/30s expiry, failed-scan atomicity, BSSID+SSID and bounded oldest eviction; no native authority\n",checks);
}
