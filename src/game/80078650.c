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

extern Menu *D_8009F468;
extern u8 D_8009F875;
extern u8 D_8009F876;
extern u8 gPlayerMenuDepth __asm__("D_8009F8C0");
extern u8 D_8009F8C1;
extern u8 D_8009F8CC;
extern void func_800785F8(void);
extern void func_800787D8(s32 a, s32 b, s32 c);
extern void func_800788B4(void);
extern void D_8007B3F4(void);

/**
 * @brief Activates a menu screen: optionally links the previous screen as parent, switches the font bank, resets input/highlight state, runs the screen's init callback and selects its first enabled item.
 *
 * @param arg0 Menu block to activate (becomes D_8009F468).
 * @param arg1 Nonzero = push: store the old screen in arg0->unk24 and run the font-bank switch first.
 *
 * @par Globals
 * D_8009F468 - active menu pointer, replaced. D_8009F8C0/D_8009F8C1 - item highlight arrays, cleared; D_8009F8CC - one-shot draw-setup gate, cleared so func_80077990 reruns. D_8009F875/D_8009F876 - cached copies of cursor unk03/unk04 when unk03 != -1.
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
void func_80078650(Menu *arg0, s32 arg1)
{
    Menu *m;

    if (arg1 != 0) {
        arg0->unk24 = D_8009F468;
    }
    D_8009F468 = arg0;
    if (arg1 != 0) {
        func_800788B4();
    }
    gPlayerMenuDepth = 0;
    D_8009F8C1 = 0;
    func_800785F8();
    D_8009F8CC = 0;
    if (D_8009F468->unk10 != 0) {
        D_8009F468->unk10();
    }
    func_800787D8(0, 1, 0);
    if ((u8)(D_8009F468->unk05 - 2) < 2) {
        func_800787D8(0, 1, 1);
    }
    if ((void *)D_8009F468->unk10 == (void *)D_8007B3F4) {
        D_8009F468->unk10();
    }
    m = D_8009F468;
    if (m->unk03 != -1) {
        D_8009F875 = m->unk03;
        D_8009F876 = m->unk04;
    }
}
