/* HUD panel/flow hooks, text renderer setup, script timer post-processing and
 * the map-light object pool (init, publish, reap, place, spawn) - single-object
 * units from src/game/ exercised on synthetic RAM under qemu-mipsel.
 *
 * Every fixture a check depends on is written before the call; the harness
 * defines every D_ label itself and never relies on retail initial memory.
 * Callees are stand-ins bound to their retail label, so a wrong binding is a
 * link error. A stand-in only records its arguments and returns a value the
 * test configured - it never invents behaviour. Three boundaries, documented
 * so the checks cannot pretend more than they prove:
 *
 *   - units that are themselves under test are linked for real, so several
 *     checks intentionally observe them through their own callees:
 *     800486FC -> 800487CC -> (stub) 80048790 and 800486FC -> 800485AC,
 *     80049100 -> 80048F98, 80049C38 -> 80049980 -> (identity) 800188D0 /
 *     8001AF84 and 80049C38 -> 800497A8 (the exhaustion-retry sweep runs the
 *     real reaper over the fixture grid, which is how the retry path is
 *     observed at all).
 *   - func_800188D0 (rotation matrix builder) and func_8001AF84 (point
 *     transform) are identity stand-ins: 80049980 prefills the matrix words
 *     [9..11] and constructs the five diamond points itself, and that
 *     construction is the property under test here. The matrix stand-in
 *     records the prefilled words, the transform stand-in copies the five
 *     points verbatim so the node's points hold exactly the constructed
 *     diamond.
 *   - Expected values are hand-worked arithmetic from the decompiled bodies
 *     (shifts, masks, fixed-point divides, struct offsets) as source
 *     evidence: literal constants, byte layouts on little-endian MIPS, and
 *     per-case arithmetic spelled in the trailing comment.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* --------------------------------------------------------------- recorder */

#define LE_CAP_MAX 64

#define LE_PLAY49260  1   /* func_80049260 pending session sfx          */
#define LE_RESOLVE    2   /* func_80029748 resolve name buffer         */
#define LE_ADDPAL     3   /* func_80048978 add palette colour          */
#define LE_LOOKUP     4   /* func_800489C8 lookup glyph                */
#define LE_PREP       5   /* func_80019A74 prepare resource             */
#define LE_VIEW1      6   /* func_80019CA0 tagged view 0x7446554E      */
#define LE_VIEW2      7   /* func_80019C38 tagged view 0x74455854      */
#define LE_RSTATE     8   /* func_80019AA8 resource state              */
#define LE_MAINT      9   /* func_80019D00 maintenance                */
#define LE_FREE      10   /* func_800181D4 free block                 */
#define LE_FREEPOOL  11   /* func_80019584 free fixed pool            */
#define LE_MAKEPOOL  12   /* func_800194DC create fixed pool          */
#define LE_ALLOC     13   /* func_800195A4 allocate node from pool    */
#define LE_RET       14   /* func_800195D0 return node to pool       */
#define LE_RNG       15   /* func_80019668 random value              */
#define LE_MATRIX    16   /* func_800188D0 build rotation matrix      */
#define LE_TRANSFORM 17   /* func_8001AF84 transform points          */
#define LE_MASK      18   /* func_80083764 subcell coverage mask      */
#define LE_CHECKADV  19   /* func_80048790 hud layout advance check   */
#define LE_SURFACE   20   /* func_80067684 surface height query       */

struct LeCall {
    int id;
    s32 a0, a1, a2, a3, a4, a5, a6, a7;
};

static struct LeCall le_calls[LE_CAP_MAX];
static int le_calls_n;

/* Sequence stand-in returns for func_80048790, consumed left to right. */
static u8 le_check_seq[8];
static int le_check_seq_n;
static u8 le_check_default;

/* Glyph records queued for func_800489C8, consumed left to right. */
struct LeGlyph;
static struct LeGlyph *le_glyph_queue[4];
static int le_glyph_queue_n;

static void le_rec(int id, s32 a0, s32 a1, s32 a2, s32 a3,
                   s32 a4, s32 a5, s32 a6, s32 a7)
{
    if (le_calls_n >= LE_CAP_MAX) return;   /* overflow is a test bug */
    le_calls[le_calls_n].id = id;
    le_calls[le_calls_n].a0 = a0;
    le_calls[le_calls_n].a1 = a1;
    le_calls[le_calls_n].a2 = a2;
    le_calls[le_calls_n].a3 = a3;
    le_calls[le_calls_n].a4 = a4;
    le_calls[le_calls_n].a5 = a5;
    le_calls[le_calls_n].a6 = a6;
    le_calls[le_calls_n].a7 = a7;
    le_calls_n++;
}

static void le_reset(void)
{
    int i;
    le_calls_n = 0;
    for (i = 0; i < LE_CAP_MAX; i++) {
        le_calls[i].id = 0;
        le_calls[i].a0 = le_calls[i].a1 = le_calls[i].a2 = le_calls[i].a3 = 0;
        le_calls[i].a4 = le_calls[i].a5 = le_calls[i].a6 = le_calls[i].a7 = 0;
    }
    for (i = 0; i < 8; i++) {
        le_check_seq[i] = 0;
    }
    le_check_seq_n = 0;
    le_check_default = 0;
    for (i = 0; i < 4; i++) {
        le_glyph_queue[i] = 0;
    }
    le_glyph_queue_n = 0;
}

/* Index of the nth call with this id (-1 when absent). */
static int le_idx(int id, int nth)
{
    int i;
    for (i = 0; i < le_calls_n; i++) {
        if (le_calls[i].id == id) {
            if (nth-- == 0) return i;
        }
    }
    return -1;
}

static int le_count(int id)
{
    int i, n = 0;
    for (i = 0; i < le_calls_n; i++)
        if (le_calls[i].id == id) n++;
    return n;
}

/* Little-endian readers over the byte packet pools. */
static u32 le_w32(const u8 *p, int off)
{
    return (u32)p[off] | ((u32)p[off + 1] << 8)
         | ((u32)p[off + 2] << 16) | ((u32)p[off + 3] << 24);
}

static s32 le_s32(const u8 *p, int off) { return (s32)le_w32(p, off); }

static s32 le_s16(const u8 *p, int off)
{
    return (s16)(p[off] | (p[off + 1] << 8));
}

/* ------------------------------------------------------------------ state */

/* Pointers and scalars the units share. Types follow the strictest reader;
 * units that declare a slot differently see the same four bytes. */
s32 le_frame_switch __asm__("D_8009FAC8");
s32 le_preempt[2] __asm__("D_8009F9B0");
s32 le_players[2] __asm__("D_8009FA0C");
u8 *le_packet_cursor __asm__("D_8009FA90");
u8 *le_packet_limit __asm__("D_8009F9C0");
s32 le_screen_state[2] __asm__("D_8009FA00");
u8 le_portrait_id[8] __asm__("D_8009F9B4");
s32 le_hud_mode2 __asm__("D_8009FAB8");
s32 le_overlay_timer __asm__("D_8009F760");
u16 le_texpage_table[0x15] __asm__("D_800A1940");
u16 le_palette_table[0x15] __asm__("D_800A1970");
u32 le_effect_id[2] __asm__("D_8009FACC");
u16 le_texbank_vram_y __asm__("D_8009F76A");
u16 le_loaded_font __asm__("D_8009F768");
void *le_font __asm__("D_8009F35C");
s32 le_ramp_fill_counter __asm__("D_8009F360");
s32 le_font_bank_count __asm__("D_8009F364");
u16 le_glyph_table[32] __asm__("D_8009AAE0");
s32 le_current_bank __asm__("D_8009F76C");
void *le_script_ticks __asm__("D_8009F770");
s32 le_script_count __asm__("D_8009F774");
void *le_sprite_desc_table __asm__("D_8009F77C");
void *le_light_grid __asm__("D_8009F780");
s32 le_light_pool __asm__("D_8009F784");
s32 le_cleanup_time __asm__("D_8009F794");
s32 le_resource_table_size __asm__("D_8009F788");
void *le_sprite_region_table __asm__("D_8009F778");
s32 le_grid_width __asm__("D_8009F78C");
s32 le_grid_height __asm__("D_8009F790");
s32 le_frame_time[2] __asm__("D_8009FAA4");

/* Node layout shared by 800497A8 / 80049980 / 80049C38 (0x44-byte pool
 * blocks): +0x00 s16 id, +0x02 s16 cell (-1 unlinked), +0x04 s32 key/life,
 * +0x08 flag byte, +0x09..0x0B payload bytes, +0x0C s32 activation tick,
 * +0x10 u16 payload word, +0x12 u16 subcell mask, +0x14 s32 depth,
 * +0x18 five 8-byte points, +0x40 next pointer. */
struct LePoint { s16 x; s16 y; s16 z; s16 pad; };
struct LeLightNode {
    s16 unk00;
    s16 unk02;
    s32 unk04;
    u8 unk08;
    u8 unk09;
    u8 unk0A;
    u8 unk0B;
    s32 unk0C;
    u16 unk10;
    u16 unk12;
    s32 unk14;
    struct LePoint points[5];
    struct LeLightNode *next;
};
static struct LeLightNode *le_grid_cells[8];

/* 80049100/80048F98 glyph record (11 bytes, mirror of the unit's struct Glyph). */
struct LeGlyph {
    u8 b00;
    u8 b01;
    u8 b02;
    u8 b03;
    u8 b04;
    u8 b05;
    u8 b06;
    u8 b07;
    s8 b08;
    u8 b09;
    u8 b0A;
};
static struct LeGlyph le_glyphs[256];

/* 80049420 table: three header words then 20-byte entries from +0xC. */
static s32 le_timer_words[16];
struct LeTimerObj { u32 f0; u8 pad[0x18]; u32 f1C; u32 f20; };
static struct LeTimerObj le_timer_obj;

/* 80049734 resource: data block + two status words. */
struct LeCdcsData { s32 unk00; s32 unk04; s32 count; };
struct LeCdcsResource {
    struct LeCdcsData *data;
    s32 unk04;
    s32 unk08;
};
static struct LeCdcsData le_cdcs_data;
static struct LeCdcsResource le_cdcs_res;

/* 80049C38 list (count + 8-byte id entries) and descriptor object
 * (0x2A bytes, mirror of the unit's struct Unk49C38). */
