#!/usr/bin/env python3
"""No-device integration: real audio_live C transport + actual Python CLI."""
import ctypes as C,importlib.util,pathlib,subprocess,tempfile,unittest,struct,contextlib,io
root=pathlib.Path(__file__).resolve().parents[2]
spec=importlib.util.spec_from_file_location('audio_cli',root/'tools/nord2-audio-live.py');cli=importlib.util.module_from_spec(spec);spec.loader.exec_module(cli)
class Request(C.Structure):_fields_=[('token',C.c_uint32),('op',C.c_uint32),('generation',C.c_uint32)]
class Snapshot(C.Structure):_fields_=[('word',C.c_uint32*48)]
class Fake:
 def __init__(self,lib,lose_ack=False):self.lib=lib;self.lose_ack=lose_ack;self.writes=0
 def read(self,code,size):
  q=Request()
  if self.lib.nal_take(C.byref(q)):
   s=Snapshot();s.word[10]=1;s.word[14]=0xfffffff9;s.word[21]=5;s.word[22]=2;s.word[41]=1
   s.word[42]=self.lib.nal_gain_value(q.generation&15) if q.op==6 else 256
   s.word[43]=self.lib.nal_gain_value((q.generation>>4)&15) if q.op==6 else 256
   s.word[44]=1024;s.word[45]=128;s.word[46]=12;s.word[47]=34
   assert self.lib.nal_complete(C.byref(q),0,C.byref(s))==0
  b=C.create_string_buffer(size)
  n=(self.lib.nal_caps if code==0x64 else self.lib.nal_reply)(b,size)
  if not n:raise cli.usb.USBError('no reply yet')
  return b.raw[:n]
 def write(self,code,value,index):
  self.writes+=1
  assert code==0x65 and self.lib.nal_submit(value,index)==1
  if self.lose_ack:raise cli.usb.USBError('lost ACK')
