/* Leaf-function suite A1/A2: hardware-free single-object functions.
 *
 * Every unit listed in an "@leaf" marker below is the *unchanged* decomp C in
 * src/game/, linked into one binary and run under qemu-mipsel. The marker
 * line is the single source of truth for tools/host_leaf_suite.py: it
 * regenerates the unit list of leaf_a1_test.sh from it and records the
 * evidence class of each function in the report.
 *
 * Evidence classes (what the expected values are based on):
 *   spec      external documentation that exists without this repo:
 *             PSn00bSDK psxgpu.h primitive macros, Psy-Q libgpu/libcd macro
 *             semantics, psx-spx GPU/controller command tables
 *   math      a mathematical identity, computed independently (leaf_vectors.py)
 *   libc      the documented semantics of the C library routine it mirrors
 *   semantic  hand-computed from the function's documented purpose (packing
 *             RGB, a free list, MSF comparison ...) with literal expectations
 *   source    the only reference is the decompiled source itself; expectations
 *             are still literal and hand-computed, but they pin behaviour,
 *             they do not prove the semantics independently
 *
 * No expected value is computed by calling a re-implementation of the game
 * function in this file. Values are literals or come from leaf_vectors.h.
 */
#include "leaf_kit.h"
#include "leaf_vectors.h"

/* Packed screen point as the game passes it: x in the low s16, y in the high. */
#define PT(x, y) ((s32)(((u32)(y) << 16) | ((u32)(x) & 0xFFFFu)))

/* ---------------------------------------------------------------- prototypes
 * Copied argument-for-argument from the unit's own __asm__ declaration. */
s32 gEmitFrontFacingTriangle(s32 origin, s32 first, s32 second, s32 *out) __asm__("func_80015AF4");
s32 gIsTriangleFrontFacing(s32 origin, s32 first, s32 second) __asm__("func_80049EF8");
int gReadHeapBlockSize(int *value) __asm__("func_80018460");
s32 gGetBlockSize(s32 *ptr) __asm__("func_8001E304");
u32 gSqrtU32Q8(u32 value) __asm__("func_800187C8");
void *gTakeFixedPoolSlot(void *state) __asm__("func_800195A4");
void gReturnFixedPoolSlot(void *state, int *link) __asm__("func_800195D0");
void gEaseVectorQuarter(s32 *src, s32 *dst) __asm__("func_8001A6F8");
s32 gApproximateDistanceXzHalfMin(s32 *a, s32 *b) __asm__("func_8001A750");
u32 gPackRgb24(u32 r, u32 g, u32 b) __asm__("func_8001BC44");
void gScaleRgb24(u32 color, s32 scale, s32 *out) __asm__("func_8001BC64");
void gSplitRgb24Shifted(unsigned int color, unsigned int *dst) __asm__("func_8001BCA4");
void gAccumScaledRgb24(u32 color, s32 scale, s32 *out) __asm__("func_8001BCD0");
void gAccumRgb24Shifted(unsigned int color, unsigned int *dst) __asm__("func_8001BD2C");
void gCopyWord(int *destination, int *source) __asm__("func_8001C27C");
s32 gCompareCdlLoc(u8 *a, u8 *b) __asm__("func_8001C3F8");
void gClearWord(int *value) __asm__("func_8001C470");
void *gFillMemory(void *destination, int value, unsigned int count) __asm__("func_8001E4C0");
void *gResolveSoundBankEntry(void *base, u32 index) __asm__("func_80021800");
void gCopyWholeWords(u32 *destination, const u32 *source, u32 byte_count) __asm__("func_80028BA0");
void gSyncCountdownFields(u8 *arg0) __asm__("func_8002B1AC");
s32 gQuarterIndex(s32 value, s32 range) __asm__("func_8003B65C");
void *gTakePoolNode(void *node) __asm__("func_8003BEF0");
unsigned char *gLookupFontGlyph(void *state, unsigned int offset) __asm__("func_800489C8");
int gIsQueuedCommandKind(unsigned int value) __asm__("func_8004DA58");
void gScaleDefinitionSubrecordFields(void *value) __asm__("func_800607B8");
void gDecodeGlyphsToName(u8 *dst, u8 *src, s32 len, u8 *table) __asm__("func_8007AA9C");
u8 gFindListEntryByValue(void *list, u8 value) __asm__("func_8007D39C");
void gComputeDualChecksum(u8 *data, u32 len, u32 *sum, u32 *other) __asm__("func_8007F688");
s32 gMinS32(s32 a, s32 b) __asm__("func_800886C8");
void gBindActuatorBuffer(void *state, int value, u8 type) __asm__("func_8008AE70");
void gSetPadCommandState(unsigned char *state, int a, int b, int c) __asm__("func_8008AE7C");
void gCmdPadActAlign(void *state) __asm__("func_8008B4EC");
void gSetPadConfigModeCmd(void *state, u8 value) __asm__("func_8008B71C");
void gSetPadModelQueryCmd(void *state) __asm__("func_8008B73C");
void gSetPadModeConstCmd(void *state, u8 value) __asm__("func_8008B750");
void gSetPadActInfoCmd(void *state, u8 value) __asm__("func_8008B770");
void gSetPadCombInfoCmd(void *state, u8 value) __asm__("func_8008B790");
void gSetPadCombContCmd(void *state) __asm__("func_8008B7B0");
int gConsumePadCmd(void *state) __asm__("func_8008C06C");
s32 gCheckPadReady(void *state) __asm__("func_8008C858");
u8 *gCopyMemOverlap(u8 *dest, u8 *src, s32 count) __asm__("func_8008D5D0");
s32 gCdPosToInt(void *location) __asm__("func_8008E720");
void gZeroWords(int *destination, unsigned int count) __asm__("func_80091B1C");
void gClearIntegerRun(int *destination, unsigned int count) __asm__("func_80091C90");
u16 gGetTPage(s32 type, s32 blend, s32 x, s32 y) __asm__("func_80092230");
u16 gGetClutId(s32 x, s32 y) __asm__("func_8009226C");
int gIsPrimListEnd(unsigned int *value) __asm__("func_80092340");
void gAddPrims(unsigned int *a, unsigned int b, unsigned int *c) __asm__("func_80092398");
void gSetPrimNextLink(unsigned int *p, unsigned int value) __asm__("func_800923D4");
void gSetPrimListEnd(unsigned int *value) __asm__("func_800923F8");
void gSetPrimSemiTrans(unsigned char *obj, int enabled) __asm__("func_80092410");
void gSetPrimRawTexture(unsigned char *p, int enabled) __asm__("func_80092438");
void gSetPolyG3(u8 *p) __asm__("func_80092488");
void gSetPolyGT3(u8 *p) __asm__("func_8009249C");
void gSetPolyF4(u8 *p) __asm__("func_800924B0");
void gSetPolyFT4(u8 *p) __asm__("func_800924C4");
void gSetPolyG4(u8 *p) __asm__("func_800924D8");
void gSetPolyGT4(u8 *p) __asm__("func_800924EC");
void gSetSprt16(u8 *p) __asm__("func_80092514");
void gSetSprt(u8 *p) __asm__("func_80092528");
void gSetTile1(u8 *p) __asm__("func_8009253C");
void gSetTile8(u8 *p) __asm__("func_80092550");
void gSetTile16(u8 *p) __asm__("func_80092564");
void gSetTile(u8 *p) __asm__("func_80092578");
void gSetLineF2(u8 *p) __asm__("func_8009258C");
void gSetLineG2(u8 *p) __asm__("func_800925A0");
void gSetLineG3(void *p) __asm__("func_800925D4");
void gSetLineF4(void *p) __asm__("func_800925F4");
void gSetDrawMaskPacket(void *packet, int flag1, int flag2) __asm__("func_80093A4C");
void gSetMaskBitPacket(void *packet, int enabled) __asm__("func_80093A74");
u32 gBuildDrawModeCommand(s32 dfe, s32 dtd, u32 tpage) __asm__("func_80093F58");
unsigned int gPackDrawOffset(unsigned int x, unsigned int y) __asm__("func_800940A8");
s32 gBuildTexWindowCommand(u8 *p) __asm__("func_800940C4");
void gMemset(unsigned char *destination, int value, unsigned int count) __asm__("func_80095830");
u16 gMdecReadHalfword(u16 *value) __asm__("func_80095FE8");

