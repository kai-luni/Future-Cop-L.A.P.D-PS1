/* leaf_render: fifteen hardware-free units around rendering primitives, map
 * geometry, sound configuration and entity setup:
 *
 *   80014490 ray-burst disc packets       80015830 line quad (type-7 part)
 *   80021554 sound storage init           8002DFC4 type-27 entity setup
 *   80040734 effect light spawn           8004F188 path-mover commands
 *   8005D690 weapon dome effect update    80063568 map tile animations
 *   800684AC map grid ray walk (DDA)      8006AB38 weapon loadout reset
 *   8006C84C dome patch mesh build        8007CACC music preview menu
 *   80083834 stale scene node reaper      80086C74 movie SPU voice attrs
 *   80094F4C GPU reset (ResetGraph)
 *
 * Fixture style: every D_ label a unit reads is defined in this file and
 * written by the test before each call; nothing relies on zero-init or on
 * retail memory. Hardware pointers (DMA2 CHCR, DPCR, GP1) point at plain
 * harness words. Every callee outside the suite is bound by its retail label
 * and only records its arguments / returns a configured value. The sound
 * getters func_8004522C / func_80045288 / func_800452C8 come from the shared
 * mock library (fcm_sound.*), never from a private copy.
 *
 * Evidence:
 *  - spec: packet words and register values follow psx-spx (GPU GP0/GP1,
 *    DMA CHCR/DPCR) and the Psy-Q libgpu / libspu structure layouts
 *    (POLY_G3, SpuVoiceAttr, SpuCommonAttr, ResetGraph modes).
 *  - math: line-quad corner offsets are checked against Euclidean geometry
 *    (perpendicular, |offset| = width) instead of the unit's own octagonal
 *    length approximation.
 *  - semantic: the DDA ray walk is checked against exact segment / grid-line
 *    intersections; the tile animations against loop / reverse / ping-pong
 *    playback order; the loadout reset against the +25 % / +50 % / 9999
 *    difficulty tiers.
 *  - source: everything else pins the documented body behaviour (doc
 *    comment of the unit) on synthetic fixtures.
 *
 * Mutation (`mutate --per-unit 8`, all 15 units): the remaining survivors are
 * equivalent mutants - `return 0` on paths that already return 0, the swap
 * guard `a < b` -> `a <= b` (80015830), `dist > 0` -> `>= 0` with zero
 * products, the unused `prev[4]` slot (800684AC), the 0x28 footprint boundary
 * where both branches give 0x50 (80040734), and 8006C84C's range check whose
 * mutant divides by zero: qemu-user returns the dividend for x / 0, which
 * happens to equal the in-range result, so it cannot be told apart here.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"
/* @mock sound_mock */
/* func_8004522C / func_80045288 / func_800452C8 (music / VAGM / voice volume
 * getters) come from the shared mock library; tests set fcm_sound.*. */
#include "sound_mock.h"

static void lr_copy(void *d, const void *s, int n)
{
    u8 *dd = d;
    const u8 *ss = s;
    while (n--) *dd++ = *ss++;
}

#define RD16(p, off) (*(s16 *)((u8 *)(p) + (off)))
#define RDU16(p, off) (*(u16 *)((u8 *)(p) + (off)))
#define RD32(p, off) (*(s32 *)((u8 *)(p) + (off)))
#define RDU32(p, off) (*(u32 *)((u8 *)(p) + (off)))
#define RD8(p, off) (*(u8 *)((u8 *)(p) + (off)))

/* ========================================================================
 * Shared mocks
 * ======================================================================== */

/* func_8001846C gSinCos(angle, *sine, *cosine): used by 80014490 and
 * 80063568. Returns the configured pair and records each angle. */
