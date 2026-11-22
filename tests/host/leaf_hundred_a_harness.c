/* Unchanged original units, synthetic RAM, no device accesses. The aliases
 * and private layouts follow src/game/<address>.c; "source" markers describe
 * observed source semantics, not independent proof of retail behavior. */
#include "leaf_kit.h"

u32 gModelTriangleListSize __asm__("D_8009F528");
u8 *gModelGeometryChunk __asm__("D_8009F52C");
u8 *gModelPolygonList __asm__("D_8009F530");
u8 *gModelTriangleList __asm__("D_8009F534");
u8 *gModelHierarchy __asm__("D_8009F538");
u8 *gModelMeshInfo __asm__("D_8009F53C");
u8 *gModelBoundingBox __asm__("D_8009F540");
u8 *gModelAnimData __asm__("D_8009F544");
u8 *gModel3dhsPayload __asm__("D_8009F548");
u8 *gModelAnimTrackChunk __asm__("D_8009F54C");
u8 *gModelAnimWaveChunk __asm__("D_8009F550");
u8 **gVertexBlockTable __asm__("D_8009F554");
u8 **gNormalBlockTable __asm__("D_8009F558");
u8 **gRelocBlockTable __asm__("D_8009F55C");
u8 **gRefBlockTable __asm__("D_8009F560");
s16 gModelUvOffset __asm__("D_8009F580");
void gIndexModelChunks(u8 *base, u32 size) __asm__("func_80012E48");
void gSetModelUvOffset(s32 low, s32 high) __asm__("func_80013AA0");

static void test_model_chunks(void)
{
    /* Each record has a 12-byte header; payloads and skipped records vary. */
    u32 chunks[] = {
        0x3444564c, 16, 1, 0xfeed,
        0xdeadbeef, 20, 0, 1, 2,
        0x3344544c, 20, 0, 3, 4,
        0x416e6d44, 28, 99, 5, 6, 7, 8,
        0x33444242, 12, 0
    };
    u8 *base = (u8 *)chunks;
    u8 *vertices[2] = { base, 0 };
    u8 *normals[2] = { 0, base };
    u8 *relocs[2] = { 0, base };
    u8 *refs[2] = { 0, base };

    /* @leaf 80012E48 source chunk stride, indexed payload, length accounting, unknown tag and animation reset */
    LK_FUNC("func_80012E48");
    gVertexBlockTable = vertices;
    gNormalBlockTable = normals;
    gRelocBlockTable = relocs;
    gRefBlockTable = refs;
    gModelTriangleListSize = 99;
    gModelTriangleList = gModelBoundingBox = 0;
    gModelGeometryChunk = gModelPolygonList = gModelHierarchy = gModelMeshInfo = base;
    gModelAnimData = gModel3dhsPayload = gModelAnimTrackChunk = gModelAnimWaveChunk = base;
    gIndexModelChunks(base, sizeof(chunks));
    FCK_CHECK(vertices[1] == base + 12 && vertices[0] == base, "only selected vertex slot changes");
    FCK_CHECK(normals[1] == base && relocs[1] == base && refs[1] == base, "other indexed tables untouched");
    FCK_CHECK(gModelTriangleList == base + 48, "triangle payload follows skipped chunk");
    FCK_EQ(gModelTriangleListSize, 8, "triangle size excludes header");
    FCK_CHECK(gModelAnimData == base + 64, "animation pointer includes rewritten count");
    FCK_EQ(chunks[16], 1, "one complete animation record");
    FCK_CHECK(gModelBoundingBox == base + 92, "box begins at third header word");
    FCK_CHECK(gModelGeometryChunk == base && gModelPolygonList == base &&
              gModelHierarchy == base && gModelMeshInfo == base, "absent geometry chunks leave pointers alone");
    FCK_CHECK(!gModel3dhsPayload && !gModelAnimTrackChunk && !gModelAnimWaveChunk, "absent animation pointers clear");
    gIndexModelChunks(0, 0);
    FCK_CHECK(!gModelAnimData && gModelTriangleList == base + 48, "empty input clears only animation pointers");

    /* @leaf 80013AA0 math two byte-coordinate offsets packed into a halfword */
    LK_FUNC("func_80013AA0");
    gModelUvOffset = 0x6969;
    gSetModelUvOffset(0xe2, 0x13);
    FCK_EQ(gModelUvOffset, 0x13e2, "V in high byte, U in low");
    gSetModelUvOffset(0, 0xff);
    FCK_EQ((u16)gModelUvOffset, 0xff00, "high byte preserved at maximum");
}

s32 gPrimaryPrngCursor __asm__("D_8009F5A8");
s32 gStateTable[20] __asm__("D_800A0480");
s32 gAdvancePrimaryPrng(void) __asm__("func_80019668");
/* The asm-only unit's void(void) declaration names the symbol, not the
 * calling convention: the body reads scale from a0 and vector from a1. */
void gVectorScaleInPlace(s32 factor, s32 *xyz) __asm__("func_8001AB78");

static void test_numeric_state(void)
{
    struct { s32 before, xyz[3], after; } vec = { 25, { 200003, -200003, 65536 }, -33 };
    static const struct { int start, dest, value; } rolls[] = {
        { 1, 0, 3 }, { 8, 7, 17 }, { 0, 19, 21 }, { 18, 17, 17 }, { 19, 18, 19 }
    };
    int i, j;

    /* @leaf 80019668 source additive generator including both cursor and tap wrap boundaries */
    LK_FUNC("func_80019668");
    for (i = 0; i < sizeof(rolls) / sizeof(rolls[0]); i++) {
        for (j = 0; j < 20; j++) gStateTable[j] = j;
        gPrimaryPrngCursor = rolls[i].start;
        FCK_EQ(gAdvancePrimaryPrng(), rolls[i].value, "sum of deliberately distinct tap indices");
        FCK_EQ(gPrimaryPrngCursor, rolls[i].dest, "cursor steps backward around 20 entries");
        FCK_EQ(gStateTable[rolls[i].dest], rolls[i].value, "sum becomes next state");
        for (j = 0; j < 20; j++) {
            if (j != rolls[i].dest)
                FCK_EQ(gStateTable[j], j, "nonselected state word preserved");
        }
    }

    /* @leaf 8001AB78 math signed Q16.16 in-place scale with a product wider than 32 bits */
    LK_FUNC("func_8001AB78");
    gVectorScaleInPlace(0x18000, vec.xyz); /* 1.5x, rounds down */
    FCK_EQ(vec.xyz[0], 300004, "positive half rounds down");
    FCK_EQ(vec.xyz[1], -300005, "negative half rounds down");
    FCK_EQ(vec.xyz[2], 98304, "unit vector component becomes factor");
    FCK_CHECK(vec.before == 25 && vec.after == -33, "three words only");
    gVectorScaleInPlace(0, vec.xyz);
    FCK_CHECK(!vec.xyz[0] && !vec.xyz[1] && !vec.xyz[2], "zero factor clears every component");
}

struct FadeLayer {
    u8 pad[0x18];
    u8 enabled, red, green, blue;
    u8 tail[0x40];
};
u8 gLayerIndex __asm__("D_8009F225");
struct FadeLayer gLayerBlocks[3] __asm__("D_800A06E0");
u8 gDisplayEnabled __asm__("D_8009F608");
u8 gCompletionFlag __asm__("D_8009FA68");
s32 gFileSlotState __asm__("D_8009F620");
void gSetFadeLayerColor(u32 color) __asm__("func_8001C040");
void gSetDisplayEnabled(u8 value) __asm__("func_8001BFD8");
void gFileReadDoneCallback(s32 result) __asm__("func_8001DFF0");

static void test_render_and_file_flags(void)
{
    struct FadeLayer expected[3];
    int i;

    /* @leaf 8001BFD8 source setter stores the supplied byte, not a normalized Boolean */
    LK_FUNC("func_8001BFD8");
    gDisplayEnabled = 1;
    gSetDisplayEnabled(0xfe);
    FCK_EQ(gDisplayEnabled, 0xfe, "noncanonical display state byte is preserved");
    gSetDisplayEnabled(0);
    FCK_EQ(gDisplayEnabled, 0, "zero disables display state");

    /* @leaf 8001C040 source first N layer colors change; enable flags and unselected layer survive */
    LK_FUNC("func_8001C040");
    lk_fill(gLayerBlocks, 0xa5, sizeof(gLayerBlocks));
    gLayerIndex = 0;
    gSetFadeLayerColor(0xca125ba7);
    FCK_CHECK(lk_all(gLayerBlocks, 0, sizeof(gLayerBlocks), 0xa5), "zero layers is a no-op");
    memcpy(expected, gLayerBlocks, sizeof(expected));
    for (i = 0; i < 2; i++) {
        expected[i].red = 0x12;
        expected[i].green = 0x5b;
        expected[i].blue = 0xa7;
    }
    gLayerIndex = 2;
    gSetFadeLayerColor(0xca125ba7);
    FCK_CHECK(memcmp(gLayerBlocks, expected, sizeof(expected)) == 0, "RGB order, stride and exclusive upper bound");

    /* @leaf 8001DFF0 semantic completion publishes the full signed result independently of its sign */
    LK_FUNC("func_8001DFF0");
    gCompletionFlag = 0;
    gFileSlotState = 123;
    gFileReadDoneCallback(-19);
    FCK_EQ(gCompletionFlag, 1, "failed read still completes");
    FCK_EQ(gFileSlotState, -19, "failure result reaches waiter");
    gFileReadDoneCallback(0x12345678);
    FCK_EQ(gFileSlotState, 0x12345678, "next result replaces previous one without truncation");
    FCK_EQ(gCompletionFlag, 1, "completion remains set");
}

