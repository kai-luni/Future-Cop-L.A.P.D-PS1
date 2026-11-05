#include "game/types.h"

typedef struct {
    u8 pad00[0xC];
    s32 unk0C;
    u8 pad10[0x4C];
    s32 unk5C;
} Unk8009FA94;

typedef struct {
    u8 pad00[0x24];
    void *unk24;
} Unk8009F468;

extern Unk8009F468 *D_8009F468;
extern u8 D_8009FA10;
extern Unk8009FA94 *D_8009FA94;
extern u8 D_800A3808[];
extern void func_800787D8(s32, s32, s32);

void func_8007B3F4(void)
{
    Unk8009FA94 *g;

    D_8009F468->unk24 = D_800A3808;
    if (D_8009FA10 != 0) {
        g = D_8009FA94;
        if (g->unk5C == 0 || g->unk0C == 1) {
            func_800787D8(3, 1, 0);
        }
    }
}
