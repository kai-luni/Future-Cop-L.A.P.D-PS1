/* Leaf-function suite A2: hardware-free single-object functions that read or
 * write a few globals.
 *
 * Same conventions as leaf_a1_harness.c (markers, evidence classes, LK_FUNC
 * attribution). The difference is data: every D_ symbol a unit references is
 * defined *here*, as plain storage bound to the retail label, and every test
 * writes the state it depends on before calling the unit. No test relies on
 * the initial (zeroed) content of a slot, so a passing check never means "the
 * retail initial value happened to be 0" - see host_test_buildability's
 * zero-slot caveat. Retail table contents (sine table, string tables) are
 * replaced by synthetic fixtures with a known shape, so nothing copyrighted is
 * needed and the expected value follows from the fixture.
 */
#include "leaf_kit.h"
#include "leaf_vectors.h"

/* ------------------------------------------------- globals the units use */
#define DGLOBAL(label, words) u32 label##_[words] __asm__(#label) = {0}
DGLOBAL(D_8009F214, 1);      /* LCG seed */
DGLOBAL(D_8009A234, 0x201);  /* u16 quarter sine table [0x401] */
DGLOBAL(D_8009F590, 1);      /* s32 *free range table */
DGLOBAL(D_8009F594, 1);      /* free range count */
DGLOBAL(D_8009F5A0, 1);      /* threshold */
DGLOBAL(D_8009F588, 1);      /* camera instance list */
DGLOBAL(D_8009F5B8, 1);      /* resource registry head */
DGLOBAL(D_8009F330, 1);      /* sfx volume */
DGLOBAL(D_8009F338, 1);      /* voice volume */
DGLOBAL(D_8009FA20, 1);      /* u8 frame callback enabled */
DGLOBAL(D_8009F99C, 1);      /* screen mode hook */
DGLOBAL(D_8009F838, 1);      /* ground key count */
DGLOBAL(D_800A31A0, 0x30);   /* ground key table, 8-byte entries */
DGLOBAL(D_8009C8A8, 1);      /* CdSyncCallback */
DGLOBAL(D_8009C8AC, 1);      /* CdReadyCallback */
DGLOBAL(D_8009DD90, 1);      /* GPU draw offset */
DGLOBAL(D_8009DDF4, 1);      /* u16 max width @F4, u16 max height @F6 share */
DGLOBAL(D_8009DDF6, 1);
DGLOBAL(D_8009DD18, 1);      /* u16 *interrupt mask */
DGLOBAL(D_800A07A8, 8);      /* sound voice masks: pending @0x14, ack @0x18 */
DGLOBAL(D_800A0B70, 0x11);   /* u16 stream buffer list [0x21] */
DGLOBAL(D_8009F6B8, 1);      /* u8 bit field */
DGLOBAL(D_8009F73C, 1);      /* free voice list */
DGLOBAL(D_8009F740, 1);      /* free voice count */
DGLOBAL(D_800A7938, 770);    /* 0x6E entries of 0x1C bytes */
DGLOBAL(D_8009F738, 1);      /* u8 volume limit selector */
DGLOBAL(D_8009AAE0, 11);     /* u16 text palette [0x14] + guard slot */
DGLOBAL(D_8009FA40, 1);      /* entity list head */
DGLOBAL(D_8009FA88, 1);      /* entity list tail */
DGLOBAL(D_8009FA64, 1);      /* camera near base */
DGLOBAL(D_8009F7F8, 1);      /* definition entry count */
DGLOBAL(D_800A2300, 0x60);   /* 0x30-byte entries */
DGLOBAL(D_8009FAA4, 1);      /* frame time */
DGLOBAL(D_8009F9A4, 1);      /* s32 *countdowns */
DGLOBAL(D_8009F980, 1);
DGLOBAL(D_8009F984, 1);
DGLOBAL(D_8009F988, 1);
DGLOBAL(D_8009C7D0, 1);      /* heap session block base */
DGLOBAL(D_800A0968, 0x40);   /* mixer state */
DGLOBAL(D_8009EA34, 1);      /* SPU normalised mode */
DGLOBAL(D_8009EEC0, 1);      /* SPU transfer flag */
DGLOBAL(D_8009DD30, 8);      /* vsync callback table */
DGLOBAL(D_800A3C20, 0x80);   /* two 0xF0-byte pad port blocks */
DGLOBAL(D_800A6FF8, 0x10);   /* two 0x18-byte actuator records */
DGLOBAL(D_800A78F0, 0x12);   /* two 0x24-byte pad buffers */
DGLOBAL(D_8009C774, 1);      /* pad ext enable */
DGLOBAL(D_8009C75C, 1);      /* u8 *pad base port */
DGLOBAL(D_8009C94C, 7);      /* CD interrupt name table */
DGLOBAL(D_8001233C, 1);      /* CD fallback message */
DGLOBAL(D_8009F798, 1);      /* fragment free list */

#define W32(base, off) (*(u32 *)((u8 *)(base) + (off)))
#define H16(base, off) (*(u16 *)((u8 *)(base) + (off)))
#define S8(base, off) (*(s8 *)((u8 *)(base) + (off)))
#define B8(base, off) (*(u8 *)((u8 *)(base) + (off)))

