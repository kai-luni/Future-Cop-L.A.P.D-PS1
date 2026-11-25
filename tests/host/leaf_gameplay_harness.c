/* leaf_gameplay: hardware-free gameplay, HUD and front-end units
 * (player resupply, chase approach speed, light-object constructor, animation
 * script commands, HUD gouraud bar, marker colour table, path-node loader,
 * sprite descriptor table, attack commands, dual-view object grid, status
 * slot icon, particle light, options preview stop, password unlocks, atlas
 * sprite).
 *
 * Fixture style
 *  - Every D_ label a unit touches is defined here under its retail label and
 *    written by the test before each call; nothing relies on zero-init, BSS
 *    or retail memory. Labels shared by several units (D_8009FA94 session
 *    pointer, D_8009FAA4 frame clock, D_8009F9F0/D_8009FA28 primitive pool,
 *    D_800A8F90 command list head) are defined once with the widest layout.
 *  - Objects are plain byte buffers filled with a canary and read back
 *    through little-endian offset accessors, so the expected layout comes
 *    from the offsets written in each test (taken from the unit's documented
 *    field map), not from re-using the unit's struct declarations.
 *  - Retail tables (password digit alphabets, effect key table, atlas table,
 *    sprite headers) are replaced by small synthetic fixtures.
 *  - Callees outside the suite are bound by their retail label and record
 *    what they receive; none of them is provided by tests/host/mocks/.
 *    func_8002A5B4 (script command removal) is a stand-in that shifts the
 *    4-entry command list down by one, which is the contract both script
 *    units rely on to terminate.
 *  - Expectations are literals worked by hand from the cited evidence; no
 *    expectation is computed by a re-implementation of a unit.
 *
 * Mutation notes (mutate --per-unit 8): the remaining survivors are
 * equivalent mutants: 80037B28 `s0 > 0` -> `>= 0` (a zero dot product
 * scales to 0 on either branch), 800511E4 `count > 0` -> `>= 0` (the loop
 * condition re-tests count), 8007EA78 `i < 10` -> `<= 10` (an 11th decoded
 * digit is never read), and `return 0` rewrites of statements that already
 * return 0 (80040E60, 8005A2BC). Script and grid units are guarded by
 * fixture bytes/cells past the documented bounds, so an off-by-one read
 * changes an observable result.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* ========================================================================
 * Byte-buffer accessors (little-endian, as on the R3000)
 * ======================================================================== */

static u8 rd8(const void *p, int off) { return ((const u8 *)p)[off]; }
static u16 rd16(const void *p, int off)
{
    const u8 *b = (const u8 *)p + off;
    return (u16)(b[0] | (b[1] << 8));
}
static s16 rs16(const void *p, int off) { return (s16)rd16(p, off); }
static u32 rd32(const void *p, int off)
{
    const u8 *b = (const u8 *)p + off;
    return (u32)b[0] | ((u32)b[1] << 8) | ((u32)b[2] << 16) | ((u32)b[3] << 24);
}
static void wr8(void *p, int off, u32 v) { ((u8 *)p)[off] = (u8)v; }
static void wr16(void *p, int off, u32 v)
{
    u8 *b = (u8 *)p + off;
    b[0] = (u8)v; b[1] = (u8)(v >> 8);
}
static void wr32(void *p, int off, u32 v)
{
    u8 *b = (u8 *)p + off;
    b[0] = (u8)v; b[1] = (u8)(v >> 8); b[2] = (u8)(v >> 16); b[3] = (u8)(v >> 24);
}
static void wrp(void *p, int off, const void *q) { wr32(p, off, (u32)q); }

/* ========================================================================
 * Globals (retail D_ labels; set per test)
 * ======================================================================== */

/* shared */
void *gSessionPtr __asm__("D_8009FA94");       /* -> session block */
u32 gFrameClock __asm__("D_8009FAA4");         /* u8 reader sees the low byte */
u8 *gPrimNext __asm__("D_8009F9F0");
u8 *gPrimEnd __asm__("D_8009FA28");
static u8 session[0x100];
static u8 prim_pool[0x34 * 4];

/* 8002FC90 resupply */
s32 gFrameDelta __asm__("D_8009F9FC");
s32 gPlayerCount __asm__("D_8009FA0C");
void *gPlayerEntities[2] __asm__("D_8009FA58");

/* 80040E60 light object */
s32 gLightObjectCount __asm__("D_8009F318");
u8 gLightHandler[4] __asm__("D_80040874");


/* 8004CAE4 marker colours and pools; D_800A8F90 is also the 80062874 object
 * list (count, pad, items[]) */
s32 gEntityIdCounter __asm__("D_8009F7A0");
s32 gProximityScriptDeadline __asm__("D_8009F7A4");
s32 gPendingMessageMask __asm__("D_8009F7A8");
u8 gMessageDoneFlag __asm__("D_8009F7AC");
s32 gRequestOffsetBias[2] __asm__("D_8009F9F4");
s32 gUnitListHead __asm__("D_8009FA40");
s32 gListTailBranch __asm__("D_8009FA88");
s32 gMarkerColorTable[34] __asm__("D_800A6F60");
u32 gCommandList[2 + 16] __asm__("D_800A8F90");

/* 800511E4 path nodes */
s32 gPlacementSlotCount __asm__("D_8009F7B8");
void *gPathNetTables[4] __asm__("D_800A2210");

/* 8005FFB8 effect key table (synthetic, 0x30-byte entries) */
u8 gEffectKeyTable[4 * 0x30] __asm__("D_800A2300");

/* 80062874 dual-view grid */
s32 gViewAnchors[4] __asm__("D_800A3190");          /* {x, z} x 2 */
s32 gCellClipRect[4] __asm__("D_800A2970");         /* x, z, w, h */
u32 gCellNodes[2 * 8] __asm__("D_800A2990");        /* {obj, next} */
/* 5x5 cells (bits at +0) plus 11 guard cells the unit must never read */
u32 gCoverage2[4 * 36] __asm__("D_800AB548");
u32 gCoverage1[4 * 36] __asm__("D_800AB3B8");
s32 gCount2 __asm__("D_8009F82C");
s32 gViewCount __asm__("D_8009F814");
s32 gCount1 __asm__("D_8009F828");
void **gList2 __asm__("D_8009F81C");
void **gList1 __asm__("D_8009F818");
u8 *gCellMap __asm__("D_8009F820");
u8 gOrigin2X __asm__("D_8009F836");
u8 gOrigin2Y __asm__("D_8009F837");
u8 gOrigin1Y __asm__("D_8009F835");
u8 gOrigin1X __asm__("D_8009F834");

/* 8006BF44 status icon packets */
u8 *gPacketCursor __asm__("D_8009FA90");
u8 *gPacketLimit __asm__("D_8009F9C0");
u32 gEnvMapTexpageWord[2] __asm__("D_800A8EA4");

/* 8007D150 options preview */
s32 gSnapshotPointer __asm__("D_8009F91C");

/* 8007EA78 password unlocks (synthetic alphabets and level table) */
u8 gPasswordBuffer[12] __asm__("D_800A3948");
u8 gOctalDigitTable[8] __asm__("D_8009C1BC");
u8 gHexDigitTable[16] __asm__("D_8009C1AC");
u8 gWideTextTable[16 * 0x24] __asm__("D_8009B4E4");
s8 gFrontEndListTable[8] __asm__("D_8009B72C");
u8 gFrontEndListFlag __asm__("D_8009F3C0");
u8 gPlayerWrapLimit __asm__("D_8009F3C1");
u8 gPausedMenuFlag __asm__("D_8009F3C3");
u8 gLockedMenuFlag __asm__("D_8009F3C4");
u8 gSavedCrimeWarProgress __asm__("D_8009F3F1");
u8 gCrimeWarProgress __asm__("D_8009F458");
u8 gSelectedMissionIndex __asm__("D_8009F880");
u8 gMissionListMinIndex __asm__("D_8009F881");
u8 gMissionListMaxIndex __asm__("D_8009F882");

/* 80082B0C atlas table */
void *gAtlasTable __asm__("D_8009F45C");

/* ========================================================================
 * Stand-ins (retail labels, record arguments)
 * ======================================================================== */

/* generic call log: id + up to 6 words */
typedef struct { s32 id; s32 a[6]; } Call;
static Call calls[16];
static s32 ncalls;
static void log_call(s32 id, s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5)
{
    if (ncalls < 16) {
        Call *c = &calls[ncalls];
        c->id = id;
        c->a[0] = a0; c->a[1] = a1; c->a[2] = a2;
        c->a[3] = a3; c->a[4] = a4; c->a[5] = a5;
    }
    ncalls++;
}
static void reset_calls(void)
{
    lk_fill(calls, 0, sizeof calls);
    ncalls = 0;
}

/* 8002FC90 callees */
enum { C_GAUGE = 1, C_QUART, C_AMMO, C_RESERVE };
void st_gauge(void *sub, s32 index, s32 value) __asm__("func_8005A190");
void st_gauge(void *sub, s32 index, s32 value) { log_call(C_GAUGE, (s32)sub, index, value, 0, 0, 0); }
void st_quartile(void *obj, s32 value) __asm__("func_8003BA50");
void st_quartile(void *obj, s32 value) { log_call(C_QUART, (s32)obj, value, 0, 0, 0, 0); }
void st_ammo(void *obj, s32 j, s32 v, s32 f) __asm__("func_8006BE90");
void st_ammo(void *obj, s32 j, s32 v, s32 f) { log_call(C_AMMO, (s32)obj, j, v, f, 0, 0); }
void st_reserve(void *obj, s32 j) __asm__("func_8006BDFC");
void st_reserve(void *obj, s32 j) { log_call(C_RESERVE, (s32)obj, j, 0, 0, 0, 0); }

/* 80037B28 callees: return scripted dot product / length */
static s32 dot_ret, len_ret, dot_calls, len_calls;
static s32 dot_b[3], len_v[3];
static void *dot_a;
s32 st_dot(s32 *a, s32 *b) __asm__("func_8001AB00");
s32 st_dot(s32 *a, s32 *b)
{
    dot_calls++;
    dot_a = a;
    dot_b[0] = b[0]; dot_b[1] = b[1]; dot_b[2] = b[2];
    return dot_ret;
}
s32 st_len(s32 *v) __asm__("func_8001A100");
s32 st_len(s32 *v)
{
    len_calls++;
    len_v[0] = v[0]; len_v[1] = v[1]; len_v[2] = v[2];
    return len_ret;
}

/* 80040E60 callees */
static u8 alloc_buf[0x200];
static s32 alloc_calls, alloc_args[2];
static void *alloc_ret;
void *st_alloc(s32 size, s32 tag) __asm__("func_80017EBC");
void *st_alloc(s32 size, s32 tag)
{
    alloc_calls++;
    alloc_args[0] = size; alloc_args[1] = tag;
    return alloc_ret;
}
static s32 motion_calls;
static void *motion_dest, *motion_src;
void st_motion(void *dest, void *src) __asm__("func_8003C14C");
void st_motion(void *dest, void *src)
{
    motion_calls++;
    motion_dest = dest; motion_src = src;
}
static s32 rot_calls, rot_args[3], rot_out[3];
void st_rotation(s32 a, s32 b, s32 c, u8 *work) __asm__("func_800188D0");
void st_rotation(s32 a, s32 b, s32 c, u8 *work)
{
    rot_calls++;
    rot_args[0] = a; rot_args[1] = b; rot_args[2] = c;
    wr32(work, 0x18, rot_out[0]);
    wr32(work, 0x1C, rot_out[1]);
    wr32(work, 0x20, rot_out[2]);
}
static s32 steps_calls, steps_count_seen, steps_0a_seen;
static void *steps_arg;
void st_color_steps(void *obj) __asm__("func_800403D0");
void st_color_steps(void *obj)
{
    steps_calls++;
    steps_arg = obj;
    steps_count_seen = gLightObjectCount;
    steps_0a_seen = rd16(obj, 0x0A);
}

/* 800449F8 / 8005FFB8 callees */
enum { C_SEL = 30, C_RM, C_FIND };
static s32 sel_calls, sel_args[4];
static void *sel_track;
void *st_select_clip(void *track, s32 index, s32 dir, u8 flags) __asm__("func_800448D8");
void *st_select_clip(void *track, s32 index, s32 dir, u8 flags)
{
    sel_calls++;
    sel_track = track;
    sel_args[0] = index; sel_args[1] = dir; sel_args[2] = flags;
    log_call(C_SEL, (s32)track, index, dir, flags, 0, 0);
    return 0;
}
static s32 rm_calls, rm_idx[8];
static void *rm_script[8];
/* Removes command i from the 4-entry (op,arg) list at +0x0C by shifting the
 * following entries down and clearing only slot 3's opcode byte at +0x12,
 * matching src/game/8002A5B4.c:58 (the trailing arg byte at +0x13 stays). */
void st_remove_cmd(u8 *script, s32 i) __asm__("func_8002A5B4");
void st_remove_cmd(u8 *script, s32 i)
{
    s32 k;
    if (rm_calls < 8) {
        rm_script[rm_calls] = script;
        rm_idx[rm_calls] = i;
    }
    rm_calls++;
    log_call(C_RM, (s32)script, i, 0, 0, 0, 0);
    for (k = i; k < 3; k++) {
        script[0xC + 2 * k] = script[0xC + 2 * k + 2];
        script[0xD + 2 * k] = script[0xD + 2 * k + 2];
    }
    script[0xC + 6] = 0;
}

/* 800511E4 callees */
static s32 res_status, res_reuse_calls, res_ack_calls;
static void *res_reuse_arg, *res_ack_arg;
s32 st_res_status(void *r) __asm__("func_80019AA8");
s32 st_res_status(void *r) { (void)r; return res_status; }
void st_res_ack(void *r) __asm__("func_80019A74");
void st_res_ack(void *r) { res_ack_calls++; res_ack_arg = r; }
void st_res_reuse(void *r) __asm__("func_80019D00");
void st_res_reuse(void *r) { res_reuse_calls++; res_reuse_arg = r; }
static s32 surf_calls, surf_x[8], surf_z[8], surf_mode[8], surf_ret[8];
s32 st_surface(s32 *pos, s32 mode) __asm__("func_80067684");
s32 st_surface(s32 *pos, s32 mode)
{
    s32 k = surf_calls++ & 7;
    surf_x[k] = pos[0]; surf_z[k] = pos[2]; surf_mode[k] = mode;
    return surf_ret[k];
}

/* 8005A2BC callees */
static s32 ctz_calls, ctz_args[4];
s32 st_ctz(u32 v) __asm__("func_800195F0");
s32 st_ctz(u32 v)
{
    if (ctz_calls < 4) ctz_args[ctz_calls] = v;
    ctz_calls++;
    return (s32)(v + 0x10);            /* marker value, not a real ctz */
}
static s32 desc_calls;
static void *desc_arg;
void st_set_desc(void *t) __asm__("func_8004979C");
void st_set_desc(void *t) { desc_calls++; desc_arg = t; }

/* 8005FFB8 callee: key lookup scripted per key */
static s32 find_calls, find_arg;
s32 st_find_key(s32 key) __asm__("func_8005FF70");
s32 st_find_key(s32 key)
{
    find_calls++;
    find_arg = key;
    log_call(C_FIND, key, 0, 0, 0, 0, 0);
    return key == 0x42 ? 2 : key == 0x10 ? 0 : -1;
}

