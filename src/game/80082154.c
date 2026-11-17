// Categories: Game Engine > Front End | Platform > Memory Card
#include "game/types.h"

extern u8 *gSaveProfileImage __asm__("D_8009F92C");
extern s32 gActivePlayerIndex __asm__("D_8009F8B8");
extern u8 gPlayerMenuDepth[] __asm__("D_8009F8C0");

extern void func_8008204C(void);
extern s32 func_80078740(void);

/**
 * @brief Handles confirmation actions on the memory-card prompt menu panel.
 *
 * Acts as item 0's callback in D_8009B9E4 for menu D_800A3718. When confirmed on
 * item 0 (arg0 == 3, arg4 == 0), it either waits for an active card transfer
 * (state word +0x200 == 3) or pops back to the parent panel (state byte +0x200 == 5).
 *
 * @param arg0 Menu item action code (only 3 = confirm is handled).
 * @param arg1 Unused widget X parameter.
 * @param arg2 Unused widget Y parameter.
 * @param arg3 Unused widget colour parameter.
 * @param arg4 Selected item index (only 0 triggers behaviour).
 *
 * @par Globals
 * D_8009F92C - card menu context (+0x200 state, +0x942 timer); D_8009F8B8 - active port index; D_8009F8C0 - per-port sub-depth byte array.
 *
 * @par Control flow
 * Guards on arg0 == 3 and arg4 == 0, branching on *(u16 *)(D_8009F92C + 0x200) == 3 versus D_8009F92C[0x200] == 5.
 *
 * @par Calls
 * func_8008204C - waits for pending card transfer when +0x200 == 3; func_80078740 - pops parent panel when +0x200 == 5.
 *
 * @par Side effects
 * Updates D_8009F8C0[D_8009F8B8] = 1 and clears *(u16 *)(D_8009F92C + 0x942) = 0 in the state-5 branch.
 *
 * @par Callers
 * Confirmed: no direct calls; address placed at 0x8009B9E8 in D_8009B9E4 (bound to D_800A3718 in func_80077AD4 at 0x80077F38) and dispatched via item->handler in func_80078DD0 (0x80079518).
 */
void func_80082154(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4)
{
    if (arg0 == 3) {
        if (*(u16 *)(gSaveProfileImage + 0x200) == 3) {
            if (arg4 == 0) {
                func_8008204C();
            }
        } else if (gSaveProfileImage[0x200] == 5) {
            if (arg4 == 0) {
                register s32 idx __asm__("$2");
                register u8 *base __asm__("$3");
                func_80078740();
                idx = gActivePlayerIndex;
                __asm__ volatile ("" : : "r" (idx));
                base = gPlayerMenuDepth;
                *(u8 *)(idx + base) = 1;
                *(u16 *)(gSaveProfileImage + 0x942) = 0;
            }
        }
    }
}
