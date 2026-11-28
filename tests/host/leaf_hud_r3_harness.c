/* HUD text/glyph and render-effect leaf suite (round 3): five unchanged
 * original MIPS C units from src/game/ running under qemu-mipsel on
 * harness-defined fixtures:
 *
 *   800447F0 gAllocPeriodicParticleEmitter  periodic effect object alloc/init
 *   8004783C gResetHudTextState             three layer-word clears + renderer reset
 *   80048F3C gFreeFontGlyphBuffers          per-font glyph block release
 *   80047868 gFreeAllFontGlyphs             wrapper over the real 80048F3C
 *   80082AB0 gEmitAtlasSpriteSimple         9-arg atlas sprite forwarder
 *
 * Callees without coverage of their own are recording stand-ins bound to the
 * retail label with __asm__("func_XXXX"): 80017EBC (allocator), 80043430
 * (emitter setup), 800181D4 (free), 80048EC4 (text renderer reset), 80082B0C
 * (atlas sprite emitter). 80047868 runs against the real 80048F3C unit that is
 * compiled into this suite, so the wrapper's delegation and the exact freed
 * pointer set are observed on the original code, not on a stand-in.
 *
 * All five units touch only pointers/RAM the harness owns: no absolute kseg0
 * access, so all five run in qemu-mipsel user mode. D_80044610 is a function
 * symbol, not a data slot: the harness defines it as a counting handler so the
 * stored update pointer can be compared by identity and then called.
 *
 * Every expectation is hand-worked from the unit's own body (source evidence);
 * no value was obtained by running the unit.
 */
/* @link 80048F3C */
#include "leaf_kit.h"

/* ============================================================================
 * Shared fixtures: D_ globals referenced by the units
 * ============================================================================ */

/* 800447F0: object->update handler symbol. */
static struct {
    s32 calls;
} s_msg;

void mock_periodic_msg(void) __asm__("D_80044610");
void mock_periodic_msg(void)
{
    s_msg.calls++;
}

/* 8004783C: three HUD layer words cleared before the renderer reset. */
s32 gHudMode2 __asm__("D_8009FAB8");
s32 gOverlayTimer __asm__("D_8009F760");
s32 gFrameSwitch __asm__("D_8009FAC8");

/* 80048F3C / 80047868: 0x11C-byte font records and the loaded-font count. */
struct HFontEntry {
    u8 unk00[0x18];
    void *glyphs;
    u8 unk1C[0x100];
};
struct HFontEntry gGlyphEntryTable[5] __asm__("D_800A19A0");
s32 gFontBankCount __asm__("D_8009F364");

/* ============================================================================
 * Recording stand-ins (all bound to the retail label)
 * ============================================================================ */

/* --- func_80017EBC: allocator; returns pre-queued pointers in call order --- */
#define ALLOC_LOG_MAX 8
static struct {
    s32 calls;
    s32 sizes[ALLOC_LOG_MAX];
    s32 tags[ALLOC_LOG_MAX];
    void *results[ALLOC_LOG_MAX];
    s32 next_index;
} s_alloc;

static void reset_alloc(void)
{
    memset(&s_alloc, 0, sizeof(s_alloc));
}

void *mock_alloc(s32 size, s32 tag) __asm__("func_80017EBC");
void *mock_alloc(s32 size, s32 tag)
{
    void *ret = 0;
    if (s_alloc.calls < ALLOC_LOG_MAX) {
        s_alloc.sizes[s_alloc.calls] = size;
        s_alloc.tags[s_alloc.calls] = tag;
    }
    if (s_alloc.next_index < ALLOC_LOG_MAX) {
        ret = s_alloc.results[s_alloc.next_index++];
    }
    s_alloc.calls++;
    return ret;
}

/* --- func_800181D4: block free --- */
#define FREE_LOG_MAX 16
static struct {
    s32 calls;
    void *ptrs[FREE_LOG_MAX];
} s_free;

static void reset_free(void)
{
    memset(&s_free, 0, sizeof(s_free));
}

void mock_free(void *block) __asm__("func_800181D4");
void mock_free(void *block)
{
    if (s_free.calls < FREE_LOG_MAX) {
        s_free.ptrs[s_free.calls] = block;
    }
    s_free.calls++;
}

/* --- func_80043430: emitter setup; returns the queued byte --- */
static struct {
    s32 calls;
    void *last_obj;
    s32 last_def;
    u8 ret[4];
    s32 ret_count;
} s_setup;

