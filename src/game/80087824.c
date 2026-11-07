typedef struct State {
    char pad[0x7B10];
    int callback;
    int irq;
    int other;
    int enabled;
    char pad2[0x1C];
    int one;
    char pad3[0x18];
    int zero;
    int a;
    int b;
    void *c;
    void *d;
    char pad4[0x10];
    int e;
} State;
extern volatile State *D_8009F4D8;
extern void *func_8001E4C0(void *, int, unsigned int);
extern void func_80086C74(void *, int, int);
extern void func_8008C9C0(void);
extern void func_8008C9E0(void);
extern int func_80098BF8(void *);
extern int func_80098FB8(void *);
extern int func_80091510(void *);
extern char D_80086950[];
extern char D_80086D74[];
int func_80087824(State *state, void *arg_c, int arg_a, void *arg_d, int arg_b, int arg_e, int arg_enabled)
{
    register void *c __asm__("$19") = arg_c;
    register int a __asm__("$17") = arg_a;
    register int b __asm__("$18") = arg_b;
    register void *d __asm__("$20") = arg_d;
    register int e __asm__("$16") = arg_e;
    register int enabled __asm__("$21") = arg_enabled;
    register int zero __asm__("$0");
    unsigned int i;
    if (state) {
    if (!a || !b || !c || !d) return 0;
    D_8009F4D8 = state;
    func_8001E4C0(state, 0, 0x7B80);
    for (i = 0; i < 0x300; i += 0x10)
        *(i + (char *)D_8009F4D8 + 0x7811) = 2;
    D_8009F4D8->one = 1;
    D_8009F4D8->zero = 0;
    D_8009F4D8->a = a;
    D_8009F4D8->b = b;
    D_8009F4D8->c = c;
    D_8009F4D8->d = d;
    D_8009F4D8->e = e;
    func_80086C74(D_8009F4D8->c, D_8009F4D8->a, e);
    func_80086C74(D_8009F4D8->d, D_8009F4D8->b, e);
    func_8008C9C0();
    D_8009F4D8->irq = func_80098BF8(0);
    D_8009F4D8->other = func_80098FB8(D_80086950);
    D_8009F4D8->enabled = enabled;
    if (enabled) D_8009F4D8->callback = func_80091510(D_80086D74);
    else { __asm__ volatile ("" : "=r"(zero)); D_8009F4D8->callback = zero; }
    func_8008C9E0();
    return 1;
    }
    return 0;
}
