#!/usr/bin/env python3
"""Run the actual Nord2 console newline logic with instrumented row clearing."""
import pathlib
import subprocess
import tempfile

root = pathlib.Path(__file__).resolve().parent.parent
source = root / 'research/ci-39158169212/buildtree/xnu/osfmk/arm64/sweet_early.c'
s = source.read_text()
start = s.index('static void\nnord2_fb_newline(void)')
end = s.index('\nstatic void\nnord2_fb_putc', start)
code = r'''
#include <stdint.h>
#include <assert.h>
#define NORD2_FB_ROWS 32u
static uint32_t nord2_fb_row, nord2_fb_col, nord2_fastlog;
static unsigned clears, scrolls, panicking;
static int panic_active(void) { return panicking; }
static void nord2_fb_clear_row(void) { assert(nord2_fb_row < 32); clears++; }
static void nord2_fb_scroll(void) { scrolls++; nord2_fb_row = 31; }
'''+s[start:end]+r'''
int main(void) {
 for (unsigned mode = 0; mode < 2; mode++) {
  for (unsigned panic = 0; panic < 2; panic++) {
   nord2_fastlog = mode; panicking = panic;
   nord2_fb_row = nord2_fb_col = clears = scrolls = 0;
   for (unsigned n = 1; n <= 5000; n++) {
    nord2_fb_col = 67;
    nord2_fb_newline();
    assert(nord2_fb_col == 0);
    if (mode && !panic) {
     assert(nord2_fb_row == n % 32);
     assert(clears == 2*n && scrolls == 0);
    } else {
     assert(nord2_fb_row == (n < 32 ? n : 31));
     assert(scrolls == (n < 32 ? 0 : n-31));
    }
   }
  }
 }
}
'''
with tempfile.TemporaryDirectory(prefix='nord2-fastlog-') as d:
    c = pathlib.Path(d) / 'test.c'
    exe = pathlib.Path(d) / 'test'
    c.write_text(code)
    subprocess.run(['clang', '-std=c11', '-O1', '-g', '-fsanitize=address,undefined',
                    '-Wall', '-Wextra', '-Werror', str(c), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
print('PASS: actual C newline logic, 20000 lines, default/fast/panic, row bounds')
