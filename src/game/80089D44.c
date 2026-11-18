// Categories: Graphics > Image Display | Resources > Deferred Initialisation
#include "game/types.h"

extern u8 func_80087D3C(void);
extern u8 gOpenVideoLayer(s32 arg0) __asm__("func_80087EA0");
extern void gPlayVideoFile(s32 arg0, s32 arg1, s32 arg2) __asm__("func_80088120");
extern void gCloseVideoLayer(void) __asm__("func_80087D48");
extern s32 func_8008A5A8(void);

/**
 * @brief Draws a named image, initialising the image layer on first use.
 *
 * @param arg0 Pointer to the image name/path (e.g. "Ea_logo.fsv" or a buffer
 *             holding "Movies\\Intro.wve").
 * @param arg1 Mode passed to the initialiser, e.g. 4 or 0x26; selects the
 *             pixel format/size class of the source.
 * @param arg2 First draw coordinate (x).
 * @param arg3 Second draw coordinate (y); the front-end passes 0 or 0x18.
 * @param arg4 Unused. Callers pass a width (0x140 / 0x100) but the body never
 *             reads it - confirmed by the byte-exact rebuild, so the retail
 *             function ignores it as well.
 * @param arg5 Unused, same as arg4 (height: 0xF0 / 0xC0 / 0x90).
 *
 * @return Nothing.
 *
 * @par Confirmed flow
 * 1. func_80087D3C() reports whether the image layer is already open.
 * 2. If not, gOpenVideoLayer(arg1) opens/initialises it for that mode; a zero
 *    result means "cannot draw" and the function returns without drawing.
 * 3. flag records that this call opened the layer.
 * 4. gPlayVideoFile(arg0, arg2, arg3) performs the actual draw.
 * 5. If this call opened the layer it is closed again with gCloseVideoLayer(),
 *    so a single shot draws and tears down, while a layer that was already
 *    open stays open.
 *
 * @par Globals
 * None directly; the callees own the image-layer state (D_8009F4xx and
 * D_8009F9xx).
 *
 * @par Callers
 * func_80076A2C (EA logo and intro frame during boot), func_80082DDC (mission
 * briefing artwork, modes 1, 0 and 2, coordinates 0x20/0x31 and 0/0x18).
 */
void gDrawImageResource(s32 arg0, s32 arg1, s32 arg2, s32 arg3) __asm__("func_80089D44");

void gDrawImageResource(s32 arg0, s32 arg1, s32 arg2, s32 arg3)
{
    u8 flag;

    flag = 0;
    if (!(func_80087D3C() & 0xFF)) {
        if (!(gOpenVideoLayer(arg1) & 0xFF)) {
            return;
        }
        flag = 1;
    }
    gPlayVideoFile(arg0, arg2, arg3);
    if (flag & 0xFF) {
        gCloseVideoLayer();
    }
}
