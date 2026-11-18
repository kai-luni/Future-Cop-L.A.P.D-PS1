// Categories: Platform > SPU | Platform > Sound > Shutdown
#include "game/types.h"

struct SpuVoiceRequest {
    u32 voices;
    u32 mask;
    s16 left;
    s16 right;
    u8 unk0C[8];
    s16 pitch;
    u8 unk16[0x2A];
};

extern void gSpuSetVoiceAttributes(struct SpuVoiceRequest *request) __asm__("func_80099368");

/**
 * @brief Zeroes left/right volumes and pitch on all SPU channels except one preserved voice.
 *
 * Builds a stack SpuVoiceRequest with voices = ~(1 << voice) & 0xFFFFFF (all 24 channels
 * except voice), mask = 0x13 (left volume, right volume, pitch), and zeroed values for gSpuSetVoiceAttributes.
 *
 * @param voice SPU channel index (0..23) spared from muting.
 *
 * @par Globals
 * None directly; gSpuSetVoiceAttributes writes D_8009EEA4 hardware voice registers.
 *
 * @par Control flow
 * Straight-line stack struct setup and single call to gSpuSetVoiceAttributes.
 *
 * @par Calls
 * gSpuSetVoiceAttributes - updates SPU voice pitch and L/R volumes for the selected voice bitmask.
 *
 * @par Side effects
 * Mutes and clears pitch on 23 of the 24 SPU hardware channels via gSpuSetVoiceAttributes.
 *
 * @par Callers
 * Confirmed: gSoundHaltAllVoices (0x8004765C, when sparing a preserved stream voice node->voice during audio shutdown) / Likely: none / Unknown: none.
 */
void func_80021BD0(s32 voice)
{
    struct SpuVoiceRequest request;

    request.voices = ~(1 << voice) & 0xFFFFFF;
    request.mask = 0x13;
    request.pitch = 0;
    request.left = 0;
    request.right = 0;
    gSpuSetVoiceAttributes(&request);
}
