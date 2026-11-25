/* Source characterizations of unchanged src/game object/state helper units
 * on synthetic RAM. Every fixture and global is initialized explicitly
 * before each check; no reliance on retail initial memory.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"
/* @mock sound_mock */
/* func_8002A16C (VAGM stream starter) comes from the shared mock library;
 * its calls are read back from fcm_sound.stream_starts/stream_last_arg0. */
#include "sound_mock.h"

/* =========================================================================
 * Shared globals (defined with retail D_ labels, zero-initialized by default
 * and explicitly reset inside each test).
 * ========================================================================= */

/* Streaming / portrait / ring queue state */
u8  gBitFieldState               __asm__("D_8009F6B8");
s32 gOutStreamId                 __asm__("D_8009F6BC");
s32 gPortraitStreamDeadline      __asm__("D_8009F6C4");
s32 gVagmReadIndex               __asm__("D_8009F6CC");
s32 gStreamAssetReleaseReadIndex __asm__("D_8009F6D0");
volatile s32 gStreamAssetReleaseWriteIndex __asm__("D_8009F6D8");
s32 gCallGateFlag                __asm__("D_8009F6DC");
u8  gVagmQueueDraining           __asm__("D_8009F308");
s32 gGameSuspended               __asm__("D_8009FA24");
s32 gFrameTickCounter            __asm__("D_8009FA2C");
u8  gStreamStartPending[4]       __asm__("D_800A0B90");
s32 gVagmRing[4]                 __asm__("D_800A0C50");
s32 gStreamAssetReleaseRing[4]   __asm__("D_800A0C60");
s32 gActiveStreamId              __asm__("D_8009F280");

/* Spawn descriptor and spawn record tables */
u8   *gSlotSpawnDescs[32]        __asm__("D_800A0C70");
u32   gSpawnTableRow             __asm__("D_8009F310");
void *gSpawnRecordMap[4][0x100]  __asm__("D_800A7040");

/* CVK sound bank, program sample table, and tracked voice handle */
struct SoundSample {
    u8 program;
    u8 unk1;
    s8 vab;
    u8 unk3;
};

struct SoundBank {
    struct SoundSample samples[4];
    u8 count;
    u8 cursor;
    u8 voices;
    u8 unk13;
    u8 unk14;
    u8 unk15;
    u8 key;
    u8 unk17;
    void *head;
};

struct VabEntry {
    s8 id;
    s8 status;
    s8 pad2;
    s8 pad3;
    void *block4;
    void *block8;
    void *blockC;
};

struct TrackedResource {
    u8 unk00[0xD];
    s8 owner;
    u8 unk0E[2];
    s32 serial;
};

struct TrackedHandle {
    struct TrackedResource *resource;
    s32 serial;
    s32 state;
    s32 id;
    s32 time;
    s32 delay;
    u8  flag;
    u8  pad19[3];
};

struct VoiceParams {
    s32 pitch;
    s32 volume;
    s32 pan;
};

u8 gCvkSystemActive              __asm__("D_8009F26C");
s32 gSpuBufferPtr                __asm__("D_8009F278");
void *gCshdNodeSlots[2]          __asm__("D_8009F640");
s32 gCvkSpuUploadComplete        __asm__("D_8009F650");
struct VabEntry gObjectSlots[8]  __asm__("D_800A0A00");
struct TrackedHandle gAssetHandle __asm__("D_800A0A80");
struct SoundBank gSoundEntryTable[110] __asm__("D_800A7938");

/* =========================================================================
 * External stand-ins (bound by retail func_ label, record calls and return
 * test-configured values; exception: mock_remove_sample for func_800242B4
 * shifts samples[] and decrements count when mock_compact_on_remove is set
 * so gClearSoundProgramSamples's drain loop terminates and observes each
 * compacted head).
 * ========================================================================= */

static u8 mock_draining_during_cb;
void mock_cb_80029650(s32 arg0) __asm__("func_80029650");
void mock_cb_80029650(s32 arg0)
{
    mock_draining_during_cb = gVagmQueueDraining;
    fck_record("cb_80029650", arg0, 0, 0);
}

void mock_stop_portrait(void) __asm__("func_80029DD8");
void mock_stop_portrait(void)
{
    fck_record("stop_portrait", 0, 0, 0);
}

static s32 mock_vagm_chunk_ret;
s32 mock_dequeue_vagm(void) __asm__("func_8002A398");
s32 mock_dequeue_vagm(void)
{
    fck_record("dequeue_vagm", 0, 0, 0);
    return mock_vagm_chunk_ret;
}

void mock_set_range(s32 start, s32 length, s32 channel, u8 looping) __asm__("func_80022B28");
void mock_set_range(s32 start, s32 length, s32 channel, u8 looping)
{
    fck_record("set_range", start, length, (channel << 8) | (s32)looping);
}

static s32 mock_rng_val;
s32 mock_rng(void) __asm__("func_80019668");
s32 mock_rng(void)
{
    fck_record("rng", mock_rng_val, 0, 0);
    return mock_rng_val;
}

void mock_select_clip(void *track, s32 index, s32 direction, s32 flags) __asm__("func_800448D8");
void mock_select_clip(void *track, s32 index, s32 direction, s32 flags)
{
    fck_record("select_clip", (s32)track, index, (direction << 8) | flags);
}

static u8 *mock_alloc_ptr;
u8 *mock_alloc(s32 size, s32 type) __asm__("func_80017EBC");
u8 *mock_alloc(s32 size, s32 type)
{
    fck_record("alloc", size, type, 0);
    return mock_alloc_ptr;
}

void mock_free(void *block) __asm__("func_800181D4");
void mock_free(void *block)
{
    fck_record("free", (s32)block, 0, 0);
}

static u32 mock_spawn_flag_word;
u32 *mock_get_flag_ptr(u32 column, u32 row) __asm__("func_8003B3D8");
u32 *mock_get_flag_ptr(u32 column, u32 row)
{
    fck_record("flag_ptr", (s32)column, (s32)row, 0);
    return &mock_spawn_flag_word;
}

struct CshdEntry { u16 id, count, offset; };
struct CshdHeader { u16 unk, count; struct CshdEntry entries[4]; };
struct CshdItem { u16 key; u8 value; u8 pad[9]; };

static struct CshdHeader *mock_cshd_header;
static s32 mock_cshd_index;
struct CshdHeader *mock_find_cshd(s32 id, s32 *out_index) __asm__("func_80023528");
struct CshdHeader *mock_find_cshd(s32 id, s32 *out_index)
{
    fck_record("find_cshd", id, 0, 0);
    if (out_index) *out_index = mock_cshd_index;
    return mock_cshd_header;
}

static void *mock_append_items[8];
static int mock_append_item_count;
s32 mock_append_sample(s32 key, s32 val, s32 group, s32 bank, void *item) __asm__("func_800241D0");
s32 mock_append_sample(s32 key, s32 val, s32 group, s32 bank, void *item)
{
    if (mock_append_item_count < 8) mock_append_items[mock_append_item_count++] = item;
    fck_record("append_sample", key, val, ((group & 0xFF) << 8) | (bank & 0xFF));
    return 0;
}

static int mock_compact_on_remove;
void mock_remove_sample(s32 slot, s32 program) __asm__("func_800242B4");
void mock_remove_sample(s32 slot, s32 program)
{
    fck_record("remove_sample", slot, program, 0);
    if (mock_compact_on_remove && slot >= 0 && slot < 110 && gSoundEntryTable[slot].count > 0) {
        int k;
        for (k = 0; k < 3; k++) {
            gSoundEntryTable[slot].samples[k] = gSoundEntryTable[slot].samples[k + 1];
        }
        gSoundEntryTable[slot].count--;
    }
}

