/* leaf_core: large hardware-free units from the 0x8008xxxx-0x8009xxxx band
 * (scene voice slots, scene node commands, movie dither tables, GPU fill
 * packets, SPU transfer/key/voice registers, SPU pool allocator).
 *
 * Fixture style: harness-defined D_ labels. Every D_ label a unit reads is
 * defined here and written by the test before each call; nothing relies on
 * zero-init or on retail memory. Register blocks the units reach through a
 * pointer global (SPU, DMA4, GPUSTAT) point at plain harness RAM.
 * Callees outside the suite are bound by their retail label and record what
 * they receive; none of them is provided by tests/host/mocks/. The SPU pool
 * units 80097B28, 800980F8 and 80098458 call the real 80097DF8 (and 80098458
 * the real 800980F8), so their expectations include the compaction pass.
 *
 * Dropped: 8008CB40 (K&R-style free). Its last store goes to the absolute
 * kseg0 address 0x8009C7D0 (`lui 0x800A; sw -0x3830`), not to the linked
 * symbol; qemu-user cannot map 0x8009C000 (MAP_FIXED mmap fails), so the
 * unit faults on every path.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"
#include "leaf_vectors.h"

/* ========================================================================
 * Globals (retail D_ labels; set per test)
 * ======================================================================== */

/* 8008363C scene voice table */
typedef struct {
    void *obj;
    s16 timer;
    s8 voice;
    u8 pad7;
} LcVoiceSlot;
typedef struct {
    u32 free;
    LcVoiceSlot slots[32];
} LcVoiceTable;
typedef struct {
    u8 pad00[0x34];
    s32 x;              /* +0x34 */
    u8 pad38[4];
    s32 z;              /* +0x3C */
    u8 pad40[0xB];
    s8 slot;            /* +0x4B */
    u8 pad4C[8];
    u16 id;             /* +0x54 */
    s8 update;          /* +0x56 */
    u8 sound;           /* +0x57 */
} LcSoundObj;
LcVoiceTable *gMissionTable __asm__("D_8009F930");

/* 800841C4 scene node grid */
typedef struct LcNode {
    u8 pad00[0x4C];
    u8 flags;           /* +0x4C */
    u8 timer;           /* +0x4D */
    u8 pad4E[2];
    s32 field50;        /* +0x50 */
    u16 id;             /* +0x54 */
    u8 pad56[2];
    struct LcNode *next; /* +0x58 */
} LcNode;
typedef struct {
    LcNode *head;
    u32 unk04;
} LcCell;
void *gMissionState __asm__("D_8009F934");
LcCell *gCellBucketGrid __asm__("D_8009F938");
s32 gCellGridWidth __asm__("D_8009F93C");
s32 gCellGridHeight __asm__("D_8009F940");
s32 gFrameTime[2] __asm__("D_8009FAA4");

/* 80085F1C movie dither tables */
s32 gMovieDitherSeedMatrix[4] __asm__("D_8001220C");
u8 gInitGuardFlag __asm__("D_8009F4D4");
u8 gScratchTable[256 + 16] __asm__("D_800A3958");
u8 gSecondScratchTable[264 + 8] __asm__("D_800A3A58");

/* 8009423C GPU fill */
u16 gMaxDrawWidth __asm__("D_8009DDF4");
u16 gMaxDrawHeight __asm__("D_8009DDF6");
u32 *gGpuGp1PortPtr __asm__("D_8009DEC8");
u32 gGpuPacketArray[12] __asm__("D_800A6280");
u32 gGpuPacketArrayAlt[6] __asm__("D_800A62A8");

/* SPU register block pointer shared by 8009745C, 80098C68, 80099368 */
u8 *gSpuRegBasePtr __asm__("D_8009EEA4");
static u16 spu_regs[0x100];            /* 0x200 bytes: voices + 0x188..0x1AB */
#define SPU16(off) (spu_regs[(off) / 2])

/* 8009745C SPU transfer */
s32 gSpuAddrShift __asm__("D_8009EECC");
u16 gSpuTransferAddr __asm__("D_8009EEBC");
s32 gSetupGuard __asm__("D_8009EEF4");
s32 gSpuDmaMainRamAddress __asm__("D_8009EEF8");
s32 gSpuDmaBlockCount __asm__("D_8009EEFC");
s32 *gDma4MemAddrPtr __asm__("D_8009EEA8");
s32 *gDma4BlockCtrlPtr __asm__("D_8009EEAC");
s32 *gDma4ChannelCtrlPtr __asm__("D_8009EEB0");
static s32 dma4[3];

/* SPU pool allocator (80097B28, 80097DF8, 800980F8, 80098458) */
s32 gArenaOffsetFlag __asm__("D_8009EA3C");
s32 gArenaCursor __asm__("D_8009EA40");
s32 gSpuAlignMask __asm__("D_8009EED4");
s32 gSpuEntryTableCapacity __asm__("D_8009EF04");
u32 *gSpuEntryTable __asm__("D_8009EF0C");
/* 80098458 stores the record count through `lui %hi(D_8009EF08)` with the
 * fixed offset -4344 (= %lo(0x8009EF08)), i.e. it assumes the label's low 16
 * bits are 0xEF08. Place the harness definition at such an address so that
 * store lands on the label, as it does in retail. */
__asm__(".data\n"
        "\t.balign 65536\n"
        "\t.space 0xEF08\n"
        "\t.globl D_8009EF08\n"
        "D_8009EF08:\n"
        "\t.word 0\n"
        "\t.text\n");
extern s32 gTableCursor __asm__("D_8009EF08");
static u32 pool[2 * 10];

/* 80098C68 SPU key on/off */
u32 gSpuRegisterShadowMode __asm__("D_8009EE90");
u32 gSpuBufferedKeyOnVoiceMask __asm__("D_8009EA58");
u32 gModeEntryWord __asm__("D_8009EA5C");
u32 gSpuDirectKeyOnVoiceMask __asm__("D_8009EA30");
u16 gSpuInitWord[4] __asm__("D_800A7028");

/* 80099368 SPU voice attributes */
u16 gVoiceSampleNote[24 + 1] __asm__("D_8009EA60");

/* ========================================================================
 * Stand-ins (retail labels, record arguments)
 * ======================================================================== */

static s32 alloc_ret, alloc_calls, alloc_args[5];
s32 lc_alloc_channel(s32 a, s32 b, s32 c, s32 d, s32 e) __asm__("func_80045678");
s32 lc_alloc_channel(s32 a, s32 b, s32 c, s32 d, s32 e)
{
    alloc_calls++;
    alloc_args[0] = a; alloc_args[1] = b; alloc_args[2] = c;
    alloc_args[3] = d; alloc_args[4] = e;
    return alloc_ret;
}

static s32 pos_calls, pos_args[5];
void lc_set_channel_pos(s32 a, s32 b, s32 c, s32 d, s32 e) __asm__("func_8004572C");
void lc_set_channel_pos(s32 a, s32 b, s32 c, s32 d, s32 e)
{
    pos_calls++;
    pos_args[0] = a; pos_args[1] = b; pos_args[2] = c;
    pos_args[3] = d; pos_args[4] = e;
}

static s32 unlink_calls;
static void *unlink_state, *unlink_node;
void func_800195D0(void *state, int *link)
{
    unlink_calls++;
    unlink_state = state;
    unlink_node = link;
}

static s32 info_calls, info_args[4];
u32 lc_read_gpu_info(u32 n) __asm__("func_800949E8");
u32 lc_read_gpu_info(u32 n)
{
    if (info_calls < 4) info_args[info_calls] = n;
    info_calls++;
    return 0x00012340u + n;
}

static s32 dma_calls;
static u32 dma_arg;
void lc_start_gpu_dma(u32 p) __asm__("func_800949A0");
void lc_start_gpu_dma(u32 p)
{
    dma_calls++;
    dma_arg = p;
}

static s32 delay_calls, read_timing_calls;
void lc_spu_access_delay(void) __asm__("func_80097940");
void lc_spu_access_delay(void) { delay_calls++; }
void lc_spu_read_timing(void) __asm__("func_80097968");
void lc_spu_read_timing(void) { read_timing_calls++; }

static s32 n2p_calls, n2p_args[4];
static u16 n2p_ret;
u16 lc_note_to_pitch(s32 a, s32 b, s32 c, s32 d) __asm__("func_800999F4");
u16 lc_note_to_pitch(s32 a, s32 b, s32 c, s32 d)
{
    n2p_calls++;
    n2p_args[0] = a; n2p_args[1] = b; n2p_args[2] = c; n2p_args[3] = d;
    return n2p_ret;
}

