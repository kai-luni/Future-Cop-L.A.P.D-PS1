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
static s32 sel_calls, sel_args[4];
static void *sel_track;
void *st_select_clip(void *track, s32 index, s32 dir, u8 flags) __asm__("func_800448D8");
void *st_select_clip(void *track, s32 index, s32 dir, u8 flags)
{
    sel_calls++;
    sel_track = track;
    sel_args[0] = index; sel_args[1] = dir; sel_args[2] = flags;
    return 0;
}
static s32 rm_calls, rm_idx[8];
/* Removes command i from the 4-entry (op,arg) list at +0x0C by shifting the
 * following entries down and zeroing the last one. */
void st_remove_cmd(u8 *script, s32 i) __asm__("func_8002A5B4");
void st_remove_cmd(u8 *script, s32 i)
{
    s32 k;
    if (rm_calls < 8) rm_idx[rm_calls] = i;
    rm_calls++;
    for (k = i; k < 3; k++) {
        script[0xC + 2 * k] = script[0xC + 2 * k + 2];
        script[0xD + 2 * k] = script[0xD + 2 * k + 2];
    }
    script[0xC + 6] = 0;
    script[0xD + 6] = 0;
}

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
static u8 an_script[0x14], an_track[0x14];

static void anim_reset(const u8 *cmds)
{
    s32 i;
    lk_fill(an_script, 0xCC, 0x0C);
    for (i = 0; i < 8; i++) an_script[0xC + i] = cmds[i];
    lk_fill(an_track, 0x11, sizeof an_track);
    sel_calls = 0; rm_calls = 0;
}

static void test_anim_script(void)
{
    static const u8 s1[8] = { 0x32, 5, 0x99, 1, 0x34, 7, 0, 0 };
    static const u8 s2[8] = { 0x33, 0xC8, 0, 0, 0x32, 1, 0, 0 };
    static const u8 s3[8] = { 0x10, 1, 0x20, 2, 0x31, 3, 0x35, 4 };
    static const u8 s4[8] = { 0, 0, 0x32, 1, 0x32, 2, 0x32, 3 };

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
    FCK_EQ(rm_idx[0], 0, "first removal at 0");
    FCK_EQ(rm_idx[1], 1, "0x34 removed at 1 after the shift (0x99 kept at 0)");
    FCK_EQ(rd8(an_track, 0x13), 0x02, "0x34: flag bit 1 set");
    FCK_EQ(rs16(an_track, 0x0C), 7, "0x34: frame = 7");
    FCK_EQ(an_script[0xC], 0x99, "unknown op stays in the list");
    FCK_EQ(an_script[0xD], 1, "unknown op arg kept");
    FCK_EQ(an_script[0xE], 0, "list terminated after it");

    anim_reset(s2);
    wr8(an_track, 0x13, 0xFF);
    gProcessAnimScriptCommands(an_script, an_track);
    FCK_EQ(rd8(an_track, 0x13), 0xFD, "0x33: only flag bit 1 cleared");
    FCK_EQ(rs16(an_track, 0x0C), 200, "0x33: arg 0xC8 is unsigned -> frame 200");
    FCK_EQ(rm_calls, 1, "0x33 removed, then op 0 stops");
    FCK_EQ(sel_calls, 0, "0x32 behind the terminator not run");

    anim_reset(s3);
    gProcessAnimScriptCommands(an_script, an_track);
    FCK_EQ(rm_calls, 0, "four unknown ops: nothing removed");
    FCK_EQ(sel_calls, 0, "four unknown ops: nothing selected");
    FCK_CHECK(lk_all(an_track, 0, sizeof an_track, 0x11), "four unknown ops: track untouched");

    anim_reset(s4);
    gProcessAnimScriptCommands(an_script, an_track);
    FCK_EQ(sel_calls, 0, "leading op 0: nothing runs");
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

int main(void)
{
    test_resupply();
    test_chase();
    test_light();
    test_anim_script();
    test_hud_bar();
    return lk_done("leaf_gameplay");
}
