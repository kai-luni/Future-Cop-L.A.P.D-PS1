#include "game/types.h"

extern void *D_800A1070[];
extern void func_8007364C(s32 arg);
extern void func_8006F170(void *arg);
extern void func_8006D770(void *a0, void *a1);
extern void func_800441DC(void *a0, void *a1);
extern void func_80040A00(void *a0, void *a1);
extern void func_80070048(void *a0, void *a1);
extern void func_8004C228(void *a0, void *a1);
extern void func_80075A84(void *a0, void *a1);

void func_8003F4A0(void *arg)
{
    func_8007364C((s32)arg);
    func_8006F170(arg);
    func_8006D770(D_800A1070[2], arg);
    func_800441DC(D_800A1070[3], arg);
    func_80040A00(D_800A1070[0], arg);
    func_80070048(D_800A1070[1], arg);
    func_8004C228(D_800A1070[4], arg);
    func_80075A84(D_800A1070[5], arg);
}
