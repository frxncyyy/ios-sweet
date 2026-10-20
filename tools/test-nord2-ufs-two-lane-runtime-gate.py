#!/usr/bin/env python3
"""Exact runtime SMP root gate; stop-only RO exception vs unchanged defaults."""
from pathlib import Path
import ast,re,subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
s=(ROOT/'research/ci-39158169212/buildtree/xnu/osfmk/arm64/sweet_early.c').read_text()
a=s.index('static bool\nnord2_smp_arg_exact(')
b=s.index('\nint\nnord2_smp_userland_ready(',a)
production=s[a:b]
# Keep the runtime exception aligned with the static image checker bundle.
checker=ast.parse((ROOT/'tools/nord2-check-xnu.py').read_text())
function=next(n for n in checker.body if isinstance(n,ast.FunctionDef) and n.name=='point23_root_checks')
assignment=next(n for n in ast.walk(function) if isinstance(n,ast.Assign) and any(isinstance(t,ast.Name) and t.id=='diagnostic' for t in n.targets))
static_required=ast.literal_eval(assignment.value)
runtime_required=dict(re.findall(r'\{ "([a-zA-Z0-9_]+)", "([a-zA-Z0-9_]+)" \}',production))
assert all(runtime_required.get(k)==v for k,v in static_required.items())
assert all(runtime_required.get(k)==v for k,v in {'cpus':'8','rd':'ufsroot0','ufsroot':'userdata','nord2stopat':'23'}.items())