/* ---------------------------------------------------------------- prototypes */
s32 gNextLcgRand(void) __asm__("func_80019720");
s32 gSineLookup(u32 angle) __asm__("func_80018534");
s32 gFindHeapBoundaryIndex(s32 value) __asm__("func_8001814C");
s32 gIsHeapRangeCountAtThreshold(void) __asm__("func_80017B88");
void *gFindInstanceById(s32 id) __asm__("func_80017B28");
s32 gHasRegisteredResource(void *resource) __asm__("func_80019AA8");
void *gFindRegisteredResource(s32 group, s32 name) __asm__("func_80019BF0");
void gSetSfxVolume(s32 volume) __asm__("func_800451F8");
void gSetVoiceVolume(s32 volume) __asm__("func_80045294");
s32 gSetScreenModeHook(s32 callback) __asm__("func_8001F0FC");
s32 gClearScreenModeHook(s32 callback) __asm__("func_8001F12C");
s32 gRegisterGroundObject(s32 key, u8 value) __asm__("func_8006314C");
void gRemoveGroundKeyEntry(s32 key) __asm__("func_80063194");
s32 gSetCdSyncCallback(s32 value) __asm__("func_8008E194");
s32 gSetCdReadyCallback(s32 value) __asm__("func_8008E1A8");
s32 gSetGpuDrawOffset(s32 value) __asm__("func_80091F60");
s32 gBuildDrawAreaCommands(s16 x, s16 y) __asm__("func_80093F78");
u16 gGetIrqMask(void) __asm__("func_800915E4");
u16 gSwapIrqMask(u16 a0) __asm__("func_800915FC");
void gQueueSoundVoiceOnMask(u32 flags) __asm__("func_80021A70");
void gMoveSoundVoiceMaskToOff(u32 flags) __asm__("func_80021A8C");
void gQueueSoundVoiceOffMask(u32 flags) __asm__("func_80021AB4");
int gGetModuleReadProgressQ10(void) __asm__("func_80028FD4");
void gMarkPortraitVoiceStopped(void) __asm__("func_80029720");
void *gPopFreeSoundVoice(void) __asm__("func_8004665C");
s32 gFindSoundBankByKey(s32 key) __asm__("func_800472E8");
void gSetSoundVoiceMixParameters(void *voice, s32 pan, s32 volume, s32 extra) __asm__("func_800477D8");
void gSetTextPaletteEntry(u32 index, u8 blue, u8 green, u8 red) __asm__("func_800493C0");
void gAppendEntityToList(void *entity) __asm__("func_8004D3D4");
void *gFindEntityByTag(s32 tag) __asm__("func_8004D69C");
void gComputeDepthCueRange(s32 *near, s32 *far) __asm__("func_800536B4");
s32 gFindEffectKeyIndex(s32 key) __asm__("func_8005FF70");
s32 gWorldToRadarCell(s32 arg0) __asm__("func_80065744");
void gStepScriptVmCountdowns(s32 amount) __asm__("func_80069D0C");
s32 gCalcMovieBufferSize(s32 a0, s32 a1, s32 a2) __asm__("func_800896D4");
void gInitHeapBlocks(u32 *arg0, u32 arg1) __asm__("func_8008CA30");
s32 gClearAudioStreamSpuBuffers(void) __asm__("func_80022C68");
void gSpuSetTransferMode(s32 mode) __asm__("func_80098F48");
s32 gSpuGetTransferMode(void) __asm__("func_80098F78");
void gSetVSyncCallbackSlot(u32 index, s32 value) __asm__("func_80091C64");
int gGetPadPortCode(void *address) __asm__("func_8008C3B4");
u8 *gLookupPadPort(u32 flags) __asm__("func_8008C3EC");
void gResetSoundEntry(s32 index) __asm__("func_80045504");
u8 gIsPadRumbleReady(s32 arg0) __asm__("func_8001EAEC");
u8 gIsPadRumbleEnabled(s32 arg0) __asm__("func_8001EB2C");
s32 gGetPadType(s32 port) __asm__("func_8001EF34");
s32 gIsPadPortState8(s32 arg0) __asm__("func_80089E40");
u8 *gGetCdIntName(u32 index) __asm__("func_8008E120");
void *gPopFragmentFreeList(void) __asm__("func_8004AA90");

static union { u32 w[128]; u8 b[512]; } nodes[4];

static void t_random_and_tables(void)
{
    int i;
    u16 *sine = (u16 *)D_8009A234_;

    /* @leaf 80019720 math Park-Miller minimal standard 16807*z mod (2^31-1), check value z10001 */
    LK_FUNC("func_80019720");
    for (i = 0; i < LV_COUNT(lv_pm); i++) {
        D_8009F214_[0] = lv_pm[i][0];
        FCK_EQ(gNextLcgRand(), lv_pm[i][1], "one step from vector seed");
        FCK_EQ(D_8009F214_[0], lv_pm[i][1], "seed updated");
    }
    D_8009F214_[0] = 1;
    for (i = 0; i < 9999; i++) gNextLcgRand();
    FCK_EQ(gNextLcgRand(), LV_PM_AFTER_10000_FROM_1, "Park-Miller published check value");

    /* @leaf 80018534 semantic 4096-unit circle from a quarter-wave table (synthetic table t[i]=i), value*2 */
    LK_FUNC("func_80018534");
    for (i = 0; i <= 0x400; i++) sine[i] = (u16)i;
    FCK_EQ(gSineLookup(0), 0, "0");
    FCK_EQ(gSineLookup(0x100), 0x200, "first quadrant direct");
    FCK_EQ(gSineLookup(0x3FF), 0x7FE, "end of first quadrant");
    FCK_EQ(gSineLookup(0x400), 0x800, "quarter: mirrored index 0x400");
    FCK_EQ(gSineLookup(0x500), 0x600, "second quadrant mirrored");
    FCK_EQ(gSineLookup(0x800), 0, "half");
    FCK_EQ(gSineLookup(0x900), -0x200, "third quadrant negated");
    FCK_EQ(gSineLookup(0xC00), -0x800, "three quarters");
    FCK_EQ(gSineLookup(0xF00), -0x200, "fourth quadrant mirrored+negated");
    FCK_EQ(gSineLookup(0x1100), 0x200, "angle wraps at 4096");
    FCK_EQ(gSineLookup(0xFFFFFF00u), -0x200, "negative angle wraps");

    /* @leaf 8001814C semantic binary search: index k with t[k] <= v < t[k+1], clamped to [0,count-1] */
    LK_FUNC("func_8001814C");
    {
        s32 t[5];
        t[0] = 0; t[1] = 10; t[2] = 20; t[3] = 30; t[4] = 40;
        D_8009F590_[0] = (u32)t; D_8009F594_[0] = 5;
        FCK_EQ(gFindHeapBoundaryIndex(25), 2, "inside");
        FCK_EQ(gFindHeapBoundaryIndex(10), 1, "on boundary");
        FCK_EQ(gFindHeapBoundaryIndex(9), 0, "just below");
        FCK_EQ(gFindHeapBoundaryIndex(40), 4, "last boundary");
        FCK_EQ(gFindHeapBoundaryIndex(1000), 4, "above -> last");
        FCK_EQ(gFindHeapBoundaryIndex(-5), 0, "below -> 0");
        FCK_EQ(gFindHeapBoundaryIndex(39), 3, "below last");
        D_8009F594_[0] = 2;
        FCK_EQ(gFindHeapBoundaryIndex(15), 1, "count 2");
        FCK_EQ(gFindHeapBoundaryIndex(5), 0, "count 2 low");
    }

    /* @leaf 80017B88 source free range count >= threshold */
    LK_FUNC("func_80017B88");
    D_8009F594_[0] = 3; D_8009F5A0_[0] = 3; FCK_EQ(gIsHeapRangeCountAtThreshold(), 1, "equal");
    D_8009F594_[0] = 2; FCK_EQ(gIsHeapRangeCountAtThreshold(), 0, "below");
    D_8009F594_[0] = 4; FCK_EQ(gIsHeapRangeCountAtThreshold(), 1, "above");
    D_8009F594_[0] = (u32)-1; D_8009F5A0_[0] = 0; FCK_EQ(gIsHeapRangeCountAtThreshold(), 0, "signed compare");
}

