/* Hardware-free tranche 2 leaf suite: 25 unchanged original MIPS C units from
 * src/game/ running under qemu-mipsel on explicit synthetic RAM fixtures,
 * @link'd original pure-RAM callees, and software-only recording stand-ins
 * (no hardware/device mocks). Uses shared sound_mock for func_80046C84.
 */
/* @mock sound_mock */
/* @link 80093F58 */
/* @link 80093F78 */
/* @link 80094010 */
/* @link 800940A8 */
/* @link 800940C4 */
/* @link 800448D8 */
/* @link 8003BF1C */
/* @link 80069B1C */
/* @link 800455DC */
/* @link 8005A654 */
/* @link 8004DA58 */
/* @link 8004D9EC */
#include "leaf_kit.h"
#include "mocks/sound_mock.h"

/* ============================================================================
 * Shared synthetic globals (D_ labels referenced by the 25 units + @link units)
 * ============================================================================
 */

/* 8001306C */
s32 *gVertexBlockTable __asm__("D_8009F554");
s32 *gNormalBlockTable __asm__("D_8009F558");
s32 *gRelocBlockTable __asm__("D_8009F55C");

/* 80045494 / 80045678 / 800455DC / 800457B8 */
struct BSoundEntry {
    u8 unk00[0x10];
    u8 active;
    u8 unk11[0x0B];
};
u8 gSoundBankTableLow[0x60] __asm__("D_800A1830");
u8 gSoundBankTableHigh[0x60] __asm__("D_800A18D0");
struct BSoundEntry gSoundEntryTable[0x80] __asm__("D_800A7938");

struct BSoundChannel {
    s32 id;
    s32 priority;
    s16 slot;
    s8 index;
    u8 unk0B;
    s32 unk0C;
    s32 unk10;
    u8 unk14[0x14];
    s32 unk28;
    struct BSoundChannel *next;
    struct BSoundChannel *prev;
};
s32 gChannelCount __asm__("D_8009F720");
struct BSoundChannel *gChannelListHead __asm__("D_8009F724");
struct BSoundChannel *gSoundChannelQueueHead __asm__("D_8009F728");
struct BSoundChannel *gSoundChannelQueueTail __asm__("D_8009F72C");
struct BSoundChannel gVoiceTable[8] __asm__("D_800A1160");

/* 8009398C / 80093AF0 / 80093F78 / 80094010 */
u16 gMaxDrawWidth __asm__("D_8009DDF4");
u16 gMaxDrawHeight __asm__("D_8009DDF6");

/* 8004DA84 / 8004D9EC / 8004DC64 */
struct BEntityMessage {
    u8 kind;
    u8 value;
    s16 timer;
    void *target;
    s32 id;
};
s32 gPendingMessageMask __asm__("D_8009F7A8");
struct BEntityMessage gPendingMessages[16] __asm__("D_800A2090");
void *gUnitListHead __asm__("D_8009FA40");

/* 8006F170 / 8003BF1C */
struct BPoolNode {
    struct BPoolNode *next;
    s32 tag;
};
struct BPool {
    void *storage;
    struct BPoolNode *active;
    struct BPoolNode *free;
};
struct BPool gTracerPool __asm__("D_800A3538");

/* 80029520 */
struct BReleaseQueues {
    s32 unk00[2];
    s32 unk08[2];
    u8 count10;
    u8 count11;
    u8 unk12;
    u8 unk13;
};
struct BReleaseQueues gReleaseQueues __asm__("D_800A0C30");

/* 80069B58 / 80069B1C / 8007A3AC */
void *gGameSession[2] __asm__("D_8009FA94");
s32 *gOpcodeTable __asm__("D_8009F9F8");
s32 *gScriptVmVars __asm__("D_8009FAD4");
s32 *gVmCountdowns __asm__("D_8009F9A4");
u8 gScriptVmDisabled __asm__("D_8009F840");
u8 gFrameTime[4] __asm__("D_8009FAA4");
u8 gScriptVmAuxVar[4] __asm__("D_8009FAB4");
u8 gScreenResetFlag[4] __asm__("D_8009FA70");
u8 gEffectId[4] __asm__("D_8009FACC");

/* 80031990 */
void *gMapZoneList __asm__("D_8009F6F8");

/* 8003FD74 */
void *gDeferredSpawnList __asm__("D_8009F30C");

/* 800834C4 */
s32 *gCellBucketGrid __asm__("D_8009F938");
s32 gCellGridWidth __asm__("D_8009F93C");
s32 gCellGridHeight __asm__("D_8009F940");

/* 8005A220 / 8005A6A4 / 8005A654 */
void **gTextureDescriptorTable __asm__("D_8009F7D8");
s32 gTexDescriptorCount __asm__("D_8009F7DC");

/* 800860CC / 80086164 */
void *gPreallocatedMovieVlcTableA __asm__("D_8009F948");
void *gPreallocatedMovieVlcTableB __asm__("D_8009F94C");
void *gWorkAreaBlock __asm__("D_8009F950");
void *gPreallocatedMovieMdecInputBuffer __asm__("D_8009F954");

/* 800427E8 */
s32 gRemainingCounter __asm__("D_8009FAB0");

/* 8007A3AC */
u8 gCwRulesMovieName[16] __asm__("D_80012088");
u8 gLayoutPointerFlagB __asm__("D_8009F3E7");
u8 gSavedCrimeWarProgress __asm__("D_8009F3F1");
u8 gCrimeWarProgress __asm__("D_8009F458");
void *gPendingRowCapture __asm__("D_8009F8C4");
s32 gDeferredTransition __asm__("D_8009F8C8");
s32 gPlayer1MenuAcceptPhase __asm__("D_800A35CC");
u8 gAudioOptionsMenuMode[4] __asm__("D_800A377D");
u8 gCrimeWarHubMenuInputMode[4] __asm__("D_800A386D");

/* ============================================================================
 * Software-only recording boundary stand-ins (all bound to retail labels)
 * ============================================================================
 */

/* --- func_80017EBC: synthetic RAM allocator stand-in --- */
#define ALLOC_LOG_MAX 16
static struct {
    s32 calls;
    s32 sizes[ALLOC_LOG_MAX];
    s32 tags[ALLOC_LOG_MAX];
    void *results[ALLOC_LOG_MAX];
    s32 next_index;
} s_alloc;

static void reset_alloc(void)
{
    memset(&s_alloc, 0, sizeof(s_alloc));
}

void *mock_alloc(s32 size, s32 tag) __asm__("func_80017EBC");
void *mock_alloc(s32 size, s32 tag)
{
    void *ret = 0;
    if (s_alloc.calls < ALLOC_LOG_MAX) {
        s_alloc.sizes[s_alloc.calls] = size;
        s_alloc.tags[s_alloc.calls] = tag;
    }
    if (s_alloc.next_index < ALLOC_LOG_MAX) {
        ret = s_alloc.results[s_alloc.next_index++];
    }
    s_alloc.calls++;
    return ret;
}

/* --- func_800181D4: synthetic RAM free stand-in --- */
#define FREE_LOG_MAX 32
static struct {
    s32 calls;
    void *ptrs[FREE_LOG_MAX];
} s_free;

static void reset_free(void)
{
    memset(&s_free, 0, sizeof(s_free));
}

void mock_free(void *ptr) __asm__("func_800181D4");
void mock_free(void *ptr)
{
    if (s_free.calls < FREE_LOG_MAX) {
        s_free.ptrs[s_free.calls] = ptr;
    }
    s_free.calls++;
}

/* --- func_80070710: beam endpoint free stand-in --- */
static struct {
    s32 calls;
    void *ptrs[8];
} s_beam_free;

static void reset_beam_free(void)
{
    memset(&s_beam_free, 0, sizeof(s_beam_free));
}

void mock_beam_free(void *ptr) __asm__("func_80070710");
void mock_beam_free(void *ptr)
{
    if (s_beam_free.calls < 8) {
        s_beam_free.ptrs[s_beam_free.calls] = ptr;
    }
    s_beam_free.calls++;
}

/* --- func_80019668: deterministic synthetic RNG stand-in --- */
static struct {
    s32 calls;
    u32 seq[40];
    s32 count;
} s_rng;

static void reset_rng(void)
{
    memset(&s_rng, 0, sizeof(s_rng));
}

s32 mock_rng(void) __asm__("func_80019668");
s32 mock_rng(void)
{
    u32 v = 0;
    if (s_rng.calls < s_rng.count) {
        v = s_rng.seq[s_rng.calls];
    }
    s_rng.calls++;
    return (s32)v;
}

/* --- func_80029650: deferred asset release stand-in --- */
static struct {
    s32 calls;
    s32 handles[16];
    u8 seen_count10[16];
    u8 seen_count11[16];
} s_asset_rel;

static void reset_asset_rel(void)
{
    memset(&s_asset_rel, 0, sizeof(s_asset_rel));
}

void mock_asset_rel(s32 handle) __asm__("func_80029650");
void mock_asset_rel(s32 handle)
{
    if (s_asset_rel.calls < 16) {
        s_asset_rel.handles[s_asset_rel.calls] = handle;
        s_asset_rel.seen_count10[s_asset_rel.calls] = gReleaseQueues.count10;
        s_asset_rel.seen_count11[s_asset_rel.calls] = gReleaseQueues.count11;
    }
    s_asset_rel.calls++;
}

/* --- func_80031BEC / func_8004953C / func_8003FD6C: tag command dispatchers --- */
static struct {
    s32 zone_calls, zone_a0, zone_kind, zone_val;
    s32 timer_calls, timer_a0, timer_kind, timer_val;
    s32 ign_calls, ign_a0, ign_kind, ign_val;
} s_tag_cmd;

static void reset_tag_cmd(void)
{
    memset(&s_tag_cmd, 0, sizeof(s_tag_cmd));
}

void mock_zone_state(s32 id, s32 kind, s32 val) __asm__("func_80031BEC");
void mock_zone_state(s32 id, s32 kind, s32 val)
{
    s_tag_cmd.zone_calls++;
    s_tag_cmd.zone_a0 = id;
    s_tag_cmd.zone_kind = kind;
    s_tag_cmd.zone_val = val;
}

void mock_timer_count(s32 index, s32 kind, s32 val) __asm__("func_8004953C");
void mock_timer_count(s32 index, s32 kind, s32 val)
{
    s_tag_cmd.timer_calls++;
    s_tag_cmd.timer_a0 = index;
    s_tag_cmd.timer_kind = kind;
    s_tag_cmd.timer_val = val;
}

void mock_ign69(s32 a0, s32 kind, s32 val) __asm__("func_8003FD6C");
void mock_ign69(s32 a0, s32 kind, s32 val)
{
    s_tag_cmd.ign_calls++;
    s_tag_cmd.ign_a0 = a0;
    s_tag_cmd.ign_kind = kind;
    s_tag_cmd.ign_val = val;
}

/* --- func_8006EE14: tracer node step stand-in --- */
static struct {
    s32 calls;
    struct BPoolNode *nodes[8];
    void *args[8];
    u8 ret[8];
} s_tracer_step;

static void reset_tracer_step(void)
{
    memset(&s_tracer_step, 0, sizeof(s_tracer_step));
}

u8 mock_step_tracer(struct BPoolNode *node, void *arg) __asm__("func_8006EE14");
u8 mock_step_tracer(struct BPoolNode *node, void *arg)
{
    u8 r = 0;
    if (s_tracer_step.calls < 8) {
        s_tracer_step.nodes[s_tracer_step.calls] = node;
        s_tracer_step.args[s_tracer_step.calls] = arg;
        r = s_tracer_step.ret[s_tracer_step.calls];
    }
    s_tracer_step.calls++;
    return r;
}

/* --- func_80046698 / func_80045548: sound voice & channel release stand-ins --- */
static struct {
    s32 voice_calls;
    s32 last_handle;
    s32 chan_calls;
    void *last_chan;
    s32 seen_id_on_voice;
    s32 seen_id_on_chan;
} s_sound_stop;

