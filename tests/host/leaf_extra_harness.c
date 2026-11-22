/* Leaf-function suite extra: 60 additional hardware-free, zero-global
 * single-object leaf functions not covered by leaf_a1, leaf_a2, or any
 * other host test script.
 *
 * Conventions match leaf_a1_harness.c:
 *   - Each unit is the unchanged decomp C in src/game/, linked into one
 *     MIPS binary and executed under qemu-mipsel.
 *   - Every tested function has its own standalone test_func_XXXXXXXX(void),
 *     an @leaf marker (single source of truth for tools/host_leaf_suite.py),
 *     and an LK_FUNC("func_XXXXXXXX") attribution block.
 *   - Evidence classes: spec, math, libc, semantic, source.
 *   - No target-code mocks, no retail data, no src/game edits.
 */
#include "leaf_kit.h"

#define W32(base, off) (*(u32 *)((u8 *)(base) + (off)))
#define S32(base, off) (*(s32 *)((u8 *)(base) + (off)))
#define H16(base, off) (*(u16 *)((u8 *)(base) + (off)))
#define S16(base, off) (*(s16 *)((u8 *)(base) + (off)))
#define B8(base, off)  (*(u8  *)((u8 *)(base) + (off)))
#define S8(base, off)  (*(s8  *)((u8 *)(base) + (off)))

/* ---------------------------------------------------------------- prototypes
 * Copied argument-for-argument from each unit's __asm__ declaration. */
u32 gComputeU32Reciprocal(u32 divisor) __asm__("func_80018818");
int gApproximateAngle256(int x, int y) __asm__("func_80018580");
s32 gCountTrailingZeros(u32 value) __asm__("func_800195F0");
s32 gApproxVectorLength(s32 *vector) __asm__("func_80019F54");
s32 gApproximateLength2(s32 *vector) __asm__("func_8001A074");
s32 gApproximateDistance3(s32 *a, s32 *b) __asm__("func_8001A4E8");
s32 gApproximateLengthS16Scaled16(s16 *vector) __asm__("func_8001AF08");
void gMultiplyMatrixVectorWithPreshift(s32 *matrix, s32 *vector, s32 *out) __asm__("func_8001AE14");
void gRotateS16PointArray(void *matrix, void *source, void *destination, s32 count) __asm__("func_8001B148");
void gTransformS16PointArray(void *matrix, void *source, void *destination, s32 count) __asm__("func_8001AF84");
s32 gEaseAngleShortest(s32 current, s32 target, s32 steps) __asm__("func_8005605C");

u8 *gCopyMemory(u8 *destination, u8 *source, u32 count) __asm__("func_8001E318");
u8 *gMoveMemory(u8 *destination, u8 *source, s32 count) __asm__("func_8001E44C");
void gExpand15To16Padded(u8 *dst, u8 *src, u32 count) __asm__("func_8008731C");
unsigned short gBuildSubcellMask(int x, int y, int left, int right, int top, int bottom) __asm__("func_80083764");
u32 gHashCheatCode(u32 state) __asm__("func_80060DD8");

void *gCdIntToPos(s32 sector, void *location) __asm__("func_8008E61C");
u32 gByteOffsetToCdlLoc(void *loc, u32 value) __asm__("func_8001C494");
void gAddPrim(unsigned int *a0, unsigned int *a1) __asm__("func_8009235C");
void gSetDrawModePacket(void *packet, int dfe, int dtd, u32 tpage) __asm__("func_80092634");
void gBuildVramTransferPkt(void *p, void *r) __asm__("func_800926C0");
void *gSetDefDispEnv(void *state, s16 x, s16 y, s16 width, s16 height) __asm__("func_800921F4");

void *gPopEmitterFreeNode(void *owner) __asm__("func_80042764");
void gPushEmitterActiveNode(void *owner, void *node) __asm__("func_80042788");
void gRecycleEmitterNode(void *owner, void *previous, void *node) __asm__("func_800427A8");
void gReleasePoolNode(void *pool, void *previous, void *node) __asm__("func_8003BF1C");
void *gFindActivePartEffectByOwner(void *pool, void *target) __asm__("func_8005CB80");
void **gGetObjectTransform(void *parent) __asm__("func_8003D9C4");
void gDispatchSpawnSlotTicks(void *node, s32 arg) __asm__("func_8003F428");

void *gFindVertexRangeEntry(void *entry, s32 key) __asm__("func_8003A90C");
s32 gFindConfigValue(void *table, s32 key, s32 *value, s32 *index) __asm__("func_80021740");
s32 gFindStreamOption(void *table, s32 key, s32 *value, s32 *index) __asm__("func_80022B84");
void *gFindRecordPayloadById(s32 id, u8 *base, s32 size) __asm__("func_80019A20");
u8 *gFindSectionHeader(u8 *arg0, u32 arg1, u32 arg2, s32 arg3) __asm__("func_80019CA0");
u8 *gFindSectionPayload(u8 *arg0, u32 arg1, u32 arg2, s32 arg3) __asm__("func_80019C38");
u32 *gParseTextureContainer(u8 *data, s32 size) __asm__("func_8001996C");
s32 gInsertSweepHitTarget(void *set, s32 value) __asm__("func_8005C2C8");

void gDrainObjectLightColor(void *object, s32 amount) __asm__("func_80040550");
void gDrainParticleColor(void *object, s32 amount) __asm__("func_80042864");
void gStepObjectLightColor(void *s, int x, int y, int z) __asm__("func_80040600");
void gStepParticleColor(u8 *a, u8 *b, int x, int y, int z) __asm__("func_800428FC");
void gInitObjectLightColorSteps(void *p) __asm__("func_800403D0");
void gComputeEmitterColorRates(void *p) __asm__("func_80043250");
void gInitGlowColorChannelSlopes(void *p) __asm__("func_8006F898");
void gInitDomeEffectFadeRate(void *self) __asm__("func_8006E720");
void gBuildDomeFaceIndices(u8 *faces, s16 rows, int cols, s16 count) __asm__("func_8006C768");

void gAddCameraShake(void *object, s32 amount) __asm__("func_80057358");
void gInitHudGauge(void *gauge, s32 value, s32 divisor) __asm__("func_8005A0C4");
void gSetPlayerCrouchBounds(void *state, int compact) __asm__("func_80051AC8");
s32 gComputeForceImpulse(void *source, void *force, s32 *out) __asm__("func_8004B244");
s32 gCalcPadTableSize(void *shape) __asm__("func_8008B05C");
void gCmdPadModeSelect(void *port) __asm__("func_8008B668");
void gResetPadPortState(void *s) __asm__("func_8008BF10");
void gBindSoundVoiceHandle(void *handle, void *resource) __asm__("func_8004749C");
void gInitMenuDescriptor(void *state, s32 arg1, u8 arg2) __asm__("func_80077884");
void gInitModelInstance(void *instance, void *model, u32 options) __asm__("func_80013AF0");
void gSetModelFrame(void *instance, s32 frame) __asm__("func_800175F8");
void gInitCardReplacementFrames(void *records) __asm__("func_80020B28");
s32 gGetEntitySpawnBank(void *object) __asm__("func_8003BF50");
s32 gHandleObjectEventMessages(s32 message, void *object) __asm__("func_8003C980");

/* Shared word-aligned scratch buffers. */
static union { u32 w[256]; u16 h[512]; u8 b[1024]; s16 s[512]; } sb1, sb2, sb3;

/* =================================================== 1. math & fixed-point */

static void test_func_80018818(void)
{
    /* @leaf 80018818 math unsigned restoring reciprocal: floor((2^32 - 1) / d) */
    LK_FUNC("func_80018818");
    FCK_EQ((s32)gComputeU32Reciprocal(1u), (s32)0xFFFFFFFFu, "d=1");
    FCK_EQ((s32)gComputeU32Reciprocal(2u), (s32)0x7FFFFFFFu, "d=2");
    FCK_EQ((s32)gComputeU32Reciprocal(3u), (s32)0x55555555u, "d=3");
    FCK_EQ((s32)gComputeU32Reciprocal(4u), (s32)0x3FFFFFFFu, "d=4");
    FCK_EQ((s32)gComputeU32Reciprocal(5u), (s32)0x33333333u, "d=5");
    FCK_EQ((s32)gComputeU32Reciprocal(10u), (s32)0x19999999u, "d=10");
    FCK_EQ((s32)gComputeU32Reciprocal(100u), (s32)0x028F5C28u, "d=100");
    FCK_EQ((s32)gComputeU32Reciprocal(0x10000u), (s32)0x0000FFFFu, "d=65536");
    FCK_EQ((s32)gComputeU32Reciprocal(0x7FFFFFFFu), 2, "d=2^31-1");
    FCK_EQ((s32)gComputeU32Reciprocal(0x80000000u), 1, "d=2^31");
    FCK_EQ((s32)gComputeU32Reciprocal(0xFFFFFFFFu), 1, "d=2^32-1");
}

static void test_func_80018580(void)
{
    /* @leaf 80018580 math octant piecewise-linear atan2 on 256-unit circle [-128, 128] */
    LK_FUNC("func_80018580");
    FCK_EQ(gApproximateAngle256(0, 0), 0, "origin");
    FCK_EQ(gApproximateAngle256(100, 0), 0, "+x axis");
    FCK_EQ(gApproximateAngle256(0, 100), 64, "+y axis");
    FCK_EQ(gApproximateAngle256(-100, 0), 128, "-x axis");
    FCK_EQ(gApproximateAngle256(0, -100), -64, "-y axis");
    /* All 8 octants with |major|=100, |minor|=50 -> offset 16 */
    FCK_EQ(gApproximateAngle256(100, 50), 16, "octant 0 (+x, +y, x>=y)");
    FCK_EQ(gApproximateAngle256(50, 100), 48, "octant 4 (+x, +y, x<y)");
    FCK_EQ(gApproximateAngle256(-50, 100), 80, "octant 5 (-x, +y, |x|<y)");
    FCK_EQ(gApproximateAngle256(-100, 50), 112, "octant 1 (-x, +y, |x|>=y)");
    FCK_EQ(gApproximateAngle256(-100, -50), -112, "octant 3 (-x, -y, |x|>=|y|)");
    FCK_EQ(gApproximateAngle256(-50, -100), -80, "octant 7 (-x, -y, |x|<|y|)");
    FCK_EQ(gApproximateAngle256(50, -100), -48, "octant 6 (+x, -y, x<|y|)");
    FCK_EQ(gApproximateAngle256(100, -50), -16, "octant 2 (+x, -y, x>=|y|)");
    FCK_EQ(gApproximateAngle256(100, 100), 32, "45 deg diagonal");
}

static void test_func_800195F0(void)
{
    int k;
    /* @leaf 800195F0 math binary-search count trailing zeros (0 -> 32, 1<<k -> k) */
    LK_FUNC("func_800195F0");
    FCK_EQ(gCountTrailingZeros(0u), 32, "ctz(0)=32");
    for (k = 0; k < 32; k++) {
        FCK_EQ(gCountTrailingZeros(1u << k), k, "ctz(1<<k)=k");
    }
    FCK_EQ(gCountTrailingZeros(0xFFFFFFFFu), 0, "all ones");
    FCK_EQ(gCountTrailingZeros(0xFFFF0000u), 16, "upper half");
    FCK_EQ(gCountTrailingZeros(0xF0F0F000u), 12, "mixed bits");
}

static void test_func_80019F54(void)
{
    s32 v[3];
    /* @leaf 80019F54 math 3D octant norm approx: max(|x|,|y|,|z|) + ((mid+min) >> 2) */
    LK_FUNC("func_80019F54");
    v[0] = 0; v[1] = 0; v[2] = 0;
    FCK_EQ(gApproxVectorLength(v), 0, "zero");
    v[0] = 100; v[1] = 0; v[2] = 0;
    FCK_EQ(gApproxVectorLength(v), 100, "x axis");
    v[0] = 0; v[1] = -80; v[2] = 0;
    FCK_EQ(gApproxVectorLength(v), 80, "-y axis");
    v[0] = 12; v[1] = -20; v[2] = 100;
    FCK_EQ(gApproxVectorLength(v), 108, "z largest: 100 + (32>>2)");
    v[0] = -40; v[1] = -40; v[2] = -40;
    FCK_EQ(gApproxVectorLength(v), 60, "equal neg: 40 + (80>>2)");
    v[0] = 1; v[1] = 2; v[2] = 3;
    FCK_EQ(gApproxVectorLength(v), 3, "small truncation: 3 + (3>>2)");
}

