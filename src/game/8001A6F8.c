#include "game/types.h"

void func_8001A6F8(s32 *src, s32 *dst)
{
    register s32 d __asm__("$6") = dst[0]; dst[0] = (d * 3 + src[0]) >> 2;
    dst[1] = (dst[1] * 3 + src[1]) >> 2;
    dst[2] = (dst[2] * 3 + src[2]) >> 2;
}
