#include "game/types.h"

extern u8 D_8009F4E0;
extern u8 D_8009F504;
extern s32 D_8009F4DC;
extern s32 D_8009F4E4;
extern s32 D_8009F4E8;
extern s32 D_8009F4EC;
extern s32 D_8009F4F0;
extern s32 D_8009F4F4;
extern s32 D_8009F4F8;
extern s32 D_8009F4FC;
extern s32 D_8009F500;
extern s32 D_8009F958;
extern s32 D_8009F95C;
extern s32 D_8009F960;
extern s32 D_8009F964;
extern s32 D_8009F968;
extern s32 D_8009FA48[];

extern s32 func_8001EF8C(s32);
extern void func_800859B0(void);
extern void func_800859D8(s32);
extern s32 func_80085A1C(void);
extern s32 func_80085C4C(s32, s32, s32, s32, s32, s32, s32);
extern s32 func_80087814(void);
extern s32 func_8008781C(void);
extern s32 func_8008799C(s32);
extern void func_80087D48(void);
extern s32 func_800896D4(s32, s32, s32);
extern void func_80089A88(s32, s32, s32);
extern void func_8008F7AC(void);
extern s32 func_80091F60(s32);
extern void func_80092BF4(s32);
extern s32 func_80097B28(s32);

s32 func_80087EA0(s32 arg0)
{
    register s32 var_s0 __asm__("$16");
    register s32 var_v0 __asm__("$2");
    s32 ptr;
    s32 var_a0;
    s32 var_a3;

    if (D_8009F504 != 0) {
        return 1;
    }

    D_8009F4E0 = 0;
    var_a0 = func_8001EF8C(0);
    var_s0 = 0x40080000;
    D_8009F958 = var_a0 & var_s0;
    var_v0 = func_8001EF8C(1) & var_s0;
    D_8009F95C = var_v0;
    var_v0 = 0x400;
    D_8009F960 = var_v0;
    var_v0 = 0x800;
    D_8009F964 = var_v0;
    var_v0 = arg0 & 0x20;
    var_s0 = var_v0 < 1;

    if (!(arg0 & 1)) {
        D_8009F968 = 0xC8;
    } else {
        D_8009F968 = 0x27;
    }

    if (var_s0 != 0) {
        func_80092BF4(0);
        func_80091F60(0);
    }

    func_8008F7AC();

    if (arg0 & 1) {
        var_a0 = 0xF6C0;
        arg0 |= 0x10;
    } else {
        arg0 |= 0x4;
        var_a0 = 0x22D00;
    }

    D_8009F4DC = arg0;
    D_8009F4E4 = func_8008799C(var_a0);
    if (D_8009F4E4 == 0) {
        goto fail;
    }

    D_8009F4FC = func_8008799C(0x6000);
    if (D_8009F4FC == 0) {
        goto fail;
    }

    D_8009F500 = func_8008799C(0x600);
    if (D_8009F500 == 0) {
        goto fail;
    }

    func_80089A88(D_8009F4FC, D_8009F500, 0);

    if (arg0 & 8) {
        func_800859D8(0);
    } else if (arg0 & 0x10) {
        func_800859D8(1);
    } else if (arg0 & 4) {
        func_800859B0();
    }

    var_a0 = func_8008799C(func_80085A1C());
    D_8009F4E8 = var_a0;
    if (var_a0 == 0) {
        goto fail;
    }

    var_a3 = 0x10000;
    if (D_8009F4DC & 1) {
        var_a3 = 0x6DE0;
    }

    ptr = D_8009F4E4;
    if (func_80085C4C(var_a0, ptr, ptr + var_a3,
                       ptr + var_a3 * 2, 1, var_s0, 0) == 0) {
        goto fail;
    }

    if (!(arg0 & 1)) {
        D_8009F4EC = func_8008799C(func_800896D4(D_8009F968, 0x4400, 1));
        if (D_8009F4EC == 0) {
            goto fail;
        }
    } else {
        D_8009F4EC = D_8009FA48[0];
        func_800896D4(D_8009F968, 0x424C, 1);
    }

    D_8009F4F0 = func_8008799C(func_8008781C());
    if (D_8009F4F0 == 0) {
        goto fail;
    }

    D_8009F4F4 = func_80097B28(func_80087814());
    if (D_8009F4F4 == 0) {
        goto fail;
    }

    D_8009F4F8 = func_80097B28(func_80087814());
    if (D_8009F4F8 == 0) {
        goto fail;
    }

    D_8009F504 = 1;
    return 1;

fail:
    func_80087D48();
    return 0;
}