static s32 wa_calls, wa_reg[4];
static u32 wa_val[4];
void lc_spu_write_addr(s32 reg, u32 v) __asm__("func_80097808");
void lc_spu_write_addr(s32 reg, u32 v)
{
    if (wa_calls < 4) { wa_reg[wa_calls] = reg; wa_val[wa_calls] = v; }
    wa_calls++;
}

/* ========================================================================
 * Units under test
 * ======================================================================== */

void gAssignSceneSoundVoice(LcSoundObj *obj) __asm__("func_8008363C");
void gSendSceneNodeCommandById(s32 id, s32 mode, s32 index) __asm__("func_800841C4");
void gMovieBuildDitherTables(void) __asm__("func_80085F1C");
s32 gSubmitGpuRectangleFill(u16 *arg0, u32 arg1) __asm__("func_8009423C");
s32 gSpuTransfer(s32 arg0, ...) __asm__("func_8009745C");
s32 gAllocSpuRamBlock(s32 size) __asm__("func_80097B28");
void gSpuCompactPool(void) __asm__("func_80097DF8");
s32 gSpuSplitPoolBlock(s32 a0, u32 a1, s32 a2) __asm__("func_800980F8");
s32 gSpuAllocAt(s32 arg, s32 size) __asm__("func_80098458");
void gSpuSetVoiceKey(int mode, u32 mask) __asm__("func_80098C68");

typedef struct { s16 left, right; } LcVol;
typedef struct {
    u32 voice;
    u32 mask;
    LcVol volume;
    LcVol volmode;
    LcVol volumex;
    u16 pitch;
    u16 note;
    u16 sample_note;
    s16 envx;
    u32 addr;
    u32 loop_addr;
    s32 a_mode;
    s32 s_mode;
    s32 r_mode;
    u16 ar, dr, sr, rr, sl, adsr1, adsr2;
} LcVoiceAttr;
void gSpuSetVoiceAttributes(LcVoiceAttr *arg) __asm__("func_80099368");

/* ======================================================================== */

static void test_scene_voice(void)
{
    static LcVoiceTable table;
    static LcSoundObj obj;

    /* @leaf 8008363C source src/game/8008363C.c: lowest set bit of the 32-slot free mask (binary search), new slot timer 300, refresh 900, position update only when +0x56 is set */
    LK_FUNC("func_8008363C");

    /* no free slot: nothing allocated */
    lk_fill(&table, 0xA5, sizeof table);
    lk_fill(&obj, 0, sizeof obj);
    table.free = 0;
    obj.slot = -1;
    alloc_calls = 0; alloc_ret = 9;
    gMissionTable = &table;
    gAssignSceneSoundVoice(&obj);
    FCK_EQ(alloc_calls, 0, "empty mask: no channel allocated");
    FCK_EQ(obj.slot, -1, "empty mask: object keeps slot -1");
    FCK_CHECK(lk_all(&table.slots, 0, sizeof table.slots, 0xA5), "empty mask: table untouched");

    /* free bits 5 and 7: bit 5 is taken */
    lk_fill(&table, 0xA5, sizeof table);
    table.free = 0xA0;
    obj.slot = -1; obj.sound = 0x21; obj.id = 0x0123; obj.x = 1000; obj.z = -2000;
    alloc_calls = 0; alloc_ret = 9;
    gAssignSceneSoundVoice(&obj);
    FCK_EQ(alloc_calls, 1, "channel allocated once");
    FCK_EQ(alloc_args[0], 0x21, "sound id from +0x57");
    FCK_EQ(alloc_args[1], 0x40000123, "id | 0x40000000");
    FCK_EQ(alloc_args[2], 1000, "x from +0x34");
    FCK_EQ(alloc_args[3], -2000, "z from +0x3C");
    FCK_EQ(alloc_args[4], -1, "fifth argument -1");
    FCK_EQ(table.free, 0x80, "bit 5 cleared");
    FCK_EQ(obj.slot, 5, "lowest free slot is 5");
    FCK_CHECK(table.slots[5].obj == &obj, "slot points at the object");
    FCK_EQ(table.slots[5].timer, 300, "new slot timer 300");
    FCK_EQ(table.slots[5].voice, 9, "voice stored");
    FCK_EQ(table.slots[7].timer, (s16)0xA5A5, "slot 7 untouched");

    /* only bit 31 free */
    table.free = 0x80000000u;
    obj.slot = -1;
    alloc_ret = 3;
    gAssignSceneSoundVoice(&obj);
    FCK_EQ(obj.slot, 31, "bit 31 found");
    FCK_EQ(table.free, 0, "mask empty afterwards");
    FCK_EQ(table.slots[31].voice, 3, "voice in slot 31");

    /* bit 0 */
    table.free = 0xFFFFFFFFu;
    obj.slot = -1;
    gAssignSceneSoundVoice(&obj);
    FCK_EQ(obj.slot, 0, "bit 0 found first");
    FCK_EQ(table.free, 0xFFFFFFFEu, "only bit 0 cleared");

    /* allocation fails */
    table.free = 0x10;
    obj.slot = -1;
    alloc_ret = -1;
    gAssignSceneSoundVoice(&obj);
    FCK_EQ(obj.slot, -1, "failed channel: slot unchanged");
    FCK_EQ(table.free, 0x10, "failed channel: mask unchanged");

    /* refresh without position update */
    table.slots[3].timer = 7;
    table.slots[3].voice = 0x15;
    obj.slot = 3; obj.update = 0;
    pos_calls = 0; alloc_calls = 0;
    gAssignSceneSoundVoice(&obj);
    FCK_EQ(table.slots[3].timer, 900, "refresh sets 900");
    FCK_EQ(pos_calls, 0, "no position update when +0x56 is 0");
    FCK_EQ(alloc_calls, 0, "refresh does not allocate");

    /* refresh with position update */
    obj.update = 1; obj.id = 0x0456; obj.x = 11; obj.z = 22;
    gAssignSceneSoundVoice(&obj);
    FCK_EQ(pos_calls, 1, "position updated");
    FCK_EQ(pos_args[0], 0x40000456, "id | 0x40000000");
    FCK_EQ(pos_args[1], 11, "x");
    FCK_EQ(pos_args[2], 22, "z");
    FCK_EQ(pos_args[3], -1, "fourth argument -1");
    FCK_EQ(pos_args[4], 0x15, "slot voice passed");

    /* slot 0 is a valid existing slot */
    table.free = 0xFFFFFFFFu;
    table.slots[0].timer = 1;
    obj.slot = 0; obj.update = 0;
    alloc_calls = 0;
    gAssignSceneSoundVoice(&obj);
    FCK_EQ(table.slots[0].timer, 900, "slot 0 refreshed");
    FCK_EQ(alloc_calls, 0, "slot 0 does not allocate");
    FCK_EQ(table.free, 0xFFFFFFFFu, "slot 0 refresh leaves the mask");
}

static LcNode nodes[4];
static LcCell cells[4];
static int mission_state_dummy;

static void node_fixture(void)
{
    int i;
    lk_fill(nodes, 0, sizeof nodes);
    lk_fill(cells, 0, sizeof cells);
    /* cell 3: A(id 5) -> B(id 7); cell 1: C(id 7); cell 0: D(id 9) */
    nodes[0].id = 5; nodes[0].next = &nodes[1];
    nodes[1].id = 7; nodes[1].next = 0;
    nodes[2].id = 7; nodes[2].next = 0;
    nodes[3].id = 9; nodes[3].next = 0;
    for (i = 0; i < 4; i++) { nodes[i].flags = 0x66; nodes[i].timer = 0x11; nodes[i].field50 = 0x1234; }
    cells[3].head = &nodes[0];
    cells[1].head = &nodes[2];
    cells[0].head = &nodes[3];
    gCellBucketGrid = cells;
    gCellGridWidth = 2;
    gCellGridHeight = 2;
    gMissionState = &mission_state_dummy;
    gFrameTime[0] = 1000;
    gFrameTime[1] = 0;
    unlink_calls = 0; unlink_state = 0; unlink_node = 0;
}

