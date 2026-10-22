#define main unused_original_main
#include "model-test.c"
#undef main
int main(void){struct stc s={0};struct stc_arbiter a={0};prepare(&s,&a);keys(&s);assert(stc_eapol(&s,42,m3,sizeof m3,2)==1);ack(&s);assert(s.phase==STC_CONNECTED);assert(stc_eapol(&s,42,m1,sizeof m1,3)==1);assert(s.outcome==STC_FAILED&&s.phase==STC_DEAUTH);cleanup(&s);puts("PASS actual transaction hook: crypto success/retransmit then unsupported postauth M1 clean closure");return 0;}