static void reset_sound_stop(void)
{
    memset(&s_sound_stop, 0, sizeof(s_sound_stop));
}

void mock_rel_voice(s32 handle) __asm__("func_80046698");
void mock_rel_voice(s32 handle)
{
    s_sound_stop.voice_calls++;
    s_sound_stop.last_handle = handle;
}

void mock_rel_chan(struct BSoundChannel *ch) __asm__("func_80045548");
void mock_rel_chan(struct BSoundChannel *ch)
{
    s_sound_stop.chan_calls++;
    s_sound_stop.last_chan = ch;
    s_sound_stop.seen_id_on_chan = ch ? ch->id : 0;
}

/* --- func_80049C38: dynamic light node spawn stand-in --- */
static struct {
    s32 calls;
    void *last_obj;
    s32 last_len;
    u8 seen_slot;
    s32 ret_val;
} s_light_spawn;

static void reset_light_spawn(s32 ret_val)
{
    memset(&s_light_spawn, 0, sizeof(s_light_spawn));
    s_light_spawn.ret_val = ret_val;
}

s32 mock_spawn_light(u8 *obj, s32 len) __asm__("func_80049C38");
s32 mock_spawn_light(u8 *obj, s32 len)
{
    s_light_spawn.calls++;
    s_light_spawn.last_obj = obj;
    s_light_spawn.last_len = len;
    s_light_spawn.seen_slot = obj ? obj[0x15] : 0;
    return s_light_spawn.ret_val;
}

/* --- func_80049980 / func_800752E0: particle light update/create stand-ins --- */
static struct {
    s32 upd_calls;
    void *upd_light;
    s32 x, z, y, w, d, angle, a7, a8;
    u8 seen_r, seen_g, seen_b;
    s32 create_calls;
    void *create_obj;
} s_plight;

static void reset_plight(void)
{
    memset(&s_plight, 0, sizeof(s_plight));
}

void mock_upd_light(u8 *light, s32 x, s32 z, s32 y, s32 w, s32 d, s32 angle, s32 a7, s32 a8)
    __asm__("func_80049980");
void mock_upd_light(u8 *light, s32 x, s32 z, s32 y, s32 w, s32 d, s32 angle, s32 a7, s32 a8)
{
    s_plight.upd_calls++;
    s_plight.upd_light = light;
    s_plight.x = x;
    s_plight.z = z;
    s_plight.y = y;
    s_plight.w = w;
    s_plight.d = d;
    s_plight.angle = angle;
    s_plight.a7 = a7;
    s_plight.a8 = a8;
    if (light) {
        s_plight.seen_r = light[9];
        s_plight.seen_g = light[10];
        s_plight.seen_b = light[11];
    }
}

void mock_create_plight(void *obj) __asm__("func_800752E0");
void mock_create_plight(void *obj)
{
    s_plight.create_calls++;
    s_plight.create_obj = obj;
}

/* --- func_800776B8: Crime War mode reset callback stand-in --- */
static struct {
    s32 calls;
    s32 seen_players;
    s32 seen_split;
    u8 seen_saved;
    u8 seen_prog;
    s32 seen_trans;
} s_cw_reset;

struct BCwMode {
    u8 unk00[0x0C];
    s32 players;
    u8 unk10[0x4C];
    s32 split;
};

static void reset_cw_reset(void)
{
    memset(&s_cw_reset, 0, sizeof(s_cw_reset));
}

void mock_cw_reset(void) __asm__("func_800776B8");
void mock_cw_reset(void)
{
    struct BCwMode *m = (struct BCwMode *)gGameSession[0];
    s_cw_reset.calls++;
    if (m) {
        s_cw_reset.seen_players = m->players;
        s_cw_reset.seen_split = m->split;
    }
    s_cw_reset.seen_saved = gSavedCrimeWarProgress;
    s_cw_reset.seen_prog = gCrimeWarProgress;
    s_cw_reset.seen_trans = gDeferredTransition;
}

/* ============================================================================
 * 1. 8001306C - gResolveModelBlockIndices
 * ============================================================================
 */
struct BModelIdxBlock {
    u32 tag;
    u32 count;
    s32 items[4];
};
extern s32 gResolveModelBlockIndices(struct BModelIdxBlock *arg0) __asm__("func_8001306C");

static void test_8001306C(void)
{
    s32 vtbl[4];
    s32 ntbl[4];
    s32 rtbl[4];
    struct BModelIdxBlock blk;
    s32 r;

    /* @leaf 8001306C source tagged LV4D/LN4D/LR3D index-to-pointer remapping with count=0 and unknown tag */
    LK_FUNC("func_8001306C");

    vtbl[0] = 0x1001; vtbl[1] = 0x1002; vtbl[2] = 0x1003; vtbl[3] = 0x1004;
    ntbl[0] = 0x2001; ntbl[1] = 0x2002; ntbl[2] = 0x2003; ntbl[3] = 0x2004;
    rtbl[0] = 0x3001; rtbl[1] = 0x3002; rtbl[2] = 0x3003; rtbl[3] = 0x3004;
    gVertexBlockTable = vtbl;
    gNormalBlockTable = ntbl;
    gRelocBlockTable = rtbl;

    /* Vertex tag 0x3444564C (LV4D): remaps 3 entries, leaves 4th sentinel untouched. */
    blk.tag = 0x3444564C;
    blk.count = 3;
    blk.items[0] = 2; blk.items[1] = 0; blk.items[2] = 3; blk.items[3] = 0x7777;
    r = gResolveModelBlockIndices(&blk);
    FCK_EQ(r, 0, "LV4D returns 0");
    FCK_EQ(blk.items[0], 0x1003, "LV4D item 0");
    FCK_EQ(blk.items[1], 0x1001, "LV4D item 1");
    FCK_EQ(blk.items[2], 0x1004, "LV4D item 2");
    FCK_EQ(blk.items[3], 0x7777, "LV4D sentinel untouched");

    /* Normal tag 0x34444E4C (LN4D): remaps 2 entries. */
    blk.tag = 0x34444E4C;
    blk.count = 2;
    blk.items[0] = 1; blk.items[1] = 3; blk.items[2] = 0x5555;
    r = gResolveModelBlockIndices(&blk);
    FCK_EQ(r, 0, "LN4D returns 0");
    FCK_EQ(blk.items[0], 0x2002, "LN4D item 0");
    FCK_EQ(blk.items[1], 0x2004, "LN4D item 1");
    FCK_EQ(blk.items[2], 0x5555, "LN4D sentinel untouched");

    /* Reloc tag 0x3344524C (LR3D): remaps 2 entries. */
    blk.tag = 0x3344524C;
    blk.count = 2;
    blk.items[0] = 3; blk.items[1] = 0; blk.items[2] = 0x6666;
    r = gResolveModelBlockIndices(&blk);
    FCK_EQ(r, 0, "LR3D returns 0");
    FCK_EQ(blk.items[0], 0x3004, "LR3D item 0");
    FCK_EQ(blk.items[1], 0x3001, "LR3D item 1");
    FCK_EQ(blk.items[2], 0x6666, "LR3D sentinel untouched");

    /* Count 0 for all three tags leaves items untouched. */
    blk.tag = 0x3444564C; blk.count = 0; blk.items[0] = 99;
    FCK_EQ(gResolveModelBlockIndices(&blk), 0, "LV4D count=0 returns 0");
    FCK_EQ(blk.items[0], 99, "LV4D count=0 untouched");

    blk.tag = 0x34444E4C; blk.count = 0; blk.items[0] = 88;
    FCK_EQ(gResolveModelBlockIndices(&blk), 0, "LN4D count=0 returns 0");
    FCK_EQ(blk.items[0], 88, "LN4D count=0 untouched");

    blk.tag = 0x3344524C; blk.count = 0; blk.items[0] = 77;
    FCK_EQ(gResolveModelBlockIndices(&blk), 0, "LR3D count=0 returns 0");
    FCK_EQ(blk.items[0], 77, "LR3D count=0 untouched");

    /* Unknown tag leaves entries untouched and still returns 0. */
    blk.tag = 0x12345678; blk.count = 2; blk.items[0] = 1; blk.items[1] = 2;
    FCK_EQ(gResolveModelBlockIndices(&blk), 0, "unknown tag returns 0");
    FCK_EQ(blk.items[0], 1, "unknown tag item 0 untouched");
    FCK_EQ(blk.items[1], 2, "unknown tag item 1 untouched");
}

/* ============================================================================
 * 2. 80045494 - gResolveSoundRecordIndex
 * ============================================================================
 */
extern s32 gResolveSoundRecordIndex(s32 id, u8 *table, s32 limit) __asm__("func_80045494");

static void test_80045494(void)
{
    u8 low_tbl[0x50];

    /* @leaf 80045494 source two-table sound ID resolution, boundary limits and active-record gate */
    LK_FUNC("func_80045494");

    lk_fill(low_tbl, 0, sizeof(low_tbl));
    lk_fill(gSoundBankTableHigh, 0, sizeof(gSoundBankTableHigh));
    lk_fill(gSoundEntryTable, 0, sizeof(gSoundEntryTable));

    /* Active entries: slots 5, 11, 22; inactive entry: slot 7. */
    gSoundEntryTable[5].active = 1;
    gSoundEntryTable[7].active = 0;
    gSoundEntryTable[11].active = 2;
    gSoundEntryTable[22].active = 1;

    low_tbl[0] = 5;
    low_tbl[1] = 5;
    low_tbl[4] = 7;
    low_tbl[0x44] = 11;
    low_tbl[0x45] = 11;
    gSoundBankTableHigh[0] = 22;       /* id = 0x46 */
    gSoundBankTableHigh[0x3B] = 11;    /* id = 0x81 (0x46 + 0x3B = 0x81) */
    gSoundBankTableHigh[0x3C] = 5;     /* id = 0x82 (at high limit) */
    gSoundBankTableHigh[2] = 7;        /* id = 0x48 -> inactive slot 7 */

    /* id <= 0 rejected even if low_tbl[0] points to an active slot. */
    FCK_EQ(gResolveSoundRecordIndex(0, low_tbl, 0x45), 0xFF, "id 0 rejected");
    FCK_EQ(gResolveSoundRecordIndex(-1, low_tbl, 0x45), 0xFF, "negative id rejected");

    /* Valid low-table lookups including boundary limit-1 and limit. */
    FCK_EQ(gResolveSoundRecordIndex(1, low_tbl, 0x45), 5, "low id 1 -> active slot 5");
    FCK_EQ(gResolveSoundRecordIndex(4, low_tbl, 0x45), 0xFF, "low id 4 -> inactive slot 7 rejected");
    FCK_EQ(gResolveSoundRecordIndex(0x44, low_tbl, 0x45), 11, "low id 0x44 (< 0x45) -> slot 11");
    FCK_EQ(gResolveSoundRecordIndex(0x45, low_tbl, 0x45), 0xFF, "low id == limit rejected");
    FCK_EQ(gResolveSoundRecordIndex(0x45, low_tbl, 0x47), 11, "low id 0x45 (< 0x47) -> slot 11");

    /* High-table switch at id >= 0x46 with fixed limit 0x82 and bias -0x46. */
    FCK_EQ(gResolveSoundRecordIndex(0x46, low_tbl, 0x10), 22, "id 0x46 switches to high[0] -> 22");
    FCK_EQ(gResolveSoundRecordIndex(0x48, low_tbl, 0x10), 0xFF, "high id 0x48 inactive slot rejected");
    FCK_EQ(gResolveSoundRecordIndex(0x81, low_tbl, 0x10), 11, "high id 0x81 (< 0x82) -> 11");
    FCK_EQ(gResolveSoundRecordIndex(0x82, low_tbl, 0x10), 0xFF, "high id 0x82 (== 0x82) rejected");
}

/* ============================================================================
 * 3. 8009398C - gBuildDrawAreaPacket (links real 80093F78 + 80094010)
 * ============================================================================
 */
extern void gBuildDrawAreaPacket(u8 *obj, u8 *rect) __asm__("func_8009398C");

