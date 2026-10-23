#!/usr/bin/env python3
"""Build the unmodified pinned HackiOS/Topaz iOS HAL on macOS; no device access."""
import argparse,hashlib,json,pathlib,subprocess,urllib.request
PIN='a79ff0772a3454fd1ef127e181e5b3bce6c9ce18'
FILES={
 'LICENSE':'f6cfd96f40e34d3679e037e1192d24a3794e41aa196ca6df76c0826ed6cd60c5',
 'audio_plugin.c':'7fab4e652d62f9ecf6cbf3a3c7a610031ac929ddc3413e428e719d2eb35be16f',
 'audio_plugin.plist':'8737f502b722742a6a37bce8c512220fabc18add88717f4eb8fae5524a08d6fd',
 'redwood_audio.h':'5ab3444730e47cb105a9ee14a924b0dca2df2b7588cf3eeba86e14a234d2dce4'}
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('output',type=pathlib.Path);a=ap.parse_args()
 a.output.mkdir();out=a.output.resolve();src=out/'source';src.mkdir()
 for name,digest in FILES.items():
  source_path=name if name=='LICENSE' else 'userland/src/'+name
  url=f'https://raw.githubusercontent.com/oosexgang/hackios-xnu-userland/{PIN}/{source_path}'
  req=urllib.request.Request(url,headers={'User-Agent':'Nord2-audio-build'})
  with urllib.request.urlopen(req,timeout=45) as r:blob=r.read(131073)
  if len(blob)>131072 or hashlib.sha256(blob).hexdigest()!=digest:raise SystemExit('upstream hash mismatch: '+name)
  (src/name).write_bytes(blob)
 (out/'LICENSE').write_bytes((src/'LICENSE').read_bytes())
 bundle=out/'RedwoodAudio.driver';bundle.mkdir()
 subprocess.run(['xcrun','--sdk','iphoneos','clang','-arch','arm64','-miphoneos-version-min=16.0','-O1',
  '-bundle','-framework','CoreFoundation','-Wl,-no_uuid','-o',str(bundle/'RedwoodAudio'),str(src/'audio_plugin.c')],check=True)
 (bundle/'Info.plist').write_bytes((src/'audio_plugin.plist').read_bytes())
 subprocess.run(['plutil','-lint',str(bundle/'Info.plist')],check=True)
 checks=[('architecture',['xcrun','lipo',str(bundle/'RedwoodAudio'),'-verify_arch','arm64'],None),
 ('platform',['xcrun','vtool','-show-build',str(bundle/'RedwoodAudio')],'platform IOS'),
 ('factory',['xcrun','nm','-gU',str(bundle/'RedwoodAudio')],' _RedwoodAudio_Create'),
 ('type',['xcrun','otool','-hv',str(bundle/'RedwoodAudio')],'BUNDLE'),
 ('dependencies',['xcrun','otool','-L',str(bundle/'RedwoodAudio')],None)]
 for name,cmd,want in checks:
  r=subprocess.run(cmd,check=True,stdout=subprocess.PIPE,text=True);(out/(name+'.txt')).write_text(r.stdout)
  if want and want not in r.stdout:raise SystemExit('HAL check failed: '+name)
 subprocess.run(['python3',str(pathlib.Path(__file__).with_name('nord2-audio-package.py')),'build',str(bundle),str(out/'audio.pkg')],check=True)
 record={'upstream':'oosexgang/hackios-xnu-userland','commit':PIN,'sources':FILES,
  'outputs':{str(p.relative_to(out)):sha(p) for p in [bundle/'RedwoodAudio',bundle/'Info.plist',out/'audio.pkg']}}
 (out/'provenance.json').write_text(json.dumps(record,indent=2)+'\n')
 print(json.dumps(record,indent=2))
if __name__=='__main__':main()
