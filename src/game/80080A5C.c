#include "game/types.h"

extern s32 func_800451E0(void);
extern s32 func_8004522C(void);
extern s32 func_80045288(void);
extern s32 func_800452C8(void);
extern s32 func_8001EB2C(s32 arg0);

extern u8 D_8009FA98;
extern u8 D_8009FA99[];

void func_80080A5C(u8 *arg0)
{
    arg0[4] = func_8004522C();
    arg0[5] = func_80045288();
    arg0[6] = func_800452C8();
    arg0[7] = func_800451E0();
    arg0[0xA] = func_8001EB2C(0);
    arg0[0xB] = func_8001EB2C(1);
    arg0[8] = D_8009FA98;
    arg0[9] = D_8009FA99[0];
}
