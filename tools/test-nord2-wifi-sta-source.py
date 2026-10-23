#!/usr/bin/env python3
"""Pin wire STA classes to original gen4m constants, independently of fixtures."""
from pathlib import Path
import argparse,hashlib,json,re,subprocess,tempfile
R=next(p for p in Path(__file__).resolve().parents if (p/'AGENTS.md').exists())
a=argparse.ArgumentParser();a.add_argument('--android-tree',type=Path,default=R.parent/'nord2-current-kernel');a.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');a.add_argument('--log-dir',type=Path,required=True);x=a.parse_args();x.log_dir.mkdir(parents=True,exist_ok=False)
pin='04d00c2565764c3922c9b4826b5acff1106e533f';assert subprocess.check_output(['git','-C',str(x.android_tree),'rev-parse','HEAD'],text=True).strip()==pin
G=x.android_tree/'drivers/misc/mediatek/connectivity/wlan/core/gen4m';mac=G/'include/nic/mac.h';cnm=G/'mgmt/cnm_mem.c'
text=mac.read_text();definitions=[]
for n in range(1,4):
 m=re.search(r'^#define\s+STA_STATE_'+str(n)+r'\s+([^\n]+)',text,re.M);assert m;definitions.append(m.group(0))
assert 'prCmdContent->ucStaState = prStaRec->ucStaState;' in cnm.read_text()
source='''#include "wifi_station_wire.h"
#include "wifi_station_source_layout.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
'''+ '\n'.join(definitions)+'''
_Static_assert(SW_STA_CLASS_1 == STA_STATE_1,"class1 wire mismatch");
_Static_assert(SW_STA_CLASS_2 == STA_STATE_2,"class2 wire mismatch");
_Static_assert(SW_STA_CLASS_3 == STA_STATE_3,"class3 wire mismatch");
int main(void){unsigned checks=0;uint8_t out[200],old[200];memset(old,0x5a,sizeof old);size_t used;
struct sw_sta p={.ap={2,0,0,0,0,2},.sta_type=65,.wlan=5,.bmc_wlan=255,.listen_interval=10,.rates=15,.basic_rates=3,.phy_set=1,.version=1};
const unsigned state[]={STA_STATE_1,STA_STATE_2,STA_STATE_3};
for(unsigned n=0;n<3;n++){p.state=(uint8_t)state[n];p.need_response=n==2;assert(!sw_sta(out,sizeof out,&used,&p)&&used==200&&out[SW_OFF_CMD_UPDATE_STA_RECORD_ucStaState]==state[n]&&out[SW_OFF_CMD_UPDATE_STA_RECORD_ucNeedResp]==(n==2)&&out[SW_OFF_CMD_UPDATE_STA_RECORD_ucBMCWlanIndex]==255);checks++;}
for(unsigned n=3;n<256;n++){p.state=(uint8_t)n;memcpy(out,old,sizeof out);assert(sw_sta(out,sizeof out,&used,&p)==SW_INVALID&&!used&&!memcmp(out,old,sizeof out));checks++;}
printf("PASS %u original-source STA class/response/bounds checks\\n",checks);return 0;}
'''
with tempfile.TemporaryDirectory() as z:
 d=Path(z);(d/'test.c').write_text(source);cmd=['clang','-std=c11','-O1','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(x.candidate),str(d/'test.c'),str(x.candidate/'wifi_station_wire.c'),'-o',str(d/'test')]
 c=subprocess.run(cmd,capture_output=True,text=True);(x.log_dir/'compile.log').write_text(c.stdout+c.stderr);c.check_returncode();v=subprocess.run([str(d/'test')],capture_output=True,text=True);(x.log_dir/'test.log').write_text(v.stdout+v.stderr);v.check_returncode();print(v.stdout,end='')
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest();(x.log_dir/'report.json').write_text(json.dumps({'passed':True,'hardware_claim':False,'android_commit':pin,'source_sha256':{str(p.relative_to(G)):sha(p) for p in [mac,cnm]},'codec_sha256':sha(x.candidate/'wifi_station_wire.c'),'header_sha256':sha(x.candidate/'wifi_station_wire.h')},indent=2)+'\n')