/* 800752E0 callees */
static u32 rand_ret;
u32 st_rand(void) __asm__("func_80019668");
u32 st_rand(void) { return rand_ret; }
static s32 light_calls, light_id;
static u8 light_req[0x30];
static void *light_result_ptr, *light_ret;
s32 st_spawn_light(s32 id, u8 *req, void **result) __asm__("func_8005A6A4");
s32 st_spawn_light(s32 id, u8 *req, void **result)
{
    s32 i;
    light_calls++;
    light_id = id;
    for (i = 0; i < 0x30; i++) light_req[i] = req[i];
    light_result_ptr = result;
    *result = light_ret;
    return 0;
}

/* 8007D150 callee */
static s32 stop_calls, stop_arg, stop_seen_ptr;
void st_stop_voice(s32 h) __asm__("func_80046A34");
void st_stop_voice(s32 h)
{
    stop_calls++;
    stop_arg = h;
    stop_seen_ptr = gSnapshotPointer;
}

/* 8007EA78 callees: log order */
enum { C_PW_REFRESH = 20, C_PW_PA, C_PW_CW, C_PW_UNLOCK };
static s32 pw_cw_seen;
void func_80077FFC(void) { log_call(C_PW_REFRESH, 0, 0, 0, 0, 0, 0); }
void st_pa_list(void) __asm__("func_80077708");
void st_pa_list(void) { log_call(C_PW_PA, 0, 0, 0, 0, 0, 0); }
void st_cw_list(void) __asm__("func_800776B8");
void st_cw_list(void) { pw_cw_seen = gCrimeWarProgress; log_call(C_PW_CW, 0, 0, 0, 0, 0, 0); }
void st_unlocks(void) __asm__("func_8007A168");
void st_unlocks(void) { log_call(C_PW_UNLOCK, gMissionListMaxIndex, 0, 0, 0, 0, 0); }

/* 80047BE8 callee */
enum { C_TILE = 10 };
void st_tile(u32 color, s32 x, s32 y, s32 w, s32 h, u8 blend) __asm__("func_8001C214");
void st_tile(u32 color, s32 x, s32 y, s32 w, s32 h, u8 blend)
{
    log_call(C_TILE, (s32)color, x, y, w, h, blend);
}

/* ========================================================================
 * Units under test
 * ======================================================================== */

void gApplyPlayerResupply(u8 *obj, u32 bits, s32 amount, s32 *acc, s32 *period) __asm__("func_8002FC90");
void gUpdateChaseApproachSpeed(s32 msg, u8 *self, s32 unused) __asm__("func_80037B28");
void *gCreateLightObject(u8 *src) __asm__("func_80040E60");
void gProcessAnimScriptCommands(u8 *script, u8 *track) __asm__("func_800449F8");
void gEmitHudGouraudBar(s32 x, s32 y, s32 z, s32 p3, s32 p4, s32 p5, s32 flag) __asm__("func_80047BE8");
void gInitMarkerColorsAndPools(void) __asm__("func_8004CAE4");
void gLoadPathNodePositions(u8 *resource) __asm__("func_800511E4");
s32 gBuildSpriteDescTable(void **array, s32 count) __asm__("func_8005A2BC");
void gApplyAttackCommands(u8 *obj, u8 *script) __asm__("func_8005FFB8");
void gBuildDualViewObjectListsAndGrid(void) __asm__("func_80062874");
void gEmitStatusSlotIcon(s32 a0, s32 a1, s32 a2, s32 a3) __asm__("func_8006BF44");
void gSpawnParticleLight(u8 *object) __asm__("func_800752E0");
void gStopOptionsPreview(s32 arg0) __asm__("func_8007D150");
s32 gApplyPasswordUnlocks(void) __asm__("func_8007EA78");
void *gDrawAtlasSprite(s32 idx, s32 x, s32 y, s32 w, s32 h, s32 tag, s32 r, s32 g, s32 b, s8 fx, s8 fy) __asm__("func_80082B0C");

/* ======================================================================== */

/* 8002FC90 fixture: player entity +0x16 type, +0x1C/+0x1E health/max,
 * +0x20 gauge-active byte, +0x24 flags, +0xAC HUD sub-block (+0x324/+0x326
 * timers), +0xB4 proxy target. */
static u8 pl0[0xC0], pl1[0xC0], proxy[0xC0];
static u8 sub0[0x330], sub1[0x330];

static void resupply_reset(void)
{
    lk_fill(pl0, 0xA5, sizeof pl0);
    lk_fill(pl1, 0xA5, sizeof pl1);
    lk_fill(proxy, 0xA5, sizeof proxy);
    lk_fill(sub0, 0xA5, sizeof sub0);
    lk_fill(sub1, 0xA5, sizeof sub1);
    wr8(pl0, 0x16, 1); wr8(pl1, 0x16, 1);
    wrp(pl0, 0xAC, sub0); wrp(pl1, 0xAC, sub1);
    wr8(pl0, 0x20, 0); wr8(pl1, 0x20, 0);
    wr32(pl0, 0x24, 0); wr32(pl1, 0x24, 0);
    gPlayerEntities[0] = pl0;
    gPlayerEntities[1] = pl1;
    gPlayerCount = 2;
    gSessionPtr = session;
    lk_fill(session, 0, sizeof session);
    gFrameDelta = 0;
    reset_calls();
}

static void test_resupply(void)
{
    s32 acc, period;

    /* @leaf 8002FC90 source src/game/8002FC90.c: interval accumulator (*acc += delta, early out below period, quotient/remainder), 0x12 proxy via +0xB4, co-op 0x20000 health min(cur+2q or cur+amount, max) with negative amount = max, quartile hook gated by +0x20/+0x24 bit 0x40000, self-only timers 0x34BC/0x1770, ammo bits 0-3 and reserve bits 4-7 */
    LK_FUNC("func_8002FC90");

    /* below the period: accumulator grows, nothing else */
    resupply_reset();
    acc = 50; period = 100; gFrameDelta = 30;
    gApplyPlayerResupply(pl0, 0x000F00FF, 0, &acc, &period);
    FCK_EQ(acc, 80, "interval: accumulator += frame delta");
    FCK_EQ(ncalls, 0, "interval: below period, no callee");
    FCK_EQ(rs16(sub0, 0x324), (s16)0xA5A5, "interval: timers untouched");

    /* exactly the period: one tick, remainder 0 */
    resupply_reset();
    gPlayerCount = 1;
    acc = 40; period = 100; gFrameDelta = 60;
    wr16(pl0, 0x1C, 10); wr16(pl0, 0x1E, 99);
    gApplyPlayerResupply(pl0, 0x20000, 0, &acc, &period);
    FCK_EQ(acc, 0, "acc == period: remainder 0");
    FCK_EQ(rs16(pl0, 0x1C), 12, "acc == period: quotient 1 -> +2");

    /* 80 + 250 = 330 over period 100: quotient 3, remainder 30 */
    resupply_reset();
    acc = 80; period = 100; gFrameDelta = 250;
    wr16(pl0, 0x1C, 100); wr16(pl0, 0x1E, 200); wr8(pl0, 0x20, 1);
    wr16(pl1, 0x1C, 199); wr16(pl1, 0x1E, 200);
    gApplyPlayerResupply(pl0, 0x20000 | 0x40000 | 0x80000 | 0x05 | 0x30, 0, &acc, &period);
    FCK_EQ(acc, 30, "tick: remainder 330 % 100");
    FCK_EQ(rs16(pl0, 0x1C), 106, "tick: self health 100 + 2*3");
    FCK_EQ(rs16(pl1, 0x1C), 200, "tick: partner health capped at max 200");
    FCK_EQ(ncalls, 7, "tick: gauge, quartile, 2 ammo, 2 reserve, partner gauge");
    FCK_EQ(calls[0].id, C_GAUGE, "tick: self gauge first");
    FCK_EQ(calls[0].a[0], (s32)sub0, "tick: gauge on self HUD block");
    FCK_EQ(calls[0].a[1], 0, "tick: gauge index 0");
    FCK_EQ(calls[0].a[2], 106, "tick: gauge value = new health");
    FCK_EQ(calls[1].id, C_QUART, "tick: quartile hook (gauge byte set, flag clear)");
    FCK_EQ(calls[1].a[0], (s32)pl0, "tick: quartile on self");
    FCK_EQ(calls[1].a[1], 100, "tick: quartile gets the old health");
    FCK_EQ(calls[2].id, C_AMMO, "tick: ammo bit 0");
    FCK_EQ(calls[2].a[1], 0, "tick: ammo slot 0");
    FCK_EQ(calls[2].a[2], 3, "tick: ammo amount = quotient");
    FCK_EQ(calls[2].a[3], 1, "tick: ammo flag 1");
    FCK_EQ(calls[3].a[1], 2, "tick: ammo bit 2 -> slot 2");
    FCK_EQ(calls[4].id, C_RESERVE, "tick: reserve bit 4");
    FCK_EQ(calls[4].a[1], 0, "tick: reserve slot 0");
    FCK_EQ(calls[5].a[1], 1, "tick: reserve bit 5 -> slot 1");
    FCK_EQ(calls[6].id, C_GAUGE, "tick: partner gauge");
    FCK_EQ(calls[6].a[0], (s32)sub1, "tick: partner HUD block");
    FCK_EQ(calls[6].a[2], 200, "tick: partner gauge value");
    FCK_EQ(rs16(sub0, 0x324), 0x34BC, "tick: 0x40000 timer 0x34BC on self");
    FCK_EQ(rs16(sub0, 0x326), 0x1770, "tick: 0x80000 timer 0x1770 on self");
    FCK_EQ(rs16(sub1, 0x324), (s16)0xA5A5, "tick: partner timer untouched");
    FCK_EQ(rs16(sub1, 0x326), (s16)0xA5A5, "tick: partner timer 2 untouched");

    /* instant: amount 30 added, capped; *acc untouched; 1 player only */
    resupply_reset();
    acc = 777; period = 1;
    gPlayerCount = 1;
    wr16(pl0, 0x1C, 50); wr16(pl0, 0x1E, 70);
    wr16(pl1, 0x1C, 10); wr16(pl1, 0x1E, 70);
    gApplyPlayerResupply(pl0, 0x1000000 | 0x20000 | 0x02, 30, &acc, &period);
    FCK_EQ(acc, 777, "instant: accumulator not touched");
    FCK_EQ(rs16(pl0, 0x1C), 70, "instant: 50 + 30 capped at 70");
    FCK_EQ(rs16(pl1, 0x1C), 10, "instant: single player, partner skipped");
    FCK_EQ(ncalls, 2, "instant: gauge + one ammo");
    FCK_EQ(calls[1].id, C_AMMO, "instant: ammo");
    FCK_EQ(calls[1].a[1], 1, "instant: bit 1 -> slot 1");
    FCK_EQ(calls[1].a[2], 30, "instant: ammo amount = direct amount");
    FCK_EQ(rs16(sub0, 0x324), (s16)0xA5A5, "instant: no timer bit, untouched");

    /* instant below max, and negative amount jumps to max */
    resupply_reset();
    gPlayerCount = 1;
    wr16(pl0, 0x1C, 50); wr16(pl0, 0x1E, 70);
    gApplyPlayerResupply(pl0, 0x1000000 | 0x20000, 5, &acc, &period);
    FCK_EQ(rs16(pl0, 0x1C), 55, "instant: 50 + 5");
    gApplyPlayerResupply(pl0, 0x1000000 | 0x20000, 0, &acc, &period);
    FCK_EQ(rs16(pl0, 0x1C), 55, "instant: amount 0 adds nothing (not max)");
    gApplyPlayerResupply(pl0, 0x1000000 | 0x20000, -1, &acc, &period);
    FCK_EQ(rs16(pl0, 0x1C), 70, "instant: negative amount = max");

    /* quartile hook suppressed by +0x24 bit 0x40000 */
    resupply_reset();
    gPlayerCount = 1;
    wr16(pl0, 0x1C, 1); wr16(pl0, 0x1E, 9); wr8(pl0, 0x20, 1);
    wr32(pl0, 0x24, 0x40000);
    gApplyPlayerResupply(pl0, 0x1000000 | 0x20000, 1, &acc, &period);
    FCK_EQ(ncalls, 1, "flag 0x40000: gauge only, no quartile");
    FCK_EQ(rs16(pl0, 0x1C), 2, "flag 0x40000: health still applied");

    /* co-op sharing blocked by session +0x5C */
    resupply_reset();
    wr32(session, 0x5C, 1);
    wr16(pl0, 0x1C, 1); wr16(pl0, 0x1E, 9);
    wr16(pl1, 0x1C, 1); wr16(pl1, 0x1E, 9);
    gApplyPlayerResupply(pl0, 0x1000000 | 0x20000, 4, &acc, &period);
    FCK_EQ(rs16(pl0, 0x1C), 5, "session +0x5C: self healed");
    FCK_EQ(rs16(pl1, 0x1C), 1, "session +0x5C: partner not shared");

    /* proxy type 0x12: target is +0xB4 (player 1); partner pl0 shares health */
    resupply_reset();
    wr8(proxy, 0x16, 0x12);
    wrp(proxy, 0xB4, pl1);
    wr16(pl0, 0x1C, 3); wr16(pl0, 0x1E, 9);
    wr16(pl1, 0x1C, 4); wr16(pl1, 0x1E, 9);
    gApplyPlayerResupply(proxy, 0x1000000 | 0x20000 | 0x40000 | 0x10, 2, &acc, &period);
    FCK_EQ(rs16(pl0, 0x1C), 5, "proxy: co-op partner shares health");
    FCK_EQ(rs16(pl1, 0x1C), 6, "proxy: resolved target healed");
    FCK_EQ(rs16(sub1, 0x324), 0x34BC, "proxy: timer on the resolved target");
    FCK_EQ(rs16(sub0, 0x324), (s16)0xA5A5, "proxy: partner timer untouched");
    FCK_EQ(calls[ncalls - 1].id, C_RESERVE, "proxy: reserve refill last");
    FCK_EQ(calls[ncalls - 1].a[0], (s32)pl1, "proxy: reserve on resolved target");
    FCK_EQ(rd16(proxy, 0x1C), 0xA5A5, "proxy: proxy object itself untouched");

    /* proxy with null +0xB4: returns after the accumulator update */
    resupply_reset();
    wr8(proxy, 0x16, 0x12);
    wrp(proxy, 0xB4, 0);
    acc = 0; period = 10; gFrameDelta = 25;
    gApplyPlayerResupply(proxy, 0x20000 | 0x01, 0, &acc, &period);
    FCK_EQ(acc, 5, "null proxy: accumulator consumed (25 % 10)");
    FCK_EQ(ncalls, 0, "null proxy: no callee");

    /* null table entry is skipped */
    resupply_reset();
    gPlayerEntities[0] = 0;
    wr16(pl1, 0x1C, 1); wr16(pl1, 0x1E, 9);
    gApplyPlayerResupply(pl1, 0x1000000 | 0x20000, 1, &acc, &period);
    FCK_EQ(ncalls, 1, "null slot 0: only slot 1 processed");
    FCK_EQ(rs16(pl1, 0x1C), 2, "null slot 0: self healed");
}

/* 80037B28 fixture: entity +0x16 type, +0x24 active word, +0x5C/+0x60/+0x64
 * position, +0x6C height bias, +0x78 heading, +0x84 speed, +0x88 target,
 * +0x90 gate; motion block at +0xDC (+0x00 id, +0x1C, +0x20, +0x30 base
 * speed, +0x32 target speed, +0x3C flags, +0x40 hold, +0x44/+0x46/+0x48). */
