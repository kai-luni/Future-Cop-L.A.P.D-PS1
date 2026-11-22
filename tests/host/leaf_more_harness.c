/* Additional original C units exercised under qemu-mipsel. No target
 * functions are mocked: every D_ symbol below is a synthetic RAM fixture.
 * "source" pins observed source behavior where the wider contract is not
 * independently established. Layouts and aliased signatures come from the
 * respective src/game units; the build script is generated from @leaf. */
#include "leaf_kit.h"

#define Q16 0x10000

s32 gSquaredLength3Q16(s32 *arg0) __asm__("func_80019FCC");
s32 gSquaredLength2Q16(s32 *arg0) __asm__("func_8001A030");
void gTransformPointQ16(volatile s32 *m, volatile s32 *v, s32 *out) __asm__("func_8001A970");
s32 gVectorDot(s32 *a, s32 *b) __asm__("func_8001AB00");
void gScaleVector2Q16(s32 scale, s32 *p) __asm__("func_8001ABE0");
void gVectorMulAdd(s32 *a, s32 *b, s32 scale, s32 *out) __asm__("func_8001AC28");
void gCrossProductQ16(s32 *a, s32 *b, s32 *out) __asm__("func_8001AD20");
s32 gComputeBoomDistance(s32 arg0) __asm__("func_800564A8");

static void test_vectors(void)
{
    s32 lengths[] = { -3 * Q16, 4 * Q16, 12 * Q16 };
    s32 tiny[] = { 200, -200, 200 };
    s32 a[] = { 2 * Q16, -3 * Q16, 4 * Q16 };
    s32 b[] = { -Q16, 5 * Q16, 2 * Q16 };
    s32 zero[] = { 0, 0, 0 };
    /* (x,y,z) -> (2y-z, x/2, -y+z) + (7,-3,2). */
    s32 affine[] = { 0, Q16 / 2, 0, 2 * Q16, 0, -Q16,
                     -Q16, 0, Q16, 7 * Q16, -3 * Q16, 2 * Q16 };
    s32 out[4];
    s32 small[] = { 3, -3, 0x1234 };

    /* @leaf 80019FCC math 3-4-12 triangle; each sub-LSB square truncates before summation */
    LK_FUNC("func_80019FCC");
    FCK_EQ(gSquaredLength3Q16(lengths), 169 * Q16, "squared 3-4-12 length");
    FCK_EQ(gSquaredLength3Q16(tiny), 0, "three squares truncate separately");

    /* @leaf 8001A030 math 3-4 triangle ignores the third word; each square truncates separately */
    LK_FUNC("func_8001A030");
    FCK_EQ(gSquaredLength2Q16(lengths), 25 * Q16, "squared 3-4 length");
    FCK_EQ(gSquaredLength2Q16(tiny), 0, "two squares truncate separately");

    /* @leaf 8001A970 math asymmetric affine map distinguishes axes, layout and translation */
    LK_FUNC("func_8001A970");
    lk_fill(out, 0xa5, sizeof(out));
    gTransformPointQ16(affine, a, out);
    FCK_EQ(out[0], -3 * Q16, "affine x");
    FCK_EQ(out[1], -2 * Q16, "affine y");
    FCK_EQ(out[2], 9 * Q16, "affine z");
    gTransformPointQ16(affine, zero, out);
    FCK_CHECK(out[0] == 7 * Q16 && out[1] == -3 * Q16 && out[2] == 2 * Q16, "origin maps to translation");
    FCK_CHECK(lk_all(out, 12, 16, 0xa5), "affine output ends after z");

    /* @leaf 8001AB00 math (2,-3,4) dot (-1,5,2) is -9; zero annihilates */
    LK_FUNC("func_8001AB00");
    FCK_EQ(gVectorDot(a, b), -9 * Q16, "signed dot product");
    FCK_EQ(gVectorDot(a, zero), 0, "dot product with zero");

    /* @leaf 8001ABE0 math negative half-scale floors a negative fractional result */
    LK_FUNC("func_8001ABE0");
    gScaleVector2Q16(-Q16 / 2, small);
    FCK_EQ(small[0], -2, "minus 1.5 rounds down");
    FCK_EQ(small[1], 1, "plus 1.5 rounds down");
    gScaleVector2Q16(0, small);
    FCK_CHECK(small[0] == 0 && small[1] == 0 && small[2] == 0x1234, "zero scale changes only two words");

    /* @leaf 8001AC28 math base plus half of a signed direction; zero scale keeps base */
    LK_FUNC("func_8001AC28");
    lk_fill(out, 0xa5, sizeof(out));
    gVectorMulAdd(a, b, Q16 / 2, out);
    FCK_EQ(out[0], 3 * Q16 / 2, "multiply-add x");
    FCK_EQ(out[1], -Q16 / 2, "multiply-add y");
    FCK_EQ(out[2], 5 * Q16, "multiply-add z");
    gVectorMulAdd(a, b, 0, out);
    FCK_CHECK(memcmp(out, a, sizeof(a)) == 0 && lk_all(out, 12, 16, 0xa5), "zero step and output boundary");

    /* @leaf 8001AD20 math oriented cross product and parallel-vector degeneracy */
    LK_FUNC("func_8001AD20");
    lk_fill(out, 0xa5, sizeof(out));
    gCrossProductQ16(a, b, out);
    FCK_EQ(out[0], -26 * Q16, "cross x");
    FCK_EQ(out[1], -8 * Q16, "cross y");
    FCK_EQ(out[2], 7 * Q16, "cross z");
    gCrossProductQ16(a, a, out);
    FCK_CHECK(memcmp(out, zero, sizeof(zero)) == 0 && lk_all(out, 12, 16, 0xa5), "parallel vectors and output boundary");

    /* @leaf 800564A8 source quadratic camera boom at baseline, symmetric half-steps, vertex and floor */
    LK_FUNC("func_800564A8");
    FCK_EQ(gComputeBoomDistance(330), 0xd800, "boom baseline");
    FCK_EQ(gComputeBoomDistance(630), 0x25800, "boom before vertex");
    FCK_EQ(gComputeBoomDistance(930), 0x2d800, "boom vertex");
    FCK_EQ(gComputeBoomDistance(1230), 0x25800, "boom after vertex");
    FCK_EQ(gComputeBoomDistance(2130), 0x9000, "boom minimum");
}

s32 gPlayerCount __asm__("D_8009FA0C");
s32 gTestFrustumFixedNear(s16 *point, s32 margin) __asm__("func_80019D90");
s32 gTestFrustumMargin(s16 *point, s32 margin) __asm__("func_80019E70");
s32 gIsInViewFrustumWithMargin(s16 *point, s32 margin) __asm__("func_80063FF8");

static void test_frusta(void)
{
    s16 p[3] = { 300, 0, 256 };

    /* @leaf 80019D90 source fixed near code 1 through depth 0x1000; side rejection and split-screen width */
    LK_FUNC("func_80019D90");
    gPlayerCount = 1;
    FCK_EQ(gTestFrustumFixedNear(p, 0), 1, "inclusive fixed near plane");
    p[2] = 257;
    FCK_EQ(gTestFrustumFixedNear(p, 0), 0, "beyond near, outside horizontally");
    p[0] = -160; p[1] = 120; p[2] = 512;
    FCK_EQ(gTestFrustumFixedNear(p, 0), 2, "single-player exact side and top edges");
    gPlayerCount = 2;
    FCK_EQ(gTestFrustumFixedNear(p, 0), 0, "split-screen rejects former side edge");
    p[0] = -80; p[1] = -121;
    FCK_EQ(gTestFrustumFixedNear(p, 0), 0, "split-screen outside bottom");
    p[1] = -120;
    FCK_EQ(gTestFrustumFixedNear(p, 0), 2, "split-screen exact side and bottom edges");

    /* @leaf 80019E70 source adjustable near code through 0x405, inclusive horizontal and vertical edges */
    LK_FUNC("func_80019E70");
    gPlayerCount = 1;
    p[0] = 300; p[1] = 0; p[2] = 64;
    FCK_EQ(gTestFrustumMargin(p, 5), 1, "depth plus margin just short of 0x406");
    FCK_EQ(gTestFrustumMargin(p, 6), 0, "at 0x406 proceeds to horizontal rejection");
    p[0] = 161; p[1] = 120; p[2] = 512;
    FCK_EQ(gTestFrustumMargin(p, 0), 0, "just outside side without margin");
    FCK_EQ(gTestFrustumMargin(p, 16), 2, "margin admits side by twelve units");
    p[0] = 160; p[1] = 121;
    FCK_EQ(gTestFrustumMargin(p, 0), 0, "outside vertically");
    gPlayerCount = 2;
    p[0] = 80; p[1] = 120;
    FCK_EQ(gTestFrustumMargin(p, 0), 2, "split-screen edges included");

    /* @leaf 80063FF8 source boolean visibility uses distinct near threshold 0x601 and split-screen side limits */
    LK_FUNC("func_80063FF8");
    gPlayerCount = 1;
    p[0] = 0; p[1] = 0; p[2] = 96;
    FCK_EQ(gIsInViewFrustumWithMargin(p, 0), 0, "one unit short of view near plane");
    FCK_EQ(gIsInViewFrustumWithMargin(p, 1), 1, "margin reaches view near plane");
    p[0] = 160; p[1] = 120; p[2] = 512;
    FCK_EQ(gIsInViewFrustumWithMargin(p, 0), 1, "single-player inclusive edges");
    gPlayerCount = 2;
    FCK_EQ(gIsInViewFrustumWithMargin(p, 0), 0, "split-screen narrower than single-player");
    p[0] = 80; p[1] = 121;
    FCK_EQ(gIsInViewFrustumWithMargin(p, 0), 0, "vertical miss despite valid x");
    p[1] = 120;
    FCK_EQ(gIsInViewFrustumWithMargin(p, 0), 1, "split-screen inclusive edges");
}

