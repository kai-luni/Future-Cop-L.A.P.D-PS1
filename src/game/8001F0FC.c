// Categories: Graphics > Sprite/Asset Subsystem | Game Engine > Callbacks
#include "game/types.h"

extern u8 gFrameCallbackEnabled __asm__("D_8009FA20");
extern s32 gScreenModeHook __asm__("D_8009F99C");
s32 gSetScreenModeHook(s32 callback) __asm__("func_8001F0FC");


/**
 * @brief Registers the subsystem callback, once only.
 *
 * @param callback Callback address stored in D_8009F99C.
 * @return 0 on success, 3 when the subsystem is disabled (D_8009FA20 == 0)
 *         or a callback is already registered.
 *
 * @par Confirmed
 * Two guards, then D_8009F99C = callback. No unregister path exists in
 * this body.
 *
 * @par Callers
 * func_8002333C (sprite/asset subsystem).
 *
 * @par Environment path
 * Registers the screen-mode/subsystem frame callback pointer once if not
 * already set and returns registration status (called by
 * func_8002333C:48,74).
 */
s32 gSetScreenModeHook(s32 callback)
{
    if (gFrameCallbackEnabled == 0) {
        return 3;
    }
    if (gScreenModeHook != 0) {
        return 3;
    }
    gScreenModeHook = callback;
    return 0;
}