static void reset_setup(void)
{
    memset(&s_setup, 0, sizeof(s_setup));
}

u8 mock_setup_emitter(void *object, s32 def) __asm__("func_80043430");
u8 mock_setup_emitter(void *object, s32 def)
{
    u8 r = 0;
    if (s_setup.ret_count < 4) {
        r = s_setup.ret[s_setup.ret_count++];
    }
    s_setup.calls++;
    s_setup.last_obj = object;
    s_setup.last_def = def;
    return r;
}

/* --- func_80048EC4: text renderer reset; records the three words it sees --- */
static struct {
    s32 calls;
    s32 seen_mode2;
    s32 seen_overlay;
    s32 seen_frame;
} s_textreset;

static void reset_textreset(void)
{
    memset(&s_textreset, 0, sizeof(s_textreset));
}

void mock_init_text(void) __asm__("func_80048EC4");
void mock_init_text(void)
{
    s_textreset.calls++;
    s_textreset.seen_mode2 = gHudMode2;
    s_textreset.seen_overlay = gOverlayTimer;
    s_textreset.seen_frame = gFrameSwitch;
}

/* --- func_80082B0C: 11-argument atlas sprite emitter --- */
static struct {
    s32 calls;
    s32 a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10;
    void *ret;
} s_atlas;

static void reset_atlas(void *ret)
{
    memset(&s_atlas, 0, sizeof(s_atlas));
    s_atlas.ret = ret;
}

void *mock_draw_atlas(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6,
                      s32 a7, s32 a8, s8 a9, s8 a10) __asm__("func_80082B0C");
void *mock_draw_atlas(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5, s32 a6,
                      s32 a7, s32 a8, s8 a9, s8 a10)
{
    s_atlas.calls++;
    s_atlas.a0 = a0;
    s_atlas.a1 = a1;
    s_atlas.a2 = a2;
    s_atlas.a3 = a3;
    s_atlas.a4 = a4;
    s_atlas.a5 = a5;
    s_atlas.a6 = a6;
    s_atlas.a7 = a7;
    s_atlas.a8 = a8;
    s_atlas.a9 = a9;
    s_atlas.a10 = a10;
    return s_atlas.ret;
}

/* ============================================================================
 * 1. 800447F0 - gAllocPeriodicParticleEmitter
 * ============================================================================
 */
struct Unk447F0 {
    u8 unk00[8];
    u16 period;
    s16 phase;
    u8 unk0C[0x18];
    void (*update)();
    u8 unk28[0x62];
    u16 basePeriod;
    u8 unk8C;
    u8 flags8D;
    u8 unk8E[0x1E];
};
extern struct Unk447F0 *gAllocPeriodicParticleEmitter(s32 def) __asm__("func_800447F0");

static struct Unk447F0 s_emit;

