// Categories: Platform > Memory Card | Resources > File Access
typedef unsigned char u8;
typedef struct { int a, b, c, pos, e, f; } Entry;
typedef struct { int state, a, b, c, index, d, callback, saved, buffer; } State;
extern State func_80010000;
extern Entry D_800A6D78[];
extern int D_8009F610[], D_8009F614[], D_8009F61C[];
extern u8 gEventBuffer[] __asm__("D_800A0798");
extern volatile u8 D_1F801800, D_1F801803;
extern void func_8001C494(int *, int);
extern int func_8008E174(int, int);
extern int func_8008E1BC(int, void *, void *);
extern void func_8008CC80(int);
extern int func_8008E194(void *);
extern void func_8001C614(void);
extern void func_8001C27C(int *, int *);
extern int func_8008E2F8(int, void *);
/**
 * @brief Reads the directory block from the memory card into the loader buffer.
 *
 * @return Nothing.
 *
 * @par Confirmed flow (follow-up of func_8001C680)
 * 1. The loader's current file slot is taken from D_800A6D78[func_80010000.index]
 *    and its position is passed to func_8001C494 together with the card buffer
 *    D_8009F614.
 * 2. The card interface is polled with func_8008E174(1, 0), the control
 *    registers D_1F801800/D_1F801803 are cleared and func_8008E1BC(0xE, ...)
 *    starts a directory read; a failure raises func_8008CC80(-1).
 * 3. The loader state (at 0x80010000, i.e. the first word of the program image)
 *    is set to 1 and func_8008E194(&func_8001C614) registers the callback that
 *    finishes the sequence.
 * 4. func_8001C27C(&D_8009F61C, &D_8009F614) refreshes the two buffers and
 *    func_8008E2F8(6, ...) opens the card.
 *
 * @par Globals
 * D_8009F610, D_8009F614, D_8009F61C (card buffers), D_800A6D78 (loader slots),
 * D_80010000 (loader state block).
 *
 * @par Callers
 * func_8001C808.
 */
void func_8001C718(void)
{
    register State *state __asm__("$16");
    int result;
    register int saved __asm__("$2");
    register State *p __asm__("$3");
    register int a0 __asm__("$4");
    register char *a1 __asm__("$5");
    register char *a2 __asm__("$6");
    a1 = (char *)D_800A6D78[func_80010000.index].pos;
    __asm__ volatile("" : "+r"(a1));
    a0 = (int)D_8009F614;
    __asm__ volatile("" : "+r"(a0));
    func_8001C494((int *)a0, (int)a1);
    while (func_8008E174(1, 0)) {}
    a0 = 14;
    a1 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a1));
    a1 -= 0x9f0;
    a2 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a0), "+r"(a1), "+r"(a2));
    D_1F801800 = 0;
    D_1F801803 = 0;
    __asm__ volatile("" : "+r"(a2) : : "memory");
    result = func_8008E1BC(a0, a1, a2 + 0x798);
    state = (State *)0x80010000;
    if (!result)
        func_8008CC80(-1);
    __asm__ volatile("" : "+r"(state));
    a0 = (int)func_8001C614;
    __asm__ volatile("" : "+r"(a0));
    state->state = 1;
    __asm__ volatile("" : : : "memory");
    saved = func_8008E194((void *)a0);
    __asm__ volatile("" : "+r"(saved));
    a0 = 0x800a0000;
    __asm__ volatile("" : "+r"(a0));
    a0 -= 0x9e4;
    __asm__ volatile("" : "+r"(a0));
    a1 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a1));
    a1 -= 0x9ec;
    __asm__ volatile("" : "+r"(a1), "+r"(saved));
    __asm__ volatile("addiu %0,%1,0" : "=r"(p) : "r"(state));
    p->saved = saved;
    func_8001C27C((int *)a0, (int *)a1);
    a1 = (char *)0x800a0000;
    __asm__ volatile("" : "+r"(a1));
    a1 -= 0x9ec;
    __asm__ volatile("" : "+r"(a1));
    if (!func_8008E2F8(6, a1))
        state->state = 0;
}
