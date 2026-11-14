typedef int s32;
typedef unsigned int u32;
struct State {
    s32 count, buffer, unk8, mode, command;
    volatile s32 unk14;
    s32 unk18;
    u32 time;
    s32 unk20;
    volatile s32 busy;
    s32 callback, callback2, callback3;
};
extern struct State D_8009CBD0;
extern s32 D_8009C814;
extern s32 D_800908A0[];
extern s32 D_8009CBF8[];
extern s32 D_8009CBFC[];
extern u32 func_80091270(s32);
extern s32 func_8008E194(s32);
extern s32 func_8008E1A8(s32);
extern s32 func_8008E5D8(s32);
extern s32 func_8008E5FC(s32);
extern void func_8008E2F8(s32,s32);

static __inline__ s32 wait_ready(struct State *state)
{
    register u32 start __asm__("$17");
    register volatile s32 *busy __asm__("$16");
    busy = (volatile s32 *)state;
    if (!busy[9]) return 1;
    start = func_80091270(-1);
    if (busy[9]) {
        busy += 9;
        do {
            if (func_80091270(-1) - start >= 121) {
                busy = D_8009CBF8;
                __asm__ volatile("" : "+r"(busy));
                func_8008E194(*busy);
                busy -= 10;
                __asm__("" : "+r"(busy));
                ((s32 *)busy)[9] = 0;
                return -1;
            }
        } while (*busy);
    }
    return 0;
}

static __inline__ void wait_again(s32 *callbacks)
{
    register u32 start __asm__("$16");
    register volatile s32 *busy __asm__("$17");
    busy = callbacks;
    if (!busy[-2]) return;
    start = func_80091270(-1);
    if (busy[-2]) {
        busy -= 2;
        do {
            if (func_80091270(-1) - start >= 121) {
                register volatile s32 *p __asm__("$16");
                p = D_8009CBF8;
                __asm__ volatile("" : "+r"(p));
                func_8008E194(*p);
                p -= 10;
                __asm__("" : "+r"(p));
                p[9] = 0;
                return;
            }
        } while (*busy);
    }
}

void func_80090E00(void)
{
    register s32 *callbacks __asm__("$17");
    register struct State *state __asm__("$16");
    struct State *other;
    if (D_8009C814 & 1) func_8008E5FC(0);
    state = &D_8009CBD0;
    __asm__ volatile("" : "+r"(state));
    state->unk14 = 0;
    if (wait_ready(state)) {
        __asm__ volatile("");
        callbacks = D_8009CBFC;
        __asm__ volatile("" : "+r"(callbacks));
        func_8008E1A8(callbacks[0]);
        if (D_8009C814 & 1) func_8008E5D8(callbacks[1]);
        func_8008E194((s32)&D_800908A0);
        func_8008E2F8(9,0);
        other = (struct State *)(callbacks - 11);
        __asm__("" : "+r"(other));
        other->busy = 1;
        wait_again(callbacks);
    }
}
