/* Source characterizations of unchanged src/game/<address>.c units.
 * Prototypes and fixture layouts come from those units' declarations.
 * All state is synthetic. Pad responses, audio descriptors and GPU packets
 * are RAM fixtures only; there is no device, BIOS, or retail data dependency.
 */
#include "leaf_kit.h"

/* CVK slot and sound-group tables (80023500.c, 80023528.c). */
typedef struct {
    s8 unk00, unk01;
    u8 ready, pad03;
    s32 unk04, unk08;
    u8 pad0C[4];
} CvkSlot;

typedef struct { u16 id, unk2, unk4; } Unk23528Entry;
typedef struct { u16 unk0, count; Unk23528Entry entries[1]; } Unk23528Header;
typedef struct { Unk23528Header header; Unk23528Entry tail[2]; } SoundGroup;

/* The original array is the first member at each symbol. Back the next
 * element as well, so an off-by-one mutation is observable in RAM. */
struct {
    CvkSlot slots[8];
    CvkSlot guard;
} cvk_slot_storage __asm__("D_800A0A00");
struct {
    Unk23528Header **slots[2];
    Unk23528Header **guard;
} cshd_slot_storage __asm__("D_8009F640");
void gMarkCvkSlotReady(u32 arg0) __asm__("func_80023500");
Unk23528Header *gFindCshdSoundGroup(s32 id, s32 *outIndex) __asm__("func_80023528");

static void test_sound_tables(void)
{
    SoundGroup first = { { 0, 3, { { 12, 0, 0 } } }, { { 65535, 0, 0 }, { 12, 0, 0 } } };
    SoundGroup second = { { 0, 2, { { 12, 0, 0 } } }, { { 45, 0, 0 }, { 91, 0, 0 } } };
    Unk23528Header guard = { 0, 1, { { 97, 0, 0 } } };
    Unk23528Header *slot0 = &first.header;
    Unk23528Header *slot1 = &second.header;
    Unk23528Header *slot_guard = &guard;
    s32 index;

    /* @leaf 80023500 source src/game/boot-frontend/gMarkCvkSlotReady.c: unsigned slot bound and byte-only ready update */
    LK_FUNC("func_80023500");
    lk_fill(&cvk_slot_storage, 0xA5, sizeof(cvk_slot_storage));
    gMarkCvkSlotReady(8);
    gMarkCvkSlotReady(0xFFFFFFFFU);
    FCK_CHECK(lk_all(&cvk_slot_storage, 0, sizeof(cvk_slot_storage), 0xA5), "invalid slot indices leave table and guard alone");
    gMarkCvkSlotReady(7);
    FCK_EQ(cvk_slot_storage.slots[7].ready, 2, "last slot ready");
    FCK_CHECK(lk_all(&cvk_slot_storage, 0, 0x72, 0xA5), "bytes before last slot state unchanged");
    FCK_CHECK(lk_all(&cvk_slot_storage, 0x73, sizeof(cvk_slot_storage), 0xA5), "bytes after last slot state unchanged");
    gMarkCvkSlotReady(0);
    FCK_EQ(cvk_slot_storage.slots[0].ready, 2, "first slot ready");

    /* @leaf 80023528 source src/game/memory-resources/gFindCshdSoundGroup.c: ordered lookup, unsigned IDs and optional output */
    LK_FUNC("func_80023528");
    cshd_slot_storage.slots[0] = &slot0;
    cshd_slot_storage.slots[1] = &slot1;
    cshd_slot_storage.guard = &slot_guard;
    index = -99;
    FCK_CHECK(gFindCshdSoundGroup(12, &index) == &first.header, "first group wins duplicate ID");
    FCK_EQ(index, 0, "first matching entry wins within a group");
    FCK_CHECK(gFindCshdSoundGroup(65535, &index) == &first.header, "zero-extended ID");
    FCK_EQ(index, 1, "nonzero entry index");
    FCK_CHECK(gFindCshdSoundGroup(45, 0) == &second.header, "second group hit without index output");
    index = -99;
    FCK_CHECK(gFindCshdSoundGroup(-1, &index) == 0, "negative argument does not alias unsigned ID");
    FCK_EQ(index, -99, "miss preserves output");
    cshd_slot_storage.slots[0] = 0;
    FCK_CHECK(gFindCshdSoundGroup(12, &index) == &second.header, "null slot skipped");
    FCK_EQ(index, 0, "index in second group");
    FCK_CHECK(gFindCshdSoundGroup(91, &index) == 0, "backed entry outside count ignored");
    FCK_CHECK(gFindCshdSoundGroup(97, &index) == 0, "backed third group outside slot table ignored");
}

/* Front-end and platform state. */
s32 gPadVblankIrqFlag __asm__("D_8009C790");
s32 gDeferredTransition __asm__("D_8009F8C8");
u8 *gPendingRowCapture __asm__("D_8009F8C4");
u8 gMultiBrfMovieName[4] __asm__("D_80012094");
u8 *gSaveProfileImage __asm__("D_8009F92C");
u8 gUiHoldFlag __asm__("D_8009F8BD");
u8 gUiInteractionEnabled __asm__("D_8009F8BE");
u8 *gNameEntryAlphabet __asm__("D_8009F878");
u8 gNameEntryCharacters[64] __asm__("D_8009B3EC");
s16 gPlayerNameLength __asm__("D_8009F87C");
s32 gHudMode2 __asm__("D_8009FAB8");
s32 gOverlayTimer __asm__("D_8009F760");
unsigned char *gFontBankTable __asm__("D_8009F468");
s32 gSpuTransferCallback __asm__("D_8009EEDC");

