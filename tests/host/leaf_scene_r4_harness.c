/* Scene tranche 4: unchanged original C units under qemu-mipsel.
 *
 * The evidence notes cite the bodies in src/game/. Expected values are
 * derived from their stores, returns and call sites, not from test runs.
 * Only the allocator and block-free boundaries are replaced; neither mock
 * writes to the game's state.
 */
#include "leaf_kit.h"

struct Pair {
    s32 x;
    s32 y;
};

struct Pair *pair_base __asm__("D_8009F7B0");
extern struct Pair *gOffsetClampedCellCoords(s32 *arg) __asm__("func_8004F09C");

static void test_8004F09C(void)
{
    static const struct Pair initial_pairs[] = {
        {17, 23}, {-3, 5}, {31, -37}, {41, 43}
    };
    static const s32 initial_coords[] = {-0x80000, 0x3456, 0x98000, -0x7654};
    struct Pair pairs[4];
    s32 coords[4];

    /* @leaf 8004F09C source gOffsetClampedCellCoords.c:29-54 null guard, next-pair return, no writes or calls */
    LK_FUNC("func_8004F09C");

    memcpy(pairs, initial_pairs, sizeof(pairs));
    memcpy(coords, initial_coords, sizeof(coords));
    fck_log_reset();

    /* Lines 36-39 skip the input reads even when arg is null. */
    pair_base = 0;
    FCK_EQ(gOffsetClampedCellCoords(0), 0, "null pair base returns null");
    FCK_EQ(pair_base, 0, "null pair base stays null");

    /* Lines 40-54 compute intermediates but never expose them; the return
     * is one struct past the interior base, not a pointer to coordinates. */
    pair_base = &pairs[1];
    FCK_EQ(gOffsetClampedCellCoords(coords), &pairs[2], "returns the adjacent pair");
    FCK_EQ(pair_base, &pairs[1], "non-null pair base stays unchanged");
    FCK_EQ(memcmp(pairs, initial_pairs, sizeof(pairs)), 0, "pair table stays unchanged");
    FCK_EQ(memcmp(coords, initial_coords, sizeof(coords)), 0, "input coordinates stay unchanged");
    FCK_EQ(fck_log_count, 0, "coordinate lookup makes no dependency calls");
}

/* These three labels are words, declared as signed words by 800617D8
 * and as pointers by 80061860. Both use the same 32-bit MIPS slots. */
void *cell_work[2] __asm__("D_8009F818");
void *cell_map __asm__("D_8009F820");
void *third_work __asm__("D_8009F830");
u8 collision_candidates[0x8c] __asm__("D_800A8DC0");

static s32 block_tokens[5];
static u32 work_blocks[3][0x100];
static void *alloc_returns[3];
static struct {
    void *map;
    void *work0;
    void *work1;
    void *third;
    u8 flag0;
    u8 flag1;
} alloc_entry[3];

static const char alloc_call[] = "func_80017EBC";
static const char free_call[] = "func_800181D4";

/* The allocator's own definition returns void *; 800617D8 saves that same
 * MIPS return word as s32. Capture state at entry without changing it. */
void *mock_alloc(s32 size, s32 tag) __asm__("func_80017EBC");
void *mock_alloc(s32 size, s32 tag)
{
    int i = fck_log_count;

    fck_record(alloc_call, size, tag, 0);
    if (i >= 3)
        return 0;

    alloc_entry[i].map = cell_map;
    alloc_entry[i].work0 = cell_work[0];
    alloc_entry[i].work1 = cell_work[1];
    alloc_entry[i].third = third_work;
    alloc_entry[i].flag0 = collision_candidates[0];
    alloc_entry[i].flag1 = collision_candidates[0x44];
    return alloc_returns[i];
}

extern void gInitCellEntityWorkBuffers(void) __asm__("func_800617D8");

