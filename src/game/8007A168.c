// Categories: Graphics > HUD | Game Engine > Front End
#include "game/types.h"

struct Entry {
    u8 unk00[0x1F];
    u8 unk1F;
    u8 pad[0x04];
};

extern u8 D_8009F3C0;
extern u8 D_8009F3C3;
extern u8 D_8009F3C4;
extern u8 D_8009F458;
extern u8 D_8009BBF8[];
extern struct Entry D_8009B4E4[];
extern s8 D_8009B72C[];

/**
 * @brief Applies the saved-menu and all-locked overlays to the screen record.
 *
 * Two independent checks speak into the persistent screen block D_8009BBF8.
 * The first (paused menu, save flag or level 8) sets the "menu" byte at 0x46 and
 * the text id 0x526C at 0x4E and latches D_8009F3C3. The second (paused menu, a
 * second save flag, or all five selected entries carrying type 0x0A) sets the
 * "locked" byte at 0x9A, text id 0x5276 at 0xA2 and latches D_8009F3C4. Both
 * latches are sticky until the caller resets them.
 *
 * @par Globals
 * D_8009BBF8 - screen record (0x46, 0x4E, 0x9A, 0xA2);
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
void func_8007A168(void)
{
    if (D_8009F3C0 != 0 || D_8009F3C3 != 0 || D_8009F458 == 8) {
        D_8009BBF8[0x46] = 1;
        *(u16 *)(D_8009BBF8 + 0x4E) = 0x526C;
        D_8009F3C3 = 1;
    }
    if (D_8009F3C0 != 0 || D_8009F3C4 != 0 ||
        (D_8009B4E4[D_8009B72C[0]].unk1F == 0xA &&
         D_8009B4E4[D_8009B72C[1]].unk1F == D_8009B4E4[D_8009B72C[0]].unk1F &&
         D_8009B4E4[D_8009B72C[2]].unk1F == D_8009B4E4[D_8009B72C[1]].unk1F &&
         D_8009B4E4[D_8009B72C[3]].unk1F == D_8009B4E4[D_8009B72C[2]].unk1F &&
         D_8009B4E4[D_8009B72C[4]].unk1F == D_8009B4E4[D_8009B72C[3]].unk1F)) {
        D_8009BBF8[0x9A] = 1;
        *(u16 *)(D_8009BBF8 + 0xA2) = 0x5276;
        D_8009F3C4 = 1;
    }
}
