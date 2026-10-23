#!/usr/bin/env python3
"""Build/verify private fixed-file Nord2 ADT audio payload; never writes a rootfs."""
import argparse, hashlib, pathlib, plistlib, struct
MAX=262144
FACTORY='7C3E5B0A-52D4-4F6B-9E0D-6B1A2D7F0C51'
def decode(blob):
    if not 96 <= len(blob) <= MAX: raise ValueError('package size')
    magic,version,total,count,a,b,reserved,ha,hb=struct.unpack('<4sIIIII8s32s32s',blob[:96])
    if (magic,version,total,count,reserved)!=(b'N2AP',1,len(blob),2,b'\0'*8): raise ValueError('header')
    if not a or not b or a+b!=len(blob)-96: raise ValueError('lengths')
    binary,plist=blob[96:96+a],blob[96+a:]
    if hashlib.sha256(binary).digest()!=ha or hashlib.sha256(plist).digest()!=hb: raise ValueError('digest')
    return binary,plist
def encode(binary,plist):
    info=plistlib.loads(plist)
    if info.get('CFBundleExecutable')!='RedwoodAudio' or info.get('CFBundleIdentifier')!='com.redwood.audio' or info.get('CFPlugInFactories',{}).get(FACTORY)!='RedwoodAudio_Create': raise ValueError('unexpected plist')
    if len(binary)<32 or binary[:4]!=b'\xcf\xfa\xed\xfe' or struct.unpack_from('<I',binary,4)[0]!=0x100000c or struct.unpack_from('<I',binary,12)[0]!=8: raise ValueError('expected arm64 MH_BUNDLE')
    blob=struct.pack('<4sIIIII8s32s32s',b'N2AP',1,96+len(binary)+len(plist),2,len(binary),len(plist),b'\0'*8,hashlib.sha256(binary).digest(),hashlib.sha256(plist).digest())+binary+plist
    decode(blob)
    return blob
if __name__=='__main__':
    p=argparse.ArgumentParser(description=__doc__); s=p.add_subparsers(dest='op',required=True)
    b=s.add_parser('build'); b.add_argument('bundle',type=pathlib.Path); b.add_argument('output',type=pathlib.Path)
    v=s.add_parser('verify'); v.add_argument('package',type=pathlib.Path)
    a=p.parse_args()
    if a.op=='build':
        blob=encode((a.bundle/'RedwoodAudio').read_bytes(),(a.bundle/'Info.plist').read_bytes())
        with a.output.open('xb') as f: f.write(blob)
    else: blob=a.package.read_bytes(); decode(blob)
    print(f'{len(blob)} bytes SHA256 {hashlib.sha256(blob).hexdigest()}')
