// Categories: AI > Targeting | Gameplay > Entity Script
#include "game/types.h"

struct Unk320E4 {
    u8 unk00[0x16];
    u8 kind;
    u8 unk17[0xB9];
    s32 baseX;
    s32 baseY;
    s32 baseZ;
    s32 targetX;
    s32 targetY;
    s32 targetZ;
    u8 unkE8[0x12];
    u8 state;
    u8 unkFB[0x11];
    s16 spreadZ;
    s16 spreadX;
};

extern s32 func_80019668(void);

/**
 * @brief Draws a randomised aim point around the flyer's base position.
 *
 * @param object Flyer block; +0xD0 the base triple, +0xDC the target
 *        triple, +0xFA the state, +0x10C/+0x10E the spread words.
 * @return Nothing.
 *
 * @par Confirmed
 * Skipped in state 3 or for kind 0x25. Otherwise targetY = baseY, and
 * targetX/Z = base + (((rand & 0x7FF) - 0x400) * spread) >> 3 — a symmetric
 * jitter of up to +/- spread/8 around the base.
 *
 * @par Likely role
 * The aim wander of the flyer spawn/tick (func_80032180, func_800324E0):
 * the spread words set the patrol box, state 3 (evade) and kind 0x25 keep
 * their own targeting.
 *
 * @par Callers
 * func_80032180, func_800324E0.
 */
void func_800320E4(struct Unk320E4 *object)
{
    if (object->state != 3 && object->kind != 0x25) {
        object->targetY = object->baseY;
        object->targetX = object->baseX +
            ((((func_80019668() & 0x7FF) - 0x400) * object->spreadX) >> 3);
        object->targetZ = object->baseZ +
            ((((func_80019668() & 0x7FF) - 0x400) * object->spreadZ) >> 3);
    }
}
