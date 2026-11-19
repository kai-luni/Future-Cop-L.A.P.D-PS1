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

extern void func_80022D1C(void *data, s32 mode, s32 length);
extern void func_80023204(void);
extern void func_800212F8(struct Request *req);

/**
 *  @brief Prepares and submits a request for the selected stream channel.
 *
 *  Uses the stream state to select a transfer mode and construct a request.
 *  The request callback continues processing when the transfer completes.
 *  @par Globals
 *  D_800A0968 supplies channel data and ring state; ready and counters are updated.
 *  @par Control flow
 *  Channel zero can return early; ring bookkeeping precedes mode selection.
 *  Looping selects mode 3; otherwise the offset selects mode 0, 1, or 2.
 *  @par Calls
 *  func_80022D1C prepares the channel buffer for the selected mode.
 *  func_800212F8 submits the request with func_80023204 as its callback.
 *  @par Side effects
 *  Clears ready, adjusts ring bookkeeping, and initiates a transfer.
 *  @par Callers
 *  Confirmed: gPrimeAudioStream, func_80022FA8, func_80023204.
 */
void func_80022D84(void)
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

    func_80022D1C((void *)gMixerState.channels[channel].start, mode, gMixerState.channels[channel].length);

    req.address = gMixerState.channels[channel].unk20 + gMixerState.unk78;
    req.size = gMixerState.channels[channel].length;
    req.data = (void *)gMixerState.channels[channel].start;
    req.callback = func_80023204;
    req.unused = gMixerState.unk10;
    if (mode == 3) {
        req.unused |= 0x80000000;
    }
    req.mode = 1;

    func_800212F8(&req);
}
