#include "game/types.h"

struct Unk4F0F0Entry {
    s32 unk00;
    u8 flags;
    u8 unk05[7];
};

struct Object {
    void *entries;
    u8 unk04[0x2C];
    s16 unk30;
    s16 unk32;
    u8 unk34[8];
    u32 flags;
    u8 unk40[2];
    s16 unk42;
};

struct Command {
    u8 op;
    u8 arg;
};

struct Script {
    u8 unk00[0x0C];
    struct Command commands[4];
    u8 unk14[0x70];
    s32 unk84;
};

extern void func_8002A5B4(struct Script *script, s32 index);

void func_8004F188(struct Object *object, struct Script *script)
{
    s32 i;
    u32 op;
    s32 arg;
    u8 *p;

    for (i = 0; i < 4;) {
        op = script->commands[i].op;
        if (op == 0) {
            break;
        }
        arg = script->commands[i].arg;
        switch (op) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            object->unk42 = arg;
            object->flags = (object->flags & 0xF0FFFFFF) | ((op - 1) << 24);
            break;

        case 19:
            if (arg == 0) {
                object->flags |= 0x40;
                script->unk84 = 0;
            } else {
                object->flags &= ~0x40;
                object->unk32 = object->unk30;
            }
            break;

        case 20:
        case 21:
            if (op == 21) {
                p = (u8 *)object->entries + (arg * 12 + 4);
                *p |= 2;
                break;
            } else {
                p = (u8 *)object->entries + (arg * 12 + 4);
                *p &= ~2;
                break;
            }

        case 22:
            arg = arg * 41;
            object->unk30 = (arg * 256 / 300) >> 4;
            break;

        default:
            i++;
            continue;
        }

        func_8002A5B4(script, i);
    }
}
