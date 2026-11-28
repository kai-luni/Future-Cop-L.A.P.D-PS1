/* SPU/sound platform-band leaf suite: five previously uncovered hardware-free
 * units from src/game/platform/, compiled unchanged with gcc 2.7.2 + maspsx and
 * run under qemu-mipsel.
 *
 * Every unit here only builds a small stack record and/or reads and writes
 * plain RAM words, so nothing needs a device mock. Each callee is a software
 * recording stand-in bound to its retail label with __asm__("func_XXXXXXXX");
 * the expectations are literal values taken from the unit's own body (mask
 * 0x3FC3 / volumes 0x3FFF, voice mask 0xFFFFFF / attribute mask 0x13, the four
 * saved-transfer globals, the six-call volume init sequence and the stream
 * table {playing, track, handle} state machine), not from re-running a
 * re-implementation. Every D_ label is defined here and set before each case.
 *
 * No unit in this suite needs tests/host/mocks/sound_mock.c: every sound entry
 * point these five units call is a stand-in below that only records.
 */
#include "leaf_kit.h"

/* ============================================================================
 * Globals referenced by the units (all D_ labels defined by this harness)
 * ============================================================================
 */

/* 80021250 gShutdownSpuTransferQueue */
s32 gSavedSpuTransferCallback __asm__("D_8009F9AC");
s32 gSavedSpuTransferMode __asm__("D_8009FA14");
s32 gSpuTransferActive __asm__("D_8009FA34");
s32 gSpuTransferDispatching __asm__("D_8009FAD0");
s32 gInitDoneFlag __asm__("D_8009F9C8");

/* 80044F8C gInitSoundState */
s32 gVolumeLevels[4] __asm__("D_800A1800");
u8 gStreamAudioFlag __asm__("D_8009F739");
u8 gSoundProcessingEnabled __asm__("D_8009F340");

/* 800471C4 gSetEntityLoopingSound: per-stream {playing, track, handle} */
struct Unk471C4Stream {
    s32 playing;
    s32 track;
    s32 handle;
};
struct Unk471C4Stream gStreamTable[8] __asm__("D_800A1810");

/* ============================================================================
 * Recording stand-ins, each bound to the retail label it replaces
 * ============================================================================
 */

/* --- func_80098FE8: SPU common-attribute setter (80021824) --- */
struct BSpuAttr {
    u32 f0;
    u16 f4;
    u16 f6;
    u16 f8;
    u16 fA;
    u16 fC;
    u16 fE;
    u16 f10;
    u16 f12;
    u32 f14;
    u32 f18;
    u16 f1C;
    u16 f1E;
    u32 f20;
    u32 f24;
};

static struct BSpuAttr s_attr;
static s32 s_attr_calls;
static u32 s_attr_ptr;

static void reset_attr(void)
{
    memset(&s_attr, 0xEE, sizeof(s_attr));
    s_attr_calls = 0;
    s_attr_ptr = 0;
}

s32 mock_spu_common(void *attr) __asm__("func_80098FE8");
s32 mock_spu_common(void *attr)
{
    s_attr_calls++;
    s_attr_ptr = (u32)attr;
    if (attr) memcpy(&s_attr, attr, sizeof(s_attr));
    return 0;
}

/* --- func_80099368: SPU multi-voice attribute setter (80021B90) --- */
struct BSpuVoiceRequest {
    u32 voices;
    u32 mask;
    s16 left;
    s16 right;
    u8 unk0C[8];
    s16 pitch;
    u8 unk16[0x2A];
};

static struct BSpuVoiceRequest s_voice;
static s32 s_voice_calls;
static u32 s_voice_ptr;

static void reset_voice(void)
{
    memset(&s_voice, 0xEE, sizeof(s_voice));
    s_voice_calls = 0;
    s_voice_ptr = 0;
}

void mock_spu_voices(void *request) __asm__("func_80099368");
void mock_spu_voices(void *request)
{
    s_voice_calls++;
    s_voice_ptr = (u32)request;
    if (request) memcpy(&s_voice, request, sizeof(s_voice));
}

/* --- func_8002129C / func_80098F48 / func_80098FB8: transfer queue (80021250) --- */
static struct {
    s32 cancel_calls;
    s32 mode_calls;
    s32 mode_value;
    s32 cb_calls;
    s32 cb_value;
    s32 cb_ret;
} s_xfer;

static void reset_xfer(void)
{
    memset(&s_xfer, 0, sizeof(s_xfer));
    fck_log_reset();
}

