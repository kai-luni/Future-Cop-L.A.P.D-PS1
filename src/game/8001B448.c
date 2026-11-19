// Categories: Resources > Memory Management
#include "game/types.h"

extern void *gPacketBufferBases __asm__("D_8009F5C8");
extern void *gSecondPacketBufferBase __asm__("D_8009F5CC");
extern void *gRenderNodeTable __asm__("D_8009F5D0");
extern void *gRenderNodeTableAlt __asm__("D_8009F5D4");
extern void *gOrderingTableBuffers __asm__("D_8009F5D8");
extern void *gSecondOrderingTableBuffer __asm__("D_8009F5DC");

extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Releases the render arena allocated by gAllocateRenderBuffers().
 *
 * The six blocks are freed in allocation order through func_800181D4(), the
 * shared deallocator: the two 52-byte-record pools, the two 1024-entry
 * 28-byte arrays and the two word tables. The globals keep their (now stale)
 * pointers; nothing is cleared here, so a later free would be a double free -
 * the callers only run this once per module teardown.
 *
 * @return Nothing.
 *
 * @par Globals written
 * Only through the deallocator; gPacketBufferBases..gSecondOrderingTableBuffer themselves are not
 * reset.
 *
 * @par Callers
 * func_800249BC, func_80024D14 (module teardown paths) and func_80076A2C
 * (after the intro has been shown, before the front-end loop starts).
 */
void func_8001B448(void)
{
    gFreeBlock(gPacketBufferBases);
    gFreeBlock(gSecondPacketBufferBase);
    gFreeBlock(gRenderNodeTable);
    gFreeBlock(gRenderNodeTableAlt);
    gFreeBlock(gOrderingTableBuffers);
    gFreeBlock(gSecondOrderingTableBuffer);
}
