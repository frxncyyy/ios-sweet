#include "wifi_station_wire.h"
#include "wifi_station_source_layout.h"
#include <string.h>

#define OFF(s,f) SW_OFF_##s##_##f
#define P8(s,f,v) b[OFF(s,f)]=(uint8_t)(v)
#define P16(s,f,v) le16(b+OFF(s,f),(v))
#define P32(s,f,v) le32(b+OFF(s,f),(v))
#define CP(s,f,p,n) memcpy(b+OFF(s,f),(p),(n))
static void le16(uint8_t *p,uint16_t v) {p[0]=(uint8_t)v;p[1]=(uint8_t)(v>>8);}
static void le32(uint8_t *p,uint32_t v) {le16(p,(uint16_t)v);le16(p+2,(uint16_t)(v>>16));}
static uint16_t rd16(const uint8_t *p) {return p[0]|((uint16_t)p[1]<<8);}
static int start(size_t *used) {if(!used)return 0;*used=0;return 1;}
static int finish(uint8_t *out,size_t cap,size_t *used,const uint8_t *b,size_t n) {
    if(!out)return SW_INVALID;
    if(cap<n)return SW_CAPACITY;
    memcpy(out,b,n);*used=n;return SW_OK;
}
static int unicast(const uint8_t p[6]) {
    unsigned i; uint8_t v=0;
    if(p[0]&1)return 0;
    for(i=0;i<6;i++)v|=p[i];
    return v!=0;
}
static int peer_ok(const struct sw_peer *p) {
    return p&&unicast(p->self)&&unicast(p->ap)&&memcmp(p->self,p->ap,6);
}
static void header(uint8_t *b,uint16_t fc,const struct sw_peer *p) {
    le16(b,fc);memcpy(b+4,p->ap,6);memcpy(b+10,p->self,6);memcpy(b+16,p->ap,6);
}
int sw_auth(uint8_t *out,size_t cap,size_t *used,const struct sw_peer *p) {
    uint8_t b[30]={0};
    if(!start(used)||!peer_ok(p))return SW_INVALID;
    header(b,0xb0,p);le16(b+26,1);
    return finish(out,cap,used,b,sizeof(b));
}
static const uint8_t rsn[]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};
static int rate_ok(uint8_t r) {
    switch(r&127) {case 2:case 4:case 11:case 22:case 12:case 18:case 24:
    case 36:case 48:case 72:case 96:case 108:return 1;default:return 0;}
}
int sw_assoc(uint8_t *out,size_t cap,size_t *used,const struct sw_assoc *p) {
    uint8_t b[110]={0};size_t i,j,n=28,first;
    if(!start(used)||!p||!peer_ok(&p->peer)||!p->ssid||!p->ssid_len||
       p->ssid_len>32||!p->rates||!p->rates_len||p->rates_len>12||
       !p->listen_interval||(p->capability&0x11)!=0x11||
       (p->capability&~0x0431u))return SW_INVALID;
    for(i=0;i<p->rates_len;i++) {
        if(!rate_ok(p->rates[i]))return SW_INVALID;
        for(j=0;j<i;j++)if((p->rates[i]&127)==(p->rates[j]&127))return SW_INVALID;
    }
    header(b,0,&p->peer);le16(b+24,p->capability);le16(b+26,p->listen_interval);
    b[n++]=0;b[n++]=(uint8_t)p->ssid_len;memcpy(b+n,p->ssid,p->ssid_len);n+=p->ssid_len;
    first=p->rates_len<8?p->rates_len:8;
    b[n++]=1;b[n++]=(uint8_t)first;memcpy(b+n,p->rates,first);n+=first;
    if(p->rates_len>8) {b[n++]=50;b[n++]=(uint8_t)(p->rates_len-8);
        memcpy(b+n,p->rates+8,p->rates_len-8);n+=p->rates_len-8;}
    memcpy(b+n,rsn,sizeof(rsn));n+=sizeof(rsn);
    return finish(out,cap,used,b,n);
}
static int key_ok(const struct sw_key *p) {
    return p&&unicast(p->ap)&&p->pairwise<=1&&p->key_id<=3&&
           (!p->pairwise||p->key_id==0)&&p->wlan!=255;
}
static int key(uint8_t *out,size_t cap,size_t *used,const struct sw_key *p,int add) {
    uint8_t b[SW_SIZE_CMD_802_11_KEY]={0};
    if(!start(used)||!key_ok(p)||(add&&(!p->key||p->key_len!=16)))return SW_INVALID;
    P8(CMD_802_11_KEY,ucAddRemove,add);
    P8(CMD_802_11_KEY,ucKeyType,p->pairwise);
    CP(CMD_802_11_KEY,aucPeerAddr,p->ap,6);
    P8(CMD_802_11_KEY,ucBssIdx,p->bss);
    P8(CMD_802_11_KEY,ucKeyId,p->key_id);
    P8(CMD_802_11_KEY,ucWlanIndex,p->wlan);
    if(add) {
        P8(CMD_802_11_KEY,ucTxKey,p->pairwise);
        P8(CMD_802_11_KEY,ucAlgorithmId,4);
        P8(CMD_802_11_KEY,ucKeyLen,16);
        CP(CMD_802_11_KEY,aucKeyMaterial,p->key,16);
    }
    /* Erase temporary key material after copying or rejecting capacity. */
    { int r=finish(out,cap,used,b,sizeof(b));volatile uint8_t *v=b;size_t n=sizeof(b);
      while(n--)*v++=0;
      return r; }
}
int sw_key_add(uint8_t *o,size_t c,size_t *u,const struct sw_key *p) {return key(o,c,u,p,1);}
int sw_key_remove(uint8_t *o,size_t c,size_t *u,const struct sw_key *p) {return key(o,c,u,p,0);}
static int channel_ok(uint8_t band,uint8_t channel) {
    /* Syntactic RF range only; real regulatory/DFS policy belongs to caller. */
    return (band==1&&channel>=1&&channel<=14)||(band==2&&channel>=32&&channel<=177);
}
int sw_channel(uint8_t *out,size_t cap,size_t *used,const struct sw_channel *p) {
    uint8_t b[SW_SIZE_CMD_CH_PRIVILEGE]={0};
    if(!start(used)||!p||p->abort>1||(p->dbdc_band!=0&&p->dbdc_band!=1&&p->dbdc_band!=4)||
       (!p->abort&&(!channel_ok(p->band,p->channel)||!p->interval_ms)))return SW_INVALID;
    P8(CMD_CH_PRIVILEGE,ucBssIndex,p->bss);P8(CMD_CH_PRIVILEGE,ucTokenID,p->token);
    P8(CMD_CH_PRIVILEGE,ucAction,p->abort);P8(CMD_CH_PRIVILEGE,ucDBDCBand,p->dbdc_band);
    if(!p->abort) {
        P8(CMD_CH_PRIVILEGE,ucPrimaryChannel,p->channel);
        P8(CMD_CH_PRIVILEGE,ucRfBand,p->band);
        P32(CMD_CH_PRIVILEGE,u4MaxInterval,p->interval_ms);
    }
    return finish(out,cap,used,b,sizeof(b));
}
static int rlm_ok(const struct sw_rlm *p) {
    return p&&channel_ok(p->band,p->channel)&&p->erp_protect<=1&&p->short_preamble<=1&&
        p->short_slot<=1&&p->tx_nss==1&&p->rx_nss==1;
}
static void rlm(uint8_t *b,const struct sw_rlm *p) {
    P8(CMD_SET_BSS_RLM_PARAM,ucBssIndex,p->bss);P8(CMD_SET_BSS_RLM_PARAM,ucRfBand,p->band);
    P8(CMD_SET_BSS_RLM_PARAM,ucPrimaryChannel,p->channel);
    P8(CMD_SET_BSS_RLM_PARAM,ucErpProtectMode,p->erp_protect);
    P8(CMD_SET_BSS_RLM_PARAM,ucUseShortPreamble,p->short_preamble);
    P8(CMD_SET_BSS_RLM_PARAM,ucUseShortSlotTime,p->short_slot);
    P8(CMD_SET_BSS_RLM_PARAM,ucTxNss,p->tx_nss);P8(CMD_SET_BSS_RLM_PARAM,ucRxNss,p->rx_nss);
}
int sw_rlm(uint8_t *out,size_t cap,size_t *used,const struct sw_rlm *p) {
    uint8_t b[SW_SIZE_CMD_SET_BSS_RLM_PARAM]={0};
    if(!start(used)||!rlm_ok(p))return SW_INVALID;
    rlm(b,p);return finish(out,cap,used,b,sizeof(b));
}
static int rates_ok(uint16_t rates,uint16_t basic,uint8_t phy) {
    return rates&&basic&&!(rates&~0x3fcfu)&&!(basic&~rates)&&phy&&!(phy&~0xfu);
}
int sw_bss(uint8_t *out,size_t cap,size_t *used,const struct sw_bss *p) {
    uint8_t b[SW_SIZE_CMD_SET_BSS_INFO]={0};
    if(!start(used)||!p||!rlm_ok(&p->rlm)||!p->ssid||!p->ssid_len||p->ssid_len>32||
       !unicast(p->ap)||p->connected>1||(p->dbdc_band!=0&&p->dbdc_band!=1&&p->dbdc_band!=4)||p->nonht_phy>3||
       p->disconnect_threshold||!rates_ok(p->rates,p->basic_rates,p->phy_set))return SW_INVALID;
    P8(CMD_SET_BSS_INFO,ucBssIndex,p->rlm.bss);
    P8(CMD_SET_BSS_INFO,ucConnectionState,p->connected?0:1);
    P8(CMD_SET_BSS_INFO,ucSSIDLen,p->ssid_len);CP(CMD_SET_BSS_INFO,aucSSID,p->ssid,p->ssid_len);
    CP(CMD_SET_BSS_INFO,aucBSSID,p->ap,6);P16(CMD_SET_BSS_INFO,u2OperationalRateSet,p->rates);
    P16(CMD_SET_BSS_INFO,u2BSSBasicRateSet,p->basic_rates);
    P8(CMD_SET_BSS_INFO,ucStaRecIdxOfAP,p->ap_sta);
    P16(CMD_SET_BSS_INFO,u2HwDefaultFixedRateCode,p->fixed_rate);
    P8(CMD_SET_BSS_INFO,ucNonHTBasicPhyType,p->nonht_phy);
    P8(CMD_SET_BSS_INFO,ucAuthMode,7);P8(CMD_SET_BSS_INFO,ucEncStatus,6);
    P8(CMD_SET_BSS_INFO,ucPhyTypeSet,p->phy_set);P8(CMD_SET_BSS_INFO,ucBMCWlanIndex,p->bmc_wlan);
    P32(CMD_SET_BSS_INFO,u4PrivateData,p->private_data);
    rlm(b+OFF(CMD_SET_BSS_INFO,rBssRlmParam),&p->rlm);
    P8(CMD_SET_BSS_INFO,ucDBDCBand,p->dbdc_band);P8(CMD_SET_BSS_INFO,ucWmmSet,p->wmm_set);
    P8(CMD_SET_BSS_INFO,ucNss,1);
    return finish(out,cap,used,b,sizeof(b));
}
int sw_sta(uint8_t *out,size_t cap,size_t *used,const struct sw_sta *p) {
    uint8_t b[SW_SIZE_CMD_UPDATE_STA_RECORD]={0};
    if(!start(used)||!p||!unicast(p->ap)||p->state<1||p->state>3||p->need_response>1||
       p->sta_type!=65||p->aid>2007||!p->listen_interval||p->wlan==255||p->bmc_wlan!=SW_WTBL_RESERVED_ENTRY||p->version>1||
       !rates_ok(p->rates,p->basic_rates,p->phy_set))return SW_INVALID;
    P8(CMD_UPDATE_STA_RECORD,ucStaIndex,p->sta);P8(CMD_UPDATE_STA_RECORD,ucStaType,p->sta_type);
    CP(CMD_UPDATE_STA_RECORD,aucMacAddr,p->ap,6);P16(CMD_UPDATE_STA_RECORD,u2AssocId,p->aid);
    P16(CMD_UPDATE_STA_RECORD,u2ListenInterval,p->listen_interval);
    P8(CMD_UPDATE_STA_RECORD,ucBssIndex,p->bss);P8(CMD_UPDATE_STA_RECORD,ucDesiredPhyTypeSet,p->phy_set);
    P16(CMD_UPDATE_STA_RECORD,u2DesiredNonHTRateSet,p->rates);
    P16(CMD_UPDATE_STA_RECORD,u2BSSBasicRateSet,p->basic_rates);
    P8(CMD_UPDATE_STA_RECORD,ucStaState,p->state);P8(CMD_UPDATE_STA_RECORD,ucRCPI,p->rcpi);
    P8(CMD_UPDATE_STA_RECORD,ucVersion,p->version);
    P8(CMD_UPDATE_STA_RECORD,ucNeedResp,p->need_response);P8(CMD_UPDATE_STA_RECORD,ucWlanIndex,p->wlan);
    P8(CMD_UPDATE_STA_RECORD,ucBMCWlanIndex,SW_WTBL_RESERVED_ENTRY);
    P8(CMD_UPDATE_STA_RECORD,ucRtsPolicy,p->rts_policy);
    P16(CMD_UPDATE_STA_RECORD,u2HwDefaultFixedRateCode,p->fixed_rate);
    return finish(out,cap,used,b,sizeof(b));
}
static int txd(uint8_t *out,size_t cap,size_t *used,const struct sw_txd *p,
               const uint8_t *frame,size_t len,int security) {
    uint8_t b[32]={0};uint32_t d0,d1,d2,d3,d5,d6,d7=0,life;uint16_t fc=0;
    if(!start(used)||!p||!frame||len>65503||p->own_mac>63||p->queue>127||
       p->wlan==255||!p->pid||p->pid>=128||p->retry_limit>31||!p->retry_limit||
       p->fixed_rate>0x3fff||p->protected_frame>1||p->spe_index>31||p->pse_header_len>230||
       (p->pse_header_len&1)||!p->lifetime_ms||p->lifetime_ms>60000)return SW_INVALID;
    if(security==1) {
        if(len<18||len>1518||frame[12]!=0x88||frame[13]!=0x8e||
           (((size_t)frame[16]<<8)|frame[17])!=len-18)return SW_INVALID;
    } else if(security==2) {
        if(len<14||len>1514||((unsigned)frame[12]<<8|frame[13])<1536||
           (frame[12]==0x88&&frame[13]==0x8e)||!p->protected_frame)return SW_INVALID;
    } else {
        if(len<26||p->protected_frame)return SW_INVALID;
        fc=rd16(frame);
        if(fc!=0xb0&&fc!=0&&fc!=0xc0)return SW_INVALID;
        if(fc==0xc0&&(len!=26||rd16(frame+24)!=3))return SW_INVALID;
        if(fc==0&&len<28)return SW_INVALID;
        if(fc==0xb0&&(len!=30||rd16(frame+24)||rd16(frame+26)!=1||rd16(frame+28)))return SW_INVALID;
    }
    life=(p->lifetime_ms*1000u/1024u)>>6;
    /* Match pinned macro, including its unreachable minimum-life branch.
     * Reject life=0 instead of silently encoding no timeout. */
    if(!life)return SW_INVALID;
    if(life>127)life=127;
    d0=(uint32_t)(32+len)|(((p->pse_header_len+(security?12u:24u))>>1)<<16)|
        (2u<<23)|((uint32_t)p->queue<<25);
    d1=p->wlan|(UINT32_C(1)<<31)|((uint32_t)p->own_mac<<24);
    if(security){d1|=1u<<15;if(security==2)d1|=1u<<10;}
    else d1|=(1u<<10)|(2u<<16)|(12u<<11);
    d2=(life<<16)|(UINT32_C(1)<<31);
    if(!security) {d2|=(fc>>4)&15u;d7=(((uint32_t)(fc>>4)&15u)<<16)|((uint32_t)p->spe_index<<11);}
    d3=((uint32_t)p->retry_limit<<11)|(p->protected_frame?2u:0u);
    d5=p->pid|(1u<<9);d6=(uint32_t)p->fixed_rate<<16;
    le32(b,d0);le32(b+4,d1);le32(b+8,d2);le32(b+12,d3);
    le32(b+20,d5);le32(b+24,d6);le32(b+28,d7);
    return finish(out,cap,used,b,sizeof(b));
}
int sw_management_txd(uint8_t *o,size_t c,size_t *u,const struct sw_txd *p,const uint8_t *f,size_t n) {return txd(o,c,u,p,f,n,0);}
int sw_security_txd(uint8_t *o,size_t c,size_t *u,const struct sw_txd *p,const uint8_t *f,size_t n) {return txd(o,c,u,p,f,n,1);}

int sw_data_txd(uint8_t *o,size_t c,size_t *u,const struct sw_txd *p,const uint8_t *f,size_t n) {return txd(o,c,u,p,f,n,2);}