void mock_unreg_bank(s32 index) __asm__("func_80021DAC");
void mock_unreg_bank(s32 index)
{
    fck_record("unreg_bank", index, 0, 0);
}

void mock_reset_banks(void) __asm__("func_80044EF8");
void mock_reset_banks(void)
{
    fck_record("reset_banks", 0, 0, 0);
}

void mock_release_owner(s32 owner) __asm__("func_80022240");
void mock_release_owner(s32 owner)
{
    fck_record("release_owner", owner, 0, 0);
}

void mock_mark_stopped(void) __asm__("func_80029720");
void mock_mark_stopped(void)
{
    fck_record("mark_stopped", 0, 0, 0);
}

void mock_clear_vol_limit(void) __asm__("func_80045370");
void mock_clear_vol_limit(void)
{
    fck_record("clear_vol_limit", 0, 0, 0);
}

s32 mock_release_handle(struct TrackedHandle *h) __asm__("func_800474F0");
s32 mock_release_handle(struct TrackedHandle *h)
{
    fck_record("release_handle", (s32)h, 0, 0);
    return 0;
}

static s32 mock_mix_ok;
static s8  mock_mix0, mock_mix1;
s32 mock_compute_mix(s32 id, s8 *mix) __asm__("func_800450A4");
s32 mock_compute_mix(s32 id, s8 *mix)
{
    fck_record("compute_mix", id, 0, 0);
    mix[0] = mock_mix0;
    mix[1] = mock_mix1;
    return mock_mix_ok;
}

static s32 mock_validate_ret;
s32 mock_validate_handle(struct TrackedHandle *h) __asm__("func_800474C4");
s32 mock_validate_handle(struct TrackedHandle *h)
{
    fck_record("validate_handle", (s32)h, 0, 0);
    return mock_validate_ret;
}

static s32 mock_update_mix_extra;
void mock_update_mix(void *voice, s32 pan, s32 volume, s32 extra) __asm__("func_80047800");
void mock_update_mix(void *voice, s32 pan, s32 volume, s32 extra)
{
    mock_update_mix_extra = extra;
    fck_record("update_mix", (s32)voice, pan, volume);
}

static s8 mock_default_vol;
s8 mock_compute_vol(void) __asm__("func_80045050");
s8 mock_compute_vol(void)
{
    fck_record("compute_vol", 0, 0, 0);
    return mock_default_vol;
}

static s32 mock_alloc_voice_ret;
static struct VoiceParams mock_alloc_params;
s32 mock_alloc_voice(s32 prio, s32 owner, struct VoiceParams *params, s32 spu_ptr) __asm__("func_800220C4");
s32 mock_alloc_voice(s32 prio, s32 owner, struct VoiceParams *params, s32 spu_ptr)
{
    mock_alloc_params = *params;
    fck_record("alloc_voice", prio, owner, spu_ptr);
    return mock_alloc_voice_ret;
}

void mock_queue_start(s32 count, s32 *channels) __asm__("func_800221E4");
void mock_queue_start(s32 count, s32 *channels)
{
    fck_record("queue_start", count, channels ? *channels : -999, 0);
}

/* =========================================================================
 * Declarations of the 18 units under test
 * ========================================================================= */
void gUpdatePortraitStream(void) __asm__("func_8002A26C");
void gQueueStreamAssetRelease(s32 arg0, s32 arg1) __asm__("func_8002A31C");
void gDrainCallbackSlotRing(void) __asm__("func_8002A408");
void gDrainVagmRing(void) __asm__("func_8002A49C");
void gResetVagmStreamState(void) __asm__("func_8002A544");
void gHandleAudioStreamRefillEvent(s32 arg0) __asm__("func_80024470");
s32  gReplaceVagmAudioStreamBuffer(s32 arg0) __asm__("func_800246A8");
void gSetType26AnimationClip(u8 *arg0, u32 arg1) __asm__("func_8002B0EC");
void gSetSlotSpawnDescriptor(u8 *arg0) __asm__("func_8003BC7C");
void gFreeIntrusiveNodePoolStorage(void *pool) __asm__("func_8003BDEC");
void gInstallSpawnRecord(void *arg0) __asm__("func_8003C404");
s32  gInstallCvkGroupSamples(s32 id) __asm__("func_800235D4");
void gClearSoundProgramSamples(void) __asm__("func_80023A24");
void gUnloadCvkSoundBank(s32 arg0) __asm__("func_80023AB4");
void gReleaseCvkSoundBanksAndVoices(void) __asm__("func_80023BF8");
void gUpdateTrackedSoundVoice(void) __asm__("func_80023D78");
void gStopTrackedSoundVoice(void) __asm__("func_80023E84");
s32  gStartTrackedSoundVoice(void) __asm__("func_80023F08");

/* =========================================================================
 * Group 1: Streaming / Ring Queue Helpers
 * ========================================================================= */

