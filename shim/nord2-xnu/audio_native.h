#ifndef NORD2_AUDIO_NATIVE_H
#define NORD2_AUDIO_NATIVE_H
struct nah_io;
enum {
 NAH_NATIVE_READY=0, NAH_NATIVE_WAIT=1,
 NAH_NATIVE_E_ARGS=-32, NAH_NATIVE_E_PAGE=-33,
 NAH_NATIVE_E_DEPENDENCIES=-34, NAH_NATIVE_E_REENTRY=-35,
 NAH_NATIVE_E_CONTEXT=-36, NAH_NATIVE_E_MAP=-37,
 NAH_NATIVE_E_WINDOW=-38, NAH_NATIVE_E_TIMEOUT=-39
};
/* Only NI calls publish, after all initial GPIO/WiFi/touch writes, before
 * polling. No MMIO/map occurs here. Retained dependency VAs publish once. */
int nah_native_publish_ready(void);
/* Side-effect-free acquire observation: WAIT, READY, or permanent error. */
int nah_native_dependency_state(void);
/* Only WAIT may be retried. Once ready, mapping/validation is one attempt.
 * No hardware MMIO is performed by prepare; nau_hw_init is called once later. */
int nah_native_prepare(struct nah_io *,void **);
#endif
