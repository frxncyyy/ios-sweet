/* STA-record BMC parity test against the selected Android driver.
 * Executed by tools/test-nord2-wifi-station.py.
 *
 * Seams (recorded, not hidden):
 *  - Reuses canonical tools/fixtures/nord2-wifi-station/transport-hardware.inc
 *    (synthetic MMIO/DMA model, auto STA-active/key-ready/token-report events).
 *    It is a firmware MODEL: nothing here proves firmware accepts 255 or that
 *    any frame reaches air.
 *  - Drives the REAL nst_peer_begin/peer_progress/command/data_transmit code
 *    and inspects bytes the transport actually published, not a helper.
 *  - Keys are all-zero public test vectors; no identity/packet secrets printed.
 */
#define NST_TEST_NO_MAIN 1
#include "transport-hardware.inc"
#ifndef SW_WTBL_RESERVED_ENTRY
/* Present only so the negative control compiles against unpatched canonical;
 * there the published STA byte stays 4 and the checks below must abort. */
#define SW_WTBL_RESERVED_ENTRY 255
#endif

struct seen {unsigned sta_n,bss_n,rlm_n,other_n;
 uint8_t sta_state[8],sta_bmc[8],sta_wlan[8],sta_resp[8],sta_index[8],bss_bmc[8],bss_conn[8];};

static int command_capture(const struct test*t){return t->capture_n>=64&&t->capture[37]==0xa0&&t->capture[38]==1;}
static void record(struct test*t,struct seen*w){
 if(!command_capture(t))return;const uint8_t*p=t->capture+64;unsigned cid=t->capture[36];
 if(cid==0x13){C(t->capture_n==64+SW_SIZE_CMD_UPDATE_STA_RECORD);C(w->sta_n<8);unsigned k=w->sta_n++;
  w->sta_state[k]=p[SW_OFF_CMD_UPDATE_STA_RECORD_ucStaState];w->sta_bmc[k]=p[SW_OFF_CMD_UPDATE_STA_RECORD_ucBMCWlanIndex];
  w->sta_wlan[k]=p[SW_OFF_CMD_UPDATE_STA_RECORD_ucWlanIndex];w->sta_resp[k]=p[SW_OFF_CMD_UPDATE_STA_RECORD_ucNeedResp];
  w->sta_index[k]=p[SW_OFF_CMD_UPDATE_STA_RECORD_ucStaIndex];}
 else if(cid==0x12){C(t->capture_n==64+SW_SIZE_CMD_SET_BSS_INFO);C(w->bss_n<8);unsigned k=w->bss_n++;
  w->bss_bmc[k]=p[SW_OFF_CMD_SET_BSS_INFO_ucBMCWlanIndex];w->bss_conn[k]=p[SW_OFF_CMD_SET_BSS_INFO_ucConnectionState];}
 else if(cid==0x19)w->rlm_n++;else w->other_n++;
}
/* One command publication per nst_step: transmit() publishes at most the one
 * queued wire, peer_progress only queues the next. Record every publication. */
static void drive(struct test*t,unsigned kind,struct seen*w){
 for(unsigned k=0;k<40&&!t->events[kind];k++){unsigned before=t->published;step(t);C(t->published-before<=1);if(t->published!=before)record(t,w);}
 C(t->events[kind]);
}
static void wait_event(struct test*t,unsigned kind,unsigned old){for(unsigned k=0;k<40&&t->events[kind]==old;k++)step(t);C(t->events[kind]==old+1);}
static void profile_of(struct test*t,struct nst_profile*profile,uint8_t*rates,uint8_t*ssid){
 *profile=(struct nst_profile){.assoc={.ssid=ssid,.ssid_len=3,.rates=rates,.rates_len=6,.capability=0x431,.listen_interval=10},.beacon_interval=100,.rcpi=100,.sta_version=1};
 memcpy(&profile->assoc.peer,&t->s.peer,12);
}