s32 gTakePadVblankIrq(void) __asm__("func_8008A5A8");
void gQueueMultiBrfMovie(s32 arg0) __asm__("func_8007BF88");
void gResetProfileTransferFlags(void) __asm__("func_8007F8E0");
void gInitNameEntryAlphabet(void) __asm__("func_800780C0");
u8 gCheckHudLayoutNeedsAdvance(void) __asm__("func_80048790");
int gSetMenuCursorToItem2(void) __asm__("func_8007F678");
void gSpuSetTransferCallback(s32 value) __asm__("func_80098FB8");

static void test_state(void)
{
    u8 previous[4];
    u8 profile[0x948];
    u8 cursors[3];

    /* @leaf 8008A5A8 source src/game/boot-frontend/gTakePadVblankIrq.c: signed read-and-clear, then empty */
    LK_FUNC("func_8008A5A8");
    gPadVblankIrqFlag = -123;
    FCK_EQ(gTakePadVblankIrq(), -123, "returns prior IRQ state");
    FCK_EQ(gPadVblankIrqFlag, 0, "IRQ state consumed");
    FCK_EQ(gTakePadVblankIrq(), 0, "second poll empty");

    /* @leaf 8007BF88 source src/game/boot-frontend/gQueueMultiBrfMovie.c: only selector 3 replaces state and pointer */
    LK_FUNC("func_8007BF88");
    lk_fill(previous, 0x55, sizeof(previous));
    lk_fill(gMultiBrfMovieName, 0x66, sizeof(gMultiBrfMovieName));
    gDeferredTransition = -27;
    gPendingRowCapture = previous;
    gQueueMultiBrfMovie(2);
    gQueueMultiBrfMovie(4);
    FCK_EQ(gDeferredTransition, -27, "neighboring selectors leave state unchanged");
    FCK_CHECK(gPendingRowCapture == previous, "neighboring selectors leave pointer unchanged");
    gQueueMultiBrfMovie(3);
    FCK_EQ(gDeferredTransition, 5, "selector three queues transition");
    FCK_CHECK(gPendingRowCapture == gMultiBrfMovieName, "selector three installs data address");

    /* @leaf 8007F8E0 source src/game/memory-resources/gResetProfileTransferFlags.c: reset only page byte and two input flags */
    LK_FUNC("func_8007F8E0");
    lk_fill(profile, 0xB7, sizeof(profile));
    gSaveProfileImage = profile;
    gUiHoldFlag = 9;
    gUiInteractionEnabled = 13;
    gResetProfileTransferFlags();
    FCK_EQ(profile[0x921], 0, "profile page cleared");
    FCK_EQ(gUiHoldFlag, 0, "UI hold cleared");
    FCK_EQ(gUiInteractionEnabled, 0, "UI interaction flag cleared");
    FCK_CHECK(lk_all(profile, 0, 0x921, 0xB7) &&
              lk_all(profile, 0x922, sizeof(profile), 0xB7), "rest of profile left alone");

    /* @leaf 800780C0 source src/game/utilities/gInitNameEntryAlphabet.c: install table address and 64-entry length */
    LK_FUNC("func_800780C0");
    lk_fill(gNameEntryCharacters, 0x4D, sizeof(gNameEntryCharacters));
    gNameEntryAlphabet = previous;
    gPlayerNameLength = -7;
    gInitNameEntryAlphabet();
    FCK_CHECK(gNameEntryAlphabet == gNameEntryCharacters, "alphabet address installed");
    FCK_EQ(gPlayerNameLength, 64, "alphabet entry count");
    FCK_CHECK(lk_all(gNameEntryCharacters, 0, sizeof(gNameEntryCharacters), 0x4D), "table contents unchanged");

    /* @leaf 80048790 source src/game/gpu-2d-hud/gCheckHudLayoutNeedsAdvance.c: advance only in mode two with timer exactly zero */
    LK_FUNC("func_80048790");
    gHudMode2 = 0;
    gOverlayTimer = 0;
    FCK_EQ(gCheckHudLayoutNeedsAdvance(), 0, "zero mode does not advance");
    gHudMode2 = 3;
    FCK_EQ(gCheckHudLayoutNeedsAdvance(), 0, "unrelated mode does not advance");
    gHudMode2 = 2;
    FCK_EQ(gCheckHudLayoutNeedsAdvance(), 1, "mode two at zero advances");
    gOverlayTimer = 1;
    FCK_EQ(gCheckHudLayoutNeedsAdvance(), 0, "positive timer holds");
    gOverlayTimer = -1;
    FCK_EQ(gCheckHudLayoutNeedsAdvance(), 0, "negative timer also holds");

    /* @leaf 8007F678 source src/game/boot-frontend/gSetMenuCursorToItem2.c: return and store two at selected byte */
    LK_FUNC("func_8007F678");
    lk_fill(cursors, 0xA5, sizeof(cursors));
    gFontBankTable = cursors + 1;
    FCK_EQ(gSetMenuCursorToItem2(), 2, "selected item returned");
    FCK_EQ(cursors[1], 2, "selected byte overwritten");
    FCK_EQ(cursors[0], 0xA5, "previous byte unchanged");
    FCK_EQ(cursors[2], 0xA5, "next byte unchanged");

    /* @leaf 80098FB8 source src/game/platform/gSpuSetTransferCallback.c: replace, retain, and remove the callback word */
    LK_FUNC("func_80098FB8");
    gSpuTransferCallback = 0x1004;
    gSpuSetTransferCallback(0x2008);
    FCK_EQ(gSpuTransferCallback, 0x2008, "callback word replaced");
    gSpuSetTransferCallback(0x2008);
    FCK_EQ(gSpuTransferCallback, 0x2008, "same callback retained");
    gSpuSetTransferCallback(0);
    FCK_EQ(gSpuTransferCallback, 0, "zero removes callback");
}

