// Categories: Game Engine > Subsystem Initialization
#include "game/types.h"

extern u8 gCvkSystemActive __asm__("D_8009F26C");
extern s32 gActiveStreamId __asm__("D_8009F280");
extern s32 gStreamCommandState __asm__("D_8009F648");
extern s32 gStreamBlockDeficitScale __asm__("D_8009F64C");

extern u8 gScaleAudioStreamBlockDeficit[] __asm__("func_8002443C");

extern s32 gConfigureAudioStreaming(s32 arg0, ...) __asm__("func_80022484");
extern u32 gIsAudioStreamConfigured(void) __asm__("func_800226C8");
extern void gStopAudioStreamAndClearChannels(void) __asm__("func_800226D8");
extern s32 gPrimeAudioStream(void) __asm__("func_80022710");
extern void gStoreStreamChannelMix(s32 arg0, s32 arg1, s32 arg2) __asm__("func_800227BC");
extern void gStartAudioStreamChannels(void) __asm__("func_800228F4");
extern void gShutdownVagmAudioStream(s32 arg0) __asm__("func_80024630");
extern s32 gReplaceVagmAudioStreamBuffer(s32 arg0) __asm__("func_800246A8");
extern void gQueueStreamAssetRelease(s32 arg0, s32 arg1) __asm__("func_8002A31C");
extern s32 gGetStereoSoundMode(void) __asm__("func_800451E0");
extern s32 gGetVagmVolumeSetting(void) __asm__("func_80045288");

s32 gInitializeVagmAudioStream(void) __asm__("func_800244A4");

/**
 * @brief Configures, primes and starts the two-channel VAGM audio stream.
 *
 * @param void No parameters.
 * @return Zero when disabled or on the explicit failure paths; otherwise the value returned by
 * gReplaceVagmAudioStreamBuffer.
 *
 * @par Confirmed mechanics
 * Requires the CVK enable byte, stops an existing stream if configured and sets the stream
 * state and scale. Passes rate 0x37B4, block size 0x2FC0, three blocks and two channels to the
 * configurator. Acquires a buffer, primes it, then selects pans 0 and 0x7F or two centered pans
 * before starting channels.
 *
 * @par Globals
 * Reads gCvkSystemActive, clears or replaces gActiveStreamId and sets gStreamCommandState and
 * gStreamBlockDeficitScale.
 *
 * @par Callers
 * Direct calls from gParseStreamedModuleChunks (func_8002902C).
 *
 * @par Role and path
 * The VAGM module-chunk path initializes the software stream and the SPU playback chain here.
 *
 * @par Unknown
 * The startup helper's channel-allocation result is not propagated.
 */
s32 gInitializeVagmAudioStream(void)
{
    s32 result;

    result = 0;
    if (gCvkSystemActive != 0) {
        if ((gIsAudioStreamConfigured() & 0xFF) != 0) {
            gStopAudioStreamAndClearChannels();
            if (gActiveStreamId != 0) {
                gQueueStreamAssetRelease(gActiveStreamId, result);
                gActiveStreamId = 0;
            }
        }
        gStreamCommandState = 1;
        gStreamBlockDeficitScale = 0x1C2;
        gActiveStreamId = 0;
        if (gConfigureAudioStreaming(1, 0x37B4, 2, 0x2FC0, 3, 3, 10, 2, 6,
                          (s32)(gScaleAudioStreamBlockDeficit + 0x34), 0) < 0) {
            gShutdownVagmAudioStream(0);
        } else {
            result = gReplaceVagmAudioStreamBuffer(4);
            if (result != 0) {
                if (gPrimeAudioStream() == 0) {
                    gShutdownVagmAudioStream(0);
                    return 0;
                }
                if (gGetStereoSoundMode() != 0) {
                    gStoreStreamChannelMix(0, gGetVagmVolumeSetting(), 0);
                    gStoreStreamChannelMix(1, gGetVagmVolumeSetting(), 0x7F);
                } else {
                    gStoreStreamChannelMix(0, gGetVagmVolumeSetting(), 0x40);
                    gStoreStreamChannelMix(1, gGetVagmVolumeSetting(), 0x40);
                }
                gStartAudioStreamChannels();
            }
        }
    }
    return result;
}
