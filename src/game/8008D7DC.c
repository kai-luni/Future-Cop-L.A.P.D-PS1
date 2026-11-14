#include "game/types.h"

extern void func_8008DBB0(void);
extern void func_8008C9A0(void);
extern void func_8008DB28(void);
extern void func_8008C9B0(void);
extern void func_8008D7B0(s32);
extern void func_8008D964(void);
extern void func_8008DAF0(s32, s32, s32, s32);
extern s32 D_8009C800;

void func_8008D7DC(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    func_8008DBB0();
    func_8008C9A0();
    func_8008DB28();
    func_8008C9B0();
    func_8008D7B0(0);
    func_8008D964();
    func_8008DAF0(arg0, arg1, arg2, arg3);
    D_8009C800 = 1;
    __asm__ volatile("" : : : "memory");
}
