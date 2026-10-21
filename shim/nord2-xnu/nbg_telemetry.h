#ifndef NBG_TELEMETRY_H
#define NBG_TELEMETRY_H
/* Separate bounded software-only log lines; all captured fields are32bit. */
#define NBG_I2C_LOG "NORD2 BAT I2C: bus=%u rc=%d stage=%u why=%u seen=%#x off=%u flight=%u last=%#x value=%#x valid=%u wr=%u width=%u\n"
#define NBG_CLOCK_LOG "NORD2 BAT CLK: bus=%u mux=%#x parent_hz=%u pins=%#x gate=%#x irq=%#x polls=%u seen=%#x\n"
#define NBG_RAW_LOG "NORD2 BAT RAW: rc=%d seen=%#x soc=%#x/%#x volt=%#x temp=%#x cur=%#x chg=%#x/%#x fault=%#x soc_valid=%u charger_valid=%u\n"
#endif
