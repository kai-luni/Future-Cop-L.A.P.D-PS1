/* Terrain surface sampling and height query - two real units, no hardware.
 *
 * Units under test (unchanged decomp sources, linked verbatim):
 *   func_80067480  gSampleTileHeight   - bilinear / half-plane surface sample
 *   func_80067684  gQuerySurfaceHeight - map walk, sample collection, ranking
 *
 * Neither unit touches GTE, MMIO or the BIOS, and neither calls anything that
 * does: func_80067684 calls exactly one function, func_80067480, and that one is
 * a leaf. So this harness runs the full real call chain - query -> cell walk ->
 * per-surface sampling - with no mocked game logic at all. The only stand-ins
 * are the two data tables the sampler reads, generated from the retail image
 * byte for byte by terrain_surface_test.sh.
 *
 * Every expected value below was recomputed independently in Python from the
 * arithmetic in the units' doc blocks plus those tables - not recorded from a
 * run of the code under test. So a failure means the code changed, not that the
 * baseline moved.
 */
#include "fc_testkit.h"

/* Generated from D_8009AFE8 / D_8009B1A4 by terrain_surface_test.sh. */
#include "terrain_tables.h"

#define SUBTILE_WORDS 0x200

/* ------------------------------------------------------------ the two units */

s32 gSampleTileHeight(u8 *data, s32 index, s32 x, s32 y, s32 *out)
    __asm__("func_80067480");
s32 gQuerySurfaceHeight(void *pos, s32 mode) __asm__("func_80067684");

/* D_8009F80C is a pointer to the map header, D_8009F808 a pointer to the block
 * table, so both start zeroed - the state the retail machine is in before a map
 * loads. build_map points them at the fixtures. */
static u32 map_header[40];
static u32 tile_block[SUBTILE_WORDS];
static u32 block_table[1];

u32 gMapGridHeaderSlot __asm__("D_8009F80C");
u32 gTileBlockTableSlot __asm__("D_8009F808");

/* ------------------------------------------------------ sampling, per mode */

/* data[i] == i makes every corner index distinguishable, so a wrong corner
 * cannot accidentally produce the right height. */
static u8 ramp[64];

static void fill_ramp(void)
{
    int i;
    for (i = 0; i < 64; i++) ramp[i] = (u8)i;
}

/* mode 1 (surface 0): the half-plane rejects y + x <= 65535. */
static void test_sample_mode1(void)
{
    s32 out;
    s32 hit;

    out = -1;
    hit = gSampleTileHeight(ramp, 0, 32768, 32768, &out);
    FCK_EQ(hit, 1, "mode1 accepts the boundary y+x==65536");
    FCK_EQ(out, 10176, "mode1 height at the boundary");

    out = -1;
    hit = gSampleTileHeight(ramp, 0, 32768, 32767, &out);
    FCK_EQ(hit, 0, "mode1 rejects y+x==65535");
    FCK_EQ(out, -1, "mode1 leaves out untouched when it rejects");

    out = -1;
    hit = gSampleTileHeight(ramp, 0, 65535, 65535, &out);
    FCK_EQ(hit, 1, "mode1 accepts the far corner");
    FCK_EQ(out, 13821, "mode1 height at the far corner");

    out = -1;
    hit = gSampleTileHeight(ramp, 0, 1, 65535, &out);
    FCK_EQ(hit, 1, "mode1 accepts x=1,y=65535");
    FCK_EQ(out, 13055, "mode1 height at x=1,y=65535");
}