struct LeIdList { volatile s32 count; struct { u8 pad00[7]; u8 id; } entries[4]; };
static struct LeIdList le_id_list;
struct LeSpawnObj {
    u8 pad00[8];
    u32 unk08;
    u32 unk0C;
    u32 unk10;
    u8 unk14;
    s8 unk15;
    s16 unk16;
    s16 unk18;
    s16 unk1A;
    s16 unk1C;
    s16 unk1E;
    s16 unk20;
    s8 unk22;
    u8 unk23;
    u8 unk24;
    u8 unk25;
    u16 unk26;
    u16 unk28;
};
static struct LeSpawnObj le_spawn_obj;
static struct LeLightNode le_alloc_nodes[4];

/* ------------------------------------------------------------ stand-ins */

static s32 le_resolve_ret;                    /* func_80029748 */
static void *le_data_view;                   /* func_80019CA0 */
static u32 le_view2_ret;                     /* func_80019C38 */
static u8 le_resource_state;                 /* func_80019AA8 */
static s32 le_pool_handle;                   /* func_800194DC */
static void *le_alloc_queue[8];              /* func_800195A4 */
static int le_alloc_queue_n;
static s32 le_rng_value;                     /* func_80019668 */
static u16 le_mask_ret;                      /* func_80083764 */
static s32 le_surface_ret;                   /* func_80067684 */

void le_stub_49260(void) __asm__("func_80049260");
void le_stub_49260(void)
{
    le_rec(LE_PLAY49260, 0, 0, 0, 0, 0, 0, 0, 0);
}

s32 le_stub_29748(u8 *name) __asm__("func_80029748");
s32 le_stub_29748(u8 *name)
{
    le_rec(LE_RESOLVE, (s32)name, 0, 0, 0, 0, 0, 0, 0);
    return le_resolve_ret;
}

u8 le_stub_48790(void) __asm__("func_80048790");
u8 le_stub_48790(void)
{
    u8 r = le_check_default;
    if (le_check_seq_n < 8) {
        r = le_check_seq[le_check_seq_n];
        le_check_seq_n++;
    }
    le_rec(LE_CHECKADV, r, 0, 0, 0, 0, 0, 0, 0);
    return r;
}

void le_stub_48978(s32 blue, s32 green, s32 red) __asm__("func_80048978");
void le_stub_48978(s32 blue, s32 green, s32 red)
{
    le_rec(LE_ADDPAL, blue, green, red, 0, 0, 0, 0, 0);
}

u8 *le_stub_489C8(void *state, u32 offset) __asm__("func_800489C8");
u8 *le_stub_489C8(void *state, u32 offset)
{
    void *r = 0;
    le_rec(LE_LOOKUP, (s32)state, (s32)offset, 0, 0, 0, 0, 0, 0);
    if (le_glyph_queue_n < 4) r = le_glyph_queue[le_glyph_queue_n];
    le_glyph_queue_n++;
    return (u8 *)r;
}

void le_stub_19A74(void *resource) __asm__("func_80019A74");
void le_stub_19A74(void *resource)
{
    le_rec(LE_PREP, (s32)resource, 0, 0, 0, 0, 0, 0, 0);
}

void *le_stub_19CA0(u32 a, u32 b, u32 c, u32 d) __asm__("func_80019CA0");
void *le_stub_19CA0(u32 a, u32 b, u32 c, u32 d)
{
    le_rec(LE_VIEW1, (s32)a, (s32)b, (s32)c, (s32)d, 0, 0, 0, 0);
    return le_data_view;
}

u32 le_stub_19C38(u32 a, u32 b, u32 c, u32 d) __asm__("func_80019C38");
u32 le_stub_19C38(u32 a, u32 b, u32 c, u32 d)
{
    le_rec(LE_VIEW2, (s32)a, (s32)b, (s32)c, (s32)d, 0, 0, 0, 0);
    return le_view2_ret;
}

u8 le_stub_19AA8(void *resource) __asm__("func_80019AA8");
u8 le_stub_19AA8(void *resource)
{
    le_rec(LE_RSTATE, (s32)resource, le_resource_state, 0, 0, 0, 0, 0, 0);
    return le_resource_state;
}

void le_stub_19D00(void *resource) __asm__("func_80019D00");
void le_stub_19D00(void *resource)
{
    le_rec(LE_MAINT, (s32)resource, 0, 0, 0, 0, 0, 0, 0);
}

void le_stub_181D4(void *block) __asm__("func_800181D4");
void le_stub_181D4(void *block)
{
    le_rec(LE_FREE, (s32)block, 0, 0, 0, 0, 0, 0, 0);
}

void le_stub_19584(s32 pool) __asm__("func_80019584");
void le_stub_19584(s32 pool)
{
    le_rec(LE_FREEPOOL, pool, 0, 0, 0, 0, 0, 0, 0);
}

s32 le_stub_194DC(s32 count, s32 size, s32 tag) __asm__("func_800194DC");
s32 le_stub_194DC(s32 count, s32 size, s32 tag)
{
    le_rec(LE_MAKEPOOL, count, size, tag, 0, 0, 0, 0, 0);
    return le_pool_handle;
}

void *le_stub_195A4(void *pool) __asm__("func_800195A4");
void *le_stub_195A4(void *pool)
{
    void *r = 0;
    le_rec(LE_ALLOC, (s32)pool, 0, 0, 0, 0, 0, 0, 0);
    if (le_alloc_queue_n < 8)
        r = le_alloc_queue[le_alloc_queue_n];
    le_alloc_queue_n++;
    return r;
}

void le_stub_195D0(void *pool, void *node) __asm__("func_800195D0");
void le_stub_195D0(void *pool, void *node)
{
    le_rec(LE_RET, (s32)pool, (s32)node, 0, 0, 0, 0, 0, 0);
}

s32 le_stub_19668(void) __asm__("func_80019668");
s32 le_stub_19668(void)
{
    le_rec(LE_RNG, le_rng_value, 0, 0, 0, 0, 0, 0, 0);
    return le_rng_value;
}

void le_stub_188D0(s32 angle, s32 arg7, s32 arg8, s32 *matrix) __asm__("func_800188D0");
void le_stub_188D0(s32 angle, s32 arg7, s32 arg8, s32 *matrix)
{
    /* Identity: 80049980 prefills words [9..11] and the flag-4 path never
     * happens here; the rotation itself has its own contract elsewhere.
     * The prefilled words are recorded so the prefill is still checked. */
    le_rec(LE_MATRIX, angle, arg7, arg8, (s32)matrix,
           matrix[9], matrix[10], matrix[11], 0);
}

void le_stub_1AF84(s32 *matrix, void *source, void *dest, s32 count) __asm__("func_8001AF84");
void le_stub_1AF84(s32 *matrix, void *source, void *dest, s32 count)
{
    /* Identity transform: copies the source points verbatim, so the node's
     * points hold exactly the diamond the unit constructed. */
    u8 *d = (u8 *)dest;
    const u8 *s = (const u8 *)source;
    int i;
    le_rec(LE_TRANSFORM, (s32)matrix, (s32)source, (s32)dest, count, 0, 0, 0, 0);
    for (i = 0; i < count * 8; i++) d[i] = s[i];
}

u16 le_stub_83764(s32 x, s32 y, s32 left, s32 right, s32 top, s32 bottom) __asm__("func_80083764");
u16 le_stub_83764(s32 x, s32 y, s32 left, s32 right, s32 top, s32 bottom)
{
    le_rec(LE_MASK, x, y, left, right, top, bottom, 0, 0);
    return le_mask_ret;
}

s32 le_stub_67684(u8 *arg0, s32 arg1) __asm__("func_80067684");
s32 le_stub_67684(u8 *arg0, s32 arg1)
{
    le_rec(LE_SURFACE, (s32)arg0, arg1, 0, 0, 0, 0, 0, 0);
    return le_surface_ret;
}

/* ------------------------------------------------------- tested prototypes */

void gEmitHudPanelSprite(s32 value) __asm__("func_800485AC");
void gUpdateFrameCloseHud(u8 enable) __asm__("func_800486FC");
void gUpdateHudLayoutMode(s32 input) __asm__("func_800487CC");
void gInitTextRendererState(void) __asm__("func_80048EC4");
s32 gEmitFontGlyphSprite(u8 character, s32 x, s32 y, u16 color) __asm__("func_80048F98");
void gDrawAlignedString(u8 *text, s32 x, s32 y, s32 color) __asm__("func_80049100");
void gLoadScriptTimers(void *obj) __asm__("func_80049420");
void gReleaseMapLightData(void) __asm__("func_8004963C");
void gInitMapLighting(void) __asm__("func_80049690");
void gLoadCdcsResource(void *resource) __asm__("func_80049734");
void gReapExpiredLightNodes(void) __asm__("func_800497A8");
void gUpdateDynamicLightNode(struct LeLightNode *node, s32 x, s32 z, s32 y,
                             s32 width, s32 depth, s32 angle, s32 arg7,
                             s32 arg8) __asm__("func_80049980");
s32 gSpawnDynamicLightNode(void *object, s32 arg1) __asm__("func_80049C38");

/* ---------------------------------------------------------------- tests */

static u8 le_pool[0x100];
static u8 le_cf_pool[0x100];

