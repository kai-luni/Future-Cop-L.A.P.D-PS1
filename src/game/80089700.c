#include "game/types.h"

typedef struct {
    s32 w[6];
} Src89700;

typedef struct {
    u8 pad0[0xC];
    s32 w[6];
    u8 pad1c[0x38 - 0x24];
} Ent89700;

typedef struct {
    u8 pad0[0x414];
    s32 unk414;
    s32 unk418;
    s32 unk41C;
    s32 unk420;
    volatile s32 unk424;
    volatile s32 unk428;
    s32 unk42C;
    s32 unk430;
    s32 unk434;
    s32 unk438;
    s32 unk43C;
    s32 unk440;
    u8 pad444[4];
    s32 unk448;
    s32 unk44C;
    u8 pad450[0x570 - 0x450];
    s32 unk570;
    s32 unk574;
    s32 unk578;
} Unk89700;

extern Unk89700 *D_8009F970;
extern u8 *D_8009F974;
extern u8 *D_8009F978;
extern u8 *D_8009F97C;
extern s32 D_8009F980;
extern s32 D_8009F984;
extern s32 D_8009F988;

extern void *func_8001E4C0(void *dst, s32 val, u32 count);
extern s32 func_8008E5D8(s32 cb);
extern s32 func_80091240(s32 v);
extern void func_80088F00(void);
extern void func_800895E4(s32 arg);

s32 func_80089700(Unk89700 *arg0, s32 arg1, Src89700 *arg2, u32 arg3)
{
    u32 i;
    s32 t;
    u8 *p;

    t = (D_8009F980 << 11) + 0x57C;
    func_8001E4C0(arg0, 0, D_8009F984 + t + D_8009F988 * 56);
    D_8009F970 = arg0;
    p = (u8 *)arg0 + 0x57C;
    D_8009F974 = p;
    arg0->unk570 = arg1;
    arg0->unk414 = 0;
    arg0->unk418 = 0;
    arg0->unk41C = 0;
    arg0->unk43C = 0;
    arg0->unk434 = 0;
    arg0->unk430 = 0;
    arg0->unk42C = 0;
    arg0->unk440 = 0;
    arg0->unk448 = 0;
    arg0->unk44C = 0;
    D_8009F978 = p + D_8009F988 * 56;
    D_8009F97C = D_8009F978 + D_8009F984;
    for (i = 0; i < arg3; i++) {
        *(Src89700 *)(D_8009F974 + i * 56 + 12) = arg2[i];
    }
    D_8009F970->unk438 = arg3;
    D_8009F970->unk424 = 0;
    D_8009F970->unk428 = 0;
    D_8009F970->unk574 = func_8008E5D8(0);
    D_8009F970->unk578 = func_80091240(0);
    func_80088F00();
    func_800895E4(1);
    return 1;
}
