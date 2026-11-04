#include "game/types.h"

#define CLAMP(v, lo, hi) ((v) < (lo) ? (lo) : ((v) > (hi) ? (hi) : (v)))

struct Unk45884 {
    s32 position[3];
    s32 angle;
};

extern struct Unk45884 D_800A1920[];
extern s32 D_8009F734;

extern void func_8001A578(u32 angle, s32 *vec);

void func_800460D4(s8 *mix)
{
    if (mix[2] == 0) {
        func_8001A578(D_800A1920[0].angle, (s32 *)(mix + 4));
        if (D_8009F734 == 2) {
            func_8001A578(D_800A1920[1].angle, (s32 *)(mix + 0xC));
            mix[1] = CLAMP(CLAMP((*(s32 *)(mix + 4) >> 11) + 0x20, 0, 0x40) +
                           CLAMP((*(s32 *)(mix + 0xC) >> 11) + 0x20, 0, 0x40),
                           0, 0x7F);
        } else {
            s32 pan = CLAMP((*(s32 *)(mix + 4) >> 10) + 0x40, 0, 0x7F);
            mix[1] = pan;
        }
        mix[2] = 1;
    }
}
