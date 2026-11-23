/* Source characterizations of audio, menu and pool routines. All inputs
 * are synthetic; the SPU start, BIOS strlen and allocation boundaries
 * use the local fixtures below. Real helper units claim no coverage. */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

#include "../../src/game/8001F180.c"
#include "../../src/game/80021800.c"
#include "../../src/game/800223B0.c"
#define gSoundSystem voice_events
#include "../../src/game/80021A70.c"
#undef gSoundSystem
#include "../../src/game/8007E8D8.c"

static u32 mc_state[0x1BB8 / 4];
struct Unk8001F180 *gOperationState = (struct Unk8001F180 *)mc_state;
void gQueueMcOpMode6(u32 arg0, u32 arg1, u32 arg2, u32 arg3) __asm__("func_8001F2E0");

static void test_memory_card_queue(void)
{
    /* @leaf 8001F2E0 source src/game/8001F2E0.c: idle manager queues mode 6; busy manager only sets the rejection word */
    LK_FUNC("func_8001F2E0");
    lk_fill(mc_state, 0xA5, sizeof(mc_state));
    mc_state[0x8F4 / 4] = 0;
    mc_state[0x8F8 / 4] = 0;
    gQueueMcOpMode6(0x123F, 27, 38, 49);
    FCK_EQ(mc_state[0x8F8 / 4], 6, "queued operation mode");
    FCK_EQ(mc_state[0x998 / 4], 0x123F, "operation source");
    FCK_EQ(mc_state[0x99C / 4], 27, "operation second argument");
    FCK_EQ(mc_state[0x910 / 4], 38, "operation third argument");
    FCK_EQ(mc_state[0x90C / 4], 49, "operation fourth argument");
    FCK_EQ(mc_state[0x1BB4 / 4], 0x48, "source converted to 64-byte units");
    FCK_EQ(mc_state[0x994 / 4], 0, "operation cursor reset");
    FCK_EQ(mc_state[0x9A4 / 4], 0, "rejection word reset");

    gQueueMcOpMode6(0, 0, 0, 0);
    FCK_EQ(mc_state[0x9A4 / 4], 1, "busy operation rejected");
    FCK_EQ(mc_state[0x8F8 / 4], 6, "busy operation remains queued");
    FCK_EQ(mc_state[0x998 / 4], 0x123F, "busy operation source unchanged");
    FCK_EQ(mc_state[0x1BB4 / 4], 0x48, "busy operation block index unchanged");
    FCK_CHECK(lk_all(mc_state, 0x9A0, 0x9A4, 0xA5), "adjacent word untouched");
}

typedef struct Request {
    int a, b, c;
    void (*callback)(int);
    int context, mode;
} Request;

Request gPendingEntryList[10] __asm__("D_800A8EA8");
int gSpuTransferWriteIndex __asm__("D_8009F998");
int gSpuTransferDispatching __asm__("D_8009FAD0");
int gSpuTransferActive __asm__("D_8009FA34");
int gSlotIndexLimit __asm__("D_8009FA80");

void gEnqueueSpuTransfer(Request *req) __asm__("func_800212F8");
void gCompleteSpuTransfer(void) __asm__("func_8002139C");

/* DMA boundary: record the address and mark the transfer active, without
 * touching the hardware. The completion callback attempts real re-entry. */
struct Unk21490 {
    s32 channel;
    s32 arg1;
    s32 arg0;
    u8 unk0C[8];
    s32 mode;
};
static const void *started_request;
static int start_count, callback_count, callback_context, callback_dispatching;

void gStartSpuTransfer(struct Unk21490 *request) __asm__("func_80021490");
void gStartSpuTransfer(struct Unk21490 *request)
{
    started_request = request;
    start_count++;
    gSpuTransferActive = 1;
}

static void record_completion(int context)
{
    callback_count++;
    callback_context = context;
    callback_dispatching = gSpuTransferDispatching;
    if (callback_count == 1)
        gCompleteSpuTransfer();
}

