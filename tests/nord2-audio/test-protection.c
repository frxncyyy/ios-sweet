#define INHERITED_MODEL_ONLY 1
#include "test-inherited.c"
static unsigned release_stale,drop_clear,drop_set,drop_ack,clears;
static int pr(void *c,uint32_t pa,unsigned w,uint32_t *v){return audited_read(c,pa,w,v);}
static int pw(void *c,uint32_t pa,unsigned w,uint32_t v){
 if(pa==NAF_IFR_PROT2_CLR){
  clears++;
  if(drop_clear==clears){cm.writes++;return 0;}
 }
 if(pa==NAF_IFR_PROT2_SET && drop_set){cm.writes++;return 0;}
 int r=audited_write(c,pa,w,v);
 if(pa==NAF_IFR_PROT2_CLR && release_stale)*regp(&cm,NAF_IFR_PROT2_STA1)|=16;
 if(pa==NAF_IFR_PROT2_SET && drop_ack)*regp(&cm,NAF_IFR_PROT2_STA1)&=~16u;
 return r;
}
static const struct nah_io pio={pr,pw,alldelay,allworker,alllease};
static void pi(const uint32_t *w){prepare(w);release_stale=drop_clear=drop_set=drop_ack=clears=0;strict=1;assert(!nau_hw_init(&hw,&pio,0,nah_board_profile()));}
int main(int argc,char **argv){
 assert(argc==2);load_board(argv[1]);
 _Static_assert(NAU_S_PROT2_EN==69,"original69 measured indices unchanged");
 _Static_assert(NAU_SNAP_COUNT==70,"single appended synthetic request word");
 assert(nau_snapshot_pa[NAU_S_PROT2_EN]==0x10001710);
 /* ACK stays1 after release during startup, shutdown and nextstart. */
 pi(actual_snapshot);release_stale=1;
 for(unsigned k=0;k<3;k++){cycle(1000000ull*(k+1),0,0);assert((reg(NAF_IFR_PROT2_STA1)&16) && !(reg(0x10001710)&16));}
 /* All request/ACK combinations are classified independently; only request
  * determines inherited ownership, never the asynchronously stale ack. */
 for(unsigned k=0;k<4;k++){
  uint32_t w[NAU_SNAP_COUNT];memcpy(w,actual_snapshot,sizeof w);
  w[NAU_S_PROT2_EN]=(w[NAU_S_PROT2_EN]&~16u)|((k&1)?16u:0);
  w[NAU_S_PROT2_STA1]=(w[NAU_S_PROT2_STA1]&~16u)|((k&2)?16u:0);
  struct naf_own o;assert(!naf_ownership(w,&o) && o.prot_clear==!(k&1));
 }
 /* Dropped initial CLEAR: request remains1 even if stale ACK is0. */
 uint32_t w[NAU_SNAP_COUNT];memcpy(w,actual_snapshot,sizeof w);w[NAU_S_PROT2_EN]|=16;
 pi(w);drop_clear=1;
 assert(nau_hw_start(&hw,48000,2,16,0,256,0,1)==NAH_E_UNCERTAIN);
 assert(!afe_accesses && hw.afe.last_pa==0x10001710 && hw.afe.failure==NAF_FAIL_POLL);
 /* Request drift must refuse before any playback writes. */
 pi(actual_snapshot);*regp(&cm,0x10001710)|=16;
 unsigned before=cm.writes,starts=bm.starts;
 assert(nau_hw_start(&hw,48000,2,16,0,256,0,1)==NAH_E_COLD);
 assert(cm.writes==before && bm.starts==starts && !afe_accesses);
 /* Dropped final CLEAR: AUDIO safelyoff, but request not restored => uncertain. */
 pi(actual_snapshot);cm.current=NAF_SRAM;
 assert(!nau_hw_start(&hw,48000,2,16,0,256,0,1));drop_clear=2;
 assert(nau_hw_stop(&hw)==NAH_E_UNCERTAIN && hw.afe.last_pa==0x10001710);
 assert(!(reg(NAF_SPM_PWR_STATUS)&(1u<<21)) && (reg(0x10001710)&16));
 /* StaleACK1 cannot let dropped SET pass; missingACK after committedSET
  * still blocks SRAM powerdown. Both keep domainon and ampsdisabled. */
 for(unsigned k=0;k<2;k++){
  pi(actual_snapshot);release_stale=1;cm.current=NAF_SRAM;
  assert(!nau_hw_start(&hw,48000,2,16,0,256,0,1));
  if(k)drop_ack=1;else drop_set=1;
  assert(nau_hw_stop(&hw)==NAH_E_UNCERTAIN);
  assert(hw.afe.last_pa==(k?NAF_IFR_PROT2_STA1:0x10001710));
  assert((reg(NAF_SPM_AUDIO_PWR)&0x111f)==0xd);
  assert((bm.slave[0].reg[0]&9)==1 && (bm.slave[1].reg[0]&9)==1);
 }
 puts("PASS protection request ownership/watch,3 stale-releaseACK cycles,droppedCLEAR/SET and mandatoryACK1; old69 indices preserved, requestword synthetic");
}