static void test_func_8001A074(void)
{
    s32 v[2];
    /* @leaf 8001A074 math 2D two-piece linear Euclidean length approximation */
    LK_FUNC("func_8001A074");
    v[0] = 0; v[1] = 0; FCK_EQ(gApproximateLength2(v), 0, "zero");
    v[0] = 100; v[1] = 0; FCK_EQ(gApproximateLength2(v), 100, "+x axis");
    v[0] = 0; v[1] = -100; FCK_EQ(gApproximateLength2(v), 100, "-y axis");
    v[0] = 100; v[1] = 50; FCK_EQ(gApproximateLength2(v), 112, "boundary b == a>>1");
    v[0] = 100; v[1] = 51; FCK_EQ(gApproximateLength2(v), 112, "boundary b > a>>1");
    v[0] = 100; v[1] = 100; FCK_EQ(gApproximateLength2(v), 141, "diagonal 100,100");
    v[0] = -300; v[1] = 400; FCK_EQ(gApproximateLength2(v), 506, "3-4-5 triangle");
    v[0] = 1000; v[1] = 1000; FCK_EQ(gApproximateLength2(v), 1414, "diagonal 1000,1000");
}

static void test_func_8001A4E8(void)
{
    s32 a[3], b[3];
    /* @leaf 8001A4E8 math 3D distance approx max(|da|,|db|,|dc|) + ((mid+min) >> 2) */
    LK_FUNC("func_8001A4E8");
    a[0] = 10; a[1] = -20; a[2] = 30;
    b[0] = 10; b[1] = -20; b[2] = 30;
    FCK_EQ(gApproximateDistance3(a, b), 0, "identical points");
    b[0] = 110; b[1] = -20; b[2] = 30;
    FCK_EQ(gApproximateDistance3(a, b), 100, "dx=100");
    a[0] = 0; a[1] = 0; a[2] = 0;
    b[0] = 12; b[1] = -100; b[2] = 20;
    FCK_EQ(gApproximateDistance3(a, b), 108, "y largest");
    FCK_EQ(gApproximateDistance3(b, a), 108, "symmetric");
    a[0] = -50; a[1] = 10; a[2] = 40;
    b[0] = 50; b[1] = 10; b[2] = -60;
    FCK_EQ(gApproximateDistance3(a, b), 125, "dx=100,dz=100 -> 100+25");
}

static void test_func_8001AF08(void)
{
    s16 v[3];
    /* @leaf 8001AF08 math s16 3D length scaled by 16: (max<<4) + ((mid+min)<<2) */
    LK_FUNC("func_8001AF08");
    v[0] = 0; v[1] = 0; v[2] = 0;
    FCK_EQ(gApproximateLengthS16Scaled16(v), 0, "zero");
    v[0] = 10; v[1] = 0; v[2] = 0;
    FCK_EQ(gApproximateLengthS16Scaled16(v), 160, "x=10 -> 160");
    v[0] = -3; v[1] = 5; v[2] = -20;
    FCK_EQ(gApproximateLengthS16Scaled16(v), 352, "20*16 + (5+3)*4");
    v[0] = -100; v[1] = -100; v[2] = -100;
    FCK_EQ(gApproximateLengthS16Scaled16(v), 2400, "equal neg");
}

static void test_func_8001AE14(void)
{
    s32 m[9], v[3], out[4];
    int i;
    /* @leaf 8001AE14 math 3x3 Q12 matrix-vector multiply with >>2 preshift */
    LK_FUNC("func_8001AE14");
    /* Identity in Q16 (so >>2 gives Q14; vector >>2 * Q14 >>12 = vector) -> scale 0x10000 */
    for (i = 0; i < 9; i++) m[i] = 0;
    m[0] = 0x10000; m[4] = 0x10000; m[8] = 0x10000;
    v[0] = 400; v[1] = -800; v[2] = 1200;
    out[3] = 0x5A5A5A5A;
    gMultiplyMatrixVectorWithPreshift(m, v, out);
    FCK_EQ(out[0], 400, "id x");
    FCK_EQ(out[1], -800, "id y");
    FCK_EQ(out[2], 1200, "id z");
    FCK_EQ(out[3], 0x5A5A5A5A, "canary untouched");
    /* Row-major permutation + sign flip: out = (y, -z, 2*x) */
    for (i = 0; i < 9; i++) m[i] = 0;
    m[1] = 0x10000; m[5] = -0x10000; m[6] = 0x20000;
    gMultiplyMatrixVectorWithPreshift(m, v, out);
    FCK_EQ(out[0], -800, "perm x=y");
    FCK_EQ(out[1], -1200, "perm y=-z");
    FCK_EQ(out[2], 800, "perm z=2x");
}

static void test_func_8001B148(void)
{
    s32 m[9];
    s16 src[8], dst[8];
    int i;
    /* @leaf 8001B148 math batch 3x3 column-dot rotation of s16 points (matrix>>4, >>12) */
    LK_FUNC("func_8001B148");
    for (i = 0; i < 9; i++) m[i] = 0;
    /* Q16 identity: >>4 gives 0x1000 (4096) */
    m[0] = 0x10000; m[4] = 0x10000; m[8] = 0x10000;
    src[0] = 10; src[1] = -20; src[2] = 30; src[3] = 0;
    src[4] = -5; src[5] = 15; src[6] = -25; src[7] = 0;
    for (i = 0; i < 8; i++) dst[i] = 0x1234;
    /* count <= 0: no-op */
    gRotateS16PointArray(m, src, dst, 0);
    FCK_EQ(dst[0], 0x1234, "count=0 untouched");
    gRotateS16PointArray(m, src, dst, -3);
    FCK_EQ(dst[0], 0x1234, "count<0 untouched");
    /* identity on 2 points */
    gRotateS16PointArray(m, src, dst, 2);
    FCK_EQ(dst[0], 10, "p0.x"); FCK_EQ(dst[1], -20, "p0.y"); FCK_EQ(dst[2], 30, "p0.z");
    FCK_EQ(dst[3], 0x1234, "p0.pad untouched");
    FCK_EQ(dst[4], -5, "p1.x"); FCK_EQ(dst[5], 15, "p1.y"); FCK_EQ(dst[6], -25, "p1.z");
    /* 90-deg Z rotation (x'=y, y'=-x, z'=z): m3=1, m1=-1, m8=1 */
    for (i = 0; i < 9; i++) m[i] = 0;
    m[3] = 0x10000; m[1] = -0x10000; m[8] = 0x10000;
    gRotateS16PointArray(m, src, dst, 1);
    FCK_EQ(dst[0], -20, "rot90 x=y"); FCK_EQ(dst[1], -10, "rot90 y=-x"); FCK_EQ(dst[2], 30, "rot90 z=z");
}

static void test_func_8001AF84(void)
{
    s32 m[12];
    s16 src[8], dst[8];
    int i;
    /* @leaf 8001AF84 math batch 3x3 affine transform of s16 points (rot + trans>>4) */
    LK_FUNC("func_8001AF84");
    for (i = 0; i < 12; i++) m[i] = 0;
    m[0] = 0x10000; m[4] = 0x10000; m[8] = 0x10000;
    m[9] = 100 << 4; m[10] = -50 << 4; m[11] = 25 << 4;
    src[0] = 10; src[1] = 20; src[2] = -5; src[3] = 0;
    src[4] = -100; src[5] = 50; src[6] = -25; src[7] = 0;
    for (i = 0; i < 8; i++) dst[i] = 0x2345;
    gTransformS16PointArray(m, src, dst, 0);
    FCK_EQ(dst[0], 0x2345, "count=0 untouched");
    gTransformS16PointArray(m, src, dst, 2);
    FCK_EQ(dst[0], 110, "p0.x+tx"); FCK_EQ(dst[1], -30, "p0.y+ty"); FCK_EQ(dst[2], 20, "p0.z+tz");
    FCK_EQ(dst[3], 0x2345, "p0.pad untouched");
    FCK_EQ(dst[4], 0, "p1.x cancels tx"); FCK_EQ(dst[5], 0, "p1.y cancels ty"); FCK_EQ(dst[6], 0, "p1.z cancels tz");
}

static void test_func_8005605C(void)
{
    /* @leaf 8005605C semantic 4096-unit shortest-arc angular easing (31/32 + stall step) */
    LK_FUNC("func_8005605C");
    FCK_EQ(gEaseAngleShortest(0, 0, 5), 0, "already at target");
    FCK_EQ(gEaseAngleShortest(0, 320, 1), 10, "1 step of 320/32");
    FCK_EQ(gEaseAngleShortest(0, 320, 2), 19, "2 steps: (310+320)/32=19");
    FCK_EQ(gEaseAngleShortest(0, 10, 1), 1, "stall breaker +1");
    FCK_EQ(gEaseAngleShortest(0, 10, 20), 10, "stall breaker clamped to diff");
    FCK_EQ(gEaseAngleShortest(10, 0, 1), 9, "stall breaker -1");
    FCK_EQ(gEaseAngleShortest(10, 0, 20), 0, "stall breaker neg clamped");
    FCK_EQ(gEaseAngleShortest(0xF00, 0x100, 1), -240, "wrap across 0: -256 -> +256");
    FCK_EQ(gEaseAngleShortest(0x100, 0xF00, 1), 240, "wrap neg across 0: +256 -> -256");
    FCK_EQ(gEaseAngleShortest(0x100, 0x200, 0), 256, "0 steps stays put");
}

/* ============================================== 2. memory & data conversion */

static void test_func_8001E318(void)
{
    u8 *ret;
    int i;
    /* @leaf 8001E318 libc memcpy with 32-byte word-unrolled, word, halfword, byte paths */
    LK_FUNC("func_8001E318");
    for (i = 0; i < 64; i++) sb1.b[i] = (u8)(0x10 + i);
    /* 32-byte unrolled + word tail (40 bytes, 4-byte aligned) */
    lk_fill(sb2.b, 0xA5, 64);
    ret = gCopyMemory(sb2.b, sb1.b, 40);
    FCK_EQ((s32)ret, (s32)sb2.b, "returns dst (word path)");
    FCK_EQ(memcmp(sb2.b, sb1.b, 40), 0, "40B word copy exact");
    FCK_CHECK(lk_all(sb2.b, 40, 64, 0xA5), "tail untouched");
    /* Halfword path (2-byte aligned, count=10) */
    lk_fill(sb2.b, 0xA5, 64);
    ret = gCopyMemory(sb2.b + 2, sb1.b + 2, 10);
    FCK_EQ((s32)ret, (s32)(sb2.b + 2), "returns dst (halfword)");
    FCK_EQ(memcmp(sb2.b + 2, sb1.b + 2, 10), 0, "10B halfword copy");
    FCK_CHECK(lk_all(sb2.b, 0, 2, 0xA5) && lk_all(sb2.b, 12, 64, 0xA5), "halfword guards");
    /* Byte path (unaligned, odd count=7) */
    lk_fill(sb2.b, 0xA5, 64);
    ret = gCopyMemory(sb2.b + 1, sb1.b + 3, 7);
    FCK_EQ((s32)ret, (s32)(sb2.b + 1), "returns dst (byte)");
    FCK_EQ(memcmp(sb2.b + 1, sb1.b + 3, 7), 0, "7B byte copy");
    FCK_CHECK(sb2.b[0] == 0xA5 && sb2.b[8] == 0xA5, "byte guards");
    /* Zero count */
    lk_fill(sb2.b, 0xA5, 16);
    ret = gCopyMemory(sb2.b, sb1.b, 0);
    FCK_EQ((s32)ret, (s32)sb2.b, "0B returns dst");
    FCK_CHECK(lk_all(sb2.b, 0, 16, 0xA5), "0B untouched");
}

static void test_func_8001E44C(void)
{
    u8 *ret;
    int i;
    /* @leaf 8001E44C libc memmove supporting forward and overlapping backward shifts */
    LK_FUNC("func_8001E44C");
    for (i = 0; i < 32; i++) sb1.b[i] = (u8)(0x30 + i);
    /* Forward overlap: dst < src */
    ret = gMoveMemory(sb1.b, sb1.b + 4, 12);
    FCK_EQ((s32)ret, (s32)sb1.b, "fwd returns dst");
    FCK_EQ(sb1.b[0], 0x34, "fwd b[0]");
    FCK_EQ(sb1.b[11], 0x3F, "fwd b[11]");
    /* Backward overlap: src < dst < src + count */
    for (i = 0; i < 32; i++) sb1.b[i] = (u8)(0x40 + i);
    ret = gMoveMemory(sb1.b + 5, sb1.b, 10);
    FCK_EQ((s32)ret, (s32)(sb1.b + 5), "bwd returns dst");
    FCK_EQ(sb1.b[5], 0x40, "bwd start");
    FCK_EQ(sb1.b[14], 0x49, "bwd end");
    FCK_EQ(sb1.b[15], 0x4F, "bwd guard");
    /* Zero count */
    ret = gMoveMemory(sb1.b + 2, sb1.b, 0);
    FCK_EQ((s32)ret, (s32)(sb1.b + 2), "zero count");
    FCK_EQ(sb1.b[2], 0x42, "zero untouched");
}

