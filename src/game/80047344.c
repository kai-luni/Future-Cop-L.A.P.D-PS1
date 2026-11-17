// Categories: Resources > Sound Registration
#include "game/types.h"

struct Unk7938 {
    u8 unk00[0x10];
    u8 active;
    u8 unk11;
    u8 unk12;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    u8 unk16;
    u8 unk17;
    s32 unk18;
};

struct SoundObj {
    u16 id;
    u8 pad[2];
    u8 unk04;
    u8 unk05;
    u8 unk06;
    u8 unk07;
    u8 unk08[4];
};

extern u8 D_8009F75C;
extern struct Unk7938 gSoundEntryTable[] __asm__("D_800A7938");
extern u8 gSoundBankTableLow[] __asm__("D_800A1830");
extern u8 gSoundIdTable[] __asm__("D_800A1880");
extern u8 D_800A188A[];

extern s32 func_800472E8(s32 key);

/**
 *  @brief Finds or registers a sound object under the low byte of a key.
 *
 *  A new registration copies bounded object fields into a slot and populates lookup tables.
 *  Existing keys and a full 110-slot table return without registering the object.
 *  @param self Object whose identifier and bounded fields are updated for a new slot.
 *  @param key Key whose low byte is used for lookup and storage.
 *  @return Existing or newly allocated slot, or -1 when no slot is available.
 *  @par Globals
 *  D_8009F75C counts slots; D_800A7938 stores entries.
 *  D_800A1830, D_800A1880 and D_800A188A map eligible object bytes to slots.
 *  @par Control flow
 *  After lookup and capacity checks, four object bytes are classified by value and object mode.
 *  @par Calls
 *  func_800472E8 searches for an existing key.
 *  @par Side effects
 *  A new slot increments the count, modifies self and writes entry and lookup-table fields.
 *  @par Callers
 *  Confirmed: func_800241D0.
 */
s32 func_80047344(struct SoundObj *self, s32 key)
{
    s32 slot;
    s32 i;
    s32 value;
    struct Unk7938 *entry;

    slot = func_800472E8(key & 0xFF);
    if (slot != -1 || D_8009F75C >= 0x6E) {
        return slot;
    }

    slot = D_8009F75C;
    D_8009F75C = slot + 1;
    entry = &gSoundEntryTable[slot];

    self->id = slot;
    if (self->unk04 == 0) {
        self->unk04 = 1;
    } else if (self->unk04 > 0xA) {
        self->unk04 = 0xA;
    }
    entry->unk13 = self->unk04;

    if (self->unk05 > 4) {
        self->unk05 = 0x4;
    }
    entry->unk14 = self->unk05;
    entry->unk15 = self->unk06;
    entry->unk16 = key;

    for (i = 0; i < 4; i++) {
        value = self->unk08[i];
        if (value > 0) {
            if (value >= 0x46) {
                if (value < 0x82) {
                    D_800A188A[value] = slot;
                }
            } else {
                if (self->unk07 == 1) {
                    gSoundBankTableLow[value] = slot;
                } else {
                    gSoundIdTable[value] = slot;
                }
            }
        }
    }
    return slot;
}