/* Shared scratch: word-aligned so struct-shaped views are legal on MIPS. */
static union { u32 w[64]; u16 h[128]; u8 b[256]; s16 s[128]; } buf, buf2;

#define W32(base, off) (*(u32 *)((u8 *)(base) + (off)))
#define H16(base, off) (*(u16 *)((u8 *)(base) + (off)))
#define S16(base, off) (*(s16 *)((u8 *)(base) + (off)))

/* ================================================================ geometry */

static void t_triangles(void)
{
    s32 out[4];
    int i;

    /* @leaf 80015AF4 semantic 2D cross product sign of packed (x,y) screen points; <0 culled */
    LK_FUNC("func_80015AF4");
    for (i = 0; i < 4; i++) out[i] = 0x5A5A5A5A;
    /* (0,0)->(10,0)->(0,10): cross = 10*10 - 0*0 = +100 -> kept, written */
    FCK_EQ(gEmitFrontFacingTriangle(PT(0, 0), PT(10, 0), PT(0, 10), out), 0, "ccw kept");
    FCK_EQ(out[0], PT(0, 0), "ccw out0");
    FCK_EQ(out[1], PT(10, 0), "ccw out1");
    FCK_EQ(out[2], PT(0, 10), "ccw out2");
    FCK_EQ(out[3], 0x5A5A5A5A, "ccw out3 untouched");
    for (i = 0; i < 4; i++) out[i] = 0x5A5A5A5A;
    /* reversed winding: cross = -100 -> culled, out untouched */
    FCK_EQ(gEmitFrontFacingTriangle(PT(0, 0), PT(0, 10), PT(10, 0), out), 1, "cw culled");
    FCK_EQ(out[0], 0x5A5A5A5A, "cw out untouched");
    /* collinear: cross = 0 -> not < 0 -> kept */
    FCK_EQ(gEmitFrontFacingTriangle(PT(0, 0), PT(5, 5), PT(10, 10), out), 0, "degenerate kept");
    FCK_EQ(out[2], PT(10, 10), "degenerate written");
    /* negative coordinates need s16 sign extension of x and arithmetic >>16 of y */
    FCK_EQ(gEmitFrontFacingTriangle(PT(-5, -5), PT(5, -5), PT(-5, 5), out), 0, "neg ccw kept");
    FCK_EQ(gEmitFrontFacingTriangle(PT(-5, -5), PT(-5, 5), PT(5, -5), out), 1, "neg cw culled");
    /* large screen span: (-1000,-1000),(1000,-1000),(-1000,1000) cross = 2000*2000 */
    FCK_EQ(gEmitFrontFacingTriangle(PT(-1000, -1000), PT(1000, -1000), PT(-1000, 1000), out), 0, "wide ccw");

    /* @leaf 80049EF8 semantic same cross product as a predicate: 1 when cross >= 0 */
    LK_FUNC("func_80049EF8");
    FCK_EQ(gIsTriangleFrontFacing(PT(0, 0), PT(10, 0), PT(0, 10)), 1, "ccw");
    FCK_EQ(gIsTriangleFrontFacing(PT(0, 0), PT(0, 10), PT(10, 0)), 0, "cw");
    FCK_EQ(gIsTriangleFrontFacing(PT(0, 0), PT(5, 5), PT(10, 10)), 1, "collinear counts as facing");
    FCK_EQ(gIsTriangleFrontFacing(PT(-5, -5), PT(5, -5), PT(-5, 5)), 1, "neg ccw");
    FCK_EQ(gIsTriangleFrontFacing(PT(-5, -5), PT(-5, 5), PT(5, -5)), 0, "neg cw");
    FCK_EQ(gIsTriangleFrontFacing(PT(0, 0), PT(1, 0), PT(0, -1)), 0, "tiny cw");

    /* @leaf 8001A750 semantic octagonal distance |dx|+|dz|-min(|dx|,|dz|)/2 on x and z only */
    LK_FUNC("func_8001A750");
    {
        s32 a[3], b[3];
        a[0] = 0; a[1] = 0; a[2] = 0;
        b[0] = 3; b[1] = 1000; b[2] = 4;          /* y must be ignored */
        FCK_EQ(gApproximateDistanceXzHalfMin(a, b), 6, "3,4 -> 7-1");
        b[0] = 10; b[2] = 0;
        FCK_EQ(gApproximateDistanceXzHalfMin(a, b), 10, "axis x");
        b[0] = 0; b[2] = -10;
        FCK_EQ(gApproximateDistanceXzHalfMin(a, b), 10, "axis -z");
        b[0] = -10; b[2] = 10;
        FCK_EQ(gApproximateDistanceXzHalfMin(a, b), 15, "diagonal 20-5");
        b[0] = 1; b[2] = 1;
        FCK_EQ(gApproximateDistanceXzHalfMin(a, b), 2, "min/2 floors 1->0");
        a[0] = 100; a[2] = -50; b[0] = 40; b[2] = 30; /* dx=60 dz=80 */
        FCK_EQ(gApproximateDistanceXzHalfMin(a, b), 110, "60,80 -> 140-30");
        FCK_EQ(gApproximateDistanceXzHalfMin(b, a), 110, "symmetric");
        FCK_EQ(gApproximateDistanceXzHalfMin(a, a), 0, "same point");
    }

    /* @leaf 8001A6F8 semantic dst += (src-dst)/4 as (3*dst+src)>>2, arithmetic shift floors */
    LK_FUNC("func_8001A6F8");
    {
        s32 src[3], dst[3];
        src[0] = 400; src[1] = 0; src[2] = 0;
        dst[0] = 0; dst[1] = 100; dst[2] = -100;
        gEaseVectorQuarter(src, dst);
        FCK_EQ(dst[0], 100, "x quarter toward 400");
        FCK_EQ(dst[1], 75, "y quarter toward 0");
        FCK_EQ(dst[2], -75, "z quarter toward 0");
        FCK_EQ(src[0], 400, "src untouched");
        dst[0] = -1; dst[1] = 1; dst[2] = 7;
        src[0] = 0; src[1] = 0; src[2] = 7;
        gEaseVectorQuarter(src, dst);
        FCK_EQ(dst[0], -1, "-3>>2 floors to -1");
        FCK_EQ(dst[1], 0, "3>>2 = 0");
        FCK_EQ(dst[2], 7, "at target stays");
    }
}

/* ================================================================== math */

static void t_math(void)
{
    int i;

    /* @leaf 800187C8 math floor(sqrt(v)*256) == isqrt(v<<16), vectors from leaf_vectors.py */
    LK_FUNC("func_800187C8");
    for (i = 0; i < LV_COUNT(lv_sqrt); i++)
        FCK_EQ(gSqrtU32Q8(lv_sqrt[i][0]), lv_sqrt[i][1], "sqrt q8 vector");

    /* @leaf 800886C8 libc signed min(a,b) */
    LK_FUNC("func_800886C8");
    FCK_EQ(gMinS32(1, 2), 1, "1<2");
    FCK_EQ(gMinS32(2, 1), 1, "2>1");
    FCK_EQ(gMinS32(-5, 3), -5, "signed compare");
    FCK_EQ(gMinS32(7, 7), 7, "equal");
    FCK_EQ(gMinS32((s32)0x80000000, 0x7FFFFFFF), (s32)0x80000000, "INT_MIN");
    FCK_EQ(gMinS32(0x7FFFFFFF, -1), -1, "INT_MAX vs -1");

    /* @leaf 8003B65C source quarter of range: <=r/4 ->0, <r/2 ->1, <3r/4 ->2, else 3 */
    LK_FUNC("func_8003B65C");
    FCK_EQ(gQuarterIndex(0, 100), 0, "0");
    FCK_EQ(gQuarterIndex(25, 100), 0, "25 inclusive");
    FCK_EQ(gQuarterIndex(26, 100), 1, "26");
    FCK_EQ(gQuarterIndex(49, 100), 1, "49");
    FCK_EQ(gQuarterIndex(50, 100), 2, "50 exclusive");
    FCK_EQ(gQuarterIndex(74, 100), 2, "74");
    FCK_EQ(gQuarterIndex(75, 100), 3, "75");
    FCK_EQ(gQuarterIndex(1000, 100), 3, "above range");
    FCK_EQ(gQuarterIndex(-5, 100), 0, "negative");
    FCK_EQ(gQuarterIndex(1, 2), 3, "range 2: 1 > 0, 1 !< 1, 1 !< (6>>2) -> 3");
}

