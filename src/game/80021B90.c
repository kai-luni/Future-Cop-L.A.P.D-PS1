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

extern void func_80099368(struct SpuVoiceRequest *request);

/**
 * @brief Zeroes left/right volumes and pitch on all 24 SPU hardware channels.
 *
 * Populates a stack SpuVoiceRequest with voices = 0xFFFFFF (channels 0..23), mask = 0x13
 * (left volume, right volume, and pitch), and zero values, then passes it to func_80099368.
 *
 * @param void No parameters.
 *
 * @par Globals
 * None directly; func_80099368 writes D_8009EEA4 hardware voice registers.
 *
 * @par Control flow
 * Straight-line stack struct initialization and single call to func_80099368.
 *
 * @par Calls
 * func_80099368 - applies SPU voice attributes (pitch and L/R volumes) across all 24 channels.
 *
 * @par Side effects
 * Mutes and zeroes pitch on all 24 SPU channels in hardware via func_80099368.
 *
 * @par Callers
 * Confirmed: func_80047554 (0x8004766C, full audio shutdown when no voice node is spared) / Likely: none / Unknown: none.
 */
void func_80021B90(void)
{
    struct SpuVoiceRequest request;

    request.voices = 0xFFFFFF;
    request.mask = 0x13;
    request.pitch = 0;
    request.left = 0;
    request.right = 0;
    func_80099368(&request);
}
