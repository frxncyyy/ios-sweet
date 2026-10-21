#include "wifi_ios_rates.h"
/* IEEE supported-rate selectors (500kbps, basic bit excluded) mapped to
 * codes the selected Apple consumer recognizes. IE11->code5 is this
 * port's explicit compatibility policy; not a recovered vendor writer. */
static const struct niwp_rate_map legacy[12]={
 {2,1},{4,2},{11,5},{22,11},
 {12,6},{18,9},{24,12},{36,18},{48,24},{72,36},{96,48},{108,54}
};
const struct niwp_rate_map *niwr_production_map(size_t *count){
 if(!count)return (const struct niwp_rate_map*)0;
 *count=12;return legacy;
}
int niwr_rates(const uint8_t*p,size_t bytes,uint32_t*out,size_t capacity,size_t*count){
 return niwp_rates(legacy,12,p,bytes,out,capacity,count);
}