/* Stream descriptor and context layouts from 80088F00.c. */
typedef struct Ent88F00 {
    u32 sectors;
    s32 w4, w8, padC;
    u32 byte_count;
    u8 pad14[0x14];
    s32 w28, ring_start, ring_end, w34;
} Ent88F00;

typedef struct {
    u8 pad00[0x42C];
    Ent88F00 *first, *current;
    u32 index, count;
    s32 status;
    s32 totals[4];
} Unk88F00;

Unk88F00 *gMovieStreamContext __asm__("D_8009F970");
Ent88F00 *gMovieStreamDescriptors __asm__("D_8009F974");
s32 gMovieRingStart __asm__("D_8009F978");
s32 gMovieRingEnd __asm__("D_8009F97C");
s32 gMovieStreamPrimeEntry(void) __asm__("func_80088F00");

static void test_movie_stream(void)
{
    Unk88F00 ctx;
    Ent88F00 entries[3];  /* last is backed but outside the logical count */

    /* @leaf 80088F00 source src/game/boot-frontend/gMovieStreamPrimeEntry.c: descriptor priming, sector rounding and exhaustion */
    LK_FUNC("func_80088F00");
    lk_fill(&ctx, 0xA5, sizeof(ctx));
    lk_fill(entries, 0xA5, sizeof(entries));
    ctx.first = ctx.current = 0;
    ctx.index = 0;
    ctx.count = 2;
    ctx.status = -7;
    entries[0].byte_count = 2048;
    entries[1].byte_count = 2049;
    gMovieStreamContext = &ctx;
    gMovieStreamDescriptors = entries;
    gMovieRingStart = 17;
    gMovieRingEnd = 73;

    FCK_EQ(gMovieStreamPrimeEntry(), -7, "nonzero status returned verbatim");
    FCK_EQ(ctx.index, 0, "nonzero status does not consume entry");
    FCK_CHECK(ctx.first == 0 && ctx.current == 0, "nonzero status does not select entry");
    FCK_EQ(entries[0].sectors, 0xA5A5A5A5U, "nonzero status leaves descriptor intact");

    ctx.status = 0;
    FCK_EQ(gMovieStreamPrimeEntry(), 0, "first descriptor primed");
    FCK_CHECK(ctx.first == entries && ctx.current == entries, "first and current point to entry zero");
    FCK_EQ(ctx.index, 1, "first entry consumed");
    FCK_EQ(entries[0].sectors, 1, "exact sector is not rounded up");
    FCK_EQ(entries[0].byte_count, 2048, "byte count retained");
    FCK_CHECK(entries[0].w4 == 0 && entries[0].w8 == 0 &&
              entries[0].w28 == 0 && entries[0].w34 == 0, "descriptor counters reset");
    FCK_EQ(entries[0].ring_start, 17, "ring start copied");
    FCK_EQ(entries[0].ring_end, 73, "ring end copied");
    FCK_CHECK(lk_all(ctx.totals, 0, sizeof(ctx.totals), 0), "first entry resets all totals");
    FCK_CHECK(lk_all(entries[0].pad14, 0, sizeof(entries[0].pad14), 0xA5), "descriptor padding retained");
    FCK_EQ(entries[1].sectors, 0xA5A5A5A5U, "second entry not touched early");

    lk_fill(ctx.totals, 0x39, sizeof(ctx.totals));
    FCK_EQ(gMovieStreamPrimeEntry(), 0, "second descriptor primed");
    FCK_CHECK(ctx.first == entries && ctx.current == entries + 1, "first pointer retained");
    FCK_EQ(ctx.index, 2, "second entry consumed");
    FCK_EQ(entries[1].sectors, 2, "partial sector rounded up");
    FCK_CHECK(lk_all(ctx.totals, 0, sizeof(ctx.totals), 0x39), "later entry preserves totals");

    FCK_EQ(gMovieStreamPrimeEntry(), 2, "count boundary reports exhaustion");
    FCK_EQ(ctx.status, 2, "exhaustion stored");
    FCK_EQ(ctx.index, 2, "exhaustion does not advance index");
    FCK_CHECK(ctx.current == entries + 1, "exhaustion retains current descriptor");
    FCK_CHECK(lk_all(entries + 2, 0, sizeof(entries[2]), 0xA5), "backing outside logical count untouched");
    FCK_EQ(gMovieStreamPrimeEntry(), 2, "exhaustion stays latched");
}

/* The signed-word buffers and nested pointer match 80056530.c offsets. */
s32 gSpringFollowHeight(s32 *obj, s32 scale, s32 count) __asm__("func_80056530");

typedef struct {
    s32 before[0x54 / 4];
    s32 target;
    s32 between[(0xAC - 0x58) / 4];
    s32 *state;
} SpringObject;

