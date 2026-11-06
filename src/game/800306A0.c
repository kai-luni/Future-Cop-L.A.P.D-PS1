#include "game/types.h"

struct AnimClip {
    u8 unk00[4];
    s16 start;
    s16 end;
    u8 unk08[8];
};

struct AnimTrack {
    struct AnimClip *clips;
    struct AnimClip *clip;
    s32 elapsed;
    s16 frame;
    s16 target;
    s8 step;
    u8 index;
    u8 count;
    u8 flags;
};

struct ModelFrame {
    u16 unk00;
    s16 unk02;
    s16 radius;
    u16 unk06;
    u8 unk08[8];
};

struct ModelFrames {
    u8 header[0x10];
    struct ModelFrame frames[1];
};

struct Model {
    u8 unk00[0x34];
    struct ModelFrames *frames;
};

struct ModelInstance {
    struct Model *model;
    u8 coord[0x3C];
    s32 radius;
    u16 unk44;
    u16 unk46;
    s16 frame;
    u8 flags;
};

struct ObjD0_306A0 {
    u8 pad00[0x14];
    u16 unk14;
    u8 unk16;
    u8 pad17[0xAC - 0x17];
    u32 unkAC;
    u8 padB0[0xDC - 0xB0];
    u32 unkDC;
};

struct Actor306A0 {
    u8 pad00[0x24];
    u32 flags24;
    struct ModelInstance instance28;
    u8 pad28[0x28];
    void *ptr9C;
    struct AnimTrack animA0;
    u32 unkB4;
    u8 padB8[0x02];
    s16 unkBA;
    u8 unkBC;
    u8 stateBD;
    s16 unkBE;
    s32 unkC0;
    s16 unkC4;
    u8 padC6[0x02];
    u16 unkC8;
    u8 padCA[0x02];
    u16 unkCC;
    u8 padCE[0x02];
    struct ObjD0_306A0 *ptrD0;
    struct ModelInstance instanceD4;
    u8 padD4[0x58 - sizeof(struct ModelInstance)];
    s16 unk12C;
};

extern u32 D_8009FAA4;
extern void *jtbl_800111DC[];

extern void func_8002FC90(void *, u32, s16, s32, s32);
extern void func_80069D68(void *, s32, u32, void *);
extern s32 func_800402DC(void *, u8, s32, s32, s32);
extern void *func_800448D8(struct AnimTrack *, s32, s32, u8);
extern void func_800175F8(struct ModelInstance *, s32);
extern void func_800188D0(s16, s32, s32, void *);
extern s32 func_80044B34(void *, void *, s32, s32);
extern void func_800302E8(struct Actor306A0 *, s32, s32);
extern u8 func_8002F8B8(u8, s16, s32, s32);
extern void func_80030498(struct Actor306A0 *, s32);
extern s32 func_8002AD58(s32, struct Actor306A0 *, s32);
extern u8 func_80017508(struct ModelInstance *, u32 *, s32);

s32 func_800306A0(struct Actor306A0 *actor, s32 delta)
{
    struct ObjD0_306A0 *obj;
    u8 local[0x50];
    u32 temp;

    if (actor->stateBD == 5) {
        if ((s16)actor->unkCC == 0x190) {
            obj = actor->ptrD0;
            func_8002FC90(obj, actor->unkB4, actor->unkC4, 0, 0);
            if (actor->ptr9C != 0 && *(u8 *)actor->ptr9C != 0 && (actor->flags24 & 0x100000) == 0) {
                func_80069D68(actor->ptr9C, 0, obj->unk14 & 0x7FFF, actor);
            }
            if (obj->unk16 == 1) {
                *(u16 *)(obj->unkAC + 0xA0) = 1;
                if (obj->unkDC != 0) {
                    local[0x49] = 0x40;
                    *(u32 *)(local + 0x44) = obj->unkDC + 0x28;
                    func_800402DC(obj, 0xC8, (s32)local, 1, 0);
                }
            }
        }
        actor->unkCC = actor->unkCC - delta;
        if ((s16)actor->unkCC <= 0) {
            register struct ObjD0_306A0 *obj2 __asm__("$7");
            obj2 = actor->ptrD0;
            if (obj2 != 0 && obj2->unk16 == 1) {
                *(u16 *)(obj2->unkAC + 0xA0) = 0;
                func_800448D8(&actor->animA0, 3, 1, 1);
                actor->stateBD = 3;
            }
        }
    }

    if (actor->stateBD == 2 || actor->stateBD == 5) {
        if (*(u32 *)((u8 *)actor->instanceD4.model + 0x10) != 0) {
            s32 zero = 0;
            func_80044B34(zero, (u8 *)actor + 0x120, delta, zero);
            func_800175F8(&actor->instanceD4, actor->unk12C);
        }
        if (actor->unkB4 & 0xF0) {
            actor->unkC8 = (actor->unkC8 - (delta << 4)) & 0xFFF;
        } else {
            actor->unkC8 = (actor->unkC8 + (delta << 2)) & 0xFFF;
        }
        func_800188D0((s16)actor->unkC8, 0, 0, (u8 *)actor + 0xD8);
    }

    temp = func_80044B34(actor, &actor->animA0, delta, 0);
    {
        register u32 shift __asm__("$16");
        register struct ModelInstance *inst __asm__("$4");
        register s32 frame __asm__("$5");
        shift = temp;
        inst = &actor->instance28;
        frame = actor->animA0.frame;
        __asm__ volatile ("" : "+r" (shift), "+r" (inst), "+r" (frame));
        shift >>= 1;
        func_800175F8(inst, frame);
        temp = shift;
    }
    temp &= 0x3F;

    switch (temp) {
    case 1:
        actor->stateBD = 2;
        break;
    case 2:
        actor->stateBD = 4;
        if (actor->unkBE >= 0) {
            actor->unkC0 = actor->unkBE * 300;
        }
        break;
    }

    if (actor->stateBD == 4 && actor->unkBE >= 0) {
        actor->unkC0 -= delta;
        if (actor->unkC0 <= 0) {
            func_800448D8(&actor->animA0, 0, 1, 0);
            actor->stateBD = 0;
        }
    }

    return 0;
}

