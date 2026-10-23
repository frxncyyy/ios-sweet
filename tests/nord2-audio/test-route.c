/* Android-derived sink oracle, not a restatement of port route macros.
 * Register/serial interpretation is modeled; physical signal remains unproven. */
#define INHERITED_MODEL_ONLY 1
#include "test-inherited.c"
#include "route-source-oracle.h"
static unsigned wrong_writes;
static int route_write(void *c,uint32_t pa,unsigned w,uint32_t v){
 if(pa==0x112104bcu || pa==0x112104c0u){wrong_writes++;assert(!"I2S1 sink must not be written");}
 return audited_write(c,pa,w,v);
}
static const struct nah_io rio={audited_read,route_write,alldelay,allworker,alllease};
static int plan_routes_i2s3(void){
 struct nau_state snap={0};snap.snap_read=NAU_SNAP_COUNT;memcpy(snap.snap,actual_snapshot,sizeof snap.snap);
 struct naf_plan p;assert(!naf_build(&snap,amps,&p));unsigned seen=0,clear=0;
 for(unsigned i=0;i<p.n_up;i++)if(p.up[i].kind==NAF_RMW){
  if(p.up[i].pa==ROUTE_CH1_PA && (p.up[i].mask&(1u<<ROUTE_CH1_BIT)) && (p.up[i].value&(1u<<ROUTE_CH1_BIT)))seen|=1;
  if(p.up[i].pa==ROUTE_CH2_PA && (p.up[i].mask&(1u<<ROUTE_CH2_BIT)) && (p.up[i].value&(1u<<ROUTE_CH2_BIT)))seen|=2;
 }
 for(unsigned i=0;i<p.n_down;i++)if(p.down[i].kind==NAF_RMW){
  if(p.down[i].pa==ROUTE_CH1_PA && p.down[i].mask==(1u<<ROUTE_CH1_BIT) && !p.down[i].value)clear|=1;
  if(p.down[i].pa==ROUTE_CH2_PA && p.down[i].mask==(1u<<ROUTE_CH2_BIT) && !p.down[i].value)clear|=2;
 }
 return seen==3 && clear==3;
}
static void keep_i2s1(void){assert(!wrong_writes && reg(0x112104bc)==0xa5a50020 && reg(0x112104c0)==0x5a5a0040);}
static void setup_route(void){
 prepare(actual_snapshot);strict=1;wrong_writes=0;
 /* Known actual silicon; controller/POR behavior still explicitly modeled. */
 bm.slave[0].reg[3]=bm.slave[1].reg[3]=0x0c74;
 /* Synthetic unrelated I2S1 data, not captured while AUDIO was off. */
 *regp(&cm,0x112104bc)=0xa5a50020;*regp(&cm,0x112104c0)=0x5a5a0040;
 assert(!nau_hw_init(&hw,&rio,0,nah_board_profile()));
 assert(hw.status.amp_revision[0]==0x0c74 && hw.status.amp_revision[1]==0x0c74);
}
int main(int argc,char **argv){
 assert(argc==2);
 if(!strcmp(argv[1],"--expect-broken")){assert(!plan_routes_i2s3());puts("REPRODUCED DL1 plan feeds I2S1 sinks while Android I2S3 sinks remain disconnected");return 0;}
 assert(plan_routes_i2s3());
 assert(NAU_A_CONN28==7 && NAU_A_CONN29==8 && nau_afe_pa[NAU_A_CONN28]==0x112104bc && nau_afe_pa[NAU_A_CONN29]==0x112104c0);
 assert(nau_afe_pa[NAU_A_CONN0]==ROUTE_CH1_PA && nau_afe_pa[NAU_A_CONN1]==ROUTE_CH2_PA);
 assert(!naf_write_allowed(0x112104bc) && !naf_write_allowed(0x112104c0));
 load_board(argv[1]);setup_route();
 int16_t pcm[480];for(unsigned i=0;i<240;i++){pcm[2*i]=8192;pcm[2*i+1]=-16384;}
 for(unsigned k=0;k<3;k++){
  uint64_t t=1000000ull*(k+1);cm.current=NAF_SRAM;
  assert(!nau_hw_start(&hw,48000,2,16,0,256,0,t));assert(!nau_hw_tick(&hw,t));
  /* Two5ms chunks finish the new start ramp before this routing oracle
   * checks full-level stereo packing. Ramp itself has independent coverage. */
  for(unsigned chunk=1;chunk<=3;chunk++){
   assert(nau_hw_feed(&hw,pcm,240)==240);cm.current=NAF_SRAM+960*chunk;
   assert(!nau_hw_tick(&hw,t+5000*chunk));
  }
  /* HD0 is packed interleaved S16LE; source physical32-bit I2S width is
   * independent. Verify values and source-selected channel paths. */
  assert((reg(NAF_AFE_DL1_CON0)&3)==0 && (reg(NAF_AFE_DL1_CON0)&(1u<<5)));
  assert((reg(NAF_AFE_CONN_24BIT)&3u)==3u); /* O0/O1 included in Android all-ports24bit */
  assert((reg(NAF_AFE_I2S_CON3)&0x300b)==0x100b); /* normal matrix,32slots,I2S,HD,on */
  assert((reg(ROUTE_CH1_PA)&(1u<<ROUTE_CH1_BIT)) && (reg(ROUTE_CH2_PA)&(1u<<ROUTE_CH2_BIT)));
  assert((reg(NAF_GPIO_MODE_32)&0xf0ff)==0x1011);
  for(unsigned i=0;i<240;i++){assert((int16_t)cm.sram[480+i]==64);assert((int16_t)(cm.sram[480+i]>>16)==-128);}
  keep_i2s1();assert(!nau_hw_stop(&hw));keep_i2s1();restored();
  assert(!(reg(ROUTE_CH1_PA)&(1u<<ROUTE_CH1_BIT)) && !(reg(ROUTE_CH2_PA)&(1u<<ROUTE_CH2_BIT)));
 }
 /* Running failure cleanup also removes corrected routes only. */
 setup_route();cm.current=NAF_SRAM;
 assert(!nau_hw_start(&hw,48000,2,16,0,256,0,3500000));assert(!nau_hw_tick(&hw,3500000));
 cm.current=NAF_SRAM-4;assert(nau_hw_tick(&hw,3502000)==NAH_E_POINTER);keep_i2s1();
 assert(!(reg(ROUTE_CH1_PA)&(1u<<ROUTE_CH1_BIT)) && !(reg(ROUTE_CH2_PA)&(1u<<ROUTE_CH2_BIT)));
 /* A source power-up failure must not touch wrong sinks either. */
 setup_route();fault=F_POWERUP;
 assert(nau_hw_start(&hw,48000,2,16,0,256,0,4000000)==NAH_E_UNCERTAIN);keep_i2s1();
 puts("PASS Android-derived DL1->I2S3 CONN0/1 routing,packedS16 stereo and32bitserial config,three0c74cycles,wrongI2S1 untouched including failure; historical observations preserved");
}