static u8 ch_self[0x130], ch_tgt[0x130];
#define MB 0xDC

static void chase_reset(void)
{
    lk_fill(ch_self, 0, sizeof ch_self);
    lk_fill(ch_tgt, 0, sizeof ch_tgt);
    wrp(ch_self, 0x88, ch_tgt);
    wr16(ch_self, MB + 0x32, 0x7777);
    wr16(ch_self, MB + 0x40, 0x5555);
    wr16(ch_tgt, MB + 0x40, 0x0009);
    wr16(ch_self, MB + 0x30, 0x0400);
    wr32(ch_self, 0x84, 0x1000);
    dot_ret = 0; len_ret = 0; dot_calls = 0; len_calls = 0;
}

static void test_chase(void)
{
    /* @leaf 80037B28 source src/game/80037B28.c: +0x90/flags 0x48/flags>>30==1 gates, |dy|-(bias<<4) < 0x1401 band, cosine = dot*(0x80000000/(len>>1))>>16 (len<16 -> 0x10000), 0xF600/0x8000 heading gates, target +0x24<0 link-match branches, otherwise speed (spd>>1)+(base>>2) or close-range base>>2 (type 26) / 0, hold 300 */
    LK_FUNC("func_80037B28");

    /* gates before any callee */
    chase_reset();
    wr16(ch_self, 0x90, 0x4000);
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(dot_calls, 0, "gate +0x90 >= 0x4000: no dot product");
    FCK_EQ(rd16(ch_self, MB + 0x40), 0x5555, "gate +0x90: hold untouched");
    chase_reset();
    wr32(ch_self, MB + 0x3C, 0x08);
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(dot_calls, 0, "gate flags & 0x08");
    chase_reset();
    wr32(ch_self, MB + 0x3C, 0x40000000);
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(dot_calls, 0, "gate flags >> 30 == 1");

    /* vertical band: |dy| 0x1401 fails, 0x1500 - (0x10 << 4) = 0x1400 passes */
    chase_reset();
    wr32(ch_tgt, 0x60, 0x1401);
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(dot_calls, 0, "dy 0x1401: outside the band");
    chase_reset();
    wr32(ch_self, 0x60, 0x1500);            /* dy = -0x1500 */
    wr16(ch_tgt, 0x6C, 0x10);
    wr32(ch_self, MB + 0x3C, 0x80000000u);  /* flags >> 30 == 2 does not gate */
    wr32(ch_self, 0x5C, 100); wr32(ch_tgt, 0x5C, 400);
    wr32(ch_self, 0x64, -50); wr32(ch_tgt, 0x64, -80);
    dot_ret = 0x1000; len_ret = 0x1000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(dot_calls, 1, "dy -0x1500 with bias 0x100: inside the band");
    FCK_CHECK(dot_a == ch_self + 0x78, "dot uses the heading at +0x78");
    FCK_EQ(dot_b[0], 300, "diff x = target - self");
    FCK_EQ(dot_b[1], -0x1500, "diff y keeps its sign");
    FCK_EQ(dot_b[2], -30, "diff z");
    FCK_EQ(len_v[0], 300, "length of the same diff");
    /* cos = 0x1000 * (0x80000000/0x800) >> 16 = 0x10000, len 0x1000 < 0x4000,
     * type != 26 -> speed 0 */
    FCK_EQ(rs16(ch_self, MB + 0x32), 0, "close and ahead, type != 26: speed 0");
    FCK_EQ(rs16(ch_self, MB + 0x40), 300, "hold 300");

    /* same, type 26: base >> 2 */
    chase_reset();
    wr8(ch_self, 0x16, 26);
    dot_ret = 0x1000; len_ret = 0x1000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rs16(ch_self, MB + 0x32), 0x100, "close, type 26: 0x400 >> 2");

    /* far: len 0x8000, dot 0x8000 -> cos 0x10000 but distance >= 0x4000 */
    chase_reset();
    dot_ret = 0x8000; len_ret = 0x8000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rs16(ch_self, MB + 0x32), 0x900, "far: (0x1000 >> 1) + (0x400 >> 2)");
    FCK_EQ(rs16(ch_self, MB + 0x40), 300, "far: hold 300");

    /* 60 degrees off (cos 0x8000) returns */
    chase_reset();
    dot_ret = 0x800; len_ret = 0x1000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rd16(ch_self, MB + 0x40), 0x5555, "cos 0x8000: no change");
    /* cos 0xC000 with +0x90 = 0x2000 returns */
    chase_reset();
    wr16(ch_self, 0x90, 0x2000);
    dot_ret = 0xC00; len_ret = 0x1000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rd16(ch_self, MB + 0x40), 0x5555, "cos 0xC000, gate 0x2000: no change");
    /* cos 0xC000 with +0x90 = 0x1FFF: close but not ahead -> far formula */
    chase_reset();
    wr16(ch_self, 0x90, 0x1FFF);
    dot_ret = 0xC00; len_ret = 0x1000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rs16(ch_self, MB + 0x32), 0x900, "cos 0xC000: approach speed");
    /* cos exactly 0xF600 is still gated by +0x90 */
    chase_reset();
    wr16(ch_self, 0x90, 0x2000);
    dot_ret = 0xF60; len_ret = 0x1000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rd16(ch_self, MB + 0x40), 0x5555, "cos 0xF600, gate 0x2000: no change");
    wr16(ch_self, 0x90, 0x1FFF);
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rs16(ch_self, MB + 0x32), 0, "cos 0xF600 > 0xF5FF, close: speed 0");
    /* negative dot: cos -0x10000 returns */
    chase_reset();
    dot_ret = -0x1000; len_ret = 0x1000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rd16(ch_self, MB + 0x40), 0x5555, "behind: no change");
    /* len < 16 counts as dead ahead regardless of the dot product */
    chase_reset();
    dot_ret = -5; len_ret = 15;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rs16(ch_self, MB + 0x32), 0, "len 15: dead ahead, close");
    FCK_EQ(rs16(ch_self, MB + 0x40), 300, "len 15: hold");

    /* active target (+0x24 < 0): different block ids -> latch */
    chase_reset();
    wr32(ch_tgt, 0x24, 0x80000000u);
    wr32(ch_tgt, MB + 0x00, 1); wr32(ch_self, MB + 0x00, 2);
    wr32(ch_tgt, 0x84, 0x400);              /* >> 1 = 0x200, capped by base */
    wr16(ch_self, MB + 0x30, 0x100);
    dot_ret = 0x1000; len_ret = 0x1000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rs16(ch_self, MB + 0x32), 0x100, "latch: target speed 0x200 capped at base 0x100");
    FCK_EQ(rs16(ch_self, MB + 0x40), 300, "latch: hold 300");
    FCK_EQ(rs16(ch_tgt, MB + 0x40), 1, "latch: positive target hold forced to 1");
    wr16(ch_self, MB + 0x30, 0x300);
    wr16(ch_tgt, MB + 0x40, 0);
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rs16(ch_self, MB + 0x32), 0x200, "latch: 0x200 under base 0x300");
    FCK_EQ(rs16(ch_tgt, MB + 0x40), 0, "latch: zero target hold stays");

    /* same id, same +0x44: compare (+0x20 << 8) - +0x1C */
    chase_reset();
    wr32(ch_tgt, 0x24, -1);
    wr32(ch_tgt, 0x84, 0x200);
    wr16(ch_tgt, MB + 0x20, 1); wr32(ch_tgt, MB + 0x1C, 0x10);   /* 0xF0 */
    wr16(ch_self, MB + 0x20, 1); wr32(ch_self, MB + 0x1C, 0x00); /* 0x100 */
    dot_ret = 0x1000; len_ret = 0x1000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rs16(ch_self, MB + 0x32), 0x100, "same link, target behind: latch 0x200>>1");
    chase_reset();
    wr32(ch_tgt, 0x24, -1);
    wr16(ch_tgt, MB + 0x20, 1); wr32(ch_tgt, MB + 0x1C, 0x00);
    wr16(ch_self, MB + 0x20, 1); wr32(ch_self, MB + 0x1C, 0x00);
    dot_ret = 0x1000; len_ret = 0x1000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rd16(ch_self, MB + 0x40), 0x5555, "same link, equal progress: no change");
    FCK_EQ(rd16(ch_self, MB + 0x32), 0x7777, "same link, equal progress: speed kept");

    /* different +0x44: self+0x44 == target+0x48 latches */
    chase_reset();
    wr32(ch_tgt, 0x24, -1);
    wr16(ch_tgt, MB + 0x44, 7); wr16(ch_self, MB + 0x44, 8);
    wr16(ch_tgt, MB + 0x48, 8);
    dot_ret = 0x1000; len_ret = 0x1000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rs16(ch_self, MB + 0x40), 300, "link +0x44 == target +0x48: latch");
    /* self+0x46 != target+0x44: hold 1 */
    chase_reset();
    wr32(ch_tgt, 0x24, -1);
    wr16(ch_tgt, MB + 0x44, 7); wr16(ch_self, MB + 0x44, 8);
    wr16(ch_tgt, MB + 0x48, 9); wr16(ch_self, MB + 0x46, 6);
    dot_ret = 0x1000; len_ret = 0x1000;
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rs16(ch_self, MB + 0x40), 1, "unlinked: hold 1");
    FCK_EQ(rd16(ch_self, MB + 0x32), 0x7777, "unlinked: speed kept");
    /* self+0x46 == target+0x44: latch */
    wr16(ch_self, MB + 0x46, 7);
    gUpdateChaseApproachSpeed(0, ch_self, 0);
    FCK_EQ(rs16(ch_self, MB + 0x40), 300, "+0x46 == target +0x44: latch");
}

/* 80040E60: source descriptor +0x34..+0x57, object of 0x98 bytes */
static u8 lt_src[0x60];

static void light_src_reset(void)
{
    s32 i;
    lk_fill(lt_src, 0xEE, sizeof lt_src);
    for (i = 0x34; i < 0x58; i++) wr8(lt_src, i, i);  /* byte = its offset */
    wr16(lt_src, 0x38, 0x0123);   /* rotation x */
    wr16(lt_src, 0x3A, 0x0100);   /* rotation y, negated */
    wr16(lt_src, 0x3C, 0x0800);   /* intensity scale */
    wr16(lt_src, 0x3E, 0x0000);
    wr16(lt_src, 0x4C, 7);        /* period */
}

static void test_light(void)
{
    void *r;

    /* @leaf 80040E60 source src/game/80040E60.c: count cap 0x15F, alloc(0x98,1), field copy map, rotation(+0x38,-+0x3A,0) vector scaled ((v>>4)*s)>>12, 300/period or -1, word flag at src+0x3C, handler D_80040874, count++ before colour steps */
    LK_FUNC("func_80040E60");

    /* cap */
    light_src_reset();
    gLightObjectCount = 0x15F;
    alloc_calls = 0; alloc_ret = alloc_buf;
    r = gCreateLightObject(lt_src);
    FCK_CHECK(r == 0, "count 0x15F: null");
    FCK_EQ(alloc_calls, 0, "count 0x15F: no allocation");

    /* allocation failure */
    gLightObjectCount = 3;
    alloc_ret = 0; motion_calls = 0;
    r = gCreateLightObject(lt_src);
    FCK_CHECK(r == 0, "alloc failure: null");
    FCK_EQ(gLightObjectCount, 3, "alloc failure: count unchanged");
    FCK_EQ(motion_calls, 0, "alloc failure: no init");

    /* full construction */
    light_src_reset();
    lk_fill(alloc_buf, 0xA5, sizeof alloc_buf);
    gLightObjectCount = 0x15E;
    alloc_ret = alloc_buf; alloc_calls = 0;
    motion_calls = 0; rot_calls = 0; steps_calls = 0;
    rot_out[0] = 0x1000; rot_out[1] = -0x1000; rot_out[2] = 0x123;
    r = gCreateLightObject(lt_src);
    FCK_CHECK(r == alloc_buf, "returns the allocation");
    FCK_EQ(alloc_args[0], 0x98, "alloc size 0x98");
    FCK_EQ(alloc_args[1], 1, "alloc tag 1");
    FCK_EQ(motion_calls, 1, "motion init once");
    FCK_CHECK(motion_dest == alloc_buf && motion_src == lt_src, "motion init (dest, src)");
    FCK_EQ(rot_args[0], 0x123, "rotation x = +0x38");
    FCK_EQ(rot_args[1], -0x100, "rotation y = -+0x3A");
    FCK_EQ(rot_args[2], 0, "rotation z = 0");
    FCK_EQ(rs16(alloc_buf, 0x70), 0x80, "+0x70 = ((0x1000>>4)*0x800)>>12");
    FCK_EQ(rs16(alloc_buf, 0x72), -0x80, "+0x72 negative component");
    FCK_EQ(rs16(alloc_buf, 0x74), 9, "+0x74 = ((0x123>>4)*0x800)>>12");
    FCK_EQ(rd8(alloc_buf, 0x64), 0x34, "+0x64 = src +0x34");
    FCK_EQ(rd16(alloc_buf, 0x12), 0x3736, "+0x12 = src +0x36");
    FCK_EQ(rd16(alloc_buf, 0x82), 0x0000, "+0x82 = src +0x3E");
    FCK_EQ(rd16(alloc_buf, 0x76), 0x4B4A, "+0x76 = src +0x4A");
    FCK_EQ(rd16(alloc_buf, 0x66), 0x5756, "+0x66 = src +0x56");
    FCK_EQ(rd16(alloc_buf, 0x7A), 0x4F4E, "+0x7A = src +0x4E");
    FCK_EQ(rd16(alloc_buf, 0x7C), 0x5150, "+0x7C = src +0x50");
    FCK_EQ(rd16(alloc_buf, 0x7E), 0x5352, "+0x7E = src +0x52");
    FCK_EQ(rd16(alloc_buf, 0x80), 0x5554, "+0x80 = src +0x54");
    FCK_EQ(rs16(alloc_buf, 0x78), 42, "+0x78 = 300 / 7");
    FCK_EQ(rd32(alloc_buf, 0x6C), 0, "+0x6C cleared");
    FCK_EQ(rd8(alloc_buf, 0x87), 0x41, "+0x87 = src +0x41");
    FCK_EQ(rd8(alloc_buf, 0x88), 0x42, "+0x88 = src +0x42");
    FCK_EQ(rd8(alloc_buf, 0x89), 0x43, "+0x89 = src +0x43");
    FCK_EQ(rd8(alloc_buf, 0x8A), 0xA5, "+0x8A untouched (src +0x44 not copied)");
    FCK_EQ(rd8(alloc_buf, 0x8B), 0x45, "+0x8B = src +0x45");
    FCK_EQ(rd8(alloc_buf, 0x8C), 0x46, "+0x8C = src +0x46");
    FCK_EQ(rd8(alloc_buf, 0x8D), 0x47, "+0x8D = src +0x47");
    FCK_EQ(rd16(alloc_buf, 0x94), 0x4948, "+0x94 = src +0x48");
    FCK_EQ(rd32(alloc_buf, 0x24), (u32)gLightHandler, "+0x24 = handler D_80040874");
    FCK_EQ(rd8(alloc_buf, 0x68), 1, "+0x68 = word at src+0x3C nonzero");
    FCK_EQ(rd16(alloc_buf, 0x08), 0, "+0x08 cleared");
    FCK_EQ(rd16(alloc_buf, 0x0A), 1, "+0x0A = 1");
    FCK_EQ(gLightObjectCount, 0x15F, "count incremented");
    FCK_EQ(steps_calls, 1, "colour steps once");
    FCK_CHECK(steps_arg == alloc_buf, "colour steps on the object");
    FCK_EQ(steps_count_seen, 0x15F, "count incremented before colour steps");
    FCK_EQ(steps_0a_seen, 1, "+0x0A set before colour steps");

    /* period 0 -> -1; negative period; the flag reads the whole word at
     * +0x3C, so a zero scale with nonzero +0x3E still sets it */
    light_src_reset();
    wr16(lt_src, 0x4C, 0);
    wr16(lt_src, 0x3C, 0);
    wr16(lt_src, 0x3E, 5);
    gLightObjectCount = 0;
    r = gCreateLightObject(lt_src);
    FCK_EQ(rs16(alloc_buf, 0x78), -1, "period 0: -1");
    FCK_EQ(rd8(alloc_buf, 0x68), 1, "scale 0, +0x3E 5: word nonzero");
    FCK_EQ(rs16(alloc_buf, 0x70), 0, "scale 0: vector 0");
    FCK_EQ(rd16(alloc_buf, 0x82), 5, "+0x82 = +0x3E");
    wr16(lt_src, 0x4C, -3);
    wr16(lt_src, 0x3E, 0);
    r = gCreateLightObject(lt_src);
    FCK_EQ(rs16(alloc_buf, 0x78), -100, "period -3: 300 / -3");
    FCK_EQ(rd8(alloc_buf, 0x68), 0, "word at +0x3C zero: flag 0");
}