static void t_lists(void)
{
    u8 *a = nodes[0].b, *b = nodes[1].b, *c = nodes[2].b;

    /* @leaf 80017B28 semantic linked-list search by id (id @0x88, next @0x8C) */
    LK_FUNC("func_80017B28");
    W32(a, 0x88) = 7; W32(a, 0x8C) = (u32)b;
    W32(b, 0x88) = 9; W32(b, 0x8C) = (u32)c;
    W32(c, 0x88) = 9; W32(c, 0x8C) = 0;
    D_8009F588_[0] = (u32)a;
    FCK_CHECK(gFindInstanceById(7) == a, "head");
    FCK_CHECK(gFindInstanceById(9) == b, "first of duplicates");
    FCK_CHECK(gFindInstanceById(5) == 0, "missing");
    D_8009F588_[0] = 0;
    FCK_CHECK(gFindInstanceById(7) == 0, "empty list");

    /* @leaf 80019BF0 semantic registry lookup by (group @0x18, name @0x1C), next @0x0C */
    /* @leaf 80019AA8 semantic registry membership by the (group, name) of a probe record */
    W32(a, 0x0C) = (u32)b; W32(a, 0x18) = 1; W32(a, 0x1C) = 100;
    W32(b, 0x0C) = (u32)c; W32(b, 0x18) = 2; W32(b, 0x1C) = 100;
    W32(c, 0x0C) = 0;      W32(c, 0x18) = 2; W32(c, 0x1C) = 200;
    D_8009F5B8_[0] = (u32)a;
    LK_FUNC("func_80019BF0");
    FCK_CHECK(gFindRegisteredResource(1, 100) == a, "a");
    FCK_CHECK(gFindRegisteredResource(2, 100) == b, "b");
    FCK_CHECK(gFindRegisteredResource(2, 200) == c, "c (tail)");
    FCK_CHECK(gFindRegisteredResource(1, 200) == 0, "group and name must both match");
    LK_FUNC("func_80019AA8");
    {
        u8 *probe = nodes[3].b;
        W32(probe, 0x18) = 2; W32(probe, 0x1C) = 200;
        FCK_EQ(gHasRegisteredResource(probe), 1, "registered");
        W32(probe, 0x1C) = 300;
        FCK_EQ(gHasRegisteredResource(probe), 0, "not registered");
        D_8009F5B8_[0] = 0;
        FCK_EQ(gHasRegisteredResource(probe), 0, "empty registry");
    }

    /* @leaf 8004D3D4 semantic tail append to a singly linked list (head/tail globals), next=0 */
    LK_FUNC("func_8004D3D4");
    D_8009FA40_[0] = 0; D_8009FA88_[0] = 0;
    W32(a, 0) = 0xDEAD; W32(b, 0) = 0xDEAD;
    gAppendEntityToList(a);
    FCK_CHECK(D_8009FA40_[0] == (u32)a, "first becomes head");
    FCK_CHECK(D_8009FA88_[0] == (u32)a, "and tail");
    FCK_EQ(W32(a, 0), 0, "next cleared");
    gAppendEntityToList(b);
    FCK_CHECK(D_8009FA40_[0] == (u32)a, "head kept");
    FCK_CHECK(W32(a, 0) == (u32)b, "a -> b");
    FCK_CHECK(D_8009FA88_[0] == (u32)b, "tail b");
    FCK_EQ(W32(b, 0), 0, "b next cleared");

    /* @leaf 8004D69C semantic list search by u16 tag @0x14 */
    LK_FUNC("func_8004D69C");
    H16(a, 0x14) = 0x1234; H16(b, 0x14) = 0xFFFF;
    FCK_CHECK(gFindEntityByTag(0x1234) == a, "a");
    FCK_CHECK(gFindEntityByTag(0xFFFF) == b, "u16 tag 0xFFFF");
    FCK_CHECK(gFindEntityByTag(-1) == 0, "-1 never equals a zero-extended tag");
    FCK_CHECK(gFindEntityByTag(5) == 0, "missing");

    /* @leaf 8004665C semantic counted free-list pop (next @0x1C) */
    LK_FUNC("func_8004665C");
    W32(a, 0x1C) = (u32)b; W32(b, 0x1C) = 0;
    D_8009F73C_[0] = (u32)a; D_8009F740_[0] = 2;
    FCK_CHECK(gPopFreeSoundVoice() == a, "pop a");
    FCK_EQ(D_8009F740_[0], 1, "count 1");
    FCK_CHECK(D_8009F73C_[0] == (u32)b, "head b");
    FCK_CHECK(gPopFreeSoundVoice() == b, "pop b");
    FCK_EQ(D_8009F740_[0], 0, "count 0");
    D_8009F73C_[0] = (u32)a;
    FCK_CHECK(gPopFreeSoundVoice() == 0, "count 0 -> NULL even with a head");
    FCK_CHECK(D_8009F73C_[0] == (u32)a, "head untouched when empty");

    /* @leaf 8004AA90 source pop that keeps the last node: NULL when head->next (@0x48) is NULL */
    LK_FUNC("func_8004AA90");
    W32(a, 0x48) = (u32)b; W32(b, 0x48) = 0;
    D_8009F798_[0] = (u32)a;
    FCK_CHECK(gPopFragmentFreeList() == a, "pop a");
    FCK_CHECK(D_8009F798_[0] == (u32)b, "head b");
    FCK_CHECK(gPopFragmentFreeList() == 0, "last node is kept");
    FCK_CHECK(D_8009F798_[0] == (u32)b, "head still b");
}

