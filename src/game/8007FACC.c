#include "game/types.h"

extern u8 D_8009F496;
extern u8 D_8009F8BD;
extern u8 D_8009F8BE;

extern void func_8002078C(void);

void func_8007FACC(void)
{
    if (D_8009F496) {
        D_8009F8BD = 1;
        D_8009F8BE = 1;
        func_8002078C();
        D_8009F496 = 0;
    }
}
