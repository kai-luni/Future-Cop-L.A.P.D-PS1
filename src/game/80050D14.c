// Categories: Gameplay > Path Following | Physics > Motion
#include "game/types.h"

struct Unk50D14 {
    u8 unk00[0x26];
    s16 unk26;
    u8 unk28[0x14];
    u32 flags;
};

struct Unk50D14Arg {
    u8 unk00[0x0C];
    u8 unk0C;
};

extern void gProcessPathMoverCommands(struct Unk50D14 *object, struct Unk50D14Arg *arg) __asm__("func_8004F188");
extern u8 func_80050B54(struct Unk50D14 *object, s32 arg1, struct Unk50D14Arg *arg);
extern s32 gStepPathMover(struct Unk50D14 *object, s32 arg1, struct Unk50D14Arg *arg) __asm__("func_8005027C");
s32 gUpdatePathMoverMotion(struct Unk50D14 *object, s32 arg1, struct Unk50D14Arg *arg) __asm__("func_80050D14");


/**
 * @brief Movement dispatcher: optional pre-step, then either the bounded
 *        turn mode or the free mover integrator.
 *
 * @param object Mover block; +0x26 a turn budget, +0x3C the flags.
 * @param arg1 Frame delta forwarded to the integrators.
 * @param arg Path record; +0xC enables the pre-step.
 * @return The integrator result, or 0 when the turn budget is exhausted.
 *
 * @par Confirmed
 * arg->unk0C != 0 runs func_8004F188(object, arg) first. Flags & 0x50
 * select the bounded mode: unk26 > 0 calls func_80050B54, else returns 0;
 * otherwise func_8005027C (the heading integrator) runs.
 *
 * @par Likely role
 * The per-frame move entry of the guided entities (func_8003802C,
 * func_80039C58, func_8003A160): bits 0x10/0x40 mark a turning-limited
 * mode whose budget unk26 drains, the pre-step likely applies the path
 * curvature before integration.
 *
 * @par Unknown
 * What the pre-step func_8004F188 changes.
 *
 * @par Callers
 * func_8003802C, func_80039C58, func_8003A160.
 */
s32 gUpdatePathMoverMotion(struct Unk50D14 *object, s32 arg1, struct Unk50D14Arg *arg)
{
    if (arg->unk0C != 0) {
        gProcessPathMoverCommands(object, arg);
    }
    if (object->flags & 0x50) {
        if (object->unk26 > 0) {
            return func_80050B54(object, arg1, arg);
        }
        return 0;
    }
    return gStepPathMover(object, arg1, arg);
}
