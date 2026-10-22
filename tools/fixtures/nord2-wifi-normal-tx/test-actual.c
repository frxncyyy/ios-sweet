#define SUBMISSION_MAIN submission_baseline
#include "../nord2-wifi-station/test-security-submission.c"
static void token_report(struct test*h,unsigned token){uint8_t p[12]={0};put16(p,12);put16(p+2,(6u<<11)|1);put32(p+4,2u<<16);put32(p+8,token);deliver(h,3,p,sizeof p);}
static void page_present(struct test*h){C(!memcmp(at(h,h->s.normal.buffer,4096),h->s.normal.page,4096));C(h->s.normal.page[33]&128);C(h->s.wire_busy&&!h->s.dma_done);}
int main(void){submission_baseline();struct test*h=calloc(1,sizeof *h);assert(h);uint8_t eth[60]={0};memset(eth,255,6);eth[6]=2;eth[11]=1;eth[12]=8;
 /* Header DDONE alone never frees the separate TXP payload; foreign, malformed,
  * duplicate, host TXS, cancellation and key submissions cannot bypass this. */
 setup(h);h->s.controlled_port=1;h->defer_token=1;C(!nst_data(&h->s,1,eth,sizeof eth));step(h);C(h->s.normal.dma_done&&!h->s.normal.report_done&&h->s.dma_completed==1&&!h->events[NST_DATA_SUBMITTED]);page_present(h);unsigned tok=h->s.normal.token;
 token_report(h,(tok+1)%2048);step(h);page_present(h);uint8_t bad[8]={0};put16(bad,8);put16(bad+2,(6u<<11)|1);put32(bad+4,2u<<16);deliver(h,3,bad,sizeof bad);step(h);page_present(h);txdone(h,127,5,0);step(h);page_present(h);nst_close_port(&h->s);C(!h->s.controlled_port&&h->s.wire_busy);uint8_t key[64]={0};C(nst_command(&h->s,2,7,key,sizeof key)==1);C(!nstx_idle(&h->s.normal));token_report(h,tok);step(h);C(h->events[NST_DATA_SUBMITTED]==1&&!h->s.wire_busy&&nstx_idle(&h->s.normal));for(unsigned i=0;i<4096;i++)C(!at(h,h->s.normal.buffer,4096)[i]);token_report(h,tok);step(h);C(h->events[NST_DATA_SUBMITTED]==1&&h->s.normal.completed==1);cleanup(h);
 /* Token may arrive first, but descriptor still owns its header. */
 setup(h);h->s.controlled_port=1;h->defer_dma=1;C(!nst_data(&h->s,1,eth,sizeof eth));step(h);C(h->s.normal.report_done&&!h->s.normal.dma_done&&!h->events[NST_DATA_SUBMITTED]);page_present(h);dma_retire(h);step(h);C(nstx_idle(&h->s.normal)&&h->events[NST_DATA_SUBMITTED]==1);cleanup(h);
 /* Missing token is uncertain payload ownership even after header DDONE. */
 setup(h);h->s.controlled_port=1;h->defer_token=1;C(!nst_data(&h->s,1,eth,sizeof eth));step(h);page_present(h);h->now=h->s.operation_deadline;C(nst_step(&h->s,&io,h,16,callback,h)<0);C(h->s.halt&&h->s.error==3&&h->s.normal.published&&h->s.normal.dma_done&&!h->s.normal.report_done&&!h->events[NST_DATA_SUBMITTED]);C(!memcmp(at(h,h->s.normal.buffer,4096),h->s.normal.page,4096));cleanup(h);
 /* Exercise both ring1024 and source token2048 wrap, keeping management domain
  * independent. Source token reports have no generation tag: reuse is not a
  * proof against a delayed duplicate beyond this completed lifetime. */
 setup(h);h->s.controlled_port=1;unsigned management_pid=h->s.next_pid;for(unsigned k=0;k<2200;k++){C(!nst_data(&h->s,UINT64_MAX,eth,sizeof eth));step(h);C(h->s.normal.next==(k+1)%1024&&h->s.normal.token_next==(k+1)%2048&&h->s.normal.completed==k+1&&h->s.normal.dma_completed==k+1&&h->s.normal.submitted==k+1);C(!h->s.halt&&!h->s.wire_busy&&!h->s.air_pending&&h->s.next_pid==management_pid&&!h->events[NST_FRAME_RESULT]);C(h->s.normal.credit_acquired==k+1&&h->s.normal.credit_released==k+1&&h->s.normal.credit_available==1&&!h->s.normal.credit_reserved);C(h->prior.completed==1&&h->init.transport.completed==6);}
 C(h->events[NST_DATA_SUBMITTED]==2200);cleanup(h);free(h);printf("PASS %u normal HW0/TXP actual-source lifetime assertions (synthetic DMA/firmware)\n",checks);return 0;}
