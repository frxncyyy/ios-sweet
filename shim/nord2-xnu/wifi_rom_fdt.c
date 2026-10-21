#include "wifi_rom_contract.h"

static uint32_t b32(const unsigned char *p)
{ return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|p[3]; }
static uint64_t b64(const unsigned char *p)
{ return ((uint64_t)b32(p)<<32)|b32(p+4); }
static int range(size_t off,size_t n,size_t total)
{ return off<=total && n<=total-off; }
static int overlaps(uint64_t base,uint64_t size)
{
    if (!size || size>UINT64_MAX-base) return -1;
    return base<NRC_CONN_BASE+NRC_CONN_BYTES && NRC_CONN_BASE<base+size;
}
static int name_eq(const char *p,size_t n,const char *want)
{
    size_t k=0;
    while(want[k])k++;
    if(n!=k)return 0;
    for(size_t i=0;i<k;i++)if(p[i]!=want[i])return 0;
    return 1;
}
static int name_base(const char *p,size_t n,const char *want)
{
    size_t k=0;
    while(want[k])k++;
    if(n<k || (n!=k && p[k]!='@'))return 0;
    for(size_t i=0;i<k;i++)if(p[i]!=want[i])return 0;
    return 1;
}
static int cells_overlap(const unsigned char *p,size_t n,int *contained)
{
    if (!n || n%16) return -1;
    for(size_t i=0;i<n;i+=16) {
        uint64_t base=b64(p+i),size=b64(p+i+8);
        int ov=overlaps(base,size);
        if (ov<0) return -1;
        if (contained && base<=NRC_CONN_BASE && size>=NRC_CONN_BYTES &&
            NRC_CONN_BASE-base<=size-NRC_CONN_BYTES) *contained=1;
        else if (!contained && ov) return -1;
    }
    return 0;
}
int nrc_ios_fixed_claim_from_fdt(const void *fdt,size_t bytes,struct nrc_claim *out)
{
    if (!out) return -1;
    *out=(struct nrc_claim){0};
    if (!fdt || bytes<40) return -1;
    const unsigned char *f=fdt;
    if (b32(f)!=0xd00dfeed) return -1;
    size_t total=b32(f+4),so=b32(f+8),st=b32(f+12),ro=b32(f+16),
        ss=b32(f+32),sz=b32(f+36);
    if (total>bytes || total<40 || !range(so,sz,total) ||
        !range(st,ss,total) || !range(ro,16,total) || ro>=so ||
        ro<40 || (ro&7) || (so&3) || (st&3) || (sz&3) ||
        b32(f+20)!=17 || b32(f+24)!=16 || so+sz>st) return -1;
    int reserve_end=0;
    for(size_t off=ro;range(off,16,so);off+=16) {
        uint64_t base=b64(f+off),size=b64(f+off+8);
        if(!base && !size){reserve_end=1;break;}
        int ov=overlaps(base,size);
        if(ov!=0) return -1;
    }
    if(!reserve_end) return -1;
    size_t off=so,end=so+sz;
    unsigned depth=0,root_cells=0,reserved_props=0,cons_props=0;
    int top_reserved=0,top_memory=0,top_consys=0,cons_node=0;
    int memory_contains=0,seen_root=0,seen_memory=0,seen_reserved=0,
        seen_top_consys=0,seen_cons=0,memory_reg_seen=0,
        reserved_reg_seen=0,ended=0;
    while(range(off,4,end)) {
        uint32_t tok=b32(f+off);off+=4;
        if(tok==1) {
            size_t begin=off;
            while(off<end && f[off])off++;
            if(off==end || depth>=32)return -1;
            size_t n=off-begin;off=(off+4)&~(size_t)3;
            if(off>end)return -1;
            depth++;
            if(depth==1) {
                if(seen_root || n)return -1;
                seen_root=1;
            } else if(depth==2) {
                top_reserved=name_eq((const char *)f+begin,n,"reserved-memory");
                top_memory=name_base((const char *)f+begin,n,"memory");
                top_consys=name_eq((const char *)f+begin,n,"consys@18000000");
                if(top_reserved && seen_reserved++)return -1;
                if(top_memory && seen_memory++)return -1;
                if(top_consys && seen_top_consys++)return -1;
            } else if(depth==3 && top_reserved) {
                cons_node=name_eq((const char *)f+begin,n,"consys-reserve-memory");
                if(cons_node){if(seen_cons)return -1;seen_cons=1;}
                reserved_reg_seen=0;
            }
            if(depth>=4 && top_reserved)return -1; /* no nested bus translation */
        } else if(tok==2) {
            if(!depth)return -1;
            if(depth==3)cons_node=0;
            if(depth==2)top_reserved=top_memory=top_consys=0;
            depth--;
        } else if(tok==3) {
            if(!range(off,8,end))return -1;
            size_t n=b32(f+off),no=b32(f+off+4);off+=8;
            if(!range(off,n,end) || no>=ss)return -1;
            size_t k=no;while(k<ss && f[st+k])k++;
            if(k==ss)return -1;
            const char *key=(const char *)f+st+no;
            size_t kn=k-no;
            const unsigned char *v=f+off;
            if(depth==1 && name_eq(key,kn,"#address-cells")) {
                if((root_cells&1) || n!=4 || b32(v)!=2)return -1;
                root_cells|=1;
            } else if(depth==1 && name_eq(key,kn,"#size-cells")) {
                if((root_cells&2) || n!=4 || b32(v)!=2)return -1;
                root_cells|=2;
            } else if(depth==2 && top_reserved && name_eq(key,kn,"#address-cells")) {
                if((reserved_props&1) || n!=4 || b32(v)!=2)return -1;
                reserved_props|=1;
            } else if(depth==2 && top_reserved && name_eq(key,kn,"#size-cells")) {
                if((reserved_props&2) || n!=4 || b32(v)!=2)return -1;
                reserved_props|=2;
            } else if(depth==2 && top_reserved && name_eq(key,kn,"ranges")) {
                if((reserved_props&4) || n)return -1;
                reserved_props|=4;
            } else if(depth==2 && top_memory && name_eq(key,kn,"reg")) {
                if(memory_reg_seen++)return -1;
                if(cells_overlap(v,n,&memory_contains))return -1;
            } else if(depth==3 && top_reserved && name_eq(key,kn,"reg")) {
                if(reserved_reg_seen++)return -1;
                if(cons_node)return -1; /* inherited claim is a separate route */
                if(cells_overlap(v,n,0))return -1;
            }
            if(depth==3 && top_reserved &&
                (name_eq(key,kn,"#address-cells") ||
                 name_eq(key,kn,"#size-cells") || name_eq(key,kn,"ranges")))return -1;
            if(depth==3 && cons_node) {
                if(name_eq(key,kn,"no-map")) {
                    if((cons_props&1) || n)return -1;
                    cons_props|=1;
                } else if(name_eq(key,kn,"size")) {
                    if((cons_props&2) || n!=8 || b64(v)!=NRC_CONN_BYTES)return -1;
                    cons_props|=2;
                } else if(name_eq(key,kn,"alignment")) {
                    if((cons_props&4) || n!=8 || b64(v)!=0x100000)return -1;
                    cons_props|=4;
                } else if(name_eq(key,kn,"alloc-ranges")) {
                    if((cons_props&8) || n!=16 || b64(v)!=0x50000000 || b64(v+8)!=0x40000000)return -1;
                    cons_props|=8;
                }
            }
            if(depth==2 && top_consys &&
                (name_eq(key,kn,"emi-addr") || name_eq(key,kn,"emi-size")))return -1;
            off+=n;off=(off+3)&~(size_t)3;
            if(off>end)return -1;
        } else if(tok==4) {
            continue;
        } else if(tok==9) {
            if(depth)return -1;
            ended=1;break;
        } else return -1;
    }
    if(!ended || !seen_root || root_cells!=3 || !seen_reserved || reserved_props!=7 ||
       !seen_memory || !memory_reg_seen || !memory_contains ||
       !seen_cons || cons_props!=15)return -1;
    *out=(struct nrc_claim){NRC_CLAIM_IOS_FIXED,NRC_CONN_BASE,NRC_CONN_BYTES};
    return 0;
}
