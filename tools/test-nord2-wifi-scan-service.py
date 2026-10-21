#!/usr/bin/env python3
"""Host-only repeated scan fixture against canonical sources; no device IO."""
from pathlib import Path
import tempfile,subprocess,json,argparse
a=argparse.ArgumentParser();a.add_argument("--log-dir",type=Path);args=a.parse_args()
R=Path(__file__).resolve().parents[1];P=R/'tools/fixtures/nord2-wifi-scan-service';S=R/'shim/nord2-xnu';F=R/'tools/fixtures/nord2-wifi-normal'
names=('wifi_default_feature','wifi_rx_readback','wifi_normal_backend','wifi_normal_config','wifi_normal_init','wifi_normal_rx','wifi_normal_transport','wifi_regulatory_policy','wifi_scan_bss','wifi_scan_plan','wifi_basic_config','wifi_basic_resources','wifi_basic_metadata','wifi_basic_primitives','wifi_capability','wifi_capability_dispatcher','wifi_main_initializer','wifi_main_dmashdl','wifi_main_init_plan','wifi_main_transport','wifi_main_layout','wifi_dma_preflight','wifi_main_command','dma_pool_contract')
with tempfile.TemporaryDirectory(prefix='nss-native-') as td:
 d=Path(td)
 for f in S.iterdir():
  if f.suffix in ('.h','.inc','.c'):(d/f.name).write_bytes(f.read_bytes())
 (d/'nss_host.h').write_bytes((F/'nss_host.h').read_bytes())
 for f in F.iterdir():
  if f.suffix=='.inc':(d/f.name).write_bytes(f.read_bytes())
 owner=(d/'owner-fixture.inc').read_text().replace('static unsigned cap_flag=1,','static unsigned ios_flag=1;\nstatic unsigned cap_flag=1,').replace('!strcmp(s,"nord2wifiios")?0:', '!strcmp(s,"nord2wifiios")?ios_flag:')
 (d/'owner-fixture.inc').write_text(owner)
 # Existing full native fixture's probes and source-owned board/auth init remain.
 test=(F/'test-full.c').read_text();test=test[:test.index('int main(')]
 test=test.replace('assert(!delivered++&&nct_native.session.cursor==(NCB_COMMANDS-1)&&nct_native.session.record[NCB_COMMANDS-1].publication_attempted&&!nct_native.session.record[NCB_COMMANDS-1].published);','assert(!delivered++);unsigned ix=nct_native.session.continuation?0:NCB_COMMANDS-1;assert(nct_native.session.cursor==ix&&nct_native.session.record[ix].publication_attempted&&!nct_native.session.record[ix].published);')
 test=test.replace('p[36]=1;p[40]', 'p[36]=cmd[64];p[40]')
 test=test.replace('static unsigned mode,delivered,probe_memory,probe_write,delayed_trigger;', 'static unsigned refill_drain;\nstatic uint64_t cancel_inflight;\nstatic unsigned mode,delivered,probe_memory,probe_write,delayed_trigger;').replace('if(cmd[36]!=3)return;', 'if(cmd[36]!=3)return; if(cancel_inflight){assert(!nord2_wifi_scan_cancel(cancel_inflight));cancel_inflight=0;}')
 test=test.replace('probe_write|=1u<<r->current;', 'probe_write|=1u<<r->current;if(refill_drain&&nct_native.session.phase==NCT_DRAIN&&r->current==0){unsigned future=(q->next+1)%q->ring.count;uint8_t*dd=pool_memory+(q->ring.descriptors-NDP_POOL_BASE)+16*future;put32(dd+4,0xc0000000u);hardware_register(q->ring.base+12,(future+1)%q->ring.count);}')
 test+=(P/'continuation-main.inc').read_text()
 (d/'test.c').write_text(test)
 # Same existing readback fixture seam; owner/checked IO are not replaced.
 rx=(d/'wifi_normal_rx.c').read_text().replace('#include "wifi_normal_rx.h"','#include "wifi_normal_rx.h"\nextern int nrd_host_probe(const struct ncd_io*,void*,struct ncr_state*,struct nrd_snapshot*,unsigned);\n#define nrd_capture nrd_host_probe');(d/'wifi_normal_rx.c').write_text(rx)
 basic=(d/'wifi_basic_config_bind.inc').read_text().replace(' int result=nbc_prepare',' sleepy_pre_basic();\n int result=nbc_prepare');(d/'wifi_basic_config_bind.inc').write_text(basic)
 cap=(d/'wifi_capability_bind.inc').read_text().replace('static int ncd_native_mread(void*c,uint64_t p,void*v,uint32_t n)\n{','static int ncd_native_mread(void*c,uint64_t p,void*v,uint32_t n)\n{full_test_probe(0);').replace('static int ncd_native_write(void*c,uint32_t p,uint32_t v)\n{','static int ncd_native_write(void*c,uint32_t p,uint32_t v)\n{nrd_forbid_write();full_test_probe(1);').replace('static int ncd_native_mwrite(void*c,uint64_t p,const void*v,uint32_t n)\n{','static int ncd_native_mwrite(void*c,uint64_t p,const void*v,uint32_t n)\n{nrd_forbid_write();');(d/'wifi_capability_bind.inc').write_text(cap)
 cmd=['clang','-std=c11','-DNSS_HOST_TEST','-DNCFG_HOST_TEST','-O1','-g','-Wall','-Wextra','-Werror','-pthread','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(d),str(d/'test.c'),*[str(d/(name+'.c')) for name in names],'-lcrypto','-o',str(d/'test')];subprocess.run(cmd,check=True);result=subprocess.run([str(d/'test'),str(R/'research/nord2/wifi/calibration/WIFI')],capture_output=True,text=True);
 if args.log_dir:args.log_dir.mkdir(parents=True,exist_ok=True);(args.log_dir/'test-native.log').write_text(result.stdout+result.stderr)
 print(result.stdout.splitlines()[-1:] if result.stdout else result.stderr);result.check_returncode()
 off=subprocess.run([str(d/'test'),str(R/'research/nord2/wifi/calibration/WIFI'),'off'],capture_output=True,text=True);print(off.stdout.splitlines()[-1:] if off.stdout else off.stderr);off.check_returncode()
 if args.log_dir:(args.log_dir/'test-default-off.log').write_text(off.stdout+off.stderr)