static void test_spu_queue(void)
{
    Request request = { 17, 29, 43, record_completion, -73, 1 };

    /* @leaf 800212F8 source src/game/800212F8.c: copies the full request, starts when empty, wraps after slot eight */
    LK_FUNC("func_800212F8");
    lk_fill(gPendingEntryList, 0xA5, sizeof(gPendingEntryList));
    gSpuTransferWriteIndex = gSlotIndexLimit = 8;
    gSpuTransferActive = start_count = 0;
    gEnqueueSpuTransfer(&request);
    FCK_EQ(gPendingEntryList[8].a, 17, "first request word copied");
    FCK_EQ(gPendingEntryList[8].b, 29, "second request word copied");
    FCK_EQ(gPendingEntryList[8].c, 43, "third request word copied");
    FCK_CHECK(gPendingEntryList[8].callback == record_completion, "callback copied");
    FCK_EQ(gPendingEntryList[8].context, -73, "callback context copied");
    FCK_EQ(gPendingEntryList[8].mode, 1, "transfer mode copied");
    FCK_EQ(gSpuTransferWriteIndex, 0, "write index wraps at nine");
    FCK_CHECK(started_request == &request, "empty queue starts the caller's request");
    FCK_EQ(start_count, 1, "first request starts once");
    FCK_CHECK(lk_all(&gPendingEntryList[9], 0, sizeof(Request), 0xA5), "slot after ring untouched");

    request.callback = 0;
    request.context = 99;
    gEnqueueSpuTransfer(&request);
    FCK_EQ(start_count, 1, "occupied queue does not start another request");
    FCK_EQ(gSpuTransferWriteIndex, 1, "ordinary write advances one slot");
    FCK_EQ(gPendingEntryList[0].context, 99, "wrapped write uses slot zero");
    FCK_EQ(gPendingEntryList[8].context, -73, "earlier request is not aliased");

    /* @leaf 8002139C source src/game/8002139C.c: dispatch guard, callback, wrapped read and final idle transition */
    LK_FUNC("func_8002139C");
    gPendingEntryList[8] = request;
    gPendingEntryList[8].callback = record_completion;
    gPendingEntryList[8].context = -73;
    gPendingEntryList[0] = request;
    gSpuTransferWriteIndex = 1;
    gSlotIndexLimit = 8;
    gSpuTransferActive = 1;
    start_count = 1;
    callback_count = callback_context = callback_dispatching = 0;
    gSpuTransferDispatching = 0;
    gCompleteSpuTransfer();
    FCK_EQ(callback_count, 1, "first completion calls callback exactly once");
    FCK_EQ(callback_context, -73, "completion uses saved context");
    FCK_EQ(callback_dispatching, 1, "dispatch guard set while callback runs");
    FCK_EQ(gSpuTransferDispatching, 0, "dispatch guard released");
    FCK_EQ(gSlotIndexLimit, 0, "read index wraps at nine");
    FCK_CHECK(started_request == &gPendingEntryList[0], "completion starts next queued slot");
    FCK_EQ(start_count, 2, "only the successor starts");
    FCK_EQ(gSpuTransferActive, 1, "queue still active with successor pending");

    gCompleteSpuTransfer();
    FCK_EQ(gSlotIndexLimit, 1, "last completion advances to writer");
    FCK_EQ(gSpuTransferActive, 0, "last completion clears active state");
    FCK_EQ(callback_count, 1, "null callback does not dispatch");
    gCompleteSpuTransfer();
    FCK_EQ(gSlotIndexLimit, 1, "empty completion leaves reader unchanged");
    FCK_EQ(start_count, 2, "empty completion does not restart DMA");
}

struct Args { s32 unk00, unk04, unk08; };
struct Sample { u8 index; s8 bank; };
struct Voice { u8 pad[0xD]; s8 slot; u8 pad2[6]; struct Sample *sample; };
struct Bank { u8 pad[0x13]; u8 cursor; };
struct SoundFixture {
    s32 max_bank;
    s32 *banks;
    s32 reserved;
    u8 *channels;
    u8 pad10[4];
    u32 pending;
    u8 pad18[0x58];
    s32 scale;
};

