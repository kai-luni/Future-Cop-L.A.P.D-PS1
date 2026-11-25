/* Shared sound-layer stand-ins. See sound_mock.h for the contract. */
#include "sound_mock.h"
#include "fcm_report.h"

struct FcmSoundMock fcm_sound;

void fcm_sound_reset(void)
{
    u8 *p = (u8 *)&fcm_sound;
    s32 i;
    for (i = 0; i < (s32)sizeof(fcm_sound); i++) p[i] = 0;
}

/* ---------------------------------------------------------------- SE trigger */

/* func_80046C84 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) -> s32
 *
 * The retail signature, taken from src/game/80046C84.c: the id and one
 * forwarded playback parameter, returning the voice handle (0 = not found).
 * Callers differ only in the C type they pass for arg1 - a game object pointer
 * in most units, a plain word in others - which is the same 32-bit register. */
s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");
s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1)
{
    fcm_sound.sfx_calls++;
    fcm_sound.sfx_last_id = id;
    fcm_sound.sfx_last_arg1 = arg1;
    if (fcm_sound.sfx_calls <= FCM_SOUND_LOG_MAX)
        fcm_sound.sfx_ids[fcm_sound.sfx_calls - 1] = id;
    else
        fcm_sound.sfx_log_overflow = 1;
    return fcm_sound.sfx_result;
}

/* ------------------------------------------------------------- option getters */

/* func_800451E0 gGetStereoSoundMode(void) -> s32 */
s32 gGetStereoSoundMode(void) __asm__("func_800451E0");
s32 gGetStereoSoundMode(void)
{
    fcm_sound.stereo_mode_reads++;
    return fcm_sound.stereo_mode;
}

/* func_8004522C gGetMusicVolume(void) -> s32 */
s32 gGetMusicVolume(void) __asm__("func_8004522C");
s32 gGetMusicVolume(void)
{
    return fcm_sound.music_volume;
}

/* func_80045288 gGetVagmVolumeSetting(void) -> s32 */
s32 gGetVagmVolumeSetting(void) __asm__("func_80045288");
s32 gGetVagmVolumeSetting(void)
{
    return fcm_sound.vagm_volume;
}

/* func_800452C8 gGetVoiceVolume(void) -> s32 */
s32 gGetVoiceVolume(void) __asm__("func_800452C8");
s32 gGetVoiceVolume(void)
{
    return fcm_sound.voice_volume;
}

/* ------------------------------------------------------------- stream control */

/* func_8002A16C gStartVagmAudioStream(s32 arg0) -> void */
void gStartVagmAudioStream(s32 arg0) __asm__("func_8002A16C");
void gStartVagmAudioStream(s32 arg0)
{
    fcm_sound.stream_starts++;
    fcm_sound.stream_last_arg0 = arg0;
}

/* func_8002A58C gIsVagmAudioStreamActive(void) -> s32 */
s32 gIsVagmAudioStreamActive(void) __asm__("func_8002A58C");
s32 gIsVagmAudioStreamActive(void)
{
    return fcm_sound.stream_active;
}
