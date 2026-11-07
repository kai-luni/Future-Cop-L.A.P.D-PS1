#include "game/types.h"

typedef struct {
    u8 pad00[8];
    s32 unk08;
    u8 pad0C[0x7B18];
    s32 unk7B24;
    u8 pad7B28[4];
    s32 unk7B2C[4];
    s32 unk7B3C;
    s32 unk7B40;
    s32 unk7B44;
    s32 unk7B48;
    s32 unk7B4C;
    void *unk7B50;
    s32 unk7B54;
    s32 unk7B58;
    u32 unk7B5C;
} Unk8009F4D8;

extern volatile Unk8009F4D8 *D_8009F4D8;

extern void func_80098A78(s32);
extern void func_80098BF8(void *);
extern void *func_800869D8(u32);
extern void *func_80086A4C(u32);
extern void func_8008684C(void *, s32, s32);
extern s32 func_800867B0(s32);
extern void func_80098BB8(u32);
void func_80086AC0(void);

void func_80086AC0(void)
{
    void *buf;

    if (D_8009F4D8->unk08 == 0) {
        return;
    }

    func_80098A78(0);
    func_80098BF8(0);

    if (D_8009F4D8->unk7B48 == 0) {
        if ((D_8009F4D8->unk7B44 * 2 - D_8009F4D8->unk7B40) < 6) {
            buf = func_800869D8(0x300);
            if (buf != 0) {
                void *other = func_80086A4C(0x300);
                D_8009F4D8->unk7B58 = 1;
                D_8009F4D8->unk7B4C = 1;
                D_8009F4D8->unk7B48 = 1;
                D_8009F4D8->unk7B50 = other;
                D_8009F4D8->unk7B54 = D_8009F4D8->unk7B3C;
                func_8008684C(buf, D_8009F4D8->unk7B3C, 0);
                D_8009F4D8->unk7B3C += 0x300;
            } else {
                D_8009F4D8->unk7B58 = 0;
                D_8009F4D8->unk7B4C = 0;
                D_8009F4D8->unk7B48 = 1;
                D_8009F4D8->unk7B50 = (u8 *)D_8009F4D8 + 0x7810;
                D_8009F4D8->unk7B54 = 0;
                func_8008684C((u8 *)D_8009F4D8 + 0x7810, 0, 0);
            }
        }
    }

    if ((D_8009F4D8->unk7B40 & 1) == 0) {
        s32 val = D_8009F4D8->unk7B2C[(D_8009F4D8->unk7B40 >> 1) & 3];
        if (val != 0) {
            D_8009F4D8->unk7B24 = func_800867B0(val);
        }
    }

    D_8009F4D8->unk7B40++;
    func_80098BB8(D_8009F4D8->unk7B5C + (D_8009F4D8->unk7B40 & 7) * 0x180);
    func_80098BF8(func_80086AC0);
    func_80098A78(1);
}