static void test_stream_rings(void)
{
    struct FckCall c;

    /* @leaf 8002A31C source src/game/8002A31C.c: 4-slot ring insert with wrap and occupied-slot callback */
    LK_FUNC("func_8002A31C");
    lk_fill(gStreamAssetReleaseRing, 0, sizeof(gStreamAssetReleaseRing));
    gStreamAssetReleaseWriteIndex = 2;
    fck_log_reset();
    gQueueStreamAssetRelease(0x1111, 0);
    FCK_EQ(gStreamAssetReleaseRing[2], 0x1111, "stored in slot 2");
    FCK_EQ(gStreamAssetReleaseWriteIndex, 3, "write index advances 2->3");
    FCK_EQ(fck_calls_to("cb_80029650"), 0, "empty slot does not trigger overflow callback");

    gQueueStreamAssetRelease(0x2222, 1);
    FCK_EQ(gStreamAssetReleaseRing[3], 0x2222, "stored in slot 3");
    FCK_EQ(gStreamAssetReleaseWriteIndex, 0, "write index wraps 3->0");

    gStreamAssetReleaseRing[0] = 0x9999;
    gQueueStreamAssetRelease(0x3333, 0);
    FCK_EQ(gStreamAssetReleaseRing[0], 0x9999, "occupied slot not overwritten");
    FCK_EQ(gStreamAssetReleaseWriteIndex, 0, "occupied slot leaves write index unchanged");
    FCK_EQ(fck_calls_to("cb_80029650"), 1, "occupied slot invokes callback once");

    /* @leaf 8002A408 source src/game/8002A408.c: drain consecutive nonzero ring slots with wrap and stop at zero */
    LK_FUNC("func_8002A408");
    gStreamAssetReleaseRing[0] = 0xAA;
    gStreamAssetReleaseRing[1] = 0;
    gStreamAssetReleaseRing[2] = 0xBB;
    gStreamAssetReleaseRing[3] = 0xCC;
    gStreamAssetReleaseReadIndex = 2;
    fck_log_reset();
    gDrainCallbackSlotRing();
    FCK_EQ(fck_calls_to("cb_80029650"), 3, "drains slots 2, 3, and wrapped 0");
    c = fck_call_n("cb_80029650", 0); FCK_EQ(c.a0, 0xBB, "first callback gets slot 2");
    c = fck_call_n("cb_80029650", 1); FCK_EQ(c.a0, 0xCC, "second callback gets slot 3");
    c = fck_call_n("cb_80029650", 2); FCK_EQ(c.a0, 0xAA, "third callback gets wrapped slot 0");
    FCK_EQ(gStreamAssetReleaseReadIndex, 1, "read index stops at zero slot 1");
    FCK_CHECK(gStreamAssetReleaseRing[0] == 0 && gStreamAssetReleaseRing[2] == 0 &&
              gStreamAssetReleaseRing[3] == 0, "drained slots cleared");

    fck_log_reset();
    gDrainCallbackSlotRing();
    FCK_EQ(fck_calls_to("cb_80029650"), 0, "already-empty slot is a no-op");
    FCK_EQ(gStreamAssetReleaseReadIndex, 1, "empty drain leaves read index unchanged");

    /* @leaf 8002A49C source src/game/8002A49C.c: gated VAGM ring drain forwards entries, wraps cursor, toggles draining flag */
    LK_FUNC("func_8002A49C");
    lk_fill(gStreamAssetReleaseRing, 0, sizeof(gStreamAssetReleaseRing));
    gStreamAssetReleaseWriteIndex = 0;
    gVagmRing[0] = 0x400;
    gVagmRing[1] = 0;
    gVagmRing[2] = 0x200;
    gVagmRing[3] = 0x300;
    gVagmReadIndex = 2;
    gVagmQueueDraining = 0x5A;
    gCallGateFlag = 0;
    gDrainVagmRing();
    FCK_EQ(gVagmReadIndex, 2, "disabled gate leaves read index untouched");
    FCK_EQ(gVagmQueueDraining, 0x5A, "disabled gate leaves draining flag untouched");
    FCK_EQ(gVagmRing[2], 0x200, "disabled gate leaves ring entries intact");

    gCallGateFlag = 1;
    gStreamAssetReleaseRing[0] = 0x777; /* force overflow cb on first entry to observe draining=1 */
    mock_draining_during_cb = 0;
    fck_log_reset();
    gDrainVagmRing();
    FCK_EQ(mock_draining_during_cb, 1, "draining flag is 1 while forwarding entries");
    FCK_EQ(gVagmQueueDraining, 0, "draining flag cleared to 0 on exit");
    FCK_EQ(gVagmReadIndex, 1, "VAGM read index wraps from 2 across 3 and 0 to stop at 1");
    FCK_CHECK(gVagmRing[0] == 0 && gVagmRing[2] == 0 && gVagmRing[3] == 0, "drained VAGM slots cleared");
    FCK_EQ(gStreamAssetReleaseRing[0], 0x777, "full release slot remains occupied");
    FCK_EQ(gStreamAssetReleaseWriteIndex, 0, "full release ring write index does not advance");
    FCK_EQ(fck_calls_to("cb_80029650"), 3, "full release ring drops all 3 drained entries to overflow cb");
    FCK_EQ(fck_call_n("cb_80029650", 0).a0, 0x200, "overflow cb receives dropped 0x200");
    FCK_EQ(fck_call_n("cb_80029650", 1).a0, 0x300, "overflow cb receives dropped 0x300");

    /* Clean transfer into empty release ring */
    lk_fill(gStreamAssetReleaseRing, 0, sizeof(gStreamAssetReleaseRing));
    gStreamAssetReleaseWriteIndex = 0;
    gVagmRing[3] = 0x501;
    gVagmRing[0] = 0x502;
    gVagmReadIndex = 3;
    gDrainVagmRing();
    FCK_EQ(gStreamAssetReleaseRing[0], 0x501, "first VAGM entry forwarded to release ring");
    FCK_EQ(gStreamAssetReleaseRing[1], 0x502, "wrapped VAGM entry forwarded to release ring");
    FCK_EQ(gVagmReadIndex, 1, "read index stops after wrapped entry");

    /* @leaf 8002A544 source src/game/8002A544.c: gate check, 0x40 bitmask pending flag, drain, and gate clear */
    LK_FUNC("func_8002A544");
    lk_fill(gStreamAssetReleaseRing, 0, sizeof(gStreamAssetReleaseRing));
    gStreamAssetReleaseWriteIndex = 0;
    lk_fill(gVagmRing, 0, sizeof(gVagmRing));
    gVagmRing[0] = 0x601;
    gVagmReadIndex = 0;
    gStreamStartPending[0] = 0;
    gBitFieldState = 0;
    gCallGateFlag = 0;
    gResetVagmStreamState();
    FCK_EQ(gStreamStartPending[0], 0, "gate 0 does not set pending flag");
    FCK_EQ(gVagmRing[0], 0x601, "gate 0 does not drain VAGM ring");

    gCallGateFlag = 2;
    gBitFieldState = 0x40;
    gStreamStartPending[0] = 0;
    gResetVagmStreamState();
    FCK_EQ(gStreamStartPending[0], 0, "bit 0x40 set suppresses pending flag");
    FCK_EQ(gVagmRing[0], 0, "VAGM ring drained when gate nonzero");
    FCK_EQ(gCallGateFlag, 0, "gate cleared to 0");

    gCallGateFlag = 1;
    gBitFieldState = 0xBF; /* all bits except 0x40 */
    gStreamStartPending[0] = 0;
    gResetVagmStreamState();
    FCK_EQ(gStreamStartPending[0], 1, "bit 0x40 clear sets pending flag even with other bits set");
    FCK_EQ(gCallGateFlag, 0, "gate cleared to 0 after reset");

    /* @leaf 8002A26C source src/game/8002A26C.c: unconditional ring drain, deadline expiry, 0x40 reset, and conditional restart */
    LK_FUNC("func_8002A26C");
    lk_fill(gStreamAssetReleaseRing, 0, sizeof(gStreamAssetReleaseRing));
    gStreamAssetReleaseRing[0] = 0x701;
    gStreamAssetReleaseReadIndex = 0;
    gPortraitStreamDeadline = 0;
    gFrameTickCounter = 100;
    gBitFieldState = 0x40;
    gOutStreamId = 7;
    gGameSuspended = 0;
    gCallGateFlag = 0;
    gStreamStartPending[0] = 0;
    fck_log_reset(); fcm_sound_reset();
    gUpdatePortraitStream();
    FCK_EQ(fck_calls_to("cb_80029650"), 1, "always drains callback ring even with deadline 0");
    FCK_EQ(fck_calls_to("stop_portrait"), 0, "deadline 0 does not stop portrait");

    /* Boundary: deadline == frameTickCounter is not yet expired (< required) */
    gPortraitStreamDeadline = 100;
    gFrameTickCounter = 100;
    fck_log_reset(); fcm_sound_reset();
    gUpdatePortraitStream();
    FCK_EQ(gPortraitStreamDeadline, 100, "deadline equal to tick does not expire");
    FCK_EQ(fck_calls_to("stop_portrait"), 0, "equal tick does not stop portrait");

    /* Expired with bit 0x40 set, unsuspended, gate == 2 -> clears state and restarts */
    gPortraitStreamDeadline = 99;
    gFrameTickCounter = 100;
    gBitFieldState = 0x41;
    gOutStreamId = 5;
    gGameSuspended = 0;
    gCallGateFlag = 2;
    gStreamStartPending[0] = 0;
    fck_log_reset(); fcm_sound_reset();
    gUpdatePortraitStream();
    FCK_EQ(gPortraitStreamDeadline, 0, "expired deadline cleared");
    FCK_EQ(fck_calls_to("stop_portrait"), 1, "expired deadline stops portrait");
    FCK_EQ(gBitFieldState, 0, "bit 0x40 clears bitfield state");
    FCK_EQ(gOutStreamId, -1, "bit 0x40 resets out stream id to -1");
    FCK_EQ(gStreamStartPending[0], 1, "gate 2 sets pending start");
    FCK_EQ(fcm_sound.stream_starts, 1, "gate 2 restarts VAGM stream");
    FCK_EQ(fcm_sound.stream_last_arg0, 1, "restart passes arg 1");

    /* Expired with bit 0x40 clear, unsuspended, gate == 0 -> preserves bitfield/id, still restarts */
    gPortraitStreamDeadline = 50;
    gFrameTickCounter = 60;
    gBitFieldState = 0x3F;
    gOutStreamId = 9;
    gGameSuspended = 0;
    gCallGateFlag = 0;
    gStreamStartPending[0] = 0;
    fck_log_reset(); fcm_sound_reset();
    gUpdatePortraitStream();
    FCK_EQ(gBitFieldState, 0x3F, "bit 0x40 clear preserves bitfield state");
    FCK_EQ(gOutStreamId, 9, "bit 0x40 clear preserves out stream id");
    FCK_EQ(fcm_sound.stream_starts, 1, "gate 0 restarts VAGM stream");

    /* Expired with gate == 1 or suspended -> no restart */
    gPortraitStreamDeadline = 50;
    gFrameTickCounter = 60;
    gGameSuspended = 0;
    gCallGateFlag = 1;
    gStreamStartPending[0] = 0;
    fck_log_reset(); fcm_sound_reset();
    gUpdatePortraitStream();
    FCK_EQ(fcm_sound.stream_starts, 0, "gate 1 does not restart stream");
    FCK_EQ(gStreamStartPending[0], 0, "gate 1 leaves pending flag 0");

    gPortraitStreamDeadline = 50;
    gFrameTickCounter = 60;
    gGameSuspended = 1;
    gCallGateFlag = 0;
    fck_log_reset(); fcm_sound_reset();
    gUpdatePortraitStream();
    FCK_EQ(fcm_sound.stream_starts, 0, "suspended game does not restart stream");
}

