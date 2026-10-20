#!/usr/bin/env python3
"""Compile the exact memory-only loader and test private vendor bytes, no phone IO."""
import ctypes as C
import hashlib
import json
import os
from pathlib import Path
import subprocess
import tempfile
ROOT=Path(__file__).resolve().parents[1]
VENDOR=ROOT/'research/nord2/wifi/vendor-read/vendor.img'
LAYOUT=ROOT/'research/nord2/wifi/firmware/verified-storage-layout.json'
class Blob(C.Structure):
    _fields_=[('name',C.c_char_p),('data',C.c_void_p),('size',C.c_uint32)]
READ=C.CFUNCTYPE(C.c_int,C.c_void_p,C.c_uint64,C.c_void_p,C.c_uint32)
def main():
    manifest=json.loads(LAYOUT.read_text())
    with tempfile.TemporaryDirectory() as td:
        so=Path(td)/'firmware.so'
        subprocess.run(['cc','-std=c11','-O2','-Wall','-Wextra','-Werror','-Wno-deprecated-declarations','-DNWF_HOST_TEST','-shared','-fPIC',str(ROOT/'shim/nord2-xnu/wifi_firmware.c'),'-lcrypto','-o',str(so)],check=True)
        lib=C.CDLL(str(so))
        lib.nwf_load.argtypes=[READ,C.c_void_p,C.c_void_p,C.c_size_t,C.c_void_p,C.c_size_t,C.POINTER(Blob)]
        lib.nwf_lz4.argtypes=[C.c_void_p,C.c_size_t,C.c_void_p,C.c_size_t]
        out=C.create_string_buffer(1326158); scratch=C.create_string_buffer(65536); blobs=(Blob*4)()
        fd=os.open(VENDOR,os.O_RDONLY); calls=[]; mode=['good']; fail_at=[0]; count=[0]
        @READ
        def reader(_,offset,dest,length):
            assert offset<=544354304 and length<=544354304-offset and length<=65536
            calls.append((offset,length))
            index=count[0]; count[0]+=1
            if mode[0]=='read_fail' and index==fail_at[0]: return -1
            data=os.pread(fd,length,offset)
            if len(data)!=length: return -1
            if mode[0]=='corrupt' and index==fail_at[0]: data=bytes([data[0]^1])+data[1:]
            if mode[0]=='output_corrupt' and index==1: out[0]=bytes([out.raw[0]^1])
            C.memmove(dest,data,length)
            return 0
        def run():
            count[0]=0
            return lib.nwf_load(reader,None,out,len(out),scratch,len(scratch),blobs)
        assert run()==0
        assert len(calls)==16
        for blob,f in zip(blobs,manifest['files']):
            assert blob.name.decode()==f['name']
            assert hashlib.sha256(C.string_at(blob.data,blob.size)).hexdigest()==f['sha256']
        assert not any(scratch.raw[:65536])
        for failure in ['read_fail','corrupt','output_corrupt']:
            for segment in range(16 if failure!='output_corrupt' else 1):
                fail_at[0]=segment; mode[0]=failure; assert run()!=0
                assert not any(out.raw[:1326158]) and not any(scratch.raw[:65536])
                assert all(not b.data and not b.name and not b.size for b in blobs)
        mode[0]='good'
        before=len(calls)
        assert lib.nwf_load(reader,None,out,1,scratch,65536,blobs)!=0
        assert lib.nwf_load(reader,None,out,1326158,out,65536,blobs)!=0
        assert len(calls)==before
        # Invalid/truncated LZ4 grammars; literal extension and overlap examples.
        for data,cap in [(b'\xf0',15),(b'\x00\x00\x00',4),(b'\x00\x01\x00',4),(b'\xf0\xff',300),(b'\x10A',2),(b'\x1fA\x01\x00\xff',1000)]:
            assert lib.nwf_lz4(data,len(data),out,cap)!=0
        assert lib.nwf_lz4(b'\x10A',2,out,1)==0 and out.raw[:1]==b'A'
        # Exhaustive small truncated/invalid blocks are bounded and deterministic.
        import random
        rng=random.Random(6893)
        for _ in range(10000):
            data=rng.randbytes(rng.randrange(1,100)); cap=rng.randrange(1,256)
            C.memset(C.addressof(out)+cap,0xa5,32)
            lib.nwf_lz4(data,len(data),out,cap)
            assert C.string_at(C.addressof(out)+cap,32)==b'\xa5'*32
        os.close(fd)
        # Run the exact C decoder under native address/undefined sanitizers.
        harness=Path(td)/'sanitize.c'
        harness.write_text(r'''#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "wifi_firmware.h"
static FILE *vendor;
static int reader(void *ctx,uint64_t off,void *dest,uint32_t n) {
 (void)ctx; if(off>NWF_VENDOR_BYTES || n>NWF_VENDOR_BYTES-off) return -1;
 return fseek(vendor,(long)off,SEEK_SET) || fread(dest,1,n,vendor)!=n;
}
int main(int argc,char **argv) {
 assert(argc==2); vendor=fopen(argv[1],"rb"); assert(vendor);
 unsigned char *out=malloc(NWF_OUTPUT_BYTES), *scratch=malloc(NWF_SCRATCH_BYTES);
 struct nwf_blob blobs[4]; assert(out && scratch);
 assert(!nwf_load(reader,0,out,NWF_OUTPUT_BYTES,scratch,NWF_SCRATCH_BYTES,blobs));
 unsigned char input[256]; unsigned r=6893;
 for(unsigned i=0;i<100000;i++) {
  r=r*1664525+1013904223; size_t n=(r%256)+1;
  for(size_t j=0;j<n;j++) {r=r*1664525+1013904223;input[j]=(unsigned char)(r>>24);}
  r=r*1664525+1013904223;
  size_t cap=(r%1024)+1;
  for(size_t k=0;k<32;k++) out[cap+k]=0xa5;
  (void)nwf_lz4(input,n,out,cap);
  for(size_t k=0;k<32;k++) assert(out[cap+k]==0xa5);
 }
 free(out);free(scratch);fclose(vendor);return 0;
}
''')
        exe=Path(td)/'sanitize'
        subprocess.run(['cc','-std=c11','-O1','-g','-fsanitize=address,undefined','-fno-omit-frame-pointer','-Wno-deprecated-declarations','-DNWF_HOST_TEST','-I'+str(ROOT/'shim/nord2-xnu'),str(harness),str(ROOT/'shim/nord2-xnu/wifi_firmware.c'),'-lcrypto','-o',str(exe)],check=True)
        subprocess.run([str(exe),str(VENDOR)],check=True)
        print('PASS: real vendor/all16 extents/four digests; each extent read/digest corruption; complete-output corruption; bounds/overlap; exact-C ASan/UBSan100k LZ4 fuzz')
if __name__=='__main__': main()