/* ========================================================== colour packing */

static void t_colour(void)
{
    s32 o[3];
    unsigned int u[3];

    /* @leaf 8001BC44 semantic 0xRRGGBB packing, each channel masked to 8 bits */
    LK_FUNC("func_8001BC44");
    FCK_EQ(gPackRgb24(0x12, 0x34, 0x56), 0x123456, "pack");
    FCK_EQ(gPackRgb24(0, 0, 0), 0, "black");
    FCK_EQ(gPackRgb24(0xFF, 0xFF, 0xFF), 0xFFFFFF, "white");
    FCK_EQ(gPackRgb24(0x1FF, 0x100, 0xABC), 0xFF00BC, "channel overflow masked");

    /* @leaf 8001BC64 semantic split 0xRRGGBB and multiply each channel */
    LK_FUNC("func_8001BC64");
    gScaleRgb24(0x102030, 3, o);
    FCK_EQ(o[0], 0x30, "r*3"); FCK_EQ(o[1], 0x60, "g*3"); FCK_EQ(o[2], 0x90, "b*3");
    gScaleRgb24(0x102030, -2, o);
    FCK_EQ(o[0], -32, "r*-2"); FCK_EQ(o[1], -64, "g*-2"); FCK_EQ(o[2], -96, "b*-2");
    gScaleRgb24(0xFF000001, 5, o);
    FCK_EQ(o[0], 0, "top byte ignored"); FCK_EQ(o[2], 5, "b");

    /* @leaf 8001BCD0 semantic like 8001BC64 but accumulates into out */
    LK_FUNC("func_8001BCD0");
    o[0] = 1; o[1] = 2; o[2] = 3;
    gAccumScaledRgb24(0x010203, 10, o);
    FCK_EQ(o[0], 11, "r acc"); FCK_EQ(o[1], 22, "g acc"); FCK_EQ(o[2], 33, "b acc");
    gAccumScaledRgb24(0x010203, -10, o);
    FCK_EQ(o[0], 1, "r undo"); FCK_EQ(o[1], 2, "g undo"); FCK_EQ(o[2], 3, "b undo");

    /* @leaf 8001BCA4 semantic channels as 16.16 fixed point (channel << 16) */
    LK_FUNC("func_8001BCA4");
    gSplitRgb24Shifted(0xAABBCC, u);
    FCK_EQ(u[0], 0xAA0000, "r<<16"); FCK_EQ(u[1], 0xBB0000, "g<<16"); FCK_EQ(u[2], 0xCC0000, "b<<16");
    gSplitRgb24Shifted(0xFF123456, u);
    FCK_EQ(u[0], 0x120000, "top byte ignored"); FCK_EQ(u[2], 0x560000, "b");

    /* @leaf 8001BD2C semantic accumulate the 16.16 channels, u32 wrap-around */
    LK_FUNC("func_8001BD2C");
    u[0] = 1; u[1] = 2; u[2] = 0xFFFF0000u;
    gAccumRgb24Shifted(0xAABB02, u);
    FCK_EQ(u[0], 0xAA0001, "r acc"); FCK_EQ(u[1], 0xBB0002, "g acc");
    FCK_EQ(u[2], 0x00010000, "b wraps modulo 2^32");
}

/* ============================================================ memory/libc */

