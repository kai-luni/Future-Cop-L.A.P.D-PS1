#include "game/types.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    u8 unk8;
    u8 pad[3];
    s32 unkC;
    s32 unk10;
} Unk800A0660;

extern s32 D_8009F5E8;
extern Unk800A0660 D_800A0660[];

void func_8001BBEC(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8 arg4)
{
    Unk800A0660 *base;
    Unk800A0660 *entry;

    base = D_800A0660;
    entry = base + D_8009F5E8;
    entry->unk8 = arg4;
    entry->unk4 = arg0;
    entry->unk0 = arg1;
    entry->unkC = arg2;
    entry->unk10 = arg3;

    D_8009F5E8++;
    if (D_8009F5E8 >= 4) {
        D_8009F5E8 = 0;
    }
}