static s32 sc_sine, sc_cosine, sc_calls, sc_angles[16];
void mock_sincos(u32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");
void mock_sincos(u32 angle, s32 *sine, s32 *cosine)
{
    if (sc_calls < 16) sc_angles[sc_calls] = (s32)angle;
    sc_calls++;
    *sine = sc_sine;
    *cosine = sc_cosine;
}
static void sc_reset(s32 s, s32 c)
{
    int i;
    sc_sine = s;
    sc_cosine = c;
    sc_calls = 0;
    for (i = 0; i < 16; i++) sc_angles[i] = -1;
}

/* func_80019668 (rand) for 80063568: returns a scripted sequence. */
static s32 rng_seq[8], rng_pos, rng_calls;
s32 mock_rand(void) __asm__("func_80019668");
s32 mock_rand(void)
{
    rng_calls++;
    return rng_seq[(rng_pos++) & 7];
}
static void rng_reset(void)
{
    int i;
    for (i = 0; i < 8; i++) rng_seq[i] = 0;
    rng_pos = rng_calls = 0;
}

/* func_8005D3FC (dome effect block alloc): 8005D690 uses the returned
 * block, 8006AB38 only calls it. */
static void *dome_ret;
void *mock_alloc_dome(void *owner, s32 index) __asm__("func_8005D3FC");
void *mock_alloc_dome(void *owner, s32 index)
{
    fck_record("alloc_dome", (s32)owner, index, 0);
    return dome_ret;
}

s32 gFrameTime __asm__("D_8009FAA4");

/* ========================================================================
 * 80014490 ray-burst disc
 * ======================================================================== */
typedef struct { s16 x, y, depth; } RbSrc;
void gEmitRayBurst(RbSrc *src, u32 p1, u32 p2, s32 p3, s32 p4, s32 p5, s32 p6) __asm__("func_80014490");
u8 *gPacketCursor __asm__("D_8009FA90");
u8 *gPacketLimit __asm__("D_8009F9C0");
static u32 pkt_area[0x400];

/* Psy-Q POLY_G3 (libgpu.h): tag, r0 g0 b0 code, x0 y0, r1 g1 b1 pad, x1 y1,
 * r2 g2 b2 pad, x2 y2 = 7 words = 0x1C bytes. */
#define G3_TAG(p) RDU32(p, 0x00)
#define G3_C0(p) RDU32(p, 0x04)
#define G3_X0(p) RD16(p, 0x08)
#define G3_Y0(p) RD16(p, 0x0A)
#define G3_C1(p) RDU32(p, 0x0C)
#define G3_X1(p) RD16(p, 0x10)
#define G3_Y1(p) RD16(p, 0x12)
#define G3_C2(p) RDU32(p, 0x14)
#define G3_X2(p) RD16(p, 0x18)
#define G3_Y2(p) RD16(p, 0x1A)

static void test_80014490(void)
{
    RbSrc src;
    u8 *base = (u8 *)pkt_area, *t;
    int k;

    /* @leaf 80014490 spec psx-spx GPU: GP0(E1h) draw mode 0x6A0 = semi-transparency mode 1 (B+F) + dither + draw-to-display, GP0(32h) shaded semi-transparent triangle; Psy-Q POLY_G3 = 0x1C bytes, tag len 6 (header packet len 1); Psy-Q rsin/rcos 4096 = full circle -> 8 spikes step 0x200; four quadrant spikes are +90 deg rotations, base half-width perpendicular = radius >> shift */
    LK_FUNC("func_80014490");

    /* Early out: depth <= near offset -> nothing written, no sin/cos. */
    lk_fill(pkt_area, 0xCD, sizeof(pkt_area));
    gPacketCursor = base;
    gPacketLimit = base + sizeof(pkt_area);
    sc_reset(0, 0x10000);
    src.x = 100; src.y = 50; src.depth = 0x200;
    gEmitRayBurst(&src, 0x11223300, 0x44556600, 8, 0x10, 0x200, 0);
    FCK_EQ((s32)gPacketCursor, (s32)base, "rb: depth == near -> cursor untouched");
    FCK_EQ(sc_calls, 0, "rb: early out never calls sin/cos");
    FCK_EQ(RDU32(base, 0), 0xCDCDCDCD, "rb: early out writes no packet");

    /* depth 0x400, near 0x200, scale 0x10 -> radius (0x10 << 9) / 0x400 = 8.
     * 8 spikes requested (radius allows 4 * 8 = 32). sin = 0.5, cos = 1.0
     * in 16.16 -> (sin, cos) * 8 = (4, 8); shift 1 -> half-width (2, 4). */
    lk_fill(pkt_area, 0xCD, sizeof(pkt_area));
    gPacketCursor = base;
    sc_reset(0x8000, 0x10000);
    src.depth = 0x400;
    gEmitRayBurst(&src, 0x11223300, 0x44556600, 8, 0x10, 0x200, (0x100 << 8) | 1);
    FCK_EQ((s32)gPacketCursor, (s32)(base + 9 * 0x1C), "rb: header + 8 POLY_G3 packets of 0x1C");
    FCK_EQ(RDU32(base, 4), 0xE10006A0, "rb: header is GP0(E1h) draw mode, additive blend");
    FCK_EQ(RDU32(base, 0), 0x01000200, "rb: header tag len 1, depth - near in low 24 bits");
    t = base + 0x1C;
    for (k = 0; k < 8; k++) {
        u8 *p = t + k * 0x1C;
        FCK_EQ(G3_TAG(p), 0x06000200, "rb: POLY_G3 tag len 6 + depth key");
        FCK_EQ(G3_C0(p), 0x32445566, "rb: code 0x32 + centre colour p2 >> 8");
        FCK_EQ(G3_C1(p), 0x00112233, "rb: vertex-1 colour p1 >> 8");
        FCK_EQ(G3_C2(p), 0x00112233, "rb: vertex-2 colour p1 >> 8");
    }
    FCK_EQ(sc_calls, 2, "rb: one sin/cos per spike of a quadrant (8 / 4)");
    FCK_EQ(sc_angles[0], 0x100 + 0x200, "rb: first angle = start + 4096 / 8");
    FCK_EQ(sc_angles[1], 0x100 + 0x400, "rb: second angle = start + 2 * 4096 / 8");
    /* quadrant 0 tip (sx + cos*r, sy + sin*r) = (108, 54) */
    FCK_EQ(G3_X0(t), 108, "rb: q0 tip x"); FCK_EQ(G3_Y0(t), 54, "rb: q0 tip y");
    /* base vertices: centre -/+ perpendicular (-sin, cos) * r >> 1 = (-2, 4) */
    FCK_EQ(G3_X1(t), 98, "rb: q0 base1 x"); FCK_EQ(G3_Y1(t), 54, "rb: q0 base1 y");
    FCK_EQ(G3_X2(t), 102, "rb: q0 base2 x"); FCK_EQ(G3_Y2(t), 46, "rb: q0 base2 y");
    /* quadrant 1 (packet 8/4 = 2) is q0 rotated +90 deg: tip (-4, 8); the
     * rotated base pair comes out with its two vertices swapped. */
    {
        u8 *q1 = t + 2 * 0x1C, *q2 = t + 4 * 0x1C, *q3 = t + 6 * 0x1C;
        FCK_EQ(G3_X0(q1), 96, "rb: q1 tip x"); FCK_EQ(G3_Y0(q1), 58, "rb: q1 tip y");
        FCK_EQ(G3_X1(q1), 104, "rb: q1 base1 x"); FCK_EQ(G3_Y1(q1), 52, "rb: q1 base1 y");
        FCK_EQ(G3_X2(q1), 96, "rb: q1 base2 x"); FCK_EQ(G3_Y2(q1), 48, "rb: q1 base2 y");
        /* quadrant 2: rotated 180 deg: (-8, -4) */
        FCK_EQ(G3_X0(q2), 92, "rb: q2 tip x"); FCK_EQ(G3_Y0(q2), 46, "rb: q2 tip y");
        FCK_EQ(G3_X1(q2), 102, "rb: q2 base1 x"); FCK_EQ(G3_Y1(q2), 46, "rb: q2 base1 y");
        /* quadrant 3: rotated 270 deg: (4, -8) */
        FCK_EQ(G3_X0(q3), 104, "rb: q3 tip x"); FCK_EQ(G3_Y0(q3), 42, "rb: q3 tip y");
        FCK_EQ(G3_X1(q3), 96, "rb: q3 base1 x"); FCK_EQ(G3_Y1(q3), 48, "rb: q3 base1 y");
        FCK_EQ(G3_X2(q3), 104, "rb: q3 base2 x"); FCK_EQ(G3_Y2(q3), 52, "rb: q3 base2 y");
    }

    /* depth exactly one above the near offset still draws */
    gPacketCursor = base;
    sc_reset(0, 0x10000);
    src.depth = 0x201;
    gEmitRayBurst(&src, 0, 0, 4, 0x10, 0x200, 0);
    FCK_EQ((s32)gPacketCursor, (s32)(base + 5 * 0x1C), "rb: depth - near = 1 -> header + 4 packets");
    FCK_EQ(RDU32(base, 0), 0x01000001, "rb: depth key 1");
    src.depth = 0x400;

    /* Requested count above 4 * radius is capped: radius 2 -> 8 spikes. */
    gPacketCursor = base;
    sc_reset(0, 0x10000);
    gEmitRayBurst(&src, 0, 0, 40, 4, 0x200, 0);
    FCK_EQ((s32)gPacketCursor, (s32)(base + 9 * 0x1C), "rb: count capped at 4 * radius");
    FCK_EQ(sc_calls, 2, "rb: capped count -> 2 sin/cos");

    /* Cursor at the limit: packets are written but the cursor never moves. */
    gPacketCursor = base;
    gPacketLimit = base;
    sc_reset(0, 0x10000);
    gEmitRayBurst(&src, 0, 0, 4, 0x10, 0x200, 0);
    FCK_EQ((s32)gPacketCursor, (s32)base, "rb: full packet area -> cursor not advanced");
}

/* ========================================================================
 * 80015830 line quad
 * ======================================================================== */
typedef struct { s16 x, y, z, pad; } LqPoint;
typedef struct { u8 pad[4]; u8 a, b, c, d; } LqIn;
typedef struct { s16 xy[8]; s32 z; } LqOut;
s32 gBuildLineQuad(LqIn *in, LqOut *out, s16 *width) __asm__("func_80015830");
LqPoint *gPartVertexBuffer __asm__("D_8009F578");
u8 *gClipFlagBuffer __asm__("D_8009F57C");
static LqPoint lq_pts[4];
static u8 lq_clip[4];
static s16 lq_width[8];

static s32 lq_abs(s32 v) { return v < 0 ? -v : v; }

static void lq_setup(void)
{
    int i;
    for (i = 0; i < 4; i++) lq_clip[i] = 0;
    for (i = 0; i < 8; i++) lq_width[i] = 0;
    gPartVertexBuffer = lq_pts;
    gClipFlagBuffer = lq_clip;
}

static void test_80015830(void)
{
    LqIn in;
    LqOut out;
    s32 r, ox, oy, dot, len2;

    /* @leaf 80015830 math corners = endpoint +/- edge normal scaled to the per-end half width; half width = w * 0x200 / z (perspective, larger z -> smaller); depth = mean z; axis-aligned edges give exact offsets, the diagonal is checked against the Euclidean length (perpendicular, |offset| within 4% of w) rather than the unit's octagonal approximation */
    LK_FUNC("func_80015830");

    lq_setup();
    in.a = 1; in.b = 2; in.c = 3; in.d = 5;
    lq_width[3 + 2] = 16;       /* first end: 16 * 0x200 / 0x200 = 16 */
    lq_width[5 + 2] = 16;       /* second end at z 0x400: 16 * 0x200 / 0x400 = 8 */
    /* horizontal edge (0,0,0x200) -> (1024,0,0x400) */
    lq_pts[1].x = 0; lq_pts[1].y = 0; lq_pts[1].z = 0x200;
    lq_pts[2].x = 1024; lq_pts[2].y = 0; lq_pts[2].z = 0x400;
    lk_fill(&out, 0xAA, sizeof(out));
    r = gBuildLineQuad(&in, &out, lq_width);
    FCK_EQ(r, 1, "lq: visible endpoints -> 1");
    FCK_EQ(out.z, 0x300, "lq: depth is the mean of both z");
    FCK_EQ(out.xy[0], 0, "lq: h corner0 x on the endpoint");
    FCK_EQ(lq_abs(out.xy[1]), 16, "lq: h corner0 y offset = first half width");
    FCK_EQ(out.xy[3], -out.xy[1], "lq: h corner1 mirrored across the edge");
    FCK_EQ(out.xy[4], 1024, "lq: h corner2 x on the second endpoint");
    FCK_EQ(lq_abs(out.xy[5]), 8, "lq: h corner2 y offset = second half width (1/z)");
    FCK_EQ(out.xy[7], -out.xy[5], "lq: h corner3 mirrored");
    FCK_CHECK((out.xy[1] > 0) == (out.xy[5] > 0), "lq: both ends offset to the same side");

    /* vertical edge, both z 0x200 */
    lq_pts[2].x = 0; lq_pts[2].y = 1024; lq_pts[2].z = 0x200;
    lq_width[7] = 8;            /* 8 * 0x200 / 0x200 = 8 */
    r = gBuildLineQuad(&in, &out, lq_width);
    FCK_EQ(out.xy[1], 0, "lq: v corner0 y on the endpoint");
    FCK_EQ(lq_abs(out.xy[0]), 16, "lq: v corner0 x offset 16");
    FCK_EQ(out.xy[2], -out.xy[0], "lq: v corner1 mirrored");
    FCK_EQ(lq_abs(out.xy[4]), 8, "lq: v corner2 x offset 8");
    FCK_EQ(out.xy[5], 1024, "lq: v corner2 y on the endpoint");
    FCK_EQ(out.z, 0x200, "lq: v depth");

    /* 3-4-5 diagonal (0,0)->(600,800), width 100 at both ends */
    lq_pts[2].x = 600; lq_pts[2].y = 800; lq_pts[2].z = 0x200;
    lq_width[5] = 100; lq_width[7] = 100;
    r = gBuildLineQuad(&in, &out, lq_width);
    ox = out.xy[0]; oy = out.xy[1];
    dot = ox * 600 + oy * 800;
    len2 = ox * ox + oy * oy;
    FCK_CHECK(lq_abs(dot) <= 2 * 1000, "lq: diagonal offset perpendicular to the edge");
    FCK_CHECK(len2 >= 96 * 96 && len2 <= 104 * 104, "lq: diagonal offset length ~ half width");
    FCK_EQ(out.xy[2], -ox, "lq: diagonal corner1 mirrored x");
    FCK_EQ(out.xy[4] - 600, ox, "lq: equal widths -> same offset at the far end");

    /* zero-length edge: no offset */
    lq_pts[2] = lq_pts[1];
    r = gBuildLineQuad(&in, &out, lq_width);
    FCK_EQ(r, 1, "lq: degenerate edge still written");
    FCK_EQ(out.xy[0] | out.xy[1] | out.xy[2] | out.xy[3], 0, "lq: degenerate edge -> zero offsets");

    /* clip bit 0 on either endpoint rejects, other clip bits do not */
    lq_pts[2].x = 1024; lq_pts[2].y = 0;
    lq_clip[2] = 1;
    lk_fill(&out, 0xAA, sizeof(out));
    r = gBuildLineQuad(&in, &out, lq_width);
    FCK_EQ(r, 0, "lq: clipped second endpoint -> 0");
    FCK_EQ((u16)out.xy[0], 0xAAAA, "lq: rejected quad leaves output untouched");
    lq_clip[2] = 0; lq_clip[1] = 1;
    FCK_EQ(gBuildLineQuad(&in, &out, lq_width), 0, "lq: clipped first endpoint -> 0");
    lq_clip[1] = 2; lq_clip[2] = 0xFE;
    FCK_EQ(gBuildLineQuad(&in, &out, lq_width), 1, "lq: only bit 0 rejects");
}

/* ========================================================================
 * 80021554 sound storage init
 * ======================================================================== */
/* D_800A07A8..D_800A07B4 are one 16-byte block in retail: the struct view
 * {bankCount, banks, channelCount, channels} and the single-word labels
 * overlap. Lay them out contiguously so both views alias as in retail. */
__asm__(".data\n"
        "\t.balign 4\n"
        "\t.globl D_800A07A8\nD_800A07A8:\t.word 0\n"
        "\t.globl D_800A07AC\nD_800A07AC:\t.word 0\n"
        "\t.globl D_800A07B0\nD_800A07B0:\t.word 0\n"
        "\t.globl D_800A07B4\nD_800A07B4:\t.word 0\n"
        "\t.text\n");
typedef struct { s32 bankCount; s32 banks; s32 channelCount; s32 channels; } SsSystem;
extern SsSystem gSoundSystem __asm__("D_800A07A8");
s32 gInitSoundStorage(s32 *arguments) __asm__("func_80021554");

static s32 cfg_keys[8], cfg_vals[8], cfg_n, cfg_calls, cfg_seen[8], cfg_idx_nonnull;
s32 mock_find_config(s32 *table, s32 key, s32 *value, s32 *index) __asm__("func_80021740");
s32 mock_find_config(s32 *table, s32 key, s32 *value, s32 *index)
{
    int i;
    (void)table;
    if (cfg_calls < 8) cfg_seen[cfg_calls] = key;
    cfg_calls++;
    if (index) cfg_idx_nonnull++;
    for (i = 0; i < cfg_n; i++)
        if (cfg_keys[i] == key) { *value = cfg_vals[i]; return 1; }
    return 0;
}
static s32 clear_storage_calls;
void mock_clear_storage(void) __asm__("func_800216D8");
void mock_clear_storage(void) { clear_storage_calls++; }

static u8 ss_banks[32];
static u8 ss_chan[0x18 * 28 + 32];

static void test_80021554(void)
{
    s32 args[4] = {0, 0, 0, 0};
    s32 r, i, ok;

    /* @leaf 80021554 source src/game/80021554.c: keys 4/5/8/9 (bank count, channel count, bank slot buffer, channel table) looked up in order; defaults 1 bank / 24 channels; banks*4 and channels*28 bytes cleared; each 28-byte channel record gets its index at +0xC; returns 0 */
    LK_FUNC("func_80021554");

    /* all keys given */
    lk_fill(ss_banks, 0xEE, sizeof(ss_banks));
    lk_fill(ss_chan, 0xEE, sizeof(ss_chan));
    gSoundSystem.bankCount = 0x55; gSoundSystem.banks = 0x55;
    gSoundSystem.channelCount = 0x55; gSoundSystem.channels = 0x55;
    cfg_n = 4; cfg_calls = 0; cfg_idx_nonnull = 0; clear_storage_calls = 0;
    cfg_keys[0] = 4; cfg_vals[0] = 3;
    cfg_keys[1] = 5; cfg_vals[1] = 5;
    cfg_keys[2] = 8; cfg_vals[2] = (s32)ss_banks;
    cfg_keys[3] = 9; cfg_vals[3] = (s32)ss_chan;
    r = gInitSoundStorage(args);
    FCK_EQ(r, 0, "ss: returns 0");
    FCK_EQ(cfg_calls, 4, "ss: four lookups");
    FCK_EQ(cfg_seen[0], 4, "ss: key 4 first"); FCK_EQ(cfg_seen[1], 5, "ss: key 5 second");
    FCK_EQ(cfg_seen[2], 8, "ss: key 8 third"); FCK_EQ(cfg_seen[3], 9, "ss: key 9 fourth");
    FCK_EQ(cfg_idx_nonnull, 0, "ss: no index out-pointer");
    FCK_EQ(gSoundSystem.bankCount, 3, "ss: bank count stored");
    FCK_EQ(gSoundSystem.banks, (s32)ss_banks, "ss: bank slot buffer stored");
    FCK_EQ(gSoundSystem.channelCount, 5, "ss: channel count stored");
    FCK_EQ(gSoundSystem.channels, (s32)ss_chan, "ss: channel table stored");
    ok = 1;
    for (i = 0; i < 12; i++) if (ss_banks[i] != 0) ok = 0;
    FCK_CHECK(ok, "ss: 3 banks * 4 bytes cleared");
    FCK_EQ(ss_banks[12], 0xEE, "ss: byte after the bank slots untouched");
    for (i = 0; i < 5; i++) FCK_EQ(RD32(ss_chan, i * 28 + 0xC), i, "ss: channel index at +0xC");
    FCK_EQ(RD32(ss_chan, 0), 0, "ss: channel 0 word 0 cleared");
    FCK_EQ(RD32(ss_chan, 4 * 28 + 0x18), 0, "ss: last channel last word cleared");
    FCK_EQ(ss_chan[5 * 28], 0xEE, "ss: byte after 5 channels untouched");
    FCK_EQ(RDU32(ss_chan, 5 * 28 + 0xC), 0xEEEEEEEE, "ss: no index written past the last channel");
    FCK_EQ(clear_storage_calls, 0, "ss: success path never clears storage");

    /* no keys: defaults 1 bank / 24 channels on the already-set buffers,
     * counts not written */
    lk_fill(ss_banks, 0xEE, sizeof(ss_banks));
    lk_fill(ss_chan, 0xEE, sizeof(ss_chan));
    gSoundSystem.bankCount = 0x77; gSoundSystem.banks = (s32)ss_banks;
    gSoundSystem.channelCount = 0x77; gSoundSystem.channels = (s32)ss_chan;
    cfg_n = 0; cfg_calls = 0;
    r = gInitSoundStorage(args);
    FCK_EQ(r, 0, "ss: defaults return 0");
    FCK_EQ(gSoundSystem.bankCount, 0x77, "ss: absent key 4 leaves bank count");
    FCK_EQ(gSoundSystem.channelCount, 0x77, "ss: absent key 5 leaves channel count");
    FCK_EQ(RD32(ss_banks, 0), 0, "ss: default one bank cleared");
    FCK_EQ(ss_banks[4], 0xEE, "ss: only one bank cleared");
    FCK_EQ(RD32(ss_chan, 23 * 28 + 0xC), 23, "ss: default 24 channels indexed");
    FCK_EQ(ss_chan[24 * 28], 0xEE, "ss: byte after 24 channels untouched");
    FCK_EQ(RDU32(ss_chan, 24 * 28 + 0xC), 0xEEEEEEEE, "ss: no index for a 25th channel");
}

/* ========================================================================
 * 8002DFC4 type-27 entity setup
 * ======================================================================== */
typedef struct { s32 x, y, z; } V3;
typedef struct {
    u8 pad00[0x34];
    u8 unk34; s8 unk35; s16 unk36; u16 unk38; s16 unk3A; s16 unk3C; s16 unk3E;
    s16 unk40; s16 unk42; s16 unk44;
    u8 unk46, unk47, unk48, unk49, unk4A, unk4B, unk4C, pad4D;
} E27Src;
typedef struct { u8 pad00[0x10]; u8 *unk10; } E27Sub;
typedef struct {
    u8 pad00[0x20]; u8 unk20; u8 unk21; u8 pad22[2]; u32 flags; E27Sub *unk28;
    u8 pad2C[0x24]; V3 pos; u8 pad5C[0x1C]; V3 off; s32 unk84; u8 pad88[0x40];
    u8 unkC8[0x14]; u8 unkDC; u8 unkDD; u16 unkDE; u16 unkE0; u16 unkE2; u16 unkE4;
    u8 unkE6; u8 padE7; s32 unkE8; s32 unkEC; s32 unkF0; V3 tmp; V3 unk100;
    u8 unk10C, unk10D, unk10E, unk10F, unk110, unk111, unk112, unk113;
} E27Obj;
typedef struct { u8 pad00[8]; E27Src *unk8; } E27Arg;
s32 gSetupType27Entity(E27Obj *obj, E27Arg *arg) __asm__("func_8002DFC4");

static s32 surf_ret, len_ret;
s32 mock_surface(V3 *pos, s32 sel) __asm__("func_80067684");
s32 mock_surface(V3 *pos, s32 sel) { fck_record("surface", (s32)pos, sel, pos->x); return surf_ret; }
void mock_rotmat(s32 yaw, s32 pitch, s32 roll, void *m) __asm__("func_800188D0");
void mock_rotmat(s32 yaw, s32 pitch, s32 roll, void *m) { fck_record("rotmat", yaw, pitch | roll, (s32)m); }
s32 mock_veclen(s32 *v) __asm__("func_8001A100");
s32 mock_veclen(s32 *v) { fck_record("veclen", (s32)v, v[0], v[2]); return len_ret; }
void mock_animclip(u8 *d, void *t, u8 f, s32 i) __asm__("func_80044988");
void mock_animclip(u8 *d, void *t, u8 f, s32 i) { fck_record("animclip", (s32)d, (s32)t, (f << 8) | i); }
void mock_attack_slot(void *in, void *out) __asm__("func_80060668");
void mock_attack_slot(void *in, void *out) { fck_record("attack_slot", (s32)in, (s32)out, 0); }

static E27Obj e27_obj;
static E27Src e27_src;
static E27Sub e27_sub;
static u8 e27_clip[4];

static void e27_setup(void)
{
    lk_fill(&e27_obj, 0, sizeof(e27_obj));
    lk_fill(&e27_src, 0, sizeof(e27_src));
    e27_obj.unk28 = &e27_sub;
    e27_sub.unk10 = 0;
    e27_obj.pos.x = 1000; e27_obj.pos.y = 0; e27_obj.pos.z = 3000;
    surf_ret = 0x500; len_ret = 0x1234;
    fck_log_reset();
}

static void test_8002DFC4(void)
{
    E27Arg arg;
    s32 r;
    struct FckCall c;

    /* @leaf 8002DFC4 source src/game/8002DFC4.c: surface selector 0->-1, 1->-2, n->n-2; y = surface + (unk36 << 4); speed = (unk40 << 4) / 300 forced >= 1 when positive; rates (v << 12) / 218 + 1; offset (x,y,z) = (3C,3E,3A) << 4; flag 8 starts at the far end with the negated offset; flag 2 -> obj flag 0x80; flag 0x40 moves unk21 to unk113 */
    LK_FUNC("func_8002DFC4");

    e27_setup();
    arg.unk8 = &e27_src;
    e27_src.unk34 = 0; e27_src.unk35 = 0; e27_src.unk36 = 2; e27_src.unk38 = 0x8400;
    e27_src.unk3A = 3; e27_src.unk3C = 1; e27_src.unk3E = -2;
    e27_src.unk40 = 10; e27_src.unk42 = 109; e27_src.unk44 = 218;
    e27_src.unk46 = 1; e27_src.unk47 = 2; e27_src.unk48 = 3; e27_src.unk49 = 4;
    e27_src.unk4A = 5; e27_src.unk4B = 6; e27_src.unk4C = 7;
    e27_obj.unk21 = 0x99; e27_obj.flags = 0x1;
    e27_obj.unk84 = 0x55; e27_obj.unkE6 = 0x55; e27_obj.unkEC = 0x55; e27_obj.unkDC = 0x55;
    r = gSetupType27Entity(&e27_obj, &arg);
    FCK_EQ(r, 0, "e27: returns 0");
    c = fck_call_n("surface", 0);
    FCK_EQ(c.a0, (s32)&e27_obj.pos, "e27: surface query on obj->pos");
    FCK_EQ(c.a1, -1, "e27: selector 0 -> -1");
    FCK_EQ(e27_obj.pos.y, 0x500 + 0x20, "e27: y = surface + (2 << 4)");
    FCK_EQ(e27_obj.unkE4, 0x8400, "e27: heading copied unsigned");
    c = fck_call_n("rotmat", 0);
    FCK_EQ(c.a0, (s16)0x8400, "e27: rotation yaw sign-extended");
    FCK_EQ(c.a1, 0, "e27: pitch/roll zero");
    FCK_EQ(c.a2, (s32)&e27_obj + 0x2C, "e27: matrix at obj + 0x2C");
    FCK_EQ(e27_obj.unkE8, 1, "e27: speed (10 << 4) / 300 = 0 raised to 1");
    FCK_EQ(e27_obj.unk84, 0, "e27: unk84 cleared");
    FCK_EQ(e27_obj.unkDE, (109 << 12) / 218 + 1, "e27: rate DE");
    FCK_EQ(e27_obj.unkE2, 2049, "e27: rate E2 = DE");
    FCK_EQ(e27_obj.unkE0, 4097, "e27: rate E0 = (218 << 12) / 218 + 1");
    FCK_EQ(e27_obj.unkE6, 0, "e27: E6 cleared");
    FCK_EQ(e27_obj.unk10C + e27_obj.unk10D * 10 + e27_obj.unk10E * 100, 321, "e27: bytes 46..48 copied");
    FCK_EQ(e27_obj.unk10F, 4, "e27: byte 49"); FCK_EQ(e27_obj.unk110, 5, "e27: byte 4A");
    FCK_EQ(e27_obj.unk111, 7, "e27: byte 4B | 1"); FCK_EQ(e27_obj.unk112, 7, "e27: byte 4C");
    FCK_EQ(e27_obj.off.x, 16, "e27: off.x = 3C << 4");
    FCK_EQ(e27_obj.off.y, -32, "e27: off.y = 3E << 4");
    FCK_EQ(e27_obj.off.z, 48, "e27: off.z = 3A << 4");
    FCK_EQ(e27_obj.tmp.x, 1000, "e27: no flag 8 -> tmp = pos");
    FCK_EQ(e27_obj.tmp.y, 0x520, "e27: tmp.y = adjusted pos.y");
    FCK_EQ(e27_obj.unk100.z, 48, "e27: unk100 = off");
    FCK_EQ(e27_obj.unkEC, 0, "e27: EC cleared");
    FCK_EQ(e27_obj.unkF0, 0x1234, "e27: F0 = offset length");
    FCK_EQ(fck_call_n("veclen", 0).a0, (s32)&e27_obj.off, "e27: length of obj->off");
    FCK_EQ(e27_obj.unkDC, 0, "e27: DC cleared");
    FCK_EQ(e27_obj.flags, 1, "e27: no flag 2 -> flags unchanged");
    FCK_EQ(fck_calls_to("animclip"), 0, "e27: no anim data -> no clip table");
    c = fck_call_n("attack_slot", 0);
    FCK_EQ(c.a0, (s32)&e27_src + 0x24, "e27: scheduler input src + 0x24");
    FCK_EQ(c.a1, (s32)&e27_obj + 0xA0, "e27: scheduler slot obj + 0xA0");
    FCK_EQ(e27_obj.unk21, 0x99, "e27: no flag 0x40 -> unk21 kept");

    /* flags 8 | 2 | 0x40, selector 1 and 7, speed 300, anim data present */
    e27_setup();
    e27_src.unk34 = 0x4A; e27_src.unk35 = 1;
    e27_src.unk3A = 3; e27_src.unk3C = 1; e27_src.unk3E = -2;
    e27_src.unk40 = 300;
    e27_sub.unk10 = e27_clip;
    e27_obj.unk21 = 0x99;
    gSetupType27Entity(&e27_obj, &arg);
    FCK_EQ(fck_call_n("surface", 0).a1, -2, "e27: selector 1 -> -2");
    FCK_EQ(e27_obj.unkE8, 16, "e27: speed (300 << 4) / 300");
    FCK_EQ(e27_obj.unkDD, 0x4A, "e27: flags byte copied");
    FCK_EQ(e27_obj.pos.x, 1016, "e27: flag 8 -> start at pos + off (x)");
    FCK_EQ(e27_obj.pos.y, 0x500 - 32, "e27: flag 8 -> start at pos + off (y)");
    FCK_EQ(e27_obj.pos.z, 3048, "e27: flag 8 -> start at pos + off (z)");
    FCK_EQ(e27_obj.tmp.z, 3048, "e27: tmp = moved pos");
    FCK_EQ(e27_obj.off.x, -16, "e27: flag 8 negates off.x");
    FCK_EQ(e27_obj.off.y, 32, "e27: flag 8 negates off.y");
    FCK_EQ(e27_obj.unk100.z, -48, "e27: unk100 = negated off");
    FCK_EQ(e27_obj.flags, 0x80, "e27: flag 2 -> obj flag 0x80");
    c = fck_call_n("animclip", 0);
    FCK_EQ(c.a0, (s32)e27_clip, "e27: clip table from sub->unk10");
    FCK_EQ(c.a1, (s32)&e27_obj + 0xC8, "e27: clip track obj + 0xC8");
    FCK_EQ(c.a2, 0x100, "e27: forward 1, index 0");
    FCK_EQ(e27_obj.unk21, 0, "e27: flag 0x40 clears unk21");
    FCK_EQ(e27_obj.unk113, 0x99, "e27: flag 0x40 keeps old unk21 in unk113");

    e27_setup();
    e27_src.unk35 = 7;
    e27_src.unk40 = -30;
    gSetupType27Entity(&e27_obj, &arg);
    FCK_EQ(fck_call_n("surface", 0).a1, 5, "e27: selector 7 -> 5");
    FCK_EQ(e27_obj.unkE8, -1, "e27: negative speed not forced to 1");

    e27_setup();
    e27_src.unk40 = 0;
    gSetupType27Entity(&e27_obj, &arg);
    FCK_EQ(e27_obj.unkE8, 0, "e27: zero speed stays 0 (only positive is raised)");
}

/* ========================================================================
 * 80040734 effect light spawn
 * ======================================================================== */
typedef struct { u8 unk00[9]; u8 red, green, blue; } ElLight;
typedef struct {
    s32 unk00, unk04; V3 position; u8 kind, slot; s16 height, width, depth, angle, unk1E, unk20;
    s8 unk22; u8 red, green, blue; u8 unk26[2]; s16 scale; u8 unk2A[6];
} ElReq;
typedef struct {
    u8 unk00[0x0C]; s16 unk0C; s8 part; u8 unk0F; u8 unk10[2]; s16 unk12; u8 unk14[2];
    s16 unk16; u8 unk18[2]; u16 flags1A; void *sub; u8 unk20[0x14]; s32 matrix[9];
    u8 unk58[0x0C]; s8 unk64; u8 pad65; u16 unk66; u8 unk68[4]; ElLight *light;
    s16 point70[4]; s16 unk78, unk7A, unk7C; u8 unk7E[9]; u8 unk87, unk88, unk89;
} ElObj;
u8 gSpawnLightForEffect(ElObj *object, V3 *position) __asm__("func_80040734");
s32 gLightObjectCount __asm__("D_8009F318");
static ElReq el_req;
static ElLight *el_ret_light;
static s32 el_ret_len, el_id;
s32 mock_spawn_light(s32 id, ElReq *req, ElLight **res) __asm__("func_8005A6A4");
s32 mock_spawn_light(s32 id, ElReq *req, ElLight **res)
{
    el_id = id;
    el_req = *req;
    *res = el_ret_light;
    return el_ret_len;
}

static void test_80040734(void)
{
    ElObj o;
    ElLight light;
    V3 pos;
    u8 r;

    /* @leaf 80040734 source src/game/80040734.c: light request footprint 0x50 below size 0x28 else 2 * size, kind | 0x40, height = y >> 4, angle/scale/colour from the object, slot -1; null light sets bit 0 of +0x0F and returns 0; first spawn records the duration in +0x0C/+0x16; the light gets the object colour */
    LK_FUNC("func_80040734");

    lk_fill(&o, 0, sizeof(o));
    lk_fill(&light, 0, sizeof(light));
    lk_fill(&el_req, 0x5A, sizeof(el_req));
    gLightObjectCount = 0;
    o.unk12 = 0x27; o.unk64 = 9; o.unk66 = 0x0105; o.unk78 = 0x300; o.unk7A = 0x123;
    o.unk7C = -4; o.unk87 = 10; o.unk88 = 20; o.unk89 = 30; o.unk16 = 0; o.unk0F = 0x80;
    pos.x = 100; pos.y = 0x345; pos.z = -7;
    el_ret_light = &light; el_ret_len = 77;
    r = gSpawnLightForEffect(&o, &pos);
    FCK_EQ(r, 1, "el: spawned -> 1");
    FCK_EQ(el_id, 9, "el: light type id from +0x64");
    FCK_EQ(el_req.width, 0x50, "el: size < 0x28 -> width 0x50");
    FCK_EQ(el_req.depth, 0x50, "el: size < 0x28 -> depth 0x50");
    FCK_EQ(el_req.position.x, 100, "el: position x copied");
    FCK_EQ(el_req.position.z, -7, "el: position z copied");
    FCK_EQ(el_req.kind, 0x45, "el: kind = low byte of unk66 | 0x40");
    FCK_EQ(el_req.height, 0x34, "el: height = y >> 4");
    FCK_EQ(el_req.angle, 0x123, "el: angle");
    FCK_EQ(el_req.unk1E, -4, "el: unk1E");
    FCK_EQ(el_req.unk22, -1, "el: slot marker -1");
    FCK_EQ(el_req.scale, 0x300, "el: scale");
    FCK_EQ(el_req.unk20, 0, "el: unk20 cleared");
    FCK_EQ(el_req.red + el_req.green * 256 + el_req.blue * 65536, 10 + 20 * 256 + 30 * 65536, "el: request colour");
    FCK_EQ((s32)o.light, (s32)&light, "el: light pointer stored");
    FCK_EQ(o.unk0C, 77, "el: first spawn stores duration in +0x0C");
    FCK_EQ(o.unk16, 77, "el: first spawn stores duration in +0x16");
    FCK_EQ(light.red + light.green * 256 + light.blue * 65536, 10 + 20 * 256 + 30 * 65536, "el: light gets object colour");
    FCK_EQ(o.unk0F, 0x80, "el: success leaves +0x0F");

    /* size 0x28 -> 2 * size; duration already set is kept */
    o.unk12 = 0x28; o.unk16 = 5; o.unk0C = 6;
    gSpawnLightForEffect(&o, &pos);
    FCK_EQ(el_req.width, 0x50, "el: size 0x28 -> 0x50 (2 * size)");
    o.unk12 = 0x40;
    gSpawnLightForEffect(&o, &pos);
    FCK_EQ(el_req.depth, 0x80, "el: size 0x40 -> 0x80");
    FCK_EQ(o.unk16, 5, "el: existing duration kept (+0x16)");
    FCK_EQ(o.unk0C, 6, "el: existing duration kept (+0x0C)");

    /* no light */
    el_ret_light = 0;
    o.unk0F = 0x80;
    r = gSpawnLightForEffect(&o, &pos);
    FCK_EQ(r, 0, "el: no light -> 0");
    FCK_EQ((s32)o.light, 0, "el: null light stored");
    FCK_EQ(o.unk0F, 0x81, "el: no light sets bit 0 of +0x0F");
}

/* ========================================================================
 * 8004F188 path-mover commands
 * ======================================================================== */
typedef struct {
    void *entries; u8 unk04[0x2C]; s16 unk30; s16 unk32; u8 unk34[8]; u32 flags; u8 unk40[2]; s16 unk42;
} PmObj;
typedef struct { u8 unk00[0x0C]; u8 cmd[4][2]; u8 unk14[0x70]; s32 unk84; } PmScript;
void gProcessPathMoverCommands(PmObj *object, PmScript *script) __asm__("func_8004F188");

/* func_8002A5B4 removes command `index` from the 4-slot queue (the caller's
 * contract: later commands move up, the freed tail slot becomes op 0). */
void mock_remove_cmd(PmScript *s, s32 index) __asm__("func_8002A5B4");
void mock_remove_cmd(PmScript *s, s32 index)
{
    int i;
    fck_record("remove_cmd", index, s->cmd[index][0], s->cmd[index][1]);
    for (i = index; i < 3; i++) { s->cmd[i][0] = s->cmd[i + 1][0]; s->cmd[i][1] = s->cmd[i + 1][1]; }
    s->cmd[3][0] = 0; s->cmd[3][1] = 0;
}

static void pm_cmds(PmScript *s, int o0, int a0, int o1, int a1, int o2, int a2, int o3, int a3)
{
    s->cmd[0][0] = o0; s->cmd[0][1] = a0; s->cmd[1][0] = o1; s->cmd[1][1] = a1;
    s->cmd[2][0] = o2; s->cmd[2][1] = a2; s->cmd[3][0] = o3; s->cmd[3][1] = a3;
}

static void test_8004F188(void)
{
    PmObj o;
    PmScript s;
    u8 entries[12 * 4];

    /* @leaf 8004F188 source src/game/8004F188.c: ops 1-7/10-15 set the motion mode (op - 1) into flag bits 24..27 and the parameter into +0x42; op 19 pauses (arg 0: flag 0x40, script +0x84 = 0) or resumes (copies +0x30 to +0x32); op 21/20 set/clear bit 1 of 12-byte entry arg at +4; op 22 speed = (arg * 41 * 256 / 300) >> 4; handled ops are removed, unknown ops stay; op 0 ends the queue */
    LK_FUNC("func_8004F188");

    lk_fill(&o, 0, sizeof(o));
    lk_fill(&s, 0, sizeof(s));
    lk_fill(entries, 0, sizeof(entries));
    o.entries = entries;
    o.flags = 0xA5000040; o.unk30 = 0x77; o.unk32 = 0;
    entries[2 * 12 + 4] = 0x01; entries[1 * 12 + 4] = 0x07;
    s.unk84 = 0x55;
    fck_log_reset();
    pm_cmds(&s, 3, 7, 19, 1, 21, 2, 20, 1);
    gProcessPathMoverCommands(&o, &s);
    FCK_EQ(o.unk42, 7, "pm: op 3 parameter into +0x42");
    FCK_EQ((s32)o.flags, (s32)0xA2000000, "pm: op 3 -> mode 2 in bits 24..27, op 19 arg 1 clears 0x40");
    FCK_EQ(o.unk32, 0x77, "pm: resume copies +0x30 to +0x32");
    FCK_EQ(entries[2 * 12 + 4], 0x03, "pm: op 21 sets bit 1 of entry 2");
    FCK_EQ(entries[1 * 12 + 4], 0x05, "pm: op 20 clears bit 1 of entry 1");
    FCK_EQ(fck_calls_to("remove_cmd"), 4, "pm: four handled commands removed");
    FCK_EQ(fck_call_n("remove_cmd", 3).a0, 0, "pm: always removes the head slot");
    FCK_EQ(s.unk84, 0x55, "pm: resume leaves script +0x84");

    /* pause, speed and an unknown op that stays queued */
    o.flags = 0x0F000000;
    fck_log_reset();
    pm_cmds(&s, 8, 9, 19, 0, 22, 30, 15, 0x80);
    gProcessPathMoverCommands(&o, &s);
    FCK_EQ((s32)o.flags, (s32)0x0E000040, "pm: op 19 arg 0 sets 0x40, op 15 -> mode 14");
    FCK_EQ(s.unk84, 0, "pm: pause clears script +0x84");
    FCK_EQ(o.unk30, 65, "pm: op 22 speed (30 * 41 * 256 / 300) >> 4");
    FCK_EQ(o.unk42, 0x80, "pm: op 15 parameter");
    FCK_EQ(fck_calls_to("remove_cmd"), 3, "pm: unknown op 8 not removed");
    FCK_EQ(fck_call_n("remove_cmd", 0).a0, 1, "pm: removal skips the unknown slot");
    FCK_EQ(s.cmd[0][0], 8, "pm: unknown op 8 still queued");

    /* four unknown ops: the walk stops after slot 3 and never reads the
     * byte after the queue (here a valid-looking op 1 / arg 0x33) */
    o.unk42 = 0;
    s.unk14[0] = 1; s.unk14[1] = 0x33;
    fck_log_reset();
    pm_cmds(&s, 8, 0, 9, 0, 16, 0, 23, 0);
    gProcessPathMoverCommands(&o, &s);
    FCK_EQ(fck_calls_to("remove_cmd"), 0, "pm: unknown ops all stay");
    FCK_EQ(o.unk42, 0, "pm: queue is four slots, the next byte is not a command");

    /* op 0 terminates: nothing after it runs */
    o.unk42 = 0;
    fck_log_reset();
    pm_cmds(&s, 0, 0, 1, 5, 1, 5, 1, 5);
    gProcessPathMoverCommands(&o, &s);
    FCK_EQ(fck_calls_to("remove_cmd"), 0, "pm: op 0 first -> nothing processed");
    FCK_EQ(o.unk42, 0, "pm: commands after op 0 ignored");
}

/* ========================================================================
 * 8005D690 weapon dome effect update
 * ======================================================================== */
typedef struct { u8 pad0[0x16]; u8 kind; u8 pad17; u8 flags; u8 pad19[0x93]; void *data; } WdObj;
typedef struct {
    WdObj *obj; WdObj *owner; u8 pad08[4]; s32 time; s32 value; u8 pad14[0x38];
    s16 low, high, other; u16 flags; s32 index;
} WdEntry;
typedef struct { s16 other, low, high; } WdParams;
typedef struct { u8 pad[0xA]; s16 value, other, low, high; } WdExtra;
/* four slots in retail; a fifth harness word holds a decoy that only an
 * over-long search would find */
WdEntry *gDomeEffectBlock[5] __asm__("D_800A22F0");
s32 gWeaponEffectUpdate(WdObj *o, WdParams *p, WdExtra *x) __asm__("func_8005D690");

static void test_8005D690(void)
{
    WdObj owner, fx;
    WdEntry e, other, decoy;
    WdParams prm;
    WdExtra ex;
    s16 data[0x60];
    s32 r;

    /* @leaf 8005D690 source src/game/8005D690.c: finds the owner's block among the four dome slots or allocates one (alloc failure -> 0); a fresh block takes low/high/other from params with high < low replaced by low * 8 / 5; non-player kinds re-arm for 120 ticks and set bit 0; the player toggles bit 0 when the timer expired, re-arms 60 ticks and, while bit 0 is set, copies value/range from extra and shows/hides the effect object by the ammo word at data + 0x8A + 2 * index (returns 1) */
    LK_FUNC("func_8005D690");

    lk_fill(&owner, 0, sizeof(owner));
    lk_fill(&fx, 0, sizeof(fx));
    lk_fill(&e, 0, sizeof(e));
    lk_fill(&other, 0, sizeof(other));
    gDomeEffectBlock[0] = 0; gDomeEffectBlock[1] = &other; gDomeEffectBlock[2] = 0; gDomeEffectBlock[3] = 0;
    gDomeEffectBlock[4] = &decoy;
    other.owner = &fx;
    lk_fill(&decoy, 0, sizeof(decoy));
    decoy.owner = &owner;
    gFrameTime = 1000;
    prm.other = 3; prm.low = 10; prm.high = 5;

    /* not found, alloc fails */
    dome_ret = 0;
    fck_log_reset();
    r = gWeaponEffectUpdate(&owner, &prm, 0);
    FCK_EQ(r, 0, "wd: alloc failure -> 0");
    FCK_EQ(fck_calls_to("alloc_dome"), 1, "wd: one alloc attempt");
    FCK_EQ(fck_call_n("alloc_dome", 0).a1, 0, "wd: alloc slot argument 0");

    /* not found, allocated, kind 0 */
    dome_ret = &e;
    e.flags = 0x10;
    owner.kind = 0;
    fck_log_reset();
    r = gWeaponEffectUpdate(&owner, &prm, &ex);
    FCK_EQ(r, 0, "wd: non-player -> 0");
    FCK_EQ(e.low, 10, "wd: low from params");
    FCK_EQ(e.high, 16, "wd: high < low -> low * 8 / 5");
    FCK_EQ(e.other, 3, "wd: other from params");
    FCK_EQ(e.time, 1120, "wd: non-player re-armed 120 ticks");
    FCK_EQ(e.flags, 0x11, "wd: non-player sets bit 0");

    /* found in slot 2, player, timer expired, bit 0 clear -> set, ammo > 0 */
    gDomeEffectBlock[2] = &e;
    e.owner = &owner; e.obj = &fx; e.time = 999; e.flags = 0x10; e.index = 3; e.value = 0;
    owner.kind = 1; owner.data = data;
    data[(0x8A + 3 * 2) / 2] = 5;
    fx.flags = 0x0F;
    ex.value = 42; ex.other = 7; ex.low = 100; ex.high = 200;
    fck_log_reset();
    r = gWeaponEffectUpdate(&owner, &prm, &ex);
    FCK_EQ(r, 1, "wd: active player effect -> 1");
    FCK_EQ(fck_calls_to("alloc_dome"), 0, "wd: existing block not reallocated");
    FCK_EQ(e.flags, 0x15, "wd: toggled on + bit 2 (ammo left)");
    FCK_EQ(fx.flags, 0x0B, "wd: effect object bit 2 cleared");
    FCK_EQ(e.time, 1060, "wd: player re-armed 60 ticks");
    FCK_EQ(e.value, 42, "wd: value from extra");
    FCK_EQ(e.low, 100, "wd: low from extra");
    FCK_EQ(e.high, 200, "wd: high from extra (>= low kept)");
    FCK_EQ(e.other, 7, "wd: other from extra");

    /* not expired, bit 0 stays set, no ammo, extra high < low */
    data[(0x8A + 3 * 2) / 2] = 0;
    ex.low = 50; ex.high = 10;
    r = gWeaponEffectUpdate(&owner, &prm, &ex);
    FCK_EQ(r, 1, "wd: still active -> 1");
    FCK_EQ(e.flags, 0x11, "wd: not expired keeps bit 0, no ammo clears bit 2");
    FCK_EQ(fx.flags, 0x0F, "wd: no ammo sets effect object bit 2");
    FCK_EQ(e.high, 80, "wd: extra high < low -> 50 * 8 / 5");

    /* timer exactly at the frame time is not expired */
    gFrameTime = 1060;
    r = gWeaponEffectUpdate(&owner, &prm, &ex);
    FCK_EQ(r, 1, "wd: frame == timer -> not expired, still active");
    FCK_EQ(e.flags & 1, 1, "wd: frame == timer keeps bit 0");
    FCK_EQ(e.time, 1120, "wd: re-armed from the current frame");

    /* expired with bit 0 set -> toggled off, returns 0, extra not copied */
    gFrameTime = 2000;
    ex.value = 99;
    r = gWeaponEffectUpdate(&owner, &prm, &ex);
    FCK_EQ(r, 0, "wd: toggled off -> 0");
    FCK_EQ(e.flags & 1, 0, "wd: expired timer toggles bit 0 off");
    FCK_EQ(e.value, 42, "wd: inactive keeps the old value");
    FCK_EQ(e.time, 2060, "wd: re-armed even when toggled off");
    FCK_EQ(decoy.time, 0, "wd: the word after the four slots is never used");

    /* player without extra behaves like a non-player */
    e.flags = 0;
    r = gWeaponEffectUpdate(&owner, &prm, 0);
    FCK_EQ(r, 0, "wd: player without extra -> 0");
    FCK_EQ(e.time, 2120, "wd: player without extra re-armed 120 ticks");
    FCK_EQ(e.flags, 1, "wd: player without extra sets bit 0");
}

/* ========================================================================
 * 80063568 map tile animations
 * ======================================================================== */
typedef struct { u8 max, cur, type, flags; s16 rate, timer; u16 *src; u16 *dst; } TaEntry;
typedef struct TaNode { struct TaNode *next; s32 pad4; s32 count; TaEntry entries[4]; } TaNode;
TaNode *gMapTileAnimList __asm__("D_8009FA04");
u8 gTilePatternCounterState[0x204] __asm__("D_800A2730");
void gStepMapTileAnimations(s32 dt) __asm__("func_80063568");

static u16 ta_frames[8][4];
static u16 ta_dst[4][4];

static void ta_frames_init(void)
{
    int f, k;
    for (f = 0; f < 8; f++)
        for (k = 0; k < 4; k++) ta_frames[f][k] = (u16)(0x100 * f + k);
}

static void ta_entry(TaEntry *e, int type, int max, int cur, int rate, int timer, int flags, int slot)
{
    e->type = type; e->max = max; e->cur = cur; e->rate = rate; e->timer = timer; e->flags = flags;
    e->src = &ta_frames[0][0];
    e->dst = ta_dst[slot];
}

static void test_80063568(void)
{
    TaNode n1, n2;
    u16 *f3;    /* 6-byte frame k starts at u16 index 3 * k */
    int i, ok;

    /* @leaf 80063568 semantic frame playback modes: type 0 loops forward (wraps to 0 at max), type 1 loops backward (0 -> max - 1), type 2 ping-pongs (reverses at max - 1 and at 0, direction in flag bit 1), type 3 picks rand % max; the countdown timer is reloaded by the rate; flag bit 0 = 8-byte frames copied as 4 words, else 6-byte frames as 3 words; flag bit 2 = frozen; sine types map (sin + 1) / 2 onto 0..max-1; the 256-byte counter field counts down by dt / 4 and reseeds from rand & 0x3F + 0x40 */
    LK_FUNC("func_80063568");

    ta_frames_init();
    f3 = &ta_frames[0][0];
    lk_fill(ta_dst, 0xEE, sizeof(ta_dst));
    lk_fill(&n1, 0, sizeof(n1));
    lk_fill(&n2, 0, sizeof(n2));
    gTilePatternCounterState[0x201] = 0;
    rng_reset();
    n1.next = &n2; n1.count = 4; n2.next = 0; n2.count = 2;
    ta_entry(&n1.entries[0], 0, 3, 2, 10, 5, 0, 0);       /* expires, wraps 2 -> 0 */
    ta_entry(&n1.entries[1], 1, 4, 0, 10, 3, 1, 1);       /* reverse wrap 0 -> 3, 8-byte frames */
    ta_entry(&n1.entries[2], 0, 5, 1, 10, 20, 0, 2);      /* not expired */
    ta_entry(&n1.entries[3], 0, 5, 1, 10, 1, 4, 3);       /* frozen */
    ta_entry(&n2.entries[0], 2, 4, 3, 7, 2, 0, 2);        /* ping-pong turn at max */
    ta_entry(&n2.entries[1], 3, 5, 0, 6, 0, 0, 3);        /* random */
    n2.entries[1].dst = ta_dst[3];
    n1.entries[3].dst = ta_dst[1];                        /* frozen must not touch it */
    n1.entries[2].dst = ta_dst[0];
    rng_seq[0] = 17;
    gMapTileAnimList = &n1;
    gStepMapTileAnimations(5);
    FCK_EQ(n1.entries[0].cur, 0, "ta: forward loop wraps at max");
    FCK_EQ(n1.entries[0].timer, 10, "ta: timer reloaded by the rate (0 + 10)");
    FCK_EQ(ta_dst[0][0], f3[0], "ta: frame 0 word 0 copied");
    FCK_EQ(ta_dst[0][2], f3[2], "ta: frame 0 word 2 copied");
    FCK_EQ(ta_dst[0][3], 0xEEEE, "ta: 6-byte frame copies only 3 words");
    FCK_EQ(n1.entries[1].cur, 3, "ta: backward loop 0 -> max - 1");
    FCK_EQ(n1.entries[1].timer, 8, "ta: timer -2 + 10");
    FCK_EQ(ta_dst[1][0], ta_frames[3][0], "ta: 8-byte frame 3 word 0");
    FCK_EQ(ta_dst[1][3], ta_frames[3][3], "ta: 8-byte frame copies 4 words");
    FCK_EQ(n1.entries[2].cur, 1, "ta: unexpired timer keeps frame");
    FCK_EQ(n1.entries[2].timer, 15, "ta: unexpired timer counts down");
    FCK_EQ(n1.entries[3].timer, 1, "ta: frozen entry timer untouched");
    FCK_EQ(n2.entries[0].cur, 2, "ta: ping-pong turns at max -> max - 2");
    FCK_EQ(n2.entries[0].flags, 2, "ta: ping-pong now runs backward");
    FCK_EQ(ta_dst[2][0], f3[2 * 3], "ta: ping-pong frame 2 copied");
    FCK_EQ(n2.entries[1].cur, 2, "ta: random frame 17 % 5");
    FCK_EQ(rng_calls, 1, "ta: one rand for the random entry");
    FCK_EQ(ta_dst[3][1], f3[2 * 3 + 1], "ta: random frame copied");

    /* ping-pong backward: 2 -> 1, then at 0 turns forward to 1 */
    gStepMapTileAnimations(7);
    FCK_EQ(n2.entries[0].cur, 1, "ta: ping-pong backward step");
    n2.entries[0].cur = 0; n2.entries[0].timer = 0;
    gStepMapTileAnimations(1);
    FCK_EQ(n2.entries[0].cur, 1, "ta: ping-pong turns at 0 -> 1");
    FCK_EQ(n2.entries[0].flags, 0, "ta: ping-pong forward again");

    /* sine types: 0x11 uses sin(angle >> 3), others cos((angle & 0x3FFF) >> 3) */
    lk_fill(ta_dst, 0xEE, sizeof(ta_dst));
    n1.next = 0; n1.count = 2;
    ta_entry(&n1.entries[0], 0x11, 6, 0, 8, 0x100, 0, 0);
    ta_entry(&n1.entries[1], 0x10, 6, 5, 32, 0x7FF0, 0, 1);
    gMapTileAnimList = &n1;
    sc_reset(0, 0x10000);
    gStepMapTileAnimations(4);
    FCK_EQ(n1.entries[0].timer, 0x104, "ta: phase += dt * rate / 8");
    FCK_EQ(sc_angles[0], 0x20, "ta: type 0x11 angle = phase >> 3");
    FCK_EQ(n1.entries[0].cur, 3, "ta: sin 0 -> middle frame max / 2");
    FCK_EQ(ta_dst[0][0], f3[3 * 3], "ta: changed sine frame copied");
    FCK_EQ(n1.entries[1].timer, 0x0000, "ta: phase wraps at 0x8000");
    FCK_EQ(sc_angles[1], 0, "ta: type 0x10 angle = (phase & 0x3FFF) >> 3");
    FCK_EQ(n1.entries[1].cur, 5, "ta: cos 1.0 -> clamped to max - 1");
    FCK_EQ(ta_dst[1][0], 0xEEEE, "ta: unchanged sine frame not copied");

    /* counter field: dt 8 -> step 2 */
    gMapTileAnimList = 0;
    rng_reset();
    rng_seq[0] = 0x105;    /* & 0x3F = 5 -> 0x45 */
    for (i = 0; i < 0x200; i++) gTilePatternCounterState[i] = 0x50;
    gTilePatternCounterState[0] = 5;
    gTilePatternCounterState[1] = 1;
    gTilePatternCounterState[0x101] = 0;
    gTilePatternCounterState[0x200] = 0;
    gTilePatternCounterState[0x201] = 1;
    gStepMapTileAnimations(8);
    FCK_EQ(gTilePatternCounterState[0], 3, "ta: counter 5 - 2");
    FCK_EQ(gTilePatternCounterState[1], 0x8A, "ta: underflow reseeds (0x45 << 1)");
    FCK_EQ(gTilePatternCounterState[0x101], 0x45, "ta: reseed period stored in the second half");
    FCK_EQ(gTilePatternCounterState[0x100], 0x50, "ta: no reseed -> period untouched");
    FCK_EQ(gTilePatternCounterState[0xFF], 0x4E, "ta: all 256 counters step");
    FCK_EQ(rng_calls, 1, "ta: one rand for one underflow");
    FCK_EQ(gTilePatternCounterState[0x200], 2, "ta: carry byte = step & 3");
    gTilePatternCounterState[0x201] = 0;
    gStepMapTileAnimations(8);
    FCK_EQ(gTilePatternCounterState[0], 3, "ta: disabled counter field untouched");
    ok = gTilePatternCounterState[0x200] == 2;
    FCK_CHECK(ok, "ta: disabled -> carry untouched");
}

/* ========================================================================
 * 800684AC map grid ray walk
 * ======================================================================== */
s32 *gMapGridHeader __asm__("D_8009F80C");
s32 gTraceMapRay(s32 *p, s32 *q, s32 arg2, s32 arg3, s32 flag) __asm__("func_800684AC");
static s32 mr_header[4];
static s32 mr_seg[8][6];
static s32 mr_calls, mr_hit_at, mr_hit_val, mr_args[3];
s32 mock_cell_ray(s32 *prev, s32 *next, s32 a2, s32 a3, s32 flag) __asm__("func_80067914");
s32 mock_cell_ray(s32 *prev, s32 *next, s32 a2, s32 a3, s32 flag)
{
    int i;
    if (mr_calls < 8)
        for (i = 0; i < 3; i++) { mr_seg[mr_calls][i] = prev[i]; mr_seg[mr_calls][3 + i] = next[i]; }
    mr_args[0] = a2; mr_args[1] = a3; mr_args[2] = flag;
    mr_calls++;
    return mr_calls == mr_hit_at ? mr_hit_val : 0;
}
static void mr_reset(void)
{
    int i, k;
    mr_header[0] = 0; mr_header[1] = 4; mr_header[2] = 4; mr_header[3] = 0;
    gMapGridHeader = mr_header;
    mr_calls = 0; mr_hit_at = -1; mr_hit_val = 0;
    for (i = 0; i < 8; i++) for (k = 0; k < 6; k++) mr_seg[i][k] = 0x7EEEEEEE;
}
/* segment endpoint check; callee points are (world z, world x, world y) */
#define MR_PT(seg, half, z, x, y, what) do { \
    FCK_EQ(mr_seg[seg][(half) * 3 + 0], z, what " z"); \
    FCK_EQ(mr_seg[seg][(half) * 3 + 1], x, what " x"); \
    FCK_EQ(mr_seg[seg][(half) * 3 + 2], y, what " y"); } while (0)

