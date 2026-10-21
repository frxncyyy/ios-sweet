#ifndef PRIVATE_NIWC_CARD_H
#define PRIVATE_NIWC_CARD_H
#include <stddef.h>
#include <stdint.h>
#define NIWC_CARD_BYTES 13u
/* Same-IPSW association-builder gates, NOT a complete Apple enum.
 * WEP_FAMILY covers40/104-bit suites; GCMP_FAMILY covers128/256-bit
 * suites because each one raw gate admits both. WPA/RSN names denote the
 * actual dictionary-key paths, not a complete protocol/security-mode enum. */
enum niwc_feature { NIWC_WEP_FAMILY,NIWC_TKIP,NIWC_CCMP,NIWC_WPA_IE,NIWC_RSN_IE,NIWC_GCMP_FAMILY,NIWC_FEATURES };
enum niwc_operation { NIWC_ASSOCIATE=1,NIWC_AUTHENTICATE=2,NIWC_INSTALL_KEY=4,
 NIWC_CIPHER_TX=8,NIWC_CIPHER_RX=16,NIWC_DISCONNECT=32,NIWC_END_TO_END=63 };
struct niwc_entry {unsigned feature,installed_operations;};
/* Native-owned reviewed inventory only; never deserialize from IOUC input.
 * Each entry must derive from actual installed association/auth/key/data/
 * disconnect handlers for that feature. Bits do not authenticate their source
 * or establish correct implementations. No current backend supplies entries. */
struct niwc_manifest {unsigned count;struct niwc_entry entry[NIWC_FEATURES];};
/* Exact bind request12 GET, small carrier, payload13, value0. Writes raw13,
 * no version prefix. A nonnull native-owned empty inventory emits exactly13 zero bytes.
 * Source-proven nonempty entries require complete implementations;
 * unknown/duplicate/partial features refuse atomically.
 * Caller owns task/generation/serialization and exact interface binding. */
int niwc_card_reply(const void *,size_t,const struct niwc_manifest *,
 void *,size_t,size_t *);
#endif
