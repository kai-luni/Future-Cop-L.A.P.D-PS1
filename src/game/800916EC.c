typedef unsigned short u16;
typedef unsigned int u32;
typedef void (*Callback)(void);
struct State { u16 active, busy; Callback handlers[11]; u16 mask; };
extern struct State D_8009CC88;
extern u16 D_8009CCB8, D_8009CC8A;
extern u16 *D_8009DD14, *D_8009DD18;
extern int D_8009DD20;
extern char D_80012830[], D_8001284C[];
extern void func_8008E000(char *, ...);
extern void func_80091B60(void);
void func_800916EC(void)
{
    register struct State *state __asm__("$17") = &D_8009CC88;
    u16 pending;
    register u16 mask __asm__("$3");
    register u16 *pa1 __asm__("$4");
    register u16 *pb1 __asm__("$2");
    register int i __asm__("$17");
    register Callback *handler __asm__("$18");
    register Callback *base __asm__("$20");
    register int one __asm__("$19");
    __asm__("" : "+r"(state));
    if (!state->active) {
        u16 v;
        v = *D_8009DD14;
        __asm__ volatile ("" : : "r"(v));
        func_8008E000(D_80012830, v);
        func_80091B60();
    }
    pa1 = D_8009DD14;
    mask = state->mask;
    __asm__ volatile ("" : "+r"(mask));
    state->busy = 1;
    __asm__ volatile ("" : : : "memory");
    pb1 = D_8009DD18;
    __asm__ volatile ("" : "+r"(pa1), "+r"(pb1));
    if ((pending = mask & *pa1 & *pb1)) {
    one = 1;
    base = state->handlers;
    do {
        i = 0;
        handler = base;
        while (pending && i < 11) {
            if (pending & 1) {
                *D_8009DD14 = ~(one << i);
                if (*handler) (*handler)();
            }
            handler++;
            pending >>= 1;
            i++;
        }
    } while ((pending = D_8009CCB8 & *D_8009DD14 & *D_8009DD18));
    }
    {
    register u16 *pa __asm__("$5");
    register u16 *pb __asm__("$6");
    pa = D_8009DD14;
    pb = D_8009DD18;
    __asm__("" : "+r"(pa), "+r"(pb));
    if (*pa & *pb) {
        int *cnt;
        __asm__("lui %0,%%hi(D_8009DD20)\n\taddiu %0,%0,%%lo(D_8009DD20)"
                : "=r"(cnt));
        if ((*cnt)++ > 0x800) {
            {
            char *msg;
            __asm__("lui %0,%%hi(D_8001284C)\n\taddiu %0,%0,%%lo(D_8001284C)"
                    : "=r"(msg));
            __asm__ volatile ("" : : "r"(msg));
            func_8008E000(msg, *pa, *pb);
            }
            D_8009DD20 = 0;
            *D_8009DD14 = 0;
        }
    } else D_8009DD20 = 0;
    }
    __asm__ volatile (".set\tnoreorder\n\t"
                      "lui\t$1,%%hi(D_8009CC8A)\n\t"
                      "jal\tfunc_80091B60\n\t"
                      "sh\t$0,%%lo(D_8009CC8A)($1)\n\t"
                      ".set\treorder"
                      : : : "$1", "$31", "memory");
}
