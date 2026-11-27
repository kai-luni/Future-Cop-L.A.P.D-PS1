// Categories: Graphics > Fade Layers | Game Engine > Frame
#include "game/types.h"

extern s16 gTeamFlag[] __asm__("D_8009F65C");
extern s16 gPlayerFadeTimers[] __asm__("D_8009FA44");
extern s32 gFrameDelta __asm__("D_8009F9FC");
extern s32 gScreenState __asm__("D_8009FA00");

extern void gEmitScreenFadeBar(s32 arg0, s32 arg1, s32 arg2, s32 arg3) __asm__("func_80026E9C");

void gTickPlayerTransitionFade(s32 arg0, s32 arg1) __asm__("func_80026F68");

/**
 * @brief Advances a player's retirement transition or drains their screen fade timer.
 *
 * @param arg0 Player index.
 * @param arg1 Non-zero selects the brighter fade colour 0x140 over 0xA0.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Tests the signed retirement halfword and either adds the frame delta or requests state 6 at
 * 0x384. Otherwise subtracts delta plus half delta from the fade timer, clamps it at zero and
 * draws a positive remainder capped at 0xFF with color selected by a1's low byte.
 *
 * @par Globals
 * Reads gFrameDelta, updates gTeamFlag and gPlayerFadeTimers and may write gScreenState.
 *
 * @par Callers
 * Direct calls from gRunSoloSessionFrame (func_800271A4), gRunSessionFrame (func_80027510).
 *
 * @par Role and path
 * Both session frame paths tick the screen transition for each player.
 *
 * @par Unknown
 * The meaning of screen state 6 comes from the screen dispatcher.
 */
void gTickPlayerTransitionFade(s32 arg0, s32 arg1)
{
    s32 temp;

    if (gTeamFlag[arg0] != 0) {
        if (gTeamFlag[arg0] >= 0x384) {
            gScreenState = 6;
        } else {
            gTeamFlag[arg0] += gFrameDelta;
        }
    } else if (gPlayerFadeTimers[arg0] != 0) {
        temp = gPlayerFadeTimers[arg0] - (gFrameDelta + gFrameDelta / 2);
        if (temp <= 0) {
            temp = 0;
        }
        gPlayerFadeTimers[arg0] = temp;
        if (temp > 0) {
            if (temp >= 0x100) {
                temp = 0xFF;
            }
            gEmitScreenFadeBar(temp, temp, temp, (arg1 & 0xFF) ? 0x140 : 0xA0);
        }
    }
}