static void test_func_8008731C(void)
{
    int i;
    /* @leaf 8008731C semantic expands 15-byte chunks to 16 bytes inserting 0 at byte 1 */
    LK_FUNC("func_8008731C");
    for (i = 0; i < 30; i++) sb1.b[i] = (u8)(1 + i);
    lk_fill(sb2.b, 0xCC, 48);
    gExpand15To16Padded(sb2.b, sb1.b, 0);
    FCK_CHECK(lk_all(sb2.b, 0, 16, 0xCC), "count=0 no-op");
    gExpand15To16Padded(sb2.b, sb1.b, 30);
    /* Chunk 0 */
    FCK_EQ(sb2.b[0], 1, "c0[0]");
    FCK_EQ(sb2.b[1], 0, "c0[1] inserted 0");
    FCK_EQ(sb2.b[2], 2, "c0[2]");
    FCK_EQ(sb2.b[15], 15, "c0[15]");
    /* Chunk 1 */
    FCK_EQ(sb2.b[16], 16, "c1[0]");
    FCK_EQ(sb2.b[17], 0, "c1[1] inserted 0");
    FCK_EQ(sb2.b[18], 17, "c1[2]");
    FCK_EQ(sb2.b[31], 30, "c1[15]");
    FCK_CHECK(lk_all(sb2.b, 32, 48, 0xCC), "tail untouched");
}

static void test_func_80083764(void)
{
    /* @leaf 80083764 semantic 4x4 subcell bitmask (MSB=cell(0,0), 2^17 tile, 2^15 subcell) */
    LK_FUNC("func_80083764");
    /* Full tile when bounds lie outside tile (x=0, y=0) */
    FCK_EQ(gBuildSubcellMask(0, 0, -1, 1 << 17, -1, 1 << 17), 0xFFFF, "full 4x4");
    /* Single subcell (0,0) -> bit 15 = 0x8000 */
    FCK_EQ(gBuildSubcellMask(0, 0, 0, 0, 0, 0), 0x8000, "top-left subcell");
    /* Single subcell (3,3) -> bit 0 = 0x0001 */
    FCK_EQ(gBuildSubcellMask(0, 0, 3 << 15, 3 << 15, 3 << 15, 3 << 15), 0x0001, "bottom-right");
    /* Center 2x2: cols 1..2, rows 1..2 -> rows 1,2 have bits 0110 -> 0x0660 */
    FCK_EQ(gBuildSubcellMask(0, 0, 1 << 15, 2 << 15, 1 << 15, 2 << 15), 0x0660, "center 2x2");
    /* Top row only (row 0, cols 0..3) -> 0xF000 */
    FCK_EQ(gBuildSubcellMask(0, 0, 0, 3 << 15, 0, 0), 0xF000, "top row");
    /* Inverted left > right within same tile -> empty mask 0 */
    FCK_EQ(gBuildSubcellMask(0, 0, 3 << 15, 1 << 15, 0, 3 << 15), 0x0000, "empty inverted");
    /* Non-zero tile index (tile x=2, y=1) */
    FCK_EQ(gBuildSubcellMask(2 << 17, 1 << 17, (2 << 17) + (1 << 15), (2 << 17) + (1 << 15),
                             (1 << 17) + (2 << 15), (1 << 17) + (2 << 15)),
           0x0040, "tile(2,1) subcell(1,2) = bit 6");
}

static void test_func_80060DD8(void)
{
    /* @leaf 80060DD8 source 10-round GF(2) linear bit-mixer for cheat code hashing */
    LK_FUNC("func_80060DD8");
    FCK_EQ((s32)gHashCheatCode(0u), 0, "zero fixed point");
    FCK_EQ((s32)gHashCheatCode(1u), 0x00000400, "bit 0 shifts 10");
    FCK_EQ((s32)gHashCheatCode(0x40u), 0x00012941, "tap activation");
    FCK_EQ((s32)gHashCheatCode(0x1234u), 0x0055CE15, "known vector 0x1234");
    FCK_EQ((s32)gHashCheatCode(0xFFFFFFFFu), (s32)0xFE7C92FCu, "all ones");
    /* Linearity over XOR: H(a ^ b) == H(a) ^ H(b) */
    FCK_EQ((s32)gHashCheatCode(0x1234u ^ 0x40u),
           (s32)(gHashCheatCode(0x1234u) ^ gHashCheatCode(0x40u)), "GF(2) linear");
}

/* ================================================= 3. CD-ROM & PS1 GPU env */

static void test_func_8008E61C(void)
{
    u8 loc[4];
    void *ret;
    /* @leaf 8008E61C spec Psy-Q CdIntToPos: (sector+150) -> BCD MSF, track untouched */
    LK_FUNC("func_8008E61C");
    loc[0] = loc[1] = loc[2] = 0xAA; loc[3] = 0x55;
    ret = gCdIntToPos(0, loc);
    FCK_EQ((s32)ret, (s32)loc, "returns loc");
    FCK_EQ(loc[0], 0x00, "LBA 0 -> 00:02:00 min");
    FCK_EQ(loc[1], 0x02, "LBA 0 -> 00:02:00 sec");
    FCK_EQ(loc[2], 0x00, "LBA 0 -> 00:02:00 frm");
    FCK_EQ(loc[3], 0x55, "track untouched");
    /* LBA 4350 -> +150 = 4500 = 1m 0s 0f */
    gCdIntToPos(4350, loc);
    FCK_EQ(loc[0], 0x01, "1m"); FCK_EQ(loc[1], 0x00, "0s"); FCK_EQ(loc[2], 0x00, "0f");
    /* LBA 54958 -> +150 = 55108 = 12*4500 + 14*75 + 58 -> 12:14:58 BCD */
    gCdIntToPos(54958, loc);
    FCK_EQ(loc[0], 0x12, "12m BCD"); FCK_EQ(loc[1], 0x14, "14s BCD"); FCK_EQ(loc[2], 0x58, "58f BCD");
}

static void test_func_8001C494(void)
{
    u8 loc[4];
    u32 rem;
    /* @leaf 8001C494 spec 2048B-sector byte offset to BCD MSF (no pregap), returns offset&0x7FF */
    LK_FUNC("func_8001C494");
    loc[3] = 0x77;
    rem = gByteOffsetToCdlLoc(loc, 0);
    FCK_EQ((s32)rem, 0, "offset 0 rem");
    FCK_EQ(loc[0], 0, "0m"); FCK_EQ(loc[1], 0, "0s"); FCK_EQ(loc[2], 0, "0f"); FCK_EQ(loc[3], 0, "track=0");
    /* 55108 sectors * 2048 + 0x345 = 0x06BA2345 -> 12:14:58 BCD, rem 0x345 */
    loc[3] = 0x99;
    rem = gByteOffsetToCdlLoc(loc, (55108u << 11) | 0x345u);
    FCK_EQ((s32)rem, 0x345, "sub-sector offset");
    FCK_EQ(loc[0], 0x12, "12m"); FCK_EQ(loc[1], 0x14, "14s"); FCK_EQ(loc[2], 0x58, "58f"); FCK_EQ(loc[3], 0, "track cleared");
}

static void test_func_8009235C(void)
{
    u32 ot, p1, p2;
    /* @leaf 8009235C spec PSn00bSDK/Psy-Q addPrim: links prim at OT head preserving length bytes */
    LK_FUNC("func_8009235C");
    ot = 0xAAFFFFFFu;
    p1 = 0x04000000u;
    p2 = 0x06123456u;
    gAddPrim(&ot, &p1);
    FCK_EQ((s32)p1, (s32)0x04FFFFFFu, "p1 gets old ot link, keeps len 4");
    FCK_EQ((s32)ot, (s32)(0xAA000000u | ((u32)&p1 & 0x00FFFFFFu)), "ot points to p1, keeps top byte");
    gAddPrim(&ot, &p2);
    FCK_EQ((s32)p2, (s32)(0x06000000u | ((u32)&p1 & 0x00FFFFFFu)), "p2 links to p1, keeps len 6");
    FCK_EQ((s32)ot, (s32)(0xAA000000u | ((u32)&p2 & 0x00FFFFFFu)), "ot points to p2");
}

static void test_func_80092634(void)
{
    /* @leaf 80092634 spec psx-spx GP0(E1h) DrawMode packet: len=1, dtd=bit9, dfe=bit10, tpage&0x9FF */
    LK_FUNC("func_80092634");
    lk_fill(sb1.b, 0xA5, 16);
    gSetDrawModePacket(sb1.b, 0, 0, 0);
    FCK_EQ(sb1.b[3], 1, "len=1");
    FCK_EQ((s32)W32(sb1.b, 4), (s32)0xE1000000u, "all zero");
    FCK_CHECK(lk_all(sb1.b, 0, 3, 0xA5), "tag addr untouched");
    gSetDrawModePacket(sb1.b, 1, 0, 0);
    FCK_EQ((s32)W32(sb1.b, 4), (s32)0xE1000400u, "dfe sets bit 10");
    gSetDrawModePacket(sb1.b, 0, 1, 0);
    FCK_EQ((s32)W32(sb1.b, 4), (s32)0xE1000200u, "dtd sets bit 9");
    gSetDrawModePacket(sb1.b, 1, 1, 0xFFFFFFFFu);
    FCK_EQ((s32)W32(sb1.b, 4), (s32)0xE1000FFFu, "mask 0x9FF | dfe | dtd = 0xFFF");
}

static void test_func_800926C0(void)
{
    s16 rect[4];
    /* @leaf 800926C0 spec psx-spx GP0(A0h) VRAM transfer packet with GP0(01h) terminator */
    LK_FUNC("func_800926C0");
    /* Small rect: 4x3 = 12 pixels -> 6 words -> len = 10, terminator at word 10 (offset 40) */
    lk_fill(sb1.b, 0xA5, 64);
    rect[0] = 64; rect[1] = 128; rect[2] = 4; rect[3] = 3;
    gBuildVramTransferPkt(sb1.b, rect);
    FCK_EQ(sb1.b[3], 10, "4x3 -> len=10");
    FCK_EQ((s32)W32(sb1.b, 4), (s32)0xA0000000u, "GP0(A0h) cmd");
    FCK_EQ((s32)W32(sb1.b, 8), (s32)((128u << 16) | 64u), "xy packed");
    FCK_EQ((s32)W32(sb1.b, 12), (s32)((3u << 16) | 4u), "wh packed");
    FCK_EQ((s32)W32(sb1.b, 40), (s32)0x01000000u, "terminator at +40");
    /* Boundary: size=12 (24 pixels, e.g. 6x4) -> len=16 */
    lk_fill(sb1.b, 0xA5, 80);
    rect[2] = 6; rect[3] = 4;
    gBuildVramTransferPkt(sb1.b, rect);
    FCK_EQ(sb1.b[3], 16, "size=12 -> len=16");
    FCK_EQ((s32)W32(sb1.b, 64), (s32)0x01000000u, "terminator at +64");
    /* Overflow: size=13 (25 pixels, 5x5) -> len=0, word 0 overwritten with 0x01000000 */
    lk_fill(sb1.b, 0xA5, 32);
    rect[2] = 5; rect[3] = 5;
    gBuildVramTransferPkt(sb1.b, rect);
    FCK_EQ((s32)W32(sb1.b, 0), (s32)0x01000000u, "size>=13 -> len=0 writes tag 0x01000000");
}

static void test_func_800921F4(void)
{
    void *ret;
    /* @leaf 800921F4 spec Psy-Q SetDefDispEnv: sets x,y,w,h and zeroes remaining 12 bytes */
    LK_FUNC("func_800921F4");
    lk_fill(sb1.b, 0xA5, 32);
    ret = gSetDefDispEnv(sb1.b, 16, 240, 320, 240);
    FCK_EQ((s32)ret, (s32)sb1.b, "returns state");
    FCK_EQ(S16(sb1.b, 0), 16, "x");
    FCK_EQ(S16(sb1.b, 2), 240, "y");
    FCK_EQ(S16(sb1.b, 4), 320, "w");
    FCK_EQ(S16(sb1.b, 6), 240, "h");
    FCK_CHECK(lk_all(sb1.b, 8, 20, 0), "12 zeroed bytes");
    FCK_CHECK(lk_all(sb1.b, 20, 32, 0xA5), "tail untouched");
}

