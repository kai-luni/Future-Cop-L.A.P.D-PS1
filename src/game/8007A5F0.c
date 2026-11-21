// Categories: Game Engine > Front-End Configuration | Game Engine > Menu System
#include "game/types.h"

struct Unk78210Mode {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C[0x50];
    s32 split;
    u8 unk60[0x38];
    s32 unk98;
};

struct Unk78210Panel {
    u8 unk00[5];
    u8 visible;
    u8 unk06[0x2A];
};

extern struct Unk78210Mode *gGameSession __asm__("D_8009FA94");
extern volatile s32 gPlayerSelection __asm__("D_8009F8F0");
extern s32 gStagedPlayer2Difficulty __asm__("D_8009F8F4");
extern s8 *gFontBankTable __asm__("D_8009F468");
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern volatile s32 gPlayer1MenuReadyFlag __asm__("D_8009F8E0");
extern s32 D_8009F8E0_arr[];
extern volatile s32 gPlayer2MenuReadyFlag __asm__("D_8009F8E4");
extern u8 gTwoPlayerMissionSelectMenuItems[] __asm__("D_8009BB34");
extern s16 gMenuPreviewScaleP1[] __asm__("D_8009BB66");
extern s16 gMenuPreviewScaleP2[] __asm__("D_8009BBBA");
extern struct Unk78210Panel gCrimeWarHubMenuDescriptor __asm__("D_800A3868");
extern struct Unk78210Panel gTwoPlayerPrecinctHubMenuDescriptor __asm__("D_800A3898");
extern struct Unk78210Panel gOnePlayerPrecinctHubMenuDescriptor __asm__("D_800A38C8");

extern void gActivateMenuPanel(struct Unk78210Panel *panel, s32 arg1) __asm__("func_80078650");

/**
 * @brief Menu-item callback handling player/split-screen option hover timing and sub-panel push on confirm.
 *
 * When a0 == 0 (draw/hover), clears D_8009F8E0_arr[player] if cursor moved off a4 and sets preview
 * scale D_8009BB66/D_8009BBBA to 0x64 (or 0x6E when ready). When a0 == 3 (confirm), writes D_8009F8F0/D_8009F8F4
 * into D_8009FA94+0x08/+0x98, syncs both players in 2P mode (D_8009F468[5] == 2), and pushes D_800A3868/898/8C8.
 *
 * @param a0 Callback phase (0 hover/draw update, 3 confirm action; other values ignored).
 * @param a1,a2,a3,a4 a1..a3 unused; a4 is the menu item index associated with this callback.
 *
 * @par Globals
 * D_8009FA94 - game mode config (+0x08, +0x5C split flag, +0x98); D_8009F8F0/D_8009F8F4 - selected options;
 * D_8009F468/D_8009F8B8/D_8009F8E0/D_8009F8E4 - menu cursor, player index, and 2P ready flags; D_8009BB34/D_8009BB66/D_8009BBBA - panel scales.
 * @par Control flow
 * Top-level split on a0 == 0 vs a0 == 3, branching on 2P menu mode (D_8009F468[5] == 2) and mode->split == 0.
 * @par Calls
 * func_80078650 - pushes the next menu panel (D_800A3868, D_800A3898, or D_800A38C8) with arg1 = 1.
 * @par Side effects
 * Updates D_8009FA94 mode fields, 2P ready flags, item scale values, and pushes active menu D_8009F468.
 * @par Callers
 * Unknown: no direct calls; address placed as menu-item callback pointers at 0x8009BAE4, 0x8009BB54, and 0x8009BBA8 in asm/us/main.s.
 */
void gHandlePlayerJoinSelection(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4) __asm__("func_8007A5F0");

void gHandlePlayerJoinSelection(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4)
{
    register struct Unk78210Mode *mode __asm__("$6");
    register s8 *p __asm__("$3");

    if (a0 != 0) {
        if (a0 != 3) {
            return;
        }
        mode = gGameSession;
        p = gFontBankTable;
        mode->unk08 = gPlayerSelection;
        __asm__ volatile("" ::: "memory");
        mode->unk98 = gStagedPlayer2Difficulty;
        if (p[5] == 2) {
            register s32 idx __asm__("$2");
            register u8 *base __asm__("$2");
            idx = gActivePlayerIndex;
            __asm__ volatile("" ::: "memory");
            idx[D_8009F8E0_arr] = 1;
            __asm__ volatile("" ::: "memory");
            if (gPlayer1MenuReadyFlag == 0 || gPlayer2MenuReadyFlag == 0) {
                return;
            }
            base = gTwoPlayerMissionSelectMenuItems;
            gPlayer1MenuReadyFlag = 0;
            gPlayer2MenuReadyFlag = 0;
            *(s16 *)(base + 0x32) = 0x64;
            *(s16 *)(base + 0x86) = 0x64;
            if (mode->split == 0) {
                mode->unk98 = gPlayerSelection;
                gActivateMenuPanel(&gCrimeWarHubMenuDescriptor, 1);
            } else {
                gActivateMenuPanel(&gTwoPlayerPrecinctHubMenuDescriptor, 1);
            }
        } else {
            if (mode->split == 0) {
                gActivateMenuPanel(&gCrimeWarHubMenuDescriptor, 1);
            } else {
                gActivateMenuPanel(&gOnePlayerPrecinctHubMenuDescriptor, 1);
            }
        }
    } else {
        register s8 *tbl __asm__("$2");
        register s32 player __asm__("$4");
        tbl = gFontBankTable;
        player = gActivePlayerIndex;
        if (tbl[player] != a4) {
            register s32 off __asm__("$3") = player << 2;
            *(s32 *)((u8 *)D_8009F8E0_arr + off) = 0;
            if (player == 1) {
                gMenuPreviewScaleP2[0] = 0x64;
            } else {
                gMenuPreviewScaleP1[0] = 0x64;
            }
            player = gActivePlayerIndex;
        }
        if (D_8009F8E0_arr[player] != 0) {
            if (player == 1) {
                gMenuPreviewScaleP2[0] = 0x6E;
            } else {
                gMenuPreviewScaleP1[0] = 0x6E;
            }
        }
    }
}
