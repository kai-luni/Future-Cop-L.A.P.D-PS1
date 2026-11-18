// Categories: Resources > Cleanup
#include "game/types.h"

extern s32 gStillRunningState __asm__("D_8009F464");
extern void *gSpriteAtlasBlock __asm__("D_8009F460");
extern s32 gAtlasTable __asm__("D_8009F45C");
extern void **gPublishedBlockPtr __asm__("D_8009F3F4");

extern void func_800898E8(void);
extern void gReleaseProfileCardOperation(void) __asm__("func_8007FACC");
extern void gShutdownProfileSubsystem(void) __asm__("func_8007FA48");
extern void func_800181D4(void *block);

/**
 *  @brief Releases the active resource state and its allocated blocks.
 *
 *  Cleanup runs only when the active flag is set, and clears the
 *  associated block and 30-entry pointer table after releasing them.
 *  @par Globals
 *  D_8009F464 gates cleanup and is cleared; D_8009F460 and D_8009F45C
 *  hold the optional block and its associated value; D_8009F3F4 is the table.
 *  @par Control flow
 *  Returns if inactive; otherwise runs three cleanup calls, then frees
 *  the optional block and each non-null table entry before the table itself.
 *  @par Calls
 *  func_800898E8, func_8007FACC and func_8007FA48 perform preliminary cleanup;
 *  func_800181D4 releases each allocated block.
 *  @par Side effects
 *  Clears the active flag, released pointers and the associated block value.
 *  @par Callers
 *  Confirmed: func_800249BC.
 */
void func_8007852C(void)
{
    u32 i;

    if (gStillRunningState == 0) {
        return;
    }
    func_800898E8();
    gReleaseProfileCardOperation();
    gShutdownProfileSubsystem();
    if (gSpriteAtlasBlock != 0) {
        func_800181D4(gSpriteAtlasBlock);
        gSpriteAtlasBlock = 0;
        gAtlasTable = 0;
    }
    gStillRunningState = 0;
    if (gPublishedBlockPtr != 0) {
        for (i = 0; i < 0x1E; i++) {
            if (gPublishedBlockPtr[i] != 0) {
                func_800181D4(gPublishedBlockPtr[i]);
                gPublishedBlockPtr[i] = 0;
            }
        }
        func_800181D4(gPublishedBlockPtr);
        gPublishedBlockPtr = 0;
    }
}
