// Categories: Game Engine > Menu System | Graphics > Text
#include "game/types.h"

typedef struct Menu {
    u8 pad00[3];
    s8 unk03;
    u8 unk04;
    u8 unk05;
    u8 pad06[0xA];
    void (*unk10)(void);
    u8 pad14[0x10];
    struct Menu *unk24;
} Menu;

extern Menu *gFontBankTable __asm__("D_8009F468");
extern u8 D_8009F875;
extern u8 gScreenUploadFlag __asm__("D_8009F876");
extern u8 gPlayerMenuDepth __asm__("D_8009F8C0");
extern u8 D_8009F8C1;
extern u8 gMenuSetupCalled __asm__("D_8009F8CC");
extern void gResetMenuDirectionHoldTicks(void) __asm__("func_800785F8");
extern void gSelectEnabledMenuItem(s32 a, s32 b, s32 c) __asm__("func_800787D8");
extern void gSyncMenuFontBank(void) __asm__("func_800788B4");
extern void gEnterGameHubMenuCallback(void) __asm__("D_8007B3F4");
void gActivateMenuPanel(Menu *arg0, s32 arg1) __asm__("func_80078650");


/**
 * @brief Activates a menu, optionally links its parent, resets input, runs the
 * on-enter callback and selects its first enabled item.
 *
 * The screen registered at D_800A3808 has seven items in D_8009BBF8.
 * On the default boot path, func_800782EC first activates the title
 * descriptor; func_800778E4 replaces it with D_800A3808 after its timeout.
 * Here activation selects enabled items and invokes the descriptor's init
 * callback. A push saves the previous screen as parent; replace does not.
 *
 * @param arg0 Menu block to activate (becomes D_8009F468).
 * @param arg1 Nonzero = push: store the old screen in arg0->unk24 and run the font-bank switch first.
 *
 * @par Globals
 * D_8009F468 - active menu pointer, replaced. D_8009F8C0/D_8009F8C1 - per-player menu-depth bytes, cleared; D_8009F8CC - one-shot draw-setup gate, cleared so func_80077990 reruns. D_8009F875/D_8009F876 - cached copies of cursor unk03/unk04 when unk03 != -1.
 *
 * @par Control flow
 * Straight-line with guards: push-only steps, null init-callback check, a second item selection when unk05 is 2 or 3, and a second init call when the callback equals func_8007B3F4.
 *
 * @par Calls
 * func_800788B4 - font bank switch from menu+8 when pushing. func_800785F8 - clears the four input-state word pairs. func_800787D8 - selects the first enabled group-0 item (and group-1 when unk05 is 2/3). The unk10 init callback runs indirectly; func_8007B3F4 (main-menu init) is invoked a second time by pointer comparison.
 *
 * @par Side effects
 * Rewrites the active-menu pointer, parent link, font bank, input/highlight state and cursor caches; runs screen init code.
 *
 * @par Callers
 * Confirmed: func_800778E4 and func_80078210 (replace, arg1=0), func_8007A5F0 (pushes panels D_800A3868/898/8C8), func_8007B854 (level screen push), func_8007FB04 and func_8008125C (push D_800A3718, arg1=1).
 */
void gActivateMenuPanel(Menu *arg0, s32 arg1)
{
    Menu *m;

    if (arg1 != 0) {
        arg0->unk24 = gFontBankTable;
    }
    gFontBankTable = arg0;
    if (arg1 != 0) {
        gSyncMenuFontBank();
    }
    gPlayerMenuDepth = 0;
    D_8009F8C1 = 0;
    gResetMenuDirectionHoldTicks();
    gMenuSetupCalled = 0;
    if (gFontBankTable->unk10 != 0) {
        gFontBankTable->unk10();
    }
    gSelectEnabledMenuItem(0, 1, 0);
    if ((u8)(gFontBankTable->unk05 - 2) < 2) {
        gSelectEnabledMenuItem(0, 1, 1);
    }
    if ((void *)gFontBankTable->unk10 == (void *)gEnterGameHubMenuCallback) {
        gFontBankTable->unk10();
    }
    m = gFontBankTable;
    if (m->unk03 != -1) {
        D_8009F875 = m->unk03;
        gScreenUploadFlag = m->unk04;
    }
}
