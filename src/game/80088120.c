// Categories: Graphics > Video Decode Output | Graphics > Frame Conversion
#include "game/types.h"

typedef struct {
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect88120;

typedef struct {
    s32 data[6];
} Params88120;

extern u8 gLayerOpenResult __asm__("D_8009F504");
extern u32 gMovieDisplayMode __asm__("D_8009F4DC");
extern void *gMovieStreamBuffer __asm__("D_8009F4EC");
extern void *gMovieAudioBlock __asm__("D_8009F4F0");
extern void *gMovieSpuBufferA __asm__("D_8009F4F4");
extern void *gMovieSpuBufferB __asm__("D_8009F4F8");
extern s32 D_8009F960;
extern s32 D_8009F964;
extern s32 gLayerModeBits __asm__("D_8009F968");
extern s32 gFileSearchGroup __asm__("D_8009F238");
extern Params88120 gFileTable __asm__("D_800A6E20");

extern s32 func_8001D5E4(s32 arg0, s32 arg1);
extern void func_8001D3A8(s32 handle);
extern void func_80092BF4(s32 arg0);
extern u8 func_8001C034(void);
extern void *func_8008799C(s32 size);
extern void func_800879BC(void *ptr);
extern void func_80092C8C(s32 arg0);
extern void func_80092F98(Rect88120 *rect, void *buf);
extern void func_80092F38(Rect88120 *rect, void *buf);
extern void func_80092FF8(Rect88120 *rect, s32 x, s32 y);
extern void func_800858F4(s32 value);
extern void func_80085900(s32 value);
extern void func_80091270(s32 arg0);
extern void gMoviePresentFrame(void) __asm__("func_80085914");
extern void func_80085AC4(u32 arg0, u32 arg1);
extern void gMovieAudioClockDisable(void) __asm__("func_80086F30");
extern s32 gMovieStreamReaderInit(void *arg0, void (*callback)(void), Params88120 *params, s32 arg3, s32 arg4) __asm__("func_80089700");
extern s32 func_80087824(void *arg0, s32 arg1, void *arg2, s32 arg3, void *arg4, s32 arg5, s32 arg6);
extern void gMovieSetAudioMode(u32 value) __asm__("func_80086F08");
extern s32 gMovieStreamDemux(s32 arg0, s32 arg1, s32 arg2) __asm__("func_800879DC");
extern s32 gMovieAudioBusyCount(void) __asm__("func_80086FA8");
extern void func_80087750(void);
extern void gMovieStreamReaderClose(void) __asm__("func_80089874");
extern s32 func_800858E0(void);
extern void func_8001C088(u8 mode, u8 enable);
extern s32 func_8001BFE4(void);

/*
 * Opens and processes a video resource, optionally converting
 * the current framebuffer between 15-bit RGB555 and 24-bit RGB888.
 *
 * When the relevant display-mode flags are set, the function
 * copies 320x256 framebuffer data into a 24-bit representation,
 * optionally clears a central 256x144 region, and prepares
 * the display for video playback.
 *
 * Initializes video-processing resources, invokes the main
 * processing routine, performs cleanup, and closes the resource.
 *
 * Afterwards, optionally converts the framebuffer back to
 * RGB555 and restores the display configuration.
 *
 * The exact video format and meanings of the input parameters
 * require confirmation from the called functions.
 *
 * Returns the result of the main processing routine, or zero
 * when initialization cannot proceed.
 */
/**
 * @brief Processes one video frame: opens the resource, optionally converts the
 *        320x256 frame buffer between RGB555 and RGB888, decodes and blits.
 *
 * @param arg0 Resource/file name passed to func_8001D5E4 (the file layer).
 * @param arg1 Mode/format selector from the caller (see gDrawImageResource: 4 for
 *             full screens, 0x26 for the narrow variant).
 * @param arg2 Second coordinate/offset from the caller.
 *
 * @return The result of the main processing routine, or 0 when the layer is not
 *         open or the resource cannot be opened.
 *
 * @par Confirmed flow (the outline comment above the function is the original
 *      author's description and is kept as-is)
 * 1. If D_8009F504 == 0 the call is a no-op returning 0.
 * 2. handle = func_8001D5E4(arg0, D_8009F238); a negative handle means "cannot
 *    open" - func_80092BF4(1) restores the display and 0 is returned.
 * 3. If the mode flags carry 0x24 (convert + clear), the function converts the
 *    current frame buffer row by row: two 1-pixel-high rectangles (0x140 and
 *    0x1E0 wide) are read with func_80092F98, the y positions depend on the
 *    current screen page (func_8001C034), and every pixel is expanded from
 *    5-6-5 to 24 bit while a central 0x100 x 0x90 region (rows 0x31..0xC0,
 *    columns 0x20..0x11F) is forced to black when the mode mask applies.
 * 4. The proper decode path then runs the resource through
 *    gMovieStreamReaderInit/func_80087824 with the parameter block D_800A6E20, writes the
 *    result with func_80092F38 / func_80092FF8 and converts back to RGB555
 *    afterwards.
 *
 * @par Globals
 * D_8009F504 (layer open), gMovieDisplayMode (mode flags), gMovieStreamBuffer/gMovieAudioBlock/
 * gMovieSpuBufferA/gMovieSpuBufferB (buffers), D_8009F960/D_8009F964/D_8009F968
 * (geometry), D_8009F238 (file layer parameter), D_800A6E20 (parameter block).
 *
 * @par Callers
 * gDrawImageResource only.
 */
u8 gPlayVideoFile(s32 arg0, s32 arg1, s32 arg2) __asm__("func_80088120");

u8 gPlayVideoFile(s32 arg0, s32 arg1, s32 arg2)
{
    Params88120 params;
    Rect88120 rectA;
    Rect88120 rectB;
    Rect88120 rectC;
    Rect88120 rectD;
    s32 handle;
    s32 result;
    s32 mode;
    s32 masked;
    s32 page;
    u16 *src16;
    u8 *dst8;
    u8 *src8;
    u16 *dst16;
    u16 *buf16;
    u8 *buf8;
    s32 row;
    u8 *in8;
    u16 *out16;
    s32 line;
    s32 col;
    s32 above;
    s32 inside;
    s32 x;
    s32 count;
    u32 c;
    u32 r;
    u32 g;
    u32 b;

    result = 0;
    if (gLayerOpenResult == 0) {
        return 0;
    }
    handle = func_8001D5E4(arg0, gFileSearchGroup);
    if (handle < 0) {
        func_80092BF4(1);
        return 0;
    }
    params = gFileTable;

    if ((gMovieDisplayMode & 0x24) == 0x24) {
        rectA.x = 0;
        rectA.w = 0x140;
        rectA.h = 1;
        rectB.x = 0;
        rectB.w = 0x1E0;
        rectB.h = 1;
        masked = gMovieDisplayMode & 2;
        page = func_8001C034();
        if (page == 0) {
            rectA.y = 0x100;
            rectB.y = 0;
        } else {
            rectA.y = 0;
            rectB.y = 0x100;
        }
        buf16 = func_8008799C(0x280);
        if (buf16 == 0) {
            return 0;
        }
        buf8 = func_8008799C(0x3C0);
        if (buf8 == 0) {
            func_800879BC(buf16);
            return 0;
        }
        for (row = 0; row < 0x100; row++) {
            func_80092C8C(0);
            func_80092F98(&rectA, buf16);
            func_80092C8C(0);
            x = 0;
            above = row < 0x31;
            inside = row < 0xC1;
            src16 = buf16;
            dst8 = buf8;
            for (; x < 0x140; x++) {
                if (masked != 0 && (u32)(x - 0x20) < 0x100 && !above && inside) {
                    *dst8++ = 0;
                    *dst8++ = 0;
                    *dst8++ = 0;
                } else {
                    c = *src16;
                    *dst8++ = ((c & 0x1F) << 3) | ((c & 0x1F) >> 2);
                    *dst8++ = (((c >> 5) & 0x1F) << 3) | (((c >> 5) & 0x1F) >> 2);
                    *dst8++ = (((c >> 10) & 0x1F) << 3) | (((c >> 10) & 0x1F) >> 2);
                }
                src16++;
            }
            func_80092F38(&rectB, buf8);
            rectA.y++;
            rectB.y++;
        }
        func_80092C8C(0);
        func_800879BC(buf8);
        func_800879BC(buf16);
        func_800858F4(page);
        func_80085900(1);
        func_80091270(0);
        gMoviePresentFrame();
        rectA.x = 0;
        rectA.w = 0x1E0;
        rectA.h = 0x100;
        if (page == 0) {
            rectA.y = 0;
            rectB.y = 0x100;
        } else {
            rectA.y = 0x100;
            rectB.y = 0;
        }
        func_80092C8C(0);
        func_80092FF8(&rectA, 0, rectB.y);
        func_80092C8C(0);
    }

    func_80085AC4(0, 1);
    if (gMovieStreamReaderInit(gMovieStreamBuffer, gMovieAudioClockDisable, &params, 1, 0x800) != 0) {
        if (func_80087824(gMovieAudioBlock, 0x400000, gMovieSpuBufferA, 0x800000, gMovieSpuBufferB, D_8009F964, 1) != 0) {
            gMovieSetAudioMode(0);
            result = gMovieStreamDemux(arg1, arg2, gLayerModeBits * D_8009F960);
            count = 3;
            while (gMovieAudioBusyCount() != 0 && count > 0) {
                func_80091270(0);
                count--;
            }
            func_80087750();
        }
        gMovieStreamReaderClose();
    }
    func_8001D3A8(handle);

    if ((gMovieDisplayMode & 0x24) == 0x24) {
        rectC.x = 0;
        rectC.w = 0x1E0;
        rectC.h = 1;
        rectD.x = 0;
        rectD.w = 0x140;
        rectD.h = 1;
        mode = func_800858E0();
        if (mode == 0) {
            rectC.y = 0x100;
            rectD.y = 0;
        } else {
            rectC.y = 0;
            rectD.y = 0x100;
        }
        in8 = func_8008799C(0x3C0);
        if (in8 == 0) {
            return 0;
        }
        out16 = func_8008799C(0x280);
        if (out16 == 0) {
            func_800879BC(in8);
            return 0;
        }
        for (line = 0; line < 0x100; line++) {
            func_80092C8C(0);
            func_80092F98(&rectC, in8);
            func_80092C8C(0);
            col = 0;
            dst16 = out16;
            src8 = in8;
            for (; col < 0x140; col++) {
                r = *src8++ >> 3;
                g = *src8++ >> 3;
                b = *src8++ >> 3;
                *dst16++ = (b << 10) | (g << 5) | r;
            }
            func_80092F38(&rectD, out16);
            rectC.y++;
            rectD.y++;
        }
        func_80092C8C(0);
        func_800879BC(out16);
        func_800879BC(in8);
        func_8001C088(mode, 0);
        func_80091270(0);
        func_8001BFE4();
        rectC.x = 0;
        rectC.w = 0x140;
        rectC.h = 0x100;
        if (mode == 0) {
            rectC.y = 0;
            rectD.y = 0x100;
        } else {
            rectC.y = 0x100;
            rectD.y = 0;
        }
        func_80092C8C(0);
        func_80092FF8(&rectC, 0, rectD.y);
        func_80092C8C(0);
        func_80091270(0);
        func_8001C088(1 - mode, 0);
        func_80091270(0);
        func_8001BFE4();
    }
    return result;
}
