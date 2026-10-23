#!/usr/bin/env python3
"""Bounded speaker controls over the native Nord2 USB console. No flash or raw writes.
status: public counters only. enable: allow HAL PCM. stop: flush/stop playback.
tone: fixed quiet1kHz/1second. silent:1second DMA/clock test, no speaker enable.
gain: absolute top/bottom digital Q15 steps while stopped/disabled.
tone-top/tone-bottom: isolated1second test at configured gain.
"""
import argparse,fcntl,importlib.util,json,os,pathlib,struct,time
spec=importlib.util.spec_from_file_location('wifi_usb',pathlib.Path(__file__).with_name('nord2-wifi-live.py'))
usb=importlib.util.module_from_spec(spec);spec.loader.exec_module(usb)
MAGIC=0x4c41444e
OPS={'status':1,'enable':2,'stop':3,'tone':4,'silent':5,'gain':6,'tone-top':7,'tone-bottom':8}
GAIN_STEPS=(0,128,256,362,512,724,1024)
FIELDS=('magic version bytes boot_lo boot_hi token op reserved result enabled initialized running silent uncertain error stop_error gate amp0 amp1 status0 status1 written_lo written_hi queued dropped_lo dropped_hi pcm silence underruns ticks max_gap peak volume muted opens writes time_lo time_hi stream_starts tone_frames hal_status board_status gain_top gain_bottom gain_max ramp_frames dma_peak_top dma_peak_bottom').split()
def words(data,n):
 if len(data)!=n*4:raise ValueError('short audio reply')
 return struct.unpack('<'+'I'*n,data)
def caps(t):
 w=words(t.read(0x64,64),16)
 if w[:4]!=(MAGIC,1,64,192) or w[4]not in(0x3e,0x1fe) or w[5:7]!=(4095,0) or w[9]!=1:raise ValueError('audio protocol unavailable or incompatible')
 return w
def gain_index(top,bottom):
 if type(top)is not int or type(bottom)is not int or top not in GAIN_STEPS or bottom not in GAIN_STEPS:
  raise ValueError('gain requires both top and bottom from '+','.join(map(str,GAIN_STEPS)))
 return GAIN_STEPS.index(top)|(GAIN_STEPS.index(bottom)<<4)
def command(t,name,timeout,top=None,bottom=None):
 if name not in OPS:raise ValueError('unknown audio command')
 if name=='gain':index=gain_index(top,bottom)
 else:
  if top is not None or bottom is not None:raise ValueError('top/bottom apply only to gain')
  index=0
 first=caps(t);boot=first[7:9];op=OPS[name]
 if not(first[4]&(1<<op)):raise ValueError('audio command not supported by this kernel')
 if first[10]:raise ValueError('another audio command is pending; do not resubmit a mutation')
 token=first[11]+1
 if token>4095:raise ValueError('audio token budget exhausted')
 deadline=time.monotonic()+timeout
 # A lost ACK may still have queued the command. Never send a fresh token.
 try:t.write(0x65,(op<<12)|token,index)
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
   if w[6:8]!=(op,index) or w[8]>4:raise ValueError('mismatched reply operation or argument')
   out=dict(zip(FIELDS if first[4]==0x1fe else FIELDS[:42],w))
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
def parse_args(argv=None):
 ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('command',choices=OPS);ap.add_argument('--timeout',type=float,default=10)
 ap.add_argument('--top',type=int,choices=GAIN_STEPS);ap.add_argument('--bottom',type=int,choices=GAIN_STEPS);a=ap.parse_args(argv)
 if not 0<a.timeout<=30:ap.error('timeout must be >0 and <=30seconds')
 if a.command=='gain' and(a.top is None or a.bottom is None):ap.error('gain requires both --top and --bottom')
 if a.command!='gain' and(a.top is not None or a.bottom is not None):ap.error('--top/--bottom apply only to gain')
 return a
def main(argv=None):
 a=parse_args(argv)
 lock=None;t=None
 try:
  lock=os.open('/tmp/nord2-audio-live-'+str(os.getuid())+'.lock',os.O_CREAT|os.O_RDWR|os.O_NOFOLLOW,0o600)
  fcntl.flock(lock,fcntl.LOCK_EX|fcntl.LOCK_NB)
  t=usb.USB();out=command(t,a.command,a.timeout,a.top,a.bottom);print(json.dumps(out));return int(out['result']!=0)
 except (OSError,ValueError,usb.ProtocolError,usb.USBError) as e:
  print(json.dumps({'ok':False,'error':str(e),'no_automatic_mutation_retry':True}));return 2
 finally:
  if t:t.close()
  if lock is not None:os.close(lock)
if __name__=='__main__':raise SystemExit(main())
