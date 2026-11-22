/* Contract checks for the shared mock library itself.
 *
 * The other harnesses use the mocks to test game units. This one tests the
 * mocks, because a mock that lies is worse than no mock at all: it turns a
 * would-be failure into a green run. Each check below pins one of the four rules
 * the library claims in its headers.
 *
 *  1. A stand-in is bound to its retail label. Not testable from here as a
 *     behaviour - a wrong binding is a link error, which is the point - but the
 *     label this file calls is the one the linker resolved, so the calls below
 *     would not link at all if the alias were wrong.
 *  2. Unconfigured returns are 0. Every mock is reset first and asked before
 *     anything is set, so a fabricated default shows up immediately.
 *  3. A mock records what it was given, and only what it was given. The payload
 *     shadow and the SE log are compared against the exact bytes and ids the
 *     call passed in.
 *  4. Unsupported work is loud. `bios_mock`'s dispatcher aborts; the test script
 *     runs this binary a second time to observe that.
 *
 * Nothing here touches a game unit, so it needs no decomp source: it is a pure
 * check of tests/host/mocks/.
 */
#include "fc_testkit.h"
#include "mocks/sound_mock.h"
#include "mocks/gpu_mock.h"
#include "mocks/bios_mock.h"

/* --------------------------------------------------------------- sound mock */

/* Rule 2: a freshly reset mock answers 0 to every getter, so a unit that reads
 * an unconfigured value fails its own check instead of passing on a plausible
 * number. */
static void test_sound_defaults_are_zero(void)
{
    fcm_sound_reset();
    FCK_EQ(gGetMusicVolume(), 0, "music volume defaults to 0");
    FCK_EQ(gGetVagmVolumeSetting(), 0, "vagm volume defaults to 0");
    FCK_EQ(gGetVoiceVolume(), 0, "voice volume defaults to 0");
    FCK_EQ(gGetStereoSoundMode(), 0, "stereo mode defaults to 0");
    FCK_EQ(gIsVagmAudioStreamActive(), 0, "stream is not active by default");
    FCK_EQ(gPlayNonpositionalSoundEffect(7, 0), 0, "the SE result defaults to 0");
    FCK_EQ(fcm_sound.sfx_calls, 1, "the SE call was counted");
}

/* Rule 3: the configured value comes back unchanged, including a negative one -
 * a mock that clamped or masked would be inventing a value. */
static void test_sound_returns_what_was_set(void)
{
    fcm_sound_reset();
    fcm_sound.sfx_result = -5;
    fcm_sound.music_volume = 12;
    fcm_sound.stereo_mode = 1;
    fcm_sound.stream_active = 1;

    FCK_EQ(gPlayNonpositionalSoundEffect(3, 0), -5, "the SE result is passed through");
    FCK_EQ(gGetMusicVolume(), 12, "the music volume is passed through");
    FCK_EQ(gGetStereoSoundMode(), 1, "the stereo mode is passed through");
    FCK_EQ(gIsVagmAudioStreamActive(), 1, "the stream state is passed through");
}

/* Rule 3 for the argument log: ids appear in call order, and the second word is
 * recorded too, so a test can assert which object an SE was played for. */
static void test_sound_records_calls_in_order(void)
{
    s32 i;

    fcm_sound_reset();
    gPlayNonpositionalSoundEffect(11, 0x100);
    gPlayNonpositionalSoundEffect(22, 0x200);
    gPlayNonpositionalSoundEffect(11, 0x300);

    FCK_EQ(fcm_sound.sfx_calls, 3, "three SE calls were counted");
    FCK_EQ(fcm_sound.sfx_ids[0], 11, "the first id is in the log");
    FCK_EQ(fcm_sound.sfx_ids[1], 22, "the second id is in the log");
    FCK_EQ(fcm_sound.sfx_ids[2], 11, "a repeated id appears again");
    FCK_EQ(fcm_sound.sfx_last_arg1, 0x300, "the last second argument is recorded");
    FCK_EQ(fcm_sound.sfx_log_overflow, 0, "the log was large enough");

    /* The overflow flag must be reachable, or a long-running test would be
     * silently truncated and still look green. */
    fcm_sound_reset();
    for (i = 0; i <= FCM_SOUND_LOG_MAX; i++) gPlayNonpositionalSoundEffect(i, 0);
    FCK_EQ(fcm_sound.sfx_calls, FCM_SOUND_LOG_MAX + 1, "all calls are counted");
    FCK_EQ(fcm_sound.sfx_log_overflow, 1, "overrunning the log is flagged");
}

