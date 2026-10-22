#ifndef NORD2_WIFI_IFNET_LINK_H
#define NORD2_WIFI_IFNET_LINK_H
#include <stddef.h>
#include <stdint.h>
#define NWIL_ETHERNET_MAX 1514u
enum nwil_send_result {
 NWIL_SEND_ACCEPTED=0, NWIL_SEND_NOT_READY=1, NWIL_SEND_BUSY=2,
 NWIL_SEND_INVALID=-1
};
#ifdef __cplusplus
extern "C" {
#endif
/* Native owner copies a complete Ethernet-II frame into bounded storage.
 * It must revalidate generation + controlled-port admission atomically with
 * acceptance and again before publication. No caller buffer is retained. */
int nord2_wifi_link_send(const uint8_t *,size_t,uint64_t generation);
/* Kernel owner callbacks only. Link-up follows genuine station/key/data-ready
 * completion; link-down is synchronous before revocation notifications.
 * Return zero or a positive BSD errno. No PMK/key bytes cross this boundary. */
int nord2_wifi_ifnet_link(uint64_t generation,int authorized);
int nord2_wifi_ifnet_receive(uint64_t generation,const uint8_t *,size_t);
#ifdef __cplusplus
}
#endif
#endif
