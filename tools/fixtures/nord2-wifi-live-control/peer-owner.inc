/* Actual retained NSS/IO guard composition. Only the MMIO model supplies fixed
 * public WTBL words; the real owner/probe/lease functions execute unchanged. */
static unsigned peer_token;
static struct nwl_snapshot peer_command(unsigned op,unsigned generation){
 struct nwl_snapshot out;assert(nwl_submit((op<<12)|++peer_token,generation));
 nwl_native_work();uint8_t b[256];assert(nwl_reply(b,sizeof b)==256);
 for(unsigned k=0;k<64;k++)out.word[k]=get32(b+4*k);
 assert(out.word[NWL_F_TOKEN]==peer_token&&!nst_native.probe&&!nst_native.state.active);return out;
}
static void peer_probe_actual(void){
 struct nst*s=&nst_native.state;assert(nwl_enabled()&&s->controlled_port&&s->ptk.installed&&s->gtk.installed&&!s->active);
 unsigned writes=mmio_writes,reads=peer_hw_reads;struct nst saved=*s;
 struct nwl_snapshot out=peer_command(NWL_PEER_PROBE,(unsigned)s->generation);
 assert(out.word[NWL_F_RESULT]==NWL_OK&&out.word[NWL_F_PEER_VALID]&&out.word[NWL_F_PEER_META]==0x502&&peer_hw_reads==reads+4);
 assert(mmio_writes==writes&&!memcmp(s,&saved,sizeof saved));
 /* A normal checked IO lease still refuses: probe is not an active pump. */
 assert(!nst_native_owner_window(&nwfn_native)&&!nst_native_probe_window(&nwfn_native));
 for(unsigned k=0;k<8;k++){
  unsigned active=nct_native.active;uint64_t generation=s->generation;
  if(k==0)s->wire_busy=1;if(k==1)s->air_pending=1;if(k==2)s->normal.published=1;if(k==3)s->callback_active=1;
  if(k==4)s->credit_available--;if(k==5)s->queue[0].next=s->queue[0].ring.count;if(k==6)nct_native.active=1;if(k==7)s->generation++;
  reads=peer_hw_reads;out=peer_command(NWL_PEER_PROBE,(unsigned)njs_native.native_generation);
  assert(out.word[NWL_F_RESULT]==NWL_REFUSED&&!out.word[NWL_F_PEER_VALID]&&peer_hw_reads==reads&&!nst_native.probe);
  *s=saved;nct_native.active=active;s->generation=generation;
 }
 peer_selector=1;peer_selector_reads=0;reads=peer_hw_reads;out=peer_command(NWL_PEER_PROBE,(unsigned)s->generation);
 assert(out.word[NWL_F_PEER_ERROR]==NWL_PEER_SELECTOR&&peer_hw_reads==reads+1);peer_selector=0;
 peer_changed=1;peer_selector_reads=0;reads=peer_hw_reads;out=peer_command(NWL_PEER_PROBE,(unsigned)s->generation);
 assert(out.word[NWL_F_PEER_ERROR]==NWL_PEER_CHANGED&&peer_hw_reads==reads+4);peer_changed=0;
 /* Unknown/stale commands cannot map-read; invalid slots cannot expose any
  * arbitrary word. Wrong physical owner must reach unchanged common guard. */
 reads=peer_hw_reads;out=peer_command(NWL_PEER_PROBE,(unsigned)s->generation+1);assert(out.word[NWL_F_RESULT]==NWL_STALE&&peer_hw_reads==reads);
 assert(!nwl_submit((11u<<12)|(peer_token+1),(unsigned)s->generation)&&peer_hw_reads==reads);
 out=peer_command(NWL_DATA_TODS,(unsigned)s->generation);assert(out.word[NWL_F_RESULT]==NWL_REFUSED&&!nst_native_next_data_profile());
 unsigned old_halt=nwfn_native.halt,old_error=nwfn_native.error,old_init=nwfn_native.init.halt,old_transport=nwfn_native.init.transport.halt;
 gic_active=1;reads=peer_hw_reads;out=peer_command(NWL_PEER_PROBE,(unsigned)s->generation);
 assert(out.word[NWL_F_RESULT]==NWL_REFUSED&&out.word[NWL_F_PEER_ERROR]==NWL_PEER_OWNER&&peer_hw_reads==reads&&!nst_native.probe&&nwfn_native.halt);
 /* Explicit fault-fixture reset only; production never clears fatal state. */
 gic_active=0;nwfn_native.halt=old_halt;nwfn_native.error=old_error;nwfn_native.init.halt=old_init;nwfn_native.init.transport.halt=old_transport;
 assert(mmio_writes==writes&&!memcmp(s,&saved,sizeof saved));
}
