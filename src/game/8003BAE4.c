#include "game/types.h"

struct Entity {
    u8 unk00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 unk20;
};

extern u8 D_8009F31C;

extern void func_8003B6A4(s32 arg_);
extern s32 func_8003B65C(s32 value, s32 range);
extern u8 func_8003B5AC(u8 arg0, u8 arg1, s32 arg2);
extern s32 func_8003BF50(struct Entity *entity);
extern s32 func_800402DC(struct Entity *owner, u8 column, s32 arg2, s32 row, s32 arg4);

void func_8003BAE4(struct Entity *entity, u16 flags)
{
    u8 i;
    u8 found;
    u8 column;
    s32 type;

    func_8003B6A4((s32)entity);

    found = 0;
    type = func_8003B65C(entity->unk1C, entity->unk1E);
    if (type == 3) {
        return;
    }

    D_8009F31C = 1;

    for (i = 0; i < 5; i++) {
        if (flags & 1) {
            column = func_8003B5AC(entity->unk20, i, type);
            if (column != 0) {
                func_800402DC(entity, column, 0, func_8003BF50(entity), 0);
                found = 1;
            }
        }
        flags >>= 1;
    }

    if (!found) {
        column = func_8003B5AC(entity->unk20, 0, type);
        if (column != 0) {
            func_800402DC(entity, column, 0, func_8003BF50(entity), 0);
        }
    }

    D_8009F31C = 0;
}