static void test_8009398C(void)
{
    u8 pkt[16];
    s16 rect[4];

    /* @leaf 8009398C spec psx-spx GP0 E3h/E4h drawing-area top-left/bottom-right commands and Psy-Q DR_AREA length */
    LK_FUNC("func_8009398C");

    /* Normal in-bounds rectangle: x=16, y=24, w=320, h=240 -> br=(335, 263). */
    gMaxDrawWidth = 1024;
    gMaxDrawHeight = 512;
    lk_fill(pkt, 0xA5, sizeof(pkt));
    rect[0] = 16; rect[1] = 24; rect[2] = 320; rect[3] = 240;
    gBuildDrawAreaPacket(pkt, (u8 *)rect);
    FCK_EQ(pkt[3], 2, "DR_AREA length byte is 2");
    FCK_EQ(lk_all(pkt, 0, 3, 0xA5), 1, "tag low 3 bytes untouched");
    FCK_EQ(*(u32 *)(pkt + 4), 0xE3006010U, "GP0 E3h top-left (16,24)");
    FCK_EQ(*(u32 *)(pkt + 8), 0xE4041D4FU, "GP0 E4h bottom-right (335,263)");
    FCK_EQ(lk_all(pkt, 12, 16, 0xA5), 1, "trailing canary untouched");

    /* Clamped negative top-left and overflowing bottom-right against 320x240. */
    gMaxDrawWidth = 320;
    gMaxDrawHeight = 240;
    lk_fill(pkt, 0x5A, sizeof(pkt));
    rect[0] = -10; rect[1] = -5; rect[2] = 500; rect[3] = 400;
    gBuildDrawAreaPacket(pkt, (u8 *)rect);
    FCK_EQ(pkt[3], 2, "clamped DR_AREA length 2");
    FCK_EQ(*(u32 *)(pkt + 4), 0xE3000000U, "GP0 E3h clamped to (0,0)");
    FCK_EQ(*(u32 *)(pkt + 8), 0xE403BD3FU, "GP0 E4h clamped to (319,239)");

    /* 1x1 corner pixel at exact max-1 boundary (639, 479). */
    gMaxDrawWidth = 640;
    gMaxDrawHeight = 480;
    rect[0] = 639; rect[1] = 479; rect[2] = 1; rect[3] = 1;
    gBuildDrawAreaPacket(pkt, (u8 *)rect);
    FCK_EQ(*(u32 *)(pkt + 4), 0xE3077E7FU, "GP0 E3h exact max-1 (639,479)");
    FCK_EQ(*(u32 *)(pkt + 8), 0xE4077E7FU, "GP0 E4h 1x1 bottom-right equals top-left");
}

/* ============================================================================
 * 4. 80093AF0 - gBuildSpritePacket (links real 80093F58/F78/4010/40A8/40C4)
 * ============================================================================
 */
extern void gBuildSpritePacket(u8 *arg0, u8 *arg1) __asm__("func_80093AF0");

static void test_80093AF0(void)
{
    u32 pkt[12];
    u8 env[0x20];

    /* @leaf 80093AF0 source DR_ENV packet builder: GP0 E1..E6 setup plus optional clamped GP0 60h clear rect */
    LK_FUNC("func_80093AF0");

    gMaxDrawWidth = 640;
    gMaxDrawHeight = 480;
    lk_fill(env, 0, sizeof(env));
    *(s16 *)(env + 0x00) = 32;     /* clip x */
    *(s16 *)(env + 0x02) = 16;     /* clip y */
    *(s16 *)(env + 0x04) = 256;    /* clip w */
    *(s16 *)(env + 0x06) = 128;    /* clip h */
    *(s16 *)(env + 0x08) = 12;     /* ofs x */
    *(s16 *)(env + 0x0A) = -4;     /* ofs y */
    *(s16 *)(env + 0x0C) = 16;     /* tw x */
    *(s16 *)(env + 0x0E) = 32;     /* tw y */
    *(s16 *)(env + 0x10) = 64;     /* tw w */
    *(s16 *)(env + 0x12) = 128;    /* tw h */
    *(u16 *)(env + 0x14) = 0x0125; /* tpage */
    env[0x16] = 1;                 /* dtd */
    env[0x17] = 1;                 /* dfe */
    env[0x18] = 0;                 /* isbg = 0 */
    env[0x19] = 0x11; env[0x1A] = 0x22; env[0x1B] = 0x33;

    /* Without clear rect: 6 command words (obj[3]=6), tail untouched. */
    lk_fill(pkt, 0xCC, sizeof(pkt));
    gBuildSpritePacket((u8 *)pkt, env);
    FCK_EQ(((u8 *)pkt)[3], 6, "isbg=0 word count is 6");
    FCK_EQ(pkt[1], 0xE3004020U, "E3h clip top-left");
    FCK_EQ(pkt[2], 0xE4023D1FU, "E4h clip bottom-right");
    FCK_EQ(pkt[3], 0xE53FE00CU, "E5h draw offset (12,-4)");
    FCK_EQ(pkt[4], 0xE1000725U, "E1h draw mode");
    FCK_EQ(pkt[5], 0xE2020A18U, "E2h texture window");
    FCK_EQ(pkt[6], 0xE6000000U, "E6h mask bit");
    FCK_EQ(pkt[7], 0xCCCCCCCCU, "isbg=0 leaves word 7 untouched");

    /* With clear rect: 9 command words (obj[3]=9), GP0 60h + (x-ofs, y-ofs) + (w, h). */
    env[0x18] = 1;
    lk_fill(pkt, 0xCC, sizeof(pkt));
    gBuildSpritePacket((u8 *)pkt, env);
    FCK_EQ(((u8 *)pkt)[3], 9, "isbg=1 word count is 9");
    FCK_EQ(pkt[7], 0x60332211U, "GP0 60h RGB command");
    FCK_EQ(pkt[8], 0x00140014U, "clear rect pos (32-12, 16-(-4))");
    FCK_EQ(pkt[9], 0x00800100U, "clear rect size (256, 128)");
    FCK_EQ(pkt[10], 0xCCCCCCCCU, "trailing guard untouched");

    /* Clear-rect width/height clamping: w > maxW-1, h < 0, and vice versa. */
    gMaxDrawWidth = 320;
    gMaxDrawHeight = 240;
    *(s16 *)(env + 0x04) = 500;
    *(s16 *)(env + 0x06) = -8;
    gBuildSpritePacket((u8 *)pkt, env);
    FCK_EQ(pkt[9], 0x0000013FU, "clear size clamps w->319, h->0");

    *(s16 *)(env + 0x04) = -3;
    *(s16 *)(env + 0x06) = 300;
    gBuildSpritePacket((u8 *)pkt, env);
    FCK_EQ(pkt[9], 0x00EF0000U, "clear size clamps w->0, h->239");
}

/* ============================================================================
 * 5. 80044988 - gSetAnimClipTable (links real 800448D8)
 * ============================================================================
 */
struct BAnimClip {
    u8 unk00[4];
    s16 start;
    s16 end;
    u8 unk08[8];
};
struct BAnimTrack {
    struct BAnimClip *clips;
    struct BAnimClip *clip;
    s32 elapsed;
    s16 frame;
    s16 target;
    s8 step;
    u8 index;
    u8 count;
    u8 flags;
};
extern void gSetAnimClipTable(u8 *data, struct BAnimTrack *track, u8 forward, s32 index)
    __asm__("func_80044988");

static void test_80044988(void)
{
    struct {
        u8 count;
        u8 pad[3];
        struct BAnimClip clips[3];
    } blob;
    struct BAnimTrack track;

    /* @leaf 80044988 source animation clip-table installation via real gSelectAnimationClip (null, forward, reverse) */
    LK_FUNC("func_80044988");

    lk_fill(&blob, 0, sizeof(blob));
    blob.count = 3;
    blob.clips[0].start = 2;  blob.clips[0].end = 10;
    blob.clips[1].start = 20; blob.clips[1].end = 35;
    blob.clips[2].start = 50; blob.clips[2].end = 40; /* inverted range */

    /* Null data clears only track->clips and preserves other fields. */
    track.clips = &blob.clips[0];
    track.clip = &blob.clips[1];
    track.elapsed = 77;
    track.frame = 12;
    track.target = 34;
    track.step = 1;
    track.index = 2;
    track.count = 9;
    track.flags = 0x55;
    gSetAnimClipTable(0, &track, 1, 1);
    FCK_EQ((u32)track.clips, 0U, "null data clears clips");
    FCK_EQ(track.elapsed, 77, "null data preserves elapsed");
    FCK_EQ(track.flags, 0x55, "null data preserves flags");
    FCK_EQ(track.count, 9, "null data preserves count");

    /* Forward install on clip 1 (20..35): step=+1, frame=20, target=35, elapsed=0, flags=0. */
    track.elapsed = 123;
    track.flags = 0xAA;
    gSetAnimClipTable((u8 *)&blob, &track, 1, 1);
    FCK_EQ((u32)track.clips, (u32)&blob.clips[0], "clips set to data+4");
    FCK_EQ((u32)track.clip, (u32)&blob.clips[1], "selected clip 1");
    FCK_EQ(track.count, 3, "count read from data[0]");
    FCK_EQ(track.index, 1, "index is 1");
    FCK_EQ(track.step, 1, "forward step is +1");
    FCK_EQ(track.frame, 20, "frame starts at clip->start");
    FCK_EQ(track.target, 35, "forward target is clip->end");
    FCK_EQ(track.elapsed, 0, "elapsed cleared");
    FCK_EQ(track.flags, 0, "flags cleared");

    /* Reverse install on clip 1 (forward=0): step=-1, target=clip->start=20. */
    track.elapsed = 99;
    track.flags = 0x33;
    gSetAnimClipTable((u8 *)&blob, &track, 0, 1);
    FCK_EQ(track.step, -1, "reverse step is -1");
    FCK_EQ(track.target, 20, "reverse target is clip->start");
    FCK_EQ(track.elapsed, 0, "reverse elapsed cleared");
    FCK_EQ(track.flags, 0, "reverse flags cleared");

    /* Forward install on inverted clip 2 (50..40): start > end flips step to -1. */
    gSetAnimClipTable((u8 *)&blob, &track, 1, 2);
    FCK_EQ((u32)track.clip, (u32)&blob.clips[2], "selected clip 2");
    FCK_EQ(track.step, -1, "inverted clip flips forward step to -1");
    FCK_EQ(track.frame, 50, "inverted clip frame is 50");
    FCK_EQ(track.target, 40, "inverted clip target is 40");
}

/* ============================================================================
 * 6. 8004DA84 - gSendEntityCommand (links real 8004DA58 + 8004D9EC)
 * ============================================================================
 */
struct BEntitySlot {
    u8 kind;
    u8 value;
};
struct BEntity {
    struct BEntity *next;
    void *update;
    s32 id;
    struct BEntitySlot slots[4];
    u16 tag;
    u8 type;
    u8 pad17;
};
extern u8 gSendEntityCommand(struct BEntity *entity, s32 kind, s32 value, s32 retry)
    __asm__("func_8004DA84");

