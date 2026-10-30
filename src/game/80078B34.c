#include "game/types.h"

typedef struct {
    s16 unk0;
    s16 unk2;
    s16 unk4;
    s16 unk6;
} Unk800A3578;

extern Unk800A3578 D_800A3578[];

void func_80078B34(s32 arg0, s32 arg1, s32 arg2)
{
    Unk800A3578 *p;
    register s32 temp __asm__("$7");

    temp = arg0;
    if (arg1 >= 2) {
        return;
    }
    p = &D_800A3578[arg1];
    switch (arg2) {
    case 1:
        p->unk2 = 0x55;
        p->unk0 = arg0;
        break;
    case 2:
        p->unk6 = 0xEB;
        p->unk4 = arg0;
        break;
    default:
        p->unk2 = 0xA0;
        p->unk0 = temp;
        break;
    }
}
