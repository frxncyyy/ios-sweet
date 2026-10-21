#include "wifi_main_phy_exchange.h"
static int halt(struct nwi_state *i,struct nfa_exchange_state *s){
 s->halt=1;i->transport.halt=1;i->transport.dirty=1;return -1;
}
int nfa_run(const struct nwt_io *io,void *ctx,struct nwi_state *init,
 const struct nfa_board *board,const struct nfa_backup_io *backup_io,
 void *backup_ctx,void *backup_dst,uint32_t backup_capacity,
 struct nfa_exchange_state *s){
 if(!io||!init||!board||!backup_io||!backup_dst||!s||s->attempted||s->halt||
    !init->initialized||init->phase!=5||!init->driver_owned||init->halt||
    init->transport.halt||init->transport.phase!=2||
    init->transport.completed!=1||init->transport.finish_phase!=1||
    !init->transport.next_sequence)return -1;
 s->attempted=1;
 uint8_t seq=init->transport.next_sequence;
 if(nfa_force_frame(board,seq,s->tx))return halt(init,s);
 if(nwt_capture(io,ctx,&init->transport,s->tx,NFA_FRAME_BYTES,5,
                s->rx,sizeof(s->rx),&s->actual_rx_bytes))return halt(init,s);
 if(nfa_force_response(s->rx,s->actual_rx_bytes,seq,0x450000,&s->event))
  return halt(init,s);
 s->event_valid=1;
 if(nfa_backup(backup_io,backup_ctx,&s->event,backup_dst,
               backup_capacity,&s->backup))return halt(init,s);
 s->backup_valid=1;return 0;
}
