#!/usr/bin/env python3
"""Exact active UFS strategy instrumentation with virtual media, no device I/O."""
from pathlib import Path
import re,subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1];s=(ROOT/'research/ci-39158169212/buildtree/xnu/bsd/dev/sweet_ufs.c').read_text()
a=s.index('static struct nord2_ufs_strategy_stats {');b=s.index('\nstatic struct nord2_ufs_link_stats',a);stats=s[a:b]
a=s.index('static void\nsweet_ufs_strategy(buf_t bp)');b=s.index('\nstatic int\nsweet_ufs_ioctl(',a);strategy=s[a:b]
for tag in ('STRATEGY','PHASES','CALLERS','SIZES'):
 literal=re.search(r'printf\("(NORD2 UFS '+tag+r':[^"\n]+)"',s)[1]
 worst=literal.replace('%llu','18446744073709551615').replace('\\n','\n')
 assert len(worst)<255,(tag,len(worst))
code=r'''
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define NORD2 1
#define B_READ 1u
#define B_RAW 0x200u
#define B_CLUSTER 0x40u
#define B_PAGEIO 0x80u
#define B_ASYNC 2u
#define UFS_BLOCK 4096u
#define BOUNCE_SIZE 1048576u
#define DMA_BOUNCE 16384u
#define MIN(a,b) ((a)<(b)?(a):(b))
typedef char *caddr_t;
struct buffer {uint64_t blkno;uint32_t count,flags,resid;int error,done;char *data;};
typedef struct buffer *buf_t;
static struct {uint64_t first_block,nblocks;uint32_t secsize;bool writable;uintptr_t dma;}su;
static int sweet_ufs_lock,held,map_error,read_error,reads,writes,maps,unmaps;
static uint64_t ticks,counter_reads;static bool nord2_ufs_profile_enabled;
static unsigned char dma[DMA_BOUNCE+BOUNCE_SIZE],disk[4*BOUNCE_SIZE];
static uint64_t nord2_ufs_counter(void){counter_reads++;return ticks++;}
static uint64_t buf_blkno(buf_t b){return b->blkno;}
static uint32_t buf_count(buf_t b){return b->count;}
static uint32_t buf_flags(buf_t b){return b->flags;}
static void buf_setresid(buf_t b,uint32_t n){b->resid=n;}
static void buf_seterror(buf_t b,int e){b->error=e;}
static void buf_biodone(buf_t b){assert(!held);b->done++;}
static int buf_map(buf_t b,caddr_t *p){assert(!held);maps++;ticks+=5;*p=b->data;return map_error;}
static void buf_unmap(buf_t b){(void)b;assert(!held);unmaps++;}
static void lck_mtx_lock(int *p){assert(p==&sweet_ufs_lock&&!held);held=1;ticks+=7;}
static void lck_mtx_unlock(int *p){assert(p==&sweet_ufs_lock&&held);held=0;}
static int ufs_read_blocks(uint64_t lba,uint32_t n){assert(held&&lba>=su.first_block&&n<=256);reads++;ticks+=50;
 if(read_error)return read_error;memcpy(dma+DMA_BOUNCE,disk+(lba-su.first_block)*4096,(size_t)n*4096);return 0;}
static int ufs_write_blocks(uint64_t lba,uint32_t n){assert(held&&lba>=su.first_block&&n<=256);writes++;ticks+=50;
 memcpy(disk+(lba-su.first_block)*4096,dma+DMA_BOUNCE,(size_t)n*4096);return 0;}
'''+stats+strategy+r'''
static void reset(void){memset(&nord2_ufs_strategy_stats,0,sizeof(nord2_ufs_strategy_stats));
 su.first_block=64;su.nblocks=1024;su.secsize=512;su.writable=true;su.dma=(uintptr_t)dma;
 held=map_error=read_error=reads=writes=maps=unmaps=0;ticks=counter_reads=0;nord2_ufs_profile_enabled=true;
 for(unsigned i=0;i<sizeof(disk);i++)disk[i]=(unsigned char)i;
}
int main(void){
 char *data=malloc(3*BOUNCE_SIZE);assert(data);struct buffer b;
 reset();b=(struct buffer){.blkno=0,.count=65536,.flags=B_READ|B_CLUSTER|B_PAGEIO|B_ASYNC,.data=data};
 sweet_ufs_strategy(&b);assert(!b.error&&!b.resid&&b.done==1&&reads==1&&!writes&&!memcmp(data,disk,b.count));
 assert(nord2_ufs_strategy_stats.buffers==1&&nord2_ufs_strategy_stats.read_buffers==1);
 assert(nord2_ufs_strategy_stats.requested_bytes==65536&&nord2_ufs_strategy_stats.sizes[4]==1);
 assert(nord2_ufs_strategy_stats.cluster_buffers==1&&nord2_ufs_strategy_stats.pageio_buffers==1&&nord2_ufs_strategy_stats.async_buffers==1);
 assert(nord2_ufs_strategy_stats.map_ticks==6&&nord2_ufs_strategy_stats.mutex_wait_ticks==8&&nord2_ufs_strategy_stats.copy_ticks==1);
 assert(nord2_ufs_strategy_stats.held_ticks>=52&&!held);
 // Larger-than-bounce read remains chunked, copied and completed correctly.
 reset();b=(struct buffer){.count=2*BOUNCE_SIZE+4096,.flags=B_READ|B_RAW,.data=data};
 sweet_ufs_strategy(&b);assert(reads==3&&!writes&&!b.resid&&!memcmp(data,disk,b.count));
 assert(nord2_ufs_strategy_stats.sizes[7]==1&&nord2_ufs_strategy_stats.copy_ticks==3&&nord2_ufs_strategy_stats.raw_buffers==1);
 // Partial write preserves both physical edge fragments and records RMW only.
 reset();memset(data,0xe5,7000);b=(struct buffer){.blkno=1,.count=7000,.flags=0,.data=data};
 sweet_ufs_strategy(&b);assert(reads==1&&writes==1&&!b.resid&&nord2_ufs_strategy_stats.rmw_reads==1);
 for(unsigned i=0;i<8192;i++)assert(disk[i]==(i>=512&&i<7512?0xe5:(unsigned char)i));
 // Disabled profiling adds no counter dependency or stats changes.
 reset();nord2_ufs_profile_enabled=false;b=(struct buffer){.count=4096,.flags=B_READ,.data=data};
 sweet_ufs_strategy(&b);assert(!counter_reads&&!nord2_ufs_strategy_stats.buffers&&!b.error&&reads==1);
 // Early rejection/map errors are intentionally outside mapped-buffer counts.
 reset();map_error=EIO;b=(struct buffer){.count=4096,.flags=B_READ,.data=data};sweet_ufs_strategy(&b);
 assert(b.error==EIO&&!nord2_ufs_strategy_stats.buffers&&!reads&&!unmaps);
 reset();read_error=EIO;b=(struct buffer){.count=4096,.flags=B_READ,.data=data};sweet_ufs_strategy(&b);
 assert(b.error==EIO&&b.resid==4096&&nord2_ufs_strategy_stats.buffers==1&&!nord2_ufs_strategy_stats.copy_ticks);
 free(data);return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='nord2-strategy-') as td:
 p=Path(td);c=p/'test.c';exe=p/'test';c.write_text(code)
 subprocess.run(['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',str(c),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
print('PASS: exact active strategy; real virtual-media read/multichunk/RMW edge preservation; timing/histogram/flags/errors/profile-off; bounded report lengths; ASan+UBSan')
