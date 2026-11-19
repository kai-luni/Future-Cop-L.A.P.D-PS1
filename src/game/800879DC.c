// Categories: Resources > Movie Playback > Stream Demux | Graphics > Video Decode | Platform > Sound
#include "game/types.h"

extern u32 gMovieDisplayMode __asm__("D_8009F4DC");
extern u8 gMovieAbortFlag __asm__("D_8009F4E0");
extern u32 gMoviePadMask0 __asm__("D_8009F958");
extern u32 gMoviePadMask1 __asm__("D_8009F95C");

extern u32 gPollPadButtons(s32 arg0) __asm__("func_8001EF8C");
extern u32 *gMovieStreamReserve(u32 *arg0) __asm__("func_80089404");
extern void gMovieAudioStart(void) __asm__("func_80086DB4");
extern void gMovieAudioClockEnable(void) __asm__("func_80086F1C");
extern u32 gMovieAudioClockRead(void) __asm__("func_80086F44");
extern u32 gMovieFrameDeadline(u32 arg0) __asm__("func_80086F58");
extern void gMovieDecodeSlice(u8 *arg0, u8 *arg1, u8 *arg2) __asm__("func_8008589C");
extern void func_80091270(s32 arg0);
extern void gMoviePresentFrame(void) __asm__("func_80085914");
extern void gMovieAudioChunkPush(u8 *arg0) __asm__("func_80086FE4");
extern void gMovieAudioChunkPushSplit(u8 *arg0) __asm__("func_80087430");
extern void gMovieBuildVlcTables(u8 *arg0) __asm__("func_80089A40");
extern void gMovieStreamAdvance(u32 arg0) __asm__("func_80089304");

/**
 * @brief Demuxes and plays a movie stream chunk by chunk: reads big-endian chunk
 *        headers from the buffered file, sends audio chunks to the mixer and
 *        video chunks to the MDEC decoder, paced by a frame clock.
 *
 * @param arg0 First buffer handed to the slice decoder gMovieDecodeSlice (that
 *             translation unit declares it without parameters, so the two
 *             buffers are only known from this call site).
 * @param arg1 Second buffer handed to gMovieDecodeSlice.
 * @param arg2 Byte budget reserved in the stream reader up front; the caller
 *             gPlayVideoFile passes D_8009F968 * D_8009F960, the frame buffer
 *             size.
 * @return 1 when the stream was consumed, 0 when a read timed out, the reader
 *         reported an empty slot, or the error paths were taken.
 *
 * @par Confirmed
 * 1. gMovieStreamReserve(&slot) reserves arg2 bytes and returns their address. Every
 *    read uses the same call: the caller puts the requested size in slot and the
 *    callee overwrites it with the size actually available.
 * 2. Every pass compares bits 0x40080000 of func_8001EF8C(0) and func_8001EF8C(1)
 *    with the previous values kept in gMoviePadMask0/gMoviePadMask1. Those calls are
 *    the per-port pad poll (see 8001EF8C.c); 0x40080000 is bit 30 | bit 19 of the
 *    complemented pad word, i.e. Cross | Start. A newly pressed button is only
 *    tolerated while the video chunk counter is below 4; at 4 or more the function
 *    returns 0 and, when Cross (bit 0x80000) is involved, also sets
 *    gMovieAbortFlag first, which aborts the whole video.
 * 3. An 8-byte header is read and both words are byte-swapped into tag and size.
 *    With gMovieDisplayMode bit 0 set, a size of 0x424C or more skips the chunk with a
 *    4-byte advance; with it clear, a size of 0x4400 or more takes the same error
 *    path.
 * 4. The payload is waited for and dispatched on the tag: 0x564C4330 builds the
 *    VLC tables from payload + 8 (gMovieBuildVlcTables); 0x41643130, 0x41643131,
 *    0x61643130, 0x61643131, 0x61643230 and 0x61643231 push compressed audio
 *    into the ring buffer (gMovieAudioChunkPush); 0x61753030 and 0x61753031 push the
 *    other audio format through gMovieAudioChunkPushSplit; 0x4D444332 and 0x4D444543 decode
 *    a video frame - the chunk with counter 1 initialises the triple buffer
 *    (gMovieAudioStart), then gMovieAudioClockEnable starts it, gMovieDecodeSlice decodes, the
 *    body spins on func_80091270(0) until gMovieAudioClockRead() reaches the deadline
 *    from gMovieFrameDeadline(chunk counter) and gMoviePresentFrame flips the buffers.
 * 5. Every branch ends at gMovieStreamAdvance(size), the reader advance, and the loop
 *    repeats until a read returns 0.
 *
 * @par Likely role
 * The playback loop of the full-motion video player: the tags are the chunk
 * FourCCs of the movie container (MDEC/MDC2 for video frames, ad10..ad21 for
 * compressed audio, au00/au01 for the other audio format, VLC0 for the Huffman
 * table block), so this function is the player's frame feeder and the place
 * where audio and video are kept in step.
 *
 * @par Unknown
 * What func_8001EF8C returns (two status words whose 0x40080000 bits are being
 * watched) and what gMovieAbortFlag signals to the rest of the player.
 *
 * @par Callers
 * gPlayVideoFile only, the video resource open and decode entry point.
 */