struct CardState {
    u8 pad00[0x8f4];
    s32 busy, operation;
    u8 pad8fc[0x10];
    s32 arg3, arg2;
    u8 pad914[0x80];
    s32 result, arg0, arg1;
    u8 pad9a0[4];
    s32 rejected;
    u8 tail[0x1210];
};
struct CardState *gOperationState __asm__("D_8009AB08");
s32 gIsMcManagerBusy(void) __asm__("func_8001F180");
void gQueueMcOpMode2(u32 arg0, u32 arg1, u32 arg2, u32 arg3) __asm__("func_8001F1B8");
int gGetMcStateSize(void) __asm__("func_80020610");
static struct CardState card, expected_card;

static void test_card_operations(void)
{
    lk_fill(&card, 0xa5, sizeof(card));
    gOperationState = &card;

    /* @leaf 8001F180 source each of the two signed pending flags independently makes the manager busy */
    LK_FUNC("func_8001F180");
    card.busy = card.operation = 0;
    FCK_EQ(gIsMcManagerBusy(), 0, "both fields idle");
    card.busy = -4;
    FCK_EQ(gIsMcManagerBusy(), 1, "negative busy field still counts");
    card.busy = 0;
    card.operation = 2;
    FCK_EQ(gIsMcManagerBusy(), 1, "queued operation alone counts");

    /* @leaf 8001F1B8 source mode-2 queue fills request only when the real busy predicate is false */
    LK_FUNC("func_8001F1B8");
    memcpy(&expected_card, &card, sizeof(card));
    expected_card.rejected = 1;
    gQueueMcOpMode2(13, 42, 87, 104);
    FCK_CHECK(memcmp(&card, &expected_card, sizeof(card)) == 0, "busy path sets rejection only");
    card.operation = 0;
    memcpy(&expected_card, &card, sizeof(card));
    expected_card.arg0 = 13;
    expected_card.arg1 = 42;
    expected_card.arg2 = 87;
    expected_card.arg3 = 104;
    expected_card.result = expected_card.rejected = 0;
    expected_card.operation = 2;
    gQueueMcOpMode2(13, 42, 87, 104);
    FCK_CHECK(memcmp(&card, &expected_card, sizeof(card)) == 0, "idle path queues arguments and clears status");

    /* @leaf 80020610 source allocation size covers the last word at offset 0x1bb4 */
    LK_FUNC("func_80020610");
    FCK_EQ(gGetMcStateSize(), sizeof(struct CardState), "memory-card allocation matches request layout extent");
}

struct Range { u16 base, start; };
struct Range gCharRangeTable[3] __asm__("D_8009AB0C");
u16 gPunctuationTable[33] __asm__("D_8009AB18");
u16 gEncodeCardTitleGlyph(u8 c) __asm__("func_800210D0");

static void test_title_glyphs(void)
{
    static const struct { u8 ascii; u16 encoded; } cases[] = {
        { 0x1f, 0 }, { ' ', 0x0080 }, { '/', 0x0f80 },
        { '0', 0x1281 }, { '9', 0x1b81 }, { ':', 0x1080 }, { '@', 0x1680 },
        { 'A', 0x3482 }, { 'Z', 0x4d82 }, { '[', 0x1780 }, { '`', 0x1c80 },
        { 'a', 0x5683 }, { 'z', 0x6f83 }, { '{', 0x1d80 }, { '~', 0x2080 },
        { 0x7f, 0 }, { 0xff, 0 }
    };
    int i;

    /* @leaf 800210D0 source ASCII group boundaries against synthetic tables and big-endian title words */
    LK_FUNC("func_800210D0");
    gCharRangeTable[0].base = 0x8112;
    gCharRangeTable[0].start = '0';
    gCharRangeTable[1].base = 0x8234;
    gCharRangeTable[1].start = 'A';
    gCharRangeTable[2].base = 0x8356;
    gCharRangeTable[2].start = 'a';
    for (i = 0; i < 33; i++) gPunctuationTable[i] = 0x8000 + i;
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++)
        FCK_EQ(gEncodeCardTitleGlyph(cases[i].ascii), cases[i].encoded, "title glyph group or unsupported byte");
}

struct VoiceSlot {
    s32 active;
    u8 opaque[0x14];
    s32 owner;
};
struct SoundSystem {
    s32 bank_count;
    void **banks;
    s32 channel_count;
    void *channels;
    s32 state[3];
    struct VoiceSlot voices[3];
    s32 volume;
};
struct StreamChannel {
    s32 id, voice, left, right;
    u8 opaque[0x14];
};
struct Mixer {
    u8 configured;
    u8 pad01[3];
    s32 active, state08, stopping;
    s16 state10;
    u8 pad12[14];
    s32 state20;
    struct StreamChannel channels[2];
    u8 pad6c[4];
    s32 rate, current_rate, state78, state7c, ready, state84, deadline;
};
struct SoundSystem gSoundSystem __asm__("D_800A07A8");
struct Mixer gMixerState __asm__("D_800A0968");
s32 gSpuUploadCompleted __asm__("D_8009F628");
void gResetSoundSystemState(void) __asm__("func_80021508");
void gSignalSpuUploadComplete(void) __asm__("func_80021888");
u32 gIsAudioStreamConfigured(void) __asm__("func_800226C8");
void gResetAudioStreamController(void) __asm__("func_80022CA8");

static void test_audio_initialization(void)
{
    struct SoundSystem sound;
    struct Mixer mix;
    void *bank = &sound;
    int i;

    /* @leaf 80021508 source resets allocations and activity, retains slot metadata, sets default volume */
    LK_FUNC("func_80021508");
    lk_fill(&gSoundSystem, 0xa5, sizeof(gSoundSystem));
    gSoundSystem.banks = &bank;
    gSoundSystem.channels = &mix;
    memcpy(&sound, &gSoundSystem, sizeof(sound));
    sound.bank_count = sound.channel_count = 0;
    sound.banks = 0;
    sound.channels = 0;
    for (i = 0; i < 3; i++) {
        sound.state[i] = 0;
        sound.voices[i].active = 0;
    }
    sound.volume = 127;
    gResetSoundSystemState();
    FCK_CHECK(memcmp(&gSoundSystem, &sound, sizeof(sound)) == 0, "reset touches precisely allocations, activity and volume");

    /* @leaf 80021888 semantic completion callback replaces pending or stale status with ready */
    LK_FUNC("func_80021888");
    gSpuUploadCompleted = -1;
    gSignalSpuUploadComplete();
    FCK_EQ(gSpuUploadCompleted, 1, "callback publishes readiness");
    gSignalSpuUploadComplete();
    FCK_EQ(gSpuUploadCompleted, 1, "repeated completion is not an increment");

    /* @leaf 800226C8 source configured query returns unsigned state without Boolean normalization */
    LK_FUNC("func_800226C8");
    gMixerState.configured = 0;
    FCK_EQ(gIsAudioStreamConfigured(), 0, "unconfigured mixer");
    gMixerState.configured = 0xd3;
    FCK_EQ(gIsAudioStreamConfigured(), 0xd3, "configuration byte is widened unsigned");

    /* @leaf 80022CA8 source reset primes both channels and snapshots rate without destroying metadata */
    LK_FUNC("func_80022CA8");
    lk_fill(&gMixerState, 0xb6, sizeof(gMixerState));
    gMixerState.rate = 32000;
    memcpy(&mix, &gMixerState, sizeof(mix));
    mix.configured = mix.active = mix.state08 = mix.stopping = mix.state10 = mix.state20 = 0;
    mix.state78 = mix.state7c = mix.state84 = mix.deadline = 0;
    mix.current_rate = 32000;
    mix.ready = 1;
    for (i = 0; i < 2; i++) {
        mix.channels[i].id = -1;
        mix.channels[i].left = 0;
        mix.channels[i].right = 64;
    }
    gResetAudioStreamController();
    FCK_CHECK(memcmp(&gMixerState, &mix, sizeof(mix)) == 0, "controller timers, channel defaults and preserved bytes");
}

