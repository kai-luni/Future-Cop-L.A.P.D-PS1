// Categories: Graphics > Sprites > Atlas | Graphics > GPU Primitives
#include "game/types.h"

extern void *gDrawAtlasSprite(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32) __asm__("func_80082B0C");
void *gEmitAtlasSpriteSimple(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s8 arg7, s8 arg8) __asm__("func_80082AB0");


/**
 * @brief Convenience form of the atlas sprite emitter: forwards the nine
 *        arguments and repeats arg6 into the three override slots.
 *
 * @param arg0..arg6 Forwarded unchanged (atlas index and placement).
 * @param arg7 First byte-sized override.
 * @param arg8 Second byte-sized override.
 * @return The callee's result (sprite packet pointer).
 *
 * @par Confirmed
 * return func_80082B0C(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg6,
 * arg6, arg7, arg8) - slots 7..9 of the 11-argument emitter all receive
 * arg6, i.e. one shared size/UV default.
 *
 * @par Callers
 * func_8007B460, func_8007C0F8, func_8007CE04, func_8007D7CC,
 * func_8007E67C, func_80080038.
 *
 * @par Environment path
 * Thin wrapper on the 2D sprite path: forwards to func_80082B0C, which emits
 * a sprite from the texture atlas table, repeating arg6 into its three
 * byte-sized colour or override slots. Callers are the menu and HUD screens
 * func_80078DD0, func_8007B460, func_8007C0F8, func_8007CE04, func_8007D7CC,
 * func_8007E67C, func_80080038 and others.
 */
void *gEmitAtlasSpriteSimple(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s8 arg7, s8 arg8)
{
    return gDrawAtlasSprite(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg6, arg6, arg7, arg8);
}
