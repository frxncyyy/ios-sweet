/* Nord2 speaker PCM endpoint used by the unmodified upstream HAL plugin.
 * Userspace only queues S16LE48k stereo. One retained worker exclusively owns
 * AFE, amplifier I2C, start/stop and the live command consumer. */
#include <kern/clock.h>
#include <kern/locks.h>
#include <kern/thread.h>
#include <kern/sched_prim.h>
#include <libkern/libkern.h>
#include <miscfs/devfs/devfs.h>
#include <pexpert/pexpert.h>
#include <pexpert/device_tree.h>
#include <libkern/crypto/sha2.h>
#include <sys/conf.h>
#include <sys/errno.h>
#include <sys/proc.h>
#include <sys/random.h>
#include <sys/systm.h>
#include <sys/uio.h>
#include <sys/ioccom.h>
#include "audio_device.h"
#include "audio_stream.h"
#include "audio_live.h"
#include "audio_hw.h"
#include "audio_native.h"
#include "audio_board.h"

#define NSA_CHUNK 256u
#define NSA_SET_VOLUME _IOW('r', 1, struct nsa_volume_ioctl)
struct nsa_volume_ioctl { uint32_t below, muted; };
static LCK_GRP_DECLARE(nsa_group, "nord2-audio");
static LCK_MTX_DECLARE(nsa_lock, &nsa_group);
static struct nsa_stream stream;
static struct nau_hw hardware;
static struct { unsigned ready, opens, stop, starts, tone_frames, tone_phase;
 unsigned tone_active, hal_status; uint64_t report_us; } device;
