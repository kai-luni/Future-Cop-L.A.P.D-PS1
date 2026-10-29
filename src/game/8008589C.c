#include "game/types.h"

extern u8 *D_8009F4BC;
extern void func_80085228(void);
extern void func_80085630(void);

void func_8008589C(void)
{
    if (*(s32 *)(D_8009F4BC + 0x12C) != 0) {
        func_80085228();
    } else {
        func_80085630();
    }
}
