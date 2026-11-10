#include "game/types.h"

extern u8 *D_8009F92C;
extern s32 D_8009F8B8;
extern u8 D_8009F8C0[];

extern void func_8008204C(void);
extern s32 func_80078740(void);

void func_80082154(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
{
    if (arg0 == 3) {
        if (*(u16 *)(D_8009F92C + 0x200) == 3) {
            if (arg4 == 0) {
                func_8008204C();
            }
        } else if (D_8009F92C[0x200] == 5) {
            if (arg4 == 0) {
                register s32 idx __asm__("$2");
                register u8 *base __asm__("$3");
                func_80078740();
                idx = D_8009F8B8;
                __asm__ volatile ("" : : "r" (idx));
                base = D_8009F8C0;
                *(u8 *)(idx + base) = 1;
                *(u16 *)(D_8009F92C + 0x942) = 0;
            }
        }
    }
}
