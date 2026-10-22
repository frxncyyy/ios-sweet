#include "wifi_station_transport.h"
#include "wifi_station_diag.h"
static uint16_t u16(const uint8_t*p){return p[0]|(uint16_t)p[1]<<8;}
static uint32_t u32(const uint8_t*p){return u16(p)|(uint32_t)u16(p+2)<<16;}
static void p16(uint8_t*p,uint16_t n){p[0]=(uint8_t)n;p[1]=(uint8_t)(n>>8);}
static void p32(uint8_t*p,uint32_t n){p16(p,(uint16_t)n);p16(p+2,(uint16_t)(n>>16));}
static void cp(void*d,const void*s,size_t n){uint8_t*a=d;const uint8_t*b=s;while(n--)*a++=*b++;}
static int eq(const void*a,const void*b,size_t n){const uint8_t*x=a,*y=b;while(n--)if(*x++!=*y++)return 0;return 1;}
static void wipe(void*p,size_t n){volatile uint8_t*q=p;while(n--)*q++=0;}
static int mac(const uint8_t*p){unsigned any=0;for(unsigned i=0;i<6;i++)any|=p[i];return any&&!(p[0]&1);}
void nst_close_port(struct nst*s){if(s){if(s->controlled_port&&!s->cleanup_started)s->deadline=s->last>UINT64_MAX-UINT64_C(30000000)?UINT64_MAX:s->last+UINT64_C(30000000);s->controlled_port=0;if(s->wire_busy&&s->wire_type==3&&!s->published){wipe(s->wire,sizeof s->wire);s->wire_busy=0;}}}
static int fail(struct nst*s,unsigned e){nsdd_summary(&s->data_diag,s->generation);{struct nstd_record record;nstd_summary(&record,s->generation,s->ticket);nstd_log(&record);}nst_close_port(s);s->halt=1;s->error=e;s->phase=NST_HALTED;if(s->init&&s->mutated){s->init->halt=1;s->init->transport.halt=1;s->init->transport.dirty=1;}return -1;}
static int source(const struct nst*s){return s&&s->prepared&&s->init&&s->previous&&s->previous_rx&&!s->halt&&!s->init->halt&&!s->init->transport.halt;}
int nst_lease_window(const struct nst*s,const struct nwi_state*i){
 if(!source(s)||s->credit_available+s->credit_reserved!=s->credit_capacity||s->credit_reserved>1||s->credit_released>s->credit_acquired||s->credit_acquired-s->credit_released!=s->credit_reserved||!s->active||s->init!=i||!i->initialized||s->tx_next>=1024||s->dma_completed>s->submitted||s->submitted-s->dma_completed>1||s->peer.bss||i->transport.tx_index!=s->tx_next||i->transport.completed!=s->base_completed+s->dma_completed-s->normal.dma_completed||i->transport.rx_index!=s->queue[1].next)return 0;
 if(s->normal.credit_available+s->normal.credit_reserved!=1||s->normal.credit_reserved>1||s->normal.credit_released>s->normal.credit_acquired||s->normal.credit_acquired-s->normal.credit_released!=s->normal.credit_reserved)return 0;
 if(s->normal.next>=1024||s->normal.token_next>=NSTX_TOKEN_COUNT||s->normal.token>=NSTX_TOKEN_COUNT||s->normal.ring!=i->rings[0].descriptors||s->normal.buffer!=i->data_buffer||!s->normal.ring||s->normal.buffer+4096!=i->end||s->normal.completed>s->normal.dma_completed||s->normal.dma_completed>s->normal.submitted||s->normal.submitted-s->normal.completed>1||s->normal.dma_completed>s->dma_completed||s->normal.submitted>s->submitted)return 0;
 for(unsigned k=0;k<5;k++){const struct ncr_queue*q=&s->queue[k];const struct nwi_ring*r=&i->rings[k+4];if(q->next>=r->count||q->ring.descriptors!=r->descriptors||q->ring.buffers!=r->buffers||q->ring.base!=r->base||q->ring.count!=r->count)return 0;}
 return 1;
}
/* nsb_scan.partial is aggregate: overflow drops whole excess records but never
 * damages retained rows. Terminal state7 excludes firmware failure; timed_out
 * and active exclude timeout/clock rollback. Only a full bounded result array
 * with real dropped count can explain a successful partial scan here. */
