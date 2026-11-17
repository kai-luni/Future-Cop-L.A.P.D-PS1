// Categories: Graphics > Fade Layers | Game Engine > Frame
#include "game/types.h"

extern s16 D_8009F65C[];
extern s16 D_8009FA44[];
extern s32 D_8009F9FC;
extern s32 gScreenState __asm__("D_8009FA00");

extern void func_80026E9C(s32 arg0, s32 arg1, s32 arg2, s32 arg3);

/**
 * @brief Per-player screen fade tick: counts the fade-in timer up to the
 *        mode switch, or drains the fade-out rectangle.
 *
 * @param arg0 Player index.
 * @param arg1 Non-zero selects the brighter fade colour 0x140 over 0xA0.
 * @return Nothing.
 *
 * @par Confirmed
 * While D_8009F65C[arg0] is non-zero it gains D_8009F9FC per call; at 0x384
 * the screen mode D_8009FA00 becomes 6. Otherwise D_8009FA44[arg0] drains
 * by D_8009F9FC + D_8009F9FC / 2 (clamped at 0); a positive remainder is
 * capped at 0xFF and drawn as a full-screen rectangle through
 * func_80026E9C(v, v, v, colour).
 *
 * @par Likely role
 * The transition fade of the frame driver func_80027510: fade-in runs the
 * timer into the gameplay mode 6, fade-out shrinks a coloured rectangle
 * over the screen at 1.5x the frame step.
 *
 * @par Unknown
 * What screen mode 6 represents (likely the mission-start state).
 *
 * @par Callers
 * func_80027510.
 */
void func_80026F68(s32 arg0, s32 arg1)
{
    s32 temp;

    if (D_8009F65C[arg0] != 0) {
        if (D_8009F65C[arg0] >= 0x384) {
            gScreenState = 6;
        } else {
            D_8009F65C[arg0] += D_8009F9FC;
        }
    } else if (D_8009FA44[arg0] != 0) {
        temp = D_8009FA44[arg0] - (D_8009F9FC + D_8009F9FC / 2);
        if (temp <= 0) {
            temp = 0;
        }
        D_8009FA44[arg0] = temp;
        if (temp > 0) {
            if (temp >= 0x100) {
                temp = 0xFF;
            }
            func_80026E9C(temp, temp, temp, (arg1 & 0xFF) ? 0x140 : 0xA0);
        }
    }
}
