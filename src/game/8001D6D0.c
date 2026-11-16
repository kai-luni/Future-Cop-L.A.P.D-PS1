// Categories: Resources > File Access | Platform > CD-ROM
#include "game/types.h"

struct UnkD6D0 {
    s32 f00;
    s32 f04;
    s32 f08;
    s32 f0C;
    s32 flags;
    s32 f14;
};

extern volatile struct UnkD6D0 D_800A6D78[];

extern s32 func_8001C478(void);
extern s32 func_8001D0A0(s32 index);

/**
 * @brief Seeks or queries the byte position of an open file slot in D_800A6D78.
 *
 * Validates slot arg0 < 4 and open state f00 != 0, waits while f00 & 0xFF is 2 or 3, waits for async loader D_80010000 via func_8001C478 if (flags & 0xF) == 1, applies whence (arg2 & 7: 0=SET, 1=CUR, 2=END, 3=tell f0C-f04), clamps relative offset to [0, f08], optionally calls func_8001D0A0 if arg2 & 8, and updates f00 (setting 0x100 at EOF). Category Resources > File Access | Platform > CD-ROM follows D_800A6D78 file descriptors and CD seek dispatch.
 *
 * @param arg0 File slot index in D_800A6D78 (0..3).
 * @param arg1 Signed byte offset applied relative to selected origin.
 * @param arg2 Low 3 bits select origin (0/1/2/3); bit 3 (0x8) triggers physical CD seek via func_8001D0A0.
 * @return Relative byte position on success/tell, or negative error code (-9 if closed/invalid, or callee error).
 *
 * @par Globals
 * D_800A6D78 - 4-slot file descriptor table (f00 state, f04 base, f08 size, f0C cursor, flags).
 *
 * @par Control flow
 * Slot validation, busy spin-wait, optional loader wait, 4-way switch on arg2 & 7, clamp to [0, f08], and optional CD seek.
 *
 * @par Calls
 * func_8001C478 - spins until D_80010000 == 0 when (flags & 0xF) == 1; func_8001D0A0 - seeks CD-ROM to f0C when arg2 & 8.
 *
 * @par Side effects
 * Updates D_800A6D78[arg0].f00 and f0C, and may issue a CD-ROM seek command.
 *
 * @par Callers
 * Confirmed: func_80028AB4 (0x80028B64 in asm/us/main.s) / Likely: none / Unknown: none.
 */
s32 func_8001D6D0(s32 arg0, s32 arg1, s32 arg2)
{
    s32 sel;
    s32 a1;
    register s32 a2 __asm__("$6");
    register s32 a0 __asm__("$4");
    register s32 lim __asm__("$3");
    s32 flags;
    register s32 test __asm__("$3");

    a1 = -9;
    if ((u32)arg0 < 4) {
        if (D_800A6D78[arg0].f00 != 0) {
            a1 = D_800A6D78[arg0].f00;
        }
    }
    if (a1 < 0) {
        return a1;
    }

    while ((D_800A6D78[arg0].f00 & 0xFF) == 2 ||
           (D_800A6D78[arg0].f00 & 0xFF) == 3) {
    }

    flags = D_800A6D78[arg0].flags;
    D_800A6D78[arg0].f00 = 2;

    test = flags & 0xF;
    __asm__("" : "+r"(test) : "r"(flags));
    if (test == 1) {
        a1 = func_8001C478();
        if (a1 < 0) {
            return a1;
        }
    }

    sel = arg2 & 7;
    a1 = D_800A6D78[arg0].f04;
    a2 = D_800A6D78[arg0].f0C;

    switch (sel) {
    case 3:
        return a2 - a1;
    default:
    case 0:
        a0 = a1 + arg1;
        break;
    case 1:
        a0 = a2 + arg1;
        break;
    case 2: {
        register s32 tmp __asm__("$2");
        tmp = a1 + D_800A6D78[arg0].f08;
        a0 = tmp + arg1;
        break;
    }
    }

    lim = D_800A6D78[arg0].f08;
    arg1 = a0 - a1;
    if (arg1 > lim || arg1 < 0) {
        if (arg1 < 0) {
            arg1 = 0;
        } else {
            arg1 = lim;
        }
        a0 = arg1 + a1;
    }
    D_800A6D78[arg0].f0C = a0;

    if (arg2 & 8) {
        s32 ret = func_8001D0A0(arg0);
        if (ret < 0) {
            return ret;
        }
    }

    {
        register s32 lim2 __asm__("$2");
        lim2 = D_800A6D78[arg0].f08;
        D_800A6D78[arg0].f00 = 1;
        if (arg1 >= lim2) {
            D_800A6D78[arg0].f00 |= 0x100;
        }
    }
    return arg1;
}
