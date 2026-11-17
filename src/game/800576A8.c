// Categories: Platform > Controller Input | Input > Remapping
#include "game/types.h"

extern s32 D_8009F7C0[];
extern u32 gControlMaskWord __asm__("D_8009F7C4");
extern s32 gPlayerCount __asm__("D_8009FA0C");

/**
 * @brief Translates a raw pad word into the game's own button mask.
 *
 * The source word comes from D_8009F7C0[index]. Most bits are copied one to
 * one, but four groups are permuted (0x08000000 to 0x04000000, 0x04000000 to
 * 0x00400000, 0x02000000 to 0x01000000 and 0x01000000 to 0x08000000, plus
 * 0x00400000 to 0x02000000), and bit 0x80000 is only set when it was already
 * raised, which keeps the direction "pressed" semantics. In screen mode 1
 * (D_8009FA0C) the three permutation bits are additionally derived from the
 * second word D_8009F7C4, so both control layouts feed the same mask.
 *
 * @param out Destination for the translated mask.
 * @param index Player index into D_8009F7C0.
 *
 * @par Globals
 * D_8009F7C0 - raw per-player words; D_8009F7C4 - second raw word;
 * D_8009FA0C - screen mode flag.
 *
 * @par Control flow
 * Straight-line bit tests, then one conditional second pass.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes *out only.
 *
 * @par Callers
 * Confirmed: func_800267C8, func_800588D4.
 */
void func_800576A8(u32 *out, s32 index)
{
    s32 in;
    u32 res;

    in = D_8009F7C0[index];

    res = -(u32)((in & 0x80000) != 0) & 0x80000;
    if (in & 0x40000) {
        res |= 0x40000;
    }
    if (in & 0x40000000) {
        res |= 0x40000000;
    }
    if (in & 0x80000000) {
        res |= 0x80000000;
    }
    if (in & 0x10000000) {
        res |= 0x10000000;
    }
    if (in & 0x20000000) {
        res |= 0x20000000;
    }
    if (in & 0x08000000) {
        res |= 0x04000000;
    }
    if (in & 0x04000000) {
        res |= 0x00400000;
    }
    if (in & 0x02000000) {
        res |= 0x01000000;
    }
    if (in & 0x01000000) {
        res |= 0x08000000;
    }
    if (in & 0x00400000) {
        res |= 0x02000000;
    }
    if (in & 0x00800000) {
        res |= 0x00800000;
    }
    if (in & 0x00100000) {
        res |= 0x00100000;
    }
    if (in & 0x00200000) {
        res |= 0x00200000;
    }

    if (gPlayerCount == 1) {
        in = gControlMaskWord;
        if (in & 0x08000000) {
            res |= 0x04000000;
        }
        if (in & 0x04000000) {
            res |= 0x00400000;
        }
        if (in & 0x02000000) {
            res |= 0x01000000;
        }
    }

    *out = res;
}