struct SoundFixture sound_fixture __asm__("D_800A07A8");

s32 gQueueBankSampleOnChannel(s32 a0, s32 a1, s32 a2, s32 a3, s32 slot, struct Args *args)
    __asm__("func_80021F98");
s32 gQueueVoiceBankSample(struct Bank *bank, struct Voice *voice, s32 pan, s32 volume, s32 extra)
    __asm__("func_80024358");

static void test_sound_bank(void)
{
    s32 banks[2];
    u32 blob[24], channels[3][7];
    u8 *sample;
    struct Args args = { 0x3456, 32, 0x40 };
    struct Bank bank;
    struct Voice voice;
    struct Sample selected;

    lk_fill(blob, 0, sizeof(blob));
    lk_fill(channels, 0xA5, sizeof(channels));
    banks[0] = 0;
    banks[1] = (s32)blob;
    blob[5] = 0x20; /* sample index 1; index 0 intentionally absent */
    sample = (u8 *)blob + 0x24;
    sample[1] = 64;
    sample[2] = 32;
    sample[0xE] = 0x78;
    sample[0xF] = 0x56;
    sound_fixture.max_bank = 1;
    sound_fixture.banks = banks;
    sound_fixture.channels = (u8 *)channels;
    sound_fixture.scale = 2;
    sound_fixture.pending = 1;

    /* @leaf 80021F98 source src/game/80021F98.c: bank and sample validation, variant offset, channel record and pending key-on */
    LK_FUNC("func_80021F98");
    FCK_EQ(gQueueBankSampleOnChannel(1, 0, 0, 0, 2, &args), 0, "missing sample rejected");
    FCK_EQ(gQueueBankSampleOnChannel(0, 1, 0, 0, 2, &args), 0, "missing bank rejected");
    FCK_EQ(gQueueBankSampleOnChannel(2, 1, 0, 0, 2, &args), 0, "bank above maximum rejected");
    FCK_EQ(gQueueBankSampleOnChannel(-1, 1, 0, 0, 2, &args), 0, "negative bank rejected");
    FCK_EQ(sound_fixture.pending, 1, "rejection leaves pending voices intact");
    FCK_CHECK(lk_all(channels, 0, sizeof(channels), 0xA5), "rejection leaves channels intact");

    sample[21] = 128;
    sample[22] = 64;
    FCK_EQ(gQueueBankSampleOnChannel(1, 1, 1, 0, 2, &args), 1, "maximum bank accepts valid variant");
    FCK_EQ(channels[2][0], sample + 20, "variant steps over one 20-byte record");
    FCK_EQ(channels[2][4], 0x3456, "explicit pitch overrides sample default");
    FCK_EQ(channels[2][5], 64, "left gain follows variant and mixer scale");
    FCK_EQ(channels[2][6], 32, "right gain follows variant and mixer scale");
    FCK_EQ(sound_fixture.pending, 5, "key-on flags OR with prior pending voices");
    FCK_CHECK(lk_all(channels[2], 4, 16, 0xA5), "unrelated channel words untouched");

    /* @leaf 80024358 source src/game/80024358.c: unavailable bank and failed lookup return zero; success uses default pitch and supplied mix */
    LK_FUNC("func_80024358");
    lk_fill(&voice, 0, sizeof(voice));
    lk_fill(&bank, 0, sizeof(bank));
    voice.sample = &selected;
    voice.slot = 1;
    bank.cursor = 7;
    selected.index = 1;
    selected.bank = -1;
    FCK_EQ(gQueueVoiceBankSample(&bank, &voice, 0x20, 32, 99), 0, "unavailable voice bank rejected");
    selected.bank = 0;
    FCK_EQ(gQueueVoiceBankSample(&bank, &voice, 0x20, 32, 99), 0, "lookup failure propagated");
    FCK_CHECK(lk_all(channels[1], 0, sizeof(channels[1]), 0xA5), "failed voice does not alter channel");
    FCK_EQ(sound_fixture.pending, 5, "failed voice does not alter key-on bits");
    selected.bank = 1;
    FCK_EQ(gQueueVoiceBankSample(&bank, &voice, 0x20, 32, 99), 1, "valid voice queued");
    FCK_EQ(channels[1][0], sample, "voice uses base variant");
    FCK_EQ(channels[1][4], 0x5678, "voice takes sample pitch");
    FCK_EQ(channels[1][5], 32, "voice volume scales left gain");
    FCK_EQ(channels[1][6], 8, "voice pan attenuates right gain");
    FCK_EQ(sound_fixture.pending, 7, "voice adds its own slot bit");
    FCK_CHECK(lk_all(channels[0], 0, sizeof(channels[0]), 0xA5), "other voice slot unchanged");
}

