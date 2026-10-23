/* Included by normal_backend_forward before the retained owner definition. */
struct nwfn_native;
static int nst_native_owner_window(const struct nwfn_native *);
static int nst_native_probe_window(const struct nwfn_native *);
static int nst_native_rx_active(const struct nwfn_native *);
static int nst_native_rx_memory(const struct nwfn_native *,uint64_t,uint32_t,unsigned);
static int nst_native_rx_write(const struct nwfn_native *,uint32_t,uint32_t);
