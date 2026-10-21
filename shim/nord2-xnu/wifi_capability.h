#ifndef NORD2_WIFI_CAPABILITY_H
#define NORD2_WIFI_CAPABILITY_H
#include <stdint.h>
#include <stddef.h>
#define NCP_QUERY_BYTES 64u
#define NCP_MAX_RX_BYTES 3840u
struct ncp_result {
 unsigned decoded,elements,unknown,known,consumed,trailing;
 uint16_t rxd_bytes,event_bytes,product,fw_version,fw_build;
 uint8_t eco,mac[6],phy[12],mac_cap[4],branch[4],date[16];
 unsigned mac_present,mac_valid,resources_supported;
 uint32_t mac_ip,bb_ip,top_ip,config_id,resource_version;
 /* Legacy: cmd total/unit,data total/unit. V1: PSEdata/PSEcmd,
  * PLEdata/PLEcmd, packed PLE/PSE page sizes, extra count. */
 uint32_t resources[6];
};
/* Pure normal CID8a frame builder. No DMA/readiness/interface publication. */
int ncp_query(uint8_t *,size_t,uint8_t);
/* Input must later come from an actual owned RX descriptor. Bounds derive
 * from that supplied extent; raw declared byte counts are captured without
 * asserting an undocumented equality. Output is unchanged on failure. */
int ncp_parse(const uint8_t *,size_t,uint8_t,struct ncp_result *);
#endif
