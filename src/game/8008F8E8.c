typedef unsigned char u8;
typedef unsigned int u32;

extern char D_8001257C[];
extern char D_80012588[];
extern char D_8009CB88[];
extern void D_8008FE28(void);
extern u8 D_8009C8C5, D_8009C8C4;
extern u32 D_8009C8AC, D_8009C8A8, D_8009C8B8, D_8009C8B4;
extern volatile u8 *D_8009CB6C, *D_8009CB78, *D_8009CB74;
extern u32 *D_8009CB7C;
extern volatile u8 D_8009CB84[];
extern void func_8008FF08(char *);
extern void func_8008E000(char *, char *);
extern void func_80091480(void);
extern void func_800914B0(int, void (*)(void));
extern int func_8008F244(int, int, int, int);
extern int func_8008ECFC(int, int);

int func_8008F8E8(void)
{
    register int z __asm__("$0");
    register volatile u8 *state __asm__("$3");
    register char *first __asm__("$4");
    first = D_8001257C;
    __asm__("" : "+r"(first));
    func_8008FF08(first);
    func_8008E000(D_80012588, D_8009CB88);
    D_8009C8C5 = 0;
    __asm__ volatile("" : : : "memory");
    D_8009C8C4 = 0;
    __asm__ volatile("" : : : "memory");
    D_8009C8AC = 0;
    __asm__ volatile("" : : : "memory");
    D_8009C8A8 = 0;
    __asm__ volatile("" : : : "memory");
    D_8009C8B8 = 0;
    __asm__ volatile("" : : : "memory");
    {
        register u32 *p __asm__("$1") = (u32 *)0x800A0000;
        __asm__("" : "+r"(p));
        *(u32 *)((char *)p - 0x374c) = 0;
        func_80091480();
    }
    {
        register void (*cb)(void) __asm__("$5") = D_8008FE28;
        __asm__("" : "+r"(cb));
        func_800914B0(2, cb);
    }
    *D_8009CB6C = 1;
    while (*D_8009CB78 & 7) {
        *D_8009CB6C = 1;
        *D_8009CB78 = 7;
        *D_8009CB74 = 7;
    }
    {
        register int a0 __asm__("$4") = 1;
        register int a1 __asm__("$5") = 0;
        register volatile u8 *port __asm__("$7");
        __asm__("" : "+r"(a0), "+r"(a1));
        state = D_8009CB84;
        state[2] = 0;
        state[1] = state[2];
        port = D_8009CB6C;
        __asm__("" : "+r"(state), "+r"(port));
        state[0] = 2;
        *port = 0;
        *D_8009CB78 = 0;
        *D_8009CB7C = 0x1325;
        func_8008F244(a0, a1, z, z);
    }
    if (D_8009C8B4 & 0x10)
        func_8008F244(1, 0, z, z);
    if (func_8008F244(10, 0, z, z))
        return -1;
    if (func_8008F244(12, 0, z, z))
        return -1;
    if (func_8008ECFC(0, z) != 2)
        return -1;
    return 0;
}
