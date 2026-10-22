static njs_snapshot js;static niwj_request copiedJoin;static njs_completion jcb;static void*jcookie;static uint64_t jepoch,jticket=800;static bool jactive,jcanceled;static int jenqueueResult;static unsigned jenqueues,jdisconnects;
extern "C" int nord2_wifi_join_enqueue(const niwj_request*q,void*c,uint64_t e,njs_completion f,uint64_t*t){if(jenqueueResult)return jenqueueResult;assert(!jactive&&q&&q->security==1);copiedJoin=*q;jcookie=c;jepoch=e;jcb=f;*t=++jticket;jactive=true;jcanceled=false;jenqueues++;js={};js.native_generation=3;js.client_epoch=e;js.ticket=jticket;js.phase=NJS_QUEUED;return 0;}
extern "C" int nord2_wifi_join_cancel(uint64_t t){assert(t==jticket&&jactive);jcanceled=true;return 0;}
extern "C" int nord2_wifi_join_disconnect(void){jdisconnects++;js.phase=NJS_LEAVING;js.associated=js.ptk_ready=js.gtk_ready=js.data_ready=0;return 0;}
extern "C" int nord2_wifi_join_copy_state(njs_snapshot*out){*out=js;return 0;}
static void joinNotice(njs_notice n){assert(jactive);jcb(jcookie,jepoch,jticket,n);if(n==NJS_RETIRED){jactive=false;niwj_clear(&copiedJoin,sizeof copiedJoin);}}

static void assertNoJoins(){assert(!jenqueues&&!jdisconnects&&!jcanceled&&!jactive);}
