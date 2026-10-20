/* Three CONN-side reads from selected soc3_0.c2164–2196. No native adapter. */
#include "wifi_cold_snapshot.h"
int nwc_snapshot(const struct nwc_ops *ops,void *context,unsigned optin,struct nwc_result *out)
{
 if(!out)return -1;
 struct nwc_result zero={0};*out=zero;
 if(optin!=1 || !ops || !ops->check || !ops->read){out->error=-1;return -1;}
 static const uint64_t address[3]={UINT64_C(0x18000010),UINT64_C(0x18000000),UINT64_C(0x180602cc)};
 for(unsigned i=0;i<3;i++) {
  if(ops->check(context,NWC_REQUIRED)){out->error=-2;return -1;}
  uint32_t value=0;out->attempted++;
  if(ops->read(context,address[i],&value)){out->error=-3;return -1;}
  out->raw[i]=value;out->valid_mask|=1u<<i;
 }
 out->cold=!(out->raw[0]&1) && !(out->raw[1]&0x80) && !(out->raw[2]&4);
 return out->cold?0:1;
}
