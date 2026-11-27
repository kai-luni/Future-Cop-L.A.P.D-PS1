// Categories: Resources > Memory Pools
#include "game/types.h"

extern void *gFixedPool3 __asm__("D_8009FA4C");
extern void *gFixedPool2 __asm__("D_8009FAAC");
extern void *gFixedPool1 __asm__("D_8009F9D0");

extern void gFreeBlock(void *block) __asm__("func_800181D4");

void gFreeFixedPools(void) __asm__("func_80017D20");

/**
 * @brief Frees all three shared fixed-size pools.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads and passes gFixedPool3, gFixedPool2, then gFixedPool1 to gFreeBlock.
 * The three global slots remain intact.
 *
 * @par Globals
 * Reads gFixedPool3, gFixedPool2, and gFixedPool1.
 *
 * @par Callers
 * Direct calls in the retail listing are from gUnloadFrontEndSubsystems,
 * gResetMissionSubsystems, func_80077088.
 *
 * @par Role and path
 * Mission and subsystem teardown release the pools created by
 * gInitHeapAndFixedPools.
 *
 * @par Unknown
 * The allocator's handling of individual invalid pointers belongs to
 * gFreeBlock.
 */
void gFreeFixedPools(void)
{
    gFreeBlock(gFixedPool3);
    gFreeBlock(gFixedPool2);
    gFreeBlock(gFixedPool1);
}
