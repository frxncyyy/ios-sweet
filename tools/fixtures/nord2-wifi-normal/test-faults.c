#define FULL_FAULTS 1
#define main full_reference_main
#include "test-full.c"
#undef main
static unsigned fault_at,fault_calls,fault_seen,fault_after;
static int full_test_fail(void){
 if(fault_seen){fault_after++;return -1;}
 if(!nct_native.active||nct_native.session.cursor!=NCB_COMMANDS-1||!ncb_native.rx.active)return 0;
 if(++fault_calls==fault_at){fault_seen=1;return -1;}return 0;
}
int main(int argc,char**argv){assert(argc==2);FILE*f=fopen(argv[1],"rb");assert(f&&fread(authenticated_board,1,sizeof(authenticated_board),f)==sizeof(authenticated_board));fclose(f);
 setup(0);run_worker();assert(!manual_halts&&fault_calls>50);unsigned total=fault_calls;
 for(unsigned n=1;n<=total;n++){
  setup(0);fault_at=n;fault_calls=fault_seen=fault_after=0;run_worker();struct nct_session*t=&nct_native.session;struct ncr_state*r=&ncb_native.rx;
  assert(fault_seen&&!fault_after&&manual_halts==1&&nwm_drain_entered&&drain_blocks==1&&t->halt&&r->halt&&t->completed==(NCB_COMMANDS-1)&&normal_pubs==NCB_COMMANDS);
  assert(t->available==2&&!t->reserved&&t->acquired==NCB_COMMANDS&&t->released==NCB_COMMANDS&&r->rearmed<=5&&!r->scan.done&&!t->scan_complete);
 }
 printf("PASS %u final26thcommand real native RX callback faults; actualallfivequeue guards/lease retained, no subsequentcallbacks/fakecompletion, sourcecredits unchanged, commonhalt/30sdrain. Synthetic only.\n",total);return 0;
}