typedef struct { s16 unk0, unk2, unk4, unk6; } Unk800A3578;
Unk800A3578 gSlotRecordTable[3] __asm__("D_800A3578");
void gQueueStatusLineText(s32 arg0, s32 arg1, s32 arg2) __asm__("func_80078B34");

void gEncodeNameToGlyphs(u8 *src, u8 *dst, s32 len, u8 *table, s32 tableLen)
    __asm__("func_8007A9D0");

/* BIOS strlen comes from the shared mock library (mocks/bios_mock.o linked
 * by the suite script); see mocks/bios_mock.h for the whole thunk contract. */
#include "mocks/bios_mock.h"

static void test_text(void)
{
    u8 name[] = { 'X', '?', 0, 'X' };
    u8 table[] = { '#', 'X', ' ', 'X' };
    u8 glyphs[6];

    /* @leaf 80078B34 source src/game/80078B34.c: status slot gate and three alignment write paths */
    LK_FUNC("func_80078B34");
    lk_fill(gSlotRecordTable, 0xA5, sizeof(gSlotRecordTable));
    gQueueStatusLineText(0x4321, 2, 2);
    FCK_CHECK(lk_all(gSlotRecordTable, 0, sizeof(gSlotRecordTable), 0xA5), "slot two rejected");
    gQueueStatusLineText(0x12345, 1, 0);
    FCK_EQ(gSlotRecordTable[1].unk0, 0x2345, "status ID stored as halfword");
    FCK_EQ(gSlotRecordTable[1].unk2, 0xA0, "default x position");
    gQueueStatusLineText(21, 0, 1);
    FCK_EQ(gSlotRecordTable[0].unk0, 21, "left status ID");
    FCK_EQ(gSlotRecordTable[0].unk2, 0x55, "left x position");
    gQueueStatusLineText(31, 0, 2);
    FCK_EQ(gSlotRecordTable[0].unk4, 31, "right status ID");
    FCK_EQ(gSlotRecordTable[0].unk6, 0xEB, "right x position");
    FCK_EQ(gSlotRecordTable[0].unk0, 21, "right update leaves left status intact");
    FCK_CHECK(lk_all(&gSlotRecordTable[1], 4, 8, 0xA5), "default update leaves right pair intact");

    /* @leaf 8007A9D0 source src/game/8007A9D0.c: first matching glyph, unknown fallback, space padding and bounded output */
    LK_FUNC("func_8007A9D0");
    lk_fill(glyphs, 0xA5, sizeof(glyphs));
    gEncodeNameToGlyphs(name, glyphs + 1, 4, table, 4);
    FCK_EQ(glyphs[1], 1, "first duplicate wins");
    FCK_EQ(glyphs[2], 0, "unmapped character falls back to zero");
    FCK_EQ(glyphs[3], 2, "NUL is replaced by space glyph");
    FCK_EQ(glyphs[4], 2, "bytes after NUL are not consumed");
    FCK_EQ(glyphs[0], 0xA5, "leading guard untouched");
    FCK_EQ(glyphs[5], 0xA5, "output ends at requested length");
    gEncodeNameToGlyphs(name, glyphs, 1, table, 0);
    FCK_EQ(glyphs[0], 0, "empty glyph table falls back to zero");
    FCK_EQ(glyphs[1], 1, "single-glyph output does not touch successor");
    gEncodeNameToGlyphs(name, glyphs + 5, 0, table, 4);
    FCK_EQ(glyphs[5], 0xA5, "zero length leaves destination unchanged");
}

