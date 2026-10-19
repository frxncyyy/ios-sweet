/*
 * First Nord 2 bring-up probe, derived from ios-sweet's Linux Image shim.
 * Only writes its own RAM and the existing ramoops pmsg buffer. The latter
 * is read as /sys/fs/pstore/pmsg-ramoops-0 after Android boots again.
 * No framebuffer, UFS, PMIC or flash writes. Does not boot XNU or iOS.
 *
 * The addresses are independently verified against the bootloader's FDT.
 * pmsg is the last zone: 0x48090000 + 0xe0000 - 0x10000 = 0x48160000.
 * Current Android exports ecc=0, pmsg_size=65536 through ramoops sysfs.
 */
#include <stdint.h>
#include <stddef.h>
#define RSR(name) ({ uint64_t r; __asm__ volatile("mrs %0, " name : "=r"(r)); r; })
#define PSTORE_BASE 0x48090000UL
#define PSTORE_SIZE 0xe0000UL
#define PMSG_SIZE 0x10000UL
#define PMSG_BASE (PSTORE_BASE + PSTORE_SIZE - PMSG_SIZE)
struct ring { uint32_t sig, start, size; char data[]; };
static volatile struct ring *logbuf;
static unsigned pos;
static uintptr_t saved_dtb;

static int same(const char *a, const char *b) {
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}
static uint32_t be32(const void *p) {
    const volatile unsigned char *b = p;
    return (uint32_t)b[0]<<24 | (uint32_t)b[1]<<16 | (uint32_t)b[2]<<8 | b[3];
}
static int contains(const char *s, unsigned n, const char *wanted) {
    for (unsigned i=0; i<n; i++) {
        unsigned j=0;
        while (wanted[j] && i+j<n && s[i+j]==wanted[j]) j++;
        if (!wanted[j]) return 1;
    }
    return 0;
}
/* Bounded FDT parsing; never log or export serial numbers / bootargs. */
static const unsigned char *prop(uintptr_t addr, const char *path,
                                const char *key, uint32_t *length) {
    const unsigned char *f=(const void *)addr;
    if (!addr || (addr & 3) || be32(f)!=0xd00dfeed) return NULL;
    uint32_t total=be32(f+4), off=be32(f+8), strings=be32(f+12);
    uint32_t ss=be32(f+32), ds=be32(f+36);
    if (total<40 || total>0x200000 || off>total || ds>total-off ||
        strings>total || ss>total-strings) return NULL;
    uint32_t end=off+ds, lens[32]; unsigned depth=0, plen=0;
    char current[256]; current[0]=0;
    while (off+4<=end) {
        uint32_t tok=be32(f+off); off+=4;
        if (tok==1) {
            if (depth==32) return NULL;
            lens[depth]=plen;
            if (depth) { if (plen+1>=sizeof(current)) return NULL; current[plen++]='/'; }
            while (off<end && f[off]) {
                if (plen+1>=sizeof(current)) return NULL;
                current[plen++]=f[off++];
            }
            if (off>=end) return NULL;
            off=(off+4)&~3U; current[plen]=0; depth++;
        } else if (tok==2) {
            if (!depth) return NULL;
            plen=lens[--depth]; current[plen]=0;
        } else if (tok==3) {
            if (off+8>end) return NULL;
            uint32_t n=be32(f+off), no=be32(f+off+4); off+=8;
            if (n>end-off || no>=ss) return NULL;
            uint32_t j=no; while (j<ss && f[strings+j]) j++;
            if (j==ss) return NULL;
            if (same(current,path) && same((const char *)f+strings+no,key)) {
                *length=n; return f+off;
            }
            off=(off+n+3)&~3U;
        } else if (tok!=4) return NULL;
    }
    return NULL;
}
static void puts_log(const char *s) {
    if (!logbuf) return;
    while (*s && pos<PMSG_SIZE-sizeof(struct ring)) logbuf->data[pos++]=*s++;
    __asm__ volatile("dsb sy" ::: "memory");
    logbuf->start=pos; logbuf->size=pos;
    __asm__ volatile("dsb sy" ::: "memory");
}
static void hex(const char *name, uint64_t v) {
    char b[19]; b[0]='0'; b[1]='x';
    for (int i=0; i<16; i++) b[2+i]="0123456789abcdef"[(v>>(60-i*4))&15];
    b[18]=0; puts_log(name); puts_log("="); puts_log(b); puts_log("\n");
}
static uint64_t smc(uint64_t fn) {
    register uint64_t x0 __asm__("x0")=fn;
    register uint64_t x1 __asm__("x1")=0, x2 __asm__("x2")=0, x3 __asm__("x3")=0;
    __asm__ volatile("smc #0" : "+r"(x0), "+r"(x1), "+r"(x2), "+r"(x3)
        : : "x4","x5","x6","x7","x8","x9","x10","x11","x12","x13",
            "x14","x15","x16","x17","memory");
    return x0;
}
static void reset(void) __attribute__((noreturn));
static void reset(void) {
#ifdef CHAIN_STOCK
    extern void chain_stock(uintptr_t dtb) __attribute__((noreturn));
    puts_log("NORD2-PROBE: CHAIN TO ORIGINAL ANDROID KERNEL\n");
    chain_stock(saved_dtb);
#else
    puts_log("NORD2-PROBE: PSCI SYSTEM_RESET\n");
    smc(0x84000009);
    for (;;) __asm__ volatile("wfe");
#endif
}
void probe_exception(uint64_t slot) {
    puts_log("NORD2-PROBE: EXCEPTION\n");
    hex("slot",slot); hex("esr",RSR("esr_el1"));
    hex("elr",RSR("elr_el1")); hex("far",RSR("far_el1"));
    reset();
}
void probe_main(uintptr_t dtb) {
    saved_dtb=dtb;
    uint32_t n=0; const unsigned char *p=prop(dtb,"","compatible",&n);
    if (!p || !contains((const char *)p,n,"mediatek,MT6893")) reset();
    p=prop(dtb,"/reserved-memory/mblock-16-pstore","reg",&n);
    if (!p || n!=16 || be32(p) || be32(p+4)!=PSTORE_BASE ||
        be32(p+8) || be32(p+12)!=PSTORE_SIZE) reset();
    p=prop(dtb,"/chosen","bootargs",&n);
    if (!p || !contains((const char *)p,n,"ramoops.pmsg_size=0x10000")) reset();
    /* Do not write through unexpected cache mappings left by a loader. */
    if (RSR("sctlr_el1") & 5) reset();
    logbuf=(volatile struct ring *)PMSG_BASE;
    logbuf->sig=0x43474244; logbuf->start=0; logbuf->size=0;
    puts_log("NORD2-PROBE v2: CPU diagnostics, no storage writes\n");
    hex("dtb",dtb); hex("CurrentEL",RSR("CurrentEL"));
    hex("MPIDR_EL1",RSR("mpidr_el1")); hex("MIDR_EL1",RSR("midr_el1"));
    hex("ID_AA64MMFR0_EL1",RSR("id_aa64mmfr0_el1"));
    hex("ID_AA64PFR0_EL1",RSR("id_aa64pfr0_el1"));
    hex("SCTLR_EL1",RSR("sctlr_el1")); hex("CNTFRQ_EL0",RSR("cntfrq_el0"));
    hex("ICC_SRE_EL1",RSR("icc_sre_el1"));
    hex("PSCI_VERSION",smc(0x84000000)); hex("SMCCC_VERSION",smc(0x80000000));
    puts_log("NORD2-PROBE: COMPLETE\n");
    reset();
}