static void t_registers_and_hooks(void)
{
    u16 mask;

    /* @leaf 800451F8 semantic clamp volume to [0, 127] */
    LK_FUNC("func_800451F8");
    gSetSfxVolume(64);   FCK_EQ(D_8009F330_[0], 64, "64");
    gSetSfxVolume(127);  FCK_EQ(D_8009F330_[0], 127, "127");
    gSetSfxVolume(128);  FCK_EQ(D_8009F330_[0], 127, "128 clamped");
    gSetSfxVolume(-1);   FCK_EQ(D_8009F330_[0], 0, "-1 clamped");
    gSetSfxVolume(0);    FCK_EQ(D_8009F330_[0], 0, "0");
    gSetSfxVolume(0x7FFFFFFF); FCK_EQ(D_8009F330_[0], 127, "INT_MAX");

    /* @leaf 80045294 semantic clamp volume to [0, 127] */
    LK_FUNC("func_80045294");
    gSetVoiceVolume(1);    FCK_EQ(D_8009F338_[0], 1, "1");
    gSetVoiceVolume(200);  FCK_EQ(D_8009F338_[0], 127, "200 clamped");
    gSetVoiceVolume((s32)0x80000000); FCK_EQ(D_8009F338_[0], 0, "INT_MIN");
    gSetVoiceVolume(127);  FCK_EQ(D_8009F338_[0], 127, "127");

    /* @leaf 8001F0FC source install hook once; 3 when disabled or occupied */
    LK_FUNC("func_8001F0FC");
    D_8009FA20_[0] = 0; D_8009F99C_[0] = 0;
    FCK_EQ(gSetScreenModeHook(0x1234), 3, "disabled");
    FCK_EQ(D_8009F99C_[0], 0, "disabled: not installed");
    D_8009FA20_[0] = 1;
    FCK_EQ(gSetScreenModeHook(0x1234), 0, "installed");
    FCK_EQ(D_8009F99C_[0], 0x1234, "stored");
    FCK_EQ(gSetScreenModeHook(0x5678), 3, "occupied");
    FCK_EQ(D_8009F99C_[0], 0x1234, "occupied: kept");

    /* @leaf 8001F12C source remove hook only when the same callback is passed */
    LK_FUNC("func_8001F12C");
    FCK_EQ(gClearScreenModeHook(0x5678), 3, "different callback");
    FCK_EQ(D_8009F99C_[0], 0x1234, "kept");
    FCK_EQ(gClearScreenModeHook(0x1234), 0, "removed");
    FCK_EQ(D_8009F99C_[0], 0, "cleared");
    FCK_EQ(gClearScreenModeHook(0), 3, "none installed");

    /* @leaf 8008E194 spec Psy-Q CdSyncCallback: install, return previous */
    LK_FUNC("func_8008E194");
    D_8009C8A8_[0] = 0x111;
    FCK_EQ(gSetCdSyncCallback(0x222), 0x111, "previous");
    FCK_EQ(D_8009C8A8_[0], 0x222, "installed");
    FCK_EQ(gSetCdSyncCallback(0), 0x222, "previous again");

    /* @leaf 8008E1A8 spec Psy-Q CdReadyCallback: install, return previous */
    LK_FUNC("func_8008E1A8");
    D_8009C8AC_[0] = 0x333;
    FCK_EQ(gSetCdReadyCallback(0x444), 0x333, "previous");
    FCK_EQ(D_8009C8AC_[0], 0x444, "installed");
    FCK_EQ(D_8009C8A8_[0], 0, "sync slot untouched");

    /* @leaf 80091F60 source swap draw offset value, return previous */
    LK_FUNC("func_80091F60");
    D_8009DD90_[0] = 5;
    FCK_EQ(gSetGpuDrawOffset(-7), 5, "previous");
    FCK_EQ(D_8009DD90_[0], -7, "stored");

    /* @leaf 800915FC spec Psy-Q SetIntrMask: store new mask, return old */
    LK_FUNC("func_800915FC");
    mask = 0x00FF;
    D_8009DD18_[0] = (u32)&mask;
    FCK_EQ(gSwapIrqMask(0x0401), 0x00FF, "old mask");
    FCK_EQ(mask, 0x0401, "new mask");
    FCK_EQ(gSwapIrqMask(0xFFFF), 0x0401, "old again");
    /* @leaf 800915E4 spec Psy-Q GetIntrMask: read current mask */
    LK_FUNC("func_800915E4");
    FCK_EQ(gGetIrqMask(), 0xFFFF, "u16 read");
    mask = 0x8001;
    FCK_EQ(gGetIrqMask(), 0x8001, "reflects memory");

    /* @leaf 80093F78 spec psx-spx GP0(E3h) x bits 0-9, y bits 10-19; source: clamp to [0, max-1] */
    LK_FUNC("func_80093F78");
    *(u16 *)D_8009DDF4_ = 1024; *(u16 *)D_8009DDF6_ = 512;
    FCK_EQ(gBuildDrawAreaCommands(100, 200), (s32)(0xE3000000u | (200u << 10) | 100u), "inside");
    FCK_EQ(gBuildDrawAreaCommands(-5, -5), (s32)0xE3000000, "negative clamps to 0");
    FCK_EQ(gBuildDrawAreaCommands(2000, 600), (s32)0xE307FFFF, "clamp to 1023,511");
    FCK_EQ(gBuildDrawAreaCommands(1023, 511), (s32)0xE307FFFF, "max inside");
    FCK_EQ(gBuildDrawAreaCommands(1024, 0), (s32)0xE30003FF, "x = max clamps");
    *(u16 *)D_8009DDF4_ = 320; *(u16 *)D_8009DDF6_ = 240;
    FCK_EQ(gBuildDrawAreaCommands(400, 300), (s32)(0xE3000000u | (239u << 10) | 319u), "320x240 limits");

    /* @leaf 80098F48 semantic Psy-Q SpuSetTransferMode: 1 (by I/O) sets the flag, anything else clears */
    /* @leaf 80098F78 semantic SpuGetTransferMode returns 1 only when the flag is 1, normalising the mode */
    LK_FUNC("func_80098F48");
    gSpuSetTransferMode(1);
    FCK_EQ(D_8009EEC0_[0], 1, "IO -> flag 1"); FCK_EQ(D_8009EA34_[0], 1, "mode stored");
    gSpuSetTransferMode(0);
    FCK_EQ(D_8009EEC0_[0], 0, "DMA -> flag 0");
    gSpuSetTransferMode(7);
    FCK_EQ(D_8009EEC0_[0], 0, "unknown -> flag 0"); FCK_EQ(D_8009EA34_[0], 7, "raw mode stored");
    LK_FUNC("func_80098F78");
    FCK_EQ(gSpuGetTransferMode(), 0, "after unknown mode -> 0");
    FCK_EQ(D_8009EA34_[0], 0, "normalised");
    gSpuSetTransferMode(1);
    FCK_EQ(gSpuGetTransferMode(), 1, "round trip IO");
    D_8009EEC0_[0] = 5;
    FCK_EQ(gSpuGetTransferMode(), 0, "corrupt flag -> 0");

    /* @leaf 80091C64 source store callback slot */
    LK_FUNC("func_80091C64");
    D_8009DD30_[3] = 1;
    gSetVSyncCallbackSlot(3, 0x99);
    FCK_EQ(D_8009DD30_[3], 0x99, "slot 3");
    FCK_EQ(D_8009DD30_[2], 0, "slot 2 untouched");
    gSetVSyncCallbackSlot(3, 0x99);
    FCK_EQ(D_8009DD30_[3], 0x99, "same value");
}

