#!/usr/bin/env python3
"""Bounded development Wi-Fi mailbox. Passwords remain on the phone.

Only compiled STATUS/capture/OFLD/disconnect operations exist. No shell, memory,
key, reset, join/retry, arbitrary register or packet operation is available.
A reply reports native counters; carrier, DHCP counts and ACK are NOT an IP lease.
"""
import argparse
import ctypes as C
import ctypes.util
import fcntl
import json
import os
from pathlib import Path
import struct
import time

MAGIC=0x4c57444e
OPS={'status':1,'capture-dhcp':2,'capture-headers':3,'capture-off':4,
     'ofld-allow':5,'ofld-drop':6,'disconnect':7}
RESULTS=('OK','UNAVAILABLE','STALE','REFUSED','LIMIT')
FIELDS=('magic version bytes boot_lo boot_hi token op expected result generation_lo '
        'generation_hi service native_phase error halt flags tx_pending rx_pending '
        'submitted completed rx_unicast rx_group tx_data tx_security tx_dhcp rx_dhcp '
        'offer ack nak rx_error rx_peer rx_key rx_replay rx_plain txs_ok txs_fail '
        'txs_ambiguous time_lo time_hi capture windows_left ofld_drop data_cursor '
        'command_cursor rx0 rx1 rx2 rx3 rx4 data_published data_report data_ddone '
        'air_pending wire_busy tx_accepted tx_local rx_delivered join_ready').split()
class ProtocolError(Exception):pass
class Busy(ProtocolError):pass
class Rebooted(ProtocolError):pass
class Ambiguous(ProtocolError):pass
class USBError(Exception):
    def __init__(self,code):self.code=code;super().__init__('libusb '+str(code))

def words(raw,count):
    if len(raw)!=count*4:raise ProtocolError('truncated mailbox structure')
    return struct.unpack('<'+str(count)+'I',raw)
def caps(raw):
    w=words(raw,16)
    if w[:7]!=(MAGIC,1,64,256,0xfe,4095,65535) or any(w[14:]):raise ProtocolError('unsupported capability ABI')
    if w[9] not in(0,1) or w[10]>2 or w[11]>4095 or w[13]>65535:raise ProtocolError('capability bounds')
    if not w[9]:raise ProtocolError('live mailbox disabled or unavailable')
    nonce=w[7]|w[8]<<32
    if not nonce:raise ProtocolError('missing boot nonce')
    if not w[11]:
        if w[10] or w[12] or w[13]:raise ProtocolError('uninitialized token state')
    elif not 1<=w[12]<=7 or (w[12]==1 and w[13]):raise ProtocolError('invalid last-command identity')
    return {'boot':nonce,'pending':w[10],'token':w[11],'op':w[12],'expected':w[13]}
def reply(raw):
    w=words(raw,64)
    if w[:3]!=(MAGIC,1,256) or any(w[len(FIELDS):]):raise ProtocolError('reply ABI/reserved fields')
    d=dict(zip(FIELDS,w));d['boot']=d['boot_lo']|d['boot_hi']<<32
    d['generation']=d['generation_lo']|d['generation_hi']<<32
    d['time_ms']=d['time_lo']|d['time_hi']<<32
    if not d['boot'] or not 1<=d['token']<=4095 or not 1<=d['op']<=7 or d['expected']>65535 or d['result']>4:raise ProtocolError('reply identity/result bounds')
    if d['op']==1 and d['expected']:raise ProtocolError('STATUS expected generation must be zero')
    if d['service']>8:raise ProtocolError('service state bounds')
    if d['flags']&~511 or d['capture']>2 or d['windows_left']>32:raise ProtocolError('snapshot bounds')
    for k in ('halt','ofld_drop','data_published','data_report','data_ddone','air_pending','wire_busy','join_ready'):
        if d[k]>1:raise ProtocolError('snapshot boolean')
    for k in ('data_cursor','command_cursor','rx0','rx1','rx2','rx3','rx4'):
        if d[k]>=1024:raise ProtocolError('ring cursor bounds')
    return d

def public(d):
    # Boot nonce is checked internally, never used as a device identifier.
    out={k:v for k,v in d.items() if k not in('magic','version','bytes','boot','boot_lo','boot_hi','generation_lo','generation_hi','time_lo','time_hi')}
    out['result_name']=RESULTS[d['result']]
    out.update(same_boot_verified=True,command_ack_is_execution_result=False,
               counters_prove_dhcp_lease=False,carrier_proves_ip=False,
               local_retirement_proves_air_success=False,internet_proven=False)
    return out

