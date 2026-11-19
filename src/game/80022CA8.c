// Categories: Platform > Sound | Platform > Streaming Audio
#include "game/types.h"

struct StreamChannel {
    s32 id;
    s32 voice;
    s32 left;
    s32 right;
    u8 unk10[0x14];
};

struct StreamState {
    u8 flag;
    u8 unk01[3];
    s32 active;
    s32 unk08;
    s32 stopping;
    s16 unk10;
    u8 unk12[0x0E];
    s32 unk20;
    struct StreamChannel channels[3];
    u8 unk90[0];
};

struct StreamTail {
    u8 unk00[0x70];
    s32 rate;
    s32 currentRate;
    s32 unk78;
    s32 unk7C;
    s32 ready;
    s32 unk84;
    s32 deadline;
};

extern struct StreamState gMixerState __asm__("D_800A0968");

/**
 * @brief Resets the global audio stream controller state D_800A0968 and its first two channels.
 *
 * Confirmed: zeroes D_800A0968 control fields (flag, active, unk08, stopping, unk10, unk20, unk78, unk7C, unk84, deadline), sets ready = 1, copies rate (+0x70) to currentRate (+0x74), and resets channels[0..1] (left = 0, right = 0x40, id = -1).
 * Category follows StreamState/StreamChannel structure D_800A0968 used with func_80046B04/func_800477D8 voice playback in func_800228F4 and func_8002283C.
 * Likely: prepares D_800A0968 for a new or restarted streaming audio session (stereo pair pan/gain left=0, right=0x40); Unknown: why only 2 of the 3 channels are reset.
 *
 * @param void No parameters.
 *
 * @par Globals
 * D_800A0968 - global StreamState/StreamTail structure reset in place.
 * @par Control flow
 * Linear field resets followed by a 2-iteration for-loop (i = 0, 1) initializing D_800A0968.channels[i].
 * @par Calls
 * None.
 * @par Side effects
 * Resets global audio streaming fields and channel[0..1] pan/id entries in D_800A0968.
 * @par Callers
 * Confirmed: gPrimeAudioStream (0x80022720), gConfigureSoundSystem (0x80021C44 in asm/us/main.s), gConfigureAudioStreaming (0x800225E8 in asm/us/main.s) / Likely: none / Unknown: none.
 */
void func_80022CA8(void)
{
    struct StreamTail *tail;
    u32 i;

    tail = (struct StreamTail *)&gMixerState;
    gMixerState.flag = 0;
    gMixerState.active = 0;
    tail->deadline = 0;
    gMixerState.unk08 = 0;
    gMixerState.stopping = 0;
    gMixerState.unk20 = 0;
    gMixerState.unk10 = 0;
    tail->unk84 = 0;
    tail->unk78 = 0;
    tail->unk7C = 0;
    tail->ready = 1;
    tail->currentRate = tail->rate;
    for (i = 0; i < 2; i++) {
        gMixerState.channels[i].left = 0;
        gMixerState.channels[i].right = 0x40;
        gMixerState.channels[i].id = -1;
    }
}
