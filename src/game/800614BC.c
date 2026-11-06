#include "game/types.h"

struct Unk614BCSub {
    u8 unk00[0xE];
    s16 unk0E;
    u16 flags;
    s16 unk12;
    u16 unk14;
    s16 unk16;
};

struct Unk614BCObj {
    u8 unk00[4];
    struct Unk614BCSub *sub;
};

void func_800614BC(struct Unk614BCObj *obj, s32 arg1, s32 arg2)
{
    register struct Unk614BCSub *sub __asm__("$4") = obj->sub;
    u16 f;
    u16 v;

    switch (arg1) {
    case 0x46:
        if (arg2) {
            if (sub->flags & 1) {
                sub->flags &= 0xFFFE;
                sub->unk14 = 0;
            }
            return;
        }
        f = sub->flags | 1;
        break;
    case 0x47:
        if (sub->unk16 >= sub->unk0E) {
            return;
        }
        if (sub->unk12 == 0) {
            return;
        }
        if (arg2) {
            register u32 raw __asm__("$3");
            register u32 part __asm__("$2");
            raw = sub->flags;
            __asm__ volatile ("" : "+r" (raw));
            part = ((raw >> 8) & 0xF) + 1;
            __asm__ volatile ("" : "+r" (raw), "+r" (part));
            raw = (raw & 0xF0FF) | (part << 8);
            sub->flags = raw;
            return;
        }
        {
            register u32 raw __asm__("$2");
            register u32 high __asm__("$3");
            raw = sub->flags;
            __asm__ volatile ("" : "+r" (raw));
            high = raw & 0xF000;
            raw &= 0xFFF;
            __asm__ volatile ("" : "+r" (raw), "+r" (high));
            high += 0x1000;
            f = raw | high;
        }
        break;
    default:
        return;
    }
    sub->flags = f;
}