static void test_stream_refill(void)
{
    s32 fake_chunk[8];
    s32 ret;
    struct FckCall c;

    /* @leaf 800246A8 source src/game/800246A8.c: release prior stream id, dequeue replacement, program channel 1 and 0 ranges */
    LK_FUNC("func_800246A8");
    lk_fill(gStreamAssetReleaseRing, 0, sizeof(gStreamAssetReleaseRing));
    gStreamAssetReleaseWriteIndex = 0;
    gActiveStreamId = 0x1234;
    mock_vagm_chunk_ret = 0;
    fck_log_reset();
    ret = gReplaceVagmAudioStreamBuffer(0);
    FCK_EQ(ret, 0, "null dequeue returns 0");
    FCK_EQ(gActiveStreamId, 0, "active stream id cleared when dequeue returns null");
    FCK_EQ(gStreamAssetReleaseRing[0], 0x1234, "prior active stream queued for release");
    FCK_EQ(fck_calls_to("set_range"), 0, "null dequeue does not program stream channels");

    fake_chunk[0] = 0x1111;
    fake_chunk[1] = 0x2222;
    gActiveStreamId = 0;
    mock_vagm_chunk_ret = (s32)fake_chunk;
    fck_log_reset();
    ret = gReplaceVagmAudioStreamBuffer(3);
    FCK_EQ(ret, (s32)fake_chunk, "returns newly dequeued chunk pointer");
    FCK_EQ(gActiveStreamId, (s32)fake_chunk, "stores new chunk pointer in active stream id");
    FCK_EQ(gStreamAssetReleaseWriteIndex, 1, "zero prior stream id does not enqueue another release");
    FCK_EQ(fck_calls_to("set_range"), 2, "programs two channel ranges");
    c = fck_call_n("set_range", 0);
    FCK_EQ(c.a0, (s32)((u8 *)fake_chunk + 0x1C + 0x2FC0), "first range starts at chunk + 0x1C + 0x2FC0");
    FCK_EQ(c.a1, 0x2FC0, "first range length is 0x2FC0");
    FCK_EQ(c.a2, (1 << 8) | 0, "first range targets channel 1 non-looping");
    c = fck_call_n("set_range", 1);
    FCK_EQ(c.a0, (s32)((u8 *)fake_chunk + 0x1C), "second range starts at chunk + 0x1C");
    FCK_EQ(c.a1, 0x2FC0, "second range length is 0x2FC0");
    FCK_EQ(c.a2, (0 << 8) | 0, "second range targets channel 0 non-looping");

    /* @leaf 80024470 source src/game/80024470.c: admits only event codes 3 and 4 to stream buffer replacement */
    LK_FUNC("func_80024470");
    gActiveStreamId = 0;
    mock_vagm_chunk_ret = 0;
    fck_log_reset();
    gHandleAudioStreamRefillEvent(-1);
    gHandleAudioStreamRefillEvent(2);
    gHandleAudioStreamRefillEvent(5);
    FCK_EQ(fck_calls_to("dequeue_vagm"), 0, "events -1, 2, 5 ignored");

    gHandleAudioStreamRefillEvent(3);
    FCK_EQ(fck_calls_to("dequeue_vagm"), 1, "event 3 forwards to replacement");
    gHandleAudioStreamRefillEvent(4);
    FCK_EQ(fck_calls_to("dequeue_vagm"), 2, "event 4 forwards to replacement");
}

/* =========================================================================
 * Group 2: Actor Animation, Spawn Descriptors, Node Pool, Spawn Record Map
 * ========================================================================= */

struct NodePool {
    void *storage;
    void *active;
    void *free;
};

struct SpawnRecord {
    u8 unk00[0x14];
    u8 unk14;
    u8 unk15[0x13];
    s16 unk28;
    s16 unk2A;
};

