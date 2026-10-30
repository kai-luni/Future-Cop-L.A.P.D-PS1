#include "game/types.h"

extern u8 *D_8009F468;
extern s8 D_8009F8BC;
extern s32 func_80048E28(s32 index);

void func_800788B4(void)
{
    register u8 *state __asm__("$2");
    register s32 previous __asm__("$3");
    register s32 index __asm__("$4");

    state = D_8009F468;
    previous = D_8009F8BC;
    index = *(s8 *)(state + 8);
    if (previous == index) {
        goto done;
    }
    D_8009F8BC = state[8];
    func_80048E28(index);
done:
    ;
}