s8 *gFontBankTable __asm__("D_8009F468");
s32 gActivePlayerIndex __asm__("D_8009F8B8");
s8 gPlayerMenuDepth[2] __asm__("D_8009F8C0");
s32 gMenuReadyToAdvancePredicate(void) __asm__("func_8007B0D4");

struct Entry {
    unsigned int unknown;
    u8 *values;
    u8 unused, replacement, limit;
    u8 pad[9];
};
struct Entry gLoadoutTable[3] __asm__("D_8009C0A8");
void gClampLoadoutSelections(int mode) __asm__("func_8007D2DC");

static void test_menu_loadout(void)
{
    s8 menu[6] = { 2, 5, 0, 0, 0, 2 };
    u8 values[3][3] = { { 4, 9, 0xA5 }, { 9, 4, 0xA5 }, { 9, 9, 0xA5 } };
    int i;

    /* @leaf 8007B0D4 source src/game/8007B0D4.c: active depth and mode-specific menu code gate return two */
    LK_FUNC("func_8007B0D4");
    gFontBankTable = menu;
    gActivePlayerIndex = 0;
    gPlayerMenuDepth[0] = 1;
    gPlayerMenuDepth[1] = 0;
    FCK_EQ(gMenuReadyToAdvancePredicate(), 2, "mode two recognizes player zero code two");
    gActivePlayerIndex = 1;
    FCK_EQ(gMenuReadyToAdvancePredicate(), 0, "closed player one menu does not advance");
    gPlayerMenuDepth[1] = 3;
    FCK_EQ(gMenuReadyToAdvancePredicate(), 2, "mode two recognizes player one code five");
    menu[5] = 3;
    FCK_EQ(gMenuReadyToAdvancePredicate(), 0, "ordinary mode rejects code five");
    menu[1] = 1;
    FCK_EQ(gMenuReadyToAdvancePredicate(), 2, "ordinary mode accepts code one");
    FCK_EQ(gActivePlayerIndex, 1, "query keeps active index");
    FCK_EQ(gPlayerMenuDepth[1], 3, "query keeps menu depth");
    FCK_EQ(menu[1], 1, "query keeps player menu code");
    FCK_EQ(menu[5], 3, "query keeps menu mode");

    /* @leaf 8007D2DC source src/game/8007D2DC.c: per-entry caps and mode-zero exemption for each player's current row */
    LK_FUNC("func_8007D2DC");
    menu[0] = 2;
    menu[1] = 5;
    for (i = 0; i < 3; i++) {
        gLoadoutTable[i].values = values[i];
        gLoadoutTable[i].limit = 4;
        gLoadoutTable[i].replacement = i + 1;
    }
    gClampLoadoutSelections(0);
    FCK_EQ(values[0][0], 4, "player zero at limit stays at limit");
    FCK_EQ(values[1][1], 4, "player one at limit stays at limit");
    FCK_EQ(values[1][0], 9, "player zero selected row exempt");
    FCK_EQ(values[0][1], 9, "player one selected row exempt");
    FCK_EQ(values[2][0], 3, "third row player zero uses its replacement");
    FCK_EQ(values[2][1], 3, "third row player one uses its replacement");
    gClampLoadoutSelections(1);
    FCK_EQ(values[1][0], 2, "nonzero mode clamps player zero selected row");
    FCK_EQ(values[0][1], 1, "nonzero mode clamps player one selected row");
    for (i = 0; i < 3; i++)
        FCK_EQ(values[i][2], 0xA5, "only the two supported player bytes can change");
}

typedef struct { u8 unk00[8]; s32 value; } D_8009BEF4_Entry;
D_8009BEF4_Entry gControllerPresetSlotTable[14] __asm__("D_8009BEF4");
u8 gPresetByteTable[56] __asm__("D_8009BFD4");
static u8 preset_indices[3];
__asm__(".globl D_8009FA99\nD_8009FA99 = preset_indices + 1");
u32 pad_mapping[29] __asm__("D_800AAC50");
void gApplyControllerPreset(s32 port, s32 value) __asm__("func_8007BFCC");