/* ============================================ 4. pools, lists & dispatch */

static void test_func_80042764(void)
{
    void *n1[2], *n2[2];
    /* @leaf 80042764 semantic pops head of emitter free list (owner+0x64) */
    LK_FUNC("func_80042764");
    lk_fill(sb1.b, 0, 0xB0);
    FCK_EQ((s32)gPopEmitterFreeNode(sb1.b), 0, "empty -> NULL");
    n1[0] = n2; n2[0] = 0;
    W32(sb1.b, 0x64) = (u32)n1;
    FCK_EQ((s32)gPopEmitterFreeNode(sb1.b), (s32)n1, "pop n1");
    FCK_EQ((s32)W32(sb1.b, 0x64), (s32)n2, "head is now n2");
    FCK_EQ((s32)gPopEmitterFreeNode(sb1.b), (s32)n2, "pop n2");
    FCK_EQ((s32)W32(sb1.b, 0x64), 0, "head is now NULL");
    FCK_EQ((s32)gPopEmitterFreeNode(sb1.b), 0, "empty again");
}

static void test_func_80042788(void)
{
    void *n1[2], *n2[2];
    /* @leaf 80042788 semantic pushes node onto emitter active list (+0x68) and ++count (+0xA4) */
    LK_FUNC("func_80042788");
    lk_fill(sb1.b, 0, 0xB0);
    gPushEmitterActiveNode(sb1.b, n1);
    FCK_EQ((s32)W32(sb1.b, 0x68), (s32)n1, "active=n1");
    FCK_EQ((s32)n1[0], 0, "n1->next=0");
    FCK_EQ(H16(sb1.b, 0xA4), 1, "count=1");
    gPushEmitterActiveNode(sb1.b, n2);
    FCK_EQ((s32)W32(sb1.b, 0x68), (s32)n2, "active=n2");
    FCK_EQ((s32)n2[0], (s32)n1, "n2->next=n1");
    FCK_EQ(H16(sb1.b, 0xA4), 2, "count=2");
}

static void test_func_800427A8(void)
{
    void *n1[2], *n2[2], *n3[2];
    /* @leaf 800427A8 semantic unlinks active node (head or mid), pushes to free, --count */
    LK_FUNC("func_800427A8");
    lk_fill(sb1.b, 0, 0xB0);
    /* active: n1 -> n2 -> n3, free: 0, count: 3 */
    n1[0] = n2; n2[0] = n3; n3[0] = 0;
    W32(sb1.b, 0x68) = (u32)n1;
    H16(sb1.b, 0xA4) = 3;
    /* Unlink middle (prev=n1, node=n2) */
    gRecycleEmitterNode(sb1.b, n1, n2);
    FCK_EQ((s32)n1[0], (s32)n3, "n1 skips n2");
    FCK_EQ((s32)W32(sb1.b, 0x64), (s32)n2, "free=n2");
    FCK_EQ((s32)n2[0], 0, "n2->next=0");
    FCK_EQ(H16(sb1.b, 0xA4), 2, "count=2");
    /* Unlink head (prev=0, node=n1) */
    gRecycleEmitterNode(sb1.b, 0, n1);
    FCK_EQ((s32)W32(sb1.b, 0x68), (s32)n3, "active=n3");
    FCK_EQ((s32)W32(sb1.b, 0x64), (s32)n1, "free=n1");
    FCK_EQ((s32)n1[0], (s32)n2, "n1->next=n2");
    FCK_EQ(H16(sb1.b, 0xA4), 1, "count=1");
}

static void test_func_8003BF1C(void)
{
    void *pool[3];
    void *n1[1], *n2[1], *n3[1];
    /* @leaf 8003BF1C semantic unlinks active node (head or mid) from Pool and pushes to free */
    LK_FUNC("func_8003BF1C");
    n1[0] = n2; n2[0] = n3; n3[0] = 0;
    pool[0] = (void *)0xDEAD; pool[1] = n1; pool[2] = 0;
    gReleasePoolNode(pool, n1, n2);
    FCK_EQ((s32)n1[0], (s32)n3, "mid unlink");
    FCK_EQ((s32)pool[2], (s32)n2, "free=n2");
    FCK_EQ((s32)n2[0], 0, "n2->next=0");
    gReleasePoolNode(pool, 0, n1);
    FCK_EQ((s32)pool[1], (s32)n3, "head unlink");
    FCK_EQ((s32)pool[2], (s32)n1, "free=n1");
    FCK_EQ((s32)n1[0], (s32)n2, "n1->next=n2");
    FCK_EQ((s32)pool[0], 0xDEAD, "storage untouched");
}

static void test_func_8005CB80(void)
{
    void *pool[3];
    u8 n1[0x2C], n2[0x2C], n3[0x2C];
    /* @leaf 8005CB80 semantic finds active node with second==target and !(flags&2) */
    LK_FUNC("func_8005CB80");
    lk_fill(n1, 0, sizeof(n1));
    lk_fill(n2, 0, sizeof(n2));
    lk_fill(n3, 0, sizeof(n3));
    W32(n1, 0) = (u32)n2; W32(n1, 8) = 0x100; B8(n1, 0x28) = 0x02; /* flagged -> skip */
    W32(n2, 0) = (u32)n3; W32(n2, 8) = 0x200; B8(n2, 0x28) = 0x00;
    W32(n3, 0) = 0;       W32(n3, 8) = 0x100; B8(n3, 0x28) = 0x05; /* bit 1 clear -> match */
    pool[0] = 0; pool[1] = n1; pool[2] = 0;
    FCK_EQ((s32)gFindActivePartEffectByOwner(pool, (void *)0x100), (s32)n3, "skips flagged n1, finds n3");
    FCK_EQ((s32)gFindActivePartEffectByOwner(pool, (void *)0x200), (s32)n2, "finds n2");
    FCK_EQ((s32)gFindActivePartEffectByOwner(pool, (void *)0x300), 0, "missing -> NULL");
    pool[1] = 0;
    FCK_EQ((s32)gFindActivePartEffectByOwner(pool, (void *)0x100), 0, "empty list -> NULL");
}

static void test_func_8003D9C4(void)
{
    /* @leaf 8003D9C4 semantic transform pointer resolution: child+4 else node+4 else &parent->fallback */
    LK_FUNC("func_8003D9C4");
    lk_fill(sb1.b, 0, 0x40); /* parent */
    lk_fill(sb2.b, 0, 0x40); /* node */
    lk_fill(sb3.b, 0, 0x40); /* child */
    /* Case 1: node == NULL -> &parent->fallback (+0x34) */
    FCK_EQ((s32)gGetObjectTransform(sb1.b), (s32)(sb1.b + 0x34), "fallback");
    /* Case 2: node != NULL, node->child == NULL -> node + 4 */
    W32(sb1.b, 0x1C) = (u32)sb2.b;
    FCK_EQ((s32)gGetObjectTransform(sb1.b), (s32)(sb2.b + 4), "node+4");
    /* Case 3: node->child != NULL -> child + 4 */
    W32(sb2.b, 0x34) = (u32)sb3.b;
    FCK_EQ((s32)gGetObjectTransform(sb1.b), (s32)(sb3.b + 4), "child+4");
}

static int g_tick_calls;
static s32 g_tick_msg[4], g_tick_node[4], g_tick_arg2[4], g_tick_arg3[4];
static void test_tick_handler(s32 msg, void *node, s32 arg2, s32 arg3)
{
    if (g_tick_calls < 4) {
        g_tick_msg[g_tick_calls] = msg;
        g_tick_node[g_tick_calls] = (s32)node;
        g_tick_arg2[g_tick_calls] = arg2;
        g_tick_arg3[g_tick_calls] = arg3;
    }
    g_tick_calls++;
}

static void test_func_8003F428(void)
{
    u8 n1[0x24], n2[0x24], n3[0x24], n4[0x24];
    u8 def_ok[0x50], def_noops[0x50];
    u32 ops[2];
    /* @leaf 8003F428 semantic dispatches ops->handler(1, node, arg, 0) along linked list skipping NULL def/ops */
    LK_FUNC("func_8003F428");
    lk_fill(n1, 0, sizeof(n1)); lk_fill(n2, 0, sizeof(n2));
    lk_fill(n3, 0, sizeof(n3)); lk_fill(n4, 0, sizeof(n4));
    lk_fill(def_ok, 0, sizeof(def_ok)); lk_fill(def_noops, 0, sizeof(def_noops));
    ops[0] = 0; ops[1] = (u32)test_tick_handler;
    W32(def_ok, 0x48) = (u32)ops;
    W32(n1, 0) = (u32)n2; W32(n1, 0x1C) = (u32)def_ok;
    W32(n2, 0) = (u32)n3; W32(n2, 0x1C) = 0; /* NULL def */
    W32(n3, 0) = (u32)n4; W32(n3, 0x1C) = (u32)def_noops; /* NULL ops */
    W32(n4, 0) = 0;       W32(n4, 0x1C) = (u32)def_ok;
    g_tick_calls = 0;
    gDispatchSpawnSlotTicks(0, 42);
    FCK_EQ(g_tick_calls, 0, "NULL list -> 0 calls");
    gDispatchSpawnSlotTicks(n1, 777);
    FCK_EQ(g_tick_calls, 2, "2 valid nodes called");
    FCK_EQ(g_tick_msg[0], 1, "call0 msg=1");
    FCK_EQ(g_tick_node[0], (s32)n1, "call0 node=n1");
    FCK_EQ(g_tick_arg2[0], 777, "call0 arg2=777");
    FCK_EQ(g_tick_arg3[0], 0, "call0 arg3=0");
    FCK_EQ(g_tick_node[1], (s32)n4, "call1 node=n4");
}

/* ======================================== 5. tables, containers & sets */

static void test_func_8003A90C(void)
{
    u8 tbl[3 * 0x14];
    /* @leaf 8003A90C semantic scans 0x14-byte range entries [start, start+count), stops at flags&0x80 */
    LK_FUNC("func_8003A90C");
    lk_fill(tbl, 0, sizeof(tbl));
    /* Entry 0: [10, 15) */
    tbl[0 * 0x14 + 3] = 10; tbl[0 * 0x14 + 4] = 5; tbl[0 * 0x14 + 7] = 0x00;
    /* Entry 1: [20, 23) */
    tbl[1 * 0x14 + 3] = 20; tbl[1 * 0x14 + 4] = 3; tbl[1 * 0x14 + 7] = 0x00;
    /* Entry 2: [40, 42) last */
    tbl[2 * 0x14 + 3] = 40; tbl[2 * 0x14 + 4] = 2; tbl[2 * 0x14 + 7] = 0x80;
    FCK_EQ((s32)gFindVertexRangeEntry(tbl, 10), (s32)(tbl + 0 * 0x14), "e0 lower bound");
    FCK_EQ((s32)gFindVertexRangeEntry(tbl, 14), (s32)(tbl + 0 * 0x14), "e0 upper bound");
    FCK_EQ((s32)gFindVertexRangeEntry(tbl, 15), 0, "gap between e0 and e1");
    FCK_EQ((s32)gFindVertexRangeEntry(tbl, 22), (s32)(tbl + 1 * 0x14), "e1 match");
    FCK_EQ((s32)gFindVertexRangeEntry(tbl, 41), (s32)(tbl + 2 * 0x14), "e2 match despite 0x80");
    FCK_EQ((s32)gFindVertexRangeEntry(tbl, 42), 0, "past last -> NULL");
    FCK_EQ((s32)gFindVertexRangeEntry(tbl, 9), 0, "below first -> NULL");
}

static void test_func_80021740(void)
{
    s32 kv[8];
    s32 val, idx;
    /* @leaf 80021740 semantic 0-terminated (key,value) table lookup with optional out pointers */
    LK_FUNC("func_80021740");
    kv[0] = 10; kv[1] = 1000;
    kv[2] = 20; kv[3] = 2000;
    kv[4] = 30; kv[5] = -5;
    kv[6] = 0;  kv[7] = 999;
    val = -1; idx = -1;
    FCK_EQ(gFindConfigValue(0, 10, &val, &idx), 0, "NULL table");
    FCK_EQ(gFindConfigValue(kv + 6, 10, &val, &idx), 0, "empty table");
    FCK_EQ(gFindConfigValue(kv, 20, &val, &idx), 1, "found key 20");
    FCK_EQ(val, 2000, "val=2000"); FCK_EQ(idx, 1, "idx=1");
    FCK_EQ(gFindConfigValue(kv, 30, 0, 0), 1, "NULL out ptrs safe");
    val = 77; idx = 88;
    FCK_EQ(gFindConfigValue(kv, 99, &val, &idx), 0, "missing key");
    FCK_EQ(val, 77, "val unchanged on miss"); FCK_EQ(idx, 88, "idx unchanged on miss");
}