static void t_sound(void)
{
    u8 *ss = (u8 *)D_800A07A8_;
    u8 *tab = (u8 *)D_800A7938_;
    u16 *list = (u16 *)D_800A0B70_;
    int i;

    /* @leaf 80021A70 semantic pending |= flags (@0x14) */
    LK_FUNC("func_80021A70");
    W32(ss, 0x14) = 0x10; W32(ss, 0x18) = 0;
    gQueueSoundVoiceOnMask(0x5);
    FCK_EQ(W32(ss, 0x14), 0x15, "or"); FCK_EQ(W32(ss, 0x18), 0, "ack untouched");
    /* @leaf 80021A8C semantic move flags from pending (@0x14) to acknowledged (@0x18) */
    LK_FUNC("func_80021A8C");
    gMoveSoundVoiceMaskToOff(0x11);
    FCK_EQ(W32(ss, 0x14), 0x04, "pending cleared"); FCK_EQ(W32(ss, 0x18), 0x11, "acked");
    /* @leaf 80021AB4 semantic flags |= mask at 0x18 */
    LK_FUNC("func_80021AB4");
    gQueueSoundVoiceOffMask(0x80000000u);
    FCK_EQ(W32(ss, 0x18), (s32)0x80000011u, "or high bit"); FCK_EQ(W32(ss, 0x14), 0x04, "pending untouched");

    /* @leaf 80028FD4 semantic progress in Q10: (done << 10) / total, total 0 -> 0 */
    LK_FUNC("func_80028FD4");
    list[0x1F] = 512; list[0x20] = 1024; FCK_EQ(gGetModuleReadProgressQ10(), 512, "half");
    list[0x1F] = 1024; FCK_EQ(gGetModuleReadProgressQ10(), 1024, "done");
    list[0x1F] = 1; list[0x20] = 3; FCK_EQ(gGetModuleReadProgressQ10(), 341, "1/3 truncates");
    list[0x1F] = 0xFFFF; list[0x20] = 0xFFFF; FCK_EQ(gGetModuleReadProgressQ10(), 1024, "u16 max");
    list[0x20] = 0; FCK_EQ(gGetModuleReadProgressQ10(), 0, "no total");

    /* @leaf 80029720 source clear bit 4; set bit 3 unless bit 5 */
    LK_FUNC("func_80029720");
    D_8009F6B8_[0] = 0xFF; gMarkPortraitVoiceStopped(); FCK_EQ(B8(D_8009F6B8_, 0), 0xEF, "bit5 set");
    D_8009F6B8_[0] = 0x10; gMarkPortraitVoiceStopped(); FCK_EQ(B8(D_8009F6B8_, 0), 0x08, "bit5 clear");
    D_8009F6B8_[0] = 0x00; gMarkPortraitVoiceStopped(); FCK_EQ(B8(D_8009F6B8_, 0), 0x08, "zero");
    D_8009F6B8_[0] = 0x30; gMarkPortraitVoiceStopped(); FCK_EQ(B8(D_8009F6B8_, 0), 0x20, "bit4+5");

    /* @leaf 800472E8 source search 0x6E 0x1C-byte banks for key @0x16, stop at used(@0x14)==0 */
    LK_FUNC("func_800472E8");
    lk_fill(tab, 0, 0x6E * 0x1C);
    B8(tab, 0 * 0x1C + 0x14) = 1; B8(tab, 0 * 0x1C + 0x16) = 5;
    B8(tab, 1 * 0x1C + 0x14) = 1; B8(tab, 1 * 0x1C + 0x16) = 9;
    B8(tab, 3 * 0x1C + 0x14) = 1; B8(tab, 3 * 0x1C + 0x16) = 7;
    FCK_EQ(gFindSoundBankByKey(5), 0, "first");
    FCK_EQ(gFindSoundBankByKey(9), 1, "second");
    FCK_EQ(gFindSoundBankByKey(7), -1, "behind an unused slot");
    for (i = 0; i < 0x6E; i++) { B8(tab, i * 0x1C + 0x14) = 1; B8(tab, i * 0x1C + 0x16) = 1; }
    B8(tab, 0x6D * 0x1C + 0x16) = 2;
    FCK_EQ(gFindSoundBankByKey(2), 0x6D, "last slot");
    FCK_EQ(gFindSoundBankByKey(3), -1, "all used, missing");

    /* @leaf 80045504 source reset one 0x1C-byte entry */
    LK_FUNC("func_80045504");
    lk_fill(tab, 0xAA, 3 * 0x1C);
    gResetSoundEntry(1);
    FCK_EQ(B8(tab, 0x1C + 0x10), 0, "active"); FCK_EQ(B8(tab, 0x1C + 0x11), 0, "11");
    FCK_EQ(B8(tab, 0x1C + 0x12), 0, "12"); FCK_EQ(B8(tab, 0x1C + 0x13), 1, "13 = 1");
    FCK_EQ(B8(tab, 0x1C + 0x14), 0, "14"); FCK_EQ(B8(tab, 0x1C + 0x15), 0, "15");
    FCK_EQ(B8(tab, 0x1C + 0x16), 0xFF, "16 = 0xFF"); FCK_EQ(B8(tab, 0x1C + 0x17), 0xAA, "17 untouched");
    FCK_EQ(W32(tab, 0x1C + 0x18), 0, "18 word");
    FCK_CHECK(lk_all(tab, 0, 0x1C + 0x10, 0xAA), "entry 0 and head of entry 1 untouched");
    FCK_CHECK(lk_all(tab, 2 * 0x1C, 3 * 0x1C, 0xAA), "entry 2 untouched");

    /* @leaf 800477D8 source store pan/volume/extra; volume halved (>>1) when the limit selector is set */
    LK_FUNC("func_800477D8");
    {
        u32 v[2];
        v[0] = 0x12345678; v[1] = 0;
        D_8009F738_[0] = 0;
        gSetSoundVoiceMixParameters(v, 10, 100, 7);
        FCK_EQ(S8(v, 4), 100, "volume"); FCK_EQ(S8(v, 5), 100, "base");
        FCK_EQ(S8(v, 6), 10, "pan"); FCK_EQ(S8(v, 7), 7, "extra");
        FCK_EQ(v[0], 0x12345678, "bank untouched");
        D_8009F738_[0] = 1;
        gSetSoundVoiceMixParameters(v, -10, 100, 0);
        FCK_EQ(S8(v, 4), 50, "halved"); FCK_EQ(S8(v, 5), 100, "base not halved");
        FCK_EQ(S8(v, 6), -10, "negative pan");
        gSetSoundVoiceMixParameters(v, 0, -1, 0);
        FCK_EQ(S8(v, 4), -1, "-1 >> 1 is arithmetic");
    }

    /* @leaf 80022C68 source clear field of count (@0x14) entries at 0x44 stride 0x24, return 0 */
    LK_FUNC("func_80022C68");
    {
        u8 *m = (u8 *)D_800A0968_;
        lk_fill(m, 0xAA, 0x100);
        W32(m, 0x14) = 2;
        FCK_EQ(gClearAudioStreamSpuBuffers(), 0, "returns 0");
        FCK_EQ(W32(m, 0x44), 0, "entry 0"); FCK_EQ(W32(m, 0x44 + 0x24), 0, "entry 1");
        FCK_EQ(W32(m, 0x44 + 0x48), (s32)0xAAAAAAAA, "entry 2 untouched");
        FCK_EQ(W32(m, 0x48), (s32)0xAAAAAAAA, "rest of entry untouched");
        W32(m, 0x14) = 0; W32(m, 0x44) = 5;
        gClearAudioStreamSpuBuffers();
        FCK_EQ(W32(m, 0x44), 5, "count 0");
    }
}