static void test_spring(void)
{
    SpringObject obj;
    s32 state[0xE0 / 4];

    /* @leaf 80056530 source src/game/gpu-2d-hud/gSpringFollowHeight.c: fixed-point spring step, snap, dead zone and damping */
    LK_FUNC("func_80056530");
    lk_fill(&obj, 0, sizeof(obj));
    lk_fill(state, 0xA5, sizeof(state));
    obj.state = state;
    obj.target = 65536;
    state[0xD4 / 4] = 0;
    state[0xD8 / 4] = -65536;
    /* A one-unit Q16 gap adds 1<<22 to velocity per tick. Two ticks
     * give (1<<23)>>14 = 512 before scaling by eight. */
    FCK_EQ(gSpringFollowHeight(obj.before, 8, 2), 4096, "two ticks integrate after discarding opposing velocity");
    FCK_EQ(state[0xD4 / 4], 4096, "new height stored");
    FCK_EQ(state[0xD8 / 4], 8388608, "integrated velocity stored");

    obj.target = 32768;
    state[0xD4 / 4] = 0;
    state[0xD8 / 4] = 0;
    FCK_EQ(gSpringFollowHeight(obj.before, 1, 1), 0, "sub-threshold step suppressed");
    FCK_EQ(state[0xD8 / 4], 524288, "dead zone does not discard velocity");

    obj.target = 65536;
    state[0xD4 / 4] = 0;
    state[0xD8 / 4] = 0;
    FCK_EQ(gSpringFollowHeight(obj.before, 1024, 1), 65536, "overshoot clamped to target");
    FCK_EQ(state[0xD4 / 4], 65536, "clamped height stored");
    FCK_EQ(gSpringFollowHeight(obj.before, 1024, 1), 65536, "already aligned height retained");
    FCK_EQ(state[0xD8 / 4], 0, "already aligned velocity cleared");
    FCK_CHECK(lk_all(state, 0, 0xD4, 0xA5) &&
              lk_all(state, 0xDC, sizeof(state), 0xA5), "adjacent state remains untouched");
}

/* Compact debris part table and 20-byte ranges from 8003A960.c. */
struct Item { u32 flags; u8 a, b, c, d; u8 pad[4]; };
struct Data { s32 count; struct Item items[1]; };
struct Range { u8 pad[3]; u8 start, count; u8 rest[15]; };
struct Info { u8 pad[0x14]; struct Range *ranges; u8 pad2[12]; struct Data *data; };
struct State { u8 pad[0xA8]; u32 bits[1]; };
void gClearDebrisPartBitsOutsideRange(struct State *self, struct Info *info, s32 index) __asm__("func_8003A960");

static void test_debris_mask(void)
{
    struct State state;
    struct Info info;
    struct Range ranges[2];
    struct { struct Data data; struct Item tail[5]; } parts;

    /* @leaf 8003A960 source src/game/boot-frontend/gClearDebrisPartBitsOutsideRange.c: filter active bits by an inclusive range and optional fourth vertex */
    LK_FUNC("func_8003A960");
    lk_fill(&state, 0xA5, sizeof(state));
    lk_fill(&info, 0, sizeof(info));
    lk_fill(ranges, 0, sizeof(ranges));
    lk_fill(&parts, 0, sizeof(parts));
    state.bits[0] = 0x8000007D;
    info.ranges = ranges;
    info.data = &parts.data;
    ranges[1].start = 10;
    ranges[1].count = 3;
    parts.data.count = 6;
    parts.data.items[0].a = 10;
    parts.data.items[0].b = 11;
    parts.data.items[0].c = 12;
    parts.tail[0].a = 9;
    parts.tail[0].b = parts.tail[0].c = 10;
    parts.tail[1].a = parts.tail[1].b = 10;
    parts.tail[1].c = 13;
    parts.tail[2] = parts.data.items[0];
    parts.tail[2].flags = 0x300;
    parts.tail[3] = parts.data.items[0];
    parts.tail[3].flags = 0x400;
    parts.tail[4] = parts.data.items[0];

    gClearDebrisPartBitsOutsideRange(&state, &info, 1);
    FCK_EQ(state.bits[0], 0x80000049U, "keeps endpoints and ignores fourth vertex except for kind four");
    gClearDebrisPartBitsOutsideRange(&state, &info, 0);
    FCK_EQ(state.bits[0], 0x80000001U, "empty range clears only active parts");
    FCK_CHECK(lk_all(state.pad, 0, sizeof(state.pad), 0xA5), "state prefix not modified");
}

/* Linked model wave nodes from 800135C0.c. */
typedef struct Node {
    struct Node *next;
    u8 *data;
    int count;
    u8 entry[8];
} Node;

void gUpdateModelWaveBytes(Node *node, int time) __asm__("func_800135C0");

static void test_model_wave(void)
{
    u8 first[24], second[12];
    Node tail = { 0, second, 1, { 0, 2, 21, 77, 99, 40, 92, 99 } };
    Node head = { &tail, first, 1, { 1, 1, 10, 45, 100, 29, 4, 139 } };

    /* @leaf 800135C0 source src/game/gpu-2d-hud/gUpdateModelWaveBytes.c: linked byte animation, phase fold and wrap */
    LK_FUNC("func_800135C0");
    lk_fill(first, 0xA5, sizeof(first));
    lk_fill(second, 0xA5, sizeof(second));
    gUpdateModelWaveBytes(&head, 1024);
    FCK_EQ(first[17], 20, "half-phase rising byte");
    FCK_EQ(first[18], 25, "half-phase falling byte");
    FCK_EQ(first[19], 120, "third animated byte");
    FCK_EQ(second[5], 40, "linked node uses its own doubled phase");
    FCK_EQ(second[6], 92, "phase 2048 folds to 2047");
    FCK_EQ(second[7], 99, "equal endpoints stay equal at folded phase");
    FCK_CHECK(lk_all(first, 0, 17, 0xA5) && lk_all(first, 20, sizeof(first), 0xA5), "only selected record bytes changed");
    FCK_CHECK(lk_all(second, 0, 5, 0xA5) && lk_all(second, 8, sizeof(second), 0xA5), "linked record canaries intact");
    gUpdateModelWaveBytes(&head, 4096);
    FCK_CHECK(first[17] == 10 && first[18] == 45 && first[19] == 100, "12-bit phase wraps to original values");
    tail.count = 0;
    second[5] = 0xA5;
    gUpdateModelWaveBytes(&head, 1024);
    FCK_EQ(second[5], 0xA5, "zero-count node left unchanged");
}