/* The stream starter records its argument and does not claim the stream is now
 * running - the state getter is the test's to set. */
static void test_sound_stream_start_does_not_invent_state(void)
{
    fcm_sound_reset();
    gStartVagmAudioStream(9);
    FCK_EQ(fcm_sound.stream_starts, 1, "the start was counted");
    FCK_EQ(fcm_sound.stream_last_arg0, 9, "the start argument was recorded");
    FCK_EQ(gIsVagmAudioStreamActive(), 0,
           "starting a stream does not fake the stream-state getter");
}

/* ----------------------------------------------------------------- gpu mock */

/* Rule 3: the rectangle and payload are the caller's, not a template. The bytes
 * are compared against the source array, so a mock that stored a scalar or
 * swapped the arguments fails here. */
static void test_gpu_records_rect_and_payload(void)
{
    static const u8 pixels[8] = { 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0 };
    struct FcmGpuRect rect;

    fcm_gpu_reset();
    fcm_gpu.copy_bytes = 8;   /* the mock cannot know the length; the test says */
    rect.x = 320; rect.y = 0; rect.w = 4; rect.h = 2;
    gLoadImage(&rect, (void *)pixels);

    FCK_EQ(fcm_gpu.loads, 1, "one transfer was counted");
    FCK_EQ(fcm_gpu.load[0].rect.x, 320, "the rectangle x is recorded");
    FCK_EQ(fcm_gpu.load[0].rect.w, 4, "the rectangle width is recorded");
    FCK_EQ(fcm_gpu.load[0].rect.h, 2, "the rectangle height is recorded");
    FCK_EQ(fcm_gpu.load[0].copied, 8, "eight payload bytes were shadowed");
    FCK_EQ(fcm_gpu.load[0].bytes[0], 0x12, "the first payload byte is copied");
    FCK_EQ(fcm_gpu.load[0].bytes[7], 0xF0, "the last payload byte is copied");
    FCK_EQ(fcm_gpu.load[0].data, (s32)(unsigned long)pixels,
           "the data pointer is recorded, not dereferenced away");
}

/* A null payload must not be dereferenced, and the copy count must say so. */
static void test_gpu_tolerates_null_payload(void)
{
    fcm_gpu_reset();
    gLoadImage(0, 0);
    FCK_EQ(fcm_gpu.loads, 1, "the transfer was still counted");
    FCK_EQ(fcm_gpu.load[0].copied, 0, "no bytes were claimed for a null payload");
    FCK_EQ(fcm_gpu.load[0].rect.w, 0, "no rectangle was claimed either");
}

/* `capture_rect` exists so a unit that passes some other layout can be counted
 * without the mock reading geometry that is not a rectangle. */
static void test_gpu_can_skip_the_rect(void)
{
    s32 bogus[4];

    bogus[0] = 0x7FFF1234;
    bogus[1] = 0x1111;
    bogus[2] = 0x2222;
    bogus[3] = 0x3333;
    fcm_gpu_reset();
    fcm_gpu.capture_rect = 0;
    gLoadImage(bogus, 0);
    FCK_EQ(fcm_gpu.loads, 1, "the transfer is still counted");
    FCK_EQ(fcm_gpu.load[0].rect.w, 0, "the geometry read is suppressed");
}

static void test_gpu_load_overflow_still_counts_calls(void)
{
    s32 i;
    fcm_gpu_reset();
    for (i = 0; i < FCM_GPU_LOG_MAX + 3; i++) gLoadImage(0, 0);
    FCK_EQ(fcm_gpu.loads, FCM_GPU_LOG_MAX + 3, "overflow transfers are counted");
    FCK_EQ(fcm_gpu.load_overflow, 1, "overflow is visible");
}

/* The sync mode is the only thing a host test can hold a DrawSync caller to. */
static void test_gpu_records_sync_modes(void)
{
    fcm_gpu_reset();
    gDrawSync(0);
    gDrawSync(1);
    FCK_EQ(fcm_gpu.syncs, 2, "two syncs were counted");
    FCK_EQ(fcm_gpu.sync_modes[0], 0, "the first mode is recorded");
    FCK_EQ(fcm_gpu.sync_modes[1], 1, "the second mode is recorded");
    FCK_EQ(fcm_gpu.sync_last_mode, 1, "the last mode is recorded");
}