class Mailbox:
    def __init__(self,transport,timeout=45,clock=time.monotonic,sleep=time.sleep):
        if not 0<timeout<=45:raise ValueError('timeout must be >0 and <=45 seconds')
        self.t=transport;self.clock=clock;self.sleep=sleep;self.deadline=clock()+timeout;self.boot=None;self.command_may_execute=False
    def remaining(self):
        n=self.deadline-self.clock()
        if n<=0:raise Ambiguous('deadline: execution may be unknown; do not issue a new disconnect token')
        return n
    def read_caps(self):
        while True:
            self.remaining()
            if hasattr(self.t,'set_timeout'):self.t.set_timeout(self.remaining())
            try:d=caps(self.t.read(0x60,64))
            except USBError as e:
                if e.code==-9:raise ProtocolError('kernel has no live mailbox support') from e
                if e.code==-7:
                    self.sleep(min(.05,self.remaining()));continue
                raise
            if self.boot is None:self.boot=d['boot']
            elif d['boot']!=self.boot:raise Rebooted('boot changed: stop; never replay a command into the new boot')
            return d
    def exchange(self,op,expected=0):
        c=self.read_caps()
        if c['pending']:raise Busy('owner already has a pending command; no new command was sent')
        if c['token']==4095:raise ProtocolError('boot command-token budget exhausted')
        token=c['token']+1;value=op<<12|token
        # Retrying a timed-out OUT repeats this exact immutable command only.
        # Never advance the token after uncertain delivery, including disconnect.
        attempts=0
        while True:
            self.remaining();c=self.read_caps()
            if c['token']==token:
                if (c['op'],c['expected'])!=(op,expected):raise ProtocolError('token conflict; no retry')
                break
            if c['token']!=token-1 or c['pending']:raise Busy('command sequence changed; no retry')
            if attempts>=3:raise Ambiguous('OUT delivery unknown; same-token attempts exhausted')
            attempts+=1;self.command_may_execute=True
            try:
                if hasattr(self.t,'set_timeout'):self.t.set_timeout(self.remaining())
                self.t.write(0x61,value,expected)
            except USBError as e:
                if e.code not in(-7,-9):raise Ambiguous('USB command delivery unknown; stop') from e
                # A STALL can be a busy/conflict refusal. Inspect caps before
                # any retry; never assume a negative transfer means no mutation.
                self.sleep(min(.05,self.remaining()));continue
            # ACK means accepted, not executed; read immutable identity again.
        while True:
            self.remaining();c=self.read_caps()
            if (c['token'],c['op'],c['expected'])!=(token,op,expected):raise ProtocolError('command identity changed while waiting')
            try:
                if hasattr(self.t,'set_timeout'):self.t.set_timeout(self.remaining())
                raw=self.t.read(0x62,256)
            except USBError as e:
                if e.code in(-7,-9):raw=b''
                else:raise Ambiguous('reply transfer failed; execution may be unknown') from e
            if raw:
                d=reply(raw)
                if d['boot']!=self.boot:raise Rebooted('reply boot changed; stop')
                if d['token']==token:
                    if (d['op'],d['expected'])!=(op,expected):raise ProtocolError('reply command mismatch')
                    self.command_may_execute=False;return d
                if d['token']>token:raise ProtocolError('unexpected newer reply')
            self.sleep(min(.1,self.remaining()))
    def command(self,name):
        if name not in OPS:raise ValueError('unsupported command')
        if name=='status':return self.exchange(1)
        status=self.exchange(1)
        if status['result']!=0:raise ProtocolError('fresh STATUS unavailable; mutation refused')
        g=status['generation']
        if not 0<=g<=65535:raise ProtocolError('generation outside exact mutation domain; mutation refused')
        return self.exchange(OPS[name],g)

    def wait_disconnected(self):
        # Only fresh STATUS is repeated. join_ready requires the owner's actual
        # fresh-scan handoff proof, not just the previous disconnect result. Never replay disconnect with a new
        # token. These are owner-reported closure fields, not RF/firmware proof.
        while True:
            d=self.exchange(1)
            if d['result']!=0:return d
            if d['join_ready'] and not (d['flags']&0xbf) and not any(d[k] for k in ('tx_pending','rx_pending','air_pending','wire_busy')):return d
            self.sleep(min(.25,self.remaining()))