struct ModeObject { u8 pad[8]; s32 value; };
struct ModeState { s32 mode; struct ModeObject *object; s32 value; };
struct ModeState gVolumeLevels __asm__("D_800A1800");
u32 gOutStreamId __asm__("D_8009F6BC");
int gTrackedSoundState __asm__("D_800A0A88");
void gSetVolumeModeState(s32 mode, struct ModeObject *object) __asm__("func_80044FF8");
s32 gIsTrackedSoundId(s32 mode) __asm__("func_80045028");
u8 gCheckOutputStreamVolumeMode(void) __asm__("func_80029BC0");
int gIsTrackedSoundPlaying(void) __asm__("func_80024170");

static void test_tracked_audio(void)
{
    struct ModeObject first = { { 0 }, 45 }, second = { { 0 }, -429 };

    /* @leaf 80044FF8 source mode selection captures the object value; disabling does not dereference or rebind it */
    LK_FUNC("func_80044FF8");
    gVolumeLevels.mode = 5;
    gVolumeLevels.object = &first;
    gVolumeLevels.value = first.value;
    gSetVolumeModeState(0, &second);
    FCK_EQ(gVolumeLevels.mode, 0, "zero is a valid active mode");
    FCK_CHECK(gVolumeLevels.object == &second, "selected object retained");
    FCK_EQ(gVolumeLevels.value, -429, "signed value captured");
    second.value = 7;
    gSetVolumeModeState(-1, 0);
    FCK_EQ(gVolumeLevels.mode, -1, "disable sentinel installed");
    FCK_CHECK(gVolumeLevels.object == &second && gVolumeLevels.value == -429, "disable retains cached binding");

    /* @leaf 80045028 source tracks an exact id only while no disable sentinel is set */
    LK_FUNC("func_80045028");
    gVolumeLevels.mode = -1;
    FCK_EQ(gIsTrackedSoundId(-1), 0, "disabled never matches itself");
    gVolumeLevels.mode = 0x1234;
    FCK_EQ(gIsTrackedSoundId(0x1234), 1, "exact id matches");
    FCK_EQ(gIsTrackedSoundId(0x34), 0, "not just low byte");

    /* @leaf 80029BC0 source output stream queries the real tracked-id predicate with its full stored id */
    LK_FUNC("func_80029BC0");
    gVolumeLevels.mode = 0x3456;
    gOutStreamId = 0x3456;
    FCK_EQ(gCheckOutputStreamVolumeMode(), 1, "matching output stream");
    gOutStreamId = 0x3457;
    FCK_EQ(gCheckOutputStreamVolumeMode(), 0, "different output stream");
    gOutStreamId = 0x3456;
    gVolumeLevels.mode = -1;
    FCK_EQ(gCheckOutputStreamVolumeMode(), 0, "tracking suspended");

    /* @leaf 80024170 source only state one represents playback */
    LK_FUNC("func_80024170");
    gTrackedSoundState = 1;
    FCK_EQ(gIsTrackedSoundPlaying(), 1, "playing state");
    gTrackedSoundState = 2;
    FCK_EQ(gIsTrackedSoundPlaying(), 0, "other nonzero state not playing");
    gTrackedSoundState = 0;
    FCK_EQ(gIsTrackedSoundPlaying(), 0, "idle state");
}

struct VagmChunk { u8 pad[0x14]; u16 count, index; };
u8 gVagmQueueDraining __asm__("D_8009F308");
s32 gVagmReadIndex __asm__("D_8009F6CC");
u32 gVagmNextChunkIndex __asm__("D_8009F6E4");
struct VagmChunk *gVagmRing[4] __asm__("D_800A0C50");
s32 gDequeueVagmChunk(void) __asm__("func_8002A398");

static void test_vagm_dequeue(void)
{
    struct VagmChunk earlier = { { 0 }, 7, 2 }, last = { { 0 }, 7, 6 };

    /* @leaf 8002A398 source blocked and empty queues preserve state; chunk and ring counters wrap separately */
    LK_FUNC("func_8002A398");
    gVagmRing[0] = &earlier;
    gVagmRing[1] = gVagmRing[2] = 0;
    gVagmRing[3] = &last;
    gVagmQueueDraining = 1;
    gVagmReadIndex = 3;
    gVagmNextChunkIndex = 99;
    FCK_EQ(gDequeueVagmChunk(), 0, "drain in progress blocks nonempty queue");
    FCK_CHECK(gVagmRing[3] == &last && gVagmReadIndex == 3 && gVagmNextChunkIndex == 99, "blocked queue is unchanged");
    gVagmQueueDraining = 0;
    FCK_EQ(gDequeueVagmChunk(), (s32)&last, "returns tail pointer");
    FCK_EQ(gVagmNextChunkIndex, 0, "chunk index wraps at count");
    FCK_CHECK(!gVagmRing[3] && gVagmReadIndex == 0, "ring wraps after slot three");
    FCK_EQ(gDequeueVagmChunk(), (s32)&earlier, "returns first slot after ring wrap");
    FCK_EQ(gVagmNextChunkIndex, 3, "ordinary chunk successor");
    FCK_CHECK(!gVagmRing[0] && gVagmReadIndex == 1, "ordinary dequeue advances one slot");
    FCK_EQ(gDequeueVagmChunk(), 0, "empty slot yields no chunk");
    FCK_CHECK(gVagmReadIndex == 1 && gVagmNextChunkIndex == 3, "empty slot doesn't consume a slot or chunk number");
}

struct ValuePair { u8 pad[0x10c]; u16 source, destination; };
void gCopyPairSourceToDestination(struct ValuePair *value) __asm__("func_8002B1D0");
u32 gInitStateWord __asm__("D_8009FA18");
u32 gGpStateWord __asm__("D_8009F6F0");
void gClearInitStateWords(void) __asm__("func_8002A598");

struct Vec3 { s32 x, y, z; };
struct StrategicEntry {
    struct Vec3 first, second;
    s16 pad18;
    u8 kind, pad1b;
};
struct StrategicObject {
    u8 pad00[0xdc];
    struct Vec3 position;
    u8 pade8[0x4c];
    struct StrategicEntry *entry;
};
struct ReserveObject { u8 pad[0xac]; u8 *control; };
struct ReserveTarget { u8 pad00[8]; s16 base; u8 pad0a[0x1e]; s16 alternative; };
struct SessionState {
    u8 pad00[8];
    s32 reserve_primary;
    u8 pad0c[0x8c];
    s32 reserve_secondary;
    u8 pad9c[0x44];
    s32 strategic_kind;
};
struct SessionState *gGameSession __asm__("D_8009FA94");
void gUpdateDroneStrategicPosition(struct StrategicObject *object) __asm__("func_800330E8");
s32 gGetScaledWeaponReserveLimit(struct ReserveObject *object, struct ReserveTarget *target, s32 flag)
    __asm__("func_8006BD48");

static void test_game_state(void)
{
    struct ValuePair pair;
    struct StrategicEntry entry = { { -5, 8, 0 }, { 2, -1, 6 }, 0, 0, 0 };
    struct StrategicObject object = { { 0 }, { 99, 99, 99 }, { 0 }, &entry };
    struct SessionState session = { { 0 }, 0, { 0 }, 2, { 0 }, 55 };
    u8 control[0xc2] = { 0 };
    struct ReserveObject reserve = { { 0 }, control };
    struct ReserveTarget target = { { 0 }, 11, { 0 }, -13 };

    /* @leaf 8002A598 source clears both independent initialization words */
    LK_FUNC("func_8002A598");
    gInitStateWord = 0x80010001;
    gGpStateWord = 0x33445566;
    gClearInitStateWords();
    FCK_EQ(gInitStateWord, 0, "first initialization word clears");
    FCK_EQ(gGpStateWord, 0, "second initialization word clears");

    /* @leaf 8002B1D0 semantic copy preserves all 16 source bits and touches no preceding bytes */
    LK_FUNC("func_8002B1D0");
    lk_fill(&pair, 0xa5, sizeof(pair));
    pair.source = 0xfd18;
    gCopyPairSourceToDestination(&pair);
    FCK_EQ(pair.destination, 0xfd18, "high bit and low byte copied");
    FCK_EQ(pair.source, 0xfd18, "source halfword preserved");
    FCK_CHECK(lk_all(pair.pad, 0, sizeof(pair.pad), 0xa5), "earlier fields preserved");

    /* @leaf 800330E8 source midpoint rounds down, endpoints select directly, only kind one updates session */
    LK_FUNC("func_800330E8");
    gGameSession = &session;
    gUpdateDroneStrategicPosition(&object);
    FCK_CHECK(object.position.x == -2 && object.position.y == 3 && object.position.z == 3, "floor of each midpoint");
    FCK_EQ(session.strategic_kind, 55, "midpoint doesn't change session kind");
    entry.kind = 2;
    gUpdateDroneStrategicPosition(&object);
    FCK_CHECK(memcmp(&object.position, &entry.second, sizeof(struct Vec3)) == 0, "kind two picks second endpoint");
    FCK_EQ(session.strategic_kind, 55, "second endpoint doesn't change session kind");
    entry.kind = 1;
    gUpdateDroneStrategicPosition(&object);
    FCK_CHECK(memcmp(&object.position, &entry.first, sizeof(struct Vec3)) == 0, "kind one picks first endpoint");
    FCK_EQ(session.strategic_kind, 1, "kind one marks the session");
    entry.first.x = 100;
    entry.kind = 3;
    gUpdateDroneStrategicPosition(&object);
    FCK_EQ(object.position.x, -5, "unknown kind leaves position alone");

    /* @leaf 8006BD48 source reserve field and session selectors are independent; signed scaling and unlimited mode */
    LK_FUNC("func_8006BD48");
    FCK_EQ(gGetScaledWeaponReserveLimit(&reserve, &target, 0x100), 11, "high flag bits ignored");
    session.reserve_primary = 1;
    FCK_EQ(gGetScaledWeaponReserveLimit(&reserve, &target, 0), 13, "quarter bonus rounds down");
    FCK_EQ(gGetScaledWeaponReserveLimit(&reserve, &target, 1), -17, "signed alternative uses floor quarter");
    control[0xc1] = 0x80;
    FCK_EQ(gGetScaledWeaponReserveLimit(&reserve, &target, 1), -20, "nonzero controller byte chooses half bonus");
    session.reserve_secondary = 3;
    FCK_EQ(gGetScaledWeaponReserveLimit(&reserve, &target, 0), 9999, "unlimited reserve ignores base field");
    session.reserve_secondary = 4;
    FCK_EQ(gGetScaledWeaponReserveLimit(&reserve, &target, 1), -13, "unknown mode uses unscaled alternative");
}

