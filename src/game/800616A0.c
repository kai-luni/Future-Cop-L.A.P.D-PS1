#include "game/types.h"

struct UnkSub {
    u8 unk00[0xC];
    u16 unk0C;
    s16 unk0E;
    u16 unk10;
    s16 unk12;
    u16 unk14;
    s16 unk16;
};

struct UnkEntity {
    u8 unk00[4];
    struct UnkSub *sub;
};

struct UnkEntityTable {
    s32 count;
    struct UnkEntity *items[1];
};

extern struct UnkEntityTable *D_8009F800;
extern void func_8006125C(struct UnkEntity *entity, s32 arg);

void func_800616A0(s32 elapsed)
{
    struct UnkEntity **cursor;
    struct UnkEntity *entity;
    struct UnkSub *sub;
    s32 remaining;
    s32 i;
    u16 flags;

    cursor = D_8009F800->items;
    for (remaining = D_8009F800->count; remaining > 0; remaining--) {
        entity = *cursor++;
        sub = entity->sub;
        for (i = (sub->unk10 >> 8) & 0xF; i > 0; i--) {
            func_8006125C(entity, 1);
        }
        for (i = sub->unk10 >> 12; i > 0; i--) {
            func_8006125C(entity, 0);
        }
        flags = sub->unk10;
        sub->unk10 = flags & 0xFF;
        if (sub->unk16 < sub->unk0E && sub->unk12 != 0 && (flags & 1) == 0) {
            sub->unk14 -= elapsed;
            if ((s16)sub->unk14 < 0) {
                sub->unk14 += sub->unk0C;
                func_8006125C(entity, 1);
            }
        }
    }
}
