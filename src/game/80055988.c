// Categories: Gameplay > Entity Update | Game Engine > State Machine
#include "game/types.h"

struct ModelInstance;
struct AnimTrack;
struct AnimClip;
struct Entity;
struct Unk471C4Object;

struct UnkS4_55988 {
    u8 pad00[0xAC];
    s16 unkAC;
};

struct UnkS1_55988 {
    u8 pad00[0x30];
    u32 flags;
    u8 pad34[0xAD - 0x34];
    s8 unkAD;
    u8 padAE[0xBC - 0xAE];
    s32 unkBC;
    s8 unkC0;
    u8 padC1[1];
    s16 unkC2;
    u8 padC4[0xDC - 0xC4];
    s16 unkDC;
    u8 unkDE;
    u8 padDF[0xE0 - 0xDF];
    s32 unkE0;
    s32 unkE4;
    s32 unkE8;
    u8 unkEC[0x146 - 0xEC];
    s8 unk146;
    u8 pad147[0x2FD - 0x147];
    s8 unk2FD;
    u8 pad2FE[0x310 - 0x2FE];
    s8 unk310;
    u8 pad311[0x31A - 0x311];
    s16 unk31A;
    u8 pad31C[0x320 - 0x31C];
    s32 unk320;
};

struct UnkS2_55988 {
    u8 pad00[0x50];
    s32 unk50;
    s32 unk54;
    s32 unk58;
    u8 pad5C[0xAC - 0x5C];
    struct UnkS1_55988 *state;
    u8 padB0[0xC8 - 0xB0];
    u8 animC8[0x0C];
    s16 unkD4;
    u8 padD6[0xDC - 0xD6];
    struct UnkS4_55988 *model;
    u8 padE0[0xF0 - 0xE0];
    s16 unkF0;
    s16 unkF2;
};

extern struct Entity *gFocusPositions __asm__("D_8009F9A0");

extern struct AnimClip *func_800448D8(struct AnimTrack *track, s32 index, s32 direction, u8 flags);
extern s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");
extern void gSetEntityLoopingSound(s32 track, struct Unk471C4Object *object) __asm__("func_800471C4");
extern void gUpdateMorphBounds(void *arg0, void *arg1, s32 arg2, void *arg3) __asm__("func_800518A8");
extern void gAdvanceAnimTrack(void *arg0, void *arg1, s32 arg2, s32 arg3) __asm__("func_80044B34");
extern void gSetEntitySmallRumble(void *state, s32 flag) __asm__("func_8001E930");
extern void gSetModelFrame(struct ModelInstance *instance, s32 frame) __asm__("func_800175F8");
extern void gRetireEntity(struct Entity *entity) __asm__("func_8004D958");
extern void gTickHoverPhysics(void *arg0, s32 arg1, s32 arg2, s32 arg3) __asm__("func_80053A74");
extern void gTickOnFootPhysics(void *arg0, s32 arg1, s32 arg2, s32 arg3) __asm__("func_800549F0");
void gUpdatePlayerFormAndMotion(struct UnkS2_55988 *arg0, s32 arg1, s32 arg2, s32 *arg3) __asm__("func_80055988");