static void test_800684AC(void)
{
    s32 p[3], q[3], r;

    /* @leaf 800684AC semantic grid DDA: cells are 0x2000 world units (0x40000 >> 5) and the map is (dim - 1) * 0x20000 wide per axis; a segment is split at every crossed cell edge, the pieces are passed in order as (z, x, y) points to the per-cell test (the callee's documented convention, also used by the .NET MechCollision.Segment), crossing points are the exact segment/grid-line intersections, the first non-zero cell result is returned, out-of-map endpoints return 0 without any cell test */
    LK_FUNC("func_800684AC");

    /* out of the map on z (p[2] < 0) and on x (>= 3 * 0x20000) */
    mr_reset();
    p[0] = 0x1000; p[1] = 0; p[2] = -1; q[0] = 0x1000; q[1] = 0; q[2] = 0x1000;
    FCK_EQ(gTraceMapRay(p, q, 1, 2, 3), 0, "mr: z outside -> 0");
    p[2] = 0x1000; q[0] = 0x60000;
    FCK_EQ(gTraceMapRay(p, q, 1, 2, 3), 0, "mr: x outside -> 0");
    FCK_EQ(mr_calls, 0, "mr: out of map -> no cell test");
    mr_header[1] = 2;    /* z extent 0x20000 */
    p[0] = 0x1000; p[2] = 0x1000; q[0] = 0x1000; q[2] = 0x20000;
    FCK_EQ(gTraceMapRay(p, q, 1, 2, 3), 0, "mr: z == extent is outside");
    FCK_EQ(mr_calls, 0, "mr: z == extent -> no cell test");
    FCK_EQ(gTraceMapRay(q, p, 1, 2, 3), 0, "mr: start z == extent is outside");
    FCK_EQ(mr_calls, 0, "mr: start z == extent -> no cell test");
    /* z == 0 is the first row and inside */
    p[2] = 0; q[2] = 0x100;
    FCK_EQ(gTraceMapRay(p, q, 1, 2, 3), 0, "mr: start z 0 clear");
    FCK_EQ(mr_calls, 1, "mr: start z 0 is inside -> one cell test");

    /* within one cell: one test p -> q, arguments forwarded */
    mr_reset();
    p[0] = 0x1000; p[1] = 0x100; p[2] = 0x1100;
    q[0] = 0x1800; q[1] = 0x180; q[2] = 0x1900;
    mr_hit_at = 1; mr_hit_val = 0x2A;
    r = gTraceMapRay(p, q, 11, 22, 33);
    FCK_EQ(r, 0x2A, "mr: single cell returns the cell result");
    FCK_EQ(mr_calls, 1, "mr: single cell -> one test");
    MR_PT(0, 0, 0x1100, 0x1000, 0x100, "mr: single prev");
    MR_PT(0, 1, 0x1900, 0x1800, 0x180, "mr: single next");
    FCK_EQ(mr_args[0] + mr_args[1] * 100 + mr_args[2] * 10000, 11 + 2200 + 330000, "mr: arg2/arg3/flag forwarded");

    /* along +z across the edges 0x2000 and 0x4000, y rising 0x100 -> 0x500 */
    mr_reset();
    p[0] = 0x1000; p[1] = 0x100; p[2] = 0x1000;
    q[0] = 0x1000; q[1] = 0x500; q[2] = 0x5000;
    r = gTraceMapRay(p, q, 0, 0, 0);
    FCK_EQ(r, 0, "mr: clear ray -> 0");
    FCK_EQ(mr_calls, 3, "mr: two z edges -> three pieces");
    MR_PT(0, 0, 0x1000, 0x1000, 0x100, "mr: +z piece0 prev");
    MR_PT(0, 1, 0x2000, 0x1000, 0x200, "mr: +z piece0 next on edge 0x2000");
    MR_PT(1, 0, 0x2000, 0x1000, 0x200, "mr: +z piece1 starts where piece0 ended");
    MR_PT(1, 1, 0x4000, 0x1000, 0x400, "mr: +z piece1 next on edge 0x4000");
    MR_PT(2, 1, 0x5000, 0x1000, 0x500, "mr: +z last piece ends at q");

    /* the same ray backwards (-z) */
    mr_reset();
    r = gTraceMapRay(q, p, 0, 0, 0);
    FCK_EQ(mr_calls, 3, "mr: -z three pieces");
    MR_PT(0, 0, 0x5000, 0x1000, 0x500, "mr: -z starts at q");
    MR_PT(0, 1, 0x4000, 0x1000, 0x400, "mr: -z first edge 0x4000");
    MR_PT(1, 1, 0x2000, 0x1000, 0x200, "mr: -z second edge 0x2000");
    MR_PT(2, 1, 0x1000, 0x1000, 0x100, "mr: -z ends at p");

    /* x-major (+x) ray across x edges 0x2000 and 0x4000 at constant z */
    mr_reset();
    p[0] = 0x1000; p[1] = 0; p[2] = 0x3000;
    q[0] = 0x5000; q[1] = 0; q[2] = 0x3000;
    gTraceMapRay(p, q, 0, 0, 0);
    FCK_EQ(mr_calls, 3, "mr: +x three pieces");
    MR_PT(0, 1, 0x3000, 0x2000, 0, "mr: +x first edge");
    MR_PT(1, 1, 0x3000, 0x4000, 0, "mr: +x second edge");
    MR_PT(2, 1, 0x3000, 0x5000, 0, "mr: +x ends at q");

    /* -x ray */
    mr_reset();
    gTraceMapRay(q, p, 0, 0, 0);
    FCK_EQ(mr_calls, 3, "mr: -x three pieces");
    MR_PT(0, 1, 0x3000, 0x4000, 0, "mr: -x first edge 0x4000");
    MR_PT(1, 1, 0x3000, 0x2000, 0, "mr: -x second edge 0x2000");

    /* slope x/z = 1/2: (z 0x1000, x 0x1000) -> (z 0x5000, x 0x3000).
     * Crossings in order: z=0x2000 at x=0x1800, x=0x2000 at z=0x3000,
     * z=0x4000 at x=0x2800; y rises 0x100 -> 0x500 (z-linear). */
    mr_reset();
    p[0] = 0x1000; p[1] = 0x100; p[2] = 0x1000;
    q[0] = 0x3000; q[1] = 0x500; q[2] = 0x5000;
    gTraceMapRay(p, q, 0, 0, 0);
    FCK_EQ(mr_calls, 4, "mr: diagonal -> 3 crossings, 4 pieces");
    MR_PT(0, 1, 0x2000, 0x1800, 0x200, "mr: diag z edge 0x2000");
    MR_PT(1, 1, 0x3000, 0x2000, 0x300, "mr: diag x edge 0x2000");
    MR_PT(2, 1, 0x4000, 0x2800, 0x400, "mr: diag z edge 0x4000");
    MR_PT(3, 1, 0x5000, 0x3000, 0x500, "mr: diag ends at q");

    /* first hit stops the walk */
    mr_reset();
    mr_hit_at = 2; mr_hit_val = 0x105;
    r = gTraceMapRay(p, q, 0, 0, 0);
    FCK_EQ(r, 0x105, "mr: first non-zero cell result returned");
    FCK_EQ(mr_calls, 2, "mr: walk stops at the hit");
}

