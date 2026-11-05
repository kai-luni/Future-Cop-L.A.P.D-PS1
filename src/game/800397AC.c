#include "game/types.h"

extern void func_800382B4(s32 arg0);
extern void func_80060668(u8 *in, u8 *out);
extern u8 *func_80039418(u8 *object, s32 arg1, s16 arg2, s32 arg3, u8 arg4, u8 arg5, s32 arg6);

s32 func_800397AC(s32 arg0, u8 *arg1, u8 *arg2)
{
    s32 i;
    register u8 *out __asm__("$19");
    register s32 offset __asm__("$18");
    u8 *base;
    u8 *obj;
    s32 step;
    u8 *result;

    func_800382B4(2);
    i = 0;
    out = arg1;
    offset = i;
    base = arg2;
    obj = *(u8 **)(base + 8);
    step = 0x44;
    for (; i < 4; i++, step += 0x10) {
        if (*(u32 *)(*(u8 **)(base + 4) + offset + 0x20) != 0) {
            if (i > 0 && (obj[i + 0x4C] & 4) == 0) {
                func_80060668(obj + step, arg1 + 0xA0);
            }
            result = func_80039418(arg1, *(s16 *)(obj + 0x48) << 8, *(s16 *)(obj + 0x4A),
                                   i, obj[i + 0x4C], obj[i + 0x50],
                                   *(u32 *)(*(u8 **)(base + 4) + offset + 0x20));
            *(u32 *)(out + 0x158) = (u32)result;
            *(u32 *)(result + 0x24) |= 0x10;
        } else {
            *(u32 *)(out + 0x158) = 0;
        }
        out += 4;
        offset += 8;
    }
    arg1[0x73] = i;
    *(u32 *)(arg1 + 0xA0) = 0;
    return 0;
}
