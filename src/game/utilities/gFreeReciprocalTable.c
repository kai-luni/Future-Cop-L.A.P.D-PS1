// Categories: Utilities > Mathematics > Tables | Utilities > Memory Management
#include "game/types.h"

extern void *gReciprocalTablePtr __asm__("D_8009F5B0");

extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Frees the 1024-entry reciprocal table built at boot.
 *
 * D_8009F5B0 is the 0x800-byte block that func_80019828 fills with packed exponent/mantissa pairs and that func_80095900 reads as a reciprocal lookup; the pointer is tested first, so the routine is safe on a boot path where the table was never built.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_8009F5B0 - table pointer; read and freed, deliberately left dangling.
 *
 * @par Control flow
 * One null test around the free, no loops.
 *
 * @par Calls
 * func_800181D4 - returns the block to the heap.
 *
 * @par Side effects
 * The table memory is released while D_8009F5B0 still points at it, so calling this twice frees the same block twice.
 *
 * @par Callers
 * Confirmed: func_800249BC (0x800249FC) and func_80024D14 (0x80024E00), the game-state reset and the subsystem shutdown.
 */
void gFreeReciprocalTable(void) __asm__("func_800198DC");

void gFreeReciprocalTable(void)
{
    if (gReciprocalTablePtr != 0) {
        gFreeBlock(gReciprocalTablePtr);
    }
}
