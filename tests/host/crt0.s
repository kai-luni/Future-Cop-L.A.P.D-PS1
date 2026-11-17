.set noreorder
.globl __start
.text
__start:
    lw      $4, 0($sp)
    addu    $5, $sp, 4
    jal     main
    nop
    move    $4, $2
    li      $2, 4001
    syscall