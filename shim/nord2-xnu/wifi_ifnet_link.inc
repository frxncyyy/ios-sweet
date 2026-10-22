/* Real BSD Ethernet endpoint for the single retained radio owner.
 * The gate linearizes admission, bind/unbind and generation changes. It is a
 * kernel-lifetime lock; no callback can race a freed lock after detach.
 * No IOKit client or mbuf is held by the native TX queue. */
static IOLock *nwil_lock;
static struct {
 ifnet_t net;
 uint64_t generation;
 bool authorized;
 unsigned transitions,rx_logs,tx_logs;
} nwil;

static bool nwil_init_lock(void) {
 if (__atomic_load_n(&nwil_lock,__ATOMIC_ACQUIRE)) return true;
 IOLock *fresh=IOLockAlloc();
 if (!fresh) return false;
 IOLock *expected=nullptr;
 if (!__atomic_compare_exchange_n(&nwil_lock,&expected,fresh,false,
                                  __ATOMIC_RELEASE,__ATOMIC_ACQUIRE))
  IOLockFree(fresh);
 return true;
}

static int nwil_bind(ifnet_t net) {
 if (!net || !nwil_init_lock()) return ENOMEM;
 IOLockLock(nwil_lock);
 int error=nwil.net ? EBUSY : ifnet_reference(net);
 if (!error) { nwil.net=net; nwil.generation=0; nwil.authorized=false; }
 IOLockUnlock(nwil_lock);
 return error;
}

static void nwil_unbind(ifnet_t net) {
 IOLock *gate=__atomic_load_n(&nwil_lock,__ATOMIC_ACQUIRE);
 if (!gate) return;
 IOLockLock(gate);
 bool owned=nwil.net==net;
 if (owned) { nwil.net=nullptr; nwil.authorized=false; nwil.generation=0; }
 IOLockUnlock(gate);
 if (owned) { ifnet_set_flags(net,0,IFF_RUNNING); ifnet_release(net); }
}

static bool nwil_active(ifnet_t net) {
 IOLock *gate=__atomic_load_n(&nwil_lock,__ATOMIC_ACQUIRE);
 if (!gate) return false;
 IOLockLock(gate);
 bool active=nwil.net==net && nwil.authorized;
 IOLockUnlock(gate);
 return active;
}

static int nwil_event(ifnet_t net,bool up) {
 /* ifnet_event posts from event_data, not sizeof(kern_event_msg): the public
  * header includes the first payload word. The callback runs without our gate. */
 struct link_data { uint32_t family,unit; char name[IFNAMSIZ]; };
 alignas(struct kern_event_msg) uint8_t bytes[KEV_MSG_HEADER_SIZE+sizeof(link_data)]={};
 static_assert(sizeof(bytes)>=sizeof(struct kern_event_msg),"complete event header");
 auto *header=(struct kern_event_msg *)(void *)bytes;
 auto *data=(link_data *)(void *)(bytes+KEV_MSG_HEADER_SIZE);
 header->total_size=sizeof(bytes);
 header->vendor_code=KEV_VENDOR_APPLE;
 header->kev_class=KEV_NETWORK_CLASS;
 header->kev_subclass=KEV_DL_SUBCLASS;
 header->event_code=up?KEV_DL_LINK_ON:KEV_DL_LINK_OFF;
 data->family=ifnet_family(net);data->unit=ifnet_unit(net);
 strlcpy(data->name,ifnet_name(net),sizeof(data->name));
 return ifnet_event(net,header);
}

extern "C" int nord2_wifi_ifnet_link(uint64_t generation,int authorized) {
 if (!generation || (authorized!=0 && authorized!=1)) return EINVAL;
 IOLock *gate=__atomic_load_n(&nwil_lock,__ATOMIC_ACQUIRE);
 if (!gate) return ENXIO;
 IOLockLock(gate);
 ifnet_t net=nwil.net;
 if (!net) { IOLockUnlock(gate); return ENXIO; }
 if (generation<nwil.generation) { IOLockUnlock(gate); return ESTALE; }
 if (authorized && nwil.authorized && generation!=nwil.generation) {
  IOLockUnlock(gate); return EBUSY; /* first revoke the previous peer */
 }
 int error=ifnet_reference(net);
 if (error) { IOLockUnlock(gate); return error; }
 bool changed=nwil.authorized!=(authorized!=0);
 error=ifnet_set_flags(net,authorized?IFF_RUNNING:0,IFF_RUNNING);
 if (!error) { nwil.generation=generation; nwil.authorized=authorized!=0; }
 if (nwil.transitions<32) {
  nwil.transitions++;
  printf("NORD2 WIFI IFNET LINK: generation=%llu authorized=%u changed=%u error=%d\n",
         (unsigned long long)generation,(unsigned)authorized,(unsigned)changed,error);
 }
 IOLockUnlock(gate);
 /* Protocol event callbacks may generate output; never hold our gate here. */
 if (!error && changed) error=nwil_event(net,authorized!=0);
 if (error && authorized) {
  /* A failed up notification is not a successful link transition. Revoke
   * locally before reporting the failure to the owner, which closes its
   * native controlled port and retires any queued transmissions. */
  IOLockLock(gate);
  bool rollback=nwil.net==net && nwil.generation==generation && nwil.authorized;
  if (rollback) { nwil.authorized=false; ifnet_set_flags(net,0,IFF_RUNNING); }
  IOLockUnlock(gate);
  if (rollback) (void)nwil_event(net,false);
 }
 ifnet_release(net);
 return error;
}

