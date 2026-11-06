#include "game/types.h"

typedef struct {
    volatile s32 unk00;
    u8 pad04[0x7B44];
    volatile s32 unk7B48;
    volatile s32 unk7B4C;
    u8 *unk7B50;
    u32 unk7B54;
} State;
extern State *D_8009F4D8;
extern void func_8008684C(u8 *, u32, u32);

void func_80086954(s32 arg0, State *arg1)
{
    if (arg1->unk7B48 == 1) {
        func_8008684C(arg1->unk7B50, arg1->unk7B54, 1);
        D_8009F4D8->unk7B48 = 2;
    } else if (arg1->unk7B48 == 2) {
        arg1->unk7B48 = 0;
        if (arg1->unk7B4C != 0) {
            arg1->unk00 += 0x300;
            arg1->unk7B4C = 0;
        }
    }
}