int nst_scan_admissible(const struct nsb_scan*s){
 return s&&s->entered&&s->done&&!s->active&&!s->timed_out&&s->completion.state==7&&
 s->completion.sequence==s->sequence&&s->count<=NSB_RESULTS&&s->count<=s->frames&&
 s->dropped<=s->frames-s->count&&s->last>=s->started&&s->last<s->deadline&&s->deadline>s->started&&
 (s->partial?(s->dropped&&s->count==NSB_RESULTS):!s->dropped);
}
int nst_prepare(struct nst*s,struct nwi_state*i,const struct nct_session*t,const struct ncr_state*r,const struct ncs_cap*c,const struct nst_peer*p,const struct sw_txd*d,uint64_t gen,uint64_t now,uint64_t end){
 if(!s||s->prepared||!i||!t||!r||!c||!p||!d||!gen||now>=end||end-now>UINT64_C(60000000)||!nct_done_receipt(t)||!t->capacity||t->init!=i||i->transport.tx_index!=(t->first_tx+t->count)%1024||i->transport.completed!=t->base_completed+t->count||i->transport.next_sequence!=(uint8_t)(t->first_sequence+t->count)||r->init!=i||r->session!=t||r->active||r->halt||r->error||!nst_scan_admissible(&r->scan)||i->transport.submit_attempted||!mac(p->self)||!mac(p->ap)||p->bss||p->sta||p->own_mac!=c->own_mac||p->group_wlan!=c->bmc||p->wlan<=p->group_wlan||p->wlan>=c->wtbl-1||c->wtbl<7||d->own_mac!=p->own_mac||d->wlan!=p->wlan||d->pse_header_len!=8||d->queue!=1||!((p->band==1&&p->channel>=1&&p->channel<=14)||(p->band==2&&p->channel>14)))return -1;
 if(i->rings[0].base!=0x18025300||i->rings[0].count!=1024||i->rings[0].rx||!i->rings[0].descriptors||i->rings[0].descriptors<i->claim.base||i->rings[0].descriptors-i->claim.base>i->claim.bytes||16384>i->claim.bytes-(i->rings[0].descriptors-i->claim.base)||!i->data_buffer||(i->data_buffer&4095)||i->data_buffer<i->claim.base||i->data_buffer-i->claim.base>i->claim.bytes||4096>i->claim.bytes-(i->data_buffer-i->claim.base)||i->end!=i->data_buffer+4096)return -1;
 s->normal.credit_available=1;s->normal.ring=i->rings[0].descriptors;s->normal.buffer=i->data_buffer;
 /* BIP0..3, initial BMC4, peer5 are already reserved. The one-interface
  * first-connection ledger owns only genuinely available6..8 below reserved
  * final WTBL entry; no arbitrary caller-selected group slots are admitted. */
 for(unsigned k=0;k<3;k++)if(p->group_wlan==4&&p->wlan==5&&6u+k<c->wtbl-1u)s->group_spare[k]=(uint8_t)(6u+k);
 s->credit_capacity=s->credit_available=t->capacity;s->init=i;s->previous=t;s->previous_rx=r;s->peer=*p;s->txd=*d;s->generation=gen;s->last=now;s->deadline=end;s->tx_next=i->transport.tx_index;s->base_completed=i->transport.completed;s->next_pid=1;
 for(unsigned k=0;k<5;k++)s->queue[k]=r->queue[k];s->prepared=1;return 0;
}
int nst_cleanup_begin(struct nst*s,uint64_t now,uint64_t end){
 if(!source(s)||s->cleanup_started||now<s->last||now>=end||end-now>UINT64_C(30000000))return -1;
 s->cleanup_started=1;s->last=now;s->deadline=end;nst_close_port(s);return 0;
}
static int acceptable(const struct nst*s){return source(s)&&!s->callback_active;}
static int busy(const struct nst*s){return s->wire_busy||s->air_pending||s->peer_kind;}
static int cid_size(uint8_t cid,size_t n){switch(cid){case 7:return n==64;case 0x0a:return n==68;case 0x11:return n==12;case 0x12:return n==116;case 0x13:return n==200;case 0x14:return n==4;case 0x16:return n==12;case 0x17:return n==4;case 0x19:return n==22;case 0x1c:return n==24;default:return 0;}}
static int group_slot(const struct nst*s,unsigned key){
 if(key>3)return -1;if(s->gtk_mapped&(1u<<key))return s->gtk_wlan[key];
 if(!s->gtk_mapped)return s->peer.group_wlan;
 for(unsigned k=0;k<3;k++){unsigned x=s->group_spare[k],used=0;if(!x)continue;for(unsigned j=0;j<4;j++)if((s->gtk_mapped&(1u<<j))&&s->gtk_wlan[j]==x)used=1;if(!used)return (int)x;}
 return -1;
}
static int command(struct nst*s,uint64_t ticket,uint8_t cid,const uint8_t*p,size_t n,unsigned internal){
 if(!acceptable(s)||!ticket||!p||!cid_size(cid,n))return -1;if(s->wire_busy||s->air_pending)return 1;
 if(cid==7&&(p[10]!=s->peer.bss||!eq(p+4,s->peer.ap,6)||p[11]!=(p[0]?4:0)||p[12]>3||p[13]!=(p[0]?16:0)||p[14]!=(p[2]?s->peer.wlan:group_slot(s,p[12]))||p[0]>1||p[2]>1||p[3]||p[1]!=(p[0]?p[2]:0)||(p[2]&&p[12])))return -1;
 if(cid==7&&!p[2]&&((!p[0]&&!(s->gtk_mapped&(1u<<p[12])))||(p[0]&&s->gtk_submission_serial==UINT32_MAX)))return -1;
 if(cid==0x1c&&(p[0]!=s->peer.bss||(!p[2]&&(p[3]!=s->peer.channel||p[5]!=s->peer.band))||p[2]>1))return -1;
 wipe(s->wire,sizeof s->wire);unsigned bytes=(unsigned)n+64;p32(s->wire,bytes|(2u<<23));p32(s->wire+4,1u<<16);p16(s->wire+32,(uint16_t)(bytes-32));s->wire[36]=cid;s->wire[37]=0xa0;s->wire[38]=1;s->wire[39]=s->init->transport.next_sequence;cp(s->wire+64,p,n);
 if(cid==7&&!p[2]){s->gtk_wlan[p[12]]=p[14];s->gtk_mapped|=1u<<p[12];}
 s->ticket=ticket;s->wire_cid=cid;s->wire_sequence=s->wire[39];s->wire_bytes=bytes;s->wire_type=0;s->wire_busy=1;s->published=s->dma_done=s->air_done=0;s->internal_wire=internal;
 return 0;
}
int nst_command(struct nst*s,uint64_t ticket,uint8_t cid,const uint8_t*p,size_t n){if(!acceptable(s))return -1;if(busy(s))return 1;return command(s,ticket,cid,p,n,0);}
static int frame(struct nst*s,uint64_t ticket,const uint8_t*p,size_t n,unsigned type,unsigned protect){
 if(!acceptable(s)||!ticket||!p||n<2||n>NST_FRAME_BYTES-32)return -1;if(busy(s))return 1;
 if(s->cleanup_started&&(type!=1||u16(p)!=0xc0))return -1;
 if(type==1&&(!s->next_pid||s->next_pid>NST_PID_MANAGEMENT_MAX))return -1;
 struct sw_txd d=s->txd;if(type==1&&(u16(p)==0xb0||u16(p)==0))d.queue=16;d.pid=(uint8_t)(type==1?s->next_pid:NST_PID_HOST);d.protected_frame=(uint8_t)protect;size_t used=0;uint8_t txd[32];int e;
 if(type==1)e=sw_management_txd(txd,sizeof txd,&used,&d,p,n);else if(type==2)e=sw_security_txd(txd,sizeof txd,&used,&d,p,n);else e=sw_data_txd(txd,sizeof txd,&used,&d,p,n);
 if(e||used!=32)return -1;
 if(type==1){if(n<24||!eq(p+4,s->peer.ap,6)||!eq(p+10,s->peer.self,6)||!eq(p+16,s->peer.ap,6))return -1;}
 else if(!eq(p+6,s->peer.self,6)||(type==2&&!eq(p,s->peer.ap,6)))return -1;
 wipe(s->wire,sizeof s->wire);cp(s->wire,txd,32);cp(s->wire+32,p,n);s->wire_bytes=(unsigned)n+32;s->ticket=ticket;s->wire_cid=0;s->wire_type=type;s->wire_pid=d.pid;s->wire_wlan=d.wlan;s->wire_busy=1;s->published=s->dma_done=s->air_done=0;s->internal_wire=0;return 0;
}
int nst_management(struct nst*s,uint64_t t,const uint8_t*p,size_t n){return frame(s,t,p,n,1,0);}
int nst_eapol(struct nst*s,uint64_t t,const uint8_t*p,size_t n,unsigned protect){if(protect>1||!s||(protect&&!s->ptk_ready))return -1;return frame(s,t,p,n,2,protect);}
int nst_data(struct nst*s,uint64_t t,const uint8_t*p,size_t n){if(!s||!s->controlled_port||s->bss_absent)return 1;return frame(s,t,p,n,3,1);}
int nst_memory_range(const struct nst*s,uint64_t a,uint32_t n,unsigned write){
 if(!source(s)||!s->active||!n)return 0;
 if(a==s->init->transport.command&&n==s->wire_bytes)return write&&s->phase==NST_TX_WRITE;
 if(a==s->init->transport.tx_ring+16u*s->tx_next&&n==16)return write?s->phase==NST_TX_WRITE:s->phase==NST_TX_READ;
 if(a==s->normal.ring+16u*s->normal.next&&n==16)return write?s->phase==NST_DATA_WRITE:s->phase==NST_DATA_READ;
 if(a==s->normal.buffer&&n==NSTX_PAGE_BYTES)return write?(s->phase==NST_DATA_WRITE||s->phase==NST_DATA_RETIRE):(s->phase==NST_DATA_VERIFY||s->phase==NST_DATA_RETIRE);
 if(s->rx_current>=5)return 0;const struct ncr_queue*q=&s->queue[s->rx_current];
 if(a==q->ring.descriptors+16u*q->next&&n==16)return write?s->phase==NST_RX_WRITE:s->phase==NST_RX_READ;
 return !write&&s->phase==NST_RX_FRAME&&n==s->rx_bytes&&a==q->ring.buffers+4096u*q->next;
}
int nst_register_write(const struct nst*s,uint32_t a,uint32_t v){if(!source(s)||!s->active)return 0;if(a==0x18025418)return s->phase==NST_TX_PUBLISH&&s->wire_busy&&!s->published&&v==(s->tx_next+1)%1024;if(a==0x18025308)return s->phase==NST_DATA_PUBLISH&&s->wire_busy&&!s->published&&!s->normal.published&&v==(s->normal.next+1)%1024;if(s->rx_current>=5||s->phase!=NST_RX_PUBLISH)return 0;const struct ncr_queue*q=&s->queue[s->rx_current];return a==q->ring.base+8&&v==q->next;}
/* After cleanup deauth DDONE, absent TXS is a bounded negative outcome,
 * not unknown DMA ownership. Security/data instead have typed local receipts
 * and never enter this air wait. Ordinary management, commands and every
 * pre-DDONE timeout retain the strict failure path. */