/* RAM-only GPU packet fixtures from 80083060.c, 80082EF0.c and 8001C148.c. */
struct Poly {
    u32 tag;
    u8 r, g, b, code;
    s16 xy[8];
    u8 pad[0x1C];
};
struct HalfRect { u32 xy, wh; };

struct Poly *gPrimPoolNext __asm__("D_8009F9F0");
struct Poly *gPrimPoolEnd __asm__("D_8009FA28");
u8 *gPacketCursor __asm__("D_8009FA90");
u8 *gPacketLimit __asm__("D_8009F9C0");
void gEmitFlatQuad(s32 x0, s32 y0, s32 x1, s32 y1, s32 x2, s32 y2, s32 x3, s32 y3,
                   u32 link, s32 r, s32 g, s32 b, s32 code) __asm__("func_80083060");
void gEmitFlatQuadRect(s32 x, s32 y, s32 w, s32 h, u32 link,
                       s32 r, s32 g, s32 b) __asm__("func_80082EF0");
void gEmitSolidTileRect(u8 red, u8 green, u8 blue, struct HalfRect *rect,
                        u8 blend) __asm__("func_8001C148");

static void test_packets(void)
{
    struct Poly polys[3];
    u32 packets[3][7];
    struct HalfRect rect = { 0x0007FFFD, 0x000B000D };
    u8 *bytes = (u8 *)packets;

    /* @leaf 80083060 source src/game/gpu-2d-hud/gEmitFlatQuad.c: quad packing, field widths and pool cursor boundary */
    LK_FUNC("func_80083060");
    lk_fill(polys, 0xA5, sizeof(polys));
    gPrimPoolNext = polys;
    gPrimPoolEnd = polys + 1;
    gEmitFlatQuad(-7, 8, 19, -10, 31, 12, 43, 65535,
                  0xAB123456U, 0x144, 0x55, 0x66, 2);
    FCK_CHECK(gPrimPoolNext == polys + 1, "quad consumes a 0x34-byte slot");
    FCK_EQ(polys[0].tag, 0x05123456, "quad length and low 24-bit link");
    FCK_EQ(polys[0].code, 0x2A, "quad command combines base and blend flag");
    FCK_CHECK(polys[0].r == 0x44 && polys[0].g == 0x55 && polys[0].b == 0x66, "quad colors narrow to bytes");
    FCK_CHECK(polys[0].xy[0] == -7 && polys[0].xy[1] == 8 &&
              polys[0].xy[2] == 19 && polys[0].xy[3] == -10 &&
              polys[0].xy[4] == 31 && polys[0].xy[5] == 12 &&
              polys[0].xy[6] == 43 && polys[0].xy[7] == -1, "quad coordinate order and halfword storage");
    FCK_CHECK(lk_all(polys[0].pad, 0, sizeof(polys[0].pad), 0xA5), "quad padding retained");
    FCK_CHECK(lk_all(polys + 1, 0, sizeof(polys[1]), 0xA5), "next slot not written early");
    /* End points to writable spill storage: the source still writes at end. */
    gEmitFlatQuad(1, 2, 3, 4, 5, 6, 7, 8, 0, 1, 2, 3, 0);
    FCK_CHECK(gPrimPoolNext == gPrimPoolEnd, "quad at limit does not advance");
    FCK_EQ(polys[1].tag, 0x05000000, "quad at limit still writes the slot");

    /* @leaf 80082EF0 source src/game/gpu-2d-hud/gEmitFlatQuadRect.c: rectangle corner order and pool cursor boundary */
    LK_FUNC("func_80082EF0");
    lk_fill(polys, 0xA5, sizeof(polys));
    gPrimPoolNext = polys;
    gPrimPoolEnd = polys + 1;
    gEmitFlatQuadRect(-6, 9, 23, 31, 0xCD123456U, 101, 37, 243);
    FCK_CHECK(gPrimPoolNext == polys + 1, "rect consumes one slot");
    FCK_EQ(polys[0].tag, 0x05123456, "rect length and link");
    FCK_EQ(polys[0].code, 0x28, "rect command");
    FCK_CHECK(polys[0].r == 101 && polys[0].g == 37 && polys[0].b == 243, "rect colors");
    FCK_CHECK(polys[0].xy[0] == -6 && polys[0].xy[1] == 9 &&
              polys[0].xy[2] == 17 && polys[0].xy[3] == 9 &&
              polys[0].xy[4] == -6 && polys[0].xy[5] == 40 &&
              polys[0].xy[6] == 17 && polys[0].xy[7] == 40, "rect corners derive from extent");
    FCK_CHECK(lk_all(polys[0].pad, 0, sizeof(polys[0].pad), 0xA5), "rect padding retained");
    gEmitFlatQuadRect(7, -8, 0, 0, 1, 1, 2, 3);
    FCK_CHECK(gPrimPoolNext == gPrimPoolEnd, "rect at limit does not advance");
    FCK_CHECK(polys[1].xy[0] == 7 && polys[1].xy[6] == 7 &&
              polys[1].xy[1] == -8 && polys[1].xy[7] == -8, "zero extent still emits rect at limit");
    FCK_CHECK(lk_all(polys + 2, 0, sizeof(polys[2]), 0xA5), "guard slot untouched");

    /* @leaf 8001C148 source src/game/gpu-2d-hud/gEmitSolidTileRect.c: draw-page and tile packets, blend and capacity boundary */
    LK_FUNC("func_8001C148");
    lk_fill(packets, 0xA5, sizeof(packets));
    gPacketCursor = bytes;
    gPacketLimit = bytes + 2 * sizeof(packets[0]);
    gEmitSolidTileRect(0x12, 0x34, 0xFE, &rect, 3);
    FCK_CHECK(gPacketCursor == gPacketLimit, "tile emission consumes two 0x1C-byte slots");
    FCK_EQ(packets[0][0], 0x01000000, "draw-page tag");
    FCK_EQ(packets[0][1], 0xE1000000U, "draw-page command");
    FCK_EQ(packets[1][0], 0x03000000, "tile tag");
    FCK_EQ(packets[1][1], 0x62FE3412, "tile command and color bytes with blend");
    FCK_EQ(packets[1][2], 0x0007FFFD, "tile XY");
    FCK_EQ(packets[1][3], 0x000B000D, "tile width and height");
    FCK_CHECK(lk_all(packets[0], 8, sizeof(packets[0]), 0xA5) &&
              lk_all(packets[1], 16, sizeof(packets[1]), 0xA5), "unused packet bytes unchanged");
    gEmitSolidTileRect(5, 7, 9, &rect, 0);
    FCK_CHECK(gPacketCursor == gPacketLimit, "full packet cursor does not advance");
    FCK_EQ(packets[2][0], 0x03000000, "tile overwrites draw-page tag at shared spill slot");
    FCK_EQ(packets[2][1], 0x60090705, "opaque tile overwrites draw-page command at shared spill slot");
}