static uint64_t now_us(void) {
 uint64_t ns=0; absolutetime_to_nanoseconds(mach_absolute_time(),&ns); return ns/1000;
}
static void notify_worker(void) { thread_wakeup((event_t)&device); }
static int audio_open(__unused dev_t d,__unused int f,__unused int t,__unused struct proc *p) {
 if (!device.ready) return ENXIO;
 lck_mtx_lock(&nsa_lock); stream.opens++; device.opens++; lck_mtx_unlock(&nsa_lock);
 return 0;
}
static int audio_close(__unused dev_t d,__unused int f,__unused int t,__unused struct proc *p) {
 lck_mtx_lock(&nsa_lock);
 if (device.opens) device.opens--; /* Drain accepted tail before idle stop. */
 lck_mtx_unlock(&nsa_lock); notify_worker(); return 0;
}
static int audio_write(__unused dev_t d,struct uio *u,__unused int flags) {
 int16_t pcm[NSA_CHUNK*2]; user_ssize_t total=uio_resid(u);
 if (total<0 || total%NSA_FRAME_BYTES) return EINVAL;
 if (total>262144) return E2BIG;
 while (uio_resid(u)) {
  unsigned n=(unsigned)uio_resid(u); if(n>sizeof pcm)n=sizeof pcm;
  int e=uiomove((caddr_t)pcm,(int)n,u); if(e)return e;
  lck_mtx_lock(&nsa_lock); (void)nsa_enqueue(&stream,pcm,n/4,now_us());
  lck_mtx_unlock(&nsa_lock);
 }
 notify_worker(); return 0;
}
static int audio_ioctl(__unused dev_t d,u_long cmd,caddr_t data,
 __unused int flags,__unused struct proc *p) {
 if(cmd!=NSA_SET_VOLUME)return ENOTTY;
 const struct nsa_volume_ioctl *v=(const void*)data;
 if(v->muted>1)return EINVAL;
 lck_mtx_lock(&nsa_lock);nsa_volume(&stream,v->below,v->muted);
 if(v->muted)device.stop=1;
 lck_mtx_unlock(&nsa_lock);notify_worker();return 0;
}
static const struct cdevsw audio_cdev={
 .d_open=audio_open,.d_close=audio_close,.d_read=eno_rdwrt,.d_write=audio_write,
 .d_ioctl=audio_ioctl,.d_stop=eno_stop,.d_reset=eno_reset,.d_select=eno_select,
 .d_mmap=eno_mmap,.d_strategy=eno_strat,.d_type=0
};
static void snapshot(struct nal_snapshot *out) {
 *out=(struct nal_snapshot){0};uint32_t *w=out->word;
 struct nah_status h;nau_hw_status(&hardware,&h);uint64_t t=now_us()/1000;
 w[NAL_F_INITIALIZED]=h.initialized;w[NAL_F_RUNNING]=h.running;
 w[NAL_F_SILENT]=h.silent_only;w[NAL_F_UNCERTAIN]=h.uncertain;
 w[NAL_F_ERROR]=(uint32_t)h.error;w[NAL_F_STOP_ERROR]=(uint32_t)h.stop_error;
 w[NAL_F_GATE]=h.gate;
 w[NAL_F_AMP0]=h.amp_revision[0];w[NAL_F_AMP1]=h.amp_revision[1];
 w[NAL_F_STATUS0]=h.amp_status[0];w[NAL_F_STATUS1]=h.amp_status[1];
 w[NAL_F_PCM]=h.pcm_frames;w[NAL_F_SILENCE]=h.silence_frames;
 w[NAL_F_UNDERRUNS]=h.underruns;w[NAL_F_TICKS]=h.ticks;
 w[NAL_F_MAX_GAP]=h.max_tick_gap_us;w[NAL_F_TIME_LO]=(uint32_t)t;w[NAL_F_TIME_HI]=(uint32_t)(t>>32);
 w[NAL_F_STREAM_STARTS]=device.starts;w[NAL_F_TONE_FRAMES]=device.tone_frames;
 w[NAL_F_HAL_STATUS]=device.hal_status;w[NAL_F_BOARD_STATUS]=nah_board_profile()!=NULL;
 lck_mtx_lock(&nsa_lock);
 w[NAL_F_ENABLED]=stream.enabled;w[NAL_F_WRITTEN_LO]=(uint32_t)stream.written;
 w[NAL_F_WRITTEN_HI]=(uint32_t)(stream.written>>32);w[NAL_F_QUEUED]=stream.count;
 w[NAL_F_DROPPED_LO]=(uint32_t)stream.dropped;w[NAL_F_DROPPED_HI]=(uint32_t)(stream.dropped>>32);
 w[NAL_F_PEAK]=stream.peak;w[NAL_F_VOLUME]=stream.below;w[NAL_F_MUTED]=stream.muted;
 w[NAL_F_OPENS]=stream.opens;w[NAL_F_WRITES]=stream.writes;
 lck_mtx_unlock(&nsa_lock);
}
static int start(unsigned silent,unsigned duration) {
 int e=nau_hw_start(&hardware,48000,2,16,silent,NAH_MAX_GAIN_Q15,duration,now_us());
 if(!e)device.starts++;
 printf("NORD2 AUDIO: start=%d silent=%u duration=%u gate=%u amp=%04x/%04x\n",
 e,silent,duration,hardware.status.gate,hardware.status.amp_revision[0],hardware.status.amp_revision[1]);
 return e;
}
static void commands(void) {
 struct nal_request q;if(!nal_take(&q))return;
 unsigned result=NAL_OK;
 if(q.op==NAL_ENABLE) {
  if(!hardware.status.initialized || hardware.status.uncertain || hardware.status.stop_error)result=NAL_UNAVAILABLE;
  else {lck_mtx_lock(&nsa_lock);nsa_enable(&stream,1);lck_mtx_unlock(&nsa_lock);}
 } else if(q.op==NAL_STOP) {
  lck_mtx_lock(&nsa_lock);nsa_enable(&stream,0);lck_mtx_unlock(&nsa_lock);
  device.tone_active=0;
  if(hardware.status.running && nau_hw_stop(&hardware))result=NAL_REFUSED;
 } else if(q.op==NAL_TONE || q.op==NAL_SILENT_TEST) {
  if(!hardware.status.initialized || hardware.status.uncertain || hardware.status.running)result=NAL_REFUSED;
  else {
   lck_mtx_lock(&nsa_lock);nsa_flush(&stream);lck_mtx_unlock(&nsa_lock);
   if(start(q.op==NAL_SILENT_TEST,1000000))result=NAL_REFUSED;
   else {device.tone_active=1;device.tone_frames=device.tone_phase=0;}
  }
 }
 struct nal_snapshot s;snapshot(&s);(void)nal_complete(&q,result,&s);
 if(!hardware.status.running || q.op!=NAL_STATUS)printf("NORD2 AUDIO LIVE: token=%u op=%u result=%u error=%d\n",q.token,q.op,result,hardware.status.error);
}
static void feed_tone(void) {
 /* Fixed1kHz sine, half scale BEFORE the independent hardware ceiling. */
 static const int16_t wave[48]={0,2139,4240,6270,8192,9974,11585,12998,14189,15137,15826,16244,
 16384,16244,15826,15137,14189,12998,11585,9974,8192,6270,4240,2139,0,-2139,-4240,-6270,-8192,-9974,
 -11585,-12998,-14189,-15137,-15826,-16244,-16384,-16244,-15826,-15137,-14189,-12998,-11585,-9974,-8192,-6270,-4240,-2139};
 int16_t pcm[NSA_CHUNK*2];unsigned n=NAH_QUEUE_FRAMES-hardware.status.queued_frames;
 if(n>NSA_CHUNK)n=NSA_CHUNK;
 if(n>NSA_RATE-device.tone_frames)n=NSA_RATE-device.tone_frames;
 for(unsigned i=0;i<n;i++){int16_t v=wave[device.tone_phase++%48];pcm[2*i]=pcm[2*i+1]=v;}
 if(n){int r=nau_hw_feed(&hardware,pcm,n);if(r>0)device.tone_frames+=(unsigned)r;}
}
static int load_board(void) {
 DTEntry chosen;const void *data=NULL;unsigned n=0;unsigned char hash[32];
 if(SecureDTLookupEntry(NULL,"/chosen",&chosen)!=kSuccess ||
    SecureDTGetProperty(chosen,"nord2-audio-board",&data,&n)!=kSuccess || n!=4163)return -1;
 SHA256_CTX ctx;SHA256_Init(&ctx);SHA256_Update(&ctx,data,n);SHA256_Final(hash,&ctx);
 return nah_board_load(data,n,hash);
}
static void audio_worker(__unused void *arg,__unused wait_result_t wr) {
 struct nah_io io;void *cookie=NULL;
 int e=load_board()?NAH_E_BOARD_CONFIG:0;
 if(!e)e=nah_native_prepare(&io,&cookie);
 if(!e)e=nau_hw_init(&hardware,&io,cookie,nah_board_profile());
 if(e)hardware.status.error=e;
 uint64_t nonce=0;read_random(&nonce,sizeof nonce);if(nonce)(void)nal_initialize(nonce);
 printf("NORD2 AUDIO: worker=%d initialized=%u gate=%u amp=%04x/%04x\n",e,
 hardware.status.initialized,hardware.status.gate,hardware.status.amp_revision[0],hardware.status.amp_revision[1]);
 for(;;) {
  commands();
  unsigned stop=0,enabled=0,queued=0;uint64_t last_sound=0,t;
  lck_mtx_lock(&nsa_lock);stop=device.stop;device.stop=0;enabled=stream.enabled;
  queued=stream.count;last_sound=stream.last_sound_us;lck_mtx_unlock(&nsa_lock);t=now_us();
  if(hardware.status.running && (stop || (!device.tone_active && (!enabled || !last_sound || t-last_sound>=750000)))) {
   device.tone_active=0;(void)nau_hw_stop(&hardware);
   lck_mtx_lock(&nsa_lock);nsa_flush(&stream);lck_mtx_unlock(&nsa_lock);queued=0;
  }
  /* No amp power for an idle HAL's zero-filled writes. Errors remain visible
   * and do not spin restarting hardware. Live ENABLE after a clean stop can
   * accept subsequent playback; uncertain bus state is never retried. */
  if(!hardware.status.running && !hardware.status.error && enabled && queued &&
     last_sound && t-last_sound<750000 && !device.tone_active)(void)start(0,0);
  if(hardware.status.running) {
   if(device.tone_active)feed_tone();
   else {
    int16_t pcm[NSA_CHUNK*2];unsigned room=NAH_QUEUE_FRAMES-hardware.status.queued_frames;
    if(room>NSA_CHUNK)room=NSA_CHUNK;
    lck_mtx_lock(&nsa_lock);unsigned n=nsa_dequeue(&stream,pcm,room);lck_mtx_unlock(&nsa_lock);
    if(n)(void)nau_hw_feed(&hardware,pcm,n);
   }
   e=nau_hw_tick(&hardware,now_us());
   if(!hardware.status.running){device.tone_active=0;
    if(e==NAH_E_IDLE && !hardware.status.uncertain && !hardware.status.stop_error)hardware.status.error=0;
    printf("NORD2 AUDIO: stopped=%d error=%d uncertain=%u pcm=%u silence=%u\n",e,hardware.status.error,
     hardware.status.uncertain,hardware.status.pcm_frames,hardware.status.silence_frames);}
  } else {lck_mtx_lock(&nsa_lock);if(!last_sound || t-last_sound>=750000)nsa_flush(&stream);lck_mtx_unlock(&nsa_lock);}
  if(!hardware.status.running && t-device.report_us>=5000000) {
   struct nal_snapshot s;snapshot(&s);device.report_us=t;
   printf("NORD2 AUDIO PCM: opens=%u writes=%u queued=%u peak=%u running=%u error=%d pcm=%u\n",
    s.word[NAL_F_OPENS],s.word[NAL_F_WRITES],s.word[NAL_F_QUEUED],s.word[NAL_F_PEAK],
    s.word[NAL_F_RUNNING],(int32_t)s.word[NAL_F_ERROR],s.word[NAL_F_PCM]);
  }
  uint64_t deadline;clock_interval_to_deadline(2,NSEC_PER_MSEC,&deadline);
  assert_wait_deadline((event_t)&device,THREAD_UNINT,deadline);thread_block(THREAD_CONTINUE_NULL);
 }
}
void nord2_audio_initialize(void) {
 unsigned mode=0;PE_parse_boot_argn("nord2audio",&mode,sizeof mode);if(mode!=1 || device.ready)return;
 nsa_initialize(&stream,1);
 int major=cdevsw_add(-1,&audio_cdev);
 if(major<0){printf("NORD2 AUDIO: no device major\n");return;}
 if(!devfs_make_node(makedev(major,0),DEVFS_CHAR,UID_ROOT,GID_WHEEL,0666,"redwood-audio")){
  (void)cdevsw_remove(major,&audio_cdev);printf("NORD2 AUDIO: devfs failed\n");return;
 }
 thread_t thread=THREAD_NULL;
 if(kernel_thread_start(audio_worker,NULL,&thread)!=KERN_SUCCESS){printf("NORD2 AUDIO: worker failed\n");return;}
 thread_deallocate(thread);device.ready=1;device.hal_status=1;
 printf("NORD2 AUDIO: devfs redwood-audio rate=48000 channels=2 bits=16\n");
}