struct SpawnFlagObject { u8 pad[0x10]; u8 flags; };
struct SpawnDescObject { u8 pad00[0x20]; u8 index; u8 pad21[3]; u32 flags; };
u32 gSpawnTypeFlags[2][256] __asm__("D_800A6578");
void *gSpawnRecordMap[2][256] __asm__("D_800A7040");
u8 *gSlotSpawnDescs[256] __asm__("D_800A0C70");
u32 *gGetSpawnTypeFlagPtr(u32 column, u32 row) __asm__("func_8003B3D8");
void gApplySpawnTypeFlags(struct SpawnFlagObject *object, u32 column, s32 row) __asm__("func_8003B3F8");
u8 gGetDestructionSpawnColumn(struct SpawnDescObject *object) __asm__("func_8003B7D8");
u8 gGetSpawnDescColumn(struct SpawnDescObject *object) __asm__("func_8003B838");

static void test_spawn_tables(void)
{
    struct SpawnFlagObject flag_object = { { 0 }, 0x15 };
    struct SpawnDescObject desc_object = { { 0 }, 255, { 0 }, 0x80000 };
    u8 desc[2] = { 0xdb, 0xf1 };
    int i;

    /* @leaf 8003B3D8 math row-major 256-column word addressing, with low-byte column wrap */
    LK_FUNC("func_8003B3D8");
    FCK_CHECK(gGetSpawnTypeFlagPtr(0, 0) == &gSpawnTypeFlags[0][0], "first cell");
    FCK_CHECK(gGetSpawnTypeFlagPtr(255, 0) == &gSpawnTypeFlags[0][255], "end of first row");
    FCK_CHECK(gGetSpawnTypeFlagPtr(0x100, 1) == &gSpawnTypeFlags[1][0], "column wraps without changing row");
    FCK_CHECK(gGetSpawnTypeFlagPtr(0xabc, 1) == &gSpawnTypeFlags[1][188], "row and nonzero wrapped column");

    /* @leaf 8003B3F8 source absent record leaves flags unchanged; present record ORs only its low byte */
    LK_FUNC("func_8003B3F8");
    lk_fill(gSpawnRecordMap, 0, sizeof(gSpawnRecordMap));
    lk_fill(gSpawnTypeFlags, 0, sizeof(gSpawnTypeFlags));
    gSpawnTypeFlags[1][255] = 0x998877a2;
    gSpawnRecordMap[0][255] = &flag_object;
    gApplySpawnTypeFlags(&flag_object, 0x1ff, 1);
    FCK_EQ(flag_object.flags, 0x15, "decoy record in different row doesn't qualify");
    gSpawnRecordMap[1][255] = &flag_object;
    gApplySpawnTypeFlags(&flag_object, 0x1ff, 1);
    FCK_EQ(flag_object.flags, 0xb7, "selected bits merge, not replace");
    FCK_CHECK(lk_all(flag_object.pad, 0, sizeof(flag_object.pad), 0), "only flags byte changes");

    for (i = 0; i < 256; i++) gSlotSpawnDescs[i] = 0;
    gSlotSpawnDescs[255] = gSlotSpawnDescs[0] = desc;

    /* @leaf 8003B7D8 source destruction uses descriptor byte one and independently gates zero index, flag and missing record */
    LK_FUNC("func_8003B7D8");
    FCK_EQ(gGetDestructionSpawnColumn(&desc_object), 0xf1, "destruction column, maximum unsigned index");
    desc_object.flags |= 0x40000;
    FCK_EQ(gGetDestructionSpawnColumn(&desc_object), 0, "exclusion flag overrides record");
    desc_object.flags = 0;
    desc_object.index = 0;
    FCK_EQ(gGetDestructionSpawnColumn(&desc_object), 0, "slot zero never uses record");
    desc_object.index = 1;
    FCK_EQ(gGetDestructionSpawnColumn(&desc_object), 0, "missing record yields zero");

    /* @leaf 8003B838 source regular spawn uses descriptor byte zero, with the same independent validity gates */
    LK_FUNC("func_8003B838");
    desc_object.flags = 0x80000;
    desc_object.index = 255;
    FCK_EQ(gGetSpawnDescColumn(&desc_object), 0xdb, "normal column distinct from destruction column");
    desc_object.flags |= 0x40000;
    FCK_EQ(gGetSpawnDescColumn(&desc_object), 0, "excluded object");
    desc_object.flags = 0;
    desc_object.index = 0;
    FCK_EQ(gGetSpawnDescColumn(&desc_object), 0, "reserved index");
    desc_object.index = 1;
    FCK_EQ(gGetSpawnDescColumn(&desc_object), 0, "null descriptor");
}

struct PlayerInfo { u8 pad[0x300]; void *target; };
struct PlayerEntity { u8 pad[0xac]; struct PlayerInfo *info; };
struct TargetObject { u8 pad[0xa0]; s8 side; };
struct PlayerEntity *gPlayerEntities[2] __asm__("D_8009FA58");
void gSetPlayerInfoTarget(struct TargetObject *object) __asm__("func_800423F0");
s32 gClearPlayerInfoTarget(s32 message, struct TargetObject *object) __asm__("func_80042470");

static void test_player_targets(void)
{
    struct TargetObject target = { { 0 }, 0 }, other = { { 0 }, 1 };
    struct PlayerInfo info0 = { { 0 }, &other }, info1 = { { 0 }, &other };
    struct PlayerEntity left = { { 0 }, &info0 }, right = { { 0 }, &info1 };

    /* @leaf 800423F0 source side routing includes both-player case and missing entity/info */
    LK_FUNC("func_800423F0");
    gPlayerEntities[0] = &left;
    gPlayerEntities[1] = &right;
    gSetPlayerInfoTarget(&target);
    FCK_CHECK(info0.target == &target && info1.target == &other, "side zero changes left only");
    target.side = 1;
    info0.target = &other;
    gSetPlayerInfoTarget(&target);
    FCK_CHECK(info0.target == &other && info1.target == &target, "side one changes right only");
    target.side = -1;
    gSetPlayerInfoTarget(&target);
    FCK_CHECK(info0.target == &target && info1.target == &target, "neutral object changes both");
    gPlayerEntities[0] = 0;
    right.info = 0;
    gSetPlayerInfoTarget(&other);
    other.side = -1;
    gSetPlayerInfoTarget(&other);
    FCK_CHECK(info0.target == &target && info1.target == &target, "absent entity and info are skipped");

    /* @leaf 80042470 source clear routes by side but never clears someone else's target */
    LK_FUNC("func_80042470");
    gPlayerEntities[0] = &left;
    right.info = &info1;
    target.side = 1;
    FCK_EQ(gClearPlayerInfoTarget(19, &target), 0, "handler reports zero");
    FCK_CHECK(info0.target == &target && !info1.target, "right-only clear");
    target.side = -1;
    info1.target = &other;
    gClearPlayerInfoTarget(19, &target);
    FCK_CHECK(!info0.target && info1.target == &other, "neutral clears match and preserves mismatch");
    target.side = 0;
    info0.target = info1.target = &target;
    gClearPlayerInfoTarget(19, &target);
    FCK_CHECK(!info0.target && info1.target == &target, "left-only clear");
    left.info = 0;
    gPlayerEntities[1] = 0;
    target.side = -1;
    FCK_EQ(gClearPlayerInfoTarget(19, &target), 0, "missing entity and info are skipped");
}

