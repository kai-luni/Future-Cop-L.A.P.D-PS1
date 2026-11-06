extern void func_8008C9A0(void);
extern void func_8008CA10(int, void *);
extern void func_8008C9B0(void);
extern char D_800A3E00[];

int func_8008D9DC(void)
{
    func_8008C9A0();
    {
        register void *base __asm__("$5") = D_800A3E00;
        __asm__ volatile("" : "+r"(base));
        func_8008CA10(1, base);
    }
    func_8008C9B0();
    return 1;
}
