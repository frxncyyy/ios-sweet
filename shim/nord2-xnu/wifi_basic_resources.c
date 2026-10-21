#include "wifi_basic_resources.h"
int nnr_plan(const struct ncp_result *c,size_t bytes,struct nnr_plan *out){
 struct nnr_plan p={0};
 if(!c||!out||!c->decoded||!c->resources_supported||c->resource_version!=0x80000001u||bytes!=76||!c->resources[1]||c->resources[5]>255)return -1;
 p.version=c->resource_version;p.pse_data=c->resources[0];p.pse_cmd=c->resources[1];
 p.ple_data=c->resources[2];p.ple_cmd=c->resources[3];
 p.pse_page_bytes=(uint16_t)c->resources[4];p.ple_page_bytes=(uint16_t)(c->resources[4]>>16);
 p.extra=(uint8_t)c->resources[5];p.tc=4;p.pse_cost=1;p.ple_cost=0;p.wire_bytes=76;
 *out=p;return 0;
}
int nnr_begin(const struct nnr_plan *p,struct nnr_budget *out){
 struct nnr_budget b={0};
 if(!p||!out||p->version!=0x80000001u||!p->pse_cmd||p->tc!=4||p->pse_cost!=1||p->ple_cost||p->wire_bytes!=76)return -1;
 b.plan=*p;b.pse_remaining=p->pse_cmd;*out=b;return 0;
}
int nnr_reserve_once(struct nnr_budget *b){
 if(!b||b->reserved||b->plan.version!=0x80000001u||b->plan.tc!=4||b->plan.pse_cost!=1||b->plan.ple_cost||b->plan.wire_bytes!=76||!b->plan.pse_cmd||b->pse_remaining!=b->plan.pse_cmd)return -1;
 b->pse_remaining--;b->reserved=1;return 0;
}