static void test_800447F0(void)
{
    struct Unk447F0 *r;

    /* @leaf 800447F0 source 0xAC/tag-1 alloc, setup-fail free-and-return-0, update/period/phase init with 0x30 double */
    LK_FUNC("func_800447F0");

    /* Alloc failure: one 0xAC/tag-1 request, no setup call, no free, returns 0. */
    reset_alloc();
    reset_setup();
    reset_free();
    r = gAllocPeriodicParticleEmitter(0x42);
    FCK_EQ((u32)r, 0U, "alloc failure returns 0");
    FCK_EQ(s_alloc.calls, 1, "alloc called once");
    FCK_EQ(s_alloc.sizes[0], 0xAC, "alloc size 0xAC");
    FCK_EQ(s_alloc.tags[0], 1, "alloc tag 1");
    FCK_EQ(s_setup.calls, 0, "no setup after alloc failure");
    FCK_EQ(s_free.calls, 0, "no free after alloc failure");

    /* Setup failure: block freed once with the allocated pointer, returns 0,
     * and the post-setup field initialisation is skipped entirely. */
    reset_alloc();
    reset_setup();
    reset_free();
    lk_fill(&s_emit, 0xEE, sizeof(s_emit));
    s_emit.basePeriod = 0x1111;
    s_emit.flags8D = 0x30;
    s_alloc.results[0] = &s_emit;
    s_setup.ret[0] = 0;
    r = gAllocPeriodicParticleEmitter(-7);
    FCK_EQ((u32)r, 0U, "setup failure returns 0");
    FCK_EQ(s_setup.calls, 1, "setup called once");
    FCK_EQ((u32)s_setup.last_obj, (u32)&s_emit, "setup receives allocated block");
    FCK_EQ(s_setup.last_def, -7, "setup receives def");
    FCK_EQ(s_free.calls, 1, "setup failure frees the block once");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&s_emit, "freed block is the allocated one");
    FCK_EQ(s_emit.period, (u16)0xEEEE, "setup failure skips period copy");
    FCK_EQ(s_emit.phase, (s16)0xEEEE, "setup failure skips phase clear");
    FCK_EQ((u32)s_emit.update, 0xEEEEEEEEU, "setup failure skips update install");

    /* Success with flags8D == 0: period = basePeriod, phase forced to 0,
     * update = D_80044610 and nothing is freed. */
    reset_alloc();
    reset_setup();
    reset_free();
    lk_fill(&s_emit, 0x11, sizeof(s_emit));
    s_emit.basePeriod = 0x1234;
    s_emit.phase = 0x7777;
    s_emit.flags8D = 0;
    s_alloc.results[0] = &s_emit;
    s_setup.ret[0] = 1;
    r = gAllocPeriodicParticleEmitter(0x42);
    FCK_EQ((u32)r, (u32)&s_emit, "success returns allocated block");
    FCK_EQ(s_alloc.calls, 1, "success makes one alloc");
    FCK_EQ(s_free.calls, 0, "success frees nothing");
    FCK_EQ((u32)s_emit.update, (u32)&mock_periodic_msg, "update = D_80044610");
    FCK_EQ(s_emit.period, 0x1234, "period = basePeriod");
    FCK_EQ(s_emit.phase, 0, "phase cleared to 0");
    FCK_EQ(s_emit.basePeriod, 0x1234, "basePeriod left intact");
    FCK_EQ(s_setup.last_def, 0x42, "success forwards def to setup");
    s_msg.calls = 0;
    s_emit.update();
    FCK_EQ(s_msg.calls, 1, "installed update pointer calls D_80044610");

    /* flags8D & 0x30 doubles the period: 0x100 -> 0x200. */
    reset_alloc();
    reset_setup();
    reset_free();
    lk_fill(&s_emit, 0, sizeof(s_emit));
    s_emit.basePeriod = 0x0100;
    s_emit.flags8D = 0x30;
    s_alloc.results[0] = &s_emit;
    s_setup.ret[0] = 1;
    r = gAllocPeriodicParticleEmitter(0);
    FCK_EQ((u32)r, (u32)&s_emit, "flags 0x30 run returns block");
    FCK_EQ(s_emit.period, 0x0200, "flags 0x30 doubles 0x100 to 0x200");

    /* Only bit 0x10 is inside the 0x30 mask. */
    reset_alloc();
    reset_setup();
    reset_free();
    lk_fill(&s_emit, 0, sizeof(s_emit));
    s_emit.basePeriod = 3;
    s_emit.flags8D = 0x10;
    s_alloc.results[0] = &s_emit;
    s_setup.ret[0] = 1;
    gAllocPeriodicParticleEmitter(0);
    FCK_EQ(s_emit.period, 6, "flags 0x10 doubles 3 to 6");

    /* Bit 0x08 alone is outside the 0x30 mask. */
    reset_alloc();
    reset_setup();
    reset_free();
    lk_fill(&s_emit, 0, sizeof(s_emit));
    s_emit.basePeriod = 6;
    s_emit.flags8D = 0x08;
    s_alloc.results[0] = &s_emit;
    s_setup.ret[0] = 1;
    gAllocPeriodicParticleEmitter(0);
    FCK_EQ(s_emit.period, 6, "flags 0x08 leaves 6 unchanged");

    /* Bit 0x40 is outside the mask. */
    reset_alloc();
    reset_setup();
    reset_free();
    lk_fill(&s_emit, 0, sizeof(s_emit));
    s_emit.basePeriod = 5;
    s_emit.flags8D = 0x40;
    s_alloc.results[0] = &s_emit;
    s_setup.ret[0] = 1;
    gAllocPeriodicParticleEmitter(0);
    FCK_EQ(s_emit.period, 5, "flags 0x40 leaves 5 unchanged");

    /* The doubled value is stored as u16: 0x8001 << 1 keeps the low half. */
    reset_alloc();
    reset_setup();
    reset_free();
    lk_fill(&s_emit, 0, sizeof(s_emit));
    s_emit.basePeriod = 0x8001;
    s_emit.flags8D = 0x20;
    s_alloc.results[0] = &s_emit;
    s_setup.ret[0] = 1;
    gAllocPeriodicParticleEmitter(0);
    FCK_EQ(s_emit.period, 0x0002, "u16 period keeps 0x8001<<1 low half");
}