/* The first four words are shared by both clamp routines; the last two
 * scales are read only by the minimap conversion. */
s32 gMapGridBounds[6] __asm__("D_800A2950");
int gMapGridHeader __asm__("D_8009F80C");
typedef struct { s32 unk0, unk4, unk8; s8 *unkC; } Unk800A2940;
Unk800A2940 gHeightGridDesc __asm__("D_800A2940");
s32 gClampToWorldBounds(s32 *arg0) __asm__("func_8006943C");
s32 gClampMapViewToBounds(s32 *arg0, s32 arg1) __asm__("func_800665E8");
void gWorldToMinimapCell(int *out_x, int *out_z, int x, int z) __asm__("func_800694C8");
s32 gSampleHeightGridBilinear(s32 *arg0) __asm__("func_80069A38");

static void test_map(void)
{
    s32 p[3] = { 0, 123, 0 };
    int x, z;
    /* Three x strips, each with three consecutive z samples. */
    s8 heights[] = { -20, 12, 28, 60, -4, 20, 100, 76, 52 };

    /* @leaf 8006943C semantic fixed-margin clamps report each changed axis; equality does not move */
    LK_FUNC("func_8006943C");
    gMapGridBounds[0] = 0x10000; gMapGridBounds[1] = 0x50000;
    gMapGridBounds[2] = -0x30000; gMapGridBounds[3] = 0x70000;
    p[0] = -0x2f800; p[2] = 0x4f800;
    FCK_EQ(gClampToWorldBounds(p), 0, "world exact limits unchanged");
    p[2]++;
    FCK_EQ(gClampToWorldBounds(p), 1, "world high z alone");
    FCK_EQ(p[2], 0x4f800, "world z upper limit");
    p[0]--;
    FCK_EQ(gClampToWorldBounds(p), 2, "world low x alone");
    FCK_EQ(p[0], -0x2f800, "world x lower limit");
    p[0] = 0x70000; p[2] = 0x10000;
    FCK_EQ(gClampToWorldBounds(p), 3, "both world axes clamped");
    FCK_CHECK(p[0] == 0x6f800 && p[1] == 123 && p[2] == 0x10800, "world high x and low z leave y alone");

    /* @leaf 800665E8 source view uses fixed x inset 0x14000 and z inset derived from view width */
    LK_FUNC("func_800665E8");
    p[0] = -0x1c000; p[2] = 0x48000;
    FCK_EQ(gClampMapViewToBounds(p, 0xc000), 0, "view exactly at both limits");
    p[2]++;
    FCK_EQ(gClampMapViewToBounds(p, 0xc000), 1, "view clamps high z only");
    FCK_EQ(p[2], 0x48000, "view high z with custom inset");
    p[0]--;
    FCK_EQ(gClampMapViewToBounds(p, 0xc000), 2, "view clamps low x only");
    FCK_EQ(p[0], -0x1c000, "view low x inset");
    p[0] = 0x70000; p[2] = 0x10000;
    FCK_EQ(gClampMapViewToBounds(p, 0x5000), 3, "both view axes clamped");
    FCK_CHECK(p[0] == 0x5c000 && p[1] == 123 && p[2] == 0x11000, "view high x, low z, unchanged y");

    /* @leaf 800694C8 source nearest-cell rounding with different synthetic scales, clipping and absent grid */
    LK_FUNC("func_800694C8");
    gMapGridBounds[4] = 0x10000; gMapGridBounds[5] = 0x20000;
    gMapGridHeader = 0;
    x = z = 99;
    gWorldToMinimapCell(&x, &z, 0x18000, -0x24000);
    FCK_CHECK(x == 0 && z == 0, "no grid clears both cell indices");
    gMapGridHeader = 1;
    gWorldToMinimapCell(&x, &z, 0x17fff, -0x24000);
    FCK_CHECK(x == 0 && z == 2, "just below half x; z at one and a half cells");
    gWorldToMinimapCell(&x, &z, 0x18000, -0x24001);
    FCK_CHECK(x == 1 && z == 1, "half x rounds up; just below half z rounds down");
    gWorldToMinimapCell(&x, &z, 0, -0x30000 + 60 * 0x8000);
    FCK_CHECK(x == 0 && z == 53, "negative x and oversized z clipped");
    gWorldToMinimapCell(&x, &z, 0x10000 + 60 * Q16, -0x40000);
    FCK_CHECK(x == 53 && z == 0, "oversized x and negative z clipped");

    /* @leaf 80069A38 math signed heightfield, unequal bilinear weights and exact cell/domain edges */
    LK_FUNC("func_80069A38");
    gHeightGridDesc.unk0 = 0x8000;
    gHeightGridDesc.unk4 = 3;
    gHeightGridDesc.unk8 = 1;
    gHeightGridDesc.unkC = heights;
    p[0] = p[2] = 0x8000;
    FCK_EQ(gSampleHeightGridBilinear(p), -20 * 256, "signed height at origin");
    /* At z=1/4 the first two strips give -12 and 44; at x=3/4 the blend is 30. */
    p[0] = 0xb000; p[2] = 0x9000;
    FCK_EQ(gSampleHeightGridBilinear(p), 30 * 256, "asymmetric bilinear sample");
    p[0] = p[2] = 0xc000;
    FCK_EQ(gSampleHeightGridBilinear(p), -4 * 256, "last eligible x/z cell");
    p[0] = 0x10000;
    FCK_EQ(gSampleHeightGridBilinear(p), 0x8000, "x needs a next strip");
    p[0] = 0xc000; p[2] = 0x10000;
    FCK_EQ(gSampleHeightGridBilinear(p), 0x8000, "z needs a next sample");
    p[2] = 0x7fff;
    FCK_EQ(gSampleHeightGridBilinear(p), 0x8000, "below z origin by one unit");
    p[0] = 0x7fff; p[2] = 0x8000;
    FCK_EQ(gSampleHeightGridBilinear(p), 0x8000, "below x origin by one unit");
}

s32 *gFreeRangeTablePtr __asm__("D_8009F590");
s32 gFreeRangeCount __asm__("D_8009F594");
s32 gHeapSearchCursor __asm__("D_8009F598");
typedef struct Block { struct Block *next; int size; } Block;
Block *gSessionBlockBase __asm__("D_8009C7D0");
s32 gFindHeapRangeForward(s32 size) __asm__("func_80017D5C");
s32 gFindHeapRangeBackward(s32 size) __asm__("func_80017E0C");
void *gAllocHeapBlock(u32 size) __asm__("func_8008CA90");

static void test_heap(void)
{
    /* Word zero is reserved; the odd indices begin ranges of 50, 10, 20, 50 bytes. */
    s32 spans[] = { 0, 100, 150, 200, 210, 300, 320, 400, 450 };
    Block area[8];
    gFreeRangeTablePtr = spans;
    gFreeRangeCount = sizeof(spans) / sizeof(spans[0]);
    gHeapSearchCursor = 5;

    /* @leaf 80017D5C semantic forward next-fit from cursor, exact fit, wrap and exhaustion */
    LK_FUNC("func_80017D5C");
    FCK_EQ(gFindHeapRangeForward(20), 5, "forward includes exact fit at cursor");
    FCK_EQ(gFindHeapRangeForward(50), 7, "forward prefers later fit over wrapped fit");
    gFreeRangeCount = 7;
    FCK_EQ(gFindHeapRangeForward(50), 1, "forward wraps to first pair");
    FCK_EQ(gFindHeapRangeForward(51), -1, "forward no sufficiently long pair");

    /* @leaf 80017E0C semantic reverse next-fit starts before cursor then wraps from last pair */
    LK_FUNC("func_80017E0C");
    gFreeRangeCount = 9;
    FCK_EQ(gFindHeapRangeBackward(10), 3, "backward starts one pair before cursor");
    FCK_EQ(gFindHeapRangeBackward(50), 1, "backward prefers first pair over wrapped fit");
    gHeapSearchCursor = 3;
    spans[2] = 105;
    FCK_EQ(gFindHeapRangeBackward(50), 7, "backward wraps to last pair");
    FCK_EQ(gFindHeapRangeBackward(51), -1, "backward no sufficiently long pair");

    /* @leaf 8008CA90 semantic circular next-fit heap splits from the high end and unlinks exact fits */
    LK_FUNC("func_8008CA90");
    /* Same circular format as gInitHeapBlocks (8008CA30), without linking that
     * already-covered function: a seven-unit free block and zero-size sentinel. */
    lk_fill(area, 0xa5, sizeof(area));
    area[0].next = &area[7]; area[0].size = 7;
    area[7].next = &area[0]; area[7].size = 0;
    gSessionBlockBase = &area[0];
    FCK_CHECK(gAllocHeapBlock(0) == 0 && gSessionBlockBase == &area[0], "zero size does not move cursor");
    FCK_CHECK(gAllocHeapBlock(49) == 0 && area[0].size == 7, "too large includes header and leaves free block");
    FCK_CHECK(gAllocHeapBlock(9) == &area[5], "9 bytes require two payload units plus header");
    FCK_CHECK(area[0].size == 4 && area[4].size == 3, "split sizes in eight-byte units");
    FCK_CHECK(gSessionBlockBase == &area[7] && area[7].next == &area[0], "split keeps free block linked");
    FCK_CHECK(gAllocHeapBlock(24) == &area[1], "24 bytes consume last four units exactly");
    FCK_CHECK(area[7].next == &area[7] && gSessionBlockBase == &area[7], "exact fit unlinks consumed block");
    FCK_CHECK(gAllocHeapBlock(1) == 0, "exhausted heap returns null");
}

