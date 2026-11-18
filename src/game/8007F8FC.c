// Categories: Resources > Save Data | Graphics > Frame State
#include "game/types.h"

extern u8 gUiHoldFlag __asm__("D_8009F8BD");
extern u8 gUiInteractionEnabled __asm__("D_8009F8BE");
extern u8 *gSaveProfileImage __asm__("D_8009F92C");

extern void gArmProfileTransfer(void) __asm__("func_8007FA88");
extern void gResetProfileTransferFlags(void) __asm__("func_8007F8E0");
extern void func_800776B8(void);

/**
 * @brief Screen-init callback of the profile save/load screen.
 *
 * Registered at +0x10 of the screen descriptor D_800A36B8, next to the update
 * callback func_8007FACC (+0x14) and the draw callback func_80080038 (+0x18). It
 * arms the transfer, clears the profile page byte and both flags, zeroes the
 * 16-bit field +0x942 and the byte +0x944 of the profile context and finally
 * resets the front-end list cursor.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_8009F92C - profile context (0x948 bytes, allocated by func_8007F9CC).
 * D_8009F8BD/D_8009F8BE - hold and interaction flags, both cleared here.
 *
 * @par Calls
 * func_8007FA88 - arms the transfer (never the movie player).
 * func_8007F8E0 - clears the page byte and the two flags.
 * func_800776B8 - resets the front-end list: cursor 0, list D_8009B724, count 8.
 *
 * @par Callers
 * Unknown: no direct call. The address is stored at D_800A36B8+0x10, so it is
 * reached through the front-end screen table.
 */
void func_8007F8FC(void)
{
    u8 *p;

    gArmProfileTransfer();
    gResetProfileTransferFlags();
    p = gSaveProfileImage;
    gUiHoldFlag = 0;
    gUiInteractionEnabled = 0;
    *(u16 *)(p + 0x942) = 0;
    *(u8 *)(p + 0x944) = 0;
    func_800776B8();
}