static int retired_air_failure(const struct nst*s){return s->cleanup_started&&s->wire_type==1&&s->air_pending&&s->dma_done&&!s->wire_busy&&s->submitted==s->dma_completed;}
static int before(struct nst*s,const struct ncd_io*o,void*c){uint64_t now;if(!nst_lease_window(s,s->init)||o->bus->owner(c)||o->now_us(c,&now)||now<s->last)return fail(s,1);s->last=now;if(!s->controlled_port&&now>=s->deadline)return fail(s,2);if((s->wire_busy||s->air_pending)&&s->published&&now>=s->operation_deadline&&!retired_air_failure(s))return fail(s,3);if(s->channel_held&&s->grant_deadline&&now>=s->grant_deadline&&!s->cleanup_started)return fail(s,4);return 0;}
static int rr(struct nst*s,const struct ncd_io*o,void*c,uint32_t a,uint32_t*v){return before(s,o,c)||o->bus->reg_read(c,a,v)?fail(s,5):0;}
static int rw(struct nst*s,const struct ncd_io*o,void*c,uint32_t a,uint32_t v){if(before(s,o,c)||!nst_register_write(s,a,v))return fail(s,6);s->mutated=1;return o->bus->reg_write(c,a,v)?fail(s,6):0;}
static int mr(struct nst*s,const struct ncd_io*o,void*c,uint64_t a,void*p,uint32_t n){return before(s,o,c)||!nst_memory_range(s,a,n,0)||o->bus->mem_read(c,a,p,n)?fail(s,7):0;}
static int mw(struct nst*s,const struct ncd_io*o,void*c,uint64_t a,const void*p,uint32_t n){if(before(s,o,c)||!nst_memory_range(s,a,n,1))return fail(s,8);s->mutated=1;return o->bus->mem_write(c,a,p,n)?fail(s,8):0;}
static int sy(struct nst*s,const struct ncd_io*o,void*c,uint64_t a,uint32_t n,unsigned way){return before(s,o,c)||!nst_memory_range(s,a,n,way==1)||o->bus->sync(c,a,n,way)?fail(s,9):0;}
static int emit(struct nst*s,nst_event_fn fn,void*c,struct nst_event*e){if(!fn)return 0;e->generation=s->generation;s->callback_active=1;int r=fn(c,e);s->callback_active=0;return r<0?fail(s,10):0;}
static int coherent(struct nst*s,const struct ncd_io*o,void*c){uint32_t v;for(unsigned d=0;d<2;d++)if(rr(s,o,c,0x18024200u+4096u*d,&v)||(v&0x00300000))return fail(s,11);return 0;}
/* Source normal AXI cut-through: ring progress and token lifetime are
 * independent. One host frame keeps the exact reserved page until BOTH retire. */
