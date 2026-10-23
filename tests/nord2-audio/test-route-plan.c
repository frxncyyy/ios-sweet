#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "audio_afe.h"
#include "route-source-oracle.h"
#include "actual-snapshot.inc"
int main(int argc,char **argv){
 struct nau_state s={0};s.snap_read=NAU_SNAP_COUNT;memcpy(s.snap,actual_snapshot,sizeof s.snap);
 struct naf_amp_evidence a[2]={{1,1,1,0,0},{1,1,1,0,0}};struct naf_plan p;
 assert(!naf_build(&s,a,&p));unsigned up=0,down=0,wrong=0;
 for(unsigned side=0;side<2;side++){
  struct naf_op *ops=side?p.down:p.up;unsigned n=side?p.n_down:p.n_up;
  for(unsigned i=0;i<n;i++)if(ops[i].kind==NAF_RMW || ops[i].kind==NAF_W){
   if(ops[i].pa==0x112104bc || ops[i].pa==0x112104c0)wrong++;
   unsigned bit=ops[i].pa==ROUTE_CH1_PA?ROUTE_CH1_BIT:ROUTE_CH2_BIT;
   unsigned mark=ops[i].pa==ROUTE_CH1_PA?1:ops[i].pa==ROUTE_CH2_PA?2:0;
   if(mark && (ops[i].mask&(1u<<bit))){if(!side && (ops[i].value&(1u<<bit)))up|=mark;if(side && !(ops[i].value&(1u<<bit)))down|=mark;}
  }
 }
 if(argc==2 && !strcmp(argv[1],"--expect-broken")){assert(up!=3 && down!=3 && wrong);puts("REPRODUCED old plan sends DL1 to I2S1 instead of Android I2S3");return 0;}
 assert(up==3 && down==3 && !wrong);
 puts("PASS independent selected-Android sink oracle: DL1CH1/2 connect/disconnect I2S3 CONN0/1; zero I2S1 writes");
}
