/*
 * Nord 2 XNU early-probe loader.
 *
 * Runs where LK loaded this Linux Image (no relocation pass, position
 * independent, MMU and caches off). It:
 *   1. validates the live FDT (MT6893, pstore/pmsg, stock Image);
 *   2. opens the ramoops pmsg ring and logs the loader stage;
 *   3. copies the SWEETXNU payload's Mach-O segments to the Nord2 window;
 *   4. places the ADT and boot_args behind the kernel;
 *   5. branches to the XNU entry point with x0 = boot_args PA.
 *
 * Image layout produced by tools/nord2-pack-xnu.py, all inside the
 * decompressed kernel blob at 0x40080000:
 *   0x000000  this loader (padded to 2 MiB)
 *   0x200000  the unchanged stock Android kernel Image (BSS included)
 *   + image_size  the SWEETXNU payload (16 KiB aligned)
 * The stock ramdisk and DT table stay where LK put them, so the chain-back
 * to Android gets its original initrd.
 *
 * Nothing here writes storage, the framebuffer or a reserved carve-out.
 */
#include <stdint.h>
#include <stddef.h>

#define PSTORE_BASE 0x48090000UL
#define PSTORE_SIZE 0xe0000UL
#define PMSG_SIZE 0x10000UL
#define PMSG_BASE (PSTORE_BASE + PSTORE_SIZE - PMSG_SIZE)
#define RING_HDR 12UL
#define PMSG_SIG 0x43474244UL

#define PAGE_16K 0x4000UL
#define BOOT_LINE_LENGTH 608
#define STOCK_KERNEL_PA 0x40280000UL
#define STOCK_IMAGE_OFFSET 0x200000UL

/* Must match tools/mkadt-nord2.py. */
#define NORD2_XNU_PHYS_BASE 0x142000000UL
#define NORD2_XNU_MEM_SIZE 0x3e000000UL

struct payload_seg {
    uint64_t vmaddr, vmsize, file_off, file_size;
};

struct payload {
    char magic[8];
    uint32_t version, nsegs;
    uint64_t virt_base, entry_va, adt_off, adt_size, kernel_end;
    uint64_t phys_base, mem_size;
    char cmdline[BOOT_LINE_LENGTH];
    struct payload_seg segs[];
};

struct boot_video {
    uint64_t v_baseAddr, v_display, v_rowBytes, v_width, v_height, v_depth;
};

struct boot_args {
    uint16_t Revision, Version;
    uint64_t virtBase, physBase, memSize, topOfKernelData;
    struct boot_video Video;
    uint32_t machineType;
    uint64_t deviceTreeP;
    uint32_t deviceTreeLength;
    char CommandLine[BOOT_LINE_LENGTH];
    uint64_t bootFlags, memSizeActual;
};

_Static_assert(__builtin_offsetof(struct payload, segs) == 0x2a8, "payload header layout");
_Static_assert(__builtin_offsetof(struct boot_args, Video) == 40, "boot_args layout");
_Static_assert(sizeof(struct boot_args) == 736, "boot_args layout");

struct ring {
    volatile uint32_t sig, start, size;
    volatile char data[];
};

static volatile struct ring *logbuf;
static unsigned logpos;
static uintptr_t g_dtb;

static void puts_log(const char *s)
{
    while (*s && logpos < PMSG_SIZE - RING_HDR) {
        logbuf->data[logpos++] = *s++;
    }
    __asm__ volatile("dsb sy" ::: "memory");
    logbuf->start = 0;
    logbuf->size = logpos;
    __asm__ volatile("dsb sy" ::: "memory");
}

/*
 * Leave the loader for the untouched stock Android kernel. The loader always
 * runs with the MMU and caches off, so this is the same state the verified
 * CPU probe used: x0 is the live FDT, x1..x3 are zero. Every failure path
 * ends here so that Android still boots and the pmsg log stays readable.
 */
static void chain_stock(void) __attribute__((noreturn));

