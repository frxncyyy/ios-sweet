#!/usr/bin/env python3
"""Host-only native normal backend integration; no device or firmware IO."""
from pathlib import Path
import argparse,hashlib,json,subprocess,tempfile
R=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--board',type=Path,default=R/'research/nord2/wifi/calibration/WIFI',help='Read-only authenticated 6146-byte board file; never embedded in fixtures/output')
parser.add_argument('--only',choices=('all','full','faults','sleepy','sleepy-faults','sleepy-decoder'),default='all')
parser.add_argument('--cc',default='clang')
parser.add_argument('--log-dir',type=Path,help='Optional directory for synthetic captured test output')
parser.add_argument('--report',type=Path,help='Optional JSON source/fixture hashes and summaries, no packet/board bytes')
args=parser.parse_args();S=R/'shim/nord2-xnu';P=R/'tools/fixtures/nord2-wifi-normal'
board=args.board.resolve()
expected='61ac3300aad6f65364f845894bb982b7c5d077346f788c29df019c93c4f3f89f'
if not board.is_file() or board.stat().st_size!=6146 or hashlib.sha256(board.read_bytes()).hexdigest()!=expected:parser.error('board identity/extent mismatch')
names=('wifi_default_feature','wifi_rx_readback','wifi_normal_backend','wifi_normal_config','wifi_normal_init','wifi_normal_rx','wifi_normal_transport','wifi_regulatory_policy','wifi_scan_bss','wifi_scan_plan','wifi_basic_config','wifi_basic_resources','wifi_basic_metadata','wifi_basic_primitives','wifi_capability','wifi_capability_dispatcher','wifi_main_initializer','wifi_main_dmashdl','wifi_main_init_plan','wifi_main_transport','wifi_main_layout','wifi_dma_preflight','wifi_main_command','dma_pool_contract')
files=[S/(name+'.c') for name in names]
sha=lambda f:hashlib.sha256(f.read_bytes()).hexdigest()
sources={str(x.relative_to(R)):sha(x) for x in S.iterdir() if x.is_file() and x.suffix in ('.c','.h','.inc')}
fixtures={str(x.relative_to(R)):sha(x) for x in P.iterdir() if x.is_file()}
report={'status':'PASS_SYNTHETIC_ONLY','native_owner_mocked':False,'checked_io_replaced':False,'board_sha256':expected,'sources':sources,'fixtures':fixtures,'summaries':[]}
with tempfile.TemporaryDirectory(prefix='nord2-normal-') as td:
 d=Path(td);adapter=(S/'wifi_capability_bind.inc').read_text()
 adapter=adapter.replace('static int ncd_native_mread(void*c,uint64_t p,void*v,uint32_t n)\n{','static int ncd_native_mread(void*c,uint64_t p,void*v,uint32_t n)\n{full_test_probe(0);')
 adapter=adapter.replace('static int ncd_native_write(void*c,uint32_t p,uint32_t v)\n{','static int ncd_native_write(void*c,uint32_t p,uint32_t v)\n{nrd_forbid_write();full_test_probe(1);')
 adapter=adapter.replace('static int ncd_native_mwrite(void*c,uint64_t p,const void*v,uint32_t n)\n{','static int ncd_native_mwrite(void*c,uint64_t p,const void*v,uint32_t n)\n{nrd_forbid_write();')
 import re
 for name in ('owner','read','write','mread','mwrite','sync','delay','now'):
  adapter,n=re.subn(r'(static int ncd_native_'+name+r'\([^)]*\)\s*\{)',r'\1\n#ifdef FULL_FAULTS\nif(full_test_fail())return -1;\n#endif\n',adapter,count=1)
  if n!=1:raise RuntimeError('callback fixture schema mismatch: '+name)
 (d/'wifi_capability_bind.inc').write_text(adapter)
 basic=(S/'wifi_basic_config_bind.inc').read_text();marker=' int result=nbc_prepare'
 if basic.count(marker)!=1:raise RuntimeError('preCID2 firmware-arrival fixture schema mismatch')
 (d/'wifi_basic_config_bind.inc').write_text(basic.replace(marker,' sleepy_pre_basic();\n'+marker))
 # Only the call site is instrumented; the canonical readback helper and native owner remain unchanged.
 rx=(S/'wifi_normal_rx.c').read_text()
 marker='#include \"wifi_normal_rx.h\"'
 if rx.count(marker)!=1:raise RuntimeError('readback call-site fixture schema mismatch')
 rx=rx.replace(marker,marker+'\nextern int nrd_host_probe(const struct ncd_io*,void*,struct ncr_state*,struct nrd_snapshot*,unsigned);\n#define nrd_capture nrd_host_probe')
 (d/'wifi_normal_rx.c').write_text(rx)
 files=[d/'wifi_normal_rx.c' if x.name=='wifi_normal_rx.c' else x for x in files]
 for name in (('full','faults','sleepy','sleepy-faults','sleepy-decoder') if args.only=='all' else (args.only,)):
  exe=d/name;cmd=[args.cc,'-std=c11','-DNCFG_HOST_TEST','-DNSS_HOST_TEST','-pthread','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-fno-omit-frame-pointer','-I'+str(d),'-I'+str(S),'-I'+str(P),str(P/('test-'+name+'.c'))]+list(map(str,[] if name=='sleepy-decoder' else files))+['-lcrypto','-o',str(exe)]
  subprocess.run(cmd,check=True);run=subprocess.run([str(exe),str(board)],capture_output=True,text=True)
  if run.returncode:
   print(run.stdout[-6000:]);print(run.stderr);run.check_returncode()
  if args.log_dir:
   args.log_dir.mkdir(parents=True,exist_ok=True);(args.log_dir/(name+'-native.log')).write_text(run.stdout)
  summary=run.stdout.splitlines()[-1];report['summaries'].append(summary);print(summary)
 if sources!={str(x.relative_to(R)):sha(x) for x in S.iterdir() if x.is_file() and x.suffix in ('.c','.h','.inc')}:raise RuntimeError('source changed during test')
 if fixtures!={str(x.relative_to(R)):sha(x) for x in P.iterdir() if x.is_file()}:raise RuntimeError('fixture changed during test')
if args.report:args.report.write_text(json.dumps(report,indent=2)+'\n')