struct Resource { u8 unk00[0x0c]; struct Resource *next, *prev; };
struct Resource *gResourceListHead __asm__("D_8009F5B8");
struct Resource *gResourceListTail __asm__("D_8009F5BC");
struct Unk5A654Sample { s16 id; u8 rate; };
struct Unk5A654Sample **gTextureDescriptorTable __asm__("D_8009F7D8");
s32 gTexDescriptorCount __asm__("D_8009F7DC");
struct Unk6AAEC { u8 id; u8 unk01[0x53]; };
s32 gDescriptorRegistryCount __asm__("D_8009F848");
struct Unk6AAEC gVmRecordTable[4] __asm__("D_800A3340");
typedef struct { u16 id, offset; } TableEntry;
typedef struct { u16 unk0, count; TableEntry entries[1]; } TableHeader;
typedef struct { TableHeader *header; u8 unk04[0x18]; s32 unk1C; } Unk800899A4;
Unk800899A4 *gTextLookupTable __asm__("D_8009F518");

void gResetResourceRegistry(void) __asm__("func_80019A64");
void gAppendResourceRegistryEntry(struct Resource *resource) __asm__("func_80019A74");
s32 gFindTexDescriptorIndex(s32 id) __asm__("func_8005A654");
struct Unk6AAEC *gRegistryFind(u8 id) __asm__("func_8006AAEC");
u8 *gLookupTextString(u16 id) __asm__("func_80089914");

static void test_registries(void)
{
    struct Resource a, b;
    struct Unk5A654Sample first = { -6, 2 }, second = { 300, 3 }, duplicate = { -6, 4 };
    struct Unk5A654Sample *samples[] = { &first, &second, &duplicate };
    /* Synthetic sorted string archive. All offsets are relative to the
     * first entry, not the header; distinct strings sit beyond the table. */
    union {
        TableHeader header;
        struct { u16 reserved, count; TableEntry entries[3]; u8 strings[12]; } data;
    } archive = { { 0, 0, { { 0, 0 } } } };
    Unk800899A4 lookup;

    /* @leaf 80019A64 semantic resetting a populated registry clears both ends, not the nodes */
    LK_FUNC("func_80019A64");
    lk_fill(&a, 0xa5, sizeof(a)); lk_fill(&b, 0x5a, sizeof(b));
    a.next = &b; a.prev = 0; b.next = 0; b.prev = &a;
    gResourceListHead = &a; gResourceListTail = &b;
    gResetResourceRegistry();
    FCK_CHECK(gResourceListHead == 0 && gResourceListTail == 0, "reset clears both registry ends");
    FCK_CHECK(a.next == &b && b.prev == &a, "reset does not unlink or clear records");

    /* @leaf 80019A74 semantic append builds forward/back links in empty and nonempty registries */
    LK_FUNC("func_80019A74");
    gResourceListHead = gResourceListTail = 0;
    a.next = a.prev = &b;
    gAppendResourceRegistryEntry(&a);
    FCK_CHECK(gResourceListHead == &a && gResourceListTail == &a, "first resource becomes both ends");
    FCK_CHECK(a.next == 0 && a.prev == 0, "first resource stale links cleared");
    b.next = &a; b.prev = 0;
    gAppendResourceRegistryEntry(&b);
    FCK_CHECK(gResourceListHead == &a && gResourceListTail == &b, "append moves only tail");
    FCK_CHECK(a.next == &b && b.prev == &a && b.next == 0, "append joins both directions");
    FCK_CHECK(lk_all(&a, 0, 12, 0xa5) && lk_all(&b, 0, 12, 0x5a), "resource payload preserved");

    /* @leaf 8005A654 semantic signed id lookup: first duplicate wins, count excludes extra entry */
    LK_FUNC("func_8005A654");
    gTextureDescriptorTable = samples;
    gTexDescriptorCount = 3;
    FCK_EQ(gFindTexDescriptorIndex(-6), 0, "first duplicate signed texture id");
    FCK_EQ(gFindTexDescriptorIndex(300), 1, "texture id wider than a byte");
    FCK_EQ(gFindTexDescriptorIndex(65530), -1, "id comparison does not truncate caller input");
    gTexDescriptorCount = 1;
    FCK_EQ(gFindTexDescriptorIndex(300), -1, "entry beyond texture count ignored");
    gTexDescriptorCount = 0;
    FCK_EQ(gFindTexDescriptorIndex(-6), -1, "empty texture registry");

    /* @leaf 8006AAEC semantic byte id lookup returns the first matching record, or null */
    LK_FUNC("func_8006AAEC");
    lk_fill(gVmRecordTable, 0xa5, sizeof(gVmRecordTable));
    gVmRecordTable[0].id = 9; gVmRecordTable[1].id = 0xf2;
    gVmRecordTable[2].id = 0xf2; gVmRecordTable[3].id = 7;
    gDescriptorRegistryCount = 3;
    FCK_CHECK(gRegistryFind(9) == &gVmRecordTable[0], "first VM record");
    FCK_CHECK(gRegistryFind(0xf2) == &gVmRecordTable[1], "high byte and first duplicate");
    FCK_CHECK(gRegistryFind(7) == 0, "entry beyond VM count ignored");
    gDescriptorRegistryCount = 0;
    FCK_CHECK(gRegistryFind(9) == 0, "empty VM registry");

    /* @leaf 80089914 semantic binary search of synthetic sorted ids, entry-relative string addresses */
    LK_FUNC("func_80089914");
    gTextLookupTable = 0;
    FCK_CHECK(gLookupTextString(30) == 0, "no text archive");
    lk_fill(&lookup, 0xa5, sizeof(lookup));
    archive.data.count = 3;
    archive.data.entries[0].id = 11; archive.data.entries[0].offset = 12;
    archive.data.entries[1].id = 30; archive.data.entries[1].offset = 16;
    archive.data.entries[2].id = 300; archive.data.entries[2].offset = 20;
    memcpy(archive.data.strings, "uno\0dos\0tri", sizeof(archive.data.strings));
    lookup.header = &archive.header;
    gTextLookupTable = &lookup;
    FCK_CHECK(gLookupTextString(11) == &archive.data.strings[0], "first archive id");
    FCK_CHECK(gLookupTextString(30) == &archive.data.strings[4], "middle archive id");
    FCK_CHECK(gLookupTextString(300) == &archive.data.strings[8], "last archive id");
    FCK_CHECK(gLookupTextString(10) == 0 && gLookupTextString(31) == 0 &&
              gLookupTextString(301) == 0, "below, between and above ids are absent");
}

struct CdlLOC { u8 minute, second, sector, track; };
typedef struct { volatile u32 field0; u8 unk04[0x14]; } Unk800A6D78;
Unk800A6D78 gFileSlotTable[4] __asm__("D_800A6D78");

void gIncrementBcdLoc(u8 *p) __asm__("func_8001C288");
s32 gCdlLocToByteOffset(struct CdlLOC *location) __asm__("func_8001C588");
s32 gCloseFileSlot(u32 index) __asm__("func_8001D3A8");

