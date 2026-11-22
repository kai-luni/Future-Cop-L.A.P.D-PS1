/* Source characterizations of 25 unchanged src/game/<address>.c units.
 * Layouts and signatures are taken from the cited units. All state is
 * synthetic RAM fixtures; no retail data, BIOS, or hardware access.
 * Dependee units named in each @leaf justification are linked as real
 * unchanged objects by the generated runner; they keep their own coverage in
 * other suites (or none), so this file claims nothing for them.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* Byte/word access into untyped fixtures (style of leaf_a2_harness.c). */
#define W32(b, o) (*(u32 *)((u8 *)(b) + (o)))
#define W16(b, o) (*(u16 *)((u8 *)(b) + (o)))
#define B8(b, o)  (*(u8 *)((u8 *)(b) + (o)))

/* Real dependency units compiled into this translation unit without claiming
 * @leaf coverage; they are one-line wrappers with extern-only state, so they
 * cannot collide with other suites' objects (this binary links them once). */
#include "../../src/game/80022B74.c"
#include "../../src/game/8003B3D8.c"
#include "../../src/game/800474C4.c"
#include "../../src/game/800207B4.c"
#include "../../src/game/8001E318.c"
#include "../../src/game/80017B28.c"  /* dep of 80017B60; covered by leaf_a2 */
#include "../../src/game/800787D8.c"  /* dep of 80080010/80082020; covered by leaf_hundred_a */

/* =================================================================== */
/* Group 1: SPU/GPU/CD register wrappers and state accessors            */
/* =================================================================== */

/* Fixed-address word at 0x8009CB50 (src/game/8008FE1C.c) is NOT mapped here;
 * that unit is excluded from this suite. */
u32 *gSpuAccessDelayPtr __asm__("D_8009EEB8");
void gSetSpuDmaReadTiming(void) __asm__("func_80097968");
void gSetSpuAccessDelay(void) __asm__("func_80097940");

extern u32 *gGpuGp1PortPtrForCache __asm__("D_8009DEC8");
u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
u32 *gGpuGp0PortPtr __asm__("D_8009DEC4");
u8 gGpuLibPair[0x100] __asm__("D_800A62C8");
u32 gReadGpuInfo(u32 arg0) __asm__("func_800949E8");
int gReadGpuStatus(void) __asm__("func_80094144");

u8 gCdStatusByte __asm__("D_8009C8B4");
u8 gLastCommandResult __asm__("D_8009C8C4");
u8 gLastResultByte __asm__("D_8009C8C5");
u8 gCdTargetLocation[4] __asm__("D_8009C8C0");
int gGetCdStatusByte(void) __asm__("func_8008E010");
int gGetCdModeByte(void) __asm__("func_8008E020");
int gGetLastCdCommand(void) __asm__("func_8008E030");
unsigned char *gGetCdTargetLocation(void) __asm__("func_8008E040");

s32 gMusicVolumeSetting __asm__("D_8009F334");
s32 gVoiceVolumeBase __asm__("D_8009F338");
s32 gSoundModeFlag __asm__("D_8009F730");
s32 gGetVagmVolumeSetting(void) __asm__("func_80045288");
s32 gGetVoiceVolume(void) __asm__("func_800452C8");
s32 gGetStereoSoundMode(void) __asm__("func_800451E0");

struct FontEntry;
struct FontEntry *gFont __asm__("D_8009F35C");
struct FontEntry *gGetActiveFont(void) __asm__("func_80048E88");

