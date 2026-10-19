/* Nord 2 XNU early-probe loader. No XNU payload is built by this file. */
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
    b.ne halt
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
    bl xnu_main
    /* xnu_main only returns on a fatal loader error. */
halt:
    msr daifset, #15
3:  wfe
    b 3b
.macro vector number
    .balign 128
    mov x0, #\number
    b loader_exception
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
