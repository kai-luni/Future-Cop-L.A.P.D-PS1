#include "game/types.h"

extern s32 D_8009F310;
extern void *jtbl_800115B8[];

extern void func_8003BC7C(void *data);
extern void func_80040DCC(void *data);
extern void func_80044770(void *data);
extern void func_8004C884(void *data);
extern void func_8006EAF0(void *data);
extern void func_800703A8(void *data);
extern void func_80073F88(void *data);
extern void func_800767E4(void *data);

s32 func_8003BFD0(s32 arg0, s32 arg1)
{
    register s32 *ptr __asm__("$16");
    register s32 remaining __asm__("$17");
    register s32 size __asm__("$18");
    register s32 old __asm__("$19");
    void **table;
    static void *const label_table[] __attribute__((used)) = {
        &&case_56, &&case_57, &&case_58, &&case_59, &&case_5A,
        &&case_5B, &&case_5C, &&case_5D, &&case_5E
    };
    register u32 type __asm__("$2");
    register u32 dispatch_index __asm__("$3");

    ptr = (s32 *)arg0;
    remaining = arg1;
    old = D_8009F310;
    table = (void **)jtbl_800115B8;
    D_8009F310 = 0;

loop:
    ptr++;
    size = *ptr;
    ptr++;
    type = ((u8 *)ptr)[4];
    dispatch_index = type - 0x56;
    if (dispatch_index < 9) {
        type = dispatch_index << 2;
        type += (u32)table;
        type = *(u32 *)type;
        goto *(void *)type;
    }
    goto default_case;

case_56:
    D_8009F310 = ptr[5];
    goto done;
case_57:
    func_80044770(ptr);
    goto done;
case_58:
    func_80040DCC(ptr);
    goto done;
case_59:
    func_8006EAF0(ptr);
    goto done;
case_5A:
    func_8004C884(ptr);
    goto done;
case_5B:
    func_8003BC7C(ptr);
    goto done;
case_5C:
    func_800703A8(ptr);
    goto done;
case_5D:
    func_80073F88(ptr);
    goto done;
case_5E:
    func_800767E4(ptr);
    goto done;

default_case:
    return 0;

done:
    ptr = (s32 *)((u8 *)ptr + (size - 8));
    remaining -= size;
    if (remaining > 0) {
        goto loop;
    }
    D_8009F310 = old;
    return 1;
}