static void test_actor_and_spawns(void)
{
    u8 actor[0x180];
    u8 src_desc[0x34];
    u8 dst_desc[0x20];
    u8 old_desc[0x1C];
    struct NodePool pool;
    u8 backing_mem[16];
    struct SpawnRecord rec_a, rec_b;
    struct FckCall c;
    int i;

    /* @leaf 8002B0EC source src/game/8002B0EC.c: clip-change guard, attack match flag, 0x3A randomization/remap and +0x74 zeroing */
    LK_FUNC("func_8002B0EC");
    lk_fill(actor, 0xA5, sizeof(actor));
    actor[0x15E] = 7;
    actor[0x164] = 9;
    *(s32 *)(actor + 0x74) = 0x12345678;
    fck_log_reset();
    gSetType26AnimationClip(actor, 7);
    FCK_EQ(fck_calls_to("select_clip"), 0, "unchanged clip id is a no-op");
    FCK_EQ(actor[0x162], 0xA5, "unchanged clip leaves match flag untouched");

    /* Normal clip matching +0x164 */
    gSetType26AnimationClip(actor, 9);
    FCK_EQ(actor[0x15E], 9, "active clip updated to 9");
    FCK_EQ(actor[0x162], 1, "match flag set when clip equals +0x164");
    FCK_EQ(fck_calls_to("rng"), 0, "non-0x3A clip does not call RNG");
    FCK_EQ(fck_calls_to("select_clip"), 1, "selects clip once");
    c = fck_call_n("select_clip", 0);
    FCK_EQ(c.a0, (s32)(actor + 0xC8), "passes track pointer actor + 0xC8");
    FCK_EQ(c.a1, 9, "passes clip 9");
    FCK_EQ(c.a2, (1 << 8) | 1, "passes direction 1 and flags 1");
    FCK_EQ(*(s32 *)(actor + 0x74), 0x12345678, "non-0x3A clip preserves +0x74");

    /* Normal clip not matching +0x164 */
    gSetType26AnimationClip(actor, 10);
    FCK_EQ(actor[0x162], 0, "match flag cleared when clip differs from +0x164");

    /* Clip 0x3A with RNG % 10 == 5 -> 0x3F (below 0x40, preserves +0x74) */
    mock_rng_val = 25;
    *(s32 *)(actor + 0x74) = 0x55555555;
    fck_log_reset();
    gSetType26AnimationClip(actor, 0x3A);
    FCK_EQ(actor[0x15E], 0x3A, "stores base 0x3A in +0x15E");
    c = fck_call_n("select_clip", 0);
    FCK_EQ(c.a1, 0x3F, "rng 25 maps 0x3A -> 0x3F");
    FCK_EQ(*(s32 *)(actor + 0x74), 0x55555555, "0x3F does not zero +0x74");

    /* Clip 0x3A with RNG % 10 == 6 -> 0x40 (zeroes +0x74) */
    actor[0x15E] = 0;
    mock_rng_val = 16;
    *(s32 *)(actor + 0x74) = 0x77777777;
    fck_log_reset();
    gSetType26AnimationClip(actor, 0x3A);
    c = fck_call_n("select_clip", 0);
    FCK_EQ(c.a1, 0x40, "rng 16 maps 0x3A -> 0x40");
    FCK_EQ(*(s32 *)(actor + 0x74), 0, "0x40 zeroes +0x74");

    /* Clip 0x3A with RNG % 10 == 8 -> 0x42 (upper edge of 0x40..0x42, zeroes +0x74) */
    actor[0x15E] = 0;
    mock_rng_val = 18;
    *(s32 *)(actor + 0x74) = 0x33333333;
    fck_log_reset();
    gSetType26AnimationClip(actor, 0x3A);
    c = fck_call_n("select_clip", 0);
    FCK_EQ(c.a1, 0x42, "rng 18 maps 0x3A -> 0x42");
    FCK_EQ(*(s32 *)(actor + 0x74), 0, "0x42 zeroes +0x74");

    /* Clip 0x3A with RNG % 10 == 9 -> 0x43 remapped to 0x44 (zeroes +0x74) */
    actor[0x15E] = 0;
    mock_rng_val = 29;
    *(s32 *)(actor + 0x74) = 0x44444444;
    fck_log_reset();
    gSetType26AnimationClip(actor, 0x3A);
    c = fck_call_n("select_clip", 0);
    FCK_EQ(c.a1, 0x44, "rng 29 remaps 0x43 -> 0x44");
    FCK_EQ(*(s32 *)(actor + 0x74), 0, "0x44 zeroes +0x74");

    /* @leaf 8003BC7C source src/game/8003BC7C.c: conditional free, 0x1C type-2 alloc, and exact descriptor field copy */
    LK_FUNC("func_8003BC7C");
    lk_fill(gSlotSpawnDescs, 0, sizeof(gSlotSpawnDescs));
    lk_fill(src_desc, 0x6D, sizeof(src_desc));
    lk_fill(dst_desc, 0xA5, sizeof(dst_desc));
    src_desc[0x14] = 5;
    src_desc[0x15] = 0xFE;
    *(u16 *)(src_desc + 0x16) = 0xBEEF;
    for (i = 0; i < 0x18; i++) src_desc[0x18 + i] = (u8)(0x10 + i);
    gSlotSpawnDescs[5] = old_desc;
    mock_alloc_ptr = dst_desc;
    fck_log_reset();
    gSetSlotSpawnDescriptor(src_desc);
    FCK_EQ(fck_calls_to("free"), 1, "existing slot descriptor freed");
    c = fck_call_n("free", 0); FCK_EQ(c.a0, (s32)old_desc, "freed prior slot pointer");
    FCK_EQ(fck_calls_to("alloc"), 1, "allocated new descriptor");
    c = fck_call_n("alloc", 0);
    FCK_EQ(c.a0, 0x1C, "allocation size is 0x1C");
    FCK_EQ(c.a1, 2, "allocation type is 2");
    FCK_CHECK(gSlotSpawnDescs[5] == dst_desc, "slot 5 updated to new block");
    for (i = 0; i < 0x18; i++) FCK_EQ(dst_desc[i], 0x10 + i, "payload bytes 0x00..0x17 copied from 0x18..0x2F");
    FCK_EQ(*(u16 *)(dst_desc + 0x18), 0xBEEF, "halfword +0x18 copied from src +0x16");
    FCK_EQ(dst_desc[0x1A], 0xFE, "byte +0x1A copied from src +0x15");
    FCK_EQ(dst_desc[0x1B], 0xA5, "byte +0x1B untouched");

    /* Empty slot does not call free */
    src_desc[0x14] = 6;
    fck_log_reset();
    gSetSlotSpawnDescriptor(src_desc);
    FCK_EQ(fck_calls_to("free"), 0, "empty slot skips free");
    FCK_CHECK(gSlotSpawnDescs[6] == dst_desc, "slot 6 populated");

    /* @leaf 8003BDEC source src/game/8003BDEC.c: null-guarded pool storage free and idempotent second call */
    LK_FUNC("func_8003BDEC");
    pool.storage = backing_mem;
    pool.active = (void *)0x1111;
    pool.free = (void *)0x2222;
    fck_log_reset();
    gFreeIntrusiveNodePoolStorage(&pool);
    FCK_EQ(fck_calls_to("free"), 1, "non-null storage freed once");
    c = fck_call_n("free", 0); FCK_EQ(c.a0, (s32)backing_mem, "freed pool storage pointer");
    FCK_CHECK(pool.storage == 0, "storage pointer cleared to null");
    FCK_CHECK(pool.active == (void *)0x1111 && pool.free == (void *)0x2222, "active and free heads preserved");
    fck_log_reset();
    gFreeIntrusiveNodePoolStorage(&pool);
    FCK_EQ(fck_calls_to("free"), 0, "second call on null storage is a no-op");

    /* @leaf 8003C404 source src/game/8003C404.c: flag byte clear, prior slot free, unk28/unk2A normalization, and 2D table install */
    LK_FUNC("func_8003C404");
    lk_fill(gSpawnRecordMap, 0, sizeof(gSpawnRecordMap));
    gSpawnTableRow = 2;
    lk_fill(&rec_a, 0xA5, sizeof(rec_a));
    rec_a.unk14 = 13;
    rec_a.unk28 = 5;
    rec_a.unk2A = 7;
    mock_spawn_flag_word = 0x11223344;
    fck_log_reset();
    gInstallSpawnRecord(&rec_a);
    FCK_EQ(mock_spawn_flag_word, 0x11223300, "low byte of flag word cleared, upper bytes preserved");
    c = fck_call_n("flag_ptr", 0);
    FCK_EQ(c.a0, 13, "flag lookup passes column unk14");
    FCK_EQ(c.a1, 2, "flag lookup passes active row");
    FCK_EQ(fck_calls_to("free"), 0, "empty slot skips free");
    FCK_EQ(rec_a.unk28, 5, "positive unk2A and unk28!=1 preserved");
    FCK_EQ(rec_a.unk2A, 7, "positive unk2A preserved");
    FCK_CHECK(gSpawnRecordMap[2][13] == &rec_a, "record installed at [row][col]");

    /* Overwrite occupied slot with unk28 == 1 -> forces unk2A = 0 and unk28 = -1 */
    lk_fill(&rec_b, 0x6D, sizeof(rec_b));
    rec_b.unk14 = 13;
    rec_b.unk28 = 1;
    rec_b.unk2A = 9;
    fck_log_reset();
    gInstallSpawnRecord(&rec_b);
    FCK_EQ(fck_calls_to("free"), 1, "occupied slot frees prior occupant");
    c = fck_call_n("free", 0); FCK_EQ(c.a0, (s32)&rec_a, "freed previous record in [2][13]");
    FCK_EQ(rec_b.unk2A, 0, "unk28==1 clears unk2A");
    FCK_EQ(rec_b.unk28, -1, "cleared unk2A forces unk28 to -1");
    FCK_CHECK(gSpawnRecordMap[2][13] == &rec_b, "new record replaces slot");

    /* Negative unk2A with unk28 != 1 -> clamped to unk2A = 0, unk28 = -1 */
    rec_a.unk14 = 14;
    rec_a.unk28 = 4;
    rec_a.unk2A = -3;
    fck_log_reset();
    gInstallSpawnRecord(&rec_a);
    FCK_EQ(fck_calls_to("free"), 0, "empty slot 14 skips free");
    FCK_EQ(rec_a.unk2A, 0, "negative unk2A clamped to 0");
    FCK_EQ(rec_a.unk28, -1, "negative unk2A forces unk28 to -1");

    /* Zero unk2A with unk28 != 1 -> unk28 forced to -1 */
    rec_a.unk14 = 15;
    rec_a.unk28 = 4;
    rec_a.unk2A = 0;
    fck_log_reset();
    gInstallSpawnRecord(&rec_a);
    FCK_EQ(fck_calls_to("free"), 0, "empty slot 15 skips free");
    FCK_EQ(rec_a.unk2A, 0, "zero unk2A remains 0");
    FCK_EQ(rec_a.unk28, -1, "zero unk2A forces unk28 to -1");
}