void gPackAnimEventBits(u32 code, u32 key, s32 *shift, u32 *bits) __asm__("func_80044AE8");

static void test_animation_event_bits(void)
{
    int shift = 0;
    u32 bits = 0x80000000;

    /* @leaf 80044AE8 source append matching six-bit events and reject a cursor at 26 bits */
    LK_FUNC("func_80044AE8");
    gPackAnimEventBits(0xdead942a, 0x2b, &shift, &bits);
    FCK_CHECK(shift == 0 && bits == 0x80000000, "mismatched ten-bit key leaves output alone");
    gPackAnimEventBits(0xdead942a, 0x2a, &shift, &bits);
    FCK_CHECK(shift == 6 && bits == 0x80000025, "first event excludes upper input bits");
    gPackAnimEventBits(0x102a, 0x2a, &shift, &bits);
    FCK_CHECK(shift == 12 && bits == 0x80000125, "second event appends to existing word");
    shift = 25;
    gPackAnimEventBits(0xfc2a, 0x2a, &shift, &bits);
    FCK_CHECK(shift == 31 && bits == 0xfe000125, "last allowed cursor 25");
    shift = 26;
    gPackAnimEventBits(0xfc2a, 0x2a, &shift, &bits);
    FCK_CHECK(shift == 26 && bits == 0xfe000125, "cursor 26 is rejected");
}

struct SoundVoice {
    s32 bank;
    s8 volume, base_volume, pan, extra;
    s32 priority;
    u8 pad[0x18];
    struct SoundVoice *next, *prev;
};
struct SoundBank {
    s32 samples[4];
    u8 count, cursor, voices;
    u8 pad[5];
    struct SoundVoice *head;
};
struct SoundBank gSoundEntryTable[2] __asm__("D_800A7938");
void gInsertSoundBankVoiceByPriority(struct SoundVoice *voice) __asm__("func_800469A0");

static void test_sound_priority(void)
{
    struct SoundVoice voice[5];
    struct SoundVoice *order[] = { &voice[1], &voice[3], &voice[0], &voice[2], &voice[4] };
    static const int priorities[] = { 5, -3, 5, 0, 12 };
    int i;

    /* @leaf 800469A0 semantic stable ascending priority insertion, with bidirectional links and isolated banks */
    LK_FUNC("func_800469A0");
    lk_fill(gSoundEntryTable, 0, sizeof(gSoundEntryTable));
    gSoundEntryTable[0].voices = 4;
    lk_fill(voice, 0, sizeof(voice));
    for (i = 0; i < 5; i++) {
        voice[i].bank = 1;
        voice[i].priority = priorities[i];
        gInsertSoundBankVoiceByPriority(&voice[i]);
    }
    FCK_CHECK(gSoundEntryTable[1].head == order[0], "lowest priority is head");
    FCK_EQ(gSoundEntryTable[1].voices, 5, "selected bank tracks all inserts");
    FCK_CHECK(gSoundEntryTable[0].voices == 4 && !gSoundEntryTable[0].head, "other bank untouched");
    for (i = 0; i < 5; i++) {
        FCK_CHECK(order[i]->prev == (i == 0 ? 0 : order[i - 1]), "back link in stable priority order");
        FCK_CHECK(order[i]->next == (i == 4 ? 0 : order[i + 1]), "forward link in stable priority order");
    }
}

struct LinkNode {
    u8 pad00[0x48];
    struct LinkNode *back;
    u8 pad4c[0x38];
    struct LinkNode *forward;
};
void gInsertNodeAfter(struct LinkNode *left, struct LinkNode *right) __asm__("func_8004AAC0");

struct Entity {
    struct Entity *next;
    s32 (*update)(s32 message, struct Entity *entity, s32 arg2, s32 arg3);
    s32 id;
    u8 pad[8];
    u16 tag;
};
struct Entity *gUnitListHead __asm__("D_8009FA40");
s32 gSendMessageByTag(s32 tag, s32 arg) __asm__("func_8004DE80");

static s32 receive_tag_message(s32 message, struct Entity *entity, s32 arg2, s32 arg3)
{
    fck_record("entity message", message, arg2, arg3);
    return entity->id;
}

static void test_lists(void)
{
    struct LinkNode first = { { 0 }, 0, { 0 }, 0 };
    struct LinkNode added = { { 0 }, 0, { 0 }, 0 };
    struct LinkNode old = { { 0 }, &first, { 0 }, 0 };
    struct Entity second = { 0, receive_tag_message, 38, { 0 }, 0xfedc };
    struct Entity winner = { &second, receive_tag_message, -91, { 0 }, 0xfedc };
    struct Entity head = { &winner, receive_tag_message, 10, { 0 }, 1 };

    /* @leaf 8004AAC0 source pushes onto secondary link chain using the new node's back field */
    LK_FUNC("func_8004AAC0");
    first.forward = &old;
    gInsertNodeAfter(&first, &added);
    FCK_CHECK(first.forward == &added && added.back == &old, "preserves prior chain through new node");
    FCK_CHECK(old.back == &first && !added.forward, "unrelated links do not change");
    first.forward = 0;
    gInsertNodeAfter(&first, &added);
    FCK_CHECK(first.forward == &added && !added.back, "empty chain installs null back link");

    /* @leaf 8004DE80 source first matching unsigned tag receives message 12 and its result propagates */
    LK_FUNC("func_8004DE80");
    gUnitListHead = &head;
    fck_log_reset();
    FCK_EQ(gSendMessageByTag(0xfedc, -32), -91, "first duplicate wins");
    FCK_EQ(fck_log_count, 1, "one callback only");
    FCK_CHECK(fck_log[0].a0 == 12 && fck_log[0].a1 == -32 && !fck_log[0].a2, "callback arguments forwarded");
    FCK_EQ(gSendMessageByTag(0x1fedc, 55), 0, "full input tag must match");
    gUnitListHead = 0;
    FCK_EQ(gSendMessageByTag(0xfedc, 55), 0, "empty list no match");
    FCK_EQ(fck_log_count, 1, "miss paths invoke no callback");
}

struct PulseColors { u8 pad[0x44]; s32 red, mixed, green; };
struct PulseColors gMarkerColorTable __asm__("D_800A6F60");
s32 gFrameTime __asm__("D_8009FAA4");
void gUpdateMarkerPulseColors(void) __asm__("func_8004CC68");

static void test_marker_pulse(void)
{
    /* RGB bytes are 8x brightness; mixed red and green are half-bright. */
    static const struct { int tick, bright, mixed; } cases[] = {
        { 0, 64, 0x402020 }, { 7, 64, 0x402020 }, { 8, 72, 0x482020 },
        { 183, 240, 0xf07878 }, { 184, 248, 0xf87878 }, { 255, 248, 0xf87878 },
        { 256, 248, 0xf87878 }, { 504, 64, 0x402020 }, { 512, 64, 0x402020 }
    };
    int i;

    /* @leaf 8004CC68 source 512-tick triangle pulse with quantization, plateau and three color layouts */
    LK_FUNC("func_8004CC68");
    lk_fill(&gMarkerColorTable, 0xa5, sizeof(gMarkerColorTable));
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        gFrameTime = cases[i].tick;
        gUpdateMarkerPulseColors();
        FCK_EQ(gMarkerColorTable.red, cases[i].bright, "red in low byte");
        FCK_EQ(gMarkerColorTable.mixed, cases[i].mixed, "half red/green, full blue");
        FCK_EQ(gMarkerColorTable.green, cases[i].bright * 256, "green in middle byte");
    }
    FCK_CHECK(lk_all(gMarkerColorTable.pad, 0, sizeof(gMarkerColorTable.pad), 0xa5), "other marker colors preserved");
}

struct MotionState {
    s32 x, y, scale;
    s16 angle;
    u8 pad0e[2];
    s16 velocity;
    u8 pad12[2];
    s32 timer;
    u8 enabled, state;
};
struct SourcePair { s32 first, second; };
struct DestinationTriple { s32 second, zero, first; };
struct FlagValue { u8 pad[0xe]; u16 flags; };
struct SourcePair gViewGroundAnchors[3] __asm__("D_800A3190");
u8 gScriptVmDisabled __asm__("D_8009F840");
void gResetMotionState(struct MotionState *state) __asm__("func_8005CEA8");
void gGetViewGroundAnchor(u32 index, struct DestinationTriple *destination) __asm__("func_80069580");
void gSetObjectFlag1000(struct FlagValue *value) __asm__("func_80073BA8");
void gDisableScriptVm(void) __asm__("func_80069D58");

