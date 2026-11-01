#include "game/types.h"

extern s32 D_8009FA60, D_8009FA2C;
extern s32 D_8009F9CC, D_8009F99C;
extern u8 D_8009FA20;
extern u8 D_8001EFF8[];

extern void func_80091510(void *arg0);

void func_8001F0B8(void)
{
    register u8 *callback_table __asm__("$4");
    callback_table = D_8001EFF8;
    D_8009FA60 = 0;
    D_8009FA2C = 0;
    D_8009F9CC = 0;
    D_8009F99C = 0;
    func_80091510(callback_table);
    D_8009FA20 = 1;
}
