#include "game/types.h"

#define B(o,n) (*(u8 *)((u8 *)(o) + (n)))
#define H(o,n) (*(s16 *)((u8 *)(o) + (n)))
#define U(o,n) (*(u16 *)((u8 *)(o) + (n)))
#define W(o,n) (*(s32 *)((u8 *)(o) + (n)))
#define F(o,n) (*(u32 *)((u8 *)(o) + (n)))
#define P(o,n) (*(u8 **)((u8 *)(o) + (n)))

extern void func_8002B4B4(u8 *, s32);
extern s32 func_80044B34(u8 *, void *, s32, s32);
extern u8 func_80060238(void *, s32, u8 *, s32, s32);
extern void func_800448D8(void *, s32, s32, u8);
extern void func_8005F6E8(void *, u8 *, u8 *, s32);
extern void func_8002A810(u8 *, s32, s32);
extern void func_800175F8(void *, s32);
extern void func_8003802C(s32, u8 *, s32);
extern u8 func_8002B1DC(u8 *, s32, u8 *);
extern s32 func_8001A100(s32 *);
extern void func_8002B0EC(u8 *, s32);
extern void func_8002B588(u8 *, s32, s32);
extern void func_80037EB4(u8 *);
extern void func_8002B1D0(u8 *);
extern void func_8002B6C8(u8 *, u8 *, s32);
extern void func_8002B1AC(u8 *);

