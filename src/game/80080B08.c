// Categories: Resources > Movie Playback | Game Engine > State Machine
#include "game/types.h"

extern u8 *gSaveProfileImage __asm__("D_8009F92C");
extern s32 D_8009F464;

extern void func_8007FB04(void);

/**
 * @brief Pumps the movie player one step and finishes the boot wait when the
 *        stream ends.
 *
 * @par Confirmed flow
 * func_8007FB04() decodes one step, then the state byte D_8009F92C[0x200] is
 * examined:
 * - 1 or 6: nothing (still pre-rolling / playing).
 * - 2: the movie ended normally, but only if D_8009F92C[0x201] == 1;
 *   otherwise D_8009F464 = 3.
 * - anything else: D_8009F464 = 3.
 * Since func_8007F8CC() reports "running" only while D_8009F464 == 2, the
 * caller's loop ends once this function has written 3.
 *
 * @return Nothing.
 *
 * @par Globals
 * D_8009F92C (movie context), D_8009F464 (player state).
 *
 * @par Callers
 * func_80076A2C only, inside the intro wait loop.
 */
void func_80080B08(void)
{
    u8 s;

    func_8007FB04();
    s = gSaveProfileImage[0x200];
    switch (s) {
    case 1:
    case 6:
        break;
    case 2:
        if (gSaveProfileImage[0x201] != 1) {
            D_8009F464 = 3;
        }
        break;
    default:
        D_8009F464 = 3;
        break;
    }
}
