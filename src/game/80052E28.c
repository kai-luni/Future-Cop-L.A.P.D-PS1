// Categories: Gameplay > Movement | Game Engine > Steering
#include "game/types.h"

struct Unk52E28 {
    u8 pad_00[0xB0];
    s16 unkB0;
    u8 pad_B2[6];
    u8 unkB8;
};

/**
 * @brief Steps a 12-bit angle towards a target that depends on the behaviour mode.
 *
 * The low three bits of 0xB8 select one of four steering rules, each mapping
 * the current angle 0xB0 into a hysteresis window: mode 1 targets 0, 0x800 or
 * 0x1000 depending on how far the angle already is; mode 2 splits at 0x800;
 * mode 3 targets 0xE00, 0x600 or -1 around 0xA00 and 0x200; mode 4 mirrors
 * mode 1 with a different split. The delta is then clamped to eight units per
 * call and added, so the movement needs several frames to reach the target.
 *
 * @param self Entity; angle at 0xB0, mode bits at 0xB8.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Switch over four modes, then a symmetric clamp on the delta.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes the angle at 0xB0.
 *
 * @par Callers
 * Confirmed: func_80053A74, func_800549F0.
 */
void func_80052E28(struct Unk52E28 *self)
{
    register s32 target __asm__("$5") = 0;
    s32 delta;

    switch (self->unkB8 & 7) {
    case 1:
        if (self->unkB0 < 0x400) {
            target = 0;
        } else {
            target = 0x800;
            if (self->unkB0 >= 0xC01) {
                target = 0x1000;
            }
        }
        break;
    case 2:
        if (self->unkB0 < 0x800) {
            target = 0x400;
        } else {
            target = 0xC00;
        }
        break;
    case 3:
        if (self->unkB0 > 0xA00) {
            target = 0xE00;
        } else {
            if (self->unkB0 < 0x200) {
                target = -1;
            } else {
                target = 0x600;
            }
        }
        break;
    case 4:
        if (self->unkB0 < 0x600) {
            target = 0x200;
        } else {
            target = 0xA00;
            if (self->unkB0 >= 0xE01) {
                target = 0x1000;
            }
        }
        break;
    }

    delta = target - self->unkB0;
    target = 8;
    if (delta > 0) {
        if (target < delta) {
            delta = target;
        }
    }
    if (delta < 0) {
        if (target < -delta) {
            delta = -target;
        }
    }

    self->unkB0 += delta;
}