static void test_disc_records(void)
{
    u8 msf[] = { 0x09, 0x58, 0x09, 0xa5 };
    struct CdlLOC loc = { 0x12, 0x34, 0x56, 0x99 };

    /* @leaf 8001C288 math packed BCD MSF tick, with 75 frames/s and 60 s/min */
    LK_FUNC("func_8001C288");
    gIncrementBcdLoc(msf);
    FCK_CHECK(msf[0] == 9 && msf[1] == 0x58 && msf[2] == 0x10, "BCD digit carry");
    msf[2] = 0x74;
    gIncrementBcdLoc(msf);
    FCK_CHECK(msf[0] == 9 && msf[1] == 0x59 && msf[2] == 0, "sector carries into second");
    msf[2] = 0x74;
    gIncrementBcdLoc(msf);
    FCK_CHECK(msf[0] == 0x10 && msf[1] == 0 && msf[2] == 0, "second carries into minute and BCD tens");
    FCK_EQ(msf[3], 0xa5, "MSF leaves track byte untouched");

    /* @leaf 8001C588 math absolute MSF sector offset times 2048, without LBA lead-in subtraction */
    LK_FUNC("func_8001C588");
    FCK_EQ(gCdlLocToByteOffset(&loc), 115929088, "12:34:56 absolute byte offset");
    loc.minute = 0; loc.second = 2; loc.sector = 0;
    FCK_EQ(gCdlLocToByteOffset(&loc), 307200, "2 seconds is not adjusted by lead-in");
    loc.second = 0; loc.sector = 1;
    FCK_EQ(gCdlLocToByteOffset(&loc), 2048, "one sector");
    loc.sector = 0;
    FCK_EQ(gCdlLocToByteOffset(&loc), 0, "zero MSF with nonzero track byte");

    /* @leaf 8001D3A8 source active states 2/3 block close; other in-range statuses clear the full word */
    LK_FUNC("func_8001D3A8");
    lk_fill(gFileSlotTable, 0xa5, sizeof(gFileSlotTable));
    gFileSlotTable[0].field0 = 0x302;
    gFileSlotTable[1].field0 = 0x403;
    gFileSlotTable[2].field0 = 0;
    gFileSlotTable[3].field0 = 0x504;
    FCK_EQ(gCloseFileSlot(0), -15, "active read state 2");
    FCK_EQ(gCloseFileSlot(1), -15, "active read state 3");
    FCK_CHECK(gFileSlotTable[0].field0 == 0x302 && gFileSlotTable[1].field0 == 0x403, "both active slots retained");
    FCK_EQ(gCloseFileSlot(2), 0xf7, "already closed sentinel");
    FCK_EQ(gCloseFileSlot(3), 4, "last slot returns low status byte");
    FCK_EQ(gFileSlotTable[3].field0, 0, "successful close clears complete status word");
    FCK_CHECK(lk_all(gFileSlotTable[3].unk04, 0, 0x14, 0xa5), "close preserves remaining slot bytes");
    /* Do not call index >= 4: this source still stores through that index. */
}

struct State { u8 pad[0x70]; s32 scale; };
struct State gSoundSystem __asm__("D_800A07A8");
/* The two stream units expose different subsets of the same 0x24-byte record. */
struct StreamChannel {
    s32 id, voice, left, right, start, length;
    u8 looping;
    u8 unk19[0x0b];
};
struct StreamState { u8 unk00[0x24]; struct StreamChannel channels[3]; };
struct StreamState gMixerState __asm__("D_800A0968");
int gMode1IndexBase __asm__("D_800A09D4");
struct Unk4749CResource { u8 unk00[0x10]; s32 serial; };
struct Unk4749CHandle { struct Unk4749CResource *resource; s32 serial; };

void gAdjustSoundBankSampleOffsets(u8 *arg0, s32 amount) __asm__("func_800219EC");
void gComputeStereoVoiceLevels(u8 *a0, s32 a1, s32 a2, s32 *a3, s32 *a4) __asm__("func_800223B0");
void gStoreStreamChannelMix(s32 channel, s32 left, s32 right) __asm__("func_800227BC");
void gSetAudioStreamChannelRange(s32 start, s32 length, s32 channel, u8 looping) __asm__("func_80022B28");
void gMarkAudioStreamAdpcmBlock(u8 *data, int mode, int offset) __asm__("func_80022D1C");
s32 gValidateSoundVoiceHandle(struct Unk4749CHandle *handle) __asm__("func_800474C4");

static void test_audio_records(void)
{
    s32 bank[32];
    u8 *bankBytes = (u8 *)bank;
    u8 gain[] = { 0x5a, 64, 32 };
    u8 adpcm[16], expected[16];
    s32 l, r;
    struct Unk4749CResource voice;
    struct Unk4749CHandle handle;

    /* @leaf 800219EC semantic relocate sample words in present bank groups; skip null offsets and empty groups */
    LK_FUNC("func_800219EC");
    lk_fill(bank, 0xa5, sizeof(bank));
    bankBytes[4] = 3;
    bank[4] = 32; bank[5] = 0; bank[6] = 96;
    bankBytes[32] = 2; bankBytes[96] = 0;
    bank[13] = 0x500; bank[18] = 0x200;
    gAdjustSoundBankSampleOffsets(bankBytes, -0x80);
    FCK_EQ(bank[13], 0x480, "first sample relocated");
    FCK_EQ(bank[18], 0x180, "second sample relocated at next record");
    FCK_CHECK(bank[4] == 32 && bank[5] == 0 && bank[6] == 96, "group offset table stays relative");
    FCK_CHECK(lk_all(bank, 56, 72, 0xa5) && lk_all(bank, 100, sizeof(bank), 0xa5), "sample payload and empty group preserved");

    /* @leaf 800223B0 math balance at center/extremes, left phase inversion, per-side Q7 gain */
    LK_FUNC("func_800223B0");
    lk_fill(&gSoundSystem, 0xa5, sizeof(gSoundSystem));
    gSoundSystem.scale = 3;
    gComputeStereoVoiceLevels(0, 64, 64, &l, &r);
    FCK_CHECK(l == 192 && r == 192, "center has equal gain");
    gComputeStereoVoiceLevels(0, 64, 0, &l, &r);
    FCK_CHECK(l == 192 && r == 0, "hard left mutes right");
    gComputeStereoVoiceLevels(0, 64, 127, &l, &r);
    FCK_CHECK(l == 3 && r == 192, "largest pan leaves one unit on left");
    gComputeStereoVoiceLevels(gain, 64, 0x80 | 96, &l, &r);
    FCK_CHECK(l == -48 && r == 48, "inverted left plus independent gains");

    /* @leaf 800227BC semantic independent channel and gain clamps; no writes to other fields */
    LK_FUNC("func_800227BC");
    lk_fill(&gMixerState, 0xa5, sizeof(gMixerState));
    gStoreStreamChannelMix(-1, -3, 128);
    FCK_CHECK(gMixerState.channels[0].left == 0 && gMixerState.channels[0].right == 127, "clamp to first channel and gain endpoints");
    gStoreStreamChannelMix(3, 127, -1);
    FCK_CHECK(gMixerState.channels[2].left == 127 && gMixerState.channels[2].right == 0, "clamp to last channel and reverse gains");
    FCK_CHECK(lk_all(&gMixerState.channels[1], 0, sizeof(struct StreamChannel), 0xa5), "middle channel not touched");
    gStoreStreamChannelMix(1, 17, 42);
    FCK_CHECK(gMixerState.channels[1].left == 17 && gMixerState.channels[1].right == 42, "in-range mixer gains unchanged");
    FCK_CHECK(lk_all(&gMixerState.channels[1], 0, 8, 0xa5) &&
              lk_all(&gMixerState.channels[1], 16, sizeof(struct StreamChannel), 0xa5), "only gain fields written");

    /* @leaf 80022B28 source stores a stream range and sets, but does not clear, looping */
    LK_FUNC("func_80022B28");
    lk_fill(&gMixerState, 0xa5, sizeof(gMixerState));
    gMixerState.channels[2].looping = 0;
    gSetAudioStreamChannelRange(21, 320, 2, 0);
    FCK_CHECK(gMixerState.channels[2].start == 21 && gMixerState.channels[2].length == 320 &&
              gMixerState.channels[2].looping == 0, "nonlooping range");
    gSetAudioStreamChannelRange(102, 1, 2, 4);
    FCK_CHECK(gMixerState.channels[2].start == 102 && gMixerState.channels[2].length == 1 &&
              gMixerState.channels[2].looping == 1, "looping normalizes nonzero input");
    gSetAudioStreamChannelRange(8, 0, 2, 0);
    FCK_CHECK(gMixerState.channels[2].start == 8 && gMixerState.channels[2].length == 0 &&
              gMixerState.channels[2].looping == 1, "nonloop request leaves prior loop flag");
    FCK_CHECK(lk_all(&gMixerState.channels[1], 0, sizeof(struct StreamChannel), 0xa5) &&
              lk_all(&gMixerState.channels[2], 0, 16, 0xa5), "range write does not change other channel or gains");

    /* @leaf 80022D1C source three marker modes distinguish OR, replace, and which offset is consumed */
    LK_FUNC("func_80022D1C");
    lk_fill(adpcm, 0xa0, sizeof(adpcm)); lk_fill(expected, 0xa0, sizeof(expected));
    gMode1IndexBase = 21;
    gMarkAudioStreamAdpcmBlock(adpcm, 2, 26);
    FCK_CHECK(memcmp(adpcm, expected, sizeof(adpcm)) == 0, "unknown marker mode is a no-op");
    gMarkAudioStreamAdpcmBlock(adpcm, 0, 26);
    expected[1] = 0xa4;
    FCK_CHECK(memcmp(adpcm, expected, sizeof(adpcm)) == 0, "mode 0 ORs header flag");
    gMarkAudioStreamAdpcmBlock(adpcm, 1, 26);
    expected[6] = 0xa1;
    FCK_CHECK(memcmp(adpcm, expected, sizeof(adpcm)) == 0, "mode 1 ORs flag at global index");
    gMarkAudioStreamAdpcmBlock(adpcm, 3, 26);
    expected[11] = 1;
    FCK_CHECK(memcmp(adpcm, expected, sizeof(adpcm)) == 0, "mode 3 replaces byte at passed index");

    /* @leaf 800474C4 semantic null and stale generation handles are rejected; equal serials are valid */
    LK_FUNC("func_800474C4");
    lk_fill(&voice, 0xa5, sizeof(voice));
    voice.serial = 70001; handle.serial = 70001; handle.resource = 0;
    FCK_EQ(gValidateSoundVoiceHandle(&handle), 0, "null resource");
    handle.resource = &voice;
    FCK_EQ(gValidateSoundVoiceHandle(&handle), 1, "matching generation");
    voice.serial += 0x10000;
    FCK_EQ(gValidateSoundVoiceHandle(&handle), 0, "same low half does not validate stale handle");
}

