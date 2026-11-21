// Categories: Graphics > HUD > Layout | Game Engine > Front-End Configuration
#include "game/types.h"

extern void gDrawAlignedMultilineText(u8 *text, s32 x, s32 y, s32 color, s32 textId,
                           s32 alignX, s32 alignY, s32 font) __asm__("func_800788F0");
extern void func_80078B34(s32 arg0, s32 arg1, s32 arg2);

/**
 * @brief Draws a three-line menu page and queues two display requests.
 *
 * @param arg0 Returned unchanged (the widget result convention).
 * @param arg1 0 draws the page; any other value skips the draw.
 * @return arg0.
 *
 * @par Confirmed
 * With arg1 == 0: three centred texts (ids 0x42E, 0x438, 0x442) at
 * (0xA0, 0x38/0x6C/0xA0) in colour 0x10, alignX 4, size 8, font 1, then
 * func_80078B34(0x456, 0, 0) and func_80078B34(0x44C, 1, 0) queue the
 * display ids into the two slots.
 *
 * @par Likely role
 * A front-end selection page (three options plus two overlay ids), reached
 * through the screen callback block like func_8007E67C.
 *
 * @par Unknown
 * The labels behind text ids 0x42E/0x438/0x442.
 *
 * @par Callers
 * No direct callers in the call graph. func_80077AD4 loads the address
 * (lui/addiu at 0x80077B88) into the screen callback block, so the screen
 * dispatcher reaches it indirectly.
 */
s32 gDrawModeHeadingPage(s32 arg0, s32 arg1) __asm__("func_8007A2C4");

s32 gDrawModeHeadingPage(s32 arg0, s32 arg1)
{
    s32 ret;

    ret = arg0;
    if (arg1 == 0) {
        gDrawAlignedMultilineText(0, 0xA0, 0x38, 0x10, 0x42E, 4, 8, 1);
        gDrawAlignedMultilineText(0, 0xA0, 0x6C, 0x10, 0x438, 4, 8, 1);
        gDrawAlignedMultilineText(0, 0xA0, 0xA0, 0x10, 0x442, 4, 8, 1);
        func_80078B34(0x456, 0, 0);
        func_80078B34(0x44C, 1, 0);
    }
    return ret;
}
