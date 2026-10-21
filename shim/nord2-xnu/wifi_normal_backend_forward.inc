static int ncb_after_basic(void);
static int ncb_rx_active(const struct nwfn_native *);
static int ncb_rx_memory(const struct nwfn_native *,uint64_t,uint32_t,unsigned);
static int ncb_rx_write(const struct nwfn_native *,uint32_t,uint32_t);
#define NCT_AFTER_BASIC() ncb_after_basic()
#define NWFN_RX_ACTIVE(n) ncb_rx_active(n)
#define NWFN_RX_MEMORY(n,a,b,w) ncb_rx_memory(n,a,b,w)
#define NWFN_RX_WRITE(n,a,v) ncb_rx_write(n,a,v)
