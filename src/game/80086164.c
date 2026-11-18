// Categories: Resources > Memory
#include "game/types.h"

extern void *D_8009F948;
extern void *D_8009F94C;
extern void *gWorkAreaBlock __asm__("D_8009F950");
extern void *D_8009F954;

extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 *  @brief Processes up to four stored block pointers and clears them.
 *
 *  Passes each non-null block pointer to the block operation, then resets
 *  all four global pointers regardless of which were present.
 *  @par Globals
 *  D_8009F948, D_8009F94C, D_8009F950, and D_8009F954 hold the pointers.
 *  @par Control flow
 *  Checks each pointer independently before clearing all four.
 *  @par Calls
 *  func_800181D4 processes each non-null block.
 *  @par Side effects
 *  Clears the four global pointers; the calls may have additional effects.
 *  @par Callers
 *  Confirmed: func_80024D14.
 */
void func_80086164(void)
{
    if (D_8009F948) {
        gFreeBlock(D_8009F948);
    }
    if (D_8009F94C) {
        gFreeBlock(D_8009F94C);
    }
    if (gWorkAreaBlock) {
        gFreeBlock(gWorkAreaBlock);
    }
    if (D_8009F954) {
        gFreeBlock(D_8009F954);
    }
    D_8009F948 = (void *)0;
    D_8009F94C = (void *)0;
    gWorkAreaBlock = (void *)0;
    D_8009F954 = (void *)0;
}
