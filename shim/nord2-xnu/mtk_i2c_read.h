#ifndef NBG_MTK_I2C_READ_H
#define NBG_MTK_I2C_READ_H
#include <stdint.h>
struct nbg_i2c_io {
 int (*read)(void *,uint32_t,unsigned,uint32_t *);
 int (*write)(void *,uint32_t,unsigned,uint32_t);
 int (*delay_us)(void *,unsigned);
};
/* Telemetry is captured from existing operations only. failure codes:
 * 1mux,2PLL,3timing,4pinmux,5gate,6busy,7DMA,8priorIRQ,9FIFO,
 * 10read,11write,12delay,13timeout,14completionIRQ,15bytebounds.
 * observed bits:1mux,2parentHz,4pinmux,8initialgate,16completionIRQ. */
struct nbg_i2c_state {unsigned entered,inflight,offline,polls;int error;
 unsigned stage,failure,observed,last_width,last_write,last_valid;
 uint32_t last_addr,last_value,mux,parent_hz,pinmux,initial_gate,completion_irq;
};
/* Exact board buses/slaves/RO commands. Parent serializes complete sample.
 * No DMA/reset/pinmux/PLL change. Unknown completion ->offline, no cleanup.
 * Supports26MHz or source-derived stable MAINPLL/UNIVPLL rate; noPLL retune. */
int nbg_i2c_read(const struct nbg_i2c_io *,void *,struct nbg_i2c_state *,unsigned,unsigned,unsigned,unsigned,uint16_t *);
#endif
