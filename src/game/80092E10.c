typedef unsigned int u32;
typedef struct { int pad[2]; void (*fn)(void *, void *, int, u32); void *ctx; } Device;
extern Device *D_8009DDE8;
extern char D_80012A7C[];
extern void func_80092CF4(char *, void *);
void func_80092E10(void *arg0, int arg1, int arg2, int arg3)
{
    register void *a __asm__("$19") = arg0;
    register int b __asm__("$18");
    register int c __asm__("$17");
    register int d __asm__("$16");
    register void *a5 __asm__("$5");
    register char *fmt __asm__("$4") = D_80012A7C;
    __asm__("" : "+r"(a), "+r"(fmt));
    b = arg1;
    a5 = a;
    __asm__("" : "+r"(b), "+r"(a5));
    c = arg2;
    __asm__("" : "+r"(c));
    d = arg3;
    func_80092CF4(fmt, a5);
    a5 = a;
    __asm__("" : "+r"(a5));
    d = (d & 255) << 16;
    __asm__("" : "+r"(d));
    d |= (c & 255) << 8;
    __asm__("" : "+r"(d));
    b &= 255;
    __asm__("" : "+r"(b));
    D_8009DDE8->fn(D_8009DDE8->ctx, a5, 8,
                     d | b);
}