void func_8002B784(u8 *obj, s32 step)
{
    s32 event;
    u8 *target;
    u8 valid;
    u8 active;
    s32 distance;
    s32 vector[3];
    s32 speed;
    register s32 difference __asm__("$2");
    s32 sign;
    u32 mode;

    func_8002B4B4(obj, step);
    if (B(obj,0x162)) {
        if (B(obj,0x15E) != B(obj,0x164) &&
            B(obj,0x15E) != B(obj,0x164) + 1) {
            B(obj,0x162) = 0;
        }
    }
    if (B(obj,0x15E) == B(obj,0x167)) {
        speed = step * 21;
    } else if (B(obj,0x162) || B(obj,0x15F) == 7 || B(obj,0x15F) == 8) {
        speed = step;
    } else {
        speed = W(obj,0x84) * step;
    }
    event = func_80044B34(obj, obj + 0xC8, speed, 0) >> 1;
    while (event) {
        switch (event) {
        case 1:
            if (func_80060238(obj + 0xA0, step, obj, 0, 0)) {
                B(obj,0x162) = 2;
                func_800448D8(obj + 0xC8, B(obj,0x164) + 1, 1, 1);
            }
            break;
        case 2:
            B(obj,0x162) = 1;
            func_8005F6E8(obj + 0xA0, obj, obj, 1);
            break;
        case 3:
            func_8002A810(obj, 1, 1);
            return;
        }
        event >>= 6;
    }
    func_800175F8(obj + 0x28, H(obj,0xD4));
    if (!B(obj,0x162) && (u32)(B(obj,0x15F) - 7) >= 2) {
        func_80060238(obj + 0xA0, step, obj, 0, 1);
    }
    if ((u32)(B(obj,0x15F) - 7) < 2) {
        if (B(obj,0x15F) == 7) {
            return;
        }
    }
    func_8003802C(6, obj, step);
    if (B(obj,0x15F) != 6 && H(obj,0xC0) == 0) {
        func_8002B1DC(obj, 6, 0);
    }
    target = P(obj,0xB8);
    valid = 0;
    if (target) {
        valid = W(target,8) == W(obj,0xBC);
    }
    if (valid) {
        vector[0] = W(target,0x5C) - W(obj,0x5C);
        vector[1] = W(target,0x60) - W(obj,0x60);
        vector[2] = W(target,0x64) - W(obj,0x64);
        distance = func_8001A100(vector) - (W(target,0x68) + W(obj,0x68));
    }
    if (F(obj,0x118) & 0x40) {
        if (H(obj,0x122) < -1) {
            if (B(obj,0x15F) != 5) {
                func_8002B1DC(obj, 5, target);
            }
            if (!valid) {
                func_8002B0EC(obj, 0);
            } else {
                func_8002B6C8(obj, target, step);
            }
        } else if (H(obj,0x10E) > 0) {
            if (H(obj,0x15C) != H(obj,0x12A)) {
                func_8002B588(obj, H(obj,0x12A), step);
                difference = H(obj,0x15C);
                __asm__ volatile ("" : "+r" (difference));
                difference -= H(obj,0x12A);
                sign = difference >> 31;
                difference = (sign + difference) ^ sign;
                if (difference <= 64) {
                    U(obj,0x15C) = U(obj,0x12A);
                }
            } else {
                F(obj,0x118) &= ~0x40;
                W(obj,0x84) = 0;
                func_80037EB4(obj);
                func_8002B0EC(obj, B(obj,0x165));
            }
        } else if (B(obj,0x15F) != 5) {
            if (!valid || distance > (H(obj,0xAA) << 4)) {
                if (B(obj,0x15F) == 1) {
                    func_8002B1DC(obj, 0, target);
                }
                func_8002B1D0(obj);
            } else if (B(obj,0x162) != 2) {
                func_8002B6C8(obj, target, step);
            }
        }
    } else {
        active = 0;
        if ((B(obj,0x15F) == 1 || B(obj,0x15F) == 4 || B(obj,0x15F) == 0) &&
            valid && distance <= (H(obj,0x15A) << 4) &&
            !(F(obj,0x118) & 0x40) && !(B(obj,0x160) & 0x80)) {
            if (W(obj,0x84) == 0) {
                if (B(obj,0x15F) == 0) {
                    func_8002B1DC(obj, 1, target);
                }
                W(obj,0x84) = 0;
                H(obj,0x10E) = 0;
                F(obj,0x118) |= 0x40;
                U(obj,0x15C) = U(obj,0x12A);
                func_8002B6C8(obj, target, step);
            } else {
                H(obj,0x10E) = 0;
            }
            active = 1;
        }
        if (!active && H(obj,0x10E) == 0) {
            U(obj,0x10E) = U(obj,0x10C);
        }
    }
    if (!(B(obj,0x160) & 2) && H(obj,0x1C) < 5 &&
        B(obj,0x15F) != 7 && B(obj,0x15F) != 4 && B(obj,0x15F) != 6) {
        if (!func_8002B1DC(obj, 6, 0)) {
            mode = (F(obj,0x118) >> 24) & 15;
            if (mode >= 1 && mode <= 6) {
                F(obj,0x118) = (F(obj,0x118) & 0xF0FFFFFF) | ((mode + 8) << 24);
            }
            B(obj,0x15F) = 2;
            func_8002B1AC(obj);
        }
    }
    switch (B(obj,0x15F)) {
    case 0:
        if (valid) {
            func_8002B1DC(obj, 1, target);
        }
        if (H(obj,0x10E) == 0) {
            U(obj,0x10E) = U(obj,0x10C);
        }
        break;
    case 1:
        if (!valid) {
            func_8002B1DC(obj, 0, 0);
        }
        break;
    case 2:
        if (!P(obj,0x138)) {
            func_8002B1DC(obj, 0, 0);
        } else if (W(obj,0x84) < H(obj,0x158)) {
            func_8002B1AC(obj);
        }
        break;
    case 3:
        if ((F(obj,0x118) >> 24) & 15) {
            F(obj,0x118) &= 0xF0FFFFFF;
        }
        break;
    case 4:
        break;
    case 5:
        if (valid) {
            func_8002B1DC(obj, 1, target);
        } else if (!(F(obj,0x118) & 0x40)) {
            if (W(obj,0x84) == 0) {
                F(obj,0x118) |= 0x40;
            }
            if (H(obj,0x10E)) {
                H(obj,0x10E) = 0;
            }
        }
        break;
    case 6:
        if (H(obj,0x124) == H(obj,0x11E)) {
            if (H(obj,0xC0) >= 0) {
                U(obj,0xC0) = U(P(obj,0xA0),4);
            }
            U(obj,0x1C) = U(obj,0x1E);
            func_8002B1DC(obj, 0, 0);
        }
        break;
    }
    if (F(obj,0x118) & 0x40) {
        if (B(obj,0x15E) != B(obj,0x167)) {
            if (valid) {
                if (B(obj,0x15E) != B(obj,0x164)) {
                    func_8002B0EC(obj, B(obj,0x164));
                }
            } else if (B(obj,0x15E) != 0) {
                func_8002B0EC(obj, 0);
            }
        }
    } else {
        if (!(B(obj,0x160) & 1) && W(obj,0x84) <= (H(obj,0x158) >> 2) + 2) {
            if (B(obj,0x15E) != B(obj,0x165)) {
                func_8002B0EC(obj, B(obj,0x165));
            }
        } else if (B(obj,0x15E) != B(obj,0x166)) {
            func_8002B0EC(obj, B(obj,0x166));
        }
    }
}