/* ============================================================================
 * 2. 8004783C - gResetHudTextState
 * ============================================================================
 */
extern void gResetHudTextState(void) __asm__("func_8004783C");

static void test_8004783C(void)
{
    /* @leaf 8004783C source three layer-word clears before the single func_80048EC4 renderer reset */
    LK_FUNC("func_8004783C");

    /* The display mode 2 that func_800486FC compares against, a stale overlay
     * marker and a set frame flag are all cleared before the reset call. */
    reset_textreset();
    gHudMode2 = 2;
    gOverlayTimer = 0x1234;
    gFrameSwitch = 1;
    gResetHudTextState();
    FCK_EQ(gHudMode2, 0, "mode word cleared");
    FCK_EQ(gOverlayTimer, 0, "overlay word cleared");
    FCK_EQ(gFrameSwitch, 0, "frame flag cleared");
    FCK_EQ(s_textreset.calls, 1, "renderer reset called once");
    FCK_EQ(s_textreset.seen_mode2, 0, "mode word clear happens before the call");
    FCK_EQ(s_textreset.seen_overlay, 0, "overlay word clear happens before the call");
    FCK_EQ(s_textreset.seen_frame, 0, "frame flag clear happens before the call");

    /* No guard: a second call clears all three again and resets again. */
    gHudMode2 = -1;
    gOverlayTimer = -1;
    gFrameSwitch = -1;
    gResetHudTextState();
    FCK_EQ(gHudMode2, 0, "second call clears mode word");
    FCK_EQ(gOverlayTimer, 0, "second call clears overlay word");
    FCK_EQ(gFrameSwitch, 0, "second call clears frame flag");
    FCK_EQ(s_textreset.calls, 2, "second call resets renderer again");
    FCK_EQ(s_textreset.seen_mode2, 0, "second call clears before delegating");
    FCK_EQ(s_textreset.seen_overlay, 0, "second call clears overlay before delegating");
    FCK_EQ(s_textreset.seen_frame, 0, "second call clears frame flag before delegating");
}

/* ============================================================================
 * 3. 80048F3C - gFreeFontGlyphBuffers
 * ============================================================================
 */
extern void gFreeFontGlyphBuffers(void) __asm__("func_80048F3C");

static void font_table_reset(void)
{
    lk_fill(gGlyphEntryTable, 0, sizeof(gGlyphEntryTable));
}