static void test_800617D8(void)
{
    int i;

    /* @leaf 800617D8 source gInitCellEntityWorkBuffers.c:42-50 map reset, three tagged allocations, two marker-byte clears */
    LK_FUNC("func_800617D8");

    cell_map = &block_tokens[0];
    cell_work[0] = &block_tokens[1];
    cell_work[1] = &block_tokens[2];
    third_work = &block_tokens[3];
    for (i = 0; i < 3; i++)
        alloc_returns[i] = work_blocks[i];
    lk_fill(work_blocks, 0x6b, sizeof(work_blocks));
    lk_fill(collision_candidates, 0xb4, sizeof(collision_candidates));
    collision_candidates[0] = 0x13;
    collision_candidates[0x44] = 0x27;
    memset(alloc_entry, 0, sizeof(alloc_entry));
    fck_log_reset();
    gInitCellEntityWorkBuffers();

    /* Lines 42-47 clear the map before the first call and publish each
     * result after that call, before moving on to the next destination. */
    FCK_EQ(fck_log_count, 3, "exactly three work allocations");
    for (i = 0; i < 3; i++) {
        FCK_EQ(fck_log[i].name, alloc_call, "work allocation call at each index");
        FCK_EQ(fck_log[i].a0, 0x400, "work allocation size");
        FCK_EQ(fck_log[i].a1, 2, "work allocation tag");
        FCK_EQ(alloc_entry[i].map, 0, "cell map is already reset at allocation");
        FCK_EQ(alloc_entry[i].third, &block_tokens[3], "third slot not yet published");
        FCK_EQ(alloc_entry[i].flag0, 0x13, "first flag not cleared until after allocation");
        FCK_EQ(alloc_entry[i].flag1, 0x27, "second flag not cleared until after allocation");
    }
    FCK_EQ(alloc_entry[0].work0, &block_tokens[1], "first slot still old at call one");
    FCK_EQ(alloc_entry[0].work1, &block_tokens[2], "second slot still old at call one");
    FCK_EQ(alloc_entry[1].work0, work_blocks[0], "call one result published by call two");
    FCK_EQ(alloc_entry[1].work1, &block_tokens[2], "second slot still old at call two");
    FCK_EQ(alloc_entry[2].work0, work_blocks[0], "first slot preserved at call three");
    FCK_EQ(alloc_entry[2].work1, work_blocks[1], "call two result published by call three");
    FCK_EQ(cell_map, 0, "cell map stays reset");
    FCK_EQ(cell_work[0], work_blocks[0], "first slot holds first allocation");
    FCK_EQ(cell_work[1], work_blocks[1], "second slot holds second allocation");
    FCK_EQ(third_work, work_blocks[2], "third slot holds third allocation");

    /* Lines 48-50 write just one byte per 0x44-byte record. Nothing in
     * lines 42-50 dereferences the returned work buffers. */
    FCK_EQ(collision_candidates[0], 0, "first record marker cleared");
    FCK_EQ(collision_candidates[0x44], 0, "second record marker cleared");
    FCK_CHECK(lk_all(collision_candidates, 1, 0x44, 0xb4), "first record payload unchanged");
    FCK_CHECK(lk_all(collision_candidates, 0x45, 0x8c, 0xb4), "second payload and trailing guard unchanged");
    FCK_CHECK(lk_all(work_blocks, 0, sizeof(work_blocks), 0x6b), "allocated bytes untouched");
}

/* 8004EFE4 passes a raw s32 in the same argument register as the pointer
 * callers. Record that word and the pair-base word observed at call entry. */
void mock_free(void *block) __asm__("func_800181D4");
void mock_free(void *block)
{
    fck_record(free_call, (s32)block, (s32)pair_base, 0);
}

#define EXPECT_FREE(index, block, what) do {                           \
    FCK_EQ(fck_log[index].name, free_call, what " invokes block free"); \
    FCK_EQ(fck_log[index].a0, block, what " passes the block");         \
} while (0)

void *tile_table __asm__("D_8009F808");
void *chunk_node __asm__("D_8009F810");
void *table_buffer __asm__("D_8009FA08");
void *terrain_base[2] __asm__("D_800A294C");
extern void gFreeTerrainWorkBuffers(void) __asm__("func_800634F4");

static void test_800634F4(void)
{
    void *tile = &block_tokens[0];
    void *chunk = &block_tokens[1];
    void *table = &block_tokens[2];
    void *height = &block_tokens[3];
    void *canary = &block_tokens[4];
    int i;
    static const struct {
        int mask;
        int count;
        int slots[4];
    } cases[] = {
        {15, 4, {0, 1, 2, 3}},
        {5,  2, {0, 2}},
        {10, 2, {1, 3}},
        {0,  0, {0}}
    };

    /* @leaf 800634F4 source gFreeTerrainWorkBuffers.c:36-47 four independent guarded frees in order, no pointer stores */
    LK_FUNC("func_800634F4");

    lk_fill(block_tokens, 0x58, sizeof(block_tokens));
    for (i = 0; i < (int)(sizeof(cases) / sizeof(cases[0])); i++) {
        void *before[4];
        int j;

        /* Lines 36-47 each check only their own slot, in tile/chunk/table/
         * heightfield order. Heightfield slot 1 is never read or freed. */
        before[0] = cases[i].mask & 1 ? tile : 0;
        before[1] = cases[i].mask & 2 ? chunk : 0;
        before[2] = cases[i].mask & 4 ? table : 0;
        before[3] = cases[i].mask & 8 ? height : 0;
        tile_table = before[0];
        chunk_node = before[1];
        table_buffer = before[2];
        terrain_base[0] = before[3];
        terrain_base[1] = canary;
        fck_log_reset();
        gFreeTerrainWorkBuffers();

        FCK_EQ(fck_log_count, cases[i].count, "guarded terrain free count");
        for (j = 0; j < cases[i].count; j++)
            EXPECT_FREE(j, before[cases[i].slots[j]], "ordered terrain free");

        FCK_EQ(tile_table, before[0], "tile slot unchanged");
        FCK_EQ(chunk_node, before[1], "chunk slot unchanged");
        FCK_EQ(table_buffer, before[2], "table slot unchanged");
        FCK_EQ(terrain_base[0], before[3], "heightfield slot zero unchanged");
        FCK_EQ(terrain_base[1], canary, "heightfield slot one unchanged");
        FCK_CHECK(lk_all(block_tokens, 0, sizeof(block_tokens), 0x58), "terrain blocks not written by caller");
    }
}

