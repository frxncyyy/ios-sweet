#ifndef NORD2_WIFI_EMI_PROBE_H
#define NORD2_WIFI_EMI_PROBE_H
/* Standalone read-only MPU observation. Original mode: nord2wifiemi=1.
 * Legacy mode: explicit nord2wifiemilegacy=1 AND nord2wifiemi=0; otherwise
 * no legacy calls. Both modes share the same once-only attempt/thread guards.
 * Call start once from a sleepable boot context outside driver/input/console
 * locks (suggested after root setup unlock, beside firmware_start). It spawns
 * its own thread; never call the probe from input polling/interrupt context.
 * Original mode issues eight structured queries (regions27/31). Legacy mode
 * issues at most128 allowlisted SA/EA/APC offset reads and stops on first error.
 * Neither retries, maps MMIO/EMI or changes power. A secure-monitor call has NO interruptible software
 * deadline: bounded query count does not guarantee bounded execution time.
 * Decoded values do not establish effective permission, priority or ownership.
 * Starts/attempts are once-only even on failure; no firmware blobs are loaded.
 * Legacy raw results remain in its fixed static buffer for the current boot.
 */
int nord2_wifi_emi_probe(void);
int nord2_wifi_emi_start(void);
#endif
