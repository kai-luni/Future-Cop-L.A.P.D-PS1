typedef unsigned char u8;
extern u8 D_8009DDF2[];
extern char D_800129D4[];
extern void (*D_8009DDEC)(char *, int, int, int);

int func_80092A84(int value)
{
    register u8 *p __asm__("$3") = D_8009DDF2;
    int old;
    __asm__ volatile("" : "+r"(p));
    old = p[0];
    p[0] = value;
    if ((u8)value) {
        register void (*fn)(char *, int, int, int) __asm__("$2") = D_8009DDEC;
        register int a1 __asm__("$5") = p[0];
        register int a2 __asm__("$6") = p[-2];
        register int a3 __asm__("$7") = p[1];
        __asm__ volatile("" : "+r"(fn), "+r"(a1), "+r"(a2), "+r"(a3));
        fn(D_800129D4, a1, a2, a3);
    }
    return old;
}
