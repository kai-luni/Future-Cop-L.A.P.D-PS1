// Categories: Game Engine > Front End | Input > Menu Navigation
#include "game/types.h"

extern u8 *D_8009F468;
extern u8 D_8009F875;
extern u8 D_8009F876;
extern u8 gPlayerMenuDepth __asm__("D_8009F8C0");
extern u8 D_8009F8C1;
extern u8 D_8009F8CC;
extern void func_800785F8(void);
extern void func_800788B4(void);

/**
 * @brief Pops the current front-end menu panel back to its parent panel.
 *
 * Checks whether D_8009F468 has a parent link at +0x24; if so, runs the panel's
 * optional exit callback (+0x14), restores D_8009F468, resets input/focus globals,
 * syncs the layout index, and copies restored header bytes +0x03/+0x04 when valid.
 *
 * @param void No parameters.
 * @return 1 if a parent panel existed and was restored, 0 if D_8009F468+0x24 was null.
 *
 * @par Globals
 * D_8009F468 - active panel; D_8009F8C0, D_8009F8C1, D_8009F8CC - cleared bytes; D_8009F875, D_8009F876 - set from panel[3..4] if (s8)panel[3] != -1.
 * @par Control flow
 * Guards on *(D_8009F468+0x24) != 0; calls *(p+0x14) if non-null, swaps D_8009F468, calls func_800785F8 and func_800788B4, and conditionally copies panel[3..4].
 * @par Calls
 * func_800785F8 - clears UI hold arrays; func_800788B4 - syncs D_8009F8BC from panel[8]; indirect call to *(p+0x14).
 * @par Side effects
 * Updates D_8009F468, clears D_8009F8C0/D_8009F8C1/D_8009F8CC and UI arrays, may update D_8009F875/D_8009F876.
 * @par Callers
 * Confirmed: func_8007F1F8, func_8007FB04, func_80082154; func_80078DD0 (at 0x80079548, 0x80079630 in asm/us/main.s).
 */
s32 func_80078740(void)
{
    register s32 ret __asm__("$3");
    register u8 *p __asm__("$4");

    p = D_8009F468;
    ret = 0;
    if (*(u32 *)(p + 0x24) != 0) {
        if (*(void (**)(void))(p + 0x14) != 0) {
            (*(void (**)(void))(p + 0x14))();
        }
        D_8009F468 = *(u8 **)(D_8009F468 + 0x24);
        gPlayerMenuDepth = 0;
        D_8009F8C1 = 0;
        func_800785F8();
        D_8009F8CC = 0;
        func_800788B4();
        if ((s8)D_8009F468[3] != -1) {
            D_8009F875 = D_8009F468[3];
            D_8009F876 = D_8009F468[4];
        }
        ret = 1;
    }
    return ret;
}
