#ifndef NORD2_WIFI_ASYNC_EVENT_H
#define NORD2_WIFI_ASYNC_EVENT_H
#include <stdint.h>
#include <stddef.h>
/* Selected EID7 is unsolicited EVENT_SLEEPY_INFO: uint8 state + reserved3.
 * Capture the raw hint only. Nonzero is not an ownership handoff or FW ACK.
 * Outer discriminator/actual copied extent govern access; inner type,
 * sequence, declared lengths and reserved bytes are captured/opaque.
 * Caller retains the owned copied frame and independently bounds total IO. */
static inline int nwe_sleepy(const uint8_t*p,size_t n,uint8_t*state){
 if(!p||!state||n<36)return -1;
 uint16_t outer=(uint16_t)p[2]|(uint16_t)p[3]<<8;
 if((outer&0x380fu)!=0x3800u||p[28]!=7)return 0;
 if(n<40)return -1;
 *state=p[36];return 1;
}
#endif