static void test_controller_preset(void)
{
    int i;
    for (i = 0; i < 14; i++) {
        gControllerPresetSlotTable[i].value = 100 + i;
        gPresetByteTable[i] = 13 - i;
        gPresetByteTable[42 + i] = i;
    }

    /* @leaf 8007BFCC source src/game/8007BFCC.c: clamps both selectors and scatters all 14 mapping words into one port */
    LK_FUNC("func_8007BFCC");
    lk_fill(pad_mapping, 0xA5, sizeof(pad_mapping));
    lk_fill(preset_indices, 0xA5, sizeof(preset_indices));
    gApplyControllerPreset(-1, -1);
    FCK_EQ(preset_indices[0], 0, "negative inputs map to port zero preset zero");
    FCK_EQ(preset_indices[1], 0xA5, "other port index untouched");
    FCK_CHECK(lk_all(pad_mapping, 56, sizeof(pad_mapping), 0xA5), "other port mapping untouched");
    for (i = 0; i < 14; i++)
        FCK_EQ(pad_mapping[i], 113 - i, "port zero uses reversed synthetic preset");

    gApplyControllerPreset(2, 4);
    FCK_EQ(preset_indices[1], 3, "high inputs map to port one preset three");
    for (i = 0; i < 14; i++) {
        FCK_EQ(pad_mapping[14 + i], 100 + i, "port one uses linear synthetic preset");
        FCK_EQ(pad_mapping[i], 113 - i, "port zero mapping preserved");
    }
    FCK_EQ(preset_indices[0], 0, "port zero index preserved");
    FCK_EQ(preset_indices[2], 0xA5, "index guard untouched");
    FCK_EQ(pad_mapping[28], 0xA5A5A5A5, "mapping guard untouched");
}

struct ProgressEntry { u8 prefix[0x1F], value, pad[4]; };
struct UnkFA94 { u8 pad00[0x8C], unk8C, pad8D[3], unk90, pad91[3], unk94; };

struct ProgressEntry progress_entries[5] __asm__("D_8009B4E4");
s8 gFrontEndListTable[5] __asm__("D_8009B72C");
struct UnkFA94 *gGameSession __asm__("D_8009FA94");
u8 gFrontEndListFlag __asm__("D_8009F3C0");
u8 gPlayerWrapLimit __asm__("D_8009F3C1");
u8 gPausedMenuFlag __asm__("D_8009F3C3");
u8 gLockedMenuFlag __asm__("D_8009F3C4");
u8 gCrimeWarProgress __asm__("D_8009F458");
u8 gPackedPasswordBytes[12] __asm__("D_800A3938");
void gPackCurrentProgressPassword(void) __asm__("func_8007F02C");

typedef struct { u8 pad_00[0x7B20]; volatile u32 value; } D_8009F4D8_Data;
static D_8009F4D8_Data movie_clock;
D_8009F4D8_Data * const gMovieAudioState __asm__("D_8009F4D8") = &movie_clock;

void gMovieAudioClockEnable(void) __asm__("func_80086F1C");