static void test_8004DA84(void)
{
    struct BEntity ent;

    /* @leaf 8004DA84 source entity-type bypass, queued-kind filter, 4-slot storage and pending-message retry */
    LK_FUNC("func_8004DA84");

    /* Immediate-accept types (2, 3, 4, 7, 15, 23) return 1 without touching slots. */
    lk_fill(&ent, 0, sizeof(ent));
    ent.type = 7;
    FCK_EQ(gSendEntityCommand(&ent, 0x22, 0x33, 0), 1, "type 7 returns 1 immediately");
    FCK_EQ(ent.slots[0].kind, 0, "type 7 does not occupy slot 0");

    ent.type = 23;
    FCK_EQ(gSendEntityCommand(&ent, 0x22, 0x33, 0), 1, "type 23 returns 1 immediately");
    FCK_EQ(ent.slots[0].kind, 0, "type 23 does not occupy slot 0");

    /* Type 18/21 filter via real gIsQueuedCommandKind (0x3C..0x45 or 0x64..0x65). */
    ent.type = 18;
    FCK_EQ(gSendEntityCommand(&ent, 0x3B, 0x11, 0), 1, "type 18 non-queued kind 0x3B returns 1 immediately");
    FCK_EQ(ent.slots[0].kind, 0, "non-queued kind does not write slot");

    FCK_EQ(gSendEntityCommand(&ent, 0x3C, 0x77, 0), 1, "type 18 queued kind 0x3C stored");
    FCK_EQ(ent.slots[0].kind, 0x3C, "slot 0 kind 0x3C");
    FCK_EQ(ent.slots[0].value, 0x77, "slot 0 value 0x77");

    ent.type = 21;
    FCK_EQ(gSendEntityCommand(&ent, 0x65, 0x88, 0), 1, "type 21 queued kind 0x65 stored in slot 1");
    FCK_EQ(ent.slots[1].kind, 0x65, "slot 1 kind 0x65");
    FCK_EQ(ent.slots[1].value, 0x88, "slot 1 value 0x88");

    /* Ordinary type (e.g. 5) fills remaining slots 2 and 3, then fails on retry=0. */
    ent.type = 5;
    ent.id = 0x4242;
    FCK_EQ(gSendEntityCommand(&ent, 0x10, 0xA1, 0), 1, "type 5 stores slot 2");
    FCK_EQ(gSendEntityCommand(&ent, 0x11, 0xA2, 0), 1, "type 5 stores slot 3");
    FCK_EQ(ent.slots[2].kind, 0x10, "slot 2 kind");
    FCK_EQ(ent.slots[3].value, 0xA2, "slot 3 value");
    FCK_EQ(gSendEntityCommand(&ent, 0x12, 0xA3, 0), 0, "full slots with retry=0 returns 0");

    /* Full slots with retry=1 enqueues into real gPendingMessages via gEnqueuePendingMessage. */
    lk_fill(gPendingMessages, 0, sizeof(gPendingMessages));
    gPendingMessageMask = 0x0003; /* bits 0,1 occupied -> picks bit 2 (slot 2) */
    FCK_EQ(gSendEntityCommand(&ent, 0x12, 0xA3, 1), 1, "full slots retry=1 succeeds");
    FCK_EQ(gPendingMessageMask, 0x0007, "pending mask sets bit 2");
    FCK_EQ((u32)gPendingMessages[2].target, (u32)&ent, "pending[2].target");
    FCK_EQ(gPendingMessages[2].id, 0x4242, "pending[2].id");
    FCK_EQ(gPendingMessages[2].kind, 0x12, "pending[2].kind");
    FCK_EQ(gPendingMessages[2].value, 0xA3, "pending[2].value");
    FCK_EQ(gPendingMessages[2].timer, 600, "pending[2].timer is 600");

    /* Pending queue full (0xFFFF) returns 0 even with retry=1. */
    gPendingMessageMask = 0xFFFF;
    FCK_EQ(gSendEntityCommand(&ent, 0x13, 0xA4, 1), 0, "full pending mask returns 0");
}

/* ============================================================================
 * 7. 8004DC64 - gSendCommandByTag (calls real 8004DA84!)
 * ============================================================================
 */
extern void gSendCommandByTag(s32 arg0, s32 kind, s32 value) __asm__("func_8004DC64");

static void test_8004DC64(void)
{
    struct BEntity e1, e2, e3, e4;

    /* @leaf 8004DC64 source special-kind direct dispatch (0x7C/0x7E/0x69) and tagged entity-list broadcast */
    LK_FUNC("func_8004DC64");

    lk_fill(&e1, 0, sizeof(e1));
    lk_fill(&e2, 0, sizeof(e2));
    lk_fill(&e3, 0, sizeof(e3));
    lk_fill(&e4, 0, sizeof(e4));
    e1.next = &e2; e1.id = 10; e1.tag = 0x200; e1.type = 5;
    e2.next = &e3; e2.id = 0;  e2.tag = 0x200; e2.type = 5; /* id==0 skipped */
    e3.next = &e4; e3.id = 30; e3.tag = 0x201; e3.type = 5; /* tag mismatch skipped */
    e4.next = 0;   e4.id = 40; e4.tag = 0x200; e4.type = 5;
    gUnitListHead = &e1;

    /* Special kinds bypass list walk. */
    reset_tag_cmd();
    gSendCommandByTag(11, 0x7C, 22);
    FCK_EQ(s_tag_cmd.zone_calls, 1, "0x7C calls zone handler");
    FCK_EQ(s_tag_cmd.zone_a0, 11, "0x7C arg0");
    FCK_EQ(s_tag_cmd.zone_kind, 0x7C, "0x7C kind");
    FCK_EQ(s_tag_cmd.zone_val, 22, "0x7C val");
    FCK_EQ(e1.slots[0].kind, 0, "0x7C does not touch list");

    gSendCommandByTag(33, 0x7E, 44);
    FCK_EQ(s_tag_cmd.timer_calls, 1, "0x7E calls timer handler");
    FCK_EQ(s_tag_cmd.timer_a0, 33, "0x7E arg0");
    FCK_EQ(s_tag_cmd.timer_kind, 0x7E, "0x7E kind");
    FCK_EQ(s_tag_cmd.timer_val, 44, "0x7E val");

    gSendCommandByTag(55, 0x69, 66);
    FCK_EQ(s_tag_cmd.ign_calls, 1, "0x69 calls ignore69 handler");
    FCK_EQ(s_tag_cmd.ign_a0, 55, "0x69 arg0");
    FCK_EQ(s_tag_cmd.ign_kind, 0x69, "0x69 kind");
    FCK_EQ(s_tag_cmd.ign_val, 66, "0x69 val");

    /* Ordinary kind walks list and delivers to matching tag && id != 0 via real gSendEntityCommand. */
    gSendCommandByTag(0x200, 0x25, 0x99);
    FCK_EQ(e1.slots[0].kind, 0x25, "e1 matched kind");
    FCK_EQ(e1.slots[0].value, 0x99, "e1 matched value");
    FCK_EQ(e2.slots[0].kind, 0, "e2 id==0 skipped");
    FCK_EQ(e3.slots[0].kind, 0, "e3 wrong tag skipped");
    FCK_EQ(e4.slots[0].kind, 0x25, "e4 matched kind");
    FCK_EQ(e4.slots[0].value, 0x99, "e4 matched value");

    /* Verify retry=1 is passed when matching entity's 4 slots are full. */
    e4.slots[1].kind = 1; e4.slots[2].kind = 2; e4.slots[3].kind = 3;
    lk_fill(gPendingMessages, 0, sizeof(gPendingMessages));
    gPendingMessageMask = 0;
    gSendCommandByTag(0x200, 0x26, 0x7A);
    FCK_EQ(gPendingMessageMask, 1, "retry=1 enqueued for full e4");
    FCK_EQ((u32)gPendingMessages[0].target, (u32)&e4, "pending target is e4");
    FCK_EQ(gPendingMessages[0].kind, 0x26, "pending kind 0x26");
    FCK_EQ(gPendingMessages[0].value, 0x7A, "pending value 0x7A");

    /* Empty list does nothing. */
    gUnitListHead = 0;
    gSendCommandByTag(0x200, 0x27, 0x55);
    FCK_EQ(gPendingMessageMask, 1, "empty list leaves pending mask unchanged");
}

/* ============================================================================
 * 8. 8006F170 - gTickTracerPool (links real 8003BF1C)
 * ============================================================================
 */
extern void gTickTracerPool(void *arg) __asm__("func_8006F170");

static void test_8006F170(void)
{
    struct BPoolNode n1, n2, n3, n4, f0;
    int cookie = 123;

    /* @leaf 8006F170 source active pool walk retiring head, middle and tail nodes via real gReleasePoolNode */
    LK_FUNC("func_8006F170");

    /* Empty pool: no calls. */
    reset_tracer_step();
    gTracerPool.storage = 0;
    gTracerPool.active = 0;
    gTracerPool.free = &f0;
    f0.next = 0;
    gTickTracerPool(&cookie);
    FCK_EQ(s_tracer_step.calls, 0, "empty active list makes 0 calls");
    FCK_EQ((u32)gTracerPool.free, (u32)&f0, "empty free list untouched");

    /* Four nodes: n1 retired (ret=0), n2 kept (ret=1), n3 retired (ret=2 != 1), n4 kept (ret=1). */
    n1.next = &n2; n1.tag = 1;
    n2.next = &n3; n2.tag = 2;
    n3.next = &n4; n3.tag = 3;
    n4.next = 0;   n4.tag = 4;
    f0.next = 0;   f0.tag = 99;
    gTracerPool.active = &n1;
    gTracerPool.free = &f0;

    reset_tracer_step();
    s_tracer_step.ret[0] = 0; /* retire head n1 */
    s_tracer_step.ret[1] = 1; /* keep n2 */
    s_tracer_step.ret[2] = 2; /* retire middle n3 (must test != 1, not == 0) */
    s_tracer_step.ret[3] = 1; /* keep tail n4 */
    gTickTracerPool(&cookie);

    FCK_EQ(s_tracer_step.calls, 4, "all 4 nodes stepped");
    FCK_EQ((u32)s_tracer_step.nodes[0], (u32)&n1, "stepped n1");
    FCK_EQ((u32)s_tracer_step.nodes[1], (u32)&n2, "stepped n2");
    FCK_EQ((u32)s_tracer_step.nodes[2], (u32)&n3, "stepped n3");
    FCK_EQ((u32)s_tracer_step.nodes[3], (u32)&n4, "stepped n4");
    FCK_EQ((u32)s_tracer_step.args[2], (u32)&cookie, "forwarded arg to step");

    /* Active list should now be n2 -> n4 -> NULL. */
    FCK_EQ((u32)gTracerPool.active, (u32)&n2, "active head is n2");
    FCK_EQ((u32)n2.next, (u32)&n4, "n2->next is n4");
    FCK_EQ((u32)n4.next, 0U, "n4->next is NULL");

    /* Free list should be n3 -> n1 -> f0 -> NULL. */
    FCK_EQ((u32)gTracerPool.free, (u32)&n3, "free head is n3");
    FCK_EQ((u32)n3.next, (u32)&n1, "n3->next is n1");
    FCK_EQ((u32)n1.next, (u32)&f0, "n1->next is f0");
}

/* ============================================================================
 * 9. 80029520 - gFlushDeferredAssetReleases
 * ============================================================================
 */
extern void gFlushDeferredAssetReleases(void) __asm__("func_80029520");

static void test_80029520(void)
{
    /* @leaf 80029520 source two-queue deferred handle release, order of count clearing and trailer reset */
    LK_FUNC("func_80029520");

    /* Both queues populated (2 + 2). */
    reset_asset_rel();
    gReleaseQueues.unk00[0] = 0x111;
    gReleaseQueues.unk00[1] = 0x222;
    gReleaseQueues.unk08[0] = 0x333;
    gReleaseQueues.unk08[1] = 0x444;
    gReleaseQueues.count10 = 2;
    gReleaseQueues.count11 = 2;
    gReleaseQueues.unk12 = 0x77;
    gReleaseQueues.unk13 = 0x12;

    gFlushDeferredAssetReleases();
    FCK_EQ(s_asset_rel.calls, 4, "4 handles released");
    FCK_EQ(s_asset_rel.handles[0], 0x111, "queue0[0]");
    FCK_EQ(s_asset_rel.handles[1], 0x222, "queue0[1]");
    FCK_EQ(s_asset_rel.handles[2], 0x333, "queue1[0]");
    FCK_EQ(s_asset_rel.handles[3], 0x444, "queue1[1]");
    FCK_EQ(s_asset_rel.seen_count10[2], 0, "count10 zeroed before queue1 loop");
    FCK_EQ(gReleaseQueues.count10, 0, "count10 cleared");
    FCK_EQ(gReleaseQueues.count11, 0, "count11 cleared");
    FCK_EQ(gReleaseQueues.unk12, 0, "unk12 cleared");
    FCK_EQ(gReleaseQueues.unk13, 0xFF, "unk13 set to 0xFF");

    /* Empty queues make 0 calls but still reset unk12=0 and unk13=0xFF. */
    reset_asset_rel();
    gReleaseQueues.count10 = 0;
    gReleaseQueues.count11 = 0;
    gReleaseQueues.unk12 = 9;
    gReleaseQueues.unk13 = 3;
    gFlushDeferredAssetReleases();
    FCK_EQ(s_asset_rel.calls, 0, "empty queues make 0 release calls");
    FCK_EQ(gReleaseQueues.unk12, 0, "empty flush clears unk12");
    FCK_EQ(gReleaseQueues.unk13, 0xFF, "empty flush sets unk13=0xFF");
}

