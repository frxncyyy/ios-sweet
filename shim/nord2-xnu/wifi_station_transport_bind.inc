/* Include after NSS globals/functions and before station_service_bind. Authority
 * is the existing retained worker and existing DMA claim, never this API alone. */
#include "wifi_station_transport.h"
#include "wifi_ios_join.h"
static struct {struct nst state;unsigned attempted,active;}nst_native;
static int nst_native_owner_window(const struct nwfn_native*n){
 return n==&nwfn_native&&nst_native.attempted==1&&nst_native.active==1&&
 ncd_native.active==1&&!ncd_native.parked&&!ncd_native.halt&&
 ncd_native.worker==current_thread()&&!nct_native.active&&
 nst_native.state.init==&nwfn_native.init&&nst_native.state.previous==&nct_native.session&&
 nst_native.state.previous_rx==&ncb_native.rx&&
 nst_lease_window(&nst_native.state,&nwfn_native.init);
}
static int nst_native_rx_active(const struct nwfn_native*n){return nst_native_owner_window(n)&&((nst_native.state.rx_current<5&&nst_native.state.phase>=NST_RX_READ&&nst_native.state.phase<=NST_RX_PUBLISH)||(nst_native.state.phase>=NST_DATA_READ&&nst_native.state.phase<=NST_DATA_RETIRE));}
static int nst_native_rx_memory(const struct nwfn_native*n,uint64_t a,uint32_t bytes,unsigned write){return nst_native_owner_window(n)&&nst_memory_range(&nst_native.state,a,bytes,write);}
static int nst_native_rx_write(const struct nwfn_native*n,uint32_t a,uint32_t v){return nst_native_owner_window(n)&&nst_register_write(&nst_native.state,a,v);}
static int nst_native_prepare(const struct niwj_target*t,uint64_t generation,uint64_t now,uint64_t deadline,struct nst_profile*out){
 if(!t||!out||!generation||nst_native.attempted||ncd_native.worker!=current_thread()||!ncd_native.active||ncd_native.parked||ncd_native.halt||nct_native.active||!nss_available()||nss_native.failed||!ncb_native.prepared||!nbc_native.state.plan.pse_data||ncb_native.rx.active||!nst_scan_admissible(&ncb_native.rx.scan)||!nct_done_receipt(&nct_native.session))return -1;
 for(unsigned k=0;k<NSS_SLOTS;k++)if(nss_native.slot[k].state==NSS_QUEUED||nss_native.slot[k].state==NSS_RUNNING)return 1;
 struct ncs_cap cap;if(!ncd_native.diag.events||ncd_native.diag.events>sizeof(ncd_native.diag.history)/sizeof(ncd_native.diag.history[0])||ncs_cap_mac(ncd_native.diag.frame,ncd_native.diag.frame_bytes,ncd_native.diag.history[ncd_native.diag.events-1].sequence,&cap)||cap.bmc!=4||cap.wtbl<7)return -1;
 unsigned allowed=0;for(unsigned k=0;k<nss_native.info.count;k++)if(nss_native.info.channels[k].band==t->band&&nss_native.info.channels[k].channel==t->channel)allowed=1;
 if(!allowed||t->record.channel_mismatch||t->record.ies_truncated||t->record.ie_bytes!=t->record.ie_total||!t->record.ssid_present||!t->record.ssid_len||t->record.ssid_len>32||t->rate_count>12||!t->rate_count||!t->record.interval)return -1;
 struct nst_profile profile={0};struct nst_peer peer={0};
 uint16_t all,basic,rate;if(nst_legacy_rates(t->rates,t->rate_count,&all,&basic,&rate))return -1;
 /* First source ledger: BSS0/ownMac from actual activated scan CAP; WTBL0..3
  * reserved for BIP,4 already assigned BMC, first unused pairwise5; STA0. */
 peer.bss=0;peer.own_mac=cap.own_mac;peer.group_wlan=cap.bmc;peer.wlan=5;peer.sta=0;peer.band=t->band;peer.channel=t->channel;
 memcpy(peer.self,nss_native.info.mac,6);memcpy(peer.ap,t->record.bssid,6);
 memcpy(profile.assoc.peer.self,peer.self,6);memcpy(profile.assoc.peer.ap,peer.ap,6);
 profile.assoc.ssid=t->record.ssid;profile.assoc.ssid_len=t->record.ssid_len;profile.assoc.rates=t->rates;profile.assoc.rates_len=t->rate_count;
 profile.assoc.listen_interval=10;profile.assoc.capability=0x11;
 /* Implemented legacy PHY attributes: HRDSSS short preamble, ERP short
  * preamble/slot, 5GHz OFDM neither. No copied HT/PMF/spectrum capability. */
 if(t->band==1){profile.assoc.capability|=t->record.capability&32;if(all&0x3fc0)profile.assoc.capability|=1024;}
 profile.beacon_interval=t->record.interval;
 if(!t->record.rxv_valid||!(t->record.group_valid&4)||(t->record.rxv[4]==255&&t->record.rxv[5]==255))return -1;
 uint8_t rcpi0=t->record.rxv[4]==255?0:t->record.rxv[4],rcpi1=t->record.rxv[5]==255?0:t->record.rxv[5];profile.rcpi=rcpi0>rcpi1?rcpi0:rcpi1;if(profile.rcpi>220)profile.rcpi=220;
 profile.sta_version=1;
 /* Selected Android source initializes fgEfuseCtrlAxOn=1, AX1/BE0 layout. */
 for(size_t k=0;k<t->record.ie_bytes;){if(t->record.ie_bytes-k<2||t->record.ies[k+1]>t->record.ie_bytes-k-2)return -1;if(t->record.ies[k]==42&&t->record.ies[k+1]==1){profile.erp_protect=!!(t->record.ies[k+2]&2);if(t->record.ies[k+2]&4)profile.assoc.capability&=~32u;}k+=2+t->record.ies[k+1];}
 /* Source TC4: MCU queue1, PSE8, retry30, lifetime2000; long legacy fixed
  * basic rate chosen from peer rates. Explicit one-stream WF0 path policy. */
 struct sw_txd txd={.wlan=peer.wlan,.own_mac=peer.own_mac,.queue=1,.pid=1,.retry_limit=30,.fixed_rate=rate,.pse_header_len=8,.lifetime_ms=2000,.spe_index=0};
 if(nst_prepare(&nst_native.state,&nwfn_native.init,&nct_native.session,&ncb_native.rx,&cap,&peer,&txd,generation,now,deadline))return -1;
 nst_native.attempted=1;nst_native.active=1;*out=profile;
 printf("NORD2 WIFI TXPROFILE: generation=%llu band=%u channel=%u rates=%u basic=%u fixed=%u own=%u wlan=%u group=%u\n",(unsigned long long)generation,peer.band,peer.channel,all,basic,rate,peer.own_mac,peer.wlan,peer.group_wlan);
 return 0;
}