static void test_node_command(void)
{
    /* @leaf 800841C4 source src/game/800841C4.c: cells scanned high to low, first id match only; 0x19 unlinks or sets deadline, 0x1A/0x1B toggle flag bits */
    LK_FUNC("func_800841C4");

    node_fixture();
    gSendSceneNodeCommandById(7, 0x19, 0);
    FCK_EQ(unlink_calls, 1, "0x19/0: unlink callback once");
    FCK_CHECK(unlink_node == &nodes[1], "highest cell's match (B) removed first");
    FCK_CHECK(unlink_state == &mission_state_dummy, "mission state passed");
    FCK_CHECK(nodes[0].next == 0, "predecessor skips B");
    FCK_CHECK(cells[3].head == &nodes[0], "cell head unchanged for inner node");
    FCK_CHECK(cells[1].head == &nodes[2], "lower-cell match untouched");

    node_fixture();
    gSendSceneNodeCommandById(5, 0x19, 0);
    FCK_CHECK(cells[3].head == &nodes[1], "head match: cell head advances");
    FCK_CHECK(unlink_node == &nodes[0], "head node handed over");

    node_fixture();
    gSendSceneNodeCommandById(9, 0x19, 2);
    FCK_EQ(nodes[3].field50, 1600, "deadline = frame + index * 300");
    FCK_EQ(nodes[3].flags, 0x66, "flag 0x80 cleared (was clear)");
    FCK_EQ(unlink_calls, 0, "positive index does not unlink");
    nodes[3].flags = 0xFF;
    gSendSceneNodeCommandById(9, 0x19, -4);
    FCK_EQ(nodes[3].field50, -1, "negative index: deadline -1");
    FCK_EQ(nodes[3].flags, 0x7F, "negative index: bit 0x80 cleared");

    node_fixture();
    gSendSceneNodeCommandById(9, 0x1A, 0);
    FCK_EQ(nodes[3].flags, 0x6E, "0x1A/0 sets 0x08");
    FCK_EQ(nodes[3].timer, 0x11, "0x1A/0 keeps timer");
    nodes[3].flags = 0xFF;
    gSendSceneNodeCommandById(9, 0x1A, 7);
    FCK_EQ(nodes[3].timer, 42, "0x1A timer = 300 / index");
    FCK_EQ(nodes[3].flags, 0xF7, "0x1A clears 0x08");

    node_fixture();
    gSendSceneNodeCommandById(7, 0x1B, 0);
    FCK_EQ(nodes[1].flags, 0x76, "0x1B/0 sets 0x10 on the first match");
    FCK_EQ(nodes[2].flags, 0x66, "second match in a lower cell untouched");
    nodes[1].flags = 0xFF;
    gSendSceneNodeCommandById(7, 0x1B, 1);
    FCK_EQ(nodes[1].flags, 0xEF, "0x1B/1 clears 0x10");

    node_fixture();
    gSendSceneNodeCommandById(9, 0x20, 0);
    FCK_EQ(nodes[3].flags, 0x66, "unknown command: flags unchanged");
    FCK_EQ(nodes[3].field50, 0x1234, "unknown command: field unchanged");
    FCK_EQ(unlink_calls, 0, "unknown command: no unlink");

    node_fixture();
    gSendSceneNodeCommandById(0x1234, 0x19, 0);
    FCK_EQ(unlink_calls, 0, "no match: nothing removed");
    FCK_CHECK(cells[3].head == &nodes[0] && nodes[0].next == &nodes[1], "no match: lists intact");

    node_fixture();
    gCellGridWidth = 1;
    gCellGridHeight = 1;
    gSendSceneNodeCommandById(5, 0x19, 0);
    FCK_EQ(unlink_calls, 0, "1x1 grid scans only cell 0");
}

static void test_dither_tables(void)
{
    int i, ok;

    /* @leaf 80085F1C math tests/host/leaf_vectors.py bayer16: recursive 2x2 refinement equals the bit-interleaved Bayer closed form, >>5; second table k>>3 clamped to 31 */
    LK_FUNC("func_80085F1C");

    for (i = 0; i < 4; i++) gMovieDitherSeedMatrix[i] = lv_bayer_std_seed[i];
    gInitGuardFlag = 0;
    lk_fill(gScratchTable, 0xEE, sizeof gScratchTable);
    lk_fill(gSecondScratchTable, 0xEE, sizeof gSecondScratchTable);
    gMovieBuildDitherTables();
    ok = 1;
    for (i = 0; i < 256; i++) if (gScratchTable[i] != lv_bayer_std[i]) ok = 0;
    FCK_CHECK(ok, "standard seed: 16x16 table matches bayer16");
    FCK_EQ(gScratchTable[0x11], lv_bayer_std[0x11], "(1,1) sample");
    FCK_CHECK(lk_all(gScratchTable, 256, 256 + 16, 0xEE), "nothing past 256 bytes");
    ok = 1;
    for (i = 0; i < 256; i++) if (gSecondScratchTable[i] != (i >> 3)) ok = 0;
    FCK_CHECK(ok, "second table k >> 3");
    ok = 1;
    for (i = 256; i < 264; i++) if (gSecondScratchTable[i] != 31) ok = 0;
    FCK_CHECK(ok, "entries 256..263 clamp to 31");
    FCK_CHECK(lk_all(gSecondScratchTable, 264, 264 + 8, 0xEE), "nothing past 264 bytes");
    FCK_EQ(gInitGuardFlag, 1, "built-once flag set");

    for (i = 0; i < 4; i++) gMovieDitherSeedMatrix[i] = lv_bayer_wrap_seed[i];
    gInitGuardFlag = 0;
    lk_fill(gScratchTable, 0xEE, sizeof gScratchTable);
    gMovieBuildDitherTables();
    ok = 1;
    for (i = 0; i < 256; i++) if (gScratchTable[i] != lv_bayer_wrap[i]) ok = 0;
    FCK_CHECK(ok, "wrapping seed: all four refinement levels match bayer16");

    lk_fill(gScratchTable, 0xEE, sizeof gScratchTable);
    lk_fill(gSecondScratchTable, 0xEE, sizeof gSecondScratchTable);
    gInitGuardFlag = 1;
    gMovieBuildDitherTables();
    FCK_CHECK(lk_all(gScratchTable, 0, sizeof gScratchTable, 0xEE), "flag set: first table untouched");
    FCK_CHECK(lk_all(gSecondScratchTable, 0, sizeof gSecondScratchTable, 0xEE), "flag set: second table untouched");
}

