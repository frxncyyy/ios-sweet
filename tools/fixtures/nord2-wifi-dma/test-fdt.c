#include "dma_pool_contract.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t be32(const unsigned char *p) {
    return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3];
}
static void putbe64(unsigned char *p,uint64_t v) {
    for(unsigned i=0;i<8;i++)p[i]=(unsigned char)(v>>(8u*(7u-i)));
}
/* Test-fixture lookup by exact node/property in the saved FDT. */
static unsigned char *property(unsigned char *f,size_t bytes,
                                const char *node,const char *key,size_t *len) {
    size_t so=be32(f+8),st=be32(f+12),ss=be32(f+32),end=so+be32(f+36),off=so;
    unsigned depth=0;int match=0;
    assert(end<=bytes && st+ss<=bytes);
    while(off+4<=end) {
        uint32_t token=be32(f+off);off+=4;
        if(token==1) {
            const char *s=(const char *)f+off;
            size_t n=0;while(n<end-off && s[n])n++;
            assert(off+n<end);
            depth++;
            if(depth==3 && strcmp(s,node)==0)match=1;
            if(depth==2 && strcmp(s,node)==0)match=1;
            off=(off+n+4u)&~(size_t)3;
        } else if(token==2) {
            if(depth==3 || depth==2)match=0;
            assert(depth);depth--;
        } else if(token==3) {
            assert(off+8<=end);
            size_t n=be32(f+off),no=be32(f+off+4);off+=8;
            assert(no<ss && off+n<=end);
            if(match && strcmp((const char *)f+st+no,key)==0) {
                *len=n;return f+off;
            }
            off=(off+n+3u)&~(size_t)3;
        } else if(token==4) {
        } else if(token==9) break;
        else assert(0);
    }
    assert(0 && "fixture property missing");return NULL;
}
static void rejects(unsigned char *f,size_t n) {
    struct ndp_claim x={9,9,9,9};
    assert(ndp_fixed_claim_from_fdt(f,n,&x)!=0);
    assert(x.base==0 && x.bytes==0 && x.source==0 && x.valid==0);
}
int main(int argc,char **argv) {
    assert(argc==2);
    FILE *fp=fopen(argv[1],"rb");assert(fp);
    assert(fseek(fp,0,SEEK_END)==0);
    long nlong=ftell(fp);assert(nlong>0 && nlong<1048576);
    size_t n=(size_t)nlong;
    assert(fseek(fp,0,SEEK_SET)==0);
    unsigned char *f=malloc(n);assert(f && fread(f,1,n,fp)==n && fgetc(fp)==EOF);
    fclose(fp);
    struct ndp_claim c={0};
    assert(ndp_fixed_claim_from_fdt(f,n,&c)==0);
    assert(c.base==NDP_POOL_BASE && c.bytes==NDP_POOL_BYTES &&
           c.source==NDP_SOURCE_IOS_FIXED && c.valid==1);
    struct ndp_subpool p;
    assert(ndp_subpool_layout(&c,&p)==0);
    assert(p.tx_desc==0x90000000u && p.tx_desc_bytes==0x4000u);
    assert(p.rx_desc==0x90004000u && p.rx_desc_bytes==0x100u);
    assert(p.tx_cmd==0x90005000u && p.tx_cmd_bytes==0x1000u);
    assert(p.rx_events==0x90006000u && p.rx_event_stride==0x1000u &&
           p.rx_event_count==16u && p.rx_events+16u*0x1000u<=NDP_POOL_BASE+NDP_SUBPOOL_BYTES);
    uint8_t wire[24];struct ndp_claim d;
    assert(ndp_claim_encode(&c,wire)==0 && ndp_claim_decode(wire,24,&d)==0);
    assert(memcmp(&c,&d,sizeof(c))==0);
    wire[20]=0;assert(ndp_claim_decode(wire,24,&d)!=0);wire[20]=1;
    assert(ndp_claim_decode(wire,23,&d)!=0);
    struct ndp_smc_request req[2];
    assert(ndp_region18_set_intent(&c,req)==0);
    assert(req[0].x[0]==0xc200050bu && req[0].x[1]==0 &&
           req[0].x[2]==0x12009000u && req[0].x[3]==0x010090f1u &&
           req[0].x[4]==0x00b6db6du);
    assert(req[1].x[3]==0x000090f1u && req[1].x[4]==0x80b6da28u);
    uint64_t raw[2][4]={{0}};
    assert(ndp_both_raw_zero((const uint64_t (*)[4])raw)==0);
    raw[1][0]=(uint64_t)-4;
    assert(ndp_both_raw_zero((const uint64_t (*)[4])raw)!=0);
    raw[1][0]=0;raw[0][1]=1;
    assert(ndp_both_raw_zero((const uint64_t (*)[4])raw)==0);
    size_t len;unsigned char *v;
    v=property(f,n,"wifi-reserve-memory","size",&len);assert(len==8);
    v[7]^=1;rejects(f,n);v[7]^=1;
    v=property(f,n,"wifi-reserve-memory","alignment",&len);assert(len==8);
    v[7]^=1;rejects(f,n);v[7]^=1;
    v=property(f,n,"wifi-reserve-memory","alloc-ranges",&len);assert(len==16);
    v[7]^=1;rejects(f,n);v[7]^=1;
    v=property(f,n,"wifi@18000000","memory-region",&len);assert(len==4);
    v[3]^=1;rejects(f,n);v[3]^=1;
    v=property(f,n,"mblock-27-DPM-reserved","reg",&len);assert(len==16);
    unsigned char saved[16];memcpy(saved,v,16);
    putbe64(v,NDP_POOL_BASE);putbe64(v+8,0x10000);
    rejects(f,n);memcpy(v,saved,16);
    unsigned char h=f[4];f[4]=0xff;rejects(f,n);f[4]=h;
    assert(ndp_fixed_claim_from_fdt(f,n,&c)==0);
    free(f);puts("DMA fixed claim, SET intent and subpool PASS");return 0;
}
