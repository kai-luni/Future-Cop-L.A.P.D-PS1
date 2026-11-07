extern void func_8008A43C(void);
extern void func_8008A3D4(void);
extern void (*D_800A3B64[])(void);

void func_8008A3A8(void)
{
    void (**p)(void) = D_800A3B64;
    asm volatile("" : "+r"(p));
    p[0] = func_8008A43C;
    p[1] = func_8008A3D4;
    p[-1] = 0;
    p[2] = 0;
}
