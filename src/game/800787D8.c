// Categories: Game Engine > Menu System | Input > Menu Navigation
#include "game/types.h"

typedef struct Entry {
    u8 pad00[0xE];
    u8 unk0E;
    s8 unk0F;
    u8 pad10[0xC];
} Entry;

extern u8 *gFontBankTable __asm__("D_8009F468");

/**
 * @brief Finds the next enabled menu item in a target group and writes its index to D_8009F468.
 *
 * Scans the Entry array at *(D_8009F468 + 0x28) wrapping modulo item count D_8009F468[2],
 * starting at arg0 and stepping forward (arg1 > 0) or backward (arg1 < 0) until an entry
 * with unk0F (+0x0F) == arg2 and unk0E (+0x0E) != 0 is found, or the full list is checked.
 *
 * @param arg0 Initial candidate item index (wrapped modulo D_8009F468[2]).
 * @param arg1 Step direction (> 0 forward, < 0 backward, 0 tests arg0 once without stepping).
 * @param arg2 Target group/cursor slot index matched against entry->unk0F and updated in D_8009F468[arg2].
 *
 * @par Globals
 * D_8009F468 - active menu descriptor (+0x02 item count, +0x28 Entry pointer, +0x00/+0x01 selected item per group).
 * @par Control flow
 * Early return if D_8009F468 == 0; infinite loop wrapping arg0, testing match, guarding on arg1 == 0 or i > count, then stepping.
 * @par Calls
 * None.
 * @par Side effects
 * Writes the selected item index into D_8009F468[entries[arg0].unk0F] on match.
 * @par Callers
 * Confirmed: func_80078650, func_8007B3F4, func_80080010, func_80082020; also direct calls at 0x800796A0 and 0x8007976C in asm/us/main.s.
 */
void gSelectEnabledMenuItem(s8 arg0, s8 arg1, s8 arg2) __asm__("func_800787D8");

void gSelectEnabledMenuItem(s8 arg0, s8 arg1, s8 arg2)
{
    Entry *entries;
    s8 i;

    if (gFontBankTable == 0) {
        return;
    }

    i = 0;
    while (1) {
        if (arg0 >= (s8)gFontBankTable[2]) {
            arg0 = 0;
        }
        if (arg0 < 0) {
            arg0 = gFontBankTable[2] - 1;
        }

        entries = *(Entry **)(gFontBankTable + 0x28);
        if (entries[arg0].unk0F == arg2 && entries[arg0].unk0E != 0) {
            gFontBankTable[entries[arg0].unk0F] = arg0;
            return;
        }

        if (arg1 == 0 || i > (s8)gFontBankTable[2]) {
            return;
        }

        i++;
        if (arg1 > 0) {
            arg0++;
        } else {
            arg0--;
        }
    }
}