static int data_transmit(struct nst*s,const struct ncd_io*o,void*c,nst_event_fn fn,void*ctx){
 struct nstx*d=&s->normal;uint32_t v;uint64_t cell=d->ring+16u*d->next;
 if(!s->published){
  if(!nstx_idle(d)||rr(s,o,c,0x180600f0,&v)||(v&3)!=3||coherent(s,o,c)||rr(s,o,c,0x18025308,&v)||v!=d->next||rr(s,o,c,0x1802530c,&v)||v!=d->next)return fail(s,40);
  s->phase=NST_DATA_READ;if(sy(s,o,c,cell,16,2)||mr(s,o,c,cell,d->descriptor,16))return -1;if(!(u32(d->descriptor+4)&0x80000000))return fail(s,41);
  size_t used=0;d->token=d->token_next;struct sw_txd options=s->txd;options.pid=NST_PID_HOST;options.protected_frame=(uint8_t)!!(u32(s->wire+12)&2);
  if(nstx_build(d->page,sizeof d->page,&used,&options,s->wire+32,s->wire_bytes-32,s->wire_type==2,d->token,d->buffer)||used!=s->wire_bytes+32||nwt_tx_desc(d->descriptor,d->buffer,64))return fail(s,42);
  s->phase=NST_DATA_WRITE;if(mw(s,o,c,d->buffer,d->page,sizeof d->page)||sy(s,o,c,d->buffer,sizeof d->page,1))return -1;
  s->phase=NST_DATA_VERIFY;if(sy(s,o,c,d->buffer,sizeof d->page,2)||mr(s,o,c,d->buffer,s->data_verify,sizeof s->data_verify)||!eq(d->page,s->data_verify,sizeof d->page))return fail(s,43);wipe(s->data_verify,sizeof s->data_verify);
  s->phase=NST_DATA_WRITE;if(mw(s,o,c,cell,d->descriptor,16)||sy(s,o,c,cell,16,1))return -1;
  if(s->last>UINT64_MAX-UINT64_C(10000000))return fail(s,44);s->operation_deadline=s->last+UINT64_C(10000000);s->phase=NST_DATA_PUBLISH;
  /* Selected AXI halTxGetPageCount returns one regardless of length.
   * The native caller proves real PSE data quota>0; this serial ledger uses
   * only one unit and source releases it after successful publication. */
  if(!d->credit_available||d->credit_reserved)return fail(s,47);d->credit_available--;d->credit_reserved=1;d->credit_acquired++;
  if(rw(s,o,c,0x18025308,(d->next+1)%1024))return -1;
  d->credit_reserved=0;d->credit_available++;d->credit_released++;
  d->published=1;d->submitted++;d->token_next=(d->token_next+1)%NSTX_TOKEN_COUNT;s->published=1;s->submitted++;
  nsdd_publish(&s->data_diag,s->generation,s->submitted,s->wire_type,s->wire+32,s->wire_bytes-32);
  {struct nstd_record record;nstd_tx(&record,s->generation,s->ticket,d->page,s->wire_bytes,s->wire_type,0);nstd_log(&record);nstd_tx(&record,s->generation,s->ticket,d->page,s->wire_bytes,s->wire_type,1);nstd_log(&record);}
 }
 if(!d->dma_done){
  s->phase=NST_DATA_WAIT;if(rr(s,o,c,0x1802530c,&v))return -1;if(v!=d->next&&v!=(d->next+1)%1024)return fail(s,45);
  if(v==d->next)return 0;s->phase=NST_DATA_READ;if(sy(s,o,c,cell,16,2)||mr(s,o,c,cell,d->descriptor,16))return -1;if(!(u32(d->descriptor+4)&0x80000000))return 0;
  d->dma_done=1;d->dma_completed++;s->dma_completed++;
  {struct nstd_record record;nstd_tx(&record,s->generation,s->ticket,d->page,s->wire_bytes,s->wire_type,2);nstd_log(&record);}
 }
 if(!d->report_done)return 0;
 /* The report proves payload retirement only, not air success. Wipe after it
  * and descriptor retirement; no key/peer cleanup could pass wire_busy before. */
 wipe(d->page,sizeof d->page);wipe(s->wire,sizeof s->wire);s->phase=NST_DATA_RETIRE;
 if(mw(s,o,c,d->buffer,d->page,sizeof d->page)||sy(s,o,c,d->buffer,sizeof d->page,1)||sy(s,o,c,d->buffer,sizeof d->page,2)||mr(s,o,c,d->buffer,s->data_verify,sizeof s->data_verify)||!eq(d->page,s->data_verify,sizeof d->page))return fail(s,46);
 wipe(s->data_verify,sizeof s->data_verify);d->next=(d->next+1)%1024;d->completed++;d->published=d->dma_done=d->report_done=0;s->dma_done=1;s->wire_busy=0;s->phase=NST_IDLE;
 struct nst_event e={.kind=s->wire_type==2?NST_SECURITY_SUBMITTED:NST_DATA_SUBMITTED,.ticket=s->ticket,.pid=NST_PID_HOST,.wlan=s->peer.wlan};return emit(s,fn,ctx,&e);
}
static int transmit(struct nst*s,const struct ncd_io*o,void*c,nst_event_fn fn,void*ctx){
 if(!s->wire_busy||(s->wire_type==3&&s->bss_absent&&!s->published))return 0;
 if(s->cleanup_started&&!s->published&&s->wire_type&&!(s->wire_type==1&&u16(s->wire+32)==0xc0)){
  wipe(s->wire,sizeof s->wire);s->wire_busy=0;struct nst_event e={.kind=s->wire_type==2?NST_SECURITY_SUBMITTED:s->wire_type==3?NST_DATA_SUBMITTED:NST_FRAME_RESULT,.ticket=s->ticket,.status=1};return emit(s,fn,ctx,&e);
 }
 if(s->wire_type==2||s->wire_type==3)return data_transmit(s,o,c,fn,ctx);
 uint32_t v;uint64_t a=s->init->transport.tx_ring+16u*s->tx_next;
 if(!s->published){
  if(rr(s,o,c,0x180600f0,&v)||(v&3)!=3||coherent(s,o,c)||rr(s,o,c,0x18025418,&v)||v!=s->tx_next||rr(s,o,c,0x1802541c,&v)||v!=s->tx_next)return fail(s,12);
  s->phase=NST_TX_READ;if(sy(s,o,c,a,16,2)||mr(s,o,c,a,s->descriptor,16))return -1;if(!(u32(s->descriptor+4)&0x80000000))return fail(s,13);
  if(nwt_tx_desc(s->descriptor,s->init->transport.command,s->wire_bytes))return fail(s,14);
  if(!s->credit_available||s->credit_reserved)return fail(s,35);s->credit_available--;s->credit_reserved=1;s->credit_acquired++;
  s->phase=NST_TX_WRITE;if(mw(s,o,c,s->init->transport.command,s->wire,s->wire_bytes)||sy(s,o,c,s->init->transport.command,s->wire_bytes,1)||mw(s,o,c,a,s->descriptor,16)||sy(s,o,c,a,16,1))return -1;
  if(s->last>UINT64_MAX-UINT64_C(3000000))return fail(s,15);s->operation_deadline=s->last+UINT64_C(3000000);s->phase=NST_TX_PUBLISH;s->init->transport.submit_attempted=1;
  if(rw(s,o,c,0x18025418,(s->tx_next+1)%1024))return -1;s->published=1;s->submitted++;s->credit_reserved=0;s->credit_available++;s->credit_released++;
  if(s->wire_type==1){s->air_pending=1;s->next_pid++;}
  nsdd_publish(&s->data_diag,s->generation,s->submitted,s->wire_type,s->wire+32,s->wire_bytes-32);
  {struct nstd_record record;nstd_tx(&record,s->generation,s->ticket,s->wire,s->wire_bytes,s->wire_type,0);nstd_log(&record);nstd_tx(&record,s->generation,s->ticket,s->wire,s->wire_bytes,s->wire_type,1);nstd_log(&record);}
  if(s->wire_cid==0x13&&s->wire[64+20]==3&&s->wire[64+51]){s->activation_pending=1;s->activation_sequence=s->wire_sequence;}
  if(s->wire_cid==7&&s->wire[64]&&s->wire[64+2])s->ptk_command=1;
  if(s->wire_cid==0x1c&&!s->wire[66]){s->channel_token=s->wire[65];s->channel_held=1;}
 }
 s->phase=NST_TX_WAIT;if(rr(s,o,c,0x1802541c,&v))return -1;if(v!=s->tx_next&&v!=(s->tx_next+1)%1024)return fail(s,16);if(v==s->tx_next)return 0;
 s->phase=NST_TX_READ;if(sy(s,o,c,a,16,2)||mr(s,o,c,a,s->descriptor,16))return -1;if(!(u32(s->descriptor+4)&0x80000000))return 0;
 {struct nstd_record record;nstd_tx(&record,s->generation,s->ticket,s->wire,s->wire_bytes,s->wire_type,2);nstd_log(&record);}
 unsigned channel_release=s->wire_cid==0x1c&&s->wire[66]==1;
 if(s->internal_wire==2&&s->wire_cid==0x16)s->pm_connected=1;
 if(s->wire_cid==7){unsigned pair=s->wire[66];if(s->wire[64]){if(pair)s->ptk_submitted=1;else{s->gtk_submitted=1;s->gtk_command_index=s->wire[76];s->gtk_submission_serial++;s->gtk_command_added=1;}}else if(!pair){s->gtk_command_added=0;unsigned index=s->wire[76];wipe(&s->gtk_keys[index],sizeof s->gtk_keys[index]);wipe(&s->gtk,sizeof s->gtk);for(unsigned k=0;k<4;k++)if(s->gtk_keys[k].installed){s->gtk=s->gtk_keys[k];break;}}}
 /* DDONE proves host buffer no longer read. Erase key-bearing host/DMA copy now. */
 unsigned bytes=s->wire_bytes;wipe(s->wire,sizeof s->wire);s->phase=NST_TX_WRITE;if(mw(s,o,c,s->init->transport.command,s->wire,bytes)||sy(s,o,c,s->init->transport.command,bytes,1))return -1;
 s->init->transport.tx_index=(s->tx_next+1)%1024;s->tx_next=s->init->transport.tx_index;s->init->transport.completed++;if(!s->wire_type)s->init->transport.next_sequence++;s->init->transport.submit_attempted=0;s->dma_completed++;s->dma_done=1;s->wire_busy=0;s->phase=NST_IDLE;
 if(channel_release){s->channel_held=0;s->grant_deadline=0;struct nst_event released={.kind=NST_CHANNEL_RELEASED,.ticket=s->ticket,.token=s->channel_token};if(emit(s,fn,ctx,&released))return -1;}
 if(!s->wire_type){if(s->internal_wire){if(s->internal_wire==1)s->peer_step++;return 0;}struct nst_event e={.kind=NST_COMMAND_DMA,.ticket=s->ticket,.cid=(uint8_t)s->wire_cid,.sequence=(uint8_t)s->wire_sequence};return emit(s,fn,ctx,&e);}return 0;
}
static uint64_t pn48(const uint8_t*p){uint64_t n=0;for(unsigned k=0;k<6;k++)n|=(uint64_t)p[k]<<(8*k);return n;}
static int group_replay(struct nst*s,uint8_t key,const uint8_t rsc[8],unsigned replace){
 if(!source(s)||s->cleanup_started||s->link_lost||s->wire_type||s->wire_cid!=7||!s->dma_done||!s->gtk_command_added||key>3||!rsc||rsc[6]||rsc[7]||!s->gtk_submitted||key!=s->gtk_command_index||!(s->gtk_mapped&(1u<<key))||s->wire_busy||s->air_pending||s->gtk_submission_serial<=s->gtk_replay_serial)return -1;
 struct nst_replay*r=&s->gtk_keys[key];if(r->installed&&!replace)return -1;
 r->installed=1;r->key_id=key;for(unsigned k=0;k<17;k++)r->pn[k]=pn48(rsc);s->gtk=*r;s->gtk_replay_serial=s->gtk_submission_serial;return 0;
}
int nst_replay_install(struct nst*s,unsigned pair,uint8_t key,const uint8_t rsc[8]){
 if(!pair)return group_replay(s,key,rsc,0);
 if(!source(s)||pair!=1||key||!rsc||rsc[6]||rsc[7]||!s->ptk_submitted||s->ptk.installed)return -1;
 s->ptk.installed=1;s->ptk.key_id=0;for(unsigned k=0;k<17;k++)s->ptk.pn[k]=pn48(rsc);return 0;
}
int nst_replay_replace_group(struct nst*s,uint8_t key,const uint8_t rsc[8]){return group_replay(s,key,rsc,1);}
int nst_authorize(struct nst*s){if(!source(s)||s->link_lost||s->cleanup_started||!s->sta_active||!s->ptk_ready||!s->ptk.installed||!s->gtk.installed||s->wire_busy||s->air_pending||s->peer_kind)return -1;s->controlled_port=1;return 0;}
/* Deferred RX event uses owned rx_frame until descriptor is safely rearmed. */
static int decode(struct nst*s){
 const uint8_t*p=s->rx_frame;size_t n=s->rx_bytes;struct nst_event*e=&s->deferred;*e=(struct nst_event){0};if(n<4){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_EXTENT);return 0;}
 unsigned type=(u16(p+2)>>11)&31;
 if(type==6){nstx_report(&s->normal,p,n);return 0;}
 if(n<24){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_EXTENT);return 0;}
 if(type==0){struct nstd_record record={.generation=s->generation,.ticket=s->ticket,.kind=6,.v={0,(uint32_t)n,u16(p)}};nstd_log(&record);}
 if(type==7&&(u16(p+2)&0x380f)==0x3800&&n>=29&&p[28]==0x0f){struct nstd_record record;nstd_txs(&record,s->generation,s->ticket,p,n,s->air_pending,s->wire_pid,s->wire_wlan,s->air_done);nstd_log(&record);}
 if(type==7&&(u16(p+2)&0x380f)==0x3800){
  if(n<36)return 0;const uint8_t*b=p+36;size_t z=n-36;e->body=b;e->bytes=z;e->sequence=p[29];
  if(p[28]==0x0f)nsdd_event(&s->data_diag,s->generation,b,z,!!s->controlled_port);
  if(p[28]==0x0f&&z>=88&&s->air_pending&&b[0]==s->wire_pid&&b[4]==s->wire_wlan&&!s->air_done){s->air_done=1;s->air_status=b[1];}
  else if(p[28]==0x10&&z>=24&&s->channel_held&&b[0]==s->peer.bss&&b[1]==s->channel_token&&b[3]==s->peer.channel&&b[5]==s->peer.band){uint32_t ms=u32(b+12);if(b[2]||!ms||ms>60000||s->last>UINT64_MAX-(uint64_t)ms*1000)return 0;s->grant_deadline=s->last+(uint64_t)ms*1000;e->kind=NST_CHANNEL_GRANT;e->bss=b[0];e->token=b[1];e->status=b[2];e->channel=b[3];e->band=b[5];e->interval_ms=ms;}
  else if(p[28]==0x0c&&z>=8&&s->activation_pending&&p[29]==s->activation_sequence&&eq(b,s->peer.ap,6)&&b[6]==s->peer.sta&&b[7]==s->peer.bss){s->sta_active=1;s->activation_pending=0;e->kind=NST_STA_ACTIVE;e->sta=b[6];e->bss=b[7];}
  else if(p[28]==0x11&&z>=4&&b[0]==s->peer.bss&&b[1]<=1){s->bss_absent=b[1];}
  else if(p[28]==0x13&&z>=4&&b[0]==s->peer.bss&&s->sta_active){nst_close_port(s);s->link_lost=1;e->kind=NST_LINK_LOST;e->status=b[1];e->bss=b[0];}
  else if(p[28]==0x24&&z>=8&&s->ptk_command&&!s->ptk_ready&&b[0]==s->peer.bss&&eq(b+2,s->peer.ap,6)){s->ptk_ready=1;e->kind=NST_KEY_READY;e->bss=b[0];e->bytes=8;}
  else if(p[28]==0xf0||(p[28]==0xed&&p[32]==0x23)||p[28]==0xfd)return -1;
  return 0;
 }
 if(type!=2&&!(type==7&&(u16(p+2)&0x380f)==0x3801))return 0;
 size_t bytes=u16(p),off=24,g1=0,g4=0;uint32_t d1=u32(p+4),d2=u32(p+8),d4=u32(p+16);unsigned groups=(d1>>11)&31;
 if(bytes>n||bytes<24){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_EXTENT);return 0;}
 if(d1&0x0f800000u||d2&0x0b800000u||(d4&3)||(d4&(3u<<11))){nsdd_rx_error(&s->data_diag,s->generation,p,n);nsdd_reject(&s->data_diag,s->generation,NSDD_RX_ERROR);return 0;}
 if(groups&8){g4=off;off+=16;}if(groups&1){g1=off;off+=16;}if(groups&2)off+=8;if(groups&4)off+=8;if(groups&16)off+=72;off+=((d2>>14)&3)*2;if(off>bytes){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_GROUP_EXTENT);return 0;}
 const uint8_t*m=p+off;size_t len=bytes-off;unsigned translated=(d2>>13)&1,sec=(d1>>16)&31,key=(d1>>21)&3,wlan=d1&1023,tid=(d2>>16)&15;
 if(!translated){
  /* Selected gen4m accepts raw 802.11 data too (que_mgt.c3222+). Support
   * only plaintext non-QoS EAPOL here, with exact FromDS/AP/self ownership.
   * No CCMP bytes, A-MSDU, four-address, fragment or arbitrary LLC fallback. */
  if(type==2&&len>=2&&(u16(m)&12)==8){
   s->rx_untranslated++;
   if(len<36||len-18>NST_MAX_ETHERNET||((d2>>8)&31)!=12||sec||
      (u16(m)&0xc7ff)!=0x0208||(u16(m+22)&15)||wlan!=s->peer.wlan||
      !eq(m+4,s->peer.self,6)||!eq(m+10,s->peer.ap,6)||!eq(m+16,s->peer.ap,6)||
      !eq(m+24,"\xaa\xaa\x03\x00\x00\x00\x88\x8e",8)||
      ((size_t)m[34]<<8|m[35])!=len-36){s->rx_untranslated_drop++;nsdd_reject(&s->data_diag,s->generation,NSDD_RX_RAW_EAPOL);return 0;}
   uint8_t da[6],sa[6];cp(da,m+4,6);cp(sa,m+16,6);
   uint8_t*ether=s->rx_frame+off+18;cp(ether,da,6);cp(ether+6,sa,6);
   s->rx_untranslated_eapol++;e->kind=NST_EAPOL;e->body=ether;e->bytes=len-18;
   e->wlan=(uint8_t)wlan;return 0;
  }
  if(len<24||((d2>>8)&31)!=12||sec||u16(m)&0xc700||u16(m+22)&15||!eq(m+10,s->peer.ap,6)||!eq(m+16,s->peer.ap,6)){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_RAW_HEADER);return 0;}
  unsigned fc=u16(m)&255;if(fc==0xb0||fc==0x10||fc==0xc0||fc==0xa0){if(!eq(m+4,s->peer.self,6))return 0;e->kind=NST_MANAGEMENT;e->body=m;e->bytes=len;}
  /* Actual first beacon supplies DTIM for deferred source PM connected command. */
  else if(fc==0x80&&len>=36&&s->sta_active&&!s->pm_scheduled&&!s->pm_connected&&!s->cleanup_started&&!s->link_lost){for(size_t k=36;k<len;){if(len-k<2||m[k+1]>len-k-2)return 0;if(m[k]==5&&m[k+1]>=4&&m[k+3]){s->dtim_period=m[k+3];s->pm_pending=1;}k+=2+m[k+1];}}
  return 0;
 }
 if(type!=2||len<14||len>NST_MAX_ETHERNET||!g4||g4+16>bytes||!eq(p+g4+2,s->peer.ap,6)||!((m[0]&1)||eq(m,s->peer.self,6))){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_PEER);return 0;}
 uint16_t fc=u16(p+g4);if((fc&0x030c)!=0x0208||(fc&0x8400)||(u16(p+g4+8)&15)){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_FC);return 0;}
 int eapol=m[12]==0x88&&m[13]==0x8e;
 if(eapol){if(!eq(m+6,s->peer.ap,6)||!eq(m,s->peer.self,6)||len<18||((size_t)m[16]<<8|m[17])!=len-18){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_EAPOL);return 0;}}
 else if(!s->controlled_port||(unsigned)(m[12]<<8|m[13])<1536){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_PORT);return 0;}
 if(sec){
  if(sec!=4||!g1||g1+16>bytes||!(fc&0x4000)){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_CIPHER);return 0;}struct nst_replay*r=(m[0]&1)?&s->gtk_keys[key]:&s->ptk;
  if(!r->installed||key!=r->key_id||(wlan!=s->peer.wlan&&(!(m[0]&1)||wlan!=s->gtk_wlan[key]))){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_KEY);return 0;}
  unsigned lane=(fc&0x0080)?tid:16;uint64_t pn=pn48(p+g1);if(!pn||pn<=r->pn[lane]){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_REPLAY);return 0;}r->pn[lane]=pn;if((m[0]&1)&&s->gtk.key_id==key)s->gtk=*r;
 }else if(!eapol||(fc&0x4000)||wlan!=s->peer.wlan){nsdd_reject(&s->data_diag,s->generation,NSDD_RX_PLAINTEXT);return 0;}
 e->kind=eapol?NST_EAPOL:NST_ETHERNET;e->body=m;e->bytes=len;e->wlan=(uint8_t)wlan;e->key_id=(uint8_t)key;return 0;
}
static int receive(struct nst*s,const struct ncd_io*o,void*c,unsigned k,nst_event_fn fn,void*ctx){
 struct ncr_queue*q=&s->queue[k];uint32_t v;s->rx_current=k;s->phase=NST_IDLE;if(rr(s,o,c,q->ring.base+12,&v))return -1;if(v>=q->ring.count)return fail(s,20);if(v==q->next)return 0;
 uint64_t d=q->ring.descriptors+16u*q->next,a=q->ring.buffers+4096u*q->next;s->phase=NST_RX_READ;if(sy(s,o,c,d,16,2)||mr(s,o,c,d,s->rx_descriptor,16))return -1;uint32_t ctl=u32(s->rx_descriptor+4);if(!(ctl&0x80000000))return 0;
 s->rx_bytes=(ctl>>16)&0x3fff;unsigned last=!!(ctl&0x40000000),skip=q->segmented||!last||!s->rx_bytes||s->rx_bytes>NST_RX_BYTES;q->segmented=!last;s->deferred=(struct nst_event){0};
 if(!skip){s->phase=NST_RX_FRAME;if(sy(s,o,c,a,s->rx_bytes,2)||mr(s,o,c,a,s->rx_frame,s->rx_bytes))return -1;nstd_rx(s->generation,s->ticket,k,s->rx_frame,s->rx_bytes);if(decode(s))return fail(s,21);}
 if(nwt_rx_desc(s->rx_descriptor,a,NST_RX_BYTES))return fail(s,22);s->phase=NST_RX_WRITE;if(mw(s,o,c,d,s->rx_descriptor,16)||sy(s,o,c,d,16,1))return -1;s->phase=NST_RX_PUBLISH;if(rw(s,o,c,q->ring.base+8,q->next))return -1;q->next=(q->next+1)%q->ring.count;if(k==1)s->init->transport.rx_index=q->next;q->rearmed++;s->phase=NST_IDLE;
 if(s->deferred.kind==NST_ETHERNET)nsdd_receive(&s->data_diag,s->generation,s->deferred.body,s->deferred.bytes);
 if(s->deferred.kind&&emit(s,fn,ctx,&s->deferred))return -1;return 1;
}
static int peer_progress(struct nst*,nst_event_fn,void*);
int nst_step(struct nst*s,const struct ncd_io*o,void*c,unsigned budget,nst_event_fn fn,void*ctx){
 if(!source(s)||s->active||s->callback_active||!o||!o->bus||!o->now_us||!o->bus->owner||!o->bus->reg_read||!o->bus->reg_write||!o->bus->mem_read||!o->bus->mem_write||!o->bus->sync||!budget||budget>16)return -1;
 s->active=1;s->entered=1;s->phase=NST_IDLE;if(before(s,o,c)||coherent(s,o,c)||transmit(s,o,c,fn,ctx))return -1;
 static const unsigned order[5]={0,2,3,4,1};unsigned used=0;while(used<budget){unsigned progress=0;for(unsigned j=0;j<5&&used<budget;j++){int e=receive(s,o,c,order[j],fn,ctx);if(e<0)return -1;if(e){used++;progress++;}}if(!progress)break;}
 if(s->air_pending&&s->dma_done&&(s->air_done||(retired_air_failure(s)&&s->last>=s->operation_deadline))){
  /*0x100 is explicitly local timeout, outside every firmware8-bit status.
   * Management PID was consumed at publication and is never reused here; a
   * late old TXS cannot complete cleanup. No authorization follows failure. */
  unsigned status=s->air_done?s->air_status:NST_TX_LOCAL_TIMEOUT;
  struct nst_event e={.kind=NST_FRAME_RESULT,.ticket=s->ticket,.status=status,.pid=(uint8_t)s->wire_pid,.wlan=(uint8_t)s->wire_wlan};
  s->air_pending=0;
  if(status==NST_TX_LOCAL_TIMEOUT)nst_close_port(s);
  if(emit(s,fn,ctx,&e))return -1;
 }
 if(s->wire_busy&&s->normal.published&&(s->wire_type==2||s->wire_type==3)&&data_transmit(s,o,c,fn,ctx))return -1;
 if(peer_progress(s,fn,ctx))return -1;s->phase=NST_IDLE;s->active=0;return 0;
}
/* Original legacy policy: bit4/5 are unsupported22/33Mbps, not OFDM6/9. */
static int rate_index(unsigned v){static const uint8_t rates[14]={2,4,11,22,44,66,12,18,24,36,48,72,96,108};for(unsigned k=0;k<14;k++)if(rates[k]==v)return (k==4||k==5)?-1:(int)k;return -1;}
static int rate_sets(const uint8_t*p,size_t n,uint16_t*all,uint16_t*basic){uint16_t a=0,b=0;for(size_t k=0;k<n;k++){int j=rate_index(p[k]&127);if(j<0||(a&(1u<<j)))return -1;a|=(uint16_t)(1u<<j);if(p[k]&128)b|=(uint16_t)(1u<<j);}if(!a||!b)return -1;*all=a;*basic=b;return 0;}
static uint16_t lowest(uint16_t rates){static const uint16_t codes[14]={0,1,2,3,0xffff,0xffff,0x4b,0x4f,0x4a,0x4e,0x49,0x4d,0x48,0x4c};static const uint8_t order[12]={0,1,2,6,7,3,8,9,10,11,12,13};for(unsigned k=0;k<12;k++)if(rates&(1u<<order[k]))return codes[order[k]];return 0xffff;}
static unsigned phy(const struct nst*s){return s->peer.band==2?8:((s->negotiated_rates&0x3fc0)?3:1);}
int nst_legacy_rates(const uint8_t*p,size_t n,uint16_t*a,uint16_t*b,uint16_t*f){uint16_t x,y;if(!p||!n||n>12||!a||!b||!f||rate_sets(p,n,&x,&y))return -1;*a=x;*b=y;*f=lowest(y);return 0;}
int nst_peer_begin(struct nst*s,uint64_t ticket,unsigned kind,const struct nst_profile*p,const uint8_t*response,size_t n){
 if(!acceptable(s)||!ticket||kind<1||kind>3)return -1;if(busy(s))return 1;
 if(kind==NST_PREPARE_PEER){
  if(!p||!p->assoc.ssid||!p->assoc.ssid_len||p->assoc.ssid_len>32||!p->assoc.rates||!p->assoc.rates_len||p->assoc.rates_len>12||p->sta_version>1||!p->assoc.listen_interval||!p->beacon_interval||!eq(p->assoc.peer.self,s->peer.self,6)||!eq(p->assoc.peer.ap,s->peer.ap,6)||rate_sets(p->assoc.rates,p->assoc.rates_len,&s->negotiated_rates,&s->basic_rates))return -1;
  s->profile=*p;cp(s->ssid,p->assoc.ssid,p->assoc.ssid_len);cp(s->rates,p->assoc.rates,p->assoc.rates_len);s->profile.assoc.ssid=s->ssid;s->profile.assoc.rates=s->rates;s->txd.fixed_rate=lowest(s->basic_rates);
 }else if(kind==NST_CONFIGURE_PEER){
  if(!response||n<30||n>512||(u16(response)&~0x0800u)!=0x10||!eq(response+4,s->peer.self,6)||!eq(response+10,s->peer.ap,6)||!eq(response+16,s->peer.ap,6)||u16(response+26)||!(u16(response+24)&1)||!(u16(response+24)&16))return -1;
  uint16_t aid=u16(response+28)&0x3fff;if(!aid||aid>2007)return -1;uint8_t rates[12];size_t count=0;unsigned seen1=0,seen50=0;
  for(size_t off=30;off<n;){if(n-off<2||response[off+1]>n-off-2)return -1;unsigned id=response[off],bytes=response[off+1];off+=2;if(id==1||id==50){if(!bytes||(id==1&&(seen1++||bytes>8))||(id==50&&seen50++)||bytes>12-count)return -1;cp(rates+count,response+off,bytes);count+=bytes;}off+=bytes;}
  uint16_t all=0,basic=0;if(!seen1||rate_sets(rates,count,&all,&basic)||basic&~s->negotiated_rates)return -1;
  all&=s->negotiated_rates;if(!all||basic&~all)return -1;s->negotiated_rates=all;s->basic_rates=basic;s->aid=aid;s->txd.fixed_rate=lowest(basic);s->profile.assoc.capability&=(uint16_t)(u16(response+24)|0x11);
 }else nst_close_port(s);
 s->peer_ticket=ticket;s->peer_kind=kind;s->peer_step=0;return 0;
}
static int peer_progress(struct nst*s,nst_event_fn fn,void*ctx){
 if(s->wire_busy||s->air_pending)return 0;
 if(!s->peer_kind){
  /* Power-management connect is source-deferred to a real postjoin beacon. */
  if(s->pm_pending&&!s->pm_scheduled&&!s->pm_connected&&!s->cleanup_started&&!s->link_lost&&s->sta_active&&s->aid){uint8_t b[12]={0};b[0]=s->peer.bss;b[1]=(uint8_t)s->dtim_period;p16(b+2,s->aid);p16(b+4,s->profile.beacon_interval);int rc=command(s,UINT64_MAX,0x16,b,sizeof b,2);if(!rc){s->pm_pending=0;s->pm_scheduled=1;}return rc;}
  return 0;
 }
 unsigned kind=s->peer_kind,step=s->peer_step;uint8_t payload[200];size_t used=0;uint8_t cid=0;
 struct sw_rlm r={s->peer.bss,s->peer.band,s->peer.channel,s->profile.erp_protect,(uint8_t)!!(s->profile.assoc.capability&32),(uint8_t)!!(s->profile.assoc.capability&1024),1,1};
 struct sw_bss b={.rlm=r,.ssid=s->ssid,.ssid_len=s->profile.assoc.ssid_len,.connected=(uint8_t)(kind==NST_CONFIGURE_PEER),.ap_sta=s->peer.sta,.bmc_wlan=s->peer.group_wlan,.nonht_phy=(uint8_t)(s->peer.band==2?3:(phy(s)==1?0:1)),.phy_set=(uint8_t)phy(s),.dbdc_band=4,.rates=s->negotiated_rates,.basic_rates=s->basic_rates,.fixed_rate=s->txd.fixed_rate};cp(b.ap,s->peer.ap,6);
 /* cnm_mem.c1141-1142: STA-record BMC is WTBL_RESERVED_ENTRY; BSS b above keeps real BMC. */
 struct sw_sta a={.bss=s->peer.bss,.sta=s->peer.sta,.wlan=s->peer.wlan,.bmc_wlan=SW_WTBL_RESERVED_ENTRY,.sta_type=65,.state=(uint8_t)(kind==NST_CONFIGURE_PEER?3:1),.need_response=(uint8_t)(kind==NST_CONFIGURE_PEER),.phy_set=(uint8_t)phy(s),.rcpi=s->profile.rcpi,.rts_policy=3,.version=s->profile.sta_version,.aid=s->aid,.listen_interval=s->profile.assoc.listen_interval,.rates=s->negotiated_rates,.basic_rates=s->basic_rates,.fixed_rate=s->txd.fixed_rate};cp(a.ap,s->peer.ap,6);
 if(kind==NST_PREPARE_PEER&&step==0){wipe(payload,68);p32(payload,13);return command(s,s->peer_ticket,0x0a,payload,68,1);}
 if(kind==NST_PREPARE_PEER)step--;
 if(kind==NST_RETIRE_PEER){
  if(step==0){payload[0]=0;payload[1]=s->peer.sta;payload[2]=s->peer.bss;payload[3]=0;used=4;cid=0x14;}
  else if(step==1){if(sw_bss(payload,sizeof payload,&used,&b))return fail(s,30);cid=0x12;}
  else if(step==2){wipe(payload,4);payload[0]=s->peer.bss;used=4;cid=0x17;}
  else{nsdd_summary(&s->data_diag,s->generation);s->peer_kind=0;s->sta_active=s->ptk_ready=0;wipe(&s->ptk,sizeof s->ptk);wipe(&s->gtk,sizeof s->gtk);wipe(s->gtk_keys,sizeof s->gtk_keys);{struct nstd_record record;nstd_summary(&record,s->generation,s->ticket);nstd_log(&record);}struct nst_event e={.kind=NST_PEER_RETIRED,.ticket=s->peer_ticket};return emit(s,fn,ctx,&e);}
 }else{
  if(step==0){if(sw_bss(payload,sizeof payload,&used,&b))return fail(s,31);cid=0x12;}
  else if(step==1){if(sw_rlm(payload,sizeof payload,&used,&r))return fail(s,32);cid=0x19;}
  else if(step==2){if(sw_sta(payload,sizeof payload,&used,&a))return fail(s,33);cid=0x13;}
  else if(kind==NST_CONFIGURE_PEER&&!s->sta_active)return 0;
  else if(kind==NST_CONFIGURE_PEER&&s->channel_held){struct sw_channel ch={s->peer.bss,s->channel_token,1,s->peer.band,s->peer.channel,4,0};if(sw_channel(payload,sizeof payload,&used,&ch))return fail(s,34);cid=0x1c;}
  else{s->peer_kind=0;struct nst_event e={.kind=kind==NST_CONFIGURE_PEER?NST_PEER_CONFIGURED:NST_PEER_PREPARED,.ticket=s->peer_ticket};return emit(s,fn,ctx,&e);}
 }
 return command(s,s->peer_ticket,cid,payload,used,1);
}