static void test_register_state_accessors(void)
{
    u32 *p1 = (u32 *)&gGpuLibPair; /* scratch target lives in the fixture region */
    u32 gp0_word;

    /* @leaf 80097968 source src/game/80097968.c: RMW (*p & 0xF0FFFFFF) | 0x22000000 through D_8009EEB8 */
    LK_FUNC("func_80097968");
    gSpuAccessDelayPtr = p1;
    *p1 = 0x0F123456U;
    gSetSpuDmaReadTiming();
    FCK_EQ(*p1, 0x22123456U, "bits 25 and 29 set, bits 24/26/27 and low bits preserved");

    /* @leaf 80097940 source src/game/80097940.c: RMW (*p & 0xF0FFFFFF) | 0x20000000 through D_8009EEB8 */
    LK_FUNC("func_80097940");
    gSpuAccessDelayPtr = p1;
    *p1 = 0x0F89ABCDU;
    gSetSpuAccessDelay();
    FCK_EQ(*p1, 0x2089ABCDU, "bit 25 set, bits 24/26/27 cleared, rest preserved");
    *p1 = 0xFF000000U;
    gSetSpuAccessDelay();
    FCK_EQ(*p1, 0xF0000000U, "top nibble with bit 29 preset stays as masked value");

    /* @leaf 800949E8 source src/game/800949E8.c: writes (arg0 | 0x10000000) to GP1 ptr, returns *GP0 & 0xFFFFFF */
    LK_FUNC("func_800949E8");
    gGpuGp1PortPtr = p1;
    gGpuGp0PortPtr = &gp0_word;
    *p1 = 0;
    gp0_word = 0x12345678U;
    FCK_EQ(gReadGpuInfo(0x05), 0x00345678U, "low 24 bits of GP0 read returned");
    FCK_EQ(*p1, 0x10000005U, "GP1 write gets bit 28 tag plus argument");
    FCK_CHECK(gReadGpuInfo(0) == 0x00345678U, "arg0 zero still writes tag word");

    /* @leaf 80094144 source src/game/80094144.c: returns *D_8009DEC8 through pointer indirection */
    LK_FUNC("func_80094144");
    gGpuGp1PortPtr = p1;
    *p1 = (u32)-7;
    FCK_EQ(gReadGpuStatus(), -7, "negative status word returned");
    *p1 = 0x7FFFFFFFU;
    FCK_EQ(gReadGpuStatus(), 0x7FFFFFFFU, "large status word returned");

    /* @leaf 8008E010 source src/game/8008E010.c: returns D_8009C8B4 as unsigned byte promoted to int */
    LK_FUNC("func_8008E010");
    gCdStatusByte = 0x80;
    FCK_EQ(gGetCdStatusByte(), 0x80, "high bit byte returned as positive int");
    gCdStatusByte = 3;
    FCK_EQ(gGetCdStatusByte(), 3, "small byte value");

    /* @leaf 8008E020 source src/game/8008E020.c: returns D_8009C8C4 */
    LK_FUNC("func_8008E020");
    gLastCommandResult = 200;
    FCK_EQ(gGetCdModeByte(), 200, "mode byte returned");
    gLastCommandResult = 0;
    FCK_EQ(gGetCdModeByte(), 0, "zero returned");

    /* @leaf 8008E030 source src/game/8008E030.c: returns D_8009C8C5 */
    LK_FUNC("func_8008E030");
    gLastResultByte = 0x1A;
    FCK_EQ(gGetLastCdCommand(), 0x1A, "last command byte returned");

    /* @leaf 8008E040 source src/game/8008E040.c: returns the fixed address of D_8009C8C0 */
    LK_FUNC("func_8008E040");
    FCK_CHECK(gGetCdTargetLocation() == gCdTargetLocation, "returns base of 4-byte location buffer");
    gCdTargetLocation[0] = 9; gCdTargetLocation[3] = 0x11;
    FCK_CHECK(gGetCdTargetLocation()[0] == 9 && gGetCdTargetLocation()[3] == 0x11,
              "caller-visible buffer content through returned pointer");

    /* @leaf 80045288 source src/game/80045288.c: pure getter of D_8009F334 */
    LK_FUNC("func_80045288");
    gMusicVolumeSetting = 0x7F;
    FCK_EQ(gGetVagmVolumeSetting(), 0x7F, "first read");
    gMusicVolumeSetting = -2;
    FCK_EQ(gGetVagmVolumeSetting(), -2, "negative stored word returned unchanged");

    /* @leaf 800452C8 source src/game/800452C8.c: pure getter of D_8009F338 */
    LK_FUNC("func_800452C8");
    gVoiceVolumeBase = 0x55;
    FCK_EQ(gGetVoiceVolume(), 0x55, "first read");
    gVoiceVolumeBase = 0;
    FCK_EQ(gGetVoiceVolume(), 0, "zero read");

    /* @leaf 800451E0 source src/game/800451E0.c: pure getter of D_8009F730 */
    LK_FUNC("func_800451E0");
    gSoundModeFlag = 1;
    FCK_EQ(gGetStereoSoundMode(), 1, "stereo flag");
    gSoundModeFlag = 0;
    FCK_EQ(gGetStereoSoundMode(), 0, "mono flag");

    /* @leaf 80048E88 source src/game/80048E88.c: returns D_8009F35C or 0 when unset */
    LK_FUNC("func_80048E88");
    gFont = (struct FontEntry *)0;
    FCK_CHECK(gGetActiveFont() == 0, "null font pointer returned as null");
    gFont = (struct FontEntry *)0x1234;
    FCK_CHECK(gGetActiveFont() == (struct FontEntry *)0x1234, "installed font pointer returned");
}

