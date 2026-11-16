// Categories: Game Engine > Front End | Resources > Level Table
#include "game/types.h"

struct Entry {
    u8 unk00[0x1F];
    u8 unk1F;
    u8 unk20;
    u8 pad[3];
};

extern u8 D_8009F3C0;
extern u8 D_8009F3C2;
extern u8 D_8009F880;
extern u8 D_8009F881;
extern u8 D_8009F882;
extern s8 *D_8009F884;
extern u8 D_8009F888;
extern u8 D_8009F889;
extern struct Entry D_8009B4E4[];
extern s8 D_8009B72C[];

void func_80077FFC(void);

/**
 * @brief Resets the front-end list state to the D_8009B72C table and optionally unlocks all five entries.
 *
 * Clears selection indices D_8009F880/D_8009F881 and sets list mode D_8009F889 = 1; when unlock
 * flag D_8009F3C0 is nonzero, forces state bytes (+0x1F, +0x20) of the five D_8009B72C-mapped
 * D_8009B4E4 entries to 10 and refreshes unlock flag D_8009F3C2 via func_80077FFC before sizing the list.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F880/D_8009F881/D_8009F889 - selection indices cleared and mode flag set to 1;
 * D_8009F3C0/D_8009F3C2 - unlock-all input and computed unlock flag;
 * D_8009B4E4/D_8009B72C/D_8009F884/D_8009F882/D_8009F888 - entry table, index map, active list ptr, max index (3+flag) and count (4+flag).
 * @par Control flow
 * Straight-line stores with a conditional D_8009F3C0 != 0 block calling func_80077FFC.
 * @par Calls
 * func_80077FFC - updates D_8009F3C2 from whether D_8009B4E4[D_8009B72C[0..3]].state == 10.
 * @par Side effects
 * Rewrites front-end cursor/list globals and (when D_8009F3C0 != 0) marks D_8009B4E4 entries unlocked.
 * @par Callers
 * Confirmed: func_8007A4CC, func_8007A56C, func_8007EA78, func_8008125C.
 */
void func_80077708(void)
{
    D_8009F880 = 0;
    D_8009F881 = 0;
    D_8009F889 = 1;

    if (D_8009F3C0 != 0) {
        D_8009B4E4[D_8009B72C[0]].unk1F = 10;
        D_8009B4E4[D_8009B72C[1]].unk1F = 10;
        D_8009B4E4[D_8009B72C[2]].unk1F = 10;
        D_8009B4E4[D_8009B72C[3]].unk1F = 10;
        D_8009B4E4[D_8009B72C[0]].unk20 = 10;
        D_8009B4E4[D_8009B72C[1]].unk20 = 10;
        D_8009B4E4[D_8009B72C[2]].unk20 = 10;
        D_8009B4E4[D_8009B72C[3]].unk20 = 10;
        D_8009B4E4[D_8009B72C[4]].unk1F = 10;
        D_8009B4E4[D_8009B72C[4]].unk20 = 10;
        func_80077FFC();
    }

    D_8009F884 = D_8009B72C;
    D_8009F882 = D_8009F3C2 + 3;
    D_8009F888 = D_8009F3C2 + 4;
}