/* mode 0 (surface 1): the mirror image - rejects y + x > 65536. */
static void test_sample_mode0(void)
{
    s32 out;
    s32 hit;

    out = -1;
    hit = gSampleTileHeight(ramp, 1, 0, 0, &out);
    FCK_EQ(hit, 1, "mode0 accepts the origin");
    FCK_EQ(out, 0, "mode0 height at the origin");

    out = -1;
    hit = gSampleTileHeight(ramp, 1, 32768, 32768, &out);
    FCK_EQ(hit, 1, "mode0 accepts the exact boundary");
    FCK_EQ(out, 3648, "mode0 height at the exact boundary");

    out = -1;
    hit = gSampleTileHeight(ramp, 1, 32768, 32769, &out);
    FCK_EQ(hit, 0, "mode0 rejects one past the boundary");

    out = -1;
    hit = gSampleTileHeight(ramp, 1, 65535, 65535, &out);
    FCK_EQ(hit, 0, "mode0 rejects the far corner");
}

/* mode 2 (surface 6): rejects y < x, i.e. accepts y >= x. */
static void test_sample_mode2(void)
{
    s32 out;
    s32 hit;

    out = -1;
    hit = gSampleTileHeight(ramp, 6, 4096, 4096, &out);
    FCK_EQ(hit, 1, "mode2 accepts y == x");
    FCK_EQ(out, 1629, "mode2 height on the diagonal");

    out = -1;
    hit = gSampleTileHeight(ramp, 6, 4096, 4095, &out);
    FCK_EQ(hit, 0, "mode2 rejects y < x by one");

    out = -1;
    hit = gSampleTileHeight(ramp, 6, 0, 65535, &out);
    FCK_EQ(hit, 1, "mode2 accepts the y-dominant extreme");
    FCK_EQ(out, 13055, "mode2 height at the y extreme");
}

/* mode 3 (surface 7): the other diagonal, accepts x >= y. */
static void test_sample_mode3(void)
{
    s32 out;
    s32 hit;

    out = -1;
    hit = gSampleTileHeight(ramp, 7, 4096, 4096, &out);
    FCK_EQ(hit, 1, "mode3 accepts x == y");
    FCK_EQ(out, 99, "mode3 height on the diagonal");

    out = -1;
    hit = gSampleTileHeight(ramp, 7, 4095, 4096, &out);
    FCK_EQ(hit, 0, "mode3 rejects x < y by one");

    out = -1;
    hit = gSampleTileHeight(ramp, 7, 65535, 0, &out);
    FCK_EQ(hit, 1, "mode3 accepts the x extreme");
    FCK_EQ(out, 766, "mode3 height at the x extreme");
}

/* index >= 60 is the quad path: no half-plane test, four corners interpolated. */
static void test_sample_quad(void)
{
    s32 out;
    s32 hit;

    out = -1;
    hit = gSampleTileHeight(ramp, 60, 0, 0, &out);
    FCK_EQ(hit, 1, "a quad always samples");
    FCK_EQ(out, 256, "quad 60 height at the origin");

    out = -1;
    hit = gSampleTileHeight(ramp, 60, 32768, 32768, &out);
    FCK_EQ(hit, 1, "the quad samples at the centre");
    FCK_EQ(out, 7040, "quad 60 height at the centre");

    /* A different quad has a different corner set, so the value must differ. */
    out = -1;
    hit = gSampleTileHeight(ramp, 61, 0, 0, &out);
    FCK_EQ(out, 512, "quad 61 has its own corner set");

    out = -1;
    hit = gSampleTileHeight(ramp, 63, 65535, 65535, &out);
    FCK_EQ(hit, 1, "the last quad samples");
    FCK_EQ(out, 14333, "quad 63 height at the far corner");
}

/* ----------------------------------------------------------- the query unit */

#define MAP_Z 3
#define MAP_X 3

/* A position that lands in cell (1,1), sub-cell tx=1 tz=1: the sub-cell index is
 * tx*16 + tz, so this exercises a non-trivial tile offset rather than tile 0. */
static s32 pos[3];

/* Deterministic irregular byte fill. A regular ramp is not good enough here:
 * neighbouring corner offsets would collide and two different surface types
 * would report the same height, which would hide a wrong corner lookup. */
