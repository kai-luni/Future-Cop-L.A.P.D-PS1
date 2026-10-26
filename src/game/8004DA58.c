int func_8004DA58(unsigned int value)
{
    register unsigned int test __asm__("$2");

    test = value - 0x3C;
    if (test < 10) {
        goto matched;
    }
    test = value - 0x64;
    if (test < 2) {
        goto matched;
    }
    return 0;

matched:
    return 1;
}
