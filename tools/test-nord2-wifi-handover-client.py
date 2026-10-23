from pathlib import Path
import argparse,tempfile,subprocess
R=next(p for p in Path(__file__).resolve().parents if(p/'AGENTS.md').exists());F=Path(__file__).resolve().parent/'fixtures/nord2-wifi-handover';P=R/'tools/fixtures/nord2-wifi-ios-client';a=argparse.ArgumentParser();a.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');a.add_argument('--log',type=Path,required=True);x=a.parse_args()
s=(R/'tools/test-nord2-wifi-ios-client.py').read_text().replace("P=Path(__file__).resolve().parent/'fixtures/nord2-wifi-ios-client'",'P=Path('+repr(str(P))+')');s=s.replace("R=next(x for x in Path(__file__).resolve().parents if(x/'AGENTS.md').exists())",'R=Path('+repr(str(R))+')')
stubs=(P/'test-join-stubs.inc').read_text().replace('int nord2_wifi_join_enqueue(', 'int original_join_enqueue(').replace('int nord2_wifi_join_cancel(', 'int original_join_cancel(')
stubs+='''
static njs_completion old_cb;static void*old_cookie;static uint64_t old_epoch,old_ticket;static bool old_canceled;
extern "C" int nord2_wifi_join_enqueue(const niwj_request*q,void*c,uint64_t e,njs_completion f,uint64_t*t){if(!logical_power)return 1;if(jactive){assert(js.phase==NJS_LEAVING&&!old_cb);old_cb=jcb;old_cookie=jcookie;old_epoch=jepoch;old_ticket=jticket;old_canceled=false;jactive=false;}return original_join_enqueue(q,c,e,f,t);}
extern "C" int nord2_wifi_join_cancel(uint64_t t){if(old_cb&&t==old_ticket){old_canceled=true;return 0;}return original_join_cancel(t);}
static void handover_power_off(void){if(jactive){js.phase=NJS_LEAVING;js.associated=js.ptk_ready=js.gtk_ready=js.data_ready=0;}}
static void oldNotice(njs_notice n){assert(old_cb);old_cb(old_cookie,old_epoch,old_ticket,n);if(n==NJS_RETIRED)old_cb=nullptr;}
'''
s=s.replace("(P/'test-join-stubs.inc').read_text()",repr(stubs));extra=(P/'test-join.inc').read_text().split('int main(){',1)[0]+(F/'client.inc').read_text();s=s.replace("(P/'test-disconnect.inc').read_text()",repr(extra))
needle="(d/'test.cpp').write_text(source)"
power_patch="source=source.replace('extern \"C\" int nord2_wifi_power_set','static void handover_power_off(void);\\nextern \"C\" int nord2_wifi_power_set').replace('logical_power=on;return 0;','logical_power=on;if(!on)handover_power_off();return 0;');"
s=s.replace(needle,power_patch+needle)
with tempfile.TemporaryDirectory()as t:
 f=Path(t)/'run.py';f.write_text(s);subprocess.run(['python3',str(f),'--only','disconnect','--candidate',str(x.candidate.resolve()),'--log',str(x.log.resolve())],check=True)
