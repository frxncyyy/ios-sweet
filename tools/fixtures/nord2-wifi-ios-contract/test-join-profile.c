#define main baseline_join_main
#include "baseline-join.c"
#undef main
static void rejected(uint8_t *p,const uint8_t *mac){struct niwj_request q,z={0};struct niwj_diagnostic d;memset(&q,0xa5,sizeof q);C(niwj_decode_identity(p,592,mac,&q,&d)==NIWJ_OPTIONS);C(!memcmp(&q,&z,sizeof q));}
static void scope(uint8_t*p,unsigned n){put32(p+536,n);memset(p+540,'x',36);p[540+n]=0;}
int main(void){C(!baseline_join_main());uint8_t p[592],mac[6]={2,1,2,3,4,5};struct niwj_request q;struct niwj_diagnostic d;
 for(unsigned flags=0;flags<256;flags++){wire(p);p[464]=(uint8_t)flags;memcpy(p+508,mac,6);if(flags&64)scope(p,36);int r=niwj_decode_identity(p,592,mac,&q,&d);C((r==0)==((flags&~0x48u)==0));}
 for(unsigned n=0;n<=40;n++){wire(p);p[464]=64;put32(p+536,n);memset(p+540,'x',40);if(n<=39)p[540+n]=0;C((niwj_decode_identity(p,592,mac,&q,&d)==0)==(n>=1&&n<=36));}
 for(unsigned n=1;n<=36;n++){wire(p);p[464]=72;memcpy(p+508,mac,6);scope(p,n);C(!niwj_decode_identity(p,592,mac,&q,&d));C(d.private_address_present&&d.owner_address_valid&&d.private_address_matches&&d.scope_id==n);p[540+n]=1;rejected(p,mac);p[540+n]=0;for(unsigned j=0;j<n;j++){p[540+j]=0;rejected(p,mac);p[540+j]='x';}}
 wire(p);p[464]=72;memcpy(p+508,mac,6);scope(p,36);rejected(p,NULL);for(unsigned i=0;i<6;i++){p[508+i]^=4;rejected(p,mac);p[508+i]^=4;}
 for(unsigned v=0;v<256;v++){wire(p);p[464]=8;uint8_t self[6]={0,1,2,3,4,5};self[0]=(uint8_t)v;memcpy(p+508,self,6);C((niwj_decode_identity(p,592,self,&q,&d)==0)==((v&3u)==2u));}
 wire(p);scope(p,36);rejected(p,mac); /* string length without flag */
 wire(p);p[464]=64;scope(p,36);memset(p+577,0xa5,3);C(!niwj_decode(p,592,&q,&d)); /* inactive tail */
 wire(p);p[464]=8;memcpy(p+508,mac,6);C(niwj_decode(p,592,&q,&d)==NIWJ_OPTIONS); /* no authority supplied */
 puts("PASS focused profile identity/scope grammar; synthetic only, no address content logged");return 0;}
