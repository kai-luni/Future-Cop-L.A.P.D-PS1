// Categories: Gameplay > Object Registry | Utilities > Lookup
#include "game/types.h"

struct Unk6AAEC {
    u8 id;
    u8 unk01[0x53];
};

extern s32 gDescriptorRegistryCount __asm__("D_8009F848");
extern struct Unk6AAEC D_800A3340[];

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
 * func_8006AB38, func_8006ACB0, func_8006B23C, func_8006BA50,
 * func_8006BDFC, func_8006BE90, func_8006C080.
 */
struct Unk6AAEC *func_8006AAEC(u8 id)
{
    s32 i;

    for (i = 0; i < gDescriptorRegistryCount; i++) {
        if (D_800A3340[i].id == id) {
            return &D_800A3340[i];
        }
    }
    return 0;
}
