#include "wifi_main_transport.h"
#include "wifi_main_layout.h"
#define DMA0 UINT32_C(0x18024000)
#define DMA1 UINT32_C(0x18025000)
#define TXBASE (DMA1+0x410)
#define RXBASE (DMA1+0x500)
static uint32_t get32(const uint8_t *p){return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;}
static int fail(struct nwt_state *s,unsigned e){s->error=e;s->halt=s->dirty||s->phase==2;return -1;}
static int own(const struct nwt_io *o,void *c,struct nwt_state *s){
 if(o->owner(c))return fail(s,2);
 s->callback_count++;return 0;
}
static int rr(const struct nwt_io *o,void *c,struct nwt_state *s,uint32_t a,uint32_t *v){if(own(o,c,s))return -1;return o->reg_read(c,a,v)?fail(s,3):0;}
static int rw(const struct nwt_io *o,void *c,struct nwt_state *s,uint32_t a,uint32_t v){if(own(o,c,s))return -1;s->dirty=1;return o->reg_write(c,a,v)?fail(s,4):0;}
static int mr(const struct nwt_io *o,void *c,struct nwt_state *s,uint64_t a,void *p,uint32_t n){if(own(o,c,s))return -1;return o->mem_read(c,a,p,n)?fail(s,5):0;}
static int mw(const struct nwt_io *o,void *c,struct nwt_state *s,uint64_t a,const void *p,uint32_t n){if(own(o,c,s))return -1;s->dirty=1;return o->mem_write(c,a,p,n)?fail(s,6):0;}
static int sync(const struct nwt_io *o,void *c,struct nwt_state *s,uint64_t a,uint32_t n,unsigned way){if(own(o,c,s))return -1;return o->sync(c,a,n,way)?fail(s,7):0;}
static int wait(const struct nwt_io *o,void *c,struct nwt_state *s){if(own(o,c,s))return -1;return o->delay_us(c,1000)?fail(s,8):0;}
static int valid(const struct nwt_io *o){return o&&o->owner&&o->reg_read&&o->reg_write&&o->mem_read&&o->mem_write&&o->sync&&o->delay_us;}
int nwt_preflight(const struct nwt_io *o,void *c,struct nwt_state *s,unsigned opt){
 if(!valid(o)||!s||opt!=1||s->entered)return -1;
 s->entered=1;s->missing_init=NWT_REQUIRED_FULL_INIT;
 for(unsigned dma=0;dma<2;dma++){
  uint32_t b=dma?DMA1:DMA0;
  if(rr(o,c,s,b+0x208,&s->cfg[dma]))return -1;
  s->cfg_reads++;
  if(s->cfg[dma]&15)return fail(s,10); /*enables/busy: no mutation allowed*/
  /* Exact defined rings changed by selectedmanualprefetch: DMA0RX0..7;
   * DMA1RX0..2 andTX0..5,8..13,16..19. No guessedreservedringreads. */
  for(unsigned r=0;r<28;r++){
   if((!dma&&r<20)||(dma&&r>=23)||(dma&&r<20&&((r>=6&&r<8)||(r>=14&&r<16))))continue;
   for(unsigned w=0;w<4;w++){
   uint32_t a=b+(r<20?0x300+16*r:0x500+16*(r-20))+4*w;
   if(rr(o,c,s,a,&s->ring[dma][r][w]))return -1;
   s->ring_reads++;
   if(s->ring[dma][r][w])return fail(s,11);
   }
  }
 }
 s->preflight_complete=1;s->phase=1;
 return fail(s,20); /*finiteunsupportedfullinit;neverresetunknownhardware*/
}
/* IRQ output is disabled: poll source-defined RX/TX coherent faults.
 * Never acknowledge, reset or attempt recovery on this path. */