static void test_gpu_fill(void)
{
    static u16 rect[4];
    static u32 gpustat;
    s32 r;

    /* @leaf 8009423C source src/game/8009423C.c: w/h clamped to [0, max-1]; 64-aligned x and w use the 5-word GP0(02h) fill packet, otherwise a GP0(60h) rectangle with E3/E4/E5 restored from GPU info 3/4/5 */
    LK_FUNC("func_8009423C");

    gMaxDrawWidth = 1024;
    gMaxDrawHeight = 512;
    gpustat = 0x1234ABCDu;
    gGpuGp1PortPtr = &gpustat;
    lk_fill(gGpuPacketArray, 0xCC, sizeof gGpuPacketArray);
    lk_fill(gGpuPacketArrayAlt, 0xCC, sizeof gGpuPacketArrayAlt);
    rect[0] = 0x40; rect[1] = 0x10; rect[2] = 0x100; rect[3] = 0x80;
    dma_calls = 0; info_calls = 0;
    r = gSubmitGpuRectangleFill(rect, 0x80332211u);
    FCK_EQ(r, 0, "returns 0");
    FCK_EQ(gGpuPacketArray[0], 0x05FFFFFFu, "aligned: tag 5 words, end of list");
    FCK_EQ(gGpuPacketArray[1], 0xE6000000u, "aligned: E6 mask bits off");
    FCK_EQ(gGpuPacketArray[2], 0xE10007CDu, "aligned: E1 = GPUSTAT&0x7FF | color bit31 -> bit 10");
    FCK_EQ(gGpuPacketArray[3], 0x02332211u, "aligned: GP0(02h) fill colour");
    FCK_EQ(gGpuPacketArray[4], 0x00100040u, "aligned: y<<16 | x");
    FCK_EQ(gGpuPacketArray[5], 0x00800100u, "aligned: h<<16 | w");
    FCK_EQ(gGpuPacketArray[6], 0xCCCCCCCCu, "aligned: sixth word untouched");
    FCK_EQ(info_calls, 0, "aligned: no GPU info reads");
    FCK_EQ(dma_calls, 1, "packet submitted once");
    FCK_EQ(dma_arg, (u32)gGpuPacketArray, "packet base submitted");

    gMaxDrawWidth = 320;
    gMaxDrawHeight = 240;
    lk_fill(gGpuPacketArray, 0xCC, sizeof gGpuPacketArray);
    rect[0] = 0x40; rect[1] = 0x20; rect[2] = 400; rect[3] = 0xFFFB;
    dma_calls = 0; info_calls = 0;
    r = gSubmitGpuRectangleFill(rect, 0x00445566u);
    FCK_EQ(r, 0, "returns 0");
    FCK_EQ(rect[2], 319, "w above max clamps to max-1");
    FCK_EQ(rect[3], 0, "negative h clamps to 0");
    FCK_EQ(gGpuPacketArray[0], ((u32)gGpuPacketArrayAlt & 0xFFFFFF) | 0x08000000u, "unaligned: 8 words, link to the restore packet");
    FCK_EQ(gGpuPacketArray[1], 0xE3000000u, "E3 area top-left 0");
    FCK_EQ(gGpuPacketArray[2], 0xE4FFFFFFu, "E4 area bottom-right max");
    FCK_EQ(gGpuPacketArray[3], 0xE5000000u, "E5 offset 0");
    FCK_EQ(gGpuPacketArray[4], 0xE6000000u, "E6");
    FCK_EQ(gGpuPacketArray[5], 0xE10003CDu, "E1 without bit 10 for clear colour bit 31");
    FCK_EQ(gGpuPacketArray[6], 0x60445566u, "GP0(60h) monochrome rectangle");
    FCK_EQ(gGpuPacketArray[7], 0x00200040u, "xy");
    FCK_EQ(gGpuPacketArray[8], 0x0000013Fu, "clamped wh");
    FCK_EQ(gGpuPacketArray[9], 0xCCCCCCCCu, "word 9 untouched");
    FCK_EQ(gGpuPacketArrayAlt[0], 0x03FFFFFFu, "restore packet: 3 words, end");
    FCK_EQ(info_calls, 3, "three GPU info reads");
    FCK_CHECK(info_args[0] == 3 && info_args[1] == 4 && info_args[2] == 5, "info indices 3, 4, 5");
    FCK_EQ(gGpuPacketArrayAlt[1], 0xE3012343u, "E3 | info(3)");
    FCK_EQ(gGpuPacketArrayAlt[2], 0xE4012344u, "E4 | info(4)");
    FCK_EQ(gGpuPacketArrayAlt[3], 0xE5012345u, "E5 | info(5)");
    FCK_EQ(dma_arg, (u32)gGpuPacketArray, "head packet submitted");

    rect[0] = 0; rect[1] = 0; rect[2] = 319; rect[3] = 239;
    gSubmitGpuRectangleFill(rect, 0);
    FCK_EQ(rect[2], 319, "w = max-1 kept");
    FCK_EQ(rect[3], 239, "h = max-1 kept");
    rect[2] = 320; rect[3] = 240;
    gSubmitGpuRectangleFill(rect, 0);
    FCK_EQ(rect[2], 319, "w = max clamps");
    FCK_EQ(rect[3], 239, "h = max clamps");

    /* unaligned packet with colour bit 31 */
    lk_fill(gGpuPacketArray, 0xCC, sizeof gGpuPacketArray);
    rect[0] = 0x10; rect[1] = 0; rect[2] = 8; rect[3] = 8;
    gSubmitGpuRectangleFill(rect, 0x80000001u);
    FCK_EQ(gGpuPacketArray[5], 0xE10007CDu, "unaligned: colour bit 31 -> E1 bit 10");
    FCK_EQ(gGpuPacketArray[6], 0x60000001u, "unaligned: bit 31 not in the colour word");

    /* x aligned but w not: unaligned packet */
    gMaxDrawWidth = 1024;
    gMaxDrawHeight = 512;
    lk_fill(gGpuPacketArray, 0xCC, sizeof gGpuPacketArray);
    rect[0] = 0x80; rect[1] = 0; rect[2] = 0x41; rect[3] = 1;
    gSubmitGpuRectangleFill(rect, 0);
    FCK_EQ(gGpuPacketArray[6], 0x60000000u, "w not 64-aligned selects GP0(60h)");
}

static void spu_reset(void)
{
    lk_fill(spu_regs, 0, sizeof spu_regs);
    gSpuRegBasePtr = (u8 *)spu_regs;
}

static void test_spu_transfer(void)
{
    s32 r;

    /* @leaf 8009745C source src/game/8009745C.c: op 2 stores addr>>shift at +0x1A6, op 0/1 poll +0x1A6 then set SPUCNT bits 4-5 to 3/2, op 3 programs DMA4 in 64-byte blocks; 0xF01 polls then -2 */
    LK_FUNC("func_8009745C");

    spu_reset();
    gSpuAddrShift = 3;
    gSpuTransferAddr = 0;
    r = gSpuTransfer(2, 0x1000);
    FCK_EQ(r, 0, "op 2 returns 0");
    FCK_EQ(gSpuTransferAddr, 0x200, "op 2: shifted address kept");
    FCK_EQ(SPU16(0x1A6), 0x200, "op 2: transfer address register");

    /* op 1: write mode */
    spu_reset();
    gSpuTransferAddr = 0x345;
    SPU16(0x1A6) = 0x345;
    SPU16(0x1AA) = 0xFFFF;
    gSetupGuard = 7;
    r = gSpuTransfer(1);
    FCK_EQ(r, 0, "op 1 returns 0");
    FCK_EQ(gSetupGuard, 0, "op 1 mode 0");
    FCK_EQ(SPU16(0x1AA), 0xFFEF, "op 1: SPUCNT bits 4-5 = 2");

    SPU16(0x1A6) = 0x344;
    SPU16(0x1AA) = 0x1234;
    r = gSpuTransfer(1);
    FCK_EQ(r, -2, "op 1: address never confirmed -> -2");
    FCK_EQ(SPU16(0x1AA), 0x1234, "op 1 timeout: SPUCNT untouched");

    /* op 0: read mode */
    SPU16(0x1A6) = 0x345;
    SPU16(0x1AA) = 0x0001;
    gSetupGuard = 7;
    r = gSpuTransfer(0);
    FCK_EQ(r, 0, "op 0 returns 0");
    FCK_EQ(gSetupGuard, 1, "op 0 mode 1");
    FCK_EQ(SPU16(0x1AA), 0x0031, "op 0: SPUCNT bits 4-5 = 3");

    /* op 3, mode 1 (read) */
    gDma4MemAddrPtr = &dma4[0];
    gDma4BlockCtrlPtr = &dma4[1];
    gDma4ChannelCtrlPtr = &dma4[2];
    dma4[0] = dma4[1] = dma4[2] = 0x5A5A5A5A;
    gSetupGuard = 1;
    SPU16(0x1AA) = 0x0030;
    delay_calls = 0; read_timing_calls = 0;
    gSpuDmaMainRamAddress = 0; gSpuDmaBlockCount = 0;
    r = gSpuTransfer(3, 0x80010000, 0x1001);
    FCK_EQ(r, 0, "op 3 returns 0");
    FCK_EQ(read_timing_calls, 1, "mode 1: read timing prepared");
    FCK_EQ(delay_calls, 0, "mode 1: no write delay");
    FCK_EQ(gSpuDmaMainRamAddress, (s32)0x80010000, "RAM address kept");
    FCK_EQ(gSpuDmaBlockCount, 0x41, "0x1001 bytes = 0x41 blocks of 64");
    FCK_EQ(dma4[0], (s32)0x80010000, "MADR");
    FCK_EQ(dma4[1], 0x00410010, "BCR = blocks<<16 | 16 words");
    FCK_EQ(dma4[2], 0x01000200, "CHCR to RAM");

    /* op 3, mode 0 (write) */
    gSetupGuard = 0;
    SPU16(0x1AA) = 0x0020;
    delay_calls = 0; read_timing_calls = 0;
    r = gSpuTransfer(3, 0x80020000, 0x80);
    FCK_EQ(r, 0, "op 3 write returns 0");
    FCK_EQ(delay_calls, 1, "mode 0: write delay prepared");
    FCK_EQ(read_timing_calls, 0, "mode 0: no read timing");
    FCK_EQ(gSpuDmaBlockCount, 2, "0x80 bytes = 2 blocks");
    FCK_EQ(dma4[1], 0x00020010, "BCR");
    FCK_EQ(dma4[2], 0x01000201, "CHCR from RAM");

    /* op 3 timeout */
    dma4[0] = dma4[1] = dma4[2] = 0x5A5A5A5A;
    gSetupGuard = 1;
    SPU16(0x1AA) = 0x0020;
    delay_calls = 0; read_timing_calls = 0;
    r = gSpuTransfer(3, 0x80020000, 0x80);
    FCK_EQ(r, -2, "op 3: SPUCNT mode mismatch -> -2");
    FCK_EQ(read_timing_calls + delay_calls, 0, "op 3 timeout: nothing prepared");
    FCK_EQ(dma4[2], 0x5A5A5A5A, "op 3 timeout: CHCR untouched");

    /* unknown op */
    gSetupGuard = 5;
    gSpuTransferAddr = 0x77;
    r = gSpuTransfer(9, 1, 2);
    FCK_EQ(r, 0, "unknown op returns 0");
    FCK_EQ(gSetupGuard, 5, "unknown op: mode unchanged");
    FCK_EQ(gSpuTransferAddr, 0x77, "unknown op: address unchanged");
}