static void test_hud_panel_sprite(void)
{
    u8 *cursor;

    /* @leaf 800485AC source src/game/800485AC.c: gates, two 0x1C packets (texpage 0xE100011F + SPRT 0x64 0x808080), x/w/u variants per player count and register arg */
    LK_FUNC("func_800485AC");
    le_reset();
    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    cursor = le_pool + 0x10;
    le_frame_switch = 0;
    le_preempt[0] = 0;
    le_players[0] = 1;
    le_packet_cursor = cursor;
    le_packet_limit = le_pool + sizeof(le_pool);
    gEmitHudPanelSprite(0);
    FCK_CHECK(le_packet_cursor == cursor, "zero frame switch returns at once");
    FCK_CHECK(lk_all(le_pool, 0, sizeof(le_pool), 0x5A),
              "no packet is written");

    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    le_frame_switch = 2;
    le_preempt[0] = 1;
    gEmitHudPanelSprite(0);
    FCK_CHECK(le_packet_cursor == cursor,
              "preempted frame returns at once");
    FCK_CHECK(lk_all(le_pool, 0, sizeof(le_pool), 0x5A),
              "preempted frame writes nothing");

    /* Single player: x 0xF4, u 0, w 0x40, two 0x1C packets. Observed as raw
     * little-endian bytes: DrawTPage packs addr(0) into bits 0..23 and len(1)
     * into bits 24..31 -> word 0x01000000, with the code word 0xE100011F at
     * +4; SPRT packs len 4 -> 0x04000000, colour 0x80 in bytes 4..6, code
     * 0x64 in byte 7, x/y at +8, u/v at +0xC/0xD, w/h at +0x10/+0x12. */
    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    le_preempt[0] = 0;
    le_players[0] = 1;
    le_frame_switch = 2;
    le_packet_cursor = cursor;
    gEmitHudPanelSprite(0);
    FCK_CHECK(le_packet_cursor == cursor + 0x38,
              "two 0x1C packets advance the cursor by 0x38");
    FCK_EQ(le_w32(le_pool, 0x10), 0x01000000, "texpage addr 0 / len 1 word");
    FCK_EQ(le_w32(le_pool, 0x14), 0xE100011F, "texpage code word");
    FCK_EQ(le_w32(le_pool, 0x2C), 0x04000000, "sprite addr 0 / len 4 word");
    FCK_CHECK(le_pool[0x30] == 0x80 && le_pool[0x31] == 0x80 &&
              le_pool[0x32] == 0x80 && le_pool[0x33] == 0x64,
              "sprite colour 0x808080 and code 0x64");
    FCK_CHECK(le_pool[0x34] == 0xF4 && le_pool[0x35] == 0x00 &&
              le_pool[0x36] == 0xB2 && le_pool[0x37] == 0x00,
              "sprite sits at x 0xF4, y 0xB2");
    FCK_CHECK(le_pool[0x38] == 0x00 && le_pool[0x39] == 0xCC,
              "sprite samples u 0, v 0xCC");
    FCK_EQ(le_s16(le_pool, 0x3C), 0x40, "sprite is 0x40 wide");
    FCK_EQ(le_s16(le_pool, 0x3E), 0x30, "sprite is 0x30 tall");
    FCK_CHECK(lk_all(le_pool, 0x18, 0x2C, 0x5A) &&
              le_pool[0x3A] == 0x5A && le_pool[0x3B] == 0x5A &&
              lk_all(le_pool, 0x40, 0x48, 0x5A),
              "packet pad bytes untouched");

    /* Two players, arg 0: x = (0 == 0) << 7 = 0x80, u 0, w 0x20. */
    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    le_packet_cursor = cursor;
    le_players[0] = 2;
    gEmitHudPanelSprite(0);
    FCK_CHECK(le_pool[0x34] == 0x80 && le_pool[0x35] == 0x00,
              "arg 0 in two-player mode draws at x 0x80");
    FCK_CHECK(le_pool[0x38] == 0x00, "arg 0 keeps u at 0");
    FCK_EQ(le_s16(le_pool, 0x3C), 0x20, "two-player mode draws 0x20 wide");

    /* Two players, arg 1: x = (1 == 0) << 7 = 0, u 0x20, w 0x20. */
    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    le_packet_cursor = cursor;
    gEmitHudPanelSprite(1);
    FCK_CHECK(le_pool[0x34] == 0x00 && le_pool[0x35] == 0x00,
              "arg 1 in two-player mode draws at x 0");
    FCK_CHECK(le_pool[0x38] == 0x20, "arg 1 samples u 0x20");
    FCK_EQ(le_s16(le_pool, 0x3C), 0x20, "arg 1 keeps the two-player width");

    /* Full pool: both packets are written in place at the cursor and the
     * cursor does not move - the sprite packet reuses the same address, so
     * it overwrites the texpage: +4..+6 colour 0x80, +7 code 0x64. */
    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    le_players[0] = 1;
    le_packet_cursor = cursor;
    le_packet_limit = cursor;
    gEmitHudPanelSprite(0);
    FCK_CHECK(le_packet_cursor == cursor, "full pool leaves the cursor alone");
    FCK_EQ(le_w32(le_pool, 0x10), 0x04000000,
           "sprite len-4 word overwrites the texpage word in place");
    FCK_EQ(le_w32(le_pool, 0x14), 0x64808080,
           "sprite colour and code overwrite the texpage code word in place");
    FCK_CHECK(le_pool[0x18] == 0xF4 && le_pool[0x1A] == 0xB2,
              "in-place sprite still carries x 0xF4, y 0xB2");
}

static void test_frame_close_hud(void)
{
    /* @leaf 800486FC source src/game/800486FC.c: mode-4 session sound first, frame switch from the name resolver, always a layout refresh, status reset only in hud mode 2 with enable and single player */
    LK_FUNC("func_800486FC");
    le_reset();
    lk_fill(le_cf_pool, 0x5A, sizeof(le_cf_pool));
    lk_fill(le_portrait_id, 0x77, sizeof(le_portrait_id));
    le_check_default = 0;                /* the layout check never demands a cycle */
    le_resolve_ret = 0x2A;
    le_screen_state[0] = 3;
    le_hud_mode2 = 1;
    le_overlay_timer = 0x2A;
    le_frame_switch = 7;
    le_players[0] = 1;
    le_preempt[0] = 0;
    le_packet_cursor = le_cf_pool;
    le_packet_limit = le_cf_pool + sizeof(le_cf_pool);
    gUpdateFrameCloseHud(0);
    FCK_EQ(le_frame_switch, 0x2A, "frame switch takes the resolver value");
    FCK_EQ(le_count(LE_PLAY49260), 0, "mode 3 does not play the session sound");
    FCK_EQ(le_hud_mode2, 1, "unchanged hud mode without an overlay edge");
    FCK_CHECK(le_packet_cursor == le_cf_pool, "no sprite outside mode 2");
    FCK_EQ(le_overlay_timer, 0x2A, "layout refresh caches the frame switch");
    FCK_CHECK(le_idx(LE_RESOLVE, 0) >= 0 &&
              le_calls[le_idx(LE_RESOLVE, 0)].a0 == (s32)le_portrait_id,
              "resolver receives the portrait id buffer");
    FCK_EQ(le_count(LE_CHECKADV), 1, "the layout refresh runs its check once");

    le_screen_state[0] = 4;
    gUpdateFrameCloseHud(0);
    FCK_EQ(le_count(LE_PLAY49260), 1, "mode 4 plays the session sound once");
    FCK_EQ(le_hud_mode2, 1, "still mode 1 with a steady overlay");

    /* A rising overlay edge through the real 800487CC forces mode 2; with
     * enable set and a single player the real 800485AC emits the two-packet
     * status frame. */
    lk_fill(le_cf_pool, 0x5A, sizeof(le_cf_pool));
    le_screen_state[0] = 3;
    le_hud_mode2 = 1;
    le_overlay_timer = 3;
    le_packet_cursor = le_cf_pool;
    gUpdateFrameCloseHud(1);
    FCK_EQ(le_hud_mode2, 2, "rising overlay edge forces hud mode 2");
    FCK_CHECK(le_packet_cursor == le_cf_pool + 0x38,
              "mode 2 with enable emits the status frame");
    FCK_EQ(le_overlay_timer, 0x2A, "the edge timer now tracks the resolver");

    lk_fill(le_cf_pool, 0x5A, sizeof(le_cf_pool));
    le_hud_mode2 = 1;
    le_overlay_timer = 3;
    le_packet_cursor = le_cf_pool;
    gUpdateFrameCloseHud(0);
    FCK_EQ(le_hud_mode2, 2, "edge forces mode 2 regardless of enable");
    FCK_CHECK(le_packet_cursor == le_cf_pool,
              "enable 0 suppresses the status reset");

    lk_fill(le_cf_pool, 0x5A, sizeof(le_cf_pool));
    le_hud_mode2 = 1;
    le_overlay_timer = 3;
    le_players[0] = 2;
    le_packet_cursor = le_cf_pool;
    gUpdateFrameCloseHud(1);
    FCK_EQ(le_hud_mode2, 2, "edge forces mode 2 in two-player mode too");
    FCK_CHECK(le_packet_cursor == le_cf_pool,
              "two-player mode suppresses the status reset");
}

static void test_hud_layout_mode(void)
{
    /* @leaf 800487CC source src/game/800487CC.c: mode-5 guard suppresses the cycle request, a rising overlay edge forces mode 2, otherwise a do-while advances mode 2 mod 3 */
    LK_FUNC("func_800487CC");
    le_reset();
    le_screen_state[0] = 5;
    le_hud_mode2 = 1;
    le_overlay_timer = 0x11;
    le_frame_switch = 0;
    le_check_seq[0] = 0;
    gUpdateHudLayoutMode(1);
    FCK_EQ(le_hud_mode2, 1, "mode-5 guard suppresses the cycle request");
    FCK_EQ(le_overlay_timer, 0, "overlay timer caches the zero frame switch");

    le_screen_state[0] = 4;
    le_hud_mode2 = 0;
    le_overlay_timer = 5;
    le_check_seq[0] = 0;
    gUpdateHudLayoutMode(1);
    FCK_EQ(le_hud_mode2, 1, "a request outside mode 5 advances 0 to 1");
    FCK_EQ(le_overlay_timer, 0, "overlay timer caches the zero frame switch");

    le_reset();
    le_hud_mode2 = 1;
    le_overlay_timer = 3;
    le_frame_switch = 7;
    gUpdateHudLayoutMode(0);
    FCK_EQ(le_hud_mode2, 2, "rising edge forces mode 2");
    FCK_EQ(le_overlay_timer, 7, "overlay timer caches the frame switch");
    FCK_EQ(le_count(LE_CHECKADV), 1,
           "a forced mode does not run a cycle at all");

    /* do-while with a rejecting check: a request from mode 2 wraps
     * 2 -> 3 -> 0 and stops. */
    le_reset();
    le_hud_mode2 = 2;
    le_overlay_timer = 0;
    le_frame_switch = 0;
    le_check_seq[0] = 0;
    gUpdateHudLayoutMode(1);
    FCK_EQ(le_hud_mode2, 0, "a requested cycle wraps mode 2 back to 0");
    FCK_EQ(le_count(LE_CHECKADV), 2,
           "the outer check plus one in-loop check stop the cycle");

    /* A demanding check alone cycles once: outer 1, in-loop 0. */
    le_reset();
    le_hud_mode2 = 0;
    le_overlay_timer = 0;
    le_frame_switch = 0;
    le_check_seq[0] = 1;
    le_check_seq[1] = 0;
    gUpdateHudLayoutMode(0);
    FCK_EQ(le_hud_mode2, 1, "a demanding check drives the cycle on its own");

    /* Steady state: rejecting check, no request - nothing moves. */
    le_hud_mode2 = 1;
    le_overlay_timer = 0;
    le_frame_switch = 0;
    le_check_seq[0] = 0;
    gUpdateHudLayoutMode(0);
    FCK_EQ(le_hud_mode2, 1, "rejecting check without request changes nothing");
}