/* Data layouts shared by the original response handlers 8008B508.c and
 * 8008B210.c. The incoming response bytes are provided directly; no SIO. */
struct PadCapabilityRecord {
    u8 protocol[2];
    u8 payload_bytes, activation_cost, high_bit;
};
struct PadCombinationRecord {
    u8 length;
    u8 reserved[3];
    u8 *data;
};
struct PadPort {
    u16 *mode_table;
    struct PadCapabilityRecord *capabilities;
    struct PadCombinationRecord *combinations;
    u8 pad0C[0x14];
    u8 *requested_actuator_map;
    u8 pad24[0x18];
    u8 *response;
    u8 pad40[6];
    u8 sequence_step, info_record_index, combination_bytes_remaining, communication_state;
    u8 pad4A[0x13];
    u8 actuator_map[6];
    u8 pad63[0x80];
    u8 mode_count;
    u8 padE4[5];
    u8 actuator_count, combination_count;
};

u8 *gCombinationPtr __asm__("D_800A3B80");
s32 gRespPadActAlign(struct PadPort *port) __asm__("func_8008B508");
s32 gRespPadTableHandshake(struct PadPort *input_port) __asm__("func_8008B210");

static void test_pad_alignment(void)
{
    struct PadPort port;
    struct PadCapabilityRecord caps[3];
    u8 requests[] = { 0, 1, 0xFF, 1, 0xFF, 2 };

    /* @leaf 8008B508 source src/game/boot-frontend/gRespPadActAlign.c: size-qualified actuator map and completion step */
    LK_FUNC("func_8008B508");
    lk_fill(&port, 0, sizeof(port));
    lk_fill(caps, 0, sizeof(caps));
    lk_fill(port.actuator_map, 0xA5, sizeof(port.actuator_map));
    port.capabilities = caps;
    port.requested_actuator_map = requests;
    port.actuator_count = 3;
    port.sequence_step = 6;
    caps[0].payload_bytes = 2;
    caps[1].payload_bytes = 2;

    FCK_EQ(gRespPadActAlign(&port), 0, "alignment returns zero");
    FCK_EQ(port.sequence_step, 0xFE, "alignment marks completion");
    FCK_EQ(port.actuator_map[0], 0xFF, "request short of required byte count rejected");
    FCK_EQ(port.actuator_map[1], 1, "first byte of exact two-byte request mapped");
    FCK_EQ(port.actuator_map[3], 1, "second byte of exact request mapped");
    FCK_EQ(port.actuator_map[5], 2, "zero payload size falls back to one byte at last position");
    FCK_EQ(port.actuator_map[2], 0xA5, "unmatched request byte left alone");
    FCK_EQ(port.actuator_map[4], 0xA5, "other unmatched byte left alone");

    lk_fill(port.actuator_map, 0x39, sizeof(port.actuator_map));
    port.actuator_count = 0;
    port.sequence_step = 6;
    FCK_EQ(gRespPadActAlign(&port), 0, "empty capability list returns zero");
    FCK_EQ(port.sequence_step, 0xFE, "empty list still marks completion");
    FCK_CHECK(lk_all(port.actuator_map, 0, sizeof(port.actuator_map), 0x39), "empty list leaves map untouched");
}