/**
 * @brief Per-tick update of the transformable player unit: runs the form
 *        change state machine, hands the progress to the animations, the shadow
 *        and the model instances, then dispatches to one of the two physics
 *        integrators.
 *
 * @param arg0 Entity block. +0x50/+0x54/+0x58 the world position, +0xAC the
 *        state block s, +0xC8 an animation track array, +0xD4 a model frame,
 *        +0xF0/+0xF2 the vertical offset bookkeeping of the transition.
 * @param arg1 step Elapsed ticks of this frame; drives the transition progress.
 * @param arg2 parameter Physics sub-step count, passed through to the
 *        integrator unchanged.
 * @param arg3 Output word, cleared first; bit 0 reports that a form change
 *        started or finished this tick.
 * @return Nothing.
 *
 * @par Confirmed
 * 1. The output word is zeroed first, then a pending impulse s +0xE0/+0xE4/
 *    +0xE8 is added to the position and cleared.
 * 2. flags = s +0x30, forced to 0 while s +0x310 is non-zero.
 * 3. (flags & 0x2800000) == 0x2800000 with s +0xC0 == 0, D_8009F9A0 == 0 and
 *    s +0xDE == 0 triggers a request: with s +0x146 set the sound
 *    func_80046C84(0x2A, obj) plays, otherwise the target form s +0x2FD flips
 *    to 1 - s +0xAD.
 * 4. s +0xAD != s +0x2FD starts the transition. With s +0xBC == 0 the direction
 *    s +0xC0 becomes 1, clips 2 of the track at model +0xA0 and 0xC of the
 *    track at obj +0xC8 are played forward, s +0xAD = 1 and s +0xC2 = 0;
 *    otherwise s +0xC0 = -1 and both clips play backwards. s +0x2FD = s +0xAD
 *    and bit 0 of the output word is set.
 * 5. While s +0xC0 != 0 the progress s +0xBC moves by +/-step * 0xA0. At the
 *    low end it clamps to 0, s +0xC0 is cleared, clips 0 and 0 are played,
 *    obj +0xF0 and s +0xDC are cleared, s +0xAD and s +0x2FD go back to 0 and
 *    func_800471C4(0xFF, obj) stops the sound track. At 0x10000 or above it
 *    clamps, clears s +0xC0, plays clips 1 and 0xD and clears obj +0xF0 and
 *    s +0xDC.
 * 6. func_800518A8(obj +0x28, s +0xEC, s +0xBC, obj) drives the shadow/cone
 *    effect, the body is lifted by height = (s +0xBC << 12) >> 16 through
 *    obj +0x54 and obj +0xF2, and while the transition runs the two clips are
 *    positioned with func_80044B34(obj, track, s +0xBC, 0) and the sub-system
 *    flag is set by func_8001E930(obj, 1) (0 otherwise).
 * 7. func_800175F8 sets both model instances to their current frame
 *    (model +0xAC and obj +0xD4), then flags and s +0x30 are masked to 0xC0000
 *    and 0x54C0000.
 * 8. With D_8009F9A0 null the masking step is skipped when s +0x31A <= 0 and
 *    s +0xDE == 0; with D_8009F9A0 set and s +0x30 carrying 0x80000,
 *    func_8004D958(D_8009F9A0) despawns that object and s +0x320 = 3. Both
 *    words are then masked to 0xC0000.
 * 9. When bits 0x200000 and 0x100000 of flags are both set, 0x100000 is
 *    cleared, so at most one strafe direction survives.
 * 10. Dispatch: s +0xAD != 1 calls func_800549F0(obj, step, parameter, flags),
 *    otherwise func_80053A74(obj, step, parameter, flags).
 *
 * @par Likely role
 * The per-frame update of the player-controlled transformer: s +0xAD is the
 * current form (0 = walker, 1 = hover), s +0x2FD the requested one, bit 0 of
 * the output word tells the caller to play the transformation sound, and the
 * two integrators are the walker and the hover drive models.
 *
 * @par Unknown
 * What the 0xC0000 and 0x54C0000 masks keep, what s +0x146 and s +0xDE signal,
 * what the state 3 written to s +0x320 means, and what D_8009F9A0 represents
 * (an attached or active entity whose 0x80000 flag forces a detach).
 *
 * @par Callers
 * func_800586D4, a trampoline that saves $sp in D_8009FAD8, switches the stack
 * to the scratchpad at 0x1F8003F4 for the duration of the call and restores it
 * afterwards.
 */
