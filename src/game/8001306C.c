#include "game/types.h"

struct Unk1306C {
    u32 unk0;
    u32 unk4;
    s32 unk8[1];
};

extern s32 *D_8009F554;
extern s32 *D_8009F558;
extern s32 *D_8009F55C;

s32 func_8001306C(struct Unk1306C *arg0)
{
    s32 *tbl;
    u32 i;

    switch (arg0->unk0) {
    case 0x3444564C:
        if (arg0->unk4 != 0) {
            tbl = D_8009F554;
            i = 0;
            do {
                arg0->unk8[i] = tbl[arg0->unk8[i]];
                i++;
            } while (i < arg0->unk4);
        }
        break;
    case 0x34444E4C:
        if (arg0->unk4 != 0) {
            tbl = D_8009F558;
            i = 0;
            do {
                arg0->unk8[i] = tbl[arg0->unk8[i]];
                i++;
            } while (i < arg0->unk4);
        }
        break;
    case 0x3344524C:
        if (arg0->unk4 != 0) {
            tbl = D_8009F55C;
            i = 0;
            do {
                arg0->unk8[i] = tbl[arg0->unk8[i]];
                i++;
            } while (i < arg0->unk4);
        }
        break;
    }
    {
        register s32 result __asm__("$2") = 0;
        __asm__ volatile ("" : "+r" (result));
        return result;
    }
}
