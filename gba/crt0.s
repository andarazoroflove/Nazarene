    .arm
    .section .text.entry, "ax"
    .global _start
_start:
    b       init_arm
    /* GBA Header Space (156 bytes logo + 36 bytes header = 192 bytes total) */
    .space  0xB8, 0

init_arm:
    /* Set IRQ stack pointer */
    mov     r0, #0x12       /* IRQ mode */
    msr     cpsr, r0
    ldr     sp, =__sp_irq

    /* Set System/User stack pointer */
    mov     r0, #0x1F       /* System mode */
    msr     cpsr, r0
    ldr     sp, =__sp_usr

    /* Copy .data from ROM to IWRAM */
    ldr     r0, =__data_load
    ldr     r1, =__data_start
    ldr     r2, =__data_end
1:  cmp     r1, r2
    ldrlo   r3, [r0], #4
    strlo   r3, [r1], #4
    blo     1b

    /* Clear .bss */
    mov     r0, #0
    ldr     r1, =__bss_start
    ldr     r2, =__bss_end
2:  cmp     r1, r2
    strlo   r0, [r1], #4
    blo     2b

    /* Jump to main */
    ldr     r3, =main
    bx      r3

hang:
    b       hang

