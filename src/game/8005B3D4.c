// Categories: Graphics > Effects > Beams | Gameplay > Damage
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk22C0Sub {
    u8 pad00[4];
    void (*unk04)(s32, void *, void *, s32);
    s32 unk08;
};

struct Unk22C0 {
    struct Unk22C0Sub *unk00;
    struct Unk22C0Sub *unk04;
    s32 unk08;
    s32 unk0C;
    struct Vec3s32 unk10;
    struct Vec3s32 unk1C;
    s16 unk28;
    u16 unk2A;
    s16 unk2C;
};

struct Local22C0 {
    u32 unk00;
    u16 unk04;
    u8 unk06;
    u8 pad07;
    void *unk08;
};

struct Unk73500;

extern struct Unk22C0 gLockEndpoints __asm__("D_800A22C0");

extern struct Unk73500 *func_80073500(struct Vec3s32 *position, s16 arg1, s16 arg2, s16 arg3);
extern void func_8007377C(void *arg0, void *arg1, s32 arg2, s32 arg3,
                          s32 arg4, s32 arg5, s32 arg6, s32 arg7,
                          s32 arg8, s32 arg9, s32 arg10, s32 arg11,
                          s32 arg12, s32 arg13);
extern s32 func_8001A4E8(s32 *a, s32 *b);

/**
 * @brief Ticks the locked-target beam: validates both endpoints, drains the
 *        timer, fires the beam and delivers the hit message in range.
 *
 * @param arg0 Frame delta subtracted from the timer.
 * @return Nothing.
 *
 * @par Confirmed
 * The global lock D_800A22C0 holds two endpoint pointers (+0/+4) with their
 * ids (+8/+0xC); a mismatched id drops the endpoint. The timer +0x28 must
 * be non-negative at entry and drains by arg0; only when it crosses below
 * zero and both endpoints survive does the beam fire: func_8007377C links
 * two func_80073500 nodes (positions +0x1C and +0x10, args 0, 0x100, 0)
 * with parameters (4, 1, 0xFF, 0x60, 0, 0x800, 0, 0, 0, 0, 0, 1). Then
 * func_8001A4E8 compares the endpoint distance against (unk2C +
 * target[0x6E]) * 16; in range, a local record {8, unk2A, target[0x14],
 * endpoint} is delivered as message 0xE through the first endpoint's
 * callback.
 *
 * @par Likely role
 * A periodic beam weapon between two locked objects (tractor/attack beam):
 * the timer sets the fire rate, the distance gate the effective range, and
 * message 0xE is the shared hit notification.
 *
 * @par Unknown
 * Who arms D_800A22C0 and the unit of the 0x800 beam parameter.
 *
 * @par Callers
 * func_8005E0AC.
 */
void func_8005B3D4(void *arg0)
{
    struct Local22C0 local;

    if (gLockEndpoints.unk28 < 0) {
        return;
    }

    if (gLockEndpoints.unk00 != 0) {
        if (gLockEndpoints.unk08 != gLockEndpoints.unk00->unk08) {
            gLockEndpoints.unk00 = 0;
        }
    }

    if (gLockEndpoints.unk04 != 0) {
        if (gLockEndpoints.unk0C != gLockEndpoints.unk04->unk08) {
            gLockEndpoints.unk04 = 0;
        }
    }

    gLockEndpoints.unk28 -= (s32)arg0;
    if (gLockEndpoints.unk28 >= 0) {
        return;
    }

    if (gLockEndpoints.unk04 == 0) {
        return;
    }
    if (gLockEndpoints.unk00 == 0) {
        return;
    }

    func_8007377C(
        func_80073500(&gLockEndpoints.unk1C, 0, 0x100, 0),
        func_80073500(&gLockEndpoints.unk10, 0, 0x100, 0),
        4, 1, 0xFF, 0x60, 0, 0x800, 0, 0, 0, 0, 0, 1);

    if (func_8001A4E8((s32 *)&gLockEndpoints.unk10,
                      (s32 *)((u8 *)gLockEndpoints.unk00 + 0x5C)) >=
        (gLockEndpoints.unk2C + *(s16 *)((u8 *)gLockEndpoints.unk00 + 0x6E)) * 16) {
        return;
    }

    local.unk06 = 8;
    local.unk04 = gLockEndpoints.unk2A;
    local.unk00 = *(u16 *)((u8 *)gLockEndpoints.unk04 + 0x14);
    local.unk08 = gLockEndpoints.unk04;

    gLockEndpoints.unk00->unk04(0xE, gLockEndpoints.unk00, &local, 0);
}
