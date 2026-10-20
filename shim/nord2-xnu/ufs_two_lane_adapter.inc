/* Opt-in stop-only adapter, included after ufs_read_blocks. Never publishes root. */
struct nord2_x2_context { uint64_t first,blocks,dma_pa; vm_offset_t dma; uint32_t valid,fail;uint64_t values[24];unsigned observing; };
static int nord2_x2_owned(void *opaque)
{
 struct nord2_x2_context *c=opaque;
 lck_mtx_assert(&sweet_ufs_lock,LCK_MTX_ASSERT_OWNED);
 uint64_t v;
#define OWN(index,expr,pass) do {v=(expr);if(c->observing){c->values[index]=v;c->valid|=1u<<(index);}if(!(pass)){if(c->observing)c->fail=(index)+1;return 0;}}while(0)
 OWN(0,nord2_x2_in_progress,v!=0);
 OWN(1,nord2_x2_stop,v!=0);
 OWN(2,nord2_x2_uncertain,v==0);
 OWN(3,nord2_root_owned,v!=0);
 OWN(4,su.ready,v==0);
 OWN(5,su.writable,v==0);
 OWN(6,su.probe_only,v==0);
 OWN(7,su.lun,v==2);
 OWN(8,su.bdev,v==0);
 OWN(9,su.bmajor,v==0);
 OWN(10,su.cmajor,v==0);
 OWN(11,su.regs,v!=0);
 OWN(12,su.dma,v==c->dma);
 OWN(13,su.dma_pa,v==c->dma_pa);
 OWN(14,su.first_block,v==c->first);
 OWN(15,su.nblocks,v==c->blocks);
 OWN(16,nord2_ufs_uic_pending,v==0);
 OWN(17,ml_get_interrupts_enabled(),v!=0);
 OWN(18,ml_at_interrupt_context(),v==0);
 OWN(19,get_preemption_level(),v==0);
 OWN(20,ufs_rd(REG_UTRLBA),v==(uint32_t)(su.dma_pa+DMA_UTRL));
 OWN(21,ufs_rd(REG_UTRLBAU),v==(uint32_t)((su.dma_pa+DMA_UTRL)>>32));
 OWN(22,ufs_rd(REG_UTRLRSR),(v&1)!=0);
#undef OWN
 c->observing=0;
 return 1;
}
static uint32_t nord2_x2_rd(void *opaque,uint32_t off)
{ (void)opaque; return ufs_rd(off); }
static void nord2_x2_wr(void *opaque,uint32_t off,uint32_t value)
{
 (void)opaque;
 /* Fail closed if future engine changes introduce any non-UIC write. */
 if(off!=REG_IS && off!=N2_UIC_CMD && off!=N2_UIC_ARG1 &&
  off!=N2_UIC_ARG2 && off!=N2_UIC_ARG3){nord2_x2_uncertain=true;return;}
 if(off==REG_IS && value!=N2_UIC_COMPLETE && value!=(N2_UIC_COMPLETE|0x10)) {
  nord2_x2_uncertain=true;return;
 }
 if(off==N2_UIC_CMD) {
  if(value!=1 && value!=2){nord2_x2_uncertain=true;return;}
  nord2_ufs_uic_pending=true;
 }
 ufs_wr(off,value);
 if(off==REG_IS && (value&N2_UIC_COMPLETE))nord2_ufs_uic_pending=false;
}
static uint64_t nord2_x2_ticks(void *opaque)
{ (void)opaque;return __builtin_arm_rsr64("CNTVCT_EL0"); }
static void nord2_x2_delay(void *opaque,unsigned usec)
{ (void)opaque;delay(usec); }
static void nord2_x2_barrier(void *opaque)
{ (void)opaque;__builtin_arm_dsb(0xf); }
static int nord2_x2_digest(void *opaque,uint64_t lba,uint32_t blocks,unsigned char hash[32])
{
 if(lba!=1 || blocks!=5 || !nord2_x2_owned(opaque))return N2L_READ_UNCERTAIN;
 int error=ufs_read_blocks(1,5);
 if(error) {
  if(error==ETIMEDOUT || nord2_x2_uncertain)return N2L_READ_UNCERTAIN;
  return N2L_READ_COMPLETED_ERROR;
 }
 SHA256_CTX ctx;SHA256_Init(&ctx);
 SHA256_Update(&ctx,(void *)(su.dma+DMA_BOUNCE),5u*UFS_BLOCK);
 SHA256_Final(hash,&ctx);
 return 0;
}
static int nord2_x2_run(struct n2l_result *result)
{
 static const unsigned char expected[32]={
  0xe0,0x1c,0x10,0x3f,0x7d,0x3c,0x63,0x99,0x36,0x4e,0x72,0x30,0x4b,0x41,0xf9,0x9b,
  0xf2,0x4b,0x51,0x7b,0x19,0x90,0xf1,0xba,0x86,0x23,0xad,0x18,0x44,0x72,0xbc,0x86};
 lck_mtx_assert(&sweet_ufs_lock,LCK_MTX_ASSERT_OWNED);
 struct nord2_x2_context c={su.first_block,su.nblocks,su.dma_pa,su.dma,0,0,{0},1};
 const struct n2l_ops ops={nord2_x2_rd,nord2_x2_wr,nord2_x2_ticks,nord2_x2_delay,
  nord2_x2_barrier,nord2_x2_owned,nord2_x2_digest,expected,
  __builtin_arm_rsr64("CNTFRQ_EL0")};
 nord2_x2_in_progress=true;
 int error=n2l_diagnostic(&ops,&c,1,result);
 result->owned_valid=c.valid;result->owned_fail=c.fail;
 for(unsigned i=0;i<24;i++)result->owned_values[i]=c.values[i];
 if(result->uncertain)nord2_x2_uncertain=true;
 nord2_x2_in_progress=false;
 return error;
}
static void nord2_x2_halt(int error,const struct n2l_result *r) __attribute__((noreturn));
static void nord2_x2_halt(int error,const struct n2l_result *r)
{
 printf("NORD2 UFS X2 STOP: rc=%d phase=%u error=%d commands=%u sets=%u reads=%u\n",
  error,r->phase,r->error,r->commands,r->sets,r->reads);
 printf("NORD2 UFS X2 STATE: uncertain=%d rollback=%d original=%d engine_root=%d engine_handoff=%d\n",
  r->uncertain,r->rollback_attempted,r->original_verified,r->root_allowed,r->handoff_allowed);
 printf("NORD2 UFS X2 LAST: attr=%#x result=%#x is=%#x hcs=%#x root_published=0 return_blocked=1\n",
  r->last_attr,r->last_result,r->last_is,r->last_hcs);
 printf("NORD2 UFS X2 SNAP A: valid=%#x rx_gear=%u rx_lanes=%u rx_term=%u tx_gear=%u tx_lanes=%u tx_term=%u\n",
  r->snapshot_mask,r->snapshot[0],r->snapshot[1],r->snapshot[2],r->snapshot[3],r->snapshot[4],r->snapshot[5]);
 printf("NORD2 UFS X2 SNAP B: hs=%u power=%#x adapt=%#x pa_timeout0=%u pa_timeout1=%u pa_timeout2=%u\n",
  r->snapshot[6],r->snapshot[7],r->snapshot[8],r->snapshot[9],r->snapshot[10],r->snapshot[11]);
 printf("NORD2 UFS X2 SNAP C: pa_timeout3=%u pa_timeout4=%u pa_timeout5=%u local_fc0=%u local_tc0=%u local_afc0=%u\n",
  r->snapshot[12],r->snapshot[13],r->snapshot[14],r->snapshot[15],r->snapshot[16],r->snapshot[17]);
 printf("NORD2 UFS X2 LIMITS: valid=%#x rx_available=%u tx_available=%u rx_connected=%u tx_connected=%u version=%#x max_rx_gear=%u\n",
  r->limits_mask,r->limits[0],r->limits[1],r->limits[2],r->limits[3],r->limits[4],r->limits[5]);
 printf("NORD2 UFS X2 PREFLIGHT: valid=%#x fail=%u owned_valid=%#x owned_fail=%u\n",r->preflight_valid,r->preflight_fail,r->owned_valid,r->owned_fail);
 printf("NORD2 UFS X2 GATE 0: v0=%#x v1=%#x v2=%#x v3=%#x\n",r->preflight[0],r->preflight[1],r->preflight[2],r->preflight[3]);
 printf("NORD2 UFS X2 GATE 4: v4=%#x v5=%#x v6=%#x v7=%#x\n",r->preflight[4],r->preflight[5],r->preflight[6],r->preflight[7]);
 printf("NORD2 UFS X2 GATE 8: v8=%#x v9=%#x v10=%#x v11=%#x\n",r->preflight[8],r->preflight[9],r->preflight[10],r->preflight[11]);
 printf("NORD2 UFS X2 GATE 12: v12=%#x v13=%#x\n",r->preflight[12],r->preflight[13]);
 printf("NORD2 UFS X2 OWN 0: v0=%#llx v1=%#llx v2=%#llx\n",(unsigned long long)r->owned_values[0],(unsigned long long)r->owned_values[1],(unsigned long long)r->owned_values[2]);
 printf("NORD2 UFS X2 OWN 3: v3=%#llx v4=%#llx v5=%#llx\n",(unsigned long long)r->owned_values[3],(unsigned long long)r->owned_values[4],(unsigned long long)r->owned_values[5]);
 printf("NORD2 UFS X2 OWN 6: v6=%#llx v7=%#llx v8=%#llx\n",(unsigned long long)r->owned_values[6],(unsigned long long)r->owned_values[7],(unsigned long long)r->owned_values[8]);
 printf("NORD2 UFS X2 OWN 9: v9=%#llx v10=%#llx v11=%#llx\n",(unsigned long long)r->owned_values[9],(unsigned long long)r->owned_values[10],(unsigned long long)r->owned_values[11]);
 printf("NORD2 UFS X2 OWN 12: v12=%#llx v13=%#llx v14=%#llx\n",(unsigned long long)r->owned_values[12],(unsigned long long)r->owned_values[13],(unsigned long long)r->owned_values[14]);
 printf("NORD2 UFS X2 OWN 15: v15=%#llx v16=%#llx v17=%#llx\n",(unsigned long long)r->owned_values[15],(unsigned long long)r->owned_values[16],(unsigned long long)r->owned_values[17]);
 printf("NORD2 UFS X2 OWN 18: v18=%#llx v19=%#llx v20=%#llx\n",(unsigned long long)r->owned_values[18],(unsigned long long)r->owned_values[19],(unsigned long long)r->owned_values[20]);
 printf("NORD2 UFS X2 OWN 21: v21=%#llx v22=%#llx\n",(unsigned long long)r->owned_values[21],(unsigned long long)r->owned_values[22]);
 /* Existing timer/watchdog path remains active with interrupts enabled.
  * No repeated output, raw watchdog access or automatic reboot/return. */
 for(;;){delay(1000000);}
}