/* ============================================================================
 * 10. 80069B58 - gInitScriptVmState (links real 80069B1C)
 * ============================================================================
 */
extern void gInitScriptVmState(void) __asm__("func_80069B58");

static void test_80069B58(void)
{
    u8 sess[0x100];
    s32 opcode_buf[34];
    s32 vars_buf[102];
    s32 cd_buf[34];
    s32 i;

    /* @leaf 80069B58 source script VM random modulo-1000 fill, 100-var clear, 32-countdown -10 init and real pointer bind */
    LK_FUNC("func_80069B58");

    lk_fill(sess, 0xAA, sizeof(sess));
    for (i = 0; i < 34; i++) opcode_buf[i] = 0x55555555;
    for (i = 0; i < 102; i++) vars_buf[i] = 0x44444444;
    for (i = 0; i < 34; i++) cd_buf[i] = 0x33333333;

    gGameSession[0] = sess;
    gOpcodeTable = &opcode_buf[1];
    gScriptVmVars = &vars_buf[1];
    gVmCountdowns = &cd_buf[1];
    gScriptVmDisabled = 1;

    reset_rng();
    s_rng.count = 36;
    s_rng.seq[0] = 0U;
    s_rng.seq[1] = 999U;
    s_rng.seq[2] = 1000U;
    s_rng.seq[3] = 0xFFFFFFFFU; /* unsigned % 1000 == 295 */
    for (i = 0; i < 32; i++) {
        s_rng.seq[4 + i] = (u32)(1000 * (i + 1) + i * 7);
    }

    gInitScriptVmState();

    FCK_EQ(s_rng.calls, 36, "36 RNG draws (4 session + 32 opcode)");
    FCK_EQ(*(u32 *)(sess + 0x18), 0U, "sess+0x18 = 0 % 1000");
    FCK_EQ(*(u32 *)(sess + 0x1C), 999U, "sess+0x1C = 999 % 1000");
    FCK_EQ(*(u32 *)(sess + 0x20), 0U, "sess+0x20 = 1000 % 1000");
    FCK_EQ(*(u32 *)(sess + 0x24), 295U, "sess+0x24 = 0xFFFFFFFFu % 1000");

    FCK_EQ(opcode_buf[0], 0x55555555, "opcode leading canary");
    FCK_EQ(opcode_buf[1], 0, "opcode[0] = 1000 % 1000");
    FCK_EQ(opcode_buf[2], 7, "opcode[1] = 2007 % 1000");
    FCK_EQ(opcode_buf[32], 31 * 7, "opcode[31] = 32217 % 1000");
    FCK_EQ(opcode_buf[33], 0x55555555, "opcode trailing canary");

    FCK_EQ(vars_buf[0], 0x44444444, "vars leading canary");
    FCK_EQ(vars_buf[1], 0, "vars[0] zeroed");
    FCK_EQ(vars_buf[50], 0, "vars[49] zeroed");
    FCK_EQ(vars_buf[100], 0, "vars[99] zeroed");
    FCK_EQ(vars_buf[101], 0x44444444, "vars trailing canary");

    FCK_EQ(cd_buf[0], 0x33333333, "countdown leading canary");
    FCK_EQ(cd_buf[1], -10, "countdown[0] = -10");
    FCK_EQ(cd_buf[32], -10, "countdown[31] = -10");
    FCK_EQ(cd_buf[33], 0x33333333, "countdown trailing canary");

    /* Real gBindGameSessionPointers binds the 4 global addresses at +0xE4..+0xF0. */
    FCK_EQ(*(u32 *)(sess + 0xE4), (u32)gFrameTime, "sess+0xE4 bound to D_8009FAA4");
    FCK_EQ(*(u32 *)(sess + 0xE8), (u32)gScriptVmAuxVar, "sess+0xE8 bound to D_8009FAB4");
    FCK_EQ(*(u32 *)(sess + 0xEC), (u32)gScreenResetFlag, "sess+0xEC bound to D_8009FA70");
    FCK_EQ(*(u32 *)(sess + 0xF0), (u32)gEffectId, "sess+0xF0 bound to D_8009FACC");
    FCK_EQ(gScriptVmDisabled, 0, "gScriptVmDisabled cleared to 0");
}

/* ============================================================================
 * 11. 80031990 - gFreeMapZoneList
 * ============================================================================
 */
struct BZoneNode {
    u8 pad[0x24];
    struct BZoneNode *next;
};
extern void gFreeMapZoneList(void) __asm__("func_80031990");

static void test_80031990(void)
{
    struct BZoneNode z1, z2, z3;

    /* @leaf 80031990 source singly-linked map zone list drain (empty and multi-node) and head clear */
    LK_FUNC("func_80031990");

    /* Empty list: 0 frees. */
    reset_free();
    gMapZoneList = 0;
    gFreeMapZoneList();
    FCK_EQ(s_free.calls, 0, "empty list frees nothing");
    FCK_EQ((u32)gMapZoneList, 0U, "empty head stays NULL");

    /* Three-node chain: frees z1, z2, z3 in order and zeroes gMapZoneList. */
    reset_free();
    z1.next = &z2;
    z2.next = &z3;
    z3.next = 0;
    gMapZoneList = &z1;
    gFreeMapZoneList();
    FCK_EQ(s_free.calls, 3, "3 nodes freed");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&z1, "freed z1 first");
    FCK_EQ((u32)s_free.ptrs[1], (u32)&z2, "freed z2 second");
    FCK_EQ((u32)s_free.ptrs[2], (u32)&z3, "freed z3 third");
    FCK_EQ((u32)gMapZoneList, 0U, "head cleared to NULL");
}

/* ============================================================================
 * 12. 8003FD74 - gQueueDeferredSpawnRequest
 * ============================================================================
 */
struct BSpawnSource {
    u8 unk00[8];
    s32 value08;
};
struct BSpawnNode {
    struct BSpawnNode *next;
    u8 type;
    u8 unk05[3];
    s32 arg08;
    u8 unk0C[4];
    struct BSpawnSource *source;
    s32 sourceValue;
    s32 arg18;
    u8 flags;
    u8 unk1D[3];
};
extern struct BSpawnNode *gQueueDeferredSpawnRequest(s32 type, s32 arg08,
    struct BSpawnSource *source, s32 arg18) __asm__("func_8003FD74");

static void test_8003FD74(void)
{
    struct BSpawnNode existing, n1, n2;
    struct BSpawnSource src;
    struct BSpawnNode *ret;

    /* @leaf 8003FD74 source deferred spawn request allocation (0x20/tag 1), OOM guard, source caching and head prepend */
    LK_FUNC("func_8003FD74");

    /* OOM returns 0 and leaves list head untouched. */
    reset_alloc();
    s_alloc.results[0] = 0;
    gDeferredSpawnList = &existing;
    ret = gQueueDeferredSpawnRequest(5, 9, 0, 0x1234);
    FCK_EQ((u32)ret, 0U, "OOM returns NULL");
    FCK_EQ(s_alloc.calls, 1, "1 alloc call on OOM");
    FCK_EQ(s_alloc.sizes[0], 0x20, "alloc size 0x20");
    FCK_EQ(s_alloc.tags[0], 1, "alloc tag 1");
    FCK_EQ((u32)gDeferredSpawnList, (u32)&existing, "OOM preserves list head");

    /* With non-null source: caches source->value08, prepends to existing head. */
    reset_alloc();
    lk_fill(&n1, 0xEE, sizeof(n1));
    s_alloc.results[0] = &n1;
    src.value08 = 0x51525354;
    ret = gQueueDeferredSpawnRequest(0x2A, 77, &src, 0xCAFE);
    FCK_EQ((u32)ret, (u32)&n1, "returns allocated node n1");
    FCK_EQ((u32)gDeferredSpawnList, (u32)&n1, "n1 prepended as head");
    FCK_EQ((u32)n1.next, (u32)&existing, "n1.next points to old head");
    FCK_EQ(n1.type, 0x2A, "n1.type");
    FCK_EQ(n1.arg08, 77, "n1.arg08");
    FCK_EQ((u32)n1.source, (u32)&src, "n1.source");
    FCK_EQ(n1.sourceValue, 0x51525354, "n1.sourceValue cached");
    FCK_EQ(n1.arg18, 0xCAFE, "n1.arg18");
    FCK_EQ(n1.flags, 0, "n1.flags cleared");

    /* With null source: does not overwrite sourceValue, prepends ahead of n1. */
    reset_alloc();
    lk_fill(&n2, 0xDD, sizeof(n2));
    n2.sourceValue = 0x13572468;
    s_alloc.results[0] = &n2;
    ret = gQueueDeferredSpawnRequest(0x03, -5, 0, 0xBEEF);
    FCK_EQ((u32)ret, (u32)&n2, "returns allocated node n2");
    FCK_EQ((u32)gDeferredSpawnList, (u32)&n2, "n2 prepended as new head");
    FCK_EQ((u32)n2.next, (u32)&n1, "n2.next points to n1");
    FCK_EQ((u32)n2.source, 0U, "n2.source is NULL");
    FCK_EQ(n2.sourceValue, 0x13572468, "null source leaves sourceValue untouched");
}

/* ============================================================================
 * 13. 80073500 - gAllocPositionNode
 * ============================================================================
 */
struct BVec3 { s32 x, y, z; };
struct BPosNode {
    void *coord;
    void *owner;
    s32 ownerId;
    struct BVec3 pos;
    s16 unk18;
    s16 unk1A;
    s16 unk1C;
    s16 unk1E;
};
extern struct BPosNode *gAllocPositionNode(struct BVec3 *position, s16 arg1, s16 arg2, s16 arg3)
    __asm__("func_80073500");

static void test_80073500(void)
{
    struct BVec3 pos;
    struct BPosNode node;
    struct BPosNode *ret;

    /* @leaf 80073500 source 0x20-byte position node constructor (tag 1), OOM return and reordered s16 tail */
    LK_FUNC("func_80073500");

    pos.x = -12345; pos.y = 67890; pos.z = 0x123456;

    /* OOM returns NULL. */
    reset_alloc();
    s_alloc.results[0] = 0;
    ret = gAllocPositionNode(&pos, 10, 20, 30);
    FCK_EQ((u32)ret, 0U, "OOM returns NULL");
    FCK_EQ(s_alloc.sizes[0], 0x20, "alloc size 0x20");
    FCK_EQ(s_alloc.tags[0], 1, "alloc tag 1");

    /* Normal allocation populates coord=0, owner=0, pos, and reordered args; leaves ownerId/+0x1E intact. */
    reset_alloc();
    lk_fill(&node, 0xAA, sizeof(node));
    node.ownerId = 0x22334455;
    node.unk1E = 0x1234;
    s_alloc.results[0] = &node;
    ret = gAllocPositionNode(&pos, -11, 222, -333);
    FCK_EQ((u32)ret, (u32)&node, "returns allocated node");
    FCK_EQ((u32)node.coord, 0U, "coord zeroed");
    FCK_EQ((u32)node.owner, 0U, "owner zeroed");
    FCK_EQ(node.ownerId, 0x22334455, "ownerId untouched");
    FCK_EQ(node.pos.x, -12345, "pos.x copied");
    FCK_EQ(node.pos.y, 67890, "pos.y copied");
    FCK_EQ(node.pos.z, 0x123456, "pos.z copied");
    FCK_EQ(node.unk18, -333, "arg3 -> +0x18");
    FCK_EQ(node.unk1A, -11, "arg1 -> +0x1A");
    FCK_EQ(node.unk1C, 222, "arg2 -> +0x1C");
    FCK_EQ(node.unk1E, 0x1234, "+0x1E untouched");
}

