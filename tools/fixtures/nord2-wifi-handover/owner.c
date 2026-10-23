#include <openssl/evp.h>
#include "wifi_wpa_crypto.h"
#include "vectors.h"
static unsigned station_pubs,station_frames,station_commands,link_up_calls,link_down_calls,key_deletes,notices[5],m1_sent,m3_sent,stale_m3,clock_expire;
static void test_prepare_clock(void){if(clock_expire){drain_now=njs_native.deadline_us*1000;clock_expire=0;}}
static unsigned callbacks;
static void completed(void*c,uint64_t generation,uint64_t ticket,enum nss_status status){assert(c==&callbacks&&generation==99&&(status==NSS_COMPLETE||status==NSS_PARTIAL));callbacks++;assert(!nord2_wifi_scan_release(ticket));}
int nord2_wifi_ifnet_link(uint64_t g,int a){assert(g);if(a){assert(nst_native.state.controlled_port);link_up_calls++;}else{assert(!nst_native.state.controlled_port);link_down_calls++;}return 0;}
int nord2_wifi_ifnet_receive(uint64_t g,const uint8_t*p,size_t n){assert(g&&p&&n>=14);return 0;}
static void drain(void){worker_thread=2;if(!setjmp(parked_exit))nss_owner_loop();}
static uint8_t*station_packet(unsigned k,unsigned type,unsigned bytes){
 struct ncr_queue*q=&nst_native.state.queue[k];unsigned at=q->next;
 uint8_t*d=pool_memory+(q->ring.descriptors-NDP_POOL_BASE)+16*at,*p=pool_memory+(q->ring.buffers-NDP_POOL_BASE)+4096*at;memset(p,0,bytes);w16(p,bytes);w16(p+2,type==7?0x3800:type<<11);
 put32(d,(uint32_t)(q->ring.buffers+4096*at));put32(d+4,0xc0000000u|(bytes<<16));put32(d+8,0);put32(d+12,0);hardware_register(q->ring.base+12,(at+1)%q->ring.count);return p;
}
static void station_mgmt(unsigned fc){
 struct nst*s=&nst_native.state;unsigned bytes=fc==0xb0?30:radio_open?33:55;uint8_t*p=station_packet(2,7,24+bytes);w16(p+2,0x3801);put32(p+8,12u<<8);p+=24;w16(p,fc);memcpy(p+4,s->peer.self,6);memcpy(p+10,s->peer.ap,6);memcpy(p+16,s->peer.ap,6);
 if(fc==0xb0){w16(p+26,2);}else{w16(p+24,radio_open?1:0x11);w16(p+28,1);p[30]=1;p[31]=1;p[32]=0x82;if(!radio_open)memcpy(p+33,rsn,22);}
}
static void station_eapol(const uint8_t*f,size_t n){
 struct nst*s=&nst_native.state;uint8_t*p=station_packet(0,2,(unsigned)n+54);put32(p+4,(8u<<11)|s->peer.wlan);put32(p+8,(1u<<13)|(7u<<8));w16(p+24,0x0208);memcpy(p+26,s->peer.ap,6);memcpy(p+40,s->peer.self,6);memcpy(p+46,s->peer.ap,6);p[52]=0x88;p[53]=0x8e;memcpy(p+54,f,n);
}
static void make_m3(uint8_t*out){
 uint8_t context[76],derived[48],plain[80],mic[20];size_t used=0;struct nst*s=&nst_native.state;
 const uint8_t*a=s->peer.self,*b=s->peer.ap;if(memcmp(a,b,6)>0){const uint8_t*t=a;a=b;b=t;}memcpy(context,a,6);memcpy(context+6,b,6);
 uint8_t snonce[32];memset(snonce,(int)(0x40+random_calls-(stale_m3?1:0)),32);const uint8_t*x=snonce,*y=m1+17;if(memcmp(x,y,32)>0){const uint8_t*t=x;x=y;y=t;}memcpy(context+12,x,32);memcpy(context+44,y,32);
 assert(!nwpc_ptk(pmk_vector,context,derived));assert(!nwpc_unwrap(ptk+16,m3+99,sizeof m3-99,plain,sizeof plain,&used)&&used==48);
 memcpy(out,m3,sizeof m3);EVP_CIPHER_CTX*c=EVP_CIPHER_CTX_new();assert(c);EVP_CIPHER_CTX_set_flags(c,EVP_CIPHER_CTX_FLAG_WRAP_ALLOW);int n=0,end=0;assert(EVP_EncryptInit_ex(c,EVP_aes_128_wrap(),0,derived+16,0)&&EVP_EncryptUpdate(c,out+99,&n,plain,(int)used)&&EVP_EncryptFinal_ex(c,out+99+n,&end)&&n+end==56);EVP_CIPHER_CTX_free(c);
 memset(out+81,0,16);const uint8_t*parts[]={out};size_t lengths[]={sizeof m3};assert(!nwpc_hmac(derived,16,parts,lengths,1,mic));memcpy(out+81,mic,16);niwj_clear(derived,sizeof derived);niwj_clear(plain,sizeof plain);
}
static void station_data_completion(unsigned next){
 struct nst*s=&nst_native.state;uint8_t*d=pool_memory+(s->normal.ring-NDP_POOL_BASE)+16*s->normal.next;
 assert(station_hardware&&s->wire_busy&&(s->wire_type==2||s->wire_type==3));station_pubs++;station_frames++;
 uint8_t*page=pool_memory+(s->normal.buffer-NDP_POOL_BASE);unsigned wirelen=(page[44]|(unsigned)page[45]<<8)&32767;
 if(s->wire_type==3&&s->data_profile==NST_DATA_TODS){const uint8_t*raw=page+64;
  assert(wirelen==s->wire_bytes-14&&raw[0]==8&&raw[1]==65&&!memcmp(raw+4,s->peer.ap,6)&&!memcmp(raw+10,s->peer.self,6)&&!memcmp(raw+16,s->wire+32,6)&&!memcmp(raw+30,s->wire+44,s->wire_bytes-44));
  assert(((get32(page+4)>>16)&3)==2&&((get32(page+8)&63)==32)&&((get32(page+28)>>20)&3)==2);
 }else{assert(wirelen==s->wire_bytes-32&&!memcmp(page+64,s->wire+32,wirelen)&&!((get32(page+4)>>16)&3));}
 uint8_t*r=station_packet(3,6,12);w16(r+2,(6u<<11)|1);put32(r+4,2u<<16);put32(r+8,s->normal.token);
 /* Inject valid M3 before local M2 receipt: actual bounded pending queue. */
 if(s->wire_type==2&&njs_native.driver.owner.transaction.wpa.tx_kind==1&&!m3_sent){uint8_t packet[sizeof m3];make_m3(packet);station_eapol(packet,sizeof packet);m3_sent++;}
 put32(d+4,get32(d+4)|0x80000000u);hardware_register(0x1802530c,next);
}
static void station_completion(unsigned next){
 struct nwt_state*t=&nwfn_native.init.transport;struct nst*s=&nst_native.state;uint8_t*d=pool_memory+(t->tx_ring-NDP_POOL_BASE)+16*t->tx_index,*p=pool_memory+(t->command-NDP_POOL_BASE);
 assert(station_hardware&&s->wire_busy);station_pubs++;
 if(s->wire_type){station_frames++;assert(s->wire_type==1);uint8_t*r=station_packet(1,7,124);r[28]=15;r[36]=s->wire_pid;r[40]=s->wire_wlan;if(s->generation>1){uint8_t*late=station_packet(4,7,124);late[28]=15;late[36]=1;late[40]=s->wire_wlan;}
 unsigned fc=s->wire[32];if(fc==0xb0||fc==0)station_mgmt(fc==0xb0?0xb0:0x10);}
 else{station_commands++;assert(p[37]==0xa0&&p[39]==t->next_sequence);
 if(p[36]==0x1c&&!p[66]){uint8_t*r=station_packet(1,7,60);r[28]=16;r[36]=s->peer.bss;r[37]=p[65];r[39]=s->peer.channel;r[41]=s->peer.band;put32(r+48,4000);}
 if(p[36]==0x13&&p[84]==SW_STA_CLASS_3&&p[115]){uint8_t*r=station_packet(1,7,44);r[28]=12;r[29]=p[39];memcpy(r+36,s->peer.ap,6);r[42]=s->peer.sta;r[43]=s->peer.bss;}
 if(p[36]==7&&p[64]&&p[66]){uint8_t*r=station_packet(1,7,44);r[28]=36;r[36]=s->peer.bss;memcpy(r+38,s->peer.ap,6);}
 if(p[36]==7&&!p[64])key_deletes++;
 }
 put32(d,0xffffffffu);put32(d+4,0xffffffffu);put32(d+8,0xdeadbeefu);put32(d+12,0x12345678u);hardware_register(0x1802541c,next);
}
static void join_completed(void*c,uint64_t epoch,uint64_t ticket,enum njs_notice notice){assert(c==notices&&epoch==7&&ticket&&notice<5);notices[notice]++;assert(njs_native.snapshot.security==NIWJ_WPA2_PSK_CCMP);assert(!pthread_mutex_trylock(&nss_native.lock->m));pthread_mutex_unlock(&nss_native.lock->m);if(notice==NJS_RETIRED)assert(!njs_rejoin_ready_locked());}
static struct niwj_request request(void){struct niwj_request q={0};q.security=radio_open?NIWJ_OPEN:NIWJ_WPA2_PSK_CCMP;q.ssid_length=1;q.ssid[0]=(uint8_t)radio_ssid;if(!radio_open){memcpy(q.pmk,pmk_vector,32);memcpy(q.station_rsn,rsn,22);}return q;}
static void until_authorized(void){for(unsigned k=0;k<1000&&!njs_native.linked;k++){
 if(njs_native.driver.owner.transaction.phase==STC_EAPOL&&!m1_sent){station_eapol(m1,sizeof m1);uint8_t*early=station_packet(4,7,44);early[28]=36;early[36]=nst_native.state.peer.bss;memcpy(early+38,nst_native.state.peer.ap,6);assert(!nst_native.state.ptk_command&&!nst_native.state.ptk_ready);m1_sent++;}
 assert(njs_owner_work()==1&&!nst_native.state.halt);
 }assert(njs_native.linked&&nwo_authorized(&njs_native.driver.owner)&&(radio_open?(!m1_sent&&!m3_sent&&!nsr_native.ptk&&!nsr_native.gtk):(m1_sent&&m3_sent&&nsr_native.ptk&&nsr_native.gtk)));}
