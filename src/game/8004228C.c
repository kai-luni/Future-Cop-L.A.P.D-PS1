#include "game/types.h"

typedef struct {
    u8 pad00[0x24];
    u32 flags;
    u8 pad28[0x78];
    u8 unkA0;
    u8 padA1[3];
    s16 unkA4;
} Object;

extern s32 D_8009FAC8;
extern s32 D_8009F9B4;
extern s32 D_8009F9B0;

void func_8004228C(Object *obj)
{
    s32 active;
    if (obj->unkA0 & 1) {
        active = D_8009FAC8;
        if ((obj->unkA0 & 2) && (!active || D_8009F9B4 != obj->unkA4)) {
            obj->unkA4 = -10;
            obj->unkA0 &= ~3;
            obj->flags &= ~8;
            D_8009F9B0--;
        } else if (active) {
            obj->unkA0 |= 2;
        }
    }
}