static void test_pad_handshake(void)
{
    struct PadPort port;
    struct PadCapabilityRecord caps[2];
    struct {
        struct PadCombinationRecord records[2];
        u8 data[16];
    } combos;
    u16 modes[3];
    u8 response[10];

    /* @leaf 8008B210 source src/game/boot-frontend/gRespPadTableHandshake.c: multi-response mode, capability and aligned combination tables */
    LK_FUNC("func_8008B210");
    lk_fill(&port, 0, sizeof(port));
    lk_fill(caps, 0xA5, sizeof(caps));
    lk_fill(&combos, 0xA5, sizeof(combos));
    lk_fill(modes, 0xA5, sizeof(modes));
    lk_fill(response, 0, sizeof(response));
    port.mode_table = modes;
    port.capabilities = caps;
    port.combinations = combos.records;
    port.response = response;
    port.mode_count = 2;
    port.actuator_count = 1;
    port.combination_count = 2;
    gCombinationPtr = 0;

    port.sequence_step = 2;
    response[4] = 0xAB;
    response[5] = 0xCD;
    FCK_EQ(gRespPadTableHandshake(&port), 0, "mode table still incomplete after first response");
    FCK_EQ(modes[0], 0xABCD, "mode word unpacked high byte first");
    FCK_EQ(port.info_record_index, 1, "next mode slot selected");
    response[4] = 0x12;
    response[5] = 0x34;
    FCK_EQ(gRespPadTableHandshake(&port), 1, "mode table complete at count");
    FCK_EQ(modes[1], 0x1234, "second mode entry");
    FCK_EQ(modes[2], 0xA5A5, "mode table guard untouched");
    FCK_EQ(port.info_record_index, 0, "mode index reset");

    port.sequence_step = 3;
    port.combination_bytes_remaining = 19;
    response[4] = 0x26;
    response[5] = 0xD3;
    response[6] = 7;
    response[7] = 11;
    FCK_EQ(gRespPadTableHandshake(&port), 1, "capability table complete at count");
    FCK_CHECK(caps[0].protocol[0] == 0x26 && caps[0].protocol[1] == 0x53 &&
              caps[0].high_bit == 1, "capability splits top protocol bit");
    FCK_CHECK(caps[0].payload_bytes == 7 && caps[0].activation_cost == 11, "capability lengths copied");
    FCK_CHECK(lk_all(caps + 1, 0, sizeof(caps[1]), 0xA5), "capability guard untouched");
    FCK_EQ(port.info_record_index, 0, "capability index reset");
    FCK_EQ(port.combination_bytes_remaining, 0, "capability completion clears byte counter");

    port.sequence_step = 4;
    response[4] = 5;
    response[5] = 0x21;
    response[6] = 0x32;
    response[7] = 0x43;
    FCK_EQ(gRespPadTableHandshake(&port), 0, "first combination needs continuation");
    FCK_EQ(combos.records[0].length, 5, "combination length captured");
    FCK_CHECK(combos.records[0].data == combos.data, "first payload begins after the records");
    FCK_CHECK(combos.data[0] == 0x21 && combos.data[1] == 0x32 &&
              combos.data[2] == 0x43, "new record copies three bytes from response offset five");
    FCK_EQ(port.combination_bytes_remaining, 2, "two payload bytes remaining");
    FCK_EQ(port.info_record_index, 0, "index stays on partial combination");

    response[2] = 0x54;
    response[3] = 0x65;
    FCK_EQ(gRespPadTableHandshake(&port), 0, "continuation finishes first combination");
    FCK_CHECK(combos.data[3] == 0x54 && combos.data[4] == 0x65, "continuation starts at response offset two");
    FCK_CHECK(gCombinationPtr == combos.data + 5, "write cursor ends after first payload");
    FCK_EQ(port.combination_bytes_remaining, 0, "continuation consumes remaining bytes");
    FCK_EQ(port.info_record_index, 1, "index advances only at combination completion");

    response[4] = 2;
    response[5] = 0x76;
    response[6] = 0x87;
    FCK_EQ(gRespPadTableHandshake(&port), 0, "second combination completes handshake");
    FCK_EQ(combos.records[1].length, 2, "second combination length");
    FCK_CHECK(combos.records[1].data == combos.data + 8, "second payload starts at four-byte boundary");
    FCK_CHECK(combos.data[8] == 0x76 && combos.data[9] == 0x87, "second payload bytes");
    FCK_CHECK(lk_all(combos.data, 5, 8, 0xA5) &&
              lk_all(combos.data, 10, sizeof(combos.data), 0xA5), "alignment padding and payload guard untouched");
    FCK_EQ(port.communication_state, 6, "handshake sets communication state");
    FCK_EQ(port.sequence_step, 0xFE, "handshake sets terminal step");
    FCK_EQ(gRespPadTableHandshake(&port), 1, "terminal step hits default return");
}

/* The port lookup seam in 8008A118.c supplies a selected test record and
 * captures the actual port argument. The original unit does the indexing. */
struct PadCombEntry {
    u8 length;
    u8 pad01[3];
    u8 *ptr;
};
struct PadCombData {
    void *pad, *pad4;
    struct PadCombEntry *entries;
    u8 pad0C[0xDE];
    u8 count;
};

static struct PadCombData *pad_lookup_result;
static int pad_lookup_arg, pad_lookup_calls;

static struct PadCombData *lookup_fixture_port(int port)
{
    pad_lookup_arg = port;
    pad_lookup_calls++;
    return pad_lookup_result;
}

struct PadCombData *(*gPadLookupPortFn)(int) __asm__("D_8009C73C");
int gGetPadCombInfo(int arg0, int index, int kind) __asm__("func_8008A118");