static int coherent(const struct nwt_io *o,void *c,struct nwt_state *s){
 s->irq_status_valid=0;
 for(unsigned dma=0;dma<2;dma++){
  if(rr(o,c,s,0x18024200u+0x1000u*dma,&s->irq_status[dma]))return -1;
  s->irq_status_valid|=1u<<dma;
  if(s->irq_status[dma]&0x00300000u)return fail(s,24);
 }
 return 0;
}
static int extent(uint64_t a,uint32_t n){return !a||a>UINT32_MAX||n-1>UINT32_MAX-a;}
int nwt_capture(const struct nwt_io *o,void *c,struct nwt_state *s,
 const uint8_t *cmd,size_t used,uint8_t eid,uint8_t *frame,size_t capacity,size_t *received){
 uint8_t d[16];size_t bytes=0;uint32_t idx,cfg;
 if(received)*received=0;
 if(!s||!cmd||used<64||used>4096||!frame||capacity<32||capacity>3840||!received||
 cmd[36]==0||cmd[37]!=0xa0||cmd[39]!=s->next_sequence)return -1;
 if(!valid(o)||!s||s->halt||s->missing_init||!s->preflight_complete||
    s->phase!=2||s->submit_attempted||s->tx_index>=NWT_TX_COUNT||s->rx_index>=NWT_RX_COUNT||
    extent(s->tx_ring,16384)||extent(s->rx_ring,256)||extent(s->rx_buffers,65536)||extent(s->command,4096)||
    (s->tx_ring&15)||(s->rx_ring&15)||(s->rx_buffers&4095)||(s->command&4095))return -1;
 const uint64_t ranges[4]={s->tx_ring,s->rx_ring,s->rx_buffers,s->command};
 const uint32_t sizes[4]={16384,256,65536,4096};
 for(unsigned a=0;a<4;a++)for(unsigned b=a+1;b<4;b++)
  if(ranges[a]<ranges[b]+sizes[b]&&ranges[b]<ranges[a]+sizes[a])return -1;

 /* Globalactualinit/sourceownership receipt must already exist. Checkselected
  * enable+addressmode beforeIO; this is not asubstitute for missinginit. */
 if(coherent(o,c,s))return -1;
 if(rr(o,c,s,DMA1+0x208,&cfg))return -1;
 if((cfg&5)!=5||(cfg&(1u<<26)))return fail(s,12);
 if(rr(o,c,s,TXBASE+8,&idx))return -1;
 if(idx!=s->tx_index)return fail(s,13);
 uint64_t txd=s->tx_ring+16u*s->tx_index,rxd=s->rx_ring+16u*s->rx_index;
 uint64_t rxbuf=s->rx_buffers+4096u*s->rx_index;
 s->rx_descriptor_address=rxd;s->rx_expected_buffer=rxbuf;
 s->rx_diag_valid=1;s->rx_failure_reason=0;s->rx_didx=0;
 s->rx_expected_eid=eid;s->rx_expected_sequence=s->next_sequence;
 s->rx_packet_type=s->rx_event_eid=s->rx_event_sequence=s->rx_status=s->rx_cid=0;
 for(unsigned word=0;word<4;word++)s->rx_descriptor_raw[word]=0;
 if(nwt_tx_desc(d,s->command,used))return fail(s,14);
 s->tx_descriptor_address=txd;s->tx_used=(uint32_t)used;s->tx_descriptor_valid=1;
 s->tx_mismatch=0;s->tx_ddone_waits=0;s->tx_consumed_zero=0;s->tx_didx=s->tx_index;
 for(unsigned word=0;word<4;word++){s->tx_submitted[word]=get32(d+4*word);s->tx_observed[word]=0;}
 if(mw(o,c,s,s->command,cmd,(uint32_t)used)||sync(o,c,s,s->command,(uint32_t)used,1)||mw(o,c,s,txd,d,16)||sync(o,c,s,txd,16,1))return -1;
 unsigned next=(s->tx_index+1)%NWT_TX_COUNT;s->submit_attempted=1;
 if(rw(o,c,s,TXBASE+8,next))return -1;
 unsigned i;
 for(i=0;i<1000;i++){
  if(i&&wait(o,c,s))return -1;
  if(coherent(o,c,s))return -1;
  if(rr(o,c,s,TXBASE+12,&idx))return -1;
  s->polls++;
  s->tx_didx=idx;
  if(idx>=NWT_TX_COUNT)return fail(s,15);
  if(idx==s->tx_index)continue;
  if(idx!=next)return fail(s,15);
  if(sync(o,c,s,txd,16,2)||mr(o,c,s,txd,d,16))return -1;
  for(unsigned word=0;word<4;word++)s->tx_observed[word]=get32(d+4*word);
  s->tx_descriptor_valid|=2;
  /* Selected hal_pdma completion waits for DMADONE before consuming any
   * descriptor fields. Preserve strict echo checks once writeback is done. */
  if(!(get32(d+4)&(1u<<31))){s->tx_ddone_waits++;continue;}
  uint32_t control=get32(d+4),length=(control>>16)&0x3fffu;
  /* Observed selected MT6893 completion consumes SDL0 and LastSec0.
   * Zero length accepts only exact DDONE-only; original-length checks
   * remain unchanged.
   * Retained software used/command extent remains the ownership authority. */
  s->tx_mismatch=(get32(d)!=(uint32_t)s->command?1u:0u)|
   (length!=0&&length!=used?2u:0u)|
   (get32(d+8)?4u:0u)|(get32(d+12)?8u:0u)|
   (length==0&&control!=UINT32_C(0x80000000)?16u:0u);
  if(s->tx_mismatch)return fail(s,16);
  s->tx_consumed_zero=control==UINT32_C(0x80000000);
  s->tx_done=1;break;
 }
 if(!s->tx_done)return fail(s,17);
 for(i=0;i<1000;i++){
  if(i&&wait(o,c,s)){s->rx_failure_reason=6;return -1;}
  if(coherent(o,c,s)){s->rx_failure_reason=5;return -1;}
  if(rr(o,c,s,RXBASE+12,&idx)){s->rx_failure_reason=6;return -1;}
  s->rx_didx=idx;s->rx_diag_valid|=2;
  s->polls++;
  if(idx>=NWT_RX_COUNT){s->rx_failure_reason=1;return fail(s,18);}
  if(idx==s->rx_index)continue;
  if(sync(o,c,s,rxd,16,2)||mr(o,c,s,rxd,d,16)){s->rx_failure_reason=6;return -1;}
  for(unsigned word=0;word<4;word++)s->rx_descriptor_raw[word]=get32(d+4*word);
  s->rx_diag_valid|=4;
  if(!(get32(d+4)&(1u<<31)))continue;
  if(nwt_rx_complete(d,rxbuf,NWT_RX_BYTES,&bytes)||bytes<32||bytes>capacity){s->rx_failure_reason=2;return fail(s,19);}
  if(sync(o,c,s,rxbuf,(uint32_t)bytes,2)||mr(o,c,s,rxbuf,frame,(uint32_t)bytes)){s->rx_failure_reason=6;return -1;}
  s->rx_packet_type=(unsigned)frame[26]|(unsigned)frame[27]<<8;
  s->rx_event_eid=frame[28];s->rx_event_sequence=frame[29];s->rx_diag_valid|=8;
  if(bytes>=34&&s->rx_packet_type==0xe000&&frame[28]==1){s->rx_status=frame[32];s->rx_cid=frame[33];s->rx_diag_valid|=16;}
  s->rx_descriptor_bytes=(uint32_t)bytes;
  s->rx_mac_bytes=get32(frame)&0xffffu;
  s->rx_init_bytes=(uint32_t)frame[24]|(uint32_t)frame[25]<<8;
  if(frame[26]!=0||frame[27]!=0xe0||frame[28]!=eid||frame[29]!=s->next_sequence){s->rx_failure_reason=3;return fail(s,21);}
  s->rx_done=1;break;
 }
 if(!s->rx_done){s->rx_failure_reason=4;return fail(s,22);}
 if(coherent(o,c,s)){s->rx_failure_reason=5;return -1;}
 /* Rearm only aftermatchingeventcopied/validated; stillownmemoryuntilreset. */
 if(nwt_rx_desc(d,rxbuf,NWT_RX_BYTES)){s->rx_failure_reason=2;return fail(s,23);}
 if(mw(o,c,s,rxd,d,16)||sync(o,c,s,rxd,16,1)||rw(o,c,s,RXBASE+8,s->rx_index)){s->rx_failure_reason=6;return -1;}
 s->tx_index=next;s->rx_index=(s->rx_index+1)%NWT_RX_COUNT;s->next_sequence++;
 s->completed++;s->submit_attempted=0;s->tx_done=s->rx_done=0;*received=bytes;
 return 0;
}

int nwt_exchange(const struct nwt_io *o,void *c,struct nwt_state *s,uint8_t cid){
 uint8_t cmd[72],frame[3840];size_t used=0,received=0;
 if(!s||cid!=(s->finish_phase?0x41:0x40)||s->finish_phase>1||nwt_finish(cmd,sizeof(cmd),cid,s->next_sequence,&used))return -1;
 if(nwt_capture(o,c,s,cmd,used,1,frame,sizeof(frame),&received))return -1;
 /* Selected source parses status and CID, ignoring embedded length fields.
  * Descriptor length need only contain the fields, within the owned buffer. */
 if(received<34||frame[32]!=0||frame[33]!=cid){s->dirty=1;return fail(s,21);}
 s->finish_phase++;return 0;
}
