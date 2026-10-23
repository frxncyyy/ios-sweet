from pathlib import Path
import subprocess,tempfile
P=Path(__file__).resolve().parent;R=P.parents[1];S=R/'shim/nord2-xnu';C=S
source=(C/'audio_device.c').read_text()
def function(sig):
 start=source.index(sig);end=source.index('{',start)+1;depth=1
 while depth:
  if source[end]=='{':depth+=1
  elif source[end]=='}':depth-=1
  end+=1
 return source[start:end]+'\n'
assert 'sound_queued && !device.tone_active' in source
with tempfile.TemporaryDirectory()as tmp:
 d=Path(tmp);(d/'device-policy.inc').write_text(function('static unsigned hardware_sound_pending(')+function('static void commands('))
 exe=d/'test-policy'
 subprocess.run(['clang','-std=c11','-DNORD2','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(C),'-I'+str(S),'-I'+str(d),str(P/'test-producer-policy.c'),str(C/'audio_stream.c'),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