extern void gFreeCellMapAndWorkBuffers(void) __asm__("func_80061860");

static void test_80061860(void)
{
    /* @leaf 80061860 source gFreeCellMapAndWorkBuffers.c:37-43 conditional grid free followed by three unconditional work frees, no stores */
    LK_FUNC("func_80061860");

    lk_fill(block_tokens, 0x62, sizeof(block_tokens));
    cell_map = &block_tokens[0];
    cell_work[0] = &block_tokens[1];
    cell_work[1] = &block_tokens[2];
    third_work = &block_tokens[3];
    fck_log_reset();
    gFreeCellMapAndWorkBuffers();

    /* Lines 37-43: with a non-null map, all four frees receive their
     * distinct original pointers; no slot or buffer contents are stored. */
    FCK_EQ(fck_log_count, 4, "non-null map plus three work frees");
    EXPECT_FREE(0, &block_tokens[0], "map first");
    EXPECT_FREE(1, &block_tokens[1], "work slot zero second");
    EXPECT_FREE(2, &block_tokens[2], "work slot one third");
    EXPECT_FREE(3, &block_tokens[3], "third work pointer last");
    FCK_EQ(cell_map, &block_tokens[0], "non-null map slot unchanged");
    FCK_EQ(cell_work[0], &block_tokens[1], "work slot zero unchanged");
    FCK_EQ(cell_work[1], &block_tokens[2], "work slot one unchanged");
    FCK_EQ(third_work, &block_tokens[3], "third work slot unchanged");
    FCK_CHECK(lk_all(block_tokens, 0, sizeof(block_tokens), 0x62), "cell blocks not written by caller");

    /* Only the map is guarded. All three work pointers are passed even
     * when null, and all four slots stay null. */
    cell_map = 0;
    cell_work[0] = 0;
    cell_work[1] = 0;
    third_work = 0;
    fck_log_reset();
    gFreeCellMapAndWorkBuffers();

    FCK_EQ(fck_log_count, 3, "null map skipped but null work pointers are freed");
    EXPECT_FREE(0, 0, "null work slot zero");
    EXPECT_FREE(1, 0, "null work slot one");
    EXPECT_FREE(2, 0, "null third work pointer");
    FCK_EQ(cell_map, 0, "null map slot unchanged");
    FCK_EQ(cell_work[0], 0, "null work slot zero unchanged");
    FCK_EQ(cell_work[1], 0, "null work slot one unchanged");
    FCK_EQ(third_work, 0, "null third work slot unchanged");
}

extern void gReleaseCoordinatePairStorage(s32 arg) __asm__("func_8004EFE4");

static void test_8004EFE4(void)
{
    struct Pair pairs[2];

    /* @leaf 8004EFE4 source gReleaseCoordinatePairStorage.c:33-36 free the nonzero argument before always clearing the pair base */
    LK_FUNC("func_8004EFE4");

    /* The argument and global deliberately differ: the free uses arg,
     * and D_8009F7B0 still holds the old base inside the call. */
    lk_fill(pairs, 0x73, sizeof(pairs));
    pair_base = &pairs[0];
    fck_log_reset();
    gReleaseCoordinatePairStorage((s32)&pairs[1]);

    FCK_EQ(fck_log_count, 1, "nonzero argument produces exactly one free");
    EXPECT_FREE(0, &pairs[1], "release frees argument rather than global");
    FCK_EQ(fck_log[0].a1, &pairs[0], "pair base cleared after free");
    FCK_EQ(pair_base, 0, "nonzero argument clears pair base");
    FCK_CHECK(lk_all(pairs, 0, sizeof(pairs), 0x73), "pair contents not written by caller");

    /* Line 33 skips the call for a zero argument, not the line 36 store. */
    pair_base = &pairs[0];
    fck_log_reset();
    gReleaseCoordinatePairStorage(0);

    FCK_EQ(fck_log_count, 0, "zero argument skips free");
    FCK_EQ(pair_base, 0, "zero argument still clears pair base");
    FCK_CHECK(lk_all(pairs, 0, sizeof(pairs), 0x73), "zero argument leaves pair contents unchanged");
}

int main(void)
{
    test_8004F09C();
    test_800617D8();
    test_800634F4();
    test_80061860();
    test_8004EFE4();
    return lk_done("leaf_scene_r4");
}
