// Categories: Resources > Movie Playback > Stream Demux | Graphics > Video Decode | Platform > Sound
#include "game/types.h"

extern u32 gChunkSkipFlag __asm__("D_8009F4DC");
extern u8 gLoopAbortFlag __asm__("D_8009F4E0");
extern u32 D_8009F958;
extern u32 D_8009F95C;

extern u32 func_8001EF8C(s32 arg0);
extern u32 *func_80089404(u32 *arg0);
extern void func_80086DB4(void);
extern void func_80086F1C(void);
extern u32 func_80086F44(void);
extern u32 func_80086F58(u32 arg0);
extern void func_8008589C(u8 *arg0, u8 *arg1, u8 *arg2);
extern void func_80091270(s32 arg0);
extern void func_80085914(void);
extern void func_80086FE4(u8 *arg0);
extern void func_80087430(u8 *arg0);
extern void func_80089A40(u8 *arg0);
extern void func_80089304(u32 arg0);

/**
 * @brief Demuxes and plays a movie stream chunk by chunk: reads big-endian chunk
 *        headers from the buffered file, sends audio chunks to the mixer and
 *        video chunks to the MDEC decoder, paced by a frame clock.
 *
 * @param arg0 First buffer handed to the slice decoder func_8008589C (that
 *             translation unit declares it without parameters, so the two
 *             buffers are only known from this call site).
 * @param arg1 Second buffer handed to func_8008589C.
 * @param arg2 Byte budget reserved in the stream reader up front; the caller
 *             func_80088120 passes D_8009F968 * D_8009F960, the frame buffer
 *             size.
 * @return 1 when the stream was consumed, 0 when a read timed out, the reader
 *         reported an empty slot, or the error paths were taken.
 *
 * @par Confirmed
 * 1. func_80089404(&slot) reserves arg2 bytes and returns their address. Every
 *    read uses the same call: the caller puts the requested size in slot and the
 *    callee overwrites it with the size actually available.
 * 2. Every pass compares bits 0x40080000 of func_8001EF8C(0) and func_8001EF8C(1)
 *    with the previous values kept in D_8009F958/D_8009F95C, so two hardware
 *    status words are polled. A change is tolerated while the video chunk
 *    counter is below 4; at 4 or more the function returns 0 and, when bit
 *    0x80000 is involved, also sets D_8009F4E0 first.
 * 3. An 8-byte header is read and both words are byte-swapped into tag and size.
 *    With D_8009F4DC bit 0 set, a size of 0x424C or more skips the chunk with a
 *    4-byte advance; with it clear, a size of 0x4400 or more takes the same error
 *    path.
 * 4. The payload is waited for and dispatched on the tag: 0x564C4330 builds the
 *    VLC tables from payload + 8 (func_80089A40); 0x41643130, 0x41643131,
 *    0x61643130, 0x61643131, 0x61643230 and 0x61643231 push compressed audio
 *    into the ring buffer (func_80086FE4); 0x61753030 and 0x61753031 push the
 *    other audio format through func_80087430; 0x4D444332 and 0x4D444543 decode
 *    a video frame - the chunk with counter 1 initialises the triple buffer
 *    (func_80086DB4), then func_80086F1C starts it, func_8008589C decodes, the
 *    body spins on func_80091270(0) until func_80086F44() reaches the deadline
 *    from func_80086F58(chunk counter) and func_80085914 flips the buffers.
 * 5. Every branch ends at func_80089304(size), the reader advance, and the loop
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
 * watched) and what D_8009F4E0 signals to the rest of the player.
 *
 * @par Callers
 * func_80088120 only, the video resource open and decode entry point.
 */
s32 func_800879DC(u8 *arg0, u8 *arg1, s32 arg2)
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
    func_80089404(&slot);

    count = 0;
    ok = 1;
    index = 0x80000;

    for (;;) {
        state0 = func_8001EF8C(0) & 0x40080000;
        state1 = func_8001EF8C(1) & 0x40080000;

        if ((state0 & ~D_8009F958) || (state1 & ~D_8009F95C)) {
            if (count >= 4) {
                if (((state0 & ~D_8009F958) & index) ||
                    ((state1 & ~D_8009F95C) & index)) {
                    gLoopAbortFlag = 1;
                }
                ok = 0;
                goto ret;
            }
        }

        D_8009F958 = state0;
        D_8009F95C = state1;

        slot = 8;
        data = (u8 *)func_80089404(&slot);
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

        if (gChunkSkipFlag & 1) {
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
        data = (u8 *)func_80089404(&slot);
        if (data == 0) {
            goto ret;
        }
        if (slot == 0) {
            goto ret;
        }

        switch (tag) {
        case 0x564C4330:
            func_80089A40(data + 8);
            break;

        case 0x41643130:
        case 0x41643131:
        case 0x61643130:
        case 0x61643131:
        case 0x61643230:
        case 0x61643231:
            func_80086FE4(data);
            break;

        case 0x61753030:
        case 0x61753031:
            func_80087430(data);
            break;

        case 0x4D444332:
        case 0x4D444543:
        {
            register u32 idx __asm__("$16");
            idx = count;

            if (idx == 1) {
                func_80086DB4();
            }
            func_80086F1C();
            count = idx + 1;
            limit = func_80086F58(idx);
            func_8008589C(data, arg0, arg1);
            while (func_80086F44() < limit) {
                func_80091270(0);
            }
            func_80085914();
        }
        break;

        default:
        err4:
            slot = 4;
            break;
        }

    common:

        func_80089304(slot);
    }

ret:
    return ok;
}