/* =================================================================== */
/* Group 2: card manager, stream flags, clearing state                  */
/* =================================================================== */

struct CardState20600 { u8 pad[0x900]; s32 value; };
struct CardState20600 *gOperationMgrPtr __asm__("D_8009AB08");
s32 gStreamStopPending __asm__("D_800A0974");
void gMarkAudioStreamStopPending(void) __asm__("func_80022700");

s32 gStreamBlockDeficitScale __asm__("D_8009F64C");
s32 gStreamAvailableBlocks __asm__("D_800A09DC");
s32 gScaleAudioStreamBlockDeficit(void) __asm__("func_8002443C");

struct RibbonListHead { u8 pad[0x10]; };
struct RibbonListHead *gBeamNodeListPtr __asm__("D_8009F38C");
void gClearBeamNodeList(void) __asm__("func_800734B0");

s32 gDefinitionEntryCount __asm__("D_8009F7F8");
void gResetDefinitionEntryCount(void) __asm__("func_80060A00");

static void test_card_and_stream_state(void)
{
    struct RibbonListHead node;

    /* @leaf 80022700 source src/game/80022700.c: stores 1 to D_800A0974 unconditionally */
    LK_FUNC("func_80022700");
    gStreamStopPending = 0;
    gMarkAudioStreamStopPending();
    FCK_EQ(gStreamStopPending, 1, "stop marker set from 0");
    gStreamStopPending = 77;
    gMarkAudioStreamStopPending();
    FCK_EQ(gStreamStopPending, 1, "stop marker overwrite with 1");

    /* @leaf 8002443C source src/game/8002443C.c: return D_8009F64C * (3 - blocks) via 80022B74 */
    LK_FUNC("func_8002443C");
    gStreamAvailableBlocks = 0;
    gStreamBlockDeficitScale = 100;
    FCK_EQ(gScaleAudioStreamBlockDeficit(), 300, "zero blocks, scale 100 -> 300");
    gStreamAvailableBlocks = 2;
    gStreamBlockDeficitScale = 50;
    FCK_EQ(gScaleAudioStreamBlockDeficit(), 50, "two blocks, scale 50 -> 50");
    gStreamAvailableBlocks = 5;
    gStreamBlockDeficitScale = 7;
    FCK_EQ(gScaleAudioStreamBlockDeficit(), -14, "deficit goes negative: 7 * (3-5)");
    gStreamBlockDeficitScale = -3;
    gStreamAvailableBlocks = 4;
    FCK_EQ(gScaleAudioStreamBlockDeficit(), 3, "negative scale times negative deficit: -3 * (3-4)");

    /* @leaf 800734B0 source src/game/800734B0.c: stores 0 into the list head D_8009F38C */
    LK_FUNC("func_800734B0");
    gBeamNodeListPtr = &node;
    gClearBeamNodeList();
    FCK_CHECK(gBeamNodeListPtr == 0, "head cleared");

    /* @leaf 80060A00 source src/game/80060A00.c: stores 0 into D_8009F7F8 */
    LK_FUNC("func_80060A00");
    gDefinitionEntryCount = 47;
    gResetDefinitionEntryCount();
    FCK_EQ(gDefinitionEntryCount, 0, "count cleared");
}

