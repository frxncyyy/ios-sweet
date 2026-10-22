#define main previous_model_main
#include "model-test.c"
#undef main
#include "wifi_wpa_diag.h"
int main(int argc,char**argv){assert(argc==2);unsigned mode=(unsigned)(argv[1][0]-'0');assert(mode<3);struct nwpd_gate r;
 assert(!nwpd_gate_record(&r,42,0,1,1,1));assert(nwpd_gate_record(&r,42,0,1,0,SIZE_MAX)&&r.reason==1&&r.bytes==UINT64_MAX);
 assert(nwpd_gate_record(&r,42,0,1,1,0)&&r.reason==2);assert(nwpd_gate_record(&r,42,0,1,1,SIZE_MAX)&&r.reason==3);assert(!nwpd_gate_record(&r,42,0,6,0,0));
 struct stc s={0};struct stc_arbiter a={0};prepare(&s,&a);
 const uint8_t*p=mode==0?NULL:mode==1?m1:(const uint8_t*)(uintptr_t)1;
 size_t n=mode==0?1:mode==1?0:SIZE_MAX;
 assert(stc_eapol(&s,42,p,n,2)==1&&s.phase==STC_DEAUTH&&s.outcome==STC_FAILED);struct nwp zero={0};assert(!memcmp(&s.wpa,&zero,sizeof zero));cleanup(&s);
 /* Direct duplicate observation cannot consume another line or other budget. */
 nwpd_gate_observe(42,0,1,p!=0,n);printf("PASS pre-engine gate %u no input read, original closure/wipe, one diagnostic\n",mode);return 0;}
