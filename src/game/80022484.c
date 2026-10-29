/* Exact .text reconstruction with GCC-2.7.2-cdk.
 * Two GNU C register declarations bind local4_loaded to $7 (a3) and
 * local0_loaded to $2 (v0). They contain no assembly instruction or barrier.
 * The ordinary-C alternative without these declarations remains available
 * separately; it matches 112 of the 116 instruction words.
 */
#include "game/types.h"


struct Entry22484 {
    u8 pad00[0x10];
    u32 zero10;
    u8 pad14[4];
    u8 zero18;
    u8 pad19[3];
    s32 id1C;
    u32 pad20;
};
struct State22484 {
    u8 pad00[0x14];
    u32 count;
    u8 pad18[0xC];
    struct Entry22484 entries[3];
};

extern u8 D_800A0968[];
extern u32 D_8009F630;
extern u32 D_8009F634;
extern u32 D_8009F638;
extern u32 D_8009F63C;

extern void func_8001E4C0(void *dst, s32 value, s32 size);
extern void func_80022B84(s32 *arg0, s32 index, u32 *out, s32 zero);
extern s32 func_80022BEC(void);
extern void func_80022CA8(void);

s32 func_80022484(s32 arg0, ...)
{
    u32 local0;
    u32 local4;
    u32 local8;
    u32 localC;
    u32 local10;
    u32 i;
    u32 tmp;
    u32 tmp2;
    register u32 local4_loaded __asm__("$7");
    register u32 local0_loaded __asm__("$2");
    u32 local10_loaded;

    s32 result;
    u8 *base;
    u8 *p;

    local4 = 5;
    local8 = 0xAC44;
    localC = 0x400;
    local0 = 0;
    local10 = 0;

    base = D_800A0968;
    func_8001E4C0(base, 0, 0x98);

    D_8009F630 = 0;
    D_8009F638 = 0;
    D_8009F63C = 0;

    func_80022B84(&arg0, 6, &local0, 0);
    func_80022B84(&arg0, 3, &local4, 0);
    func_80022B84(&arg0, 1, &local8, 0);
    func_80022B84(&arg0, 2, &localC, 0);
    func_80022B84(&arg0, 0xA, &local10, 0);

    local0_loaded = local0;
    tmp = (localC * 3 + (localC >> 1)) * 300 / local8;
    local4_loaded = local4;
    local10_loaded = local10;
    tmp2 = (local8 << 12) / 0xAC44;

    *(u32 *)(base + 0x90) = 0;
    *(u32 *)(base + 0x94) = 0;
    *(u32 *)(base + 0x1C) = local8;
    *(u32 *)(base + 0x6C) = localC;
    *(u32 *)(base + 0x70) = local4_loaded;
    D_8009F634 = local0_loaded;
    *(u32 *)(base + 0x14) = local10_loaded;
    *(u32 *)(base + 0x18) = tmp2;
    *(u32 *)(base + 0x8C) = (tmp + 10) * 3;

    result = func_80022BEC();
    func_80022CA8();

    for (i = 0; i < ((struct State22484 *)D_800A0968)->count; i++) {
        ((struct State22484 *)base)->entries[i].zero18 = 0;
        ((struct State22484 *)base)->entries[i].id1C = -1;
        ((struct State22484 *)base)->entries[i].zero10 = 0;
    }

    D_800A0968[0] = 1;
    return result;
}
