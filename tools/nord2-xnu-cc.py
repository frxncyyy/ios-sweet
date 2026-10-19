#!/usr/bin/env python3
"""Compile one xnu source with the flags the CI build used, without the CI log.

  nord2-xnu-cc.py BUILDTREE COMPONENT SOURCE OUT.o [EXTRA ...]

BUILDTREE  extracted xnu-buildtree artifact (build/xnu.obj, xnu/, fakeroot/,
           sdk-kernel-platform/)
COMPONENT  osfmk, bsd, pexpert, ... (selects the generated Makefile dir)
SOURCE     .c / .s file (may live outside BUILDTREE, e.g. a patched copy)
EXTRA      appended after the generated flags (-D..., -U...)

The flag set is assembled the way makedefs/MakeInc.{def,rule} do it for
`make SDKROOT=macosx TARGET_CONFIGS="DEVELOPMENT ARM64 SWEET"` (darwin-xnu-build):
CFLAGS_GEN + MACHINE_FLAGS_ARM64_SWEET + CFLAGS_ARM64 (PLATFORM=MacOSX, so
ARM_LARGE_MEMORY=1 and the 0xfffffe0007004000 link address) + -O2 +
deployment flags + DEFINES (incl. CONFIG_DEFINES from the generated Makefile),
then the per-component Makefile additions and INCFLAGS.  Apple's -arch arm64
-mmacosx-version-min=14.0 becomes -target arm64-apple-macos14.0 for upstream
clang.  Env CLANG overrides the compiler; DRY=1 prints the argv as JSON.
"""
import json, os, re, shlex, subprocess, sys

COMPONENTS = ('osfmk', 'bsd', 'libkern', 'iokit', 'pexpert', 'libsa', 'security', 'san')
# Component Makefile.template additions (CFLAGS+= / SFLAGS+=)
COMPONENT_CFLAGS = {
	'osfmk': ['-include', 'meta_features.h', '-DMACH_KERNEL_PRIVATE', '-DMACH_KERNEL'],
}
COMPONENT_SFLAGS = {
	'osfmk': ['-include', 'meta_features.h'],
}
LINK_ADDR = '0xfffffe0007004000'


def config_defines(makefile):
	text = open(makefile, errors='replace').read()
	m = re.search(r'^export CONFIG_DEFINES = (.*)$', text, re.M)
	return shlex.split(m.group(1))


def argv(bt, comp, src, out, extra):
	obj = os.path.join(bt, 'build/xnu.obj')
	cfgdir = os.path.join(obj, 'DEVELOPMENT_ARM64_SWEET', comp, 'DEVELOPMENT')
	srcroot = os.path.join(bt, 'xnu')
	asm = src.endswith('.s')
	arm64 = ['-Darm64', '-DARM64', '-D__ARM64__', '-DLP64', '-DPAGE_SIZE_FIXED',
		'-DVM_KERNEL_LINK_ADDRESS=' + LINK_ADDR, '-DARM_LARGE_MEMORY=1',
		'-momit-leaf-frame-pointer', '-fno-strict-aliasing', '-D__API__=v4', '-mkernel']
	machine = ['-DARM64_BOARD_CONFIG_SWEET', '-march=armv8.2-a+crypto']
	deploy = ['-target', 'arm64-apple-macos14.0', '-DXNU_TARGET_OS_OSX', '-DXNU_PLATFORM_MacOSX']
	defines = ['-DAPPLE', '-DKERNEL', '-DKERNEL_PRIVATE', '-DXNU_KERNEL_PRIVATE',
		'-DPRIVATE', '-D__MACHO__=1', '-Dvolatile=__volatile', '-DXNU_KERN_EVENT_DATA_IS_VLA',
		'-DCURRENT_MACHINE_CONFIG_LC=sweet'] + config_defines(os.path.join(cfgdir, 'Makefile'))
	if asm:
		flags = ['-D__ASSEMBLER__', '-DASSEMBLER'] + machine + arm64 + deploy + defines \
			+ COMPONENT_SFLAGS.get(comp, [])
	else:
		gen = ['-g', '-nostdinc', '-ferror-limit=10000', '-fno-builtin', '-fno-common',
			'-ftrivial-auto-var-init=zero', '-fsigned-bitfields', '-fmerge-all-constants',
			'-fno-c++-static-destructors']
		flags = gen + machine + arm64 + ['-O2'] + deploy + defines \
			+ COMPONENT_CFLAGS.get(comp, []) + ['-w']
	inc = ['-I.', '-I' + os.path.join(srcroot, comp), '-I' + os.path.join(obj, 'EXPORT_HDRS', comp)]
	inc += ['-I' + os.path.join(obj, 'EXPORT_HDRS', c) for c in COMPONENTS if c != comp]
	inc += ['-I' + os.path.join(srcroot, 'EXTERNAL_HEADERS')]
	if comp == 'osfmk':
		inc += ['-I' + os.path.join(srcroot, 'osfmk', 'libsa')]
	inc += ['-I' + os.path.join(bt, 'fakeroot/usr/local/include/kernel'),
		'-I' + os.path.join(bt, 'sdk-kernel-platform/platform')]
	cc = os.environ.get('CLANG', 'clang')
	return cfgdir, [cc, '-c'] + flags + inc + list(extra) + [os.path.abspath(src), '-o', os.path.abspath(out)]


def main():
	if len(sys.argv) < 5:
		sys.exit(__doc__)
	bt, comp, src, out = (os.path.abspath(sys.argv[1]), sys.argv[2], sys.argv[3], sys.argv[4])
	cwd, cmd = argv(bt, comp, src, out, sys.argv[5:])
	if os.environ.get('DRY'):
		print(json.dumps({'cwd': cwd, 'argv': cmd}, indent=1))
		return
	sys.exit(subprocess.run(cmd, cwd=cwd).returncode)


if __name__ == '__main__':
	main()
