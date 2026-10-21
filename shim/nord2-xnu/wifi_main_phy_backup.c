#include "wifi_main_phy_backup.h"
#define NFA_BACKUP_CHUNK 4096u
#define NFA_CONN_BYTES 0x450000u
static int fail(struct nfa_backup_state *s,int e){s->error=e;s->halt=1;return e;}
int nfa_backup(const struct nfa_backup_io *io,void *ctx,
 const struct nfa_result *result,void *dst,uint32_t capacity,
 struct nfa_backup_state *s){
 if(!io||!io->owner||!io->read_emi||!io->barrier||!result||!dst||!s||
    s->attempted||s->halt||!result->valid||!result->force_success||
    !result->backup_needed||!result->emi_bytes||
    result->emi_offset>=NFA_CONN_BYTES||
    result->emi_bytes>NFA_CONN_BYTES-result->emi_offset||
    result->emi_bytes>capacity)return -1;
 if(io->owner(ctx))return fail(s,-2);
 s->attempted=1;s->emi_offset=result->emi_offset;
 s->emi_bytes=result->emi_bytes;
 for(uint32_t off=0;off<result->emi_bytes;){
  uint32_t n=result->emi_bytes-off;
  if(n>NFA_BACKUP_CHUNK)n=NFA_BACKUP_CHUNK;
  if(io->owner(ctx))return fail(s,-2);
  if(io->read_emi(ctx,result->emi_offset+off,(uint8_t *)dst+off,n))
   return fail(s,-3);
  off+=n;s->bytes_copied=off;s->reads++;
 }
 if(io->owner(ctx)||io->barrier(ctx))return fail(s,-4);
 s->valid=1;return 0;
}
