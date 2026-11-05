#include "game/types.h"

extern u16 D_8009DDF4;
extern u16 D_8009DDF6;
extern s32 func_80093F78(s16 x, s16 y);
extern s32 func_80094010(s32 x, s32 y);
extern u32 func_800940A8(u32 x, u32 y);
extern u32 func_80093F58(s32 dfe, s32 dtd, u32 tpage);
extern s32 func_800940C4(void *p);

void func_80093AF0(u8 *arg0, u8 *arg1)
{
    u8 *rect = arg1;
    u8 *obj = arg0;

    u16 buf[4];
    u32 *p;
    s32 count;

    *(s32 *)(obj + 4) = func_80093F78(*(s16 *)(rect + 0), *(s16 *)(rect + 2));
    *(s32 *)(obj + 8) = func_80094010(
        (s16)(*(u16 *)(rect + 4) + *(u16 *)(rect + 0) - 1),
        (s16)(*(u16 *)(rect + 2) + *(u16 *)(rect + 6) - 1));
    *(s32 *)(obj + 0xC) = func_800940A8(*(s16 *)(rect + 8), *(s16 *)(rect + 0xA));
    *(s32 *)(obj + 0x10) = func_80093F58(*(u8 *)(rect + 0x17), *(u8 *)(rect + 0x16), *(u16 *)(rect + 0x14));
    *(s32 *)(obj + 0x14) = func_800940C4(rect + 0xC);
    *(s32 *)(obj + 0x18) = 0xE6000000;

    count = 7;
    if (rect[0x18] != 0) {
        buf[0] = *(u16 *)(rect + 0);
        buf[1] = *(u16 *)(rect + 2);
        buf[2] = *(u16 *)(rect + 4);
        buf[3] = *(u16 *)(rect + 6);

        buf[2] = (s16)buf[2] < 0 ? 0 : ((s16)D_8009DDF4 - 1 < (s16)buf[2] ? D_8009DDF4 - 1 : buf[2]);
        buf[3] = (s16)buf[3] < 0 ? 0 : ((s16)D_8009DDF6 - 1 < (s16)buf[3] ? D_8009DDF6 - 1 : buf[3]);

        p = (u32 *)obj;
        buf[0] -= *(u16 *)(rect + 8);
        buf[1] -= *(u16 *)(rect + 0xA);
        p[count++] = 0x60000000 | (rect[0x1B] << 16) | (rect[0x1A] << 8) | rect[0x19];
        p[count++] = *(u32 *)&buf[0];
        p[count++] = *(u32 *)&buf[2];
    }
    obj[3] = count - 1;
}