/* ---- SPU pool fixtures ---- */

static void pool_set(s32 cursor, s32 cap, const u32 *rec, int n)
{
    int i;
    for (i = 0; i < 20; i++) pool[i] = 0xDEAD0000u + i;
    for (i = 0; i < 2 * n; i++) pool[i] = rec[i];
    gSpuEntryTable = pool;
    gTableCursor = cursor;
    gSpuEntryTableCapacity = cap;
    gArenaOffsetFlag = 0;
    gArenaCursor = 0x10000;
    gSpuAddrShift = 3;
    gSpuAlignMask = 7;
}

static int pool_is(const u32 *rec, int n)
{
    int i;
    for (i = 0; i < 2 * n; i++) if (pool[i] != rec[i]) return 0;
    return 1;
}

static void test_spu_compact(void)
{
    /* @leaf 80097DF8 source src/game/80097DF8.c: merges contiguous free records, tombstones size 0, sorts by address up to the tail, drops the first tombstone, folds trailing free records into the tail */
    LK_FUNC("func_80097DF8");
    {
        static const u32 in[] = { 0x00000000, 0x100, 0x80000100, 0x80, 0x80000180, 0x40, 0x400001C0, 0x1000 };
        static const u32 out[] = { 0x00000000, 0x100, 0x40000100, 0x10C0 };
        pool_set(3, 8, in, 4);
        gSpuCompactPool();
        FCK_EQ(gTableCursor, 1, "merge+fold: two records left");
        FCK_CHECK(pool_is(out, 2), "merged free run folded into the tail");
    }
    {
        static const u32 in[] = { 0x00000200, 0x40, 0x00000000, 0x80, 0x00000100, 0, 0x40000240, 0x500 };
        static const u32 out[] = { 0x00000000, 0x80, 0x00000200, 0x40, 0x40000240, 0x500 };
        pool_set(3, 8, in, 4);
        gSpuCompactPool();
        FCK_EQ(gTableCursor, 2, "zero-size record dropped");
        FCK_CHECK(pool_is(out, 3), "records sorted by address, tail moved into the tombstone");
    }
    {
        static const u32 in[] = { 0x80000000, 0x40, 0x00000080, 0x40, 0x400000C0, 0x100 };
        pool_set(2, 8, in, 3);
        gSpuCompactPool();
        FCK_EQ(gTableCursor, 2, "free record before an allocated one stays");
        FCK_CHECK(pool_is(in, 3), "no change when nothing is contiguous-free or trailing");
    }
    {
        static const u32 in[] = { 0x80000000, 0x40, 0x80000050, 0x30, 0x40000080, 0x100 };
        static const u32 out[] = { 0x40000000, 0x170 };
        pool_set(2, 8, in, 3);
        gSpuCompactPool();
        FCK_EQ(gTableCursor, 0, "trailing free records fold without a contiguity test");
        FCK_CHECK(pool_is(out, 1), "tail starts at the first folded record with summed size");
        FCK_EQ(pool[2], 0x40000050, "slot 1 left as stale tail record");
    }
    {
        static const u32 in[] = { 0x80000000, 0x40, 0x80000040, 0x40, 0x00000200, 0x10 };
        pool_set(0, 8, in, 3);
        gSpuCompactPool();
        FCK_EQ(pool[1], 0x80, "cursor 0: record 0 still merged with record 1");
        FCK_EQ(pool[2], 0x2FFFFFFF, "cursor 0: merged record tombstoned");
    }
    {
        static const u32 in[] = { 0x00000000, 0x40, 0x80000040, 0x40, 0x80000080, 0x40, 0x00000200, 0x10 };
        pool_set(1, 8, in, 4);
        gSpuCompactPool();
        FCK_EQ(pool[3], 0x80, "record at the cursor merges with the next one (scan includes the cursor)");
        FCK_EQ(pool[4], 0x2FFFFFFF, "record past the cursor tombstoned");
    }
    {
        static const u32 in[] = { 0x00000100, 0x10, 0x00000100, 0x20, 0x40000200, 0x100 };
        pool_set(2, 8, in, 3);
        gSpuCompactPool();
        FCK_CHECK(pool_is(in, 3), "equal addresses are not swapped");
    }
}

static void test_spu_alloc(void)
{
    s32 r;

    /* @leaf 80097B28 source src/game/80097B28.c: size rounded up to 8 (below 8 to 0), untouched pool cut from record 0, freed block split with the remainder at the cursor, tail must exceed the reserve; with the real 80097DF8 */
    LK_FUNC("func_80097B28");
    {
        static const u32 in[] = { 0x40001000, 0x10000 };
        static const u32 out1[] = { 0x00001000, 0x88, 0x40001088, 0xFF78 };
        static const u32 out2[] = { 0x00001000, 0x88, 0x00001088, 0x40, 0x400010C8, 0xFF38 };
        pool_set(0, 8, in, 1);
        r = gAllocSpuRamBlock(0x81);
        FCK_EQ(r, 0x1000, "first block at the pool start");
        FCK_EQ(gTableCursor, 1, "tail record appended");
        FCK_CHECK(pool_is(out1, 2), "0x81 rounded to 0x88, tail shrinks");
        r = gAllocSpuRamBlock(0x40);
        FCK_EQ(r, 0x1088, "second block follows the first");
        FCK_EQ(gTableCursor, 2, "second tail record");
        FCK_CHECK(pool_is(out2, 3), "table after two allocations");
    }
    {
        static const u32 in[] = { 0x80001000, 0x100, 0x00001100, 0x40, 0x40001140, 0x1000, 0xAAAA0000, 0x5555 };
        static const u32 out[] = { 0x00001000, 0x40, 0x80001040, 0xC0, 0x00001100, 0x40, 0x40001140, 0x1000 };
        pool_set(2, 8, in, 4);
        r = gAllocSpuRamBlock(0x40);
        FCK_EQ(r, 0x1000, "freed block reused");
        FCK_EQ(gTableCursor, 3, "remainder record inserted");
        FCK_CHECK(pool_is(out, 4), "remainder free, sorted by address");
    }
    {
        static const u32 in[] = { 0x80001000, 0x20, 0x00001020, 0x100, 0x00001120, 0x10 };
        pool_set(2, 3, in, 3);
        r = gAllocSpuRamBlock(0x40);
        FCK_EQ(r, -1, "no free block large enough -> -1");
        FCK_CHECK(pool_is(in, 3), "failed search leaves the table");
    }
    {
        static const u32 in[] = { 0x40001000, 0xC0 };
        pool_set(0, 8, in, 1);
        gArenaOffsetFlag = 1;
        gArenaCursor = 0xFFF0;               /* reserve (0x10000-0xFFF0)<<3 = 0x80 */
        r = gAllocSpuRamBlock(0x48);
        FCK_EQ(r, -1, "tail minus reserve 0x40 < 0x48 -> -1");
        FCK_CHECK(pool_is(in, 1), "rejected tail untouched");
        r = gAllocSpuRamBlock(0x40);
        FCK_EQ(r, 0x1000, "tail minus reserve == request fits");
    }
    {
        static const u32 in[] = { 0x40001000, 0x100 };
        pool_set(0, 8, in, 1);
        r = gAllocSpuRamBlock(5);
        FCK_EQ(r, 0x40001000, "size below 8 rounds to 0; the empty block is dropped and the tail word returned");
        FCK_EQ(gTableCursor, 0, "only the tail remains");
    }
    {
        static const u32 in[] = { 0x80001000, 0x40, 0x00001040, 0x40, 0x00001080, 0x40, 0x400010C0, 0x100 };
        static const u32 out[] = { 0x00001000, 0x40, 0x00001040, 0x40, 0x00001080, 0x40, 0x400010C0, 0x100 };
        pool_set(3, 4, in, 4);
        r = gAllocSpuRamBlock(0x40);
        FCK_EQ(r, 0x1000, "free block of exactly the request is taken before the tail");
        FCK_EQ(gTableCursor, 3, "exact fit adds no record");
        FCK_CHECK(pool_is(out, 4), "exact fit only clears the free flag");
    }
    {
        static const u32 in[] = { 0x80001000, 0x100, 0x00002000, 0x10 };
        pool_set(0, 0, in, 2);
        r = gAllocSpuRamBlock(0x40);
        FCK_EQ(r, -1, "capacity 0: the scan sees no record");
        FCK_EQ(pool[1], 0x100, "capacity 0: free record untouched");
    }
}