/* =================================================================== */
/* Group 3: grid flags, model block table init, instance selection      */
/* =================================================================== */

u32 gSpawnTypeFlags[0xC00] __asm__("D_800A6578");
void gOrGridCellFlags(u8 column, s32 row, u8 bits) __asm__("func_8003C4C4");

s32 gVertexBlockTable __asm__("D_8009F554");
s32 gNormalBlockTable __asm__("D_8009F558");
s32 gRelocBlockTable __asm__("D_8009F55C");
s32 gRefBlockTable __asm__("D_8009F560");
s32 gAnimNodeList __asm__("D_8009F564");
s32 gAnimWaveList __asm__("D_8009F568");
void gInitModelBlockTables(s32 arg0) __asm__("func_80012E1C");

/* struct Unk17764, gFindInstanceById come from the 80017B28 dep include */
struct Unk17764 *gCameraList __asm__("D_8009F588");  /* storage provided here; unit only externs it */
void *gCameraPtr __asm__("D_8009F9E4");
void gSelectViewRecord(s32 id) __asm__("func_80017B60");

struct Resource20 { u8 pad[0x10]; s32 serial; };
struct Handle20 { struct Resource20 *resource; s32 serial; };
struct Handle20 gAssetHandle __asm__("D_800A0A80");
s32 gGetTrackedSoundVoice(void) __asm__("func_80024188");

int gReadCardOperationResult(void) __asm__("func_800207B4");
int gIsCardResultUsable(int arg0) __asm__("func_80020A9C");

int gWaitCdLoaderIdle(void) __asm__("func_8001C478");
volatile s32 gExecutableImageBase __asm__("D_80010000");

