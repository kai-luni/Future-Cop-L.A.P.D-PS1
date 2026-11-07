#include "game/types.h"

struct Vec3 { s32 x, y, z; };
struct Range { s32 count; s16 data[1]; };
struct Model { u8 pad[12]; u32 flags; u8 pad10[12]; s32 **frames; u8 pad20[16]; u8 slots[8]; struct Range *range; };
struct Instance { struct Model *model; u8 pad[0x44]; s16 frame; };

void func_8003CE30(struct Instance *input, s32 index, struct Vec3 *out)
{
    register struct Instance *instance __asm__("$4") = input;
    register struct Model *model __asm__("$7");
    register struct Range *range __asm__("$3");
    s16 *p;
    s32 count;
    register s32 slot __asm__("$4");
    register s32 frame __asm__("$3");
    register u8 *slots __asm__("$4");
    register s32 **frames __asm__("$2");
    register s32 **frameptr __asm__("$3");
    register s32 offset __asm__("$2");

    if ((u32)index >= 4) goto zero;
    __asm__("" : "+r"(instance));
    model = instance->model;
    if ((model->flags >> 25) & 1) {
        range = model->range;
        if (!range || index < 0) goto zero;
        count = range->count;
        p = (s16 *)((u8 *)range + 4);
        if (index >= count) goto zero;
        p = (s16 *)((u8 *)p + instance->frame * count * 8);
    } else {
        frame = instance->frame;
        __asm__("" : "+r"(frame));
        slots = (u8 *)model + index;
        __asm__("" : "+r"(slots));
        frames = model->frames;
        __asm__("" : "+r"(frame), "+r"(frames));
        slot = slots[0x30];
        __asm__("" : "+r"(slot), "+r"(frame));
        frame <<= 2;
        __asm__("" : "+r"(frame));
        frameptr = (s32 **)(frame + (u32)frames);
        __asm__("" : "+r"(frameptr));
        offset = slot * 8;
        __asm__("" : "+r"(offset), "+r"(frameptr));
        p = (s16 *)((u8 *)frameptr[1] + (offset + 4));
        if (slot == 255) goto zero;
    }
    out->x = p[0] << 4;
    out->y = p[1] << 4;
    out->z = p[2] << 4;
    return;
zero:
    out->x = 0;
    out->y = 0;
    out->z = 0;
}
