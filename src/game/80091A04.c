typedef unsigned short u16;
typedef unsigned int u32;
extern char D_8009CC88[];
extern volatile u16 *D_8009DD14;
extern volatile u16 *D_8009DD18;
extern volatile u32 *D_8009DD1C;
extern void func_8008C9A0(void);
extern void func_80091B70(void);

void *func_80091A04(void)
{
    register char *p __asm__("$16") = D_8009CC88;
    volatile u16 *a;
    volatile u16 *b;
    volatile u32 *c;
    u32 temp;
    __asm__("" : "+r"(p));
    if (*(u16 *)p == 0)
        return 0;
    func_8008C9A0();
    b = D_8009DD18;
    c = D_8009DD1C;
    *(u16 *)(p + 0x32) = *b;
    temp = *c;
    a = D_8009DD14;
    *(u32 *)(p + 0x34) = temp;
    *b = 0;
    *a = *b;
    *(u32 *)D_8009DD1C = *D_8009DD1C & 0x77777777;
    func_80091B70();
    *(u16 *)p = 0;
    return p;
}
