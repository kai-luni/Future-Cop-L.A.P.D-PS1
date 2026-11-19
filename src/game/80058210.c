// Categories: Gameplay > Damage | Gameplay > Health | Gameplay > Player
#include "game/types.h"

struct Unk58210U {
    u8 unk00[0x1C];
    u16 unk1C;
};

struct Unk58210Sub {
    u8 unk00[0x1C];
    u16 unk1C;
    u8 unk1E[0x84];
    s16 unkA2;
    u8 unkA4[0x1D];
    s8 unkC1;
    u8 unkC2[0x264];
    s16 unk326;
};

struct Unk58210 {
    u8 unk00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 unk20;
    u8 unk21[3];
    u32 unk24;
    u8 unk28[0x84];
    struct Unk58210Sub *unkAC;
    u8 unkB0[0x2C];
    struct Unk58210Sub *unkDC;
};

struct Unk58210Global {
    u8 unk00[0x5C];
    s32 unk5C;
};

extern u8 gPlayerInvulnerable[] __asm__("D_8009FA8C");
extern struct Unk58210Global *gGameSession __asm__("D_8009FA94");

extern void func_80057358(void *object, s32 amount);
extern void gFlagQuartileChange(void *object, s32 value) __asm__("func_8003BA50");

/**
 * @brief Deducts player health, charges gauge unkA2/charge, and flags quartile drops.
 *
 * Guards against non-positive damage, non-zero D_8009FA8C[unkAC->unkC1], or active
 * invulnerability timer unkAC->unk326 > 0, then subtracts amount from object->unk1C
 * and unkDC->unk1C, scales unkAC->unkA2 and info->charge, and calls func_8003BA50.
 *
 * @param object Player entity block (+0x1C/+0x1E health/max, +0x20/+0x24 flags, +0xAC/+0xDC sub-blocks).
 * @param amount Damage amount subtracted from current health (<= 0 ignored).
 *
 * @par Globals
 * D_8009FA8C - per-player gate byte table indexed by unkAC->unkC1; D_8009FA94 - game state (+0x5C halves charge gain).
 *
 * @par Control flow
 * Three early returns (amount <= 0, D_8009FA8C[unkC1] != 0, unk326 > 0), then clamps unkA2 to 0x1C2 and charge gain to 0x200 before calling func_80057358 and conditionally func_8003BA50.
 *
 * @par Calls
 * func_80057358 - adds max/4 (clamped to 0xFF) into unkAC->charge; func_8003BA50 - sets flag 0x4000 if health crossed a quartile.
 *
 * @par Side effects
 * Updates object->unk1C, unkDC->unk1C, unkAC->unkA2, charge (+0xD0), and potentially object->flags.
 *
 * @par Callers
 * Confirmed: func_80058378 (0x80058398 primary target, 0x80058484 co-op mirror across D_8009FA58).
 */
void func_80058210(struct Unk58210 *object, s32 amount)
{
    u8 *table;
    u8 *entry;
    u32 index;
    u16 health;
    s32 oldHealth;
    s32 value;
    s32 max;

    health = ((struct Unk58210U *)object)->unk1C;
    __asm__("" ::: "memory");
    oldHealth = object->unk1C;

    if (amount <= 0) {
        return;
    }

    __asm__("lui\t%0,%%hi(D_8009FA8C)\n\taddiu\t%0,%0,%%lo(D_8009FA8C)"
            : "=r"(table));
    index = object->unkAC->unkC1;
    entry = (u8 *)(index + (u32)table);
    if (*entry != 0) {
        return;
    }

    if (object->unkAC->unk326 > 0) {
        return;
    }

    object->unk1C = health - amount;
    object->unkDC->unk1C = object->unk1C;

    value = object->unkAC->unkA2 + ((amount * 5) << 10) / object->unk1E;

    if (value >= 0) {
        if (value < 0x1C3) {
            object->unkAC->unkA2 = value;
        } else {
            object->unkAC->unkA2 = 0x1C2;
        }
    } else {
        object->unkAC->unkA2 = 0;
    }

    if (value >= 0) {
        if (value < 0x201) {
            max = value;
        } else {
            max = 0x200;
        }
    } else {
        max = 0;
    }

    if (gGameSession->unk5C != 0) {
        max /= 2;
    }

    func_80057358(object, max);

    if (object->unk20 != 0 && (object->unk24 & 0x40000) == 0) {
        gFlagQuartileChange(object, oldHealth);
    }
}
