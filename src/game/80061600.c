#include "game/types.h"

struct EntitySub2 {
    u8 unk00[0x1D];
    u8 unk1D;
};

struct EntitySub1 {
    u8 unk00[0x28];
    struct EntitySub2 *unk28;
};

struct Entity {
    void *unk00;
    struct EntitySub1 *unk04;
};

struct EntityTable {
    s32 count;
    struct Entity *items[1];
};

extern struct EntityTable *D_8009F800;

extern void func_800614BC(struct Entity *entity, s32 arg1, s32 arg2);

void func_80061600(s32 message, s32 arg1, s32 arg2)
{
    struct Entity **cursor;
    struct Entity *entity;
    s32 remaining;

    cursor = D_8009F800->items;
    for (remaining = D_8009F800->count; remaining > 0; remaining--) {
        entity = *cursor++;
        if (entity->unk04->unk28->unk1D == message) {
            func_800614BC(entity, arg1, arg2);
        }
    }
}