static void test_func_80022B84(void)
{
    s32 kv[6];
    s32 val, idx;
    /* @leaf 80022B84 semantic 0-terminated stream option lookup with optional out pointers */
    LK_FUNC("func_80022B84");
    kv[0] = 7; kv[1] = 44100;
    kv[2] = 9; kv[3] = 22050;
    kv[4] = 0; kv[5] = 0;
    val = -1; idx = -1;
    FCK_EQ(gFindStreamOption(0, 7, &val, &idx), 0, "NULL table");
    FCK_EQ(gFindStreamOption(kv, 7, &val, &idx), 1, "found key 7");
    FCK_EQ(val, 44100, "val=44100"); FCK_EQ(idx, 0, "idx=0");
    FCK_EQ(gFindStreamOption(kv, 9, &val, 0), 1, "NULL index ptr safe");
    FCK_EQ(val, 22050, "val=22050");
    FCK_EQ(gFindStreamOption(kv, 123, &val, &idx), 0, "miss");
}

static void test_func_80019A20(void)
{
    /* @leaf 80019A20 semantic variable-length record scan returning payload (rec+0xC) or NULL */
    LK_FUNC("func_80019A20");
    lk_fill(sb1.b, 0, 96);
    /* Rec 0 at +0: id=0x1111, offset=0x18 */
    S32(sb1.b, 0x00) = 0x1111; S32(sb1.b, 0x04) = 0x18;
    /* Rec 1 at +0x18: id=0x2222, offset=0x20 */
    S32(sb1.b, 0x18) = 0x2222; S32(sb1.b, 0x1C) = 0x20;
    /* Rec 2 at +0x38: id=0x3333, offset=0x14 */
    S32(sb1.b, 0x38) = 0x3333; S32(sb1.b, 0x3C) = 0x14;
    FCK_EQ((s32)gFindRecordPayloadById(0x1111, sb1.b, 0x4C), (s32)(sb1.b + 0x0C), "rec0 payload");
    FCK_EQ((s32)gFindRecordPayloadById(0x2222, sb1.b, 0x4C), (s32)(sb1.b + 0x24), "rec1 payload");
    FCK_EQ((s32)gFindRecordPayloadById(0x3333, sb1.b, 0x4C), (s32)(sb1.b + 0x44), "rec2 payload");
    FCK_EQ((s32)gFindRecordPayloadById(0x4444, sb1.b, 0x4C), 0, "miss -> NULL");
    FCK_EQ((s32)gFindRecordPayloadById(0x3333, sb1.b, 0x38), 0, "beyond size -> NULL");
}

static void test_func_80019CA0(void)
{
    /* @leaf 80019CA0 semantic scans chunk headers (tag@0, size@4, id@8); size==0 -> NULL */
    LK_FUNC("func_80019CA0");
    lk_fill(sb1.b, 0, 96);
    /* Sec 0 at +0: tag=0x41414141, len=0x18, id=1 */
    W32(sb1.b, 0x00) = 0x41414141u; S32(sb1.b, 0x04) = 0x18; S32(sb1.b, 0x08) = 1;
    /* Sec 1 at +0x18: tag=0x41414141, len=0x18, id=2 */
    W32(sb1.b, 0x18) = 0x41414141u; S32(sb1.b, 0x1C) = 0x18; S32(sb1.b, 0x20) = 2;
    /* Sec 2 at +0x30: tag=0x42424242, len=0x10, id=2 */
    W32(sb1.b, 0x30) = 0x42424242u; S32(sb1.b, 0x34) = 0x10; S32(sb1.b, 0x38) = 2;
    FCK_EQ((s32)gFindSectionHeader(sb1.b, 0, 0x41414141u, 1), 0, "size=0 -> NULL");
    FCK_EQ((s32)gFindSectionHeader(sb1.b, 0x40, 0x41414141u, 1), (s32)(sb1.b + 0x00), "sec0");
    FCK_EQ((s32)gFindSectionHeader(sb1.b, 0x40, 0x41414141u, 2), (s32)(sb1.b + 0x18), "sec1");
    FCK_EQ((s32)gFindSectionHeader(sb1.b, 0x40, 0x42424242u, 2), (s32)(sb1.b + 0x30), "sec2 tag+id");
    /* Characterization: on miss, returns last inspected header pointer */
    FCK_EQ((s32)gFindSectionHeader(sb1.b, 0x40, 0x43434343u, 9), (s32)(sb1.b + 0x30), "miss returns last hdr");
}

static void test_func_80019C38(void)
{
    /* @leaf 80019C38 semantic scans chunk headers returning payload (+0xC); id==0 -> NULL */
    LK_FUNC("func_80019C38");
    lk_fill(sb1.b, 0, 96);
    W32(sb1.b, 0x00) = 0x54455354u; S32(sb1.b, 0x04) = 0x18; S32(sb1.b, 0x08) = 10;
    W32(sb1.b, 0x18) = 0x54455354u; S32(sb1.b, 0x1C) = 0x18; S32(sb1.b, 0x20) = 20;
    FCK_EQ((s32)gFindSectionPayload(sb1.b, 0x30, 0x54455354u, 0), 0, "id=0 -> NULL");
    FCK_EQ((s32)gFindSectionPayload(sb1.b, 0x30, 0x54455354u, 10), (s32)(sb1.b + 0x0C), "sec0 payload");
    FCK_EQ((s32)gFindSectionPayload(sb1.b, 0x30, 0x54455354u, 20), (s32)(sb1.b + 0x24), "sec1 payload");
    /* Characterization: on miss, p reaches base+size and returns p+0xC */
    FCK_EQ((s32)gFindSectionPayload(sb1.b, 0x30, 0x54455354u, 99), (s32)(sb1.b + 0x3C), "miss returns end+0xC");
}

static void test_func_8001996C(void)
{
    u32 *ccb;
    /* @leaf 8001996C semantic parses CCB/PLUT/PDAT/PIX4 chunks, patches ccb[10]=pdat, ccb[11]=plut */
    LK_FUNC("func_8001996C");
    lk_fill(sb1.b, 0, 192);
    /* Chunk 0 (+0x00, len 0x40): 'CCB ' -> ccb = body+1 = +0x0C */
    W32(sb1.b, 0x00) = 0x43434220u; W32(sb1.b, 0x04) = 0x40u;
    /* Chunk 1 (+0x40, len 0x18): 'PLUT' -> plut = body+1 = +0x4C */
    W32(sb1.b, 0x40) = 0x504C5554u; W32(sb1.b, 0x44) = 0x18u;
    /* Chunk 2 (+0x58, len 0x18): 'PDAT' -> pdat = body = +0x60 */
    W32(sb1.b, 0x58) = 0x50444154u; W32(sb1.b, 0x5C) = 0x18u;
    ccb = gParseTextureContainer(sb1.b, 0x70);
    FCK_EQ((s32)ccb, (s32)(sb1.b + 0x0C), "returns ccb");
    FCK_EQ((s32)ccb[10], (s32)(sb1.b + 0x60), "ccb[10]=pdat");
    FCK_EQ((s32)ccb[11], (s32)(sb1.b + 0x4C), "ccb[11]=plut");
    /* Also test PIX4 tag + unknown chunk skipped */
    W32(sb1.b, 0x40) = 0x4A554E4Bu; /* 'JUNK' ignored -> plut stays 0 */
    W32(sb1.b, 0x58) = 0x50495834u; /* 'PIX4' */
    ccb = gParseTextureContainer(sb1.b, 0x70);
    FCK_EQ((s32)ccb[10], (s32)(sb1.b + 0x60), "PIX4 sets pdat");
    FCK_EQ((s32)ccb[11], 0, "missing PLUT -> 0");
}

static void test_func_8005C2C8(void)
{
    int i;
    /* @leaf 8005C2C8 semantic deduplicated fixed set of 10 s32 items (0=added, 1=dup/full) */
    LK_FUNC("func_8005C2C8");
    lk_fill(sb1.b, 0, 0x80);
    for (i = 0; i < 10; i++) {
        FCK_EQ(gInsertSweepHitTarget(sb1.b, 100 + i), 0, "insert unique");
    }
    FCK_EQ(B8(sb1.b, 0x2A), 10, "count=10");
    FCK_EQ(S32(sb1.b, 0x38 + 0 * 4), 100, "item[0]");
    FCK_EQ(S32(sb1.b, 0x38 + 9 * 4), 109, "item[9]");
    /* Full set rejects new item */
    FCK_EQ(gInsertSweepHitTarget(sb1.b, 999), 1, "full rejects");
    FCK_EQ(B8(sb1.b, 0x2A), 10, "count still 10");
    /* Duplicate rejection when not full */
    B8(sb1.b, 0x2A) = 3;
    FCK_EQ(gInsertSweepHitTarget(sb1.b, 101), 1, "dup rejected");
    FCK_EQ(B8(sb1.b, 0x2A), 3, "count unchanged on dup");
    FCK_EQ(gInsertSweepHitTarget(sb1.b, 555), 0, "non-dup accepted");
    FCK_EQ(B8(sb1.b, 0x2A), 4, "count=4");
    FCK_EQ(S32(sb1.b, 0x38 + 3 * 4), 555, "item[3]=555");
}

/* ============================================ 6. color channels & meshes */

static void test_func_80040550(void)
{
    /* @leaf 80040550 semantic cascading drain second->first->total through object+0x6C ptr */
    LK_FUNC("func_80040550");
    lk_fill(sb1.b, 0, 0x80);
    lk_fill(sb2.b, 0, 0x10);
    W32(sb1.b, 0x6C) = (u32)sb2.b;
    /* total==0 -> early return leaving second/first untouched */
    B8(sb2.b, 9) = 0; B8(sb2.b, 10) = 10; B8(sb2.b, 11) = 20;
    gDrainObjectLightColor(sb1.b, 5);
    FCK_EQ(B8(sb2.b, 11), 20, "total=0 skips drain");
    /* Partial drain from second */
    B8(sb2.b, 9) = 30; B8(sb2.b, 10) = 15; B8(sb2.b, 11) = 10;
    gDrainObjectLightColor(sb1.b, 6);
    FCK_EQ(B8(sb2.b, 11), 4, "second 10-6=4");
    FCK_EQ(B8(sb2.b, 10), 15, "first untouched");
    FCK_EQ(B8(sb2.b, 9), 30, "total untouched");
    /* Cascade across second + first + partial total */
    gDrainObjectLightColor(sb1.b, 25);
    FCK_EQ(B8(sb2.b, 11), 0, "second exhausted");
    FCK_EQ(B8(sb2.b, 10), 0, "first exhausted");
    FCK_EQ(B8(sb2.b, 9), 24, "total 30-(25-4-15)=24");
    /* Over-drain clamps total to 0 */
    gDrainObjectLightColor(sb1.b, 100);
    FCK_EQ(B8(sb2.b, 9), 0, "total clamped to 0");
}

static void test_func_80042864(void)
{
    /* @leaf 80042864 semantic direct-field cascading drain second(+17)->first(+16)->total(+15) */
    LK_FUNC("func_80042864");
    lk_fill(sb1.b, 0, 0x20);
    B8(sb1.b, 0x15) = 0; B8(sb1.b, 0x16) = 8; B8(sb1.b, 0x17) = 9;
    gDrainParticleColor(sb1.b, 5);
    FCK_EQ(B8(sb1.b, 0x17), 9, "total=0 no-op");
    B8(sb1.b, 0x15) = 20; B8(sb1.b, 0x16) = 10; B8(sb1.b, 0x17) = 5;
    gDrainParticleColor(sb1.b, 12);
    FCK_EQ(B8(sb1.b, 0x17), 0, "second 0");
    FCK_EQ(B8(sb1.b, 0x16), 3, "first 10-7=3");
    FCK_EQ(B8(sb1.b, 0x15), 20, "total untouched");
    gDrainParticleColor(sb1.b, 30);
    FCK_EQ(B8(sb1.b, 0x16), 0, "first 0");
    FCK_EQ(B8(sb1.b, 0x15), 0, "total clamped 0");
}

