#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "wifi_station_data_diag.h"
static unsigned checks;
#define C(x) do{checks++;assert(x);}while(0)
static unsigned reserve(uint64_t g,unsigned kind,unsigned protect){uint8_t f[60]={0};f[12]=8;return nsdd_pid_reserve(g,5,kind,protect,f,sizeof f);}
static struct nsdd_txs status(unsigned pid,unsigned wlan,unsigned raw){return (struct nsdd_txs){.pid=pid,.wlan=wlan,.status=raw,.bytes=88};}
int main(void){
 nsdd_test_boot_reset();uint8_t f[60]={0};f[12]=8;unsigned id=0;
 C(!nsdd_pid_reserve(0,5,3,1,f,sizeof f));C(!nsdd_pid_reserve(1,255,3,1,f,sizeof f));C(!nsdd_pid_reserve(1,5,0,1,f,sizeof f));C(!nsdd_pid_reserve(1,5,3,2,f,sizeof f));C(!nsdd_pid_reserve(1,5,3,1,NULL,60));C(!nsdd_pid_reserve(1,5,3,1,(void*)1,SIZE_MAX));
 for(unsigned n=0;n<14;n++)C(!nsdd_pid_reserve(1,5,3,1,f,n));
 id=nsdd_pid_reserve(1,5,3,1,f,sizeof f);C(id==64);struct nsdd_pid_record original=*nsdd_test_pid(id);memset(f,0xa5,sizeof f);C(!memcmp(&original,nsdd_test_pid(id),sizeof original));
 struct nsdd_txs x=status(id,5,0);nsdd_pid_status(1,&x);C(!nsdd_test_pid(id)->seen);nsdd_pid_attempt(1,id,5);nsdd_pid_status(1,&x);C(!nsdd_test_pid(id)->seen);nsdd_pid_publish(1,id,5,7);nsdd_pid_status(1,&x);C(nsdd_test_pid(id)->seen&&!nsdd_test_pid(id)->local);x.status=3;nsdd_pid_status(1,&x);C(!nsdd_test_pid(id)->status);nsdd_pid_local(1,id,5);C(nsdd_test_pid(id)->local);
 nsdd_pid_finish(1);original=*nsdd_test_pid(id);nsdd_pid_finish(1);nsdd_pid_status(1,&x);nsdd_pid_status(2,&x);C(!memcmp(&original,nsdd_test_pid(id),sizeof original));
 unsigned canceled=reserve(2,2,0);C(canceled==32);nsdd_pid_finish(2);C(!nsdd_test_pid(canceled)->published&&!nsdd_test_pid(canceled)->attempted);unsigned next=reserve(3,2,1);C(next==33);C(nsdd_test_pid(canceled)->generation==2);nsdd_pid_attempt(3,next,5);nsdd_pid_publish(3,next,5,8);nsdd_pid_local(3,next,5);x=status(next,6,0);nsdd_pid_status(3,&x);C(!nsdd_test_pid(next)->seen);x.wlan=5;nsdd_pid_status(2,&x);C(!nsdd_test_pid(next)->seen);nsdd_pid_status(3,&x);C(nsdd_test_pid(next)->local&&nsdd_test_pid(next)->seen);
 x.status=256;unsigned z=reserve(4,3,1);nsdd_pid_attempt(4,z,5);nsdd_pid_publish(4,z,5,9);x.pid=z;nsdd_pid_status(4,&x);C(!nsdd_test_pid(z)->seen);x.status=0;x.pid=127;nsdd_pid_status(4,&x);C(!nsdd_test_pid(z)->seen);x.pid=0;nsdd_pid_status(4,&x);x.pid=126;nsdd_pid_status(4,&x);C(!nsdd_test_pid(z)->seen);
 unsigned all[127]={0};all[32]=all[33]=all[64]=all[65]=1;
 for(unsigned kind=1;kind<=3;kind++){unsigned count=kind==1?31:kind==2?30:61;for(unsigned k=0;k<count;k++){unsigned p=reserve(10+kind,kind,kind==3);C(p>0&&p<127&&!all[p]);all[p]=1;C(nsdd_test_pid(p)->kind==kind);}}
 C(!reserve(20,1,0));for(unsigned k=0;k<1000;k++){C(reserve(20+k,2,0)==127);C(reserve(20+k,3,1)==127);}for(unsigned k=1;k<=126;k++)C(all[k]);
 /* Session-local diagnostic resets cannot reset the boot PID ledger. */
 struct nsdd session={0};session=(struct nsdd){0};(void)session;C(reserve(9000,2,0)==127);C(!reserve(9001,1,0));
 for(unsigned k=1;k<=126;k++)nsdd_pid_finish(nsdd_test_pid(k)->generation);
 puts("PASS boot unique ranges, cancellation/generation persistence, immutable metadata, prepublish/wrongWLAN/stale/duplicate/short-domain isolation and TXS/local order");printf("PASS %u checks\n",checks);return 0;
}
