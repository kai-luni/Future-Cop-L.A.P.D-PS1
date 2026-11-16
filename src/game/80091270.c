// Categories: Game Engine > Frame | Platform > Interrupts
#include "game/gpu_globals.h"

#include "game/timer_globals.h"

typedef int s32;
typedef unsigned int u32;
extern volatile u32 *gGpuGp1StatusPtr __asm__("D_8009CC18");
extern volatile u32 *gTimer1CounterPtr __asm__("D_8009CC1C");
extern volatile u32 D_8009CC20;
extern s32 D_8009CC24;
extern volatile s32 D_8009DD50;
extern void func_800913E8(s32, s32);

/**
 * @brief Waits for the frame counter to advance and returns the elapsed
 *        16-bit time; negative input only reads the counter.
 *
 * @param input Number of frames to wait (0 = resync only, 1 = no wait,
 *              < 0 = return the current counter D_8009DD50).
 * @return Elapsed counter ticks (16-bit difference), or D_8009DD50 when
 *         input < 0.
 *
 * @par Confirmed
 * Samples *D_8009CC1C twice until stable; elapsed = (now - D_8009CC20)
 * & 0xFFFF. For input != 1: target = D_8009CC24 + input - 1 (input > 0)
 * or D_8009CC24; func_800913E8(target, input - 1 or 0) waits for the
 * counter D_8009DD50 with a timeout and an error path; then
 * func_800913E8(D_8009DD50 + 1, 1). When the saved *D_8009CC18 has bit
 * 0x400000, spins until bit 31 of *D_8009CC18 toggles. D_8009CC24 is
 * updated to D_8009DD50 and D_8009CC20 re-sampled stably.
 *
 * @par Likely role
 * Frame pacing for the CD/MDEC paths: D_8009DD50 is the interrupt-driven
 * frame counter, the 0x400000/0x80000000 pair a display-status handshake.
 *
 * @par Callers
 * func_8008204C, func_80085C4C, func_800879DC, func_80087D48,
 * func_8008ECFC, func_8008EF7C, func_8008F244, func_8008FAC8,
 * func_800908D4, func_80090C18, func_80090E00, func_80090FA0,
 * func_8009113C, func_800951D8, func_8009520C, func_800953F0,
 * func_800954DC, func_800955C8, func_8009570C.
 */
s32 func_80091270(s32 input)
{
    register s32 arg0 __asm__("$4") = input;
    u32 saved = *gGpuGp1StatusPtr;
    volatile u32 now;
    u32 elapsed;
    do {
        now = *gTimer1CounterPtr;
    } while (now != *gTimer1CounterPtr);
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
        saved = *gGpuGp1StatusPtr;
        {
            register s32 value __asm__("$4") = D_8009DD50;
            register s32 one __asm__("$5");
            __asm__("" : "=r"(value) : "0"(value));
            one = 1;
            __asm__("" : "=r"(one) : "0"(one));
            func_800913E8(value + 1, one);
        }
        if (saved & 0x400000) {
            while (!((saved ^ *gGpuGp1StatusPtr) & 0x80000000)) {}
        }
        D_8009CC24 = D_8009DD50;
        do {
            D_8009CC20 = *gTimer1CounterPtr;
        } while (D_8009CC20 != *gTimer1CounterPtr);
    }
    return elapsed;
}
