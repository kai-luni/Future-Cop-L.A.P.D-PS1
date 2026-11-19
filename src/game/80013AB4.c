// Categories: Graphics > Renderer Setup | Resources > Memory Management
#include "game/types.h"

extern void *gLevelVertexBlock __asm__("D_8009FA1C");
extern void *gPartVertexBuffer __asm__("D_8009F578");
extern void *gClipFlagBuffer __asm__("D_8009F57C");

extern void gFreeBlock(void *block) __asm__("func_800181D4");

void gFreeModelRendererBuffers(void) __asm__("func_80013AB4");

/**
 * @brief Releases the three buffers allocated for the model renderer.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Loads each of three GP-relative pointer words into $a0 and calls the free
 * routine. The global words are left as they were.
 *
 * @par Globals
 * Reads gLevelVertexBlock, gPartVertexBuffer, and gClipFlagBuffer.
 *
 * @par Callers
 * Direct calls in the retail listing are from gResetMissionSubsystems.
 *
 * @par Role and path
 * Engine graphics teardown pairs this with gInitModelRendererBuffers.
 *
 * @par Unknown
 * The caller controls when the saved pointers cease to be usable.
 */
void gFreeModelRendererBuffers(void)
{
    gFreeBlock(gLevelVertexBlock);
    gFreeBlock(gPartVertexBuffer);
    gFreeBlock(gClipFlagBuffer);
}