class USB:
    """System libusb; exact console product+VID/PID. Never reads serial strings."""
    def __init__(self):
        self.timeout_ms=500
        self.lib=C.CDLL(ctypes.util.find_library('usb-1.0') or 'libusb-1.0.so.0');self.ctx=C.c_void_p();self.handle=C.c_void_p()
        p=C.c_void_p
        for name,result,args in [('init',C.c_int,[C.POINTER(p)]),('exit',None,[p]),('get_device_list',C.c_ssize_t,[p,C.POINTER(C.POINTER(p))]),('free_device_list',None,[C.POINTER(p),C.c_int]),('get_bus_number',C.c_uint8,[p]),('get_device_address',C.c_uint8,[p]),('open',C.c_int,[p,C.POINTER(p)]),('close',None,[p]),('control_transfer',C.c_int,[p,C.c_uint8,C.c_uint8,C.c_uint16,C.c_uint16,p,C.c_uint16,C.c_uint])]:
            f=getattr(self.lib,'libusb_'+name);f.restype=result;f.argtypes=args
        if self.lib.libusb_init(C.byref(self.ctx)):raise ProtocolError('libusb initialization failed')
        try:
            found=[]
            for node in Path('/sys/bus/usb/devices').iterdir():
                try:
                    if (node/'product').read_text().strip()=='Nord2 XNU console' and (node/'idVendor').read_text().strip()=='18d1' and (node/'idProduct').read_text().strip()=='d001':found.append((int((node/'busnum').read_text()),int((node/'devnum').read_text())))
                except (OSError,ValueError):pass
            if len(found)!=1:raise ProtocolError('expected exactly one native Nord2 console')
            devices=C.POINTER(p)();n=self.lib.libusb_get_device_list(self.ctx,C.byref(devices))
            if n<0:raise ProtocolError('USB enumeration failed')
            try:
                for i in range(n):
                    d=devices[i]
                    if (self.lib.libusb_get_bus_number(d),self.lib.libusb_get_device_address(d))==found[0]:
                        e=self.lib.libusb_open(d,C.byref(self.handle))
                        if e:raise USBError(e)
                        break
            finally:self.lib.libusb_free_device_list(devices,1)
            if not self.handle:raise ProtocolError('native console disappeared')
        except BaseException:self.close();raise
    def set_timeout(self,remaining):self.timeout_ms=max(1,min(500,int(remaining*1000)))
    def read(self,code,size):
        b=C.create_string_buffer(size);n=self.lib.libusb_control_transfer(self.handle,0xc0,code,0,0,b,size,self.timeout_ms)
        if n<0:raise USBError(n)
        return b.raw[:n]
    def write(self,code,value,index):
        n=self.lib.libusb_control_transfer(self.handle,0x40,code,value,index,None,0,self.timeout_ms)
        if n<0:raise USBError(n)
        if n!=0:raise ProtocolError('unexpected OUT data length')
    def close(self):
        if self.handle:self.lib.libusb_close(self.handle);self.handle=C.c_void_p()
        if self.ctx:self.lib.libusb_exit(self.ctx);self.ctx=C.c_void_p()

def lock_file():
    path='/tmp/nord2-wifi-live-'+str(os.getuid())+'.lock'
    fd=os.open(path,os.O_CREAT|os.O_RDWR|os.O_NOFOLLOW,0o600)
    try:fcntl.flock(fd,fcntl.LOCK_EX|fcntl.LOCK_NB)
    except OSError:os.close(fd);raise Busy('another live CLI process holds the lock')
    return fd

def main(argv=None):
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('--timeout',type=float,default=45);sp=ap.add_subparsers(dest='cmd',required=True)
    s=sp.add_parser('status');s.add_argument('--watch',type=float,default=0)
    s=sp.add_parser('capture');s.add_argument('mode',choices=('dhcp','headers','off'))
    s=sp.add_parser('rx-ofld');s.add_argument('mode',choices=('allow','drop'));sp.add_parser('disconnect')
    a=ap.parse_args(argv)
    if not 0<a.timeout<=45:ap.error('--timeout must be >0 and <=45 seconds')
    watch=getattr(a,'watch',0)
    if not 0<=watch<=45:ap.error('--watch must be <=45 seconds')
    lock=None;t=None;m=None
    try:
        lock=lock_file();t=USB();m=Mailbox(t,a.timeout);end=m.clock()+watch
        name=a.cmd if a.cmd in('status','disconnect') else ('capture-' if a.cmd=='capture' else 'ofld-')+a.mode
        while True:
            d=m.command(name);out=public(d)
            if name=='disconnect':out['async_leave_requested']=d['result']==0;out['firmware_retirement_proven']=False
            print(json.dumps(out),flush=True)
            if name=='disconnect' and d['result']==0:
                d=m.wait_disconnected();out=public(d);out['disconnect_status_observed']=d['result']==0;out['firmware_retirement_proven']=False;print(json.dumps(out),flush=True)
            if d['result'] or not watch or m.clock()>=end:return 0 if d['result']==0 else 1
            m.sleep(min(1,m.remaining()))
    except (ProtocolError,USBError,OSError,ValueError) as e:
        print(json.dumps({'ok':False,'error':str(e),'ambiguous_execution':isinstance(e,Ambiguous) or bool(m and m.command_may_execute),'no_new_token_retry_performed':True,'DHCP_lease_proven':False}));return 2
    finally:
        if t:t.close()
        if lock is not None:os.close(lock)
if __name__=='__main__':raise SystemExit(main())