static void test_spu_split(void)
{
    s32 r;

    /* @leaf 800980F8 source src/game/800980F8.c: splits record a0 at a1 into leading free gap, block of a2 and trailing free/tail record; capacity and reserve checks return -1; with the real 80097DF8 */
    LK_FUNC("func_800980F8");
    {
        static const u32 in[] = { 0x00001000, 0x100, 0x40001100, 0x1000 };
        static const u32 out[] = { 0x00001000, 0x100, 0x80001100, 0x100, 0x00001200, 0x80, 0x40001280, 0xE80 };
        pool_set(1, 8, in, 2);
        r = gSpuSplitPoolBlock(1, 0x1200, 0x80);
        FCK_EQ(r, 0x1200, "tail with gap returns a1");
        FCK_EQ(gTableCursor, 3, "gap, block and tail records");
        FCK_CHECK(pool_is(out, 4), "tail split with leading gap");
        pool_set(1, 2, in, 2);
        r = gSpuSplitPoolBlock(1, 0x1200, 0x80);
        FCK_EQ(r, -1, "tail with gap needs two free slots");
        FCK_CHECK(pool_is(in, 2) && gTableCursor == 1, "capacity failure leaves the table");
    }
    {
        static const u32 in[] = { 0x40001000, 0x400 };
        static const u32 out[] = { 0x00001000, 0x100, 0x40001100, 0x300 };
        pool_set(0, 8, in, 1);
        r = gSpuSplitPoolBlock(0, 0x1000, 0x100);
        FCK_EQ(r, 0x1000, "tail without gap");
        FCK_EQ(gTableCursor, 1, "block and tail");
        FCK_CHECK(pool_is(out, 2), "tail split at its start");

        pool_set(0, 1, in, 1);
        r = gSpuSplitPoolBlock(0, 0x1000, 0x100);
        FCK_EQ(r, 0x1000, "no gap: capacity test is cursor > capacity - 1");

        pool_set(0, 8, in, 1);
        gArenaOffsetFlag = 1;
        gArenaCursor = 0xFFE0;              /* reserve 0x100 */
        r = gSpuSplitPoolBlock(0, 0x1000, 0x301);
        FCK_EQ(r, -1, "reserve leaves 0x300 < 0x301");
        FCK_CHECK(pool_is(in, 1), "reserve failure leaves the table");
        r = gSpuSplitPoolBlock(0, 0x1000, 0x300);
        FCK_EQ(r, 0x1000, "exactly the space above the reserve");
        FCK_EQ(pool[3], 0x100, "tail record keeps size minus the block; the reserve is not subtracted");
    }
    {
        static const u32 in[] = { 0x80001000, 0x200, 0x00001200, 0x40, 0x40001240, 0x1000 };
        static const u32 out[] = { 0x80001000, 0x80, 0x00001080, 0x40, 0x800010C0, 0x140,
                                   0x00001200, 0x40, 0x40001240, 0x1000 };
        pool_set(2, 8, in, 3);
        r = gSpuSplitPoolBlock(0, 0x1080, 0x40);
        FCK_EQ(r, 0x1080, "free block with gap");
        FCK_EQ(gTableCursor, 4, "gap, block, rest, tail plus the allocated neighbour");
        FCK_CHECK(pool_is(out, 5), "three-way split sorted by address");
    }
    {
        static const u32 in[] = { 0x80001000, 0x100, 0x00001100, 0x40, 0x40001140, 0x1000 };
        static const u32 out[] = { 0x80001000, 0x80, 0x00001080, 0x80, 0x00001100, 0x40, 0x40001140, 0x1000 };
        pool_set(2, 8, in, 3);
        r = gSpuSplitPoolBlock(0, 0x1080, 0x80);
        FCK_EQ(r, 0x1080, "block ends at the free record's end");
        FCK_EQ(gTableCursor, 3, "empty remainder record removed by the compaction");
        FCK_CHECK(pool_is(out, 4), "gap and block only");
    }
    {
        static const u32 in[] = { 0x80001000, 0x100, 0x00001100, 0x40, 0x40001140, 0x1000 };
        static const u32 out[] = { 0x00001000, 0x40, 0x80001040, 0xC0, 0x00001100, 0x40, 0x40001140, 0x1000 };
        pool_set(2, 8, in, 3);
        r = gSpuSplitPoolBlock(0, 0x1000, 0x40);
        FCK_EQ(r, 0x1000, "free block without gap");
        FCK_EQ(gTableCursor, 3, "remainder appended");
        FCK_CHECK(pool_is(out, 4), "block then free remainder");
    }
    {
        static const u32 in[] = { 0x80001000, 0x40, 0x00001040, 0x40, 0x40001080, 0x100 };
        static const u32 out[] = { 0x00001000, 0x40, 0x00001040, 0x40, 0x40001080, 0x100 };
        pool_set(2, 8, in, 3);
        r = gSpuSplitPoolBlock(0, 0x1000, 0x40);
        FCK_EQ(r, 0x1000, "exact fit");
        FCK_EQ(gTableCursor, 2, "exact fit adds no record");
        FCK_CHECK(pool_is(out, 3), "exact fit only clears the free flag");

        pool_set(2, 8, in, 3);
        r = gSpuSplitPoolBlock(0, 0x1010, 0x40);
        FCK_EQ(r, -1, "0x30 left after the gap < 0x40");
        FCK_CHECK(pool_is(in, 3), "too small leaves the table");
    }
}

static void test_spu_alloc_at(void)
{
    s32 r;

    /* @leaf 80098458 source src/game/80098458.c: address and size rounded to 8, below 0x1010 or past 0x80000-reserve -1; untouched pool cut directly, otherwise first free record containing or above x, else the tail start, split by the real 800980F8 */
    LK_FUNC("func_80098458");
    {
        static const u32 in[] = { 0x40001010, 0x7EFF0 };
        static const u32 out[] = { 0x80001010, 0xFF8, 0x00002008, 0x108, 0x40002110, 0x7DEF0 };
        pool_set(0, 8, in, 1);
        r = gSpuAllocAt(0x2003, 0x101);
        FCK_EQ(r, 0x2008, "x rounded up to 8");
        FCK_EQ(gTableCursor, 2, "record count 2 stored via the %hi(D_8009EF08) store");
        FCK_CHECK(pool_is(out, 3), "gap, block, tail");
    }
    {
        static const u32 in[] = { 0x40001010, 0x7EFF0 };
        static const u32 out[] = { 0x00001010, 0x1000, 0x40002010, 0x7DFF0 };
        pool_set(0, 8, in, 1);
        r = gSpuAllocAt(0x1010, 0x1000);
        FCK_EQ(r, 0x1010, "block at the header end");
        FCK_EQ(gTableCursor, 1, "record count 1");
        FCK_CHECK(pool_is(out, 2), "block and tail");
    }
    {
        static const u32 in[] = { 0x40001010, 0x7EFF0 };
        pool_set(0, 8, in, 1);
        FCK_EQ(gSpuAllocAt(0x1000, 8), -1, "x below 0x1010");
        FCK_EQ(gSpuAllocAt(0x7F000, 0x1008), -1, "end past 0x80000");
        gArenaOffsetFlag = 1;
        gArenaCursor = 0xFF00;              /* reserve 0x800 */
        FCK_EQ(gSpuAllocAt(0x7F000, 0x808), -1, "end past 0x80000 - reserve");
        FCK_CHECK(pool_is(in, 1) && gTableCursor == 0, "rejections leave the table");
        r = gSpuAllocAt(0x7F000, 0x800);
        FCK_EQ(r, 0x7F000, "end exactly at 0x80000 - reserve is accepted");
        pool_set(0, 1, in, 1);
        FCK_EQ(gSpuAllocAt(0x2000, 0x40), -1, "gap case needs capacity for two records");
        FCK_CHECK(pool_is(in, 1) && gTableCursor == 0, "capacity failure leaves the table");
    }
    {
        static const u32 in[] = { 0x00001010, 0x100, 0x80001110, 0x200, 0x00001310, 0x40, 0x40001350, 0x7ECB0 };
        static const u32 out[] = { 0x00001010, 0x100, 0x80001110, 0xF0, 0x00001200, 0x80,
                                   0x80001280, 0x90, 0x00001310, 0x40, 0x40001350, 0x7ECB0 };
        pool_set(3, 8, in, 4);
        r = gSpuAllocAt(0x1200, 0x80);
        FCK_EQ(r, 0x1200, "x inside a free record");
        FCK_EQ(gTableCursor, 5, "free record split three ways");
        FCK_CHECK(pool_is(out, 6), "table after the split");
    }
    {
        static const u32 in[] = { 0x00001010, 0x100, 0x80001110, 0x200, 0x00001310, 0x40, 0x40001350, 0x7ECB0 };
        static const u32 out[] = { 0x00001010, 0x100, 0x00001110, 0x80, 0x80001190, 0x180,
                                   0x00001310, 0x40, 0x40001350, 0x7ECB0 };
        pool_set(3, 8, in, 4);
        r = gSpuAllocAt(0x1020, 0x80);
        FCK_EQ(r, 0x1110, "x below the free record: block at its start");
        FCK_EQ(gTableCursor, 4, "block and remainder");
        FCK_CHECK(pool_is(out, 5), "table after the split");
    }
    {
        static const u32 in[] = { 0x80001010, 0x40, 0x00001050, 0x40, 0x40001090, 0x100 };
        static const u32 out[] = { 0x80001010, 0x40, 0x00001050, 0x40, 0x00001090, 0x80, 0x40001110, 0x80 };
        pool_set(2, 8, in, 3);
        gArenaOffsetFlag = 1;
        gArenaCursor = 0xFFF0;              /* reserve 0x80 */
        r = gSpuAllocAt(0x1010, 0xC0);
        FCK_EQ(r, -1, "free record too small, tail minus reserve too small");
        FCK_CHECK(pool_is(in, 3), "failure leaves the table");
        r = gSpuAllocAt(0x1010, 0x80);
        FCK_EQ(r, 0x1090, "falls through to the tail start");
        FCK_EQ(gTableCursor, 3, "tail split");
        FCK_CHECK(pool_is(out, 4), "block and tail");
    }
}

