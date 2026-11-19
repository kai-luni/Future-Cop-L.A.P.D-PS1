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

void gResetAudioStreamController(void) __asm__("func_80022CA8");

/**
 * @brief Resets the audio stream controller and the first two channel mix records.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Clears control fields including the sequence and deadline, sets +0x80 to 1 and copies +0x70
 * to +0x74. A two-iteration loop stores channel ID -1, level 0 and pan 0x40 at a stride of
 * 0x24.
 *
 * @par Globals
 * Updates gMixerState, including gStreamStopPending and gStreamAvailableBlocks at embedded
 * offsets.
 *
 * @par Callers
 * Direct calls from gConfigureSoundSystem (func_80021C20), gConfigureAudioStreaming
 * (func_80022484), gPrimeAudioStream (func_80022710).
 *
 * @par Role and path
 * Sound setup, mixer configuration and stream priming reset the controller before using it.
 *
 * @par Unknown
 * The reset loop covers exactly two mixer channels.
 */
void gResetAudioStreamController(void)
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
