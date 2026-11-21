/* Behavioural checks for the units renamed in naming round 7.
 *
 * The unit under test is the original decomp source, compiled unchanged: the
 * options screen init callback. It claims four of the renamed globals - the
 * three saved volume words and the menu background index - so the checks
 * observe exactly what those new names say: the volumes are snapshotted, and
 * the background index follows the 2x2 state matrix.
 *
 * The audio getters it calls and the session block it reads through inline asm
 * are provided here. The entry point and the globals are bound to their retail
 * labels, so a broken binding fails at link time instead of silently testing
 * the wrong symbol. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

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

/* Audio layer stand-ins. */
static s32 music_volume;
static s32 vagm_volume;
static s32 voice_volume;
static s32 sound_mode;
static s32 stream_active;
static s32 stream_starts;

s32 gGetMusicVolume(void) __asm__("func_8004522C");
s32 gGetMusicVolume(void) { return music_volume; }

s32 gGetVagmVolumeSetting(void) __asm__("func_80045288");
s32 gGetVagmVolumeSetting(void) { return vagm_volume; }

s32 gGetVoiceVolume(void) __asm__("func_800452C8");
s32 gGetVoiceVolume(void) { return voice_volume; }

s32 gGetStereoSoundMode(void) __asm__("func_800451E0");
s32 gGetStereoSoundMode(void) { return sound_mode; }

s32 gIsVagmAudioStreamActive(void) __asm__("func_8002A58C");
s32 gIsVagmAudioStreamActive(void) { return stream_active; }

void gStartVagmAudioStream(s32 id) __asm__("func_8002A16C");
void gStartVagmAudioStream(s32 id) { (void)id; stream_starts++; }

/* --- 8007D18C: prime the options screen --------------------------------- */
void gInitOptionsScreen(void) __asm__("func_8007D18C");

static void prime(void)
{
    music_volume = 0;
    vagm_volume = 0;
    voice_volume = 0;
    sound_mode = 0;
    stream_active = 1;
    stream_starts = 0;
    session.selection = 0;
    session.group = 0;
    gGameSession[0] = &session;
    gGameSession[1] = 0;
}

static void test_volume_snapshot(void)
{
    prime();
    music_volume = 7;
    vagm_volume = 5;
    voice_volume = 3;

    gInitOptionsScreen();
    CHECK(gSavedOptionsSfxVolume == 7, "the music volume is snapshotted into the sfx word");
    CHECK(gSavedOptionsVagmVolume == 5, "the vagm volume is snapshotted");
    CHECK(gSavedOptionsVoiceVolume == 3, "the voice volume is snapshotted");

    /* A second run must overwrite, not accumulate. */
    music_volume = 1;
    vagm_volume = 2;
    voice_volume = 4;
    gInitOptionsScreen();
    CHECK(gSavedOptionsSfxVolume == 1, "a second init overwrites the sfx word");
    CHECK(gSavedOptionsVagmVolume == 2, "a second init overwrites the vagm word");
    CHECK(gSavedOptionsVoiceVolume == 4, "a second init overwrites the voice word");
}

static void test_stream_gate(void)
{
    prime();
    stream_active = 1;
    gInitOptionsScreen();
    CHECK(stream_starts == 0, "a busy stream is not restarted");

    prime();
    stream_active = 0;
    gInitOptionsScreen();
    CHECK(stream_starts == 1, "an idle stream is started once");
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
