#include "game/types.h"

typedef struct {
    u8 pad00[8];
    s32 unk08;
    s32 unk0C;
    u8 pad10[0x10];
    u8 unk20;
    u8 pad21[0x23];
    s32 unk44;
    s32 unk48;
    s32 unk4C;
} Unk800A0B70;

extern Unk800A0B70 D_800A0B70;
extern u8 D_8009F6B8;
extern s32 D_8009F6DC;
extern s32 D_8009F6E0;
extern s32 D_8009F6E4;

extern s32 func_80045288(void);

void func_8002A16C(s32 arg0)
{
    Unk800A0B70 *p;
    s32 value;

    if ((arg0 & 0xFF) != 0) {
        value = func_80045288();
    } else {
        value = 1;
    }

    p = &D_800A0B70;

    if (p->unk44 != 0 && value > 0 && p->unk20 != 0 &&
        !(D_8009F6B8 & 0x40)) {
        if (D_8009F6DC == 0) {
            D_8009F6DC = 3;
            D_8009F6E0 = 0;
        } else {
            D_8009F6DC = 4;
        }

        p->unk0C = -1;
        p->unk20 = 0;
        p->unk08 = p->unk4C + p->unk48 + D_8009F6E4 * 0x6000;
    }
}
