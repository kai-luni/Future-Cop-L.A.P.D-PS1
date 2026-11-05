#include "game/types.h"

struct Unk78210Mode {
    u8 unk00[0x0C];
    s32 players;
    u8 unk10[0x4C];
    s32 split;
};

extern struct Unk78210Mode *D_8009FA94;
extern u8 D_8009F459;
extern u8 D_8009F3E6;
extern void *D_8009F8C4;
extern s32 D_8009F8C8;
extern u8 D_800A377D[];
extern u8 D_800A368B[];
extern u8 D_80012094[];

extern void func_80077708(void);

void func_8007A56C(s32 a0)
{
    register struct Unk78210Mode *mode __asm__("$3");

    if (a0 != 3) {
        return;
    }
    mode = D_8009FA94;
    mode->players = 2;
    mode->split = 1;
    D_800A377D[0] = 0;
    __asm__ volatile("" ::: "memory");
    D_8009F459 = 0;
    func_80077708();
    D_8009F8C8 = 5;
    D_8009F8C4 = 0;
    if (D_8009F3E6 == 0) {
        D_8009F3E6 = 1;
        D_8009F8C4 = D_80012094;
    }
    D_800A368B[0] = 5;
}