/* 800449F8: script commands at +0x0C (4 x {op, arg}); track +0x0C frame,
 * +0x13 flags */
static u8 an_script[0x16], an_track[0x14];

static void anim_reset(const u8 *cmds)
{
    s32 i;
    lk_fill(an_script, 0xCC, 0x0C);
    for (i = 0; i < 8; i++) an_script[0xC + i] = cmds[i];
    an_script[0x14] = 0x34;                /* guard past the 4-entry list */
    an_script[0x15] = 0x09;
    lk_fill(an_track, 0x11, sizeof an_track);
    sel_calls = 0; rm_calls = 0;
    reset_calls();
}

static void test_anim_script(void)
{
    static const u8 s1[8] = { 0x32, 5, 0x99, 1, 0x34, 7, 0, 0 };
    static const u8 s2[8] = { 0x33, 0xC8, 0, 0, 0x32, 1, 0, 0 };
    static const u8 s3[8] = { 0x10, 1, 0x20, 2, 0x31, 3, 0x35, 4 };
    static const u8 s4[8] = { 0, 0, 0x32, 1, 0x32, 2, 0x32, 3 };
    static const u8 s5[8] = { 0x34, 0, 0, 0, 0, 0, 0, 0 };
    static const u8 s6[8] = { 0x33, 0, 0, 0, 0, 0, 0, 0 };

    /* @leaf 800449F8 source src/game/800449F8.c: ops 0x32 select clip (track, arg, 1, 0), 0x33/0x34 clear/set flag bit 1 and set frame = unsigned arg, executed ops removed in place, unknown ops skipped, op 0 stops */
    LK_FUNC("func_800449F8");

    anim_reset(s1);
    wr8(an_track, 0x13, 0x00);
    gProcessAnimScriptCommands(an_script, an_track);
    FCK_EQ(sel_calls, 1, "0x32: select once");
    FCK_CHECK(sel_track == an_track, "0x32: select on the track");
    FCK_EQ(sel_args[0], 5, "0x32: clip index = arg");
    FCK_EQ(sel_args[1], 1, "0x32: direction 1");
    FCK_EQ(sel_args[2], 0, "0x32: flags 0");
    FCK_EQ(rm_calls, 2, "two commands executed and removed");
    FCK_CHECK(rm_script[0] == an_script && rm_script[1] == an_script, "removals target the script");
    FCK_EQ(rm_idx[0], 0, "first removal at 0");
    FCK_EQ(rm_idx[1], 1, "0x34 removed at 1 after skipping unknown 0x99");
    FCK_EQ(ncalls, 3, "call sequence: select clip then two removals");
    FCK_CHECK(calls[0].id == C_SEL && calls[1].id == C_RM && calls[2].id == C_RM,
              "0x32 selects clip before removing at 0, then removes subsequent 0x34 at 1");
    FCK_EQ(rd8(an_track, 0x13), 0x02, "subsequent 0x34: flag bit 1 set");
    FCK_EQ(rs16(an_track, 0x0C), 7, "subsequent 0x34: frame = 7");

    anim_reset(s2);
    wr8(an_track, 0x13, 0xFF);
    gProcessAnimScriptCommands(an_script, an_track);
    FCK_EQ(rd8(an_track, 0x13), 0xFD, "0x33: only flag bit 1 cleared");
    FCK_EQ(rs16(an_track, 0x0C), 200, "0x33: arg 0xC8 is unsigned -> frame 200");
    FCK_EQ(rm_calls, 1, "0x33 removed, then op 0 stops");
    FCK_EQ(rm_idx[0], 0, "0x33 removed at index 0");
    FCK_EQ(sel_calls, 0, "0x32 behind the terminator not run");

    anim_reset(s3);
    gProcessAnimScriptCommands(an_script, an_track);
    FCK_EQ(rm_calls, 0, "four unknown ops: nothing removed");
    FCK_EQ(sel_calls, 0, "four unknown ops: nothing selected");
    FCK_CHECK(lk_all(an_track, 0, sizeof an_track, 0x11), "four unknown ops: track untouched (no fifth entry read)");

    anim_reset(s4);
    gProcessAnimScriptCommands(an_script, an_track);
    FCK_EQ(sel_calls, 0, "leading op 0: nothing runs");

    anim_reset(s5);
    gProcessAnimScriptCommands(an_script, an_track);
    FCK_EQ(rs16(an_track, 0x0C), 0, "0x34 arg 0: frame 0");
    FCK_EQ(rd8(an_track, 0x13), 0x13, "0x34: flag bit 1 added to 0x11");

    anim_reset(s6);
    wr8(an_track, 0x13, 0x02);
    gProcessAnimScriptCommands(an_script, an_track);
    FCK_EQ(rs16(an_track, 0x0C), 0, "0x33 arg 0: frame 0");
    FCK_EQ(rd8(an_track, 0x13), 0x00, "0x33: flag bit 1 cleared");
}

/* 80047BE8: two POLY_G4 at the primitive pool (psx-spx GP0(38h) shaded
 * 4-point opaque: colour/cmd, xy, colour, xy, colour, xy, colour, xy;
 * Psy-Q setPolyG4 tag length 8) plus HUD tiles. */
static void test_hud_bar(void)
{
    u8 *p, *q;

    /* @leaf 80047BE8 source src/game/80047BE8.c: background tile, two POLY_G4 (GP0 0x38, tag len 8, low 24 tag bits cleared) with dim=p/2 and bright=p channels in opposite horizontal gradients, height (z+1)/2, flag + frame byte >= 0x80 halves, pool cursor +0x34 unless full, five tick tiles */
    LK_FUNC("func_80047BE8");

    lk_fill(prim_pool, 0xA5, sizeof prim_pool);
    gPrimNext = prim_pool;
    gPrimEnd = prim_pool + sizeof prim_pool;
    gFrameClock = 0;
    reset_calls();
    gEmitHudGouraudBar(100, 50, 31, 0x40, 0x20, 0x81, 0);
    p = prim_pool; q = prim_pool + 0x34;
    FCK_CHECK(gPrimNext == prim_pool + 0x68, "two primitives taken (0x34 each)");
    FCK_EQ(rd32(p, 0), 0x08000000, "prim 1: tag length 8, link cleared");
    FCK_EQ(rd8(p, 7), 0x38, "prim 1: GP0 0x38");
    /* p = 2*in: 0x80, 0x40, 0x102; dim = p/4: 0x20, 0x10, 0x40; z' = 16 */
    FCK_EQ(rd8(p, 4), 0x20, "prim 1 v0 R dim");
    FCK_EQ(rd8(p, 5), 0x10, "prim 1 v0 G dim");
    FCK_EQ(rd8(p, 6), 0x40, "prim 1 v0 B dim");
    FCK_EQ(rs16(p, 0x08), 100, "prim 1 v0 x");
    FCK_EQ(rs16(p, 0x0A), 34, "prim 1 v0 y = 50 - 16");
    FCK_EQ(rd8(p, 0x0C), 0x80, "prim 1 v1 R bright");
    FCK_EQ(rd8(p, 0x0D), 0x40, "prim 1 v1 G bright");
    FCK_EQ(rd8(p, 0x0E), 0x02, "prim 1 v1 B bright (0x102 truncated)");
    FCK_EQ(rs16(p, 0x10), 104, "prim 1 v1 x = x + 4");
    FCK_EQ(rs16(p, 0x12), 34, "prim 1 v1 y");
    FCK_EQ(rd8(p, 0x14), 0x20, "prim 1 v2 R dim");
    FCK_EQ(rd8(p, 0x16), 0x40, "prim 1 v2 B dim");
    FCK_EQ(rs16(p, 0x18), 100, "prim 1 v2 x");
    FCK_EQ(rs16(p, 0x1A), 50, "prim 1 v2 y = base");
    FCK_EQ(rd8(p, 0x1C), 0x80, "prim 1 v3 R bright");
    FCK_EQ(rd8(p, 0x1E), 0x02, "prim 1 v3 B bright");
    FCK_EQ(rs16(p, 0x20), 104, "prim 1 v3 x");
    FCK_EQ(rs16(p, 0x22), 50, "prim 1 v3 y");
    FCK_EQ(rd32(q, 0), 0x08000000, "prim 2: tag length 8");
    FCK_EQ(rd8(q, 7), 0x38, "prim 2: GP0 0x38");
    FCK_EQ(rd8(q, 4), 0x80, "prim 2 v0 bright");
    FCK_EQ(rd8(q, 0x0C), 0x20, "prim 2 v1 dim");
    FCK_EQ(rd8(q, 0x14), 0x80, "prim 2 v2 bright");
    FCK_EQ(rd8(q, 0x1C), 0x20, "prim 2 v3 dim");
    FCK_EQ(rs16(q, 0x08), 104, "prim 2 v0 x");
    FCK_EQ(rs16(q, 0x10), 108, "prim 2 v1 x");
    FCK_EQ(rs16(q, 0x18), 104, "prim 2 v2 x");
    FCK_EQ(rs16(q, 0x20), 108, "prim 2 v3 x");
    FCK_EQ(rs16(q, 0x0A), 34, "prim 2 v0 y");
    FCK_EQ(rs16(q, 0x22), 50, "prim 2 v3 y");
    FCK_EQ(ncalls, 6, "six tiles");
    FCK_EQ(calls[0].a[0], 0x2000, "tile 0 colour");
    FCK_EQ(calls[0].a[1], 200, "tile 0 x = 2x");
    FCK_EQ(calls[0].a[2], 4, "tile 0 y = 2y - 0x60");
    FCK_EQ(calls[0].a[3], 0x10, "tile 0 w");
    FCK_EQ(calls[0].a[4], 0x60, "tile 0 h");
    FCK_EQ(calls[0].a[5], 1, "tile 0 blend 1");
    FCK_EQ(calls[1].a[0], 0x7FFF, "tick 1 white");
    FCK_EQ(calls[1].a[1], 204, "tick 1 x = 2*(x+4-2)");
    FCK_EQ(calls[1].a[2], 4, "tick 1 y = 2y - 0x60");
    FCK_EQ(calls[1].a[3], 8, "tick 1 w 8");
    FCK_EQ(calls[1].a[4], 2, "tick 1 h 2");
    FCK_EQ(calls[1].a[5], 0, "tick 1 blend 0");
    FCK_EQ(calls[2].a[2], 52, "tick 2 y = 2y - 0x30");
    FCK_EQ(calls[3].a[2], 98, "tick 3 y = 2y - 2");
    FCK_EQ(calls[4].a[1], 206, "tick 4 x = 2*(x+4-1)");
    FCK_EQ(calls[4].a[2], 28, "tick 4 y = 2y - 0x48");
    FCK_EQ(calls[4].a[3], 4, "tick 4 w 4");
    FCK_EQ(calls[5].a[2], 76, "tick 5 y = 2y - 0x18");

    /* blink: flag set and frame byte 0x80 halves every channel */
    lk_fill(prim_pool, 0xA5, sizeof prim_pool);
    gPrimNext = prim_pool;
    gFrameClock = 0x80;
    reset_calls();
    gEmitHudGouraudBar(0, 10, -3, 0x40, 0x20, 0x81, 1);
    FCK_EQ(rd8(prim_pool, 0x0C), 0x40, "blink: bright R halved");
    FCK_EQ(rd8(prim_pool, 0x0E), 0x81, "blink: bright B 0x102/2");
    FCK_EQ(rd8(prim_pool, 4), 0x10, "blink: dim R halved");
    FCK_EQ(rd8(prim_pool, 6), 0x20, "blink: dim B halved");
    FCK_EQ(rs16(prim_pool, 0x0A), 11, "negative height: (-3+1)/2 = -1, y0 = 11");
    /* the clock is read as a byte: 0x100 has low byte 0 -> no halving */
    lk_fill(prim_pool, 0xA5, sizeof prim_pool);
    gPrimNext = prim_pool;
    gFrameClock = 0x100;
    gEmitHudGouraudBar(0, 10, 0, 0x40, 0x20, 0x81, 1);
    FCK_EQ(rd8(prim_pool, 0x0C), 0x80, "clock byte 0: not halved");
    gFrameClock = 0x80;
    gPrimNext = prim_pool;
    gEmitHudGouraudBar(0, 10, 0, 0x40, 0x20, 0x81, 0);
    FCK_EQ(rd8(prim_pool, 0x0C), 0x80, "flag 0: not halved");

    /* pool exhausted: both writes land on the cursor, cursor kept */
    lk_fill(prim_pool, 0xA5, sizeof prim_pool);
    gPrimNext = prim_pool + 0x34;
    gPrimEnd = prim_pool + 0x34;
    gFrameClock = 0;
    gEmitHudGouraudBar(100, 50, 31, 0x40, 0x20, 0x81, 0);
    FCK_CHECK(gPrimNext == prim_pool + 0x34, "full pool: cursor unchanged");
    FCK_EQ(rs16(prim_pool + 0x34, 0x08), 104, "full pool: second prim overwrote the first");
    FCK_CHECK(lk_all(prim_pool, 0, 0x34, 0xA5), "full pool: nothing written below the cursor");
}