static void test_progress_and_movie(void)
{
    static const u8 expected[10] = { 3, 13, 11, 10, 3, 5, 5, 19, 5, 6 };
    static const u8 all_ten[10] = { 9, 13, 13, 10, 17, 5, 10, 19, 11, 2 };
    struct UnkFA94 session;
    int i;

    /* @leaf 8007F02C source src/game/8007F02C.c: routes session flags and indexed progress into the real 8007E8D8 packer */
    LK_FUNC("func_8007F02C");
    lk_fill(&session, 0xA5, sizeof(session));
    session.unk8C = 3;
    session.unk90 = 1;
    session.unk94 = 4;
    gGameSession = &session;
    gFrontEndListFlag = 2;
    gCrimeWarProgress = 9;
    gPlayerWrapLimit = 1;
    gPausedMenuFlag = 1;
    gLockedMenuFlag = 0;
    gFrontEndListTable[0] = 4;
    gFrontEndListTable[1] = 0;
    gFrontEndListTable[2] = 3;
    gFrontEndListTable[3] = 1;
    gFrontEndListTable[4] = 2;
    progress_entries[4].value = 6;
    progress_entries[0].value = 3;
    progress_entries[3].value = 5;
    progress_entries[1].value = 8;
    progress_entries[2].value = 12;
    lk_fill(gPackedPasswordBytes, 0xA5, sizeof(gPackedPasswordBytes));
    gPackCurrentProgressPassword();
    for (i = 0; i < 10; i++)
        FCK_EQ(gPackedPasswordBytes[i], expected[i], "ordinary packed progress byte");
    FCK_CHECK(lk_all(gPackedPasswordBytes, 10, 12, 0xA5), "packing is limited to ten bytes");
    FCK_CHECK(lk_all(&session, 0, 0x8C, 0xA5), "session prefix is not modified");

    for (i = 0; i < 5; i++)
        if (i != 2)
            progress_entries[i].value = 10;
    gPackCurrentProgressPassword();
    for (i = 0; i < 10; i++)
        FCK_EQ(gPackedPasswordBytes[i], all_ten[i], "all-ten progress folds the fifth value");
    FCK_EQ(progress_entries[2].value, 12, "packer does not change progress table");

    /* @leaf 80086F1C source src/game/80086F1C.c: enable replaces the movie clock word with one and preserves preceding state */
    LK_FUNC("func_80086F1C");
    lk_fill(&movie_clock, 0xA5, sizeof(movie_clock));
    gMovieAudioClockEnable();
    FCK_EQ(movie_clock.value, 1, "enable stores one over nonzero prior value");
    FCK_CHECK(lk_all(&movie_clock, 0, 0x7B20, 0xA5), "only clock enable word changes");
    gMovieAudioClockEnable();
    FCK_EQ(movie_clock.value, 1, "repeated enable remains one");
}

struct FixedPool { s32 stride; u8 *end; s32 count, available; void *free; };
struct Pool { void *storage, *active, *free; };

struct FixedPool *gCreateFixedPool(s32 count, s32 size, s32 tag) __asm__("func_800194DC");
s32 gInitIntrusiveNodePool(struct Pool *pool, s32 size, s32 count) __asm__("func_8003BE2C");
void gAllocLightGrid(s32 width, s32 height) __asm__("func_800496D0");
void **gLightGrid __asm__("D_8009F780");
s32 gLightGridWidth __asm__("D_8009F78C");
s32 gLightGridHeight __asm__("D_8009F790");

/* Single aligned block with a caller-selected capacity. Invalid/oversized
 * requests fail; record byte count and tag so allocator arguments are tested. */
static u32 arena[24];
static s32 arena_capacity, requested_size, requested_tag;

void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
void *gAllocMemory(s32 size, s32 tag)
{
    requested_size = size;
    requested_tag = tag;
    if (size <= 0 || size > arena_capacity)
        return 0;
    return arena;
}

