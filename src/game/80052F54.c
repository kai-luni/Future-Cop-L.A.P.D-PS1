// Categories: Gameplay > Combat | Gameplay > Difficulty
#include "game/types.h"

#define FMUL(d, a, b) __asm__("mult %1,%2\n\tmflo $8\n\tmfhi $9\n\tsll $9,$9,16\n\tsrl $8,$8,16\n\tor %0,$9,$8" : "=r"(d) : "r"(a), "r"(b) : "$8", "$9")

struct ObjectInfo {
    u8 pad00[0xC1];
    s8 side;
};

struct Object {
    u8 pad00[0x1E];
    s16 unk1E;
    u8 pad20[0xAC - 0x20];
    struct ObjectInfo *info;
};

struct Game {
    u8 pad00[8];
    s32 mode0;
    u8 pad0C[0x98 - 0xC];
    s32 mode1;
};

extern struct Game *gGameSession __asm__("D_8009FA94");
extern s32 gSqrt12(s32 value) __asm__("func_80095900");
s32 gComputeLandingDamage(struct Object *obj, s32 val) __asm__("func_80052F54");


/**
 * @brief Landing-impact damage curve scaled by object and difficulty.
 *
 * Converts a hard-landing rebound speed into damage. Speeds at or below
 * 0x93333 do no damage; above that the excess is clamped at 0x300000,
 * normalized (shift 9, divide 183501, shift 7) and shaped by a three-piece
 * curve: linear below 0x8000, square-root-weighted up to 0xC000, cubic above.
 * The result is multiplied by the object impact scale at +0x1E and by 100,
 * then divided by the difficulty divisor picked through the object's side.
 *
 * @param obj Entity block; +0x1E impact scale, +0xAC info record whose side
 *        byte at +0xC1 selects the difficulty word.
 * @param val Rebound speed in fixed point.
 * @return The damage amount; 0 below the threshold or in difficulty mode 3.
 *
 * @par Globals
 * D_8009FA94 - game settings block; +0x08 and +0x98 hold the difficulty mode
 * words for side 0 and non-zero side respectively.
 *
 * @par Control flow
 * Threshold gate, clamp, normalization, three FMUL curve bands, side
 * selection, then a four-way switch: modes 0/1/2 divide by 100/125/150 and
 * mode 3 zeroes the result.
 *
 * @par Calls
 * func_80095900 - square root used by the middle curve band.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_80053A74 (hover-form physics tick) and func_800549F0
 * (on-foot locomotion tick); both add the result to the impact total and
 * switch the actor to state 0x12 when the landing speed exceeds the limit.
 */
s32 gComputeLandingDamage(struct Object *obj, s32 val)
{
    s32 *modePtr;
    s32 mode;
    s32 temp;

    if (val > 0x93333) {
        if (val > 0x300000) {
            val = 0x300000;
        }
        val -= 0x93333;

        val <<= 9;
        val /= 183501;
        val <<= 7;

        if (val <= 0x8000) {
            FMUL(val, val, 0x9CC4);
        } else if (val <= 0xC000) {
            FMUL(temp, val, gSqrt12(val) << 2);
            FMUL(val, temp, 0xDDB3);
        } else {
            FMUL(val, val, val);
        }

        temp = obj->unk1E;
        FMUL(val, temp, val);

        val *= 100;

        if (obj->info->side != 0) {
            modePtr = &gGameSession->mode1;
        } else {
            modePtr = &gGameSession->mode0;
        }
        mode = *modePtr;

        switch (mode) {
        case 2:
            val /= 150;
            break;
        case 1:
            val /= 125;
            break;
        case 0:
            val /= 100;
            break;
        case 3:
            val = 0;
            break;
        }

        return val;
    }

    return 0;
}
