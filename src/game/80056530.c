typedef int s32;
typedef unsigned int u32;
#define FMUL(d,a,b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

s32 func_80056530(s32 *obj, s32 scale, s32 count)
{
    register s32 *state __asm__("$10");
    register s32 delta __asm__("$11");
    register s32 value __asm__("$3");
    register s32 step __asm__("$7");
    register s32 current __asm__("$2");
    register s32 rounded __asm__("$4");
    register s32 magnitude __asm__("$3");
    state = *(s32 **)((char *)obj + 0xac);
    value = obj[0x54 / 4];
    current = state[0xd4 / 4];
    delta = value - current;
    if (delta != 0) {
        value = delta << 3;
        FMUL(step, value, value);
        FMUL(value, step, value);
        rounded = value;
        if (value < 0) rounded = value + 7;
        value = rounded >> 3;
        if ((value ^ state[0xd8 / 4]) < 0)
            state[0xd8 / 4] = 0;
        while (count > 0) {
            state[0xd8 / 4] += value;
            count--;
        }
        current = state[0xd8 / 4];
        current >>= 14;
        step = current * scale;
        if (((step >> 31) + step ^ (step >> 31)) < 256) {
            step = 0;
            __asm__("" : "+r"(step));
        }
        magnitude = ((step >> 31) + step) ^ (step >> 31);
        __asm__("" : "+r"(magnitude));
        if (((delta >> 31) + delta ^ (delta >> 31)) < magnitude)
            step = delta;
        current = state[0xd4 / 4];
        value = current + step;
    } else {
        __asm__("" : "+r"(delta));
        state[0xd8 / 4] = 0;
    }
    state[0xd4 / 4] = value;
    return value;
}