static void test_80048F3C(void)
{
    int b0, b1, b2, b3, b4;

    /* @leaf 80048F3C source counted walk of the 0x11C-byte font records freeing only entry->glyphs */
    LK_FUNC("func_80048F3C");

    /* Count 0: nothing is freed even though every record holds a block. */
    font_table_reset();
    reset_free();
    gGlyphEntryTable[0].glyphs = &b0;
    gGlyphEntryTable[1].glyphs = &b1;
    gFontBankCount = 0;
    gFreeFontGlyphBuffers();
    FCK_EQ(s_free.calls, 0, "count 0 frees nothing");
    FCK_EQ((u32)gGlyphEntryTable[0].glyphs, (u32)&b0, "count 0 keeps record 0 pointer");
    FCK_EQ((u32)gGlyphEntryTable[1].glyphs, (u32)&b1, "count 0 keeps record 1 pointer");
    FCK_EQ(gFontBankCount, 0, "count 0 stays 0");

    /* Count 3: exactly records 0..2 are freed, in index order, once each;
     * record 3 is outside the count and must be skipped. */
    font_table_reset();
    reset_free();
    gGlyphEntryTable[0].glyphs = &b0;
    gGlyphEntryTable[1].glyphs = &b1;
    gGlyphEntryTable[2].glyphs = &b2;
    gGlyphEntryTable[3].glyphs = &b3;
    gGlyphEntryTable[0].unk00[0] = 0xA5;
    gGlyphEntryTable[2].unk1C[0xFF] = 0x5A;
    gFontBankCount = 3;
    gFreeFontGlyphBuffers();
    FCK_EQ(s_free.calls, 3, "count 3 frees exactly 3 blocks");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&b0, "free 0 is record 0 glyphs");
    FCK_EQ((u32)s_free.ptrs[1], (u32)&b1, "free 1 is record 1 glyphs");
    FCK_EQ((u32)s_free.ptrs[2], (u32)&b2, "free 2 is record 2 glyphs");
    FCK_EQ(gFontBankCount, 3, "count is not reset by the free walk");
    FCK_EQ((u32)gGlyphEntryTable[0].glyphs, (u32)&b0, "record 0 pointer not cleared");
    FCK_EQ((u32)gGlyphEntryTable[2].glyphs, (u32)&b2, "record 2 pointer not cleared");
    FCK_EQ((u32)gGlyphEntryTable[3].glyphs, (u32)&b3, "record 3 pointer never freed");
    FCK_EQ(gGlyphEntryTable[0].unk00[0], 0xA5, "record 0 header byte untouched");
    FCK_EQ(gGlyphEntryTable[2].unk1C[0xFF], 0x5A, "record 2 tail byte untouched");

    /* Count 1: only record 0, one free. */
    font_table_reset();
    reset_free();
    gGlyphEntryTable[0].glyphs = &b0;
    gGlyphEntryTable[1].glyphs = &b1;
    gFontBankCount = 1;
    gFreeFontGlyphBuffers();
    FCK_EQ(s_free.calls, 1, "count 1 frees one block");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&b0, "count 1 frees record 0");

    /* Identical pointers in two records are freed twice: no dedupe. */
    font_table_reset();
    reset_free();
    gGlyphEntryTable[0].glyphs = &b4;
    gGlyphEntryTable[1].glyphs = &b4;
    gFontBankCount = 2;
    gFreeFontGlyphBuffers();
    FCK_EQ(s_free.calls, 2, "shared pointer freed twice");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&b4, "shared pointer free 0");
    FCK_EQ((u32)s_free.ptrs[1], (u32)&b4, "shared pointer free 1");

    /* Negative count: the loop body never runs. */
    font_table_reset();
    reset_free();
    gGlyphEntryTable[0].glyphs = &b0;
    gFontBankCount = -1;
    gFreeFontGlyphBuffers();
    FCK_EQ(s_free.calls, 0, "negative count frees nothing");
    FCK_EQ(gFontBankCount, -1, "negative count left unchanged");

    /* A null glyphs slot is passed to the free callee unguarded. */
    font_table_reset();
    reset_free();
    gGlyphEntryTable[0].glyphs = 0;
    gFontBankCount = 1;
    gFreeFontGlyphBuffers();
    FCK_EQ(s_free.calls, 1, "null glyphs still makes one free call");
    FCK_EQ((u32)s_free.ptrs[0], 0U, "free callee received null");
}

/* ============================================================================
 * 4. 80047868 - gFreeAllFontGlyphs (runs the real 80048F3C)
 * ============================================================================
 */
extern void gFreeAllFontGlyphs(void) __asm__("func_80047868");

static void test_80047868(void)
{
    int b0, b1, b2;

    /* @leaf 80047868 source single delegation to the real 80048F3C with no count or pointer change of its own */
    LK_FUNC("func_80047868");

    /* Count 2: the wrapper produces exactly the two record frees of the real
     * walk, leaves the count and the pointers alone, skips record 2. */
    font_table_reset();
    reset_free();
    gGlyphEntryTable[0].glyphs = &b0;
    gGlyphEntryTable[1].glyphs = &b1;
    gGlyphEntryTable[2].glyphs = &b2;
    gGlyphEntryTable[0].unk00[0] = 0x3C;
    gFontBankCount = 2;
    gFreeAllFontGlyphs();
    FCK_EQ(s_free.calls, 2, "count 2 frees two blocks");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&b0, "first free is record 0");
    FCK_EQ((u32)s_free.ptrs[1], (u32)&b1, "second free is record 1");
    FCK_EQ(gFontBankCount, 2, "wrapper leaves font count non-zero");
    FCK_EQ((u32)gGlyphEntryTable[0].glyphs, (u32)&b0, "wrapper does not clear record 0 pointer");
    FCK_EQ((u32)gGlyphEntryTable[2].glyphs, (u32)&b2, "record 2 outside count untouched");
    FCK_EQ(gGlyphEntryTable[0].unk00[0], 0x3C, "wrapper touches no record byte");

    /* Count 0: delegation happens but frees nothing. */
    font_table_reset();
    reset_free();
    gGlyphEntryTable[0].glyphs = &b0;
    gFontBankCount = 0;
    gFreeAllFontGlyphs();
    FCK_EQ(s_free.calls, 0, "count 0 wrapper frees nothing");
    FCK_EQ(gFontBankCount, 0, "count 0 stays 0");

    /* Count 1: one free for record 0 only. */
    font_table_reset();
    reset_free();
    gGlyphEntryTable[0].glyphs = &b0;
    gGlyphEntryTable[1].glyphs = &b1;
    gFontBankCount = 1;
    gFreeAllFontGlyphs();
    FCK_EQ(s_free.calls, 1, "count 1 frees one block");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&b0, "count 1 frees record 0 only");
    FCK_EQ(gFontBankCount, 1, "count 1 left non-zero");
}