void mock_xfer_cancel(void) __asm__("func_8002129C");
void mock_xfer_cancel(void)
{
    s_xfer.cancel_calls++;
    fck_record("func_8002129C", 0, 0, 0);
}

void mock_xfer_mode(s32 value) __asm__("func_80098F48");
void mock_xfer_mode(s32 value)
{
    s_xfer.mode_calls++;
    s_xfer.mode_value = value;
    fck_record("func_80098F48", value, 0, 0);
}

s32 mock_xfer_callback(s32 callback) __asm__("func_80098FB8");
s32 mock_xfer_callback(s32 callback)
{
    s_xfer.cb_calls++;
    s_xfer.cb_value = callback;
    fck_record("func_80098FB8", callback, 0, 0);
    return s_xfer.cb_ret;
}

/* --- func_800451B4/EC/F8, 80045238, 80045294, 800453B8: volume init (80044F8C) --- */
void mock_stereo_mode(s32 value) __asm__("func_800451B4");
void mock_stereo_mode(s32 value)
{
    fck_record("func_800451B4", value, 0, 0);
}

void mock_spatial_mode(s32 value) __asm__("func_800451EC");
void mock_spatial_mode(s32 value)
{
    fck_record("func_800451EC", value, 0, 0);
}

void mock_sfx_volume(s32 value) __asm__("func_800451F8");
void mock_sfx_volume(s32 value)
{
    fck_record("func_800451F8", value, 0, 0);
}

void mock_vagm_volume(s32 value) __asm__("func_80045238");
void mock_vagm_volume(s32 value)
{
    fck_record("func_80045238", value, 0, 0);
}

void mock_voice_volume(s32 value) __asm__("func_80045294");
void mock_voice_volume(s32 value)
{
    fck_record("func_80045294", value, 0, 0);
}

void mock_init_pools(void) __asm__("func_800453B8");
void mock_init_pools(void)
{
    fck_record("func_800453B8", 0, 0, 0);
}

/* --- func_800243F8 / func_80046E88: looping stream start/stop (800471C4) --- */
static struct {
    s32 stop_calls;
    s32 stop_handle;
    s32 play_calls;
    s32 play_track;
    s32 play_a1;
    s32 play_a2;
    s32 play_a3;
    s32 play_handle;
    void *play_object;
} s_loop;

static void reset_loop(void)
{
    memset(&s_loop, 0, sizeof(s_loop));
    fck_log_reset();
}

void mock_stop_voice(s32 handle) __asm__("func_800243F8");
void mock_stop_voice(s32 handle)
{
    s_loop.stop_calls++;
    s_loop.stop_handle = handle;
    fck_record("func_800243F8", handle, 0, 0);
}

void mock_play_on_voice(s32 track, s32 a1, s32 a2, s32 a3, s32 handle, void *object)
    __asm__("func_80046E88");
void mock_play_on_voice(s32 track, s32 a1, s32 a2, s32 a3, s32 handle, void *object)
{
    s_loop.play_calls++;
    s_loop.play_track = track;
    s_loop.play_a1 = a1;
    s_loop.play_a2 = a2;
    s_loop.play_a3 = a3;
    s_loop.play_handle = handle;
    s_loop.play_object = object;
    fck_record("func_80046E88", track, handle, a3);
}

/* ============================================================================
 * 1. 80021824 - gInitSpuCommonAttributes
 * ============================================================================
 */
extern void gInitSpuCommonAttributes(void) __asm__("func_80021824");

static void test_80021824(void)
{
    /* @leaf 80021824 source one 0x28-byte attribute record: mask 0x3FC3, both master volumes 0x3FFF, all other fields 0 */
    LK_FUNC("func_80021824");

    reset_attr();
    gInitSpuCommonAttributes();

    FCK_EQ(s_attr_calls, 1, "attributes setter called exactly once");
    FCK_EQ(s_attr_ptr != 0, 1, "record address passed in a0");
    FCK_EQ((s32)sizeof(struct BSpuAttr), 0x28, "record is 0x28 bytes");

    FCK_EQ(s_attr.f0, 0x3FC3, "attribute mask 0x3FC3");
    FCK_EQ(s_attr.f4, 0x3FFF, "left master volume 0x3FFF");
    FCK_EQ(s_attr.f6, 0x3FFF, "right master volume 0x3FFF");
    FCK_EQ(s_attr.f8, 0, "f8 zero");
    FCK_EQ(s_attr.fA, 0, "fA zero");
    FCK_EQ(s_attr.fC, 0, "fC zero");
    FCK_EQ(s_attr.fE, 0, "fE zero");
    FCK_EQ(s_attr.f10, 0, "f10 zero");
    FCK_EQ(s_attr.f12, 0, "f12 zero");
    FCK_EQ(s_attr.f14, 0, "f14 zero");
    FCK_EQ(s_attr.f18, 0, "f18 zero");
    FCK_EQ(s_attr.f1C, 0, "f1C zero");
    FCK_EQ(s_attr.f1E, 0, "f1E zero");
    FCK_EQ(s_attr.f20, 0, "f20 zero");
    FCK_EQ(s_attr.f24, 0, "f24 zero");
}

