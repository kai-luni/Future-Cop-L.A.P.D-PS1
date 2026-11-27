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

void gSilenceAllSpuVoices(void) __asm__("func_80021B90");

/**
 * @brief Zeroes the left and right volumes and pitch of all 24 SPU voices.
 *
 * @param void No parameters.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Writes a stack request with voice mask 0xFFFFFF, attribute mask 0x13 and three zero
 * halfwords, then passes its address in a0 to gSpuSetVoiceAttributes.
 *
 * @par Globals
 * None directly. The callee writes the selected voice registers through gSpuRegBasePtr.
 *
 * @par Callers
 * Direct calls from gSoundHaltAllVoices (func_80047554).
 *
 * @par Role and path
 * The audio halt path uses this when it has no voice to spare.
 *
 * @par Unknown
 * The SPU driver determines how these writes affect any samples already in flight.
 */
void gSilenceAllSpuVoices(void)
{
    struct SpuVoiceRequest request;

    request.voices = 0xFFFFFF;
    request.mask = 0x13;
    request.pitch = 0;
    request.left = 0;
    request.right = 0;
    gSpuSetVoiceAttributes(&request);
}