static void t_memory(void)
{
    int w[6];
    void *r;

    /* @leaf 8001C27C libc *dst = *src */
    LK_FUNC("func_8001C27C");
    w[0] = 0x11111111; w[1] = 0x12345678; w[2] = 0x33333333;
    gCopyWord(&w[0], &w[1]);
    FCK_EQ(w[0], 0x12345678, "copied"); FCK_EQ(w[1], 0x12345678, "src kept");
    FCK_EQ(w[2], 0x33333333, "neighbour kept");

    /* @leaf 8001C470 libc *p = 0 */
    LK_FUNC("func_8001C470");
    w[0] = -1; w[1] = -1; w[2] = -1;
    gClearWord(&w[1]);
    FCK_EQ(w[1], 0, "cleared"); FCK_EQ(w[0], -1, "left kept"); FCK_EQ(w[2], -1, "right kept");

    /* @leaf 80018460 semantic heap block header: size word at payload-4 */
    LK_FUNC("func_80018460");
    w[0] = 0x1234; w[1] = 77; w[2] = -8;
    FCK_EQ(gReadHeapBlockSize(&w[1]), 0x1234, "header of w[1]");
    FCK_EQ(gReadHeapBlockSize(&w[3]), -8, "signed header");

    /* @leaf 8001E304 semantic header read with NULL -> -1 */
    LK_FUNC("func_8001E304");
    FCK_EQ(gGetBlockSize(0), -1, "null");
    w[0] = 4096;
    FCK_EQ(gGetBlockSize(&w[1]), 4096, "header");
    w[0] = 0;
    FCK_EQ(gGetBlockSize(&w[1]), 0, "zero header");

    /* @leaf 8001E4C0 libc memset with return value = dest, low byte of value */
    LK_FUNC("func_8001E4C0");
    lk_fill(buf.b, 0xEE, 32);
    r = gFillMemory(buf.b + 1, 0x1A5, 5);
    FCK_CHECK(r == (void *)(buf.b + 1), "returns dest");
    FCK_CHECK(lk_all(buf.b, 1, 6, 0xA5), "5 bytes set to low byte");
    FCK_EQ(buf.b[0], 0xEE, "before kept"); FCK_EQ(buf.b[6], 0xEE, "after kept");
    r = gFillMemory(buf.b + 10, 0, 0);
    FCK_CHECK(r == (void *)(buf.b + 10), "count 0 returns dest");
    FCK_EQ(buf.b[10], 0xEE, "count 0 writes nothing");

    /* @leaf 80095830 libc memset (void) */
    LK_FUNC("func_80095830");
    lk_fill(buf.b, 0xEE, 32);
    gMemset(buf.b + 3, 0, 7);
    FCK_CHECK(lk_all(buf.b, 3, 10, 0), "7 zero bytes");
    FCK_EQ(buf.b[2], 0xEE, "before kept"); FCK_EQ(buf.b[10], 0xEE, "after kept");
    gMemset(buf.b, 0x7F, 0);
    FCK_EQ(buf.b[0], 0xEE, "count 0");
    gMemset(buf.b, -1, 1);
    FCK_EQ(buf.b[0], 0xFF, "value -1 -> 0xFF");

    /* @leaf 80091B1C libc word clear of count words */
    LK_FUNC("func_80091B1C");
    lk_fill(buf.b, 0xEE, 32);
    gZeroWords((int *)&buf.w[1], 3);
    FCK_EQ(buf.w[0], 0xEEEEEEEE, "before"); FCK_EQ(buf.w[1], 0, "w1");
    FCK_EQ(buf.w[3], 0, "w3"); FCK_EQ(buf.w[4], 0xEEEEEEEE, "after");
    gZeroWords((int *)&buf.w[5], 0);
    FCK_EQ(buf.w[5], 0xEEEEEEEE, "count 0");

    /* @leaf 80091C90 libc word clear of count words (second copy in the image) */
    LK_FUNC("func_80091C90");
    lk_fill(buf.b, 0xEE, 32);
    gClearIntegerRun((int *)&buf.w[2], 2);
    FCK_EQ(buf.w[1], 0xEEEEEEEE, "before"); FCK_EQ(buf.w[2], 0, "w2");
    FCK_EQ(buf.w[3], 0, "w3"); FCK_EQ(buf.w[4], 0xEEEEEEEE, "after");
    gClearIntegerRun((int *)&buf.w[0], 0);
    FCK_EQ(buf.w[0], 0xEEEEEEEE, "count 0");

    /* @leaf 80028BA0 libc word copy of byte_count/4 words, remainder bytes dropped */
    LK_FUNC("func_80028BA0");
    lk_fill(buf.b, 0xEE, 32);
    buf2.w[0] = 0x01020304; buf2.w[1] = 0x05060708; buf2.w[2] = 0x090A0B0C;
    gCopyWholeWords(buf.w, buf2.w, 10);
    FCK_EQ(buf.w[0], 0x01020304, "w0"); FCK_EQ(buf.w[1], 0x05060708, "w1");
    FCK_EQ(buf.w[2], 0xEEEEEEEE, "10 bytes -> 2 words only");
    gCopyWholeWords(buf.w + 4, buf2.w, 3);
    FCK_EQ(buf.w[4], 0xEEEEEEEE, "3 bytes -> nothing");

    /* @leaf 8008D5D0 libc overlap-safe copy (memmove); return is dest (backward) or dest+n (forward) */
    LK_FUNC("func_8008D5D0");
    {
        u8 *p;
        int i;
        for (i = 0; i < 16; i++) buf.b[i] = (u8)i;
        p = gCopyMemOverlap(buf.b + 2, buf.b, 6);   /* dest > src, overlapping */
        FCK_EQ(buf.b[2], 0, "fwd-overlap b2"); FCK_EQ(buf.b[7], 5, "fwd-overlap b7");
        FCK_EQ(buf.b[8], 8, "fwd-overlap untouched");
        FCK_CHECK(p == buf.b + 2, "dest>=src returns dest");
        for (i = 0; i < 16; i++) buf.b[i] = (u8)i;
        p = gCopyMemOverlap(buf.b, buf.b + 3, 5);   /* dest < src, overlapping */
        FCK_EQ(buf.b[0], 3, "back-overlap b0"); FCK_EQ(buf.b[4], 7, "back-overlap b4");
        FCK_EQ(buf.b[5], 5, "back-overlap untouched");
        FCK_CHECK(p == buf.b + 5, "dest<src returns dest+count");
        p = gCopyMemOverlap(buf.b, buf.b + 3, 0);
        FCK_CHECK(p == buf.b, "count 0");
        p = gCopyMemOverlap(buf.b, buf.b + 3, -4);
        FCK_EQ(buf.b[0], 3, "negative count copies nothing");
        for (i = 0; i < 16; i++) buf.b[i] = (u8)i;
        p = gCopyMemOverlap(buf.b + 4, buf.b + 4, 3);  /* dest == src takes the backward path */
        FCK_CHECK(p == buf.b + 4, "dest==src returns dest");
        FCK_EQ(buf.b[4], 4, "dest==src content kept");
        p = gCopyMemOverlap(buf.b + 6, buf.b + 2, 3);  /* exactly count bytes, no extra */
        FCK_EQ(buf.b[6], 2, "b6"); FCK_EQ(buf.b[8], 4, "b8");
        FCK_EQ(buf.b[5], 5, "byte before dest untouched");
        FCK_EQ(buf.b[9], 9, "byte after dest untouched");
    }

    /* @leaf 80095FE8 libc u16 load, zero-extended */
    LK_FUNC("func_80095FE8");
    buf.h[0] = 0xBEEF; buf.h[1] = 0xFFFF;
    FCK_EQ(gMdecReadHalfword(&buf.h[0]), 0xBEEF, "beef");
    FCK_EQ(gMdecReadHalfword(&buf.h[1]), 0xFFFF, "ffff not sign-extended");
}

/* ======================================================== data structures */

static void t_structs(void)
{
    /* @leaf 800195A4 semantic free-list pop: head at +0x10, count at +0x0C */
    /* @leaf 800195D0 semantic free-list push (LIFO pair of 800195A4) */
    u32 st[5];
    int na[1], nb[1];
    void *r;
    st[3] = 0; st[4] = 0;
    LK_FUNC("func_800195A4");
    r = gTakeFixedPoolSlot(st);
    FCK_CHECK(r == 0, "empty pool -> NULL");
    FCK_EQ(st[3], 0, "empty pool count unchanged");
    LK_FUNC("func_800195D0");
    na[0] = 0x777; nb[0] = 0x777;
    gReturnFixedPoolSlot(st, na);
    FCK_EQ(na[0], 0, "first push links to old head NULL");
    FCK_CHECK(st[4] == (u32)na, "head = a");
    FCK_EQ(st[3], 1, "count 1");
    gReturnFixedPoolSlot(st, nb);
    FCK_CHECK(nb[0] == (int)na, "b links to a");
    FCK_CHECK(st[4] == (u32)nb, "head = b");
    FCK_EQ(st[3], 2, "count 2");
    LK_FUNC("func_800195A4");
    r = gTakeFixedPoolSlot(st);
    FCK_CHECK(r == (void *)nb, "LIFO: b first");
    FCK_CHECK(st[4] == (u32)na, "head back to a");
    FCK_EQ(st[3], 1, "count 1 after pop");
    r = gTakeFixedPoolSlot(st);
    FCK_CHECK(r == (void *)na, "then a");
    FCK_EQ(st[3], 0, "count 0");
    FCK_CHECK(st[4] == 0, "head NULL");
    FCK_CHECK(gTakeFixedPoolSlot(st) == 0, "drained");

    /* @leaf 8003BEF0 source node{prev,next,tail}: pop tail; tail->prev=node->next; node->next=tail */
    LK_FUNC("func_8003BEF0");
    {
        u32 node[3], t1[3], t0[3];
        node[0] = 0x100; node[1] = 0x200; node[2] = 0;
        FCK_CHECK(gTakePoolNode(node) == 0, "no tail -> NULL");
        FCK_EQ(node[1], 0x200, "no tail: next unchanged");
        t0[0] = 0; t1[0] = (u32)t0;               /* chain: t1 -> t0 */
        node[2] = (u32)t1;
        FCK_CHECK(gTakePoolNode(node) == (void *)t1, "returns tail");
        FCK_CHECK(node[2] == (u32)t0, "tail advanced to t1->previous");
        FCK_EQ(t1[0], 0x200, "t1->previous = old node->next");
        FCK_CHECK(node[1] == (u32)t1, "node->next = taken");
        FCK_EQ(node[0], 0x100, "node->previous untouched");
    }

    /* @leaf 80021800 source offset table at base+0x10; 0 offset -> NULL, else base+offset */
    LK_FUNC("func_80021800");
    lk_fill(buf.b, 0, 64);
    W32(buf.b, 0x10) = 0x40; W32(buf.b, 0x14) = 0; W32(buf.b, 0x18) = 8;
    FCK_CHECK(gResolveSoundBankEntry(buf.b, 0) == (void *)(buf.b + 0x40), "entry 0");
    FCK_CHECK(gResolveSoundBankEntry(buf.b, 1) == 0, "empty entry");
    FCK_CHECK(gResolveSoundBankEntry(buf.b, 2) == (void *)(buf.b + 8), "entry 2");

    /* @leaf 8002B1AC source copy s16 at 0x158 into 0x10C when different, then mirror 0x10C to 0x10E */
    LK_FUNC("func_8002B1AC");
    {
        static union { u32 w[96]; u8 b[384]; } o;
        S16(o.b, 0x10C) = 5; S16(o.b, 0x158) = 9; S16(o.b, 0x10E) = 0;
        gSyncCountdownFields(o.b);
        FCK_EQ(S16(o.b, 0x10C), 9, "updated"); FCK_EQ(S16(o.b, 0x10E), 9, "mirrored");
        FCK_EQ(S16(o.b, 0x158), 9, "source kept");
        S16(o.b, 0x10C) = 7; S16(o.b, 0x158) = 7; S16(o.b, 0x10E) = 1;
        gSyncCountdownFields(o.b);
        FCK_EQ(S16(o.b, 0x10C), 7, "equal kept"); FCK_EQ(S16(o.b, 0x10E), 7, "mirror always");
        S16(o.b, 0x10C) = 0; S16(o.b, 0x158) = -1;
        gSyncCountdownFields(o.b);
        FCK_EQ(H16(o.b, 0x10C), 0xFFFF, "negative copied"); FCK_EQ(H16(o.b, 0x10E), 0xFFFF, "mirror");
    }

    /* @leaf 800489C8 source glyph record = values(+0x10) + 11 * byte[0x1C + offset] */
    LK_FUNC("func_800489C8");
    lk_fill(buf.b, 0, 64);
    W32(buf.b, 0x10) = (u32)buf2.b;
    buf.b[0x1C] = 0; buf.b[0x1D] = 3; buf.b[0x1F] = 255;
    FCK_CHECK(gLookupFontGlyph(buf.b, 0) == buf2.b, "index 0");
    FCK_CHECK(gLookupFontGlyph(buf.b, 1) == buf2.b + 33, "index 3 -> 33");
    FCK_CHECK(gLookupFontGlyph(buf.b, 3) == buf2.b + 2805, "index 255 unsigned");

    /* @leaf 8007D39C source find first entry (stride 6) with value; not found -> 0 */
    LK_FUNC("func_8007D39C");
    {
        u32 list[3];
        lk_fill(buf2.b, 0, 64);
        buf2.b[0] = 10; buf2.b[6] = 20; buf2.b[12] = 30; buf2.b[18] = 20;
        list[0] = (u32)buf2.b; list[1] = 0; list[2] = 4;  /* count byte at +8 */
        FCK_EQ(gFindListEntryByValue(list, 10), 0, "first");
        FCK_EQ(gFindListEntryByValue(list, 20), 1, "first of duplicates");
        FCK_EQ(gFindListEntryByValue(list, 30), 2, "third");
        FCK_EQ(gFindListEntryByValue(list, 99), 0, "missing -> 0");
        list[2] = 2;
        FCK_EQ(gFindListEntryByValue(list, 30), 0, "beyond count -> 0");
        list[2] = 0;
        FCK_EQ(gFindListEntryByValue(list, 10), 0, "empty list");
    }

    /* @leaf 8008C858 source ready u16 at 0xE6==0 -> 1; device byte 0x46==0xFF -> 0; else 1 */
    LK_FUNC("func_8008C858");
    lk_fill(buf.b, 0, 256);
    FCK_EQ(gCheckPadReady(buf.b), 1, "not ready flag -> 1");
    H16(buf.b, 0xE6) = 1; buf.b[0x46] = 0xFF;
    FCK_EQ(gCheckPadReady(buf.b), 0, "ready and no device");
    buf.b[0x46] = 0x41;
    FCK_EQ(gCheckPadReady(buf.b), 1, "ready and device");
    H16(buf.b, 0xE6) = 0;
    FCK_EQ(gCheckPadReady(buf.b), 1, "device 0xFF but not ready -> 1");
}

