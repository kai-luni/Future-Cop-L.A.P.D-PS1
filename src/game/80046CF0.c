// Categories: Platform > Sound > SE | Gameplay > Weapons
#include "game/types.h"

extern int gBeamPartUpdate();

struct Unk6C488Stats {
    u8 unk00[0xC1];
    s8 unkC1;
};

struct Unk6C488 {
    u8 unk00[0xAC];
    struct Unk6C488Stats *stats;
};

struct UnkHandle {
    u8 unk00[0x10];
    s32 field10;
};

struct UnkA10E0 {
    s32 status;
    s32 unk4;
    s32 bank;
    struct UnkHandle *handle;
    s32 field10;
};

extern u8 gSoundBankTableLow[] __asm__("D_800A1830");
extern struct UnkA10E0 gPlayerSlots[] __asm__("D_800A10E0");

extern s32 gResolveSoundRecordIndex(s32 id, u8 *table, s32 limit) __asm__("func_80045494");
extern s32 gPlaySpatialSoundBankSlot(s32 arg0, s32 arg1, u32 index, s32 arg3, s32 arg4) __asm__("func_80046F9C");
void gRefreshPlayerWeaponSound(struct Unk6C488 *object, s32 state) __asm__("func_80046CF0");


/**
 * @brief Starts or refreshes a looping weapon sound in one of the player's
 *        three voice slots.
 *
 * @param object Entity whose stats block +0xAC provides the player index
 *        byte +0xC1.
 * @param state Sound state id resolved against the table D_800A1830
 *        (limit 0x47).
 * @return Nothing.
 *
 * @par Confirmed
 * Only player indices < 2 act. func_80045494 resolves the bank; 0xFF
 * aborts. The three slots D_800A10E0[player * 3] are scanned: an occupied
 * slot with the same bank and a matching handle field10 is refreshed
 * (status 0x1E, unk4 = 1); a same-bank slot with a stale handle is freed.
 * Otherwise the first free slot gets func_80046F9C(0, 0, bank, object, 0);
 * a non-zero handle stores status 0x1E, unk4 = 1, bank, field10 and the
 * handle pointer.
 *
 * @par Likely role
 * The sustained-fire sound manager of the weapon handlers (gBeamPartUpdate,
 * func_8006C488): the handle identity check restarts the loop only when
 * the voice was recycled, so continuous fire does not retrigger.
 *
 * @par Unknown
 * What status 0x1E encodes in the voice state machine.
 *
 * @par Callers
 * gBeamPartUpdate, func_8006C488.
 */
void gRefreshPlayerWeaponSound(struct Unk6C488 *object, s32 state)
{
    s32 s1;
    s32 s2;
    u32 i;
    s32 firstFree;
    struct UnkA10E0 *entry;
    s32 handle;

    s1 = object->stats->unkC1;
    if (s1 < 2) {
        s2 = gResolveSoundRecordIndex(state, gSoundBankTableLow, 0x47);
        firstFree = -1;
        if (s2 != 0xFF) {
            entry = &gPlayerSlots[s1 * 3];
            for (i = 0; i < 3; i++, entry++) {
                if (entry->status != 0) {
                    if (entry->bank == s2) {
                        if (entry->field10 == entry->handle->field10) {
                            entry->status = 0x1E;
                            entry->unk4 = 1;
                            goto afterLoop;
                        }
                        entry->status = 0;
                    }
                }
                if (entry->status == 0 && firstFree == -1) {
                    firstFree = i;
                }
            }
        afterLoop:
            if (i != 3) {
                return;
            }
            if (firstFree == -1) {
                return;
            }
            entry = &gPlayerSlots[s1 * 3] + firstFree;
            handle = gPlaySpatialSoundBankSlot(0, 0, s2, (s32)object, 0);
            if (handle != 0) {
                entry->status = 0x1E;
                entry->unk4 = 1;
                entry->bank = s2;
                entry->field10 = ((struct UnkHandle *)handle)->field10;
                entry->handle = (struct UnkHandle *)handle;
            } else {
                entry->status = 0;
            }
        }
    }
}
