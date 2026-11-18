// Categories: Game Engine > Menu System | Game Engine > Front End
#include "game/types.h"

struct Unk78210Mode {
    u8 unk00[0x0C];
    s32 players;
    u8 unk10[0x4C];
    s32 split;
};

struct Unk78210Panel {
    u8 unk00[5];
    u8 visible;
    u8 unk06[0x2A];
};

extern struct Unk78210Mode *gGameSession[] __asm__("D_8009FA94");
extern struct Unk78210Panel D_800A3868;
extern struct Unk78210Panel D_800A3898;
extern struct Unk78210Panel D_800A38C8;
extern u8 D_800A377D[];
extern s32 D_8009F8E0;
extern s32 D_8009F8E4;

extern void gUpdateMainMenuUnlocks(void) __asm__("func_8007A168");
extern void func_80078650(struct Unk78210Panel *panel, s32 arg1);

/**
 * @brief Selects and activates the appropriate front-end menu panel for 1-player or 2-player mode and split state.
 *
 * Confirmed: calls func_8007A168 first; if D_8009FA94[0]->players == 1, activates D_800A38C8 (when split == 1) or D_800A3868 with visible = 1 via func_80078650 and sets D_800A377D[0] = 1; otherwise clears D_8009F8E0, D_8009F8E4, and D_800A377D[0], and activates D_800A3898 (split == 1) or D_800A3868 with visible = 0.
 * Category follows menu panel blocks D_800A3868/D_800A3898/D_800A38C8, active-menu switcher func_80078650, and front-end callers func_8008125C/func_800782EC.
 * Likely: central front-end screen router selecting Precinct Assault vs Crime War / 1P vs 2P menu panels upon entering or returning to menus; Unknown: meaning of D_8009F8E0, D_8009F8E4, and D_800A377D[0].
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_8009FA94 - mission/mode struct array (players +0x0C, split +0x5C); D_800A3868/D_800A3898/D_800A38C8 - menu panel structs; D_800A377D/D_8009F8E0/D_8009F8E4 - front-end state words/flags.
 * @par Control flow
 * Unconditional call to func_8007A168, followed by two-level branching on players == 1 and split == 1.
 * @par Calls
 * func_8007A168 - updates unlock flags in D_8009BBF8/D_8009F3C3/D_8009F3C4; func_80078650 - activates chosen menu panel with arg1 = 0 (replace).
 * @par Side effects
 * Switches active menu D_8009F468 via func_80078650, sets D_800A3868.visible, and updates D_800A377D[0], D_8009F8E0, and D_8009F8E4.
 * @par Callers
 * Confirmed: func_8008125C (0x80081BF8 in asm/us/main.s); func_800782EC (0x800784E4 in asm/us/main.s, cases 1 and 2) / Likely: none / Unknown: none.
 */
void gRouteFrontEndModeMenu(void) __asm__("func_80078210");

void gRouteFrontEndModeMenu(void)
{
    gUpdateMainMenuUnlocks();
    if (gGameSession[0]->players == 1) {
        if (gGameSession[0]->split == 1) {
            func_80078650(&D_800A38C8, 0);
        } else {
            func_80078650(&D_800A3868, 0);
            D_800A3868.visible = 1;
        }
        D_800A377D[0] = 1;
    } else {
        *(volatile s32 *)&D_8009F8E0 = 0;
        *(volatile s32 *)&D_8009F8E4 = 0;
        *(volatile u8 *)&D_800A377D[0] = 0;
        if (((volatile struct Unk78210Mode *)gGameSession[0])->split == 1) {
            func_80078650(&D_800A3898, 0);
        } else {
            func_80078650(&D_800A3868, 0);
            D_800A3868.visible = 0;
        }
    }
}
