#include "wifi_rom_xnu.h"
#include "nord2-wifi-rom-host-platform.h"
#include <assert.h>
#include <stdio.h>
struct host_pe_state PE_state;
static boot_args ba={0x142000000ULL,0x3e000000,0x3e000000};
static unsigned char claim[24],*memory;static FILE *vendorfile;
static unsigned opt,allocs,frees,smcs,maps,powered,failgroup,reads;
int PE_parse_boot_argn(const char *k,void *p,int n){assert(!strcmp(k,"nord2wifirom")&&n==4);memcpy(p,&opt,4);return opt!=0;}
int SecureDTLookupEntry(void *p,const char *s,DTEntry *e){assert(!p&&!strcmp(s,"/chosen"));*e=claim;return 0;}
int SecureDTGetProperty(DTEntry e,const char *k,const void **p,unsigned *n){assert(e==claim);if(!strcmp(k,"dram-base")){*p=&ba.physBase;*n=8;}else if(!strcmp(k,"dram-size")){*p=&ba.memSize;*n=8;}else{assert(!strcmp(k,"nord2-conn-claim"));*p=claim;*n=24;}return 0;}
void *kalloc_data(size_t n,int flags){assert(flags==3);allocs++;return malloc(n);}
void kfree_data(void *p,size_t n){(void)n;frees++;free(p);}
int sweet_ufs_read_vendor(uint64_t off,void *p,uint32_t n){reads++;assert(n<=65536);return fseek(vendorfile,(long)off,SEEK_SET)||fread(p,1,n,vendorfile)!=n;}
vm_offset_t ml_io_map(vm_offset_t base,vm_size_t n){assert(powered&&smcs==2&&base==NRC_CONN_BASE&&n==NRC_CONN_BYTES);maps++;return (uintptr_t)memory;}
int nrb_host_smc(const uint64_t a[8],uint64_t out[4]){struct nrc_smc_request q[2];struct nrc_claim c={NRC_CLAIM_IOS_FIXED,NRC_CONN_BASE,NRC_CONN_BYTES};assert(!powered&&!maps&&smcs<2&&!nrc_region27_set_intent(&c,q));assert(!memcmp(a,q[smcs].x,64));smcs++;out[0]=failgroup==smcs?0x200c:0;out[1]=1;out[2]=2;out[3]=3;return 0;}
static int owner(void *p,enum nrb_owner_stage s){assert(p==&powered);return (s==NRB_COLD_BEFORE_POWER)?powered:!powered;}
static void release(struct nrb_native *n){free(n->output);free(n->backup);free(n->scratch);}
int main(int argc,char **argv){assert(argc==2);vendorfile=fopen(argv[1],"rb");assert(vendorfile);PE_state.bootArgs=&ba;memory=malloc(NRC_CONN_BYTES);assert(memory);for(unsigned i=0;i<8;i++){claim[i]=(unsigned char)(NRC_CONN_BASE>>(i*8));claim[8+i]=(unsigned char)(NRC_CONN_BYTES>>(i*8));}claim[16]=3;claim[20]=1;
 struct nrb_native n={0};assert(nrb_xnu_prepare(&n)==EPERM&&!allocs&&!smcs);opt=1;claim[20]=0;assert(nrb_xnu_prepare(&n)==EINVAL&&!allocs);claim[20]=1;n=(struct nrb_native){0};ba.memSizeActual--;assert(nrb_xnu_prepare(&n)==EINVAL&&!allocs);ba.memSizeActual++;
 for(unsigned g=1;g<=3;g++){n=(struct nrb_native){0};smcs=maps=powered=0;failgroup=g<3?g:0;assert(!nrb_xnu_prepare(&n)&&n.enabled);assert(nrb_xnu_prepare(&n)==EALREADY);int e=nrb_xnu_policy(&n,owner,&powered);if(g<3){assert(e&&n.state.halt&&smcs==g&&!maps&&n.output&&n.backup);release(&n);continue;}assert(!e&&n.state.accepted==2);powered=1;memset(memory,0xa5,NRC_CONN_BYTES);assert(!nrb_xnu_place(&n)&&n.state.placed&&maps==1&&n.state.writes==88&&n.state.reads==176);for(unsigned i=0;i<4;i++){struct nwf_rom_span *s=&n.state.plan.spans[i];const unsigned char *src=s->blob?n.state.wifi:n.state.mcu;assert(!memcmp(memory+s->emi_offset,src+s->source_offset,s->bytes));}assert(nrb_xnu_place(&n)&&maps==1);release(&n);}
 printf("PASS actual native adapter mock: defaultoff/currentclaim/highRAM/verifiedvendor supply/%u bounded reads/exactSET errors group1+0/persistent four-span copy/retained receipt/no retry; allocations %u decoderfrees %u\n",reads,allocs,frees);free(memory);fclose(vendorfile);}
