#include "game/types.h"

extern u8 D_80012BC8;
extern volatile u32 *D_8009E9F4;
extern volatile u32 *D_8009E9C8;
extern volatile u32 *D_8009E9D4;
extern void func_8008E000(void *arg0, s32 arg1);

s32 func_8009649C(s32 arg0)
{
    register s32 result __asm__("$2");
    func_8008E000(&D_80012BC8, arg0);
    *D_8009E9F4 = 0x80000000;
    *D_8009E9C8 = 0;
    *D_8009E9D4 = 0;
    __asm__ volatile ("" : : : "memory");
    result = 0;
    __asm__ volatile ("" : "=r"(result) : "0"(result));
    (void)*D_8009E9D4;
    *D_8009E9F4 = 0x60000000;
    return result;
}
