#include "game/types.h"

extern s32 D_8009F934;
extern void *D_8009F938;
extern void *D_8009F930;

extern void func_800181D4(void *block);
extern void func_80019584(s32 pool);

void func_80083428(void)
{
    func_80019584(D_8009F934);
    D_8009F934 = 0;
    if (D_8009F938 != 0) {
        func_800181D4(D_8009F938);
    }
    D_8009F938 = 0;
    if (D_8009F930 != 0) {
        func_800181D4(D_8009F930);
    }
}