/* 8004CAE4: 24-bit BGR marker colours (0xF8 bright / 0x78 dim channel). */
static void test_marker_colors(void)
{
    static const u32 expect[32] = {
        0x000000, 0x0000F8, 0xF80000, 0x00F800, 0xF8F800, 0x00F8F8, 0xF800F8, 0xF8F8F8,
        0x0078F8, 0x000078, 0x780000, 0x007800, 0x787800, 0x007878, 0x780078, 0x787878,
        0x003878, 0x0000F8, 0xF80000, 0x00F800, 0x8080F8, 0xF88070, 0x80F880, 0x0000F8,
        0xF80000, 0x00F800, 0xF8F800, 0x00F8F8, 0xF800F8, 0xF8F8F8, 0x0078F8, 0x787878,
    };
    s32 i, ok;

    /* @leaf 8004CAE4 source src/game/8004CAE4.c: 32-entry marker colour table literals, list heads/counters cleared, done flag 1, list capacity 0xFE (session +0x5C == 1), 0x12C (+0x0C == 2) or 0xC8 */
    LK_FUNC("func_8004CAE4");

    lk_fill(gMarkerColorTable, 0x5A, sizeof gMarkerColorTable);
    lk_fill(gCommandList, 0x5A, sizeof gCommandList);
    lk_fill(gRequestOffsetBias, 0x5A, sizeof gRequestOffsetBias);
    gEntityIdCounter = 11; gProximityScriptDeadline = 12; gPendingMessageMask = 13;
    gMessageDoneFlag = 0; gUnitListHead = 14; gListTailBranch = 15;
    lk_fill(session, 0, sizeof session);
    gSessionPtr = session;
    wr32(session, 0x5C, 1);
    wr32(session, 0x0C, 2);
    gInitMarkerColorsAndPools();
    ok = 1;
    for (i = 0; i < 32; i++)
        if ((u32)gMarkerColorTable[i] != expect[i]) ok = 0;
    FCK_CHECK(ok, "all 32 marker colours");
    FCK_EQ(gMarkerColorTable[4], 0xF8F800, "colour 4 cyan-ish 0xF8F800");
    FCK_EQ(gMarkerColorTable[21], 0xF88070, "colour 21 0xF88070");
    FCK_EQ(gMarkerColorTable[31], 0x787878, "colour 31 dim white");
    FCK_EQ((u32)gMarkerColorTable[32], 0x5A5A5A5Au, "entry 32 untouched");
    FCK_EQ(gCommandList[0], 0, "list head cleared");
    FCK_EQ(gCommandList[1], 0xFE, "+0x5C == 1 wins: capacity 0xFE");
    FCK_EQ(gCommandList[2], 0x5A5A5A5Au, "list item 0 untouched");
    FCK_EQ(gEntityIdCounter, 0, "entity id counter cleared");
    FCK_EQ(gProximityScriptDeadline, 0, "proximity deadline cleared");
    FCK_EQ(gPendingMessageMask, 0, "pending mask cleared");
    FCK_EQ(gMessageDoneFlag, 1, "done flag set");
    FCK_EQ(gUnitListHead, 0, "unit list cleared");
    FCK_EQ(gListTailBranch, 0, "tail branch cleared");
    FCK_EQ(gRequestOffsetBias[0], 0, "request bias [0] cleared");
    FCK_EQ((u32)gRequestOffsetBias[1], 0x5A5A5A5Au, "request bias [1] untouched");

    wr32(session, 0x5C, 0);
    wr32(session, 0x0C, 2);
    gInitMarkerColorsAndPools();
    FCK_EQ(gCommandList[1], 0x12C, "+0x0C == 2: capacity 0x12C");
    wr32(session, 0x0C, 1);
    gInitMarkerColorsAndPools();
    FCK_EQ(gCommandList[1], 0xC8, "default capacity 0xC8");
    wr32(session, 0x5C, 2);
    wr32(session, 0x0C, 2);
    gInitMarkerColorsAndPools();
    FCK_EQ(gCommandList[1], 0x12C, "+0x5C == 2 falls through to the +0x0C test");
}

/* 800511E4: resource +0x00 data, +0x08 state; data +0x0E count, entries of
 * 12 bytes at +0x14 (+0 flags, +2 z, +4 x, +6 type/height word). */
static u8 pn_res[0x20], pn_data[0x14 + 12 * 6];

static void pn_entry(s32 i, u32 flags, s32 z, s32 x, u32 w6)
{
    u8 *e = pn_data + 0x14 + 12 * i;
    wr8(e, 0, flags); wr8(e, 1, 0x77);
    wr16(e, 2, z); wr16(e, 4, x); wr16(e, 6, w6);
    wr32(e, 8, 0x66666666u);
}

static void test_path_nodes(void)
{
    u8 *e;

    /* @leaf 800511E4 source src/game/800511E4.c: u8 status gate (else func_80019D00), slot table publish/increment, per entry position (x=+4<<8, z=+2<<8), type 5 height (s16 word)*16 else surface query with mode -1/-2/type-2, rewrite +4 = (h+0x7F)>>8, +2 = x>>8, +6 = z>>8, flags low nibble cleared */
    LK_FUNC("func_800511E4");

    /* status nonzero: reuse path only */
    lk_fill(pn_res, 0x33, sizeof pn_res);
    wrp(pn_res, 0, pn_data);
    lk_fill(pn_data, 0x44, sizeof pn_data);
    res_status = 1; res_ack_calls = 0; res_reuse_calls = 0; surf_calls = 0;
    gPlacementSlotCount = 1;
    lk_fill(gPathNetTables, 0, sizeof gPathNetTables);
    gLoadPathNodePositions(pn_res);
    FCK_EQ(res_reuse_calls, 1, "status 1: reuse called");
    FCK_CHECK(res_reuse_arg == pn_res, "status 1: reuse on the resource");
    FCK_EQ(res_ack_calls, 0, "status 1: no load");
    FCK_EQ(gPlacementSlotCount, 1, "status 1: slot count kept");
    FCK_EQ(rd32(pn_res, 8), 0x33333333u, "status 1: state untouched");

    /* status 0x100 truncates to 0 in u8: load path */
    lk_fill(pn_data, 0x44, sizeof pn_data);
    wr16(pn_data, 0x0E, 5);
    pn_entry(0, 0xAB, 10, 20, 0x0000);          /* type 0 -> mode -1 */
    pn_entry(1, 0x0F, -3, -4, 0x1231);          /* type 1 -> mode -2 */
    pn_entry(2, 0xF7, 1, 2, 0x0007);            /* type 7 -> mode 5 */
    pn_entry(3, 0x50, 6, 7, 0xFFF5);            /* type 5, height -11*16 */
    pn_entry(4, 0x05, 8, 9, 0x0105);            /* type 5, height 0x105*16 */
    pn_entry(5, 0x99, 1, 1, 0x0000);            /* beyond count */
    res_status = 0x100; res_reuse_calls = 0; surf_calls = 0;
    surf_ret[0] = 0x1234; surf_ret[1] = -0x100; surf_ret[2] = 0x80;
    gPlacementSlotCount = 1;
    lk_fill(gPathNetTables, 0, sizeof gPathNetTables);
    gLoadPathNodePositions(pn_res);
    FCK_EQ(res_reuse_calls, 0, "status 0x100: low byte 0 loads");
    FCK_EQ(res_ack_calls, 1, "load: ack once");
    FCK_EQ(rd32(pn_res, 8), 0, "load: resource +0x08 cleared");
    FCK_CHECK(gPathNetTables[1] == pn_data + 0x0C, "load: table[slot] = data + 0x0C");
    FCK_CHECK(gPathNetTables[2] == 0, "load: next slot untouched");
    FCK_EQ(gPlacementSlotCount, 2, "load: slot count incremented");
    FCK_EQ(surf_calls, 3, "three surface queries (types 0, 1, 7)");
    FCK_EQ(surf_x[0], 20 * 256, "query 0 x = +4 << 8");
    FCK_EQ(surf_z[0], 10 * 256, "query 0 z = +2 << 8");
    FCK_EQ(surf_mode[0], -1, "type 0 -> mode -1");
    FCK_EQ(surf_x[1], -4 * 256, "query 1 negative x");
    FCK_EQ(surf_mode[1], -2, "type 1 -> mode -2");
    FCK_EQ(surf_mode[2], 5, "type 7 -> mode 5");
    e = pn_data + 0x14;
    FCK_EQ(rs16(e, 4), 0x12, "entry 0: height (0x1234+0x7F)>>8");
    FCK_EQ(rs16(e, 2), 20, "entry 0: +2 = x");
    FCK_EQ(rs16(e, 6), 10, "entry 0: +6 = z");
    FCK_EQ(rd8(e, 0), 0xA0, "entry 0: flags low nibble cleared");
    FCK_EQ(rd8(e, 1), 0x77, "entry 0: +1 untouched");
    e += 12;
    FCK_EQ(rs16(e, 4), -1, "entry 1: (-0x100+0x7F)>>8 arithmetic = -1");
    FCK_EQ(rs16(e, 2), -4, "entry 1: +2 = x");
    FCK_EQ(rs16(e, 6), -3, "entry 1: +6 = z");
    FCK_EQ(rd8(e, 0), 0x00, "entry 1: flags 0x0F -> 0");
    e += 12;
    FCK_EQ(rs16(e, 4), 0, "entry 2: (0x80+0x7F)>>8 = 0");
    FCK_EQ(rd8(e, 0), 0xF0, "entry 2: flags 0xF7 -> 0xF0");
    e += 12;
    FCK_EQ(rs16(e, 4), -1, "entry 3: (-176+0x7F)>>8 = -1");
    FCK_EQ(rs16(e, 2), 7, "entry 3: +2 = x");
    FCK_EQ(rs16(e, 6), 6, "entry 3: +6 = z");
    e += 12;
    FCK_EQ(rs16(e, 4), 16, "entry 4: (4176+0x7F)>>8 = 16");
    e += 12;
    FCK_EQ(rd8(e, 0), 0x99, "entry 5 beyond count untouched");
    FCK_EQ(rd16(e, 6), 0, "entry 5 word untouched");

    /* count 0: table published, nothing walked */
    wr16(pn_data, 0x0E, 0);
    surf_calls = 0;
    gLoadPathNodePositions(pn_res);
    FCK_EQ(gPlacementSlotCount, 3, "count 0: slot still advances");
    FCK_CHECK(gPathNetTables[2] == pn_data + 0x0C, "count 0: published at slot 2");
    FCK_EQ(surf_calls, 0, "count 0: no queries");
}

/* 8005A2BC: sprite headers {?, ?, frames, ctz-src, col<<4, row, ?, ?} then
 * 8-byte frames; output {count, items[12]: frames, ctz, -, frames ptr}. */
static u8 sd_a[8 + 8 * 3], sd_b[8 + 8 * 3], sd_hdr[4 + 12 * 3];

static void test_sprite_desc(void)
{
    void *arr[2];
    s32 r;

    /* @leaf 8005A2BC source src/game/8005A2BC.c: alloc(count*12+4, 2) or 0, item = {frames byte 2, ctz(byte 3), ptr+8}, per frame byte +6 += 0x80 and byte +7 = (col<<6)|row with col from byte 4 >> 4 wrapping 4 -> next row, table installed, returns 1 */
    LK_FUNC("func_8005A2BC");

    lk_fill(sd_a, 0, sizeof sd_a);
    sd_a[2] = 3; sd_a[3] = 0x08; sd_a[4] = 0x2F; sd_a[5] = 1;
    sd_a[8 + 6] = 0x10; sd_a[16 + 6] = 0x90; sd_a[24 + 6] = 0xFF;
    sd_a[8 + 7] = 0xEE; sd_a[16 + 7] = 0xEE; sd_a[24 + 7] = 0xEE;
    lk_fill(sd_b, 0, sizeof sd_b);
    sd_b[2] = 2; sd_b[3] = 0x01; sd_b[4] = 0x50; sd_b[5] = 3;
    arr[0] = sd_a; arr[1] = sd_b;
    lk_fill(sd_hdr, 0xA5, sizeof sd_hdr);
    alloc_ret = sd_hdr; alloc_calls = 0; ctz_calls = 0; desc_calls = 0;
    r = gBuildSpriteDescTable(arr, 2);
    FCK_EQ(r, 1, "returns 1");
    FCK_EQ(alloc_args[0], 28, "alloc 2*12+4");
    FCK_EQ(alloc_args[1], 2, "alloc tag 2");
    FCK_EQ(rd32(sd_hdr, 0), 2, "header count");
    FCK_EQ(rd8(sd_hdr, 4), 3, "item 0 frames");
    FCK_EQ(ctz_args[0], 0x08, "ctz gets header byte 3");
    FCK_EQ(rd8(sd_hdr, 5), 0x18, "item 0 +1 = ctz result");
    FCK_EQ(rd32(sd_hdr, 12), (u32)(sd_a + 8), "item 0 frames pointer = header + 8");
    FCK_EQ(sd_a[8 + 6], 0x90, "frame 0 +6: 0x10 + 0x80");
    FCK_EQ(sd_a[8 + 7], 0x81, "frame 0 +7: col 2, row 1");
    FCK_EQ(sd_a[16 + 6], 0x10, "frame 1 +6: 0x90 + 0x80 wraps");
    FCK_EQ(sd_a[16 + 7], 0xC1, "frame 1 +7: col 3, row 1");
    FCK_EQ(sd_a[24 + 6], 0x7F, "frame 2 +6: 0xFF + 0x80 wraps");
    FCK_EQ(sd_a[24 + 7], 0x02, "frame 2 +7: col wrapped to 0, row 2");
    FCK_EQ(sd_a[4], 0x2F, "header byte 4 untouched");
    FCK_EQ(rd8(sd_hdr, 16), 2, "item 1 frames");
    FCK_EQ(rd8(sd_hdr, 17), 0x11, "item 1 ctz result");
    /* col 5 never equals 4: (5<<6)|3 truncates to 0x43, (6<<6)|3 to 0x83 */
    FCK_EQ(sd_b[8 + 7], 0x43, "start col 5: 0x143 truncated");
    FCK_EQ(sd_b[16 + 7], 0x83, "col 6, no row wrap");
    FCK_EQ(sd_b[8 + 6], 0x80, "item 1 frame 0 +6 += 0x80");
    FCK_EQ(desc_calls, 1, "table installed");
    FCK_CHECK(desc_arg == sd_hdr, "installed table is the allocation");

    /* allocation failure */
    alloc_ret = 0; desc_calls = 0; ctz_calls = 0;
    r = gBuildSpriteDescTable(arr, 2);
    FCK_EQ(r, 0, "alloc failure returns 0");
    FCK_EQ(desc_calls, 0, "alloc failure: nothing installed");
    FCK_EQ(ctz_calls, 0, "alloc failure: nothing walked");

    /* count 0 */
    alloc_ret = sd_hdr; desc_calls = 0;
    r = gBuildSpriteDescTable(arr, 0);
    FCK_EQ(alloc_args[0], 4, "count 0: alloc 4");
    FCK_EQ(rd32(sd_hdr, 0), 0, "count 0: header count 0");
    FCK_EQ(desc_calls, 1, "count 0: still installed");
}

/* 8005FFB8: object +0x00 entry ptr, +0x04 flags, +0x06 mode, +0x08 arg,
 * +0x20 key word; script commands at +0x0C. */
static u8 at_obj[0x24], at_script[0x16];

static void attack_reset(const u8 *cmds)
{
    s32 i;
    lk_fill(at_obj, 0x5A, sizeof at_obj);
    lk_fill(at_script, 0xCC, 0x0C);
    for (i = 0; i < 8; i++) at_script[0xC + i] = cmds[i];
    at_script[0x14] = 0x21;                /* guard past the 4-entry list */
    at_script[0x15] = 0x09;
    rm_calls = 0; find_calls = 0;
    reset_calls();
}

