#include "game/types.h"

struct Unk69B58 {
    u8 unk00[0x18];
    u32 unk18;
    u32 unk1C;
    u32 unk20;
    u32 unk24;
};

extern struct Unk69B58 *D_8009FA94;
extern s32 *D_8009F9F8;
extern s32 *D_8009FAD4;
extern s32 *D_8009F9A4;
extern u8 D_8009F840;

extern s32 func_80019668(void);
extern void func_80069B1C(void);

void func_80069B58(void)
{
    s32 i;
    s32 value;

    D_8009FA94->unk18 = (u32)func_80019668() % 1000;
    D_8009FA94->unk1C = (u32)func_80019668() % 1000;
    D_8009FA94->unk20 = (u32)func_80019668() % 1000;
    D_8009FA94->unk24 = (u32)func_80019668() % 1000;

    for (i = 0; i < 0x20; i++) {
        D_8009F9F8[i] = (u32)func_80019668() % 1000;
    }

    for (i = 0x63; i >= 0; i--) {
        D_8009FAD4[i] = 0;
    }

    value = -10;
    for (i = 0x1F; i >= 0; i--) {
        D_8009F9A4[i] = value;
    }

    func_80069B1C();
    D_8009F840 = 0;
}
