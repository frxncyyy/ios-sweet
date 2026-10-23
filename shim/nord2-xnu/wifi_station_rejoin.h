#ifndef WIFI_STATION_REJOIN_H
#define WIFI_STATION_REJOIN_H
/* Host-only teardown ledger. CID07 metadata is captured BEFORE nst_step wipes
 * its key-bearing command buffer, and consumed only on that command's actual
 * DDONE event. No keys, peer addresses or passwords are retained here. */
struct nsr_ledger {
 uint64_t refresh_requested_us,closed_generation,closed_us,scan_ticket,pending_ticket,admitted_generation;
 unsigned hw0_next,token_next,next_pid,scan_state,scan_ready,refresh_after_scan;
 unsigned ptk,gtk,pending,bad,key_pair,key_index,key_add;
};
static void nsr_key_capture(struct nsr_ledger*l,const struct nst*s){
 if(!s->wire_busy||s->wire_type||s->wire_cid!=7)return;
 if(l->pending){if(l->pending_ticket!=s->ticket)l->bad=1;return;}
 l->pending=1;l->pending_ticket=s->ticket;l->key_add=s->wire[64];l->key_pair=s->wire[66];l->key_index=s->wire[76];
 if(l->key_add>1||l->key_pair>1||l->key_index>3)l->bad=1;
}
static int nsr_key_complete(struct nsr_ledger*l,const struct nst_event*e){
 if(!l->pending||e->ticket!=l->pending_ticket||l->bad){l->bad=1;return -1;}
 if(l->key_pair)l->ptk=l->key_add;
 else if(l->key_add)l->gtk|=1u<<l->key_index;
 else l->gtk&=~(1u<<l->key_index);
 l->pending=0;l->pending_ticket=0;return 0;
}
#endif
