/* MT6893 diagnostic only. No XNU payload and no storage writes. */
.section .text.head, "ax"
.global _head
_head:
    b entry
    .long 0
    .quad 0x80000
    .quad _end
    .quad 0xa
    .quad 0, 0, 0
    .ascii "ARM\x64"
    .long 0
.text
entry:
    msr daifset, #15
    mov x19, x0
    mrs x1, CurrentEL
    cmp x1, #4
#ifdef CHAIN_STOCK
    b.ne chain_stock
#else
    b.ne reset
#endif
    msr spsel, #1
    adr x1, _stack_top
    mov sp, x1
    adr x1, vectors
    msr vbar_el1, x1
    isb
    adr x1, _bss_start
    adr x2, _bss_end
1:  cmp x1, x2
    b.hs 2f
    str xzr, [x1], #8
    b 1b
2:  mov x0, x19
    bl probe_main
#ifdef CHAIN_STOCK
.global chain_stock
chain_stock:
    msr daifset, #15
    mov x1, xzr
    mov x2, xzr
    mov x3, xzr
    adr x4, _head
    add x4, x4, #0x200, lsl #12
    dsb sy
    ic iallu
    dsb sy
    isb
    br x4
#endif
reset:
    mov x0, #9
    movk x0, #0x8400, lsl #16
    smc #0
3:  wfe
    b 3b
.macro vector number
    .balign 128
    mov x0, #\number
    b probe_exception
.endm
.balign 2048
vectors:
    vector 0
    vector 1
    vector 2
    vector 3
    vector 4
    vector 5
    vector 6
    vector 7
    vector 8
    vector 9
    vector 10
    vector 11
    vector 12
    vector 13
    vector 14
    vector 15