/* ============================================================================
 * 14. 80073590 - gAllocAttachedEffectNode
 * ============================================================================
 */
struct BAttOwner {
    u8 unk00[8];
    s32 id;
    u8 unk0C[0x1C];
    u8 coord[4];
};
struct BAttNode {
    void *coord;
    struct BAttOwner *owner;
    s32 ownerId;
    u8 kind;
    u8 unk0D[0x0B];
    s16 unk18;
    s16 unk1A;
    s16 unk1C;
    s16 unk1E;
};
extern struct BAttNode *gAllocAttachedEffectNode(struct BAttOwner *owner, void *coord, u8 kind,
    s16 arg3, s16 arg4, s16 arg5) __asm__("func_80073590");

static void test_80073590(void)
{
    struct BAttOwner owner;
    struct BAttNode node;
    struct BAttNode *ret;
    u32 explicit_coord[3];

    /* @leaf 80073590 source attached effect node constructor: OOM, owner+0x28 coord fallback, explicit coord, null owner */
    LK_FUNC("func_80073590");

    lk_fill(&owner, 0, sizeof(owner));
    owner.id = 0x76543210;

    /* OOM returns NULL. */
    reset_alloc();
    s_alloc.results[0] = 0;
    ret = gAllocAttachedEffectNode(&owner, 0, 7, 1, 2, 3);
    FCK_EQ((u32)ret, 0U, "OOM returns NULL");
    FCK_EQ(s_alloc.sizes[0], 0x20, "alloc size 0x20");
    FCK_EQ(s_alloc.tags[0], 1, "alloc tag 1");

    /* coord == 0 with non-null owner falls back to &owner.coord[0] (owner + 0x28). */
    reset_alloc();
    lk_fill(&node, 0xBB, sizeof(node));
    s_alloc.results[0] = &node;
    ret = gAllocAttachedEffectNode(&owner, 0, 0x19, 101, -202, 303);
    FCK_EQ((u32)ret, (u32)&node, "returns node");
    FCK_EQ((u32)node.coord, (u32)owner.coord, "coord falls back to owner+0x28");
    FCK_EQ((u32)node.owner, (u32)&owner, "owner stored");
    FCK_EQ(node.ownerId, 0x76543210, "ownerId copied");
    FCK_EQ(node.kind, 0x19, "kind stored");
    FCK_EQ(node.unk18, 303, "arg5 -> +0x18");
    FCK_EQ(node.unk1A, 101, "arg3 -> +0x1A");
    FCK_EQ(node.unk1C, -202, "arg4 -> +0x1C");

    /* Explicit coord overrides owner->coord while still copying owner->id. */
    reset_alloc();
    lk_fill(&node, 0xCC, sizeof(node));
    s_alloc.results[0] = &node;
    ret = gAllocAttachedEffectNode(&owner, explicit_coord, 0x2A, 4, 5, 6);
    FCK_EQ((u32)node.coord, (u32)explicit_coord, "explicit coord kept");
    FCK_EQ(node.ownerId, 0x76543210, "ownerId copied with explicit coord");

    /* Null owner with null coord leaves ownerId untouched and sets coord=0, owner=0. */
    reset_alloc();
    lk_fill(&node, 0xDD, sizeof(node));
    node.ownerId = 0x11223344;
    s_alloc.results[0] = &node;
    ret = gAllocAttachedEffectNode(0, 0, 0x3B, 7, 8, 9);
    FCK_EQ((u32)node.coord, 0U, "null owner + null coord -> coord=0");
    FCK_EQ((u32)node.owner, 0U, "owner=0");
    FCK_EQ(node.ownerId, 0x11223344, "null owner leaves ownerId untouched");
    FCK_EQ(node.kind, 0x3B, "kind stored for null owner");
}

/* ============================================================================
 * 15. 800834C4 - gAllocCellBucketGrid
 * ============================================================================
 */
extern void gAllocCellBucketGrid(s32 width, s32 height) __asm__("func_800834C4");

static void test_800834C4(void)
{
    s32 buf[16];
    s32 i;

    /* @leaf 800834C4 source scene-graph bucket grid allocation (width*height*8, tag 2) and full downward zeroing */
    LK_FUNC("func_800834C4");

    /* 3 x 2 = 6 cells = 12 words = 48 bytes; surround with sentinel words. */
    for (i = 0; i < 16; i++) buf[i] = 0x7E7E7E7E;
    reset_alloc();
    s_alloc.results[0] = &buf[1];
    gCellGridWidth = -1;
    gCellGridHeight = -1;
    gCellBucketGrid = 0;

    gAllocCellBucketGrid(3, 2);
    FCK_EQ(gCellGridWidth, 3, "width recorded");
    FCK_EQ(gCellGridHeight, 2, "height recorded");
    FCK_EQ(s_alloc.calls, 1, "1 alloc call");
    FCK_EQ(s_alloc.sizes[0], 48, "alloc size is 3*2*8=48");
    FCK_EQ(s_alloc.tags[0], 2, "alloc tag is 2");
    FCK_EQ((u32)gCellBucketGrid, (u32)&buf[1], "grid pointer published");
    FCK_EQ(buf[0], 0x7E7E7E7E, "leading sentinel untouched");
    FCK_EQ(buf[1], 0, "cell 0 word 0 zeroed");
    FCK_EQ(buf[2], 0, "cell 0 word 1 zeroed");
    FCK_EQ(buf[11], 0, "cell 5 word 0 zeroed");
    FCK_EQ(buf[12], 0, "cell 5 word 1 zeroed");
    FCK_EQ(buf[13], 0x7E7E7E7E, "trailing sentinel untouched");

    /* Zero-size grid (0 x 5): allocates 0 bytes and writes nothing to buffer. */
    buf[1] = 0x12345678;
    reset_alloc();
    s_alloc.results[0] = &buf[1];
    gAllocCellBucketGrid(0, 5);
    FCK_EQ(s_alloc.sizes[0], 0, "0x5 allocates 0 bytes");
    FCK_EQ(gCellGridWidth, 0, "width 0 stored");
    FCK_EQ(gCellGridHeight, 5, "height 5 stored");
    FCK_EQ(buf[1], 0x12345678, "0-cell grid writes 0 words");
}

/* ============================================================================
 * 16. 8005A220 - gFreeTextureDescriptors
 * ============================================================================
 */
extern void gFreeTextureDescriptors(void) __asm__("func_8005A220");

static void test_8005A220(void)
{
    void *entries[3];
    int e0, e1, e2;

    /* @leaf 8005A220 source reverse-order descriptor entry free, table free, pointer clear and unconditional count reset */
    LK_FUNC("func_8005A220");

    /* Null table: makes 0 frees but still clears gTexDescriptorCount. */
    reset_free();
    gTextureDescriptorTable = 0;
    gTexDescriptorCount = 7;
    gFreeTextureDescriptors();
    FCK_EQ(s_free.calls, 0, "null table makes 0 frees");
    FCK_EQ(gTexDescriptorCount, 0, "null table still zeroes count");

    /* 3-entry table: frees entries[2], entries[1], entries[0], then table itself. */
    reset_free();
    entries[0] = &e0;
    entries[1] = &e1;
    entries[2] = &e2;
    gTextureDescriptorTable = entries;
    gTexDescriptorCount = 3;
    gFreeTextureDescriptors();
    FCK_EQ(s_free.calls, 4, "3 entries + table = 4 frees");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&e2, "freed entry 2 first");
    FCK_EQ((u32)s_free.ptrs[1], (u32)&e1, "freed entry 1 second");
    FCK_EQ((u32)s_free.ptrs[2], (u32)&e0, "freed entry 0 third");
    FCK_EQ((u32)s_free.ptrs[3], (u32)entries, "freed table last");
    FCK_EQ((u32)gTextureDescriptorTable, 0U, "table pointer nulled");
    FCK_EQ(gTexDescriptorCount, 0, "count zeroed");

    /* Non-null table with count=0: frees only the table pointer. */
    reset_free();
    gTextureDescriptorTable = entries;
    gTexDescriptorCount = 0;
    gFreeTextureDescriptors();
    FCK_EQ(s_free.calls, 1, "count=0 frees only table");
    FCK_EQ((u32)s_free.ptrs[0], (u32)entries, "freed table pointer");
    FCK_EQ((u32)gTextureDescriptorTable, 0U, "table pointer nulled");
}

/* ============================================================================
 * 17. 800860CC - gAllocDecodeBuffers
 * ============================================================================
 */
extern void gAllocDecodeBuffers(void) __asm__("func_800860CC");

static void test_800860CC(void)
{
    int b0, b1, b2, b3;

    /* @leaf 800860CC source four static decoder buffer allocations (0x6000, 0x600, 0x2000, 0x600; tag 2) in exact order */
    LK_FUNC("func_800860CC");

    reset_alloc();
    s_alloc.results[0] = &b0;
    s_alloc.results[1] = &b1;
    s_alloc.results[2] = &b2;
    s_alloc.results[3] = &b3;
    gPreallocatedMovieVlcTableA = (void *)0x11;
    gPreallocatedMovieVlcTableB = (void *)0x22;
    gWorkAreaBlock = (void *)0x33;
    gPreallocatedMovieMdecInputBuffer = (void *)0x44;

    gAllocDecodeBuffers();
    FCK_EQ(s_alloc.calls, 4, "4 allocations performed");
    FCK_EQ(s_alloc.sizes[0], 0x6000, "alloc 0 size 0x6000");
    FCK_EQ(s_alloc.tags[0], 2, "alloc 0 tag 2");
    FCK_EQ(s_alloc.sizes[1], 0x600, "alloc 1 size 0x600");
    FCK_EQ(s_alloc.tags[1], 2, "alloc 1 tag 2");
    FCK_EQ(s_alloc.sizes[2], 0x2000, "alloc 2 size 0x2000");
    FCK_EQ(s_alloc.tags[2], 2, "alloc 2 tag 2");
    FCK_EQ(s_alloc.sizes[3], 0x600, "alloc 3 size 0x600");
    FCK_EQ(s_alloc.tags[3], 2, "alloc 3 tag 2");
    FCK_EQ((u32)gPreallocatedMovieVlcTableA, (u32)&b0, "D_8009F948 gets alloc 0");
    FCK_EQ((u32)gPreallocatedMovieVlcTableB, (u32)&b1, "D_8009F94C gets alloc 1");
    FCK_EQ((u32)gPreallocatedMovieMdecInputBuffer, (u32)&b2, "D_8009F954 gets alloc 2");
    FCK_EQ((u32)gWorkAreaBlock, (u32)&b3, "D_8009F950 gets alloc 3");
}

/* ============================================================================
 * 18. 80086164 - gFreeMovieBuffers
 * ============================================================================
 */
extern void gFreeMovieBuffers(void) __asm__("func_80086164");