/* ===================================================== text and checksums */

static void t_text(void)
{
    u8 table[256], dst[8], src[8];
    int i;
    for (i = 0; i < 256; i++) table[i] = (u8)i;
    table[1] = 'X';

    /* @leaf 8007AA9C semantic translate via table, trailing spaces -> NUL, empty -> " " */
    LK_FUNC("func_8007AA9C");
    lk_fill(dst, 0xEE, 8);
    src[0] = 'A'; src[1] = 'B'; src[2] = ' '; src[3] = ' ';
    gDecodeGlyphsToName(dst, src, 4, table);
    FCK_EQ(dst[0], 'A', "A"); FCK_EQ(dst[1], 'B', "B");
    FCK_EQ(dst[2], 0, "trailing space 1"); FCK_EQ(dst[3], 0, "trailing space 2");
    FCK_EQ(dst[4], 0, "terminator"); FCK_EQ(dst[5], 0xEE, "beyond kept");
    src[0] = 'A'; src[1] = ' '; src[2] = 1; src[3] = ' ';
    gDecodeGlyphsToName(dst, src, 4, table);
    FCK_EQ(dst[1], ' ', "inner space kept"); FCK_EQ(dst[2], 'X', "table lookup");
    FCK_EQ(dst[3], 0, "trailing");
    src[0] = ' '; src[1] = ' '; src[2] = ' ';
    lk_fill(dst, 0xEE, 8);
    gDecodeGlyphsToName(dst, src, 3, table);
    FCK_EQ(dst[0], ' ', "all blank -> single space"); FCK_EQ(dst[1], 0, "rest NUL");
    FCK_EQ(dst[3], 0, "terminator");
    lk_fill(dst, 0xEE, 8);
    gDecodeGlyphsToName(dst, src, 0, table);
    FCK_EQ(dst[0], ' ', "len 0 -> single space"); FCK_EQ(dst[1], 0xEE, "len 0 writes only dst[0]");

    /* @leaf 8007F688 source sum += b[i]+i; other = ((b[i]^other)<<1)+i, hand-computed */
    LK_FUNC("func_8007F688");
    {
        u32 s = 0xDEAD, o = 0xBEEF;
        src[0] = 1; src[1] = 2; src[2] = 3;
        gComputeDualChecksum(src, 3, &s, &o);
        FCK_EQ(s, 9, "sum (1+0)+(2+1)+(3+2)");
        FCK_EQ(o, 6, "other 2 -> 1 -> 6");
        s = 0xDEAD; o = 0xBEEF;
        gComputeDualChecksum(src, 0, &s, &o);
        FCK_EQ(s, 0, "len 0 clears sum"); FCK_EQ(o, 0, "len 0 clears other");
        src[0] = 0xFF;
        gComputeDualChecksum(src, 1, &s, &o);
        FCK_EQ(s, 255, "byte unsigned"); FCK_EQ(o, 510, "0xFF<<1");
    }
}

/* =============================================================== CD-ROM */

static void t_cd(void)
{
    u8 a[4], b[4];
    int i;

    /* @leaf 8008E720 spec Psy-Q CdPosToInt: BCD MSF -> sector - 150, vectors from leaf_vectors.py */
    LK_FUNC("func_8008E720");
    for (i = 0; i < LV_COUNT(lv_cd); i++) {
        u32 loc;
        loc = (u32)lv_cd[i][0] | ((u32)lv_cd[i][1] << 8) | ((u32)lv_cd[i][2] << 16) | 0x99000000u;
        FCK_EQ(gCdPosToInt(&loc), lv_cd[i][3], "CdPosToInt vector (track byte ignored)");
    }

    /* @leaf 8001C3F8 semantic lexicographic MSF compare; sign of (b - a); track byte ignored */
    LK_FUNC("func_8001C3F8");
    a[0] = 0x01; a[1] = 0x20; a[2] = 0x30; a[3] = 0x00;
    b[0] = 0x01; b[1] = 0x20; b[2] = 0x30; b[3] = 0x55;
    FCK_EQ(gCompareCdlLoc(a, b), 0, "equal");
    b[2] = 0x31; FCK_EQ(gCompareCdlLoc(a, b), 1, "later sector");
    b[2] = 0x29; FCK_EQ(gCompareCdlLoc(a, b), -1, "earlier sector");
    b[1] = 0x21; FCK_EQ(gCompareCdlLoc(a, b), 1, "second beats sector");
    b[1] = 0x19; b[2] = 0x74; FCK_EQ(gCompareCdlLoc(a, b), -1, "earlier second");
    b[0] = 0x02; b[1] = 0x00; FCK_EQ(gCompareCdlLoc(a, b), 1, "minute beats all");
    b[0] = 0x00; b[1] = 0x59; FCK_EQ(gCompareCdlLoc(a, b), -1, "earlier minute");
    a[0] = 0x00; a[1] = 0xFF; b[1] = 0x00; FCK_EQ(gCompareCdlLoc(a, b), -1, "bytes are unsigned");
}