static void test_text_init(void)
{
    int i;
    int bad;

    /* @leaf 80048EC4 source src/game/80048EC4.c: VRAM Y 1, font/count/effect cleared, twenty texpage/palette words zeroed, white 0x1F1F1F registered; the colour arguments are all equal so the blue/green/red order is not observed here */
    LK_FUNC("func_80048EC4");
    le_reset();
    lk_fill(le_texpage_table, 0x3C, sizeof(le_texpage_table));
    lk_fill(le_palette_table, 0x5C, sizeof(le_palette_table));
    le_texpage_table[0] = 0x1234;
    le_palette_table[0] = 0x5678;
    le_texbank_vram_y = 0x77;
    le_loaded_font = 0x88;
    le_font = (void *)0x9999;
    le_ramp_fill_counter = 0x1234;
    le_font_bank_count = 0x5678;
    le_effect_id[0] = 0xABCD;
    gInitTextRendererState();
    FCK_EQ(le_texbank_vram_y, 1, "texture VRAM Y resets to 1");
    FCK_EQ(le_loaded_font, 0, "loaded font cleared");
    FCK_EQ((s32)le_font, 0, "active font pointer cleared");
    FCK_EQ(le_ramp_fill_counter, 0, "ramp fill counter cleared");
    FCK_EQ(le_font_bank_count, 0, "font bank count cleared");
    FCK_EQ((s32)le_effect_id[0], 0, "effect id word cleared");
    bad = 0;
    for (i = 0; i < 0x14; i++) {
        if (le_texpage_table[i] != 0 || le_palette_table[i] != 0) {
            bad = 1;
        }
    }
    FCK_CHECK(bad == 0, "all twenty texpage and palette words cleared");
    FCK_CHECK(le_texpage_table[0x14] == 0x3C3C &&
              le_palette_table[0x14] == 0x5C5C,
              "clearing stops at the twentieth entry");
    FCK_EQ(le_count(LE_ADDPAL), 1, "one palette colour registered");
    FCK_CHECK(le_idx(LE_ADDPAL, 0) >= 0 &&
              le_calls[le_idx(LE_ADDPAL, 0)].a0 == 0x1F &&
              le_calls[le_idx(LE_ADDPAL, 0)].a1 == 0x1F &&
              le_calls[le_idx(LE_ADDPAL, 0)].a2 == 0x1F,
              "the registered colour is white 0x1F/0x1F/0x1F");
}

static void test_glyph_sprite(void)
{
    struct LeGlyph *g;

    /* @leaf 80048F98 source src/game/80048F98.c: SPRT len 4 / code 100, RGB split (>>8)&0x7C, (>>3)&0x7C, (c&0x1F)<<2 of the palette word, u/v from the glyph, x/y biased by the signed glyph offsets, w0E is the loaded font, returns the advance byte */
    LK_FUNC("func_80048F98");
    le_reset();
    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    lk_fill((u8 *)le_glyphs, 0, sizeof(le_glyphs));
    g = &le_glyphs[65];
    g->b02 = 0x34; g->b03 = 0x78;       /* w10 / w12 sources */
    g->b04 = 0x23;                       /* sprite u source   */
    g->b06 = 9;                          /* sprite v source   */
    g->b08 = 0x0E;                       /* advance return    */
    g->b09 = (u8)-7;                     /* x bias            */
    g->b0A = 9;                          /* y bias            */
    le_glyph_queue[0] = g;
    le_glyph_queue_n = 0;
    le_font = (void *)0x1234;
    le_glyph_table[2] = 0x7FFF;          /* white             */
    le_loaded_font = 4;
    le_current_bank = 3;
    le_palette_table[3] = 0x002A;        /* byte 6 = 0x2A     */
    le_packet_cursor = le_pool + 0x10;
    le_packet_limit = le_pool + sizeof(le_pool);
    FCK_EQ(gEmitFontGlyphSprite(65, 100, 20, 2), 0x0E,
           "the glyph advance byte is returned");
    FCK_CHECK(le_packet_cursor == le_pool + 0x2C,
              "cursor moves by one 0x1C packet");
    FCK_CHECK(le_idx(LE_LOOKUP, 0) >= 0 &&
              le_calls[le_idx(LE_LOOKUP, 0)].a0 == 0x1234 &&
              le_calls[le_idx(LE_LOOKUP, 0)].a1 == 65,
              "glyph lookup receives the font state and the character");
    /* word0: addr 0 in bytes 0..2, len 4 in byte 3 -> 0x04000000 */
    FCK_EQ(le_w32(le_pool, 0x10), 0x04000000, "sprite addr 0 / len 4 word");
    /* 0x7FFF: b04 = 0x7F & 0x7C = 0x7C, b05 = 0xFFF >> 3 -> 0x7C,
     * b06 = (0x1F) << 2 = 0x7C - white through and through. */
    FCK_CHECK(le_pool[0x14] == 0x7C && le_pool[0x15] == 0x7C &&
              le_pool[0x16] == 0x7C,
              "white palette word decomposes into equal RGB bytes");
    FCK_CHECK(le_pool[0x17] == 100, "sprite code is 100");
    FCK_EQ(le_s16(le_pool, 0x18), 100 - 7, "x carries the -7 glyph bias");
    FCK_EQ(le_s16(le_pool, 0x1A), 20 + 9, "y carries the +9 glyph bias");
    FCK_CHECK(le_pool[0x1C] == 0x23, "u comes from glyph byte 4");
    FCK_CHECK(le_pool[0x1D] == 9 + 0x2A,
              "v is glyph byte 6 plus the bank's palette byte");
    FCK_EQ(le_s16(le_pool, 0x1E), 4, "texpage word w0E is the loaded font");
    FCK_EQ(le_s16(le_pool, 0x20), 0x34, "w10 copies glyph byte 2");
    FCK_EQ(le_s16(le_pool, 0x22), 0x78, "w12 copies glyph byte 3");

    /* Green 0x03E0: b04 = (0x3E0 >> 8) & 0x7C = 0, b05 = (0x3E0 >> 3) & 0x7C
     * = 0x7C, b06 = (0x3E0 & 0x1F) << 2 = 0. */
    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    le_packet_cursor = le_pool + 0x10;
    le_glyph_queue_n = 0;                /* requeue the same glyph record */
    le_glyph_table[1] = 0x03E0;
    gEmitFontGlyphSprite(66, 0, 0, 1);
    FCK_CHECK(le_pool[0x14] == 0x00 && le_pool[0x15] == 0x7C &&
              le_pool[0x16] == 0x00,
              "green palette word decomposes RGB as 0/0x7C/0");

    /* Blue 0x7C00: b04 = (0x7C00 >> 8) & 0x7C = 0x7C, b05 = 0, b06 = 0. */
    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    le_packet_cursor = le_pool + 0x10;
    le_glyph_queue_n = 0;
    le_glyph_table[0] = 0x7C00;
    gEmitFontGlyphSprite(67, 0, 0, 0);
    FCK_CHECK(le_pool[0x14] == 0x7C && le_pool[0x15] == 0x00 &&
              le_pool[0x16] == 0x00,
              "blue palette word decomposes RGB as 0x7C/0/0");

    /* Full pool: the sprite is still written in place, cursor stays. */
    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    le_glyph_queue[0] = g;
    le_glyph_queue_n = 0;
    le_glyph_table[2] = 0x7FFF;
    le_packet_cursor = le_pool + 0x20;
    le_packet_limit = le_pool + 0x20;
    gEmitFontGlyphSprite(65, 0, 0, 2);
    FCK_CHECK(le_packet_cursor == le_pool + 0x20,
              "full pool leaves the cursor alone");
    FCK_CHECK(le_pool[0x27] == 100, "in-place sprite still carries code 100");
    FCK_CHECK(le_pool[0x24] == 0x7C && le_pool[0x25] == 0x7C,
              "in-place sprite still carries the white split");
}

