#include "game/types.h"

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect88120;

typedef struct {
    s32 data[6];
} Params88120;

extern u8 D_8009F504;
extern u32 D_8009F4DC;
extern void *D_8009F4EC;
extern void *D_8009F4F0;
extern void *D_8009F4F4;
extern void *D_8009F4F8;
extern s32 D_8009F960;
extern s32 D_8009F964;
extern s32 D_8009F968;
extern s32 D_8009F238;
extern Params88120 D_800A6E20;

extern s32 func_8001D5E4(s32 arg0, s32 arg1);
extern void func_8001D3A8(s32 handle);
extern void func_80092BF4(s32 arg0);
extern u8 func_8001C034(void);
extern void *func_8008799C(s32 size);
extern void func_800879BC(void *ptr);
extern void func_80092C8C(s32 arg0);
extern void func_80092F98(Rect88120 *rect, void *buf);
extern void func_80092F38(Rect88120 *rect, void *buf);
extern void func_80092FF8(Rect88120 *rect, s32 x, s32 y);
extern void func_800858F4(s32 value);
extern void func_80085900(s32 value);
extern void func_80091270(s32 arg0);
extern void func_80085914(void);
extern void func_80085AC4(u32 arg0, u32 arg1);
extern void func_80086F30(void);
extern s32 func_80089700(void *arg0, void (*callback)(void), Params88120 *params, s32 arg3, s32 arg4);
extern s32 func_80087824(void *arg0, s32 arg1, void *arg2, s32 arg3, void *arg4, s32 arg5, s32 arg6);
extern void func_80086F08(u32 value);
extern s32 func_800879DC(s32 arg0, s32 arg1, s32 arg2);
extern s32 func_80086FA8(void);
extern void func_80087750(void);
extern void func_80089874(void);
extern s32 func_800858E0(void);
extern void func_8001C088(u8 mode, u8 enable);
extern s32 func_8001BFE4(void);

