// Categories: Graphics > Text > Debug Numbers | Game Engine > Frame
#include "game/types.h"

extern s32 gFrameTickCounter __asm__("D_8009FA2C");
extern s32 gMoviePathPrefix __asm__("D_8009F3D0");
extern s32 gIdleDeadline __asm__("D_8009F870");
extern s8 gOverlayDelay __asm__("D_8009F874");
extern u8 gMovieClipPathFormat[] __asm__("D_8009F4AC");

extern void gSetAudioPlaybackEnabled(u8 enable) __asm__("func_80047690");
extern void gDrawImageResource(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) __asm__("func_80089D44");
extern s32 gSpillVarargsArguments(u8 *buffer, u8 *format, s32 value, s32 arg3) __asm__("func_8008CD30");
void gPlayMovieClip(s32 arg0, s32 arg1) __asm__("func_80082DDC");


/**
 * @brief Plays a named video resource in one of two layouts and accounts the
 *        elapsed time.
 *
 * @param arg0 Video resource name appended to the movie path prefix.
 * @param arg1 Layout: 1 selects the small layout, 0/2 the wide layout.
 * @return Nothing.
 *
 * @par Confirmed
 * Saves D_8009FA2C, disables via func_80047690(0), formats
 * func_8008CD30(buf, D_8009F4AC, D_8009F3D0, arg0) and draws through
 * gDrawImageResource: layout 1 at (0x26, 0x20, 0x31, 0x100, 0x90), layouts 0/2
 * at (4, 0, 0x18, 0x140, 0xC0). Re-enables, sets D_8009F874 = -1 and adds
 * the elapsed D_8009FA2C delta to D_8009F870.
 *
 * @par Confirmed resource path
 * D_8009F4AC is "%s%s" in the retail executable; D_8009F3D0 supplies the
 * movie path prefix. gDrawImageResource ultimately calls gPlayVideoFile.
 * Its width and height arguments are unused.
 *
 * @par Callers
 * func_80079F54, func_80079FE0, func_8007A0A4.
 */
void gPlayMovieClip(s32 arg0, s32 arg1)
{
    u8 buf[0x28];
    s32 start;

    start = gFrameTickCounter;
    gSetAudioPlaybackEnabled(0);
    switch (arg1) {
    case 1:
        gSpillVarargsArguments(buf, gMovieClipPathFormat, gMoviePathPrefix, arg0);
        gDrawImageResource((s32)buf, 0x26, 0x20, 0x31, 0x100, 0x90);
        break;
    case 0:
    case 2:
        gSpillVarargsArguments(buf, gMovieClipPathFormat, gMoviePathPrefix, arg0);
        gDrawImageResource((s32)buf, 4, 0, 0x18, 0x140, 0xC0);
        break;
    }
    gSetAudioPlaybackEnabled(1);
    gOverlayDelay = -1;
    gIdleDeadline += gFrameTickCounter - start;
}
