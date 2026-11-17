// Categories: Game Engine > Front End | Resources > Level Table
#include "game/types.h"

extern u8 gMissionListTable[] __asm__("D_8009B724");
extern u8 gFrontEndListFlag __asm__("D_8009F3C0");
extern u8 D_8009F880;
extern u8 D_8009F881;
extern u8 D_8009F882;
extern u8 D_8009F889;
extern u8 D_8009F458;
extern u8 D_8009F3F1;
extern u8 *D_8009F884;
extern u8 D_8009F888;

/**
 * @brief Resets the front-end mission list state to the default 8-entry table (D_8009B724).
 *
 * Zeroes the active selection indices and mode flag, unlocks all eight entries
 * when D_8009F3C0 is nonzero, and binds the front-end list pointer and count
 * to D_8009B724 (contrasting with func_80077708's D_8009B72C list).
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F880/D_8009F881/D_8009F889 - selection indices and list mode flag, cleared to 0.
 * D_8009F3C0 - unlock-all flag; when nonzero sets D_8009F882 = 7 and D_8009F458/D_8009F3F1 = 8, else D_8009F882 = 0.
 * D_8009F884/D_8009F888 - list pointer set to D_8009B724 and entry count set to 8.
 * @par Control flow
 * Straight-line stores around a single if/else check on D_8009F3C0.
 * @par Calls
 * None.
 * @par Side effects
 * Overwrites front-end list cursor, unlock limit, stage indices, list pointer and count globals.
 * @par Callers
 * Confirmed: func_8007A3AC, func_8007A450, func_8007EA78, func_8007F8FC, func_8008125C.
 */
void func_800776B8(void)
{
    D_8009F880 = 0;
    D_8009F881 = 0;
    D_8009F889 = 0;
    if (gFrontEndListFlag != 0) {
        D_8009F882 = 7;
        D_8009F458 = 8;
        D_8009F3F1 = 8;
    } else {
        D_8009F882 = 0;
    }
    D_8009F884 = gMissionListTable;
    D_8009F888 = 8;
}
