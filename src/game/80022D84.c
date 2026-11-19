// Categories: Game Engine > Audio Streaming | Resources > Requests
#include "game/types.h"

struct StreamChannel {
    s32 id;
    s32 voice;
    s32 left;
    s32 right;
    s32 start;
    s32 length;
    u8 looping;
    u8 pad19[7];
    s32 unk20;
};

struct StreamState {
    u8 flag;
    u8 pad01[3];
    s32 active;
    s32 unk08;
    s32 stopping;
    u16 unk10;
    u8 pad12[14];
    s32 unk20;
    struct StreamChannel channels[2];
    s32 unk6C;
    s32 unk70;
    s32 unk74;
    s32 unk78;
    s32 unk7C;
    volatile s32 ready;
};

struct Request {
    s32 address;
    u32 size;
    void *data;
    void (*callback)(void);
    u32 unused;
    s32 mode;
};

extern struct StreamState gMixerState __asm__("D_800A0968");

extern void gMarkAudioStreamAdpcmBlock(void *data, s32 mode, s32 length) __asm__("func_80022D1C");
extern void gCompleteAudioStreamTransfer(void) __asm__("func_80023204");
extern void func_800212F8(struct Request *req);

void gSubmitAudioStreamTransfer(void) __asm__("func_80022D84");

/**
 * @brief Prepares a stream audio block and submits its SPU transfer request.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Selects a channel from mixer +0x20 and checks source and available blocks for channel zero.
 * Updates ring read offset and readiness, selects mode 0 to 3 from looping and offset, marks
 * the ADPCM block and fills the stack request with address, length, source, completion
 * callback, sequence and DMA mode.
 *
 * @par Globals
 * Reads and updates gMixerState, including gStreamDmaChannelIndex and gStreamAvailableBlocks.
 *
 * @par Callers
 * Direct calls from gHandleAudioStreamSpuIrq (D_80022FA8), gCompleteAudioStreamTransfer
 * (D_80023204), gPrimeAudioStream (func_80022710).
 *
 * @par Role and path
 * Priming, SPU IRQ and DMA completion all use this request builder.
 *
 * @par Unknown
 * The request word's high bit is set for mode 3 and interpreted by the completion callback.
 */
void gSubmitAudioStreamTransfer(void)
{
    struct Request req;
    s32 mode;
    s32 channel;

    channel = gMixerState.unk20;
    if (channel == 0) {
        if (gMixerState.channels[0].start == 0 || gMixerState.unk74 == 0) {
            return;
        }
    }

    if (gMixerState.unk08 != 0) {
        if (channel == 0) {
            if (gMixerState.unk74 == gMixerState.unk70) {
                gMixerState.unk78 = gMixerState.unk7C + gMixerState.unk6C;
                if (gMixerState.unk78 == gMixerState.unk74 * gMixerState.unk6C) {
                    gMixerState.unk78 = 0;
                }
                gMixerState.unk74--;
            }
        }
    }

    gMixerState.ready = 0;
    if (channel == 0) {
        gMixerState.unk74--;
    }

    if (gMixerState.channels[channel].looping != 0) {
        mode = 3;
    } else if (gMixerState.unk78 == 0) {
        mode = 0;
    } else if (gMixerState.unk78 == (gMixerState.unk70 - 1) * gMixerState.unk6C) {
        mode = 1;
    } else {
        mode = 2;
    }

    gMarkAudioStreamAdpcmBlock((void *)gMixerState.channels[channel].start, mode, gMixerState.channels[channel].length);

    req.address = gMixerState.channels[channel].unk20 + gMixerState.unk78;
    req.size = gMixerState.channels[channel].length;
    req.data = (void *)gMixerState.channels[channel].start;
    req.callback = gCompleteAudioStreamTransfer;
    req.unused = gMixerState.unk10;
    if (mode == 3) {
        req.unused |= 0x80000000;
    }
    req.mode = 1;

    func_800212F8(&req);
}