/* =========================================================================
 * Group 3: CVK Sound Bank / Sample Table / Tracked Voice Helpers
 * ========================================================================= */

struct CshdFixture {
    struct CshdHeader hdr;
    struct CshdItem items[3];
};

static void test_cvk_banks(void)
{
    struct CshdFixture fix;
    u8 blk_a[8], blk_b[8], blk_c[8], cshd0[8], cshd1[8];
    struct FckCall c;
    s32 ret;
    int i;

    /* @leaf 800235D4 source src/game/800235D4.c: CSHD header lookup, 8-slot scan, and 12-byte item install loop */
    LK_FUNC("func_800235D4");
    lk_fill(gObjectSlots, 0xFF, sizeof(gObjectSlots));
    lk_fill(&fix, 0, sizeof(fix));
    fix.hdr.count = 2;
    fix.hdr.entries[1].id = 4;
    fix.hdr.entries[1].count = 2;
    fix.hdr.entries[1].offset = (u16)((u8 *)fix.items - (u8 *)&fix);
    fix.items[0].key = 12; fix.items[0].value = 34;
    fix.items[1].key = 56; fix.items[1].value = 78;

    /* Null header -> returns 0 */
    mock_cshd_header = 0;
    mock_cshd_index = 1;
    mock_append_item_count = 0;
    fck_log_reset();
    ret = gInstallCvkGroupSamples(4);
    FCK_EQ(ret, 0, "null header returns 0");
    FCK_EQ(fck_calls_to("append_sample"), 0, "null header appends nothing");

    /* Header found, but id not in gObjectSlots -> returns 0 */
    mock_cshd_header = &fix.hdr;
    fck_log_reset();
    ret = gInstallCvkGroupSamples(4);
    FCK_EQ(ret, 0, "missing object slot returns 0");

    /* Slot 7 matches id=4 with bank value=3 -> appends 2 items and returns 1 */
    gObjectSlots[7].id = 3;
    gObjectSlots[7].status = 4;
    mock_append_item_count = 0;
    fck_log_reset();
    ret = gInstallCvkGroupSamples(4);
    FCK_EQ(ret, 1, "matched group and slot returns 1");
    FCK_EQ(fck_calls_to("append_sample"), 2, "appends both sample items");
    c = fck_call_n("append_sample", 0);
    FCK_EQ(c.a0, 12, "first item key");
    FCK_EQ(c.a1, 34, "first item value");
    FCK_EQ(c.a2, (4 << 8) | 3, "first item group=4 bank=3");
    c = fck_call_n("append_sample", 1);
    FCK_EQ(c.a0, 56, "second item key");
    FCK_EQ(c.a1, 78, "second item value");
    FCK_CHECK(mock_append_items[0] == &fix.items[0] && mock_append_items[1] == &fix.items[1],
              "passes exact item pointers in order");

    /* Empty entry (count == 0) still returns 1 with 0 appends */
    fix.hdr.entries[1].count = 0;
    fck_log_reset();
    ret = gInstallCvkGroupSamples(4);
    FCK_EQ(ret, 1, "empty matched entry returns 1");
    FCK_EQ(fck_calls_to("append_sample"), 0, "empty entry appends nothing");

    /* @leaf 80023A24 source src/game/80023A24.c: drains all 110 program entries calling remove count times on head byte */
    LK_FUNC("func_80023A24");
    lk_fill(gSoundEntryTable, 0, sizeof(gSoundEntryTable));
    gSoundEntryTable[0].count = 2;
    gSoundEntryTable[0].samples[0].program = 11;
    gSoundEntryTable[0].samples[1].program = 22;
    gSoundEntryTable[109].count = 1;
    gSoundEntryTable[109].samples[0].program = 99;
    mock_compact_on_remove = 1;
    fck_log_reset();
    gClearSoundProgramSamples();
    FCK_EQ(fck_calls_to("remove_sample"), 3, "removes 2 from slot 0 and 1 from slot 109");
    c = fck_call_n("remove_sample", 0);
    FCK_CHECK(c.a0 == 0 && c.a1 == 11, "first removal gets slot 0 head 11");
    c = fck_call_n("remove_sample", 1);
    FCK_CHECK(c.a0 == 0 && c.a1 == 22, "second removal re-reads compacted head 22");
    c = fck_call_n("remove_sample", 2);
    FCK_CHECK(c.a0 == 109 && c.a1 == 99, "last slot 109 drained");
    mock_compact_on_remove = 0;

    /* @leaf 80023AB4 source src/game/80023AB4.c: negative guard, backward sample removal, bank unregister, block frees, first-match break */
    LK_FUNC("func_80023AB4");
    lk_fill(gObjectSlots, 0, sizeof(gObjectSlots));
    for (i = 0; i < 8; i++) { gObjectSlots[i].id = -1; gObjectSlots[i].status = -1; }
    lk_fill(gSoundEntryTable, 0, sizeof(gSoundEntryTable));
    gObjectSlots[2].id = 5;
    gObjectSlots[2].status = 3;
    gObjectSlots[2].pad2 = 7;
    gObjectSlots[2].block4 = blk_a;
    gObjectSlots[2].block8 = blk_b;
    /* Duplicate matching slot at index 5 must NOT be touched due to break */
    gObjectSlots[5].id = 6;
    gObjectSlots[5].status = 3;
    gObjectSlots[5].block4 = blk_c;

    gSoundEntryTable[1].count = 3;
    gSoundEntryTable[1].samples[0].program = 10; gSoundEntryTable[1].samples[0].vab = 3;
    gSoundEntryTable[1].samples[1].program = 20; gSoundEntryTable[1].samples[1].vab = 9;
    gSoundEntryTable[1].samples[2].program = 30; gSoundEntryTable[1].samples[2].vab = 3;
    gSoundEntryTable[109].count = 1;
    gSoundEntryTable[109].samples[0].program = 40; gSoundEntryTable[109].samples[0].vab = 3;

    fck_log_reset();
    gUnloadCvkSoundBank(-1);
    FCK_EQ(fck_log_count, 0, "negative arg0 returns immediately");
    FCK_EQ(gObjectSlots[2].status, 3, "negative arg0 leaves slots untouched");

    fck_log_reset();
    gUnloadCvkSoundBank(3);
    FCK_EQ(fck_calls_to("remove_sample"), 3, "removes 3 matching samples across 110 banks");
    c = fck_call_n("remove_sample", 0);
    FCK_CHECK(c.a0 == 1 && c.a1 == 30, "backward scan removes idx 2 (prog 30) before idx 0");
    c = fck_call_n("remove_sample", 1);
    FCK_CHECK(c.a0 == 1 && c.a1 == 10, "backward scan removes idx 0 (prog 10), skips vab 9");
    c = fck_call_n("remove_sample", 2);
    FCK_CHECK(c.a0 == 109 && c.a1 == 40, "scans through bank 109");
    FCK_EQ(fck_calls_to("unreg_bank"), 1, "unregisters bank id once");
    c = fck_call_n("unreg_bank", 0); FCK_EQ(c.a0, 5, "unregisters id 5");
    FCK_EQ(fck_calls_to("free"), 2, "frees block4 and block8");
    c = fck_call_n("free", 0); FCK_EQ(c.a0, (s32)blk_a, "frees block4 first");
    c = fck_call_n("free", 1); FCK_EQ(c.a0, (s32)blk_b, "frees block8 second");
    FCK_CHECK(gObjectSlots[2].id == -1 && gObjectSlots[2].status == -1 && gObjectSlots[2].pad2 == 0 &&
              gObjectSlots[2].block4 == 0 && gObjectSlots[2].block8 == 0, "matched slot retired");
    FCK_CHECK(gObjectSlots[5].status == 3 && gObjectSlots[5].block4 == blk_c,
              "second matching slot untouched because of break");

    /* Slot with id == -1 and null blocks skips unregister and free */
    gObjectSlots[7].id = -1;
    gObjectSlots[7].status = 0;
    gObjectSlots[7].pad2 = 4;
    gObjectSlots[7].block4 = 0;
    gObjectSlots[7].block8 = 0;
    fck_log_reset();
    gUnloadCvkSoundBank(0);
    FCK_EQ(fck_calls_to("unreg_bank"), 0, "id -1 skips unregister");
    FCK_EQ(fck_calls_to("free"), 0, "null blocks skip free");
    FCK_CHECK(gObjectSlots[7].status == -1 && gObjectSlots[7].pad2 == 0, "slot 7 status 0 still retired");

    /* @leaf 80023BF8 source src/game/80023BF8.c: CVK active gate, 8-slot unload, sample drain, CSHD frees, and table reset */
    LK_FUNC("func_80023BF8");
    lk_fill(gObjectSlots, 0, sizeof(gObjectSlots));
    for (i = 0; i < 8; i++) { gObjectSlots[i].id = -1; gObjectSlots[i].status = -1; }
    lk_fill(gSoundEntryTable, 0, sizeof(gSoundEntryTable));
    gObjectSlots[0].id = 2; gObjectSlots[0].status = 1; gObjectSlots[0].block4 = blk_a;
    gObjectSlots[7].id = 4; gObjectSlots[7].status = 2;
    gSoundEntryTable[3].count = 1; gSoundEntryTable[3].samples[0].program = 77; gSoundEntryTable[3].samples[0].vab = 99;
    gCshdNodeSlots[0] = cshd0;
    gCshdNodeSlots[1] = cshd1;

    gCvkSystemActive = 0;
    fck_log_reset();
    gReleaseCvkSoundBanksAndVoices();
    FCK_EQ(fck_log_count, 0, "inactive CVK does nothing");
    FCK_CHECK(gCshdNodeSlots[0] == cshd0 && gCshdNodeSlots[1] == cshd1, "inactive CVK preserves CSHD slots");

    gCvkSystemActive = 1;
    fck_log_reset();
    gReleaseCvkSoundBanksAndVoices();
    FCK_EQ(fck_calls_to("unreg_bank"), 2, "unloads both active slots including slot 7");
    FCK_EQ(fck_calls_to("remove_sample"), 1, "drains remaining program samples");
    FCK_EQ(fck_calls_to("free"), 3, "frees slot block4 plus both CSHD node slots");
    FCK_CHECK(gCshdNodeSlots[0] == 0 && gCshdNodeSlots[1] == 0, "CSHD node slots cleared");
    FCK_EQ(fck_calls_to("reset_banks"), 1, "resets sound bank tables at end");
}

