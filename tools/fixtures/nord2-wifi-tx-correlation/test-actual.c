#define SUBMISSION_MAIN existing_submission_main
#include "test-security-submission.c"
#undef SUBMISSION_MAIN
#include "packets.h"
static void token(struct test*h){uint8_t p[12]={0};put16(p,12);put16(p+2,(6u<<11)|1);put32(p+4,2u<<16);put32(p+8,h->s.normal.token|((unsigned)h->s.peer.wlan<<15)|(1u<<25));deliver(h,3,p,sizeof p);}
int main(void){struct test*h=calloc(1,sizeof *h);assert(h);struct composed x;struct stc_arbiter a={0};init_join(h,&x,&a);m2_retired(&x);
 C(h->s.wire_pid==NSDD_PID_SECURITY_MIN);C((get32(h->capture+20)&127)==h->s.wire_pid);C(nsdd_test_pid(32)->published&&nsdd_test_pid(32)->local&&!nsdd_test_pid(32)->seen&&nsdd_test_pid(32)->eapol_class==1&&!nsdd_test_pid(32)->protected_frame);
 eapol_rx(&x,m3,sizeof m3);advance(&x);finish_keys(&x);C(nsdd_test_pid(33)->published&&nsdd_test_pid(33)->local&&!nsdd_test_pid(33)->seen&&nsdd_test_pid(33)->eapol_class==2&&!nsdd_test_pid(33)->protected_frame);
 uint8_t eth[1518];size_t n=dhcp_packet(eth,1,0,2);memcpy(eth+6,h->s.peer.self,6);unsigned air=h->events[NST_FRAME_RESULT],data=h->events[NST_DATA_SUBMITTED];
 /* No ID is consumed by wholly invalid wire or mismatched source. */
 C(nst_data(&h->s,UINT64_MAX,eth,13)<0);eth[6]^=2;C(nst_data(&h->s,UINT64_MAX,eth,n)<0);eth[6]^=2;
 h->defer_dma=h->defer_token=1;C(!nst_data(&h->s,UINT64_MAX,eth,n));unsigned pid=h->s.wire_pid;C(pid==64&&!nsdd_test_pid(pid)->published);advance(&x);
 C((get32(h->capture+20)&127)==pid&&nsdd_test_pid(pid)->published&&!nsdd_test_pid(pid)->local&&nsdd_test_pid(pid)->dhcp==1&&nsdd_test_pid(pid)->protected_frame);
 txdone(h,pid,h->s.peer.wlan^1,0);advance(&x);C(!nsdd_test_pid(pid)->seen&&h->s.wire_busy);
 txdone(h,pid,h->s.peer.wlan,3);advance(&x);C(nsdd_test_pid(pid)->seen&&nsdd_test_pid(pid)->status==3&&!nsdd_test_pid(pid)->local&&h->s.wire_busy&&!h->s.dma_done);
 txdone(h,pid,h->s.peer.wlan,0);advance(&x);C(nsdd_test_pid(pid)->status==3&&h->s.wire_busy&&h->events[NST_DATA_SUBMITTED]==data&&h->events[NST_FRAME_RESULT]==air);
 dma_retire(h);advance(&x);C(h->s.normal.dma_done&&h->s.wire_busy&&!nsdd_test_pid(pid)->local);token(h);advance(&x);
 C(!h->s.wire_busy&&nsdd_test_pid(pid)->local&&h->events[NST_DATA_SUBMITTED]==data+1&&h->s.controlled_port&&nwo_authorized(&x.n.owner));
 /* Genuine local receipt precedes TXS: no air-dependent lifetime. */
 h->defer_dma=h->defer_token=0;C(!nst_data(&h->s,UINT64_MAX,eth,n));unsigned after=h->s.wire_pid;advance(&x);C(after==65&&nsdd_test_pid(after)->local&&!nsdd_test_pid(after)->seen);
 txdone(h,after,h->s.peer.wlan,0);advance(&x);C(nsdd_test_pid(after)->seen&&!nsdd_test_pid(after)->status&&h->events[NST_DATA_SUBMITTED]==data+2&&h->events[NST_FRAME_RESULT]==air);
 /* Reserve then close before publication: burned identity with no false TXS. */
 C(!nst_data(&h->s,UINT64_MAX,eth,n));unsigned canceled=h->s.wire_pid;C(canceled==66&&!nsdd_test_pid(canceled)->published);nst_close_port(&h->s);txdone(h,canceled,h->s.peer.wlan,0);advance(&x);C(!nsdd_test_pid(canceled)->seen&&!nsdd_test_pid(canceled)->attempted&&!h->s.wire_busy);
 cancel_done(&x);C(nsdd_test_pid(canceled)->closed&&!nsdd_test_pid(canceled)->published);C(nsdd_test_pid(pid)->closed&&nsdd_test_pid(pid)->local&&nsdd_test_pid(pid)->status==3);
 cleanup(h);preserve_diag_boot=1;setup(h);h->s.controlled_port=1;C(!nst_data(&h->s,1,eth,n));C(h->s.wire_pid==67);C(nsdd_test_pid(pid)->closed);cleanup(h);free(h);
 printf("PASS %u actual owner/crypto/TXP/token checks: unique final wire PID, DHCP/M2/M4 categories, pre/post local TXS, cancellation and boot persistence\n",checks);return 0;}