/* ============================================================================
 * 2. 80021B90 - gSilenceAllSpuVoices
 * ============================================================================
 */
extern void gSilenceAllSpuVoices(void) __asm__("func_80021B90");

static void test_80021B90(void)
{
    /* @leaf 80021B90 source one voice-attribute record: all 24 voices (0xFFFFFF), mask 0x13, zero L/R volume and zero pitch */
    LK_FUNC("func_80021B90");

    reset_voice();
    gSilenceAllSpuVoices();

    FCK_EQ(s_voice_calls, 1, "voice attribute setter called exactly once");
    FCK_EQ(s_voice_ptr != 0, 1, "request address passed in a0");
    FCK_EQ((s32)sizeof(struct BSpuVoiceRequest), 0x40, "request is 0x40 bytes");

    FCK_EQ(s_voice.voices, 0xFFFFFF, "voice bitmask selects all 24 voices");
    FCK_EQ(s_voice.mask, 0x13, "attribute mask 0x13 (VOL L, VOL R, pitch)");
    FCK_EQ(s_voice.left, 0, "left volume 0");
    FCK_EQ(s_voice.right, 0, "right volume 0");
    FCK_EQ(s_voice.pitch, 0, "pitch 0");
}

/* ============================================================================
 * 3. 80021250 - gShutdownSpuTransferQueue
 * ============================================================================
 */
extern void gShutdownSpuTransferQueue(void) __asm__("func_80021250");

static const char *kNoCalls[] = { 0 };

static const char *kShutdownOrder[] = {
    "func_8002129C", "func_80098F48", "func_80098FB8"
};

static void test_80021250(void)
{
    /* @leaf 80021250 source init-flag gated cancel/mode/callback restore in that order, then unconditional clear of active, dispatch guard and flag */
    LK_FUNC("func_80021250");

    /* Initialized queue: cancel, restore saved mode, restore saved callback. */
    reset_xfer();
    gSavedSpuTransferMode = 0x2A;
    gSavedSpuTransferCallback = 0x5A5A;
    gSpuTransferActive = 7;
    gSpuTransferDispatching = 9;
    gInitDoneFlag = 1;

    gShutdownSpuTransferQueue();
    FCK_EQ(s_xfer.cancel_calls, 1, "cancel called when init flag set");
    FCK_EQ(s_xfer.mode_calls, 1, "transfer mode restored once");
    FCK_EQ(s_xfer.mode_value, 0x2A, "restored saved transfer mode 0x2A");
    FCK_EQ(s_xfer.cb_calls, 1, "transfer callback restored once");
    FCK_EQ(s_xfer.cb_value, 0x5A5A, "restored saved callback 0x5A5A");
    fck_expect_order(kShutdownOrder, 3, "shutdown restore sequence");
    FCK_EQ(gSpuTransferActive, 0, "active word cleared");
    FCK_EQ(gSpuTransferDispatching, 0, "dispatch guard cleared");
    FCK_EQ(gInitDoneFlag, 0, "init flag cleared");
    FCK_EQ(gSavedSpuTransferMode, 0x2A, "saved mode left untouched");
    FCK_EQ(gSavedSpuTransferCallback, 0x5A5A, "saved callback left untouched");

    /* Any non-zero flag (not only 1) runs the restore path. */
    reset_xfer();
    gSavedSpuTransferMode = -3;
    gSavedSpuTransferCallback = -4;
    gSpuTransferActive = 1;
    gSpuTransferDispatching = 2;
    gInitDoneFlag = 2;

    gShutdownSpuTransferQueue();
    FCK_EQ(s_xfer.cancel_calls, 1, "flag=2 still cancels");
    FCK_EQ(s_xfer.mode_value, -3, "flag=2 restores signed mode -3");
    FCK_EQ(s_xfer.cb_value, -4, "flag=2 restores signed callback -4");
    FCK_EQ(gSpuTransferActive, 0, "flag=2 clears active word");
    FCK_EQ(gSpuTransferDispatching, 0, "flag=2 clears dispatch guard");
    FCK_EQ(gInitDoneFlag, 0, "flag=2 clears init flag");

    /* Uninitialized queue: no restore calls, but the three words still clear. */
    reset_xfer();
    gSavedSpuTransferMode = 0x11;
    gSavedSpuTransferCallback = 0x22;
    gSpuTransferActive = 5;
    gSpuTransferDispatching = 6;
    gInitDoneFlag = 0;

    gShutdownSpuTransferQueue();
    FCK_EQ(s_xfer.cancel_calls, 0, "flag=0 makes no cancel call");
    FCK_EQ(s_xfer.mode_calls, 0, "flag=0 makes no mode restore");
    FCK_EQ(s_xfer.cb_calls, 0, "flag=0 makes no callback restore");
    FCK_EQ(fck_log_count, 0, "flag=0 records no call at all");
    FCK_EQ(gSpuTransferActive, 0, "flag=0 still clears active word");
    FCK_EQ(gSpuTransferDispatching, 0, "flag=0 still clears dispatch guard");
    FCK_EQ(gInitDoneFlag, 0, "flag=0 keeps flag cleared");
    FCK_EQ(gSavedSpuTransferMode, 0x11, "flag=0 leaves saved mode alone");
    FCK_EQ(gSavedSpuTransferCallback, 0x22, "flag=0 leaves saved callback alone");
}