struct SoundChannel {
    s32 id, priority;
    s16 slot;
    s8 index;
    u8 unk0B;
    s32 unk0C, unk10;
    u8 unk14[0x14];
    s32 unk28;
    struct SoundChannel *next, *prev;
};
s32 gChannelCount __asm__("D_8009F720");
struct SoundChannel *gSoundChannelQueueHead __asm__("D_8009F728");
struct SoundChannel *gSoundChannelQueueTail __asm__("D_8009F72C");
struct SoundChannel gVoiceTable[2] __asm__("D_800A1160");
void gQueueSoundChannelByPriority(struct SoundChannel *channel) __asm__("func_800455DC");
void gSetSoundChannelPosition(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 index) __asm__("func_8004572C");
s32 gIsSoundChannelActive(s32 arg0, s32 index) __asm__("func_80045768");

static void test_sound_channels(void)
{
    struct SoundChannel a, b, c, d;

    /* @leaf 800455DC semantic ordered doubly-linked insertion, stable equal priority and zero-priority tail */
    LK_FUNC("func_800455DC");
    lk_fill(&a, 0xa5, sizeof(a)); lk_fill(&b, 0xa5, sizeof(b));
    lk_fill(&c, 0xa5, sizeof(c)); lk_fill(&d, 0xa5, sizeof(d));
    a.priority = 5; b.priority = 0; c.priority = 9; d.priority = 5;
    gChannelCount = 0;
    gSoundChannelQueueHead = gSoundChannelQueueTail = 0;
    gQueueSoundChannelByPriority(&a);
    FCK_CHECK(gSoundChannelQueueHead == &a && gSoundChannelQueueTail == &a &&
              !a.prev && !a.next && gChannelCount == 1, "first sound channel links and count");
    gQueueSoundChannelByPriority(&b);
    FCK_CHECK(a.next == &b && b.prev == &a && !b.next && gSoundChannelQueueTail == &b, "zero priority appends");
    gQueueSoundChannelByPriority(&c);
    FCK_CHECK(gSoundChannelQueueHead == &c && !c.prev && c.next == &a && a.prev == &c, "higher priority prepends");
    gQueueSoundChannelByPriority(&d);
    FCK_CHECK(a.next == &d && d.prev == &a && d.next == &b && b.prev == &d, "equal priority inserts after prior equal");
    FCK_CHECK(gSoundChannelQueueHead == &c && gSoundChannelQueueTail == &b && gChannelCount == 4, "middle insertion preserves endpoints and increments count");

    /* @leaf 8004572C source positional pair changes only selected channel; negative index ignored */
    LK_FUNC("func_8004572C");
    lk_fill(gVoiceTable, 0xa5, sizeof(gVoiceTable));
    gSetSoundChannelPosition(99, 101, -303, -88, -1);
    FCK_CHECK(lk_all(gVoiceTable, 0, sizeof(gVoiceTable), 0xa5), "negative channel does not write");
    gSetSoundChannelPosition(99, 101, -303, -88, 1);
    FCK_CHECK(gVoiceTable[1].unk0C == 101 && gVoiceTable[1].unk10 == -303, "signed position pair, not other arguments");
    FCK_CHECK(lk_all(&gVoiceTable[0], 0, sizeof(struct SoundChannel), 0xa5) &&
              lk_all(&gVoiceTable[1], 0, 12, 0xa5) &&
              lk_all(&gVoiceTable[1], 20, sizeof(struct SoundChannel), 0xa5), "position only writes the selected pair");

    /* @leaf 80045768 semantic active iff a valid channel is assigned and has nonzero priority */
    LK_FUNC("func_80045768");
    gVoiceTable[0].id = 0; gVoiceTable[0].priority = 0;
    gVoiceTable[1].id = -1; gVoiceTable[1].priority = 7;
    FCK_EQ(gIsSoundChannelActive(123, -1), 0, "negative channel inactive");
    FCK_EQ(gIsSoundChannelActive(123, 0), 0, "assigned but no priority");
    FCK_EQ(gIsSoundChannelActive(123, 1), 0, "unassigned despite priority");
    gVoiceTable[1].id = 9;
    FCK_EQ(gIsSoundChannelActive(0, 1), 1, "assigned with nonzero priority");
}

struct PadMapping { u32 masks[14]; };
struct PadMapping gPadMappingTable[2] __asm__("D_800AAC50");
struct PadActuator {
    u8 port, unk01, motor[2], smallIndex, largeIndex, unk06[2];
    s32 smallTimer, largeTimer;
    u8 unk10[6], unk16, enabled;
};
struct PadActuator gPadActuators[2] __asm__("D_800A6FF8");
s32 gFrameTickCounter __asm__("D_8009FA2C");
/* Demo playback increments the D_8009F7C0 address; the separately named
 * D_8009F7C4 in its guard must resolve to the second word of that same array. */
s32 gPadWords[2] __asm__("D_8009F7C0");
__asm__(".globl D_8009F7C4\n.set D_8009F7C4, D_8009F7C0 + 4");
struct Entry { u16 count, value; };
struct ByteEntry { u8 count, value; };
struct Data { s8 count; u8 pad[19]; struct Entry *entries[3]; struct ByteEntry *last; };
struct Data *gStreamDescriptorPtr __asm__("D_8009F7D0");

void gInitPadMapping(u32 port) __asm__("func_8001E588");
void gSetSmallMotor(s32 port, s32 enable) __asm__("func_8001E9AC");
void gSetLargeMotor(s32 port, s32 level) __asm__("func_8001EA5C");
void gTranslatePadButtons(u32 *out, s32 index) __asm__("func_800576A8");
s32 gStepDemoInputPlayback(void) __asm__("func_80059DB8");