static void test_attack_cmds(void)
{
    static const u8 s1[8] = { 0x21, 0x90, 0x24, 9, 0x1E, 0, 0x26, 0x42 };
    static const u8 s2[8] = { 0x1E, 1, 0x26, 0x07, 0x1F, 0, 0x23, 3 };
    static const u8 s3[8] = { 0x25, 1, 0x1D, 2, 0x27, 3, 0x00, 4 };
    static const u8 s4[8] = { 0x25, 1, 0x1D, 2, 0x27, 3, 0x28, 4 };
    static const u8 s5[8] = { 0x26, 0x10, 0, 0, 0, 0, 0, 0 };

    /* @leaf 8005FFB8 source src/game/8005FFB8.c: ops 0x1F..0x23 set mode op-0x1E and +0x08 = arg, 0x1E arg 0 sets flag 0x100 else clears it, 0x26 binds table entry idx*0x30 and copies its +0x02 when the key is found, executed ops removed, others skipped */
    LK_FUNC("func_8005FFB8");

    lk_fill(gEffectKeyTable, 0x11, sizeof gEffectKeyTable);
    wr16(gEffectKeyTable, 2 * 0x30 + 2, 0xBEEF);

    attack_reset(s1);
    wr16(at_obj, 4, 0x0001);
    gApplyAttackCommands(at_obj, at_script);
    FCK_EQ(rd8(at_obj, 6), 3, "0x21: mode 3");
    FCK_EQ(rd16(at_obj, 8), 0x90, "0x21: +0x08 = unsigned arg");
    FCK_EQ(rd16(at_obj, 4), 0x0101, "subsequent 0x1E arg 0: flag 0x100 set, others kept");
    FCK_EQ(find_calls, 1, "subsequent 0x26: key lookup");
    FCK_EQ(find_arg, 0x42, "subsequent 0x26: lookup key = arg");
    FCK_EQ(rd32(at_obj, 0), (u32)(gEffectKeyTable + 2 * 0x30), "subsequent 0x26: entry = table + 2*0x30");
    FCK_EQ(rd16(at_obj, 0x20), 0xBEEF, "subsequent 0x26: +0x20 = entry +0x02");
    FCK_EQ(rm_calls, 3, "three executed ops removed (0x24 skipped)");
    FCK_CHECK(rm_script[0] == at_script && rm_script[1] == at_script && rm_script[2] == at_script,
              "removals target the script");
    FCK_EQ(rm_idx[0], 0, "0x21 removed at 0");
    FCK_EQ(rm_idx[1], 1, "0x1E removed at 1 after skipping unknown 0x24");
    FCK_EQ(rm_idx[2], 1, "0x26 removed at 1 after shifting into slot 1");
    FCK_CHECK(ncalls == 4 && calls[0].id == C_RM && calls[1].id == C_RM &&
              calls[2].id == C_FIND && calls[3].id == C_RM,
              "call order: remove 0x21, remove 0x1E, find key 0x42, remove 0x26");

    attack_reset(s2);
    wr16(at_obj, 4, 0xFFFF);
    gApplyAttackCommands(at_obj, at_script);
    FCK_EQ(rd16(at_obj, 4), 0xFEFF, "0x1E arg 1: only flag 0x100 cleared");
    FCK_EQ(rd32(at_obj, 0), 0x5A5A5A5Au, "0x26 miss: entry untouched");
    FCK_EQ(rd16(at_obj, 0x20), 0x5A5A, "0x26 miss: +0x20 untouched");
    FCK_EQ(rm_calls, 4, "miss still removes the command");
    FCK_CHECK(rm_idx[0] == 0 && rm_idx[1] == 0 && rm_idx[2] == 0 && rm_idx[3] == 0,
              "four consecutive handled ops all remove at index 0");
    FCK_EQ(rd8(at_obj, 6), 5, "subsequent 0x23 last: mode 5");
    FCK_EQ(rd16(at_obj, 8), 3, "subsequent 0x23: +0x08 = 3");

    attack_reset(s3);
    gApplyAttackCommands(at_obj, at_script);
    FCK_EQ(rm_calls, 0, "0x25/0x1D/0x27: skipped");
    FCK_CHECK(lk_all(at_obj, 0, sizeof at_obj, 0x5A), "skipped ops: object untouched");

    attack_reset(s4);
    gApplyAttackCommands(at_obj, at_script);
    FCK_EQ(rm_calls, 0, "four unknown ops: nothing removed");
    FCK_CHECK(lk_all(at_obj, 0, sizeof at_obj, 0x5A), "four unknown ops: no fifth entry read");

    attack_reset(s5);
    wr16(gEffectKeyTable, 2, 0x0D0E);
    gApplyAttackCommands(at_obj, at_script);
    FCK_EQ(rd32(at_obj, 0), (u32)gEffectKeyTable, "0x26 index 0: entry = table base");
    FCK_EQ(rd16(at_obj, 0x20), 0x0D0E, "0x26 index 0: +0x20 = entry 0 +0x02");
}

/* 80062874: objects +0x08 active, +0x24 flags, +0x5C z, +0x60 y, +0x64 x,
 * +0x68 half-size. Coarse cells 1<<17 (5x5 window), 4x4 sub-cells 1<<15
 * with bit 15 - (4*row + col), cell map in 1<<15 units. */
static u8 gv_obj[9][0x6C];
static void *gv_l1[8], *gv_l2[8];
static u8 gv_map[32 * 32];

static void gv_set(s32 i, s32 active, s32 flags, s32 x, s32 z, s32 r)
{
    lk_fill(gv_obj[i], 0, 0x6C);
    wr32(gv_obj[i], 0x08, active);
    wr32(gv_obj[i], 0x24, flags);
    wr32(gv_obj[i], 0x5C, z);
    wr32(gv_obj[i], 0x60, 0x7777);
    wr32(gv_obj[i], 0x64, x);
    wr32(gv_obj[i], 0x68, r);
}

static void gv_reset(void)
{
    s32 i;
    lk_fill(gCommandList, 0, sizeof gCommandList);
    lk_fill(gCoverage1, 0, sizeof gCoverage1);
    lk_fill(gCoverage2, 0, sizeof gCoverage2);
    for (i = 25; i < 36; i++) {
        gCoverage1[4 * i] = 0xFFFF;
        gCoverage2[4 * i] = 0xFFFF;
    }
    lk_fill(gCellNodes, 0xA5, sizeof gCellNodes);
    lk_fill(gv_l1, 0xA5, sizeof gv_l1);
    lk_fill(gv_l2, 0xA5, sizeof gv_l2);
    lk_fill(gv_map, 0, sizeof gv_map);
    gList1 = gv_l1; gList2 = gv_l2; gCellMap = gv_map;
    gOrigin1X = 0; gOrigin1Y = 0; gOrigin2X = 0; gOrigin2Y = 0;
    gViewAnchors[0] = 0x200000; gViewAnchors[1] = 0x200000;
    gViewAnchors[2] = 0x200000; gViewAnchors[3] = 0x200000;
    gCellClipRect[0] = 0; gCellClipRect[1] = 0;
    gCellClipRect[2] = 32; gCellClipRect[3] = 32;
    gCount1 = 0; gCount2 = 0; gViewCount = 0x55;
}

static void gv_list(s32 n)
{
    s32 i;
    gCommandList[0] = n;
    for (i = 0; i < n; i++) gCommandList[2 + i] = (u32)gv_obj[i];
}

static void test_dual_view(void)
{
    /* @leaf 80062874 source src/game/80062874.c: per active object nearest square point to each view anchor, 5x5 coarse window (1<<17) minus origin, 4x4 sub-cell bit 15-(4r+c); view 1 appends on bit hit, view 2 appends whenever the cell is nonzero, 256 caps skip the object; flags 4 excludes, hit or 0x100 links into the 1<<15 cell map (nodes from 1, next = previous head) */
    LK_FUNC("func_80062874");

    /* scenario 1: view 1 hits, linking, flags 4 / 0x100 / inactive */
    gv_reset();
    gViewAnchors[0] = 0x28000; gViewAnchors[1] = 0x48000;
    gCoverage1[4 * 11] = 0x0400;            /* cell (row 2, col 1), sub (1,1) */
    gv_set(0, 1, 0, 0x28000, 0x48000, 0x1000);      /* A hit */
    gv_set(1, 0, 0x100, 0x28000, 0x48000, 0x1000);  /* B inactive */
    gv_set(2, 1, 0, 0x28000, 0x48000, 0);           /* C hit, same map cell */
    gv_set(3, 1, 4, 0x28000, 0x48000, 0);           /* D hit, flags 4 */
    gv_set(4, 1, 0x100, 0x80000, 0x8000, 0);        /* E no hit, 0x100 */
    gv_set(5, 1, 0, 0x80000, 0x8000, 0);            /* F no hit */
    gv_set(6, 1, 0, 0x30000, 0x48000, 0);           /* G cell set, sub bit clear */
    gv_list(7);
    gCount1 = 2; gCount2 = 3;
    gBuildDualViewObjectListsAndGrid();
    FCK_EQ(gCount1, 5, "view 1: three hits appended after 2");
    FCK_CHECK(gv_l1[2] == gv_obj[0], "view 1 list: A");
    FCK_CHECK(gv_l1[3] == gv_obj[2], "view 1 list: C");
    FCK_CHECK(gv_l1[4] == gv_obj[3], "view 1 list: D (flags 4 still listed)");
    FCK_EQ((u32)gv_l1[5], 0xA5A5A5A5u, "view 1 list: G not appended");
    FCK_EQ(gCount2, 3, "view 2: empty grid, count kept");
    FCK_EQ(gViewCount, 4, "three nodes linked, next index 4");
    FCK_EQ(gv_map[9 * 32 + 5], 2, "A/C cell (5, 9) head = node 2");
    FCK_EQ(gCellNodes[2], (u32)gv_obj[0], "node 1 = A");
    FCK_EQ(gCellNodes[3], 0, "node 1 next = null");
    FCK_EQ(gCellNodes[4], (u32)gv_obj[2], "node 2 = C");
    FCK_EQ(gCellNodes[5], (u32)&gCellNodes[2], "node 2 next = node 1");
    FCK_EQ(gv_map[1 * 32 + 16], 3, "E cell (16, 1) head = node 3");
    FCK_EQ(gCellNodes[6], (u32)gv_obj[4], "node 3 = E");
    FCK_EQ(gCellNodes[7], 0, "node 3 next = null");
    FCK_EQ(gCellNodes[8], 0xA5A5A5A5u, "node 4 unused");
    FCK_EQ(gv_map[9 * 32 + 6], 0, "G cell (6, 9) not linked");

    /* scenario 2: view 2 with origins, clip rect offset */
    gv_reset();
    gOrigin2X = 1; gOrigin2Y = 1;
    gViewAnchors[2] = 0x48000; gViewAnchors[3] = 0x48000;
    gCellClipRect[0] = 8; gCellClipRect[1] = 8;
    gCellClipRect[2] = 4; gCellClipRect[3] = 4;
    gCoverage2[4 * 6] = 0x8000;             /* cell (row 1, col 1), sub (0,0) */
    gv_set(0, 1, 0, 0x48000, 0x48000, 0x100);       /* G: cell set, own bit clear */
    gv_set(1, 1, 0, 0x40000, 0x40000, 0x100);       /* H: clamped +r, sub (0,0) */
    gv_set(2, 1, 0x100, 0x10000, 0x10000, 0);       /* I: left of both windows */
    gv_list(3);
    gBuildDualViewObjectListsAndGrid();
    FCK_EQ(gCount2, 2, "view 2: G and H appended");
    FCK_CHECK(gv_l2[0] == gv_obj[0], "view 2 list: G (cell nonzero, no hit)");
    FCK_CHECK(gv_l2[1] == gv_obj[1], "view 2 list: H");
    FCK_EQ(gCount1, 0, "view 1 untouched");
    FCK_EQ(gViewCount, 2, "only H linked");
    FCK_EQ(gv_map[0], 1, "H at clip-relative (0, 0)");
    FCK_EQ(gCellNodes[2], (u32)gv_obj[1], "node 1 = H");
    FCK_CHECK(lk_all(gv_map, 1, sizeof gv_map, 0), "G and I not linked");

    /* scenario 3: caps */
    gv_reset();
    gViewAnchors[0] = 0x28000; gViewAnchors[1] = 0x48000;
    gViewAnchors[2] = 0x28000; gViewAnchors[3] = 0x48000;
    gCoverage1[4 * 11] = 0x0400;
    gCoverage2[4 * 11] = 0x0400;
    gv_set(0, 1, 0x100, 0x28000, 0x48000, 0);
    gv_list(1);
    gCount1 = 256; gCount2 = 0;
    gBuildDualViewObjectListsAndGrid();
    FCK_EQ(gCount1, 256, "cap 1: count kept");
    FCK_EQ(gCount2, 0, "cap 1: view 2 skipped as well");
    FCK_EQ(gViewCount, 1, "cap 1: not linked despite 0x100");
    gCount1 = 0; gCount2 = 256;
    gBuildDualViewObjectListsAndGrid();
    FCK_EQ(gCount1, 1, "cap 2: view 1 still appended");
    FCK_EQ(gCount2, 256, "cap 2: count kept");
    FCK_EQ(gViewCount, 1, "cap 2: not linked");
    gCount1 = 0; gCount2 = 0;
    gBuildDualViewObjectListsAndGrid();
    FCK_EQ(gCount2, 1, "below caps: view 2 appended");
    FCK_EQ(gViewCount, 2, "below caps: linked");

    /* window edges: relative row 5 / col 5 are outside (row 5 would read
     * the guard cells, col 5 would alias cell 5 = row 1 col 0), (4, 4) is
     * the last cell */
    gv_reset();
    gCoverage1[4 * 5] = 0xFFFF;
    gCoverage2[4 * 5] = 0xFFFF;
    gCoverage1[4 * 24] = 0x8000;
    gv_set(0, 1, 0, 0x00000, 0xA0000, 0);           /* row 5, col 0 */
    gv_set(1, 1, 0, 0xA0000, 0x00000, 0);           /* row 0, col 5 */
    gv_set(2, 1, 0, 0x80000, 0x80000, 0);           /* row 4, col 4, sub (0,0) */
    gv_list(3);
    gBuildDualViewObjectListsAndGrid();
    FCK_EQ(gCount1, 1, "edges: only (4, 4) hits view 1");
    FCK_CHECK(gv_l1[0] == gv_obj[2], "edges: view 1 list holds the (4, 4) object");
    FCK_EQ(gCount2, 0, "edges: view 2 ignores rows/cols 5");
    gv_reset();
    gViewAnchors[0] = 0;
    gViewAnchors[2] = 0;
    gCoverage2[4 * 5] = 0xFFFF;
    gOrigin2X = 1; gOrigin2Y = 1;
    gv_set(0, 1, 0, 0x20000, 0xC0000, 0);           /* view 2 row 5 */
    gv_set(1, 1, 0, 0xC0000, 0x20000, 0);           /* view 2 col 5 */
    gv_set(2, 1, 0, 0x00000, 0x20000, 0);           /* view 2 col -1 */
    gv_list(3);
    gBuildDualViewObjectListsAndGrid();
    FCK_EQ(gCount2, 0, "edges with origin 1: rows/cols 5 and -1 ignored");
    FCK_EQ(gCount1, 0, "edges with origin 1: view 1 grid empty");

    /* empty list still publishes the counters */
    gv_reset();
    gv_list(0);
    gCount1 = 9; gCount2 = 8;
    gBuildDualViewObjectListsAndGrid();
    FCK_EQ(gViewCount, 1, "empty list: view count 1");
    FCK_EQ(gCount1, 9, "empty list: count 1 kept");
    FCK_EQ(gCount2, 8, "empty list: count 2 kept");
}

