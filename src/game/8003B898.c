#include "game/types.h"

struct Entity {
    u8 pad00[0x20];
    u8 column;
    u8 pad21[3];
    u32 flags;
};

struct Entry {
    u8 pad00[0x1A];
    u8 column;
};

extern struct Entry *D_800A0C70[];
extern u8 func_8003B4E0(u8 index, u8 slot, u8 mode);
extern s32 func_8003BF50(struct Entity *entity);
extern s32 func_800402DC(struct Entity *entity, u8 column, s32 arg2, s32 row, s32 arg4);

void func_8003B898(u8 index, struct Entity *entity, s32 arg2, u16 flags, s32 mode)
{
    u8 i;
    u8 found;
    u8 column;
    u8 special;

    found = 0;
    special = mode == 2;
    if (entity->flags & 0x40000) {
        func_800402DC(entity, entity->column, arg2, func_8003BF50(entity), 0);
    } else if (D_800A0C70[index] != 0) {
        if (mode == 0) {
            if (D_800A0C70[index]->column != 0) {
                func_800402DC(entity, D_800A0C70[index]->column, arg2,
                             func_8003BF50(entity), 0);
            }
        } else {
            for (i = 0; i < 5; i++) {
                if (flags & 1) {
                    column = func_8003B4E0(index, i, special);
                    if (column != 0) {
                        func_800402DC(entity, column, arg2, func_8003BF50(entity), 0);
                        found = 1;
                    }
                }
                flags >>= 1;
            }
            if (!found) {
                column = func_8003B4E0(index, 0, special);
                if (column != 0) {
                    func_800402DC(entity, column, arg2, func_8003BF50(entity), 0);
                }
            }
        }
    }
}
