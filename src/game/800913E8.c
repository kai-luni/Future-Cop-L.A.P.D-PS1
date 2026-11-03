#include "game/types.h"

extern volatile s32 D_8009DD50;

extern void func_8008CA20(s32, s32);
extern void func_8008D7B0(s32);
extern void func_8008FF08(void *);

void func_800913E8(s32 arg0, s32 arg1)
{
    volatile s32 temp = arg1 << 15;

    while (D_8009DD50 < arg0) {
        if (--temp == -1) {
            register u8 *base __asm__("$4") = (u8 *)0x80010000;
            __asm__("" : "=r"(base) : "0"(base));
            func_8008FF08(base + 0x27EC);
            func_8008D7B0(0);
            func_8008CA20(3, 0);
            break;
        }
    }
}
