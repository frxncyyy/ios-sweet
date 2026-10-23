from pathlib import Path
import subprocess,argparse
P=Path(__file__).resolve().parent;R=next(p for p in P.parents if (p/'AGENTS.md').exists());F=P/'fixtures/nord2-wifi-lifecycle';P=R/'tools'
a=argparse.ArgumentParser();a.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');a.add_argument('--log-dir',type=Path,required=True);x=a.parse_args();W=x.log_dir.resolve();W.mkdir(parents=True,exist_ok=True)
s=(P/'test-nord2-wifi-join-owner.py').read_text();start=s.index(" test+='''");end=s.index(" (d/'test.c').write_text(test)",start)
s=s[:start]+' test+='+repr((F/'owner.c').read_text())+'\n'+s[end:]
old="P=Path(__file__).resolve().parent;R=next(p for p in P.parents if (p/'AGENTS.md').exists());"
s=s.replace(old,'R=Path('+repr(str(R))+');P=Path('+repr(str(P))+');')
insert="""
 owner=(d/'owner-fixture.inc').read_text().replace('static unsigned normal_enabled,','static unsigned station_hardware;static void station_completion(unsigned);static void station_data_completion(unsigned);\\nstatic unsigned normal_enabled,').replace('if(normal_enabled&&t->completed>=5){','if(normal_enabled&&t->completed>=5){if(station_hardware){station_completion(next);return;}')
 owner=owner.replace('assert(a!=0x18024228','if(a==0x18025308&&station_hardware)station_data_completion(v);assert(a!=0x18024228')
 owner='static void test_prepare_clock(void);\\n#define NJS_LIVE_CONFIGURE() test_prepare_clock()\\n'+owner
 owner='static unsigned lifecycle_off_nct;extern int nord2_wifi_power_set(unsigned);'+owner
 owner=owner.replace('if(normal_enabled&&t->completed>=5){','if(normal_enabled&&t->completed>=5){if(lifecycle_off_nct){lifecycle_off_nct=0;assert(!nord2_wifi_power_set(0));}')
 (d/'owner-fixture.inc').write_text(owner)
 host=(d/'nss_host.h').read_text().replace('static void read_random(void*p,unsigned n){memset(p,0x41,n);}','static unsigned random_calls;static void read_random(void*p,unsigned n){random_calls++;memset(p,0x40+random_calls,n);}')
 (d/'nss_host.h').write_text(host)
 (d/'vectors.h').write_bytes((J/'vectors.h').read_bytes())
"""
s=s.replace(" # Existing full native fixture's probes",insert+" # Existing full native fixture's probes")
# Synthetic radio beacon becomes complete WPA2/legacy scan input, through real parser.
old='static void beacon(unsigned k,unsigned id,unsigned ddone){uint8_t p[63]={0};w16(p,63);w16(p+2,0x3801);put32(p+8,12u<<8);put32(p+12,6u<<8);p[24]=0x80;p[34]=2;p[40]=2;w16(p+56,100);p[60]=0;p[61]=1;p[62]=(uint8_t)id;deliver(k,p,sizeof(p),ddone);}'
new='static void beacon(unsigned k,unsigned id,unsigned ddone){uint8_t p[96]={0};w16(p,96);w16(p+2,0x3801);put32(p+4,4u<<11);put32(p+8,12u<<8);put32(p+12,6u<<8);p[28]=100;p[29]=255;p[32]=0x80;p[42]=2;p[48]=2;w16(p+64,100);w16(p+66,0x11);p[68]=0;p[69]=1;p[70]=(uint8_t)id;p[71]=1;p[72]=1;p[73]=0x82;const unsigned char rsnbytes[22]={48,20,1,0,0,15,172,4,1,0,0,15,172,4,1,0,0,15,172,2,0,0};memcpy(p+74,rsnbytes,22);deliver(k,p,sizeof(p),ddone);}'
# Replacement occurs before generated test.c write, after base fixture loaded.
needle=" (d/'test.c').write_text(test)";s=s.replace(needle,' test=test.replace('+repr(old)+','+repr(new)+')\n'+needle)
f=W/'runner.py';f.write_text(s)
subprocess.run(['python3',str(f),'--candidate',str(x.candidate.resolve()),'--log-dir',str(W)],check=True)
