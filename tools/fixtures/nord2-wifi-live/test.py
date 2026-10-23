#!/usr/bin/env python3
"""Mocked EP0 tests. No libusb initialization or device enumeration."""
import importlib.util,json,struct,sys,tempfile,subprocess,ctypes,os,re
from pathlib import Path
p=Path(sys.argv[1]);sp=importlib.util.spec_from_file_location('live',p);m=importlib.util.module_from_spec(sp);sp.loader.exec_module(m)
checks=0
def check(v):
 global checks
 checks+=1;assert v
def throws(kind,f):
 global checks
 try:f()
 except kind:checks+=1;return
 raise AssertionError('expected '+str(kind))
class Clock:
 def __init__(self):self.now=0
 def clock(self):return self.now
 def sleep(self,n):self.now+=n
class Mock:
 def __init__(self):
  self.boot=7;self.token=0;self.op=0;self.expected=0;self.pending=0;self.generation=1;self.result=0;self.delay=0;self.reads=0;self.calls=[];self.reply=None;self.fault=None;self.executed=[];self.before_cap=None;self.ready=1;self.recovering=False;self.ready_polls=0;self.ready_after=2
 def cap(self):return [m.MAGIC,1,64,256,254,4095,65535,self.boot,0,1,self.pending,self.token,self.op,self.expected,0,0]
 def pump(self):
  if self.pending:
   self.reads+=1
   if self.reads>self.delay:
    self.pending=0;w=[0]*64;w[:11]=[m.MAGIC,1,256,self.boot,0,self.token,self.op,self.expected,self.result,self.generation,0];w[40]=32;
    if self.op==1 and self.recovering:
     self.ready_polls+=1
     if self.ready_polls>=self.ready_after:self.ready=1;self.recovering=False
    w[57]=self.ready;self.reply=w;self.executed.append((self.token,self.op,self.expected))
 def read(self,code,size):
  if code==0x60:
   if self.before_cap:self.before_cap(self)
   return struct.pack('<16I',*self.cap())
  assert code==0x62;self.pump()
  return b'' if self.reply is None else struct.pack('<64I',*self.reply)
 def write(self,code,value,index):
  assert code==0x61;token=value&4095;op=value>>12;self.calls.append((token,op,index));f=self.fault
  if f=='lost-before':self.fault=None;raise m.USBError(-7)
  if f=='reboot':self.boot+=1;self.token=0;self.op=0;self.expected=0;self.pending=0;raise m.USBError(-7)
  if f=='conflict':self.token=token;self.op=7 if op!=7 else 2;self.expected=index;raise m.USBError(-9)
  if token==self.token:
   if (op,index)!=(self.op,self.expected):raise m.USBError(-9)
  elif token!=self.token+1 or self.pending:raise m.USBError(-9)
  else:
   self.token=token;self.op=op;self.expected=index;self.pending=1;self.reads=0
   if op==7:self.ready=0;self.recovering=True;self.ready_polls=0
  if f=='lost-after':self.fault=None;raise m.USBError(-7)
def box(t,timeout=5):
 c=Clock();return m.Mailbox(t,timeout,c.clock,c.sleep),c
for name,op in m.OPS.items():
 if op>7:continue
 t=Mock();b,c=box(t);r=b.command(name);check(r['result']==0 and r['op']==op);check(t.executed==([(1,1,0)] if op==1 else [(1,1,0),(2,op,1)]));check(not m.public(r)['counters_prove_dhcp_lease'])
# Generation0 is valid for pre-Join capture; never truncate a high generation.
t=Mock();t.generation=0;b,c=box(t);check(b.command('capture-dhcp')['expected']==0)
for g in(65535,65536,2**32+1,2**64-1):
 t=Mock();t.generation=g&0xffffffff
 old=t.pump
 def pump(t=t,g=g,old=old):
  old()
  if t.reply:t.reply[9]=g&0xffffffff;t.reply[10]=g>>32
 t.pump=pump;b,c=box(t)
 if g==65535:check(b.command('ofld-drop')['expected']==65535)
 else:throws(m.ProtocolError,lambda:b.command('ofld-drop'));check(len(t.calls)==1)
# Old read-only kernels refuse CAP without issuing OUT.
t=Mock();t.read=lambda *a:(_ for _ in()).throw(m.USBError(-9));b,c=box(t);throws(m.ProtocolError,lambda:b.command('disconnect'));check(not t.calls)
# Busy previous command cannot be superseded, even STATUS.
t=Mock();t.token=1;t.op=7;t.pending=2;b,c=box(t);throws(m.Busy,lambda:b.command('status'));check(not t.calls)
# A lost OUT ACK is never recovered with a different token, for disconnect too.
for fault in('lost-before','lost-after'):
 t=Mock();b,c=box(t);b.command('status');t.fault=fault;r=b.exchange(7,1);check(r['op']==7);check(all(x==(2,7,1) for x in t.calls[1:]));check(t.executed.count((2,7,1))==1)
