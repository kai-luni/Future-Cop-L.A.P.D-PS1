// Categories: Gameplay > Object Registry | Utilities > Lookup
#include "game/types.h"

extern int gBeamPartUpdate();
extern int gWeaponFireDispatch();
extern int gWeaponSlotUpdate();

struct Unk6AAEC {
    u8 id;
    u8 unk01[0x53];
};

extern s32 gDescriptorRegistryCount __asm__("D_8009F848");
extern struct Unk6AAEC gVmRecordTable[] __asm__("D_800A3340");

/**
 * @brief Finds the record with the given id in the object registry.
 *
 * @param id Identifier stored in byte 0 of each record.
 * @return Pointer to the record in D_800A3340, or 0 when absent.
 *
 * @par Confirmed
 * Linear scan of D_8009F848 entries (stride 0x54), comparing the id byte;
 * first match wins.
 *
 * @par Callers
 * func_8006AB38, gWeaponFireDispatch, gBeamPartUpdate, gWeaponSlotUpdate,
 * func_8006BDFC, func_8006BE90, func_8006C080.
 */
struct Unk6AAEC *gRegistryFind(u8 id) __asm__("func_8006AAEC");

struct Unk6AAEC *gRegistryFind(u8 id)
{
    s32 i;

    for (i = 0; i < gDescriptorRegistryCount; i++) {
        if (gVmRecordTable[i].id == id) {
            return &gVmRecordTable[i];
        }
    }
    return 0;
}
