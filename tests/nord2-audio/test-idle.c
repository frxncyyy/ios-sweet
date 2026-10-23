#include <assert.h>
#include <stdio.h>
#include "audio_stream.h"
static struct nsa_stream s;
static const int16_t click[4]={12000,-12000,6000,-6000};
static void idle_fresh(uint64_t now){nsa_flush_idle(&s,now);}
int main(void){
 /* Exact interleaving: worker snapshots idle before HAL writes one click. */
 nsa_initialize(&s,1);uint64_t stale=s.last_sound_us;
 assert(nsa_enqueue(&s,click,2,1000000)==2);
 if(!stale||1000000-stale>=750000)nsa_flush(&s);
 assert(s.count==0); /* Existing worker loses wholeacceptedshortsound. */
 nsa_initialize(&s,1);stale=s.last_sound_us;(void)stale;
 assert(nsa_enqueue(&s,click,2,1000000)==2);idle_fresh(1000001);
 assert(s.count==2);int16_t out[4];assert(nsa_dequeue(&s,out,2)==2&&out[0]==12000&&out[1]==-12000);
 /* A new unmuted click during ampstop must survive the stopcompletion. */
 nsa_initialize(&s,1);nsa_volume(&s,0,1);nsa_volume(&s,0,0);
 assert(nsa_enqueue(&s,click,2,2000000)==2);
 /* Fixed stop does not flushstream. Its freshidlecheck preservesnewPCM. */
 idle_fresh(2001000);assert(s.count==2);
 /* Zero-only idle queue is still bounded and discarded. */
 nsa_initialize(&s,1);int16_t zeros[4]={0};assert(nsa_enqueue(&s,zeros,2,3000000)==2);idle_fresh(3000001);assert(!s.count);
 /* Deliberate policy: age never discards meaningful queued PCM. */
 nsa_initialize(&s,1);assert(nsa_enqueue(&s,click,2,4000000)==2);
 idle_fresh(4750000);assert(s.count==2&&s.sound_frames==2);
 assert(nsa_dequeue(&s,out,2)==2&&out[0]==12000&&out[1]==-12000);
 assert(!s.count&&!s.sound_frames);
 /* After that old sound drains, fresh HALzero tails are still discarded:
  * zero-writes must not reset last_sound_us or keep the amplifieralive. */
 assert(nsa_enqueue(&s,zeros,2,4750001)==2);idle_fresh(4750001);
 assert(!s.count&&!s.sound_frames&&s.last_sound_us==4000000);
 puts("PASS: reproducedacceptedclickloss; freshlockedidlecheckpreservesnewandoldPCM;drainedsoundzerotailsstillflush");
}
