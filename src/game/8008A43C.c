extern int D_8009C778, D_8009C77C, D_8009C760, D_8009C76C, D_8009C768, D_8009C770;
extern int D_800A3B70, D_800A3B74;
extern char *D_8009C75C;
extern short *D_8009C78C;
extern void (*D_8009C728)(int);
extern int func_8008A6CC(char *);
extern void func_8008A8E0(char *);

int func_8008A43C(void)
{
    register int v asm("$3") = D_8009C778;
    int one = 1;
    register int *r asm("$1");
    asm("" : "+r"(v));
    asm("lui %0,%%hi(D_8009C790)" : "=r"(r) : : "memory");
    *(int *)((char *)r - 14448) = one;
    if (v != 0) {
        register int *p asm("$4") = &D_800A3B70;
        asm("" : "+r"(p));
        if (*p < 150) {
            *p = *p + 1;
        }
    }
    if (D_8009C77C == 0) {
        register int *q asm("$4") = &D_800A3B74;
        asm("" : "+r"(q));
        if (*q < 150) {
            *q = *q + 1;
        }
    }
    if (D_8009C760 != 0) {
        if (D_8009C778 <= D_8009C77C) {
            D_8009C76C = 0;
            D_8009C768 = D_8009C778;
            if (func_8008A6CC(D_8009C75C + D_8009C778 * 240) == 0) {
                D_8009C728(0xFFFF);
            }
            D_8009C770 = 0;
            while (D_8009C768 <= D_8009C77C) {
                func_8008A8E0(D_8009C75C + D_8009C768 * 240);
            }
            D_8009C78C[7] = 0x88;
        }
    }
    return 0;
}
