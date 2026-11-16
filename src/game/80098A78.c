// Categories: Platform > SPU Transfer
#include "game/spu_globals.h"

#include "game/types.h"

typedef struct {
    u8 pad[0x1AA];
    volatile u16 flags;
} Unk9EEA4;

extern Unk9EEA4 *gSpuRegBasePtr __asm__("D_8009EEA4");
extern void func_8008E000(char *a, char *b);

/**
 * @brief Waits for the SPU transfer flag to settle, with a timeout.
 *
 * Modes 0 and 3 clear bit 0x40 of the status word and then spin until the bit
 * is gone; modes 1 and 3 set it and spin until it appears. Both waits give up
 * after 0xF00 iterations and report the timeout through func_8008E000 with
 * two fixed strings from the image (0x80012D2C/0x80012D3C when clearing,
 * 0x80012D2C/0x80012D4C when setting), returning -1. Otherwise the mode is
 * returned unchanged, which makes the call chainable.
 *
 * @param arg Mode: 0 wait for clear, 1 wait for set, 3 do both in that order.
 * @return The mode, or -1 after a timeout.
 *
 * @par Globals
 * D_8009EEA4 - device block; the status word sits at 0x1AA.
 *
 * @par Control flow
 * Two independent bounded spin loops.
 *
 * @par Calls
 * func_8008E000 - diagnostic print on timeout.
 *
 * @par Side effects
 * Writes the status bit; the spin loops read hardware state.
 *
 * @par Callers
 * Confirmed: func_80087750 (sound shutdown). Likely: func_80022F44,
 * func_80022F7C, func_80022FA8 (same block).
 */
s32 func_80098A78(s32 arg)
{
    u32 i;
    register char *a0 __asm__("$4");
    register char *a1 __asm__("$5");

    if (arg == 0 || arg == 3) {
        gSpuRegBasePtr->flags &= 0xFFBF;
        i = 0;
        while (gSpuRegBasePtr->flags & 0x40) {
            if (++i > 0xF00) {
                a0 = (char *)0x80010000;
                __asm__("" : "=r"(a0) : "0"(a0));
                a0 += 0x2D2C;
                a1 = (char *)0x80010000;
                __asm__("" : "=r"(a1) : "0"(a1));
                a1 += 0x2D3C;
                func_8008E000(a0, a1);
                return -1;
            }
        }
    }
    if (arg == 1 || arg == 3) {
        gSpuRegBasePtr->flags |= 0x40;
        i = 0;
        while (!(gSpuRegBasePtr->flags & 0x40)) {
            if (++i > 0xF00) {
                a0 = (char *)0x80010000;
                __asm__("" : "=r"(a0) : "0"(a0));
                a0 += 0x2D2C;
                a1 = (char *)0x80010000;
                __asm__("" : "=r"(a1) : "0"(a1));
                a1 += 0x2D4C;
                func_8008E000(a0, a1);
                return -1;
            }
        }
    }
    return arg;
}
