#!/usr/bin/env python3
"""Host-only actual IOUC wire/lifetime regression; no device or firmware IO."""
from pathlib import Path
import subprocess,tempfile,argparse
R=next(x for x in Path(__file__).resolve().parents if(x/'AGENTS.md').exists());P=Path(__file__).resolve().parent/'fixtures/nord2-wifi-ios-client';B=P;S=R/'shim/nord2-xnu'
ap=argparse.ArgumentParser();ap.add_argument('--log',type=Path);ap.add_argument('--candidate',type=Path,help='Optional reviewed private source overlay; default tests canonical runtime');ap.add_argument('--only',choices=('all','card'),default='all');args=ap.parse_args()
names=('join','station_view','binding','control','event','lifecycle','card','policy','rates','mapping','age','snapshot','bss','scan_contract','cursor','scalar')
with tempfile.TemporaryDirectory(prefix='niwl-compat-') as t:
 d=Path(t);dev=d/'xnu/bsd/dev';arm=d/'xnu/iokit/Kernel/arm';dev.mkdir(parents=True);arm.mkdir(parents=True)
 for f in S.iterdir():
  if f.suffix in ('.h','.c','.cpp'):((arm if f.suffix=='.cpp' else dev)/f.name).write_bytes(f.read_bytes())
 if args.candidate:
  for f in args.candidate.iterdir():((arm if f.suffix=='.cpp' else dev)/f.name).write_bytes(f.read_bytes())
 source_inventory=(args.candidate/'wifi_station_service_bind.inc' if args.candidate and(args.candidate/'wifi_station_service_bind.inc').exists()else S/'wifi_station_service_bind.inc').read_text()
 start=source_inventory.index('int nord2_wifi_join_card_inventory(');end=source_inventory.index('\n}',start)+2
 (d/'test-card-inventory.inc').write_text(source_inventory[start:end]+'\n')
 h=(B/'test-xnu.h').read_text().replace('kIODirectionOut=2','kIODirectionOut=2,kIODirectionIn=1').replace('virtual size_t readBytes(size_t,void*,size_t)=0;','virtual size_t readBytes(size_t,void*,size_t)=0;virtual size_t writeBytes(size_t,const void*,size_t)=0;')
 h=h.replace('bool failPrepare=false,failRead=false,failComplete=false;','bool failPrepare=false,failRead=false,failWrite=false,failComplete=false;unsigned writes=0;')
 h=h.replace('assert(dir==kIODirectionOut);','assert(dir==kIODirectionOut||dir==kIODirectionIn);').replace('assert(dir==kIODirectionOut&&depth);','assert((dir==kIODirectionOut||dir==kIODirectionIn)&&depth);')
 h=h.replace('std::memcpy(p,(char*)bytes+o,n);return n;}};', 'std::memcpy(p,(char*)bytes+o,n);return n;}size_t writeBytes(size_t o,const void*p,size_t n)override{assert(depth);writes++;if(failWrite)return n?n-1:0;if(o>length||n>length-o)return 0;std::memcpy((char*)bytes+o,p,n);return n;}};')
 h=h.replace('size_t structureOutputSize=0;', 'size_t structureOutputSize=0,structureOutputDescriptorSize=0;');(d/'test-xnu.h').write_text(h)
 source=(B/'test-integrated.cpp').read_text().replace('"LifecycleShell.cpp"','"wifi_ios_client.cpp"').replace('int main(){','static int oldmain(){').replace('out->count=1;','out->scanning=active;out->count=1;')
 ix=source.rfind('}');source=source[:ix]+' return 0;\n'+source[ix:]
 source+=(P/'test-join-stubs.inc').read_text();legacy_extra=(P/'test-extra.inc').read_text().replace('int main(){','int main(){assertNoJoins();');source+=legacy_extra;(d/'test.cpp').write_text(source)
 flags=['-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(d),'-I'+str(dev),'-I'+str(arm)]
 objs=[]
 for name in names:
  f=dev/('wifi_ios_'+name+'.c');o=d/(name+'.o');subprocess.run(['clang','-std=c11',*flags,'-c',str(f),'-o',str(o)],check=True);objs.append(str(o))
 logs=[]
 if args.only=='all':
  subprocess.run(['clang++','-std=c++17',*flags,'-Wno-unused-function','-pthread',str(d/'test.cpp'),*objs,'-o',str(d/'test')],check=True)
  result=subprocess.run([str(d/'test')],text=True,capture_output=True);print(result.stdout);print(result.stderr);result.check_returncode()
  join_source=source[:source.index(legacy_extra)]+(P/'test-join.inc').read_text()
  (d/'join.cpp').write_text(join_source)
  subprocess.run(['clang++','-std=c++17',*flags,'-Wno-unused-function','-pthread',str(d/'join.cpp'),*objs,'-o',str(d/'join')],check=True)
  joined=subprocess.run([str(d/'join')],text=True,capture_output=True);print(joined.stdout);print(joined.stderr);joined.check_returncode()
  stub=(B/'test-integrated.cpp').read_text();stub=stub[stub.index('static nss_completion callback;'):stub.index('static void finish')]
  lifetime=(B/'test-lifecycle.cpp').read_text().replace('"LifecycleShell.cpp"','"wifi_ios_client.cpp"').replace('static task owner_task,other_task;',stub+'\n'+(P/'test-join-stubs.inc').read_text()+'\nstatic task owner_task,other_task;')
  lifetime=lifetime.replace('int main(){','int main(){assertNoJoins();');(d/'lifetime.cpp').write_text(lifetime)
  subprocess.run(['clang++','-std=c++17',*flags,'-Wno-unused-function','-Wno-unused-but-set-variable','-pthread',str(d/'lifetime.cpp'),*objs,'-o',str(d/'lifetime')],check=True)
  life=subprocess.run([str(d/'lifetime')],text=True,capture_output=True);print(life.stdout);print(life.stderr);life.check_returncode()
  logs=[result.stdout+result.stderr,joined.stdout+joined.stderr,life.stdout+life.stderr]
 card_source=source[:source.index(legacy_extra)]+(P/'test-card.inc').read_text();(d/'card.cpp').write_text(card_source)
 subprocess.run(['clang++','-std=c++17',*flags,'-Wno-unused-function','-pthread',str(d/'card.cpp'),*objs,'-o',str(d/'card')],check=True)
 card=subprocess.run([str(d/'card')],text=True,capture_output=True);print(card.stdout);print(card.stderr);card.check_returncode();logs.append(card.stdout+card.stderr)
 if args.log:args.log.write_text(''.join(logs))
