// Categories: Resources > Movie Playback | Graphics > Frame State
#include "game/types.h"

extern u8 *gSaveProfileImage __asm__("D_8009F92C");
extern u8 gUiHoldFlag __asm__("D_8009F8BD");
extern u8 D_8009F8BE;
extern u8 D_800A36BD;
extern void *gGameSession __asm__("D_8009FA94");

extern void func_8007FA88(void);
extern void func_800776B8(void);
extern void func_8007F02C(void);

/**
 * @brief Clears the frame/movie state flags of the movie context.
 *
 * Writes three bytes: D_8009F92C[0x921] (a field of the 0x948-byte movie
 * context allocated by func_8007F9CC), D_8009F8BD and D_8009F8BE. The last two
 * are the "pending" flags that func_8007FACC sets and consumes, so this is the
 * reset that makes the next func_8007FA88/8007FACC round start from a clean
 * state.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_8009F92C (movie context), D_8009F8BD/D_8009F8BE (pending flags),
 * D_800A36BD (declared, not referenced by the generated code).
 *
 * @par Callers
 * func_80076A2C (during boot, after the player is armed) and func_8007F8FC.
 */
void func_8007F8E0(void)
{
    gSaveProfileImage[0x921] = 0;
    gUiHoldFlag = 0;
    D_8009F8BE = 0;
}
