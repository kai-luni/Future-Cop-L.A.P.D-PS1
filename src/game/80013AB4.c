// Categories: Graphics > Renderer Setup | Resources > Memory Management
#include "game/types.h"

extern void *D_8009FA1C;
extern void *D_8009F578;
extern void *D_8009F57C;

extern void func_800181D4(void *block);

/**
 * @brief Frees the three render scratch buffers allocated by func_80013A14.
 *
 * Releases the 0x400-byte mesh/graphics buffer D_8009FA1C, the 0x800-byte transformed vertex
 * buffer D_8009F578, and the 0x100-byte clip/visibility flag buffer D_8009F57C via func_800181D4.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA1C, D_8009F578, D_8009F57C - render buffer pointers passed sequentially to func_800181D4.
 *
 * @par Control flow
 * Straight-line sequence of three func_800181D4 deallocation calls.
 *
 * @par Calls
 * func_800181D4 - frees each heap/range-allocated render scratch block.
 *
 * @par Side effects
 * Deallocates the three render scratch buffers from the global allocator/range table.
 *
 * @par Callers
 * Confirmed: func_80024D14 (0x80024DA0, during between-mission global subsystem reset) / Likely: none / Unknown: none.
 */
void func_80013AB4(void)
{
    func_800181D4(D_8009FA1C);
    func_800181D4(D_8009F578);
    func_800181D4(D_8009F57C);
}
