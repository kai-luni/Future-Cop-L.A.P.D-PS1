#include "game/types.h"

struct Unk800A0B70 {
    u8 unk00[0x2C];
    void *unk2C;
};

extern struct Unk800A0B70 D_800A0B70;
extern s32 D_8009F6E8;
extern void func_80019584();

void func_80029E64(void)
{
    if (D_800A0B70.unk2C != 0 && D_8009F6E8 == 0) {
        func_80019584(D_800A0B70.unk2C);
        D_800A0B70.unk2C = 0;
    }
}
