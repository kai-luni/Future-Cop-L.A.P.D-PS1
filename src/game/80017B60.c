// Categories: Graphics > Map | Resources > Asset Selection
#include "game/types.h"

extern void *gCamera __asm__("D_8009F9E4");

extern void *func_80017B28(s32 id);

/**
 * @brief Selects the active map record by id and stores it in D_8009F9E4.
 *
 * @param id Record id searched in the list at D_8009F588.
 * @return Nothing.
 *
 * @par Confirmed
 * D_8009F9E4 = func_80017B28(id); the lookup walks the linked list whose
 * nodes carry the id at +0x88. The map renderer func_8006531C reads the
 * selected record's words [0] and [2] as the grid origin.
 *
 * @par Callers
 * func_80027510, func_80056634, func_800569EC, func_800573A4,
 * func_80058C3C, func_80066C9C, func_80066E98.
 */
void func_80017B60(s32 id)
{
    gCamera = func_80017B28(id);
}
