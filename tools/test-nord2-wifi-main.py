#!/usr/bin/env python3
"""Host checks of the actual Nord2 Wi-Fi init/transport; no device access."""
from pathlib import Path
import subprocess,tempfile,argparse
ap=argparse.ArgumentParser();ap.add_argument("--private-fixtures",action="store_true");args=ap.parse_args()
R=Path(__file__).resolve().parents[1];S=R/'shim/nord2-xnu';F=R/'tools/fixtures/nord2-wifi-main'
with tempfile.TemporaryDirectory(prefix='nord2-wifi-main-') as d:
 for test,names in [
  ('command',['command']),('plan',['init_plan']),
  ('dmashdl',['dmashdl','init_plan']),
  ('transport',['transport','command','layout']),
  ('initializer',['initializer','dmashdl','init_plan','transport','command','layout']),
  ('native',['initializer','dmashdl','init_plan','transport','command','layout']),
  ('halt',['transport','command','layout','phy_exchange','phy_force','phy_backup'])
 ]+([('spans',['fw_place','phy_force','phy_backup','layout'])] if args.private_fixtures else []):
  files=[S/('wifi_main_'+n+'.c') for n in names]
  if test in ('dmashdl','initializer','native'):files += [S/'wifi_dma_preflight.c']
  if test in ('initializer','native'):files += [S/'dma_pool_contract.c']
  exe=Path(d)/test
  subprocess.run(['cc','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-omit-frame-pointer','-fno-sanitize-recover=all','-DNFA_HOST_TEST','-I'+str(S),str(F/('test-'+test+'.c')),*map(str,files),'-lcrypto','-o',str(exe)],check=True)
  extra=[]
  if test=='spans':
   extra=[str(R/'research/nord2/wifi/calibration/WIFI'),str(R/'research/nord2/wifi/firmware/WIFI_RAM_CODE_soc3_0_1a_1.bin'),str(R/'research/nord2/wifi/firmware/soc3_0_patch_wmmcu_1a_1_hdr.bin'),str(R/'dtb/nord2/live.dtb')]
  result=subprocess.run([str(exe),*extra],capture_output=True,text=True)
  if result.returncode:
   print(result.stdout);print(result.stderr);result.check_returncode()
  print(result.stdout.splitlines()[-1])
print('PASS canonical protocol/scheduler/initializer/transport/native/terminal checks; hardware pending')