static void
chain_stock(void)
{
    __asm__ volatile(
        "msr daifset, #15\n"
        "dsb sy\n"
        "ic iallu\n"
        "dsb sy\n"
        "isb\n"
        "mov x0, %0\n"
        "mov x1, xzr\n"
        "mov x2, xzr\n"
        "mov x3, xzr\n"
        "movz x9, #0\n"
        "movk x9, #0x4028, lsl #16\n"
        "br x9\n"
        : : "r"(g_dtb) : "x0", "x1", "x2", "x3", "x9", "memory");
    __builtin_unreachable();
}


static void hex_log(const char *name, uint64_t v)
{
    char b[19];
    b[0] = '0'; b[1] = 'x';
    for (int i = 0; i < 16; i++) {
        b[2 + i] = "0123456789abcdef"[(v >> (60 - i * 4)) & 15];
    }
    b[18] = 0;
    puts_log(name); puts_log("="); puts_log(b); puts_log("\n");
}

/* MMU off: everything is Device memory, so copy in aligned 8-byte words. */
static void copy64(uintptr_t dst, uintptr_t src, uint64_t n)
{
    volatile uint64_t *d = (volatile uint64_t *)dst;
    volatile const uint64_t *s = (volatile const uint64_t *)src;
    for (uint64_t i = 0; i < (n + 7) / 8; i++) {
        d[i] = s[i];
    }
}

static void zero64(uintptr_t dst, uint64_t n)
{
    volatile uint64_t *d = (volatile uint64_t *)dst;
    for (uint64_t i = 0; i < n / 8; i++) {
        d[i] = 0;
    }
}

static int same(const char *a, const char *b)
{
    while (*a && *a == *b) {
        a++; b++;
    }
    return *a == *b;
}

