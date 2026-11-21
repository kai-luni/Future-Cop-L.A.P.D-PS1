// Categories: Gameplay > Weapon Loadout | Game Engine > Difficulty
#include "game/types.h"

struct Unk6BDFCCtl {
    u8 unk00[0x78];
    s32 unk78[3];
    s16 unk84[3];
    s16 unk8A[3];
    u8 unk90[3];
    u8 unk93[3];
};

struct Unk6BDFCObj {
    u8 unk00[0xAC];
    struct Unk6BDFCCtl *ctl;
};

struct Unk6BDFCTgt {
    u8 unk00[0x2A];
    u8 unk2A;
    u8 unk2B[0x19];
    s32 unk44;
    u8 unk48[8];
    s32 unk50;
};

extern struct Unk6BDFCTgt *gRegistryFind(u8 id) __asm__("func_8006AAEC");
extern s32 gGetScaledWeaponReserveLimit(struct Unk6BDFCObj *obj, struct Unk6BDFCTgt *tgt, s32 flag) __asm__("func_8006BD48");

#define U8(off)  (*(u8 *)((u32)ctl + idx + (off)))
#define S16(off) (*(s16 *)((u32)ctl + (idx << 1) + (off)))

/**
 * @brief Refreshes one weapon slot's counters and swaps its two range values.
 *
 * The record for slot idx comes from gRegistryFind with the slot's id byte at
 * 0x90. While the slot's secondary counter 0x8A is not positive the record's
 * ammo byte 0x2A is copied into 0x93 and its two range words 0x44/0x50 are
 * swapped, so the reload alternates between the two bands. The counter is then
 * recomputed by func_8006BD48 with flag 1.
 *
 * @param obj Object whose control block (0xAC) holds the three slots.
 * @param idx Slot index 0..2.
 *
 * @par Globals
 * None directly; the slot arrays live in the control block.
 *
 * @par Control flow
 * One gate on the counter, then an unconditional recompute.
 *
 * @par Calls
 * gRegistryFind - weapon record lookup. func_8006BD48 - counter/ammo update.
 *
 * @par Side effects
 * Writes the record's ammo byte and the two range words, and the slot counter.
 *
 * @par Callers
 * Confirmed: func_8006AB38 (loadout reset) and the weapon update loop.
 */
void func_8006BDFC(struct Unk6BDFCObj *obj, s32 idx)
{
    struct Unk6BDFCCtl *ctl = obj->ctl;
    struct Unk6BDFCTgt *tgt = gRegistryFind(U8(0x90));
    s16 val = S16(0x8A);
    s32 t;
    s32 t2;

    if (val <= 0) {
        U8(0x93) = tgt->unk2A;
        t = tgt->unk50;
        t2 = tgt->unk44;
        tgt->unk44 = t;
        tgt->unk50 = t2;
    }
    S16(0x8A) = gGetScaledWeaponReserveLimit(obj, tgt, 1);
}