u8 func_80088120(s32 arg0, s32 arg1, s32 arg2)
{
    Params88120 params;
    Rect88120 rectA;
    Rect88120 rectB;
    Rect88120 rectC;
    Rect88120 rectD;
    s32 handle;
    s32 result;
    s32 mode;
    s32 masked;
    s32 page;
    u16 *src16;
    u8 *dst8;
    u8 *src8;
    u16 *dst16;
    u16 *buf16;
    u8 *buf8;
    s32 row;
    u8 *in8;
    u16 *out16;
    s32 line;
    s32 col;
    s32 above;
    s32 inside;
    s32 x;
    s32 count;
    u32 c;
    u32 r;
    u32 g;
    u32 b;

    result = 0;
    if (D_8009F504 == 0) {
        return 0;
    }
    handle = func_8001D5E4(arg0, D_8009F238);
    if (handle < 0) {
        func_80092BF4(1);
        return 0;
    }
    params = D_800A6E20;

    if ((D_8009F4DC & 0x24) == 0x24) {
        rectA.x = 0;
        rectA.w = 0x140;
        rectA.h = 1;
        rectB.x = 0;
        rectB.w = 0x1E0;
        rectB.h = 1;
        masked = D_8009F4DC & 2;
        page = func_8001C034();
        if (page == 0) {
            rectA.y = 0x100;
            rectB.y = 0;
        } else {
            rectA.y = 0;
            rectB.y = 0x100;
        }
        buf16 = func_8008799C(0x280);
        if (buf16 == 0) {
            return 0;
        }
        buf8 = func_8008799C(0x3C0);
        if (buf8 == 0) {
            func_800879BC(buf16);
            return 0;
        }
        for (row = 0; row < 0x100; row++) {
            func_80092C8C(0);
            func_80092F98(&rectA, buf16);
            func_80092C8C(0);
            x = 0;
            above = row < 0x31;
            inside = row < 0xC1;
            src16 = buf16;
            dst8 = buf8;
            for (; x < 0x140; x++) {
                if (masked != 0 && (u32)(x - 0x20) < 0x100 && !above && inside) {
                    *dst8++ = 0;
                    *dst8++ = 0;
                    *dst8++ = 0;
                } else {
                    c = *src16;
                    *dst8++ = ((c & 0x1F) << 3) | ((c & 0x1F) >> 2);
                    *dst8++ = (((c >> 5) & 0x1F) << 3) | (((c >> 5) & 0x1F) >> 2);
                    *dst8++ = (((c >> 10) & 0x1F) << 3) | (((c >> 10) & 0x1F) >> 2);
                }
                src16++;
            }
            func_80092F38(&rectB, buf8);
            rectA.y++;
            rectB.y++;
        }
        func_80092C8C(0);
        func_800879BC(buf8);
        func_800879BC(buf16);
        func_800858F4(page);
        func_80085900(1);
        func_80091270(0);
        func_80085914();
        rectA.x = 0;
        rectA.w = 0x1E0;
        rectA.h = 0x100;
        if (page == 0) {
            rectA.y = 0;
            rectB.y = 0x100;
        } else {
            rectA.y = 0x100;
            rectB.y = 0;
        }
        func_80092C8C(0);
        func_80092FF8(&rectA, 0, rectB.y);
        func_80092C8C(0);
    }

    func_80085AC4(0, 1);
    if (func_80089700(D_8009F4EC, func_80086F30, &params, 1, 0x800) != 0) {
        if (func_80087824(D_8009F4F0, 0x400000, D_8009F4F4, 0x800000, D_8009F4F8, D_8009F964, 1) != 0) {
            func_80086F08(0);
            result = func_800879DC(arg1, arg2, D_8009F968 * D_8009F960);
            count = 3;
            while (func_80086FA8() != 0 && count > 0) {
                func_80091270(0);
                count--;
            }
            func_80087750();
        }
        func_80089874();
    }
    func_8001D3A8(handle);

    if ((D_8009F4DC & 0x24) == 0x24) {
        rectC.x = 0;
        rectC.w = 0x1E0;
        rectC.h = 1;
        rectD.x = 0;
        rectD.w = 0x140;
        rectD.h = 1;
        mode = func_800858E0();
        if (mode == 0) {
            rectC.y = 0x100;
            rectD.y = 0;
        } else {
            rectC.y = 0;
            rectD.y = 0x100;
        }
        in8 = func_8008799C(0x3C0);
        if (in8 == 0) {
            return 0;
        }
        out16 = func_8008799C(0x280);
        if (out16 == 0) {
            func_800879BC(in8);
            return 0;
        }
        for (line = 0; line < 0x100; line++) {
            func_80092C8C(0);
            func_80092F98(&rectC, in8);
            func_80092C8C(0);
            col = 0;
            dst16 = out16;
            src8 = in8;
            for (; col < 0x140; col++) {
                r = *src8++ >> 3;
                g = *src8++ >> 3;
                b = *src8++ >> 3;
                *dst16++ = (b << 10) | (g << 5) | r;
            }
            func_80092F38(&rectD, out16);
            rectC.y++;
            rectD.y++;
        }
        func_80092C8C(0);
        func_800879BC(out16);
        func_800879BC(in8);
        func_8001C088(mode, 0);
        func_80091270(0);
        func_8001BFE4();
        rectC.x = 0;
        rectC.w = 0x140;
        rectC.h = 0x100;
        if (mode == 0) {
            rectC.y = 0;
            rectD.y = 0x100;
        } else {
            rectC.y = 0x100;
            rectD.y = 0;
        }
        func_80092C8C(0);
        func_80092FF8(&rectC, 0, rectD.y);
        func_80092C8C(0);
        func_80091270(0);
        func_8001C088(1 - mode, 0);
        func_80091270(0);
        func_8001BFE4();
    }
    return result;
}
