#include "game/types.h"

extern void func_800207CC(void);
extern s32 func_80020DF8(u32 slot);

u32 func_80020E68(u32 size)
{
    register u32 k asm("s0") = size;
    register u32 j asm("s1");
    register u32 i asm("s2");
    register u32 limit asm("s3");
    register u32 result asm("s4") = 0;
    register u32 two asm("s5");
    register u32 bound asm("s6");
    register u32 tmp asm("v0");

    asm("" : "+r"(k), "+r"(result));
    func_800207CC();
    i = result;
    two = 2;
    k = (k + 0x1FFF) >> 13;
    tmp = 16;
    asm("" : "+r"(tmp));
    bound = tmp - k;
    j = k;
    do {
        k = i;
        if (i < j) {
            limit = j;
            do {
                if (func_80020DF8(k) != two)
                    break;
                k++;
            } while (k < limit);
        }
        if (k == j) {
            result = i;
            break;
        }
        i++;
        j++;
    } while (i <= bound);
    return result;
}
