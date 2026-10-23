#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "audio_hw.h"
static struct nau_hw hardware;
static struct {unsigned amp_report_seq;} device;
static unsigned calls,failed,printed,reads_after_failed;
static const uint8_t registers[NAH_O_FIELDS]={0,0x10,0x11,0x13,0x14,0x58,0x20,0x23,0x26};
static const uint16_t values[NAH_O_FIELDS]={0x18,0x16,0xe080,0x800f,0x48,0x1614,0x2890,3,1};
static unsigned noclk;
static int amp_read(struct nau_hw*h,unsigned amp,unsigned reg,uint16_t*v){(void)h;assert(!failed);unsigned at=calls++;assert(amp==at/NAH_O_FIELDS&&reg==registers[at%NAH_O_FIELDS]);if(calls==failed){reads_after_failed++;return NAH_E_IO;}*v=values[at%NAH_O_FIELDS];if(noclk&&reg==0x10)*v|=0x80;return 0;}
static unsigned fail_at;
static int tested_amp_read(struct nau_hw*h,unsigned amp,unsigned reg,uint16_t*v){if(fail_at&&calls+1==fail_at){calls++;failed=1;return NAH_E_IO;}return amp_read(h,amp,reg,v);}
#define amp_read tested_amp_read
#include "observed-capture.inc"
#undef amp_read
static int capture_printf(const char*fmt,...){assert(strstr(fmt,"NORD2 AUDIO AMP START:"));va_list ap;va_start(ap,fmt);assert(va_arg(ap,unsigned)==7);unsigned a=va_arg(ap,unsigned);assert(a==printed);assert(va_arg(ap,unsigned)==hardware.status.amp_observed_valid[a]);for(unsigned i=0;i<NAH_O_FIELDS;i++)assert(va_arg(ap,unsigned)==hardware.status.amp_observed[a][i]);va_end(ap);printed++;return 0;}
#define printf capture_printf
#include "observed-report.inc"
#undef printf
int main(void){
 hardware.amps[0].rev=hardware.amps[1].rev=0x0c74;
 assert(!amp_observe_startup(&hardware));assert(calls==2*NAH_O_FIELDS);
 for(unsigned a=0;a<2;a++){assert(hardware.status.amp_observed_valid[a]==((1u<<NAH_O_FIELDS)-1));for(unsigned i=0;i<NAH_O_FIELDS;i++)assert(hardware.status.amp_observed[a][i]==values[i]);}
 hardware.status.amp_observed_seq=7;hardware.status.running=1;report_amp_observed();assert(!printed&&!device.amp_report_seq);
 hardware.status.running=0;report_amp_observed();assert(printed==2&&device.amp_report_seq==7&&calls==2*NAH_O_FIELDS);report_amp_observed();assert(printed==2&&calls==2*NAH_O_FIELDS);
 for(unsigned n=1;n<=2*NAH_O_FIELDS;n++){memset(&hardware.status,0,sizeof(hardware.status));calls=failed=0;fail_at=n;assert(amp_observe_startup(&hardware)==NAH_E_IO);assert(calls==n&&failed);for(unsigned a=0;a<2;a++){unsigned done=n-1>a*NAH_O_FIELDS?n-1-a*NAH_O_FIELDS:0;if(done>NAH_O_FIELDS)done=NAH_O_FIELDS;assert(hardware.status.amp_observed_valid[a]==((1u<<done)-1));}}
 memset(&hardware.status,0,sizeof(hardware.status));calls=failed=fail_at=0;noclk=1;assert(amp_observe_startup(&hardware)==NAH_E_AMP_FAULT);assert(calls==2&&hardware.status.amp_observed_valid[0]==3&&hardware.status.amp_observed[0][NAH_O_STATUS10]==0x96);
 assert(!reads_after_failed);puts("PASS18fixed startupreads, all18failurepositions stop immediately/partialvalid, NOCLKsaved+not swallowed; report deferreduntilstop/once/noMMIO");}