static void test_pool_layouts(void)
{
    struct FixedPool *fixed;
    struct Pool pool;
    int i;

    /* @leaf 800194DC source src/game/800194DC.c: rounded stride, backwards free list and null allocation result */
    LK_FUNC("func_800194DC");
    lk_fill(arena, 0xA5, sizeof(arena));
    arena_capacity = 43;
    FCK_EQ(gCreateFixedPool(3, 5, 2), 0, "insufficient space returns null");
    FCK_EQ(requested_size, 44, "allocation includes 20-byte header and rounded entries");
    FCK_EQ(requested_tag, 2, "allocation tag forwarded");
    FCK_CHECK(lk_all(arena, 0, sizeof(arena), 0xA5), "failed allocation writes nothing");

    arena_capacity = 44;
    fixed = gCreateFixedPool(3, 5, 2);
    FCK_EQ(fixed, arena, "success returns initialized pool");
    if (fixed) {
        FCK_EQ(fixed->stride, 8, "stride rounded up to word boundary");
        FCK_EQ(fixed->end, &arena[11], "end points just beyond last entry");
        FCK_EQ(fixed->count, 3, "pool stores count");
        FCK_EQ(fixed->available, 3, "all entries free");
        FCK_EQ(fixed->free, &arena[9], "free head is last entry");
    }
    FCK_EQ(arena[9], &arena[7], "last entry links to middle");
    FCK_EQ(arena[7], &arena[5], "middle links to first");
    FCK_EQ(arena[5], 0, "first entry terminates list");
    FCK_EQ(arena[10], 0xA5A5A5A5, "entry payload untouched");
    FCK_EQ(arena[11], 0xA5A5A5A5, "past-end guard untouched");

    /* @leaf 8003BE2C source src/game/8003BE2C.c: forward links at caller stride, list publication only after allocation */
    LK_FUNC("func_8003BE2C");
    lk_fill(arena, 0xA5, sizeof(arena));
    pool.active = &arena[1];
    pool.free = &arena[2];
    arena_capacity = 35;
    FCK_EQ(gInitIntrusiveNodePool(&pool, 12, 3), 0, "allocation failure returns zero");
    FCK_EQ(requested_size, 36, "requested bytes are count times stride");
    FCK_EQ(requested_tag, 1, "intrusive pool uses tag one");
    FCK_EQ(pool.storage, 0, "failed allocation clears storage");
    FCK_EQ(pool.active, &arena[1], "failed allocation does not clear active list");
    FCK_EQ(pool.free, &arena[2], "failed allocation does not clear free list");
    FCK_CHECK(lk_all(arena, 0, sizeof(arena), 0xA5), "failed pool initialization writes no nodes");

    arena_capacity = 36;
    FCK_EQ(gInitIntrusiveNodePool(&pool, 12, 3), 1, "allocation success returns one");
    FCK_EQ(pool.storage, arena, "allocated storage published");
    FCK_EQ(pool.free, arena, "first entry is free head");
    FCK_EQ(pool.active, 0, "active list starts empty");
    FCK_EQ(arena[0], &arena[3], "first entry links forwards");
    FCK_EQ(arena[3], &arena[6], "middle entry links forwards");
    FCK_EQ(arena[6], 0, "last entry terminates chain");
    FCK_EQ(arena[7], 0xA5A5A5A5, "node payload untouched");
    FCK_EQ(arena[9], 0xA5A5A5A5, "past-end guard untouched");

    /* @leaf 800496D0 source src/game/800496D0.c: dimensions published and exactly width times height pointers cleared */
    LK_FUNC("func_800496D0");
    lk_fill(arena, 0xA5, sizeof(arena));
    gAllocLightGrid(3, 2);
    FCK_EQ(requested_size, 24, "three by two grid uses six words");
    FCK_EQ(requested_tag, 2, "lighting grid uses tag two");
    FCK_EQ(gLightGridWidth, 3, "grid width published");
    FCK_EQ(gLightGridHeight, 2, "grid height published");
    FCK_EQ(gLightGrid, arena, "grid pointer published");
    for (i = 0; i < 6; i++)
        FCK_EQ(arena[i], 0, "grid cell zero initialized, including index zero");
    FCK_EQ(arena[6], 0xA5A5A5A5, "grid first out-of-range word untouched");
    gAllocLightGrid(0, 2);
    FCK_EQ(gLightGrid, 0, "zero-cell grid publishes null without writing cells");
    FCK_EQ(gLightGridWidth, 0, "zero width replaces previous width");
    FCK_EQ(gLightGridHeight, 2, "height preserved even with zero width");
    FCK_EQ(arena[6], 0xA5A5A5A5, "zero-cell grid leaves guard intact");
}

int main(void)
{
    test_memory_card_queue();
    test_spu_queue();
    test_sound_bank();
    test_text();
    test_menu_loadout();
    test_controller_preset();
    test_progress_and_movie();
    test_pool_layouts();
    return lk_done("leaf_fewmore_a");
}