static void test_func_80040600(void)
{
    /* @leaf 80040600 semantic 3-channel clamped step toward target (s+0x8B..0x8D) */
    LK_FUNC("func_80040600");
    lk_fill(sb1.b, 0, 0xA0);
    lk_fill(sb2.b, 0, 0x20);
    W32(sb1.b, 0x6C) = (u32)sb2.b;
    /* Targets: x=100, y=50, z=200; current: x=90, y=60, z=100 */
    B8(sb1.b, 0x8B) = 100; B8(sb1.b, 0x8C) = 50; B8(sb1.b, 0x8D) = 200;
    B8(sb2.b, 9) = 90; B8(sb2.b, 10) = 60; B8(sb2.b, 11) = 100;
    gStepObjectLightColor(sb1.b, 5, -4, 0);
    FCK_EQ(B8(sb2.b, 9), 95, "x +5 unclamped");
    FCK_EQ(B8(sb2.b, 10), 56, "y -4 unclamped");
    FCK_EQ(B8(sb2.b, 11), 100, "z 0 untouched");
    gStepObjectLightColor(sb1.b, 20, -20, 150);
    FCK_EQ(B8(sb2.b, 9), 100, "x clamped hi");
    FCK_EQ(B8(sb2.b, 10), 50, "y clamped lo");
    FCK_EQ(B8(sb2.b, 11), 200, "z clamped hi");
}

static void test_func_800428FC(void)
{
    /* @leaf 800428FC semantic 3-channel particle color step with ping-pong (XOR 8/16/32) vs clamp (OR 1/2/4) */
    LK_FUNC("func_800428FC");
    lk_fill(sb1.b, 0, 0xA0); /* a */
    lk_fill(sb2.b, 0, 0x20); /* b */
    /* Min bounds at a[0x8F..0x91], max bounds at a[0x92..0x94] */
    sb1.b[0x8F] = 10; sb1.b[0x90] = 20; sb1.b[0x91] = 30;
    sb1.b[0x92] = 50; sb1.b[0x93] = 60; sb1.b[0x94] = 70;
    sb1.b[0x8C] = 0; /* clamp mode */
    sb2.b[0x14] = 0; sb2.b[0x15] = 45; sb2.b[0x16] = 25; sb2.b[0x17] = 50;
    gStepParticleColor(sb1.b, sb2.b, 10, -10, 5);
    FCK_EQ(sb2.b[0x15], 50, "x clamped max");
    FCK_EQ(sb2.b[0x16], 20, "y clamped min");
    FCK_EQ(sb2.b[0x17], 55, "z unclamped");
    FCK_EQ(sb2.b[0x14], 3, "clamp flags x(1)|y(2)");
    /* Ping-pong mode (a[0x8C] & 2) */
    sb1.b[0x8C] = 2;
    sb2.b[0x14] = 0; sb2.b[0x17] = 65;
    gStepParticleColor(sb1.b, sb2.b, 0, 0, 10);
    FCK_EQ(sb2.b[0x17], 70, "z clamped max");
    FCK_EQ(sb2.b[0x14], 32, "ping-pong XOR bit 5");
    gStepParticleColor(sb1.b, sb2.b, 0, 0, -50);
    FCK_EQ(sb2.b[0x17], 30, "z clamped min");
    FCK_EQ(sb2.b[0x14], 0, "ping-pong XOR toggles back");
}

static void test_func_800403D0(void)
{
    /* @leaf 800403D0 semantic Q12 color slopes: delta mode vs sum mode (flags66&0x100), div<16->0 */
    LK_FUNC("func_800403D0");
    lk_fill(sb1.b, 0, 0xA0);
    sb1.b[0x87] = 10; sb1.b[0x88] = 20; sb1.b[0x89] = 30;
    sb1.b[0x8B] = 26; sb1.b[0x8C] = 4;  sb1.b[0x8D] = 30;
    /* div94 < 16 -> all 0 */
    H16(sb1.b, 0x66) = 0; S16(sb1.b, 0x94) = 15;
    S16(sb1.b, 0x8E) = 99; S16(sb1.b, 0x90) = 99; S16(sb1.b, 0x92) = 99;
    gInitObjectLightColorSteps(sb1.b);
    FCK_EQ(S16(sb1.b, 0x8E), 0, "div<16 out8E=0");
    FCK_EQ(S16(sb1.b, 0x90), 0, "div<16 out90=0");
    FCK_EQ(S16(sb1.b, 0x92), 0, "div<16 out92=0");
    /* Delta mode, div94 = 64 */
    S16(sb1.b, 0x94) = 64;
    gInitObjectLightColorSteps(sb1.b);
    FCK_EQ(S16(sb1.b, 0x8E), 1024, "(16<<12)/64");
    FCK_EQ(S16(sb1.b, 0x90), -1024, "(-16<<12)/64");
    FCK_EQ(S16(sb1.b, 0x92), 0, "(0<<12)/64");
    /* Sum mode (flags66 & 0x100), div94 = 60 -> ((10+20+30)<<12)/60 = 4096 */
    H16(sb1.b, 0x66) = 0x100; S16(sb1.b, 0x94) = 60;
    gInitObjectLightColorSteps(sb1.b);
    FCK_EQ(S16(sb1.b, 0x8E), 4096, "sum mode 8E");
    FCK_EQ(S16(sb1.b, 0x90), 4096, "sum mode 90");
    FCK_EQ(S16(sb1.b, 0x92), 4096, "sum mode 92");
}

static void test_func_80043250(void)
{
    /* @leaf 80043250 semantic Q12 emitter color rates + endpoint swap when slope < 0 */
    LK_FUNC("func_80043250");
    lk_fill(sb1.b, 0, 0xB0);
    sb1.b[0x8C] = 0;
    sb1.b[0x8F] = 10; sb1.b[0x90] = 40; sb1.b[0x91] = 20;
    sb1.b[0x92] = 26; sb1.b[0x93] = 24; sb1.b[0x94] = 20;
    S16(sb1.b, 0xA6) = 64;
    gComputeEmitterColorRates(sb1.b);
    FCK_EQ(S16(sb1.b, 0x96), 1024, "ch0 +1024");
    FCK_EQ(S16(sb1.b, 0x98), -1024, "ch1 -1024");
    FCK_EQ(S16(sb1.b, 0x9A), 0, "ch2 0");
    /* ch1 was negative -> a90/a93 swapped; ch0/ch2 not swapped */
    FCK_EQ(sb1.b[0x8F], 10, "ch0 not swapped");
    FCK_EQ(sb1.b[0x90], 24, "ch1 swapped lo");
    FCK_EQ(sb1.b[0x93], 40, "ch1 swapped hi");
    /* Sum mode (flags & 1) with a6 < 16 -> 0 */
    sb1.b[0x8C] = 1; S16(sb1.b, 0xA6) = 15;
    gComputeEmitterColorRates(sb1.b);
    FCK_EQ(S16(sb1.b, 0x96), 0, "a6<16 -> 0");
    /* Sum mode with a6 = 54 -> (10+24+20)<<12 / 54 = 4096 */
    S16(sb1.b, 0xA6) = 54;
    gComputeEmitterColorRates(sb1.b);
    FCK_EQ(S16(sb1.b, 0x96), 4096, "sum mode 4096");
    FCK_EQ(S16(sb1.b, 0x9A), 4096, "sum mode all 3");
}

static void test_func_8006F898(void)
{
    /* @leaf 8006F898 semantic dual 3-ch Q12 glow slopes, copies start to m..r, swaps neg endpoints */
    LK_FUNC("func_8006F898");
    lk_fill(sb1.b, 0, 0x90);
    S16(sb1.b, 0x68) = 64; /* d0 */
    S16(sb1.b, 0x6A) = 10; /* d1 < 16 -> u,v,w = 0 */
    /* a..f at 0x6C..0x71: (10,50,20) -> (26,34,20) */
    sb1.b[0x6C] = 10; sb1.b[0x6D] = 50; sb1.b[0x6E] = 20;
    sb1.b[0x6F] = 26; sb1.b[0x70] = 34; sb1.b[0x71] = 20;
    /* g..l at 0x72..0x77 */
    sb1.b[0x72] = 1; sb1.b[0x73] = 2; sb1.b[0x74] = 3;
    sb1.b[0x75] = 9; sb1.b[0x76] = 8; sb1.b[0x77] = 7;
    gInitGlowColorChannelSlopes(sb1.b);
    /* Slopes at 0x80..0x8B (note struct alignment: m..r at 0x79..0x7F, x..w at 0x80) */
    FCK_EQ(S16(sb1.b, 0x80), 1024, "x=+1024");
    FCK_EQ(S16(sb1.b, 0x82), -1024, "y=-1024");
    FCK_EQ(S16(sb1.b, 0x84), 0, "z=0");
    FCK_EQ(S16(sb1.b, 0x86), 0, "d1<16 u=0");
    FCK_EQ(S16(sb1.b, 0x88), 0, "d1<16 v=0");
    FCK_EQ(S16(sb1.b, 0x8A), 0, "d1<16 w=0");
    /* Copied initial values before swap: m=10, n=50, o=20; p=1, q=2, r=3 */
    FCK_EQ(sb1.b[0x79], 10, "m=a"); FCK_EQ(sb1.b[0x7A], 50, "n=b"); FCK_EQ(sb1.b[0x7B], 20, "o=c");
    FCK_EQ(sb1.b[0x7D], 1, "p=g"); FCK_EQ(sb1.b[0x7E], 2, "q=h"); FCK_EQ(sb1.b[0x7F], 3, "r=i");
    /* b/e swapped because y < 0 */
    FCK_EQ(sb1.b[0x6D], 34, "b swapped"); FCK_EQ(sb1.b[0x70], 50, "e swapped");
}

static void test_func_8006E720(void)
{
    /* @leaf 8006E720 source dome fade rate from max of 6 colors and sub->unk48*2400 */
    LK_FUNC("func_8006E720");
    lk_fill(sb1.b, 0, 0xA0);
    lk_fill(sb2.b, 0, 0x60);
    W32(sb1.b, 0x20) = (u32)sb2.b;
    /* unk86 < 16 -> 0 */
    S16(sb1.b, 0x86) = 15;
    gInitDomeEffectFadeRate(sb1.b);
    FCK_EQ(S16(sb1.b, 0x86), 0, "unk86<16 -> 0");
    /* t=100 -> d=240000; maxc=200 (at 0x8F) -> (((0x80000000/240000)>>4)*200)>>4 = 6987 */
    S16(sb2.b, 0x48) = 100;
    S16(sb1.b, 0x86) = 16;
    sb1.b[0x89] = 10; sb1.b[0x8A] = 50; sb1.b[0x8B] = 30;
    sb1.b[0x8D] = 40; sb1.b[0x8E] = 90; sb1.b[0x8F] = 200;
    gInitDomeEffectFadeRate(sb1.b);
    FCK_EQ(S16(sb1.b, 0x86), 6987, "t=100 maxc=200 -> 6987");
    /* Max in first triple (0x89=128), t=200 -> d=480000 -> (((4473)>>4)*128)>>4 = 2232 */
    S16(sb2.b, 0x48) = 200;
    S16(sb1.b, 0x86) = 20;
    sb1.b[0x89] = 128; sb1.b[0x8F] = 10;
    gInitDomeEffectFadeRate(sb1.b);
    FCK_EQ(S16(sb1.b, 0x86), 2232, "t=200 maxc=128 -> 2232");
}

static void test_func_8006C768(void)
{
    /* @leaf 8006C768 semantic dome topology: (rows-1) cap triangles (3,0,v,v+1) + quad strips */
    LK_FUNC("func_8006C768");
    lk_fill(sb1.b, 0xAA, 64);
    /* rows=3, count=7: 2 cap tris (verts 1-2, 2-3), then 1 ring of 2 quads (f=1,s=4 -> (1,4,5,2),(2,5,6,3)) */
    gBuildDomeFaceIndices(sb1.b, 3, 0, 7);
    /* Face 0: tri 3, 0, 1, 2 */
    FCK_EQ(sb1.b[0], 3, "f0 tri"); FCK_EQ(sb1.b[1], 0, "f0 v0");
    FCK_EQ(sb1.b[2], 1, "f0 v1"); FCK_EQ(sb1.b[3], 2, "f0 v2");
    FCK_EQ(sb1.b[4], 0xAA, "f0 pad untouched");
    /* Face 1: tri 3, 0, 2, 3 */
    FCK_EQ(sb1.b[5], 3, "f1 tri"); FCK_EQ(sb1.b[6], 0, "f1 v0");
    FCK_EQ(sb1.b[7], 2, "f1 v1"); FCK_EQ(sb1.b[8], 3, "f1 v2");
    /* Face 2: quad 4, 1, 4, 5, 2 */
    FCK_EQ(sb1.b[10], 4, "f2 quad"); FCK_EQ(sb1.b[11], 1, "f2 v0");
    FCK_EQ(sb1.b[12], 4, "f2 v1"); FCK_EQ(sb1.b[13], 5, "f2 v2"); FCK_EQ(sb1.b[14], 2, "f2 v3");
    /* Face 3: quad 4, 2, 5, 6, 3 */
    FCK_EQ(sb1.b[15], 4, "f3 quad"); FCK_EQ(sb1.b[16], 2, "f3 v0");
    FCK_EQ(sb1.b[17], 5, "f3 v1"); FCK_EQ(sb1.b[18], 6, "f3 v2"); FCK_EQ(sb1.b[19], 3, "f3 v3");
    FCK_CHECK(lk_all(sb1.b, 20, 64, 0xAA), "tail untouched");
}

