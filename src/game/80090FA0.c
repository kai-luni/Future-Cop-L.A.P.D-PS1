typedef unsigned int u32;
typedef int s32;
struct State {
    s32 count;
    void * volatile buffer;
    s32 unk8;
    s32 mode;
    s32 command;
    s32 unk14;
    s32 unk18;
    u32 time;
    s32 unk20;
    volatile s32 busy;
    s32 callback;
    s32 callback2;
    s32 callback3;
};
extern struct State D_8009CBD0;
extern volatile s32 D_8009CBF4[];
extern s32 D_8009CBF8[];
extern s32 D_8009C814;
extern u32 func_80091270(s32);
extern s32 func_8008E194(s32);
extern s32 func_8008E1A8(s32);
extern s32 func_8008E5D8(s32);
extern s32 func_8008E010(void);
extern void func_8008E42C(s32, s32, s32);
extern s32 func_80090C18(s32);

s32 func_80090FA0(s32 count, void *buffer, s32 mode)
{
    register s32 n __asm__("$20");
    register void *buf __asm__("$19");
    register s32 m __asm__("$18");
    register volatile s32 *busy __asm__("$16");
    register u32 start __asm__("$17");
    register struct State *state __asm__("$4");
    register struct State *other __asm__("$3");
    register s32 command __asm__("$2");
    register s32 z __asm__("$4");
    s32 mv;
    n = count;
    buf = buffer;
    busy = D_8009CBF4;
    __asm__ volatile("" : "+r" (n), "+r" (buf), "+r" (busy));
    m = mode;
    if (*busy) {
        start = func_80091270(-1);
        while (*busy) {
            if (func_80091270(-1) - start >= 121) {
                busy = D_8009CBF8;
                __asm__ volatile("" : "+r" (busy));
                func_8008E194(*busy);
                busy -= 10;
                busy[9] = 0;
                goto ready;
            }
        }
    }
ready:
    __asm__ volatile("");
    state = &D_8009CBD0;
    __asm__ volatile("" : "+r" (state));
    state->mode = m;
    switch (((volatile struct State *)state)->mode & 0x30) {
    case 0:
        state->command = 0x200;
        break;
    case 0x20:
        state->command = 0x249;
        break;
    default:
        command = 0x246;
        __asm__ volatile("" : "+r" (command));
        other = &D_8009CBD0;
        __asm__ volatile("" : "+r" (other));
        other->command = command;
        break;
    }
    busy = (volatile s32 *)&D_8009CBD0;
    __asm__ volatile("" : "+r" (busy));
    mv = ((volatile struct State *)busy)->mode;
    __asm__ volatile("" : "+r"(mv));
    z = 0;
    __asm__ volatile("" : "+r"(z));
    ((volatile struct State *)busy)->mode = mv | 0x20;
    ((struct State *)busy)->buffer = buf;
    __asm__ volatile("" : "+r"(n));
    ((struct State *)busy)->count = n;
    ((struct State *)busy)->callback = func_8008E194(z);
    ((struct State *)busy)->callback2 = func_8008E1A8(0);
    {
        s32 flags = D_8009C814;
        __asm__ volatile("" : "+r" (flags));
        if (flags & 1)
            ((struct State *)busy)->callback3 = func_8008E5D8(0);
    }
    ((struct State *)busy)->time = func_80091270(-1);
    __asm__ volatile("" : : : "memory");
    if (func_8008E010() & 0xe0) {
        register s32 zero __asm__("$5");
        register s32 nine __asm__("$4");
        nine = 9;
        __asm__ volatile("" : "+r" (nine));
        zero = 0;
        __asm__ volatile("" : "+r" (zero));
        func_8008E42C(nine, zero, 0);
    }
    {
        s32 result = func_80090C18(0);
        __asm__ volatile(".set reorder");
        return result > 0;
    }
}
