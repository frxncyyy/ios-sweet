/* SPDX-License-Identifier: MIT
 * Runs the ACTUAL installer control flow with an in-memory fixed-path backend.
 * Does not claim kernel vnode/HFS hardware coverage. Faults at every mutation.
 */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <setjmp.h>
#include <openssl/sha.h>
#include "audio_package.h"
enum path { A,B,S,SD,SE,SP,AD,AE,AP,V,D,J,C,R,P,VP,DP,TUN,PATH_COUNT };
#define VDIR 2
#define kSuccess 0
static int quiet_printf(const char *format,...){(void)format;return 0;}
#define printf quiet_printf
typedef int DTEntry;
struct identity {uint64_t ino;uint32_t fsid,type;};
struct journal {unsigned char magic[8];uint32_t version,disabled;struct identity original,custom,virtual;uint32_t executable_size,plist_size;unsigned char executable_hash[32],plist_hash[32],original_hash[32],digest[32];};
static struct {unsigned node[PATH_COUNT];struct journal journal;unsigned char commit[32],rollback[32];} disk;
static int ctx,bootmode,mutations,fault,crash,after;
static jmp_buf jump;
static unsigned char wire[98];
static void hash(const void *p,unsigned n,unsigned char o[32]){SHA256(p,n,o);}
static int check_hash(const unsigned char *p,unsigned n,const unsigned char h[32]){unsigned char o[32];hash(p,n,o);return memcmp(o,h,32)?EINVAL:0;}
static int mode(void){return bootmode;}
static int vfs_context_kernel(void){return 0;}
static int SecureDTLookupEntry(void *a,const char *b,DTEntry *c){(void)a;(void)b;*c=1;return 0;}
static int SecureDTGetProperty(DTEntry a,const char *b,const void **p,unsigned *n){(void)a;(void)b;*p=wire;*n=sizeof(wire);return 0;}
static int before(void){mutations++;if(fault==mutations&&!after){if(crash)longjmp(jump,1);return EIO;}return 0;}
static int done(void){if(fault==mutations&&after){if(crash)longjmp(jump,1);return EIO;}return 0;}
static int absent(enum path p){return disk.node[p]?EEXIST:0;}
static int directory(enum path p){return disk.node[p]?0:ENOENT;}
static int identity(enum path p,struct identity *o){if(!disk.node[p])return ENOENT;*o=(struct identity){disk.node[p],1,VDIR};return 0;}
static int same(enum path p,const struct identity *o){struct identity t;int e=identity(p,&t);return e?e:(memcmp(&t,o,sizeof(t))?EINVAL:0);}
static int sync_path(enum path p){(void)p;int e=before();return e?e:done();}
static int mkdir_fixed(enum path p){int e=before();if(e)return e;if(disk.node[p])return EEXIST;disk.node[p]=p==S?2:8;return done();}
static int create_file(enum path p,const void *b,unsigned n,unsigned m){(void)m;int e=before();if(e)return e;if(disk.node[p])return EEXIST;disk.node[p]=9;if(p==J){assert(n==sizeof(disk.journal));memcpy(&disk.journal,b,n);}if(p==C)memcpy(disk.commit,b,32);if(p==R)memcpy(disk.rollback,b,32);return done();}
static int move(enum path f,enum path t,const struct identity *expected){int e=before();if(e)return e;if((e=same(f,expected)))return e;if(disk.node[t])return EEXIST;disk.node[t]=disk.node[f];disk.node[f]=0;return done();}
static int custom(const struct journal *j,int active){int e=same(active?A:S,&j->custom);return e?e:((disk.node[SE]&&disk.node[SP]&&disk.node[SD])?0:EINVAL);}
static void seal(struct journal *j){hash(&j->original,sizeof(j->original)+sizeof(j->custom)+sizeof(j->virtual),j->original_hash);hash(j,offsetof(struct journal,digest),j->digest);}
static int journal_read(struct journal *j){if(!disk.node[J])return ENOENT;*j=disk.journal;return check_hash((void *)j,offsetof(struct journal,digest),j->digest);}
static int marker(enum path p,const struct journal *j){if(!disk.node[p])return ENOENT;return memcmp(p==C?disk.commit:disk.rollback,j->digest,32)?EINVAL:0;}
static int baseline_original(void){return disk.node[A]==1&&disk.node[D]==3&&!disk.node[V]&&!disk.node[B]&&!disk.node[S]&&!disk.node[C]&&!disk.node[R]?0:EINVAL;}
/* Same recovery transition function as production; shared by source extraction below. */
#include "audio_install_restore.inc"
#include "audio_install_flow.inc"
#undef printf
static void initial(int disabled){memset(&disk,0,sizeof(disk));disk.node[A]=1;disk.node[disabled?D:V]=3;disk.node[P]=disk.node[VP]=disk.node[DP]=disk.node[TUN]=8;mutations=0;fault=0;bootmode=1;}
static int original(int disabled){return disk.node[A]==1&&!disk.node[B]&&disk.node[disabled?D:V]==3&&!disk.node[disabled?V:D];}
static int installed(void){return disk.node[A]==2&&disk.node[B]==1&&disk.node[V]==3&&!disk.node[D]&&disk.node[C];}
int main(void){
 memcpy(wire,"N2AP",4);wire[4]=1;wire[8]=sizeof(wire);wire[12]=2;wire[16]=wire[20]=1;wire[96]=7;wire[97]=8;
 hash(wire+96,1,wire+32);hash(wire+97,1,wire+64);
 unsigned cases=0;
 for(volatile int disabled=0;disabled<2;disabled++){
 initial(disabled);bootmode=2;if(disabled)assert(!nord2_audio_install());else assert(nord2_audio_install());assert(original(disabled));bootmode=1;
 assert(!nord2_audio_install());assert(installed());volatile int install_ops=mutations;
 fault=0;mutations=0;assert(!nord2_audio_install());assert(installed()); /* idempotent */
 bootmode=2;mutations=0;assert(!nord2_audio_install());assert(original(disabled));volatile int rollback_ops=mutations;
 assert(!nord2_audio_install());assert(original(disabled));
 for(volatile int operation=1;operation<=install_ops;operation++)for(volatile int a=0;a<2;a++)for(volatile int c=0;c<2;c++){
 initial(disabled);fault=operation;after=a;crash=c;
 if(!setjmp(jump))(void)nord2_audio_install();
 fault=0;mutations=0;bootmode=0;assert(!nord2_audio_install());
 assert(original(disabled)||installed());cases++;
 }
 for(volatile int operation=1;operation<=rollback_ops;operation++)for(volatile int a=0;a<2;a++)for(volatile int c=0;c<2;c++){
 initial(disabled);assert(!nord2_audio_install());fault=operation;mutations=0;after=a;crash=c;bootmode=2;
 if(!setjmp(jump))(void)nord2_audio_install();
 fault=0;mutations=0;bootmode=2;assert(!nord2_audio_install());assert(original(disabled));cases++;
 }
 /* Unknown backup/stage/marker or both VirtualAudio locations refuse overwrite. */
 enum path collision[]={B,S,C,R};
 for(unsigned i=0;i<sizeof(collision)/sizeof(collision[0]);i++){initial(disabled);disk.node[collision[i]]=77;assert(nord2_audio_install());assert(disk.node[A]==1&&disk.node[disabled?D:V]==3&&!disk.node[disabled?V:D]);assert(disk.node[collision[i]]==77);}
 initial(disabled);disk.node[disabled?V:D]=77;assert(nord2_audio_install());assert(disk.node[A]==1&&!disk.node[B]);
 /* Repair known rollback layouts even if the commit marker remains valid and
  * rollback-intent marker was lost or could not be written. */
 initial(disabled);assert(!nord2_audio_install());
 disk.node[S]=disk.node[A];disk.node[A]=0;bootmode=0;
 assert(!nord2_audio_install());assert(original(disabled));
 if(disabled){initial(disabled);assert(!nord2_audio_install());
 disk.node[D]=disk.node[V];disk.node[V]=0;bootmode=0;
 assert(!nord2_audio_install());assert(original(disabled));}
 /* An invalid/missing commit means recover original rather than load custom. */
 initial(disabled);assert(!nord2_audio_install());disk.commit[0]^=1;bootmode=0;
 assert(!nord2_audio_install());assert(original(disabled));
 /* Committed payload damage is refused; original and backup are preserved. */
 initial(disabled);assert(!nord2_audio_install());disk.node[SE]=0;bootmode=0;assert(nord2_audio_install());assert(disk.node[A]==2&&disk.node[B]==1);
 }
 assert(ctx==0);
 printf("PASS real install-flow fault model: %u injected failure/crash scenarios plus idempotence/collisions/damage\n",cases);
}
