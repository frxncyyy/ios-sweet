#ifndef PRIVATE_NORD2_MAIN_HALT_H
#define PRIVATE_NORD2_MAIN_HALT_H
#include "wifi_main_wfdma_native.h"
/* A firmware or calibration fault terminates the shared fresh radio owner.
 * WFDMA callbacks and future commands must see the same sticky failure. */
static inline int nwm_lifetime_halt(struct nwfn_native *n){
 n->halt=1;
 n->init.halt=1;
 n->init.transport.halt=1;
 n->init.transport.dirty=1;
 return -1;
}
#endif
