// Categories: Game Engine > Options Menu | Graphics > HUD > Gauge | Platform > Sound
#include "game/types.h"

extern s8 *gFontBankTable __asm__("D_8009F468");
extern s32 gPaletteCycleCounter __asm__("D_8009FA60");
extern void func_800788F0(s32, s32, s32, s32, s32, s32, s32, s32);
extern s32 func_8004522C(void);
extern s32 func_80045288(void);
extern s32 func_800452C8(void);
extern void func_8007CE04(s32, s32, s32, s32);
extern void func_80078B34(s32, s32, s32);

/**
 * @brief Draws the options screen: three labels, three volume gauges and the
 *        mode-dependent display requests.
 *
 * @param arg0 Must be 0 (the draw action); any other value returns.
 * @return Nothing.
 *
 * @par Confirmed
 * Texts 0xFA0/0x1004/0x1068 at (0xA0, 0x3C/0x70/0x8D), colour 0x10, font 2.
 * Three gauges via func_8007CE04(0x14, y, value, 0x7F) at y 0x4B/0x57/0x62
 * with the values from func_8004522C/func_80045288/func_800452C8 (the three
 * volume getters). The display request in slot 0 follows *D_8009F468:
 * modes 1/2/3 -> 0x10F4, 4 -> 0x10FE, 5 -> 0x10EA, 0 -> 0x10D6; slot 1 gets
 * 0x10CC or 0x44C depending on palette bit 9 of D_8009FA60.
 *
 * @par Likely role
 * The audio/options page: the three gauges are the music/SE/voice volumes
 * (limit 0x7F matches the sound clamp), the mode byte selects the prompt
 * line.
 *
 * @par Callers
 * No direct callers in the call graph. func_80077AD4 stores the address in
 * the screen callback block D_800A3778.unk18, so the screen dispatcher
 * reaches it indirectly.
 */
void func_8007CFA0(s32 arg0)
{
    if (arg0 != 0) return;
    func_800788F0(0, 0xA0, 0x3C, 0x10, 0xFA0, 4, 8, 2);
    func_800788F0(0, 0xA0, 0x70, 0x10, 0x1004, 4, 8, 2);
    func_800788F0(0, 0xA0, 0x8D, 0x10, 0x1068, 4, 8, 2);
    func_8007CE04(0x14, 0x4B, func_8004522C(), 0x7F);
    func_8007CE04(0x14, 0x57, func_80045288(), 0x7F);
    func_8007CE04(0x14, 0x62, func_800452C8(), 0x7F);
    switch (*gFontBankTable) {
    case 1:
    case 2:
    case 3: func_80078B34(0x10F4, 0, 0); break;
    case 4: func_80078B34(0x10FE, 0, 0); break;
    case 5: func_80078B34(0x10EA, 0, 0); break;
    case 0: func_80078B34(0x10D6, 0, 0); break;
    }
    func_80078B34(((gPaletteCycleCounter >> 9) & 1) ? 0x10CC : 0x44C, 1, 0);
}
