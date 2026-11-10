__asm__(".section .text.target,\"ax\",@progbits\n\t.balign 4");
typedef unsigned char u8;
typedef int s32;
typedef unsigned int u32;

typedef struct Callbacks {
    void *pad[6];
    void (*unk18)(s32);
} Callbacks;

extern u8 D_8009DDF2;
extern char D_80012AE8[];
extern void (*D_8009DDEC)(char *, s32);
extern volatile u32 *D_8009DED4, *D_8009DEC8;
extern u32 D_8009DEFC, D_8009DF00;
extern Callbacks *D_8009DDE8;
extern int func_80091270(int);
extern int func_8009520C(void);
extern void func_800914E0(int, void (*)(void));
extern void D_80095808(void);

int func_8009570C(s32 arg0)
{
    int result;
    if (D_8009DDF2 >= 2) {
        register char *msg __asm__("$4");
        __asm__("lui %0,%%hi(D_80012AE8)\n\taddiu %0,%0,%%lo(D_80012AE8)" : "=r"(msg));
        D_8009DDEC(msg, arg0);
    }
    D_8009DEFC = func_80091270(-1) + 0xF0;
    D_8009DF00 = 0;
    if (!((*D_8009DED4 & 0x01000000) || !(*D_8009DEC8 & 0x04000000))) goto success;
loop:
    if (func_8009520C()) { result = -1; goto done; }
check:
    if (*D_8009DED4 & 0x01000000) goto loop;
    if (!(*D_8009DEC8 & 0x04000000)) goto loop;
success:
    {
        register void (*callback)(void) __asm__("$5");
        __asm__("lui %0,%%hi(D_80095808)\n\taddiu %0,%0,%%lo(D_80095808)" : "=r"(callback));
        func_800914E0(2, callback);
    }
    D_8009DDE8->unk18(arg0);
    result = 0;
done:
    __asm__ volatile(".set reorder");
    return result;
}
