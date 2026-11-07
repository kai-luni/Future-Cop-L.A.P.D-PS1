typedef unsigned int u32;
extern int D_8009DEFC, D_8009DF00;
extern volatile u32 D_8009DEEC;
extern u32 D_8009DEE8, D_8009DEF8;
extern volatile u32 *D_8009DEC8, *D_8009DECC, *D_8009DED4, *D_8009DEE4;
extern char D_80012B48[];
extern int func_80091270(int);
extern u32 func_800915FC(u32);
extern void func_8008E000(char *, u32, u32, u32, u32);

int func_8009520C(void)
{
    u32 saved;
    register u32 count __asm__("$5");
    register int *counter __asm__("$3");
    register volatile u32 *port __asm__("$6");
    register char *message __asm__("$4");
    if (D_8009DEFC < func_80091270(-1)) goto fail;
    __asm__("lui %0,%%hi(D_8009DF00)\n\taddiu %0,%0,%%lo(D_8009DF00)" : "=r"(counter));
    if ((*counter)++ > 0xF0000) {
fail:
        __asm__ volatile("lui %0,%%hi(D_8009DEC8)\n\tlw %0,%%lo(D_8009DEC8)(%0)\n\tlui %1,%%hi(D_80012B48)\n\taddiu %1,%1,%%lo(D_80012B48)\n\tlw $2,0(%0)" : "=r"(port), "=r"(message));
        count = D_8009DEE8;
        func_8008E000(message, (count - D_8009DEEC) & 0x3f, *port, *D_8009DED4, *D_8009DECC);
        saved = func_800915FC(0);
        D_8009DEEC = 0;
        D_8009DEF8 = saved;
        D_8009DEE8 = D_8009DEEC;
        *D_8009DED4 = 0x401;
        *D_8009DEE4 |= 0x800;
        *D_8009DEC8 = 0x2000000;
        *D_8009DEC8 = 0x1000000;
        func_800915FC(D_8009DEF8);
        return -1;
    }
    return 0;
}
