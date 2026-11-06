extern void func_8008C444(void);
extern void func_8008C858(void);
extern void func_8008C518(void);
extern void (*D_8009C740)(void);
extern void (*D_8009C744)(void);

void func_8008C410(void)
{
    D_8009C740 = func_8008C444;
    D_8009C744 = func_8008C858;
    {
        register char *base __asm__("$1");
        register void (*value)(void) __asm__("$2");
        __asm__("" ::: "memory");
        value = func_8008C518;
        __asm__("" : "+r" (value) : : "memory");
        base = (char *)0x800a0000;
        __asm__("" : "+r" (base));
        *(void (**)(void))(base - 14520) = value;
    }
}