static void test_draw_string(void)
{
    u8 text[8];
    struct LeGlyph *ga;
    struct LeGlyph *gb;

    /* @leaf 80049100 source src/game/80049100.c: no loaded font draws nothing, one texpage word table[bank] & 0x9FF | 0xE1000000 first, per-character advances accumulate on x, chars 10..0x25 are control codes that re-colour instead of drawing */
    LK_FUNC("func_80049100");
    le_reset();
    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    lk_fill((u8 *)le_glyphs, 0, sizeof(le_glyphs));
    ga = &le_glyphs[65];                  /* 'A' */
    ga->b02 = 0x34; ga->b03 = 0x78;
    ga->b04 = 0x23; ga->b06 = 9;
    ga->b08 = 10; ga->b09 = (u8)-7; ga->b0A = 9;
    gb = &le_glyphs[66];                  /* 'B' */
    gb->b02 = 0x34; gb->b03 = 0x78;
    gb->b04 = 0x11; gb->b06 = 3;
    gb->b08 = 12; gb->b09 = (u8)-3; gb->b0A = 4;
    le_glyph_table[2] = 0x7FFF;           /* draw colour      */
    le_glyph_table[6] = 0x7C00;           /* recolour target  */
    le_palette_table[3] = 0x002A;
    le_loaded_font = 0;
    le_packet_cursor = le_pool + 0x10;
    le_packet_limit = le_pool + sizeof(le_pool);
    text[0] = 'A'; text[1] = 0;
    gDrawAlignedString(text, 100, 20, 2);
    FCK_CHECK(le_packet_cursor == le_pool + 0x10,
              "no loaded font draws nothing");

    le_loaded_font = 4;
    le_current_bank = 1;
    le_texpage_table[1] = 0x09AB;         /* bank 1 texpage word */
    le_glyph_queue[0] = ga;
    le_glyph_queue[1] = gb;
    le_glyph_queue_n = 0;
    text[0] = 'A'; text[1] = 'B'; text[2] = 0;
    gDrawAlignedString(text, 100, 20, 2);
    FCK_CHECK(le_packet_cursor == le_pool + 0x10 + 3 * 0x1C,
              "texpage plus two glyph packets");
    /* 0x09AB & 0x9FF = 0x09AB; | 0xE1000000 -> 0xE10009AB at packet +4. */
    FCK_EQ(le_w32(le_pool, 0x10), 0x01000000, "texpage addr/len word");
    FCK_EQ(le_w32(le_pool, 0x14), 0xE10009AB,
           "texpage code takes bank 1 table entry");
    FCK_CHECK(le_idx(LE_LOOKUP, 0) >= 0 &&
              le_calls[le_idx(LE_LOOKUP, 0)].a1 == 65 &&
              le_calls[le_idx(LE_LOOKUP, 1)].a1 == 66,
              "both characters go through the glyph lookup");
    FCK_EQ(le_s16(le_pool, 0x34), 100 - 7, "first glyph draws at x minus 7");
    FCK_EQ(le_s16(le_pool, 0x50), 100 + 10 - 3,
           "second glyph draws at x plus the first advance, minus its own bias");
    FCK_EQ(le_s16(le_pool, 0x52), 20 + 4, "second glyph carries its own y bias");
    FCK_EQ(le_count(LE_LOOKUP), 2, "no lookups outside the two characters");

    /* Control character 0x10 (in 10..0x25): no glyph is drawn, colour
     * becomes 0x10 - 10 = 6, so the next glyph samples table word 6. */
    le_reset();
    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    le_packet_cursor = le_pool + 0x10;
    le_glyph_queue[0] = ga;
    le_glyph_queue_n = 0;
    text[0] = 0x10; text[1] = 'A'; text[2] = 0;
    gDrawAlignedString(text, 100, 20, 2);
    FCK_CHECK(le_packet_cursor == le_pool + 0x10 + 2 * 0x1C,
              "texpage packet plus one glyph packet");
    FCK_EQ(le_count(LE_LOOKUP), 1, "the control code performs no lookup");
    /* 0x7C00: b04 = 0x7C, b05 = 0, b06 = 0 - table word 6 is the blue probe. */
    FCK_CHECK(le_pool[0x30] == 0x7C && le_pool[0x31] == 0x00 &&
              le_pool[0x32] == 0x00,
              "control character recolours the next glyph from table word 6");
    FCK_EQ(le_s16(le_pool, 0x34), 100 - 7,
           "the control code does not advance x");

    /* Cursor exactly at the limit: the texpage packet is still written in
     * place, glyphs are still emitted in place, and the cursor does not
     * advance (the strict < is the property). */
    le_reset();
    lk_fill(le_pool, 0x5A, sizeof(le_pool));
    le_loaded_font = 4;
    le_current_bank = 1;
    le_packet_cursor = le_pool + 0x10;
    le_packet_limit = le_pool + 0x10;
    le_glyph_queue[0] = ga;
    le_glyph_queue_n = 0;
    le_glyph_table[2] = 0x7FFF;
    text[0] = 'A'; text[1] = 0;
    gDrawAlignedString(text, 100, 20, 2);
    FCK_CHECK(le_packet_cursor == le_pool + 0x10,
              "cursor at the limit does not advance");
    FCK_CHECK(le_idx(LE_LOOKUP, 0) >= 0,
              "the glyph is still looked up once the pool is full");
    /* Both packets land in place at the cursor: the sprite overwrites
     * the texpage, so +0x14 holds colour/code, not 0xE10009AB. */
    FCK_EQ(le_w32(le_pool, 0x10), 0x04000000,
           "the in-place sprite len-4 word wins over the texpage word");
    FCK_CHECK(le_pool[0x17] == 100 && le_pool[0x18] == 100 - 7,
              "the in-place sprite carries code 100 and the biased x");
    FCK_EQ(le_count(LE_LOOKUP), 1, "exactly one glyph emitted");
}

static void test_script_timers(void)
{
    /* @leaf 80049420 source src/game/80049420.c: header word 1 gives (len-0xC)/20 entries, each entry's word 1 *5 (negative mirrored into word 2), word 2 random % product, words 3 and 4 += base; tagged views 0x7446554E / 0x74455854 */
    LK_FUNC("func_80049420");
    le_reset();
    lk_fill((u8 *)le_timer_words, 0, sizeof(le_timer_words));
    le_timer_words[1] = 0x34;              /* (0x34 - 0xC) / 20 = 2 entries */
    /* entry 0 at +0xC (words 3..7): the loop reads words 4..7. */
    le_timer_words[4] = 7;                 /* 7 * 5 = 35                     */
    le_timer_words[5] = 0;
    le_timer_words[6] = 100;
    le_timer_words[7] = -50;
    /* entry 1 at +0x20 (words 8..12). */
    le_timer_words[9] = -3;                /* -15 -> mirrored 15             */
    le_timer_words[10] = 0;
    le_timer_words[11] = 7;
    le_timer_words[12] = -9;
    le_timer_obj.f0 = 0x1111;
    le_timer_obj.f1C = 0x2222;
    le_timer_obj.f20 = 0x3333;
    le_data_view = (void *)le_timer_words;
    le_view2_ret = 0x1234;
    le_rng_value = 17;
    gLoadScriptTimers(&le_timer_obj);
    FCK_EQ(le_script_count, 2, "entry count published");
    FCK_CHECK(le_script_ticks == (void *)(le_timer_words + 3),
              "table pointer published at the first entry");
    FCK_EQ(le_timer_words[4], 35, "7 * 5 = 35");
    FCK_EQ(le_timer_words[5], 17, "17 mod 35 = 17");
    FCK_EQ(le_timer_words[6], 0x1234 + 100, "base adds to the third word");
    FCK_EQ(le_timer_words[7], 0x1234 - 50, "base adds to the fourth word");
    FCK_EQ(le_timer_words[9], 15, "-3 * 5 = -15 mirrors to 15");
    FCK_EQ(le_timer_words[10], 15, "mirrored value fills the next word");
    FCK_EQ(le_timer_words[11], 0x1234 + 7, "base adds to the third word");
    FCK_EQ(le_timer_words[12], 0x1234 - 9, "base adds to the fourth word");
    FCK_EQ(le_timer_words[3], 0, "entry word 0 is left alone");
    FCK_EQ(le_count(LE_RNG), 1, "only the positive product draws a value");
    FCK_CHECK(le_calls[0].id == LE_PREP && le_calls[1].id == LE_VIEW1 &&
              le_calls[2].id == LE_VIEW2 && le_calls[3].id == LE_RNG,
              "prepare, view, view, then the random value");
    FCK_CHECK(le_calls[1].a0 == 0x1111 && le_calls[1].a1 == 0x3333 &&
              le_calls[1].a2 == 0x7446554E && le_calls[1].a3 == 0x2222,
              "first view tags 0x7446554E with the object words");
    FCK_CHECK(le_calls[2].a0 == 0x1111 && le_calls[2].a1 == 0x3333 &&
              le_calls[2].a2 == 0x74455854 && le_calls[2].a3 == 0x2222,
              "second view tags 0x74455854 with the object words");
    FCK_CHECK(le_calls[0].a0 == (s32)&le_timer_obj,
              "prepare receives the resource object");
}

static void test_release_light_data(void)
{
    /* @leaf 8004963C source src/game/8004963C.c: guarded frees for table and grid, unconditional pool free, all three pointers cleared */
    LK_FUNC("func_8004963C");
    le_reset();
    le_sprite_desc_table = (void *)0x6000;
    le_light_grid = (void *)0x7000;
    le_light_pool = 0x175;
    gReleaseMapLightData();
    FCK_EQ(le_count(LE_FREE), 2, "two plain blocks released");
    FCK_CHECK(le_calls[0].id == LE_FREE && le_calls[0].a0 == 0x6000 &&
              le_calls[1].id == LE_FREEPOOL && le_calls[1].a0 == 0x175 &&
              le_calls[2].id == LE_FREE && le_calls[2].a0 == 0x7000,
              "descriptor table freed, pool handle freed, then grid freed");
    FCK_CHECK(le_idx(LE_FREEPOOL, 0) >= 0 &&
              le_calls[le_idx(LE_FREEPOOL, 0)].a0 == 0x175,
              "pool handle forwarded to the pool free helper");
    FCK_CHECK(le_sprite_desc_table == 0 && le_light_grid == 0 &&
              le_light_pool == 0, "all three pointers cleared");
    FCK_EQ(le_calls_n, 3, "no other helper runs");

    le_reset();
    le_sprite_desc_table = 0;
    le_light_grid = (void *)0x2100;
    le_light_pool = 0x3200;
    gReleaseMapLightData();
    FCK_CHECK(le_calls[0].id == LE_FREEPOOL && le_calls[0].a0 == 0x3200,
              "pool is freed before the guarded blocks and unconditionally");
    FCK_CHECK(le_calls[1].id == LE_FREE && le_calls[1].a0 == 0x2100,
              "null table is skipped, the grid still freed");
    FCK_CHECK(le_sprite_desc_table == 0 && le_light_grid == 0 &&
              le_light_pool == 0, "all three pointers cleared again");

    le_reset();
    le_sprite_desc_table = 0;
    le_light_grid = 0;
    le_light_pool = 0;
    gReleaseMapLightData();
    FCK_EQ(le_calls_n, 1, "all-null teardown still frees the pool handle");
    FCK_CHECK(le_calls[0].id == LE_FREEPOOL && le_calls[0].a0 == 0,
              "the pool free runs even with a null handle");
}

