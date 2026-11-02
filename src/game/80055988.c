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

extern struct Entity *D_8009F9A0;

extern struct AnimClip *func_800448D8(struct AnimTrack *track, s32 index, s32 direction, u8 flags);
extern s32 func_80046C84(s32 id, s32 arg1);
extern void func_800471C4(s32 track, struct Unk471C4Object *object);
extern void func_800518A8(void *arg0, void *arg1, s32 arg2, void *arg3);
extern void func_80044B34(void *arg0, void *arg1, s32 arg2, s32 arg3);
extern void func_8001E930(void *state, s32 flag);
extern void func_800175F8(struct ModelInstance *instance, s32 frame);
extern void func_8004D958(struct Entity *entity);
extern void func_80053A74(void *arg0, s32 arg1, s32 arg2, s32 arg3);
extern void func_800549F0(void *arg0, s32 arg1, s32 arg2, s32 arg3);

void func_80055988(struct UnkS2_55988 *arg0, s32 arg1, s32 arg2, s32 *arg3)
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
            if (D_8009F9A0 == 0) {
                if (obj->state->unkDE == 0) {
                    if (obj->state->unk146 != 0) {
                        func_80046C84(0x2A, (s32)obj);
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
                func_800471C4(0xFF, (struct Unk471C4Object *)obj);
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

        func_800518A8((void *)((u8 *)obj + 0x28), state->unkEC, state->unkBC, obj);

        height = (state->unkBC << 12) >> 16;
        obj->unk54 += height - obj->unkF2;
        obj->unkF2 = height;

        if (state->unkC0 != 0) {
            func_80044B34(obj, (u8 *)model + 0xA0, state->unkBC, 0);
            func_80044B34(obj, obj->animC8, state->unkBC, 0);
            func_8001E930(obj, 1);
        } else {
            func_8001E930(obj, 0);
        }

        func_800175F8((struct ModelInstance *)((u8 *)model + 0x28), model->unkAC);
        func_800175F8((struct ModelInstance *)((u8 *)obj + 0x28), obj->unkD4);
        flags &= 0xC0000;
        state->flags &= 0x54C0000;
    }

    if (D_8009F9A0 == 0) {
        if (state->unk31A <= 0 && obj->state->unkDE == 0) {
            goto skip_mask;
        }
    } else {
        if (state->flags & 0x80000) {
            func_8004D958(D_8009F9A0);
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
        func_800549F0(obj, step, parameter, flags);
    } else {
        func_80053A74(obj, step, parameter, flags);
    }
}
