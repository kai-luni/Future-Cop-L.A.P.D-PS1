// Categories: Platform > Sound | Utilities > Mathematics > Vector
#include "game/types.h"

#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))

struct Unk45884 {
    s32 position[3];
    s32 angle;
};

extern struct Unk45884 gPositionAngleTable[] __asm__("D_800A1920");
extern s32 gSoundSpatialMode __asm__("D_8009F734");

extern void func_8001A578(u32 angle, s32 *vec);

/**
 * @brief Computes 0..0x7F stereo pan in mix[1] from listener-relative delta vectors when mix[2] is 0.
 *
 * When mix[2] == 0, rotates listener-0 delta (mix+4) by D_800A1920[0].angle via func_8001A578; in 2-listener mode (D_8009F734 == 2), also rotates mix+0xC by D_800A1920[1].angle, sums two clamped [0, 0x40] halves into [0, 0x7F], else scales (x >> 10) + 0x40 into [0, 0x7F], and sets mix[2] = 1. Category Platform > Sound | Utilities > Mathematics > Vector reflects audio pan calculation and 2D vector rotation.
 *
 * @param mix Sound mix buffer (+0x01 pan output, +0x02 computed flag, +0x04/+0x0C 2D XZ listener deltas).
 *
 * @par Globals
 * D_800A1920 - listener position/angle array (reads [0].angle and [1].angle); D_8009F734 - active listener count (2 selects dual-listener mix).
 *
 * @par Control flow
 * Outer guard on mix[2] == 0, containing a branch on D_8009F734 == 2 for two-listener vs single-listener pan clamping.
 *
 * @par Calls
 * func_8001A578 - rotates 2D fixed-point XZ delta vectors in place by listener yaw angle.
 *
 * @par Side effects
 * Rotates mix+4 (and mix+0xC in 2-listener mode) in place, writes clamped stereo pan to mix[1], and sets mix[2] = 1.
 *
 * @par Callers
 * Confirmed: func_800450A4 (0x800450E0), func_80046E88 (0x80046F30); also func_800462D0 (0x8004654C, 0x800465D8) and func_80046F9C (0x80047028) in asm/us/main.s / Likely: none / Unknown: none.
 */
void func_800460D4(s8 *mix)
{
    if (mix[2] == 0) {
        func_8001A578(gPositionAngleTable[0].angle, (s32 *)(mix + 4));
        if (gSoundSpatialMode == 2) {
            func_8001A578(gPositionAngleTable[1].angle, (s32 *)(mix + 0xC));
            mix[1] = CLAMP(CLAMP((*(s32 *)(mix + 4) >> 11) + 0x20, 0, 0x40) +
                           CLAMP((*(s32 *)(mix + 0xC) >> 11) + 0x20, 0, 0x40),
                           0, 0x7F);
        } else {
            s32 pan = CLAMP((*(s32 *)(mix + 4) >> 10) + 0x40, 0, 0x7F);
            mix[1] = pan;
        }
        mix[2] = 1;
    }
}
