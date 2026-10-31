#include "game/types.h"

extern s32 D_8009F5C0;
extern s32 D_8009F5C8;
extern s32 D_8009F5CC;
extern s32 D_8009F5D0;
extern s32 D_8009F5D4;
extern s32 D_8009F5D8;
extern s32 D_8009F5DC;
extern s32 D_8009F5E0;
extern s32 D_8009F5E4;
extern volatile s32 D_8009F5E8;
extern volatile s32 D_8009F5EC;
extern volatile s32 D_8009F5F0;
extern s32 D_8009F5F4;
extern s32 D_8009F5F8;
extern s32 D_8009F5FC;
extern s32 D_8009F600;
extern s32 D_8009F9C0;
extern volatile s32 D_8009F9D4;
extern s32 D_8009F9F0;
extern s32 D_8009FA28;
extern s32 D_8009FA90;

extern s32 func_80017EBC(s32, s32);
extern u8 func_8001C034(void);
extern void func_80093178(s32, s32);

void func_8001B2C8(s32 arg0, s32 arg1)
{
    s32 allocationSize;
    register s32 value __asm__("$3");
    register s32 offset __asm__("$2");
    register s32 count28 __asm__("$4");

    D_8009F5F4 = arg0;
    D_8009F5C0 = 0;
    D_8009F5F8 = 0x400;
    D_8009F5FC = arg1;

    D_8009F5C8 = func_80017EBC(D_8009F5F4 * 0x34, 2);

    allocationSize = D_8009F5F4 * 0x34;
    D_8009F5CC = func_80017EBC(allocationSize, 2);

    allocationSize = D_8009F5F8 * 0x1C;
    D_8009F5D0 = func_80017EBC(allocationSize, 2);

    allocationSize = D_8009F5F8 * 0x1C;
    D_8009F5D4 = func_80017EBC(allocationSize, 2);

    D_8009F5D8 = func_80017EBC((arg1 + 2) * 4, 2);
    D_8009F5DC = func_80017EBC((arg1 + 2) * 4, 2);

    D_8009F600 = 0;
    D_8009F9F0 = D_8009F5C8;
    D_8009F5E0 = D_8009F5C8;
    D_8009FA90 = D_8009F5D0;
    D_8009F5E4 = D_8009F5D0;

    func_80093178(D_8009F5D8, D_8009F5FC + 2);

    value = D_8009F5F4;
    count28 = D_8009F5F8;
    offset = value * 0x34;
    offset -= 0x34;
    value = D_8009F5E0;
    value += offset;
    D_8009FA28 = value;

    offset = count28 * 0x1C;
    offset -= 0x1C;
    value = D_8009F5E4;
    value += offset;
    D_8009F9C0 = value;

    value = func_8001C034();
    if (value != 0) {
        value = 0x100;
    }
    offset = 0x100 - value;
    D_8009F9D4 = offset;
    D_8009F5EC = 0;
    D_8009F5E8 = 0;
    D_8009F5F0 = 0;
}
