#!/usr/bin/env python3
"""Source-pinned scan-cancel command identity/extent oracle; offline only."""
from pathlib import Path
import argparse, hashlib, json, re, subprocess
R=next(p for p in Path(__file__).resolve().parents if (p/'AGENTS.md').exists())
a=argparse.ArgumentParser();a.add_argument('--source-root',type=Path,required=True);a.add_argument('--candidate',type=Path,default=R/'shim/nord2-xnu');a.add_argument('--log-dir',type=Path,required=True);x=a.parse_args();w=x.log_dir.resolve();w.mkdir(parents=True,exist_ok=True)
h=x.source_root.resolve()/'drivers/misc/mediatek/connectivity/wlan/core/gen4m/include/wsys_cmd_handler_fw.h'
s=h.read_text();start=s.index('enum ENUM_CMD_ID {');end=s.index('CMD_ID_SCAN_CANCEL,',start)+len('CMD_ID_SCAN_CANCEL,');enum=s[start:end]+'\n};\n'
start=s.index('struct CMD_SCAN_CANCEL {');end=s.index('};',start)+2;shape=s[start:end]
t=(x.candidate/'wifi_station_transport.c').read_text();start=t.index('static int cid_size(');end=t.index('\n',start);size=t[start:end]
code='#include <assert.h>\n#include "wifi_station_transport.h"\n'+enum+shape+'\n'+size+'''
_Static_assert(CMD_ID_SCAN_REQ_V2==3,"scan v2 ABI");
_Static_assert(CMD_ID_NIC_POWER_CTRL==4,"power ABI");
_Static_assert(CMD_ID_SCAN_CANCEL==0x1b,"cancel ABI");
_Static_assert(NST_CID_SCAN_CANCEL==CMD_ID_SCAN_CANCEL,"native cancellation must use source wire ID");
_Static_assert(sizeof(struct CMD_SCAN_CANCEL)==4,"cancel extent");
_Static_assert(offsetof(struct CMD_SCAN_CANCEL,ucSeqNum)==0,"sequence");
_Static_assert(offsetof(struct CMD_SCAN_CANCEL,ucIsExtChannel)==1,"extension");
_Static_assert(offsetof(struct CMD_SCAN_CANCEL,aucReserved)==2,"reserved");
int main(void){for(size_t n=0;n<65;n++){assert(cid_size(CMD_ID_SCAN_CANCEL,n)==(n==sizeof(struct CMD_SCAN_CANCEL)));assert(!cid_size(CMD_ID_NIC_POWER_CTRL,n));}return 0;}
'''
(w/'oracle.c').write_text(code);cmd=['cc','-std=c11','-Wall','-Wextra','-Werror','-I'+str(x.candidate.resolve()),'-I'+str(R/'shim/nord2-xnu'),str(w/'oracle.c'),'-o',str(w/'oracle')];subprocess.run(cmd,check=True);subprocess.run([str(w/'oracle')],check=True)
sha=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
report={'status':'PASS','assertions':138,'source_header_sha256':sha(h),'candidate_header_sha256':sha(x.candidate/'wifi_station_transport.h'),'candidate_transport_sha256':sha(x.candidate/'wifi_station_transport.c'),'cancel_wire_id':27,'power_wire_id':4,'cancel_bytes':4,'hardware_actions':False}
(w/'report.json').write_text(json.dumps(report,indent=2)+'\n');print('PASS original Android command enum, cancel layout and actual native cid_size; power command refused')
