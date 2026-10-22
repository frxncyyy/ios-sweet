#!/usr/bin/env python3
"""Canonical runtime tests; native queue/hardware remain explicit host doubles."""
from pathlib import Path
import tempfile,shutil,subprocess,json,argparse
R=next(x for x in Path(__file__).resolve().parents if (x/'AGENTS.md').exists());S=R/'shim/nord2-xnu';F=R/'tools/fixtures/nord2-wifi-ios-client';C=R/'tools/fixtures/nord2-wifi-ios-contract'
ap=argparse.ArgumentParser(description='Synthetic production join profile/identity and atomic diagnostic regression; no device IO')
ap.add_argument('--candidate',type=Path,help='Optional source overlay directory');ap.add_argument('--log',type=Path);args=ap.parse_args();logs=[]

with tempfile.TemporaryDirectory(prefix='join-profile-') as td:
 d=Path(td);arm=d/'xnu/iokit/Kernel/arm';dev=d/'xnu/bsd/dev';arm.mkdir(parents=True);dev.mkdir(parents=True)
 for folder in ([S,args.candidate] if args.candidate else [S]):
  for f in folder.iterdir():
   if f.suffix in ('.c','.cpp','.h'):shutil.copy2(f,(arm if f.suffix=='.cpp' else dev)/f.name)
 flags=['-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(d),'-I'+str(dev),'-I'+str(arm)]
 shutil.copy2(R/'tools/fixtures/nord2-wifi-ios-contract/test-join.c',d/'baseline-join.c')
 subprocess.run(['clang','-std=c11',*flags,str(C/'test-join-profile.c'),str(dev/'wifi_ios_join.c'),'-o',str(d/'pure')],check=True)
 r=subprocess.run([str(d/'pure')],capture_output=True,text=True);logs.append(r.stdout+r.stderr);r.check_returncode();print(r.stdout)
 # Reuse the production IOUC fixture ABI/stubs but run only new join composition.
 runner=(R/'tools/test-nord2-wifi-ios-client.py').read_text();h=(F/'test-xnu.h').read_text().replace('kIODirectionOut=2','kIODirectionOut=2,kIODirectionIn=1').replace('virtual size_t readBytes(size_t,void*,size_t)=0;','virtual size_t readBytes(size_t,void*,size_t)=0;virtual size_t writeBytes(size_t,const void*,size_t)=0;');h=h.replace('bool failPrepare=false,failRead=false,failComplete=false;','bool failPrepare=false,failRead=false,failWrite=false,failComplete=false;unsigned writes=0;').replace('assert(dir==kIODirectionOut);','assert(dir==kIODirectionOut||dir==kIODirectionIn);').replace('assert(dir==kIODirectionOut&&depth);','assert((dir==kIODirectionOut||dir==kIODirectionIn)&&depth);');h=h.replace('std::memcpy(p,(char*)bytes+o,n);return n;}};','std::memcpy(p,(char*)bytes+o,n);return n;}size_t writeBytes(size_t o,const void*p,size_t n)override{assert(depth);writes++;if(failWrite)return n?n-1:0;if(o>length||n>length-o)return 0;std::memcpy((char*)bytes+o,p,n);return n;}};').replace('size_t structureOutputSize=0;','size_t structureOutputSize=0,structureOutputDescriptorSize=0;');(d/'test-xnu.h').write_text(h)
 source=(F/'test-integrated.cpp').read_text().replace('"LifecycleShell.cpp"','"wifi_ios_client.cpp"').replace('int main(){','static int oldmain(){');ix=source.rfind('}');source=source[:ix]+' return 0;\n'+source[ix:];source=source.replace('extern "C" int nord2_wifi_scan_info(nss_info*out){','static int ownerUnavailable;\nextern "C" int nord2_wifi_scan_info(nss_info*out){if(ownerUnavailable)return 1;')
 inventory=(S/'wifi_station_service_bind.inc').read_text();start=inventory.index('int nord2_wifi_join_card_inventory(');end=inventory.index('\n}',start)+2;(d/'test-card-inventory.inc').write_text(inventory[start:end]+'\n')
 source+=(F/'test-join-stubs.inc').read_text();helpers=(F/'test-join.inc').read_text();source+=helpers[:helpers.index('int main(){')];source+='#include <algorithm>\n#include <vector>\n'+(F/'test-join-profile.inc').read_text();(d/'profile.cpp').write_text(source)
 names=('join','station_view','binding','control','event','lifecycle','card','policy','rates','mapping','age','snapshot','bss','scan_contract','cursor','scalar');objs=[]
 for name in names:
  o=d/(name+'.o');subprocess.run(['clang','-std=c11',*flags,'-c',str(dev/('wifi_ios_'+name+'.c')),'-o',str(o)],check=True);objs.append(str(o))
 subprocess.run(['clang++','-std=c++17',*flags,'-Wno-unused-function','-pthread',str(d/'profile.cpp'),*objs,'-o',str(d/'client')],check=True)
 r=subprocess.run([str(d/'client')],capture_output=True,text=True);logs.append(r.stdout+r.stderr);r.check_returncode();print(r.stdout.splitlines()[-1])

if args.log:
 args.log.parent.mkdir(parents=True,exist_ok=True);args.log.write_text("\n".join(logs))
