#ifndef NORD2_BATTERY_H
#define NORD2_BATTERY_H
#include <stdint.h>
/* Sample-only transport: SMBus command pointer+read, no data writes/control,
 * MAC, dataflash, unseal, learning, charger enable, GPIO or PMIF operations.
 * Caller serializes against controller users/VOOC gauge master and proves
 * the selected pre-VOOC default policy, correct bus/pinmux/clock ownership.
 * If a VOOC protocol driver is later added it must supply its real dynamic
 * allow_reading gate; this observer implements no fastcharge negotiation. */
struct nbg_ops {
 int (*owned)(void *);
 int (*read)(void *,unsigned bus,unsigned slave,unsigned command,unsigned bytes,uint16_t *);
 int (*delay_ms)(void *,unsigned);
};
/* current_register_identified must prove BQ28Z610/ZY0603 source identity;
 * no arbitrary device-ID constants or automatic control writes here. */
struct nbg_sample {
 uint16_t raw_soc[2],raw_voltage,raw_temperature,raw_current,raw_status[2],raw_fault;
 unsigned raw_valid; /*bits0/1SOC1/2,2voltage,3temp,4current,5status0,6fault,7status1*/
 unsigned soc_valid,voltage_valid,temperature_valid,current_valid,charger_valid;
 unsigned percent,pack_mv;int temperature_centi_c,current_ma;
 unsigned external,charging,full;int error;
};
int nbg_collect(const struct nbg_ops *,void *,unsigned enabled,unsigned current_register_identified,struct nbg_sample *);
#endif
