/* Actual binder/pure transport, real NST/NJS structs. Native context/locks,
 * nonce/clock and disconnect API are typed host doubles; no radio/device. */
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "wifi_station_transport.h"
#include "wifi_station_service.h"
#include "wifi_live_control.c"
#include "packets.h"
static unsigned checks,depth,locks,disconnect_calls,disconnect_result,available=1,option=1,rejoin_ready,ready_calls;
#define C(x) do{checks++;assert(x);}while(0)
static uintptr_t thread=7;
static uintptr_t current_thread(void){return thread;}
static struct{uintptr_t worker;}ncd_native;
static struct{void*lock;unsigned failed;}nss_native;
static struct{struct nst state;}nst_native;
static struct{struct njs_snapshot snapshot;uint64_t native_generation,serial;void*notice_lock,*completion;unsigned queued,active,linked,used,retiring,leave,canceled,tx_count,rx_count,tx_accepted,tx_local,rx_delivered;}njs_native;
static int nss_available(void){return available;}
static void IOLockLock(void*p){C(p==nss_native.lock&&depth==0);depth=1;locks++;}
static void IOLockUnlock(void*p){C(p==nss_native.lock&&depth==1);depth=0;}
static int PE_parse_boot_argn(const char*n,void*out,size_t z){C(!strcmp(n,"nord2wifilive")&&z==sizeof(unsigned));memcpy(out,&option,z);return 1;}
static void read_random(void*out,size_t z){C(z==8);uint64_t n=123456;memcpy(out,&n,z);}
static uint64_t njs_uptime_us(void){return UINT64_C(123456789000);}
int nord2_wifi_join_disconnect(void){C(!depth);disconnect_calls++;if(!disconnect_result){njs_native.leave=1;nst_native.state.controlled_port=0;}return disconnect_result;}
static int njs_rejoin_ready_locked(void){C(depth==1);ready_calls++;return rejoin_ready;}
#include "wifi_live_control_bind.inc"
static uint32_t rd(const uint8_t*p){return p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static unsigned token;
static void fresh(void){memset(&live,0,sizeof live);memset(&nst_native,0,sizeof nst_native);memset(&njs_native,0,sizeof njs_native);memset(&nwl_native,0,sizeof nwl_native);ncd_native.worker=thread=7;nss_native.lock=&locks;nss_native.failed=0;available=1;option=1;depth=locks=disconnect_calls=disconnect_result=token=ready_calls=rejoin_ready=0;njs_native.native_generation=1;njs_native.active=1;nst_native.state.prepared=1;}
static unsigned command(unsigned op,unsigned gen,struct nwl_snapshot*out){C(nwl_submit((op<<12)|++token,gen));nwl_native_work();uint8_t b[256];C(nwl_reply(b,sizeof b)==256);for(unsigned k=0;k<64;k++)out->word[k]=rd(b+4*k);C(out->word[5]==token&&out->word[6]==op&&out->word[7]==gen);return out->word[8];}
static void counters(struct nsdd*d){d->tx_data=17;d->rx_group=18;d->tx_dhcp=3;d->headers_rx=4;d->dhcp_rx=12;d->records=55;d->summary=0;d->rx_error_logs=8;d->live_tx=64;d->live_rx=64;d->live_epoch=4;d->live_mode=2;}
static void preserved(struct nst before){struct nst after=nst_native.state;before.data_diag.live_mode=after.data_diag.live_mode;before.data_diag.live_epoch=after.data_diag.live_epoch;before.data_diag.live_tx=after.data_diag.live_tx;before.data_diag.live_rx=after.data_diag.live_rx;C(!memcmp(&before,&after,sizeof before));}
int main(void){struct nwl_snapshot out;fresh();option=0;nwl_native_start();C(!nwl_enabled());option=1;thread=8;nwl_native_start();C(!nwl_enabled());thread=7;nwl_native_start();C(nwl_enabled());
 struct nst*s=&nst_native.state;s->submitted=1;s->dma_completed=1;s->normal.completed=1;s->deadline=30000000;s->operation_deadline=1000000;s->grant_deadline=50000;s->ptk.installed=1;s->ptk.pn[3]=123;s->gtk_keys[2].installed=1;s->gtk_keys[2].pn[7]=456;memset(s->wire,0x5a,sizeof s->wire);counters(&s->data_diag);struct nst before=*s;unsigned origlocks=locks;
 C(command(NWL_STATUS,0,&out)==NWL_OK);C(!memcmp(&before,s,sizeof before));C(out.word[NWL_F_COMPLETED]==1&&out.word[NWL_F_SUBMITTED]==1);C(locks==origlocks+2);C(out.word[NWL_F_TIME_LO]==123456789);for(unsigned k=NWL_F_END;k<64;k++)C(!out.word[k]);
 /* JOIN_READY describes native admission readiness, not association/IP. */
 njs_native.active=0;C(command(NWL_STATUS,0,&out)==NWL_OK&&!out.word[NWL_F_JOIN_READY]);C(!ready_calls);
 njs_native.notice_lock=&locks;C(command(NWL_STATUS,0,&out)==NWL_OK&&out.word[NWL_F_JOIN_READY]);C(!ready_calls);
 for(unsigned k=0;k<6;k++){njs_native.active=njs_native.queued=njs_native.retiring=nss_native.failed=0;njs_native.completion=0;njs_native.serial=0;
  if(k==0)njs_native.active=1;if(k==1)njs_native.queued=1;if(k==2)njs_native.retiring=1;if(k==3)njs_native.completion=&locks;if(k==4)njs_native.serial=255;if(k==5)nss_native.failed=1;
  C(command(NWL_STATUS,0,&out)==NWL_OK&&!out.word[NWL_F_JOIN_READY]);}
 njs_native.active=njs_native.queued=njs_native.retiring=nss_native.failed=0;njs_native.completion=0;njs_native.serial=0;njs_native.used=1;
 C(command(NWL_STATUS,0,&out)==NWL_OK&&!out.word[NWL_F_JOIN_READY]);rejoin_ready=1;C(command(NWL_STATUS,0,&out)==NWL_OK&&out.word[NWL_F_JOIN_READY]);C(ready_calls==2);
 njs_native.used=0;njs_native.active=1;njs_native.notice_lock=0;
 njs_native.native_generation=UINT64_C(65537);C(command(NWL_CAPTURE_DHCP,1,&out)==NWL_STALE);C(!memcmp(&before,s,sizeof before));C(out.word[NWL_F_GENERATION_LO]==65537);njs_native.native_generation=UINT64_C(0x100000001);C(command(NWL_OFLD_DROP,1,&out)==NWL_STALE);C(out.word[NWL_F_GENERATION_HI]==1);njs_native.native_generation=1;
 C(command(NWL_CAPTURE_HEADERS,0,&out)==NWL_STALE);C(!memcmp(&before,s,sizeof before));
 available=0;origlocks=locks;C(command(NWL_STATUS,0,&out)==NWL_UNAVAILABLE);C(locks==origlocks);for(unsigned k=9;k<64;k++)C(!out.word[k]);available=1;thread=8;C(command(NWL_STATUS,0,&out)==NWL_UNAVAILABLE);C(!memcmp(&before,s,sizeof before));thread=7;
 /* Capture touches only live emission quotas, never original accounting,
  * PID state, key/PN or deadline. */
 uint8_t f[60]={0};f[12]=8;unsigned pid=nsdd_pid_reserve(1,5,3,1,f,sizeof f);C(pid==64);struct nsdd_pid_record ledger=*nsdd_test_pid(pid);
 for(unsigned k=1;k<=32;k++){C(command(k&1?NWL_CAPTURE_DHCP:NWL_CAPTURE_HEADERS,1,&out)==NWL_OK);C(nwl_native.windows==k&&out.word[NWL_F_WINDOWS_LEFT]==32-k);C(s->data_diag.live_epoch==k&&s->data_diag.live_tx==0&&s->data_diag.live_rx==0);preserved(before);C(!memcmp(&ledger,nsdd_test_pid(pid),sizeof ledger));}
 C(command(NWL_CAPTURE_DHCP,1,&out)==NWL_LIMIT);C(nwl_native.windows==32);C(command(NWL_CAPTURE_STOP,1,&out)==NWL_OK);C(!nwl_native.mode&&!s->data_diag.live_mode);preserved(before);
 /* Synthetic boundary setup: command reserved the32nd window while idle.
  * Configure consumes it once; following generation cannot gain free quota. */
 nwl_native.windows=31;njs_native.active=0;C(command(NWL_CAPTURE_HEADERS,1,&out)==NWL_OK);C(nwl_native.windows==32&&nwl_native.pending&&nwl_native.mode==2&&!s->data_diag.live_mode);
 njs_native.active=1;nwl_configure_station();C(!nwl_native.pending&&nwl_native.windows==32&&s->data_diag.live_mode==2&&s->data_diag.live_epoch==32);preserved(before);
 nwl_configure_station();C(!nwl_native.mode&&!s->data_diag.live_mode&&nwl_native.windows==32);preserved(before);
 C(command(NWL_CAPTURE_STOP,1,&out)==NWL_OK&&!nwl_native.pending&&!nwl_native.mode);
 /* Another isolated boundary setup proves STOP cancels pending reservation. */
 nwl_native.windows=31;njs_native.active=0;C(command(NWL_CAPTURE_DHCP,1,&out)==NWL_OK&&nwl_native.pending);C(command(NWL_CAPTURE_STOP,1,&out)==NWL_OK&&!nwl_native.pending&&!nwl_native.mode);njs_native.active=1;nwl_configure_station();C(nwl_native.windows==32&&!s->data_diag.live_mode);
 /* A current prepared capture is notpending; newgeneration charges again. */
 nwl_native.windows=0;C(command(NWL_CAPTURE_HEADERS,1,&out)==NWL_OK&&!nwl_native.pending&&nwl_native.windows==1&&s->data_diag.live_mode==2);nwl_configure_station();C(nwl_native.windows==2&&s->data_diag.live_epoch==2);preserved(before);C(command(NWL_CAPTURE_STOP,1,&out)==NWL_OK);

 for(unsigned k=0;k<8;k++){njs_native.active=1;s->prepared=1;s->cleanup_started=0;njs_native.leave=njs_native.canceled=njs_native.retiring=nss_native.failed=s->halt=0;
  if(k==0)njs_native.active=0;if(k==1)s->prepared=0;if(k==2)s->cleanup_started=1;if(k==3)njs_native.leave=1;if(k==4)njs_native.canceled=1;if(k==5)njs_native.retiring=1;if(k==6)nss_native.failed=1;if(k==7)s->halt=1;
  C(command(NWL_OFLD_DROP,1,&out)==NWL_REFUSED);C(!s->ofld_drop);
 }
 njs_native.active=s->prepared=1;s->cleanup_started=njs_native.leave=njs_native.canceled=njs_native.retiring=nss_native.failed=s->halt=0;
 C(command(NWL_OFLD_DROP,1,&out)==NWL_OK&&s->ofld_drop&&out.word[NWL_F_OFLD_DROP]==1);C(command(NWL_OFLD_ALLOW,1,&out)==NWL_OK&&!s->ofld_drop);before.ofld_drop=s->ofld_drop;preserved(before);
 s->controlled_port=1;C(command(NWL_DISCONNECT,1,&out)==NWL_OK);C(disconnect_calls==1&&!depth&&njs_native.leave&&!s->controlled_port);disconnect_result=1;C(command(NWL_DISCONNECT,1,&out)==NWL_REFUSED);C(disconnect_calls==2);
 /* Full-width max accepted, overflow stale without truncation. */
 njs_native.native_generation=65535;njs_native.leave=0;C(command(NWL_CAPTURE_STOP,65535,&out)==NWL_OK);njs_native.native_generation=65536;C(command(NWL_CAPTURE_STOP,0,&out)==NWL_STALE);
 struct nsdd d={0};uint8_t frame[400]={0};frame[12]=8;frame[13]=6;
 nsdd_capture(&d,2,1);for(unsigned k=0;k<70;k++){nsdd_publish(&d,1,k+1,3,frame,60);nsdd_receive(&d,1,frame,60);}C(d.live_tx==64&&d.live_rx==64&&d.tx_data==70&&d.rx_unicast==70);C(d.headers_tx==4&&d.headers_rx==4);
 struct nsdd saved=d;nsdd_capture(&d,1,2);saved.live_mode=1;saved.live_epoch=2;saved.live_tx=saved.live_rx=0;C(!memcmp(&saved,&d,sizeof d));nsdd_publish(&d,1,71,3,frame,60);nsdd_receive(&d,1,frame,60);C(!d.live_tx&&!d.live_rx);
 size_t z=dhcp_packet(frame,1,0,1);nsdd_publish(&d,1,72,3,frame,z);nsdd_receive(&d,1,frame,z);C(d.live_tx==1&&d.live_rx==1&&d.tx_dhcp==1&&d.rx_dhcp==1);nsdd_capture(&d,0,2);C(!d.live_mode&&!d.live_tx&&!d.live_rx&&d.tx_data==72&&d.rx_group==1);
 saved=d;nsdd_capture(&d,3,2);C(!memcmp(&saved,&d,sizeof d));nsdd_capture(&d,1,0);C(!memcmp(&saved,&d,sizeof d));nsdd_capture(&d,1,33);C(!memcmp(&saved,&d,sizeof d));
 puts("PASS actual live binder: owner/full-generation/snapshot/capture-only/32windows/profile/disconnect lock ordering");printf("PASS %u binder assertions\n",checks);return 0;}
