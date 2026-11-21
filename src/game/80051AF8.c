// Categories: Gameplay > Entity Update | Graphics > Animation
#include "game/types.h"

struct Unk51AF8State {
    u8 unk00[0xA2];
    s16 unkA2;
    u8 unkA4[0x09];
    s8 unkAD;
    u8 unkAE[0x0E];
    s32 unkBC;
    u8 unkC0[0x02];
    s16 unkC2;
    u8 unkC4[0x0C];
    s16 unkD0;
    u8 unkD2[0x0A];
    s16 unkDC;
    u8 unkDE;
    u8 unkDF;
    s32 unkE0;
    s32 unkE4;
    s32 unkE8;
    u8 unkEC[0x5A];
    u8 unk146;
    u8 unk147;
    u8 unk148[0x08];
    s32 unk150;
    s32 unk154;
    s32 unk158;
    s32 unk15C;
    s32 unk160;
    s32 unk164;
    s32 unk168;
    s32 unk16C;
    s32 unk170;
    u8 unk174[0x1A4];
    s16 unk318;
    u8 unk31A[0x0A];
    s16 unk324;
    s16 unk326;
};

struct Unk51AF8Arg {
    u8 unk00[0xAC];
    struct Unk51AF8State *unkAC;
    u8 unkB0[0x08];
    u8 unkB8;
    u8 unkB9;
    u8 unkBA;
    u8 unkBB;
    u8 unkBC[0x06];
    u8 unkC2;
    u8 unkC3;
    u8 unkC4[0x04];
    u8 unkC8[0x0C];
    s16 unkD4;
    u8 unkD6[0x06];
    u8 *unkDC;
    u8 unkE0[0x10];
    s16 unkF0;
    s16 unkF2;
};

struct AnimClip;
struct AnimTrack;
struct ModelInstance;

extern void gResetCompoundVisualSubpart(void *owner) __asm__("func_800516A0");
extern struct AnimClip *gSelectAnimationClip(struct AnimTrack *track, s32 index, s32 direction, u8 flags) __asm__("func_800448D8");
extern void gSetModelFrame(struct ModelInstance *instance, s32 frame) __asm__("func_800175F8");
extern void gUpdateMorphBounds(void *arg0, void *arg1, s32 arg2, void *arg3) __asm__("func_800518A8");
void gResetCompoundEntityAnimation(struct Unk51AF8Arg *arg0) __asm__("func_80051AF8");


/**
 * @brief Resets a compound entity's state and restarts its animation tracks.
 *
 * @param arg0 Entity block; +0x28 the model instance, +0xAC the state block,
 *        +0xC8 an animation track, +0xD4 a frame value, +0xDC a second
 *        model holder.
 * @return Nothing.
 *
 * @par Confirmed
 * Clears ~25 state fields (counters, angles, the 0x150..0x170 block with
 * unk16C = 0x10000), runs func_800516A0(arg0), then selects the animation:
 * state unkAD == 1 uses track 0xD with unkF2 = 0x1000 and unkBC = 0x10000,
 * otherwise track 0 with both zero. func_800448D8 starts the tracks at
 * +0xC8 and at the second holder +0xA0, func_800175F8 sets both model
 * frames, func_800518A8 initialises the motion block at state +0xEC, and
 * the remaining counters are zeroed.
 *
 * @par Likely role
 * The (re)spawn reset of the compound-object handlers func_800579D4 and
 * func_80058714: the two animation variants are the idle and the active
 * pose of the carried object.
 *
 * @par Unknown
 * What state unkAD distinguishes beyond the track choice.
 *
 * @par Callers
 * func_800579D4, func_80058714.
 */
void gResetCompoundEntityAnimation(struct Unk51AF8Arg *arg0)
{
    struct Unk51AF8State *s1;
    struct ModelInstance *model;

    s1 = arg0->unkAC;

    arg0->unkBA = 0;
    s1->unk150 = 0;
    s1->unk154 = 0;
    s1->unk158 = 0;
    s1->unk168 = 0;
    s1->unk16C = 0x10000;
    s1->unk170 = 0;
    arg0->unkB8 = 0;
    arg0->unkB9 = 1;
    arg0->unkBB = 0;
    arg0->unkC2 = 0;
    arg0->unkF0 = 0;
    s1->unk318 = 0;
    s1->unkDC = 0;
    s1->unkC2 = 0;
    arg0->unkC3 = 0;
    gResetCompoundVisualSubpart(arg0);

    if (s1->unkAD == 1) {
        arg0->unkF2 = 0x1000;
        s1->unkBC = 0x10000;
        gSelectAnimationClip((struct AnimTrack *)((u8 *)arg0 + 0xC8), 0xD, 1, 0);
    } else {
        arg0->unkF2 = 0;
        s1->unkBC = 0;
        gSelectAnimationClip((struct AnimTrack *)((u8 *)arg0 + 0xC8), 0, 1, 0);
    }

    model = (struct ModelInstance *)((u8 *)arg0 + 0x28);
    gSelectAnimationClip((struct AnimTrack *)(arg0->unkDC + 0xA0), s1->unkAD == 1, 1, 0);
    gSetModelFrame((struct ModelInstance *)(arg0->unkDC + 0x28),
                  *(s16 *)(arg0->unkDC + 0xAC));
    gSetModelFrame(model, arg0->unkD4);
    gUpdateMorphBounds(model, (u8 *)s1 + 0xEC, s1->unkBC, arg0);

    s1->unkE8 = 0;
    s1->unkE4 = 0;
    s1->unkE0 = 0;
    s1->unk15C = 0;
    s1->unk160 = 0;
    s1->unk164 = 0;
    s1->unkDE = 0;
    s1->unk147 = 0;
    s1->unk146 = 0;
    s1->unk324 = 0;
    s1->unk326 = 0;
    s1->unkD0 = 0;
    s1->unkA2 = 0;
}