static void test_tracked_voice(void)
{
    struct TrackedResource res;
    struct FckCall c;
    s32 ret;

    /* @leaf 80023E84 source src/game/80023E84.c: state==1 owner/volume cleanup plus unconditional handle release and deadline clear */
    LK_FUNC("func_80023E84");
    lk_fill(&res, 0, sizeof(res));
    res.owner = -5;
    lk_fill(&gAssetHandle, 0, sizeof(gAssetHandle));
    gAssetHandle.resource = &res;
    gAssetHandle.state = 0;
    gAssetHandle.time = 500;
    gAssetHandle.flag = 0;
    fck_log_reset();
    gStopTrackedSoundVoice();
    FCK_EQ(fck_calls_to("release_owner"), 0, "state 0 skips owner release");
    FCK_EQ(fck_calls_to("clear_vol_limit"), 0, "state 0 skips volume limit clear");
    FCK_EQ(fck_calls_to("release_handle"), 1, "always releases handle");
    FCK_EQ(gAssetHandle.time, 0, "always clears deadline");
    FCK_EQ(fck_calls_to("mark_stopped"), 1, "always marks portrait voice stopped");

    /* state == 1, non-null resource, flag == 0 -> releases owner, clears state & vol limit */
    gAssetHandle.resource = &res;
    gAssetHandle.state = 1;
    gAssetHandle.time = 700;
    gAssetHandle.flag = 0;
    fck_log_reset();
    gStopTrackedSoundVoice();
    FCK_EQ(fck_calls_to("release_owner"), 1, "state 1 releases owner voices");
    c = fck_call_n("release_owner", 0); FCK_EQ(c.a0, -5, "passes signed owner byte");
    FCK_EQ(gAssetHandle.state, 0, "clears state to 0");
    FCK_EQ(fck_calls_to("clear_vol_limit"), 1, "flag 0 clears volume limit");
    FCK_EQ(gAssetHandle.time, 0, "clears deadline");

    /* state == 1, null resource, flag == 1 -> skips owner release and vol limit clear */
    gAssetHandle.resource = 0;
    gAssetHandle.state = 1;
    gAssetHandle.flag = 1;
    fck_log_reset();
    gStopTrackedSoundVoice();
    FCK_EQ(fck_calls_to("release_owner"), 0, "null resource skips owner release");
    FCK_EQ(fck_calls_to("clear_vol_limit"), 0, "flag 1 skips volume limit clear");
    FCK_EQ(gAssetHandle.state, 0, "still clears state to 0");

    /* @leaf 80023D78 source src/game/80023D78.c: deadline window [tick, tick+0xBB8], active-without-deadline stop, and gated mix update */
    LK_FUNC("func_80023D78");
    lk_fill(&gAssetHandle, 0, sizeof(gAssetHandle));
    gAssetHandle.resource = &res;
    gAssetHandle.id = 42;
    gCvkSystemActive = 1;
    gFrameTickCounter = 1000;
    mock_mix_ok = 1; mock_mix0 = 80; mock_mix1 = 64;
    mock_validate_ret = 1;

    /* Valid deadline at exact upper edge tick + 0xBB8 -> updates mix, does not stop */
    gAssetHandle.state = 1;
    gAssetHandle.time = 1000 + 0xBB8;
    gAssetHandle.flag = 1;
    fck_log_reset();
    gUpdateTrackedSoundVoice();
    FCK_EQ(fck_calls_to("mark_stopped"), 0, "exact upper edge tick+0xBB8 does not stop");
    FCK_EQ(fck_calls_to("update_mix"), 1, "eligible active voice updates mix");
    c = fck_call_n("update_mix", 0);
    FCK_EQ(c.a0, (s32)&res, "update_mix gets resource pointer");
    FCK_EQ(c.a1, 64, "update_mix gets pan=mix[1]");
    FCK_EQ(c.a2, 80, "update_mix gets vol=mix[0]");
    FCK_EQ(mock_update_mix_extra, -1, "update_mix extra arg is -1");

    /* Exact lower edge tick -> does not stop */
    gAssetHandle.time = 1000;
    fck_log_reset();
    gUpdateTrackedSoundVoice();
    FCK_EQ(fck_calls_to("mark_stopped"), 0, "exact lower edge tick does not stop");
    FCK_EQ(fck_calls_to("update_mix"), 1, "lower edge still updates mix");

    /* Beyond upper edge tick + 0xBB9 -> stops */
    gAssetHandle.time = 1000 + 0xBB9;
    fck_log_reset();
    gUpdateTrackedSoundVoice();
    FCK_EQ(gAssetHandle.time, 0, "over-upper deadline cleared");
    FCK_EQ(gAssetHandle.state, 0, "over-upper deadline stops voice");
    FCK_EQ(fck_calls_to("update_mix"), 0, "stopped voice does not update mix");

    /* Below lower edge tick - 1 -> stops */
    gAssetHandle.state = 1;
    gAssetHandle.time = 999;
    fck_log_reset();
    gUpdateTrackedSoundVoice();
    FCK_EQ(gAssetHandle.state, 0, "past deadline stops voice");

    /* Zero deadline while active -> stops */
    gAssetHandle.state = 1;
    gAssetHandle.time = 0;
    fck_log_reset();
    gUpdateTrackedSoundVoice();
    FCK_EQ(gAssetHandle.state, 0, "active handle with zero deadline stops");

    /* Failed mix or validation stops active voice */
    gAssetHandle.state = 1;
    gAssetHandle.time = 1500;
    mock_mix_ok = 0;
    fck_log_reset();
    gUpdateTrackedSoundVoice();
    FCK_EQ(gAssetHandle.state, 0, "failed mix lookup stops voice");

    gAssetHandle.state = 1;
    gAssetHandle.time = 1500;
    mock_mix_ok = 1;
    mock_validate_ret = 0;
    fck_log_reset();
    gUpdateTrackedSoundVoice();
    FCK_EQ(gAssetHandle.state, 0, "failed handle validation stops voice");

    /* @leaf 80023F08 source src/game/80023F08.c: SPU guard, mix vs default volume/pan, alloc failure vs start and deadline */
    LK_FUNC("func_80023F08");
    res.owner = 3;
    lk_fill(&gAssetHandle, 0, sizeof(gAssetHandle));
    gAssetHandle.resource = &res;
    gAssetHandle.id = 77;
    gAssetHandle.delay = 250;
    gFrameTickCounter = 1000;

    /* Default path (flag == 0), alloc succeeds */
    gSpuBufferPtr = 0x80100000;
    gAssetHandle.flag = 0;
    mock_validate_ret = 1;
    mock_default_vol = 95;
    mock_alloc_voice_ret = 0;
    fck_log_reset();
    ret = gStartTrackedSoundVoice();
    FCK_EQ(ret, 1, "successful start returns 1");
    FCK_EQ(gAssetHandle.state, 1, "sets state to 1");
    FCK_EQ(gAssetHandle.time, 1250, "sets deadline to tick + delay");
    FCK_EQ(mock_alloc_params.pitch, 0x400, "pitch is 0x400");
    FCK_EQ(mock_alloc_params.volume, 95, "default volume from gComputeTrackedVoiceVolume");
    FCK_EQ(mock_alloc_params.pan, 0x40, "default pan is 0x40");
    FCK_EQ(fck_calls_to("update_mix"), 0, "flag 0 skips update_mix");
    FCK_EQ(fck_calls_to("queue_start"), 1, "queues channel start");
    c = fck_call_n("queue_start", 0);
    FCK_CHECK(c.a0 == 1 && c.a1 == 3, "queues 1 channel for owner 3");

    /* Flag == 1 path with valid mix -> uses mix and calls update_mix */
    gAssetHandle.state = 0;
    gAssetHandle.flag = 1;
    mock_mix_ok = 1; mock_mix0 = 50; mock_mix1 = 30;
    fck_log_reset();
    ret = gStartTrackedSoundVoice();
    FCK_EQ(ret, 1, "flag 1 start returns 1");
    FCK_EQ(mock_alloc_params.volume, 50, "flag 1 uses mix[0] volume");
    FCK_EQ(mock_alloc_params.pan, 30, "flag 1 uses mix[1] pan");
    FCK_EQ(fck_calls_to("update_mix"), 1, "flag 1 calls update_mix");

    /* Alloc failure (-1) -> releases handle and returns 0 */
    gAssetHandle.state = 0;
    gAssetHandle.flag = 0;
    mock_alloc_voice_ret = -1;
    fck_log_reset();
    ret = gStartTrackedSoundVoice();
    FCK_EQ(ret, 0, "alloc failure returns 0");
    FCK_EQ(fck_calls_to("release_handle"), 1, "alloc failure releases handle");
    FCK_EQ(fck_calls_to("queue_start"), 0, "alloc failure does not queue start");

    /* Flag == 1 with failed mix -> stops voice, does not allocate */
    gAssetHandle.state = 0;
    gAssetHandle.flag = 1;
    mock_mix_ok = 0;
    mock_alloc_voice_ret = 0;
    fck_log_reset();
    ret = gStartTrackedSoundVoice();
    FCK_EQ(ret, 0, "failed mix returns 0");
    FCK_EQ(fck_calls_to("alloc_voice"), 0, "failed mix skips allocation");
    FCK_EQ(fck_calls_to("mark_stopped"), 1, "failed mix stops voice");

    /* Null SPU buffer ptr triggers stop; invalid handle returns 0 */
    gSpuBufferPtr = 0;
    gAssetHandle.state = 1;
    mock_validate_ret = 0;
    fck_log_reset();
    ret = gStartTrackedSoundVoice();
    FCK_EQ(ret, 0, "null SPU ptr stops active state and returns 0");
    FCK_EQ(fck_calls_to("mark_stopped"), 1, "null SPU ptr calls stop");
}

int main(void)
{
    test_stream_rings();
    test_stream_refill();
    test_actor_and_spawns();
    test_cvk_banks();
    test_tracked_voice();
    return lk_done("leaf_object");
}
