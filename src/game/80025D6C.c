#include "game/types.h"

extern s16 D_8009FA44[];
extern s32 *D_8009FA58[];
extern s32 D_8009FA0C;

extern s32 func_8001A4E8(s32 *a, s32 *b);

void func_80025D6C(s32 *arg0)
{
    s32 i;
    register s16 *out __asm__("$18");
    register s32 **in __asm__("$17");
    s32 value;
    register s32 distance __asm__("$3");

    i = 0;
    if (i < D_8009FA0C) {
        out = D_8009FA44;
        in = D_8009FA58;
        do {
            distance = func_8001A4E8(arg0, *in + 20);
            if (distance >= 0x6000) { value = 0x168; value -= (distance - 0x6000) >> 8; if (value < 0) value = 0; } else { value = 0x168; }
            *out = value;
            out++;
            in++;
            i++;
        } while (i < D_8009FA0C);
    }
}