static void t_misc(void)
{
    u16 *pal = (u16 *)D_8009AAE0_;
    s32 near, far;
    int i;

    /* @leaf 800493C0 spec PS1 15-bit VRAM colour r | g<<5 | b<<10; index >= 0x14 ignored */
    LK_FUNC("func_800493C0");
    for (i = 0; i <= 0x15; i++) pal[i] = 0xAAAA;
    gSetTextPaletteEntry(3, 31, 0, 0);  FCK_EQ(pal[3], 0x7C00, "blue");
    gSetTextPaletteEntry(0, 0, 31, 0);  FCK_EQ(pal[0], 0x03E0, "green");
    gSetTextPaletteEntry(0x13, 0, 0, 31); FCK_EQ(pal[0x13], 0x001F, "red, last slot");
    gSetTextPaletteEntry(1, 1, 2, 3);   FCK_EQ(pal[1], 0x0443, "mixed");
    gSetTextPaletteEntry(0x14, 31, 31, 31); FCK_EQ(pal[0x14], 0xAAAA, "index 0x14 rejected (guard slot)");
    FCK_EQ(pal[2], 0xAAAA, "untouched slot");
    FCK_EQ(((u16 *)D_8009AAE0_)[0x13], 0x001F, "slot 0x13 unchanged by rejected write");

    /* @leaf 800536B4 source near = (d<<8)/608; far = ((d-0x1A000)<<8)/192 when d >= 0x1A000 else 0 */
    LK_FUNC("func_800536B4");
    D_8009FA64_[0] = 608; near = far = 99;
    gComputeDepthCueRange(&near, &far);
    FCK_EQ(near, 256, "near 608"); FCK_EQ(far, 0, "far below start");
    D_8009FA64_[0] = 0x1A000 + 192;
    gComputeDepthCueRange(&near, &far);
    FCK_EQ(near, 44921, "near truncated"); FCK_EQ(far, 256, "far");
    D_8009FA64_[0] = 0x1A000;
    gComputeDepthCueRange(&near, &far);
    FCK_EQ(far, 0, "far at boundary");

    /* @leaf 8005FF70 semantic linear search of u8 key in 0x30-byte entries; -1 when missing */
    LK_FUNC("func_8005FF70");
    {
        u8 *t = (u8 *)D_800A2300_;
        lk_fill(t, 0, 0x30 * 4);
        t[0] = 4; t[0x30] = 8; t[0x60] = 4; t[0x90] = 200;
        D_8009F7F8_[0] = 3;
        FCK_EQ(gFindEffectKeyIndex(4), 0, "first");
        FCK_EQ(gFindEffectKeyIndex(8), 1, "second");
        FCK_EQ(gFindEffectKeyIndex(200), -1, "beyond count");
        FCK_EQ(gFindEffectKeyIndex(0x104), -1, "u8 key vs wider int");
        D_8009F7F8_[0] = 4;
        FCK_EQ(gFindEffectKeyIndex(200), 3, "unsigned key 200");
        D_8009F7F8_[0] = 0;
        FCK_EQ(gFindEffectKeyIndex(4), -1, "empty");
    }

    /* @leaf 80065744 source radar cell blink/cycle from frame time */
    LK_FUNC("func_80065744");
    D_8009FAA4_[0] = 64;
    FCK_EQ(gWorldToRadarCell(0), 0, "zero");
    FCK_EQ(gWorldToRadarCell(5), 5, "(5*64)>>6 % 28");
    FCK_EQ(gWorldToRadarCell(30), 2, "30 % 28");
    FCK_EQ(gWorldToRadarCell(-5), 22, "27 - 5");
    D_8009FAA4_[0] = 1024;
    FCK_EQ(gWorldToRadarCell(0x21), 0x1C, "odd blink phase");
    D_8009FAA4_[0] = 2048;
    FCK_EQ(gWorldToRadarCell(0x21), 0, "even blink phase");
    FCK_EQ(gWorldToRadarCell(0x20), 16, "0x20 is a cycle value: (32*2048>>6)%28 = 1024%28 = 16");

    /* @leaf 80069D0C source decrement 32 positive countdowns; results below 5 snap to 0 */
    LK_FUNC("func_80069D0C");
    {
        s32 t[33];
        for (i = 0; i < 33; i++) t[i] = 50;
        t[0] = 10; t[1] = 5; t[2] = 0; t[3] = -3; t[4] = 8; t[31] = 100;
        D_8009F9A4_[0] = (u32)t;
        gStepScriptVmCountdowns(3);
        FCK_EQ(t[0], 7, "10-3"); FCK_EQ(t[1], 0, "2 snaps to 0"); FCK_EQ(t[2], 0, "0 untouched");
        FCK_EQ(t[3], -3, "negative untouched"); FCK_EQ(t[4], 5, "5 stays");
        FCK_EQ(t[31], 97, "last of 32"); FCK_EQ(t[32], 50, "33rd untouched");
    }

    /* @leaf 800896D4 source size = bytes + (sectors<<11) + 0x57C + 56*descriptors; stores the inputs */
    LK_FUNC("func_800896D4");
    FCK_EQ(gCalcMovieBufferSize(4, 100, 3), 100 + 4 * 2048 + 0x57C + 3 * 56, "size");
    FCK_EQ(D_8009F980_[0], 4, "sectors"); FCK_EQ(D_8009F984_[0], 100, "bytes");
    FCK_EQ(D_8009F988_[0], 3, "descriptors");
    FCK_EQ(gCalcMovieBufferSize(0, 0, 0), 0x57C, "header only");

    /* @leaf 8008CA30 source heap init: size rounded down to 8, end sentinel, base[1] = blocks-1 */
    LK_FUNC("func_8008CA30");
    {
        u32 *h = nodes[0].w;
        lk_fill(h, 0xAA, 0x80);
        D_8009C7D0_[0] = 0x55;
        gInitHeapBlocks(0, 64);
        FCK_EQ(D_8009C7D0_[0], 0x55, "NULL base ignored");
        gInitHeapBlocks(h, 0);
        FCK_EQ(D_8009C7D0_[0], 0x55, "zero size ignored");
        gInitHeapBlocks(h, 0x43);
        FCK_CHECK(h[0] == (u32)&h[14], "base -> sentinel at 0x40-8");
        FCK_EQ(h[1], 7, "0x40/8 - 1 blocks");
        FCK_CHECK(h[14] == (u32)h, "sentinel -> base");
        FCK_EQ(h[15], 0, "sentinel size 0");
        FCK_CHECK(D_8009C7D0_[0] == (u32)h, "session base");
        FCK_EQ(h[16], (s32)0xAAAAAAAA, "beyond rounded size untouched");
    }

    /* @leaf 8008C3B4 source port block address -> 0x10 / 0x20, else 0xFF */
    /* @leaf 8008C3EC source flags & 0xF0 selects the second 0xF0-byte port block */
    LK_FUNC("func_8008C3B4");
    {
        u8 *base = (u8 *)D_800A3C20_;
        FCK_EQ(gGetPadPortCode(base), 0x10, "port 1");
        FCK_EQ(gGetPadPortCode(base + 0xF0), 0x20, "port 2");
        FCK_EQ(gGetPadPortCode(base + 1), 0xFF, "inside block");
        FCK_EQ(gGetPadPortCode(base + 0x1E0), 0xFF, "third block");
        LK_FUNC("func_8008C3EC");
        FCK_CHECK(gLookupPadPort(0x00) == base, "port 1");
        FCK_CHECK(gLookupPadPort(0x10) == base + 0xF0, "port 2");
        FCK_CHECK(gLookupPadPort(0x0F) == base, "low nibble ignored");
        FCK_CHECK(gLookupPadPort(0xF00) == base, "bits above 0xF0 ignored");
    }

    /* @leaf 8001EAEC source byte 0x16 of actuator record port (clamped: <0 ->0, >=2 ->1) */
    /* @leaf 8001EB2C source byte 0x17 of the same clamped record */
    {
        u8 *act = (u8 *)D_800A6FF8_;
        lk_fill(act, 0, 0x40);
        act[0x16] = 0x11; act[0x17] = 0x12; act[0x18 + 0x16] = 0x21; act[0x18 + 0x17] = 0x22;
        LK_FUNC("func_8001EAEC");
        FCK_EQ(gIsPadRumbleReady(0), 0x11, "port 0");
        FCK_EQ(gIsPadRumbleReady(1), 0x21, "port 1");
        FCK_EQ(gIsPadRumbleReady(5), 0x21, "clamped high");
        FCK_EQ(gIsPadRumbleReady(-1), 0x11, "clamped low");
        LK_FUNC("func_8001EB2C");
        FCK_EQ(gIsPadRumbleEnabled(0), 0x12, "port 0");
        FCK_EQ(gIsPadRumbleEnabled(1), 0x22, "port 1");
        FCK_EQ(gIsPadRumbleEnabled(2), 0x22, "clamped high");
        FCK_EQ(gIsPadRumbleEnabled(-9), 0x12, "clamped low");
    }

    /* @leaf 8001EF34 spec psx-spx pad ID high nibble: 4 digital, 7 analog; status byte != 0 -> 0 */
    LK_FUNC("func_8001EF34");
    {
        u8 *pb = (u8 *)D_800A78F0_;
        lk_fill(pb, 0, 0x48);
        pb[1] = 0x41;               FCK_EQ(gGetPadType(0), 4, "digital pad 41h");
        pb[1] = 0x73;               FCK_EQ(gGetPadType(0), 7, "analog pad 73h");
        pb[1] = 0x23;               FCK_EQ(gGetPadType(0), 0, "negcon 23h unsupported");
        pb[1] = 0x53;               FCK_EQ(gGetPadType(0), 0, "analog stick 53h unsupported");
        pb[0] = 0xFF; pb[1] = 0x41; FCK_EQ(gGetPadType(0), 0, "status FFh: no pad");
        pb[0x24] = 0; pb[0x25] = 0x73; FCK_EQ(gGetPadType(1), 7, "port 1 at +0x24");
    }

    /* @leaf 80089E40 source flag && port byte [(arg>>4)*240 + 0xE8] == 8 */
    LK_FUNC("func_80089E40");
    {
        u8 *pp = nodes[1].b;
        lk_fill(pp, 0, 512);
        pp[0xE8] = 8; pp[240 + 0xE8] = 7;
        D_8009C75C_[0] = (u32)pp;
        D_8009C774_[0] = 0;
        FCK_EQ(gIsPadPortState8(0x00), 0, "disabled");
        D_8009C774_[0] = 1;
        FCK_EQ(gIsPadPortState8(0x00), 1, "port 0 state 8");
        FCK_EQ(gIsPadPortState8(0x0F), 1, "low nibble ignored");
        FCK_EQ(gIsPadPortState8(0x10), 0, "port 1 state 7");
    }

    /* @leaf 8008E120 source name table for index & 0xFF < 7, else fallback message */
    LK_FUNC("func_8008E120");
    for (i = 0; i < 7; i++) D_8009C94C_[i] = 0x1000 + i;
    FCK_EQ(gGetCdIntName(0), 0x1000, "0");
    FCK_EQ(gGetCdIntName(6), 0x1006, "6");
    FCK_CHECK(gGetCdIntName(7) == (u8 *)D_8001233C_, "7 -> fallback");
    FCK_EQ(gGetCdIntName(0x103), 0x1003, "only low byte used");
}