static void test_motion_anchor_and_flags(void)
{
    struct MotionState motion, expected;
    struct DestinationTriple point = { 89, 99, 109 };
    struct { struct FlagValue value; u16 guard; } record;

    /* @leaf 8005CEA8 source reset restores identity scale and touches only motion fields */
    LK_FUNC("func_8005CEA8");
    lk_fill(&motion, 0xa5, sizeof(motion));
    memcpy(&expected, &motion, sizeof(motion));
    expected.x = expected.y = expected.angle = expected.velocity = expected.timer = expected.state = 0;
    expected.scale = 65536; /* Q16.16 identity */
    expected.enabled = 1;
    gResetMotionState(&motion);
    FCK_CHECK(memcmp(&motion, &expected, sizeof(expected)) == 0, "motion identity and padding preservation");

    /* @leaf 80069580 source selected anchor maps to ground-plane X and Z, always replacing Y with zero */
    LK_FUNC("func_80069580");
    gViewGroundAnchors[0].first = 11;
    gViewGroundAnchors[0].second = -22;
    gViewGroundAnchors[1].first = 333;
    gViewGroundAnchors[1].second = -444;
    gViewGroundAnchors[2].first = -123456;
    gViewGroundAnchors[2].second = 0x76543210;
    gGetViewGroundAnchor(2, &point);
    FCK_EQ(point.second, 0x76543210, "last fixture anchor X");
    FCK_EQ(point.zero, 0, "on ground plane");
    FCK_EQ(point.first, -123456, "last fixture anchor Z");
    gGetViewGroundAnchor(0, &point);
    FCK_CHECK(point.first == 11 && point.second == -22 && !point.zero, "first fixture anchor replaces all coordinates");

    /* @leaf 80073BA8 semantic set a single flag, preserving adjacent bytes and other bits */
    LK_FUNC("func_80073BA8");
    lk_fill(&record, 0xa5, sizeof(record));
    record.value.flags = 0x8395;
    gSetObjectFlag1000(&record.value);
    FCK_EQ(record.value.flags, 0x9395, "only target bit becomes set");
    gSetObjectFlag1000(&record.value);
    FCK_EQ(record.value.flags, 0x9395, "set is idempotent");
    FCK_CHECK(lk_all(record.value.pad, 0, sizeof(record.value.pad), 0xa5) &&
              record.guard == 0xa5a5, "adjacent bytes unchanged");
    record.value.flags = 0;
    gSetObjectFlag1000(&record.value);
    FCK_EQ(record.value.flags, 0x1000, "set only target bit in an empty flag word");

    /* @leaf 80069D58 source disable latches one instead of toggling or incrementing the flag */
    LK_FUNC("func_80069D58");
    gScriptVmDisabled = 0;
    gDisableScriptVm();
    FCK_EQ(gScriptVmDisabled, 1, "running VM becomes disabled");
    gDisableScriptVm();
    FCK_EQ(gScriptVmDisabled, 1, "disabled VM stays disabled");
}

u8 gMissionListTable[8] __asm__("D_8009B724");
u8 gFrontEndListFlag __asm__("D_8009F3C0");
u8 gSelectedMissionIndex __asm__("D_8009F880");
u8 gMissionListMinIndex __asm__("D_8009F881");
u8 gMissionListMaxIndex __asm__("D_8009F882");
u8 *gMissionListTablePtr __asm__("D_8009F884");
u8 gMissionListEntryCount __asm__("D_8009F888");
u8 gMissionListMode __asm__("D_8009F889");
u8 gCrimeWarProgress __asm__("D_8009F458");
u8 gSavedCrimeWarProgress __asm__("D_8009F3F1");
void gSetupCrimeWarMissionList(void) __asm__("func_800776B8");

struct WideText { u8 pad[0x1f]; u8 level; u8 tail[4]; };
struct WideText gWideTextTable[6] __asm__("D_8009B4E4");
s8 gFrontEndListTable[5] __asm__("D_8009B72C");
u16 gMainMenuItems[0x52] __asm__("D_8009BBF8"); /* halfword alignment for labels */
u8 gPausedMenuFlag __asm__("D_8009F3C3");
u8 gLockedMenuFlag __asm__("D_8009F3C4");
void gUpdateMainMenuUnlocks(void) __asm__("func_8007A168");

static void test_menu_progress(void)
{
    u8 *items = (u8 *)gMainMenuItems;
    int i;

    /* @leaf 800776B8 source full and restricted mission lists differ in accessible range and progress writes */
    LK_FUNC("func_800776B8");
    lk_fill(gMissionListTable, 0xd1, sizeof(gMissionListTable));
    gFrontEndListFlag = 0;
    gSelectedMissionIndex = 5;
    gMissionListMinIndex = 4;
    gMissionListMaxIndex = 3;
    gMissionListEntryCount = 2;
    gMissionListMode = 1;
    gMissionListTablePtr = 0;
    gCrimeWarProgress = 6;
    gSavedCrimeWarProgress = 3;
    gSetupCrimeWarMissionList();
    FCK_CHECK(!gSelectedMissionIndex && !gMissionListMinIndex && !gMissionListMode, "restricted list selection, lower bound and mode reset");
    FCK_EQ(gMissionListMaxIndex, 0, "restricted list exposes first entry only");
    FCK_CHECK(gCrimeWarProgress == 6 && gSavedCrimeWarProgress == 3, "restricted list doesn't overwrite progress");
    FCK_CHECK(gMissionListTablePtr == gMissionListTable && gMissionListEntryCount == 8, "full table still bound in restricted list");
    gFrontEndListFlag = 2;
    gSetupCrimeWarMissionList();
    FCK_EQ(gMissionListMaxIndex, 7, "full list exposes all eight entries");
    FCK_CHECK(gCrimeWarProgress == 8 && gSavedCrimeWarProgress == 8, "full access synchronizes both progress values");
    FCK_CHECK(lk_all(gMissionListTable, 0, sizeof(gMissionListTable), 0xd1), "list data never modified");

    /* @leaf 8007A168 source separate unlock gates, progress threshold, all five levels, and latch behavior */
    LK_FUNC("func_8007A168");
    lk_fill(gMainMenuItems, 0xb7, sizeof(gMainMenuItems));
    lk_fill(gWideTextTable, 0, sizeof(gWideTextTable));
    for (i = 0; i < 5; i++) {
        gFrontEndListTable[i] = i + 1;
        gWideTextTable[i + 1].level = 10;
    }
    gWideTextTable[5].level = 9;
    gPausedMenuFlag = gLockedMenuFlag = gFrontEndListFlag = 0;
    gCrimeWarProgress = 7;
    gUpdateMainMenuUnlocks();
    FCK_CHECK(lk_all(items, 0, sizeof(gMainMenuItems), 0xb7), "one short mission and one short level leave entries locked");
    FCK_CHECK(!gPausedMenuFlag && !gLockedMenuFlag, "locked flags unchanged");
    gCrimeWarProgress = 8;
    gUpdateMainMenuUnlocks();
    FCK_CHECK(gPausedMenuFlag == 1 && !gLockedMenuFlag, "mission gate independent of level gate");
    FCK_CHECK(items[0x46] == 1 && gMainMenuItems[0x4e / 2] == 0x526c, "mission gate installs control and label");
    FCK_EQ(items[0x9a], 0xb7, "level control untouched");
    gCrimeWarProgress = 0;
    items[0x46] = 0;
    gWideTextTable[5].level = 10;
    gUpdateMainMenuUnlocks();
    FCK_CHECK(items[0x46] == 1 && gPausedMenuFlag == 1, "mission gate stays latched");
    FCK_CHECK(items[0x9a] == 1 && gMainMenuItems[0xa2 / 2] == 0x5276 && gLockedMenuFlag == 1,
              "all five referenced levels unlock independent item");
}

struct MenuEntry { u8 pad[0xe]; u8 enabled; s8 group; u8 tail[0xc]; };
struct MenuState {
    u8 selected[2], count, pad03[0x19];
    s32 (*query)(s32 player);
    u8 pad20[8];
    struct MenuEntry *entries;
};
u8 *gFontBankTable __asm__("D_8009F468");
s32 gMenuUpHoldTicks[3] __asm__("D_8009F890");
s32 gMenuDownHoldTicks[3] __asm__("D_8009F898");
s32 gMenuLeftHoldTicks[3] __asm__("D_8009F8A0");
s32 gMenuRightHoldTicks[3] __asm__("D_8009F8A8");
s32 gActivePlayerIndex __asm__("D_8009F8B8");
volatile s32 gFrameTickCounter __asm__("D_8009FA2C");
void gResetMenuDirectionHoldTicks(void) __asm__("func_800785F8");
void gSelectEnabledMenuItem(s8 start, s8 direction, s8 group) __asm__("func_800787D8");
s32 gCheckMenuDirectionRepeat(s32 *tick, s32 mask) __asm__("func_80082D30");

