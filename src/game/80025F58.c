#include "game/types.h"

struct Unk25F58 {
    u8 unk00[4];
    u8 *unk04;
    u8 *unk08;
    u8 *unk0C;
};

extern struct Unk25F58 *D_8009F2CC;
extern u8 D_8009F2EC[];
extern u8 D_8009ABE8[];

extern s32 func_800451E0(void);
extern s32 func_8004522C(void);
extern s32 func_80045288(void);
extern s32 func_800452C8(void);
extern s32 func_8008CCC0(u8 *arg0);
extern void func_8008CD30(u8 *arg0, u8 *arg1, s32 arg2);

void func_80025F58(void)
{
    s32 value;
    u8 *ptr;

    value = (func_8004522C() + 1) * 100 / 128;
    ptr = D_8009F2CC->unk04;
    func_8008CD30(ptr + func_8008CCC0(ptr) - 3, D_8009F2EC, value);

    value = (func_80045288() + 1) * 100 / 128;
    ptr = D_8009F2CC->unk08;
    func_8008CD30(ptr + func_8008CCC0(ptr) - 3, D_8009F2EC, value);

    value = (func_800452C8() + 1) * 100 / 128;
    ptr = D_8009F2CC->unk0C;
    func_8008CD30(ptr + func_8008CCC0(ptr) - 3, D_8009F2EC, value);

    value = func_800451E0();
    D_8009ABE8[4] = value ? 4 : 5;
}
