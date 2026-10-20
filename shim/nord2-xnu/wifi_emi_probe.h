#ifndef NORD2_WIFI_EMI_PROBE_H
#define NORD2_WIFI_EMI_PROBE_H
/* Standalone read-only MPU observation. Explicit nord2wifiemi=1 only.
 * Call start once from a sleepable boot context outside driver/input/console
 * locks (suggested after root setup unlock, beside firmware_start). It spawns
 * its own thread; never call the probe from input polling/interrupt context.
 * Only eight structured READ queries (regions27/31), no retry, no MMIO/EMI
 * mapping or power changes. A secure-monitor call has NO interruptible software
 * deadline: bounded query count does not guarantee bounded execution time.
 * Decoded values do not establish effective permission, priority or ownership.
 * Starts/attempts are once-only even on failure; no blobs/data retained.
 */
int nord2_wifi_emi_probe(void);
int nord2_wifi_emi_start(void);
#endif
