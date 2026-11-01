#include "game/types.h"

struct FixedPool;

extern s32 D_800A0C70[];
extern s32 D_800A7040[2][256];
extern s32 D_800A1070[];

extern s32 D_8009F310;
extern s32 D_8009F314;
extern struct FixedPool *D_8009F70C;

extern void func_8006EDC4(void);
extern void func_8005E1BC(void);
extern struct FixedPool *func_800194DC(s32 count, s32 size, s32 tag);

void func_8003F79C(void)
{
    s32 i;
    s32 j;

    for (i = 0xFF; i >= 0; i--) {
        D_800A0C70[i] = 0;
    }

    for (j = 0; j < 2; j++) {
        s32 *p = D_800A7040[j];
        for (i = 0xFF; i >= 0; i--) {
            p[i] = 0;
        }
    }

    D_8009F310 = 0;
    func_8006EDC4();
    func_8005E1BC();
    D_8009F70C = func_800194DC(0x100, 8, 1);
    D_8009F314 = 0;

    for (i = 6; i >= 0; i--) {
        D_800A1070[i] = 0;
    }
}
