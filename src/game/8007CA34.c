#include "game/types.h"

extern s32 D_8009F908;
extern s32 D_8009F90C;
extern s32 D_8009F910;
extern s32 D_8009F914;
extern s32 D_8009F91C;

extern void func_800451F8(s32 volume);
extern void func_80045238(s32 volume);
extern void func_80045294(s32 volume);
extern void func_800451B4(s32 enable);
extern void func_80046A34(s32 handle);
extern s32 func_80045288(void);
extern s32 func_8002A58C(void);
extern void func_80024630(s32 arg0);

void func_8007CA34(void)
{
    register s32 flag __asm__("$2");

    if (flag == 0) {
        func_800451F8(D_8009F908);
        func_80045238(D_8009F90C);
        func_80045294(D_8009F910);
        func_800451B4(D_8009F914);
    }
    if (D_8009F91C != 0) {
        func_80046A34(D_8009F91C);
        D_8009F91C = 0;
    }
    if (func_80045288() == 0) {
        if ((func_8002A58C() & 0xFF) == 1) {
            func_80024630(0);
        }
    }
}