/* ============================================================================
 * 5. 80082AB0 - gEmitAtlasSpriteSimple
 * ============================================================================
 */
extern void *gEmitAtlasSpriteSimple(s32 arg0, s32 arg1, s32 arg2, s32 arg3,
                                    s32 arg4, s32 arg5, s32 arg6, s8 arg7,
                                    s8 arg8) __asm__("func_80082AB0");

static void test_80082AB0(void)
{
    void *r;

    /* @leaf 80082AB0 source nine-argument forward to 80082B0C with arg6 repeated into slots 7..9 and result passthrough */
    LK_FUNC("func_80082AB0");

    /* Distinct arguments: slots 0..6 forwarded unchanged, callee slots 7,8,9
     * all receive arg6, byte overrides land in the last two slots and the
     * callee result is returned unchanged. */
    reset_atlas((void *)0xCAFE1234);
    r = gEmitAtlasSpriteSimple(11, 22, 33, 44, 55, 66, 77, 0x12, 0x7F);
    FCK_EQ(s_atlas.calls, 1, "one callee call");
    FCK_EQ((u32)r, 0xCAFE1234U, "callee result returned");
    FCK_EQ(s_atlas.a0, 11, "arg0 forwarded");
    FCK_EQ(s_atlas.a1, 22, "arg1 forwarded");
    FCK_EQ(s_atlas.a2, 33, "arg2 forwarded");
    FCK_EQ(s_atlas.a3, 44, "arg3 forwarded");
    FCK_EQ(s_atlas.a4, 55, "arg4 forwarded");
    FCK_EQ(s_atlas.a5, 66, "arg5 forwarded");
    FCK_EQ(s_atlas.a6, 77, "arg6 forwarded");
    FCK_EQ(s_atlas.a7, 77, "slot 7 repeats arg6");
    FCK_EQ(s_atlas.a8, 77, "slot 8 repeats arg6");
    FCK_EQ(s_atlas.a9, 0x12, "arg7 forwarded to slot 9");
    FCK_EQ(s_atlas.a10, 0x7F, "arg8 forwarded to slot 10");

    /* The repeated slot tracks arg6 instead of a fixed constant, negative
     * byte overrides survive, and a null callee result is passed through. */
    reset_atlas(0);
    r = gEmitAtlasSpriteSimple(1, 2, 3, 4, 5, 6, 0xFF, -1, -128);
    FCK_EQ(s_atlas.calls, 1, "second call reaches callee once");
    FCK_EQ((u32)r, 0U, "null callee result returned as 0");
    FCK_EQ(s_atlas.a6, 0xFF, "arg6 0xFF forwarded");
    FCK_EQ(s_atlas.a7, 0xFF, "slot 7 repeats 0xFF");
    FCK_EQ(s_atlas.a8, 0xFF, "slot 8 repeats 0xFF");
    FCK_EQ(s_atlas.a9, -1, "arg7 -1 forwarded");
    FCK_EQ(s_atlas.a10, -128, "arg8 -128 forwarded");

    /* Negative arg6 is repeated sign-intact. */
    reset_atlas((void *)0x1);
    gEmitAtlasSpriteSimple(0, 0, 0, 0, 0, 0, -3, 0, 0);
    FCK_EQ(s_atlas.a6, -3, "arg6 -3 forwarded");
    FCK_EQ(s_atlas.a7, -3, "slot 7 repeats -3");
    FCK_EQ(s_atlas.a8, -3, "slot 8 repeats -3");
    FCK_EQ(s_atlas.a9, 0, "arg7 0 forwarded to slot 9");
    FCK_EQ(s_atlas.a10, 0, "arg8 0 forwarded to slot 10");
}

int main(void)
{
    test_800447F0();
    test_8004783C();
    test_80048F3C();
    test_80047868();
    test_80082AB0();
    return lk_done("leaf_hud_r3");
}