static void test_init_lighting(void)
{
    /* @leaf 80049690 source src/game/80049690.c: five state words cleared, pool 0x180 x 0x44 tag 2 published, descriptor table cleared */
    LK_FUNC("func_80049690");
    le_reset();
    le_cleanup_time = 7;
    le_resource_table_size = 9;
    le_sprite_region_table = (void *)0xA;
    le_light_grid = (void *)0xB;
    le_sprite_desc_table = (void *)0xC;
    le_light_pool = 0;
    le_pool_handle = 0x6000;
    gInitMapLighting();
    FCK_EQ(le_cleanup_time, 0, "last cleanup time cleared");
    FCK_EQ(le_resource_table_size, 0, "resource table size cleared");
    FCK_EQ((s32)le_sprite_region_table, 0, "sprite region table cleared");
    FCK_EQ((s32)le_light_grid, 0, "grid pointer cleared");
    FCK_EQ((s32)le_sprite_desc_table, 0, "descriptor table cleared");
    FCK_CHECK(le_idx(LE_MAKEPOOL, 0) >= 0 &&
              le_calls[le_idx(LE_MAKEPOOL, 0)].a0 == 0x180 &&
              le_calls[le_idx(LE_MAKEPOOL, 0)].a1 == 0x44 &&
              le_calls[le_idx(LE_MAKEPOOL, 0)].a2 == 2,
              "pool is 0x180 nodes of 0x44 bytes under tag 2");
    FCK_EQ(le_light_pool, 0x6000, "pool handle published");
    FCK_EQ(le_calls_n, 1, "one allocator call");
}

static void test_load_cdcs(void)
{
    /* @leaf 80049734 source src/game/80049734.c: fresh resource clears the status word, prepares and publishes table pointer and count; loaded resource only runs maintenance */
    LK_FUNC("func_80049734");
    le_reset();
    le_resource_state = 0;
    le_sprite_region_table = (void *)0x1234;
    le_resource_table_size = 0x5678;
    le_cdcs_res.data = &le_cdcs_data;
    le_cdcs_res.unk08 = 0x77;
    le_cdcs_data.count = 5;
    gLoadCdcsResource(&le_cdcs_res);
    FCK_EQ(le_cdcs_res.unk08, 0, "fresh resource clears its status word");
    FCK_CHECK(le_sprite_region_table == (void *)&le_cdcs_data.count,
              "region table points at the count word");
    FCK_EQ(le_resource_table_size, 5, "table size takes the count word");
    FCK_CHECK(le_calls[0].id == LE_RSTATE &&
              le_calls[0].a1 == 0 && le_calls[0].a0 == (s32)&le_cdcs_res,
              "fresh path reads the state of the given resource");
    FCK_CHECK(le_calls[1].id == LE_PREP && le_calls[1].a0 == (s32)&le_cdcs_res,
              "fresh path prepares the resource once");
    FCK_CHECK(le_count(LE_MAINT) == 0, "fresh path skips maintenance");

    le_reset();
    le_resource_state = 1;
    le_sprite_region_table = (void *)0x1234;
    le_resource_table_size = 0x5678;
    le_cdcs_res.unk08 = 0x77;
    gLoadCdcsResource(&le_cdcs_res);
    FCK_CHECK(le_count(LE_MAINT) == 1 && le_calls[0].id == LE_RSTATE &&
              le_calls[1].id == LE_MAINT && le_calls[1].a0 == (s32)&le_cdcs_res,
              "loaded resource runs maintenance with the resource");
    FCK_EQ(le_cdcs_res.unk08, 0x77, "loaded path leaves the status word");
    FCK_CHECK(le_sprite_region_table == (void *)0x1234 &&
              le_resource_table_size == 0x5678,
              "loaded path publishes nothing");
}

static void test_reap_lights(void)
{
    struct LeLightNode a, b, c, d;

    /* @leaf 800497A8 source src/game/800497A8.c: bucket sweep from last to first, head/middle unlink, expired only while key >= 0 and flag 0x40 clear, nodes returned to the pool */
    LK_FUNC("func_800497A8");
    le_reset();
    lk_fill((u8 *)&a, 0, sizeof(a));
    lk_fill((u8 *)&b, 0, sizeof(b));
    lk_fill((u8 *)&c, 0, sizeof(c));
    lk_fill((u8 *)&d, 0, sizeof(d));
    le_frame_time[0] = 6;
    le_light_pool = 0xB0B;
    le_grid_width = 1;
    le_grid_height = 1;
    le_light_grid = 0;
    gReapExpiredLightNodes();
    FCK_EQ(le_calls_n, 0, "null grid reaps nothing");

    le_light_grid = (void *)le_grid_cells;
    a.unk04 = 3;                          /* 3 <= 6: expired   */
    a.unk08 = 0;
    a.next = 0;
    le_grid_cells[0] = &a;
    gReapExpiredLightNodes();
    FCK_CHECK(le_count(LE_RET) == 1 &&
              le_calls[le_idx(LE_RET, 0)].a1 == (s32)&a &&
              le_calls[le_idx(LE_RET, 0)].a0 == 0xB0B,
              "expired head is returned to the configured pool");
    FCK_CHECK(le_grid_cells[0] == 0, "cell head cleared after the head unlink");

    /* Middle unlink: a stays (7 > 6), b is expired. */
    le_reset();
    a.unk04 = 7;
    b.unk04 = 2;
    b.unk08 = 0;
    a.next = &b;
    b.next = 0;
    le_grid_cells[0] = &a;
    gReapExpiredLightNodes();
    FCK_CHECK(le_grid_cells[0] == &a && a.next == 0,
              "live head keeps the cell and sees the unlinked node gone");
    FCK_CHECK(le_count(LE_RET) == 1 && le_calls[0].a1 == (s32)&b,
              "expired middle node is returned once");
    FCK_CHECK(b.unk04 == 2, "the returned node's payload is untouched");

    /* Two expired nodes in one bucket both go, head first. */
    le_reset();
    a.unk04 = 1;
    b.unk04 = 2;
    a.unk08 = 0;
    a.next = &b;
    b.next = 0;
    le_grid_cells[0] = &a;
    gReapExpiredLightNodes();
    FCK_CHECK(le_count(LE_RET) == 2 && le_calls[0].a1 == (s32)&a &&
              le_calls[1].a1 == (s32)&b,
              "both expired nodes are returned head first");
    FCK_CHECK(le_grid_cells[0] == 0, "bucket drained");

    /* Expiry exactly at the frame time is reaped (the inclusive <= is the
     * property): 6 <= 6. */
    le_reset();
    c.unk04 = 6;
    c.unk08 = 0;
    c.next = 0;
    le_grid_cells[0] = &c;
    gReapExpiredLightNodes();
    FCK_CHECK(le_count(LE_RET) == 1 && le_calls[0].a1 == (s32)&c,
              "expiry equal to the frame time is reaped");
    FCK_CHECK(le_grid_cells[0] == 0, "the exactly-expired head is unlinked");

    /* Flag 0x40 stops the walk before any free. */
    le_reset();
    a.unk04 = 1;
    a.unk08 = 0x40;
    a.next = 0;
    le_grid_cells[0] = &a;
    gReapExpiredLightNodes();
    FCK_EQ(le_calls_n, 0, "flagged node is never reaped");

    /* Negative key stops the walk; even an expired successor is untouched. */
    a.unk04 = -1;
    a.unk08 = 0;
    b.unk04 = 1;
    b.unk08 = 0;
    a.next = &b;
    b.next = 0;
    le_grid_cells[0] = &a;
    gReapExpiredLightNodes();
    FCK_EQ(le_calls_n, 0,
           "negative key stops the walk and nothing is reaped");

    /* Two buckets, last index first. */
    le_reset();
    le_grid_width = 2;
    le_grid_height = 1;
    c.unk04 = 1;
    c.unk08 = 0;
    c.next = 0;
    d.unk04 = 1;
    d.unk08 = 0;
    d.next = 0;
    le_grid_cells[1] = &c;
    le_grid_cells[0] = &d;
    gReapExpiredLightNodes();
    FCK_CHECK(le_count(LE_RET) == 2 && le_calls[0].a1 == (s32)&c &&
              le_calls[1].a1 == (s32)&d,
              "higher bucket index is swept first");
}