static void test_spu_key(void)
{
    /* @leaf 80098C68 source src/game/80098C68.c: low 24 voice bits; direct path writes KON/KOFF at +0x188/+0x18C and a soft mask, shadow path (D_8009EE90 bit 0) writes D_800A7028 and clears the opposite field */
    LK_FUNC("func_80098C68");

    spu_reset();
    lk_fill(gSpuInitWord, 0x5A, sizeof gSpuInitWord);
    gSpuRegisterShadowMode = 0;
    gSpuDirectKeyOnVoiceMask = 0x000001;
    gSpuBufferedKeyOnVoiceMask = 0x11;
    gModeEntryWord = 0x10;
    gSpuSetVoiceKey(1, 0x12345678u);
    FCK_EQ(SPU16(0x188), 0x5678, "KON low");
    FCK_EQ(SPU16(0x18A), 0x34, "KON high (bits 16-23 only)");
    FCK_EQ(SPU16(0x18C), 0, "KOFF untouched");
    FCK_EQ(gSpuDirectKeyOnVoiceMask, 0x345679, "direct mask ORed");
    FCK_EQ(gModeEntryWord, 0x10, "direct path leaves the shadow flag word");
    FCK_EQ(gSpuBufferedKeyOnVoiceMask, 0x11, "direct path leaves the buffered mask");
    FCK_CHECK(lk_all(gSpuInitWord, 0, sizeof gSpuInitWord, 0x5A), "direct path leaves the shadow ports");

    spu_reset();
    gSpuDirectKeyOnVoiceMask = 0xFFFFFF;
    gSpuSetVoiceKey(0, 0xFF345678u);
    FCK_EQ(SPU16(0x18C), 0x5678, "KOFF low");
    FCK_EQ(SPU16(0x18E), 0x34, "KOFF high");
    FCK_EQ(SPU16(0x188), 0, "KON untouched");
    FCK_EQ(gSpuDirectKeyOnVoiceMask, 0xCBA987, "direct mask cleared");

    spu_reset();
    gSpuDirectKeyOnVoiceMask = 0x77;
    gSpuSetVoiceKey(2, 0xFFFFFF);
    FCK_EQ(gSpuDirectKeyOnVoiceMask, 0x77, "mode 2: no change");
    FCK_CHECK(lk_all(spu_regs, 0, sizeof spu_regs, 0), "mode 2: no register write");

    spu_reset();
    gSpuRegisterShadowMode = 2;          /* bit 0 clear: still direct */
    gSpuDirectKeyOnVoiceMask = 0;
    gSpuSetVoiceKey(1, 0x3);
    FCK_EQ(SPU16(0x188), 3, "shadow flag bit 0 decides the path");

    spu_reset();
    gSpuRegisterShadowMode = 1;
    gSpuDirectKeyOnVoiceMask = 0x55;
    gSpuBufferedKeyOnVoiceMask = 0x800000;
    gModeEntryWord = 0x10;
    gSpuInitWord[0] = 0; gSpuInitWord[1] = 0;
    gSpuInitWord[2] = 0xFFFF; gSpuInitWord[3] = 0x00FF;
    gSpuSetVoiceKey(1, 0x12345678u);
    FCK_EQ(gSpuInitWord[0], 0x5678, "shadow KON low");
    FCK_EQ(gSpuInitWord[1], 0x34, "shadow KON high");
    FCK_EQ(gSpuInitWord[2], 0xA987, "shadow KOFF low loses the keyed-on bits");
    FCK_EQ(gSpuInitWord[3], 0xCB, "shadow KOFF high loses the keyed-on bits");
    FCK_EQ(gModeEntryWord, 0x11, "pending flag bit 0 set");
    FCK_EQ(gSpuBufferedKeyOnVoiceMask, 0xB45678, "buffered mask ORed");
    FCK_EQ(gSpuDirectKeyOnVoiceMask, 0x55, "shadow path leaves the direct mask");
    FCK_CHECK(lk_all(spu_regs, 0, sizeof spu_regs, 0), "shadow path: no register write");

    gSpuInitWord[0] = 0xFFFF; gSpuInitWord[1] = 0x00FF;
    gSpuInitWord[2] = 0x1111; gSpuInitWord[3] = 0x2222;
    gModeEntryWord = 0;
    gSpuBufferedKeyOnVoiceMask = 0xFFFFFF;
    gSpuSetVoiceKey(0, 0x00010001u);
    FCK_EQ(gSpuInitWord[2], 0x0001, "shadow KOFF low");
    FCK_EQ(gSpuInitWord[3], 0x0001, "shadow KOFF high");
    FCK_EQ(gSpuInitWord[0], 0xFFFE, "shadow KON low loses the keyed-off bit");
    FCK_EQ(gSpuInitWord[1], 0x00FE, "shadow KON high loses the keyed-off bit");
    FCK_EQ(gModeEntryWord, 1, "pending flag set");
    FCK_EQ(gSpuBufferedKeyOnVoiceMask, 0xFEFFFE, "buffered mask cleared");

    gSpuInitWord[0] = 0x8000; gSpuInitWord[1] = 0x0080;
    gSpuSetVoiceKey(0, 0x00010001u);
    FCK_EQ(gSpuInitWord[0], 0x8000, "no overlap: KON low kept");
    FCK_EQ(gSpuInitWord[1], 0x0080, "no overlap: KON high kept");
}

static void attr_reset(LcVoiceAttr *a)
{
    lk_fill(a, 0, sizeof *a);
    spu_reset();
    lk_fill(gVoiceSampleNote, 0x77, sizeof gVoiceSampleNote);
    n2p_calls = 0; n2p_ret = 0x1ABC;
    wa_calls = 0;
}

#define VREG(ch, i) (spu_regs[(ch) * 8 + (i)])

