// Categories: Platform > Audio | Resources > Buffer Management
#include "game/types.h"

extern s32 gActiveStreamId __asm__("D_8009F280");

extern void func_8002A31C(s32 arg0, s32 arg1);
extern s32 func_8002A398(void);
extern void gSetAudioStreamChannelRange(s32 start, s32 length, s32 channel, u8 looping) __asm__("func_80022B28");

s32 gReplaceVagmAudioStreamBuffer(s32 arg0) __asm__("func_800246A8");

/**
 * @brief Releases the current VAGM buffer and installs a queued replacement in two stream
 * channels.
 *
 * @param arg0 Unused argument.
 * @return The newly stored resource value, or zero if none was obtained.
 *
 * @par Confirmed mechanics
 * Releases a prior nonzero buffer with mode 1, obtains the next via func_8002A398 and records
 * it. On success reads header +4 and points channels 1 and 0 at successive 0x2FC0-byte regions
 * beginning at +0x1C.
 *
 * @par Globals
 * Updates gActiveStreamId. The source-range setter updates gMixerState.
 *
 * @par Callers
 * Direct calls from gHandleAudioStreamRefillEvent (func_80024470), gInitializeVagmAudioStream
 * (func_800244A4).
 *
 * @par Role and path
 * Used both for initial VAGM setup and by the refill callback.
 *
 * @par Unknown
 * The header read result and the incoming argument are unused by this function.
 */
s32 gReplaceVagmAudioStreamBuffer(s32 arg0)
{
    s32 *v1;
    u8 *s0;

    s0 = 0;
    if (gActiveStreamId != 0) {
        func_8002A31C(gActiveStreamId, 1);
        gActiveStreamId = 0;
    }
    v1 = (s32 *)func_8002A398();
    gActiveStreamId = (s32)v1;
    if (v1 != 0) {
        (void)*(volatile s32 *)(v1 + 1);
        s0 = (u8 *)v1 + 0x1C;
    }
    if (s0 != 0) {
        gSetAudioStreamChannelRange((s32)(s0 + 0x2FC0), 0x2FC0, 1, 0);
        gSetAudioStreamChannelRange((s32)s0, 0x2FC0, 0, 0);
    }
    return gActiveStreamId;
}