static void test_pad_combination_query(void)
{
    struct PadCombData port;
    struct PadCombEntry entries[3];
    u8 payload0[] = { 0x12, 0x34 };
    u8 payload1[] = { 0x8E, 0xFF, 0x6D };
    u8 payload2[] = { 0x79 };

    /* @leaf 8008A118 source src/game/memory-resources/gGetPadCombInfo.c: query count, record length, bytes and bounds */
    LK_FUNC("func_8008A118");
    lk_fill(&port, 0, sizeof(port));
    lk_fill(entries, 0, sizeof(entries));
    port.entries = entries;
    port.count = 2;
    entries[0].length = 2;
    entries[0].ptr = payload0;
    entries[1].length = 2;
    entries[1].ptr = payload1;
    entries[2].length = 1;
    entries[2].ptr = payload2;
    gPadLookupPortFn = lookup_fixture_port;
    pad_lookup_result = &port;
    pad_lookup_arg = -1;
    pad_lookup_calls = 0;

    FCK_EQ(gGetPadCombInfo(37, -2, 0), 2, "negative index queries record count");
    FCK_EQ(pad_lookup_arg, 37, "port selector forwarded");
    FCK_EQ(gGetPadCombInfo(21, 1, -2), 2, "negative kind queries record length");
    FCK_EQ(pad_lookup_arg, 21, "changed port selector forwarded");
    FCK_EQ(gGetPadCombInfo(21, 0, 1), 0x34, "first record byte");
    FCK_EQ(gGetPadCombInfo(21, 1, 1), 255, "second record byte is unsigned");
    FCK_EQ(gGetPadCombInfo(21, 1, 2), 0, "kind equal to length is rejected");
    FCK_EQ(gGetPadCombInfo(21, 2, 0), 0, "index equal to count is rejected");
    FCK_EQ(pad_lookup_calls, 6, "each query looks up its port once");
}

/* Callback lists from 8003FE90.c; the callback only observes detachment. */
struct Target { u8 pad[0x1C]; u8 flags; };
struct Owner { u8 pad[0x44]; struct Target *target; };
struct RetiredNode {
    struct RetiredNode *next;
    u8 pad04[0x18];
    struct Owner *owner;
    u8 pad20[4];
    void (*callback)(int, struct RetiredNode *, int, int);
};

struct RetiredNode *gListHeads[7] __asm__("D_800A1070");
void gRetireDeferredNodes(struct Target *arg) __asm__("func_8003FE90");

static struct RetiredNode *retired_order[6];
static int retired_count, retired_bad_message, retired_not_detached, retired_flag_cleared_early;

static void record_retirement(int message, struct RetiredNode *node, int arg2, int arg3)
{
    if (retired_count < 6)
        retired_order[retired_count] = node;
    retired_count++;
    if (message != 10 || arg2 != 0 || arg3 != 0)
        retired_bad_message++;
    if (node->next != 0)
        retired_not_detached++;
    if (!(node->owner->target->flags & 1))
        retired_flag_cleared_early++;
}

static void test_retirement(void)
{
    struct Target target, other;
    struct Owner matching, not_matching;
    struct RetiredNode nodes[6];
    int i;

    /* @leaf 8003FE90 source src/game/boot-frontend/gRetireDeferredNodes.c: unlink matching nodes across lists before notification */
    LK_FUNC("func_8003FE90");
    lk_fill(&target, 0, sizeof(target));
    lk_fill(&other, 0, sizeof(other));
    lk_fill(&matching, 0, sizeof(matching));
    lk_fill(&not_matching, 0, sizeof(not_matching));
    lk_fill(nodes, 0, sizeof(nodes));
    target.flags = 0xA7;
    other.flags = 0x63;
    matching.target = &target;
    not_matching.target = &other;
    retired_count = retired_bad_message = retired_not_detached = retired_flag_cleared_early = 0;
    for (i = 0; i < 7; i++)
        gListHeads[i] = 0;
    for (i = 0; i < 6; i++) {
        nodes[i].owner = &matching;
        nodes[i].callback = record_retirement;
        retired_order[i] = 0;
    }
    nodes[0].next = nodes + 1;
    nodes[1].next = nodes + 2;
    nodes[2].next = nodes + 3;
    nodes[1].owner = nodes[4].owner = &not_matching;
    gListHeads[0] = nodes;
    gListHeads[3] = nodes + 4;
    gListHeads[6] = nodes + 5;

    gRetireDeferredNodes(&target);
    FCK_CHECK(gListHeads[0] == nodes + 1 && nodes[1].next == 0, "head and consecutive matches removed around survivor");
    FCK_CHECK(gListHeads[3] == nodes + 4 && nodes[4].next == 0, "unrelated list unchanged");
    FCK_CHECK(gListHeads[6] == 0, "matching singleton in last list removed");
    FCK_EQ(retired_count, 4, "one callback per removed node");
    FCK_CHECK(retired_order[0] == nodes && retired_order[1] == nodes + 2 &&
              retired_order[2] == nodes + 3 && retired_order[3] == nodes + 5, "retirement follows traversal order");
    FCK_EQ(retired_bad_message, 0, "callbacks receive message ten and two zero arguments");
    FCK_EQ(retired_not_detached, 0, "callbacks observe detached nodes");
    FCK_EQ(retired_flag_cleared_early, 0, "target flag remains set during callbacks");
    FCK_EQ(target.flags, 0xA6, "only target low flag bit cleared");
    FCK_EQ(other.flags, 0x63, "unrelated target flags untouched");

    gRetireDeferredNodes(&target);
    FCK_EQ(retired_count, 4, "second retirement does not re-notify");
    FCK_EQ(target.flags, 0xA6, "cleared flag remains clear");
}

int main(void)
{
    test_sound_tables();
    test_state();
    test_movie_stream();
    test_spring();
    test_debris_mask();
    test_model_wave();
    test_packets();
    test_pad_alignment();
    test_pad_handshake();
    test_pad_combination_query();
    test_retirement();
    return lk_done("leaf_follow_a");
}
