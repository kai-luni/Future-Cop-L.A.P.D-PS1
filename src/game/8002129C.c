#include "game/types.h"

struct Struct800A8EA8 {
    u8 pad0[0xC];
    s32 unk0C;
    u8 pad10[0x8];
};

extern s32 D_8009F9C8;
extern s32 D_8009FA80;
extern s32 D_8009F998;
extern s32 D_8009FA34;
extern struct Struct800A8EA8 D_800A8EA8[];

void func_8002129C(void)
{
    s32 timeout = 0xFFFF;

    if (D_8009F9C8 != 0) {
        D_8009F998 = D_8009FA80;
        D_800A8EA8[D_8009FA80].unk0C = 0;
        while (D_8009FA34 != 0 && timeout-- != 0) {
        }
    }
}