static void test_update_light_node(void)
{
    struct LeLightNode node, node2, other, n1;
    int m;

    /* @leaf 80049980 source src/game/80049980.c: cell (z>>17) + W*(x>>17), head/tail insert and both unlink paths, depth key keeps the larger extent, flat record points, rotated diamond through the prefilled matrix, coverage mask takes (z, x, z +- w/2, x +- d/2) */
    LK_FUNC("func_80049980");
    le_reset();
    lk_fill((u8 *)&node, 0xA5, sizeof(node));
    node.unk00 = 0x7777;
    node.unk02 = -1;
    node.unk04 = 5;
    node.unk08 = 4;                       /* flat record path */
    le_grid_width = 2;
    le_grid_height = 2;
    le_grid_cells[0] = le_grid_cells[1] = le_grid_cells[2] = le_grid_cells[3] = 0;
    le_light_grid = (void *)le_grid_cells;
    le_mask_ret = 0x1D;
    /* index = (0x30 >> 17) + 2 * (0x20123 >> 17) = 0 + 2 = 2 */
    gUpdateDynamicLightNode(&node, 0x20123, 0x30, 0x50, 0x68, 0x90, 0, 0, 0);
    FCK_EQ(node.unk02, 2, "cell index is (z>>17) + W*(x>>17)");
    FCK_CHECK(le_grid_cells[2] == &node, "empty cell takes the node at the head");
    FCK_CHECK(node.unk00 == 0x7777, "the id field is never read here");
    FCK_EQ(node.unk14, 0x90,
           "depth key keeps the larger extent (max of 0x90 and 0x68)");
    FCK_EQ(node.points[0].x, 0x12, "flat record x is (x & 0x1FFFF) >> 4");
    FCK_EQ(node.points[0].z, 0x3, "flat record z is (z & 0x1FFFF) >> 4");
    FCK_EQ(node.points[0].y, 0x50 >> 4, "flat record y is y >> 4");
    FCK_EQ(node.points[1].x, 0x68 >> 4, "flat record stores the width >> 4");
    FCK_EQ(node.points[1].y, 0x90 >> 4, "flat record stores the depth >> 4");
    FCK_CHECK(node.points[1].z == (s16)0xA5A5,
              "zero angle leaves point 1 z alone");
    FCK_CHECK((node.unk08 & 8) == 0, "zero angle clears flag 8");
    FCK_CHECK(node.points[2].x == (s16)0xA5A5 && node.points[4].y == (s16)0xA5A5,
              "flat record leaves points 2..4 alone");
    FCK_EQ(node.unk12, 0x1D, "coverage mask stored as returned");
    m = le_idx(LE_MASK, 0);
    FCK_CHECK(m >= 0 &&
              le_calls[m].a0 == 0x30 && le_calls[m].a1 == 0x20123 &&
              le_calls[m].a2 == 0x30 - 0x34 && le_calls[m].a3 == 0x30 + 0x34 &&
              le_calls[m].a4 == 0x20123 - 0x48 && le_calls[m].a5 == 0x20123 + 0x48,
              "mask takes (z, x, z +- width/2, x +- depth/2)");

    /* Flat record with an angle: point 1 z holds the angle, flag 8 set.
     * Negative key into an empty cell: still a head insert. */
    lk_fill((u8 *)&node2, 0xA5, sizeof(node2));
    node2.unk02 = -1;
    node2.unk04 = -2;
    node2.unk08 = 4;
    le_grid_cells[2] = 0;
    gUpdateDynamicLightNode(&node2, 0x20123, 0x30, 0x50, 0x68, 0x90, 0x1B2, 0, 0);
    FCK_CHECK(le_grid_cells[2] == &node2,
              "negative key still inserts at an empty head");
    FCK_EQ(node2.points[1].z, 0x1B2, "flat record with angle stores it in point 1 z");
    FCK_CHECK((node2.unk08 & 8) == 8, "non-zero angle sets flag 8");

    /* Head-relink: node sits at the head of cell 0 and moves to cell 2
     * through the rotated path. w32 = (0x68/2)>>4 = 3, d32 = (0x90/2)>>4 = 4:
     * diamond (in x, z): p1 (-4, 3), p2 (4, 3), p3 (4, -3), p4 (-4, -3). */
    le_reset();
    lk_fill((u8 *)&node, 0xA5, sizeof(node));
    node.unk02 = 0;
    node.unk04 = 5;
    node.unk08 = 0;                       /* rotated path */
    node.next = 0;
    le_grid_cells[0] = &node;
    le_grid_cells[1] = 0;
    le_grid_cells[2] = 0;
    le_mask_ret = 0x221;
    gUpdateDynamicLightNode(&node, 0x20123, 0x30, 0x50, 0x68, 0x90, 0x15, 7, 8);
    FCK_CHECK(le_grid_cells[0] == 0 && le_grid_cells[2] == &node,
              "old cell head cleared, node re-inserted at the new head");
    m = le_idx(LE_MATRIX, 0);
    FCK_CHECK(m >= 0 && le_calls[m].a0 == 0x15 && le_calls[m].a1 == 7 &&
              le_calls[m].a2 == 8,
              "rotation receives the angle and the two forwarded arguments");
    FCK_CHECK(m >= 0 && le_calls[m].a4 == 0x123 && le_calls[m].a5 == 0x50 &&
              le_calls[m].a6 == 0x30,
              "matrix words 9..11 are prefilled with (x&0x1FFFF, y, z&0x1FFFF)");
    FCK_CHECK(node.points[0].x == 0 && node.points[0].y == 0 &&
              node.points[0].z == 0,
              "diamond centre point is zero");
    FCK_CHECK(node.points[1].x == -4 && node.points[1].z == 3,
              "point 1 is -d32 in x and w32 in z");
    FCK_CHECK(node.points[2].x == 4 && node.points[2].z == 3,
              "point 2 is d32 in x and w32 in z");
    FCK_CHECK(node.points[3].x == 4 && node.points[3].z == -3,
              "point 3 is d32 in x and -w32 in z");
    FCK_CHECK(node.points[4].x == -4 && node.points[4].z == -3,
              "point 4 is -d32 in x and -w32 in z");
    FCK_CHECK(le_idx(LE_TRANSFORM, 0) >= 0 &&
              le_calls[le_idx(LE_TRANSFORM, 0)].a2 == (s32)&node.points[0] &&
              le_calls[le_idx(LE_TRANSFORM, 0)].a3 == 5,
              "transform targets the node points with count 5");
    FCK_EQ(node.unk14, 0x90, "rotated path keeps the larger extent too");
    FCK_EQ(node.unk12, 0x221, "rotated path stores the mask as returned");

    /* Tail append: negative key with a populated target cell walks to
     * the tail. */
    le_reset();
    lk_fill((u8 *)&node, 0xA5, sizeof(node));
    lk_fill((u8 *)&other, 0, sizeof(other));
    node.unk02 = -1;
    node.unk04 = -2;
    node.unk08 = 4;
    node.next = 0;
    other.next = 0;
    le_grid_cells[1] = &other;
    le_grid_cells[0] = 0;
    le_grid_cells[2] = 0;
    le_mask_ret = 0x11;
    gUpdateDynamicLightNode(&node, 0x12345, 0x22334, 0x210, 0x400, 0x400, 0, 0, 0);
    FCK_EQ(node.unk02, 1, "index 1 = (0x22334 >> 17) + 2 * (0x12345 >> 17)");
    FCK_CHECK(le_grid_cells[1] == &other && other.next == &node && node.next == 0,
              "negative key with a populated cell appends at the tail");
    m = le_idx(LE_MASK, 0);
    FCK_CHECK(m >= 0 &&
              le_calls[m].a0 == 0x22334 && le_calls[m].a1 == 0x12345 &&
              le_calls[m].a2 == 0x22134 && le_calls[m].a3 == 0x22534 &&
              le_calls[m].a4 == 0x12145 && le_calls[m].a5 == 0x12545,
              "mask corners are z/x half-extents of 0x200");
    FCK_EQ(node.unk14, 0x400, "equal extents keep the depth");

    /* Predecessor unlink: node sits behind n1 in cell 0 and moves away. */
    le_reset();
    lk_fill((u8 *)&node, 0xA5, sizeof(node));
    lk_fill((u8 *)&n1, 0, sizeof(n1));
    node.unk02 = 0;
    node.unk04 = 5;
    node.unk08 = 4;
    node.next = 0;
    n1.next = &node;
    le_grid_cells[0] = &n1;
    le_grid_cells[1] = 0;
    le_grid_cells[2] = 0;
    gUpdateDynamicLightNode(&node, 0x20123, 0x30, 0x50, 0x68, 0x90, 0, 0, 0);
    FCK_CHECK(le_grid_cells[0] == &n1 && n1.next == 0,
              "predecessor search unlinks node from behind n1");
    FCK_CHECK(le_grid_cells[2] == &node && node.unk02 == 2,
              "re-inserted at the new cell head");
}

