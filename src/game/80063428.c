// Categories: Graphics > Terrain | Game Engine > Init
typedef unsigned char u8;
typedef signed int s32;

extern u8 gTilePatternCounterState[] __asm__("D_800A2730");
extern u8 gTileDefTable[] __asm__("D_800A6F38");
extern s32 gTexpageBaseTable[] __asm__("D_800A8E80");

extern s32 gMapGridHeader __asm__("D_8009F80C");
extern s32 gMapChunkNode __asm__("D_8009F810");
extern s32 gMapTileAnimList __asm__("D_8009FA04");
extern int *gTileBlockTable __asm__("D_8009F808");
extern int *gTableBuffer __asm__("D_8009FA08");

extern void gClearTerrainState(void) __asm__("func_80063134");
extern void *gAllocMemory(s32 size, s32 mode) __asm__("func_80017EBC");
extern int func_80018460(int *value);
extern void func_8008CD20(void *ptr, s32 arg1, s32 arg2);
int *gInitMapTables(void) __asm__("func_80063428");


/**
 * @brief Clears map/terrain tables and allocates the tile and mesh pointer buffers.
 *
 * Zeroes 0x201 bytes at D_800A2730, calls func_80063134, clears D_8009F80C and D_8009F810,
 * allocates and zeroes D_8009F808 (0x1A0 B) and D_8009FA08 (0x1B18 B), and resets D_800A8E80[0..9], D_8009FA04, and D_800A6F38[0].
 *
 * @param void No parameters.
 * @return Pointer to the newly allocated and zeroed D_8009FA08 buffer.
 *
 * @par Globals
 * D_800A2730 (0x201 bytes cleared), D_8009F808/D_8009FA08 (tag-2 buffers),
 * D_8009F80C, D_8009F810, D_8009FA04, D_800A8E80[0..9], D_800A6F38[0] (all cleared).
 * @par Control flow
 * Backwards 0x201-byte clear loop, straight-line resets/allocations using func_80018460 size headers, and 10-entry reverse zero loop.
 * @par Calls
 * func_80063134 (clears map state), func_80017EBC (allocates tag-2 blocks), func_80018460 (reads block size), func_8008CD20 (memset zero).
 * @par Side effects
 * Allocates two tag-2 heap buffers and resets terrain/texture globals.
 * @par Callers
 * Confirmed: func_80024A24 (mission boot).
 */
int *gInitMapTables(void)
{
    s32 i;

    {
        register s32 count __asm__("$3");
        register u8 *cursor __asm__("$2");

        count = 0x200;
        cursor = gTilePatternCounterState;
        cursor += 0x200;
        do {
            *cursor = 0;
            --count;
            --cursor;
        } while (count >= 0);
    }

    gClearTerrainState();
    gMapGridHeader = 0;
    gMapChunkNode = 0;
    gTileBlockTable = gAllocMemory(0x1A0, 2);
    func_8008CD20(gTileBlockTable, 0, func_80018460(gTileBlockTable));
    gTableBuffer = gAllocMemory(0x1B18, 2);
    func_8008CD20(gTableBuffer, 0, func_80018460(gTableBuffer));

    for (i = 9; i >= 0; i--) {
        gTexpageBaseTable[i] = 0;
    }

    {
        register int *result __asm__("$2");

        result = gTableBuffer;
        gMapTileAnimList = 0;
        gTileDefTable[0] = 0;
        return result;
    }
}
