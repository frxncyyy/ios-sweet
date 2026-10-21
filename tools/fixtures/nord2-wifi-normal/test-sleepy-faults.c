#define FULL_FAULTS 1
#define SLEEPY_FAULTS 1
#include "test-sleepy.c"

static unsigned fault_at,fault_calls,fault_seen,fault_after;
static int full_test_fail(void){
 if(fault_seen){fault_after++;return -1;}
 if(!nbc_native.state.entered||nbc_native.state.completed)return 0;
 if(++fault_calls==fault_at){fault_seen=1;return -1;}return 0;
}
int main(int argc,char**argv){assert(argc==2);FILE*f=fopen(argv[1],"rb");assert(f&&fread(authenticated_board,1,sizeof(authenticated_board),f)==sizeof(authenticated_board));fclose(f);
 scenario(1,1,1,1);run_worker();assert(!manual_halts&&fault_calls>100);unsigned total=fault_calls;
 for(unsigned k=1;k<=total;k++){
  scenario(1,1,1,1);fault_at=k;fault_calls=fault_seen=fault_after=0;run_worker();struct nbc_state*b=&nbc_native.state;
  assert(fault_seen&&!fault_after&&manual_halts==1&&nwm_drain_entered&&b->halt&&!b->completed&&!normal_pubs&&basic_pubs<=1&&nwfn_native.init.transport.completed==4);
  if(b->budget.reserved)assert(b->budget.pse_remaining==2);else assert(b->budget.pse_remaining==3);
  assert(b->rearmed<=2&&b->sleepy_events<=2);
 }
 printf("PASS %u real-owner BASIC callback faults with queued pre/postCID2 sleepy; no postfailureIO/retry/refund/fakeACK, exact incomplete4/drain. Synthetic only.\n",total);return 0;
}
