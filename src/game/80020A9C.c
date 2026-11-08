extern int func_800207B4(int);

int func_80020A9C(int arg0)
{
    register int value __asm__("$4");
    register int result __asm__("$2");

    value = func_800207B4(arg0);
    if (value == -2)
        return 1;
    if (value == -3) {
        value = func_800207B4(arg0);
        if (value == -2)
            return 1;
        result = 1;
        __asm__("" : "+r"(result));
        if (value == -1)
            return result;
        return (unsigned int)~value >> 31;
    }
    if (value == -1)
        return 1;
    return (unsigned int)~value >> 31;
}
