typedef int s32;
typedef unsigned int u32;
extern volatile u32 *D_8009CC18;
extern volatile u32 *D_8009CC1C;
extern volatile u32 D_8009CC20;
extern s32 D_8009CC24;
extern volatile s32 D_8009DD50;
extern void func_800913E8(s32, s32);

s32 func_80091270(s32 input)
{
    register s32 arg0 __asm__("$4") = input;
    u32 saved = *D_8009CC18;
    volatile u32 now;
    u32 elapsed;
    do {
        now = *D_8009CC1C;
    } while (now != *D_8009CC1C);
    elapsed = (now - D_8009CC20) & 0xffff;
    if (arg0 < 0) return D_8009DD50;
    if (arg0 != 1) {
        register s32 target __asm__("$2");
        register s32 count __asm__("$5");
        if (arg0 > 0) {
            target = D_8009CC24 - 1;
            __asm__("" : "=r"(target) : "0"(target));
            target += arg0;
        }
        else target = D_8009CC24;
        count = 0;
        if (arg0 > 0) count = arg0 - 1;
        func_800913E8(target, count);
        saved = *D_8009CC18;
        {
            register s32 value __asm__("$4") = D_8009DD50;
            register s32 one __asm__("$5");
            __asm__("" : "=r"(value) : "0"(value));
            one = 1;
            __asm__("" : "=r"(one) : "0"(one));
            func_800913E8(value + 1, one);
        }
        if (saved & 0x400000) {
            while (!((saved ^ *D_8009CC18) & 0x80000000)) {}
        }
        D_8009CC24 = D_8009DD50;
        do {
            D_8009CC20 = *D_8009CC1C;
        } while (D_8009CC20 != *D_8009CC1C);
    }
    return elapsed;
}