static void test_80086164(void)
{
    int b0, b1, b2, b3;

    /* @leaf 80086164 source guarded free of four decoder buffer pointers and unconditional clearing of all four */
    LK_FUNC("func_80086164");

    /* All four non-null: freed in order 948, 94C, 950, 954 then nulled. */
    reset_free();
    gPreallocatedMovieVlcTableA = &b0;
    gPreallocatedMovieVlcTableB = &b1;
    gWorkAreaBlock = &b2;
    gPreallocatedMovieMdecInputBuffer = &b3;
    gFreeMovieBuffers();
    FCK_EQ(s_free.calls, 4, "all 4 freed");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&b0, "freed D_8009F948 first");
    FCK_EQ((u32)s_free.ptrs[1], (u32)&b1, "freed D_8009F94C second");
    FCK_EQ((u32)s_free.ptrs[2], (u32)&b2, "freed D_8009F950 third");
    FCK_EQ((u32)s_free.ptrs[3], (u32)&b3, "freed D_8009F954 fourth");
    FCK_EQ((u32)gPreallocatedMovieVlcTableA, 0U, "948 cleared");
    FCK_EQ((u32)gPreallocatedMovieVlcTableB, 0U, "94C cleared");
    FCK_EQ((u32)gWorkAreaBlock, 0U, "950 cleared");
    FCK_EQ((u32)gPreallocatedMovieMdecInputBuffer, 0U, "954 cleared");

    /* Partial set (only 94C and 954 non-null): frees only those two. */
    reset_free();
    gPreallocatedMovieVlcTableA = 0;
    gPreallocatedMovieVlcTableB = &b1;
    gWorkAreaBlock = 0;
    gPreallocatedMovieMdecInputBuffer = &b3;
    gFreeMovieBuffers();
    FCK_EQ(s_free.calls, 2, "only 2 non-null freed");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&b1, "freed 94C");
    FCK_EQ((u32)s_free.ptrs[1], (u32)&b3, "freed 954");
    FCK_EQ((u32)gPreallocatedMovieVlcTableB, 0U, "94C cleared");
    FCK_EQ((u32)gPreallocatedMovieMdecInputBuffer, 0U, "954 cleared");

    /* All null: 0 frees. */
    reset_free();
    gFreeMovieBuffers();
    FCK_EQ(s_free.calls, 0, "all null makes 0 frees");
}

/* ============================================================================
 * 19. 800427E8 - gFreeEmitterObject
 * ============================================================================
 */
struct BEmitter {
    u8 unk00[0x6C];
    void *unk6C;
    u8 unk70[0x34];
    s16 unkA4;
    u8 unkA6[2];
    void *unkA8;
};
extern void gFreeEmitterObject(struct BEmitter *object) __asm__("func_800427E8");

static void test_800427E8(void)
{
    struct BEmitter em;
    int blk6c, blka8;

    /* @leaf 800427E8 source live-counter decrement by signed unkA4 and conditional/unconditional block teardown */
    LK_FUNC("func_800427E8");

    /* With optional unkA8 non-null: frees unkA8, unk6C, object in order. */
    reset_free();
    lk_fill(&em, 0, sizeof(em));
    em.unk6C = &blk6c;
    em.unkA4 = 17;
    em.unkA8 = &blka8;
    gRemainingCounter = 100;
    gFreeEmitterObject(&em);
    FCK_EQ(gRemainingCounter, 83, "counter decremented 100 - 17 = 83");
    FCK_EQ(s_free.calls, 3, "3 blocks freed when unkA8 != 0");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&blka8, "freed unkA8 first");
    FCK_EQ((u32)s_free.ptrs[1], (u32)&blk6c, "freed unk6C second");
    FCK_EQ((u32)s_free.ptrs[2], (u32)&em, "freed object last");

    /* With unkA8 == 0 and negative unkA4: counter increases, only 2 frees. */
    reset_free();
    em.unkA4 = -5;
    em.unkA8 = 0;
    gRemainingCounter = 40;
    gFreeEmitterObject(&em);
    FCK_EQ(gRemainingCounter, 45, "signed negative unkA4 increases counter 40 - (-5) = 45");
    FCK_EQ(s_free.calls, 2, "2 blocks freed when unkA8 == 0");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&blk6c, "freed unk6C first");
    FCK_EQ((u32)s_free.ptrs[1], (u32)&em, "freed object second");
}

/* ============================================================================
 * 20. 80071A68 - gFreeBeamNode
 * ============================================================================
 */
struct BBeamEntry {
    void *data;
    void *extra;
    s32 unk08;
    s32 unk0C;
};
struct BBeamNode {
    s32 unk00;
    void *first;
    void *second;
    s16 count;
    s16 unk0E;
    struct BBeamEntry *entries;
};
extern void gFreeBeamNode(struct BBeamNode *object) __asm__("func_80071A68");

static void test_80071A68(void)
{
    struct BBeamNode beam;
    struct BBeamEntry ents[2];
    int p_first, p_second, d0, x0, d1;

    /* @leaf 80071A68 source compound beam teardown: optional endpoint frees, entry extra/data loop, array and self free */
    LK_FUNC("func_80071A68");

    /* Full case: first, second, entry[0] with extra, entry[1] without extra. */
    reset_free();
    reset_beam_free();
    ents[0].data = &d0; ents[0].extra = &x0; ents[0].unk08 = 0; ents[0].unk0C = 0;
    ents[1].data = &d1; ents[1].extra = 0;   ents[1].unk08 = 0; ents[1].unk0C = 0;
    beam.first = &p_first;
    beam.second = &p_second;
    beam.count = 2;
    beam.entries = ents;

    gFreeBeamNode(&beam);
    FCK_EQ(s_beam_free.calls, 2, "both endpoints released via 80070710");
    FCK_EQ((u32)s_beam_free.ptrs[0], (u32)&p_first, "endpoint first");
    FCK_EQ((u32)s_beam_free.ptrs[1], (u32)&p_second, "endpoint second");
    FCK_EQ(s_free.calls, 5, "x0, d0, d1, entries, beam = 5 frees");
    FCK_EQ((u32)s_free.ptrs[0], (u32)&x0, "entry[0].extra freed");
    FCK_EQ((u32)s_free.ptrs[1], (u32)&d0, "entry[0].data freed");
    FCK_EQ((u32)s_free.ptrs[2], (u32)&d1, "entry[1].data freed (null extra skipped)");
    FCK_EQ((u32)s_free.ptrs[3], (u32)ents, "entries array freed");
    FCK_EQ((u32)s_free.ptrs[4], (u32)&beam, "beam object freed last");

    /* Minimal case: null first/second, count=0. */
    reset_free();
    reset_beam_free();
    beam.first = 0;
    beam.second = 0;
    beam.count = 0;
    beam.entries = ents;
    gFreeBeamNode(&beam);
    FCK_EQ(s_beam_free.calls, 0, "null endpoints skip 80070710");
    FCK_EQ(s_free.calls, 2, "count=0 frees entries + beam");
    FCK_EQ((u32)s_free.ptrs[0], (u32)ents, "entries freed");
    FCK_EQ((u32)s_free.ptrs[1], (u32)&beam, "beam freed");
}

/* ============================================================================
 * 21. 80045678 - gAllocPositionalSoundChannel (uses real 80045494 + 800455DC!)
 * ============================================================================
 */
extern s32 gAllocPositionalSoundChannel(s32 sound, s32 id, s32 arg2, s32 arg3)
    __asm__("func_80045678");

static void test_80045678(void)
{
    struct BSoundChannel ch0, ch1, existing;
    s32 idx;

    /* @leaf 80045678 source sound channel allocation using real ID resolver and real priority queue insertion */
    LK_FUNC("func_80045678");

    lk_fill(gSoundBankTableLow, 0, sizeof(gSoundBankTableLow));
    lk_fill(gSoundBankTableHigh, 0, sizeof(gSoundBankTableHigh));
    lk_fill(gSoundEntryTable, 0, sizeof(gSoundEntryTable));
    gSoundBankTableLow[3] = 14;
    gSoundEntryTable[14].active = 1;

    lk_fill(&ch0, 0xCC, sizeof(ch0));
    lk_fill(&ch1, 0xCC, sizeof(ch1));
    lk_fill(&existing, 0, sizeof(existing));
    ch0.index = 7; ch0.next = &ch1;
    ch1.index = 9; ch1.next = 0;
    existing.id = 1; existing.priority = 5; existing.next = 0; existing.prev = 0;

    gChannelListHead = &ch0;
    gSoundChannelQueueHead = &existing;
    gSoundChannelQueueTail = &existing;
    gChannelCount = 1;

    /* Unresolved sound ID returns -1 without touching queue or count. */
    idx = gAllocPositionalSoundChannel(2, 0x100, 11, 22);
    FCK_EQ(idx, -1, "unknown sound returns -1");
    FCK_EQ(gChannelCount, 1, "count unchanged on unknown sound");
    FCK_EQ((u32)gChannelListHead, (u32)&ch0, "free head unchanged");

    /* Busy cap (gChannelCount == 0x1E) returns -1. */
    gChannelCount = 0x1E;
    idx = gAllocPositionalSoundChannel(3, 0x100, 11, 22);
    FCK_EQ(idx, -1, "channel cap 0x1E returns -1");
    FCK_EQ((u32)gChannelListHead, (u32)&ch0, "free head unchanged at cap");

    /* Normal allocation at count 0x1D (< 0x1E): pops ch0, initializes fields, appends at tail. */
    gChannelCount = 0x1D;
    idx = gAllocPositionalSoundChannel(3, 0xABCD, 0x111, 0x222);
    FCK_EQ(idx, 7, "returns popped channel->index 7");
    FCK_EQ((u32)gChannelListHead, (u32)&ch1, "free list advanced to ch1");
    FCK_EQ(ch0.id, 0xABCD, "ch0.id set");
    FCK_EQ(ch0.unk28, 0, "ch0.unk28 cleared");
    FCK_EQ(ch0.slot, 14, "ch0.slot set to resolved 14");
    FCK_EQ(ch0.unk0C, 0x111, "ch0.unk0C set");
    FCK_EQ(ch0.unk10, 0x222, "ch0.unk10 set");
    FCK_EQ(ch0.priority, 0, "ch0.priority cleared");
    FCK_EQ((u32)existing.next, (u32)&ch0, "appended after existing tail");
    FCK_EQ((u32)ch0.prev, (u32)&existing, "ch0.prev links to existing");
    FCK_EQ((u32)ch0.next, 0U, "ch0.next is NULL");
    FCK_EQ((u32)gSoundChannelQueueTail, (u32)&ch0, "queue tail updated to ch0");
    FCK_EQ(gChannelCount, 0x1E, "queue insertion incremented count to 0x1E");
}

/* ============================================================================
 * 22. 800457B8 - gStopSoundChannelByIndex
 * ============================================================================
 */
extern void gStopSoundChannelByIndex(s32 arg0, s32 index) __asm__("func_800457B8");

static void test_800457B8(void)
{
    /* @leaf 800457B8 source sound channel stop guards (index<0, id==-1), conditional voice release and channel unlink */
    LK_FUNC("func_800457B8");

    lk_fill(gVoiceTable, 0, sizeof(gVoiceTable));
    gVoiceTable[0].id = -1;
    gVoiceTable[1].id = 0x44; gVoiceTable[1].unk28 = 0x999;
    gVoiceTable[2].id = 0x55; gVoiceTable[2].unk28 = 0;

    /* Negative index aborts. */
    reset_sound_stop();
    gStopSoundChannelByIndex(0x1234, -1);
    FCK_EQ(s_sound_stop.voice_calls, 0, "index -1 skips voice release");
    FCK_EQ(s_sound_stop.chan_calls, 0, "index -1 skips channel release");

    /* Already-free channel (id == -1) aborts. */
    reset_sound_stop();
    gStopSoundChannelByIndex(0x1234, 0);
    FCK_EQ(s_sound_stop.voice_calls, 0, "id==-1 skips voice release");
    FCK_EQ(s_sound_stop.chan_calls, 0, "id==-1 skips channel release");

    /* Active channel with voice handle: clears id=-1, releases voice handle, unlinks channel. */
    reset_sound_stop();
    gStopSoundChannelByIndex(0x1234, 1);
    FCK_EQ(gVoiceTable[1].id, -1, "channel 1 id set to -1");
    FCK_EQ(s_sound_stop.voice_calls, 1, "voice handle released");
    FCK_EQ(s_sound_stop.last_handle, 0x999, "released handle 0x999");
    FCK_EQ(s_sound_stop.chan_calls, 1, "channel released");
    FCK_EQ((u32)s_sound_stop.last_chan, (u32)&gVoiceTable[1], "released &gVoiceTable[1]");
    FCK_EQ(s_sound_stop.seen_id_on_chan, -1, "id already -1 before channel release");

    /* Active channel with unk28 == 0: skips voice release, still clears id and unlinks channel. */
    reset_sound_stop();
    gStopSoundChannelByIndex(0x1234, 2);
    FCK_EQ(gVoiceTable[2].id, -1, "channel 2 id set to -1");
    FCK_EQ(s_sound_stop.voice_calls, 0, "zero unk28 skips voice release");
    FCK_EQ(s_sound_stop.chan_calls, 1, "channel 2 still released");
    FCK_EQ((u32)s_sound_stop.last_chan, (u32)&gVoiceTable[2], "released &gVoiceTable[2]");
}