/* ============================================================================
 * 4. 80044F8C - gInitSoundState
 * ============================================================================
 */
extern void gInitSoundState(void) __asm__("func_80044F8C");

static const char *kInitOrder[] = {
    "func_800451B4", "func_800451EC", "func_800451F8",
    "func_80045238", "func_80045294", "func_800453B8"
};

static void test_80044F8C(void)
{
    /* @leaf 80044F8C source six-call init sequence (1, 0, 0x40, 0x40, 0x40, no arg) then slot list head -1 and two flags */
    LK_FUNC("func_80044F8C");

    fck_log_reset();
    gVolumeLevels[0] = 0x77;
    gVolumeLevels[1] = 0x1234;
    gStreamAudioFlag = 0xAA;
    gSoundProcessingEnabled = 0x55;

    gInitSoundState();

    fck_expect_order(kInitOrder, 6, "sound init call order");
    FCK_EQ(fck_call_n("func_800451B4", 0).a0, 1, "stereo mode enabled with 1");
    FCK_EQ(fck_call_n("func_800451EC", 0).a0, 0, "spatial mode set to 0");
    FCK_EQ(fck_call_n("func_800451F8", 0).a0, 0x40, "SFX volume 0x40");
    FCK_EQ(fck_call_n("func_80045238", 0).a0, 0x40, "VAGM volume 0x40");
    FCK_EQ(fck_call_n("func_80045294", 0).a0, 0x40, "voice volume 0x40");

    FCK_EQ(gVolumeLevels[0], -1, "slot list head marked empty with -1");
    FCK_EQ(gVolumeLevels[1], 0x1234, "second slot word untouched");
    FCK_EQ(gStreamAudioFlag, 0, "stream audio flag cleared");
    FCK_EQ(gSoundProcessingEnabled, 1, "sound processing enabled");
}

/* ============================================================================
 * 5. 800471C4 - gSetEntityLoopingSound
 * ============================================================================
 */
struct Unk471C4Info {
    u8 unk00[0xC1];
    s8 stream;
};

struct Unk471C4Object {
    u8 unk00[0xAC];
    struct Unk471C4Info *info;
};

extern void gSetEntityLoopingSound(s32 track, struct Unk471C4Object *object)
    __asm__("func_800471C4");

static const char *kLoopStopPlay[] = { "func_800243F8", "func_80046E88" };
static const char *kLoopStopOnly[] = { "func_800243F8" };
static const char *kLoopPlayOnly[] = { "func_80046E88" };

