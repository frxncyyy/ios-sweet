from pathlib import Path
import argparse,subprocess,tempfile
P=Path(__file__).resolve().parent;R=next(p for p in P.parents if(p/'AGENTS.md').exists());a=argparse.ArgumentParser();a.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');a.add_argument('--log',type=Path,required=True);x=a.parse_args()
s=(R/'tools/test-nord2-wifi-ios-client.py').read_text();s=s.replace("R=next(x for x in Path(__file__).resolve().parents if(x/'AGENTS.md').exists());P=Path(__file__).resolve().parent/'fixtures/nord2-wifi-ios-client';",'R=Path('+repr(str(R))+');P=R/"tools/fixtures/nord2-wifi-ios-client";')
s=s.replace("(P/'test-disconnect.inc').read_text()",repr((P/'fixtures/nord2-wifi-lifecycle/test-power.inc').read_text()))
# Ordinary client fixture provides the shared logical power/epoch model.
with tempfile.TemporaryDirectory(prefix='lifecycle-client-')as t:
 f=Path(t)/'run.py';f.write_text(s);subprocess.run(['python3',str(f),'--candidate',str(x.candidate.resolve()),'--only','disconnect','--log',str(x.log.resolve())],check=True)