/* ================================================= pad command state block
 * CommandState offsets from include/game/command_state.h (static-asserted
 * there): source 0x20, inline 0x24, value 0x2C, type 0x34, active 0x35,
 * current 0x36, previous 0x37. Command bytes: psx-spx "Controllers -
 * Configuration Commands": 43h config mode, 45h get status/model,
 * 46h/47h/4Ch constant queries, 4Dh rumble (actuator) mapping. */

static void t_pad(void)
{
    u8 *s = buf.b;

    /* @leaf 8008AE70 source store word to inline+4 (0x28), type byte at 0x34 */
    LK_FUNC("func_8008AE70");
    lk_fill(s, 0xEE, 64);
    gBindActuatorBuffer(s, 0x12345678, 0x9A);
    FCK_EQ(W32(s, 0x28), 0x12345678, "word at 0x28"); FCK_EQ(s[0x34], 0x9A, "type");
    FCK_EQ(W32(s, 0x24), 0xEEEEEEEE, "inline[0..3] kept"); FCK_EQ(s[0x35], 0xEE, "active kept");

    /* @leaf 8008AE7C source current(0x36)=a, value(0x2C)=b, active(0x35)=c */
    LK_FUNC("func_8008AE7C");
    lk_fill(s, 0xEE, 64);
    gSetPadCommandState(s, 0x142, 0xCAFEBABE, 0x103);
    FCK_EQ(s[0x36], 0x42, "current low byte"); FCK_EQ(W32(s, 0x2C), 0xCAFEBABE, "value");
    FCK_EQ(s[0x35], 0x03, "active low byte"); FCK_EQ(s[0x37], 0xEE, "previous kept");

    /* @leaf 8008B4EC spec psx-spx 4Dh rumble mapping: current=0x4D, 6 bytes, value=source */
    LK_FUNC("func_8008B4EC");
    lk_fill(s, 0xEE, 64);
    W32(s, 0x20) = (u32)buf2.b;
    gCmdPadActAlign(s);
    FCK_EQ(s[0x36], 0x4D, "cmd 4Dh"); FCK_EQ(s[0x35], 6, "6 parameter bytes");
    FCK_CHECK(W32(s, 0x2C) == (u32)buf2.b, "value = source buffer");

    /* @leaf 8008B71C spec psx-spx 43h enter/exit config mode, 1 inline byte */
    LK_FUNC("func_8008B71C");
    lk_fill(s, 0xEE, 64);
    gSetPadConfigModeCmd(s, 1);
    FCK_EQ(s[0x36], 0x43, "cmd 43h"); FCK_EQ(s[0x35], 1, "1 byte");
    FCK_EQ(s[0x24], 1, "inline[0]"); FCK_CHECK(W32(s, 0x2C) == (u32)(s + 0x24), "value -> inline");
    FCK_EQ(s[0x25], 0xEE, "inline[1] kept");

    /* @leaf 8008B73C spec psx-spx 45h get status/model, no parameters */
    LK_FUNC("func_8008B73C");
    lk_fill(s, 0xEE, 64);
    gSetPadModelQueryCmd(s);
    FCK_EQ(s[0x36], 0x45, "cmd 45h"); FCK_EQ(s[0x35], 0, "0 bytes"); FCK_EQ(W32(s, 0x2C), 0, "no buffer");

    /* @leaf 8008B750 spec psx-spx 4Ch constant query, 1 inline byte */
    LK_FUNC("func_8008B750");
    lk_fill(s, 0xEE, 64);
    gSetPadModeConstCmd(s, 0xFF);
    FCK_EQ(s[0x36], 0x4C, "cmd 4Ch"); FCK_EQ(s[0x35], 1, "1 byte"); FCK_EQ(s[0x24], 0xFF, "inline[0]");
    FCK_CHECK(W32(s, 0x2C) == (u32)(s + 0x24), "value -> inline");

    /* @leaf 8008B770 spec psx-spx 46h actuator info query, 1 inline byte */
    LK_FUNC("func_8008B770");
    lk_fill(s, 0xEE, 64);
    gSetPadActInfoCmd(s, 2);
    FCK_EQ(s[0x36], 0x46, "cmd 46h"); FCK_EQ(s[0x35], 1, "1 byte"); FCK_EQ(s[0x24], 2, "inline[0]");
    FCK_CHECK(W32(s, 0x2C) == (u32)(s + 0x24), "value -> inline");

    /* @leaf 8008B790 spec psx-spx 47h combination info query, 1 inline byte */
    LK_FUNC("func_8008B790");
    lk_fill(s, 0xEE, 64);
    gSetPadCombInfoCmd(s, 0);
    FCK_EQ(s[0x36], 0x47, "cmd 47h"); FCK_EQ(s[0x35], 1, "1 byte"); FCK_EQ(s[0x24], 0, "inline[0]");
    FCK_CHECK(W32(s, 0x2C) == (u32)(s + 0x24), "value -> inline");

    /* @leaf 8008B7B0 source command 0x4B without parameters (byte not in the psx-spx core table) */
    LK_FUNC("func_8008B7B0");
    lk_fill(s, 0xEE, 64);
    gSetPadCombContCmd(s);
    FCK_EQ(s[0x36], 0x4B, "cmd 4Bh"); FCK_EQ(s[0x35], 0, "0 bytes"); FCK_EQ(W32(s, 0x2C), 0, "no buffer");

    /* @leaf 8008C06C source return current, move it to previous, clear current */
    LK_FUNC("func_8008C06C");
    lk_fill(s, 0xEE, 64);
    s[0x36] = 0x45; s[0x37] = 0x11;
    FCK_EQ(gConsumePadCmd(s), 0x45, "returns current");
    FCK_EQ(s[0x36], 0, "current cleared"); FCK_EQ(s[0x37], 0x45, "previous = old current");
    FCK_EQ(gConsumePadCmd(s), 0, "second consume -> 0");
    FCK_EQ(s[0x37], 0, "previous = 0");
    s[0x36] = 0xF0;
    FCK_EQ(gConsumePadCmd(s), 0xF0, "byte is unsigned");

    /* @leaf 8004DA58 source command kinds 0x3C..0x45 and 0x64..0x65 (unsigned range test) */
    LK_FUNC("func_8004DA58");
    FCK_EQ(gIsQueuedCommandKind(0x3B), 0, "below");
    FCK_EQ(gIsQueuedCommandKind(0x3C), 1, "lo1");
    FCK_EQ(gIsQueuedCommandKind(0x45), 1, "hi1");
    FCK_EQ(gIsQueuedCommandKind(0x46), 0, "gap");
    FCK_EQ(gIsQueuedCommandKind(0x63), 0, "gap2");
    FCK_EQ(gIsQueuedCommandKind(0x64), 1, "lo2");
    FCK_EQ(gIsQueuedCommandKind(0x65), 1, "hi2");
    FCK_EQ(gIsQueuedCommandKind(0x66), 0, "above");
    FCK_EQ(gIsQueuedCommandKind(0), 0, "zero (wraps, not < 10)");
    FCK_EQ(gIsQueuedCommandKind(0xFFFFFFFFu), 0, "max");
}