/* ========================================================================
 * 8006AB38 weapon loadout reset
 * ======================================================================== */
typedef struct { u8 pad0[4]; u8 kind; u8 flags; s16 value1; u8 pad8[2]; u8 value2; u8 padB[0x1b]; s16 value3; } WlData;
typedef struct {
    u8 pad0[0x68]; s32 field68, field6c, field70; u8 pad74[4]; s32 field78[3];
    s16 field84[3]; s16 field8a[3]; u8 field90[3]; u8 field93[3]; u8 pad96[2];
    s32 field98; u8 pad9c[9]; u8 fielda5; u8 pada6[0x1b]; s8 fieldc1;
} WlState;
typedef struct { u8 pad[0xac]; WlState *state; } WlOwner;
typedef struct { u8 pad[8]; s32 field8; u8 padc[0x8c]; s32 field98; } WlSession;
WlSession *gGameSession __asm__("D_8009FA94");
void gResetPlayerWeaponLoadout(WlOwner *owner) __asm__("func_8006AB38");
static WlData wl_data[3];
WlData *mock_registry(u8 id) __asm__("func_8006AAEC");
WlData *mock_registry(u8 id)
{
    fck_record("registry", id, 0, 0);
    return &wl_data[id - 0x20];
}

static void wl_data_init(void)
{
    lk_fill(wl_data, 0, sizeof(wl_data));
    wl_data[0].value1 = 100; wl_data[0].value3 = 40; wl_data[0].value2 = 3; wl_data[0].kind = 1;
    wl_data[1].value1 = 8;   wl_data[1].value3 = 6;  wl_data[1].value2 = 9; wl_data[1].kind = 13; wl_data[1].flags = 4;
    wl_data[2].value1 = 0;   wl_data[2].value3 = 1;  wl_data[2].value2 = 0; wl_data[2].kind = 2;
}

