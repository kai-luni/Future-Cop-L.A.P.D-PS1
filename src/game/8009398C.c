// Categories: Platform > GPU | Graphics > Display Setup
#include "game/types.h"

extern s32 gBuildDrawAreaCommands(s16 arg0, s16 arg1) __asm__("func_80093F78");
extern s32 gPackDrawAreaBottomRight(s16 arg0, s16 arg1) __asm__("func_80094010");

/**
 * @brief Builds a "drawing area" primitive (GP0 0xE4) for a rectangle.
 *
 * @param obj Target packet; obj[3] receives the command length 2.
 * @param rect Source rectangle: x, y, w, h as four half-words.
 *
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * obj[4] = func_80093F78(x, y) (the top-left coordinate word) and
 * obj[8] = func_80094010(x + w - 1, y + h - 1) (the bottom-right one). Both
 * helpers build the packed 16-bit coordinate words.
 *
 * @par Callers
 * func_8001B4A8, once per frame when D_8009FA0C >= 2 or a full-screen frame is
 * requested - it sets the clip area before the list is submitted.
 */
void func_8009398C(u8 *obj, u8 *rect)
{
    *(u8 *)(obj + 3) = 2;
    *(s32 *)(obj + 4) = gBuildDrawAreaCommands(*(s16 *)(rect + 0), *(s16 *)(rect + 2));
    *(s32 *)(obj + 8) = gPackDrawAreaBottomRight(
        (s16)(*(u16 *)(rect + 0) + *(u16 *)(rect + 4) - 1),
        (s16)(*(u16 *)(rect + 2) + *(u16 *)(rect + 6) - 1));
}
