extern unsigned int D_80076A0C[];
extern void func_8008C9C0(void);
extern void func_8008C9E0(void);

void func_800772E0(void)
{
    unsigned int *src = D_80076A0C;
    unsigned int *dst = (unsigned int *)0x80;
    int i;

    func_8008C9C0();
    for (i = 0; i < 8; i++) {
        *dst++ = *src++;
    }
    func_8008C9E0();
}