/* ===================================== 7. gameplay, physics, pad & state */

static void test_func_80057358(void)
{
    /* @leaf 80057358 semantic adds min(amount/4, 0xFF) to charge when charge < 0x100 */
    LK_FUNC("func_80057358");
    lk_fill(sb1.b, 0, 0xB4);
    lk_fill(sb2.b, 0, 0xE0);
    W32(sb1.b, 0xAC) = (u32)sb2.b;
    /* Normal add: 100/4 = 25 */
    S16(sb2.b, 0xD0) = 10;
    gAddCameraShake(sb1.b, 100);
    FCK_EQ(S16(sb2.b, 0xD0), 35, "10 + 100/4");
    /* Clamped add: 2000/4 = 500 -> clamped to 0xFF (255) */
    S16(sb2.b, 0xD0) = 20;
    gAddCameraShake(sb1.b, 2000);
    FCK_EQ(S16(sb2.b, 0xD0), 275, "20 + 255");
    /* Already >= 0x100 -> untouched */
    S16(sb2.b, 0xD0) = 0x100;
    gAddCameraShake(sb1.b, 400);
    FCK_EQ(S16(sb2.b, 0xD0), 0x100, "charge>=0x100 untouched");
}

static void test_func_8005A0C4(void)
{
    /* @leaf 8005A0C4 semantic HUD gauge init: clamp value>=0, limit=div/3, step=0x600000/div, state 0/1/2 */
    LK_FUNC("func_8005A0C4");
    lk_fill(sb1.b, 0xA5, 32);
    gInitHudGauge(sb1.b, 200, 300);
    FCK_EQ(S32(sb1.b, 0), 300, "div=300");
    FCK_EQ(S32(sb1.b, 4), 200, "val=200");
    FCK_EQ(S32(sb1.b, 8), 0x600000 / 300, "step=20971");
    FCK_EQ(S32(sb1.b, 12), 100, "limit=100");
    FCK_EQ(B8(sb1.b, 16), 2, "val>limit -> state 2");
    gInitHudGauge(sb1.b, 100, 300);
    FCK_EQ(B8(sb1.b, 16), 1, "val==limit -> state 1");
    gInitHudGauge(sb1.b, -50, 300);
    FCK_EQ(S32(sb1.b, 4), 0, "neg clamped to 0");
    FCK_EQ(B8(sb1.b, 16), 0, "val==0 -> state 0");
}

static void test_func_80051AC8(void)
{
    /* @leaf 80051AC8 semantic sets crouch bounds center+-0x19 (compact: 0xCC, normal: 0x126) */
    LK_FUNC("func_80051AC8");
    lk_fill(sb1.b, 0, 0xB4);
    lk_fill(sb2.b, 0, 0x2E8);
    lk_fill(sb3.b, 0xA5, 32);
    W32(sb1.b, 0xAC) = (u32)sb2.b;
    W32(sb2.b, 0x2DC) = (u32)sb3.b;
    /* Note: record = base + 8, right at +2 (base+10), left at +8 (base+16) */
    gSetPlayerCrouchBounds(sb1.b, 0);
    FCK_EQ(S16(sb3.b, 10), 0x126 + 0x19, "normal right");
    FCK_EQ(S16(sb3.b, 16), 0x126 - 0x19, "normal left");
    gSetPlayerCrouchBounds(sb1.b, 1);
    FCK_EQ(S16(sb3.b, 10), 0xCC + 0x19, "compact right");
    FCK_EQ(S16(sb3.b, 16), 0xCC - 0x19, "compact left");
}

static void test_func_8004B244(void)
{
    s32 out[3];
    /* @leaf 8004B244 semantic scaled force impulse ((v>>4)*scale)>>8 with flag gating & sign flip */
    LK_FUNC("func_8004B244");
    lk_fill(sb1.b, 0, 0x20); /* source */
    lk_fill(sb2.b, 0, 0x50); /* force */
    out[0] = out[1] = out[2] = 0x5A5A;
    /* NULL force -> 0 */
    FCK_EQ(gComputeForceImpulse(sb1.b, 0, out), 0, "NULL force");
    /* force->flags bit 1 clear -> 0 */
    S32(sb2.b, 4) = 0x400; S32(sb2.b, 8) = -0x800; S32(sb2.b, 12) = 0x200;
    H16(sb2.b, 0x40) = 8; /* strength=8 -> scale=64 */
    B8(sb2.b, 0x49) = 0;
    FCK_EQ(gComputeForceImpulse(sb1.b, sb2.b, out), 0, "flag2 clear");
    /* source->flags & 0x200 -> scale=0 -> returns 0 */
    B8(sb2.b, 0x49) = 2;
    H16(sb1.b, 0x1A) = 0x200;
    FCK_EQ(gComputeForceImpulse(sb1.b, sb2.b, out), 0, "0x200 suppresses");
    /* Normal positive scale: (0x400>>4)*64>>8 = 64*64>>8 = 16 */
    H16(sb1.b, 0x1A) = 0;
    FCK_EQ(gComputeForceImpulse(sb1.b, sb2.b, out), 1, "active returns 1");
    FCK_EQ(out[0], 16, "out.x"); FCK_EQ(out[1], -32, "out.y"); FCK_EQ(out[2], 8, "out.z");
    /* Reversed (0x400) */
    H16(sb1.b, 0x1A) = 0x400;
    FCK_EQ(gComputeForceImpulse(sb1.b, sb2.b, out), 1, "reversed returns 1");
    FCK_EQ(out[0], -16, "rev out.x"); FCK_EQ(out[1], 32, "rev out.y"); FCK_EQ(out[2], -8, "rev out.z");
    /* Zero strength -> 0 */
    H16(sb2.b, 0x40) = 0;
    FCK_EQ(gComputeForceImpulse(sb1.b, sb2.b, out), 0, "zero strength");
}

static void test_func_8008B05C(void)
{
    /* @leaf 8008B05C semantic DualShock pad table size: (((w+1)>>1)<<2) + (((h*5+3)&0xFFC)+4) + data */
    LK_FUNC("func_8008B05C");
    lk_fill(sb1.b, 0, 0x100);
    sb1.b[0xE3] = 0; sb1.b[0xE9] = 0; W32(sb1.b, 0xEC) = 0;
    FCK_EQ(gCalcPadTableSize(sb1.b), 4, "0,0,0 -> 4");
    sb1.b[0xE3] = 1; sb1.b[0xE9] = 1; W32(sb1.b, 0xEC) = 100;
    /* w: ((2)>>1)<<2 = 4; h: ((5+3)&0xFFC)+4 = 12; total = 116 */
    FCK_EQ(gCalcPadTableSize(sb1.b), 116, "1,1,100 -> 116");
    sb1.b[0xE3] = 2; sb1.b[0xE9] = 3; W32(sb1.b, 0xEC) = 0;
    /* w: ((3)>>1)<<2 = 4; h: ((15+3)&0xFFC)+4 = 16+4 = 20; total = 24 */
    FCK_EQ(gCalcPadTableSize(sb1.b), 24, "2,3,0 -> 24");
    sb1.b[0xE3] = 5; sb1.b[0xE9] = 4; W32(sb1.b, 0xEC) = 16;
    /* w: ((6)>>1)<<2 = 12; h: ((20+3)&0xFFC)+4 = 20+4 = 24; total = 52 */
    FCK_EQ(gCalcPadTableSize(sb1.b), 52, "5,4,16 -> 52");
}

static void test_func_8008B668(void)
{
    /* @leaf 8008B668 spec DualShock cmd select: step 2 -> 0x44 len 2, step 3 -> 0x4D len 6 */
    LK_FUNC("func_8008B668");
    lk_fill(sb1.b, 0xA5, 0x80);
    /* Step 1 -> untouched */
    sb1.b[0x46] = 1;
    gCmdPadModeSelect(sb1.b);
    FCK_EQ(sb1.b[0x36], 0xA5, "step 1 untouched");
    /* Step 2 -> cmd 0x44, payload = port+0x51, len = 2 */
    sb1.b[0x46] = 2;
    gCmdPadModeSelect(sb1.b);
    FCK_EQ(sb1.b[0x36], 0x44, "step 2 cmd 0x44");
    FCK_EQ(sb1.b[0x35], 2, "step 2 len 2");
    FCK_EQ((s32)W32(sb1.b, 0x2C), (s32)(sb1.b + 0x51), "step 2 payload +0x51");
    /* Step 3 -> cmd 0x4D, payload = port+0x5D, len = 6 */
    sb1.b[0x46] = 3;
    gCmdPadModeSelect(sb1.b);
    FCK_EQ(sb1.b[0x36], 0x4D, "step 3 cmd 0x4D");
    FCK_EQ(sb1.b[0x35], 6, "step 3 len 6");
    FCK_EQ((s32)W32(sb1.b, 0x2C), (s32)(sb1.b + 0x5D), "step 3 payload +0x5D");
}

static void test_func_8008BF10(void)
{
    /* @leaf 8008BF10 semantic resets pad port state and fills 6 actuator entries with 0xFF when flag49!=0 */
    LK_FUNC("func_8008BF10");
    lk_fill(sb1.b, 0x55, 0xF0);
    /* flag49 == 0 -> untouched */
    sb1.b[0x49] = 0;
    gResetPadPortState(sb1.b);
    FCK_EQ((s32)W32(sb1.b, 0), 0x55555555, "flag49=0 untouched");
    /* flag49 != 0 -> cleared + entries 0x5D..0x62 = 0xFF */
    sb1.b[0x49] = 1;
    gResetPadPortState(sb1.b);
    FCK_EQ((s32)W32(sb1.b, 0x00), 0, "a=0");
    FCK_EQ((s32)W32(sb1.b, 0x04), 0, "b=0");
    FCK_EQ((s32)W32(sb1.b, 0x08), 0, "c=0");
    FCK_EQ((s32)W32(sb1.b, 0x14), 0, "x=0");
    FCK_EQ((s32)W32(sb1.b, 0x18), 0, "y=0");
    FCK_EQ(sb1.b[0x46], 0, "flag46=0");
    FCK_EQ(sb1.b[0x49], 0, "flag49=0");
    FCK_CHECK(lk_all(sb1.b, 0x5D, 0x63, 0xFF), "6 actuator entries=0xFF");
    FCK_EQ(sb1.b[0xE3], 0, "e3=0"); FCK_EQ(sb1.b[0xE4], 0, "e4=0");
    FCK_EQ(H16(sb1.b, 0xE6), 0, "e6=0");
    FCK_EQ(sb1.b[0xE9], 0, "e9=0"); FCK_EQ(sb1.b[0xEA], 0, "ea=0");
    FCK_EQ(sb1.b[0xE5], 0x55, "pad byte untouched");
}

static void test_func_8004749C(void)
{
    u32 handle[2];
    u8 res[0x14];
    /* @leaf 8004749C semantic binds sound voice handle to resource+serial; NULL-safe */
    LK_FUNC("func_8004749C");
    handle[0] = 0x1111; handle[1] = 0x2222;
    S32(res, 0x10) = 0x12345678;
    gBindSoundVoiceHandle(0, res);
    gBindSoundVoiceHandle(handle, 0);
    FCK_EQ((s32)handle[0], 0x1111, "NULL resource leaves handle untouched");
    gBindSoundVoiceHandle(handle, res);
    FCK_EQ((s32)handle[0], (s32)res, "resource bound");
    FCK_EQ((s32)handle[1], 0x12345678, "serial copied");
}

static void test_func_80077884(void)
{
    /* @leaf 80077884 source initializes 44-byte menu descriptor with defaults, arg1@+0x28, arg2@+2 */
    LK_FUNC("func_80077884");
    lk_fill(sb1.b, 0xA5, 64);
    gInitMenuDescriptor(sb1.b, 0x80123456, 7);
    FCK_EQ(sb1.b[0], 0, "unk00=0");
    FCK_EQ(sb1.b[1], 0, "unk01=0");
    FCK_EQ(sb1.b[2], 7, "unk02=arg2");
    FCK_EQ(S8(sb1.b, 3), -1, "unk03=-1");
    FCK_EQ(S16(sb1.b, 6), 100, "unk06=100");
    FCK_EQ(sb1.b[0x0A], 0xA0, "unk0A=0xA0");
    FCK_EQ(sb1.b[0x0B], 0x6E, "unk0B=0x6E");
    FCK_EQ(S32(sb1.b, 0x0C), 0, "unk0C=0");
    FCK_EQ(S32(sb1.b, 0x24), 0, "unk24=0");
    FCK_EQ(S32(sb1.b, 0x28), (s32)0x80123456u, "unk28=arg1");
    FCK_CHECK(lk_all(sb1.b, 0x2C, 64, 0xA5), "tail untouched");
}

