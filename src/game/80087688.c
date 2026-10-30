#include "game/types.h"

typedef struct {
    s32 unk0;
    s32 unk4;
    s32 unk8;
    s32 unkC;
    u8 pad[0x7B14];
    volatile s32 unk7B24;
    s32 unk7B28;
    s32 unk7B2C[4];
    s32 unk7B3C;
} Unk8009F4D8;

extern Unk8009F4D8 *D_8009F4D8;
extern void func_8008C9C0(void);
extern void func_8008C9E0(void);

void func_80087688(void)
{
    u32 i;

    func_8008C9C0();

    D_8009F4D8->unk0 = 0;
    D_8009F4D8->unk4 = 0;
    D_8009F4D8->unkC = 0;

    for (i = 0; i < 4; i++) {
        D_8009F4D8->unk7B2C[i] = 0;
    }

    D_8009F4D8->unk7B3C = 1;
    D_8009F4D8->unk7B24 = 0;

    func_8008C9E0();
}
