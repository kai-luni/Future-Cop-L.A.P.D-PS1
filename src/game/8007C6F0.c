// Categories: Game Engine > Front-End Configuration | Graphics > HUD
#include "game/types.h"

extern volatile s32 gPaletteCycleCounter __asm__("D_8009FA60");
extern u8 D_8009F8D8[];

extern void func_80082EF0(s32 x, s32 y, s32 w, s32 h, u32 link, s32 r, s32 g, s32 b);
extern void func_8007C0F8(s32 arg0);
extern s32 func_8001EF34(s32 port);
extern u8 func_8001EAEC(s32 arg0);
extern void func_80078B34(s32 arg0, s32 arg1, s32 arg2);

/**
 * @brief Profile-slot screen pass: black background, the slot list, and two
 *        display requests chosen by the palette phase and the pad state.
 *
 * @param slot The slot to draw; the body runs only when it matches the
 *        current target (register comparison at entry).
 * @return Nothing.
 *
 * @par Confirmed
 * func_80082EF0(0x20, 0x30, 0x100, 0x90, 0xBB8, 0, 0, 0) draws the black
 * box, func_8007C0F8(slot) draws the slot list. The display ids start at
 * (0x1324, 0x6AE); when palette bit 9 of D_8009FA60 is clear they become
 * (0x132E, 0x10CC), and with bit 10 set plus both pad checks
 * (func_8001EF34(slot == 1), func_8001EAEC(slot == 1)) true, the second id
 * becomes 0x1338 or 0x133D depending on D_8009F8D8[slot]. Both ids are
 * queued via func_80078B34 into slots 0/1.
 *
 * @par Likely role
 * The profile selection screen: the blinking palette bits drive the
 * "press start" prompt variants, the pad checks detect the connected
 * controller of the active player.
 *
 * @par Unknown
 * The labels behind ids 0x1324..0x133D.
 *
 * @par Callers
 * No direct callers in the call graph and no address reference in the
 * sources; likely reached through a runtime-built screen table
 * (unconfirmed).
 */
void func_8007C6F0(s32 slot)
{
    register s32 target __asm__("$2");
    s32 a;
    s32 b;
    s32 t;

    if (slot != target) {
        return;
    }

    func_80082EF0(0x20, 0x30, 0x100, 0x90, 0xBB8, 0, 0, 0);
    func_8007C0F8(slot);

    a = 0x6AE;
    b = 0x1324;
    t = (slot == 1);
    if (((gPaletteCycleCounter >> 9) & 1) == 0) {
        b = 0x132E;
        a = 0x10CC;
        if (((gPaletteCycleCounter >> 10) & 1) != 0) {
            if (func_8001EF34(t) && func_8001EAEC(t)) {
                __asm__("");
                if (D_8009F8D8[slot] != 0) {
                    a = 0x1338;
                } else {
                    a = 0x133D;
                }
            }
        }
    }

    func_80078B34(b, 0, 0);
    func_80078B34(a, 1, 0);
}
