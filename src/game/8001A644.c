#include "game/types.h"

extern s32 func_80019F54(s32 *vector);
extern s32 func_80095C14(u32 value);

s32 func_8001A644(s32 *vector)
{
    s32 result;
    s32 shift;

    result = func_80019F54(vector);
    shift = result ? 0x1F - func_80095C14(result) : 0;
    if (shift >= 0x10) {
        s32 s, x, y, z;
        s = shift - 0xF;
        x = vector[0];
        y = vector[1];
        z = vector[2];
        vector[0] = x >> s;
        vector[1] = y >> s;
        vector[2] = z >> s;
    } else {
        s32 s, x, y, z;
        s = 0xF - shift;
        x = vector[0];
        y = vector[1];
        z = vector[2];
        vector[0] = x << s;
        vector[1] = y << s;
        vector[2] = z << s;
    }
    return result;
}
