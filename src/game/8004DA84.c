#include "game/types.h"

struct EntitySlot {
    u8 kind;
    u8 value;
};

struct Entity {
    struct Entity *next;
    void *update;
    s32 id;
    struct EntitySlot slots[4];
    u8 unk14;
    u8 unk15;
    u8 type;
};

extern u8 func_8004DA58(u32 value);
extern u8 func_8004D9EC(struct Entity *target, s32 kind, s32 value);

u8 func_8004DA84(struct Entity *entity, s32 kind, s32 value, s32 retry)
{
    s32 i;

    switch (entity->type) {
    case 18:
    case 21:
        if (func_8004DA58(kind) != 0) {
            break;
        }
        /* Failed filter joins the immediate-success return. */
    case 2:
    case 3:
    case 4:
    case 7:
    case 15:
    case 23:
        return 1;
    case 5:
    case 6:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13:
    case 14:
    case 16:
    case 17:
    case 19:
    case 20:
    case 22:
    default:
        break;
    }

    for (i = 0; i < 4; i++) {
        if (entity->slots[i].kind == 0) {
            entity->slots[i].kind = kind;
            entity->slots[i].value = value;
            return 1;
        }
    }

    if (retry != 0) {
        return func_8004D9EC(entity, kind, value);
    }
    return 0;
}
