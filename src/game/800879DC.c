#include "game/types.h"

extern u32 D_8009F4DC;
extern u8 D_8009F4E0;
extern u32 D_8009F958;
extern u32 D_8009F95C;

extern u32 func_8001EF8C(s32 arg0);
extern u32 *func_80089404(u32 *arg0);
extern void func_80086DB4(void);
extern void func_80086F1C(void);
extern u32 func_80086F44(void);
extern u32 func_80086F58(u32 arg0);
extern void func_8008589C(u8 *arg0, u8 *arg1, u8 *arg2);
extern void func_80091270(s32 arg0);
extern void func_80085914(void);
extern void func_80086FE4(u8 *arg0);
extern void func_80087430(u8 *arg0);
extern void func_80089A40(u8 *arg0);
extern void func_80089304(u32 arg0);

s32 func_800879DC(u8 *arg0, u8 *arg1, s32 arg2)
{
    u32 slot;
    u32 state0;
    u32 state1;
    u32 tag;
    u32 size;
    u32 limit;
    u32 count;
    u32 index;
    u8 *data;
    s32 ok;

    slot = arg2;
    func_80089404(&slot);

    count = 0;
    ok = 1;
    index = 0x80000;

    for (;;) {
        state0 = func_8001EF8C(0) & 0x40080000;
        state1 = func_8001EF8C(1) & 0x40080000;

        if ((state0 & ~D_8009F958) || (state1 & ~D_8009F95C)) {
            if (count >= 4) {
                if (((state0 & ~D_8009F958) & index) ||
                    ((state1 & ~D_8009F95C) & index)) {
                    D_8009F4E0 = 1;
                }
                ok = 0;
                goto ret;
            }
        }

        D_8009F958 = state0;
        D_8009F95C = state1;

        slot = 8;
        data = (u8 *)func_80089404(&slot);
        if (data == 0) {
            goto ret;
        }
        if (slot == 0) {
            goto ret;
        }

        tag = ((*(u32 *)data >> 24) | ((*(u32 *)data >> 8) & 0xFF00) |
               ((*(u32 *)data & 0xFF00) << 8) | (*(u32 *)data << 24));
        size = ((*(u32 *)(data + 4) >> 24) | ((*(u32 *)(data + 4) >> 8) & 0xFF00) |
                ((*(u32 *)(data + 4) & 0xFF00) << 8) | (*(u32 *)(data + 4) << 24));

        if (D_8009F4DC & 1) {
            if (size >= 0x424C) {
                slot = 4;
                goto common;
            }
        } else {
            if (size >= 0x4400) {
                goto err4;
            }
        }

        slot = size;
        data = (u8 *)func_80089404(&slot);
        if (data == 0) {
            goto ret;
        }
        if (slot == 0) {
            goto ret;
        }

        switch (tag) {
        case 0x564C4330:
            func_80089A40(data + 8);
            break;

        case 0x41643130:
        case 0x41643131:
        case 0x61643130:
        case 0x61643131:
        case 0x61643230:
        case 0x61643231:
            func_80086FE4(data);
            break;

        case 0x61753030:
        case 0x61753031:
            func_80087430(data);
            break;

        case 0x4D444332:
        case 0x4D444543:
        {
            register u32 idx __asm__("$16");
            idx = count;

            if (idx == 1) {
                func_80086DB4();
            }
            func_80086F1C();
            count = idx + 1;
            limit = func_80086F58(idx);
            func_8008589C(data, arg0, arg1);
            while (func_80086F44() < limit) {
                func_80091270(0);
            }
            func_80085914();
        }
        break;

        default:
        err4:
            slot = 4;
            break;
        }

    common:

        func_80089304(slot);
    }

ret:
    return ok;
}