static void test_800471C4(void)
{
    struct Unk471C4Info info;
    struct Unk471C4Object object;

    /* @leaf 800471C4 source stream-indexed state machine: equal-track early return, playing stop, track record, 0xFF stop-only, otherwise play and mark playing */
    LK_FUNC("func_800471C4");

    memset(&info, 0, sizeof(info));
    memset(&object, 0, sizeof(object));
    memset(gStreamTable, 0, sizeof(gStreamTable));
    object.info = &info;

    /* Same track as the stream already holds, while playing: nothing happens. */
    info.stream = 2;
    gStreamTable[2].playing = 1;
    gStreamTable[2].track = 7;
    gStreamTable[2].handle = 0x111;
    reset_loop();
    gSetEntityLoopingSound(7, &object);
    fck_expect_order(kNoCalls, 0, "equal track makes no call");
    FCK_EQ(gStreamTable[2].playing, 1, "equal track keeps playing");
    FCK_EQ(gStreamTable[2].track, 7, "equal track keeps track");
    FCK_EQ(gStreamTable[2].handle, 0x111, "equal track keeps handle");

    /* Different track, stream still playing: stop old handle, record track, play. */
    gStreamTable[2].playing = 5;
    gStreamTable[2].track = 7;
    gStreamTable[2].handle = 0x1234;
    reset_loop();
    gSetEntityLoopingSound(9, &object);
    fck_expect_order(kLoopStopPlay, 2, "switch stops then plays");
    FCK_EQ(s_loop.stop_calls, 1, "old handle stopped once");
    FCK_EQ(s_loop.stop_handle, 0x1234, "stopped stream handle 0x1234");
    FCK_EQ(s_loop.play_calls, 1, "new track played once");
    FCK_EQ(s_loop.play_track, 9, "played track 9");
    FCK_EQ(s_loop.play_a1, 0, "play arg1 = 0");
    FCK_EQ(s_loop.play_a2, 0, "play arg2 = 0");
    FCK_EQ(s_loop.play_a3, -1, "play arg3 = -1");
    FCK_EQ(s_loop.play_handle, 0x1234, "play reuses stream handle");
    FCK_EQ((u32)s_loop.play_object, (u32)&object, "play receives the entity");
    FCK_EQ(gStreamTable[2].track, 9, "stream track updated to 9");
    FCK_EQ(gStreamTable[2].playing, 1, "stream marked playing");

    /* Different track on an idle stream: no stop call, straight to play. */
    gStreamTable[0].playing = 0;
    gStreamTable[0].track = 3;
    gStreamTable[0].handle = 0x42;
    info.stream = 0;
    reset_loop();
    gSetEntityLoopingSound(4, &object);
    fck_expect_order(kLoopPlayOnly, 1, "idle stream only plays");
    FCK_EQ(s_loop.stop_calls, 0, "idle stream skips stop");
    FCK_EQ(s_loop.play_calls, 1, "idle stream plays once");
    FCK_EQ(s_loop.play_track, 4, "idle stream plays track 4");
    FCK_EQ(s_loop.play_handle, 0x42, "idle stream uses its own handle");
    FCK_EQ(gStreamTable[0].track, 4, "idle stream track updated");
    FCK_EQ(gStreamTable[0].playing, 1, "idle stream marked playing");

    /* Track 0xFF on a playing stream: stop, clear playing, record 0xFF, no play. */
    gStreamTable[5].playing = 3;
    gStreamTable[5].track = 8;
    gStreamTable[5].handle = 0x777;
    info.stream = 5;
    reset_loop();
    gSetEntityLoopingSound(0xFF, &object);
    fck_expect_order(kLoopStopOnly, 1, "0xFF stops without playing");
    FCK_EQ(s_loop.stop_calls, 1, "0xFF stops the active handle");
    FCK_EQ(s_loop.stop_handle, 0x777, "0xFF stops stream 5 handle 0x777");
    FCK_EQ(s_loop.play_calls, 0, "0xFF starts no new track");
    FCK_EQ(gStreamTable[5].track, 0xFF, "0xFF recorded as current track");
    FCK_EQ(gStreamTable[5].playing, 0, "0xFF leaves stream not playing");

    /* Track 0xFF on an idle stream: no calls at all, track still recorded. */
    gStreamTable[5].playing = 0;
    gStreamTable[5].track = 1;
    gStreamTable[5].handle = 0x777;
    reset_loop();
    gSetEntityLoopingSound(0xFF, &object);
    fck_expect_order(kNoCalls, 0, "idle 0xFF makes no call");
    FCK_EQ(gStreamTable[5].track, 0xFF, "idle 0xFF records track");
    FCK_EQ(gStreamTable[5].playing, 0, "idle 0xFF keeps playing clear");
}

int main(void)
{
    test_80021824();
    test_80021B90();
    test_80021250();
    test_80044F8C();
    test_800471C4();
    return lk_done("leaf_spu_r3");
}
