#include "game/types.h"

typedef struct Ent88F00 {
    u32 w0;
    s32 w4;
    s32 w8;
    s32 padC;
    u32 w10;
    u8 pad14[0x14];
    s32 w28;
    s32 w2C;
    s32 w30;
    s32 w34;
} Ent88F00;

typedef struct {
    u8 pad0[0x42C];
    Ent88F00 *unk42C;
    Ent88F00 *unk430;
    u32 unk434;
    u32 unk438;
    s32 unk43C;
    s32 unk440;
    s32 unk444;
    s32 unk448;
    s32 unk44C;
} Unk88F00;

extern Unk88F00 *D_8009F970;
extern Ent88F00 *D_8009F974;
extern s32 D_8009F978;
extern s32 D_8009F97C;

s32 func_80088F00(void)
{
    if (D_8009F970->unk43C == 0) {
        if (D_8009F970->unk434 < D_8009F970->unk438) {
            D_8009F970->unk430 = D_8009F974 + D_8009F970->unk434;
            if (D_8009F970->unk42C == 0) {
                D_8009F970->unk42C = D_8009F970->unk430;
            }
            D_8009F970->unk430->w0 = (D_8009F970->unk430->w10 + 0x7FF) >> 11;
            D_8009F970->unk430->w4 = 0;
            D_8009F970->unk430->w8 = 0;
            D_8009F970->unk430->w28 = 0;
            if (D_8009F970->unk434 == 0) {
                D_8009F970->unk444 = 0;
                D_8009F970->unk440 = 0;
                D_8009F970->unk448 = 0;
                D_8009F970->unk44C = 0;
            }
            D_8009F970->unk434++;
            D_8009F970->unk430->w34 = 0;
            D_8009F970->unk430->w2C = D_8009F978;
            D_8009F970->unk430->w30 = D_8009F97C;
            D_8009F970->unk43C = 0;
        } else {
            D_8009F970->unk43C = 2;
        }
        return D_8009F970->unk43C;
    }
    return D_8009F970->unk43C;
}
