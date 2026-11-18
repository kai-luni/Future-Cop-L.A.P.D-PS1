// Categories: Resources > Save Data | Graphics > Frame State
#include "game/types.h"

extern u8 *gSaveProfileImage __asm__("D_8009F92C");
extern u8 gUiHoldFlag __asm__("D_8009F8BD");
extern u8 gUiInteractionEnabled __asm__("D_8009F8BE");
extern u8 D_800A36BD;
extern void *gGameSession __asm__("D_8009FA94");

extern void func_8007FA88(void);
extern void func_800776B8(void);
extern void func_8007F02C(void);

/**
 * @brief Clears the profile page byte and both front-end input flags.
 *
 * Writes D_8009F92C[0x921] (the page byte of the 0x948-byte profile context
 * allocated by func_8007F9CC), gUiHoldFlag and gUiInteractionEnabled. The two
 * flags are the hold/interaction pair that func_8007FACC raises while a card
 * transfer owns the screen, so this is the reset that lets the next
 * func_8007FA88 / func_8007FACC round start clean.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_8009F92C (profile context), D_8009F8BD/D_8009F8BE (hold and interaction
 * flags), D_800A36BD (declared, not referenced by the generated code).
 *
 * @par Callers
 * func_80076A2C (boot, after arming the transfer) and func_8007F8FC.
 */
void gResetProfileTransferFlags(void) __asm__("func_8007F8E0");

void gResetProfileTransferFlags(void)
{
    gSaveProfileImage[0x921] = 0;
    gUiHoldFlag = 0;
    gUiInteractionEnabled = 0;
}
