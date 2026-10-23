/* SPDX-License-Identifier: MIT
 * Private Nord2 audio package parser. No filesystem or allocation side effects.
 * Wire fields are explicitly little endian; no packed-struct dereferences.
 */
#ifndef NORD2_AUDIO_PACKAGE_H
#define NORD2_AUDIO_PACKAGE_H
#define N2AP_MAX 262144u
#define N2AP_HEADER 96u
struct n2ap_entry { const unsigned char *bytes; unsigned length; };
struct n2ap_view { struct n2ap_entry executable, plist; };
typedef int (*n2ap_hash_fn)(const unsigned char *, unsigned,
    const unsigned char expected[32]);
static unsigned n2ap_u32(const unsigned char *p)
{ return (unsigned)p[0] | (unsigned)p[1]<<8 | (unsigned)p[2]<<16 | (unsigned)p[3]<<24; }
static int n2ap_parse(const void *data, unsigned size, n2ap_hash_fn hash,
    struct n2ap_view *out)
{
    const unsigned char *p = data;
    unsigned a, b, i;
    if (!p || !out || !hash || size < N2AP_HEADER || size > N2AP_MAX) return -1;
    if (p[0]!='N'||p[1]!='2'||p[2]!='A'||p[3]!='P'||n2ap_u32(p+4)!=1 ||
        n2ap_u32(p+8)!=size || n2ap_u32(p+12)!=2) return -2;
    a=n2ap_u32(p+16); b=n2ap_u32(p+20);
    if (!a || !b || a > size-N2AP_HEADER || b != size-N2AP_HEADER-a) return -3;
    for (i=24;i<32;i++) if(p[i]) return -4;
    if (hash(p+N2AP_HEADER,a,p+32) || hash(p+N2AP_HEADER+a,b,p+64)) return -5;
    out->executable.bytes=p+N2AP_HEADER; out->executable.length=a;
    out->plist.bytes=p+N2AP_HEADER+a; out->plist.length=b;
    return 0;
}
#endif
