#include "game/types.h"

struct Sel {
    s8 mode;
    u8 pad1[4];
    s8 state;
};

struct Voice {
    s32 bank;
    s8 volume;
    s8 baseVolume;
    s8 pan;
    s8 extra;
};

extern u8 D_8009F904;
extern struct Sel *D_8009F468;
extern struct Voice *D_8009F91C;
extern s32 D_8009F920;

extern s32 func_8004522C(void);
extern void func_800451F8(s32 volume);
extern s32 func_80045288(void);
extern void func_80045238(s32 volume);
extern s32 func_800452C8(void);
extern void func_80045294(s32 volume);
extern void func_80046A34(s32 handle);
extern s32 func_80046A58(s32 arg0, s32 arg1, s32 arg2, s32 id);
extern s32 func_80046E88(s32 track, s32 arg1, s32 arg2, s32 arg3, s32 handle, void *object);
extern void func_80047800(struct Voice *voice, s32 pan, s32 volume, s32 extra);

s32 func_8007CACC(s32 flags, s32 arg1)
{
    s32 mode;
    register s32 k __asm__("$2");

    mode = 0;
    if (flags & 0x40000000) {
        D_8009F904 = arg1;
    }
    if (D_8009F468->state == 0 || (D_8009F468->state == 1 && arg1 == 0)) {
        if (D_8009F468->mode == 1) {
            mode = 1;
        } else if (D_8009F468->mode == 3) {
            mode = 2;
        }
        if (flags & 0x800000) {
            switch (D_8009F468->mode) {
            case 1:
                func_800451F8(func_8004522C() - 1);
                break;
            case 2:
                func_80045238(func_80045288() - 1);
                break;
            case 3:
                func_80045294(func_800452C8() - 1);
                break;
            }
        } else if (flags & 0x200000) {
            switch (D_8009F468->mode) {
            case 1:
                func_800451F8(func_8004522C() + 1);
                break;
            case 2:
                func_80045238(func_80045288() + 1);
                break;
            case 3:
                func_80045294(func_800452C8() + 1);
                break;
            }
        }
        if (mode == 1) {
            if (D_8009F91C != 0) {
                if (D_8009F920 != 0x4A) {
                    func_80046A34((s32)D_8009F91C);
                    D_8009F91C = 0;
                }
            }
            if (D_8009F91C == 0) {
                D_8009F91C = (struct Voice *)func_80046A58(func_8004522C(), 10, 0, 0x4A);
                if (D_8009F91C == 0) {
                    return flags;
                }
                func_80046E88(0x4A, 0, 0, -1, (s32)D_8009F91C, 0);
                k = 0x4A;
                D_8009F920 = k;
                return flags;
            }
            func_80047800(D_8009F91C, D_8009F91C->pan, func_8004522C(), -1);
        } else if (mode == 2) {
            if (D_8009F91C != 0) {
                if (D_8009F920 != 0x4B) {
                    func_80046A34((s32)D_8009F91C);
                    D_8009F91C = 0;
                }
            }
            if (D_8009F91C == 0) {
                D_8009F91C = (struct Voice *)func_80046A58(func_800452C8(), 10, 0, 0x4B);
                if (D_8009F91C == 0) {
                    return flags;
                }
                func_80046E88(0x4B, 0, 0, -1, (s32)D_8009F91C, 0);
                k = 0x4B;
                D_8009F920 = k;
                return flags;
            }
            func_80047800(D_8009F91C, D_8009F91C->pan, func_800452C8(), -1);
        } else {
            if (D_8009F91C != 0) {
                func_80046A34((s32)D_8009F91C);
                D_8009F91C = 0;
            }
        }
    }
    return flags;
}
