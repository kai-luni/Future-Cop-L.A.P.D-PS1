#include "game/types.h"

struct Unk78210Mode {
    u8 unk00[0x0C];
    s32 players;
    u8 unk10[0x4C];
    s32 split;
};

extern s32 D_800A35CC;
extern struct Unk78210Mode *D_8009FA94;
extern u8 D_800A377D[];
extern u8 D_8009F459;
extern u8 D_8009F3E6;
extern void *D_8009F8C4;
extern s32 D_8009F8C8;
extern u8 D_80012094[];
extern u8 D_800A368B;

extern void func_80077708(void);
extern s32 func_80046C84(s32 id, s32 arg1);

void func_8007A4CC(s32 a0)
{
    struct Unk78210Mode *mode;

    if (a0 != 3) {
        return;
    }
    if (D_800A35CC == 0) {
        mode = D_8009FA94;
        mode->players = 1;
        mode->split = 1;
        D_800A377D[0] = 1;
        __asm__ volatile("" ::: "memory");
        D_8009F459 = 0;
        func_80077708();
        D_8009F8C8 = 5;
        D_8009F8C4 = 0;
        if (D_8009F3E6 == 0) {
            D_8009F3E6 = 1;
            D_8009F8C4 = D_80012094;
        }
        {
            register s32 b3 __asm__("$3") = 0x800A0000;
            __asm__ volatile("" : "+r"(b3));
            *(u8 *)(b3 + 0x368B) = 9;
        }
    } else {
        func_80046C84(0x4C, 0);
    }
}
