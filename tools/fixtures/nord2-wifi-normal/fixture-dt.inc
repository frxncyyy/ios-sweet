typedef int DTEntry;
#define kSuccess 0
static uint8_t authenticated_board[6146];static unsigned no_board;
static int SecureDTLookupEntry(void*root,const char*path,DTEntry*out){assert(!root&&!strcmp(path,"/chosen"));*out=1;return 0;}
static int SecureDTGetProperty(DTEntry chosen,const char*name,const void**out,unsigned*bytes){assert(chosen==1&&!strcmp(name,"nord2-wifi-calibration"));if(no_board)return -1;*out=authenticated_board;*bytes=sizeof(authenticated_board);return 0;}
