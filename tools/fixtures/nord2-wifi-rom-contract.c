#include "wifi_rom_contract.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc,char **argv)
{
    assert(argc==2);
    const unsigned char reg[16]={0,0,0,0,0x8f,0xb0,0,0,
        0,0,0,0,0,0x45,0,0};
    const unsigned char tag_addr[8]={0,0,0,0,0x8f,0xb0,0,0};
    const unsigned char tag_size[4]={0,0x45,0,0};
    struct nrc_claim claim;
    /* Saved DT's dynamic no-map/size without fixed reg or LK tag fails. */
    assert(nrc_boot_claim(0,0,1,0,0,0,0,0,&claim)<0);
    assert(claim.source==NRC_CLAIM_NONE);
    assert(nrc_boot_claim(reg,16,1,0,0,0,0,0,&claim)==0);
    assert(claim.source==NRC_CLAIM_FIXED_REG);
    assert(claim.base==NRC_CONN_BASE && claim.bytes==NRC_CONN_BYTES);
    assert(nrc_boot_claim(0,0,0,0,tag_addr,8,tag_size,4,&claim)==0);
    assert(claim.source==NRC_CLAIM_LK_TAG);
    assert(nrc_boot_claim(reg,16,1,0,tag_addr,8,tag_size,4,&claim)==0);
    unsigned char bad_reg[16];memcpy(bad_reg,reg,16);bad_reg[7]=1;
    assert(nrc_boot_claim(bad_reg,16,1,0,0,0,0,0,&claim)<0);
    assert(claim.source==NRC_CLAIM_NONE);
    assert(nrc_boot_claim(reg,16,0,0,0,0,0,0,&claim)<0);
    assert(nrc_boot_claim(reg,16,1,1,0,0,0,0,&claim)<0);
    unsigned char bad_tag[8];memcpy(bad_tag,tag_addr,8);bad_tag[7]=1;
    assert(nrc_boot_claim(reg,16,1,0,bad_tag,8,tag_size,4,&claim)<0);
    assert(nrc_boot_claim(0,0,0,0,tag_addr,7,tag_size,4,&claim)<0);
    assert(nrc_boot_claim(0,0,0,0,tag_addr,8,tag_size,3,&claim)<0);
    assert(nrc_boot_claim(0,0,0,0,tag_addr,8,tag_size,4,&claim)==0);

    struct nrc_smc_request req[2];
    assert(nrc_region27_set_intent(&claim,req)==0);
    assert(req[0].x[0]==0xc200050b && req[0].x[1]==0);
    assert(req[0].x[2]==0x1b008fb0 && req[0].x[3]==0x01008ff4);
    assert(req[0].x[4]==0x00b6db6d);
    assert(req[1].x[0]==0xc200050b && req[1].x[1]==0);
    assert(req[1].x[2]==0x1b008fb0 && req[1].x[3]==0x00008ff4);
    assert(req[1].x[4]==0x00b6d028);
    for(unsigned i=0;i<2;i++)for(unsigned j=5;j<8;j++)assert(req[i].x[j]==0);
    claim.base++;
    assert(nrc_region27_set_intent(&claim,req)<0);

    struct nrc_step s[NRC_HANDOFF_STEPS];nrc_handoff_steps(s);
    assert(s[0].op==NRC_WRITE_LITERAL && s[0].physical==0x18001120 && s[0].value==0x89040000);
    assert(s[1].op==NRC_WRITE_LITERAL && s[1].physical==0x185001dc && s[1].value==0);
    assert(s[2].op==NRC_WRITE_LITERAL && s[2].physical==0x185001e0 && s[2].value==0);
    assert(s[3].op==NRC_CLEAR_BIT0 && s[3].physical==0x18000018);
    assert(s[4].op==NRC_SET_BIT0 && s[4].physical==0x18000010);
    assert(s[5].op==NRC_POLL_EQUAL && s[5].physical==0x184c1604 &&
        s[5].value==0x1d1e && s[5].max_reads==102 && s[5].delay_us==1000);

    FILE *fp=fopen(argv[1],"rb");assert(fp);
    assert(fseek(fp,0,SEEK_END)==0);long fl=ftell(fp);assert(fl>40);
    assert(fseek(fp,0,SEEK_SET)==0);
    unsigned char *fdt=malloc((size_t)fl);assert(fdt);
    assert(fread(fdt,1,(size_t)fl,fp)==(size_t)fl);fclose(fp);
    assert(nrc_ios_fixed_claim_from_fdt(fdt,(size_t)fl,&claim)==0);
    assert(claim.source==NRC_CLAIM_IOS_FIXED);
    assert(nrc_region27_set_intent(&claim,req)==0);
    /* A current-boot fixed modem reservation collision must stop selection. */
    const unsigned char ccci_reg[16]={0,0,0,0,0x8a,0,0,0,
        0,0,0,0,3,0x56,0,0};
    size_t hit=(size_t)fl;
    for(size_t i=0;i+16<=(size_t)fl;i++)if(!memcmp(fdt+i,ccci_reg,16)) {
        if(hit!=(size_t)fl)abort();
        hit=i;
    }
    assert(hit!=(size_t)fl);
    fdt[hit+4]=0x8f;fdt[hit+5]=0xb0;
    assert(nrc_ios_fixed_claim_from_fdt(fdt,(size_t)fl,&claim)<0);
    assert(claim.source==NRC_CLAIM_NONE);
    free(fdt);
    return 0;
}