static u8 fill_byte(int offset)
{
    u32 v = (u32)offset * 0x9E3779B9u + 0x5A5A5A5Au;
    v ^= v >> 13;
    return (u8)(v & 0xFF);
}

/* Build a tile block holding `count` surfaces of the given types per sub-cell.
 *
 * Layout the units read:
 *   header +0x04 = z dimension, +0x08 = x dimension, +0x14 = cell index words
 *   cell word -> slot, block = block_table[(slot >> 2) & 0x3FFF]
 *   block + 0x3CC + 2 * tile = u16 descriptor: (offset << 4) | count
 *   block + 0x5D0 + offset   = descriptor list, type = (word >> 15) & 0x7F
 *   block + 0x0C + 3 * tile2 = the s8 corner height table for that sub-cell
 */
static void build_map(const int *types, int count)
{
    int i;
    u8 *blk = (u8 *)tile_block;

    for (i = 0; i < SUBTILE_WORDS; i++) tile_block[i] = 0;
    for (i = 0; i < 40; i++) map_header[i] = 0;
    for (i = 0; i < (int)sizeof(tile_block); i++) blk[i] = fill_byte(i);

    map_header[0x04 / 4] = MAP_Z;
    map_header[0x08 / 4] = MAP_X;

    /* Every cell resolves to slot 0, so every cell uses block 0. */
    for (i = 0; i < MAP_Z * MAP_X; i++)
        map_header[(0x14 / 4) + i] = 0;

    block_table[0] = (u32)(unsigned long)tile_block;
    gMapGridHeaderSlot = (u32)(unsigned long)map_header;
    gTileBlockTableSlot = (u32)(unsigned long)block_table;

    for (i = 0; i < 0x100; i++)
        *(u16 *)(blk + 0x3CC + 2 * i) = (u16)((0u << 4) | (u32)count);

    for (i = 0; i < count; i++)
        *(u32 *)(blk + 0x5D0 + 4 * i) = ((u32)types[i] & 0x7Fu) << 15;
}

static void aim(void)
{
    pos[0] = 0x3FFF;   /* x */
    pos[1] = 0;        /* y, unused by this unit */
    pos[2] = 0x3FFF;   /* z */
}

/* A mode below -2 is refused before anything is read. */
static void test_query_rejects_bad_mode(void)
{
    int types[1];
    types[0] = 1;
    build_map(types, 1);
    aim();
    FCK_EQ(gQuerySurfaceHeight(pos, -3), 0, "mode -3 is refused");
}

/* The bounds check is `z17 < dim - 1`, so the last row and column are outside. */
static void test_query_rejects_outside_map(void)
{
    int types[1];
    types[0] = 1;
    build_map(types, 1);

    pos[0] = 0; pos[1] = 0; pos[2] = -1;
    FCK_EQ(gQuerySurfaceHeight(pos, -1), 0, "a negative z is outside the map");

    pos[0] = 0; pos[1] = 0; pos[2] = (MAP_Z - 1) << 17;
    FCK_EQ(gQuerySurfaceHeight(pos, -1), 0, "z at the last row is outside");

    pos[0] = -1; pos[1] = 0; pos[2] = 0;
    FCK_EQ(gQuerySurfaceHeight(pos, -1), 0, "a negative x is outside the map");

    pos[0] = (MAP_X - 1) << 17; pos[1] = 0; pos[2] = 0;
    FCK_EQ(gQuerySurfaceHeight(pos, -1), 0, "x at the last column is outside");

    /* One step inside the last row must be accepted, not just "not rejected". */
    pos[0] = 0; pos[1] = 0; pos[2] = ((MAP_Z - 1) << 17) - 1;
    FCK_CHECK(gQuerySurfaceHeight(pos, -1) != 0,
              "one step inside the last row is accepted");
}