static void test_spawn_light(void)
{
    struct LeLightNode *n;
    s32 ret;

    /* @leaf 80049C38 source src/game/80049C38.c: both size words >= 0x40 and a grid required, id-list dedup keeping the count on a miss, pool exhaustion retries after a real reap, surface modes -3/0/1/other via call args -1/-2/value-2, negative angles randomised through & 0xFFF, life offset by the frame tick, node placed via the real 80049980 */
    LK_FUNC("func_80049C38");
    le_reset();
    lk_fill((u8 *)&le_spawn_obj, 0, sizeof(le_spawn_obj));
    lk_fill((u8 *)&le_alloc_nodes, 0x5C, sizeof(le_alloc_nodes));
    le_alloc_nodes[0].unk00 = 0x7777;
    le_alloc_nodes[0].next = 0;
    le_alloc_queue[0] = &le_alloc_nodes[0];
    le_alloc_queue[1] = 0;
    le_alloc_queue_n = 0;
    le_glyph_queue_n = 0;
    le_grid_width = 2;
    le_grid_height = 1;
    le_grid_cells[0] = 0;
    le_grid_cells[1] = 0;
    le_light_grid = (void *)le_grid_cells;
    le_light_pool = 0xB0B;
    le_frame_time[0] = 0x2A1;
    le_sprite_region_table = 0;
    le_rng_value = 0x3A5;
    le_mask_ret = 0x1D;
    le_spawn_obj.unk08 = 0x12345;         /* x                     */
    le_spawn_obj.unk10 = 0x22334;         /* z                     */
    le_spawn_obj.unk14 = 0;               /* 0 -> rotated node path */
    le_spawn_obj.unk15 = -7;              /* negative key          */
    le_spawn_obj.unk16 = 0x21;            /* height offset         */
    le_spawn_obj.unk18 = 0x40;
    le_spawn_obj.unk1A = 0x40;
    le_spawn_obj.unk1C = -1;              /* angle <- randomised   */
    le_spawn_obj.unk1E = -2;              /* arg7 <- randomised    */
    le_spawn_obj.unk20 = 0x1B2;           /* arg8 fixed            */
    le_spawn_obj.unk22 = -3;              /* no surface query      */
    le_spawn_obj.unk23 = 1;
    le_spawn_obj.unk24 = 2;
    le_spawn_obj.unk25 = 3;
    le_spawn_obj.unk28 = 0x9C38;
    ret = gSpawnDynamicLightNode(&le_spawn_obj, -5);
    n = &le_alloc_nodes[0];
    FCK_CHECK(ret == (s32)n, "spawn returns the allocated node");
    FCK_EQ(le_count(LE_ALLOC), 1, "one successful allocation, no reap needed");
    FCK_EQ(n->unk00, -7, "negative key becomes the node index");
    FCK_EQ(n->unk02, 1, "the real 80049980 placed the node in cell 1");
    FCK_CHECK(le_grid_cells[1] == n, "node was inserted at the empty cell head");
    FCK_EQ(n->unk04, -5, "negative life is kept as-is");
    FCK_EQ(n->unk0C, 0x2A1, "persistent light stores the frame tick");
    FCK_EQ(n->unk10, 0x9C38, "persistent light stores the payload word");
    FCK_CHECK(n->unk09 == 1 && n->unk0A == 2 && n->unk0B == 3,
              "colour bytes copied from the descriptor");
    FCK_EQ(n->unk08, 0, "rotated descriptor leaves the flag byte clear");
    FCK_EQ(n->unk12, 0x1D, "mask stored through the real update");
    /* width and depth are unk18/unk1A << 4 = 0x400; equal extents keep 0x400. */
    FCK_EQ(n->unk14, 0x400, "0x40 extent becomes 0x400 after << 4");
    FCK_EQ(le_count(LE_RNG), 2, "two negative angle words randomised");
    FCK_CHECK(le_idx(LE_MATRIX, 0) >= 0 &&
              le_calls[le_idx(LE_MATRIX, 0)].a0 == 0x3A5 &&
              le_calls[le_idx(LE_MATRIX, 0)].a1 == 0x3A5 &&
              le_calls[le_idx(LE_MATRIX, 0)].a2 == 0x1B2,
              "angle words forwarded as (0x3A5 & 0xFFF, 0x3A5 & 0xFFF, fixed)");
    FCK_CHECK(n->points[0].x == 0 && n->points[0].z == 0,
              "rotated diamond centre point is zero");
    FCK_CHECK(n->points[1].x == -(0x400 / 2 >> 4) &&
              n->points[1].z == 0x400 / 2 >> 4,
              "diamond point 1 is (-0x20, +0x20) in (x, z)");
    FCK_EQ(le_count(LE_SURFACE), 0, "surface mode -3 skips the query");

    /* Surface mode 0 -> call arg -1, the answer feeds the geometry height;
     * a non-negative life is offset by the frame tick; an id found in the
     * list leaves the persistent-life words alone. */
    le_reset();
    lk_fill((u8 *)&le_alloc_nodes[1], 0x5C, sizeof(le_alloc_nodes[1]));
    le_alloc_nodes[1].next = 0;
    le_alloc_queue[0] = &le_alloc_nodes[1];
    le_alloc_queue_n = 0;
    le_surface_ret = 0x12350;
    le_mask_ret = 0x11;
    le_frame_time[0] = 0x2A1;
    lk_fill((u8 *)&le_id_list, 0, sizeof(le_id_list));
    le_id_list.count = 3;
    le_id_list.entries[0].id = 3;
    le_id_list.entries[1].id = 5;
    le_id_list.entries[2].id = 9;
    le_sprite_region_table = (void *)&le_id_list;
    le_spawn_obj.unk14 = 4;              /* flat node path       */
    le_spawn_obj.unk15 = 5;               /* key found at slot 1   */
    le_spawn_obj.unk16 = 0x21;
    le_spawn_obj.unk18 = 0x40;
    le_spawn_obj.unk1A = 0x3F;            /* one size already clears the gate */
    le_spawn_obj.unk1C = 0x111;
    le_spawn_obj.unk1E = 0x222;
    le_spawn_obj.unk20 = 0x333;
    le_spawn_obj.unk22 = 0;               /* surface mode 0       */
    le_grid_cells[1] = 0;
    n = &le_alloc_nodes[1];
    ret = gSpawnDynamicLightNode(&le_spawn_obj, 0x40);
    FCK_CHECK(ret == (s32)n, "a single gate-clearing size is enough");
    FCK_EQ(n->unk00, 1, "id found in the list becomes the node index");
    FCK_EQ(le_count(LE_SURFACE), 1, "surface mode 0 triggers the query");
    FCK_CHECK(le_idx(LE_SURFACE, 0) >= 0 &&
              le_calls[le_idx(LE_SURFACE, 0)].a0 == (s32)&le_spawn_obj.unk08 &&
              le_calls[le_idx(LE_SURFACE, 0)].a1 == -1,
              "surface mode 0 queries the position words with argument -1");
    /* y arg = 0x12350 + (0x21 << 4) = 0x12560; flat path stores y >> 4. */
    FCK_EQ(n->points[0].y, 0x1256,
           "surface answer plus (unk16 << 4) becomes the geometry height");
    FCK_EQ(n->unk04, 0x40 + 0x2A1, "non-negative life is offset by the frame tick");
    FCK_CHECK(n->unk0C == 0x5C5C5C5C && n->unk10 == 0x5C5C,
              "resolved id leaves the persistent-life words alone");
    FCK_EQ(n->points[1].z, 0x111, "flat record stores the fixed angle");
    FCK_CHECK((n->unk08 & 8) == 8 && (n->unk08 & 4) == 4,
              "flat descriptor sets flags 4 and 8 for a non-zero angle");

    /* Id miss keeps the count as the index. */
    le_reset();
    lk_fill((u8 *)&le_alloc_nodes[2], 0x5C, sizeof(le_alloc_nodes[2]));
    le_alloc_nodes[2].next = 0;
    le_alloc_queue[0] = &le_alloc_nodes[2];
    le_alloc_queue_n = 0;
    le_mask_ret = 0x11;
    le_frame_time[0] = 0x2A1;
    le_sprite_region_table = (void *)&le_id_list;
    le_spawn_obj.unk15 = 7;               /* id not in the list    */
    le_spawn_obj.unk22 = -3;
    le_grid_cells[1] = 0;
    ret = gSpawnDynamicLightNode(&le_spawn_obj, 0);
    FCK_EQ(le_alloc_nodes[2].unk00, 3, "id miss keeps the list count as index");

    /* Key exactly 0 still scans the id list (the inclusive >= is the
     * property): 0 is not in the list, so the miss keeps the count. */
    le_reset();
    lk_fill((u8 *)&le_alloc_nodes[3], 0x5C, sizeof(le_alloc_nodes[3]));
    le_alloc_nodes[3].next = 0;
    le_alloc_queue[0] = &le_alloc_nodes[3];
    le_alloc_queue_n = 0;
    le_mask_ret = 0x11;
    le_frame_time[0] = 0;
    le_sprite_region_table = (void *)&le_id_list;
    le_spawn_obj.unk15 = 0;
    le_spawn_obj.unk22 = -3;
    le_grid_cells[1] = 0;
    ret = gSpawnDynamicLightNode(&le_spawn_obj, 0);
    FCK_CHECK(ret == (s32)&le_alloc_nodes[3],
              "key 0 spawns like any non-negative key");
    FCK_EQ(le_alloc_nodes[3].unk00, 3,
           "key 0 scans the list and keeps the count on a miss");

    /* Gate blocks: both sizes below 0x40, or a null grid. */
    le_reset();
    le_spawn_obj.unk18 = 0x3F;
    le_spawn_obj.unk1A = 0x3F;
    le_spawn_obj.unk15 = -7;
    le_alloc_queue[0] = &le_alloc_nodes[3];
    le_alloc_queue_n = 0;
    FCK_EQ(gSpawnDynamicLightNode(&le_spawn_obj, 0), 0,
           "both sizes below 0x40 spawn nothing");
    le_spawn_obj.unk18 = 0x40;
    le_light_grid = 0;
    FCK_EQ(gSpawnDynamicLightNode(&le_spawn_obj, 0), 0,
           "null grid spawns nothing");
    FCK_EQ(le_alloc_queue_n, 0, "no allocation attempted past the gates");

    /* Pool exhaustion: the first allocation fails, the real 800497A8 sweep
     * reaps the expired bucket head, the retry fails too and 0 comes back. */
    le_reset();
    lk_fill((u8 *)&le_alloc_nodes[3], 0xA5, sizeof(le_alloc_nodes[3]));
    le_alloc_nodes[3].unk04 = 1;          /* 1 <= 6: expired after the reap */
    le_alloc_nodes[3].unk08 = 0;
    le_alloc_nodes[3].next = 0;
    le_grid_cells[0] = &le_alloc_nodes[3];
    le_grid_cells[1] = 0;
    le_light_grid = (void *)le_grid_cells;
    le_grid_width = 2;
    le_grid_height = 1;
    le_light_pool = 0xB0B;
    le_frame_time[0] = 6;
    le_alloc_queue[0] = 0;
    le_alloc_queue[1] = 0;
    le_alloc_queue_n = 0;
    le_spawn_obj.unk18 = 0x40;
    le_spawn_obj.unk1A = 0x40;
    le_spawn_obj.unk22 = -3;
    FCK_EQ(gSpawnDynamicLightNode(&le_spawn_obj, 0), 0,
           "exhausted pool after a sweep reports failure");
    FCK_EQ(le_alloc_queue_n, 2, "allocation was tried twice");
    FCK_CHECK(le_count(LE_RET) == 1 &&
              le_calls[le_idx(LE_RET, 0)].a1 == (s32)&le_alloc_nodes[3],
              "the real reaper returned the expired node to the pool");
    FCK_CHECK(le_grid_cells[0] == 0, "reaper drained the expired bucket");
    FCK_EQ(le_count(LE_RNG) + le_count(LE_SURFACE), 0,
           "failed spawn draws no random values and queries nothing");

    /* Surface modes 1 and 2 map to the -2 and value-2 call arguments. */
    le_reset();
    lk_fill((u8 *)&le_alloc_nodes[0], 0x5C, sizeof(le_alloc_nodes[0]));
    le_alloc_nodes[0].next = 0;
    le_alloc_queue[0] = &le_alloc_nodes[0];
    le_alloc_queue_n = 0;
    le_grid_cells[0] = 0;
    le_grid_cells[1] = 0;
    le_light_grid = (void *)le_grid_cells;
    le_surface_ret = 0;
    le_mask_ret = 0x11;
    le_spawn_obj.unk15 = -7;
    le_spawn_obj.unk22 = 1;
    le_spawn_obj.unk18 = 0x40;
    le_spawn_obj.unk1A = 0x40;
    gSpawnDynamicLightNode(&le_spawn_obj, 0);
    FCK_CHECK(le_idx(LE_SURFACE, 0) >= 0 &&
              le_calls[le_idx(LE_SURFACE, 0)].a1 == -2,
              "surface mode 1 queries with argument -2");
    le_reset();
    le_alloc_queue[0] = &le_alloc_nodes[0];
    le_alloc_queue_n = 0;
    le_spawn_obj.unk22 = 5;
    gSpawnDynamicLightNode(&le_spawn_obj, 0);
    FCK_CHECK(le_idx(LE_SURFACE, 0) >= 0 &&
              le_calls[le_idx(LE_SURFACE, 0)].a1 == 3,
              "surface mode 5 queries with argument value - 2");
}

int main(void)
{
    test_hud_panel_sprite();
    test_frame_close_hud();
    test_hud_layout_mode();
    test_text_init();
    test_glyph_sprite();
    test_draw_string();
    test_script_timers();
    test_release_light_data();
    test_init_lighting();
    test_load_cdcs();
    test_reap_lights();
    test_update_light_node();
    test_spawn_light();
    return lk_done("leaf_engage");
}