static void test_input(void)
{
    /* Source's default action order, expressed as bit numbers. */
    static const u8 bits[] = { 20, 22, 23, 21, 31, 30, 29, 28, 27, 26, 24, 25, 16, 19 };
    struct PadActuator *act = &gPadActuators[1];
    struct PadActuator before;
    u32 translated;
    int i;
    struct Entry first[] = { { 2, 0x1234 }, { 1, 0xabcd }, { 0, 0 } };
    struct Entry second[] = { { 3, 0x4321 }, { 0, 0 } };
    struct ByteEntry footer[] = { { 1, 7 }, { 2, 96 } };
    struct Data demo;

    /* @leaf 8001E588 source default action order populates only the addressed pad; invalid port is ignored */
    LK_FUNC("func_8001E588");
    lk_fill(gPadMappingTable, 0xa5, sizeof(gPadMappingTable));
    gInitPadMapping(2);
    FCK_CHECK(lk_all(gPadMappingTable, 0, sizeof(gPadMappingTable), 0xa5), "port 2 rejected");
    gInitPadMapping(1);
    FCK_CHECK(lk_all(&gPadMappingTable[0], 0, sizeof(struct PadMapping), 0xa5), "mapping port zero untouched");
    for (i = 0; i < sizeof(bits); i++)
        FCK_EQ(gPadMappingTable[1].masks[i], 1u << bits[i], "default action bit");

    /* @leaf 8001E9AC source enabled small motor uses configured actuator slot and a 300-tick deadline */
    LK_FUNC("func_8001E9AC");
    lk_fill(gPadActuators, 0xa5, sizeof(gPadActuators));
    act->enabled = 0; act->smallIndex = 1;
    act->motor[0] = 19; act->motor[1] = 23; act->smallTimer = 29;
    gFrameTickCounter = 9000;
    before = *act;
    gSetSmallMotor(1, 1);
    FCK_CHECK(memcmp(act, &before, sizeof(before)) == 0, "disabled small motor leaves full record unchanged");
    act->enabled = 1; act->smallIndex = 0xff;
    before = *act;
    gSetSmallMotor(1, 1);
    FCK_CHECK(memcmp(act, &before, sizeof(before)) == 0, "unmapped small motor leaves full record unchanged");
    act->smallIndex = 1;
    gSetSmallMotor(1, -4);
    FCK_CHECK(act->motor[1] == 1 && act->smallTimer == 9300, "nonzero enables mapped small motor");
    gSetSmallMotor(1, 0);
    FCK_CHECK(act->motor[0] == 19 && act->motor[1] == 0 && act->smallTimer == 0, "off clears selected motor and timer only");
    FCK_CHECK(lk_all(&gPadActuators[0], 0, sizeof(*act), 0xa5), "small motor leaves other port unchanged");

    /* @leaf 8001EA5C semantic large motor saturates to a byte; zero clears timer and unmapped slot is ignored */
    LK_FUNC("func_8001EA5C");
    lk_fill(act, 0xa5, sizeof(*act));
    act->enabled = 1; act->largeIndex = 0xff;
    gFrameTickCounter = 45;
    before = *act;
    gSetLargeMotor(1, 256);
    FCK_CHECK(memcmp(act, &before, sizeof(before)) == 0, "unmapped large motor leaves full record unchanged");
    act->largeIndex = 0; act->enabled = 0;
    before = *act;
    gSetLargeMotor(1, 255);
    FCK_CHECK(memcmp(act, &before, sizeof(before)) == 0, "disabled large motor ignores in-range level");
    act->enabled = 1;
    gSetLargeMotor(1, 256);
    FCK_CHECK(act->motor[0] == 255 && act->largeTimer == 345, "level above byte saturates");
    gFrameTickCounter = 80;
    gSetLargeMotor(1, 1);
    FCK_CHECK(act->motor[0] == 1 && act->largeTimer == 380, "lowest positive level refreshes timer");
    gSetLargeMotor(1, -1);
    FCK_CHECK(act->motor[0] == 0 && act->motor[1] == 0xa5 && act->largeTimer == 0, "negative stops only selected motor");

    /* @leaf 800576A8 source remap selected pad bits and merge only allowed secondary bits in solo play */
    LK_FUNC("func_800576A8");
    gPlayerCount = 2;
    gPadWords[0] = 0x89480055; gPadWords[1] = 0x06400000;
    gTranslatePadButtons(&translated, 0);
    FCK_EQ(translated, 0x8e080000, "mixed preserved, remapped and dropped input bits");
    gTranslatePadButtons(&translated, 1);
    FCK_EQ(translated, 0x03400000, "split-screen selects second pad");
    gPlayerCount = 1;
    gPadWords[0] = 0; gPadWords[1] = 0x8e080000;
    gTranslatePadButtons(&translated, 0);
    FCK_EQ(translated, 0x05400000, "solo merges three secondary actions, not other bits");
    gPlayerCount = 2;
    gTranslatePadButtons(&translated, 0);
    FCK_EQ(translated, 0, "split-screen does not merge second pad");

    /* @leaf 80059DB8 semantic run-length demo playback advances at run boundaries, aborts on live input or end */
    LK_FUNC("func_80059DB8");
    lk_fill(&demo, 0, sizeof(demo));
    demo.count = 2; demo.entries[0] = first; demo.entries[1] = second; demo.last = footer;
    gStreamDescriptorPtr = &demo;
    gPadWords[0] = 0; gPadWords[1] = 0x20;
    FCK_EQ(gStepDemoInputPlayback(), -1, "second live pad aborts playback");
    FCK_CHECK(first[0].count == 2 && second[0].count == 3 && footer[0].count == 1, "live pad abort consumes no runs");
    gPadWords[1] = 0;
    FCK_EQ(gStepDemoInputPlayback(), 7, "first demo footer run");
    FCK_CHECK(gPadWords[0] == 0x12340000 && gPadWords[1] == 0x43210000, "two demo pad outputs occupy adjacent words");
    FCK_EQ(gStepDemoInputPlayback(), -1, "pending pad outputs prevent another tick");
    gPadWords[0] = gPadWords[1] = 0;
    FCK_EQ(gStepDemoInputPlayback(), 96, "footer advances immediately after first run");
    FCK_CHECK(demo.entries[0] == first && first[0].count == 0 && footer[1].count == 1, "first pad run consumed before moving");
    gPadWords[0] = gPadWords[1] = 0;
    FCK_EQ(gStepDemoInputPlayback(), 96, "third tick uses second footer value");
    FCK_CHECK(demo.entries[0] == &first[1] && (u32)gPadWords[0] == 0xabcd0000 &&
              second[0].count == 0 && footer[1].count == 0, "advance first pad run and exhaust second");
    gPadWords[0] = gPadWords[1] = 0;
    FCK_EQ(gStepDemoInputPlayback(), -1, "zero-count pad sentinel ends demo before footer");
    FCK_CHECK(gPadWords[0] == 0 && gPadWords[1] == 0 && demo.last == &footer[1], "end leaves outputs and footer unchanged");
}

struct AnimClip { u8 unk00[4]; s16 start, end; u8 unk08[8]; };
struct AnimTrack {
    struct AnimClip *clips, *clip;
    s32 elapsed;
    s16 frame, target;
    s8 step;
    u8 index, count, flags;
};
struct Obj {
    s32 next, base[3], pos[3], out[3], velocity[3];
    s16 offset[3];
    u16 time, limit, flags;
};
struct Entity { struct Entity *next; void *update; s32 id; };
struct EntityMessage { u8 kind, value; s16 timer; struct Entity *target; s32 id; };
struct EntityMessage gPendingMessages[16] __asm__("D_800A2090");
s32 gPendingMessageMask __asm__("D_8009F7A8");
struct Unk4F0F0Entry { s32 unk00; u8 flags, unk05[7]; };
struct Unk4F0F0Entry *gPathNetTables[3] __asm__("D_800A2210");
s32 gPlacementSlotCount __asm__("D_8009F7B8");

typedef struct { short x, y, z, sx, sy, sz; } Box;
typedef struct { int x, y, z; } Vec;
struct AnimClip *gSelectAnimationClip(struct AnimTrack *track, s32 index, s32 direction, u8 flags) __asm__("func_800448D8");
s32 gStepTracerNode(struct Obj *obj, s32 delta) __asm__("func_8006EE14");
u8 gStepParticleColorChannels(u8 *base, s32 d1, s32 d2, s32 d3, s32 a5, u8 a6, u8 a7) __asm__("func_800751A4");
int gFindFirstOverlappingModelBox(Box *input, Vec *pos, int radius, int count, int input_stride) __asm__("func_800618CC");
void gRemoveEntityCommand(u8 *script, s32 index) __asm__("func_8002A5B4");
s32 gEnqueuePendingMessage(struct Entity *target, u8 kind, u8 value) __asm__("func_8004D9EC");
void gSetPathNodeBlocked(s32 index, u32 mode, s32 entry) __asm__("func_8004F0F0");

static void test_animation_and_effects(void)
{
    struct AnimClip clips[2];
    struct AnimTrack track, oldTrack;
    struct Obj tracer;
    u8 color[0x19];

    /* @leaf 800448D8 source selection stores clip and direction; direction 2 retains elapsed time */
    LK_FUNC("func_800448D8");
    lk_fill(clips, 0xa5, sizeof(clips));
    lk_fill(&track, 0xa5, sizeof(track));
    track.clips = 0; track.clip = &clips[1];
    oldTrack = track;
    FCK_CHECK(gSelectAnimationClip(&track, 0, 1, 1) == 0 &&
              memcmp(&track, &oldTrack, sizeof(track)) == 0, "missing clip table leaves track unchanged");
    clips[0].start = 3; clips[0].end = 15;
    clips[1].start = 50; clips[1].end = 20;
    track.clips = clips; track.count = 2; track.elapsed = 96; track.flags = 0x80;
    FCK_CHECK(gSelectAnimationClip(&track, 0, 1, 4) == &clips[0], "forward clip pointer");
    FCK_CHECK(track.frame == 3 && track.target == 15 && track.step == 1 && track.elapsed == 0, "forward frame state");
    FCK_CHECK(track.clip == &clips[0] && track.index == 0 && track.flags == 0x84 && track.count == 2, "track stores selection and merges flags");
    FCK_CHECK(gSelectAnimationClip(&track, 0, -1, 0) == &clips[0] &&
              track.frame == 3 && track.target == 3 && track.step == -1, "reverse direction targets start");
    track.elapsed = 700;
    FCK_CHECK(gSelectAnimationClip(&track, 1, 2, 2) == &clips[1] &&
              track.clip == &clips[1] && track.index == 1, "direction 2 reselects second clip");
    FCK_CHECK(track.frame == 50 && track.target == 20 && track.step == -1 &&
              track.elapsed == 700 && track.flags == 0x86, "descending clip inverts step without resetting time");

    /* @leaf 8006EE14 semantic tracer integrates signed velocity and offset, skips one advance and expires at equality */
    LK_FUNC("func_8006EE14");
    lk_fill(&tracer, 0xa5, sizeof(tracer));
    tracer.base[0] = 100; tracer.base[1] = -200; tracer.base[2] = 300;
    tracer.velocity[0] = 32; tracer.velocity[1] = -48; tracer.velocity[2] = 7;
    tracer.offset[0] = 2; tracer.offset[1] = -3; tracer.offset[2] = 1;
    tracer.time = 3; tracer.limit = 8; tracer.flags = 0x8040;
    FCK_EQ(gStepTracerNode(&tracer, 4), 1, "tracer alive on first tick");
    FCK_CHECK(tracer.time == 3 && tracer.flags == 0x40, "skip flag consumes itself without advancing time");
    FCK_CHECK(tracer.pos[0] == 106 && tracer.pos[1] == -209 && tracer.pos[2] == 301 &&
              tracer.out[0] == 138 && tracer.out[1] == -257 && tracer.out[2] == 317, "initial tracer position and render offset");
    FCK_EQ(gStepTracerNode(&tracer, 4), 1, "tracer remains alive at limit minus one");
    FCK_CHECK(tracer.time == 7 && tracer.pos[0] == 114 && tracer.pos[1] == -221 &&
              tracer.pos[2] == 303 && tracer.out[2] == 319, "advance uses base rather than accumulating prior position");
    FCK_EQ(gStepTracerNode(&tracer, 1), 0, "tracer expires exactly at limit");
    FCK_CHECK(tracer.time == 8 && tracer.pos[0] == 114 && tracer.out[2] == 319, "expiry does not extrapolate fields");

    /* @leaf 800751A4 semantic counts channels still moving and saturates positive/negative steps at target */
    LK_FUNC("func_800751A4");
    lk_fill(color, 0xa5, sizeof(color));
    color[0x15] = 10; color[0x16] = 40; color[0x17] = 7;
    FCK_EQ(gStepParticleColorChannels(color, 3, -8, 0, 20, 20, 99), 2, "two color channels still moving");
    FCK_CHECK(color[0x15] == 13 && color[0x16] == 32 && color[0x17] == 7, "positive, negative and stationary color update");
    FCK_EQ(gStepParticleColorChannels(color, 7, -30, 5, 20, 20, 10), 0, "exact target and both overshoots finish");
    FCK_CHECK(color[0x15] == 20 && color[0x16] == 20 && color[0x17] == 10, "clamp each channel separately");
    FCK_EQ(gStepParticleColorChannels(color, -2, 3, -3, 11, 40, 9), 2, "reverse directions with one saturated channel");
    FCK_CHECK(color[0x15] == 18 && color[0x16] == 23 && color[0x17] == 9 &&
              lk_all(color, 0, 0x15, 0xa5) && color[0x18] == 0xa5, "only color bytes touched");
}

