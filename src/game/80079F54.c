#include "game/types.h"

struct UnkA3808 {
    u8 unk00[0x24];
    u32 unk24;
};

struct UnkB4E4 {
    u8 unk00[0x1F];
    u8 unk1F;
    u8 unk20;
    u8 pad[0x03];
};

extern struct UnkA3808 D_800A3808;
extern u8 D_8009F3E8;
extern u8 D_8009FA10;
extern u8 D_80011E60[];
extern struct UnkB4E4 D_8009B4E4[];

extern void func_80082DDC(s32 arg0, s32 arg1);
extern void func_8007A168(void);

void func_80079F54(void)
{
    u32 i;

    if (D_800A3808.unk24 != 0) {
        if (D_8009F3E8 != 0) {
            func_80082DDC((s32)D_80011E60, 0);
            D_8009F3E8 = 0;
        }
        D_800A3808.unk24 = 0;
    }
    D_8009FA10 = 0;
    for (i = 0; i < 16; i++) {
        D_8009B4E4[i].unk1F = 0;
        D_8009B4E4[i].unk20 = 0;
    }
    func_8007A168();
}