class Tests(unittest.TestCase):
 @classmethod
 def setUpClass(cls):
  cls.tmp=tempfile.TemporaryDirectory();p=pathlib.Path(cls.tmp.name)
  subprocess.run(['cc','-shared','-fPIC',str(root/'shim/nord2-xnu/audio_live.c'),'-o',str(p/'live.so')],check=True)
  cls.lib=C.CDLL(str(p/'live.so'));cls.lib.nal_initialize.argtypes=[C.c_uint64];assert cls.lib.nal_initialize(0x123456789abcdef)==0
 @classmethod
 def tearDownClass(cls):cls.tmp.cleanup()
 def test_all_commands_and_signed_counters(self):
  t=Fake(self.lib)
  for name in cli.OPS:
   out=cli.command(t,name,.5,256,362) if name=='gain' else cli.command(t,name,.5);self.assertEqual(out['op'],cli.OPS[name]);self.assertEqual(out['error'],-7);self.assertEqual(out['written'],(2<<32)|5);self.assertEqual(out['board_status'],1);self.assertFalse(out['sound_audible_proven'])
  self.assertEqual(t.writes,len(cli.OPS))
 def test_all_gain_pairs_and_extended_status(self):
  t=Fake(self.lib)
  self.assertEqual(len(cli.FIELDS),48)
  for top in cli.GAIN_STEPS:
   for bottom in cli.GAIN_STEPS:
    out=cli.command(t,'gain',.5,top,bottom)
    self.assertEqual((out['gain_top'],out['gain_bottom'],out['gain_max']),(top,bottom,1024))
    self.assertEqual((out['ramp_frames'],out['dma_peak_top'],out['dma_peak_bottom']),(128,12,34))
  self.assertEqual(t.writes,49)
 def test_gain_ack_loss_no_resubmit(self):
  t=Fake(self.lib,True);out=cli.command(t,'gain',.5,1024,128)
  self.assertEqual((out['gain_top'],out['gain_bottom']),(1024,128));self.assertEqual(t.writes,1)
 def test_old_kernel_legacy_commands_and_new_mutation_rejection(self):
  class Legacy(Fake):
   def read(self,code,size):
    data=super().read(code,size)
    if code==0x64:
     w=list(struct.unpack('<16I',data));w[4]=0x3e;data=struct.pack('<16I',*w)
    return data
  t=Legacy(self.lib)
  for name in ('status','enable','stop','tone','silent'):
   out=cli.command(t,name,.5);self.assertNotIn('gain_top',out)
  self.assertEqual(t.writes,5)
  for name in ('gain','tone-top','tone-bottom'):
   before=t.writes
   with self.assertRaisesRegex(ValueError,'not supported'):
    cli.command(t,name,.5,256,512) if name=='gain' else cli.command(t,name,.5)
   self.assertEqual(t.writes,before)
 def test_invalid_gain_arguments_never_write(self):
  t=Fake(self.lib)
  for top,bottom in ((None,None),(256,None),(None,256),(-1,256),(256,2048),(True,256),(256.0,256),(7,0)):
   with self.assertRaises(ValueError):cli.command(t,'gain',.5,top,bottom)
  with self.assertRaises(ValueError):cli.command(t,'tone',.5,256,256)
  self.assertEqual(t.writes,0)
 def test_cli_argument_parser(self):
  a=cli.parse_args(['gain','--top','0','--bottom','1024']);self.assertEqual((a.top,a.bottom),(0,1024))
  for argv in (['gain'],['gain','--top','256'],['gain','--bottom','256'],['gain','--top','256.0','--bottom','256'],['gain','--top','2048','--bottom','0'],['tone-top','--top','256']):
   with contextlib.redirect_stderr(io.StringIO()),self.assertRaises(SystemExit):cli.parse_args(argv)
 def test_mismatched_argument_after_single_write(self):
  class WrongArgument(Fake):
   def read(self,code,size):
    data=super().read(code,size)
    if code==0x66:
     w=list(struct.unpack('<48I',data));w[7]^=1;data=struct.pack('<48I',*w)
    return data
  t=WrongArgument(self.lib)
  with self.assertRaisesRegex(ValueError,'argument'):cli.command(t,'gain',.5,512,724)
  self.assertEqual(t.writes,1)
 def test_lost_ack_no_resubmit(self):
  t=Fake(self.lib,True);out=cli.command(t,'tone',.5);self.assertEqual(out['result'],0);self.assertEqual(t.writes,1)
 def test_pending_prevents_mutation(self):
  t=Fake(self.lib);cap=cli.caps(t);self.assertEqual(self.lib.nal_submit((4<<12)|(cap[11]+1),0),1)
  # Prevent fake worker consuming pending request in initialCAP read.
  class Pending(Fake):
   def read(self,code,size):
    b=C.create_string_buffer(size);n=self.lib.nal_caps(b,size);return b.raw[:n]
  blocked=Pending(self.lib)
  with self.assertRaisesRegex(ValueError,'pending'):cli.command(blocked,'tone',.5)
  self.assertEqual(blocked.writes,0)
  t.read(0x64,64)
 def test_previous_reply_waits_for_matching_token(self):
  class Delayed(Fake):
   def write(self,code,value,index):
    self.value=value;self.writes+=1;assert self.lib.nal_submit(value,index)==1;self.delayed=True
   def read(self,code,size):
    if self.delayed:
     if code==0x64:
      b=C.create_string_buffer(size);n=self.lib.nal_caps(b,size);return b.raw[:n]
     self.delayed=False
     b=C.create_string_buffer(size);n=self.lib.nal_reply(b,size);return b.raw[:n]
    return super().read(code,size)
  t=Delayed(self.lib);t.delayed=False
  old=cli.command(Fake(self.lib),'status',.5)['token']
  out=cli.command(t,'tone',.5);self.assertEqual(out['token'],old+1);self.assertEqual(t.writes,1)
 def test_reboot_rejected_after_single_write(self):
  class Reboot(Fake):
   def read(self,code,size):
    data=super().read(code,size)
    if self.writes and code==0x64:
     w=list(struct.unpack('<16I',data));w[7]^=1;data=struct.pack('<16I',*w)
    return data
  t=Reboot(self.lib)
  with self.assertRaisesRegex(ValueError,'rebooted'):cli.command(t,'tone',.5)
  self.assertEqual(t.writes,1)
if __name__=='__main__':unittest.main()
