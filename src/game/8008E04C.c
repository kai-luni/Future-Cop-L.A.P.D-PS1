#include "game/types.h"

extern void func_8008F89C(void);
extern s32 func_8008F8E8(void);
extern s32 func_8008F7AC(void);

s32 func_8008E04C(s32 mode)
{
    if (mode == 2) {
        func_8008F89C();
        return 1;
    }
    if (func_8008F8E8() != 0) {
        return 0;
    }
    if (mode == 1) {
        if (func_8008F7AC() != 0) {
            return 0;
        }
    }
    return 1;
}