static void test_voice_attr(void)
{
    static LcVoiceAttr a;

    /* @leaf 80099368 spec Psy-Q libspu SpuSetVoiceAttr: SPU_VOICE_* mask bits (VOLL 1 .. ADSR2 1<<18, 0 = all), SPU_VOICE_DIRECT/LINEAR/EXP mode codes; psx-spx SPU voice registers (+0 vol L/R with sweep bits 15-12, +4 pitch, +6/+E start/repeat, +8/+A ADSR field widths and mode bits) */
    LK_FUNC("func_80099368");

    attr_reset(&a);
    a.voice = (1 << 0) | (1 << 5);
    a.mask = 0x10;                              /* SPU_VOICE_PITCH */
    a.pitch = 0x1000;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 2), 0x1000, "voice 0 pitch at +4");
    FCK_EQ(VREG(5, 2), 0x1000, "voice 5 pitch at +4");
    FCK_EQ(VREG(1, 2), 0, "voice 1 not selected");
    FCK_EQ(VREG(0, 0), 0, "pitch only: volume untouched");
    FCK_EQ(gVoiceSampleNote[0], 0x7777, "pitch only: note table untouched");

    attr_reset(&a);
    a.voice = 0xFF000000u;                      /* bits above voice 23 */
    a.mask = 0x50;                              /* PITCH | SAMPLE_NOTE */
    a.pitch = 0x1000; a.sample_note = 0x1234;
    gSpuSetVoiceAttributes(&a);
    FCK_CHECK(lk_all(spu_regs, 0, sizeof spu_regs, 0), "voices 24+ ignored: no register write");
    FCK_EQ(gVoiceSampleNote[24], 0x7777, "voices 24+ ignored: note table");

    attr_reset(&a);
    a.voice = 1 << 23;
    a.mask = 0x60;                              /* NOTE | SAMPLE_NOTE */
    a.sample_note = 0x3C05;
    a.note = 0x4802;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(gVoiceSampleNote[23], 0x3C05, "sample note stored for voice 23");
    FCK_EQ(gVoiceSampleNote[24], 0x7777, "past voice 23 untouched");
    FCK_EQ(n2p_calls, 1, "note converted once");
    FCK_CHECK(n2p_args[0] == 0x3C && n2p_args[1] == 5 && n2p_args[2] == 0x48 && n2p_args[3] == 2,
              "converter gets sample note and note as (note, fine)");
    FCK_EQ(VREG(23, 2), 0x1ABC, "converted pitch at +4");

    attr_reset(&a);
    a.voice = 1;
    a.mask = 0x05;                              /* VOLL | VOLMODEL */
    a.volmode.left = 3;                         /* LINEARDecN */
    a.volume.left = 0x200;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 0), 0xA07F, "sweep: bit15|direction, step clamped to 0x7F");
    a.volmode.left = 7;                         /* EXPDec */
    a.volume.left = -5;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 0), 0xE000, "sweep: negative step clamps to 0");
    a.volmode.left = 1; a.volume.left = 0x80;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 0), 0x807F, "sweep: 0x80 clamps to 0x7F");
    a.volume.left = 0x7F;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 0), 0x807F, "sweep: 0x7F kept");
    a.volmode.left = 2; a.volume.left = 0x10;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 0), 0x9010, "LINEARIncR: phase bit 12");
    a.volmode.left = 0;                         /* DIRECT */
    a.volume.left = 0x1234;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 0), 0x1234, "direct volume, bit 15 clear");
    a.mask = 0x01;                              /* VOLL without VOLMODEL */
    a.volmode.left = 3;
    a.volume.left = (s16)0x9234;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 0), 0x1234, "mode ignored without VOLMODEL: low 15 bits");
    FCK_EQ(VREG(0, 1), 0, "right volume untouched");

    attr_reset(&a);
    a.voice = 1;
    a.mask = 0x0A;                              /* VOLR | VOLMODER */
    a.volmode.right = 5;                        /* EXPIncN */
    a.volume.right = 0x40;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 1), 0xC040, "right: exponential increase");
    FCK_EQ(VREG(0, 0), 0, "left untouched");
    a.volmode.right = 4; a.volume.right = 0x7F;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 1), 0xB07F, "LINEARDecR");
    a.volmode.right = 6; a.volume.right = 0x80;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 1), 0xD07F, "EXPIncR, 0x80 clamps");
    a.volmode.right = 1; a.volume.right = 1;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 1), 0x8001, "LINEARIncN");

    attr_reset(&a);
    a.voice = 1 << 2;
    a.mask = 0x80 | 0x10000;                    /* WDSA | LSAX */
    a.addr = 0x1000; a.loop_addr = 0x2000;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(wa_calls, 2, "two address writes");
    FCK_CHECK(wa_reg[0] == 0x13 && wa_val[0] == 0x1000, "start address register +6 of voice 2");
    FCK_CHECK(wa_reg[1] == 0x17 && wa_val[1] == 0x2000, "repeat address register +E of voice 2");

    attr_reset(&a);
    a.voice = 1;
    a.mask = 0x20000 | 0x40000;                 /* ADSR1 | ADSR2 */
    a.adsr1 = 0xBEEF; a.adsr2 = 0xCAFE;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 4), 0xBEEF, "raw ADSR1 at +8");
    FCK_EQ(VREG(0, 5), 0xCAFE, "raw ADSR2 at +A");

    attr_reset(&a);
    a.voice = 1;
    a.mask = 0x900;                             /* AR | AMODE */
    VREG(0, 4) = 0x00AB;
    a.ar = 0x90; a.a_mode = 5;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 4), 0xFFAB, "attack rate clamps to 7 bits, exponential bit 15");
    a.ar = 0x12; a.a_mode = 1;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 4), 0x12AB, "linear attack");
    a.mask = 0x800; a.a_mode = 5;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 4), 0x12AB, "AR without AMODE stays linear");

    attr_reset(&a);
    a.voice = 1;
    a.mask = 0x1000;                            /* DR */
    VREG(0, 4) = 0x1234;
    a.dr = 5;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 4), 0x1254, "decay shift bits 4-7");
    a.dr = 0x30;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 4), 0x12F4, "decay clamps to 0xF");

    attr_reset(&a);
    a.voice = 1;
    a.mask = 0x2200;                            /* SR | SMODE */
    VREG(0, 5) = 0x00FF;
    a.sr = 0x81; a.s_mode = 7;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 5), 0xDFFF, "sustain rate clamps to 0x7F, exponential decrease bits 15/14");
    a.s_mode = 1;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 5), 0x1FFF, "linear increase");
    a.s_mode = 5; a.sr = 2;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 5), 0x80BF, "exponential increase");
    a.mask = 0x2000; a.sr = 5;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 5), 0x417F, "without SMODE: linear decrease");

    attr_reset(&a);
    a.voice = 1;
    a.mask = 0x4400;                            /* RR | RMODE */
    VREG(0, 5) = 0xABCD;
    a.rr = 0x25; a.r_mode = 7;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 5), 0xABFF, "release clamps to 0x1F, exponential bit 5");
    a.rr = 4; a.r_mode = 3;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 5), 0xABC4, "linear release");

    attr_reset(&a);
    a.voice = 1;
    a.mask = 0x8000;                            /* SL */
    VREG(0, 4) = 0x12F4;
    a.sl = 0x13;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 4), 0x12FF, "sustain level clamps to 0xF");
    a.sl = 2;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(0, 4), 0x12F2, "sustain level bits 0-3");

    /* mask 0: every field, in the documented order */
    attr_reset(&a);
    a.voice = 1 << 23;
    a.mask = 0;
    a.pitch = 0x1111;
    a.sample_note = 0x3000; a.note = 0x3C00;
    a.volume.left = 0x10; a.volmode.left = 2;
    a.volume.right = 0x3FFF; a.volmode.right = 0;
    a.addr = 0x100; a.loop_addr = 0x200;
    a.adsr1 = 0x1111; a.adsr2 = 0x2222;
    a.ar = 5; a.a_mode = 5; a.dr = 3; a.sl = 7;
    a.sr = 0x10; a.s_mode = 5; a.rr = 3; a.r_mode = 7;
    gSpuSetVoiceAttributes(&a);
    FCK_EQ(VREG(23, 2), 0x1ABC, "all: converted pitch overrides the raw pitch");
    FCK_EQ(gVoiceSampleNote[23], 0x3000, "all: sample note");
    FCK_EQ(VREG(23, 0), 0x9010, "all: left volume with mode");
    FCK_EQ(VREG(23, 1), 0x3FFF, "all: right volume direct");
    FCK_EQ(wa_calls, 2, "all: both addresses");
    FCK_EQ(VREG(23, 4), 0x8537, "all: ADSR1 then AR/AMODE, DR, SL");
    FCK_EQ(VREG(23, 5), 0x8423, "all: ADSR2 then SR/SMODE, RR/RMODE");
    FCK_EQ(VREG(22, 4), 0, "voice 22 untouched");
}

int main(void)
{
    test_scene_voice();
    test_node_command();
    test_dither_tables();
    test_gpu_fill();
    test_spu_transfer();
    test_spu_compact();
    test_spu_alloc();
    test_spu_split();
    test_spu_alloc_at();
    test_spu_key();
    test_voice_attr();
    return lk_done("leaf_core");
}
