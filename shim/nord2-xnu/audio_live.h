#ifndef NORD2_AUDIO_LIVE_H
#define NORD2_AUDIO_LIVE_H
#include <stddef.h>
#include <stdint.h>
#define NAL_MAGIC UINT32_C(0x4c41444e)
#define NAL_VERSION 1u
#define NAL_CAP_WORDS 16u
#define NAL_WORDS 48u
#define NAL_CAP_BYTES (NAL_CAP_WORDS*4u)
#define NAL_BYTES (NAL_WORDS*4u)
#define NAL_MAX_TOKEN 4095u
#define NAL_USB_CAP 0x64u
#define NAL_USB_COMMAND 0x65u
#define NAL_USB_REPLY 0x66u
enum nal_op {NAL_STATUS=1,NAL_ENABLE,NAL_STOP,NAL_TONE,NAL_SILENT_TEST};
#define NAL_COMMANDS UINT32_C(0x3e)
enum nal_result {NAL_OK,NAL_UNAVAILABLE,NAL_STALE,NAL_REFUSED,NAL_LIMIT};
enum nal_field {
 NAL_F_MAGIC,NAL_F_VERSION,NAL_F_BYTES,NAL_F_BOOT_LO,NAL_F_BOOT_HI,
 NAL_F_TOKEN,NAL_F_OP,NAL_F_RESERVED,NAL_F_RESULT,
 NAL_F_ENABLED,NAL_F_INITIALIZED,NAL_F_RUNNING,NAL_F_SILENT,NAL_F_UNCERTAIN,
 NAL_F_ERROR,NAL_F_STOP_ERROR,NAL_F_GATE,NAL_F_AMP0,NAL_F_AMP1,
 NAL_F_STATUS0,NAL_F_STATUS1,NAL_F_WRITTEN_LO,NAL_F_WRITTEN_HI,
 NAL_F_QUEUED,NAL_F_DROPPED_LO,NAL_F_DROPPED_HI,NAL_F_PCM,NAL_F_SILENCE,
 NAL_F_UNDERRUNS,NAL_F_TICKS,NAL_F_MAX_GAP,NAL_F_PEAK,NAL_F_VOLUME,NAL_F_MUTED,
 NAL_F_OPENS,NAL_F_WRITES,NAL_F_TIME_LO,NAL_F_TIME_HI,NAL_F_STREAM_STARTS,
 NAL_F_TONE_FRAMES,NAL_F_HAL_STATUS,NAL_F_BOARD_STATUS,NAL_F_END
};
struct nal_request {uint32_t token,op,generation;};
struct nal_snapshot {uint32_t word[NAL_WORDS];};
/* EP0 is single producer; a dedicated audio worker consumes commands. ACK
 * means queued, never that sound reached a speaker. No raw addresses, arbitrary
 * writes, executable upload, I2C commands or PCM leave/enter through USB. */
int nal_initialize(uint64_t nonce);
int nal_enabled(void);
int nal_submit(unsigned value,unsigned index);
int nal_take(struct nal_request *);
int nal_complete(const struct nal_request *,unsigned,const struct nal_snapshot *);
size_t nal_caps(uint8_t *,size_t);
size_t nal_reply(uint8_t *,size_t);
int nal_usb(const uint8_t setup[8],uint8_t *,size_t,uint32_t *length);
#endif
