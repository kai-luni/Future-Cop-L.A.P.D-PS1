#include "game/types.h"

struct Unk78210Mode {
    u8 unk00[0x0C];
    s32 players;
    u8 unk10[0x4C];
    s32 split;
};

extern s32 D_800A35CC;
extern struct Unk78210Mode *D_8009FA94[];
extern u8 D_8009F3F1;
extern u8 D_8009F458;
extern u8 D_8009F3E7;
extern void *D_8009F8C4;
extern s32 D_8009F8C8;
extern u8 D_800A377D[];
extern u8 D_800A386D[];
extern u8 D_80012088[];

extern void func_800776B8(void);
extern s32 func_80046C84(s32 id, s32 arg1);

void func_8007A3AC(s32 a0)
{
    struct Unk78210Mode *mode;

    if (a0 != 3) {
        return;
    }
    if (D_800A35CC == 0) {
        mode = D_8009FA94[0];
        D_8009F3F1 = 0;
        mode->players = 1;
        mode->split = 0;
        D_800A377D[0] = 1;
        D_800A386D[0] = 1;
        __asm__ volatile("" ::: "memory");
        D_8009F458 = 0;
        func_800776B8();
        D_8009F8C8 = 4;
        D_8009F8C4 = 0;
        if (D_8009F3E7 == 0) {
            D_8009F3E7 = 1;
            D_8009F8C4 = D_80012088;
        }
    } else {
        func_80046C84(0x4C, 0);
    }
}