static void codec(void){
 /* Direct builder: fail-closed for any non-reserved STA BMC, exact placement. */
 uint8_t out[SW_SIZE_CMD_UPDATE_STA_RECORD];memset(out,0xcc,sizeof out);size_t used=7;
 struct sw_sta a={.bss=0,.sta=0,.wlan=5,.bmc_wlan=4,.sta_type=65,.state=1,.need_response=0,.phy_set=1,.rcpi=100,.rts_policy=3,.version=1,.aid=0,.listen_interval=10,.rates=0x000f,.basic_rates=0x0003,.fixed_rate=0};a.ap[0]=2;a.ap[5]=2;
 C(sw_sta(out,sizeof out,&used,&a)==SW_INVALID);C(used==0);for(size_t k=0;k<sizeof out;k++)C(out[k]==0xcc);
 a.bmc_wlan=SW_WTBL_RESERVED_ENTRY;C(sw_sta(out,sizeof out,&used,&a)==SW_OK);C(used==SW_SIZE_CMD_UPDATE_STA_RECORD);
 C(out[SW_OFF_CMD_UPDATE_STA_RECORD_ucBMCWlanIndex]==255);C(out[SW_OFF_CMD_UPDATE_STA_RECORD_ucWlanIndex]==5);
 C(out[SW_OFF_CMD_UPDATE_STA_RECORD_ucStaState]==1);C(out[SW_OFF_CMD_UPDATE_STA_RECORD_ucStaType]==65);
 a.state=3;a.need_response=1;a.aid=1;C(sw_sta(out,sizeof out,&used,&a)==SW_OK);
 C(out[SW_OFF_CMD_UPDATE_STA_RECORD_ucBMCWlanIndex]==255&&out[SW_OFF_CMD_UPDATE_STA_RECORD_ucStaState]==3&&out[SW_OFF_CMD_UPDATE_STA_RECORD_ucNeedResp]==1);
 a.wlan=255;C(sw_sta(out,sizeof out,&used,&a)==SW_INVALID); /* peer WLAN is never the reserved entry */
 /* BSS-info BMC is a different field: real group slot is preserved. */
 uint8_t bss[SW_SIZE_CMD_SET_BSS_INFO];uint8_t ssid[3]={'l','a','b'};
 struct sw_bss b={.rlm={0,1,6,0,0,0,1,1},.ssid=ssid,.ssid_len=3,.connected=1,.ap_sta=0,.bmc_wlan=4,.nonht_phy=1,.phy_set=1,.dbdc_band=4,.rates=0x000f,.basic_rates=0x0003,.fixed_rate=0};b.ap[0]=2;b.ap[5]=2;
 C(sw_bss(bss,sizeof bss,&used,&b)==SW_OK);C(bss[SW_OFF_CMD_SET_BSS_INFO_ucBMCWlanIndex]==4);C(bss[SW_OFF_CMD_SET_BSS_INFO_ucConnectionState]==0);
 /* Key ABI unchanged: group key may target slot4, never the reserved entry. */
 uint8_t key[16]={0},body[64];struct sw_key k={.bss=0,.wlan=4,.pairwise=0,.key_id=1,.key=key,.key_len=16};k.ap[0]=2;k.ap[5]=2;
 C(sw_key_add(body,sizeof body,&used,&k)==SW_OK&&body[SW_OFF_CMD_802_11_KEY_ucWlanIndex]==4);
 k.wlan=SW_WTBL_RESERVED_ENTRY;C(sw_key_add(body,sizeof body,&used,&k)==SW_INVALID);
}

