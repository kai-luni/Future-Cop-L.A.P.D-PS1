#include "game/types.h"

void func_800959F0(u32 arg0, u32 arg1) {
    arg0 <<= 16;
    arg1 <<= 16;
    __asm__ volatile("ctc2 %0, $24" : : "r"(arg0));
    __asm__ volatile("ctc2 %0, $25" : : "r"(arg1));
}
