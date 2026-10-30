#include "game/types.h"

struct Unk3C404 {
    u8 unk00[0x14];
    u8 unk14;
    u8 unk15[0x13];
    s16 unk28;
    s16 unk2A;
};

extern u32 D_8009F310;
extern void *D_800A7040[][0x100];
extern u32 *func_8003B3D8(u32 column, u32 row);
extern void func_800181D4(void *block);

void func_8003C404(struct Unk3C404 *arg0)
{
    *(u8 *)func_8003B3D8(arg0->unk14, D_8009F310) = 0;
    if (D_800A7040[D_8009F310][arg0->unk14] != 0) {
        func_800181D4(D_800A7040[D_8009F310][arg0->unk14]);
    }
    if (arg0->unk28 == 1) {
        arg0->unk2A = 0;
    }
    if (arg0->unk2A <= 0) {
        arg0->unk2A = 0;
        arg0->unk28 = -1;
    }
    D_800A7040[D_8009F310][arg0->unk14] = arg0;
}
