// Categories: Platform > Streaming Audio | Platform > Sound > Voices
#include "game/types.h"

struct StreamChannel {
    s32 id;
    s32 voice;
    u8 unk08[0x1C];
};

struct StreamState {
    u8 flag;
    u8 unk01[3];
    s32 active;
    s32 unk08;
    u8 unk0C[4];
    s16 unk10;
    u8 unk12[2];
    u32 channelCount;
    u8 unk18[0x0C];
    struct StreamChannel channels[3];
};

extern struct StreamState gMixerState __asm__("D_800A0968");

extern void gDisarmAudioStreamSpuIrq(void) __asm__("func_80022F7C");
extern void gSpuInstallIrqCallback(s32 mode) __asm__("func_80098BF8");
extern void gSpuCancelDmaRequests(void) __asm__("func_8002129C");
extern void gReleaseSoundVoicesByOwner(s32 owner) __asm__("func_80022240");
extern void func_80046A34(s32 handle);

s32 gStopAudioStreamPlayback(void) __asm__("func_80022A64");

/**
 * @brief Stops active stream playback, cancels its SPU callback and releases allocated voices.
 *
 * @param void No parameters.
 * @return Always 0.
 *
 * @par Confirmed mechanics
 * Tests mixer +0x04, clears the 16-bit sequence and disables the IRQ through
 * gDisarmAudioStreamSpuIrq. Uninstalls the handler, clears active and readiness fields, calls
 * gSpuCancelDmaRequests and walks the configured channels, releasing IDs other than -1 and
 * their voice handles.
 *
 * @par Globals
 * Reads and updates the active fields and channel records in gMixerState.
 *
 * @par Callers
 * Direct calls from gStopAudioStreamAndClearChannels (func_800226D8).
 *
 * @par Role and path
 * The stream shutdown wrapper calls this before clearing channel buffer addresses.
 *
 * @par Unknown
 * The voice cleanup helpers own the lifetime of the referenced voice records.
 */
s32 gStopAudioStreamPlayback(void)
{
    u32 i;

    if (gMixerState.active == 0) {
        return 0;
    }
    gMixerState.unk10 = 0;
    gDisarmAudioStreamSpuIrq();
    gSpuInstallIrqCallback(0);
    gMixerState.flag = 0;
    gMixerState.active = 0;
    gMixerState.unk08 = 0;
    gSpuCancelDmaRequests();
    for (i = 0; i < gMixerState.channelCount; i++) {
        if (gMixerState.channels[i].id != -1) {
            gReleaseSoundVoicesByOwner(gMixerState.channels[i].id);
            func_80046A34(gMixerState.channels[i].voice);
        }
    }
    return 0;
}