/* ================================================= GPU packet primitives
 * Expected (len, code) pairs are the PSn00bSDK psxgpu.h initialiser macros:
 * setPolyG3 (6,0x30) setPolyGT3 (9,0x34) setPolyF4 (5,0x28) setPolyFT4 (9,0x2c)
 * setPolyG4 (8,0x38) setPolyGT4 (12,0x3c) setSprt16 (3,0x7c) setSprt (4,0x64)
 * setTile1 (2,0x68) setTile8 (2,0x70) setTile16 (2,0x78) setTile (3,0x60)
 * setLineF2 (3,0x40) setLineG2 (4,0x50) setLineG3 (7,0x58,pad@28=0x55555555)
 * setLineF4 (6,0x4c,pad@24=0x55555555). len is tag byte 3, code is byte 7. */

static void check_prim(const char *name, void (*fn)(u8 *), int len, int code)
{
    LK_FUNC(name);
    lk_fill(buf.b, 0xA5, 40);
    fn(buf.b);
    FCK_EQ(buf.b[3], len, "setlen");
    FCK_EQ(buf.b[7], code, "setcode");
    FCK_CHECK(lk_all(buf.b, 0, 3, 0xA5), "tag addr bytes untouched");
    FCK_CHECK(lk_all(buf.b, 4, 7, 0xA5), "rgb0 untouched");
    FCK_CHECK(lk_all(buf.b, 8, 40, 0xA5), "body untouched");
}

static void t_prims(void)
{
    unsigned int t[3];
    /* @leaf 80092488 spec PSn00bSDK setPolyG3 */
    check_prim("func_80092488", gSetPolyG3, 6, 0x30);
    /* @leaf 8009249C spec PSn00bSDK setPolyGT3 */
    check_prim("func_8009249C", gSetPolyGT3, 9, 0x34);
    /* @leaf 800924B0 spec PSn00bSDK setPolyF4 */
    check_prim("func_800924B0", gSetPolyF4, 5, 0x28);
    /* @leaf 800924C4 spec PSn00bSDK setPolyFT4 */
    check_prim("func_800924C4", gSetPolyFT4, 9, 0x2C);
    /* @leaf 800924D8 spec PSn00bSDK setPolyG4 */
    check_prim("func_800924D8", gSetPolyG4, 8, 0x38);
    /* @leaf 800924EC spec PSn00bSDK setPolyGT4 */
    check_prim("func_800924EC", gSetPolyGT4, 12, 0x3C);
    /* @leaf 80092514 spec PSn00bSDK setSprt16 (unit doc calls it gInitPolyF3Packet) */
    check_prim("func_80092514", gSetSprt16, 3, 0x7C);
    /* @leaf 80092528 spec PSn00bSDK setSprt */
    check_prim("func_80092528", gSetSprt, 4, 0x64);
    /* @leaf 8009253C spec PSn00bSDK setTile1 (unit doc calls it gInitPolyFT3Packet) */
    check_prim("func_8009253C", gSetTile1, 2, 0x68);
    /* @leaf 80092550 spec PSn00bSDK setTile8 */
    check_prim("func_80092550", gSetTile8, 2, 0x70);
    /* @leaf 80092564 spec PSn00bSDK setTile16 (unit doc calls it gInitPolyF4Packet) */
    check_prim("func_80092564", gSetTile16, 2, 0x78);
    /* @leaf 80092578 spec PSn00bSDK setTile */
    check_prim("func_80092578", gSetTile, 3, 0x60);
    /* @leaf 8009258C spec PSn00bSDK setLineF2 */
    check_prim("func_8009258C", gSetLineF2, 3, 0x40);
    /* @leaf 800925A0 spec PSn00bSDK setLineG2 */
    check_prim("func_800925A0", gSetLineG2, 4, 0x50);

    /* @leaf 800925D4 spec PSn00bSDK setLineG3: len 7, code 0x58, terminator at +28 */
    LK_FUNC("func_800925D4");
    lk_fill(buf.b, 0xA5, 40);
    gSetLineG3(buf.b);
    FCK_EQ(buf.b[3], 7, "setlen"); FCK_EQ(buf.b[7], 0x58, "setcode");
    FCK_EQ(W32(buf.b, 28), 0x55555555, "polyline terminator");
    FCK_CHECK(lk_all(buf.b, 8, 28, 0xA5), "vertices untouched");
    FCK_CHECK(lk_all(buf.b, 32, 40, 0xA5), "beyond untouched");

    /* @leaf 800925F4 spec PSn00bSDK setLineF4: len 6, code 0x4c, terminator at +24 */
    LK_FUNC("func_800925F4");
    lk_fill(buf.b, 0xA5, 40);
    gSetLineF4(buf.b);
    FCK_EQ(buf.b[3], 6, "setlen"); FCK_EQ(buf.b[7], 0x4C, "setcode");
    FCK_EQ(W32(buf.b, 24), 0x55555555, "polyline terminator");
    FCK_CHECK(lk_all(buf.b, 8, 24, 0xA5), "vertices untouched");
    FCK_CHECK(lk_all(buf.b, 28, 40, 0xA5), "beyond untouched");

    /* @leaf 80092410 spec PSn00bSDK setSemiTrans: code |= 2 / &= ~2 */
    LK_FUNC("func_80092410");
    lk_fill(buf.b, 0xA5, 8);
    buf.b[7] = 0x2C;
    gSetPrimSemiTrans(buf.b, 1);  FCK_EQ(buf.b[7], 0x2E, "on");
    gSetPrimSemiTrans(buf.b, -7); FCK_EQ(buf.b[7], 0x2E, "on idempotent, any non-zero");
    gSetPrimSemiTrans(buf.b, 0);  FCK_EQ(buf.b[7], 0x2C, "off");
    buf.b[7] = 0xFF; gSetPrimSemiTrans(buf.b, 0); FCK_EQ(buf.b[7], 0xFD, "only bit 1 cleared");
    FCK_CHECK(lk_all(buf.b, 0, 7, 0xA5), "other bytes untouched");

    /* @leaf 80092438 spec PSn00bSDK setShadeTex: code |= 1 / &= ~1 */
    LK_FUNC("func_80092438");
    lk_fill(buf.b, 0xA5, 8);
    buf.b[7] = 0x2C;
    gSetPrimRawTexture(buf.b, 1);  FCK_EQ(buf.b[7], 0x2D, "on");
    gSetPrimRawTexture(buf.b, 0);  FCK_EQ(buf.b[7], 0x2C, "off");
    buf.b[7] = 0xFF; gSetPrimRawTexture(buf.b, 0); FCK_EQ(buf.b[7], 0xFE, "only bit 0 cleared");
    FCK_CHECK(lk_all(buf.b, 0, 7, 0xA5), "other bytes untouched");

    /* @leaf 80092340 spec PSn00bSDK isendprim: addr field == 0xffffff */
    LK_FUNC("func_80092340");
    t[0] = 0x04FFFFFF; FCK_EQ(gIsPrimListEnd(t), 1, "end, len byte ignored");
    t[0] = 0x00FFFFFE; FCK_EQ(gIsPrimListEnd(t), 0, "not end");
    t[0] = 0xFFFFFFFF; FCK_EQ(gIsPrimListEnd(t), 1, "all ones");
    t[0] = 0xFF000000; FCK_EQ(gIsPrimListEnd(t), 0, "only len byte");

    /* @leaf 800923D4 spec PSn00bSDK setaddr on a P_TAG: low 24 bits replaced, len kept */
    LK_FUNC("func_800923D4");
    t[0] = 0x07ABCDEF; gSetPrimNextLink(t, 0x80123456);
    FCK_EQ(t[0], 0x07123456, "addr replaced, len kept");
    t[0] = 0x00000000; gSetPrimNextLink(t, 0xFFFFFFFF);
    FCK_EQ(t[0], 0x00FFFFFF, "value high byte ignored");

    /* @leaf 800923F8 spec PSn00bSDK termPrim: addr = 0xffffff, len kept */
    LK_FUNC("func_800923F8");
    t[0] = 0x05000000; gSetPrimListEnd(t); FCK_EQ(t[0], 0x05FFFFFF, "terminated");
    t[0] = 0x09123456; gSetPrimListEnd(t); FCK_EQ(t[0], 0x09FFFFFF, "addr overwritten");

    /* @leaf 80092398 spec PSn00bSDK addPrims(ot, p0, p1): p1.addr = ot.addr, ot.addr = p0 */
    LK_FUNC("func_80092398");
    t[0] = 0xAA123456;           /* ot entry */
    t[1] = 0x09FFFFFF;           /* p1 tag, len 9 */
    gAddPrims(&t[0], 0x80ABCDEFu, &t[1]);
    FCK_EQ(t[1], 0x09123456, "p1 links to old ot target, len kept");
    FCK_EQ(t[0], 0xAAABCDEF, "ot -> p0, ot high byte kept");
}