# Retry never crosses reboot or conflict.
for fault,exc in [('reboot',m.Rebooted),('conflict',m.ProtocolError)]:
 t=Mock();b,c=box(t);b.command('status');t.fault=fault;throws(exc,lambda:b.exchange(7,1));check(len(t.calls)==2)
# Pending worker with no reply is bounded, remains ambiguous, no new token.
t=Mock();t.delay=10000;b,c=box(t,.25);throws(m.Ambiguous,lambda:b.exchange(7,1));check(c.now<=.25);check(t.calls==[(1,7,1)]);check(b.command_may_execute)
# Owner result is authoritative; STALE/REFUSED/LIMIT never reported as OK.
for result in range(1,5):
 t=Mock();t.result=result;b,c=box(t);r=b.command('status');check(r['result']==result and m.public(r)['result_name']==m.RESULTS[result]);throws(m.ProtocolError,lambda:b.command('disconnect'));check(all(x[1]==1 for x in t.calls))
# Delayed completion may return the previous complete reply until its token wins.
t=Mock();b,c=box(t);b.command('status');t.delay=3;r=b.exchange(2,1);check(r['token']==2 and c.now>.1)
# Disconnect follow-up sends STATUS only and waits on owner flags, never a new7.
t=Mock();b,c=box(t);b.command('disconnect');d=b.wait_disconnected();check(d['op']==1 and d['join_ready']==1);check(t.ready_polls==2);check(sum(x[1]==7 for x in t.calls)==1)
t=Mock();b,c=box(t,.5);old=t.pump
def busy_disconnect():
 old()
 if t.reply:t.reply[15]=0xbf
t.pump=busy_disconnect;b.command('disconnect');throws(m.Ambiguous,b.wait_disconnected);check(sum(x[1]==7 for x in t.calls)==1)
# Idle flags alone cannot finish a disconnect while fresh-scan readiness is0.
t=Mock();t.ready_after=10000;b,c=box(t,.5);b.command('disconnect');throws(m.Ambiguous,b.wait_disconnected);check(t.ready==0 and t.ready_polls>0);check(sum(x[1]==7 for x in t.calls)==1)
t=Mock();t.generation=0;b,c=box(t);r=b.command('status');check(r['join_ready']==1 and r['generation']==0);check('time_ms' in m.public(r) and 'time_us' not in m.public(r))
# Source/header contract pins are checked by the runner; parser mutations stay strict.
t=Mock();b,c=box(t);r=b.command('status');raw=t.read(0x62,256);check(len(m.FIELDS)==63)
for k,v in [(0,0),(1,3),(2,255),(5,0),(5,4096),(6,0),(6,8),(7,65536),(8,5),(11,9),(15,512),(39,3),(40,33),(41,2),(42,1024),(57,2),(58,1),(63,1)]:
 w=list(struct.unpack('<64I',raw));w[k]=v;throws(m.ProtocolError,lambda w=w:m.reply(struct.pack('<64I',*w)))
for n in(0,1,255,257,512):throws(m.ProtocolError,lambda n=n:m.reply(bytes(n)))
for k,v in [(0,0),(1,2),(2,63),(3,255),(4,0),(5,4096),(6,65536),(7,0),(9,0),(9,2),(10,3),(11,4096),(12,8),(13,65536),(14,1),(15,1)]:
 w=t.cap();w[k]=v;throws(m.ProtocolError,lambda w=w:m.caps(struct.pack('<16I',*w)))
# Version2 extension: old STATUS remains strict; new commands require support.
t=Mock();b,c=box(t);throws(m.ProtocolError,lambda:b.command('peer'));check(all(x[1]==1 for x in t.calls))
class V2(Mock):
 def __init__(self):super().__init__();self.profile=0
 def cap(self):
  w=super().cap();w[1]=2;w[4]=0x7fe;return w
 def pump(self):
  super().pump()
  if not self.reply:return
  w=self.reply;w[1]=2
  if self.op in(9,10):self.profile=self.op==10
  w[58]=int(self.profile)
  if self.op==8:w[60]=1;w[61]=2|(1<<2)|(4<<8);w[62]=0
for name,op in [('peer',8),('data-ethernet',9),('data-tods',10)]:
 t=V2();b,c=box(t);d=b.command(name);check(d['op']==op and t.calls[-1][1]==op)
 if op==8:check(m.public(d)['peer']==dict(wlan=5,from_ds=0,to_ds=1,muar=1,cipher=4,qos=0))
 if op==10:check(d['next_data_profile']==1 and d['data_profile']==0)
