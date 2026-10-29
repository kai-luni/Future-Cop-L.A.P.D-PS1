void func_8008C9C0(void) {
    __asm__ volatile (
        "mfc0 $4, $12\n\t"
        "nop\n\t"
        "addiu $1, $0, -0x402\n\t"
        "and $4, $4, $1\n\t"
        "mtc0 $4, $12\n\t"
        "nop\n\t"
    );
}