static void test_spatial_and_script(void)
{
    struct BoxSlot { Box box; int padding; } boxes[] = {
        { { 10, -4, 2, 4, 4, 4 }, 0x12345678 },
        { { 10, -4, 2, 2, 1, 2 }, 0x12345678 },
        { { 10, -4, 2, 3, 2, 3 }, 0x12345678 }
    };
    Vec pos = { 160, -64, 32 };
    static const u8 original[] = { 1, 0x81, 2, 0x82, 3, 0x83, 4, 0x84 };
    static const u8 noFirst[] = { 2, 0x82, 3, 0x83, 4, 0x84, 0, 0x84 };
    static const u8 noMiddle[] = { 1, 0x81, 2, 0x82, 4, 0x84, 0, 0x84 };
    static const u8 noLast[] = { 1, 0x81, 2, 0x82, 3, 0x83, 0, 0x84 };
    u8 script[0x18];
    struct Entity target = { 0, 0, -739 };
    struct Unk4F0F0Entry nodes[3];

    /* @leaf 800618CC source strided search skips aggregate box in multi-box models; expanded edges are strict */
    LK_FUNC("func_800618CC");
    FCK_EQ(gFindFirstOverlappingModelBox(&boxes[0].box, &pos, 0, 0, sizeof(boxes[0])), -1, "no boxes");
    FCK_EQ(gFindFirstOverlappingModelBox(&boxes[0].box, &pos, 0, 1, sizeof(boxes[0])), 0, "sole box uses index zero");
    FCK_EQ(gFindFirstOverlappingModelBox(&boxes[0].box, &pos, 0, 3, sizeof(boxes[0])), 1, "both parts overlap; first part wins");
    pos.y = -48;
    FCK_EQ(gFindFirstOverlappingModelBox(&boxes[0].box, &pos, 0, 3, sizeof(boxes[0])), 2, "first part on strict y edge; second fits");
    FCK_EQ(gFindFirstOverlappingModelBox(&boxes[0].box, &pos, 1, 3, sizeof(boxes[0])), 1, "one unit of radius admits edge");
    pos.y = -64; pos.z = -16;
    FCK_EQ(gFindFirstOverlappingModelBox(&boxes[0].box, &pos, 0, 3, sizeof(boxes[0])), -1, "z edge excludes both parts, even inside aggregate");

    /* @leaf 8002A5B4 source removes first, middle or final command, invalidating trailing opcode only */
    LK_FUNC("func_8002A5B4");
    lk_fill(script, 0xa5, sizeof(script));
    memcpy(script + 0x0c, original, sizeof(original));
    gRemoveEntityCommand(script, 0);
    FCK_CHECK(memcmp(script + 0x0c, noFirst, sizeof(noFirst)) == 0, "remove first shifts all later pairs");
    memcpy(script + 0x0c, original, sizeof(original));
    gRemoveEntityCommand(script, 2);
    FCK_CHECK(memcmp(script + 0x0c, noMiddle, sizeof(noMiddle)) == 0, "remove third shifts just last pair");
    memcpy(script + 0x0c, original, sizeof(original));
    gRemoveEntityCommand(script, 3);
    FCK_CHECK(memcmp(script + 0x0c, noLast, sizeof(noLast)) == 0, "remove last keeps trailing argument");
    FCK_CHECK(lk_all(script, 0, 0x0c, 0xa5) && lk_all(script, 0x14, sizeof(script), 0xa5), "queue extent respected");

    /* @leaf 8004D9EC semantic enqueue uses the first free slot including bit 15, records payload and rejects full queue */
    LK_FUNC("func_8004D9EC");
    lk_fill(gPendingMessages, 0xa5, sizeof(gPendingMessages));
    gPendingMessageMask = 0x000b;
    FCK_EQ(gEnqueuePendingMessage(&target, 0x92, 0x7f), 1, "hole in pending mask is usable");
    FCK_EQ(gPendingMessageMask, 0x000f, "first free bit marked");
    FCK_CHECK(gPendingMessages[2].kind == 0x92 && gPendingMessages[2].value == 0x7f &&
              gPendingMessages[2].target == &target && gPendingMessages[2].id == -739 &&
              gPendingMessages[2].timer == 600, "message snapshots target id, payload and timeout");
    FCK_CHECK(lk_all(gPendingMessages, 0, 2 * sizeof(struct EntityMessage), 0xa5) &&
              lk_all(&gPendingMessages[3], 0, sizeof(struct EntityMessage), 0xa5), "occupied slots were not overwritten");
    gPendingMessageMask = 0x7fff;
    FCK_EQ(gEnqueuePendingMessage(&target, 1, 2), 1, "last queue slot usable");
    FCK_CHECK(gPendingMessageMask == 0xffff && gPendingMessages[15].kind == 1, "full mask and final slot");
    FCK_EQ(gEnqueuePendingMessage(0, 99, 99), 0, "full queue rejects before dereferencing target");
    FCK_CHECK(gPendingMessageMask == 0xffff && gPendingMessages[15].kind == 1 &&
              gPendingMessages[2].kind == 0x92, "full queue unchanged");

    /* @leaf 8004F0F0 semantic toggle one blocked bit; unknown modes and invalid table indices are ignored */
    LK_FUNC("func_8004F0F0");
    lk_fill(nodes, 0xa5, sizeof(nodes));
    gPlacementSlotCount = 2;
    gPathNetTables[0] = &nodes[2]; gPathNetTables[1] = nodes; gPathNetTables[2] = &nodes[2];
    gSetPathNodeBlocked(1, 1, 1);
    FCK_EQ(nodes[1].flags, 0xa7, "set blocked bit preserves others");
    gSetPathNodeBlocked(1, 2, 1);
    FCK_EQ(nodes[1].flags, 0xa7, "unknown byte mode leaves flags unchanged");
    gSetPathNodeBlocked(1, 0x100, 1);
    FCK_EQ(nodes[1].flags, 0xa7, "large mode is not truncated to clear");
    gSetPathNodeBlocked(2, 1, 0);
    gSetPathNodeBlocked(-1, 1, 0);
    FCK_CHECK(lk_all(&nodes[2], 0, sizeof(nodes[2]), 0xa5), "indices outside active count do not write");
    gSetPathNodeBlocked(1, 0, 1);
    FCK_CHECK(lk_all(nodes, 0, sizeof(nodes), 0xa5), "clear blocked bit restores all fixture bytes");
}

struct Unk31Node {
    s16 x, unk02, z, unk06;
    s32 id;
    s16 extentX, unk0E, extentZ, unk12;
    u8 flags, unk15;
    s16 unk16;
    u8 unk18[0x0c];
    struct Unk31Node *next;
};
struct Vec3s32 { s32 x, y, z; };
struct Unk31Node *gMapZoneList __asm__("D_8009F6F8");
s32 gTestCameraZoneOverlap(struct Vec3s32 *position, s32 radius) __asm__("func_80031B40");
void gSetMapZoneState(s32 id, s32 kind, s32 mode) __asm__("func_80031BEC");