void gUpdatePlayerFormAndMotion(struct UnkS2_55988 *arg0, s32 arg1, s32 arg2, s32 *arg3)
{
    register struct UnkS2_55988 *obj __asm__("$18");
    register struct UnkS1_55988 *state __asm__("$17");
    register struct UnkS4_55988 *model __asm__("$20");
    register s32 flags __asm__("$19");
    register s32 step __asm__("$22");
    register s32 parameter __asm__("$23");
    register s32 *out __asm__("$21");
    s32 height;

    obj = arg0;
    step = arg1;
    out = arg3;
    __asm__ volatile("sw $0, 0(%0)" : : "r"(out));
    parameter = arg2;
    state = obj->state;
    model = obj->model;

    if (state->unkE0 != 0 || state->unkE4 != 0 || state->unkE8 != 0) {
        obj->unk50 += state->unkE0;
        obj->unk54 += state->unkE4;
        obj->unk58 += state->unkE8;
        state->unkE8 = 0;
        state->unkE4 = 0;
        state->unkE0 = 0;
    }

    flags = state->flags;
    if (state->unk310 != 0) {
        flags = 0;
    }

    if ((flags & 0x2800000) == 0x2800000) {
        if (state->unkC0 == 0) {
            if (gFocusPositions == 0) {
                if (obj->state->unkDE == 0) {
                    if (obj->state->unk146 != 0) {
                        gPlayNonpositionalSoundEffect(0x2A, (s32)obj);
                    } else {
                        state->unk2FD = 1 - (u8)state->unkAD;
                    }
                }
            }
        }
    }

    if (state->unkAD != state->unk2FD) {
        if (state->unkBC == 0) {
            state->unkC0 = 1;
            func_800448D8((struct AnimTrack *)((u8 *)model + 0xA0), 2, 1, 0);
            func_800448D8((struct AnimTrack *)obj->animC8, 0xC, 1, 0);
            state->unkAD = 1;
            state->unkC2 = 0;
        } else {
            state->unkC0 = -1;
            func_800448D8((struct AnimTrack *)((u8 *)model + 0xA0), 2, -1, 0);
            func_800448D8((struct AnimTrack *)obj->animC8, 0xC, -1, 0);
        }
        state->unk2FD = (u8)state->unkAD;
        *out |= 1;
    }

    if (state->unkC0 != 0) {
        if (state->unkC0 < 0) {
            state->unkBC -= step * 0xA0;
            if (state->unkBC < 0) {
                state->unkBC = 0;
                state->unkC0 = 0;
                func_800448D8((struct AnimTrack *)((u8 *)model + 0xA0), 0, 1, 0);
                obj->unkF0 = 0;
                state->unkDC = 0;
                func_800448D8((struct AnimTrack *)obj->animC8, 0, 1, 0);
                state->unkAD = 0;
                state->unk2FD = 0;
                gSetEntityLoopingSound(0xFF, (struct Unk471C4Object *)obj);
            }
        } else if (state->unkC0 > 0) {
            state->unkBC += step * 0xA0;
            if (state->unkBC > 0x10000) {
                state->unkBC = 0x10000;
                state->unkC0 = 0;
                func_800448D8((struct AnimTrack *)((u8 *)model + 0xA0), 1, 1, 0);
                func_800448D8((struct AnimTrack *)obj->animC8, 0xD, 1, 0);
                obj->unkF0 = 0;
                state->unkDC = 0;
            }
        }

        gUpdateMorphBounds((void *)((u8 *)obj + 0x28), state->unkEC, state->unkBC, obj);

        height = (state->unkBC << 12) >> 16;
        obj->unk54 += height - obj->unkF2;
        obj->unkF2 = height;

        if (state->unkC0 != 0) {
            gAdvanceAnimTrack(obj, (u8 *)model + 0xA0, state->unkBC, 0);
            gAdvanceAnimTrack(obj, obj->animC8, state->unkBC, 0);
            gSetEntitySmallRumble(obj, 1);
        } else {
            gSetEntitySmallRumble(obj, 0);
        }

        gSetModelFrame((struct ModelInstance *)((u8 *)model + 0x28), model->unkAC);
        gSetModelFrame((struct ModelInstance *)((u8 *)obj + 0x28), obj->unkD4);
        flags &= 0xC0000;
        state->flags &= 0x54C0000;
    }

    if (gFocusPositions == 0) {
        if (state->unk31A <= 0 && obj->state->unkDE == 0) {
            goto skip_mask;
        }
    } else {
        if (state->flags & 0x80000) {
            gRetireEntity(gFocusPositions);
            state->unk320 = 3;
        }
    }
    flags &= 0xC0000;
    state->flags &= 0xC0000;

skip_mask:
    if ((flags & 0x300000) == 0x300000) {
        flags &= 0xFFCFFFFF;
    }

    if (state->unkAD != 1) {
        gTickOnFootPhysics(obj, step, parameter, flags);
    } else {
        gTickHoverPhysics(obj, step, parameter, flags);
    }
}
