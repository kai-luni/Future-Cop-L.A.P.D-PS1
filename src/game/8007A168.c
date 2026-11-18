// Categories: Graphics > HUD | Game Engine > Front End
#include "game/types.h"

struct Entry {
    u8 unk00[0x1F];
    u8 unk1F;
    u8 pad[0x04];
};

extern u8 gFrontEndListFlag __asm__("D_8009F3C0");
extern u8 gPausedMenuFlag __asm__("D_8009F3C3");
extern u8 gLockedMenuFlag __asm__("D_8009F3C4");
extern u8 gCrimeWarProgress __asm__("D_8009F458");
extern u8 gMainMenuItems[] __asm__("D_8009BBF8");
extern struct Entry gWideTextTable[] __asm__("D_8009B4E4");
extern s8 gFrontEndListTable[] __asm__("D_8009B72C");

/**
 * @brief Enables two conditional entries in the main-menu item array.
 *
 * The first check (paused menu, save flag or level 8) enables item 2 at
 * offset 0x46, assigns text ID 0x526C at 0x4E and latches D_8009F3C3.
 * The second (paused menu, a second save flag, or all five selected entries
 * carrying type 0x0A) enables item 5 at 0x9A, assigns text ID 0x5276 at
 * 0xA2 and latches D_8009F3C4. Both latches remain set until reset elsewhere.
 *
 * @par Globals
 * D_8009BBF8 - main-menu item array (0x46, 0x4E, 0x9A, 0xA2);
 * D_8009F3C0/D_8009F3C3/D_8009F3C4 and D_8009F458 - menu/save flags;
 * D_8009B4E4 and D_8009B72C - entry table and the five selected indices.
 *
 * @par Control flow
 * Two sequential condition blocks, each a handful of tests.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Writes the screen record and sets the two latches.
 *
 * @par Callers
 * Confirmed: func_80078210 (mode panel setup) and the menu redraw path.
 */
void gUpdateMainMenuUnlocks(void) __asm__("func_8007A168");

void gUpdateMainMenuUnlocks(void)
{
    if (gFrontEndListFlag != 0 || gPausedMenuFlag != 0 || gCrimeWarProgress == 8) {
        gMainMenuItems[0x46] = 1;
        *(u16 *)(gMainMenuItems + 0x4E) = 0x526C;
        gPausedMenuFlag = 1;
    }
    if (gFrontEndListFlag != 0 || gLockedMenuFlag != 0 ||
        (gWideTextTable[gFrontEndListTable[0]].unk1F == 0xA &&
         gWideTextTable[gFrontEndListTable[1]].unk1F == gWideTextTable[gFrontEndListTable[0]].unk1F &&
         gWideTextTable[gFrontEndListTable[2]].unk1F == gWideTextTable[gFrontEndListTable[1]].unk1F &&
         gWideTextTable[gFrontEndListTable[3]].unk1F == gWideTextTable[gFrontEndListTable[2]].unk1F &&
         gWideTextTable[gFrontEndListTable[4]].unk1F == gWideTextTable[gFrontEndListTable[3]].unk1F)) {
        gMainMenuItems[0x9A] = 1;
        *(u16 *)(gMainMenuItems + 0xA2) = 0x5276;
        gLockedMenuFlag = 1;
    }
}
