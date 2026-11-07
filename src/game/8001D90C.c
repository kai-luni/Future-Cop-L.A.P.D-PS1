typedef signed int s32;
typedef unsigned int u32;
struct Entry { volatile s32 state; s32 start, size, pos, flags, pad; };
extern struct Entry D_800A6D78[];
extern char func_80010010[];
extern s32 func_8001D0A0(s32);
extern s32 func_8001CE54(s32, s32, s32, void (*)(s32,s32));
s32 func_8001D90C(s32 index, volatile s32 arg1, s32 arg2, void (*arg3)(s32,s32))
{
    register s32 count __asm__("$19") = arg2;
    register void (*callback)(s32,s32) __asm__("$23") = arg3;
    register s32 state __asm__("$16");
    register s32 transferred __asm__("$18");
    register s32 flags __asm__("$21");
    s32 end = 0;
    register s32 type __asm__("$20");
    register s32 one __asm__("$22");
    s32 diff;
    register s32 finaltype __asm__("$3");
    register u32 test __asm__("$2");
    flags = D_800A6D78[index].flags;
    do {
        state = -9;
        if ((u32)index < 4 && D_800A6D78[index].state != 0)
            state = D_800A6D78[index].state;
        if (state < 0) return state;
        if (state & 0x100) return -14;
        test = state - 2;
        __asm__("" : "+r"(test));
    } while (test < 2);
    D_800A6D78[index].state = 3;
    __asm__("" ::: "memory");
    diff = D_800A6D78[index].pos - D_800A6D78[index].start;
    if (diff >= D_800A6D78[index].size) {
        D_800A6D78[index].state = 0x101;
        return -14;
    }
    if (diff + count >= D_800A6D78[index].size) {
        count = D_800A6D78[index].size - diff;
        end = 0x100;
        if (flags == 0x11) func_80010010[20] = 0;
    }
    state = 0;
    type = flags & 15;
    one = 1;
    transferred = state;
    if (type != one) {
        D_800A6D78[index].state = 2;
        state = func_8001D0A0(index);
    }
    if (state >= 0) {
        state = func_8001CE54(index, arg1, count, callback);
        if (state >= 0) {
            transferred = state;
            state = 0;
            if (type != one) D_800A6D78[index].pos += transferred;
            __asm__("" : "+r"(state));
        }
    }
    __asm__("" : "+r"(flags));
    finaltype = flags & 15;
    if (state < 0) {
        D_800A6D78[index].state = 1;
        if (callback) callback(transferred, state);
        return state;
    }
    if (finaltype != 1) {
        D_800A6D78[index].state = end | 1;
        if (callback) callback(transferred, state);
    }
    return transferred;
}
