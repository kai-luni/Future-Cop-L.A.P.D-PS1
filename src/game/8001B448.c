// Categories: Resources > Memory Management
#include "game/types.h"

extern void *gPacketBufferBases __asm__("D_8009F5C8");
extern void *gSecondPacketBufferBase __asm__("D_8009F5CC");
extern void *gRenderNodeTable __asm__("D_8009F5D0");
extern void *gRenderNodeTableAlt __asm__("D_8009F5D4");
extern void *gOrderingTableBuffers __asm__("D_8009F5D8");
extern void *gSecondOrderingTableBuffer __asm__("D_8009F5DC");

extern void gFreeBlock(void *block) __asm__("func_800181D4");

void gFreeRenderBuffers(void) __asm__("func_8001B448");

/**
 * @brief Releases the six buffer allocations used by the paired render arenas.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads six successive GP-relative pointer words and passes each in turn to
 * gFreeBlock. There are no stores to those words.
 *
 * @par Globals
 * Reads gPacketBufferBases, gSecondPacketBufferBase, gRenderNodeTable,
 * gRenderNodeTableAlt, gOrderingTableBuffers, and gSecondOrderingTableBuffer.
 *
 * @par Callers
 * Direct calls in the retail listing are from gUnloadFrontEndSubsystems,
 * gResetMissionSubsystems, gGameMain.
 *
 * @par Role and path
 * Front end, mission, and subsystem teardown free the storage from
 * gAllocateRenderBuffers.
 *
 * @par Unknown
 * The caller controls when frame submission has stopped using the saved
 * pointers.
 */
void gFreeRenderBuffers(void)
{
    gFreeBlock(gPacketBufferBases);
    gFreeBlock(gSecondPacketBufferBase);
    gFreeBlock(gRenderNodeTable);
    gFreeBlock(gRenderNodeTableAlt);
    gFreeBlock(gOrderingTableBuffers);
    gFreeBlock(gSecondOrderingTableBuffer);
}
