// Categories: Resources > Memory Allocation | Game Engine > Subsystem Init
#include "game/types.h"

extern s32 gCellEntityList[2] __asm__("D_8009F818");
extern s32 gCellMap __asm__("D_8009F820");
extern s32 D_8009F830;
extern u8 D_800A8DC0[];

extern s32 func_80017EBC(s32, s32);

/**
 * @brief Allocates the three 0x400-byte work buffers of a subsystem and clears them.
 *
 * Three blocks of 0x400 bytes with allocator tag 2 are taken: two go into the
 * array D_8009F818, the third into D_8009F830. The active counter D_8009F820 is
 * reset and the first byte of both 0x44-byte records in D_800A8DC0 is cleared
 * in reverse order, which zeroes the "in use" marker of each record.
 *
 * @par Globals
 * D_8009F818 - two buffer pointers; D_8009F830 - third buffer;
 * D_8009F820 - active counter; D_800A8DC0 - records with a 0x44 stride.
 *
 * @par Control flow
 * Two short loops, no nesting.
 *
 * @par Calls
 * func_80017EBC - upper allocator.
 *
 * @par Side effects
 * Allocates 0xC00 bytes and resets the bookkeeping words.
 *
 * @par Callers
 * Confirmed: func_80024A24 (mission boot).
 */
void func_800617D8(void)
{
    register s32 *p __asm__("$17");
    s32 i;

    gCellMap = 0;
    for (i = 0, p = gCellEntityList; i < 2; i++) {
        *p = func_80017EBC(0x400, 2);
        p++;
    }
    D_8009F830 = func_80017EBC(0x400, 2);
    for (i = 1; i >= 0; i--) {
        D_800A8DC0[i * 0x44] = 0;
    }
}
