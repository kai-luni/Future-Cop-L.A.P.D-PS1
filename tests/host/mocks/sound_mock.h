/* Shared stand-ins for the PS1 sound layer (retail labels func_80046C84,
 * func_800451E0, func_8004522C, func_80045288, func_800452C8, func_8002A16C,
 * func_8002A58C).
 *
 * Three harnesses used to carry their own private copy of the SE trigger
 * (func_80046C84) and naming round 7 carried a private copy of the whole
 * getter/stream group. They now all link this one object, so a signature
 * disagreement is a single place to fix instead of three.
 *
 * Contract, and why it is what it is:
 *
 *  1. Each stand-in is *defined* under its retail label via an asm alias, so a
 *     wrong binding is a link error rather than a silently different function.
 *  2. The signatures are the retail ones: 32-bit words, SE id then the
 *     forwarded parameter, all getters `-> s32` with no arguments (they only
 *     read a global), the stream starter takes one word.
 *  3. Nothing is invented. Every field of `fcm_sound` starts at 0, which is
 *     what the retail image holds before any code writes it, and it is
 *     `fcm_sound_reset()`'s job to put it back there between cases. A test
 *     that depends on a return value must configure it first; a getter that
 *     was left alone answers 0 and the check fails honestly instead of passing
 *     on a plausible-looking number.
 *  4. The mock only records. It never synthesises audio state: `stream_starts`
 *     counts calls, it does not pretend the stream is now running.
 */
#ifndef FCM_SOUND_MOCK_H
#define FCM_SOUND_MOCK_H

#include "fcm_types.h"

/* Enough to assert "no sound", "one sound", or a short ordered sequence. A
 * test needing more than this would be asserting a log, not behaviour. */
#define FCM_SOUND_LOG_MAX 64

struct FcmSoundMock {
    /* --- values the stand-ins return (all default 0) --- */
    s32 sfx_result;      /* func_80046C84: voice handle, 0 = id not found */
    s32 music_volume;    /* func_8004522C  D_8009F330 */
    s32 vagm_volume;     /* func_80045288  */
    s32 voice_volume;    /* func_800452C8  */
    s32 stereo_mode;     /* func_800451E0  D_8009F730 */
    s32 stream_active;   /* func_8002A58C  D_8009F6DC != 0 */

    /* --- what the stand-ins saw --- */
    s32 sfx_calls;
    s32 sfx_last_id;
    s32 sfx_last_arg1;
    s32 sfx_ids[FCM_SOUND_LOG_MAX];   /* one entry per call, in order */
    s32 sfx_log_overflow;             /* 1 = more calls than the log holds */

    s32 stream_starts;
    s32 stream_last_arg0;
    s32 stream_target_overflow;       /* starts after the first, ignored */

    s32 stereo_mode_reads;            /* func_800451E0 call count */
};

/* One global, so the harness reads `fcm_sound.sfx_ids[0]` directly. */
extern struct FcmSoundMock fcm_sound;

/* Zero every field, including the log. Call it at the top of each case. */
void fcm_sound_reset(void);

/* The stand-ins, each bound to its retail label. A harness calls these by their
 * readable names; the linker resolves them to the retail symbols, so a unit
 * under test that calls func_80046C84 reaches the same code the test does, and
 * a wrong alias is a link error rather than a second, quietly different
 * function. The signatures are the retail ones - 32-bit words throughout. */
s32 gPlayNonpositionalSoundEffect(s32 id, s32 arg1) __asm__("func_80046C84");
s32 gGetStereoSoundMode(void) __asm__("func_800451E0");
s32 gGetMusicVolume(void) __asm__("func_8004522C");
s32 gGetVagmVolumeSetting(void) __asm__("func_80045288");
s32 gGetVoiceVolume(void) __asm__("func_800452C8");
void gStartVagmAudioStream(s32 arg0) __asm__("func_8002A16C");
s32 gIsVagmAudioStreamActive(void) __asm__("func_8002A58C");

#endif /* FCM_SOUND_MOCK_H */