static void ledger_negatives(void){
 struct nsr_ledger l={0};struct nst*s=calloc(1,sizeof(*s));assert(s);s->wire_busy=1;s->wire_cid=7;s->ticket=1;
 struct nst_event e={.kind=NST_COMMAND_DMA,.cid=7,.ticket=1};
 s->wire[64]=1;s->wire[66]=1;nsr_key_capture(&l,s);assert(l.pending&&!l.ptk);assert(!nsr_key_complete(&l,&e)&&l.ptk&&!l.pending);
 for(unsigned k=0;k<4;k++){s->wire[66]=0;s->wire[76]=(uint8_t)k;s->ticket=++e.ticket;nsr_key_capture(&l,s);assert(!nsr_key_complete(&l,&e));}assert(l.gtk==15);
 for(unsigned k=0;k<4;k++){s->wire[64]=0;s->wire[76]=(uint8_t)k;s->ticket=++e.ticket;nsr_key_capture(&l,s);assert(l.gtk&(1u<<k));assert(!nsr_key_complete(&l,&e)&&!(l.gtk&(1u<<k)));}assert(!l.gtk&&l.ptk);
 s->wire[66]=1;s->wire[76]=0;s->ticket=++e.ticket;nsr_key_capture(&l,s);assert(!nsr_key_complete(&l,&e)&&!l.ptk);
 s->wire[64]=1;s->ticket=++e.ticket;nsr_key_capture(&l,s);e.ticket++;assert(nsr_key_complete(&l,&e)<0&&l.bad&&!l.ptk);free(s);
}
int main(int argc,char**argv){assert(!initial_open);assert(argc==2);FILE*f=fopen(argv[1],"rb");assert(f&&fread(authenticated_board,1,sizeof authenticated_board,f)==sizeof authenticated_board);fclose(f);
 ledger_negatives();setup(0);run_worker();assert(nss_native.ready);worker_thread=2;
 /* Actual received synthetic beacon exposes real WPA2 selection inputs. */
 struct niwj_request q=request();uint64_t ticket=0;unsigned saved_pid=0,saved_token=0,saved_hw=0;
 assert(nst_native_next_data_profile()==NST_DATA_ETHERNET&&nst_native_data_profile()==NST_DATA_ETHERNET);
 assert(nst_native_set_next_data_profile(2)<0);
 for(unsigned cycle=0;cycle<2;cycle++){
  assert(!nst_native_set_next_data_profile(0)&&nst_native_next_data_profile()==0);
  if(!cycle){assert(!nord2_wifi_join_enqueue(&q,notices,7,join_completed,&ticket)&&ticket==1);}else{ticket=njs_handover.ticket;assert(ticket==2&&nsr_native.scan_ready&&njs_handover.state==1);}niwj_clear(&q,sizeof q);handover_park=0;
  station_hardware=1;until_authorized();if(cycle)assert(njs_native.admitted_us>UINT64_C(8000000)&&njs_native.deadline_us-njs_native.admitted_us<=NJS_JOIN_BUDGET_US&&njs_native.deadline_us>njs_native.driver.now_ms*1000);assert(random_calls==cycle+1&&nst_native.state.generation==ticket&&nst_native.state.channel_token==ticket);
  assert(nst_native_data_profile()==0&&nst_native_set_next_data_profile(1)<0&&nst_native_next_data_profile()==0);
  if(!radio_open){assert(!nst_native_probe_window(&nwfn_native));assert(!nst_native_probe_begin()&&nst_native_probe_window(&nwfn_native));assert(nst_native_probe_begin()<0&&nst_native_set_next_data_profile(cycle)<0);
  assert(!nst_native_owner_window(&nwfn_native)&&!nst_native_rx_memory(&nwfn_native,nst_native.state.normal.buffer,4096,0));nst_native_probe_end();assert(!nst_native_probe_window(&nwfn_native));}else assert(nst_native_probe_begin()<0);
  if(cycle){assert(nst_native.state.next_pid>saved_pid&&nst_native.state.normal.token_next>=saved_token&&nst_native.state.normal.next>=saved_hw);}
  assert(njs_native.snapshot.security==(radio_open?NIWJ_OPEN:NIWJ_WPA2_PSK_CCMP));
  if(radio_open){struct nst*st=&nst_native.state;assert(!st->ptk_ready&&!st->ptk.installed&&!st->gtk.installed&&st->controlled_port);uint8_t*r=station_packet(0,2,100);put32(r+4,st->peer.wlan|(8u<<11));put32(r+8,1u<<13);w16(r+24,0x0208);memcpy(r+26,st->peer.ap,6);memcpy(r+40,st->peer.self,6);memcpy(r+46,st->peer.ap,6);r[52]=8;r[53]=6;unsigned before=njs_native.rx_delivered;assert(njs_owner_work()==1&&njs_native.rx_delivered==before+1);}
  uint8_t data[60]={0};memcpy(data,nst_native.state.peer.ap,6);memcpy(data+6,nss_native.info.mac,6);data[12]=8;assert(!nord2_wifi_link_send(data,sizeof data,ticket));while(njs_native.tx_count||nst_native.state.wire_busy)assert(njs_owner_work()==1);if(cycle)assert(nst_native.state.normal.token_next>saved_token&&nst_native.state.normal.next>saved_hw);
  /* Pending DMA/keys/port/cookie cannot become a reusable session. */
  assert(!njs_rejoin_ready_locked());assert(!nord2_wifi_join_disconnect());if(!cycle){
   unsigned rd=mmio_reads,wr=mmio_writes;struct niwj_request next=request();next.ssid[0]='Y';uint64_t nt=0;assert(!nord2_wifi_join_enqueue(&next,notices,7,join_completed,&nt)&&nt==2&&njs_handover.state==1&&njs_native.native_generation==1);niwj_clear(&next,sizeof next);struct njs_snapshot pending;assert(!nord2_wifi_join_copy_state(&pending)&&pending.ticket==2&&pending.phase==NJS_QUEUED&&!pending.data_ready&&!pending.associated);assert(rd==mmio_reads&&wr==mmio_writes);handover_park=1;
  }for(unsigned k=0;k<1000&&!njs_native.scan_returned;k++)assert(njs_owner_work()==1);
  assert(njs_native.scan_returned&&!nsr_native.ptk&&!nsr_native.gtk&&!nsr_native.pending&&!nsr_native.bad&&key_deletes==random_calls*2&&notices[NJS_RETIRED]==cycle+1);
  assert(njs_native.used&&nst_native.attempted&&njs_native.arbiter.connection_used&&!njs_rejoin_ready_locked());saved_pid=nst_native.state.next_pid;saved_token=nst_native.state.normal.token_next;saved_hw=nst_native.state.normal.next;
  if(!cycle){radio_ssid='Y';radio_ap=9;drain_now+=UINT64_C(9000000000);assert(njs_handover.state==1&&njs_test_uptime_us()-njs_handover.admitted>UINT64_C(8000000));}q=request();uint64_t untouched=123;if(!cycle)assert(nord2_wifi_join_enqueue(&q,notices,7,join_completed,&untouched)==2&&untouched==123);
  /* Late TXS is copied into old queue then genuinely drained by return scan. */
  uint8_t*r=station_packet(1,7,124);r[28]=15;r[36]=1;r[40]=5;
  if(cycle==1)assert(!nord2_wifi_scan_cancel(nsr_native.scan_ticket));
  station_hardware=0;normal_pubs+=station_commands;station_commands=0;delivered=0;drain();
  if(cycle==1){assert(!njs_rejoin_ready_locked());struct nss_request refresh={0};uint64_t scan_ticket=0;assert(!nord2_wifi_scan_enqueue(&refresh,&callbacks,99,completed,&scan_ticket));delivered=0;drain();assert(callbacks==1);}
  assert(nsr_native.scan_ready&&nss_return.consumed&&njs_rejoin_ready_locked()&&!nss_native.failed&&!manual_halts);
  /* Foreign origin/callback/key/DMA/halts refuse reuse without any IO. */
  unsigned reads=mmio_reads,writes=mmio_writes;
  nsr_native.ptk=1;assert(!njs_rejoin_ready_locked());nsr_native.ptk=0;
  nsr_native.gtk=8;assert(!njs_rejoin_ready_locked());nsr_native.gtk=0;
  nsr_native.pending=1;assert(!njs_rejoin_ready_locked());nsr_native.pending=0;
  nss_native.slot[0].callback_busy=1;assert(!njs_rejoin_ready_locked());nss_native.slot[0].callback_busy=0;
  ncd_native.halt=1;assert(!njs_rejoin_ready_locked());ncd_native.halt=0;
  assert(reads==mmio_reads&&writes==mmio_writes&&njs_rejoin_ready_locked());m1_sent=m3_sent=0;
 }
 assert(delivered&&nrd_calls&&nrd_calls==nrd_restored&&nrd_checked==nrd_calls&&station_pubs>station_frames&&link_down_calls>=2);assert(njs_rejoin_ready_locked()&&notices[NJS_JOINED]==2&&notices[NJS_LEFT]==2&&notices[NJS_RETIRED]==2&&link_up_calls==2&&key_deletes==4&&random_calls==2&&!manual_halts);IOLock*notice=njs_native.notice_lock;assert(!pthread_mutex_destroy(&notice->m));free(notice);assert(!pthread_mutex_destroy(&nss_native.lock->m));free(nss_native.lock);puts("PASS actual owning two different WPA2 networks: first immediate SET22-to-SET20 queued before old retirement, exact teardown/fresh scan before new prepare, distinct target and fresh nonce/key gates; synthetic hardware only");return 0;}
