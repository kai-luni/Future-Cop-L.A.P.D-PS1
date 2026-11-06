#include "game/types.h"

typedef struct {
    u8 unk00[4];
    s32 unk04;
    u8 unk08[4];
    s32 unk0C;
    u8 unk10[0x78];
    s32 unk88;
} Unk800A0968;

extern Unk800A0968 D_800A0968;
extern s32 D_8009FA2C;
extern void func_800226D8(void);
extern void func_8002A23C(void);

void func_80022654(void)
{
    if (D_800A0968.unk04 != 0) {
        if (D_800A0968.unk0C != 0) {
            func_800226D8();
        } else if (D_800A0968.unk88 < D_8009FA2C) {
            D_800A0968.unk88 = -1;
            func_8002A23C();
        }
    }
}