static void test_func_80013AF0(void)
{
    /* @leaf 80013AF0 semantic initializes ModelInstance from frame 0 and maps option/model flag bits */
    LK_FUNC("func_80013AF0");
    /* sb1=instance, sb2=model, sb3=frames */
    lk_fill(sb1.b, 0xA5, 0x50);
    lk_fill(sb2.b, 0, 0x40);
    lk_fill(sb3.b, 0, 0x40);
    /* NULL model case */
    gInitModelInstance(sb1.b, 0, 0x80000u | 0x8000u | 0x10000u);
    FCK_EQ((s32)W32(sb1.b, 0), 0, "model=0");
    FCK_EQ(S32(sb1.b, 0x40), 0, "radius=0");
    FCK_EQ(H16(sb1.b, 0x44), 0, "unk44=0");
    FCK_EQ(H16(sb1.b, 0x46), 0, "unk46=0");
    FCK_EQ(S16(sb1.b, 0x48), 0, "frame=0");
    FCK_EQ(sb1.b[0x4A], 0x10 | 0x08 | 0x04, "all 3 option bits");
    /* Non-NULL model with bit 30 set */
    W32(sb2.b, 0x0C) = 0x40000000u;
    W32(sb2.b, 0x34) = (u32)sb3.b;
    /* frames[0] starts at sb3+0x10: unk00@+0, radius@+4, unk06@+6 */
    H16(sb3.b, 0x10) = 0x1234;
    S16(sb3.b, 0x14) = 50;
    H16(sb3.b, 0x16) = 0x5678;
    gInitModelInstance(sb1.b, sb2.b, 0x10000u);
    FCK_EQ((s32)W32(sb1.b, 0), (s32)sb2.b, "model set");
    FCK_EQ(S32(sb1.b, 0x40), 800, "radius=50<<4");
    FCK_EQ(H16(sb1.b, 0x44), 0x1234, "unk44");
    FCK_EQ(H16(sb1.b, 0x46), 0x5678, "unk46");
    FCK_EQ(sb1.b[0x4A], 0x44, "opt 0x10000 -> 4 | model bit30 -> 0x40");
}

static void test_func_800175F8(void)
{
    /* @leaf 800175F8 semantic updates ModelInstance frame metadata unless already at target frame */
    LK_FUNC("func_800175F8");
    lk_fill(sb1.b, 0, 0x50);
    lk_fill(sb2.b, 0, 0x40);
    lk_fill(sb3.b, 0, 0x50);
    W32(sb1.b, 0) = (u32)sb2.b;
    W32(sb2.b, 0x34) = (u32)sb3.b;
    /* Frame 0 at +0x10, Frame 2 at +0x30 */
    H16(sb3.b, 0x30) = 0x2222; S16(sb3.b, 0x34) = 75; H16(sb3.b, 0x36) = 0x3333;
    /* Same frame -> early return without touching radius */
    S16(sb1.b, 0x48) = 2; S32(sb1.b, 0x40) = 9999;
    gSetModelFrame(sb1.b, 2);
    FCK_EQ(S32(sb1.b, 0x40), 9999, "same frame no-op");
    /* Change from frame 0 to frame 2 */
    S16(sb1.b, 0x48) = 0;
    gSetModelFrame(sb1.b, 2);
    FCK_EQ(S16(sb1.b, 0x48), 2, "frame=2");
    FCK_EQ(S32(sb1.b, 0x40), 1200, "radius=75<<4");
    FCK_EQ(H16(sb1.b, 0x44), 0x2222, "unk44");
    FCK_EQ(H16(sb1.b, 0x46), 0x3333, "unk46");
}

static u8 g_card_records[0x25 * 0x80];

static void test_func_80020B28(void)
{
    /* @leaf 80020B28 semantic marks memory card replacement frames [0x10..0x23] with 0xFF/0xFFFF */
    LK_FUNC("func_80020B28");
    lk_fill(g_card_records, 0x33, sizeof(g_card_records));
    gInitCardReplacementFrames(g_card_records);
    /* Frame 0x0F untouched */
    FCK_EQ((s32)W32(g_card_records, 0x0F * 0x80), 0x33333333, "0x0F untouched");
    /* Frame 0x10 marked */
    FCK_EQ((s32)W32(g_card_records, 0x10 * 0x80), (s32)0xFFFFFFFFu, "0x10 bytes=0xFF");
    FCK_EQ(H16(g_card_records, 0x10 * 0x80 + 8), 0xFFFF, "0x10 val=0xFFFF");
    FCK_EQ(g_card_records[0x10 * 0x80 + 4], 0x33, "0x10 pad untouched");
    /* Frame 0x23 marked */
    FCK_EQ((s32)W32(g_card_records, 0x23 * 0x80), (s32)0xFFFFFFFFu, "0x23 bytes=0xFF");
    FCK_EQ(H16(g_card_records, 0x23 * 0x80 + 8), 0xFFFF, "0x23 val=0xFFFF");
    /* Frame 0x24 untouched */
    FCK_EQ((s32)W32(g_card_records, 0x24 * 0x80), 0x33333333, "0x24 untouched");
}

static void test_func_8003BF50(void)
{
    /* @leaf 8003BF50 semantic entity spawn bank classification by kind and linked object kind */
    LK_FUNC("func_8003BF50");
    lk_fill(sb1.b, 0, 0xD0);
    lk_fill(sb2.b, 0, 0xD0);
    FCK_EQ(gGetEntitySpawnBank(0), 0, "NULL -> 0");
    sb1.b[0x16] = 1; FCK_EQ(gGetEntitySpawnBank(sb1.b), 1, "kind 1 -> 1");
    sb1.b[0x16] = 0x12; FCK_EQ(gGetEntitySpawnBank(sb1.b), 1, "kind 0x12 -> 1");
    sb1.b[0x16] = 9; FCK_EQ(gGetEntitySpawnBank(sb1.b), 0, "default -> 0");
    /* kind 7 follows unkC0 */
    sb1.b[0x16] = 7; W32(sb1.b, 0xC0) = 0;
    FCK_EQ(gGetEntitySpawnBank(sb1.b), 0, "kind 7 NULL unkC0 -> 0");
    W32(sb1.b, 0xC0) = (u32)sb2.b; sb2.b[0x16] = 0x11;
    FCK_EQ(gGetEntitySpawnBank(sb1.b), 0, "kind 7 unkC0!=0x12 -> 0");
    sb2.b[0x16] = 0x12;
    FCK_EQ(gGetEntitySpawnBank(sb1.b), 1, "kind 7 unkC0==0x12 -> 1");
    /* kind 2 and 0xF follow unkC8 */
    sb1.b[0x16] = 2; W32(sb1.b, 0xC8) = (u32)sb2.b;
    FCK_EQ(gGetEntitySpawnBank(sb1.b), 1, "kind 2 unkC8==0x12 -> 1");
    sb1.b[0x16] = 0xF; sb2.b[0x16] = 3;
    FCK_EQ(gGetEntitySpawnBank(sb1.b), 0, "kind 0xF unkC8!=0x12 -> 0");
}

static int g_evt_calls;
static s32 g_evt_msg, g_evt_arg3;
static u16 g_evt_id;
static u8 g_evt_en;
static void test_evt_callback(s32 msg, void *handler, void *event, s32 arg3)
{
    (void)handler;
    g_evt_calls++;
    g_evt_msg = msg;
    g_evt_arg3 = arg3;
    g_evt_id = H16(event, 4);
    g_evt_en = B8(event, 6);
}

static void test_func_8003C980(void)
{
    u8 obj[0x30], link[0x40], def[0x40];
    u32 handler[2];
    /* @leaf 8003C980 semantic event router for messages 7 (returns 1), 8 (sets flag), 9 (dispatches msg 14) */
    LK_FUNC("func_8003C980");
    lk_fill(obj, 0, sizeof(obj));
    lk_fill(link, 0, sizeof(link));
    lk_fill(def, 0, sizeof(def));
    handler[0] = 0; handler[1] = (u32)test_evt_callback;
    W32(link, 4) = (u32)handler;
    H16(def, 0x2E) = 0x42;
    W32(obj, 0x1C) = (u32)link;
    W32(obj, 0x20) = (u32)def;
    /* Unknown message -> 0, flags untouched */
    FCK_EQ(gHandleObjectEventMessages(5, obj), 0, "msg 5 -> 0");
    FCK_EQ(obj[0x0F], 0, "msg 5 flags=0");
    /* Message 7 without state&4 -> returns 1, flags untouched */
    FCK_EQ(gHandleObjectEventMessages(7, obj), 1, "msg 7 -> 1");
    FCK_EQ(obj[0x0F], 0, "msg 7 no state&4");
    /* Message 7 with state&4 -> sets flags|=1 */
    obj[0x10] = 4;
    FCK_EQ(gHandleObjectEventMessages(7, obj), 1, "msg 7 state&4 -> 1");
    FCK_EQ(obj[0x0F], 1, "msg 7 sets flag");
    /* Message 8 -> sets flags|=1, returns 0 */
    obj[0x0F] = 0;
    FCK_EQ(gHandleObjectEventMessages(8, obj), 0, "msg 8 -> 0");
    FCK_EQ(obj[0x0F], 1, "msg 8 sets flag");
    /* Message 9 with disabled link -> no callback */
    g_evt_calls = 0; link[0x38] = 1;
    FCK_EQ(gHandleObjectEventMessages(9, obj), 0, "msg 9 disabled -> 0");
    FCK_EQ(g_evt_calls, 0, "disabled skips cb");
    /* Message 9 enabled -> invokes callback(14, handler, &event, 0) */
    link[0x38] = 0;
    FCK_EQ(gHandleObjectEventMessages(9, obj), 0, "msg 9 enabled -> 0");
    FCK_EQ(g_evt_calls, 1, "cb called once");
    FCK_EQ(g_evt_msg, 14, "cb msg=14");
    FCK_EQ(g_evt_id, 0x42, "event.id=0x42");
    FCK_EQ(g_evt_en, 1, "event.enabled=1");
    FCK_EQ(g_evt_arg3, 0, "cb arg3=0");
}

int main(void)
{
    /* 1. Math & fixed-point (11) */
    test_func_80018818();
    test_func_80018580();
    test_func_800195F0();
    test_func_80019F54();
    test_func_8001A074();
    test_func_8001A4E8();
    test_func_8001AF08();
    test_func_8001AE14();
    test_func_8001B148();
    test_func_8001AF84();
    test_func_8005605C();

    /* 2. Memory & data conversion (5) */
    test_func_8001E318();
    test_func_8001E44C();
    test_func_8008731C();
    test_func_80083764();
    test_func_80060DD8();

    /* 3. CD-ROM & PS1 GPU env (6) */
    test_func_8008E61C();
    test_func_8001C494();
    test_func_8009235C();
    test_func_80092634();
    test_func_800926C0();
    test_func_800921F4();

    /* 4. Pools, lists & dispatch (7) */
    test_func_80042764();
    test_func_80042788();
    test_func_800427A8();
    test_func_8003BF1C();
    test_func_8005CB80();
    test_func_8003D9C4();
    test_func_8003F428();

    /* 5. Tables, containers & sets (8) */
    test_func_8003A90C();
    test_func_80021740();
    test_func_80022B84();
    test_func_80019A20();
    test_func_80019CA0();
    test_func_80019C38();
    test_func_8001996C();
    test_func_8005C2C8();

    /* 6. Color channels & meshes (9) */
    test_func_80040550();
    test_func_80042864();
    test_func_80040600();
    test_func_800428FC();
    test_func_800403D0();
    test_func_80043250();
    test_func_8006F898();
    test_func_8006E720();
    test_func_8006C768();

    /* 7. Gameplay, physics, pad & state (14) */
    test_func_80057358();
    test_func_8005A0C4();
    test_func_80051AC8();
    test_func_8004B244();
    test_func_8008B05C();
    test_func_8008B668();
    test_func_8008BF10();
    test_func_8004749C();
    test_func_80077884();
    test_func_80013AF0();
    test_func_800175F8();
    test_func_80020B28();
    test_func_8003BF50();
    test_func_8003C980();

    return lk_done("leaf extra");
}