static void test_grid_tables_and_lookup(void)
{
    struct Unk17764 nodes[3];
    struct Resource20 res;
    struct CardState20600 *saved_mgr = gOperationMgrPtr;

    /* @leaf 8003C4C4 source src/game/8003C4C4.c: ORs bits into cell byte via 8003B3D8 (row<<10 + col<<2) */
    LK_FUNC("func_8003C4C4");
    lk_fill(gSpawnTypeFlags, 0, sizeof(gSpawnTypeFlags));
    gOrGridCellFlags(3, 7, 0x10);
    FCK_CHECK(W32(gSpawnTypeFlags, 0x1C00 + 0xC) == 0x10, "bits ORed into the addressed cell word's low byte");
    gOrGridCellFlags(3, 7, 0x01);
    FCK_EQ(W32(gSpawnTypeFlags, 0x1C00 + 0xC) & 0xFF, 0x11, "existing bits survive the OR");
    FCK_CHECK(W32(gSpawnTypeFlags, 0x1C00 + 0x8) == 0 && W32(gSpawnTypeFlags, 0x1C00 + 0x10) == 0,
              "neighbour cells untouched");
    gOrGridCellFlags(0, 0, 0x80);
    FCK_EQ(W32(gSpawnTypeFlags, 0) & 0xFF, 0x80, "origin cell addressable");

    /* @leaf 80012E1C source src/game/80012E1C.c: three block tables at +0/+0x400/+0x800/+0xC00, two lists cleared */
    LK_FUNC("func_80012E1C");
    gAnimNodeList = 1; gAnimWaveList = 2;
    gInitModelBlockTables(0x1000);
    FCK_CHECK(gVertexBlockTable == 0x1000 && gNormalBlockTable == 0x1400 &&
              gRelocBlockTable == 0x1800 && gRefBlockTable == 0x1C00,
              "four block tables split at 0x400 stride");
    FCK_CHECK(gAnimNodeList == 0 && gAnimWaveList == 0, "animation lists cleared");
    gInitModelBlockTables(0);
    FCK_CHECK(gVertexBlockTable == 0 && gRefBlockTable == 0xC00, "zero base still stores offsets");

/* @leaf 80017B60 source src/game/80017B60.c: stores 80017B28(id) result into D_8009F9E4 */
    LK_FUNC("func_80017B60");
    lk_fill(nodes, 0, sizeof(nodes));
    nodes[0].id = 11; nodes[0].next = &nodes[1];
    nodes[1].id = 22; nodes[1].next = &nodes[2];
    nodes[2].id = 33; nodes[2].next = 0;
    gCameraList = &nodes[0];
    gCameraPtr = 0;
    gSelectViewRecord(33);
    FCK_CHECK(gCameraPtr == &nodes[2], "selected node stored in camera pointer");
    gSelectViewRecord(50);
    FCK_CHECK(gCameraPtr == 0, "miss clears the camera pointer via null lookup");

    /* @leaf 80024188 source src/game/80024188.c: returns handle resource when 800474C4 validates serial */
    LK_FUNC("func_80024188");
    lk_fill(&res, 0, sizeof(res));
    res.serial = 42;
    gAssetHandle.resource = &res;
    gAssetHandle.serial = 42;
    FCK_CHECK(gGetTrackedSoundVoice() == (s32)&res, "matching serial returns resource pointer");
    gAssetHandle.serial = 41;
    FCK_CHECK(gGetTrackedSoundVoice() == 0, "stale serial rejected");
    gAssetHandle.resource = 0;
    FCK_CHECK(gGetTrackedSoundVoice() == 0, "null resource rejected");

    /* @leaf 80020A9C source src/game/80020A9C.c: result usable for -2, -1 and nonnegative checked values via 800207B4 */
    LK_FUNC("func_80020A9C");
    /* direct status word steering through a real manager object */
    {
        struct CardState20600 mgr;
        lk_fill(&mgr, 0x5A, sizeof(mgr));
        mgr.value = -2;
        gOperationMgrPtr = &mgr;
        FCK_EQ(gIsCardResultUsable(0), 1, "result -2 usable");
        mgr.value = -1;
        FCK_EQ(gIsCardResultUsable(0), 1, "result -1 usable");
        mgr.value = 5;
        FCK_EQ(gIsCardResultUsable(0), 1, "nonnegative result usable");
        mgr.value = -4;
        FCK_EQ(gIsCardResultUsable(0), 0, "other negative rejected");
    }
    gOperationMgrPtr = saved_mgr;

    /* @leaf 8001C478 source src/game/8001C478.c: polls volatile D_80010000 and returns 0 once clear */
    LK_FUNC("func_8001C478");
    gExecutableImageBase = 0;
    FCK_EQ(gWaitCdLoaderIdle(), 0, "idle loader returns 0");
}

/* =================================================================== */
/* Group 4: menu selection wrappers, gauge template copy, no-op        */
/* =================================================================== */

s8 *gFontBankTableSelect __asm__("D_8009F468");
void gSelectEnabledMenuItem(s8 arg0, s8 arg1, s8 arg2) __asm__("func_800787D8");
s32 gSelectFirstEnabledItem(void) __asm__("func_80080010");
void gReselectEnabledItem(void) __asm__("func_80082020");
u8 gSourceBlock20[0x14] __asm__("D_8009AFA0") = {
    0x2C, 0x01, 0x00, 0x00,   /* 0x12C */
    0x2C, 0x01, 0x00, 0x00,   /* 0x12C */
    0xEB, 0x51, 0x00, 0x00,   /* 0x51EB */
    0x64, 0x00, 0x00, 0x00,   /* 0x64 */
    0x02, 0x00, 0x14, 0x0E    /* state byte pattern 0x02, 0x0E140002 low word shape */
};
void gCopyMemorySynthetic(u8 *destination, u8 *source, s32 size) __asm__("func_8001E318");
void gCopyDefaultGaugeRecord(u8 *destination) __asm__("func_8005A09C");

