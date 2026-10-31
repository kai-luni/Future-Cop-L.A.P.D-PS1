#include "game/types.h"

extern u8 *D_8009F92C;

extern void func_8007F688(u8 *arg0, s32 arg1, u8 *arg2, u8 *arg3);

void func_8007F6E0(void)
{
    u32 i;
    s32 offset;

    func_8007F688(D_8009F92C + 0x40C, 0x10, D_8009F92C + 0x41C, D_8009F92C + 0x420);
    for (i = 0, offset = 0x424; i < 10; i++) {
        func_8007F688(D_8009F92C + offset, 0x68, D_8009F92C + offset + 0x68, D_8009F92C + offset + 0x6C);
        offset += 0x70;
    }
}