/* 8006BF44: DR_TPAGE (Psy-Q setDrawTPage: tag len 1, GP0(E1h) word
 * 0xE1000000 | dtd<<9 | dfe<<10 | (tpage & 0x9FF)) followed by a SPRT
 * (Psy-Q setSprt: tag len 4, code 0x64; setSemiTrans sets bit 1 -> 0x66;
 * psx-spx GP0(64h..67h): colour+cmd, yx, clut|v|u, h|w). */
static u8 si_pool[0x1C * 3];

static void test_status_icon(void)
{
    u8 *p, *q;

    /* @leaf 8006BF44 spec Psy-Q libgpu setDrawTPage/_get_mode (0xE1000000, dtd bit 9, dfe bit 10, tpage & 0x9FF) and setSprt/setSemiTrans (len 4, code 0x64|2); psx-spx GP0(E1h) and GP0(64h) word layout; icon UV (0xA6, 0x9C+16*slot or 0x96/0xBC), 16x16 at (x, y-16), blink 0xC0/0xA0/0xA0 vs grey 0x60 pinned from src/game/8006BF44.c */
    LK_FUNC("func_8006BF44");

    lk_fill(si_pool, 0xA5, sizeof si_pool);
    gPacketCursor = si_pool;
    gPacketLimit = si_pool + sizeof si_pool;
    gEnvMapTexpageWord[0] = 0x7FC0FFFFu;    /* clut 0x7FC0, tpage bits all set */
    gFrameClock = 0;
    gEmitStatusSlotIcon(2, 1, 40, 100);
    p = si_pool; q = si_pool + 0x1C;
    FCK_CHECK(gPacketCursor == si_pool + 0x38, "two packets of 0x1C");
    FCK_EQ(rd32(p, 0), 0x01000000, "tpage: tag len 1, link 0");
    FCK_EQ(rd32(p, 4), 0xE1000FFFu, "tpage: 0xE1 | dfe 0x400 | dtd 0x200 | (0xFFFF & 0x9FF)");
    FCK_EQ(rd32(q, 0), 0x04000000, "sprt: tag len 4, link 0");
    FCK_EQ(rd8(q, 7), 0x66, "sprt: GP0 0x64 + semi-transparent");
    FCK_EQ(rd8(q, 4), 0x60, "no blink request: grey R");
    FCK_EQ(rd8(q, 5), 0x60, "grey G");
    FCK_EQ(rd8(q, 6), 0x60, "grey B");
    FCK_EQ(rs16(q, 8), 40, "x");
    FCK_EQ(rs16(q, 0xA), 84, "y = 100 - 16");
    FCK_EQ(rd8(q, 0xC), 0xA6, "slot icon u");
    FCK_EQ(rd8(q, 0xD), 0xBC, "slot 2: v = 0x9C + 32");
    FCK_EQ(rd16(q, 0xE), 0x7FC0, "clut = texture word >> 16");
    FCK_EQ(rd16(q, 0x10), 16, "w 16");
    FCK_EQ(rd16(q, 0x12), 16, "h 16");
    FCK_CHECK(lk_all(si_pool, 0x38, sizeof si_pool, 0xA5), "third slot untouched");

    /* blink: a1 and frame bit 0x40; slot -1 uses the alternate icon */
    lk_fill(si_pool, 0xA5, sizeof si_pool);
    gPacketCursor = si_pool;
    gEnvMapTexpageWord[0] = 0x12340000u | 0x0160;
    gFrameClock = 0x40;
    gEmitStatusSlotIcon(-1, 1, -3, 5);
    q = si_pool + 0x1C;
    FCK_EQ(rd32(si_pool, 4), 0xE1000760u, "tpage keeps bits 5-8, forces 9-10");
    FCK_EQ(rd8(q, 4), 0xC0, "blink R");
    FCK_EQ(rd8(q, 5), 0xA0, "blink G");
    FCK_EQ(rd8(q, 6), 0xA0, "blink B");
    FCK_EQ(rd8(q, 0xC), 0x96, "slot -1: u 0x96");
    FCK_EQ(rd8(q, 0xD), 0xBC, "slot -1: v 0xBC");
    FCK_EQ(rs16(q, 8), -3, "negative x");
    FCK_EQ(rs16(q, 0xA), -11, "y = 5 - 16");
    FCK_EQ(rd16(q, 0xE), 0x1234, "clut");
    gPacketCursor = si_pool;
    gFrameClock = 0xBF;                    /* bit 0x40 clear */
    gEmitStatusSlotIcon(0, 1, 0, 0);
    FCK_EQ(rd8(q, 4), 0x60, "frame bit 0x40 clear: grey");
    FCK_EQ(rd8(q, 0xD), 0x9C, "slot 0: v 0x9C");
    gPacketCursor = si_pool;
    gFrameClock = 0x40;
    gEmitStatusSlotIcon(7, 0, 0, 0);
    FCK_EQ(rd8(q, 4), 0x60, "a1 0: grey despite frame bit");
    FCK_EQ(rd8(q, 0xD), 0x0C, "slot 7: 0x9C + 0x70 truncated to a byte");

    /* exhausted pool: both packets at the cursor, cursor kept */
    lk_fill(si_pool, 0xA5, sizeof si_pool);
    gPacketCursor = si_pool + 0x1C;
    gPacketLimit = si_pool + 0x1C;
    gEmitStatusSlotIcon(1, 0, 9, 9);
    FCK_CHECK(gPacketCursor == si_pool + 0x1C, "full: cursor unchanged");
    FCK_EQ(rd32(si_pool + 0x1C, 0), 0x04000000, "full: sprt overwrote the tpage packet");
    FCK_EQ(rd8(si_pool + 0x1C, 7), 0x66, "full: sprt code at the cursor");
    FCK_CHECK(lk_all(si_pool, 0, 0x1C, 0xA5), "full: nothing below the cursor");
}

/* 800752E0: object +0x04 position, +0x10 size, +0x15..+0x17 RGB, +0x1A
 * angle, +0x24 light; request of 0x30 bytes handed to func_8005A6A4. */
static u8 pl_obj[0x28], pl_light[0x10];

static void test_particle_light(void)
{
    /* @leaf 800752E0 source src/game/800752E0.c: request = position, kind 0x16, height y>>4, width = depth = size<<1, angle rand & 0xFFF (also stored at +0x1A), +0x1E/+0x20 0, +0x22 -1, scale -1, RGB; effect 0xA into +0x24; RGB mirrored into light +9..+11 when non-null */
    LK_FUNC("func_800752E0");

    lk_fill(pl_obj, 0x5A, sizeof pl_obj);
    wr32(pl_obj, 4, 0x1000); wr32(pl_obj, 8, -0x100); wr32(pl_obj, 0xC, 0x23456);
    wr16(pl_obj, 0x10, 0x9000);
    wr8(pl_obj, 0x15, 0x11); wr8(pl_obj, 0x16, 0x22); wr8(pl_obj, 0x17, 0x33);
    lk_fill(pl_light, 0xEE, sizeof pl_light);
    rand_ret = 0x12345; light_ret = pl_light; light_calls = 0;
    gSpawnParticleLight(pl_obj);
    FCK_EQ(light_calls, 1, "effect spawned once");
    FCK_EQ(light_id, 0xA, "effect id 0xA");
    FCK_CHECK(light_result_ptr == pl_obj + 0x24, "result slot = object +0x24");
    FCK_EQ(rd32(light_req, 0x08), 0x1000, "request x");
    FCK_EQ(rd32(light_req, 0x0C), (u32)-0x100, "request y");
    FCK_EQ(rd32(light_req, 0x10), 0x23456, "request z");
    FCK_EQ(rd8(light_req, 0x14), 0x16, "kind 0x16");
    FCK_EQ(rs16(light_req, 0x16), -0x10, "height = y >> 4 (arithmetic)");
    FCK_EQ(rs16(light_req, 0x18), 0x2000, "width = (0x9000 << 1) as s16");
    FCK_EQ(rs16(light_req, 0x1A), 0x2000, "depth = width");
    FCK_EQ(rs16(light_req, 0x1C), 0x345, "angle = rand & 0xFFF");
    FCK_EQ(rs16(pl_obj, 0x1A), 0x345, "angle stored in the object");
    FCK_EQ(rs16(light_req, 0x1E), 0, "+0x1E = 0");
    FCK_EQ(rs16(light_req, 0x20), 0, "+0x20 = 0");
    FCK_EQ(rd8(light_req, 0x22), 0xFF, "+0x22 = -1");
    FCK_EQ(rd8(light_req, 0x23), 0x11, "request R");
    FCK_EQ(rd8(light_req, 0x24), 0x22, "request G");
    FCK_EQ(rd8(light_req, 0x25), 0x33, "request B");
    FCK_EQ(rs16(light_req, 0x28), -1, "scale -1");
    FCK_CHECK(rd32(pl_obj, 0x24) == (u32)pl_light, "light pointer stored");
    FCK_EQ(rd8(pl_light, 9), 0x11, "light R mirrored");
    FCK_EQ(rd8(pl_light, 10), 0x22, "light G mirrored");
    FCK_EQ(rd8(pl_light, 11), 0x33, "light B mirrored");
    FCK_EQ(rd8(pl_light, 8), 0xEE, "light +8 untouched");
    FCK_EQ(rd8(pl_light, 12), 0xEE, "light +12 untouched");
    FCK_EQ(rd8(pl_obj, 0x18), 0x5A, "object +0x18 untouched");

    /* no light returned */
    lk_fill(pl_light, 0xEE, sizeof pl_light);
    light_ret = 0; rand_ret = 0xFFFFF000u;
    wr16(pl_obj, 0x10, 3);
    gSpawnParticleLight(pl_obj);
    FCK_EQ(rd32(pl_obj, 0x24), 0, "null light stored");
    FCK_CHECK(lk_all(pl_light, 0, sizeof pl_light, 0xEE), "null light: nothing mirrored");
    FCK_EQ(rs16(pl_obj, 0x1A), 0, "rand 0xFFFFF000 & 0xFFF = 0");
    FCK_EQ(rs16(light_req, 0x18), 6, "width 3 << 1");
}

static void test_options_preview(void)
{
    /* @leaf 8007D150 source src/game/8007D150.c: only message 3 stops the preview voice handle D_8009F91C (when nonzero) and clears it */
    LK_FUNC("func_8007D150");

    gSnapshotPointer = 0x1234; stop_calls = 0;
    gStopOptionsPreview(2);
    FCK_EQ(stop_calls, 0, "message 2: no stop");
    FCK_EQ(gSnapshotPointer, 0x1234, "message 2: handle kept");
    gStopOptionsPreview(4);
    FCK_EQ(stop_calls, 0, "message 4: no stop");
    gStopOptionsPreview(3);
    FCK_EQ(stop_calls, 1, "message 3: stop once");
    FCK_EQ(stop_arg, 0x1234, "message 3: handle passed");
    FCK_EQ(stop_seen_ptr, 0x1234, "handle cleared after the stop call");
    FCK_EQ(gSnapshotPointer, 0, "message 3: handle cleared");
    gStopOptionsPreview(3);
    FCK_EQ(stop_calls, 1, "message 3, handle 0: no stop");
    gSnapshotPointer = -1;
    gStopOptionsPreview(3);
    FCK_EQ(stop_arg, -1, "negative handle is nonzero");
    FCK_EQ(gSnapshotPointer, 0, "negative handle cleared");
}

/* 8007EA78: synthetic alphabets: even positions use hex table
 * 'P'..'A' (digit j = 'A'+15-j), odd positions octal table 'Q'..'X'; the
 * buffer holds char - 0x41 and a digit decodes as (j - position) & mask.
 * The passwords below were encoded by hand from the chosen nibble vectors
 * b[0..9] (comment), using the field map of the unit's header:
 * n0 = b1&3 | (b3&3)<<2, n1 = (b2>>2)&3 | (b8&3)<<2, n2 = b4, n3 = b6,
 * n4 = (b0>>2)&3 | (b5<<1)&0xC; flags f1 b1.2, f2 b5.0, f3 b7.2, f4 b7.0,
 * f5 b7.1, f6 b9.0, f7 b3.2; checks b0&3, (b9>>1)&3, b2&3|b8&0xC. */
static void pw_reset(const u8 *pw)
{
    s32 i;
    for (i = 0; i < 16; i++) gHexDigitTable[i] = (u8)(0x41 + 15 - i);
    for (i = 0; i < 8; i++) gOctalDigitTable[i] = (u8)(0x51 + i);
    for (i = 0; i < 10; i++) gPasswordBuffer[i] = pw[i];
    lk_fill(gWideTextTable, 0xA5, sizeof gWideTextTable);
    gFrontEndListTable[0] = 3; gFrontEndListTable[1] = 0; gFrontEndListTable[2] = 7;
    gFrontEndListTable[3] = 12; gFrontEndListTable[4] = 5;
    lk_fill(session, 0x77, sizeof session);
    gSessionPtr = session;
    wr32(session, 0x5C, 0);
    gFrontEndListFlag = 0x77; gPlayerWrapLimit = 0x77; gPausedMenuFlag = 0x77;
    gLockedMenuFlag = 0x77; gSavedCrimeWarProgress = 0x77; gCrimeWarProgress = 0x77;
    gSelectedMissionIndex = 0x77; gMissionListMinIndex = 0x77; gMissionListMaxIndex = 0x77;
    reset_calls();
}

static s32 pw_untouched(void)
{
    return gFrontEndListFlag == 0x77 && gPlayerWrapLimit == 0x77 &&
           gPausedMenuFlag == 0x77 && gLockedMenuFlag == 0x77 &&
           gCrimeWarProgress == 0x77 && gSavedCrimeWarProgress == 0x77 &&
           gMissionListMaxIndex == 0x77 &&
           lk_all(gWideTextTable, 0, sizeof gWideTextTable, 0xA5) &&
           lk_all(session, 0x8C, 0x98, 0x77) && ncalls == 0;
}

#define WT(e, off) rd8(gWideTextTable, (e) * 0x24 + (off))

