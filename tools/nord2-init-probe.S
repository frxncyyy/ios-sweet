/*
 * Bounded PID1 diagnostic for an HFS RAM root. No UFS access.
 * Build with clang -target arm64-apple-ios16.0, then tools/mkinit.py.
 *
 * Success requires getpid()==1 and an exact read of /nord2-probe.txt.
 * Exiting PID1 deliberately panics XNU; the Nord2 panic hook saves the log
 * and returns to the preserved Android kernel. Exit 66 is the expected end,
 * not a boot failure. Other exit codes indicate a failed probe.
 */
#define SYS_exit   1
#define SYS_read   3
#define SYS_write  4
#define SYS_open   5
#define SYS_getpid 20
#define SYS_select_nocancel 407

	.text
	.globl	_start
	.p2align 2
_start:
	mov	x9, sp
	and	x9, x9, #~15
	mov	sp, x9
	sub	sp, sp, #64

	adr	x0, console
	mov	x1, #2
	mov	x2, #0
	mov	x16, #SYS_open
	svc	#0x80
	b.cs	fail_console
	mov	x19, x0

	mov	x16, #SYS_getpid
	svc	#0x80
	b.cs	fail_pid
	cmp	x0, #1
	b.ne	fail_pid

	adr	x0, filename
	mov	x1, #0
	mov	x2, #0
	mov	x16, #SYS_open
	svc	#0x80
	b.cs	fail_open
	mov	x1, sp
	mov	x2, #32
	mov	x16, #SYS_read
	svc	#0x80
	b.cs	fail_read
	adr	x9, expected
	adr	x10, expected_end
	sub	x10, x10, x9
	cmp	x0, x10
	b.ne	fail_read
	mov	x11, sp
compare:
	ldrb	w12, [x9], #1
	ldrb	w13, [x11], #1
	cmp	w12, w13
	b.ne	fail_contents
	subs	x10, x10, #1
	b.ne	compare

	mov	x0, x19
	adr	x1, success
	adr	x2, success_end
	sub	x2, x2, x1
	mov	x20, x2
	mov	x16, #SYS_write
	svc	#0x80
	b.cs	fail_console
	cmp	x0, x20
	b.ne	fail_console
#ifdef NORD2_LIVE_PROBE
	/* Ten timer sleeps prove that USB polling and EL0 both keep running. */
	mov	x21, #10
live_tick:
	mov	x9, #1
	str	x9, [sp, #32]		/* user64_timeval.tv_sec */
	str	xzr, [sp, #40]		/* tv_usec and padding */
	mov	x0, #0
	mov	x1, #0
	mov	x2, #0
	mov	x3, #0
	add	x4, sp, #32
	mov	x16, #SYS_select_nocancel
	svc	#0x80
	b.cs	fail_sleep
	cbnz	x0, fail_sleep
	mov	x0, x19
	adr	x1, live_message
	adr	x2, live_message_end
	sub	x2, x2, x1
	mov	x20, x2
	mov	x16, #SYS_write
	svc	#0x80
	b.cs	fail_console
	cmp	x0, x20
	b.ne	fail_console
	subs	x21, x21, #1
	b.ne	live_tick
#endif
	mov	x0, #66
	b	finish
fail_pid:
	mov	x0, #70
	b	failed
fail_open:
	mov	x0, #71
	b	failed
fail_read:
	mov	x0, #72
	b	failed
fail_contents:
	mov	x0, #73
	b	failed
fail_console:
	mov	x0, #74
	b	finish
#ifdef NORD2_LIVE_PROBE
fail_sleep:
	mov	x0, #75
	b	failed
#endif
failed:
	mov	x20, x0
	mov	x0, x19
	adr	x1, failure
	adr	x2, failure_end
	sub	x2, x2, x1
	mov	x16, #SYS_write
	svc	#0x80
	mov	x0, x20
finish:
	mov	x16, #SYS_exit
	svc	#0x80
	b	finish

console:
	.asciz	"/dev/console"
filename:
	.asciz	"/nord2-probe.txt"
expected:
	.ascii	"NORD2_HFS_READ_OK\n"
expected_end:
success:
	.ascii	"\nNORD2 USERLAND: HFS read verified; getpid=1; EL0 syscalls OK\n"
#ifdef NORD2_LIVE_PROBE
	.ascii	"NORD2 USERLAND: starting ten timed USB live-log ticks\n"
#else
	.ascii	"NORD2 USERLAND: deliberate exit 66; diagnostic complete\n"
#endif
success_end:
#ifdef NORD2_LIVE_PROBE
live_message:
	.ascii	"NORD2 USERLAND: live tick; PID1 and USB polling running\n"
live_message_end:
#endif
failure:
	.ascii	"\nNORD2 USERLAND: diagnostic FAILED; see PID1 exit code\n"
failure_end:
