/* Actual native known-DDONE DEAUTH fallback; security/data have local receipts. */
#define SUBMISSION_MAIN security_submission_unused_main
#include "test-security-submission.c"
#undef SUBMISSION_MAIN
int main(void){struct test*h=calloc(1,sizeof *h);assert(h);struct composed x;struct stc_arbiter a={0};
for(unsigned late=0;late<3;late++){memset(&a,0,sizeof a);init_join(h,&x,&a);C(nwn_cancel(&x.n,17,h->now/1000)>=0);h->defer_air=1;unsigned air=h->events[NST_FRAME_RESULT];advance(&x);C(!h->s.air_pending&&h->s.dma_done&&h->events[NST_FRAME_RESULT]==air+1&&x.n.owner.transaction.phase==STC_RETIRE);unsigned pid=h->s.wire_pid;if(late)txdone(h,pid,h->s.peer.wlan,late==2?7:0);advance(&x);C(!h->s.halt&&!h->s.air_pending&&h->events[NST_FRAME_RESULT]==air+1&&!h->s.controlled_port);phase(&x,STC_DONE);C(!h->s.sta_active&&!h->init.transport.dirty&&x.n.owner.transaction.outcome==STC_CANCELED);cleanup(h);}
/* Known-DONE cleanup never forgives unknown descriptor storage. */
memset(&a,0,sizeof a);init_join(h,&x,&a);C(nwn_cancel(&x.n,17,h->now/1000)>=0);h->defer_air=h->defer_dma=1;advance(&x);C(h->s.wire_busy&&!h->s.dma_done);h->now=h->s.operation_deadline;C(nst_step(&h->s,&io,h,16,native_event,&x)<0);C(h->s.error==3&&h->s.halt&&h->init.transport.dirty);cleanup(h);
/* Initial auth still requires real TXS, even after its own DMA has retired. */
setup(h);struct sw_peer p;memcpy(&p,&h->s.peer,12);uint8_t auth[30];size_t n;C(!sw_auth(auth,sizeof auth,&n,&p));h->defer_air=1;C(!nst_management(&h->s,1,auth,n));step(h);h->now=h->s.operation_deadline;C(nst_step(&h->s,&io,h,16,callback,h)<0);C(h->s.halt&&h->s.error==3);cleanup(h);
/* Cleanup/global deadline and rollback override even an actual queued TXS. */
for(unsigned rollback=0;rollback<2;rollback++){memset(&a,0,sizeof a);init_join(h,&x,&a);C(nwn_cancel(&x.n,17,h->now/1000)>=0);h->defer_air=1;advance(&x);txdone(h,h->s.wire_pid,h->s.peer.wlan,0);unsigned air=h->events[NST_FRAME_RESULT];h->now=rollback?h->s.last-1:h->s.deadline;C(nst_step(&h->s,&io,h,16,native_event,&x)<0);C(h->s.halt&&h->s.error==(rollback?1u:2u)&&h->events[NST_FRAME_RESULT]==air);cleanup(h);}
struct stc bad={0};bad.attempted=1;bad.generation=17;bad.phase=STC_REMOVE_PTK;bad.action.kind=STC_DELETE_PTK;bad.action.ticket=999;bad.action.generation=17;bad.last_ms=1;bad.cleanup_deadline=30001;struct stc_arbiter arb={.mode=2,.generation=17};bad.arbiter=&arb;C(stc_receipt(&bad,17,999,0,1)<0&&bad.phase==STC_HALTED);
C(NST_TX_LOCAL_TIMEOUT==256);free(h);printf("PASS %u assertions: actual management AIR_TX, knownDDONE negative DEAUTH and final bounded realTXS drain, unknownDMA/keydelete/globalclock remainfatal; synthetic only\n",checks);return 0;}