int main(void){
 struct test*t=calloc(1,sizeof *t);assert(t);codec();
 uint8_t rates[6]={0x82,0x84,0x8b,0x96,96,108};uint8_t ssid[3]={'l','a','b'};struct nst_profile profile;struct seen w={0};
 setup(t);profile_of(t,&profile,rates,ssid);
 /* M2/M4 policy precondition: protected EAPOL is refused before PTK readiness. */
 uint8_t eapol[22]={0};memcpy(eapol,t->s.peer.ap,6);memcpy(eapol+6,t->s.peer.self,6);eapol[12]=0x88;eapol[13]=0x8e;eapol[14]=2;eapol[15]=3;eapol[17]=4;
 C(nst_eapol(&t->s,90,eapol,sizeof eapol,1)<0);C(!t->s.wire_busy);

 /* STA state1 (prepare): STA-record BMC reserved, BSS BMC real slot4. */
 C(!nst_peer_begin(&t->s,1,NST_PREPARE_PEER,&profile,NULL,0));drive(t,NST_PEER_PREPARED,&w);
 C(t->published==4&&w.other_n==1&&w.bss_n==1&&w.rlm_n==1&&w.sta_n==1);
 C(w.sta_state[0]==1&&w.sta_resp[0]==0&&w.sta_wlan[0]==5&&w.sta_index[0]==0);
 C(w.sta_bmc[0]==SW_WTBL_RESERVED_ENTRY);
 C(w.bss_bmc[0]==4&&w.bss_conn[0]==1);
 C(t->s.peer.group_wlan==4&&t->s.peer.wlan==5&&t->s.group_spare[0]==6&&t->s.group_spare[1]==7&&t->s.group_spare[2]==8);

 uint8_t ch[24];size_t used;struct sw_channel channel={0,1,0,1,6,4,4000};C(!sw_channel(ch,sizeof ch,&used,&channel));C(!nst_command(&t->s,2,28,ch,used));until(t,NST_CHANNEL_GRANT);

 /* STA state3 (configure): reserved STA BMC must not break source activation
  * (transport.c activation keys on state byte20 + NeedResp byte51 only). */
 uint8_t response[38];association(response,t);
 C(!nst_peer_begin(&t->s,4,NST_CONFIGURE_PEER,NULL,response,sizeof response));drive(t,NST_PEER_CONFIGURED,&w);
 C(w.sta_n==2&&w.bss_n==2);
 C(w.sta_state[1]==3&&w.sta_resp[1]==1&&w.sta_wlan[1]==5&&w.sta_bmc[1]==SW_WTBL_RESERVED_ENTRY);
 C(w.bss_bmc[1]==4&&w.bss_conn[1]==0);
 C(t->s.sta_active&&!t->s.channel_held&&t->events[NST_CHANNEL_RELEASED]);

 /* PTK on AP WLAN5; GTK on initial BMC slot4 (unchanged mapping). */
 uint8_t key[16]={0},body[64];struct sw_key k={.bss=0,.wlan=5,.pairwise=1,.key_id=0,.key=key,.key_len=16};memcpy(k.ap,t->s.peer.ap,6);
 C(!sw_key_add(body,sizeof body,&used,&k));C(!nst_command(&t->s,5,7,body,used));wait_event(t,NST_COMMAND_DMA,t->events[NST_COMMAND_DMA]);
 C(t->s.ptk_ready&&t->events[NST_KEY_READY]);uint8_t rsc[8]={0};C(!nst_replay_install(&t->s,1,0,rsc));
 k.wlan=5;k.pairwise=0;k.key_id=0;C(!sw_key_add(body,sizeof body,&used,&k));C(nst_command(&t->s,60,7,body,used)<0); /* GTK never on peer WLAN */
 k.wlan=4;C(!sw_key_add(body,sizeof body,&used,&k));C(!nst_command(&t->s,6,7,body,used));wait_event(t,NST_COMMAND_DMA,t->events[NST_COMMAND_DMA]);
 rsc[0]=2;C(!nst_replay_install(&t->s,0,0,rsc));C((t->s.gtk_mapped&1u)&&t->s.gtk_wlan[0]==4);
 C(!nst_authorize(&t->s));C(t->s.controlled_port);

 /* Ordinary protected Ethernet data: normal TXD0, AP WLAN5, PF set, HF non-802.11 + Ethernet-II. */
 uint8_t eth[60]={0};memset(eth,255,6);memcpy(eth+6,t->s.peer.self,6);eth[12]=8;eth[13]=0;
 unsigned pub=t->published;C(!nst_data(&t->s,7,eth,sizeof eth));wait_event(t,NST_DATA_SUBMITTED,t->events[NST_DATA_SUBMITTED]);C(t->published==pub+1);
 uint32_t d0=get32(t->capture),d1=get32(t->capture+4),d3=get32(t->capture+12);
 C((d1&1023u)==5);C(d3&2u);C(d1&(1u<<15));C(((d1>>16)&3u)==0);C(((d0>>23)&3u)==0);
 C(t->capture_n==32+sizeof eth&&!memcmp(t->capture+32,eth,sizeof eth));

 /* M2/M4 unprotected and G2 protected EAPOL on the same AP WLAN; policy unchanged. */
 pub=t->published;C(!nst_eapol(&t->s,8,eapol,sizeof eapol,0));wait_event(t,NST_SECURITY_SUBMITTED,t->events[NST_SECURITY_SUBMITTED]);C(t->published==pub+1);
 d1=get32(t->capture+4);d3=get32(t->capture+12);C((d1&1023u)==5&&!(d3&2u));
 pub=t->published;C(!nst_eapol(&t->s,9,eapol,sizeof eapol,1));wait_event(t,NST_SECURITY_SUBMITTED,t->events[NST_SECURITY_SUBMITTED]);C(t->published==pub+1);
 d1=get32(t->capture+4);d3=get32(t->capture+12);C((d1&1023u)==5&&(d3&2u));

 /* Cleanup/retire: no STA-record update, BSS disconnect keeps slot4, keys wiped. */
 t->now+=1000;C(!nst_cleanup_begin(&t->s,t->now,t->now+UINT64_C(30000000)));unsigned sta_before=w.sta_n,bss_before=w.bss_n;
 C(!nst_peer_begin(&t->s,10,NST_RETIRE_PEER,NULL,NULL,0));drive(t,NST_PEER_RETIRED,&w);
 C(w.sta_n==sta_before&&w.bss_n==bss_before+1&&w.bss_bmc[w.bss_n-1]==4&&w.bss_conn[w.bss_n-1]==1);
 C(!t->s.sta_active&&!t->s.ptk.installed&&!t->s.gtk.installed&&!t->s.controlled_port);
 for(unsigned x=0;x<w.sta_n;x++)C(w.sta_bmc[x]==SW_WTBL_RESERVED_ENTRY&&w.sta_wlan[x]==5);
 for(unsigned x=0;x<w.bss_n;x++)C(w.bss_bmc[x]==4);
 cleanup(t);

 /* Failure ownership at every hardware boundary while STA1 is serialized. */
 setup(t);profile_of(t,&profile,rates,ssid);C(!nst_peer_begin(&t->s,1,NST_PREPARE_PEER,&profile,NULL,0));
 {struct seen z={0};drive(t,NST_PEER_PREPARED,&z);C(z.sta_bmc[0]==SW_WTBL_RESERVED_ENTRY);}unsigned calls=t->calls;cleanup(t);
 for(unsigned f=1;f<=calls;f++){setup(t);profile_of(t,&profile,rates,ssid);C(!nst_peer_begin(&t->s,1,NST_PREPARE_PEER,&profile,NULL,0));t->fail_at=f;unsigned failed=0;
  for(unsigned x=0;x<40&&!failed&&!t->events[NST_PEER_PREPARED];x++){if(nst_step(&t->s,&io,t,16,callback,t)<0)failed=1;else t->now+=1000;}
  C(failed&&!t->events[NST_PEER_PREPARED]);C(t->s.halt&&!t->s.controlled_port);
  C(t->s.credit_available+t->s.credit_reserved==t->s.credit_capacity);C(t->s.credit_acquired-t->s.credit_released==t->s.credit_reserved);cleanup(t);}
 free(t);
 printf("PASS %u checks: STA1/STA3 published BMC255, BSS/GTK slot4, AP WLAN5 protected normal data, M2/M4/G2 policy, retire and failure ownership (synthetic model, no hardware claim)\n",checks);
 return 0;
}