t=V2();t.ready=0;b,c=box(t);throws(m.ProtocolError,lambda:b.command('data-tods'));check(all(q[1]==1 for q in t.calls))
t=V2();b,c=box(t);b.command('peer');raw=t.read(0x62,256)
for k,v in [(58,2),(59,2),(60,2),(61,16384),(62,6),(63,1),(60,0),(62,1)]:
 w=list(struct.unpack('<64I',raw));w[k]=v;throws(m.ProtocolError,lambda w=w:m.reply(struct.pack('<64I',*w)))
for result in range(1,5):
 w=list(struct.unpack('<64I',raw));w[8]=result;throws(m.ProtocolError,lambda w=w:m.reply(struct.pack('<64I',*w)))
 w[60]=w[61]=0;check(m.reply(struct.pack('<64I',*w))['result']==result)
w=list(struct.unpack('<64I',raw));w[6]=1;throws(m.ProtocolError,lambda:m.reply(struct.pack('<64I',*w)))
t=V2();b,c=box(t);b.command('status');t.cap=lambda:[m.MAGIC,1,64,256,254,4095,65535,t.boot,0,1,0,t.token,1,0,0,0];throws(m.ProtocolError,b.read_caps)
# Token exhaustion refuses before OUT, and new process starts at last+1.
t=Mock();t.token=4095;t.op=1;b,c=box(t);throws(m.ProtocolError,lambda:b.command('status'));check(not t.calls)
t=Mock();t.token=18;t.op=6;t.expected=1;b,c=box(t);check(b.command('status')['token']==19)
# Timing limits, lock exclusion and no hidden libusb actions on module import.
for seconds in(0,-1,46,float('nan'),float('inf')):throws(ValueError,lambda seconds=seconds:m.Mailbox(Mock(),seconds))
fd=m.lock_file()
try:throws(m.Busy,m.lock_file)
finally:os.close(fd)
for secret in('ssid','bssid','password','pmk','nonce','mac_address','ip_address'):
 check(secret not in m.public(r))
check(m.public(r)['carrier_proves_ip'] is False and m.public(r)['internet_proven'] is False)
# Real canonical mailbox implementation, USB mocked in memory only.
R=next(x for x in p.resolve().parents if(x/'AGENTS.md').is_file())
with tempfile.TemporaryDirectory(prefix='nord2-live-host-') as td:
 libpath=Path(td)/'mailbox.so';model=Path(__file__).with_name('mailbox.c')
 build=subprocess.run(['cc','-std=c11','-O2','-Wall','-Wextra','-Werror','-Wno-misleading-indentation','-fPIC','-shared','-I'+str(R/'shim/nord2-xnu'),str(R/'shim/nord2-xnu/wifi_live_control.c'),str(model),'-o',str(libpath)],capture_output=True,text=True)
 assert not build.returncode,build.stderr
 lib=ctypes.CDLL(str(libpath));lib.mock_initialize.restype=ctypes.c_int;lib.mock_write.argtypes=[ctypes.c_uint,ctypes.c_uint];lib.mock_read.argtypes=[ctypes.c_uint,ctypes.c_void_p,ctypes.c_uint];check(lib.mock_initialize()==0)
 class Actual:
  def __init__(self):self.lose=False;self.writes=[]
  def read(self,code,size):
   out=ctypes.create_string_buffer(size);n=lib.mock_read(code,out,size)
   if n<0:raise m.USBError(n)
   return out.raw[:n]
  def write(self,code,value,index):
   check(code==0x61);self.writes.append((value,index));n=lib.mock_write(value,index)
   if n<0:raise m.USBError(n)
   if self.lose:self.lose=False;raise m.USBError(-7)
 t=Actual();b,c=box(t)
 first=b.command('status');check(first['generation']==42 and first['join_ready']==1)
 check(b.command('capture-dhcp')['capture']==1)
 check(b.command('capture-headers')['capture']==2)
 check(b.command('capture-off')['capture']==0)
 check(b.command('ofld-drop')['ofld_drop']==1)
 check(b.command('ofld-allow')['ofld_drop']==0)
 before=lib.mock_executed();t.lose=True
 check(b.exchange(7,42)['result']==0);check(lib.mock_executed()==before+1)
 check(b.wait_disconnected()['join_ready']==1);check(lib.mock_ready_polls()==2)
 check(b.exchange(5,41)['result']==2)
 check(b.command('peer')['peer_valid']==1)
 check(b.command('data-tods')['next_data_profile']==1)
 check(b.command('data-ethernet')['next_data_profile']==0)
 check(len(t.writes)==lib.mock_executed())
print(json.dumps({'passed':True,'checks':checks,'USB_device_calls':False,'mocked_transport_only':True,'actual_c_mailbox_bridge':True,'dhcp_or_internet_claim':False}))
