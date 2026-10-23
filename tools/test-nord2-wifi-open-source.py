#!/usr/bin/env python3
from pathlib import Path
import re,subprocess,tempfile,argparse,hashlib,json
R=next(p for p in Path(__file__).resolve().parents if(p/'AGENTS.md').exists());a=argparse.ArgumentParser();a.add_argument('--android-tree',type=Path,required=True);a.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');a.add_argument('--output',type=Path,required=True);x=a.parse_args();G=x.android_tree/'drivers/misc/mediatek/connectivity/wlan/core/gen4m';x.output.mkdir(parents=True,exist_ok=False)
assert subprocess.check_output(['git','-C',str(x.android_tree),'rev-parse','HEAD'],text=True).strip()=='04d00c2565764c3922c9b4826b5acff1106e533f'
def decl(name,kind='struct'):
 s=re.sub(r'/\*.*?\*/|//[^\n]*','',(G/'include/wsys_cmd_handler_fw.h').read_text(),flags=re.S);m=re.search(kind+' '+name+r'\s*\{',s);assert m;at=m.end();depth=1
 while depth:depth+=(s[at]=='{')-(s[at]=='}');at+=1
 return s[m.start():s.index(';',at)+1]+'\n'
s=(G/'mgmt/privacy.c').read_text();at=s.index('u_int8_t secEnabledInAis(');end=s.index('}\t',at)+1;function=s[at:end]
pre='''#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#define HE_OP_BYTE_NUM 3
#define CFG_SUPPORT_CONNAC2X 1
#define BIT(n) (UINT32_C(1)<<(n))
#define BITS(a,b) ((UINT32_MAX>>(31-(b))) & (UINT32_MAX<<(a)))
#define TX_DESC_SET_FIELD(d,v,m,s) ((d)=((d)&~(m))|(((uint32_t)(v)<<(s))&(m)))
#define MSEC_TO_USEC(x) ((x)*1000)
#define USEC_PER_TU 1024
#define TX_DESC_TX_TIME_NO_LIMIT 0
#define TRUE 1
#define FALSE 0
#define IN
#define DEBUGFUNC(x)
#define DBGLOG(...) ((void)0)
typedef uint8_t u_int8_t;
'''
header=re.sub(r'^\s*#include[^\n]*','',(G/'include/nic/nic_connac2x_tx.h').read_text(),flags=re.M)
c=pre+decl('ENUM_PARAM_AUTH_MODE','enum')+decl('ENUM_WEP_STATUS','enum')+decl('CMD_SET_BSS_RLM_PARAM')+decl('CMD_SET_BSS_INFO')+header+'''
struct ADAPTER {enum ENUM_WEP_STATUS enc;};
static enum ENUM_WEP_STATUS aisGetEncStatus(struct ADAPTER*a,uint8_t i){assert(i==0);return a->enc;}
'''+function+'''
#include "wifi_station_wire.h"
#include "wifi_station_tx.h"
int main(void){struct ADAPTER a={ENUM_ENCRYPTION_DISABLED};assert(AUTH_MODE_OPEN==0&&ENUM_ENCRYPTION_DISABLED==1&&ENUM_ENCRYPTION1_ENABLED==0&&!secEnabledInAis(&a,0));
uint8_t out[4096];size_t used;struct sw_bss p={.rlm={0,1,1,0,0,0,1,1},.ssid=(const uint8_t*)"open",.ssid_len=4,.ap={2,0,0,0,0,2},.connected=1,.ap_sta=0,.bmc_wlan=4,.nonht_phy=0,.phy_set=1,.dbdc_band=4,.rates=15,.basic_rates=3,.security=SW_SECURITY_OPEN};
struct CMD_SET_BSS_INFO b={0};b.ucSSIDLen=4;memcpy(b.aucSSID,p.ssid,4);memcpy(b.aucBSSID,p.ap,6);b.u2OperationalRateSet=15;b.u2BSSBasicRateSet=3;b.ucAuthMode=AUTH_MODE_OPEN;b.ucEncStatus=a.enc;b.ucPhyTypeSet=1;b.ucBMCWlanIndex=4;b.rBssRlmParam.ucRfBand=1;b.rBssRlmParam.ucPrimaryChannel=1;b.rBssRlmParam.ucTxNss=b.rBssRlmParam.ucRxNss=1;b.ucDBDCBand=4;b.ucNss=1;
assert(!sw_bss(out,4096,&used,&p)&&used==sizeof b&&!memcmp(out,&b,sizeof b));
unsigned checks=1;uint8_t eth[60]={0};memset(eth,255,6);eth[6]=2;eth[11]=1;eth[12]=8;eth[13]=6;
for(unsigned secure=0;secure<2;secure++)for(unsigned pid=1;pid<128;pid+=7){a.enc=secure?ENUM_ENCRYPTION3_ENABLED:ENUM_ENCRYPTION_DISABLED;struct sw_txd d={.wlan=5,.own_mac=1,.queue=1,.pid=pid,.retry_limit=30,.fixed_rate=11,.pse_header_len=8,.lifetime_ms=2000,.protected_frame=secEnabledInAis(&a,0)};struct HW_MAC_CONNAC2X_TX_DESC t={0};
HAL_MAC_CONNAC2X_TXD_SET_TX_BYTE_COUNT(&t,92);HAL_MAC_CONNAC2X_TXD_SET_ETHER_TYPE_OFFSET(&t,10);HAL_MAC_CONNAC2X_TXD_SET_QUEUE_INDEX(&t,1);HAL_MAC_CONNAC2X_TXD_SET_PKT_FORMAT(&t,0);HAL_MAC_CONNAC2X_TXD_SET_WLAN_INDEX(&t,5);HAL_MAC_CONNAC2X_TXD_SET_VTA(&t,1);HAL_MAC_CONNAC2X_TXD_SET_ETHERNET_II(&t);HAL_MAC_CONNAC2X_TXD_SET_LONG_FORMAT(&t);HAL_MAC_CONNAC2X_TXD_SET_OWN_MAC_INDEX(&t,1);HAL_MAC_CONNAC2X_TXD_SET_REMAINING_LIFE_TIME_IN_MS(&t,2000);HAL_MAC_CONNAC2X_TXD_SET_REMAINING_TX_COUNT(&t,30);HAL_MAC_CONNAC2X_TXD_SET_FIXED_RATE_ENABLE(&t);HAL_MAC_CONNAC2X_TXD_SET_FR_RATE(&t,11);HAL_MAC_CONNAC2X_TXD_SET_BA_DISABLE(&t);HAL_MAC_CONNAC2X_TXD_SET_PID(&t,pid);HAL_MAC_CONNAC2X_TXD_SET_TXS_TO_MCU(&t);if(secEnabledInAis(&a,0))HAL_MAC_CONNAC2X_TXD_SET_PROTECTION(&t);
assert(!nstx_build(out,4096,&used,&d,eth,60,0,17,0x90855000)&&used==124&&!memcmp(out,&t,32));checks++;}
printf("PASS %u selected Android OPEN/WPA BSS and full TXD/protection oracles\\n",checks);return 0;}
'''
with tempfile.TemporaryDirectory() as z:
 T=Path(z);(T/'oracle.c').write_text(c);P=x.candidate
 for S in [R/'shim/nord2-xnu',P]:
  for f in S.iterdir():
   if f.suffix in ('.c','.h','.inc'):(T/f.name).write_bytes(f.read_bytes())
 cmd=['clang','-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(T),str(T/'oracle.c'),str(T/'wifi_station_wire.c'),str(T/'wifi_station_tx.c'),'-o',str(T/'test')];v=subprocess.run(cmd,capture_output=True,text=True);(x.output/'compile.log').write_text(v.stdout+v.stderr);v.check_returncode();v=subprocess.run([str(T/'test')],capture_output=True,text=True);(x.output/'test.log').write_text(v.stdout+v.stderr);v.check_returncode();print(v.stdout,end='')
(x.output/'report.json').write_text(json.dumps({'passed':True,'hardware_claim':False,'source_sha256':{p:hashlib.sha256((G/p).read_bytes()).hexdigest() for p in ['include/wsys_cmd_handler_fw.h','include/nic/nic_connac2x_tx.h','mgmt/privacy.c']},'summary':v.stdout.strip()},indent=2)+'\n')