/* Compare an 8-byte magic that is not NUL-terminated (same() would read on). */
static int magic8(const char *a, const char *b)
{
    for (int i = 0; i < 8; i++) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

static uint32_t be32(const void *p)
{
    const volatile unsigned char *b = p;
    return (uint32_t)b[0] << 24 | (uint32_t)b[1] << 16 | (uint32_t)b[2] << 8 | b[3];
}

static int contains(const char *s, unsigned n, const char *wanted)
{
    for (unsigned i = 0; i < n; i++) {
        unsigned j = 0;
        while (wanted[j] && i + j < n && s[i + j] == wanted[j]) {
            j++;
        }
        if (!wanted[j]) {
            return 1;
        }
    }
    return 0;
}

/* Bounded FDT parsing; never log or export serial numbers / bootargs. */
static const unsigned char *prop(uintptr_t addr, const char *path,
    const char *key, uint32_t *length)
{
    const unsigned char *f = (const void *)addr;
    if (!addr || (addr & 3) || be32(f) != 0xd00dfeed) {
        return NULL;
    }
    uint32_t total = be32(f + 4), off = be32(f + 8), strings = be32(f + 12);
    uint32_t ss = be32(f + 32), ds = be32(f + 36);
    if (total < 40 || total > 0x200000 || off > total || ds > total - off ||
        strings > total || ss > total - strings) {
        return NULL;
    }
    uint32_t end = off + ds, lens[32];
    unsigned depth = 0, plen = 0;
    char current[256];
    current[0] = 0;
    while (off + 4 <= end) {
        uint32_t tok = be32(f + off);
        off += 4;
        if (tok == 1) {
            if (depth == 32) {
                return NULL;
            }
            lens[depth] = plen;
            if (depth) {
                if (plen + 1 >= sizeof(current)) {
                    return NULL;
                }
                current[plen++] = '/';
            }
            while (off < end && f[off]) {
                if (plen + 1 >= sizeof(current)) {
                    return NULL;
                }
                current[plen++] = f[off++];
            }
            if (off >= end) {
                return NULL;
            }
            off = (off + 4) & ~3U;
            current[plen] = 0;
            depth++;
        } else if (tok == 2) {
            if (!depth) {
                return NULL;
            }
            plen = lens[--depth];
            current[plen] = 0;
        } else if (tok == 3) {
            if (off + 8 > end) {
                return NULL;
            }
            uint32_t n = be32(f + off), no = be32(f + off + 4);
            off += 8;
            if (n > end - off || no >= ss) {
                return NULL;
            }
            uint32_t j = no;
            while (j < ss && f[strings + j]) {
                j++;
            }
            if (j == ss) {
                return NULL;
            }
            if (same(current, path) && same((const char *)f + strings + no, key)) {
                *length = n;
                return f + off;
            }
            off = (off + n + 3) & ~3U;
        } else if (tok != 4) {
            return NULL;
        }
    }
    return NULL;
}

static void fatal(const char *why)
{
    puts_log("NORD2-XNU: LOADER FATAL: ");
    puts_log(why);
    puts_log("\n");
    /* Never trap here: Android must boot so the log survives. */
    chain_stock();
}

void loader_exception(uint64_t slot)
{
    puts_log("NORD2-XNU: LOADER EXCEPTION\n");
    hex_log("slot", slot);
    uint64_t esr, elr, far;
    __asm__ volatile("mrs %0, esr_el1" : "=r"(esr));
    __asm__ volatile("mrs %0, elr_el1" : "=r"(elr));
    __asm__ volatile("mrs %0, far_el1" : "=r"(far));
    hex_log("esr", esr); hex_log("elr", elr); hex_log("far", far);
    chain_stock();
}

/* ---- ADT: fill in the /chosen chain-back properties ---- */

/* ADT integers are little-endian (unlike the FDT, which is big-endian). */
static uint32_t adt_le32(const unsigned char *p)
{
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 | (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

/*
 * The ADT generator emits /chosen with three reserved properties
 * (nord2-stock-entry, nord2-stock-dtb, nord2-xnu-region) holding zeros.
 * Only their values are written here.
 */
static void patch_chosen(unsigned char *adt, uint32_t size, uint64_t entry,
    uint64_t dtb, uint64_t region_start, uint64_t region_end)
{
    unsigned char *end = adt + size;
    unsigned char *p = adt;
    if (size < 16) {
        fatal("ADT truncated");
    }
    /* Root node starts at offset 0: {u32 nprops, u32 nchildren, props...}. */
    uint32_t nprops = adt_le32(p);
    p += 8;
    for (uint32_t i = 0; i < nprops; i++) {
        if (p + 36 > end) {
            fatal("ADT truncated");
        }
        uint32_t len = adt_le32(p + 32) & 0x7fffffff;
        p += 36 + ((len + 3) & ~3U);
    }
    /* /chosen is the first child node. */
    unsigned char *node = p;
    uint32_t cnprops = adt_le32(node);
    unsigned char *q = node + 8;
    int seen = 0;
    for (uint32_t i = 0; i < cnprops; i++) {
        if (q + 36 > end) {
            fatal("ADT truncated");
        }
        char *name = (char *)q;
        uint32_t len = adt_le32(q + 32) & 0x7fffffff;
        unsigned char *val = q + 36;
        if (val + len > end) {
            fatal("ADT truncated");
        }
        if (same(name, "nord2-stock-entry")) {
            if (len != 8) {
                fatal("ADT nord2-stock-entry size");
            }
            for (int b = 0; b < 8; b++) {
                val[b] = (unsigned char)(entry >> (b * 8));
            }
            seen++;
        } else if (same(name, "nord2-stock-dtb")) {
            if (len != 8) {
                fatal("ADT nord2-stock-dtb size");
            }
            for (int b = 0; b < 8; b++) {
                val[b] = (unsigned char)(dtb >> (b * 8));
            }
            seen++;
        } else if (same(name, "nord2-xnu-region")) {
            if (len != 16) {
                fatal("ADT nord2-xnu-region size");
            }
            for (int b = 0; b < 8; b++) {
                val[b] = (unsigned char)(region_start >> (b * 8));
                val[8 + b] = (unsigned char)(region_end >> (b * 8));
            }
            seen++;
        }
        q = val + ((len + 3) & ~3U);
    }
    if (seen != 3) {
        fatal("ADT missing chain-back properties");
    }
}

typedef void (*xnu_entry_t)(uint64_t boot_args_pa);

void xnu_main(uintptr_t dtb)
{
    uint32_t n = 0;
    const unsigned char *p;

    /* Bring the log up first: every later failure must be readable. */
    g_dtb = dtb;
    logbuf = (volatile struct ring *)PMSG_BASE;
    logpos = 0;
    logbuf->sig = PMSG_SIG;
    logbuf->start = 0;
    logbuf->size = 0;
    puts_log("NORD2-XNU LOADER: early probe, no storage writes\n");
    hex_log("dtb", dtb);

    p = prop(dtb, "", "compatible", &n);
    if (!p || !contains((const char *)p, n, "mediatek,MT6893")) {
        fatal("not MT6893");
    }
    p = prop(dtb, "/reserved-memory/mblock-16-pstore", "reg", &n);
    if (!p || n != 16 || be32(p) || be32(p + 4) != PSTORE_BASE ||
        be32(p + 8) || be32(p + 12) != PSTORE_SIZE) {
        fatal("unexpected pstore reservation");
    }
    p = prop(dtb, "/chosen", "bootargs", &n);
    if (!p || !contains((const char *)p, n, "ramoops.pmsg_size=0x10000")) {
        fatal("pmsg zone not 64K");
    }
    if (__builtin_arm_rsr64("sctlr_el1") & 5) {
        fatal("MMU or D-cache already on");
    }
    puts_log("NORD2-XNU LOADER: live FDT and cache state ok\n");

    /* Stock Android Image header: image_size at offset 16. */
    const unsigned char *img = (const unsigned char *)STOCK_KERNEL_PA;
    if (img[0x38] != 'A' || img[0x39] != 'R' || img[0x3a] != 'M' || img[0x3b] != 0x64) {
        fatal("stock Image magic missing");
    }
    uint64_t image_size = 0;
    for (int i = 7; i >= 0; i--) {
        image_size = (image_size << 8) | img[16 + i];
    }
    if (image_size < 0x100000 || image_size > 0x8000000) {
        fatal("implausible stock image_size");
    }
    uintptr_t payload_pa = (STOCK_KERNEL_PA + image_size + PAGE_16K - 1) & ~(PAGE_16K - 1);
    hex_log("stock_image_size", image_size);
    hex_log("payload", payload_pa);

    const struct payload *pl = (const struct payload *)payload_pa;
    if (!magic8(pl->magic, "SWEETXNU")) {
        fatal("no SWEETXNU payload after the stock kernel");
    }
    if (pl->version != 1 || pl->nsegs == 0 || pl->nsegs > 64 ||
        pl->phys_base != NORD2_XNU_PHYS_BASE ||
        pl->mem_size != NORD2_XNU_MEM_SIZE) {
        fatal("unexpected payload window");
    }
    /* The payload header + segments + ADT live contiguously after image_size. */
    uint64_t payload_limit = 0x10000000;
    if (pl->adt_off + pl->adt_size > payload_limit) {
        fatal("payload ADT out of range");
    }
    puts_log("NORD2-XNU LOADER: payload ok\n");
    hex_log("nsegs", pl->nsegs);
    hex_log("entry_va", pl->entry_va);

    uint64_t kernel_end_pa = pl->kernel_end - pl->virt_base + pl->phys_base;
    if (kernel_end_pa + pl->adt_size + 2 * PAGE_16K > NORD2_XNU_PHYS_BASE + NORD2_XNU_MEM_SIZE) {
        fatal("kernel does not fit the window");
    }
    if (pl->kernel_end <= pl->entry_va) {
        fatal("entry outside the kernel");
    }
    if (payload_pa + pl->adt_off + pl->adt_size > STOCK_KERNEL_PA + 0x6000000) {
        fatal("payload overlaps the ramdisk");
    }

    uint64_t delta = pl->phys_base - pl->virt_base;
#ifndef NORD2_LOADER_ONLY
    puts_log("NORD2-XNU LOADER: copying kernel\n");
    for (uint32_t i = 0; i < pl->nsegs; i++) {
        const struct payload_seg *s = &pl->segs[i];
        if (s->file_size > s->vmsize) {
            fatal("bad segment");
        }
        uintptr_t dst = (uintptr_t)(s->vmaddr + delta);
        if (dst < NORD2_XNU_PHYS_BASE ||
            dst + s->vmsize > NORD2_XNU_PHYS_BASE + NORD2_XNU_MEM_SIZE) {
            fatal("segment outside the window");
        }
        copy64(dst, payload_pa + s->file_off, s->file_size);
        if (s->vmsize > s->file_size) {
            zero64(dst + s->file_size, s->vmsize - s->file_size);
        }
    }
#else
    puts_log("NORD2-XNU LOADER: validation only, no segment copy\n");
#endif

    uintptr_t adt_pa = (uintptr_t)kernel_end_pa;
    copy64(adt_pa, payload_pa + pl->adt_off, pl->adt_size);
    /*
     * start.s maps from the 32 MiB block containing the mach header:
     *   blocks = (mem_size - (header_pa - phys_base)) >> 25
     * 32 MiB L2 block descriptors, so the identity map is
     * [phys_base, phys_base + blocks*32MiB). This is the only range that
     * stays mapped after the bootstrap tables are active; the chain-back
     * trampoline is copied to region_start + 16K.
     */
    uint64_t head_off = pl->segs[0].vmaddr - pl->virt_base;
    if (head_off >= pl->mem_size) {
        fatal("kernel header outside the window");
    }
    uint64_t region_start = pl->phys_base;
    uint64_t region_end = pl->phys_base +
        (((pl->mem_size - head_off) >> 25) << 25);
    patch_chosen((unsigned char *)adt_pa, (uint32_t)pl->adt_size,
        STOCK_KERNEL_PA, dtb, region_start, region_end);

    uintptr_t ba_pa = (adt_pa + pl->adt_size + PAGE_16K - 1) & ~(PAGE_16K - 1);
    uintptr_t top = ba_pa + PAGE_16K;
    zero64(ba_pa, PAGE_16K);
    volatile struct boot_args *ba = (volatile struct boot_args *)ba_pa;
    ba->Revision = 2;
    ba->Version = 2;
    ba->virtBase = pl->virt_base;
    ba->physBase = pl->phys_base;
    ba->memSize = pl->mem_size;
    ba->topOfKernelData = top;
    /* No framebuffer: the panel is DSI command mode. Text console stays off. */
    ba->Video.v_baseAddr = 0;
    ba->Video.v_display = 0;
    ba->Video.v_rowBytes = 0;
    ba->Video.v_width = 0;
    ba->Video.v_height = 0;
    ba->Video.v_depth = 0;
    ba->deviceTreeP = adt_pa - delta;	/* xnu reads it with the MMU on: a VA */
    ba->deviceTreeLength = (uint32_t)pl->adt_size;
    for (int i = 0; i < BOOT_LINE_LENGTH; i++) {
        ba->CommandLine[i] = pl->cmdline[i];
    }
    ba->memSizeActual = pl->mem_size;

    hex_log("kernel_end", kernel_end_pa);
    hex_log("adt", adt_pa);
    hex_log("boot_args", ba_pa);
    hex_log("entry", pl->entry_va + delta);
#if defined(NORD2_LOADER_ONLY) || defined(NORD2_COPY_ONLY)
    puts_log("NORD2-XNU LOADER: TEST MODE, chaining to stock (no XNU)\n");
    chain_stock();
#endif
    puts_log("NORD2-XNU LOADER: JUMP TO XNU\n");

    __asm__ volatile("dsb sy; ic iallu; dsb sy; isb" ::: "memory");
    ((xnu_entry_t)(uintptr_t)(pl->entry_va + delta))(ba_pa);
    fatal("XNU returned");
}
