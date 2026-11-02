#include "game/types.h"

extern u8 D_8009F234;
extern s32 D_8009F22C;
extern s32 D_8009F230;
extern void *D_8009F240;

extern void D_8001CDD0(void);
extern void D_8001CDF0(void);
extern u8 D_8009AABC[];
extern s32 D_800A6D78[];
extern s32 D_800A8E68[];
extern u32 jtbl_80010AA4[];
extern s32 func_8001D130(s32 arg0);

s32 func_8001D248(s32 arg0, s32 arg1, u8 *arg2)
{
    register s32 count __asm__("$18");
    u8 *arg2_reg;
    register s32 i __asm__("$16");
    register s32 *entry __asm__("$17");
    register void **table __asm__("$19");
    register s32 idx __asm__("$3");
    register s32 v0 __asm__("$2");
    s32 *clear;
    static void *const label_table[] __attribute__((used)) = {
        &&case_zero, &&case_three, &&case_one, &&default_case
    };

    arg2_reg = arg2;
    count = 0;

    if (arg0 != 0) {
        if (arg1 != 0) {
            goto have_args;
        }
        return -0x11;
    }
    if (arg1 != 0) {
        return -0x11;
    }
    D_8009F230 = (s32)&D_8001CDD0;
    D_8009F22C = (s32)&D_8001CDF0;
    goto cleared;
have_args:
    D_8009F230 = arg0;
    D_8009F22C = arg1;
cleared:

    i = 0;
    clear = (s32 *)D_800A6D78;
    for (; i < 4; i++) {
        *clear = 0;
        clear = (s32 *)((u8 *)clear + 0x18);
    }

    i = 1;
    v0 = 0x80010000;
    __asm__("" : "+r"(v0));
    table = (void **)(v0 + 0xAA4);
    v0 = (s32)D_800A8E68;
    entry = (s32 *)(v0 + 4);

loop:
    idx = func_8001D130(i) + 0x10;
    if ((u32)idx >= 0x11) {
        goto default_case;
    }
    v0 = idx;
    v0 = v0 << 2;
    v0 = v0 + (s32)table;
    v0 = *(s32 *)v0;
    goto *(void *)v0;

case_zero:
    *entry = 0;
    count++;
    goto next;

case_three:
    *entry = 3;
    count++;
    goto next;

case_one:
    v0 = 1;
    goto store;

default_case:
    v0 = 2;

store:
    *entry = v0;

next:
    i++;
    entry++;
    if (i < 4) {
        goto loop;
    }

    if (count == 0) {
        return -7;
    }
    D_8009F234 = 1;
    if (arg2_reg == 0) {
        D_8009F240 = D_8009AABC;
        return 0;
    }
    D_8009F240 = arg2_reg;
    return 0;
}