static s32 read_menu_direction(s32 player)
{
    fck_record("menu query", player, 0, 0);
    return 0x04;
}

static void test_menu_navigation(void)
{
    struct MenuEntry entries[4] = {
        { { 0 }, 1, 0, { 0 } }, { { 0 }, 0, 1, { 0 } },
        { { 0 }, 1, 1, { 0 } }, { { 0 }, 1, 1, { 0 } }
    };
    struct MenuState menu = { { 99, 98 }, 4, { 0 }, 0, { 0 }, entries };
    s32 *directions[] = { gMenuUpHoldTicks, gMenuDownHoldTicks, gMenuLeftHoldTicks, gMenuRightHoldTicks };
    s32 tick = 0;
    int i;

    /* @leaf 800785F8 source resets timers for exactly two players and four independent directions */
    LK_FUNC("func_800785F8");
    for (i = 0; i < 4; i++) {
        directions[i][0] = 101 + i;
        directions[i][1] = -80 - i;
        directions[i][2] = 203 + i;
    }
    gResetMenuDirectionHoldTicks();
    for (i = 0; i < 4; i++) {
        FCK_CHECK(!directions[i][0] && !directions[i][1], "both players reset");
        FCK_EQ(directions[i][2], 203 + i, "adjacent test guard not touched");
    }

    /* @leaf 800787D8 source selection skips disabled/wrong-group entries and wraps in either direction */
    LK_FUNC("func_800787D8");
    gFontBankTable = 0;
    gSelectEnabledMenuItem(1, 1, 1);
    gFontBankTable = (u8 *)&menu;
    gSelectEnabledMenuItem(1, 0, 1);
    FCK_EQ(menu.selected[1], 98, "zero direction won't search past disabled candidate");
    gSelectEnabledMenuItem(4, 1, 1);
    FCK_EQ(menu.selected[1], 2, "wrap forward, bypass group zero and disabled same-group entry");
    gSelectEnabledMenuItem(-1, -1, 1);
    FCK_EQ(menu.selected[1], 3, "wrap backward to last entry");
    gSelectEnabledMenuItem(3, 1, 0);
    FCK_CHECK(menu.selected[0] == 0 && menu.selected[1] == 3, "only requested group selection changes");
    entries[2].enabled = entries[3].enabled = 0;
    gSelectEnabledMenuItem(2, -1, 1);
    FCK_EQ(menu.selected[1], 3, "no qualifying entry preserves selection");

    /* @leaf 80082D30 source repeat checks callback and direction gates and fires exactly at the delay threshold */
    LK_FUNC("func_80082D30");
    gActivePlayerIndex = 1;
    gFrameTickCounter = 281;
    fck_log_reset();
    tick = 100;
    FCK_EQ(gCheckMenuDirectionRepeat(&tick, 4), 0, "missing direction callback");
    menu.query = read_menu_direction;
    tick = 0;
    FCK_EQ(gCheckMenuDirectionRepeat(&tick, 4), 0, "zero tick means no hold");
    FCK_EQ(fck_log_count, 0, "no direction read without a hold");
    tick = 100;
    FCK_EQ(gCheckMenuDirectionRepeat(&tick, 8), 0, "different direction doesn't repeat");
    gFrameTickCounter = 280;
    FCK_EQ(gCheckMenuDirectionRepeat(&tick, 4), 0, "one tick before delay");
    FCK_EQ(tick, 100, "ineligible calls preserve timestamp");
    gFrameTickCounter = 281;
    FCK_EQ(gCheckMenuDirectionRepeat(&tick, 4), 1, "exactly 181 ticks fires");
    FCK_EQ(tick, 131, "timestamp moves to 150 ticks before now");
    FCK_CHECK(fck_log_count == 3 && fck_log[2].a0 == 1, "query reads selected player on eligible holds");
}

struct MovieSlice { u8 pad00[0x114]; s32 active, pad118, present; };
struct MovieSlice *gMovieSliceContext __asm__("D_8009F4BC");
s32 gStripScaleEnable __asm__("D_8009F4C0");
s32 gStripDitherEnabled __asm__("D_8009F4C4");
s32 gStripStepWidth __asm__("D_8009F4C8");
s32 gBufferCount __asm__("D_8009F4CC");
s32 gReleaseDoneFlag __asm__("D_8009F508");
s32 gGetMovieActiveBufferIndex(void) __asm__("func_800858E0");
void gSetMoviePresentFlag(s32 present) __asm__("func_80085900");
void gSetMovieStripDecodeDefaults(void) __asm__("func_800859B0");
void gMovieSetScaleMode(s32 mode) __asm__("func_800859D8");

static void test_movie_slice_state(void)
{
    struct MovieSlice slice = { { 0 }, 2, 0x4567, -9 };

    gMovieSliceContext = &slice;
    /* @leaf 800858E0 source returns selected buffer index rather than adjacent presentation flag */
    LK_FUNC("func_800858E0");
    FCK_EQ(gGetMovieActiveBufferIndex(), 2, "last buffer of three");
    slice.active = 0;
    FCK_EQ(gGetMovieActiveBufferIndex(), 0, "first buffer");

    /* @leaf 80085900 source stores the full word and doesn't change the active buffer */
    LK_FUNC("func_80085900");
    gSetMoviePresentFlag(0x12345678);
    FCK_EQ(slice.present, 0x12345678, "presentation word is not narrowed");
    FCK_CHECK(slice.active == 0 && slice.pad118 == 0x4567, "neighboring words untouched");
    gSetMoviePresentFlag(0);
    FCK_EQ(slice.present, 0, "clears presentation");

    /* @leaf 800859B0 source resets strip defaults including release state from stale settings */
    LK_FUNC("func_800859B0");
    gStripScaleEnable = gStripStepWidth = gStripDitherEnabled = gBufferCount = gReleaseDoneFlag = -7;
    gSetMovieStripDecodeDefaults();
    FCK_CHECK(gStripScaleEnable == 1 && gStripStepWidth == 3, "three-step scale defaults");
    FCK_CHECK(gStripDitherEnabled == 0 && gBufferCount == 3, "undithered triple-buffer defaults");
    FCK_EQ(gReleaseDoneFlag, 1, "release starts in completed state");

    /* @leaf 800859D8 source mode tests only low byte, sets two buffers in either mode and preserves release */
    LK_FUNC("func_800859D8");
    gStripScaleEnable = gStripStepWidth = gStripDitherEnabled = gBufferCount = -6;
    gReleaseDoneFlag = -4;
    gMovieSetScaleMode(0x100);
    FCK_CHECK(gStripScaleEnable == 2 && gStripStepWidth == 2 && !gStripDitherEnabled && gBufferCount == 2,
              "zero low byte selects undithered two-step");
    gBufferCount = 3;
    gMovieSetScaleMode(0x180);
    FCK_CHECK(gStripScaleEnable == 1 && gStripStepWidth == 3 && gStripDitherEnabled == 1 && gBufferCount == 2,
              "nonzero low byte selects dithered three-step");
    FCK_EQ(gReleaseDoneFlag, -4, "mode doesn't reset release");
}

struct MovieAudio {
    u32 start, end;
    u8 ring_and_pad[0x7b2c - 8];
    u32 busy[4];
    u8 pad7b3c[0x38];
    u32 mode, blocked;
};
struct MovieAudio *gMovieAudioState __asm__("D_8009F4D8");
static struct MovieAudio audio;
void gMovieSetAudioMode(u32 value) __asm__("func_80086F08");
s32 gMovieAudioBusyCount(void) __asm__("func_80086FA8");
void *gPeekMovieAudioFirstBuffer(u32 index) __asm__("func_800869D8");