s32 gMovieStreamDemux(u8 *arg0, u8 *arg1, s32 arg2) __asm__("func_800879DC");

s32 gMovieStreamDemux(u8 *arg0, u8 *arg1, s32 arg2)
{
    u32 slot;
    u32 state0;
    u32 state1;
    u32 tag;
    u32 size;
    u32 limit;
    u32 count;
    u32 index;
    u8 *data;
    s32 ok;

    slot = arg2;
    gMovieStreamReserve(&slot);

    count = 0;
    ok = 1;
    index = 0x80000;

    for (;;) {
        state0 = gPollPadButtons(0) & 0x40080000;
        state1 = gPollPadButtons(1) & 0x40080000;

        if ((state0 & ~gMoviePadMask0) || (state1 & ~gMoviePadMask1)) {
            if (count >= 4) {
                if (((state0 & ~gMoviePadMask0) & index) ||
                    ((state1 & ~gMoviePadMask1) & index)) {
                    gMovieAbortFlag = 1;
                }
                ok = 0;
                goto ret;
            }
        }

        gMoviePadMask0 = state0;
        gMoviePadMask1 = state1;

        slot = 8;
        data = (u8 *)gMovieStreamReserve(&slot);
        if (data == 0) {
            goto ret;
        }
        if (slot == 0) {
            goto ret;
        }

        tag = ((*(u32 *)data >> 24) | ((*(u32 *)data >> 8) & 0xFF00) |
               ((*(u32 *)data & 0xFF00) << 8) | (*(u32 *)data << 24));
        size = ((*(u32 *)(data + 4) >> 24) | ((*(u32 *)(data + 4) >> 8) & 0xFF00) |
                ((*(u32 *)(data + 4) & 0xFF00) << 8) | (*(u32 *)(data + 4) << 24));

        if (gMovieDisplayMode & 1) {
            if (size >= 0x424C) {
                slot = 4;
                goto common;
            }
        } else {
            if (size >= 0x4400) {
                goto err4;
            }
        }

        slot = size;
        data = (u8 *)gMovieStreamReserve(&slot);
        if (data == 0) {
            goto ret;
        }
        if (slot == 0) {
            goto ret;
        }

        switch (tag) {
        case 0x564C4330:
            gMovieBuildVlcTables(data + 8);
            break;

        case 0x41643130:
        case 0x41643131:
        case 0x61643130:
        case 0x61643131:
        case 0x61643230:
        case 0x61643231:
            gMovieAudioChunkPush(data);
            break;

        case 0x61753030:
        case 0x61753031:
            gMovieAudioChunkPushSplit(data);
            break;

        case 0x4D444332:
        case 0x4D444543:
        {
            register u32 idx __asm__("$16");
            idx = count;

            if (idx == 1) {
                gMovieAudioStart();
            }
            gMovieAudioClockEnable();
            count = idx + 1;
            limit = gMovieFrameDeadline(idx);
            gMovieDecodeSlice(data, arg0, arg1);
            while (gMovieAudioClockRead() < limit) {
                func_80091270(0);
            }
            gMoviePresentFrame();
        }
        break;

        default:
        err4:
            slot = 4;
            break;
        }

    common:

        gMovieStreamAdvance(slot);
    }

ret:
    return ok;
}
