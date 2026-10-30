#include "game/types.h"

extern void func_8001E9AC(s32 port, s32 enable);

void func_8001E96C(s32 port, s32 enable)
{
    s32 clamped;

    if (port >= 0) {
        if (port < 2) {
            clamped = port;
        } else {
            clamped = 1;
        }
    } else {
        clamped = 0;
    }
    func_8001E9AC(clamped, enable);
}
