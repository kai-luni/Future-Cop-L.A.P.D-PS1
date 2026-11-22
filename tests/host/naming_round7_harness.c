/* Behavioural checks for the units renamed in naming round 7.
 *
 * The unit under test is the original decomp source, compiled unchanged: the
 * options screen init callback. It claims four of the renamed globals - the
 * three saved volume words and the menu background index - so the checks
 * observe exactly what those new names say: the volumes are snapshotted, and
 * the background index follows the 2x2 state matrix.
 *
 * The audio layer it calls (the volume getters, the stereo-mode getter, the
 * stream-state query and the stream starter) comes from the shared mock at
 * mocks/sound_mock.c, which defines each of those stand-ins under its retail
 * label. This harness used to carry its own private copies; the assertions
 * below are unchanged, they just read the mock's state now. The entry point
 * and the globals are bound to their retail labels, so a broken binding fails
 * at link time instead of silently testing the wrong symbol. */
/* The mock header brings the shared integer typedefs (mocks/fcm_types.h), so
 * this harness does not repeat them - a second copy would be a redefinition. */
#include "mocks/sound_mock.h"

void __main(void) {}

static int failures;

static void say(const char *text)
{
    int length = 0;
    register long v0 __asm__("$2") = 4004;
    register long a0 __asm__("$4") = 1;
    register long a1 __asm__("$5") = (long)text;
    register long a2 __asm__("$6");
    while (text[length]) length++;
    a2 = length;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2)
                     : "memory", "$7", "$8", "$9", "$10", "$11", "$12", "$13",
                       "$14", "$15", "$24", "$25");
}

#define CHECK(condition, message) do { \
    if (!(condition)) { say("FAIL: " message "\n"); failures++; } \
} while (0)

/* Same, but for a message held in a variable rather than a literal. */
#define CHECK_TEXT(condition, message) do { \
    if (!(condition)) { say("FAIL: "); say(message); say("\n"); failures++; } \
} while (0)

/* --- state the unit reads and writes ------------------------------------- */

struct SessionBlock {
    u8 pad00[0xC];
    s32 selection;
    u8 pad10[0x4C];
    s32 group;
};

static struct SessionBlock session;

/* D_8009FA94 - the session pointer table the unit loads through inline asm. */
struct SessionBlock *gGameSession[2] __asm__("D_8009FA94");

/* The renamed globals, defined under their retail labels. */
s32 gSavedOptionsSfxVolume __asm__("D_8009F908");
s32 gSavedOptionsVagmVolume __asm__("D_8009F90C");
s32 gSavedOptionsVoiceVolume __asm__("D_8009F910");
u8 gAudioOptionsMenuBackgroundIndex[4] __asm__("D_800A377B");

/* The remaining words the unit touches; they are not under test here but the
 * unit writes them, so they need storage. */
s32 gSavedOptionsSoundMode __asm__("D_8009F914");
s32 gTestResultFlag __asm__("D_8009F918");
s32 gSnapshotPointer __asm__("D_8009F91C");
s32 gPreviewTrackId __asm__("D_8009F920");
u8 gSnapshotResult __asm__("D_8009F904");
u16 gSoundWidgetLabel[4] __asm__("D_8009BABE");

/* Audio layer stand-ins live in mocks/sound_mock.c; the harness sets their
 * values through fcm_sound and reads the call counters back from it. */

/* --- 8007D18C: prime the options screen --------------------------------- */
void gInitOptionsScreen(void) __asm__("func_8007D18C");

static void prime(void)
{
    fcm_sound_reset();
    fcm_sound.stream_active = 1;
    session.selection = 0;
    session.group = 0;
    gGameSession[0] = &session;
    gGameSession[1] = 0;
}

static void test_volume_snapshot(void)
{
    prime();
    fcm_sound.music_volume = 7;
    fcm_sound.vagm_volume = 5;
    fcm_sound.voice_volume = 3;

    gInitOptionsScreen();
    CHECK(gSavedOptionsSfxVolume == 7, "the music volume is snapshotted into the sfx word");
    CHECK(gSavedOptionsVagmVolume == 5, "the vagm volume is snapshotted");
    CHECK(gSavedOptionsVoiceVolume == 3, "the voice volume is snapshotted");

    /* A second run must overwrite, not accumulate. */
    fcm_sound.music_volume = 1;
    fcm_sound.vagm_volume = 2;
    fcm_sound.voice_volume = 4;
    gInitOptionsScreen();
    CHECK(gSavedOptionsSfxVolume == 1, "a second init overwrites the sfx word");
    CHECK(gSavedOptionsVagmVolume == 2, "a second init overwrites the vagm word");
    CHECK(gSavedOptionsVoiceVolume == 4, "a second init overwrites the voice word");
}

static void test_stream_gate(void)
{
    prime();
    fcm_sound.stream_active = 1;
    gInitOptionsScreen();
    CHECK(fcm_sound.stream_starts == 0, "a busy stream is not restarted");

    prime();
    fcm_sound.stream_active = 0;
    gInitOptionsScreen();
    CHECK(fcm_sound.stream_starts == 1, "an idle stream is started once");
}

/* The index follows the two state words; only an exact 1 counts as set. */
static struct MatrixCase {
    s32 selection;
    s32 group;
    u8 expected;
    const char *message;
} matrix_cases[6] = {
        { 1, 1, 9, "selection and group both set give 9" },
        { 0, 1, 3, "group set, selection unset give 3" },
        { 1, 0, 1, "selection set, group unset give 1" },
        { 0, 0, 8, "both unset give 8" },
        { 1, 2, 1, "only an exact 1 counts as set in the group word" },
        { 2, 1, 3, "only an exact 1 counts as set in the selection word" }
};

static void test_background_matrix(void)
{
    int i;

    for (i = 0; i < 6; i++) {
        prime();
        session.selection = matrix_cases[i].selection;
        session.group = matrix_cases[i].group;
        gAudioOptionsMenuBackgroundIndex[0] = 0xAA;
        gInitOptionsScreen();
        CHECK_TEXT(gAudioOptionsMenuBackgroundIndex[0] == matrix_cases[i].expected,
                   matrix_cases[i].message);
    }
}

int main(void)
{
    test_volume_snapshot();
    test_stream_gate();
    test_background_matrix();

    if (failures == 0) say("naming round 7: all checks passed\n");
    return failures;
}
