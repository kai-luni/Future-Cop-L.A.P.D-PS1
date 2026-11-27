// Categories: Game Engine > Front End | Resources > Save Data
#include "game/types.h"

struct Unk77FFC {
    u8 unk00[0x1F];
    u8 state;
    u8 unk20[4];
};

extern struct Unk77FFC gWideTextTable[] __asm__("D_8009B4E4");
extern s8 gFrontEndListTable[] __asm__("D_8009B72C");
extern u8 gUnlockFlag __asm__("D_8009F3C2");

/**
 * @brief Updates the front-end unlock flag D_8009F3C2 from the first four entry states.
 *
 * Checks whether the four D_8009B4E4 records indexed by D_8009B72C[0..3] all have
 * state (+0x1F) equal to 10, setting D_8009F3C2 to 1 if true or 0 otherwise.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009B4E4 - entry table (state byte at +0x1F); D_8009B72C - index map for entries 0..3; D_8009F3C2 - unlock flag written (0 or 1).
 * @par Control flow
 * Single four-way equality test on D_8009B4E4[D_8009B72C[0..3]].state == 10 with an if/else assignment to D_8009F3C2.
 * @par Calls
 * None.
 * @par Side effects
 * Writes 1 or 0 to D_8009F3C2 (read by func_80077708 to size the active list).
 * @par Callers
 * Confirmed: func_80077708, func_800780DC, func_8007EA78, func_8008125C.
 */
void gUpdatePrecinctUnlockFlag(void) __asm__("func_80077FFC");

void gUpdatePrecinctUnlockFlag(void)
{
    if (gWideTextTable[gFrontEndListTable[0]].state == 10 &&
        gWideTextTable[gFrontEndListTable[1]].state == gWideTextTable[gFrontEndListTable[0]].state &&
        gWideTextTable[gFrontEndListTable[2]].state == gWideTextTable[gFrontEndListTable[1]].state &&
        gWideTextTable[gFrontEndListTable[3]].state == gWideTextTable[gFrontEndListTable[2]].state) {
        gUnlockFlag = 1;
    } else {
        gUnlockFlag = 0;
    }
}
