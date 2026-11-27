// Categories: Resources > Memory Management | Graphics > Image Decode & Upload
#include "game/types.h"

extern void *gPreallocatedMovieVlcTableA __asm__("D_8009F948");
extern void *gPreallocatedMovieVlcTableB __asm__("D_8009F94C");
extern void *gWorkAreaBlock __asm__("D_8009F950");
extern void *gPreallocatedMovieMdecInputBuffer __asm__("D_8009F954");

extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
void gAllocDecodeBuffers(void) __asm__("func_800860CC");


/**
 * @brief Allocates the four static working buffers of the image decoder.
 *
 * All four globals are nulled first and then filled from the tag-2 pool with
 * 0x6000, 0x600, 0x2000 and 0x600 bytes. They are the "static" set of the
 * decode/upload path func_800861E4, used when D_8009F9C4 is 0: D_8009F948 and
 * D_8009F94C are the packed source and its unpack buffer, D_8009F954 and
 * D_8009F950 the destination block and the work area. The allocating path of
 * func_800861E4 grabs 0x10000/0x3000 instead and leaves these alone.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Globals
 * D_8009F948 - 0x6000-byte compressed-source buffer.
 * D_8009F94C - 0x600-byte unpack/palette buffer.
 * D_8009F954 - 0x2000-byte destination block.
 * D_8009F950 - 0x600-byte work area.
 * All four are freed again by func_80086164; no bounds check is performed here.
 *
 * @par Control flow
 * Four stores, four allocations, four stores; the allocation and store order
 * differ (D_8009F954 is allocated before D_8009F950), which is what the retail
 * scheduling does. No branches.
 *
 * @par Calls
 * func_80017EBC - tagged allocation, tag 2, four times.
 *
 * @par Side effects
 * Allocates 0x8C00 bytes in total and publishes them through the four globals.
 *
 * @par Callers
 * Confirmed: func_80024A24 (mission boot).
 *
 * @par Environment path
 * Not a draw routine, but it provisions the image decode/upload path:
 * allocates the static working set used by func_800861E4 when it is not in
 * its allocating mode, all from the tag-2 pool. Called once by the mission
 * boot func_80024A24; func_80086164 frees the same globals.
 */
void gAllocDecodeBuffers(void)
{
    gPreallocatedMovieVlcTableA = (void *)0;
    gPreallocatedMovieVlcTableB = (void *)0;
    gWorkAreaBlock = (void *)0;
    gPreallocatedMovieMdecInputBuffer = (void *)0;

    gPreallocatedMovieVlcTableA = gAllocMemory(0x6000, 2);
    gPreallocatedMovieVlcTableB = gAllocMemory(0x600, 2);
    gPreallocatedMovieMdecInputBuffer = gAllocMemory(0x2000, 2);
    gWorkAreaBlock = gAllocMemory(0x600, 2);
}