/* --------------------------------------------------------------- bios mock */

/* Rule 3 for the BIOS: the services really do the string work, so a unit that
 * depends on them produces the right string rather than an empty one. */
static void test_bios_string_services(void)
{
    u8 buf[32];
    s32 i;

    for (i = 0; i < 32; i++) buf[i] = 0xEE;

    fcm_bios_reset();
    gBiosStrcpy(buf, (void *)"abc");
    FCK_EQ(gBiosStrlen(buf), 3, "strcpy then strlen agrees on the length");
    gBiosStrcat(buf, (void *)"de");
    FCK_EQ(buf[0], 'a', "strcpy wrote the first byte");
    FCK_EQ(buf[4], 'e', "strcat appended at the terminator");
    FCK_EQ(buf[5], 0, "strcat left a terminator");
    FCK_EQ(fcm_bios.calls, 3, "three service calls were counted");

    FCK_EQ(gBiosStrcmp((void *)"abc", (void *)"abc"), 0, "strcmp finds a match");
    FCK_EQ(gBiosStrlen((void *)""), 0, "strlen of the empty string is 0");
    FCK_EQ(gBiosToUpper('q'), 'Q', "toupper converts a lower-case letter");
    FCK_EQ(gBiosToLower('Q'), 'q', "tolower converts an upper-case letter");
    FCK_EQ(gBiosToUpper('7'), '7', "toupper leaves a digit alone");
}

/* The service *sequence* is recorded, which is what makes a call-chain
 * assertion possible at all. */
static void test_bios_records_service_sequence(void)
{
    u8 buf[16];

    fcm_bios_reset();
    gBiosStrcpy(buf, (void *)"xy");
    gBiosStrlen(buf);
    gBiosToUpper(buf[0]);

    FCK_EQ(fcm_bios.calls, 3, "three calls are counted");
    FCK_EQ(fcm_bios.services[0], FCM_BIOS_A0_STRCPY, "step 0 is strcpy");
    FCK_EQ(fcm_bios.services[1], FCM_BIOS_A0_STRLEN, "step 1 is strlen");
    FCK_EQ(fcm_bios.services[2], FCM_BIOS_A0_TOUPPER, "step 2 is toupper");
    FCK_EQ(fcm_bios.seq_overflow, 0, "the sequence log was large enough");
}

/* B0:38 is recorded rather than taken, so a unit's fatal path can be asserted;
 * `exit_stops` is the opt-in for a test that wants the run to end. */
static void test_bios_exit_is_recorded_not_taken(void)
{
    fcm_bios_reset();
    fcm_bios.exit_stops = 0;
    gBiosExit(3);
    FCK_EQ(fcm_bios.exit_calls, 1, "the exit was counted");
    FCK_EQ(fcm_bios.exit_code, 3, "the exit code was recorded");
    FCK_EQ(fcm_bios.last_table, 0xB0, "the table was B0");
    FCK_EQ(fcm_bios.last_service, FCM_BIOS_B0_EXIT, "the service was B0:38");
}

/* ------------------------------------------------------- the loud failure */

/* Reached from the test script's second run. Rule 4: an unsupported service must
 * stop the run with the vector named, never return 0. */
static int expect_abort_run(char which)
{
    fck_write("mocks-selftest: probing an unsupported BIOS service\n");
    if (which == 'a') {
        fcm_bios_dispatch(0xA0, 0x99, 0, 0, 0);
    } else if (which == 'b') {
        fcm_bios_dispatch(0xB0, 0x01, 0, 0, 0);
    } else {
        fcm_bios_dispatch(0xC0, 0x00, 0, 0, 0);
    }
    fck_write("FAIL: the unsupported call returned instead of aborting\n");
    return 1;
}

int main(int argc, char **argv)
{
    if (argc > 1) return expect_abort_run(argv[1][0]);

    test_sound_defaults_are_zero();
    test_sound_returns_what_was_set();
    test_sound_records_calls_in_order();
    test_sound_stream_start_does_not_invent_state();

    test_gpu_records_rect_and_payload();
    test_gpu_tolerates_null_payload();
    test_gpu_can_skip_the_rect();
    test_gpu_load_overflow_still_counts_calls();
    test_gpu_records_sync_modes();

    test_bios_string_services();
    test_bios_records_service_sequence();
    test_bios_exit_is_recorded_not_taken();

    return fck_report("mocks-selftest");
}
