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

# Compile the complete production push routine. Replace only its external MMIO,
# counter and barrier dependencies, preserving its control flow and constants.
start = s.index('static void\nnord2_fb_push(void)')
end = s.index('\nstatic void\nnord2_fb_scroll', start)
defines = '\n'.join(line for line in s.splitlines() if line.startswith('#define NORD2_DSI') or
                    line.startswith('#define NORD2_DISP_MUTEX') or line.startswith('#define NORD2_MUTEX_EN0'))
code = r'''
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
'''+defines+r'''
static uint32_t nord2_fastlog;
static uint64_t nord2_fb_pushes, nord2_fb_completed, nord2_fb_busy_timeouts;
static uint64_t nord2_fb_frame_timeouts, nord2_fb_throttle_skips, nord2_fb_last_push;
static bool nord2_fb_have_push;
static unsigned panicking, busy, generate_done, launched, starts, clears, barriers, mmio_ops;
static unsigned busy_reads, release_busy_after, completion_reads, complete_after;
static uint32_t status;
static uint64_t ticks, frequency, step;
static bool panic_active(void) { return panicking; }
static uint64_t fake_counter(const char *name) {
 if (!strcmp(name, "CNTFRQ_EL0")) return frequency;
 assert(!strcmp(name, "CNTVCT_EL0"));
 uint64_t v=ticks; ticks+=step; return v;
}
#define __builtin_arm_rsr64 fake_counter
#define __builtin_arm_dsb(v) do { assert((v)==0xf); barriers++; } while(0)
static uint32_t nord2_fb_reg_read(uintptr_t pa) {
 assert(pa==NORD2_DSI0+NORD2_DSI_INTSTA); mmio_ops++;
 if (busy) {
  busy_reads++;
  if (release_busy_after && busy_reads >= release_busy_after) busy=0;
 }
 if (launched && generate_done && ++completion_reads >= complete_after)
  status |= NORD2_DSI_FRAME_DONE;
 return status | (busy ? NORD2_DSI_BUSY : 0);
}
static void nord2_fb_reg_write(uintptr_t pa, uint32_t v) {
 mmio_ops++;
 if (pa==NORD2_DSI0+NORD2_DSI_INTSTA) {
  assert(v == (uint32_t)~NORD2_DSI_FRAME_DONE);
  status &= v; clears++; return;
 }
 if (pa==NORD2_DISP_MUTEX+NORD2_MUTEX_EN0) { assert(v==1); return; }
 assert(pa==NORD2_DSI0+NORD2_DSI_START);
 assert(!busy); // No START write, including START=0, is legal while busy.
 assert(v<=1);
 if (v) {
  assert(!(status & NORD2_DSI_FRAME_DONE)); // stale DONE was cleared first
  assert(barriers==starts+1); starts++; launched=1;
 }
}
static void reset(void) {
 nord2_fastlog=panicking=busy=launched=starts=clears=barriers=mmio_ops=0;
 busy_reads=release_busy_after=completion_reads=0;
 generate_done=1; complete_after=3; status=1u<<7;
 ticks=0; frequency=1000000; step=100;
 nord2_fb_pushes=nord2_fb_completed=nord2_fb_busy_timeouts=0;
 nord2_fb_frame_timeouts=nord2_fb_throttle_skips=nord2_fb_last_push=0;
 nord2_fb_have_push=false;
}
'''+s[start:end]+r'''
int main(void) {
 reset(); status |= NORD2_DSI_FRAME_DONE; nord2_fb_push();
 assert(starts==1 && clears==2 && completion_reads==3);
 assert(status==(1u<<7) && nord2_fb_completed==1);
 // Stale DONE alone cannot masquerade as completion of the new frame.
 reset(); status |= NORD2_DSI_FRAME_DONE; generate_done=0; nord2_fb_push();
 assert(starts==1 && nord2_fb_completed==0 && nord2_fb_frame_timeouts==1);
 assert(status==(1u<<7));
 reset(); busy=1; nord2_fb_push();
 assert(starts==0 && clears==0 && barriers==0 && nord2_fb_busy_timeouts==1);
 assert(ticks<=10200);
 reset(); busy=1; release_busy_after=3; nord2_fb_push();
 assert(starts==1 && nord2_fb_completed==1 && busy_reads==3);
 reset(); generate_done=0; nord2_fb_push();
 assert(starts==1 && nord2_fb_frame_timeouts==1 && ticks<=20500);
 // Stalled counters retain independent hard iteration bounds.
 reset(); busy=1; step=0; nord2_fb_push();
 assert(nord2_fb_busy_timeouts==1 && busy_reads==1000000 && starts==0);
 reset(); generate_done=0; step=0; nord2_fb_push();
 assert(nord2_fb_frame_timeouts==1 && starts==1);
 reset(); frequency=0; nord2_fb_push();
 assert(nord2_fb_busy_timeouts==1 && mmio_ops==0);
 // Fast-mode gate including exact 100ms boundary and failure retry throttle.
 reset(); nord2_fastlog=1; step=0; nord2_fb_push();
 unsigned ops=mmio_ops; ticks=99999; nord2_fb_push();
 assert(nord2_fb_pushes==1 && nord2_fb_throttle_skips==1 && mmio_ops==ops);
 ticks=100000; nord2_fb_push();
 assert(nord2_fb_pushes==2 && nord2_fb_completed==2);
 reset(); nord2_fastlog=1; busy=1; nord2_fb_push();
 ops=mmio_ops; nord2_fb_push();
 assert(nord2_fb_pushes==1 && nord2_fb_throttle_skips==1 && mmio_ops==ops);
 // Default output and panic bypass rate gating; both retain safe transaction order.
 for (unsigned panic=0; panic<2; panic++) {
  reset(); step=0; nord2_fastlog=panic; panicking=panic;
  nord2_fb_push(); nord2_fb_push();
  assert(nord2_fb_pushes==2 && nord2_fb_completed==2 && !nord2_fb_throttle_skips);
 }
 // Counter wrapping uses unsigned elapsed deltas, including the rate gate.
 reset(); nord2_fastlog=1; step=0; ticks=UINT64_MAX-50000; nord2_fb_push();
 ticks=49998; nord2_fb_push(); assert(nord2_fb_throttle_skips==1);
 ticks=49999; nord2_fb_push(); assert(nord2_fb_completed==2);
}
'''
with tempfile.TemporaryDirectory(prefix='nord2-push-') as d:
    c = pathlib.Path(d) / 'test.c'
    exe = pathlib.Path(d) / 'test'
    c.write_text(code)
    subprocess.run(['clang', '-std=c11', '-O1', '-g', '-fsanitize=address,undefined',
                    '-Wall', '-Wextra', '-Werror', str(c), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
print('PASS: actual C push, W0C/stale DONE/BUSY/timeouts/stalled clock/10Hz/panic/default/wrap')