static void test_movie_audio_state(void)
{
    int i;

    gMovieAudioState = &audio;
    lk_fill(&audio, 0xa5, sizeof(audio));

    /* @leaf 80086F08 source mode write preserves the whole unsigned word and neighboring state */
    LK_FUNC("func_80086F08");
    gMovieSetAudioMode(0x89abcdef);
    FCK_CHECK(audio.mode == 0x89abcdef, "all mode bits preserved");
    FCK_CHECK(lk_all(&audio, 0, 0x7b74, 0xa5) && audio.blocked == 0xa5a5a5a5, "only mode word changes");
    gMovieSetAudioMode(0);
    FCK_EQ(audio.mode, 0, "returns to mode zero");

    /* @leaf 80086FA8 source counts nonzero busy words, excludes adjacent words before and after four slots */
    LK_FUNC("func_80086FA8");
    for (i = 0; i < 4; i++) audio.busy[i] = 0;
    FCK_EQ(gMovieAudioBusyCount(), 0, "no busy slots");
    audio.busy[0] = 0x80000000;
    audio.busy[3] = 0xffffffff;
    FCK_EQ(gMovieAudioBusyCount(), 2, "first and last nonzero slots both counted once");
    audio.busy[1] = 5;
    audio.busy[2] = 9;
    FCK_EQ(gMovieAudioBusyCount(), 4, "all four slots busy");

    /* @leaf 800869D8 source peek gates on queue length and blocked state and returns wrapped head, not indexed pointer */
    LK_FUNC("func_800869D8");
    audio.start = 15360 + 7;
    audio.end = audio.start + 3;
    audio.blocked = 0;
    FCK_CHECK(gPeekMovieAudioFirstBuffer(0) == (u8 *)&audio + 23, "read head wraps into first buffer");
    FCK_CHECK(gPeekMovieAudioFirstBuffer(2) == (u8 *)&audio + 23, "last valid index still returns head");
    FCK_CHECK(gPeekMovieAudioFirstBuffer(3) == 0, "one past queue length rejected");
    audio.blocked = 8;
    FCK_CHECK(gPeekMovieAudioFirstBuffer(0) == 0, "blocked state wins over nonempty queue");
    audio.blocked = 0;
    audio.end = audio.start;
    FCK_CHECK(gPeekMovieAudioFirstBuffer(0) == 0, "empty queue rejected");
}

u8 *gByteTable __asm__("D_8009F9D8");
u8 *gCharTable __asm__("D_8009FABC");
void gConvertMovieStripToRgb555(u32 *src, u32 *dst, s16 *rect) __asm__("func_800847E0");

static void test_movie_rgb555(void)
{
    u8 quantize[264], thresholds[256];
    u32 input[17][12], output[17 * 8 + 2];
    static const u8 group[12] = { 0, 255, 7, 20, 40, 80, 248, 12, 32, 4, 16, 193 };
    /* Four pixels with threshold 8 (no rounding) and threshold 4, then
     * packed in little-endian pairs. For example, 0/255/7 -> 0/31/0. */
    static const u32 no_round[2] = { 0x28a203e0, 0x6040103f };
    static const u32 round[2] = { 0x28a307e0, 0x6041105f };
    s16 rect[4] = { 7, 4, 16, 0 };
    int i, j;

    /* @leaf 800847E0 source RGB555 channel packing, threshold equality, last lookup index and 16-row dither wrap */
    LK_FUNC("func_800847E0");
    for (i = 0; i < 264; i++) quantize[i] = (i < 248 ? i / 8 : 31);
    lk_fill(thresholds, 8, sizeof(thresholds));
    lk_fill(thresholds + 16, 4, 16);
    gByteTable = quantize;
    gCharTable = thresholds;
    for (i = 0; i < 17; i++)
        for (j = 0; j < 4; j++) memcpy((u8 *)input[i] + j * sizeof(group), group, sizeof(group));
    lk_fill(output, 0xa5, sizeof(output));
    gConvertMovieStripToRgb555(input[0], output + 1, rect);
    FCK_CHECK(lk_all(output, 0, sizeof(output), 0xa5), "zero height doesn't write");
    rect[3] = 17;
    gConvertMovieStripToRgb555(input[0], output + 1, rect);
    for (i = 0; i < 17; i++) {
        const u32 *pair = i == 1 ? round : no_round;
        for (j = 0; j < 8; j += 2) {
            FCK_EQ(output[1 + i * 8 + j], pair[0], "first packed pixel pair");
            FCK_EQ(output[2 + i * 8 + j], pair[1], "second packed pixel pair");
        }
    }
    FCK_CHECK(output[0] == 0xa5a5a5a5 && output[137] == 0xa5a5a5a5, "writes exactly eight words per row");
}

s16 gAtanTable[1025] __asm__("D_8009E0A0");
s32 gAtan2(s32 x, s32 y) __asm__("func_80095C30");
void gZeroIntArray(int *destination, unsigned int count) __asm__("func_80091F38");

static void test_bearings_and_word_clear(void)
{
    int words[] = { 90, 5, -65, 0x7fffffff, -9 };
    int i;

    /* @leaf 80095C30 source synthetic angle taps prove octant/quadrant folding and the large-ratio indexing path */
    LK_FUNC("func_80095C30");
    for (i = 0; i < 1025; i++) gAtanTable[i] = -100;
    gAtanTable[0] = 0;
    gAtanTable[256] = 161;
    gAtanTable[512] = 302;
    gAtanTable[1024] = 512;
    FCK_EQ(gAtan2(0, 0), 0, "origin defined as zero");
    FCK_EQ(gAtan2(1, 0), 1024, "positive X is quarter turn");
    FCK_EQ(gAtan2(0, -3), 2048, "negative Y is half turn");
    FCK_EQ(gAtan2(7, 7), 512, "diagonal looks up endpoint");
    FCK_EQ(gAtan2(2, 8), 161, "one-quarter ratio reads tap 256");
    FCK_EQ(gAtan2(8, 2), 863, "swapped axes complement to quarter turn");
    FCK_EQ(gAtan2(-2, 8), -161, "negative X flips sign");
    FCK_EQ(gAtan2(2, -8), 1887, "negative Y reflects across horizontal axis");
    FCK_EQ(gAtan2(-8, -2), -1185, "both signs compose with swapped axes");
    FCK_EQ(gAtan2(0x200000, 0x400000), 302, "large numerator avoids left-shift overflow");
    FCK_EQ(gAtan2(0x400000, 0x200000), 722, "large numerator in opposite octant");

    /* @leaf 80091F38 semantic zero count is a no-op; nonzero count clears only requested words */
    LK_FUNC("func_80091F38");
    gZeroIntArray(words + 1, 0);
    FCK_CHECK(words[1] == 5 && words[2] == -65 && words[3] == 0x7fffffff, "zero count leaves words alone");
    gZeroIntArray(words + 1, 3);
    FCK_CHECK(!words[1] && !words[2] && !words[3], "three complete words cleared");
    FCK_CHECK(words[0] == 90 && words[4] == -9, "both guards intact");
}

s32 gSpuAddrShift __asm__("D_8009EECC");
s32 gSpuEntryTableCapacity __asm__("D_8009EF04");
s32 gTableCursor __asm__("D_8009EF08");
u8 *gSpuEntryTable __asm__("D_8009EF0C");
s32 gSpuInitPool(s32 count, u8 *table) __asm__("func_80097AC8");

static void test_spu_pool(void)
{
    u32 old[2] = { 8, 9 };
    u32 table[5] = { 17, 29, 31, 45, 61 };

    /* @leaf 80097AC8 source rejects zero/negative capacity, then describes free extent and publishes allocator table */
    LK_FUNC("func_80097AC8");
    gSpuEntryTable = (u8 *)old;
    gSpuAddrShift = 3;
    gSpuEntryTableCapacity = 7;
    gTableCursor = 3;
    FCK_EQ(gSpuInitPool(0, (u8 *)table), 0, "reject zero capacity");
    FCK_EQ(gSpuInitPool(-1, (u8 *)table), 0, "reject negative capacity");
    FCK_CHECK(gSpuEntryTable == (u8 *)old && gSpuEntryTableCapacity == 7 && gTableCursor == 3,
              "rejected calls preserve allocator state");
    FCK_CHECK(table[0] == 17 && table[1] == 29, "rejected calls leave proposed table alone");
    FCK_EQ(gSpuInitPool(2, (u8 *)table), 2, "successful call returns capacity");
    FCK_CHECK(gSpuEntryTable == (u8 *)table && gSpuEntryTableCapacity == 2 && !gTableCursor, "new pool is active");
    FCK_EQ(table[0], 0x40001010, "first free descriptor base and tag");
    FCK_EQ(table[1], 524288 - 4112, "512 KiB minus reserved prefix");
    FCK_CHECK(table[2] == 31 && table[3] == 45 && table[4] == 61, "other entries and guard unmodified");
    gSpuAddrShift = 0;
    FCK_EQ(gSpuInitPool(1, (u8 *)table), 1, "single slot is accepted");
    FCK_EQ(table[1], 65536 - 4112, "shift zero uses 64 KiB extent");
}

int main(void)
{
    test_model_chunks();
    test_numeric_state();
    test_render_and_file_flags();
    test_card_operations();
    test_title_glyphs();
    test_audio_initialization();
    test_tracked_audio();
    test_vagm_dequeue();
    test_game_state();
    test_spawn_tables();
    test_player_targets();
    test_animation_event_bits();
    test_sound_priority();
    test_lists();
    test_marker_pulse();
    test_motion_anchor_and_flags();
    test_menu_progress();
    test_menu_navigation();
    test_movie_slice_state();
    test_movie_audio_state();
    test_movie_rgb555();
    test_bearings_and_word_clear();
    test_spu_pool();
    return lk_done("leaf_hundred_a");
}
