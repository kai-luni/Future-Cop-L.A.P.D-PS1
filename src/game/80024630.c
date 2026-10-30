#include "game/types.h"

extern u8 D_8009F26C;
extern s32 D_8009F280;
extern s32 D_8009F648;

extern void func_800226D8(void);
extern void func_80022700(void);
extern void func_8002A31C(s32 arg0, s32 arg1);
extern void func_8002A544(void);

void func_80024630(s32 arg0)
{
    if (D_8009F26C != 0) {
        if (D_8009F648 == 1) {
            if (arg0 != 0) {
                func_80022700();
            } else {
                func_800226D8();
            }
            D_8009F648 = 0;
        }
        if (D_8009F280 != 0) {
            func_8002A31C(D_8009F280, 0);
            D_8009F280 = 0;
        }
        func_8002A544();
    }
}
