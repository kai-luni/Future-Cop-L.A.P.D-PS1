#include "game/types.h"

extern s32 D_8009CBEC[];
extern s32 func_80091270(s32 arg0);
extern void func_80090C18(s32 arg0);
extern s32 func_8008E174(s32 mode, u8 *result);

s32 func_8009113C(s32 mode, u8 *result)
{
    register s32 ret __asm__("$16");
    register s32 *ptr __asm__("$17");
    register s32 *flag __asm__("$18");
    register s32 saved_mode __asm__("$19");
    register u8 *saved_result __asm__("$20");
    s32 now;

    saved_mode = mode;
    saved_result = result;
    ptr = D_8009CBEC;
    flag = ptr + 2;
    do {
        now = func_80091270(-1);
        if (ptr[0] + 0x4b0 < now) {
            ret = -1;
        } else {
            if (ptr[-2] < 0) goto timeout;
            now = func_80091270(-1);
            if (ptr[-1] + 0x3c < now) {
            timeout:
                func_80090C18(1);
                ret = ptr[-7];
            } else {
                ret = ptr[-2];
            }
        }
    } while (saved_mode == 0 && ((*flag != 0 && ret == 0) || ret > 0));
    func_8008E174(1, saved_result);
    if (*flag != 0 && ret == 0) {
        ret = 1;
    }
    return ret;
}