static void t_ground_keys(void)
{
    u32 *t = D_800A31A0_;
    int i;

    /* @leaf 8006314C semantic append (key, value) to a 0x18-entry table; 0 when full */
    LK_FUNC("func_8006314C");
    D_8009F838_[0] = 0;
    FCK_EQ(gRegisterGroundObject(0x111, 1), 1, "first");
    FCK_EQ(gRegisterGroundObject(0x222, 2), 1, "second");
    FCK_EQ(gRegisterGroundObject(0x333, 3), 1, "third");
    FCK_EQ(D_8009F838_[0], 3, "count");
    FCK_EQ(t[0], 0x111, "key 0"); FCK_EQ(B8(t, 4), 1, "value 0");
    FCK_EQ(t[4], 0x333, "key 2"); FCK_EQ(B8(t, 20), 3, "value 2");
    D_8009F838_[0] = 0x17;
    FCK_EQ(gRegisterGroundObject(0x444, 4), 1, "last free slot");
    FCK_EQ(gRegisterGroundObject(0x555, 5), 0, "full");
    FCK_EQ(D_8009F838_[0], 0x18, "count capped");

    /* @leaf 80063194 semantic swap-remove by key: last entry fills the hole; last itself -> key 0 */
    LK_FUNC("func_80063194");
    D_8009F838_[0] = 3;
    gRemoveGroundKeyEntry(0x111);
    FCK_EQ(D_8009F838_[0], 2, "count 2");
    FCK_EQ(t[0], 0x333, "last moved into slot 0"); FCK_EQ(B8(t, 4), 3, "with its value");
    FCK_EQ(t[2], 0x222, "middle kept");
    gRemoveGroundKeyEntry(0x999);
    FCK_EQ(D_8009F838_[0], 2, "missing key: no change");
    gRemoveGroundKeyEntry(0x222);
    FCK_EQ(D_8009F838_[0], 1, "count 1");
    FCK_EQ(t[2], 0, "removing the last clears its key");
    FCK_EQ(t[0], 0x333, "slot 0 kept");
    for (i = 0; i < 1; i++) gRemoveGroundKeyEntry(0x333);
    FCK_EQ(D_8009F838_[0], 0, "empty");
}

int main(void)
{
    t_random_and_tables();
    t_lists();
    t_registers_and_hooks();
    t_sound();
    t_misc();
    t_ground_keys();
    return lk_done("leaf a2");
}