static void test_8006AB38(void)
{
    WlOwner owner;
    WlState st;
    WlSession ses;

    /* @leaf 8006AB38 semantic difficulty tiers for the three weapon slots: tier 0 = registry capacity, tier 1 = +25 % (x + x/4), tier 2 = +50 % (x + x/2), tier 3 = 9999; tier from session +8 (player 1, state +0xC1 == 0) or +0x98 (player 2); per slot ammo word copied, counter cleared, flag 4 clears +0xA5, kind 13 allocates the dome effect for that slot; the shared state words are reset (+0x68 = -1) */
    LK_FUNC("func_8006AB38");

    wl_data_init();
    lk_fill(&st, 0x55, sizeof(st));
    lk_fill(&ses, 0, sizeof(ses));
    owner.state = &st;
    gGameSession = &ses;
    st.field90[0] = 0x20; st.field90[1] = 0x21; st.field90[2] = 0x22;
    st.fieldc1 = 0;
    ses.field8 = 0; ses.field98 = 3;
    dome_ret = 0;
    fck_log_reset();
    gResetPlayerWeaponLoadout(&owner);
    FCK_EQ(fck_calls_to("registry"), 3, "wl: three registry lookups");
    FCK_EQ(fck_call_n("registry", 2).a0, 0x22, "wl: slot ids from +0x90");
    FCK_EQ(st.field84[0], 100, "wl: tier 0 keeps capacity");
    FCK_EQ(st.field8a[0], 40, "wl: tier 0 keeps reserve");
    FCK_EQ(st.field93[1], 9, "wl: ammo type byte copied");
    FCK_EQ(st.field78[2], 0, "wl: counter cleared");
    FCK_EQ(st.fielda5, 0, "wl: flag 4 weapon clears +0xA5");
    FCK_EQ(fck_calls_to("alloc_dome"), 1, "wl: one kind-13 weapon");
    FCK_EQ(fck_call_n("alloc_dome", 0).a0, (s32)&owner, "wl: dome effect for the owner");
    FCK_EQ(fck_call_n("alloc_dome", 0).a1, 1, "wl: dome effect slot index");
    FCK_EQ(st.field98, 0, "wl: +0x98 cleared");
    FCK_EQ(st.field6c, 0, "wl: +0x6C cleared");
    FCK_EQ(st.field70, 0, "wl: +0x70 cleared");
    FCK_EQ(st.field68, -1, "wl: +0x68 = -1");

    /* tier 1 */
    ses.field8 = 1;
    gResetPlayerWeaponLoadout(&owner);
    FCK_EQ(st.field84[0], 125, "wl: tier 1 +25 % capacity");
    FCK_EQ(st.field8a[0], 50, "wl: tier 1 +25 % reserve");
    FCK_EQ(st.field84[1], 10, "wl: tier 1 8 + 8/4");
    /* tier 2 */
    ses.field8 = 2;
    gResetPlayerWeaponLoadout(&owner);
    FCK_EQ(st.field84[0], 150, "wl: tier 2 +50 % capacity");
    FCK_EQ(st.field8a[1], 9, "wl: tier 2 6 + 3");
    FCK_EQ(st.field8a[2], 1, "wl: tier 2 1 + 0");
    /* player 2 uses session +0x98 = tier 3 */
    st.fieldc1 = 1;
    lk_fill(&st.fielda5, 0x77, 1);
    wl_data[1].flags = 0;
    gResetPlayerWeaponLoadout(&owner);
    FCK_EQ(st.field84[2], 9999, "wl: tier 3 capacity 9999");
    FCK_EQ(st.field8a[0], 9999, "wl: tier 3 reserve 9999");
    FCK_EQ(st.fielda5, 0x77, "wl: no flag-4 weapon keeps +0xA5");
    /* unknown tier behaves like tier 0 */
    st.fieldc1 = 0; ses.field8 = 7;
    gResetPlayerWeaponLoadout(&owner);
    FCK_EQ(st.field84[0], 100, "wl: unknown tier -> base values");
}

