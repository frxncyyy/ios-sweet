/* Actual amp/transport/AFE lifecycle with temporal startup responses. */
#define main original_full_main
#include "test-full.c"
#undef main
static void load(const char *path){unsigned char blob[4163];FILE*f=fopen(path,"rb");assert(f);assert(fread(blob,1,sizeof(blob),f)==sizeof(blob));fclose(f);static const uint8_t digest[32]={0x63,0x53,0x00,0xfb,0xc9,0x6f,0x25,0x8f,0xf9,0xa3,0x5f,0x82,0x43,0x5f,0x2e,0x9d,0x3f,0xf6,0xf1,0x37,0x0c,0x15,0x60,0x67,0xb4,0x01,0x46,0xe0,0x11,0x03,0x86,0x92};assert(!nah_board_load(blob,sizeof(blob),digest));}
static void prepare(unsigned mode){full_prepare(0x0c74,0x0c74,0);bm.slave[0].startup_mode=mode;assert(!nau_hw_init(&hw,&allio,0,nah_board_profile()));}
int main(int argc,char**argv){assert(argc==2);load(argv[1]);
 const unsigned transient[]={M_START_CLOCK_DIP,M_START_DELAY_CLOCK,M_START_PRE_TDM,M_START_POST_NOCLK,M_START_POST_CLOCK_DIP};
 for(unsigned i=0;i<sizeof(transient)/sizeof(*transient);i++){
  prepare(transient[i]);assert(!nau_hw_start(&hw,48000,2,16,0,256,0,0));
  const struct nah_amp_start *d=&hw.status.amp_start[0];assert(d->stage==NAH_AS_READY&&!d->error&&d->valid==3&&d->attempts>=5);assert(d->first_bad_valid);
  if(transient[i]==M_START_CLOCK_DIP || transient[i]==M_START_DELAY_CLOCK)assert(d->first_bad_stage==NAH_AS_PRE_WAIT&&!(d->first_bad_clocks&0x80));
  if(transient[i]==M_START_PRE_TDM)assert(d->first_bad_stage==NAH_AS_PRE_WAIT&&(d->first_bad_status&0x40));
  if(transient[i]==M_START_POST_CLOCK_DIP)assert(d->first_bad_stage==NAH_AS_POST_WAIT&&!(d->first_bad_clocks&0x80));
  if(transient[i]==M_START_POST_NOCLK)assert(d->first_bad_stage==NAH_AS_POST_CLEAR&&(d->first_bad_status&0x80));
  struct nah_amp_start saved=*d;assert(!nau_hw_stop(&hw));assert(!memcmp(&saved,&hw.status.amp_start[0],sizeof saved));assert(!hw.status.uncertain&&!hw.status.stop_error);
 }
 const unsigned persistent[]={M_START_PERSIST_NOCLK,M_START_POST_TDM,M_START_PERSIST_CLOCK};
 for(unsigned i=0;i<sizeof(persistent)/sizeof(*persistent);i++){
  prepare(persistent[i]);assert(nau_hw_start(&hw,48000,2,16,0,256,0,0)==NAH_E_AMP_FAULT);
  const struct nah_amp_start*d=&hw.status.amp_start[0];assert(d->error==NAH_E_AMP_FAULT&&d->first_bad_valid&&d->failure_reg&&d->failure_pa);assert(d->attempts<=103&&bm.slave[0].pre_samples<=101&&bm.slave[0].post_samples<=102);
  assert(d->stage==(persistent[i]==M_START_PERSIST_CLOCK?NAH_AS_PRE_WAIT:NAH_AS_POST_WAIT));assert(!hw.status.running&&!hw.status.uncertain&&!hw.status.stop_error);assert((bm.slave[0].reg[0]&9)==1&&(bm.slave[1].reg[0]&9)==1);assert(!hw.status.pcm_frames);
  assert(hw.status.amp_status[0]==0); /* cleanup changed its old diagnostics */
  if(persistent[i]==M_START_PERSIST_NOCLK)assert(d->status&0x80);
  if(persistent[i]==M_START_POST_TDM)assert(d->status&0x40);
  if(persistent[i]==M_START_PERSIST_CLOCK)assert(!(d->clocks&0x80)&&!bm.slave[0].post_samples);
  if(persistent[i]==M_START_PERSIST_CLOCK)for(unsigned t=0;t<bm.ntrans;t++)assert(!(bm.trans[t].writing&&bm.trans[t].ptr==0&&(bm.trans[t].value&8)));
  struct nah_amp_start saved=*d;assert(!nau_hw_stop(&hw));assert(!memcmp(&saved,&hw.status.amp_start[0],sizeof saved));
 }
 prepare(M_START_POST_IO);assert(nau_hw_start(&hw,48000,2,16,0,256,0,0)==NAH_E_IO);assert(hw.status.amp_start[0].stage==NAH_AS_POST_WAIT&&hw.status.amp_start[0].error==NAH_E_IO&&hw.status.amp_start[0].failure_reg==0x10);assert(hw.status.amp_start[0].attempts==3&&!hw.status.running&&!hw.status.pcm_frames);
 /* Persistent pre-enable TDM must never set AMPE. */
 prepare(M_START_CLEAN);bm.slave[0].reg[0x10]=0x40;assert(nau_hw_start(&hw,48000,2,16,0,256,0,0)==NAH_E_AMP_FAULT);assert(hw.status.amp_start[0].stage==NAH_AS_PRE_WAIT&&hw.status.amp_start[0].attempts==101&&!bm.slave[0].post_samples);for(unsigned t=0;t<bm.ntrans;t++)assert(!(bm.trans[t].writing&&bm.trans[t].ptr==0&&(bm.trans[t].value&8)));
 prepare(M_START_CLEAN);for(unsigned n=0;n<3;n++){assert(!nau_hw_start(&hw,48000,2,16,0,256,0,n*1000000));assert(hw.status.amp_start[0].stage==NAH_AS_READY&&hw.status.amp_start[1].stage==NAH_AS_READY);assert(hw.status.amp_start[0].attempts==4&&!hw.status.amp_start[0].first_bad_valid);assert(!nau_hw_stop(&hw));}
 assert(!bm.slave[5].accessed&&!bm.slave[6].accessed);
 puts("PASS actual startup INIT_CF order, transient clock/TDM/post-AMPE NOCLK settle, bounded persistent pre/post faults, immediateIO, immutable cause through cleanup, three clean0c74 restarts; no PCM on refused start");}
