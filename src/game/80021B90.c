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
 * @brief Zeroes left/right volumes and pitch on all 24 SPU hardware channels.
 *
 * Populates a stack SpuVoiceRequest with voices = 0xFFFFFF (channels 0..23), mask = 0x13
 * (left volume, right volume, and pitch), and zero values, then passes it to gSpuSetVoiceAttributes.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; gSpuSetVoiceAttributes writes D_8009EEA4 hardware voice registers.
 *
 * @par Control flow
 * Straight-line stack struct initialization and single call to gSpuSetVoiceAttributes.
 *
 * @par Calls
 * gSpuSetVoiceAttributes - applies SPU voice attributes (pitch and L/R volumes) across all 24 channels.
 *
 * @par Side effects
 * Mutes and zeroes pitch on all 24 SPU channels in hardware via gSpuSetVoiceAttributes.
 *
 * @par Callers
 * Confirmed: gSoundHaltAllVoices (0x8004766C, full audio shutdown when no voice node is spared) / Likely: none / Unknown: none.
 */
void func_80021B90(void)
{
    struct SpuVoiceRequest request;

    request.voices = 0xFFFFFF;
    request.mask = 0x13;
    request.pitch = 0;
    request.left = 0;
    request.right = 0;
    gSpuSetVoiceAttributes(&request);
}
