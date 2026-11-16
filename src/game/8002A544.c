// Categories: Platform > Streaming Audio | Resources > Asset Loader
#include "game/types.h"

extern s32 D_8009F6DC;
extern u8 D_8009F6B8;
extern u8 D_800A0B90[];
extern void func_8002A49C(void);

/**
 * @brief Flushes queued VAGM audio chunks and clears the active stream flag D_8009F6DC.
 *
 * When D_8009F6DC is non-zero, sets D_800A0B90[0] = 1 unless bit 0x40 of D_8009F6B8 is set,
 * drains the 4-slot VAGM chunk ring via func_8002A49C, and clears D_8009F6DC to 0.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009F6DC - active VAGM stream state/flag (tested and cleared to 0); D_8009F6B8 - flags byte; D_800A0B90 - completion flag byte.
 *
 * @par Control flow
 * Outer guard on D_8009F6DC != 0 with an inner test on (D_8009F6B8 & 0x40) == 0 before calling func_8002A49C.
 *
 * @par Calls
 * func_8002A49C - drains pending VAGM ring entries in D_800A0C50 via func_8002A31C.
 *
 * @par Side effects
 * May write D_800A0B90[0] = 1, drains D_800A0C50 via func_8002A49C, and sets D_8009F6DC = 0.
 *
 * @par Callers
 * Confirmed: func_8002902C (0x80029310, on VAGM stream start failure) and func_80024630 (0x80024690, stream stop) / Likely: none / Unknown: none.
 */
void func_8002A544(void)
{
    if (D_8009F6DC != 0) {
        if ((D_8009F6B8 & 0x40) == 0) {
            D_800A0B90[0] = 1;
        }
        func_8002A49C();
        D_8009F6DC = 0;
    }
}
