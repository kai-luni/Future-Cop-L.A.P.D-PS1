#include "game/types.h"

typedef struct {
    s32 unk00;
    u8 pad04[0x7B28];
    s32 unk7B2C[4];
    u8 pad7B3C[8];
    volatile s32 unk7B44;
    s32 unk7B48;
    s32 unk7B4C;
    s32 unk7B50;
    s32 unk7B54;
    s32 unk7B58;
    s32 unk7B5C[4];
} Unk8009F4D8;

extern Unk8009F4D8 *D_8009F4D8;

extern void func_80098F48(s32 mode);
extern u32 func_80098EE8(u32 address);
extern void func_80098E88(u8 *data, u32 size);

void func_8008684C(u8 *arg0, u32 arg1, u32 arg2)
{
    u32 i;
    u32 addr;

    for (i = 0; i < 0x300; i += 0x10) {
        arg0[i + 1] = 2;
    }

    if ((D_8009F4D8->unk7B44 & 3) == 0) {
        arg0[1] = 6;
    } else if (((D_8009F4D8->unk7B44 + 1) & 3) == 0) {
        arg0[0x2F1] = 3;
    }

    D_8009F4D8->unk7B2C[D_8009F4D8->unk7B44 & 3] = arg1;

    addr = D_8009F4D8->unk7B5C[arg2] + (D_8009F4D8->unk7B44 & 3) * 0x300;

    if (arg2 == 1) {
        D_8009F4D8->unk7B44 = D_8009F4D8->unk7B44 + 1;
    }

    func_80098F48(0);
    func_80098EE8(addr);
    func_80098E88(arg0, 0x300);
}
