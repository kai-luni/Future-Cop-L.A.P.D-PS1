#include "game/types.h"

extern void (*D_8009F99C)(void);
extern s32 D_8009F9CC;
extern s32 D_8009FA00;
extern s32 D_8009FA60;
extern volatile s32 D_8009FA2C;
extern volatile u8 D_8009FA3C;

extern s32 func_80092C8C(s32 arg0);
extern void func_8001BFE4(void);
extern void func_8001BFA8(void);

void func_8001EFF8(void)
{
    if (D_8009F99C != 0) {
        D_8009F99C();
    }
    if (D_8009FA00 == 4 || D_8009FA00 == 13) {
        D_8009FA60 += 5;
    }
    D_8009FA2C += 5;
    if (D_8009FA3C != 0) {
        if (func_80092C8C(1) == 0) {
            D_8009FA3C = 0;
            func_8001BFE4();
            func_8001BFA8();
        }
    }
    D_8009F9CC = 1 - D_8009F9CC;
}
