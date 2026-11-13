#include "game/types.h"

extern s8 *D_8009F468;
extern s32 D_8009FA60;
extern void func_800788F0(s32, s32, s32, s32, s32, s32, s32, s32);
extern s32 func_8004522C(void);
extern s32 func_80045288(void);
extern s32 func_800452C8(void);
extern void func_8007CE04(s32, s32, s32, s32);
extern void func_80078B34(s32, s32, s32);

void func_8007CFA0(s32 arg0)
{
    if (arg0 != 0) return;
    func_800788F0(0, 0xA0, 0x3C, 0x10, 0xFA0, 4, 8, 2);
    func_800788F0(0, 0xA0, 0x70, 0x10, 0x1004, 4, 8, 2);
    func_800788F0(0, 0xA0, 0x8D, 0x10, 0x1068, 4, 8, 2);
    func_8007CE04(0x14, 0x4B, func_8004522C(), 0x7F);
    func_8007CE04(0x14, 0x57, func_80045288(), 0x7F);
    func_8007CE04(0x14, 0x62, func_800452C8(), 0x7F);
    switch (*D_8009F468) {
    case 1:
    case 2:
    case 3: func_80078B34(0x10F4, 0, 0); break;
    case 4: func_80078B34(0x10FE, 0, 0); break;
    case 5: func_80078B34(0x10EA, 0, 0); break;
    case 0: func_80078B34(0x10D6, 0, 0); break;
    }
    func_80078B34(((D_8009FA60 >> 9) & 1) ? 0x10CC : 0x44C, 1, 0);
}