/* ========================================================================
 * 8006C84C dome patch mesh
 * ======================================================================== */
typedef struct {
    void *vertices; void *faces; s16 face_count; s16 vertex_count; s8 width; s8 height; u8 pad[2];
    s32 unk10; s32 unk14; u8 flags; u8 pad19[3];
} DmMesh;
DmMesh *gBuildDomePatchMesh(s16 arg0, s32 flags) __asm__("func_8006C84C");
s32 gTableBuffer __asm__("D_8009FA08");
s32 gLevelVertexBlock __asm__("D_8009FA1C");
static u8 dm_pool[3][0x400];
static s32 dm_allocs, dm_fail_at;
void *mock_alloc(s32 size, s32 tag) __asm__("func_80017EBC");
void *mock_alloc(s32 size, s32 tag)
{
    fck_record("alloc", size, tag, 0);
    dm_allocs++;
    if (dm_allocs == dm_fail_at) return 0;
    return dm_pool[(dm_allocs - 1) % 3];
}
void mock_free(void *p) __asm__("func_800181D4");
void mock_free(void *p) { fck_record("free", (s32)p, 0, 0); }
void mock_gen_verts(void *v, s16 step, s16 flags) __asm__("func_8006C4C4");
void mock_gen_verts(void *v, s16 step, s16 flags) { fck_record("gen_verts", (s32)v, step, flags); }
void mock_build_faces(void *f, s32 rows, s32 cols, s32 count) __asm__("func_8006C768");
void mock_build_faces(void *f, s32 rows, s32 cols, s32 count) { fck_record("build_faces", (s32)f, rows, (cols << 16) | count); }

static void dm_reset(int fail_at)
{
    dm_allocs = 0; dm_fail_at = fail_at;
    lk_fill(dm_pool, 0, sizeof(dm_pool));
    gTableBuffer = 0x1111; gLevelVertexBlock = 0x2222;
    fck_log_reset();
}

static void test_8006C84C(void)
{
    DmMesh *m;
    struct FckCall c;

    /* @leaf 8006C84C source src/game/8006C84C.c: step = 0x200 / (0x200 / n) for n in 1..0x200 else 0x200; count = 0x200 / step, rows = count + 1, width = 2 * rows - 2, vertices = rows * width + 1 (8 bytes each), faces = count * (2 * rows - 3) + rows - 1 (5 bytes each); flag bit 0 halves the width; any failed allocation frees what was allocated and returns 0 */
    LK_FUNC("func_8006C84C");

    dm_reset(-1);
    m = gBuildDomePatchMesh(0x80, 0x11);
    FCK_EQ((s32)m, (s32)dm_pool[0], "dm: mesh header is the first block");
    FCK_EQ(fck_calls_to("alloc"), 3, "dm: three allocations");
    c = fck_call_n("alloc", 0);
    FCK_EQ(c.a0, 0x1C, "dm: header 0x1C bytes"); FCK_EQ(c.a1, 1, "dm: tag 1");
    FCK_EQ(m->height, 4, "dm: 0x200 / 0x80 = 4 bands");
    FCK_EQ(m->vertex_count, 41, "dm: 5 rows * 8 + 1 vertices");
    FCK_EQ(fck_call_n("alloc", 1).a0, 41 * 8, "dm: 8 bytes per vertex");
    FCK_EQ(m->face_count, 32, "dm: 4 * 7 + 4 faces");
    FCK_EQ(fck_call_n("alloc", 2).a0, 32 * 5, "dm: 5 bytes per face");
    FCK_EQ(m->width, 4, "dm: width 8 halved by flag bit 0");
    FCK_EQ(m->flags, 0x11, "dm: flags stored");
    FCK_EQ(m->unk10, 0x1111, "dm: table buffer");
    FCK_EQ(m->unk14, 0x2222, "dm: level vertex block");
    c = fck_call_n("gen_verts", 0);
    FCK_EQ(c.a0, (s32)dm_pool[1], "dm: vertices generated into the vertex block");
    FCK_EQ(c.a1, 0x80, "dm: vertex step");
    FCK_EQ(c.a2, 0x10, "dm: only flag 0x10 forwarded");
    c = fck_call_n("build_faces", 0);
    FCK_EQ(c.a0, (s32)dm_pool[2], "dm: faces into the face block");
    FCK_EQ(c.a1, 5, "dm: rows");
    FCK_EQ(c.a2, (9 << 16) | 41, "dm: cols 9, vertex count 41");

    /* n = 0x60: 0x200 / 0x60 = 5 -> step 0x66, count 5 */
    dm_reset(-1);
    m = gBuildDomePatchMesh(0x60, 0);
    FCK_EQ(fck_call_n("gen_verts", 0).a1, 0x66, "dm: step 0x200 / 5");
    FCK_EQ(m->height, 5, "dm: 5 bands");
    FCK_EQ(m->width, 10, "dm: width 10 unhalved");
    FCK_EQ(m->vertex_count, 61, "dm: 6 * 10 + 1");
    FCK_EQ(m->face_count, 5 * 9 + 5, "dm: 5 * 9 + 5 faces");

    /* out of range n = 0 -> one band */
    dm_reset(-1);
    m = gBuildDomePatchMesh(0, 0);
    FCK_EQ(fck_call_n("gen_verts", 0).a1, 0x200, "dm: n 0 -> step 0x200");
    FCK_EQ(m->vertex_count, 5, "dm: 2 * 2 + 1 vertices");
    FCK_EQ(m->face_count, 2, "dm: 1 * 1 + 1 faces");

    /* n = 0x200 is the last in-range value (one band), 0x201 is out of range */
    dm_reset(-1);
    m = gBuildDomePatchMesh(0x200, 0);
    FCK_EQ(fck_call_n("gen_verts", 0).a1, 0x200, "dm: n 0x200 -> step 0x200");
    dm_reset(-1);
    m = gBuildDomePatchMesh(0x201, 0);
    FCK_EQ(fck_call_n("gen_verts", 0).a1, 0x200, "dm: n 0x201 out of range -> step 0x200");
    FCK_EQ(m->vertex_count, 5, "dm: n 0x201 -> one band");

    /* allocation failures */
    dm_reset(1);
    FCK_EQ((s32)gBuildDomePatchMesh(0x80, 0), 0, "dm: header alloc failure -> 0");
    FCK_EQ(fck_calls_to("free"), 0, "dm: nothing to free");
    dm_reset(2);
    FCK_EQ((s32)gBuildDomePatchMesh(0x80, 0), 0, "dm: vertex alloc failure -> 0");
    FCK_EQ(fck_call_n("free", 0).a0, (s32)dm_pool[0], "dm: header freed");
    FCK_EQ(fck_calls_to("free"), 1, "dm: only the header freed");
    dm_reset(3);
    FCK_EQ((s32)gBuildDomePatchMesh(0x80, 0), 0, "dm: face alloc failure -> 0");
    FCK_EQ(fck_call_n("free", 0).a0, (s32)dm_pool[1], "dm: vertices freed first");
    FCK_EQ(fck_call_n("free", 1).a0, (s32)dm_pool[0], "dm: then the header");
    FCK_EQ(fck_calls_to("gen_verts"), 0, "dm: failure -> no generation");
}

/* ========================================================================
 * 8007CACC music preview menu
 * ======================================================================== */
typedef struct { s8 mode; u8 pad1[4]; s8 state; } MpSel;
typedef struct { s32 bank; s8 volume, baseVolume, pan, extra; } MpVoice;
u8 gSnapshotResult __asm__("D_8009F904");
MpSel *gFontBankTable __asm__("D_8009F468");
MpVoice *gSnapshotPointer __asm__("D_8009F91C");
s32 gPreviewTrackId __asm__("D_8009F920");
s32 gHandleMusicPreviewMenu(s32 flags, s32 arg1) __asm__("func_8007CACC");

static s32 mp_alloc_ret;
void mock_set_sfx(s32 v) __asm__("func_800451F8");
void mock_set_sfx(s32 v) { fck_record("set_music", v, 0, 0); }
void mock_set_vagm(s32 v) __asm__("func_80045238");
void mock_set_vagm(s32 v) { fck_record("set_vagm", v, 0, 0); }
void mock_set_voice(s32 v) __asm__("func_80045294");
void mock_set_voice(s32 v) { fck_record("set_voice", v, 0, 0); }
void mock_stop_voice(s32 h) __asm__("func_80046A34");
void mock_stop_voice(s32 h) { fck_record("stop", h, 0, 0); }
s32 mock_alloc_voice(s32 a0, s32 a1, s32 a2, s32 id) __asm__("func_80046A58");
s32 mock_alloc_voice(s32 a0, s32 a1, s32 a2, s32 id) { fck_record("alloc_voice", a0, (a1 << 8) | a2, id); return mp_alloc_ret; }
void mock_play(s32 t, s32 a1, s32 a2, s32 a3, s32 h, void *o) __asm__("func_80046E88");
void mock_play(s32 t, s32 a1, s32 a2, s32 a3, s32 h, void *o) { fck_record("play", t, a3, h + (s32)o + a1 + a2); }
void mock_mix(MpVoice *v, s32 pan, s32 vol, s32 extra) __asm__("func_80047800");
void mock_mix(MpVoice *v, s32 pan, s32 vol, s32 extra) { fck_record("mix", (s32)v, pan, (vol << 8) | (extra & 0xFF)); }

