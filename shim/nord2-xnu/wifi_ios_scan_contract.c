#include "wifi_ios_scan_contract.h"
static uint16_t g16(const uint8_t*p){return p[0]|(uint16_t)p[1]<<8;}
static uint32_t g32(const uint8_t*p){return g16(p)|(uint32_t)g16(p+2)<<16;}
static void p16(uint8_t*p,uint16_t x){p[0]=(uint8_t)x;p[1]=(uint8_t)(x>>8);}
static void p32(uint8_t*p,uint32_t x){p16(p,(uint16_t)x);p16(p+2,(uint16_t)(x>>16));}
static void copy(void*d,const void*s,size_t n){uint8_t*a=d;const uint8_t*b=s;while(n--)*a++=*b++;}
static int ies(const uint8_t*p,size_t n){size_t i=0;while(i<n){if(n-i<2||p[i+1]>n-i-2)return -1;i+=2+p[i+1];}return 0;}
static int valid(const struct niws_result*r){return r&&r->rate_count<=NIWS_MAX_RATES&&r->ssid_bytes<=32&&r->ie_bytes<=NIWS_MAX_IE&&!ies(r->ie,r->ie_bytes);}
int niws_decode_result(const uint8_t*p,size_t n,struct niws_result*out){
 struct niws_result r={0};if(!p||!out||n!=NIWS_SCAN_RESULT_BYTES||g32(p)!=1)return -1;
 r.channel=g32(p+8);r.channel_flags=g32(p+12);r.noise_valid=g16(p+16);
 r.noise=(int16_t)g16(p+18);r.snr=(int16_t)g16(p+20);r.rssi=(int16_t)g16(p+22);
 r.beacon_interval=g16(p+24);r.capabilities=g16(p+26);copy(r.bssid,p+28,6);r.rate_count=p[34];r.ssid_bytes=p[96];
 if(r.rate_count>NIWS_MAX_RATES||r.ssid_bytes>32)return -1;
 for(unsigned i=0;i<r.rate_count;i++)r.rates[i]=g32(p+36+4*i);
 copy(r.ssid,p+97,r.ssid_bytes);r.age=g32(p+132);r.flags=p[136];r.ie_bytes=g16(p+138);
 if(r.ie_bytes>NIWS_MAX_IE)return -1;
 copy(r.ie,p+140,r.ie_bytes);
 if(!valid(&r))return -1;
 *out=r;return 0;
}
int niws_encode_result(const struct niws_result*r,uint8_t*p,size_t n){
 if(!p||n!=NIWS_SCAN_RESULT_BYTES||!valid(r))return -1;
 /* Caller must provide disjoint output; pure helper receives no backend. */
 for(size_t i=0;i<n;i++)p[i]=0;
 p32(p,1);p32(p+8,r->channel);p32(p+12,r->channel_flags);p16(p+16,r->noise_valid);p16(p+18,(uint16_t)r->noise);p16(p+20,(uint16_t)r->snr);p16(p+22,(uint16_t)r->rssi);p16(p+24,r->beacon_interval);p16(p+26,r->capabilities);copy(p+28,r->bssid,6);p[34]=r->rate_count;
 for(unsigned i=0;i<r->rate_count;i++)p32(p+36+4*i,r->rates[i]);
 p[96]=r->ssid_bytes;copy(p+97,r->ssid,r->ssid_bytes);p32(p+132,r->age);p[136]=r->flags;p16(p+138,r->ie_bytes);copy(p+140,r->ie,r->ie_bytes);return 0;
}
int niws_scan_request_header(const uint8_t*p,size_t n,uint32_t*out){if(!p||!out||n!=NIWS_SCAN_REQUEST_BYTES||g32(p)!=1)return -1;
 *out=g32(p+4);return 0;}
int niws_decode_scan_done(const uint8_t*p,size_t n,const char name[16],struct niws_scan_done*out){
 struct niws_scan_done r;if(!p||!name||!out||n!=44||g32(p)!=44||g32(p+20)!=10)return -1;
 /* Exact interface comparison, no unbounded string read. Header vendor/class/
  * subclass are captured below only after caller stream validation elsewhere. */
 for(unsigned i=0;i<16;i++)if(p[24+i]!=(uint8_t)name[i])return -1;
 r.id=g32(p+16);r.status=g32(p+40);*out=r;return 0;
}

int niws_decode_pipe_scan_done(const uint8_t*p,size_t n,const char name[16],struct niws_scan_done*out){
 struct niws_scan_done r={0};
 if(!p||!name||!out||n!=52||g32(p)!=87||g16(p+24)!=4||g16(p+26)||g32(p+28)!=10)return -1;
 for(unsigned i=0;i<16;i++)if(p[32+i]!=(uint8_t)name[i])return -1;
 /* Pipe header does not provide the kernel-event monotonic ID interpreted
  * by our other decoder; zero here is metadata absence, not a real event ID. */
 r.status=g32(p+48);*out=r;return 0;
}

int niws_scan_envelope(unsigned selector,const uint8_t*p,size_t n,const char name[16],uint32_t*out){
 size_t cap;if(!p||!name||!out)return -1;
 if(selector==0&&n==15408)cap=15360;
 else if(selector==6&&n==245808)cap=245760;
 else return -1;
 if(g32(p)!=UINT32_C(0x802869c8)||g32(p+4)!=NIWS_SCAN_REQUEST_BYTES||g32(p+8+cap+16)!=10)return -1;
 for(unsigned i=0;i<16;i++)if(p[8+cap+i]!=(uint8_t)name[i])return -1;
 return niws_scan_request_header(p+8,NIWS_SCAN_REQUEST_BYTES,out);
}
