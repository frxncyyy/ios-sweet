from pathlib import Path
import subprocess,argparse
R=next(p for p in Path(__file__).resolve().parents if (p/"AGENTS.md").exists());F=Path(__file__).resolve().parent;P=R/"tools"
a=argparse.ArgumentParser();a.add_argument('--initial-open',type=int,choices=[0,1],default=1);a.add_argument('--board',type=Path,required=True);a.add_argument('--bearssl',type=Path,required=True);a.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');a.add_argument('--log-dir',type=Path,required=True);x=a.parse_args();W=x.log_dir.resolve();W.mkdir(parents=True,exist_ok=True)
s=(P/'test-nord2-wifi-join-owner.py').read_text();start=s.index(" test+='''");end=s.index(" (d/'test.c').write_text(test)",start)
s=s[:start]+' test+='+repr((F/'owner.c').read_text())+'\n'+s[end:]
s=s.replace("B=R/'research/ci-39158169212/buildtree/xnu/libkern/crypto/sweet_bearssl'",'B=Path('+repr(str(x.bearssl.resolve()))+')')
old="P=Path(__file__).resolve().parent;R=next(p for p in P.parents if (p/'AGENTS.md').exists());"
s=s.replace(old,'R=Path('+repr(str(R))+');P=Path('+repr(str(P))+');')
insert="""
 owner=(d/'owner-fixture.inc').read_text().replace('static unsigned normal_enabled,','static unsigned station_hardware;static void station_completion(unsigned);static void station_data_completion(unsigned);\\nstatic unsigned normal_enabled,').replace('if(normal_enabled&&t->completed>=5){','if(normal_enabled&&t->completed>=5){if(station_hardware){station_completion(next);return;}')
 owner=owner.replace('assert(a!=0x18024228','if(a==0x18025308&&station_hardware)station_data_completion(v);assert(a!=0x18024228')
 scan=(d/'wifi_scan_service_bind.inc').read_text().replace('int join_work=njs_owner_work();','if(handover_park&&njs_handover.state&&nsr_native.scan_ready&&!njs_native.active)longjmp(parked_exit,1);int join_work=njs_owner_work();');(d/'wifi_scan_service_bind.inc').write_text(scan)
 owner='static unsigned handover_park,radio_ssid=88,radio_ap;\\n'+owner
 owner='static void test_prepare_clock(void);\\n#define NJS_LIVE_CONFIGURE() test_prepare_clock()\\n'+owner
 (d/'owner-fixture.inc').write_text(owner)
 host=(d/'nss_host.h').read_text().replace('static void read_random(void*p,unsigned n){memset(p,0x41,n);}','static unsigned random_calls;static void read_random(void*p,unsigned n){random_calls++;memset(p,0x40+random_calls,n);}')
 (d/'nss_host.h').write_text(host)
 (d/'vectors.h').write_bytes((J/'vectors.h').read_bytes())
"""
s=s.replace(" # Existing full native fixture's probes",insert+" # Existing full native fixture's probes")
# Synthetic radio beacon becomes complete WPA2/legacy scan input, through real parser.
old='static void beacon(unsigned k,unsigned id,unsigned ddone){uint8_t p[63]={0};w16(p,63);w16(p+2,0x3801);put32(p+8,12u<<8);put32(p+12,6u<<8);p[24]=0x80;p[34]=2;p[40]=2;w16(p+56,100);p[60]=0;p[61]=1;p[62]=(uint8_t)id;deliver(k,p,sizeof(p),ddone);}'
new='static void beacon(unsigned k,unsigned id,unsigned ddone){(void)id;uint8_t p[96]={0};w16(p,96);w16(p+2,0x3801);put32(p+4,4u<<11);put32(p+8,12u<<8);put32(p+12,6u<<8);p[28]=100;p[29]=255;p[32]=0x80;p[42]=2;p[47]=radio_ap;p[48]=2;p[53]=radio_ap;w16(p+64,100);w16(p+66,0x11);p[68]=0;p[69]=1;p[70]=(uint8_t)radio_ssid;p[71]=1;p[72]=1;p[73]=0x82;const unsigned char rsnbytes[22]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};memcpy(p+74,rsnbytes,22);deliver(k,p,sizeof(p),ddone);}'
# Replacement occurs before generated test.c write, after base fixture loaded.
needle=" (d/'test.c').write_text(test)";s=s.replace(needle,' test=test.replace('+repr(old)+','+repr(new)+')\n'+needle)
s=s.replace(" (d/'test.c').write_text(test)"," test='static unsigned initial_open=INITIAL_OPEN,radio_open=INITIAL_OPEN;\\n'+test\n (d/'test.c').write_text(test)")
s=s.replace('w16(p,96);','w16(p,radio_open?74:96);').replace('w16(p+66,0x11);','w16(p+66,radio_open?1:0x11);').replace('memcpy(p+74,rsnbytes,22);deliver(k,p,sizeof(p),ddone);','if(!radio_open)memcpy(p+74,rsnbytes,22);deliver(k,p,radio_open?74:96,ddone);')
s=s.replace('INITIAL_OPEN',str(x.initial_open))
f=W/'runner.py';f.write_text(s)
subprocess.run(['python3',str(f),'--candidate',str(x.candidate.resolve()),'--log-dir',str(W),'--board',str(x.board.resolve())],check=True)