static void test_8007CACC(void)
{
    MpSel sel;
    MpVoice voice, voice2;
    s32 r;

    /* @leaf 8007CACC semantic options menu sound rows: row 1 = music, 2 = VAGM, 3 = voice; left (0x800000) lowers and right (0x200000) raises the selected setting by one from its getter; on the music row the music preview track 0x4A plays at the music volume, on the voice row the voice preview 0x4B at the voice volume (a different preview is stopped first, a running one is only re-mixed); other rows stop any preview; bit 0x40000000 latches arg1; the input word is returned unchanged */
    LK_FUNC("func_8007CACC");

    gFontBankTable = &sel;
    lk_fill(&voice, 0, sizeof(voice));
    lk_fill(&voice2, 0, sizeof(voice2));

    /* music row, right: raise music volume, start preview 0x4A */
    sel.mode = 1; sel.state = 0;
    fcm_sound_reset();
    fcm_sound.music_volume = 9;
    gSnapshotPointer = 0; gPreviewTrackId = 0; gSnapshotResult = 0x33;
    mp_alloc_ret = (s32)&voice;
    fck_log_reset();
    r = gHandleMusicPreviewMenu(0x200000, 0);
    FCK_EQ(r, 0x200000, "mp: flags returned");
    FCK_EQ(gSnapshotResult, 0x33, "mp: no latch bit -> result kept");
    FCK_EQ(fck_calls_to("stop"), 0, "mp: no running preview -> nothing stopped");
    FCK_EQ(fck_call_n("set_music", 0).a0, 10, "mp: right raises music volume by one");
    FCK_EQ(fck_call_n("alloc_voice", 0).a0, 9, "mp: preview at the music volume");
    FCK_EQ(fck_call_n("alloc_voice", 0).a1, 10 << 8, "mp: preview priority 10");
    FCK_EQ(fck_call_n("alloc_voice", 0).a2, 0x4A, "mp: music preview id 0x4A");
    FCK_EQ(fck_call_n("play", 0).a0, 0x4A, "mp: play 0x4A");
    FCK_EQ(fck_call_n("play", 0).a1, -1, "mp: play arg3 -1");
    FCK_EQ(fck_call_n("play", 0).a2, (s32)&voice, "mp: play on the new voice");
    FCK_EQ((s32)gSnapshotPointer, (s32)&voice, "mp: voice stored");
    FCK_EQ(gPreviewTrackId, 0x4A, "mp: preview id stored");
    FCK_EQ(fck_calls_to("mix"), 0, "mp: fresh start not re-mixed");

    /* music row again, left: preview already running -> re-mix only */
    voice.pan = 0x20;
    fck_log_reset();
    gHandleMusicPreviewMenu(0x800000, 0);
    FCK_EQ(fck_call_n("set_music", 0).a0, 8, "mp: left lowers music volume by one");
    FCK_EQ(fck_calls_to("stop"), 0, "mp: same preview not stopped");
    FCK_EQ(fck_calls_to("alloc_voice"), 0, "mp: same preview not reallocated");
    FCK_EQ(fck_call_n("mix", 0).a0, (s32)&voice, "mp: running preview re-mixed");
    FCK_EQ(fck_call_n("mix", 0).a1, 0x20, "mp: pan kept");
    FCK_EQ(fck_call_n("mix", 0).a2, (9 << 8) | 0xFF, "mp: new music volume, extra -1");

    /* voice row: preview 0x4A is stopped, 0x4B started at the voice volume */
    sel.mode = 3;
    fcm_sound.voice_volume = 4;
    mp_alloc_ret = (s32)&voice2;
    fck_log_reset();
    gHandleMusicPreviewMenu(0x200000, 0);
    FCK_EQ(fck_call_n("set_voice", 0).a0, 5, "mp: right raises voice volume");
    FCK_EQ(fck_call_n("stop", 0).a0, (s32)&voice, "mp: other preview stopped");
    FCK_EQ(fck_call_n("alloc_voice", 0).a0, 4, "mp: voice preview at voice volume");
    FCK_EQ(fck_call_n("alloc_voice", 0).a2, 0x4B, "mp: voice preview id 0x4B");
    FCK_EQ(gPreviewTrackId, 0x4B, "mp: preview id 0x4B");
    FCK_EQ((s32)gSnapshotPointer, (s32)&voice2, "mp: voice preview stored");

    /* voice row again: the running voice preview is only re-mixed */
    voice2.pan = 0x11;
    fck_log_reset();
    gHandleMusicPreviewMenu(0, 0);
    FCK_EQ(fck_calls_to("stop"), 0, "mp: same voice preview not stopped");
    FCK_EQ(fck_calls_to("alloc_voice"), 0, "mp: same voice preview not reallocated");
    FCK_EQ(fck_call_n("mix", 0).a0, (s32)&voice2, "mp: voice preview re-mixed");
    FCK_EQ(fck_call_n("mix", 0).a2, (4 << 8) | 0xFF, "mp: re-mix at the voice volume");

    /* VAGM row: setting adjusted, preview stopped */
    sel.mode = 2;
    fcm_sound.vagm_volume = 7;
    fck_log_reset();
    gHandleMusicPreviewMenu(0x800000, 0);
    FCK_EQ(fck_call_n("set_vagm", 0).a0, 6, "mp: left lowers VAGM volume");
    FCK_EQ(fck_call_n("stop", 0).a0, (s32)&voice2, "mp: VAGM row stops the preview");
    FCK_EQ((s32)gSnapshotPointer, 0, "mp: preview cleared");
    FCK_EQ(fck_calls_to("alloc_voice"), 0, "mp: VAGM row has no preview");

    /* allocation failure leaves no preview */
    sel.mode = 1;
    mp_alloc_ret = 0;
    gPreviewTrackId = 0x11;
    fck_log_reset();
    r = gHandleMusicPreviewMenu(0x200000, 0);
    FCK_EQ(r, 0x200000, "mp: no voice -> flags still returned");
    FCK_EQ(fck_calls_to("play"), 0, "mp: no voice -> no play");
    FCK_EQ(gPreviewTrackId, 0x11, "mp: no voice -> id unchanged");

    /* latch bit; state 1 with arg1 != 0 and state 2 are ignored */
    sel.state = 1;
    gSnapshotPointer = &voice;
    fck_log_reset();
    r = gHandleMusicPreviewMenu(0x40200000, 5);
    FCK_EQ(gSnapshotResult, 5, "mp: latch bit stores arg1");
    FCK_EQ(r, 0x40200000, "mp: flags returned (latched)");
    FCK_EQ(fck_log_count, 0, "mp: state 1 with arg1 -> no sound work");
    sel.state = 2;
    gHandleMusicPreviewMenu(0x200000, 0);
    FCK_EQ(fck_log_count, 0, "mp: state 2 -> no sound work");
    sel.state = 1;
    fck_log_reset();
    gHandleMusicPreviewMenu(0x200000, 0);
    FCK_EQ(fck_calls_to("set_music"), 1, "mp: state 1 with arg1 0 handled");
}

/* ========================================================================
 * 80083834 stale scene node reaper
 * ======================================================================== */
typedef struct RnNode {
    u8 pad0[0x4b]; s8 index; u8 flags; u8 pad4d[3]; s32 time; u16 sound; u8 pad56; u8 active;
    struct RnNode *next;
} RnNode;
typedef struct { RnNode *head; s32 time; } RnBucket;
typedef struct { s32 mask; u8 entries[4][8]; } RnState;
RnState *gMissionTable __asm__("D_8009F930");
void *gMissionState __asm__("D_8009F934");
RnBucket *gCellBucketGrid __asm__("D_8009F938");
s32 gCellGridWidth __asm__("D_8009F93C");
s32 gCellGridHeight __asm__("D_8009F940");
void gReclaimStaleSceneNodes(s32 arg0, u8 arg1) __asm__("func_80083834");
void mock_stop_channel(s32 s, s32 ch) __asm__("func_800457B8");
void mock_stop_channel(s32 s, s32 ch) { fck_record("stop_channel", s, ch, 0); }
void mock_pool_free(void *pool, RnNode *n) __asm__("func_800195D0");
void mock_pool_free(void *pool, RnNode *n) { fck_record("pool_free", (s32)pool, (s32)n, 0); }

static RnNode rn_nodes[6];

static void rn_node(int i, s32 time, int active, int index, int sound, RnNode *next)
{
    lk_fill(&rn_nodes[i], 0, sizeof(RnNode));
    rn_nodes[i].time = time; rn_nodes[i].active = active; rn_nodes[i].index = index;
    rn_nodes[i].sound = sound; rn_nodes[i].next = next;
}

static void test_80083834(void)
{
    RnBucket grid[2];
    RnState st;
    s32 pool;

    /* @leaf 80083834 source src/game/80083834.c: no grid -> nothing; cells walked from the last to the first, a cell newer than arg0 is skipped; in each cell's list, nodes whose time <= frame time are unlinked and returned to the pool (head or interior), playing nodes with a channel index mark the channel in the mission mask and stop sound | 0x40000000 on entries[index].byte6; a negative time or flag 0x80 ends the cell; arg1 != 0 returns after the first interior removal */
    LK_FUNC("func_80083834");

    lk_fill(&st, 0, sizeof(st));
    gMissionTable = &st;
    gMissionState = &pool;
    gCellGridWidth = 2; gCellGridHeight = 1;
    gFrameTime = 10;

    gCellBucketGrid = 0;
    fck_log_reset();
    gReclaimStaleSceneNodes(100, 0);
    FCK_EQ(fck_log_count, 0, "rn: no grid -> nothing");

    /* cell 1: A(5, playing ch 2) -> B(100) -> C(3) -> D(4) ; cell 0 too new */
    rn_node(3, 4, 0, -1, 0, 0);
    rn_node(2, 3, 0, -1, 0, &rn_nodes[3]);
    rn_node(1, 100, 1, 1, 0, &rn_nodes[2]);
    rn_node(0, 5, 1, 2, 0x12, &rn_nodes[1]);
    rn_node(4, 1, 0, -1, 0, 0);
    grid[0].head = &rn_nodes[4]; grid[0].time = 200;
    grid[1].head = &rn_nodes[0]; grid[1].time = 50;
    st.mask = 1; st.entries[2][6] = 0x0B;
    gCellBucketGrid = grid;
    fck_log_reset();
    gReclaimStaleSceneNodes(100, 0);
    FCK_EQ(st.mask, 5, "rn: channel 2 marked in the mission mask");
    FCK_EQ(fck_calls_to("stop_channel"), 1, "rn: only the playing node stops a channel");
    FCK_EQ(fck_call_n("stop_channel", 0).a0, 0x40000012, "rn: stop sound | 0x40000000");
    FCK_EQ(fck_call_n("stop_channel", 0).a1, 0x0B, "rn: channel from entries[2] byte 6");
    FCK_EQ(fck_calls_to("pool_free"), 3, "rn: three stale nodes freed");
    FCK_EQ(fck_call_n("pool_free", 0).a1, (s32)&rn_nodes[0], "rn: head freed first");
    FCK_EQ(fck_call_n("pool_free", 0).a0, (s32)&pool, "rn: freed into the mission pool");
    FCK_EQ(fck_call_n("pool_free", 2).a1, (s32)&rn_nodes[3], "rn: interior nodes freed in order");
    FCK_EQ((s32)grid[1].head, (s32)&rn_nodes[1], "rn: new head is the live node");
    FCK_EQ((s32)rn_nodes[1].next, 0, "rn: live node unlinked from the stale tail");
    FCK_EQ((s32)grid[0].head, (s32)&rn_nodes[4], "rn: too-new cell skipped");

    /* arg1: stop after the first interior removal; flag 0x80 / negative time stop a cell */
    rn_node(2, 2, 0, -1, 0, 0);
    rn_node(1, 3, 0, -1, 0, &rn_nodes[2]);
    rn_node(0, 100, 0, -1, 0, &rn_nodes[1]);
    rn_node(4, 1, 0, -1, 0, 0);
    rn_nodes[4].flags = 0x80;
    grid[0].head = &rn_nodes[4]; grid[0].time = 0;
    grid[1].head = &rn_nodes[0]; grid[1].time = 0;
    fck_log_reset();
    gReclaimStaleSceneNodes(100, 1);
    FCK_EQ(fck_calls_to("pool_free"), 1, "rn: arg1 -> one interior removal");
    FCK_EQ((s32)rn_nodes[0].next, (s32)&rn_nodes[2], "rn: interior node unlinked");
    fck_log_reset();
    grid[1].head = 0;
    gReclaimStaleSceneNodes(100, 0);
    FCK_EQ(fck_calls_to("pool_free"), 0, "rn: flag 0x80 head ends the cell");
    rn_nodes[4].flags = 0; rn_nodes[4].time = -1;
    gReclaimStaleSceneNodes(100, 0);
    FCK_EQ(fck_calls_to("pool_free"), 0, "rn: negative time ends the cell");

    /* boundaries: cell time == arg0 is processed, node time == frame time
     * is stale, and cell 0 (the last one walked) is included */
    rn_node(4, 10, 0, -1, 0, 0);
    grid[0].head = &rn_nodes[4]; grid[0].time = 100;
    grid[1].head = 0;
    fck_log_reset();
    gReclaimStaleSceneNodes(100, 0);
    FCK_EQ(fck_calls_to("pool_free"), 1, "rn: cell 0 at arg0, node at frame time -> freed");
    FCK_EQ((s32)grid[0].head, 0, "rn: cell 0 emptied");
}

/* ========================================================================
 * 80086C74 movie SPU voice attributes
 * ======================================================================== */
