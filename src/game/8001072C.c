void func_8001072C(void *src, void *dst)
{
    __asm__ volatile(
        "lwc2 $0, 0($4)\n\t"
        "lwc2 $1, 4($4)\n\t"
        "nop\n\t"
        "nop\n\t"
        "cop2 0x0180001\n\t"
        "swc2 $14, 0($5)\n\t"
        "cfc2 $2, $31"
        :
        :
        : "$2", "memory"
    );
}
