#!/usr/bin/env python3
"""Bounded speaker controls over the native Nord2 USB console. No flash or raw writes.
status: public counters only. enable: allow HAL PCM. stop: flush/stop playback.
tone: fixed quiet1kHz/1second. silent:1second DMA/clock test, no speaker enable.
"""
import argparse,fcntl,importlib.util,json,os,pathlib,struct,time
spec=importlib.util.spec_from_file_location('wifi_usb',pathlib.Path(__file__).with_name('nord2-wifi-live.py'))
usb=importlib.util.module_from_spec(spec);spec.loader.exec_module(usb)
MAGIC=0x4c41444e
OPS={'status':1,'enable':2,'stop':3,'tone':4,'silent':5}
FIELDS=('magic version bytes boot_lo boot_hi token op reserved result enabled initialized running silent uncertain error stop_error gate amp0 amp1 status0 status1 written_lo written_hi queued dropped_lo dropped_hi pcm silence underruns ticks max_gap peak volume muted opens writes time_lo time_hi stream_starts tone_frames hal_status board_status').split()
def words(data,n):
 if len(data)!=n*4:raise ValueError('short audio reply')
 return struct.unpack('<'+'I'*n,data)
def caps(t):
 w=words(t.read(0x64,64),16)
 if w[:7]!=(MAGIC,1,64,192,0x3e,4095,0) or w[9]!=1:raise ValueError('audio protocol unavailable or incompatible')
 return w
def command(t,name,timeout):
 first=caps(t);boot=first[7:9]
 if first[10]:raise ValueError('another audio command is pending; do not resubmit a mutation')
 token=first[11]+1
 if token>4095:raise ValueError('audio token budget exhausted')
 op=OPS[name];deadline=time.monotonic()+timeout
 # A lost ACK may still have queued the command. Never send a fresh token.
 try:t.write(0x65,(op<<12)|token,0)
 except usb.USBError:pass
 while time.monotonic()<deadline:
  current=caps(t)
  if current[7:9]!=boot:raise ValueError('phone rebooted while command was pending')
  if current[11]>token:raise ValueError('another client changed the command token')
  try:w=words(t.read(0x66,192),48)
  except usb.USBError:time.sleep(.05);continue
  if w[0:3]!=(MAGIC,1,192):raise ValueError('invalid reply identity')
  if w[3:5]!=boot:raise ValueError('stale boot reply')
  if w[5]==token:
   if w[6:8]!=(op,0) or w[8]>4:raise ValueError('mismatched reply operation')
   out=dict(zip(FIELDS,w))
   for f in ('error','stop_error'):out[f]=struct.unpack('<i',struct.pack('<I',out[f]))[0]
   out['written']=(out.pop('written_hi')<<32)|out.pop('written_lo')
   out['dropped']=(out.pop('dropped_hi')<<32)|out.pop('dropped_lo')
   out['time_ms']=(out.pop('time_hi')<<32)|out.pop('time_lo')
   for f in ('magic','bytes','boot_lo','boot_hi','reserved'):out.pop(f)
   out['sound_audible_proven']=False
   return out
  if w[5]>token:raise ValueError('unexpected newer reply')
  time.sleep(.05)
 raise ValueError('audio command outcome unknown; inspect status, do not repeat a mutation blindly')
def main():
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('command',choices=OPS);ap.add_argument('--timeout',type=float,default=10);a=ap.parse_args()
 if not 0<a.timeout<=30:ap.error('timeout must be >0 and <=30seconds')
 lock=None;t=None
 try:
  lock=os.open('/tmp/nord2-audio-live-'+str(os.getuid())+'.lock',os.O_CREAT|os.O_RDWR|os.O_NOFOLLOW,0o600)
  fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
  t=usb.USB();out=command(t,a.command,a.timeout);print(json.dumps(out));return int(out['result']!=0)
 except (OSError,ValueError,usb.ProtocolError,usb.USBError) as e:
  print(json.dumps({'ok':False,'error':str(e),'no_automatic_mutation_retry':True}));return 2
 finally:
  if t:t.close()
  if lock is not None:os.close(lock)
if __name__=='__main__':raise SystemExit(main())