/* ============================================================================
 * 23. 8005A6A4 - gSpawnDynamicLightEffect (links real 8005A654)
 * ============================================================================
 */
struct BLightSample {
    s16 id;
    u8 rate;
    u8 pad;
};
struct BLightObj {
    u8 unk00[0x15];
    u8 slot;
    u8 unk16[0x12];
    s16 scale;
};
extern s32 gSpawnDynamicLightEffect(s32 id, struct BLightObj *object, s32 *result)
    __asm__("func_8005A6A4");

static void test_8005A6A4(void)
{
    struct BLightSample s0, s1, s2;
    void *tbl[3];
    struct BLightObj obj;
    s32 out_val;
    s32 len;

    /* @leaf 8005A6A4 source descriptor lookup via real 8005A654, complemented slot byte, rate*scale and optional result out */
    LK_FUNC("func_8005A6A4");

    s0.id = 10; s0.rate = 3;
    s1.id = 20; s1.rate = 7;
    s2.id = 30; s2.rate = 12;
    tbl[0] = &s0; tbl[1] = &s1; tbl[2] = &s2;
    gTextureDescriptorTable = tbl;
    gTexDescriptorCount = 3;

    /* Unknown ID returns 0 without calling spawn or touching object->slot / *result. */
    lk_fill(&obj, 0, sizeof(obj));
    obj.slot = 0x55;
    obj.scale = 9;
    out_val = 0x777;
    reset_light_spawn(0x1234);
    len = gSpawnDynamicLightEffect(99, &obj, &out_val);
    FCK_EQ(len, 0, "unknown id returns 0");
    FCK_EQ(s_light_spawn.calls, 0, "unknown id makes 0 spawn calls");
    FCK_EQ(obj.slot, 0x55, "unknown id preserves slot");
    FCK_EQ(out_val, 0x777, "unknown id preserves *result");

    /* Matched ID 20 (index 1, rate 7): slot = ~1 = 0xFE, length = 7*9 = 63, writes *result. */
    reset_light_spawn(0x4321);
    len = gSpawnDynamicLightEffect(20, &obj, &out_val);
    FCK_EQ(len, 63, "length = 7 * 9 = 63");
    FCK_EQ(obj.slot, 0xFE, "slot = (u8)~1 = 0xFE");
    FCK_EQ(s_light_spawn.calls, 1, "spawn called once");
    FCK_EQ((u32)s_light_spawn.last_obj, (u32)&obj, "passed object");
    FCK_EQ(s_light_spawn.last_len, 63, "passed length 63");
    FCK_EQ(s_light_spawn.seen_slot, 0xFE, "slot set before spawn call");
    FCK_EQ(out_val, 0x4321, "*result receives spawn return");

    /* Matched ID 10 (index 0, rate 3) with null result pointer: no crash, slot = ~0 = 0xFF. */
    obj.scale = 5;
    reset_light_spawn(0x9999);
    len = gSpawnDynamicLightEffect(10, &obj, 0);
    FCK_EQ(len, 15, "length = 3 * 5 = 15");
    FCK_EQ(obj.slot, 0xFF, "slot = (u8)~0 = 0xFF");
    FCK_EQ(s_light_spawn.calls, 1, "spawn called with null result ptr");
}

/* ============================================================================
 * 24. 800753D0 - gUpdateParticleLight
 * ============================================================================
 */
struct BPLightNode {
    u8 unk00[9];
    u8 red;
    u8 green;
    u8 blue;
};
struct BPLightObj {
    s32 unk00;
    s32 x;
    s32 y;
    s32 z;
    s16 size;
    u8 unk12[3];
    u8 red;
    u8 green;
    u8 blue;
    u8 unk18[2];
    s16 angle;
    u8 unk1C[8];
    struct BPLightNode *light;
};
extern void gUpdateParticleLight(struct BPLightObj *object) __asm__("func_800753D0");

static void test_800753D0(void)
{
    struct BPLightObj obj;
    struct BPLightNode lnode;

    /* @leaf 800753D0 source particle light RGB sync, x/z/y axis swap, size<<5 scaling and 0x500 spawn threshold */
    LK_FUNC("func_800753D0");

    /* Existing light: copies RGB and updates with (x, z, y, size<<5, size<<5, angle, 0, 0). */
    lk_fill(&obj, 0, sizeof(obj));
    lk_fill(&lnode, 0, sizeof(lnode));
    obj.x = 100; obj.y = 200; obj.z = 300;
    obj.size = 0x10; /* << 5 == 0x200 */
    obj.red = 0x12; obj.green = 0x34; obj.blue = 0x56;
    obj.angle = -45;
    obj.light = &lnode;
    reset_plight();
    gUpdateParticleLight(&obj);
    FCK_EQ(s_plight.upd_calls, 1, "existing light calls update");
    FCK_EQ(s_plight.create_calls, 0, "existing light skips create");
    FCK_EQ((u32)s_plight.upd_light, (u32)&lnode, "passed light node");
    FCK_EQ(s_plight.x, 100, "arg1 = x");
    FCK_EQ(s_plight.z, 300, "arg2 = z (swapped)");
    FCK_EQ(s_plight.y, 200, "arg3 = y (swapped)");
    FCK_EQ(s_plight.w, 0x200, "width = size << 5");
    FCK_EQ(s_plight.d, 0x200, "depth = size << 5");
    FCK_EQ(s_plight.angle, -45, "angle forwarded");
    FCK_EQ(s_plight.a7, 0, "arg7 = 0");
    FCK_EQ(s_plight.a8, 0, "arg8 = 0");
    FCK_EQ(s_plight.seen_r, 0x12, "red synced before update");
    FCK_EQ(s_plight.seen_g, 0x34, "green synced before update");
    FCK_EQ(s_plight.seen_b, 0x56, "blue synced before update");

    /* Null light at exact boundary size=0x28 (0x28 << 5 == 0x500): NOT > 0x500 -> no spawn. */
    obj.light = 0;
    obj.size = 0x28;
    reset_plight();
    gUpdateParticleLight(&obj);
    FCK_EQ(s_plight.upd_calls, 0, "null light skips update");
    FCK_EQ(s_plight.create_calls, 0, "size<<5 == 0x500 does not spawn");

    /* Null light just above boundary size=0x29 (0x29 << 5 == 0x520 > 0x500): spawns. */
    obj.size = 0x29;
    reset_plight();
    gUpdateParticleLight(&obj);
    FCK_EQ(s_plight.create_calls, 1, "size<<5 == 0x520 spawns light");
    FCK_EQ((u32)s_plight.create_obj, (u32)&obj, "spawn receives object");
}

/* ============================================================================
 * 25. 8007A3AC - gSelectCrimeWar1P (uses shared @mock sound_mock!)
 * ============================================================================
 */
extern void gSelectCrimeWar1P(s32 a0) __asm__("func_8007A3AC");

static void test_8007A3AC(void)
{
    struct BCwMode mode;

    /* @leaf 8007A3AC source Crime War 1P confirm guard, mode setup, one-shot rules movie and locked-phase SE 0x4C */
    LK_FUNC("func_8007A3AC");

    lk_fill(&mode, 0, sizeof(mode));
    gGameSession[0] = &mode;

    /* Action != 3 returns immediately without touching mode, reset callback, or sound. */
    mode.players = 2; mode.split = 1;
    gPlayer1MenuAcceptPhase = 0;
    gDeferredTransition = 99;
    reset_cw_reset();
    fcm_sound_reset();
    gSelectCrimeWar1P(2);
    FCK_EQ(mode.players, 2, "a0!=3 leaves players unchanged");
    FCK_EQ(gDeferredTransition, 99, "a0!=3 leaves transition unchanged");
    FCK_EQ(s_cw_reset.calls, 0, "a0!=3 skips reset callback");
    FCK_EQ(fcm_sound.sfx_calls, 0, "a0!=3 plays no sound");

    /* Action == 3, accept phase == 0, first time (gLayoutPointerFlagB == 0): sets 1P non-split and rules movie. */
    gSavedCrimeWarProgress = 7;
    gCrimeWarProgress = 8;
    gAudioOptionsMenuMode[0] = 0;
    gCrimeWarHubMenuInputMode[0] = 0;
    gLayoutPointerFlagB = 0;
    gPendingRowCapture = (void *)0x1234;
    gSelectCrimeWar1P(3);
    FCK_EQ(mode.players, 1, "1P players = 1");
    FCK_EQ(mode.split, 0, "1P split = 0");
    FCK_EQ(gSavedCrimeWarProgress, 0, "saved progress cleared");
    FCK_EQ(gCrimeWarProgress, 0, "progress cleared");
    FCK_EQ(gAudioOptionsMenuMode[0], 1, "audio menu mode set to 1");
    FCK_EQ(gCrimeWarHubMenuInputMode[0], 1, "hub menu input mode set to 1");
    FCK_EQ(s_cw_reset.calls, 1, "func_800776B8 called once");
    FCK_EQ(s_cw_reset.seen_players, 1, "players=1 set before reset");
    FCK_EQ(s_cw_reset.seen_split, 0, "split=0 set before reset");
    FCK_EQ(s_cw_reset.seen_saved, 0, "saved progress cleared before reset");
    FCK_EQ(s_cw_reset.seen_prog, 0, "progress cleared before reset");
    FCK_EQ(gDeferredTransition, 4, "deferred transition set to 4");
    FCK_EQ(gLayoutPointerFlagB, 1, "first-time rules flag latched to 1");
    FCK_EQ((u32)gPendingRowCapture, (u32)gCwRulesMovieName, "pending row capture points to rules movie");
    FCK_EQ(fcm_sound.sfx_calls, 0, "unlocked confirm plays no error sound");

    /* Second time (gLayoutPointerFlagB == 1): gPendingRowCapture stays 0. */
    reset_cw_reset();
    gPendingRowCapture = (void *)0x5678;
    gSelectCrimeWar1P(3);
    FCK_EQ(s_cw_reset.calls, 1, "second confirm calls reset");
    FCK_EQ((u32)gPendingRowCapture, 0U, "second confirm clears pending row capture to 0");
    FCK_EQ(gLayoutPointerFlagB, 1, "rules flag stays 1");

    /* Locked accept phase (!= 0): plays SE 0x4C with arg1=0 via shared sound_mock, skips reset. */
    reset_cw_reset();
    fcm_sound_reset();
    gPlayer1MenuAcceptPhase = 1;
    gDeferredTransition = 77;
    gSelectCrimeWar1P(3);
    FCK_EQ(s_cw_reset.calls, 0, "locked phase skips reset");
    FCK_EQ(gDeferredTransition, 77, "locked phase preserves transition");
    FCK_EQ(fcm_sound.sfx_calls, 1, "locked phase plays 1 SFX");
    FCK_EQ(fcm_sound.sfx_last_id, 0x4C, "locked phase SFX id is 0x4C");
    FCK_EQ(fcm_sound.sfx_last_arg1, 0, "locked phase SFX arg1 is 0");
}

int main(void)
{
    test_8001306C();
    test_80045494();
    test_8009398C();
    test_80093AF0();
    test_80044988();
    test_8004DA84();
    test_8004DC64();
    test_8006F170();
    test_80029520();
    test_80069B58();
    test_80031990();
    test_8003FD74();
    test_80073500();
    test_80073590();
    test_800834C4();
    test_8005A220();
    test_800860CC();
    test_80086164();
    test_800427E8();
    test_80071A68();
    test_80045678();
    test_800457B8();
    test_8005A6A4();
    test_800753D0();
    test_8007A3AC();
    return lk_done("leaf_hw_free_b");
}
