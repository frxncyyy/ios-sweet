/* Host-only USB model uses the actual kernel mailbox C implementation. */
#include "wifi_live_control.h"
static unsigned executed,disconnected,ready_polls;
static uint32_t generation=42,capture,windows=32,drop;
int mock_initialize(void){return nwl_initialize(UINT64_C(0x12345678));}
int mock_write(unsigned value,unsigned index){
 uint8_t setup[8]={0x40,0x61,(uint8_t)value,(uint8_t)(value>>8),(uint8_t)index,(uint8_t)(index>>8),0,0},out[256];uint32_t n;
 return nwl_usb(setup,out,sizeof out,&n)==2?0:-9;
}
int mock_read(unsigned code,uint8_t*out,unsigned size){
 if(code==0x62){struct nwl_request q;if(nwl_take(&q)){
  struct nwl_snapshot s={{0}};unsigned result=NWL_OK;
  if(q.op!=NWL_STATUS&&q.generation!=generation)result=NWL_STALE;
  else if(q.op>=NWL_CAPTURE_DHCP&&q.op<=NWL_CAPTURE_STOP){capture=q.op==NWL_CAPTURE_STOP?0:q.op-1;if(capture&&windows)windows--;}
  else if(q.op==NWL_OFLD_ALLOW)drop=0;
  else if(q.op==NWL_OFLD_DROP)drop=1;
  if(q.op==NWL_DISCONNECT&&result==NWL_OK){disconnected=1;ready_polls=0;}
  if(q.op==NWL_STATUS&&disconnected&&++ready_polls>=2)disconnected=0;
  s.word[NWL_F_JOIN_READY]=!disconnected;
  s.word[NWL_F_GENERATION_LO]=generation;s.word[NWL_F_CAPTURE]=capture;s.word[NWL_F_WINDOWS_LEFT]=windows;s.word[NWL_F_OFLD_DROP]=drop;
  if(nwl_complete(&q,result,&s))return -1;executed++;
 }}
 uint8_t setup[8]={0xc0,(uint8_t)code,0,0,0,0,(uint8_t)size,(uint8_t)(size>>8)};uint32_t n=0;
 return nwl_usb(setup,out,size,&n)==1?(int)n:-9;
}
unsigned mock_executed(void){return executed;}

unsigned mock_ready_polls(void){return ready_polls;}