/* With a single surface, the lowest, the highest and index 0 must all agree. */
static void test_query_single_surface(void)
{
    int types[1];
    s32 high, low, first;

    types[0] = 6;
    build_map(types, 1);
    aim();

    high = gQuerySurfaceHeight(pos, -1);
    low = gQuerySurfaceHeight(pos, -2);
    first = gQuerySurfaceHeight(pos, 0);

    FCK_EQ(high, -14078, "the single surface height");
    FCK_EQ(low, high, "the lowest equals the highest for one surface");
    FCK_EQ(first, high, "index 0 equals the highest for one surface");
}

/* Two surfaces: the list is sorted ascending, so -2 is the minimum and -1 the
 * maximum regardless of the descriptor order. */
static void test_query_ranks_surfaces(void)
{
    int forward[2];
    int reversed[2];
    s32 lo_f, hi_f, lo_r, hi_r;

    forward[0] = 6; forward[1] = 7;
    build_map(forward, 2);
    aim();
    lo_f = gQuerySurfaceHeight(pos, -2);
    hi_f = gQuerySurfaceHeight(pos, -1);

    reversed[0] = 7; reversed[1] = 6;
    build_map(reversed, 2);
    aim();
    lo_r = gQuerySurfaceHeight(pos, -2);
    hi_r = gQuerySurfaceHeight(pos, -1);

    FCK_EQ(lo_f, -14078, "type 6 samples lower");
    FCK_EQ(hi_f, -14075, "type 7 samples higher");
    FCK_CHECK(lo_f < hi_f, "the sorted list orders the two surfaces");
    /* The order in the descriptor list must not change the answer. */
    FCK_EQ(lo_r, lo_f, "reversing the list keeps the minimum");
    FCK_EQ(hi_r, hi_f, "reversing the list keeps the maximum");

    /* mode 0 / 1 index the same ascending list. */
    FCK_EQ(gQuerySurfaceHeight(pos, 0), lo_f, "index 0 is the first of the list");
    FCK_EQ(gQuerySurfaceHeight(pos, 1), hi_f, "index 1 is the second of the list");
}

/* A sub-cell with no surfaces must report failure, not a stale value. */
static void test_query_empty_subtile(void)
{
    int types[1];
    int tile;
    u8 *blk = (u8 *)tile_block;

    types[0] = 1;
    build_map(types, 1);
    aim();

    /* Sub-cell index for tx=1 tz=1 is 1*16 + 1. */
    tile = ((pos[0] >> 13) & 0xF) * 16 + ((pos[2] >> 13) & 0xF);
    *(u16 *)(blk + 0x3CC + 2 * tile) = 0;
    FCK_EQ(gQuerySurfaceHeight(pos, -1), 0, "an empty sub-cell reports 0");
}

/* A descriptor type at or above 0x47 ends the list, so it yields nothing. */
static void test_query_type_out_of_range(void)
{
    int types[1];

    types[0] = 0x47;
    build_map(types, 1);
    aim();
    FCK_EQ(gQuerySurfaceHeight(pos, -1), 0, "a type >= 0x47 ends the list");
}

/* A valid type whose half-plane test rejects at this angle yields nothing
 * either - a zero from the sampler is a miss, not a height of zero. */
static void test_query_sample_miss(void)
{
    int types[1];

    types[0] = 1;   /* mode 0: rejects y + x > 65536, which holds here */
    build_map(types, 1);
    aim();
    FCK_EQ(gQuerySurfaceHeight(pos, -1), 0, "a missed sample reports 0");
}

int main(void)
{
    fill_ramp();

    test_sample_mode1();
    test_sample_mode0();
    test_sample_mode2();
    test_sample_mode3();
    test_sample_quad();

    test_query_rejects_bad_mode();
    test_query_rejects_outside_map();
    test_query_single_surface();
    test_query_ranks_surfaces();
    test_query_empty_subtile();
    test_query_type_out_of_range();
    test_query_sample_miss();

    return fck_report("terrain-surface");
}