static void test_zones(void)
{
    struct Unk31Node zones[3];
    struct Vec3s32 pos = { 1298, 0x1234, -538 };

    /* @leaf 80031B40 semantic overlap needs camera flag and two inclusive axes, with radius extending the zone */
    LK_FUNC("func_80031B40");
    lk_fill(zones, 0xa5, sizeof(zones));
    gMapZoneList = 0;
    FCK_EQ(gTestCameraZoneOverlap(&pos, 2), 0, "empty camera zone list");
    gMapZoneList = zones;
    zones[0].flags = 0x10; zones[0].x = 5; zones[0].z = -2;
    zones[0].extentX = 20; zones[0].extentZ = 20;
    zones[0].next = &zones[1];
    zones[1].flags = 0x40; zones[1].x = 5; zones[1].z = -2;
    zones[1].extentX = 1; zones[1].extentZ = 1; zones[1].next = 0;
    FCK_EQ(gTestCameraZoneOverlap(&pos, 2), 0, "unflagged containing zone ignored; eligible zone misses z");
    pos.z = -530;
    FCK_EQ(gTestCameraZoneOverlap(&pos, 1), 0, "one unit outside expanded bounds");
    FCK_EQ(gTestCameraZoneOverlap(&pos, 2), 1, "on both expanded edges, second list node");
    pos.x = 1261;
    FCK_EQ(gTestCameraZoneOverlap(&pos, 2), 0, "negative x side misses with valid z");

    /* @leaf 80031BEC source searches by id, updates first matching node for kind 0x7c with three mode outcomes */
    LK_FUNC("func_80031BEC");
    zones[0].id = -1; zones[1].id = zones[2].id = 23;
    zones[1].next = &zones[2]; zones[2].next = 0; zones[2].flags = 0x40;
    zones[1].unk15 = 0; zones[1].unk16 = 127;
    gSetMapZoneState(99, 0x7c, 0);
    gSetMapZoneState(23, 0x7b, 0);
    FCK_CHECK(zones[1].flags == 0x40 && zones[1].unk15 == 0 && zones[1].unk16 == 127, "missing id or other kind is ignored");
    gSetMapZoneState(23, 0x7c, 0);
    FCK_CHECK(zones[0].flags == 0x10 && zones[1].flags == 0x50 && zones[2].flags == 0x40, "mode zero sets only first matching node");
    gSetMapZoneState(23, 0x7c, 2);
    FCK_CHECK(zones[1].flags == 0x50 && zones[1].unk15 == 1 && zones[1].unk16 == 127, "other mode raises separate status byte");
    gSetMapZoneState(23, 0x7c, 1);
    FCK_CHECK(zones[1].flags == 0x40 && zones[1].unk15 == 1 && zones[1].unk16 == 0, "mode one clears bit and halfword, not status byte");
}

typedef struct { s32 unk0, unk4; u8 unk8, pad[3]; s32 unkC, unk10; } Unk800A0660;
s32 gUploadRingWriteIdx __asm__("D_8009F5E8");
Unk800A0660 gUploadRing[4] __asm__("D_800A0660");
struct Rect { s16 x, y, w, h; };
struct Packet { u8 pad[3]; u8 len; u32 cmd, mode, xy, pos, wh; };
s16 gMaxDrawWidth __asm__("D_8009DDF4");
s16 gMaxDrawHeight __asm__("D_8009DDF6");
void gQueueVramUpload(s32 arg0, s32 arg1, s32 arg2, s32 arg3, u8 arg4) __asm__("func_8001BBEC");
void gSetDrawMovePacket(struct Packet *p, struct Rect *r, int x, int y) __asm__("func_80092660");
s32 gPackDrawAreaBottomRight(s32 x, s32 y) __asm__("func_80094010");

static void test_graphics_records(void)
{
    struct Packet packets[2];
    struct Rect rect = { -10, 8, 17, 29 };

    /* @leaf 8001BBEC semantic writes one RAM upload descriptor, including last slot and wrap */
    LK_FUNC("func_8001BBEC");
    lk_fill(gUploadRing, 0xa5, sizeof(gUploadRing));
    gUploadRingWriteIdx = 3;
    gQueueVramUpload(10, -20, 30, 40, 241);
    FCK_EQ(gUploadRingWriteIdx, 0, "upload ring wraps at four");
    FCK_CHECK(gUploadRing[3].unk0 == -20 && gUploadRing[3].unk4 == 10 &&
              gUploadRing[3].unk8 == 241 && gUploadRing[3].unkC == 30 &&
              gUploadRing[3].unk10 == 40, "last descriptor field order and widths");
    FCK_CHECK(lk_all(gUploadRing, 0, 3 * sizeof(Unk800A0660), 0xa5) &&
              lk_all(gUploadRing[3].pad, 0, 3, 0xa5), "only used fields of one record written");
    gQueueVramUpload(1, 2, 3, 4, 5);
    FCK_CHECK(gUploadRingWriteIdx == 1 && gUploadRing[0].unk8 == 5 &&
              gUploadRing[3].unk8 == 241, "next enqueue uses slot zero without reusing the last");

    /* @leaf 80092660 source RAM move packet layout, signed coordinate packing and zero-size suppresses length */
    LK_FUNC("func_80092660");
    lk_fill(packets, 0xa5, sizeof(packets));
    gSetDrawMovePacket(packets, &rect, -6, 17);
    FCK_EQ(packets[0].len, 5, "move has five command words");
    FCK_CHECK(packets[0].cmd == 0x01000000 && packets[0].mode == 0x80000000, "move command words");
    FCK_EQ(packets[0].xy, 0x0008fff6, "pack source coordinates");
    FCK_EQ(packets[0].pos, 0x0011fffa, "pack destination coordinates");
    FCK_EQ(packets[0].wh, 0x001d0011, "pack size");
    rect.w = 0;
    gSetDrawMovePacket(packets, &rect, 0, 0);
    FCK_EQ(packets[0].len, 0, "zero width suppresses packet");
    rect.w = 1; rect.h = 0;
    gSetDrawMovePacket(packets, &rect, 0, 0);
    FCK_EQ(packets[0].len, 0, "zero height independently suppresses packet");
    FCK_CHECK(lk_all(packets[0].pad, 0, 3, 0xa5) &&
              lk_all(&packets[1], 0, sizeof(struct Packet), 0xa5), "link bytes and following packet preserved");

    /* @leaf 80094010 source E4 payload clamps bottom-right coordinates to independent RAM dimensions */
    LK_FUNC("func_80094010");
    gMaxDrawWidth = 320; gMaxDrawHeight = 200;
    FCK_EQ(gPackDrawAreaBottomRight(7, 13), 0xe4003407, "interior y and x fields");
    FCK_EQ(gPackDrawAreaBottomRight(319, 199), 0xe4031d3f, "maximum valid x and y");
    FCK_EQ(gPackDrawAreaBottomRight(-1, 200), 0xe4031c00, "clamp negative x and oversized y");
    FCK_EQ(gPackDrawAreaBottomRight(320, -1), 0xe400013f, "clamp oversized x and negative y");
    gMaxDrawWidth = 1024; gMaxDrawHeight = 512;
    FCK_EQ(gPackDrawAreaBottomRight(1024, 512), 0xe407ffff, "new dimensions use full x and nine y bits");
}

struct State821EC {
    u8 pad0[0x200], mode, kind, pad202[5], flag, pad208[0x73a];
    u16 value;
};
struct State821EC *gSaveProfileImage __asm__("D_8009F92C");
u8 gUiHoldFlag __asm__("D_8009F8BD");
u8 gUiInteractionEnabled __asm__("D_8009F8BE");
void gUpdateMemoryCardPromptOnExit(void) __asm__("func_800821EC");

static void test_prompt(void)
{
    struct State821EC profile;

    /* @leaf 800821EC source mode 3 enables interaction iff flag is clear; mode 5 preserves kinds 2 and 3 */
    LK_FUNC("func_800821EC");
    lk_fill(&profile, 0xa5, sizeof(profile));
    gSaveProfileImage = &profile;
    gUiHoldFlag = 9; gUiInteractionEnabled = 9;
    profile.mode = 1;
    gUpdateMemoryCardPromptOnExit();
    FCK_CHECK(profile.mode == 1 && profile.value == 0xa5a5 && gUiHoldFlag == 9 &&
              gUiInteractionEnabled == 9, "unrelated prompt mode no-op");
    profile.mode = 3; profile.flag = 0;
    gUpdateMemoryCardPromptOnExit();
    FCK_CHECK(profile.mode == 4 && gUiHoldFlag == 0 && gUiInteractionEnabled == 1, "exit enters mode four and enables interaction");
    profile.mode = 3; profile.flag = 128; gUiHoldFlag = 9;
    gUpdateMemoryCardPromptOnExit();
    FCK_CHECK(profile.mode == 4 && gUiHoldFlag == 0 && gUiInteractionEnabled == 0, "any nonzero flag disables interaction");
    profile.mode = 5; profile.kind = 2;
    gUpdateMemoryCardPromptOnExit();
    FCK_CHECK(profile.mode == 2 && profile.value == 0xa5a5, "kind two preserves value");
    profile.mode = 5; profile.kind = 3;
    gUpdateMemoryCardPromptOnExit();
    FCK_CHECK(profile.mode == 2 && profile.value == 0xa5a5, "kind three preserves value");
    profile.mode = 5; profile.kind = 4;
    gUpdateMemoryCardPromptOnExit();
    FCK_CHECK(profile.mode == 2 && profile.value == 0x385e, "kind above preservation range resets value");
    profile.mode = 5; profile.kind = 1; profile.value = 0xa5a5;
    gUpdateMemoryCardPromptOnExit();
    FCK_CHECK(profile.mode == 2 && profile.value == 0x385e && gUiInteractionEnabled == 0, "kind below range also resets value, not UI");
}

int main(void)
{
    test_vectors();
    test_frusta();
    test_map();
    test_heap();
    test_registries();
    test_disc_records();
    test_audio_records();
    test_sound_channels();
    test_input();
    test_animation_and_effects();
    test_spatial_and_script();
    test_zones();
    test_graphics_records();
    test_prompt();
    return lk_done("leaf_more");
}