boot=(ROOT/'research/ci-39158169212/buildtree/xnu/pexpert/pexpert/arm64/boot.h').read_text()
length=int(re.search(r'#define BOOT_LINE_LENGTH\s+(\d+)',boot)[1])
# Startup and scheduler completion must use the same exact gate.
assert s.count('nord2_smp_userland_config(stop)')==2
assert 'cpus == 8 && nord2_smp_userland_config(stop)' in s
assert 'continuing to read-only UFS lane diagnostic stop' in s
code=r'''
#define _POSIX_C_SOURCE 200809L
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BOOT_LINE_LENGTH LENGTH_VALUE
struct boot_args { char CommandLine[BOOT_LINE_LENGTH]; };
static struct boot_args ba;static bool no_boot_args;
static void *PE_boot_args(void){return no_boot_args?NULL:&ba;}
/* Numeric parser model deliberately allows noncanonical spellings and takes
 * first duplicate; diagnostic must reject these via exact production tokens. */
static int PE_parse_boot_argn(const char *key,void *value,int size){
 char copy[BOOT_LINE_LENGTH];memcpy(copy,ba.CommandLine,sizeof(copy));copy[sizeof(copy)-1]=0;
 char *save=NULL;size_t n=strlen(key);
 for(char *t=strtok_r(copy," \t\n\v\f\r",&save);t;t=strtok_r(NULL," \t\n\v\f\r",&save)){
  if(strncmp(t,key,n)||t[n]!='=')continue;
  if(!strcmp(key,"rd")||!strcmp(key,"ufsroot")){strncpy(value,t+n+1,(size_t)size);return 1;}
  assert(size==sizeof(int));*(int *)value=(int)strtol(t+n+1,0,0);return 1;
 }
 return 0;
}
'''.replace('LENGTH_VALUE',str(length))+production+r'''
static const char *required[]={"nord2stopat=23","cpus=8","rd=ufsroot0","ufsroot=userdata",
 "nord2ufs2lane=1","ufsrw=0","rootrw=0","nord2input=0","nord2wifi=0",
 "nord2wififw=0","nord2wifiemi=0","nord2fbdiag=0","nord2ufsprofile=1"};
static void add(const char *token){
 size_t n=strlen(ba.CommandLine);assert(n+strlen(token)+2<sizeof(ba.CommandLine));
 strcat(ba.CommandLine,token);strcat(ba.CommandLine," ");
}
static void bundle(int omit){memset(&ba,0,sizeof(ba));no_boot_args=false;
 for(unsigned i=0;i<sizeof(required)/sizeof(required[0]);i++)if((int)i!=omit)add(required[i]);}
static void command(const char *text){memset(&ba,0,sizeof(ba));no_boot_args=false;assert(strlen(text)<sizeof(ba.CommandLine));strcpy(ba.CommandLine,text);}
int main(void){
 bundle(-1);assert(nord2_smp_lane_diag_config()&&nord2_smp_userland_config(23));
 assert(!nord2_smp_userland_config(21)&&!nord2_smp_userland_config(22)&&!nord2_smp_userland_config(24));
 for(unsigned i=0;i<sizeof(required)/sizeof(required[0]);i++){
  bundle((int)i);assert(!nord2_smp_userland_config(23));
  add(required[i]);assert(nord2_smp_userland_config(23));
  add(required[i]);assert(!nord2_smp_userland_config(23)); // duplicate same value
  bundle((int)i);char changed[64];snprintf(changed,sizeof(changed),"%s9",required[i]);add(changed);
  assert(!nord2_smp_userland_config(23));
  bundle(-1);add(changed);assert(!nord2_smp_userland_config(23)); // contradictory duplicate
  bundle((int)i);strcpy(changed,required[i]);*strchr(changed,'=')=0;add(changed);
  assert(!nord2_smp_userland_config(23)); // bare flag, no explicit value
 }
 bundle(4);add("nord2ufs2lane=01");assert(!nord2_smp_userland_config(23));
 bundle(4);add("nord2ufs2lane=0x1");assert(!nord2_smp_userland_config(23));
 bundle(4);add("nord2ufs2lane=0");assert(!nord2_smp_userland_config(23));
 bundle(4);add("nord2ufs2lane=");assert(!nord2_smp_userland_config(23));
 bundle(-1);add("nord2ufs2lanex=9");assert(nord2_smp_userland_config(23));
 bundle(-1);for(size_t i=0;i<strlen(ba.CommandLine);i++)if(ba.CommandLine[i]==' ')ba.CommandLine[i]='\t';
 assert(nord2_smp_userland_config(23));
 // Exact608-byte unterminated input and absent boot structure must fail closed.
 memset(ba.CommandLine,'x',sizeof(ba.CommandLine));assert(!nord2_smp_userland_config(23));
 no_boot_args=true;assert(!nord2_smp_userland_config(23));
 // Ordinary existing modes remain unchanged, including their PE parser semantics.
 command("rd=ufsroot0 ufsroot=userdata ufsrw=1 rootrw=1");assert(nord2_smp_userland_config(23));
 assert(!nord2_smp_userland_config(22));
 command("rd=ufsroot0 ufsroot=userdata ufsrw=0 rootrw=0");assert(nord2_smp_userland_config(22));assert(!nord2_smp_userland_config(23));
 command("rd=md0 ufsrw=0 rootrw=0");assert(nord2_smp_userland_config(21));assert(!nord2_smp_userland_config(22));
 command("rd=ufsroot0 ufsroot=userdata ufsrw=1 rootrw=1 ufsrw=0");assert(nord2_smp_userland_config(23));
 command("rd=ufsroot0 ufsroot=userdata ufsrw=1");assert(!nord2_smp_userland_config(23));
 return 0;
}
'''
with tempfile.TemporaryDirectory(prefix='nord2-x2-runtime-') as td:
 p=Path(td);c=p/'test.c';exe=p/'test';c.write_text(code)
 subprocess.run(['clang','-std=c11','-O1','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',str(c),'-o',str(exe)],check=True)
 subprocess.run([str(exe)],check=True)
print('PASS: exact runtime SMP gate; explicit RO23 bundle; every missing/duplicate/wrong/bare arg; numeric spelling; bounded/null input; ordinary21/22/RW23 unchanged; ASan+UBSan')
