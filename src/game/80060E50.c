// Categories: Game Engine > Object Registry | Resources > Memory Management
#include "game/types.h"

struct Unk60E28Table;

extern struct Unk60E28Table *gRegistryTable __asm__("D_8009F800");
extern void func_800181D4(void *block);

/**
 * @brief Frees the 0x80-slot live-object registry table D_8009F800.
 *
 * Passes D_8009F800 (the 0x204-byte table allocated by func_80060E28 at mission boot) directly to func_800181D4 during global subsystem teardown; leaves D_8009F800 unchanged until reallocated.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F800 - pointer to the 0x80-entry live-object registry table, read and freed.
 *
 * @par Control flow
 * Straight-line load of D_8009F800 and call to func_800181D4.
 *
 * @par Calls
 * func_800181D4 - releases the D_8009F800 heap block.
 *
 * @par Side effects
 * Frees the memory block referenced by D_8009F800.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024DD8, between-mission subsystem reset) / Likely: none / Unknown: none.
 */
void func_80060E50(void)
{
    func_800181D4(gRegistryTable);
}
