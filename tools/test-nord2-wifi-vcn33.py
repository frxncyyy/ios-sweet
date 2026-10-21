#!/usr/bin/env python3
"""Test actual VCN33 rail configuration and native binding with synthetic PMIF."""
from pathlib import Path
import subprocess,tempfile
R=Path(__file__).resolve().parents[1];P=R/'tools/fixtures/nord2-wifi-vcn33';S=R/'shim/nord2-xnu'
with tempfile.TemporaryDirectory() as t:
 subprocess.run(['clang','-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(S),str(P/'test.c'),str(S/'wifi_vcn33.c'),'-o',t+'/test'],check=True)
 subprocess.run([t+'/test'],check=True)
 subprocess.run(['clang','-std=c11','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(S),str(P/'test-bind.c'),str(S/'wifi_vcn33.c'),'-o',t+'/bind'],check=True)
 subprocess.run([t+'/bind'],check=True)
 subprocess.run(['clang','--target=aarch64-none-elf','-ffreestanding','-std=c11','-Wall','-Wextra','-Werror','-c',str(S/'wifi_vcn33.c'),'-o',t+'/test.o'],check=True)
