/* SPDX-License-Identifier: MIT */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include "audio_package.h"
static int digest(const unsigned char *p,unsigned n,const unsigned char h[32])
{ unsigned char d[32]; SHA256(p,n,d); return memcmp(d,h,32); }
int main(int argc,char **argv)
{
    assert(argc==2); FILE *f=fopen(argv[1],"rb"); assert(f);
    unsigned char *p=malloc(N2AP_MAX+1); assert(p);
    unsigned n=(unsigned)fread(p,1,N2AP_MAX+1,f); fclose(f);
    struct n2ap_view v={0}; assert(!n2ap_parse(p,n,digest,&v));
    assert(v.executable.bytes==p+96 && v.plist.bytes+v.plist.length==p+n);
    for(unsigned i=0;i<n;i++) {
        p[i]^=1; assert(n2ap_parse(p,n,digest,&v)); p[i]^=1;
    }
    for(unsigned i=0;i<n;i++) assert(n2ap_parse(p,i,digest,&v));
    assert(n2ap_parse(p,N2AP_MAX+1,digest,&v));
    assert(n2ap_parse(NULL,n,digest,&v));
    assert(n2ap_parse(p,n,NULL,&v)); assert(n2ap_parse(p,n,digest,NULL));
    memset(p+16,0xff,8); assert(n2ap_parse(p,n,digest,&v));
    free(p); puts("PASS parser corruption/truncation/overflow/null tests");
}