static void test_password(void)
{
    /* b = 8,5,13,5,4,3,2,1,8,3: n = 5,3,4,2,6,0, flags 1101011 (f1..f7), sums 20/25 */
    static const u8 pw1[10] = { 0x07, 0x16, 0x00, 0x10, 0x07, 0x10, 0x07, 0x10, 0x0F, 0x14 };
    /* b = 9,3,5,2,12,4,10,0,7,2: n1 13, n2 12, n3 10, n4 10 -> n5 5, n1 = n2 = 10; n0 11 -> 8; sum 53 */
    static const u8 pw2[10] = { 0x06, 0x14, 0x08, 0x15, 0x0F, 0x11, 0x0F, 0x17, 0x00, 0x13 };
    /* b = 9,3,9,0,12,4,10,0,14,2: n1 exactly 10, n2 12, n3 10, n4 10 -> n5 2; sum 45 */
    static const u8 pw8[10] = { 0x06, 0x14, 0x04, 0x13, 0x0F, 0x11, 0x0F, 0x17, 0x09, 0x13 };
    /* pw1 with b9 = 1 (second check 0 != 25&3) */
    static const u8 pw3[10] = { 0x07, 0x16, 0x00, 0x10, 0x07, 0x10, 0x07, 0x10, 0x0F, 0x12 };
    /* pw1 with b8 = 12 (third check 13 != 25&15) */
    static const u8 pw4[10] = { 0x07, 0x16, 0x00, 0x10, 0x07, 0x10, 0x07, 0x10, 0x0B, 0x14 };
    /* pw1 with an 'A' at odd position 3 (not an octal digit) */
    static const u8 pw5[10] = { 0x07, 0x16, 0x00, 0x00, 0x07, 0x10, 0x07, 0x10, 0x0F, 0x14 };
    /* pw1 with a 'Q' at even position 0 (not a hex digit) */
    static const u8 pw6[10] = { 0x10, 0x16, 0x00, 0x10, 0x07, 0x10, 0x07, 0x10, 0x0F, 0x14 };
    /* pw1 with b0 = 9 (first check 1 != 20&3) */
    static const u8 pw7[10] = { 0x06, 0x16, 0x00, 0x10, 0x07, 0x10, 0x07, 0x10, 0x0F, 0x14 };
    s32 r;

    /* @leaf 8007EA78 source src/game/8007EA78.c: 10-digit decode ((j - i) & 7 odd / & 0xF even through the digit tables, stop on a miss), nibble field map, 10/10/10/10 promotion of n2+n1-20 into n5, clamps 8/10, three checksums (sum&3, sum+flags &3, &0xF), flag-gated session/menu writes, level table +0x1F/+0x20, PA vs CW mission list with max index min(progress, 7) */
    LK_FUNC("func_8007EA78");

    pw_reset(pw1);
    r = gApplyPasswordUnlocks();
    FCK_EQ(r, 1, "pw1 accepted");
    FCK_EQ(rd32(session, 0x8C), 1, "f1: session +0x8C = 1");
    FCK_EQ(rd32(session, 0x90), 1, "f2: session +0x90 = 1");
    FCK_EQ(rd32(session, 0x94), 0, "f2: session +0x94 = f3 (0)");
    FCK_EQ(gFrontEndListFlag, 1, "f7 -> D_8009F3C0");
    FCK_EQ(gPlayerWrapLimit, 1, "f4 -> D_8009F3C1");
    FCK_EQ(gPausedMenuFlag, 0x77, "f5 clear: D_8009F3C3 untouched");
    FCK_EQ(gLockedMenuFlag, 1, "f6 -> D_8009F3C4");
    FCK_EQ(gCrimeWarProgress, 5, "progress n0 = 5");
    FCK_EQ(gSavedCrimeWarProgress, 5, "saved progress n0");
    FCK_EQ(WT(3, 0x1F), 3, "level list[0] (entry 3) = n1");
    FCK_EQ(WT(0, 0x1F), 4, "level list[1] (entry 0) = n2");
    FCK_EQ(WT(7, 0x1F), 2, "level list[2] (entry 7) = n3");
    FCK_EQ(WT(12, 0x1F), 6, "level list[3] (entry 12) = n4");
    FCK_EQ(WT(5, 0x1F), 0, "level list[4] (entry 5) = n5");
    FCK_EQ(WT(3, 0x20), 3, "entry 3 +0x20 mirrors +0x1F");
    FCK_EQ(WT(12, 0x20), 6, "entry 12 +0x20");
    FCK_EQ(WT(5, 0x20), 0, "entry 5 +0x20");
    FCK_EQ(WT(1, 0x1F), 0xA5, "entry 1 untouched");
    FCK_EQ(WT(3, 0x1E), 0xA5, "entry 3 +0x1E untouched");
    FCK_EQ(WT(3, 0x21), 0xA5, "entry 3 +0x21 untouched");
    FCK_EQ(ncalls, 3, "refresh, CW list, unlocks");
    FCK_EQ(calls[0].id, C_PW_REFRESH, "refresh first");
    FCK_EQ(calls[1].id, C_PW_CW, "CW list (session +0x5C != 1)");
    FCK_EQ(pw_cw_seen, 5, "progress set before the list setup");
    FCK_EQ(calls[2].id, C_PW_UNLOCK, "unlocks last");
    FCK_EQ(calls[2].a[0], 5, "mission bounds set before unlocks");
    FCK_EQ(gMissionListMaxIndex, 5, "max index = progress 5");
    FCK_EQ(gMissionListMinIndex, 0, "min index 0");
    FCK_EQ(gSelectedMissionIndex, 5, "selected = max");

    pw_reset(pw2);
    wr32(session, 0x5C, 1);
    r = gApplyPasswordUnlocks();
    FCK_EQ(r, 1, "pw2 accepted");
    FCK_EQ(gCrimeWarProgress, 8, "n0 11 clamped to 8");
    FCK_EQ(WT(3, 0x1F), 10, "promotion: n1 = 10");
    FCK_EQ(WT(0, 0x1F), 10, "promotion: n2 = 10");
    FCK_EQ(WT(7, 0x1F), 10, "n3 = 10");
    FCK_EQ(WT(12, 0x1F), 10, "n4 = 10");
    FCK_EQ(WT(5, 0x1F), 5, "promotion: n5 = 12 + 13 - 20");
    FCK_EQ(gFrontEndListFlag, 0x77, "no flags: D_8009F3C0 untouched");
    FCK_EQ(gLockedMenuFlag, 0x77, "no flags: D_8009F3C4 untouched");
    FCK_EQ(rd32(session, 0x8C), 0x77777777u, "no flags: session untouched");
    FCK_EQ(calls[1].id, C_PW_PA, "session +0x5C == 1: PA list");
    FCK_EQ(gMissionListMaxIndex, 0x77, "PA: mission bounds untouched");
    pw_reset(pw2);
    r = gApplyPasswordUnlocks();
    FCK_EQ(gMissionListMaxIndex, 7, "CW: progress 8 capped at 7");
    FCK_EQ(gSelectedMissionIndex, 7, "CW: selected 7");
    FCK_EQ(gCrimeWarProgress, 8, "CW: progress itself stays 8");

    pw_reset(pw8);
    r = gApplyPasswordUnlocks();
    FCK_EQ(r, 1, "pw8 accepted (promotion at n1 == 10)");
    FCK_EQ(WT(5, 0x1F), 2, "promotion: n5 = 12 + 10 - 20");
    FCK_EQ(WT(0, 0x1F), 10, "promotion: n2 = 10");
    FCK_EQ(gCrimeWarProgress, 3, "pw8 progress 3");

    pw_reset(pw3);
    FCK_EQ(gApplyPasswordUnlocks(), 0, "second checksum mismatch: 0");
    FCK_CHECK(pw_untouched(), "second checksum mismatch: no side effect");
    pw_reset(pw4);
    FCK_EQ(gApplyPasswordUnlocks(), 0, "third checksum mismatch: 0");
    FCK_CHECK(pw_untouched(), "third checksum mismatch: no side effect");
    pw_reset(pw7);
    FCK_EQ(gApplyPasswordUnlocks(), 0, "first checksum mismatch: 0");
    FCK_CHECK(pw_untouched(), "first checksum mismatch: no side effect");
    pw_reset(pw5);
    FCK_EQ(gApplyPasswordUnlocks(), 0, "non-octal digit at odd position: 0");
    FCK_CHECK(pw_untouched(), "bad digit: no side effect");
    pw_reset(pw6);
    FCK_EQ(gApplyPasswordUnlocks(), 0, "non-hex digit at even position: 0");
    FCK_CHECK(pw_untouched(), "bad hex digit: no side effect");
}

/* 80082B0C: POLY_FT4 (Psy-Q setPolyFT4: tag len 9, code 0x2C; psx-spx
 * GP0(2Ch): colour+cmd, xy0, clut|v0|u0, xy1, tpage|v1|u1, xy2, v2|u2, xy3,
 * v3|u3; vertex order TL, TR, BL, BR). Atlas table: +2 count, entries of
 * 12 bytes at +8: tpage, u, v, w, h, clut. */
static u8 at_table[8 + 12 * 3], at_pool[0x34 * 3];

static void test_atlas_sprite(void)
{
    u8 *p;

    /* @leaf 80082B0C spec Psy-Q libgpu setPolyFT4 (len 9, code 0x2C) and psx-spx GP0(2Ch) word layout/vertex order; tag low 24 bits from the caller; anchors (flagX 4 centre / 2 right, flagY 0x20 centre / 0x10 bottom), -1 size from the entry and UV extent from the entry's low size bytes pinned from src/game/80082B0C.c */
    LK_FUNC("func_80082B0C");

    lk_fill(at_table, 0, sizeof at_table);
    wr16(at_table, 2, 3);
    p = at_table + 8;                                     /* entry 0 */
    wr16(p, 0, 0x1234); wr8(p, 2, 0x10); wr8(p, 4, 0x20);
    wr16(p, 6, 0x18); wr16(p, 8, 0x0C); wr16(p, 0xA, 0x7FC0);
    p = at_table + 8 + 12 * 2;                            /* entry 2 */
    wr16(p, 0, 0x0055); wr8(p, 2, 0xF0); wr8(p, 4, 0xF8);
    wr16(p, 6, 0x118); wr16(p, 8, 0x010); wr16(p, 0xA, 0x4000);
    gAtlasTable = at_table;
    lk_fill(at_pool, 0xA5, sizeof at_pool);
    gPrimNext = at_pool;
    gPrimEnd = at_pool + sizeof at_pool;

    p = gDrawAtlasSprite(0, 100, 50, -1, -1, 0xFFABCDEF, 0x11, 0x22, 0x33, 0, 0);
    FCK_CHECK(p == at_pool, "returns the pool primitive");
    FCK_CHECK(gPrimNext == at_pool + 0x34, "cursor advanced 0x34");
    FCK_EQ(rd32(p, 0), 0x09ABCDEF, "tag: len 9 | (tag & 0xFFFFFF)");
    FCK_EQ(rd8(p, 7), 0x2C, "GP0 0x2C");
    FCK_EQ(rd8(p, 4), 0x11, "R");
    FCK_EQ(rd8(p, 5), 0x22, "G");
    FCK_EQ(rd8(p, 6), 0x33, "B");
    FCK_EQ(rs16(p, 0x08), 100, "v0 x (TL)");
    FCK_EQ(rs16(p, 0x0A), 50, "v0 y");
    FCK_EQ(rs16(p, 0x10), 124, "v1 x (TR) = x + entry w 0x18");
    FCK_EQ(rs16(p, 0x12), 50, "v1 y");
    FCK_EQ(rs16(p, 0x18), 100, "v2 x (BL)");
    FCK_EQ(rs16(p, 0x1A), 62, "v2 y = y + entry h 0x0C");
    FCK_EQ(rs16(p, 0x20), 124, "v3 x (BR)");
    FCK_EQ(rs16(p, 0x22), 62, "v3 y");
    FCK_EQ(rd8(p, 0x0C), 0x10, "u0");
    FCK_EQ(rd8(p, 0x0D), 0x20, "v0");
    FCK_EQ(rd16(p, 0x0E), 0x7FC0, "clut in word 3");
    FCK_EQ(rd8(p, 0x14), 0x28, "u1 = u + w");
    FCK_EQ(rd8(p, 0x15), 0x20, "v1");
    FCK_EQ(rd16(p, 0x16), 0x1234, "tpage in word 5");
    FCK_EQ(rd8(p, 0x1C), 0x10, "u2");
    FCK_EQ(rd8(p, 0x1D), 0x2C, "v2 = v + h");
    FCK_EQ(rd8(p, 0x24), 0x28, "u3");
    FCK_EQ(rd8(p, 0x25), 0x2C, "v3");

    /* centre anchors with explicit size; UV extent still from the entry */
    p = gDrawAtlasSprite(0, 100, 50, 40, 31, 0, 1, 2, 3, 4, 0x20);
    FCK_CHECK(p == at_pool + 0x34, "second primitive");
    FCK_EQ(rs16(p, 0x08), 80, "centre x: 100 - 40/2");
    FCK_EQ(rs16(p, 0x10), 120, "centre x2");
    FCK_EQ(rs16(p, 0x0A), 35, "centre y: 50 - 31/2");
    FCK_EQ(rs16(p, 0x1A), 66, "centre y2 = 35 + 31");
    FCK_EQ(rd8(p, 0x14), 0x28, "explicit size: UV from the entry");
    FCK_EQ(rd32(p, 0), 0x09000000, "tag 0");

    /* right/bottom anchors; entry 2: u/v wrap, w 0x118 uses low byte for UV */
    p = gDrawAtlasSprite(2, 300, 200, -1, -1, 0x123, 0, 0, 0, 2, 0x10);
    FCK_EQ(rs16(p, 0x08), 300 - 0x118, "right anchor: x = x - w (halfword w)");
    FCK_EQ(rs16(p, 0x10), 300, "right anchor: x2 = x");
    FCK_EQ(rs16(p, 0x0A), 200 - 0x10, "bottom anchor: y = y - h");
    FCK_EQ(rs16(p, 0x22), 200, "bottom anchor: y2 = y");
    FCK_EQ(rd8(p, 0x14), 0x08, "u1 = 0xF0 + 0x18 (low byte of 0x118) wraps");
    FCK_EQ(rd8(p, 0x1D), 0x08, "v2 = 0xF8 + 0x10 wraps");
    FCK_EQ(rd16(p, 0x16), 0x0055, "entry 2 tpage");
    FCK_EQ(rd16(p, 0x0E), 0x4000, "entry 2 clut");
    /* other anchor codes act as left/top */
    p = gDrawAtlasSprite(0, 10, 10, -1, -1, 0, 0, 0, 0, 1, 8);
    FCK_EQ(rs16(p, 0x08), 10, "flagX 1: left");
    FCK_EQ(rs16(p, 0x1A), 22, "flagY 8: top");

    /* rejects: no table, idx < 0, idx == count */
    gPrimNext = at_pool;
    lk_fill(at_pool, 0xA5, sizeof at_pool);
    gAtlasTable = 0;
    FCK_CHECK(gDrawAtlasSprite(0, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0) == 0, "no table: null");
    gAtlasTable = at_table;
    FCK_CHECK(gDrawAtlasSprite(-1, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0) == 0, "idx -1: null");
    FCK_CHECK(gDrawAtlasSprite(3, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0) == 0, "idx == count: null");
    FCK_CHECK(gPrimNext == at_pool, "rejects: cursor unchanged");
    FCK_CHECK(lk_all(at_pool, 0, sizeof at_pool, 0xA5), "rejects: pool untouched");
    FCK_CHECK(gDrawAtlasSprite(2, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0) == at_pool, "idx count-1 accepted");

    /* exhausted pool */
    gPrimNext = at_pool + 0x34;
    gPrimEnd = at_pool + 0x34;
    p = gDrawAtlasSprite(0, 0, 0, -1, -1, 0, 0, 0, 0, 0, 0);
    FCK_CHECK(p == at_pool + 0x34, "full: primitive at the cursor");
    FCK_CHECK(gPrimNext == at_pool + 0x34, "full: cursor unchanged");
}

int main(void)
{
    test_resupply();
    test_chase();
    test_light();
    test_anim_script();
    test_hud_bar();
    test_marker_colors();
    test_path_nodes();
    test_sprite_desc();
    test_attack_cmds();
    test_dual_view();
    test_status_icon();
    test_particle_light();
    test_options_preview();
    test_password();
    test_atlas_sprite();
    return lk_done("leaf_gameplay");
}