/* ============================================ GPU environment commands */

static void t_gpu_env(void)
{
    int i;

    /* @leaf 80092230 spec PSn00bSDK getTPage, vectors from leaf_vectors.py */
    LK_FUNC("func_80092230");
    for (i = 0; i < LV_COUNT(lv_tpage); i++)
        FCK_EQ(gGetTPage(lv_tpage[i][0], lv_tpage[i][1], lv_tpage[i][2], lv_tpage[i][3]),
               lv_tpage[i][4], "getTPage vector");

    /* @leaf 8009226C spec PSn00bSDK getClut, vectors from leaf_vectors.py */
    LK_FUNC("func_8009226C");
    for (i = 0; i < LV_COUNT(lv_clut); i++)
        FCK_EQ(gGetClutId(lv_clut[i][0], lv_clut[i][1]), lv_clut[i][2], "getClut vector");

    /* @leaf 80093F58 spec Psy-Q get_mode / PSn00bSDK setDrawTPage: E1h | dtd<<9 | dfe<<10 | tpage&0x9ff */
    LK_FUNC("func_80093F58");
    FCK_EQ(gBuildDrawModeCommand(0, 0, 0), (s32)0xE1000000, "plain");
    FCK_EQ(gBuildDrawModeCommand(1, 0, 0), (s32)0xE1000400, "dfe bit 10");
    FCK_EQ(gBuildDrawModeCommand(0, 1, 0), (s32)0xE1000200, "dtd bit 9");
    FCK_EQ(gBuildDrawModeCommand(2, -3, 0x1FF), (s32)0xE10007FF, "non-zero flags, tpage");
    FCK_EQ(gBuildDrawModeCommand(0, 0, 0xFFFF), (s32)0xE10009FF, "tpage masked 0x9ff");

    /* @leaf 800940A8 spec PSn00bSDK setDrawOffset: E5h | (y&0x7ff)<<11 | (x&0x7ff) */
    LK_FUNC("func_800940A8");
    FCK_EQ(gPackDrawOffset(0, 0), (s32)0xE5000000, "origin");
    FCK_EQ(gPackDrawOffset(160, 120), (s32)(0xE5000000u | (120u << 11) | 160u), "160,120");
    FCK_EQ(gPackDrawOffset(0xFFFFFFFFu, 0), (s32)0xE50007FF, "x masked to 11 bits");
    FCK_EQ(gPackDrawOffset(0x800, 0x800), (s32)0xE5000000, "bit 11 dropped");
    FCK_EQ(gPackDrawOffset(0, 0x7FF), (s32)0xE53FF800, "y max");

    /* @leaf 800940C4 spec Psy-Q get_tw / psx-spx GP0(E2h); NULL -> 0 */
    LK_FUNC("func_800940C4");
    FCK_EQ(gBuildTexWindowCommand(0), 0, "NULL rect");
    for (i = 0; i < LV_COUNT(lv_tw); i++) {
        s16 r[4];
        r[0] = (s16)lv_tw[i][0]; r[1] = (s16)lv_tw[i][1];
        r[2] = (s16)lv_tw[i][2]; r[3] = (s16)lv_tw[i][3];
        FCK_EQ(gBuildTexWindowCommand((u8 *)r), lv_tw[i][4], "tex window vector");
    }

    /* @leaf 80093A4C spec psx-spx GP0(E6h): bit0 set-mask, bit1 check-mask; len 2, 2nd word 0 */
    LK_FUNC("func_80093A4C");
    lk_fill(buf.b, 0xA5, 16);
    gSetDrawMaskPacket(buf.b, 0, 0);
    FCK_EQ(buf.b[3], 2, "len 2"); FCK_EQ(W32(buf.b, 4), (s32)0xE6000000, "no bits");
    FCK_EQ(W32(buf.b, 8), 0, "second word cleared");
    FCK_CHECK(lk_all(buf.b, 0, 3, 0xA5), "tag addr untouched");
    gSetDrawMaskPacket(buf.b, 1, 0); FCK_EQ(W32(buf.b, 4), (s32)0xE6000002, "flag1 -> bit1 check");
    gSetDrawMaskPacket(buf.b, 0, 1); FCK_EQ(W32(buf.b, 4), (s32)0xE6000001, "flag2 -> bit0 set");
    gSetDrawMaskPacket(buf.b, 5, -3); FCK_EQ(W32(buf.b, 4), (s32)0xE6000003, "both, any non-zero");

    /* @leaf 80093A74 spec psx-spx GP0(E6h) set-mask bit only; len 2, 2nd word 0 */
    LK_FUNC("func_80093A74");
    lk_fill(buf.b, 0xA5, 16);
    gSetMaskBitPacket(buf.b, 1);
    FCK_EQ(buf.b[3], 2, "len 2"); FCK_EQ(W32(buf.b, 4), (s32)0xE6000001, "on");
    FCK_EQ(W32(buf.b, 8), 0, "second word cleared");
    gSetMaskBitPacket(buf.b, 0); FCK_EQ(W32(buf.b, 4), (s32)0xE6000000, "off");
    gSetMaskBitPacket(buf.b, 0x100); FCK_EQ(W32(buf.b, 4), (s32)0xE6000001, "any non-zero");
}

/* ================================================== definition records */

static void t_records(void)
{
    /* @leaf 800607B8 source s16 +6 /= 300; +8 = (v<<8)/300 if non-zero; +0xC = (v<<12)/218; C truncation */
    LK_FUNC("func_800607B8");
    lk_fill(buf.b, 0xA5, 16);
    S16(buf.b, 6) = 600; S16(buf.b, 8) = 300; S16(buf.b, 0xC) = 218;
    gScaleDefinitionSubrecordFields(buf.b);
    FCK_EQ(S16(buf.b, 6), 2, "600/300"); FCK_EQ(S16(buf.b, 8), 256, "300<<8/300");
    FCK_EQ(S16(buf.b, 0xC), 4096, "218<<12/218");
    FCK_CHECK(lk_all(buf.b, 0, 6, 0xA5), "head untouched");
    FCK_CHECK(lk_all(buf.b, 0xA, 0xC, 0xA5), "gap untouched");
    S16(buf.b, 6) = -599; S16(buf.b, 8) = 0; S16(buf.b, 0xC) = -1;
    gScaleDefinitionSubrecordFields(buf.b);
    FCK_EQ(S16(buf.b, 6), -1, "-599/300 truncates toward 0");
    FCK_EQ(S16(buf.b, 8), 0, "zero skipped");
    FCK_EQ(S16(buf.b, 0xC), -18, "-4096/218 truncates toward 0");
    S16(buf.b, 6) = 299; S16(buf.b, 8) = -1; S16(buf.b, 0xC) = 1;
    gScaleDefinitionSubrecordFields(buf.b);
    FCK_EQ(S16(buf.b, 6), 0, "299/300"); FCK_EQ(S16(buf.b, 8), 0, "-256/300 -> 0");
    FCK_EQ(S16(buf.b, 0xC), 18, "4096/218");
}

int main(void)
{
    t_triangles();
    t_math();
    t_colour();
    t_memory();
    t_structs();
    t_text();
    t_cd();
    t_pad();
    t_prims();
    t_gpu_env();
    t_records();
    return lk_done("leaf a1");
}
