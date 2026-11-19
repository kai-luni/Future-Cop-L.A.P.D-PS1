// Categories: Graphics > Video Layer | Resources > Movie Playback
#include "game/types.h"

extern u8 gMovieAbortFlag __asm__("D_8009F4E0");
extern u8 gLayerOpenResult __asm__("D_8009F504");
extern s32 gMovieDisplayMode __asm__("D_8009F4DC");
extern s32 D_8009F4E4;
extern s32 D_8009F4E8;
extern s32 gMovieStreamBuffer __asm__("D_8009F4EC");
extern s32 gMovieAudioBlock __asm__("D_8009F4F0");
extern s32 gMovieSpuBufferA __asm__("D_8009F4F4");
extern s32 gMovieSpuBufferB __asm__("D_8009F4F8");
extern s32 D_8009F4FC;
extern s32 D_8009F500;
extern s32 gMoviePadMask0 __asm__("D_8009F958");
extern s32 gMoviePadMask1 __asm__("D_8009F95C");
extern s32 D_8009F960;
extern s32 D_8009F964;
extern s32 gLayerModeBits __asm__("D_8009F968");
extern s32 gChainedNodes[] __asm__("D_8009FA48");

extern s32 gPollPadButtons(s32) __asm__("func_8001EF8C");
extern void func_800859B0(void);
extern void gMovieSetScaleMode(s32) __asm__("func_800859D8");
extern s32 func_80085A1C(void);
extern s32 gMovieInitContext(s32, s32, s32, s32, s32, s32, s32) __asm__("func_80085C4C");
extern s32 func_80087814(void);
extern s32 func_8008781C(void);
extern s32 func_8008799C(s32);
extern void gCloseVideoLayer(void) __asm__("func_80087D48");
extern s32 gCalcMovieBufferSize(s32, s32, s32) __asm__("func_800896D4");
extern void gBuildMovieVlcTables(s32, s32, s32) __asm__("func_80089A88");
extern void func_8008F7AC(void);
extern s32 func_80091F60(s32);
extern void func_80092BF4(s32);
extern s32 func_80097B28(s32);

/**
 * @brief Opens the video/image layer: sets the frame geometry, allocates the
 *        buffers and initialises the decoder tables.
 *
 * @param arg0 Mode bits. bit0 selects the "narrow" variant (D_8009F968 = 0x27
 *             instead of 0xC8, buffer 0xF6C0 instead of 0x22D00, extra flag
 *             0x10 instead of 0x4); bit1 (0x20) suppresses the direct
 *             display reset; bits 2 (0x4), 3 (0x8) and 4 (0x10) select the
 *             decoder initialisation path.
 * @return 1 when the layer is open, 0 when an allocation failed (the layer is
 *         then closed again through gCloseVideoLayer).
 *
 * @par Confirmed flow
 * 1. Already open (D_8009F504 != 0) returns 1 immediately.
 * 2. func_8001EF8C(0) and func_8001EF8C(1) are masked with 0x40080000 and
 *    stored in gMoviePadMask0/gMoviePadMask1 (two per-channel flag sets).
 *    D_8009F960 = 0x400, D_8009F964 = 0x800 (destination strides) and
 *    D_8009F968 = 0xC8 or 0x27 (row length).
 * 3. Unless bit 0x20 is set, func_80092BF4(0) and func_80091F60(0) reset the
 *    display; func_8008F7AC() then prepares the video hardware.
 * 4. Main buffer: func_8008799C(0x22D00) (or 0xF6C0), then 0x6000 and 0x600
 *    bytes; func_80089A88(video, table, 0) builds the decode tables.
 * 5. gMovieSetScaleMode(0/1) or func_800859B0() select the scaling mode according
 *    to the mode bits.
 * 6. func_8008799C(func_80085A1C()) sizes the ring buffer and
 *    gMovieInitContext(ring, ptr, ptr+a3, ptr+2*a3, 1, narrow, 0) initialises the
 *    decoder with a3 = 0x10000 (or 0x6DE0 in the narrow variant).
 * 7. One more buffer of func_800896D4(D_8009F968, 0x4400, 1) bytes for the
 *    wide variant (the narrow one reuses D_8009FA48[0] and still calls
 *    func_800896D4 with 0x424C), then func_8008781C() bytes, and two blocks of
 *    func_80087814() bytes each.
 * 8. D_8009F504 = 1 and 1 is returned; any failed allocation jumps to the
 *    failure path that calls gCloseVideoLayer() and returns 0.
 *
 * @par Globals
 * gMovieDisplayMode (mode + derived flags), gMoviePadMask0/gMoviePadMask1/D_8009F960/
 * D_8009F964/D_8009F968 (geometry), D_8009F4E4/D_8009F4E8/gMovieStreamBuffer/
 * gMovieAudioBlock/gMovieSpuBufferA/gMovieSpuBufferB/D_8009F4FC/D_8009F500 (buffers),
 * D_8009F504 (open flag), D_8009FA48.
 *
 * @par Likely role
 * "Open a movie/image for playback": two variants (full screen 0x140 wide and
 * a narrower 0x100-wide one), a hardware decoder with a triple-buffer ring, and
 * two tables that later get copied into the scratchpad by func_800861E4.
 *
 * @par Callers
 * gDrawImageResource, at most once per draw unless the layer is already open.
 */
