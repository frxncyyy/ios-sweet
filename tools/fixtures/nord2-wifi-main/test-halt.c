#include "wifi_main_halt.h"
#include "wifi_main_phy_exchange.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
static unsigned calls;
static int owner(void *p){(void)p;calls++;return 0;}
static int read32(void *p,uint32_t a,uint32_t *v){(void)p;(void)a;(void)v;calls++;return 0;}
static int write32(void *p,uint32_t a,uint32_t v){(void)p;(void)a;(void)v;calls++;return 0;}
static int memread(void *p,uint64_t a,void *v,uint32_t n){(void)p;(void)a;(void)v;(void)n;calls++;return 0;}
static int memwrite(void *p,uint64_t a,const void *v,uint32_t n){(void)p;(void)a;(void)v;(void)n;calls++;return 0;}
static int syncmem(void *p,uint64_t a,uint32_t n,unsigned way){(void)p;(void)a;(void)n;(void)way;calls++;return 0;}
static int delay_us(void *p,unsigned us){(void)p;(void)us;calls++;return 0;}
int main(void){
 struct nwfn_native n={0};
 struct nwt_io io={owner,read32,write32,memread,memwrite,syncmem,delay_us};
 struct nfa_board board={0};struct nfa_exchange_state exchange={0};
 struct nfa_backup_io backup={0};uint8_t tx[64]={0},rx[64]={0},dst=0;
 size_t received=0;
 n.init.initialized=1;n.init.phase=5;n.init.driver_owned=1;
 n.init.transport.phase=2;n.init.transport.preflight_complete=1;
 n.init.transport.next_sequence=1;
 n.init.transport.tx_ring=0x90020000;n.init.transport.rx_ring=0x90024000;
 n.init.transport.rx_buffers=0x90025000;n.init.transport.command=0x90035000;
 tx[36]=8;tx[37]=0xa0;tx[39]=1;
 assert(nwm_lifetime_halt(&n)==-1);
 assert(n.halt&&n.init.halt&&n.init.transport.halt&&n.init.transport.dirty);
 assert(nwt_capture(&io,&n,&n.init.transport,tx,sizeof(tx),5,rx,sizeof(rx),&received)==-1);
 assert(nwt_exchange(&io,&n,&n.init.transport,0x41)==-1);
 assert(nfa_run(&io,&n,&n.init,&board,&backup,&n,&dst,1,&exchange)==-1);
 assert(calls==0);
 puts("PASS: shared terminal firmware failure prevents PHY, finish and transport callbacks");
}
