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

void gSilenceSpuVoicesExcept(s32 voice) __asm__("func_80021BD0");

/**
 * @brief Zeroes the left and right volumes and pitch of all selected SPU voices while sparing
 * one voice.
 *
 * @param voice SPU channel index (0..23) spared from muting.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Uses SLLV and NOR to clear the input voice bit from 0xFFFFFF, then submits a stack request
 * with attribute mask 0x13 and three zero halfwords.
 *
 * @par Globals
 * None directly. The callee writes the selected voice registers through gSpuRegBasePtr.
 *
 * @par Callers
 * Direct calls from gSoundHaltAllVoices (func_80047554).
 *
 * @par Role and path
 * The audio halt path uses this form when retaining a voice node.
 *
 * @par Unknown
 * The input index is used in a variable shift without a range check.
 */
void gSilenceSpuVoicesExcept(s32 voice)
{
    struct SpuVoiceRequest request;

    request.voices = ~(1 << voice) & 0xFFFFFF;
    request.mask = 0x13;
    request.pitch = 0;
    request.left = 0;
    request.right = 0;
    gSpuSetVoiceAttributes(&request);
}