static bool nwil_ethernet(const uint8_t *frame,size_t length) {
 if (!frame || length<14 || length>NWIL_ETHERNET_MAX) return false;
 unsigned type=(unsigned)frame[12]*256u+frame[13];
 /* EAPOL belongs exclusively to the owner's authenticator. No VLAN or
  * 802.3-length/SNAP emulation is advertised by this first data path. */
 return type>=0x600 && type!=0x888e && type!=0x8100 && type!=0x88a8;
}

static errno_t nwil_ioctl(ifnet_t net,u_long command,void *data) {
 /* IFF_UP is administrative; only the native controlled port controls
  * IFF_RUNNING. No flags ioctl may manufacture an authenticated carrier. */
 if (command==SIOCSIFFLAGS) return 0;
 if (command==SIOCSIFADDR) return ifnet_set_flags(net,IFF_UP,IFF_UP);
 /* Native station activation admits directed, broadcast and all multicast
  * RX (source filter 0x0d). BSD keeps protocol multicast memberships. */
 if (command==SIOCADDMULTI || command==SIOCDELMULTI) return 0;
 if (!data) return EINVAL;
 if (command==SIOCSIFMTU) {
  int mtu=((struct ifreq *)data)->ifr_mtu;
  return mtu<(int)IF_MINMTU || mtu>1500 ? EINVAL : ifnet_set_mtu(net,(unsigned)mtu);
 }
 if (command==SIOCGIFDEVMTU) {
  struct ifreq *request=(struct ifreq *)data;
  request->ifr_devmtu.ifdm_current=ifnet_mtu(net);
  request->ifr_devmtu.ifdm_min=IF_MINMTU;
  request->ifr_devmtu.ifdm_max=1500;
  return 0;
 }
 if (command!=SIOCGIFMEDIA) return EOPNOTSUPP;
 struct ifmediareq *request=(struct ifmediareq *)data;
 int requested=request->ifm_count,media=IFM_IEEE80211|IFM_AUTO;
 request->ifm_current=media;request->ifm_active=media;
 request->ifm_status=IFM_AVALID|(nwil_active(net)?IFM_ACTIVE:0);
 request->ifm_mask=0;request->ifm_count=1;
 if (requested<0) return EINVAL;
 if (requested>0 && request->ifm_ulist)
  return copyout(&media,(user_addr_t)request->ifm_ulist,sizeof(media));
 return 0;
}

static errno_t nwil_output(ifnet_t net,mbuf_t packet) {
 if (!packet) return EINVAL;
 if (mbuf_nextpkt(packet)) { mbuf_freem_list(packet); return EINVAL; }
 uint64_t generation=0;
 IOLock *gate=__atomic_load_n(&nwil_lock,__ATOMIC_ACQUIRE);
 if (gate) {
  IOLockLock(gate);
  if (nwil.net==net && nwil.authorized) generation=nwil.generation;
  IOLockUnlock(gate);
 }
 if (!generation) { mbuf_freem(packet); return ENETDOWN; }
 size_t length=(mbuf_flags(packet)&MBUF_PKTHDR)?mbuf_pkthdr_len(packet):0;
 uint8_t frame[NWIL_ETHERNET_MAX];
 errno_t error=EINVAL;
 if (length>=14 && length<=sizeof(frame) && !mbuf_copydata(packet,0,length,frame)
     && nwil_ethernet(frame,length)) {
  int result=nord2_wifi_link_send(frame,length,generation);
  error=result==NWIL_SEND_ACCEPTED?0:result==NWIL_SEND_BUSY?ENOBUFS:
        result==NWIL_SEND_NOT_READY?ENETDOWN:EINVAL;
  if (!error) {
   bpf_tap_out(net,DLT_EN10MB,packet,nullptr,0);
   ifnet_stat_increment_out(net,1,(uint32_t)length,0);
  }
 }
 mbuf_freem(packet);
 return error;
}

extern "C" int nord2_wifi_ifnet_receive(uint64_t generation,
                                       const uint8_t *frame,size_t length) {
 if (!generation || !nwil_ethernet(frame,length)) return EINVAL;
 IOLock *gate=__atomic_load_n(&nwil_lock,__ATOMIC_ACQUIRE);
 if (!gate) return ENXIO;
 IOLockLock(gate);
 ifnet_t net=nwil.net;
 int error=(!net || !nwil.authorized || generation!=nwil.generation)?ENETDOWN:
           ifnet_reference(net);
 IOLockUnlock(gate);
 if (error) return error;
 mbuf_t packet=nullptr;
 error=mbuf_getpacket(MBUF_DONTWAIT,&packet);
 if (!error && mbuf_maxlen(packet)<length) error=ENOBUFS;
 if (error) { if(packet)mbuf_freem(packet); ifnet_release(net); return error; }
 memcpy(mbuf_data(packet),frame,length);
 mbuf_setlen(packet,length);mbuf_pkthdr_setlen(packet,length);
 error=mbuf_pkthdr_setrcvif(packet,net);
 if (!error) {
  void *header=mbuf_data(packet);
  mbuf_pkthdr_setheader(packet,header);
  mbuf_pkthdr_setlen(packet,length-14);
  error=mbuf_setdata(packet,(uint8_t *)header+14,length-14);
  if (!error) {
   bpf_tap_in(net,DLT_EN10MB,packet,header,14);
   struct ifnet_stat_increment_param stats={};
   stats.packets_in=1;stats.bytes_in=(uint32_t)length;
   /* ifnet_input consumes even on failure. Native RX storage remains owned
    * by the caller and can be reused immediately after this copy. */
   error=ifnet_input(net,packet,&stats);packet=nullptr;
  }
 }
 if (packet) mbuf_freem(packet);
 ifnet_release(net);
 return error;
}