/* Psy-Q libspu.h SpuVoiceAttr (0x40 bytes) and SpuCommonAttr head. */
typedef struct {
    u32 voice; u32 mask; s16 vol_l, vol_r; s16 volmode_l, volmode_r; s16 volx_l, volx_r;
    u16 pitch, note, sample_note; s16 envx; u32 addr, loop_addr;
    s32 a_mode, s_mode, r_mode; u16 ar, dr, sr, rr, sl, adsr1, adsr2, pad;
} SpuVoiceAttr;
typedef struct { u32 mask; s16 mvol_l, mvol_r; } SpuCommonAttrHead;
typedef struct { u8 pad[0x7B64]; void *first; void *second; } MaState;
MaState *gMovieAudioState __asm__("D_8009F4D8");
void gMovieAudioSetVoiceAttrs(void *object, s32 value, s32 c) __asm__("func_80086C74");
static SpuCommonAttrHead ma_common;
static SpuVoiceAttr ma_voice;
static s32 ma_common_calls, ma_voice_calls;
void mock_spu_common(SpuCommonAttrHead *a) __asm__("func_80098FE8");
void mock_spu_common(SpuCommonAttrHead *a) { ma_common = *a; ma_common_calls++; }
void mock_spu_voice(SpuVoiceAttr *a) __asm__("func_80099368");
void mock_spu_voice(SpuVoiceAttr *a) { ma_voice = *a; ma_voice_calls++; }
static MaState ma_state;

/* Psy-Q libspu.h mask bits */
#define SPU_COMMON_MVOLL (1 << 0)
#define SPU_COMMON_MVOLR (1 << 1)
#define SPU_VOICE_VOLL (1 << 0)
#define SPU_VOICE_VOLR (1 << 1)
#define SPU_VOICE_PITCH (1 << 4)
#define SPU_VOICE_WDSA (1 << 7)
#define SPU_VOICE_ADSR_AMODE (1 << 8)
#define SPU_VOICE_ADSR_SMODE (1 << 9)
#define SPU_VOICE_ADSR_RMODE (1 << 10)
#define SPU_VOICE_ADSR_AR (1 << 11)
#define SPU_VOICE_ADSR_DR (1 << 12)
#define SPU_VOICE_ADSR_SR (1 << 13)
#define SPU_VOICE_ADSR_RR (1 << 14)
#define SPU_VOICE_ADSR_SL (1 << 15)
#define SPU_VOICE_LINEARIncN 1
#define SPU_VOICE_LINEARDecN 3

static void test_80086C74(void)
{
    int voices_a, voices_b;

    /* @leaf 80086C74 spec Psy-Q libspu.h: SpuSetCommonAttr mask SPU_COMMON_MVOLL|MVOLR (3) with master volume 0x3FFF both sides; SpuSetVoiceAttr with SpuVoiceAttr layout (voice, mask, volume l/r, pitch +0x14, addr +0x1C, a/s/r_mode +0x24.., ar..sl +0x30..) and mask VOLL|VOLR|PITCH|WDSA|AMODE|SMODE|RMODE|AR|DR|SR|RR|SL = 0xFF93; modes LINEARIncN (1) attack/sustain, LINEARDecN (3) release; sl 15; volume = voice volume * 127 on the side whose stream voice matches */
    LK_FUNC("func_80086C74");

    voices_a = 0x1; voices_b = 0x2;
    ma_state.first = &voices_a; ma_state.second = &voices_b;
    gMovieAudioState = &ma_state;
    fcm_sound_reset();
    fcm_sound.voice_volume = 100;
    lk_fill(&ma_voice, 0x5A, sizeof(ma_voice));
    ma_common_calls = ma_voice_calls = 0;

    gMovieAudioSetVoiceAttrs(&voices_a, 0x1000, 0x0800);
    FCK_EQ(ma_common_calls, 1, "ma: one SpuSetCommonAttr");
    FCK_EQ(ma_common.mask, SPU_COMMON_MVOLL | SPU_COMMON_MVOLR, "ma: common mask MVOLL|MVOLR");
    FCK_EQ(ma_common.mvol_l, 0x3FFF, "ma: master volume left max");
    FCK_EQ(ma_common.mvol_r, 0x3FFF, "ma: master volume right max");
    FCK_EQ(ma_voice_calls, 1, "ma: one SpuSetVoiceAttr");
    FCK_EQ(ma_voice.voice, (s32)&voices_a, "ma: voice selector forwarded");
    FCK_EQ(ma_voice.mask, SPU_VOICE_VOLL | SPU_VOICE_VOLR | SPU_VOICE_PITCH | SPU_VOICE_WDSA |
           SPU_VOICE_ADSR_AMODE | SPU_VOICE_ADSR_SMODE | SPU_VOICE_ADSR_RMODE | SPU_VOICE_ADSR_AR |
           SPU_VOICE_ADSR_DR | SPU_VOICE_ADSR_SR | SPU_VOICE_ADSR_RR | SPU_VOICE_ADSR_SL, "ma: voice attribute mask");
    FCK_EQ(ma_voice.vol_l, 12700, "ma: first stream voice -> left = volume * 127");
    FCK_EQ(ma_voice.vol_r, 0, "ma: not the second stream voice -> right 0");
    FCK_EQ(ma_voice.pitch, 0x0800, "ma: pitch");
    FCK_EQ(ma_voice.addr, 0x1000, "ma: SPU start address");
    FCK_EQ(ma_voice.a_mode, SPU_VOICE_LINEARIncN, "ma: attack linear increase");
    FCK_EQ(ma_voice.s_mode, SPU_VOICE_LINEARIncN, "ma: sustain linear increase");
    FCK_EQ(ma_voice.r_mode, SPU_VOICE_LINEARDecN, "ma: release linear decrease");
    FCK_EQ(ma_voice.ar | ma_voice.dr | ma_voice.sr | ma_voice.rr, 0, "ma: ar/dr/sr/rr 0");
    FCK_EQ(ma_voice.sl, 15, "ma: sustain level 15");

    gMovieAudioSetVoiceAttrs(&voices_b, 0x2000, 0x1000);
    FCK_EQ(ma_voice.vol_l, 0, "ma: second voice -> left 0");
    FCK_EQ(ma_voice.vol_r, 12700, "ma: second voice -> right volume");
    FCK_EQ(ma_voice.addr, 0x2000, "ma: second address");

    ma_state.second = &voices_a;
    fcm_sound.voice_volume = 2;
    gMovieAudioSetVoiceAttrs(&voices_a, 0, 0);
    FCK_EQ(ma_voice.vol_l, 254, "ma: both sides when both match (left)");
    FCK_EQ(ma_voice.vol_r, 254, "ma: both sides when both match (right)");
    gMovieAudioSetVoiceAttrs(&voices_b, 0, 0);
    FCK_EQ(ma_voice.vol_l | ma_voice.vol_r, 0, "ma: unknown voice -> silent");
}

/* ========================================================================
 * 80094F4C GPU reset
 * ======================================================================== */
u32 gGpuJobTail __asm__("D_8009DEEC");
u32 gGpuJobHead __asm__("D_8009DEE8");
u32 gSavedIrqMask __asm__("D_8009DEF8");
u32 *gDma2ChannelCtrlPtr __asm__("D_8009DED4");
u32 *gDmaControlGpuPtr __asm__("D_8009DEE4");
u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
u8 gGpuLibPair[0x100] __asm__("D_800A62C8");
u8 gGpuDmaQueue[0x1800] __asm__("D_800A9450");
s32 gResetGpuHardware(s32 a0) __asm__("func_80094F4C");

static u32 gr_chcr, gr_dpcr, gr_gp1;
static u32 gr_irq_ret, gr_probe_ret;
u32 mock_swap_irq(u32 m) __asm__("func_800915FC");
u32 mock_swap_irq(u32 m)
{
    fck_record("swap_irq", (s32)m, (s32)gr_gp1, (s32)gr_chcr);
    return gr_irq_ret;
}
u32 mock_probe(u32 a0) __asm__("func_80095350");
u32 mock_probe(u32 a0) { fck_record("probe", (s32)a0, 0, 0); return gr_probe_ret; }
void mock_memset(u8 *d, int v, unsigned n) __asm__("func_80095830");
void mock_memset(u8 *d, int v, unsigned n) { fck_record("memset", (s32)d, v, (s32)n); }

static void gr_reset(void)
{
    gr_chcr = 0x55; gr_dpcr = 0x0001; gr_gp1 = 0x77777777;
    gDma2ChannelCtrlPtr = &gr_chcr;
    gDmaControlGpuPtr = &gr_dpcr;
    gGpuGp1PortPtr = &gr_gp1;
    gGpuJobTail = 5; gGpuJobHead = 6; gSavedIrqMask = 0;
    gr_irq_ret = 0x0000ABCD; gr_probe_ret = 0x2;
    fck_log_reset();
}

static void test_80094F4C(void)
{
    s32 r;

    /* @leaf 80094F4C spec psx-spx GPU/DMA + Psy-Q libgpu ResetGraph(mode): DMA2 CHCR 0x401 = from RAM, linked-list sync mode 2; DPCR bit 11 (0x800) = DMA2 master enable; GP1(00h) = reset GPU; GP1(01h) = reset command buffer; source (src/game/80094F4C.c): mode & 7 dispatch (0 and 5 run full GPU reset + DMA2/DPCR setup + 0x100/0x1800 memset, with only (mode & 7) == 0 calling probe and returning its GPU type; 1 and 3 configure DMA2/DPCR and end with GP1(01h) — the RAM-backed port only observes the final GP1(01h) store, not the overwritten GP1(02h) store; modes 2/4/6/7 touch no hardware), IRQ mask save/restore and job queue clear */
    LK_FUNC("func_80094F4C");

    gr_reset();
    r = gResetGpuHardware(0);
    FCK_EQ(r, 2, "gr: mode 0 returns the probed GPU type");
    FCK_EQ(gr_chcr, 0x401, "gr: DMA2 CHCR from RAM, linked list");
    FCK_EQ(gr_dpcr, 0x801, "gr: DPCR DMA2 enable bit 11 set, others kept");
    FCK_EQ(gr_gp1, 0, "gr: GP1(00h) reset GPU");
    FCK_EQ(gGpuJobTail, 0, "gr: job tail cleared");
    FCK_EQ(gGpuJobHead, 0, "gr: job head cleared");
    FCK_EQ(gSavedIrqMask, 0xABCD, "gr: previous IRQ mask saved");
    FCK_EQ(fck_calls_to("swap_irq"), 2, "gr: mask swapped out and back");
    FCK_EQ(fck_call_n("swap_irq", 0).a0, 0, "gr: interrupts masked first");
    FCK_EQ(fck_call_n("swap_irq", 0).a2, 0x55, "gr: masked before touching DMA");
    FCK_EQ(fck_call_n("swap_irq", 1).a0, 0xABCD, "gr: saved mask restored");
    FCK_EQ(fck_call_n("swap_irq", 1).a2, 0x401, "gr: restored after the reset");
    FCK_EQ(fck_calls_to("memset"), 2, "gr: two state blocks cleared");
    FCK_EQ(fck_call_n("memset", 0).a0, (s32)gGpuLibPair, "gr: lib state block");
    FCK_EQ(fck_call_n("memset", 0).a2, 0x100, "gr: lib state 0x100 bytes");
    FCK_EQ(fck_call_n("memset", 1).a0, (s32)gGpuDmaQueue, "gr: DMA queue block");
    FCK_EQ(fck_call_n("memset", 1).a1 | fck_call_n("memset", 0).a1, 0, "gr: cleared to 0");
    FCK_EQ(fck_call_n("memset", 1).a2, 0x1800, "gr: DMA queue 0x1800 bytes");
    FCK_EQ(fck_call_n("probe", 0).a0, 0, "gr: probe gets the mode");

    /* mode 1: cancel drawing (RAM-backed port observes final GP1(01h) store only) */
    gr_reset();
    r = gResetGpuHardware(1);
    FCK_EQ(r, 0, "gr: mode 1 returns 0");
    FCK_EQ(gr_chcr, 0x401, "gr: mode 1 DMA2 CHCR");
    FCK_EQ(gr_dpcr, 0x801, "gr: mode 1 DPCR");
    FCK_EQ(gr_gp1, 0x1000000, "gr: last GP1 write is GP1(01h) reset command buffer");
    FCK_EQ(fck_calls_to("memset"), 0, "gr: mode 1 keeps lib state");
    FCK_EQ(fck_calls_to("probe"), 0, "gr: mode 1 no probe");

    /* mode 3 behaves like 1; mode 2 touches no hardware */
    gr_reset();
    r = gResetGpuHardware(3);
    FCK_EQ(r, 0, "gr: mode 3 returns 0");
    FCK_EQ(gr_chcr, 0x401, "gr: mode 3 DMA2 CHCR");
    FCK_EQ(gr_dpcr, 0x801, "gr: mode 3 DPCR");
    FCK_EQ(gr_gp1, 0x1000000, "gr: mode 3 last GP1 write is GP1(01h)");
    FCK_EQ(fck_calls_to("memset"), 0, "gr: mode 3 keeps lib state");
    FCK_EQ(fck_calls_to("probe"), 0, "gr: mode 3 no probe");
    gr_reset();
    r = gResetGpuHardware(2);
    FCK_EQ(r, 0, "gr: mode 2 returns 0");
    FCK_EQ(gr_chcr, 0x55, "gr: mode 2 leaves DMA2");
    FCK_EQ(gr_gp1, 0x77777777, "gr: mode 2 leaves GP1");
    FCK_EQ(gGpuJobHead, 0, "gr: mode 2 still empties the job queue");

    /* source (mode & 7): mode 8 has low 3 bits 0 -> full reset, probe gets full word */
    gr_reset();
    r = gResetGpuHardware(8);
    FCK_EQ(r, 2, "gr: mode & 7 == 0 -> full reset result");
    FCK_EQ(fck_call_n("probe", 0).a0, 8, "gr: probe gets the unmasked mode");
    /* source (mode & 7): mode 5 runs full hardware reset + memset, skips probe, returns 0 */
    gr_reset();
    r = gResetGpuHardware(5);
    FCK_EQ(r, 0, "gr: mode 5 returns 0");
    FCK_EQ(gr_chcr, 0x401, "gr: mode 5 DMA2 CHCR");
    FCK_EQ(gr_dpcr, 0x801, "gr: mode 5 DPCR");
    FCK_EQ(gr_gp1, 0, "gr: mode 5 resets the GPU with GP1(00h)");
    FCK_EQ(fck_calls_to("memset"), 2, "gr: mode 5 clears lib state");
    FCK_EQ(fck_calls_to("probe"), 0, "gr: mode 5 skips probe");
}

int main(void)
{
    test_80014490();
    test_80015830();
    test_80021554();
    test_8002DFC4();
    test_80040734();
    test_8004F188();
    test_8005D690();
    test_80063568();
    test_800684AC();
    test_8006AB38();
    test_8006C84C();
    test_8007CACC();
    test_80083834();
    test_80086C74();
    test_80094F4C();
    return lk_done("leaf_render");
}
