typedef unsigned char u8;
extern int D_800A09D4;

void func_80022D1C(u8 *data, int mode, int offset)
{
    register int index __asm__("$2");
    register u8 *ptr __asm__("$3");
    if (mode == 1) goto one;
    if (mode == 0) goto zero;
    if (mode == 3) goto three;
    goto done;
zero:
    data[1] |= 4;
    goto done;
one:
    index = D_800A09D4 - 15;
    __asm__("" : "=r"(index) : "0"(index));
    ptr = data + index;
    __asm__("" : "=r"(ptr) : "0"(ptr));
    *ptr |= 1;
    goto done;
three:
    data[offset - 15] = 1;
done:
    return;
}
