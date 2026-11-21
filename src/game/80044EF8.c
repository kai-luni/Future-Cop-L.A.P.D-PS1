// Categories: Game Engine > Table Initialization
#include "game/types.h"

extern u8 gSoundBankTableLow[] __asm__("D_800A1830");
extern u8 gSoundIdTable[] __asm__("D_800A1880");
extern u8 gSoundBankTableHigh[] __asm__("D_800A18D0");
extern u8 gRegisteredSoundBankCount __asm__("D_8009F75C");

extern void func_80045504(s32 index);

/**
 *  @brief Resets three byte tables and initializes 110 indexed entries.
 *
 *  Fills the first two tables with 70 sentinel bytes each and the third
 *  with 60 sentinel bytes, then clears a global byte before dispatching initialization.
 *  @par Globals
 *  D_800A1830, D_800A1880, and D_800A18D0 receive 0xFF; D_8009F75C is cleared.
 *  @par Control flow
 *  Two fill loops precede a loop over indices 0 through 109.
 *  @par Calls
 *  func_80045504 is called once for each index.
 *  @par Side effects
 *  Overwrites the specified table ranges and global byte; the callee may have further effects.
 *  @par Callers
 *  Confirmed: func_800453B8.
 */
void func_80044EF8(void)
{
    s32 i;
    u8 *entry;
    u8 *other;
    u8 value;

    entry = gSoundBankTableLow;
    other = gSoundIdTable;
    i = 0;
    do {
        i++;
        *entry++ = 0xFF;
        *other++ = 0xFF;
    } while (i < 0x46);
    entry = gSoundBankTableHigh;
    value = 0xFF;
    for (i = 0x3B; i >= 0; i--) {
        *entry++ = value;
    }
    gRegisteredSoundBankCount = 0;
    for (i = 0; i < 0x6E; i++) {
        func_80045504(i);
    }
}