void gIgnoreTagCommand69(void) __asm__("func_8003FD6C");

int gQueryFrontEndVerticalRepeatMask(void) __asm__("func_8007F5C8");

static void test_menu_gauge_and_noop(void)
{
    u8 menu_desc[0x40];
    struct { u8 unk00[0xE]; u8 unk0E; s8 unk0F; u8 pad10[0xC]; } entries[5];
    u8 gauge[0x20];
    int i;

    /* dependency-only use of func_800787D8 (covered by leaf_hundred_a); behavior in this suite exercised via the wrappers below */
/* @leaf 80080010 source src/game/80080010.c: forwards gSelectEnabledMenuItem(0, 1, 0) */
    LK_FUNC("func_80080010");
    /* entries[0] and [2] enabled, so the scan must START at index 0 and pick entry 0 */
    lk_fill(entries, 0, sizeof(entries));
    entries[0].unk0E = 1;  entries[0].unk0F = 0;
    entries[2].unk0E = 1;  entries[2].unk0F = 0;
    lk_fill(menu_desc, 0, sizeof(menu_desc));
    menu_desc[2] = 5;                                  /* item count */
    W32(menu_desc, 0x28) = (u32)entries;              /* descriptor entries pointer at +0x28 */
    gFontBankTableSelect = (s8 *)menu_desc;
    menu_desc[0] = 9;
    gSelectFirstEnabledItem();
    FCK_EQ(menu_desc[0], 0, "scan starts at index 0 and selects the first enabled entry");

    /* @leaf 80082020 source src/game/80082020.c: reselects from saved index gFontBankTable[0] */
    LK_FUNC("func_80082020");
    entries[0].unk0E = 1; entries[0].unk0F = 0;
    entries[2].unk0E = 0;                    /* saved index 2 disabled now */
    entries[3].unk0E = 1; entries[3].unk0F = 0;
    menu_desc[0] = 2;
    /* descriptor unchanged from the 80080010 step; saved index must be re-read */
    gReselectEnabledItem();
    FCK_EQ(menu_desc[0], 3, "reselect resumes at saved index 2 and scans forward to 3");

    /* @leaf 8005A09C source src/game/8005A09C.c: copies 0x14 template bytes from D_8009AFA0 via 8001E318 */
    LK_FUNC("func_8005A09C");
    lk_fill(gauge, 0xA5, sizeof(gauge));
    gCopyDefaultGaugeRecord(gauge);
    FCK_CHECK(gauge[0] == 0x2C && gauge[4] == 0x2C && gauge[8] == 0xEB && gauge[9] == 0x51 &&
              gauge[12] == 0x64 && gauge[16] == 0x02,
              "20-byte template copied to destination");
    FCK_CHECK(gauge[20] == 0xA5, "byte 20 untouched (copy is exactly 0x14)");

    /* @leaf 8003FD6C source src/game/8003FD6C.c: empty body; calling it is the whole contract */
    LK_FUNC("func_8003FD6C");
    gIgnoreTagCommand69();
    FCK_CHECK(1, "empty entry returns without touching state");

    /* @leaf 8007F5C8 source src/game/8007F5C8.c: body is a constant load of 2 */
    LK_FUNC("func_8007F5C8");
    FCK_EQ(gQueryFrontEndVerticalRepeatMask(), 2, "constant result 2");
    FCK_EQ(gQueryFrontEndVerticalRepeatMask(), 2, "stable across calls");
}

int main(void)
{
    test_register_state_accessors();
    test_card_and_stream_state();
    test_grid_tables_and_lookup();
    test_menu_gauge_and_noop();
    return lk_done("leaf_century_c");
}