s32 gOpenVideoLayer(s32 arg0) __asm__("func_80087EA0");

s32 gOpenVideoLayer(s32 arg0)
{
    register s32 var_s0 __asm__("$16");
    register s32 var_v0 __asm__("$2");
    s32 ptr;
    s32 var_a0;
    s32 var_a3;

    if (gLayerOpenResult != 0) {
        return 1;
    }

    gMovieAbortFlag = 0;
    var_a0 = gPollPadButtons(0);
    var_s0 = 0x40080000;
    gMoviePadMask0 = var_a0 & var_s0;
    var_v0 = gPollPadButtons(1) & var_s0;
    gMoviePadMask1 = var_v0;
    var_v0 = 0x400;
    D_8009F960 = var_v0;
    var_v0 = 0x800;
    D_8009F964 = var_v0;
    var_v0 = arg0 & 0x20;
    var_s0 = var_v0 < 1;

    if (!(arg0 & 1)) {
        gLayerModeBits = 0xC8;
    } else {
        gLayerModeBits = 0x27;
    }

    if (var_s0 != 0) {
        func_80092BF4(0);
        func_80091F60(0);
    }

    func_8008F7AC();

    if (arg0 & 1) {
        var_a0 = 0xF6C0;
        arg0 |= 0x10;
    } else {
        arg0 |= 0x4;
        var_a0 = 0x22D00;
    }

    gMovieDisplayMode = arg0;
    D_8009F4E4 = func_8008799C(var_a0);
    if (D_8009F4E4 == 0) {
        goto fail;
    }

    D_8009F4FC = func_8008799C(0x6000);
    if (D_8009F4FC == 0) {
        goto fail;
    }

    D_8009F500 = func_8008799C(0x600);
    if (D_8009F500 == 0) {
        goto fail;
    }

    gBuildMovieVlcTables(D_8009F4FC, D_8009F500, 0);

    if (arg0 & 8) {
        gMovieSetScaleMode(0);
    } else if (arg0 & 0x10) {
        gMovieSetScaleMode(1);
    } else if (arg0 & 4) {
        func_800859B0();
    }

    var_a0 = func_8008799C(func_80085A1C());
    D_8009F4E8 = var_a0;
    if (var_a0 == 0) {
        goto fail;
    }

    var_a3 = 0x10000;
    if (gMovieDisplayMode & 1) {
        var_a3 = 0x6DE0;
    }

    ptr = D_8009F4E4;
    if (gMovieInitContext(var_a0, ptr, ptr + var_a3,
                       ptr + var_a3 * 2, 1, var_s0, 0) == 0) {
        goto fail;
    }

    if (!(arg0 & 1)) {
        gMovieStreamBuffer = func_8008799C(gCalcMovieBufferSize(gLayerModeBits, 0x4400, 1));
        if (gMovieStreamBuffer == 0) {
            goto fail;
        }
    } else {
        gMovieStreamBuffer = gChainedNodes[0];
        gCalcMovieBufferSize(gLayerModeBits, 0x424C, 1);
    }

    gMovieAudioBlock = func_8008799C(func_8008781C());
    if (gMovieAudioBlock == 0) {
        goto fail;
    }

    gMovieSpuBufferA = func_80097B28(func_80087814());
    if (gMovieSpuBufferA == 0) {
        goto fail;
    }

    gMovieSpuBufferB = func_80097B28(func_80087814());
    if (gMovieSpuBufferB == 0) {
        goto fail;
    }

    gLayerOpenResult = 1;
    return 1;

fail:
    gCloseVideoLayer();
    return 0;
}
