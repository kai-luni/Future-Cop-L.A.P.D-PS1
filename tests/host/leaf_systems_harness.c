/* Animation stepping, memory-card format, the entity field query, map-zone
 * loading, spawn-chain marking, anchor node sync, the weapon-sound pool, the
 * path-curve offsets, looping-path advance, hover movement, the options sound
 * toggle, the rectangle draw list and the two SPU attribute setters -
 * single-object units from src/game/ exercised on synthetic RAM under
 * qemu-mipsel.
 *
 * Discipline: every fixture a check depends on is written before the call, and
 * every D_ label the units read is defined here - nothing relies on retail
 * initial memory or on BSS being zero. Callees are stand-ins bound to their
 * retail label, so a wrong binding is a link error; a stand-in only records its
 * arguments and returns a value the test configured, it never invents game
 * behaviour. The three exceptions are documented where they occur:
 *
 *   - func_8001E4C0 (memory-card fill) and func_8001AB78 (vector scale) are
 *     semantic stand-ins: the fill really writes the byte, the scaler really
 *     does nothing, so the checks state exactly which part of the unit is
 *     observed. Every other stand-in records only.
 *   - the five command builders behind 80093CE8 are linked for real, so their
 *     packet words come from the real E1h/E2h/E3h/E4h/E5h/E6h encoding and the
 *     expected literals are hand-decoded from the PSX GPU packet layout.
 *   - expected values are hand-worked from the decompiled bodies (shifts,
 *     masks, 16.16 fixed-point products, struct offsets on little-endian MIPS);
 *     each check's trailing comment spells the arithmetic it encodes.
 *
 * Skipped: 8005AB8C. Both of its branches read fixed kseg0 addresses baked
 * into the instruction stream as `lui 0x800B`/`lui 0x800A` constants (0x800A8EA6
 * the CLUT word, 0x8009AFC0 the palette source, 0x800A6F60 the palette table).
 * Host binaries are linked at 0x00400000, so nothing this harness defines can
 * live at those addresses: a fixture written there would be emulator RAM that
 * happens to answer, exactly the retail-memory dependency the suite forbids.
 * The unit stays untested until its table bases become symbols.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"
/* @mock sound_mock */
/* func_80046C84 (SE trigger) and func_800451E0 (stereo mode getter) come from
 * the shared mock library; tests configure and inspect fcm_sound.*. */
#include "sound_mock.h"

/* Units under test, under their retail labels. */
void gStepAnimations(s32 dt) __asm__("func_800136C8");
s32 gPrepareCardFormat(void) __asm__("func_80020B64");
struct Entity;
s32 gTestEntityFieldMatch(u8 arg0, s16 arg1, struct Entity *arg2, u8 arg3) __asm__("func_8002F8B8");
void gLoadMapZone(void *obj) __asm__("func_800319D4");
void gMarkSpawnRecordChains(s32 row) __asm__("func_8003C590");
void gSyncAttachedNodes(void) __asm__("func_8003D240");
void gRefreshPlayerWeaponSound(void *object, s32 state) __asm__("func_80046CF0");
void gApplyPathCurveOffsets(void *obj, s32 input, s32 *vec) __asm__("func_8004FFD0");
s32 gAdvanceLoopingPathPosition(void *path, s32 *pos, s32 budget, s32 *counter) __asm__("func_800513D0");
void gBuildHoverMovementVector(void *a0, s32 a1, s32 a2, s32 *a3, s32 a4) __asm__("func_800537D0");
void gToggleSoundEnable(s32 arg0) __asm__("func_8007D27C");
void gBuildRectDrawList(u8 *obj, u8 *rect) __asm__("func_80093CE8");
void gSpuSetCommonAttributes(void *attr) __asm__("func_80098FE8");
void gSpuSetVoiceAttributesOne(s32 ch, void *attr) __asm__("func_80099C58");

/* --------------------------------------------------------------- recorder */

#define LS_CAP 96

enum {
    LS_WAVE = 1,    /* func_800135C0 animation wave pass            */
    LS_SINCOS,      /* func_8001846C sine/cosine                   */
    LS_RAND,        /* func_80019668 pseudo-random word            */
    LS_FILL,        /* func_8001E4C0 memory fill                   */
    LS_CARDINIT,    /* func_80020B28 replacement frames            */
    LS_BUSY,        /* func_8001F180 card manager busy             */
    LS_FIND,        /* func_80019C38 tagged record lookup          */
    LS_ALLOC,       /* func_80017EBC block allocation              */
    LS_FREE,        /* func_800181D4 block free                    */
    LS_SURF,        /* func_80067684 surface height query          */
    LS_FLAGPTR,     /* func_8003B3D8 spawn type flag word          */
    LS_ORFLAG,      /* func_8003C4C4 or grid cell flags            */
    LS_CYCLE,       /* func_8003C500 mark spawn cycle              */
    LS_POINT,       /* func_8003D0F0 resolve model effect point    */
    LS_FREEBUF,     /* func_8003CF20 free owned buffer node        */
    LS_RESOLVE,     /* func_80045494 resolve sound id              */
    LS_PLAY,        /* func_80046F9C play standing sound           */
    LS_SETMODE,     /* func_800451B4 set stereo mode               */
    LS_SCALE,       /* func_8001AB78 scale vector in place         */
    LS_DIST,        /* func_8001A13C distance between points       */
    LS_SEP,         /* func_80053730 two-player separation         */
    LS_ENCODE,      /* func_800999F4 encode/write SPU address      */
    LS_NOTE2PITCH   /* func_80097808 sample note to pitch          */
};

struct LsCall { int id; s32 a0, a1, a2, a3, a4, a5; };

static struct LsCall ls_calls[LS_CAP];
static int ls_n;

static void ls_rec(int id, s32 a0, s32 a1, s32 a2, s32 a3, s32 a4, s32 a5)
{
    if (ls_n >= LS_CAP) return;              /* overflow is a test bug */
    ls_calls[ls_n].id = id;
    ls_calls[ls_n].a0 = a0;
    ls_calls[ls_n].a1 = a1;
    ls_calls[ls_n].a2 = a2;
    ls_calls[ls_n].a3 = a3;
    ls_calls[ls_n].a4 = a4;
    ls_calls[ls_n].a5 = a5;
    ls_n++;
}

static void ls_reset(void)
{
    int i;
    ls_n = 0;
    for (i = 0; i < LS_CAP; i++) {
        ls_calls[i].id = 0;
        ls_calls[i].a0 = ls_calls[i].a1 = ls_calls[i].a2 = 0;
        ls_calls[i].a3 = ls_calls[i].a4 = ls_calls[i].a5 = 0;
    }
}

static int ls_count(int id)
{
    int i, n = 0;
    for (i = 0; i < ls_n; i++) if (ls_calls[i].id == id) n++;
    return n;
}

/* Index of the nth (0-based) call with this id, -1 when absent. */
static int ls_idx(int id, int nth)
{
    int i;
    for (i = 0; i < ls_n; i++) {
        if (ls_calls[i].id == id) {
            if (nth-- == 0) return i;
        }
    }
    return -1;
}

static struct LsCall *ls_call(int id, int nth)
{
    int i = ls_idx(id, nth);
    return (i < 0) ? (struct LsCall *)0 : &ls_calls[i];
}

/* Id of the nth recorded call in time order, 0 when out of range. */
static int ls_id(int nth)
{
    return (nth < 0 || nth >= ls_n) ? 0 : ls_calls[nth].id;
}

static u32 ls_w32(const u8 *p, int off)
{
    return (u32)p[off] | ((u32)p[off + 1] << 8)
         | ((u32)p[off + 2] << 16) | ((u32)p[off + 3] << 24);
}

static s32 ls_s32(const u8 *p, int off) { return (s32)ls_w32(p, off); }

/* ======================================================== 800136C8 stepping */

/* Node: +0x00 next, +0x04 pad, +0x08 entry count, +0x0C entries[]. Entry is
 * 16 bytes: +0 max, +1 cur, +2 type, +3 flags, +4 rate, +6 timer, +8 src,
 * +0xC dst. */
#define LS_ANIM_ENTRIES 3

struct LsAnimEntry {
    u8 max; u8 cur; u8 type; u8 flags;
    s16 rate; s16 timer;
    u16 *src; u16 *dst;
};
struct LsAnimNode {
    struct LsAnimNode *next;
    s32 pad04;
    s32 count;
    struct LsAnimEntry e[LS_ANIM_ENTRIES];
};

struct LsAnimNode *ls_anim_nodes __asm__("D_8009F564");
struct LsAnimNode *ls_anim_waves __asm__("D_8009F568");
u32 ls_frame_time __asm__("D_8009FAA4");

static s32 ls_rand_value;
static s32 ls_sin_value, ls_cos_value;
static u32 ls_sincos_angle;

void ls_stub_wave(void *list, u32 frame) __asm__("func_800135C0");
void ls_stub_sincos(u32 angle, s32 *s, s32 *c) __asm__("func_8001846C");
s32 ls_stub_rand(void) __asm__("func_80019668");

void ls_stub_wave(void *list, u32 frame)
{
    ls_rec(LS_WAVE, (s32)list, (s32)frame, 0, 0, 0, 0);
}

void ls_stub_sincos(u32 angle, s32 *s, s32 *c)
{
    ls_rec(LS_SINCOS, (s32)angle, 0, 0, 0, 0, 0);
    ls_sincos_angle = angle;
    *s = ls_sin_value;
    *c = ls_cos_value;
}

s32 ls_stub_rand(void)
{
    ls_rec(LS_RAND, 0, 0, 0, 0, 0, 0);
    return ls_rand_value;
}

static u16 ls_anim_src[512];
static u16 ls_anim_dst[8];

/* @leaf 800136C8 source src/game/800136C8.c: type<16 entries count down timer by dt, reload rate and advance cur by type (0 forward, 1 backward, 2 ping-pong toggling flag 2, 3 random % max), types >=16 run a 15-bit angle clock scaled by dt*rate>>3 through sincos and quantise to max; a changed entry copies 3 (or 4 with flag 1) u16 from src + cur*6 / cur*8 */
static void ls_test_anim(void)
{
    struct LsAnimNode node;
    struct LsAnimNode wave;
    struct LsAnimEntry *e;
    int i;

    for (i = 0; i < 512; i++) ls_anim_src[i] = (u16)(0x1000 + i);
    for (i = 0; i < 8; i++) ls_anim_dst[i] = 0xEEEE;

    LK_FUNC("func_800136C8");

    /* empty world: one wave pass with the frame clock, nothing else */
    ls_reset();
    ls_anim_nodes = 0;
    ls_anim_waves = &wave;
    ls_frame_time = 0x00010002u;
    gStepAnimations(7);
    FCK_EQ(ls_count(LS_WAVE), 1, "wave pass run once");
    FCK_EQ((s32)ls_call(LS_WAVE, 0)->a0, (s32)&wave, "wave list argument");
    FCK_EQ((s32)ls_call(LS_WAVE, 0)->a1, 0x00010002, "frame clock argument");
    FCK_EQ(ls_n, 1, "no other callee on an empty list");

    /* type 0, timer expires, flag 1 selects the 8-byte source stride */
    node.next = 0; node.count = 1;
    e = &node.e[0];
    e->max = 4; e->cur = 1; e->type = 0; e->flags = 1;
    e->rate = 20; e->timer = 5; e->src = ls_anim_src; e->dst = ls_anim_dst;
    ls_anim_nodes = &node;
    ls_reset();
    gStepAnimations(10);
    FCK_EQ(e->timer, 15, "timer 5-10 reloaded by rate 20 -> 15");
    FCK_EQ(e->cur, 1 + 1, "cur advanced 1 -> 2");
    FCK_EQ(ls_anim_dst[0], ls_anim_src[8], "dst[0] = src[cur*8 bytes = 16 -> word 8]");
    FCK_EQ(ls_anim_dst[1], ls_anim_src[9], "dst[1] = next word");
    FCK_EQ(ls_anim_dst[2], ls_anim_src[10], "dst[2] = next word");
    FCK_EQ(ls_anim_dst[3], ls_anim_src[11], "flag 1 copies the fourth word");

    /* type 0, timer survives: no copy at all */
    for (i = 0; i < 8; i++) ls_anim_dst[i] = 0xEEEE;
    e->max = 4; e->cur = 1; e->type = 0; e->flags = 0;
    e->rate = 20; e->timer = 50;
    ls_reset();
    gStepAnimations(10);
    FCK_EQ(e->timer, 40, "timer 50-10 without reload");
    FCK_EQ(e->cur, 1, "cur untouched");
    FCK_CHECK(lk_all(ls_anim_dst, 0, 8, 0xEE), "no copy while the timer runs");

    /* type 0 wraps past max back to 0, flag 0 selects the 6-byte stride */
    e->max = 2; e->cur = 1; e->type = 0; e->flags = 0;
    e->rate = 1; e->timer = 1;
    ls_reset();
    gStepAnimations(10);
    FCK_EQ(e->timer, -8, "timer 1-10+1");
    FCK_EQ(e->cur, 0, "cur 1 -> 2 == max -> 0");
    FCK_EQ(ls_anim_dst[0], ls_anim_src[0], "wrapped copy from cur*6 = 0");
    FCK_EQ(ls_anim_dst[3], (u16)0xEEEE, "flag 0 leaves the fourth word");

    /* type 1 counts backwards, 0 wraps to max-1 */
    e->max = 3; e->cur = 0; e->type = 1; e->flags = 0;
    e->rate = 1; e->timer = 1;
    ls_reset();
    gStepAnimations(5);
    FCK_EQ(e->timer, -3, "timer 1-5+1");
    FCK_EQ(e->cur, 2, "cur 0 -> max 3 -> 2");
    FCK_EQ(ls_anim_dst[0], ls_anim_src[6], "copy from cur*6 bytes = 12 -> word 6");

    e->max = 3; e->cur = 2; e->type = 1; e->timer = 1; e->rate = 1;
    ls_reset();
    gStepAnimations(5);
    FCK_EQ(e->cur, 1, "cur 2 -> 1");

    /* type 2 ping-pong: forward step inside the range keeps flag clear */
    e->max = 4; e->cur = 2; e->type = 2; e->flags = 0;
    e->rate = 1; e->timer = 1;
    ls_reset();
    gStepAnimations(5);
    FCK_EQ(e->cur, 3, "cur 2 -> 3, not yet max");
    FCK_EQ(e->flags, 0, "no direction flip inside the range");
    FCK_EQ(ls_anim_dst[0], ls_anim_src[9], "copy from cur*6 bytes = 18 -> word 9");

    /* hitting max from the forward side backs off to max-2 and flips flag 2 */
    e->max = 4; e->cur = 3; e->type = 2; e->flags = 0;
    e->rate = 1; e->timer = 1;
    ls_reset();
    gStepAnimations(5);
    FCK_EQ(e->cur, 2, "cur 3 -> 4 == max -> max-2");
    FCK_EQ(e->flags, 2, "flag 2 set on the forward turn");

    /* reverse side: flag 2 set, 0 -> 1 and the flag clears at the bottom */
    e->max = 4; e->cur = 0; e->type = 2; e->flags = 2;
    e->rate = 1; e->timer = 1;
    ls_reset();
    gStepAnimations(5);
    FCK_EQ(e->cur, 1, "reverse from 0 -> 1");
    FCK_EQ(e->flags, 0, "flag 2 cleared after the low turn");

    e->max = 4; e->cur = 2; e->type = 2; e->flags = 2;
    e->rate = 1; e->timer = 1;
    ls_reset();
    gStepAnimations(5);
    FCK_EQ(e->cur, 1, "reverse 2 -> 1");
    FCK_EQ(e->flags, 2, "mid-range reverse keeps flag 2");

    /* type 3 picks rand % max */
    ls_rand_value = 1000;
    e->max = 7; e->cur = 0; e->type = 3; e->flags = 0;
    e->rate = 1; e->timer = 1;
    ls_reset();
    gStepAnimations(5);
    FCK_EQ(ls_count(LS_RAND), 1, "one random draw");
    FCK_EQ(e->cur, 6, "1000 % 7 = 6");

    /* flag 4 skips the entry completely */
    e->max = 4; e->cur = 2; e->type = 0; e->flags = 4;
    e->rate = 1; e->timer = 1;
    for (i = 0; i < 8; i++) ls_anim_dst[i] = 0xEEEE;
    ls_reset();
    gStepAnimations(100);
    FCK_EQ(e->timer, 1, "skipped entry keeps its timer");
    FCK_EQ(e->cur, 2, "skipped entry keeps cur");
    FCK_CHECK(lk_all(ls_anim_dst, 0, 8, 0xEEEE), "skipped entry copies nothing");

    /* type 0x11: sine path, angle 0+16, sin 0 -> (0x10000*100)>>17 = 50 */
    ls_sin_value = 0; ls_cos_value = 0;
    e->max = 100; e->cur = 0; e->type = 0x11; e->flags = 1;
    e->rate = 8; e->timer = 0;
    ls_reset();
    gStepAnimations(16);
    FCK_EQ(ls_count(LS_SINCOS), 1, "one trig lookup");
    FCK_EQ((s32)ls_sincos_angle, 2, "angle 16 >> 3 = 2");
    FCK_EQ(e->timer, 16, "angle clock 0+16");
    FCK_EQ(e->cur, 50, "(0+0x10000)*100>>17 = 50");
    FCK_EQ(ls_anim_dst[0], ls_anim_src[200], "copy from cur*8 bytes = 400 -> word 200");
    FCK_EQ(ls_anim_dst[3], ls_anim_src[203], "flag 1 copies four words");

    /* any other type >=16 takes the cosine output and masks the angle to 14 bits:
     * 0x3FF0 + 16 = 0x4000, 0x4000 & 0x3FFF = 0, cos 0x8000 ->
     * (0x8000+0x10000)*100>>17 = 75 */
    ls_sin_value = 0; ls_cos_value = 0x8000;
    e->max = 100; e->cur = 0; e->type = 0x12; e->flags = 0;
    e->rate = 8; e->timer = 0x3FF0;
    ls_reset();
    gStepAnimations(16);
    FCK_EQ((s32)ls_sincos_angle, 0, "(0x3FF0+16) & 0x3FFF = 0, >>3 = 0");
    FCK_EQ(e->timer, 0x4000, "15-bit clock keeps 0x4000");
    FCK_EQ(e->cur, 75, "(0x8000+0x10000)*100>>17 = 75");

    /* a full-scale sine clamps to max-1 */
    ls_sin_value = 0x10000;
    e->max = 100; e->cur = 0; e->type = 0x11; e->flags = 0;
    e->rate = 8; e->timer = 0;
    ls_reset();
    gStepAnimations(16);
    FCK_EQ(e->cur, 99, "(0x10000+0x10000)*100>>17 = 100 -> 99");

    /* dt is the same register for both entry kinds: the trig entry rewrites it
     * (16*16>>3 = 32) before the counter entry sees it, so timer 20 - 32 = -12
     * reloads by rate 1 to -11 */
    node.count = 2;
    node.e[0].max = 100; node.e[0].cur = 0; node.e[0].type = 0x11;
    node.e[0].flags = 0; node.e[0].rate = 16; node.e[0].timer = 0;
    node.e[0].src = ls_anim_src; node.e[0].dst = ls_anim_dst;
    node.e[1].max = 4; node.e[1].cur = 1; node.e[1].type = 0;
    node.e[1].flags = 0; node.e[1].rate = 1; node.e[1].timer = 20;
    node.e[1].src = ls_anim_src; node.e[1].dst = ls_anim_dst;
    ls_sin_value = 0; ls_cos_value = 0;
    ls_reset();
    gStepAnimations(16);
    FCK_EQ(node.e[0].timer, 32, "trig clock 0 + 16*16>>3");
    FCK_EQ(node.e[0].cur, 50, "sin 0 -> 50");
    FCK_EQ(node.e[1].timer, -11, "counter entry uses the scaled dt: 20-32+1");
    FCK_EQ(node.e[1].cur, 2, "couner entry still advances");

    /* entry loop stops at count: the third entry stays untouched */
    node.count = 1;
    node.e[1].timer = 77;
    ls_reset();
    gStepAnimations(16);
    FCK_EQ(node.e[1].timer, 77, "entry beyond count untouched");
}

/* ==================================================== 80020B64 card format */

/* The unit works on the operation state block at D_8009AB08: the format
 * records start at +0x9B0, 0x1200 bytes are cleared, and sixteen 0x80-byte
 * records are stamped before the 0x11B0 end marker. */
u8 *ls_card_state __asm__("D_8009AB08");
static u8 ls_card[0x2300];
static s32 ls_card_busy;

void ls_stub_fill(void *dst, int value, unsigned int count) __asm__("func_8001E4C0");
void ls_stub_cardinit(void *records) __asm__("func_80020B28");
s32 ls_stub_busy(void) __asm__("func_8001F180");

/* Semantic stand-in: the BIOS fill really writes the byte, which is exactly
 * what the unit relies on (the cleared region is the fixture). */
void ls_stub_fill(void *dst, int value, unsigned int count)
{
    u8 *p = (u8 *)dst;
    ls_rec(LS_FILL, (s32)dst, value, (s32)count, 0, 0, 0);
    while (count--) *p++ = (u8)value;
}

void ls_stub_cardinit(void *records)
{
    ls_rec(LS_CARDINIT, (s32)records, 0, 0, 0, 0, 0);
}

s32 ls_stub_busy(void)
{
    ls_rec(LS_BUSY, 0, 0, 0, 0, 0, 0);
    return ls_card_busy;
}

/* @leaf 80020B64 source src/game/80020B64.c: clears 0x1200 bytes at +0x9B0, stamps sixteen 0x80-byte records (record 0 "MC" head, the rest 0xA0 with an FFFF word at +8) each closed by an XOR of its first 0x7F bytes at +0x7F, hands the table to 80020B28, and only when the manager is idle publishes +0x998 = 1, +0x99C = 0x23, +0x910 = record 1, +0x8F8 = 5 and four zero words */
static void ls_test_card_format(void)
{
    u8 *rec0 = ls_card + 0x9B0;
    u8 *rec1 = ls_card + 0xA30;
    u8 *rec2 = ls_card + 0xAB0;
    u8 *rec15 = ls_card + 0x9B0 + 15 * 0x80;

    LK_FUNC("func_80020B64");

    /* busy manager: the records are still written, the request words are not */
    lk_fill(ls_card, 0xEE, (int)sizeof(ls_card));
    ls_card_state = ls_card;
    ls_card_busy = 1;
    ls_reset();
    FCK_EQ(gPrepareCardFormat(), 1, "busy manager returns 1");
    FCK_EQ(ls_count(LS_FILL), 1, "one clear");
    FCK_EQ((s32)ls_call(LS_FILL, 0)->a0, (s32)rec0, "clear starts at the records");
    FCK_EQ(ls_call(LS_FILL, 0)->a1, 0, "cleared with a zero byte");
    FCK_EQ(ls_call(LS_FILL, 0)->a2, 0x1200, "clear covers 0x1200 bytes");
    FCK_EQ((s32)ls_call(LS_CARDINIT, 0)->a0, (s32)rec0, "replacement frames get the table");
    FCK_EQ(ls_id(0), LS_FILL, "clear runs first");
    FCK_EQ(ls_id(1), LS_CARDINIT, "then the replacement frames");
    FCK_EQ(ls_id(2), LS_BUSY, "then the manager probe");
    FCK_EQ(ls_s32(ls_card, 0x8F8), 0xEEEEEEEE, "busy path leaves +0x8F8 alone");
    FCK_EQ(ls_s32(ls_card, 0x998), 0xEEEEEEEE, "busy path leaves +0x998 alone");

    /* idle manager: records plus the request publication */
    lk_fill(ls_card, 0xEE, (int)sizeof(ls_card));
    ls_card_busy = 0;
    ls_reset();
    FCK_EQ(gPrepareCardFormat(), 0, "idle manager returns 0");
    FCK_EQ(rec0[0], 0x4D, "record 0 head 'M'");
    FCK_EQ(rec0[1], 0x43, "record 0 head 'C'");
    FCK_EQ(rec0[0x7F], 0x4D ^ 0x43, "checksum 0x4D^0x43 = 0x0E");
    FCK_EQ(*(u16 *)(rec0 + 8), 0, "record 0 carries no FFFF word");
    FCK_CHECK(lk_all(rec0, 2, 0x7F, 0), "record 0 body cleared");
    FCK_EQ(rec1[0], 0xA0, "record 1 free marker 0xA0");
    FCK_EQ(*(u16 *)(rec1 + 8), 0xFFFF, "record 1 end word FFFF");
    FCK_EQ(rec1[0x7F], 0xA0 ^ 0xFF ^ 0xFF, "checksum 0xA0^0xFF^0xFF = 0xA0");
    FCK_EQ(rec2[0], 0xA0, "record 2 free marker");
    FCK_EQ(rec2[0x7F], 0xA0, "record 2 checksum");
    FCK_EQ(rec15[0], 0xA0, "record 15 free marker");
    FCK_EQ(*(u16 *)(rec15 + 8), 0xFFFF, "record 15 end word");
    FCK_EQ(rec15[0x7F], 0xA0, "record 15 checksum");
    FCK_CHECK(lk_all(ls_card, 0x9B0 + 0x800, 0x9B0 + 0x1200, 0),
              "bytes past the sixteenth record stay cleared");
    FCK_CHECK(lk_all(ls_card, 0, 0x8F8, 0xEE), "the clear does not touch the state words before +0x8F8");
    FCK_CHECK(lk_all(ls_card, 0x1BB0, 0x1BB4, 0xEE), "nothing written past +0x1BB0");
    FCK_EQ(ls_s32(ls_card, 0x998), 1, "+0x998 request flag");
    FCK_EQ(ls_s32(ls_card, 0x99C), 0x23, "+0x99C command 0x23");
    FCK_EQ(ls_s32(ls_card, 0x910), (s32)rec1, "+0x910 points at record 1");
    FCK_EQ(ls_s32(ls_card, 0x90C), 0, "+0x90C cleared");
    FCK_EQ(ls_s32(ls_card, 0x994), 0, "+0x994 cleared");
    FCK_EQ(ls_s32(ls_card, 0x9A4), 0, "+0x9A4 cleared");
    FCK_EQ(ls_s32(ls_card, 0x1BB4), 0, "+0x1BB4 cleared");
    FCK_EQ(ls_s32(ls_card, 0x8F8), 5, "+0x8F8 retry count 5");

    /* the same object can be formatted again: the busy probe comes last, so a
     * second idle call rewrites an already stamped table */
    ls_reset();
    FCK_EQ(gPrepareCardFormat(), 0, "second call still returns 0");
    FCK_EQ(rec0[0x7F], 0x0E, "checksum unchanged by a rewrite");
    FCK_EQ(ls_count(LS_FILL), 1, "one clear per call");
}

/* ============================================== 8002F8B8 entity field query */

/* Entity fields the unit reads: +0x14 u16, +0x16 u8 kind, +0x17/+0x18 u8,
 * +0xAC pointer to a block whose +0x40 word is tested, +0xC8 partner. */
struct LsEcInner { u8 pad00[0x40]; s32 unk40; };
struct LsEcEntity {
    u8 pad00[0x14];
    u16 unk14;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 pad19[0x93];
    struct LsEcInner *unkAC;
    u8 padB0[0x18];
    struct LsEcEntity *unkC8;
};

/* The unit iterates from D_8009FA58 to the address D_8009FA60, i.e. over the
 * two entity pointers stored there. Both labels are one 8-byte array in the
 * retail image, so the harness materialises exactly that shape. */
__asm__("\t.data\n"
        "\t.balign 8\n"
        "\t.globl D_8009FA58\n"
        "D_8009FA58:\n"
        "\t.word 0\n"
        "\t.word 0\n"
        "\t.globl D_8009FA60\n"
        "D_8009FA60:\n"
        "\t.word 0\n"
        "\t.text\n");

extern struct LsEcEntity *ls_player_slots[] __asm__("D_8009FA58");

static struct LsEcEntity ls_ec_self, ls_ec_other;
static struct LsEcInner ls_ec_self_in, ls_ec_other_in;

static void ls_ec_init(struct LsEcEntity *e, u8 kind, u16 w14, u8 w17, u8 w18)
{
    lk_fill(e, 0, (int)sizeof(*e));
    e->unk14 = w14;
    e->unk16 = kind;
    e->unk17 = w17;
    e->unk18 = w18;
    e->unkAC = 0;
    e->unkC8 = 0;
}

/* @leaf 8002F8B8 source src/game/8002F8B8.c: selector 8 maps kind 0x12 to 1 and every other kind to itself, 1/3/4 take +0x14/+0x17/+0x18, 2 aborts on kind 0x15, 5 tests bit (1<<+0x18) of the argument, 7 needs kind 2 and a partner of kind 0x12; on a match the filter either returns 1 or walks both player slots, skipping the caller itself when it is kind 1 and failing on any other slot whose +0xAC->+0x40 is non-zero */
static void ls_test_entity_field_match(void)
{
    LK_FUNC("func_8002F8B8");

    /* selector 1: raw kind (+0x16) comparison */
    ls_ec_init(&ls_ec_self, 5, 7, 0, 0);
    FCK_EQ(gTestEntityFieldMatch(1, 5, &ls_ec_self, 0), 1, "sel 1 matches the kind");
    FCK_EQ(gTestEntityFieldMatch(1, 6, &ls_ec_self, 0), 0, "sel 1 rejects");

    /* selector 8: kind 0x12 reports as 1 */
    ls_ec_init(&ls_ec_self, 0x12, 0, 0, 0);
    FCK_EQ(gTestEntityFieldMatch(8, 1, &ls_ec_self, 0), 1, "kind 0x12 queries as 1");
    FCK_EQ(gTestEntityFieldMatch(8, 0x12, &ls_ec_self, 0), 0, "kind 0x12 is not 0x12");
    ls_ec_init(&ls_ec_self, 0x11, 0, 0, 0);
    FCK_EQ(gTestEntityFieldMatch(8, 0x11, &ls_ec_self, 0), 1, "other kinds pass through");

    /* selector 2: kind 0x15 vetoes the +0x14 comparison */
    ls_ec_init(&ls_ec_self, 0x15, 9, 0, 0);
    FCK_EQ(gTestEntityFieldMatch(2, 9, &ls_ec_self, 0), 0, "kind 0x15 returns 0");
    ls_ec_init(&ls_ec_self, 0x14, 9, 0, 0);
    FCK_EQ(gTestEntityFieldMatch(2, 9, &ls_ec_self, 0), 1, "kind 0x14 compares +0x14");

    /* selectors 3 and 4 */
    ls_ec_init(&ls_ec_self, 0, 0, 0x0A, 0x0B);
    FCK_EQ(gTestEntityFieldMatch(3, 0x0A, &ls_ec_self, 0), 1, "sel 3 matches +0x17");
    FCK_EQ(gTestEntityFieldMatch(4, 0x0B, &ls_ec_self, 0), 1, "sel 4 matches +0x18");

    /* selector 5 masks the argument with 1 << (+0x18): bit 0 of an even
     * argument is clear and 0 == 0 matches; an odd argument leaves 1 */
    ls_ec_init(&ls_ec_self, 0, 0, 0, 0);
    FCK_EQ(gTestEntityFieldMatch(5, 0x10, &ls_ec_self, 0), 1, "even argument masks to 0");
    FCK_EQ(gTestEntityFieldMatch(5, 1, &ls_ec_self, 0), 0, "odd argument leaves 1");
    ls_ec_init(&ls_ec_self, 0, 0, 0, 3);
    FCK_EQ(gTestEntityFieldMatch(5, 0x08, &ls_ec_self, 0), 0, "bit 3 masked to 8 != 3");

    /* selector 7: kind 2 plus a partner of kind 0x12 */
    ls_ec_init(&ls_ec_self, 2, 0, 0, 0);
    ls_ec_init(&ls_ec_other, 0x12, 0, 0, 0);
    ls_ec_self.unkC8 = &ls_ec_other;
    FCK_EQ(gTestEntityFieldMatch(7, 0x99, &ls_ec_self, 0), 1, "kind 2 with a 0x12 partner");
    ls_ec_init(&ls_ec_other, 0x11, 0, 0, 0);
    FCK_EQ(gTestEntityFieldMatch(7, 0x99, &ls_ec_self, 0), 0, "partner of another kind");
    ls_ec_self.unkC8 = 0;
    FCK_EQ(gTestEntityFieldMatch(7, 0x99, &ls_ec_self, 0), 0, "no partner");
    ls_ec_init(&ls_ec_self, 3, 0, 0, 0);
    ls_ec_self.unkC8 = &ls_ec_other;
    FCK_EQ(gTestEntityFieldMatch(7, 0x99, &ls_ec_self, 0), 0, "kind 3 never matches 7");

    /* selector 0 falls into the uninitialised default: not observed here. */

    /* filter walk: a kind 5 caller checks both slots, the second one has a
     * live +0x40 */
    ls_ec_init(&ls_ec_self, 5, 3, 0, 0);
    ls_ec_init(&ls_ec_other, 5, 0, 0, 0);
    ls_ec_self_in.unk40 = 0;
    ls_ec_other_in.unk40 = 1;
    ls_ec_self.unkAC = &ls_ec_self_in;
    ls_ec_other.unkAC = &ls_ec_other_in;
    ls_player_slots[0] = &ls_ec_self;
    ls_player_slots[1] = &ls_ec_other;
    FCK_EQ(gTestEntityFieldMatch(1, 5, &ls_ec_self, 1), 0, "second slot blocks the match");
    ls_ec_other_in.unk40 = 0;
    FCK_EQ(gTestEntityFieldMatch(1, 5, &ls_ec_self, 1), 1, "both slots idle -> 1");

    /* a kind 1 caller only ever checks its own slot: other slots, including a
     * null one, are skipped before the +0x40 read */
    ls_ec_init(&ls_ec_self, 1, 3, 0, 0);
    ls_ec_init(&ls_ec_other, 5, 0, 0, 0);
    ls_ec_self.unkAC = &ls_ec_self_in;
    ls_ec_other.unkAC = &ls_ec_other_in;
    ls_ec_self_in.unk40 = 0;
    ls_ec_other_in.unk40 = 7;
    ls_player_slots[0] = 0;
    ls_player_slots[1] = &ls_ec_self;
    FCK_EQ(gTestEntityFieldMatch(1, 1, &ls_ec_self, 1), 1, "null slot and the caller itself are handled");
    ls_player_slots[0] = &ls_ec_other;
    FCK_EQ(gTestEntityFieldMatch(1, 1, &ls_ec_self, 1), 1, "a neighbour is not consulted for a kind 1 caller");

    /* the same neighbour blocks a kind 5 caller */
    ls_ec_self.unk16 = 5;
    FCK_EQ(gTestEntityFieldMatch(1, 5, &ls_ec_self, 1), 0, "the live neighbour blocks a kind 5 caller");

    /* the caller's own +0x40 is read as well */
    ls_ec_self_in.unk40 = 9;
    ls_player_slots[0] = 0;
    ls_player_slots[1] = &ls_ec_self;
    FCK_EQ(gTestEntityFieldMatch(1, 5, &ls_ec_self, 1), 0, "caller's own +0x40 blocks");

    /* filter off: the slots are not consulted at all */
    FCK_EQ(gTestEntityFieldMatch(1, 5, &ls_ec_self, 0), 1, "arg3 0 skips the walk");
    ls_player_slots[0] = 0;
    ls_player_slots[1] = 0;
}

/* ==================================================== 800319D4 map zone load */

/* Node written by the unit (0x28 bytes). */
struct LsZone {
    s16 f0, f2, f4;
    u8 pad06[2];
    s32 f8;
    s16 fC, fE, f10, f12;
    u8 f14, f15;
    s16 f16;
    s32 f18, f1C, f20;
    struct LsZone *f24;
};

/* Asset wrapper the unit reads: +0x00 blob pointer, +0x1C/+0x20 keys, +0x24
 * the word copied into the node. */
struct LsZoneObj {
    u8 *f0;
    u8 pad04[0x18];
    s32 f1C;
    s32 f20;
    s32 f24;
};

struct LsZone *ls_zone_list __asm__("D_8009F6F8");

static u8 ls_zone_record[0x40];
static u8 *ls_zone_find_ret;
static s32 ls_zone_surf;
static struct LsZone ls_zone_a, ls_zone_b;
static void *ls_alloc_ret;
static struct LsZoneObj ls_zone_obj;

void *ls_stub_alloc(s32 size, s32 tag) __asm__("func_80017EBC");
void ls_stub_free(void *block) __asm__("func_800181D4");
void *ls_stub_find(void *blob, s32 a1, s32 tag, s32 a3) __asm__("func_80019C38");
s32 ls_stub_surf(void *pos, s32 mode) __asm__("func_80067684");

void *ls_stub_alloc(s32 size, s32 tag)
{
    ls_rec(LS_ALLOC, size, tag, 0, 0, 0, 0);
    return ls_alloc_ret;
}

void ls_stub_free(void *block)
{
    ls_rec(LS_FREE, (s32)block, 0, 0, 0, 0, 0);
}

void *ls_stub_find(void *blob, s32 a1, s32 tag, s32 a3)
{
    ls_rec(LS_FIND, (s32)blob, a1, tag, a3, 0, 0);
    return ls_zone_find_ret;
}

s32 ls_stub_surf(void *pos, s32 mode)
{
    ls_rec(LS_SURF, (s32)pos, mode, 0, 0, 0, 0);
    return ls_zone_surf;
}

static void ls_zone_record_init(s32 tag, s32 x, s32 y, s32 z, u16 f14, u16 f16,
                                u16 f18, s8 f1A, u8 f1B, u16 f1C)
{
    lk_fill(ls_zone_record, 0x5A, (int)sizeof(ls_zone_record));
    *(s32 *)(ls_zone_record + 0x00) = tag;
    *(s32 *)(ls_zone_record + 0x04) = 0x04040404;
    *(s32 *)(ls_zone_record + 0x08) = x;
    *(s32 *)(ls_zone_record + 0x0C) = y;
    *(s32 *)(ls_zone_record + 0x10) = z;
    *(u16 *)(ls_zone_record + 0x14) = f14;
    *(u16 *)(ls_zone_record + 0x16) = f16;
    *(u16 *)(ls_zone_record + 0x18) = f18;
    *(u8 *)(ls_zone_record + 0x1A) = (u8)f1A;
    *(u8 *)(ls_zone_record + 0x1B) = f1B;
    *(u16 *)(ls_zone_record + 0x1C) = f1C;
}

/* @leaf 800319D4 source src/game/800319D4.c: looks the object up by its +0x20/+0x1C keys under tag 0x74414354, allocates a 0x28 node tagged 3 and prepends it to D_8009F6F8, seeding x/z from the object in 8.8, the record tag, a surface height queried in mode -1/-2/type-2 then scaled to 8.8, half the three u16 extents, the +0x1C word, +0x1B, a random byte and the -1/0 sentinels, then frees the object */
static void ls_test_map_zone(void)
{
    LK_FUNC("func_800319D4");

    ls_zone_record_init(0x11223344, 0x123400, 0x10, -0x200, 0x0080, 0x00C8,
                        0x0190, 0, 0x7E, 0x0BEE);
    ls_zone_find_ret = ls_zone_record + 4;
    ls_zone_surf = 0x12345;
    ls_rand_value = 0x1234ABCD;
    ls_zone_list = 0;
    ls_alloc_ret = &ls_zone_a;
    lk_fill(&ls_zone_obj, 0, (int)sizeof(ls_zone_obj));
    ls_zone_obj.f0 = ls_zone_record;
    ls_zone_obj.f24 = 0x11;
    ls_zone_obj.f1C = 0x2233;
    ls_zone_obj.f20 = 0x4455;
    ls_reset();
    gLoadMapZone(&ls_zone_obj);

    FCK_EQ((s32)ls_call(LS_FIND, 0)->a0, (s32)ls_zone_record, "lookup gets the blob");
    FCK_EQ(ls_call(LS_FIND, 0)->a1, 0x4455, "lookup key +0x20");
    FCK_EQ(ls_call(LS_FIND, 0)->a2, 0x74414354, "record tag 0x74414354");
    FCK_EQ(ls_call(LS_FIND, 0)->a3, 0x2233, "lookup key +0x1C");
    FCK_EQ(ls_call(LS_ALLOC, 0)->a0, 0x28, "node size 0x28");
    FCK_EQ(ls_call(LS_ALLOC, 0)->a1, 3, "node tag 3");
    FCK_EQ((s32)ls_call(LS_SURF, 0)->a0, (s32)(ls_zone_record + 8), "height from the record position +0x08");
    FCK_EQ(ls_call(LS_SURF, 0)->a1, -1, "type 0 queries in mode -1");
    FCK_EQ((s32)ls_call(LS_FREE, 0)->a0, (s32)&ls_zone_obj, "object freed last");
    FCK_EQ(ls_id(ls_n - 1), LS_FREE, "free is the final call");

    FCK_EQ(ls_zone_a.f8, 0x11223344, "record tag copied");
    FCK_EQ(ls_zone_a.f0, 0x1234, "x 0x123400 >> 8");
    FCK_EQ(ls_zone_a.f4, -2, "z -0x200 >> 8");
    FCK_EQ(ls_zone_a.f2, 0x123, "0x12345 >> 8 keeps the full word");
    FCK_EQ(ls_zone_a.fC, 0x64, "0x00C8 / 2");
    FCK_EQ(ls_zone_a.fE, 0xC8, "0x0190 / 2");
    FCK_EQ(ls_zone_a.f10, 0x40, "0x0080 / 2");
    FCK_EQ(ls_zone_a.f12, 0x0BEE, "+0x1C word");
    FCK_EQ(ls_zone_a.f14, 0x7E, "+0x14 = record +0x1B");
    FCK_EQ(ls_zone_a.f15, 0, "+0x15 always cleared");
    FCK_EQ(ls_zone_a.f16, 0xCD, "random word & 0xFF");
    FCK_EQ(ls_zone_a.f18, 0x11, "object +0x24 copied");
    FCK_EQ(ls_zone_a.f1C, 0, "+0x1C cleared");
    FCK_EQ(ls_zone_a.f20, -1, "+0x20 sentinel -1");
    FCK_EQ((s32)ls_zone_a.f24, 0, "first node has no successor");
    FCK_EQ((s32)ls_zone_list, (s32)&ls_zone_a, "list head published");

    /* a second node is prepended, modes follow the record type */
    ls_zone_record_init(0x55667788, -0x100, 0, 0x100, 0xFFFE, 0x0002, 0x0004,
                        1, 0x01, 0xFFFF);
    ls_zone_surf = -1;
    ls_alloc_ret = &ls_zone_b;
    ls_reset();
    gLoadMapZone(&ls_zone_obj);
    FCK_EQ(ls_call(LS_SURF, 0)->a1, -2, "type 1 queries in mode -2");
    FCK_EQ((s32)ls_zone_b.f24, (s32)&ls_zone_a, "new node points at the old head");
    FCK_EQ((s32)ls_zone_list, (s32)&ls_zone_b, "head moved to the new node");
    FCK_EQ(ls_zone_b.f0, -1, "x -0x100 >> 8");
    FCK_EQ(ls_zone_b.f4, 1, "z 0x100 >> 8");
    FCK_EQ(ls_zone_b.f2, -1, "-1 >> 8 stays -1");
    FCK_EQ(ls_zone_b.fC, 1, "0x0002 / 2");
    FCK_EQ(ls_zone_b.fE, 2, "0x0004 / 2");
    FCK_EQ(ls_zone_b.f10, -1, "(s16)0xFFFE / 2");
    FCK_EQ(ls_zone_b.f12, -1, "+0x1C word 0xFFFF as s16");
    FCK_EQ(ls_zone_b.f14, 0x01, "+0x14 = record +0x1B");
    FCK_EQ(ls_zone_b.f15, 0, "+0x15 cleared again");

    /* type 5 asks for mode 3, type 2 for mode 0, type 0xFF keeps the type-2
     * arithmetic */
    ls_zone_record_init(0, 0, 0, 0, 0, 0, 0, 5, 0, 0);
    ls_reset();
    gLoadMapZone(&ls_zone_obj);
    FCK_EQ(ls_call(LS_SURF, 0)->a1, 3, "type 5 -> mode 3");

    ls_zone_record_init(0, 0, 0, 0, 0, 0, 0, 2, 0, 0);
    ls_reset();
    gLoadMapZone(&ls_zone_obj);
    FCK_EQ(ls_call(LS_SURF, 0)->a1, 0, "type 2 -> mode 0");

    ls_zone_record_init(0, 0, 0, 0, 0, 0, 0, -1, 0, 0);
    ls_reset();
    gLoadMapZone(&ls_zone_obj);
    FCK_EQ(ls_call(LS_SURF, 0)->a1, -3, "type 0xFF -> mode type-2 = -3");

    /* a failed allocation is not observed here: the unit dereferences the
     * result unconditionally, so only a successful one is exercised. */
}

/* =============================================== 8003C590 spawn chain marking */

/* Descriptor cell (0x4C bytes) and the 2-row grid of 256 pointers per row. */
struct LsSpawnCell {
    u8 unk00[4];
    u8 type;
    u8 unk05[0x0F];
    u8 id;
    u8 next;
    u8 unk16[0x0A];
    s32 unk20;
    u8 unk24[4];
    s16 unk28;
    s16 unk2A;
    u8 unk2C[0x1C];
    s16 unk48;
};

struct LsSpawnCell *ls_spawn_map[2][256] __asm__("D_800A7040");
static u32 ls_flag_words[256];
static struct LsSpawnCell ls_cells[8];

u32 *ls_stub_flagptr(u32 column, u32 row) __asm__("func_8003B3D8");
void ls_stub_orflags(u8 column, s32 row, u8 bits) __asm__("func_8003C4C4");
void ls_stub_cycle(s32 column, s32 stop, s32 row) __asm__("func_8003C500");

u32 *ls_stub_flagptr(u32 column, u32 row)
{
    ls_rec(LS_FLAGPTR, (s32)column, (s32)row, 0, 0, 0, 0);
    return &ls_flag_words[column & 0xFF];
}

void ls_stub_orflags(u8 column, s32 row, u8 bits)
{
    ls_rec(LS_ORFLAG, column, row, bits, 0, 0, 0);
}

void ls_stub_cycle(s32 column, s32 stop, s32 row)
{
    ls_rec(LS_CYCLE, column, stop, row, 0, 0, 0);
}

static void ls_spawn_reset(void)
{
    int i;
    lk_fill(ls_spawn_map, 0, (int)sizeof(ls_spawn_map));
    for (i = 0; i < 256; i++) ls_flag_words[i] = 0;
    lk_fill(ls_cells, 0, (int)sizeof(ls_cells));
    ls_reset();
}

static struct LsSpawnCell *ls_cell(int n, u8 type, u8 next, s32 unk20, s16 unk28,
                                   s16 unk2A, s16 unk48)
{
    struct LsSpawnCell *c = &ls_cells[n];
    c->type = type;
    c->next = next;
    c->unk20 = unk20;
    c->unk28 = unk28;
    c->unk2A = unk2A;
    c->unk48 = unk48;
    return c;
}

/* @leaf 8003C590 source src/game/8003C590.c: clears a 256-word visit table, then for i = 1..255 of one D_800A7040 row (1024-byte stride) skips empty, already-visited and type-flag-bit-4 slots, tags the chain with i while walking cell->next, and sets bit 4 through 8003C4C4 where (+0x28==0 && +0x2A>0) or (+0x20==0 && type 0x57/0x58/0x59, or 0x5C with +0x48==0); a next slot already tagged i closes the cycle: the tag is cleared and 8003C500 is called with (i, current, row) */
static void ls_test_spawn_chains(void)
{
    LK_FUNC("func_8003C590");

    /* empty row: nothing at all */
    ls_spawn_reset();
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_n, 0, "empty row touches nothing");

    /* a single active cell is flagged once */
    ls_spawn_reset();
    ls_spawn_map[0][3] = ls_cell(0, 0x57, 0, 0, 0, 0, 0);
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_count(LS_ORFLAG), 1, "one flagged cell");
    FCK_EQ(ls_call(LS_ORFLAG, 0)->a0, 3, "cell index 3");
    FCK_EQ(ls_call(LS_ORFLAG, 0)->a1, 0, "row 0");
    FCK_EQ(ls_call(LS_ORFLAG, 0)->a2, 4, "bit 4");
    FCK_EQ(ls_count(LS_CYCLE), 0, "no cycle");

    /* +0x2A > 0 marks even an otherwise inactive type */
    ls_spawn_reset();
    ls_spawn_map[0][4] = ls_cell(0, 0x5A, 0, 0, 0, 1, 0);
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_count(LS_ORFLAG), 1, "+0x2A > 0 marks type 0x5A");
    FCK_EQ(ls_call(LS_ORFLAG, 0)->a0, 4, "cell index 4");

    /* +0x28 != 0 disables that shortcut */
    ls_spawn_reset();
    ls_spawn_map[0][4] = ls_cell(0, 0x5A, 0, 0, 1, 1, 0);
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_count(LS_ORFLAG), 0, "+0x28 != 0 is not enough");

    /* +0x20 != 0 vetoes the type rule */
    ls_spawn_reset();
    ls_spawn_map[0][5] = ls_cell(0, 0x57, 0, 9, 0, 0, 0);
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_count(LS_ORFLAG), 0, "+0x20 != 0 vetoes 0x57");

    /* 0x5C keys off +0x48 */
    ls_spawn_reset();
    ls_spawn_map[0][6] = ls_cell(0, 0x5C, 0, 0, 0, 0, 0);
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_count(LS_ORFLAG), 1, "0x5C with +0x48 == 0");
    ls_spawn_reset();
    ls_spawn_map[0][6] = ls_cell(0, 0x5C, 0, 0, 0, 0, 5);
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_count(LS_ORFLAG), 0, "0x5C with +0x48 != 0");

    /* a slot whose type flag already carries bit 4 is skipped */
    ls_spawn_reset();
    ls_spawn_map[0][3] = ls_cell(0, 0x57, 0, 0, 0, 0, 0);
    ls_flag_words[3] = 4;
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_count(LS_ORFLAG), 0, "bit 4 in the type flags skips the slot");
    FCK_EQ(ls_count(LS_FLAGPTR), 1, "the flag word is consulted");
    FCK_EQ(ls_call(LS_FLAGPTR, 0)->a0, 3, "flag lookup column");
    FCK_EQ(ls_call(LS_FLAGPTR, 0)->a1, 0, "flag lookup row");

    /* chain 5 -> 7: both marked, in order */
    ls_spawn_reset();
    ls_spawn_map[0][5] = ls_cell(0, 0x57, 7, 0, 0, 0, 0);
    ls_spawn_map[0][7] = ls_cell(1, 0x59, 0, 0, 0, 0, 0);
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_count(LS_ORFLAG), 2, "chain of two marked");
    FCK_EQ(ls_call(LS_ORFLAG, 0)->a0, 5, "head first");
    FCK_EQ(ls_call(LS_ORFLAG, 1)->a0, 7, "then the tail");

    /* chain into an empty slot stops after the current cell */
    ls_spawn_reset();
    ls_spawn_map[0][5] = ls_cell(0, 0x57, 9, 0, 0, 0, 0);
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_count(LS_ORFLAG), 1, "chain into a hole marks one cell");

    /* 2 -> 3 -> 2 closes a cycle twice: once from 2, once from 3, because the
     * closure clears the tag of the closing cell and the second walk re-tags
     * itself */
    ls_spawn_reset();
    ls_spawn_map[0][2] = ls_cell(0, 0x57, 3, 0, 0, 0, 0);
    ls_spawn_map[0][3] = ls_cell(1, 0x57, 2, 0, 0, 0, 0);
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_count(LS_ORFLAG), 4, "both cells marked on both walks");
    FCK_EQ(ls_count(LS_CYCLE), 2, "two cycle closures");
    FCK_EQ(ls_call(LS_CYCLE, 0)->a0, 2, "closure from i = 2");
    FCK_EQ(ls_call(LS_CYCLE, 0)->a1, 3, "at the node that pointed back");
    FCK_EQ(ls_call(LS_CYCLE, 1)->a0, 3, "closure from i = 3");
    FCK_EQ(ls_call(LS_CYCLE, 1)->a1, 2, "at the other node");

    /* a self-loop closes immediately */
    ls_spawn_reset();
    ls_spawn_map[0][4] = ls_cell(0, 0x57, 4, 0, 0, 0, 0);
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_count(LS_ORFLAG), 1, "self-loop marks once");
    FCK_EQ(ls_count(LS_CYCLE), 1, "self-loop closes once");
    FCK_EQ(ls_call(LS_CYCLE, 0)->a0, 4, "cycle column = the node");
    FCK_EQ(ls_call(LS_CYCLE, 0)->a1, 4, "cycle stop = the node");

    /* row 1 reads its own 1024-byte slice and reports row 1 */
    ls_spawn_reset();
    ls_spawn_map[1][2] = ls_cell(0, 0x57, 0, 0, 0, 0, 0);
    gMarkSpawnRecordChains(1);
    FCK_EQ(ls_count(LS_ORFLAG), 1, "row 1 sees its own cell");
    FCK_EQ(ls_call(LS_ORFLAG, 0)->a0, 2, "column 2");
    FCK_EQ(ls_call(LS_ORFLAG, 0)->a1, 1, "row 1 reported");
    FCK_EQ(ls_call(LS_FLAGPTR, 0)->a1, 1, "flag lookup row 1");

    /* two independent chains in one row are both walked; 8 is inactive but its
     * successor 9 is reached through the chain, so only 1 and 9 are set */
    ls_spawn_reset();
    ls_spawn_map[0][1] = ls_cell(0, 0x58, 2, 0, 0, 0, 0);
    ls_spawn_map[0][8] = ls_cell(1, 0x5B, 9, 0, 0, 0, 0);
    ls_spawn_map[0][9] = ls_cell(2, 0x5E, 0, 0, 0, 1, 0);
    gMarkSpawnRecordChains(0);
    FCK_EQ(ls_count(LS_ORFLAG), 2, "1 and 9 marked; 8 is inactive");
    FCK_EQ(ls_call(LS_ORFLAG, 0)->a0, 1, "chain A head");
    FCK_EQ(ls_call(LS_ORFLAG, 1)->a0, 9, "chain B tail via +0x2A");
}

/* ================================================= 8003D240 anchor node sync */

/* 0x4C-byte anchor node. */
struct LsAnchor {
    struct LsAnchor *next;
    s32 unk04;
    s32 unk08, unk0C, unk10, unk14, unk18, unk1C, unk20, unk24;
    s32 unk28, unk2C, unk30;
    struct LsAnchor *shadow;
    u8 unk38;
    u8 flags39;
    s16 unk3A;
    u16 unk3C;
    s16 unk3E;
    u8 unk40[4];
    s32 unk44;
    s32 unk48;
};

struct LsAnchor *ls_anchor_list __asm__("D_8009F320");

static u8 ls_anchor_src[0x80];
static struct LsAnchor ls_anchor_shadow, ls_anchor_shadow2;

void ls_stub_freebuf(struct LsAnchor *object) __asm__("func_8003CF20");
void ls_stub_point(void *src, s32 a1, s32 a2, void *dst) __asm__("func_8003D0F0");

void ls_stub_freebuf(struct LsAnchor *object)
{
    ls_rec(LS_FREEBUF, (s32)object, 0, 0, 0, 0, 0);
}

/* Records only: the point is one of the twelve words already in the fixture, so
 * writing something into the destination would mask what the unit copied. */
void ls_stub_point(void *src, s32 a1, s32 a2, void *dst)
{
    ls_rec(LS_POINT, (s32)src, a1, a2, (s32)dst, 0, 0);
}

static void ls_anchor_src_init(void)
{
    int off;
    lk_fill(ls_anchor_src, 0x33, (int)sizeof(ls_anchor_src));
    *(s32 *)(ls_anchor_src + 0x08) = 0x7777;      /* parent frame id */
    for (off = 0x2C; off <= 0x58; off += 4)
        *(s32 *)(ls_anchor_src + off) = 0x10000000 + off;
}

static void ls_anchor_init(struct LsAnchor *o)
{
    lk_fill(o, 0, (int)sizeof(*o));
    o->unk08 = 0;
    o->unk28 = 100;
    o->unk2C = 200;
    o->unk30 = 300;
    o->unk3A = 1;
    o->unk3E = 1;
}

/* @leaf 8003D240 source src/game/8003D240.c: walks D_8009F320 and refreshes a node whose +0x38 is 0 and whose source (+0x04) reports a different word at +0x08: either a 0x4C-byte shadow copy of source+0x28 (flag bit 2 clear), or the twelve source words from +0x2C..+0x58 into +0x04..+0x30 with the fetched point added to +0x28..+0x30; the frame word +0x3E == 0 sets flag bit 8, the shadow is committed and freed once bits 1 and 3 are both set, and a zero +0x3A unlinks and destroys the node */
static void ls_test_anchor_sync(void)
{
    struct LsAnchor o1, o2, o3;

    LK_FUNC("func_8003D240");
    ls_anchor_src_init();

    /* already updated: no refresh, no alloc, node stays */
    ls_anchor_init(&o1);
    o1.unk38 = 1;
    o1.flags39 = 0x10;
    o1.unk04 = (s32)ls_anchor_src;
    ls_anchor_list = &o1;
    ls_reset();
    gSyncAttachedNodes();
    FCK_EQ(ls_n, 0, "updated node calls nothing");
    FCK_EQ(o1.unk04, (s32)ls_anchor_src, "+0x04 untouched");
    FCK_EQ(o1.flags39, 0x10, "+0x39 untouched");
    FCK_EQ((s32)ls_anchor_list, (s32)&o1, "node stays linked");

    /* refresh with flag bit 2 set: direct copy, no shadow */
    ls_anchor_init(&o1);
    o1.unk04 = (s32)ls_anchor_src;
    o1.flags39 = 0x04;
    ls_anchor_list = &o1;
    ls_reset();
    gSyncAttachedNodes();
    FCK_EQ(ls_count(LS_ALLOC), 0, "bit 2 suppresses the shadow");
    FCK_EQ(ls_count(LS_POINT), 1, "one point fetched");
    FCK_EQ((s32)ls_call(LS_POINT, 0)->a0, (s32)(ls_anchor_src + 0x28), "point from source+0x28");
    FCK_EQ(ls_call(LS_POINT, 0)->a1, -1, "mode -1");
    FCK_EQ(ls_call(LS_POINT, 0)->a2, 0, "third argument 0");
    FCK_EQ((s32)ls_call(LS_POINT, 0)->a3, (s32)&o1.unk28, "point into the node");
    FCK_EQ(o1.unk04, 0x1000002C, "+0x04 = source+0x2C");
    FCK_EQ(o1.unk08, 0x10000030, "+0x08 = source+0x30");
    FCK_EQ(o1.unk24, 0x1000004C, "+0x24 = source+0x4C");
    FCK_EQ(o1.unk28, 100 + 0x10000050, "+0x28 += source+0x50");
    FCK_EQ(o1.unk2C, 200 + 0x10000054, "+0x2C += source+0x54");
    FCK_EQ(o1.unk30, 300 + 0x10000058, "+0x30 += source+0x58");
    FCK_EQ(o1.unk38, 1, "update marker set");
    FCK_EQ(o1.flags39, 0x05, "flags |= 1 on top of bit 2");
    FCK_EQ((s32)o1.shadow, 0, "no shadow kept");

    /* refresh with bit 2 clear and the frame word zero: the shadow is taken,
     * committed in the same call and freed */
    ls_anchor_init(&o1);
    o1.unk04 = (s32)ls_anchor_src;
    o1.flags39 = 0;
    o1.unk3E = 0;
    lk_fill(&ls_anchor_shadow, 0, (int)sizeof(ls_anchor_shadow));
    ls_alloc_ret = &ls_anchor_shadow;
    ls_anchor_list = &o1;
    ls_reset();
    gSyncAttachedNodes();
    FCK_EQ(ls_count(LS_ALLOC), 1, "shadow allocated");
    FCK_EQ(ls_call(LS_ALLOC, 0)->a0, 0x4C, "shadow size 0x4C");
    FCK_EQ(ls_call(LS_ALLOC, 0)->a1, 1, "shadow tag 1");
    FCK_EQ(ls_anchor_shadow.unk04, 0x1000002C, "shadow copied source+0x2C");
    FCK_EQ(ls_anchor_shadow.unk30, 0x10000058, "shadow copied source+0x58");
    FCK_EQ(ls_count(LS_POINT), 1, "point fetched from the shadow");
    FCK_EQ((s32)ls_call(LS_POINT, 0)->a0, (s32)&ls_anchor_shadow, "point source is the shadow");
    FCK_EQ(ls_count(LS_FREE), 1, "shadow freed");
    FCK_EQ((s32)ls_call(LS_FREE, 0)->a0, (s32)&ls_anchor_shadow, "freed block is the shadow");
    FCK_EQ(o1.unk04, 0x1000002C, "commit copied shadow +0x04");
    FCK_EQ(o1.unk24, 0x1000004C, "commit copied shadow +0x24");
    FCK_EQ(o1.unk28, 0x200000A0, "commit copies then adds the shadow +0x28");
    FCK_EQ(o1.unk38, 1, "commit clears the update marker");
    FCK_EQ(o1.flags39, 0x01, "(0x00|2|1|8) & 0xF5 | 1");
    FCK_EQ((s32)o1.shadow, 0, "shadow pointer cleared");

    /* allocation failure falls back to the direct path */
    ls_anchor_init(&o1);
    o1.unk04 = (s32)ls_anchor_src;
    o1.flags39 = 0;
    o1.unk3E = 1;
    ls_alloc_ret = 0;
    ls_anchor_list = &o1;
    ls_reset();
    gSyncAttachedNodes();
    FCK_EQ(ls_count(LS_ALLOC), 1, "allocation attempted");
    FCK_EQ(ls_count(LS_POINT), 1, "direct path used instead");
    FCK_EQ(o1.flags39, 0x01, "no bit 2 without a shadow");
    FCK_EQ((s32)o1.shadow, 0, "no shadow pointer");

    /* pending shadow, node not refreshing, both flags present: commit only */
    ls_anchor_init(&o1);
    o1.unk38 = 1;
    o1.flags39 = 0x0A;
    lk_fill(&ls_anchor_shadow2, 0, (int)sizeof(ls_anchor_shadow2));
    ls_anchor_shadow2.unk04 = 0x20000004;
    ls_anchor_shadow2.unk24 = 0x20000024;
    ls_anchor_shadow2.unk28 = 0x10;
    ls_anchor_shadow2.unk2C = 0x20;
    ls_anchor_shadow2.unk30 = 0x30;
    o1.shadow = &ls_anchor_shadow2;
    ls_anchor_list = &o1;
    ls_reset();
    gSyncAttachedNodes();
    FCK_EQ(ls_count(LS_POINT), 1, "pending shadow fetched a point");
    FCK_EQ((s32)ls_call(LS_POINT, 0)->a0, (s32)&ls_anchor_shadow2, "point source is the shadow");
    FCK_EQ(ls_count(LS_FREE), 1, "pending shadow freed");
    FCK_EQ(o1.unk04, 0x20000004, "commit copied +0x04");
    FCK_EQ(o1.unk24, 0x20000024, "commit copied +0x24");
    FCK_EQ(o1.unk28, 0x20, "+0x28 assigned then doubled");
    FCK_EQ(o1.unk2C, 0x40, "+0x2C assigned then doubled");
    FCK_EQ(o1.unk30, 0x60, "+0x30 assigned then doubled");
    FCK_EQ(o1.flags39, 0x01, "(0x0A & 0xF5) | 1");
    FCK_EQ((s32)o1.shadow, 0, "shadow pointer cleared");

    /* shadow present but the frame bit is missing: nothing happens */
    ls_anchor_init(&o1);
    o1.unk38 = 1;
    o1.flags39 = 0x02;
    o1.shadow = &ls_anchor_shadow2;
    ls_anchor_list = &o1;
    ls_reset();
    gSyncAttachedNodes();
    FCK_EQ(ls_count(LS_FREE), 0, "bit 8 missing keeps the shadow");
    FCK_EQ((s32)o1.shadow, (s32)&ls_anchor_shadow2, "shadow still attached");
    FCK_EQ(o1.flags39, 0x02, "flags untouched");

    /* frame word zero with no shadow only sets flag bit 8 */
    ls_anchor_init(&o1);
    o1.unk38 = 1;
    o1.flags39 = 0x20;
    o1.unk3E = 0;
    ls_anchor_list = &o1;
    ls_reset();
    gSyncAttachedNodes();
    FCK_EQ(ls_n, 0, "no callee for a lone flag update");
    FCK_EQ(o1.flags39, 0x28, "flags |= 8");

    /* zero child count unlinks the head node and destroys it */
    ls_anchor_init(&o1);
    ls_anchor_init(&o2);
    o1.unk38 = 1; o1.flags39 = 0;
    o2.unk38 = 1; o2.flags39 = 0;
    o1.unk3A = 0;
    o1.next = &o2;
    ls_anchor_list = &o1;
    ls_reset();
    gSyncAttachedNodes();
    FCK_EQ((s32)ls_anchor_list, (s32)&o2, "head replaced by the successor");
    FCK_EQ((s32)o1.next, 0, "unlinked node has no successor");
    FCK_EQ(ls_count(LS_FREEBUF), 1, "one node destroyed");
    FCK_EQ((s32)ls_call(LS_FREEBUF, 0)->a0, (s32)&o1, "destroyed node is the head");

    /* a middle node is unlinked through its predecessor */
    ls_anchor_init(&o1);
    ls_anchor_init(&o2);
    ls_anchor_init(&o3);
    o1.unk38 = 1; o1.flags39 = 0;
    o2.unk38 = 1; o2.flags39 = 0;
    o3.unk38 = 1; o3.flags39 = 0;
    o2.unk3A = 0;
    o1.next = &o2;
    o2.next = &o3;
    ls_anchor_list = &o1;
    ls_reset();
    gSyncAttachedNodes();
    FCK_EQ((s32)o1.next, (s32)&o3, "predecessor relinked past the middle node");
    FCK_EQ((s32)o2.next, 0, "middle node detached");
    FCK_EQ(ls_count(LS_FREEBUF), 1, "one middle node destroyed");
    FCK_EQ((s32)ls_call(LS_FREEBUF, 0)->a0, (s32)&o2, "destroyed node is the middle");

    /* an empty list is a no-op */
    ls_anchor_list = 0;
    ls_reset();
    gSyncAttachedNodes();
    FCK_EQ(ls_n, 0, "empty list calls nothing");
}

/* =========================================== 80046CF0 weapon sound slot pool */

/* Stats block +0xC1 is the player index; the object's +0xAC points at it. */
struct LsWStats { u8 pad00[0xC1]; s8 idx; };
struct LsWObj { u8 pad00[0xAC]; struct LsWStats *stats; };
struct LsWHandle { u8 pad00[0x10]; s32 f10; };
struct LsWSlot {
    s32 status;
    s32 unk4;
    s32 bank;
    struct LsWHandle *handle;
    s32 f10;
};

struct LsWSlot ls_wslots[6] __asm__("D_800A10E0");
u8 ls_bank_table __asm__("D_800A1830");

static s32 ls_resolve_ret;
static struct LsWHandle *ls_play_ret;

s32 ls_stub_resolve(s32 id, u8 *table, s32 limit) __asm__("func_80045494");
s32 ls_stub_play(s32 a0, s32 a1, u32 index, s32 a3, s32 a4) __asm__("func_80046F9C");

s32 ls_stub_resolve(s32 id, u8 *table, s32 limit)
{
    ls_rec(LS_RESOLVE, id, (s32)table, limit, 0, 0, 0);
    return ls_resolve_ret;
}

s32 ls_stub_play(s32 a0, s32 a1, u32 index, s32 a3, s32 a4)
{
    ls_rec(LS_PLAY, a0, a1, (s32)index, a3, a4, 0);
    return (s32)ls_play_ret;
}

static void ls_w_reset(void)
{
    int i;
    for (i = 0; i < 6; i++) {
        ls_wslots[i].status = 0;
        ls_wslots[i].unk4 = 0;
        ls_wslots[i].bank = 0;
        ls_wslots[i].handle = 0;
        ls_wslots[i].f10 = 0;
    }
    ls_reset();
}

static struct LsWHandle ls_wh_a, ls_wh_b;

/* @leaf 80046CF0 source src/game/80046CF0.c: acts only for player indices below 2, resolves the state against D_800A1830 with limit 0x47 and aborts on 0xFF; in the three slots at D_800A10E0[player*3] it refreshes (status 0x1E, +4 = 1) an occupied slot of the same bank whose +0x10 matches the handle's, frees (status 0) a same-bank slot with a stale handle, remembers the first free slot, and otherwise starts func_80046F9C(0, 0, bank, object, 0), storing status 0x1E, +4 = 1, the bank, the handle word and the handle on success */
static void ls_test_weapon_sound(void)
{
    struct LsWObj object;
    struct LsWStats stats;
    struct LsWHandle handle;

    LK_FUNC("func_80046CF0");
    lk_fill(&stats, 0, (int)sizeof(stats));
    lk_fill(&object, 0, (int)sizeof(object));
    object.stats = &stats;
    handle.f10 = 0x99;

    /* player index 2 and above do nothing */
    stats.idx = 2;
    ls_w_reset();
    gRefreshPlayerWeaponSound(&object, 0x12);
    FCK_EQ(ls_n, 0, "index 2 calls nothing");
    stats.idx = 3;
    gRefreshPlayerWeaponSound(&object, 0x12);
    FCK_EQ(ls_n, 0, "index 3 calls nothing");

    /* an unresolvable state aborts before touching the slots */
    stats.idx = 0;
    ls_w_reset();
    ls_resolve_ret = 0xFF;
    gRefreshPlayerWeaponSound(&object, 0x12);
    FCK_EQ(ls_count(LS_RESOLVE), 1, "state resolved once");
    FCK_EQ(ls_call(LS_RESOLVE, 0)->a0, 0x12, "state id passed through");
    FCK_EQ((s32)ls_call(LS_RESOLVE, 0)->a1, (s32)&ls_bank_table, "table D_800A1830");
    FCK_EQ(ls_call(LS_RESOLVE, 0)->a2, 0x47, "limit 0x47");
    FCK_EQ(ls_count(LS_PLAY), 0, "0xFF does not start a sound");
    FCK_EQ(ls_wslots[0].status, 0, "slot 0 untouched");

    /* free slot: the sound is started and the slot filled */
    ls_w_reset();
    ls_resolve_ret = 5;
    ls_play_ret = &handle;
    gRefreshPlayerWeaponSound(&object, 0x12);
    FCK_EQ(ls_count(LS_PLAY), 1, "one sound started");
    FCK_EQ(ls_call(LS_PLAY, 0)->a0, 0, "argument 0");
    FCK_EQ(ls_call(LS_PLAY, 0)->a1, 0, "argument 1");
    FCK_EQ(ls_call(LS_PLAY, 0)->a2, 5, "resolved bank as the index");
    FCK_EQ((s32)ls_call(LS_PLAY, 0)->a3, (s32)&object, "object passed through");
    FCK_EQ(ls_call(LS_PLAY, 0)->a4, 0, "argument 4");
    FCK_EQ(ls_wslots[0].status, 0x1E, "slot 0 status 0x1E");
    FCK_EQ(ls_wslots[0].unk4, 1, "slot 0 +4 = 1");
    FCK_EQ(ls_wslots[0].bank, 5, "slot 0 bank 5");
    FCK_EQ(ls_wslots[0].f10, 0x99, "slot 0 keeps the handle word");
    FCK_EQ((s32)ls_wslots[0].handle, (s32)&handle, "slot 0 keeps the handle");
    FCK_EQ(ls_wslots[1].status, 0, "slot 1 stays free");

    /* a failed start leaves the slot free */
    ls_w_reset();
    ls_play_ret = 0;
    gRefreshPlayerWeaponSound(&object, 0x12);
    FCK_EQ(ls_count(LS_PLAY), 1, "start attempted");
    FCK_EQ(ls_wslots[0].status, 0, "failed start keeps status 0");
    FCK_EQ((s32)ls_wslots[0].handle, 0, "failed start stores no handle");

    /* same bank and the same handle word: refreshed in place, no new sound */
    ls_w_reset();
    ls_play_ret = &handle;
    ls_wslots[0].status = 7;
    ls_wslots[0].bank = 5;
    ls_wslots[0].handle = &ls_wh_a;
    ls_wslots[0].f10 = 0x99;
    ls_wh_a.f10 = 0x99;
    gRefreshPlayerWeaponSound(&object, 0x12);
    FCK_EQ(ls_count(LS_PLAY), 0, "matching handle restarts nothing");
    FCK_EQ(ls_wslots[0].status, 0x1E, "status refreshed");
    FCK_EQ(ls_wslots[0].unk4, 1, "+4 set");
    FCK_EQ((s32)ls_wslots[0].handle, (s32)&ls_wh_a, "handle unchanged");

    /* same bank, stale handle: the slot is freed and immediately reused */
    ls_w_reset();
    ls_play_ret = &handle;
    ls_wslots[0].status = 7;
    ls_wslots[0].bank = 5;
    ls_wslots[0].handle = &ls_wh_b;
    ls_wslots[0].f10 = 0x99;
    ls_wh_b.f10 = 0x77;
    gRefreshPlayerWeaponSound(&object, 0x12);
    FCK_EQ(ls_count(LS_PLAY), 1, "stale slot reused");
    FCK_EQ(ls_wslots[0].status, 0x1E, "slot re-filled");
    FCK_EQ((s32)ls_wslots[0].handle, (s32)&handle, "new handle stored");

    /* a stale slot whose restart fails is left free */
    ls_w_reset();
    ls_play_ret = 0;
    ls_wslots[0].status = 7;
    ls_wslots[0].bank = 5;
    ls_wslots[0].handle = &ls_wh_b;
    ls_wslots[0].f10 = 0x99;
    ls_wh_b.f10 = 0x77;
    gRefreshPlayerWeaponSound(&object, 0x12);
    FCK_EQ(ls_count(LS_PLAY), 1, "restart attempted");
    FCK_EQ(ls_wslots[0].status, 0, "stale slot left free");

    /* a slot of another bank is occupied and skipped: the next one is used */
    ls_w_reset();
    ls_play_ret = &handle;
    ls_wslots[0].status = 3;
    ls_wslots[0].bank = 9;
    ls_wslots[0].handle = &ls_wh_a;
    ls_wslots[0].f10 = 0x11;
    gRefreshPlayerWeaponSound(&object, 0x12);
    FCK_EQ(ls_count(LS_PLAY), 1, "second slot used");
    FCK_EQ(ls_wslots[1].status, 0x1E, "slot 1 filled");
    FCK_EQ(ls_wslots[0].status, 3, "slot 0 of another bank untouched");
    FCK_EQ(ls_wslots[0].bank, 9, "slot 0 bank untouched");

    /* all three slots busy with other banks: nothing starts */
    ls_w_reset();
    ls_play_ret = &handle;
    ls_wslots[0].status = 3; ls_wslots[0].bank = 9; ls_wslots[0].handle = &ls_wh_a;
    ls_wslots[1].status = 3; ls_wslots[1].bank = 9; ls_wslots[1].handle = &ls_wh_a;
    ls_wslots[2].status = 3; ls_wslots[2].bank = 9; ls_wslots[2].handle = &ls_wh_a;
    gRefreshPlayerWeaponSound(&object, 0x12);
    FCK_EQ(ls_count(LS_PLAY), 0, "no free slot, no sound");

    /* player 1 works on the second slot triple */
    stats.idx = 1;
    ls_w_reset();
    ls_play_ret = &handle;
    ls_wslots[3].status = 3;
    ls_wslots[3].bank = 9;
    ls_wslots[3].handle = &ls_wh_a;
    ls_wslots[3].f10 = 0x11;
    gRefreshPlayerWeaponSound(&object, 0x12);
    FCK_EQ(ls_count(LS_PLAY), 1, "sound started for player 1");
    FCK_EQ(ls_wslots[4].status, 0x1E, "slot 4 filled, the second of the triple");
    FCK_EQ(ls_wslots[0].status, 0, "player 0 slot untouched");

    /* a player 1 slot with the matching bank refreshes without a new sound */
    ls_reset();
    ls_wslots[4].bank = 5;
    ls_wslots[4].f10 = 0x99;
    ls_wslots[4].handle = &ls_wh_a;
    ls_wh_a.f10 = 0x99;
    gRefreshPlayerWeaponSound(&object, 0x12);
    FCK_EQ(ls_count(LS_PLAY), 0, "player 1 handle match");
    FCK_EQ(ls_wslots[4].status, 0x1E, "player 1 slot refreshed");
}

/* ============================================== 8004FFD0 path curve offsets */

/* Same field layout the unit declares: size at 0x24, rate/scale/rate2 at
 * 0x28/0x2A/0x2C, flags at 0x3C, angle/angle2 at 0x4C/0x4E, angle3/angle4 at
 * 0x52/0x54, turn/turnout at 0x58/0x5A. */
typedef struct {
    char p0[0x16];
    s16 x, y, z;
    char p1[8];
    s16 size;
    char p2[2];
    s16 rate, scale, rate2;
    char p3[14];
    u32 flags;
    char p4[12];
    u16 angle, angle2;
    char p5[2];
    u16 angle3, angle4;
    char p6[2];
    s16 turn;
    u16 turnout;
} LsCurveObj;

typedef struct { char p[0x50]; s32 x, y, z; } LsCurveVec;

static void ls_curve_reset(LsCurveObj *o)
{
    lk_fill(o, 0, (int)sizeof(*o));
    o->turnout = 0xDEAD;
    o->angle = 0x0000;
    o->angle2 = 0x7777;
    o->angle3 = 0x0000;
    o->angle4 = 0x6666;
}

/* @leaf 8004FFD0 source src/game/8004FFD0.c: with a non-zero turn and bits 28-29 of flags set, the 16.16 size minus |input| is scaled by 0x80000000/(size/2) and multiplied with turn, leaving turnout = ((turn*r)>>16) & 0xFFF; rate drives angle2 = (angle + ((scale*4*rate)>>9)*(size+input)>>16) & 0xFFF unless flag 0x1000, and always adds (x,y,z)<<8 scaled by q = (((scale<<10)*(size-|input|))>>16 >> 2)^2 >> 12 to the vector; rate2 turns angle4 = (angle3 + ((rate2<<16) * (((size+input)>>1 * scale<<10)>>16) >> 16)) & 0xFFF */
static void ls_test_path_curve(void)
{
    LsCurveObj o;
    LsCurveVec v;

    LK_FUNC("func_8004FFD0");

    /* everything off: the outputs keep their preset values */
    ls_curve_reset(&o);
    lk_fill(&v, 0, (int)sizeof(v));
    gApplyPathCurveOffsets(&o, 0x1234, &v);
    FCK_EQ(o.turnout, 0xDEAD, "turn 0 leaves turnout");
    FCK_EQ(o.angle2, 0x7777, "rate 0 leaves angle2");
    FCK_EQ(o.angle4, 0x6666, "rate2 0 leaves angle4");
    FCK_EQ(v.x | v.y | v.z, 0, "no vector contribution");

    /* turn without the flag bits 28-29 does nothing */
    o.turn = 5;
    o.flags = 0x01000000;
    gApplyPathCurveOffsets(&o, 0, &v);
    FCK_EQ(o.turnout, 0xDEAD, "flags bits 28-29 zero leaves turnout");

    /* input 0: r is exactly the full scale, so turnout = turn & 0xFFF */
    o.flags = 0x30000000;
    o.size = 0x100;
    gApplyPathCurveOffsets(&o, 0, &v);
    FCK_EQ(o.turnout, 5, "turnout = turn & 0xFFF");

    o.turn = 0x1234;
    gApplyPathCurveOffsets(&o, 0, &v);
    FCK_EQ(o.turnout, 0x234, "turn 0x1234 masks to 0x234");

    /* half the size in 8.8 halves the turnout, truncated */
    o.turn = 5;
    gApplyPathCurveOffsets(&o, 0x8000, &v);
    FCK_EQ(o.turnout, 2, "5 * 0x8000 >> 16 = 2");
    gApplyPathCurveOffsets(&o, -0x8000, &v);
    FCK_EQ(o.turnout, 2, "the input is used through its absolute value");

    /* at or beyond the size the turnout collapses to 0 */
    gApplyPathCurveOffsets(&o, 0x10000, &v);
    FCK_EQ(o.turnout, 0, "input == size -> 0");
    gApplyPathCurveOffsets(&o, 0x40000, &v);
    FCK_EQ(o.turnout, 0xFF1, "input beyond size: (5 * -(0x30000)) >> 16 = -15 & 0xFFF");

    /* a negative size mirrors the half-scale case */
    o.size = -0x100;
    gApplyPathCurveOffsets(&o, 0, &v);
    FCK_EQ(o.turnout, 5, "negative size, input 0 -> full turnout");
    gApplyPathCurveOffsets(&o, 0x8000, &v);
    FCK_EQ(o.turnout, 7, "negative size widens the scale: 0x18000 >> 16 = 1.5 * 5");

    /* rate path: angle2 and the vector */
    ls_curve_reset(&o);
    lk_fill(&v, 0, (int)sizeof(v));
    o.size = 0x100;
    o.scale = 0x10;
    o.rate = 8;
    o.rate2 = 8;
    o.x = 3; o.y = -2; o.z = 5;
    gApplyPathCurveOffsets(&o, 0, &v);
    FCK_EQ(o.angle2, 1, "angle 0 + ((0x40*8)>>9)*0x10000>>16 = 1");
    FCK_EQ(o.angle4, 1, "angle3 0 + ((8<<16)*0x2000>>16)>>16 = 1");
    FCK_EQ(v.x, 48, "3<<8 scaled by q = 0x1000 -> 48");
    FCK_EQ(v.y, -32, "-2<<8 -> -32");
    FCK_EQ(v.z, 80, "5<<8 -> 80");

    /* the vector accumulates across calls */
    gApplyPathCurveOffsets(&o, 0, &v);
    FCK_EQ(v.x, 96, "second call adds again");
    FCK_EQ(v.z, 160, "second call adds again on z");

    /* a high base angle wraps under the 0xFFF mask */
    o.angle = 0x0FFF;
    o.angle3 = 0x0FFF;
    gApplyPathCurveOffsets(&o, 0, &v);
    FCK_EQ(o.angle2, 0, "(0xFFF + 1) & 0xFFF");
    FCK_EQ(o.angle4, 0, "(0xFFF + 1) & 0xFFF");

    /* flag 0x1000 suppresses angle2 but not the vector */
    o.angle = 0x0100;
    o.angle2 = 0x7777;
    o.flags = 0x1000;
    v.x = v.y = v.z = 0;
    gApplyPathCurveOffsets(&o, 0, &v);
    FCK_EQ(o.angle2, 0x7777, "angle2 untouched with flag 0x1000");
    FCK_EQ(v.x, 48, "vector still updated");

    /* rate 0 with rate2 alone only touches angle4 */
    o.rate = 0;
    o.flags = 0;
    o.angle3 = 0;
    o.angle2 = 0x7777;
    o.angle4 = 0x6666;
    v.x = v.y = v.z = 0;
    gApplyPathCurveOffsets(&o, 0, &v);
    FCK_EQ(o.angle2, 0x7777, "rate 0 leaves angle2");
    FCK_EQ(o.angle4, 1, "rate2 still writes angle4");
    FCK_EQ(v.x, 0, "rate 0 leaves the vector");
}

/* ============================================ 800513D0 looping path advance */

typedef struct { s32 x, y, z; } LsPathVec;
typedef struct {
    s32 unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    LsPathVec unk10;
    s32 *unk1C;
    s32 *unk20;
    s32 unk24;
    s32 unk28;
    LsPathVec unk2C;
} LsPath;

static s32 ls_path_dist;

void ls_stub_scale(s32 length, s32 *vector) __asm__("func_8001AB78");
s32 ls_stub_dist(s32 *a, s32 *b) __asm__("func_8001A13C");

/* Documented stand-in: the real scaler is a fixed-point vector routine outside
 * this suite, so the harness replaces it with an identity whose effect is
 * readable (x = len, y = -len, z = 2*len). That keeps the checks on the unit's
 * own contributions - the length it passes and the addition it performs. */
void ls_stub_scale(s32 length, s32 *vector)
{
    ls_rec(LS_SCALE, length, (s32)vector, 0, 0, 0, 0);
    vector[0] = length;
    vector[1] = -length;
    vector[2] = 2 * length;
}

s32 ls_stub_dist(s32 *a, s32 *b)
{
    ls_rec(LS_DIST, (s32)a, (s32)b, 0, 0, 0, 0);
    return ls_path_dist;
}

/* @leaf 800513D0 source src/game/800513D0.c: returns 0 unless +0x00 is set, then spends the budget in steps of at most (unk08 - unk0C), scaling the +0x10 vector by the step and adding it to the position; when budget remains it loads (z, x, length) from the 3-word record at +0x20, bumps +0x24 and the optional counter, advances +0x20 by 12, and on +0x24 == +0x28 wraps to +0x1C, reports 1 and snaps the position to +0x2C when the XZ distance is below 0x2000 */
static void ls_test_path_advance(void)
{
    LsPath p;
    LsPathVec pos;
    s32 counter;
    s32 recs[6];

    LK_FUNC("func_800513D0");

    /* inactive path */
    lk_fill(&p, 0, (int)sizeof(p));
    pos.x = 1; pos.y = 2; pos.z = 3;
    counter = 0;
    ls_reset();
    FCK_EQ(gAdvanceLoopingPathPosition(&p, &pos, 0x100, &counter), 0, "inactive returns 0");
    FCK_EQ(ls_n, 0, "inactive calls nothing");
    FCK_EQ(pos.x, 1, "position untouched");

    /* a step that stays inside the segment */
    lk_fill(&p, 0, (int)sizeof(p));
    p.unk00 = 1;
    p.unk08 = 0x1000;
    p.unk0C = 0;
    p.unk10.x = 1; p.unk10.y = 2; p.unk10.z = 3;
    p.unk24 = 0; p.unk28 = 2;
    pos.x = 0; pos.y = 0; pos.z = 0;
    counter = 0;
    ls_reset();
    FCK_EQ(gAdvanceLoopingPathPosition(&p, &pos, 0x100, &counter), 0, "no wrap");
    FCK_EQ(ls_count(LS_SCALE), 1, "one scaling step");
    FCK_EQ(ls_call(LS_SCALE, 0)->a0, 0x100, "step length is the budget");
    FCK_EQ(p.unk0C, 0x100, "progress advanced");
    FCK_EQ(pos.x, 0x100, "x += 0x100");
    FCK_EQ(pos.y, -0x100, "y -= 0x100");
    FCK_EQ(pos.z, 0x200, "z += 0x200");
    FCK_EQ(counter, 0, "no segment transition");
    FCK_EQ(p.unk24, 0, "segment index unchanged");

    /* a budget that exactly fills the segment does not advance it */
    p.unk0C = 0;
    p.unk08 = 0x100;
    pos.x = pos.y = pos.z = 0;
    ls_reset();
    FCK_EQ(gAdvanceLoopingPathPosition(&p, &pos, 0x100, &counter), 0, "exact fill does not wrap");
    FCK_EQ(p.unk0C, 0x100, "progress at the segment end");
    FCK_EQ(ls_count(LS_SCALE), 1, "single step");
    FCK_EQ(counter, 0, "no transition");

    /* one segment transition with the leftover budget consumed by the next */
    recs[0] = 0x11;  /* z */
    recs[1] = 0x22;  /* x */
    recs[2] = 0x2000; /* length */
    recs[3] = 0x33;
    recs[4] = 0x44;
    recs[5] = 0x3000;
    lk_fill(&p, 0, (int)sizeof(p));
    p.unk00 = 1;
    p.unk08 = 0x100;
    p.unk0C = 0;
    p.unk10.z = 3; p.unk10.x = 1; p.unk10.y = 2;
    p.unk1C = recs;
    p.unk20 = recs;
    p.unk24 = 0;
    p.unk28 = 5;      /* no wrap this call */
    pos.x = pos.y = pos.z = 0;
    counter = 0;
    ls_reset();
    FCK_EQ(gAdvanceLoopingPathPosition(&p, &pos, 0x300, &counter), 0, "no wrap yet");
    FCK_EQ(ls_count(LS_SCALE), 2, "two steps");
    FCK_EQ(ls_call(LS_SCALE, 0)->a0, 0x100, "first step fills the segment");
    FCK_EQ(ls_call(LS_SCALE, 1)->a0, 0x200, "second step is the leftover");
    FCK_EQ(pos.x, 0x100 + 0x200, "x accumulates both steps");
    FCK_EQ(pos.z, 0x200 + 0x400, "z accumulates both steps");
    FCK_EQ(p.unk10.x, 0x22, "x from record word 1");
    FCK_EQ(p.unk10.z, 0x11, "z from record word 0");
    FCK_EQ(p.unk08, 0x2000, "segment length from record word 2");
    FCK_EQ(p.unk0C, 0x200, "progress restarted then advanced");
    FCK_EQ(p.unk24, 1, "segment index bumped");
    FCK_EQ((s32)p.unk20, (s32)(recs + 3), "record pointer advanced by 3 words");
    FCK_EQ(counter, 1, "counter bumped once");

    /* wrapping: the index reaches +0x28, the pointer returns to +0x1C and a
     * near position snaps to +0x2C */
    lk_fill(&p, 0, (int)sizeof(p));
    p.unk00 = 1;
    p.unk08 = 0x100;
    p.unk0C = 0;
    p.unk10.x = 1; p.unk10.y = 0; p.unk10.z = 0;
    p.unk1C = recs;
    p.unk20 = recs;
    p.unk24 = 1;
    p.unk28 = 2;
    recs[0] = 0; recs[1] = 0; recs[2] = 0x1000;
    p.unk2C.x = 0x4000; p.unk2C.y = 0x5000; p.unk2C.z = 0x6000;
    pos.x = pos.y = pos.z = 0;
    counter = 0;
    ls_path_dist = 0x1FFF;
    ls_reset();
    FCK_EQ(gAdvanceLoopingPathPosition(&p, &pos, 0x300, &counter), 1, "wrap reported");
    FCK_EQ(p.unk24, 0, "index reset to 0");
    FCK_EQ((s32)p.unk20, (s32)recs, "record pointer back to the base");
    FCK_EQ(ls_count(LS_DIST), 1, "distance checked once on wrap");
    FCK_EQ((s32)ls_call(LS_DIST, 0)->a0, (s32)&pos, "distance from the position");
    FCK_EQ((s32)ls_call(LS_DIST, 0)->a1, (s32)&p.unk2C, "distance to +0x2C");
    /* the snap runs inside the loop, so the remaining 0x200 is spent on the
     * new segment right after it and moves the position off the record */
    FCK_EQ(pos.x, 0x4200, "snapped to +0x2C.x then +0x200");
    FCK_EQ(pos.y, 0x4E00, "snapped to +0x2C.y then -0x200");
    FCK_EQ(pos.z, 0x6400, "snapped to +0x2C.z then +0x400");

    /* the same wrap with a far position keeps the accumulated value */
    lk_fill(&p, 0, (int)sizeof(p));
    p.unk00 = 1;
    p.unk08 = 0x100;
    p.unk0C = 0;
    p.unk10.x = 1; p.unk10.y = 0; p.unk10.z = 0;
    p.unk1C = recs;
    p.unk20 = recs;
    p.unk24 = 1;
    p.unk28 = 2;
    recs[0] = 0; recs[1] = 0; recs[2] = 0x1000;
    p.unk2C.x = 0x4000; p.unk2C.y = 0x5000; p.unk2C.z = 0x6000;
    pos.x = pos.y = pos.z = 0;
    ls_path_dist = 0x2000;
    ls_reset();
    FCK_EQ(gAdvanceLoopingPathPosition(&p, &pos, 0x300, &counter), 1, "wrap still reported");
    FCK_EQ(pos.x, 0x300, "position keeps x = 0x100 + 0x200");
    FCK_EQ(pos.y, -0x300, "position keeps the y accumulation");

    /* a null counter is allowed */
    lk_fill(&p, 0, (int)sizeof(p));
    p.unk00 = 1;
    p.unk08 = 0x100;
    p.unk10.x = 1;
    p.unk1C = recs;
    p.unk20 = recs;
    p.unk28 = 1;
    recs[2] = 0x40;
    pos.x = pos.y = pos.z = 0;
    ls_reset();
    FCK_EQ(gAdvanceLoopingPathPosition(&p, &pos, 0x200, 0), 1, "null counter accepted");
    FCK_EQ(pos.x, 0x200, "still advanced");

    /* two wraps in one call, the second one reports too */
    lk_fill(&p, 0, (int)sizeof(p));
    p.unk00 = 1;
    p.unk08 = 0x10;
    p.unk0C = 0;
    p.unk10.x = 1;
    p.unk1C = recs;
    p.unk20 = recs;
    p.unk24 = 0;
    p.unk28 = 1;
    recs[0] = 0; recs[1] = 0; recs[2] = 0x10;
    pos.x = pos.y = pos.z = 0;
    counter = 0;
    ls_path_dist = 0xFFFF;
    ls_reset();
    FCK_EQ(gAdvanceLoopingPathPosition(&p, &pos, 0x30, &counter), 1, "wrap reported");
    FCK_EQ(counter, 2, "two transitions");
    FCK_EQ(ls_count(LS_SCALE), 3, "three steps");
    FCK_EQ(ls_count(LS_DIST), 2, "two distance checks");
}

/* ============================================= 800537D0 hover move vector */

struct LsHover { u8 pad0[0xA0]; s32 fieldA0; u8 pad1[0x4]; s32 fieldA8; };

static s32 ls_hover_a3[4];
static s32 ls_hover_sep_x, ls_hover_sep_z;
static s32 ls_sep_calls;

void ls_stub_sep(struct LsHover *a0, s32 *out) __asm__("func_80053730");

void ls_stub_sep(struct LsHover *a0, s32 *out)
{
    ls_rec(LS_SEP, (s32)a0, (s32)out, 0, 0, 0, 0);
    ls_hover_sep_x = out[0];
    ls_hover_sep_z = out[2];
    ls_sep_calls++;
}

/* FMUL in the unit is (a*b)>>16 on the 64-bit product; all fixtures use 16.16
 * values (0x10000 = 1.0) so the expected components are exact products. */
/* @leaf 800537D0 source src/game/800537D0.c: a zero magnitude clears a3[0] and a3[2]; otherwise the two source fields at +0xA0/+0xA8 are rotated (a2 != 0: (A8, -A0)), left as they are (a2 == 0, a4 == 0), or combined after scaling the magnitude by 181/256 and testing it against 1: a4 == 1 gives (A0+A8, A8-A0) for a positive and (A0-A8, A8+A0) for a negative magnitude, a4 != 1 the exact opposite; the outputs are ((t4*t2)>>16) and ((a1*t3)>>16) and func_80053730 always runs last on the same outputs */
static void ls_test_hover_vector(void)
{
    struct LsHover h;

    LK_FUNC("func_800537D0");
    h.fieldA0 = 0x10000;   /* 1.0 */
    h.fieldA8 = 0x20000;   /* 2.0 */

    /* zero magnitude */
    ls_hover_a3[0] = 7; ls_hover_a3[2] = 9;
    ls_sep_calls = 0;
    ls_reset();
    gBuildHoverMovementVector(&h, 0, 0, ls_hover_a3, 0);
    FCK_EQ(ls_hover_a3[0], 0, "zero magnitude clears a3[0]");
    FCK_EQ(ls_hover_a3[2], 0, "zero magnitude clears a3[2]");
    FCK_EQ(ls_sep_calls, 1, "separation still called");
    FCK_EQ(ls_id(ls_n - 1), LS_SEP, "separation is the last call");
    FCK_EQ((s32)ls_call(LS_SEP, 0)->a0, (s32)&h, "separation gets the object");
    FCK_EQ((s32)ls_call(LS_SEP, 0)->a1, (s32)ls_hover_a3, "separation gets the outputs");

    /* plain 16.16 scaling */
    ls_reset();
    gBuildHoverMovementVector(&h, 0x8000, 0, ls_hover_a3, 0);
    FCK_EQ(ls_hover_a3[0], 0x8000, "0.5 * 1.0 = 0.5");
    FCK_EQ(ls_hover_a3[2], 0x10000, "0.5 * 2.0 = 1.0");

    /* quarter turn */
    ls_reset();
    gBuildHoverMovementVector(&h, 0x8000, 1, ls_hover_a3, 0);
    FCK_EQ(ls_hover_a3[0], 0x10000, "0.5 * 2.0 after the rotation");
    FCK_EQ(ls_hover_a3[2], -0x8000, "0.5 * -1.0 after the rotation");

    /* a4 == 1 with a positive scaled magnitude: (A0+A8, A8-A0) */
    ls_reset();
    gBuildHoverMovementVector(&h, 0x200, 0, ls_hover_a3, 1);
    FCK_EQ(ls_hover_a3[0], 362 * 3, "(0x200*181)>>8 = 362, times 3.0");
    FCK_EQ(ls_hover_a3[2], 362, "times 1.0");

    /* a4 != 1 with the same input mirrors the transform */
    ls_reset();
    gBuildHoverMovementVector(&h, 0x200, 0, ls_hover_a3, 2);
    FCK_EQ(ls_hover_a3[0], -(362), "times -1.0");
    FCK_EQ(ls_hover_a3[2], 362 * 3, "times 3.0");

    /* a4 == 1 with a negative magnitude: (A0-A8, A8+A0) */
    ls_reset();
    gBuildHoverMovementVector(&h, -0x200, 0, ls_hover_a3, 1);
    FCK_EQ(ls_hover_a3[0], 362, "-1.0 * -1.0");
    FCK_EQ(ls_hover_a3[2], -362 * 3, "-1.0 * 3.0");

    /* a4 != 1 with a negative magnitude mirrors that too */
    ls_reset();
    gBuildHoverMovementVector(&h, -0x200, 0, ls_hover_a3, 2);
    FCK_EQ(ls_hover_a3[0], -362 * 3, "-1.0 * 3.0");
    FCK_EQ(ls_hover_a3[2], -362, "-1.0 * 1.0");

    /* the separation step sees the final components */
    FCK_EQ(ls_hover_sep_x, -362 * 3, "separation saw a3[0]");
    FCK_EQ(ls_hover_sep_z, -362, "separation saw a3[2]");
    FCK_EQ(ls_count(LS_SEP), 1, "exactly one separation call per invocation");

    /* a diagonal request with a2 set ignores a4 */
    ls_hover_a3[0] = 0; ls_hover_a3[2] = 0;
    ls_reset();
    gBuildHoverMovementVector(&h, 0x10000, 1, ls_hover_a3, 1);
    FCK_EQ(ls_hover_a3[0], 0x20000, "rotation wins over a4");
    FCK_EQ(ls_hover_a3[2], -0x10000, "rotation on the second component");
}

/* ============================================== 8007D27C sound enable toggle */

u16 ls_sound_label[2] __asm__("D_8009BABE");

/* func_80046C84 and func_800451E0 are linked from mocks/sound_mock.c via
 * @mock sound_mock; only the local setter func_800451B4 lives here and
 * updates fcm_sound.stereo_mode while snapshotting prior shared-mock counts. */
void ls_stub_setmode(s32 enable) __asm__("func_800451B4");

void ls_stub_setmode(s32 enable)
{
    ls_rec(LS_SETMODE, enable, fcm_sound.sfx_calls, fcm_sound.stereo_mode_reads, 0, 0, 0);
    fcm_sound.stereo_mode = enable;
}

/* @leaf 8007D27C source src/game/8007D27C.c: only actions 1 and 2 act; they play cue (0x47, 0), write the inverted stereo enable through 800451B4, then pick the label id from a fresh 800451E0 read: 0x100E when enabled, 0x1018 when muted */
static void ls_test_sound_toggle(void)
{
    LK_FUNC("func_8007D27C");

    /* actions outside {1, 2} do nothing */
    fcm_sound_reset();
    fcm_sound.stereo_mode = 0;
    ls_sound_label[0] = 0xAAAA;
    ls_sound_label[1] = 0x5555;
    ls_reset();
    gToggleSoundEnable(0);
    FCK_EQ(ls_n, 0, "action 0 does not call setmode");
    FCK_EQ(fcm_sound.sfx_calls, 0, "action 0 plays no cue");
    FCK_EQ(fcm_sound.stereo_mode_reads, 0, "action 0 does not read stereo mode");
    FCK_EQ(ls_sound_label[0], 0xAAAA, "label untouched by action 0");
    FCK_EQ(ls_sound_label[1], 0x5555, "index 1 canary untouched by action 0");
    fcm_sound_reset();
    fcm_sound.stereo_mode = 0;
    ls_reset();
    gToggleSoundEnable(3);
    FCK_EQ(ls_n, 0, "action 3 does not call setmode");
    FCK_EQ(fcm_sound.sfx_calls, 0, "action 3 plays no cue");
    FCK_EQ(fcm_sound.stereo_mode_reads, 0, "action 3 does not read stereo mode");
    FCK_EQ(fcm_sound.stereo_mode, 0, "mode untouched by action 3");

    /* muted -> enabled */
    fcm_sound_reset();
    fcm_sound.stereo_mode = 0;
    ls_sound_label[0] = 0xAAAA;
    ls_sound_label[1] = 0x5555;
    ls_reset();
    gToggleSoundEnable(1);
    FCK_EQ(fcm_sound.sfx_calls, 1, "one cue played");
    FCK_EQ(fcm_sound.sfx_ids[0], 0x47, "cue id 0x47");
    FCK_EQ(fcm_sound.sfx_last_arg1, 0, "cue argument 0");
    FCK_EQ(ls_count(LS_SETMODE), 1, "setmode called once");
    FCK_EQ(ls_call(LS_SETMODE, 0)->a0, 1, "0 ^ 1 = 1");
    FCK_EQ(ls_call(LS_SETMODE, 0)->a1, 1, "cue played before setmode");
    FCK_EQ(ls_call(LS_SETMODE, 0)->a2, 1, "current mode read once before setmode");
    FCK_EQ(fcm_sound.stereo_mode_reads, 2, "mode read a second time after setmode for label");
    FCK_EQ(fcm_sound.stereo_mode, 1, "mode is now enabled");
    FCK_EQ(ls_sound_label[0], 0x100E, "enabled label 0x100E");
    FCK_EQ(ls_sound_label[1], 0x5555, "only index 0 written (index 1 canary intact)");

    /* enabled -> muted */
    fcm_sound_reset();
    fcm_sound.stereo_mode = 1;
    ls_sound_label[0] = 0xBBBB;
    ls_sound_label[1] = 0x6666;
    ls_reset();
    gToggleSoundEnable(2);
    FCK_EQ(ls_call(LS_SETMODE, 0)->a0, 0, "1 ^ 1 = 0");
    FCK_EQ(ls_call(LS_SETMODE, 0)->a1, 1, "action 2 plays cue before setmode");
    FCK_EQ(ls_call(LS_SETMODE, 0)->a2, 1, "action 2 reads mode before setmode");
    FCK_EQ(fcm_sound.stereo_mode_reads, 2, "action 2 reads mode twice");
    FCK_EQ(fcm_sound.stereo_mode, 0, "mode is now muted");
    FCK_EQ(ls_sound_label[0], 0x1018, "muted label 0x1018");
    FCK_EQ(ls_sound_label[1], 0x6666, "action 2 leaves index 1 canary intact");
    FCK_EQ(fcm_sound.sfx_calls, 1, "action 2 plays the cue too");

    /* a pre-set mode other than 0 or 1 still toggles bit 0 only */
    fcm_sound_reset();
    fcm_sound.stereo_mode = 2;
    ls_sound_label[0] = 0xCCCC;
    ls_sound_label[1] = 0x7777;
    ls_reset();
    gToggleSoundEnable(1);
    FCK_EQ(ls_call(LS_SETMODE, 0)->a0, 3, "2 ^ 1 = 3");
    FCK_EQ(ls_sound_label[0], 0x100E, "nonzero result takes the enabled label");
    FCK_EQ(ls_sound_label[1], 0x7777, "bit-0 toggle leaves index 1 canary intact");
}

/* =========================================== GPU packet builders (helpers) */

__asm__("\t.data\n"
        "\t.balign 8\n"
        "\t.globl D_8009DDF4\n"
        "D_8009DDF4:\n"
        "\t.half 0\n"
        "\t.globl D_8009DDF6\n"
        "D_8009DDF6:\n"
        "\t.half 0\n"
        "\t.half 0\n"
        "\t.half 0\n"
        "\t.text\n");

u32 gBuildDrawModeCommand(s32 dfe, s32 dtd, u32 tpage) __asm__("func_80093F58");
s32 gBuildDrawAreaCommands(s16 x, s16 y) __asm__("func_80093F78");
s32 gPackDrawAreaBottomRight(s32 x, s32 y) __asm__("func_80094010");
u32 gPackDrawOffset(u32 x, u32 y) __asm__("func_800940A8");
s32 gBuildTexWindowCommand(u8 *p) __asm__("func_800940C4");

extern u16 ls_max_w __asm__("D_8009DDF4");
extern u16 ls_max_h __asm__("D_8009DDF6");

/* The five packet builders under 0x80093F58..0x800940C4 are separate units in
 * src/game and are linked for real. Their GP0 bit layouts follow psx-spx
 * "GPU Rendering Attributes" and Psy-Q libgpu.h (_get_mode, SetDefDrawEnv /
 * SetDrawArea, SetDrawOffset, _get_tw / SetTexWindow); C API and clamping /
 * null-pointer details are separated below as source checks. */

/* @leaf 80093F58 spec psx-spx GPU Rendering Attributes GP0(E1h) Draw Mode + Psy-Q libgpu.h _get_mode/setDrawTPage: opcode 0xE1, bit 9 = dither (dtd), bit 10 = draw-to-display (dfe), bits 0-8 and 11 = texpage/texture-disable (0x9FF); source (src/game/80093F58.c): non-zero dtd/dfe normalization and tpage & 0x9FF masking */
static void ls_test_draw_mode(void)
{
    LK_FUNC("func_80093F58");
    /* spec: psx-spx GP0(E1h) / Psy-Q _get_mode bit layout */
    FCK_EQ(gBuildDrawModeCommand(0, 0, 0), 0xE1000000, "bare opcode");
    FCK_EQ(gBuildDrawModeCommand(1, 0, 0), 0xE1000400, "dfe sets bit 10");
    FCK_EQ(gBuildDrawModeCommand(0, 1, 0), 0xE1000200, "dtd sets bit 9");
    FCK_EQ(gBuildDrawModeCommand(1, 1, 0x123), 0xE1000723, "both flags plus the tpage");
    /* source (src/game/80093F58.c): C API boolean normalization and 0x9FF mask */
    FCK_EQ(gBuildDrawModeCommand(5, 5, 0), 0xE1000600, "any non-zero value counts as set");
    FCK_EQ(gBuildDrawModeCommand(0, 0, 0xFFFF), 0xE10009FF, "tpage masked to 0x9FF");
    FCK_EQ(gBuildDrawModeCommand(0, 0, 0x0A00), 0xE1000800, "bit 10 of the tpage is dropped");
}

/* @leaf 80093F78 spec psx-spx GPU Rendering Attributes GP0(E3h) Set Drawing Area top left + Psy-Q libgpu.h SetDrawArea/SetDefDrawEnv: opcode 0xE3, X1 in bits 0-9 (10-bit), Y1 in bits 10-19 (10-bit on newer 208-pin GPU; GPU-variant dependent, 9-bit bits 10-18 on older 160-pin GPU); source (src/game/80093F78.c): clamps x/y to [0, D_8009DDF4-1] / [0, D_8009DDF6-1] before 0x3FF masking */
static void ls_test_draw_area(void)
{
    LK_FUNC("func_80093F78");
    ls_max_w = 0x140;
    ls_max_h = 0x00F0;
    /* spec: psx-spx GP0(E3h) X1 bits 0-9, Y1 bits 10-19 (10-bit Y GPU-variant dependent) */
    FCK_EQ(gBuildDrawAreaCommands(0x40, 0x20), 0xE3008040, "x 0x40 y 0x20");
    FCK_EQ(gBuildDrawAreaCommands(0, 0), 0xE3000000, "origin");
    /* source (src/game/80093F78.c): clamping against D_8009DDF4/D_8009DDF6 and 10-bit mask */
    FCK_EQ(gBuildDrawAreaCommands(-1, -5), 0xE3000000, "negatives clamp to 0");
    FCK_EQ(gBuildDrawAreaCommands(0x200, 0x200), 0xE303BD3F, "oversize clamps to 0x13F/0xEF");
    FCK_EQ(gBuildDrawAreaCommands(0x13F, 0xEF), 0xE303BD3F, "the maximum itself is kept");
    ls_max_w = 0x800;
    ls_max_h = 0x800;
    FCK_EQ(gBuildDrawAreaCommands(0x7FF, 0x7FF), 0xE30FFFFF, "10-bit fields mask 0x7FF to 0x3FF");
}

/* @leaf 80094010 spec psx-spx GPU Rendering Attributes GP0(E4h) Set Drawing Area bottom right + Psy-Q libgpu.h SetDrawArea/SetDefDrawEnv: opcode 0xE4, X2 in bits 0-9 (10-bit), Y2 in bits 10-19 (10-bit on newer 208-pin GPU; GPU-variant dependent, 9-bit bits 10-18 on older 160-pin GPU); source (src/game/80094010.c): clamps (s16)x/(s16)y to [0, D_8009DDF4-1] / [0, D_8009DDF6-1] before 0x3FF masking */
static void ls_test_draw_area_bottom_right(void)
{
    LK_FUNC("func_80094010");
    ls_max_w = 0x140;
    ls_max_h = 0x00F0;
    /* spec: psx-spx GP0(E4h) X2 bits 0-9, Y2 bits 10-19 (10-bit Y GPU-variant dependent) */
    FCK_EQ(gPackDrawAreaBottomRight(0x7F, 0x4F), 0xE4013C7F, "x 0x7F y 0x4F");
    FCK_EQ(gPackDrawAreaBottomRight(0, 0), 0xE4000000, "origin");
    /* source (src/game/80094010.c): clamping against D_8009DDF4/D_8009DDF6 */
    FCK_EQ(gPackDrawAreaBottomRight(-1, -1), 0xE4000000, "negatives clamp to 0");
    FCK_EQ(gPackDrawAreaBottomRight(0x400, 0x200), 0xE403BD3F, "oversize clamps to 0x13F/0xEF");
    FCK_EQ(gPackDrawAreaBottomRight(0x10, 0x200), 0xE403BC10, "only the oversize axis clamps");
    FCK_EQ(gPackDrawAreaBottomRight(0x400, 0x10), 0xE400413F, "the other oversize axis clamps");
}

/* @leaf 800940A8 spec psx-spx GPU Rendering Attributes GP0(E5h) Set Drawing Offset + Psy-Q libgpu.h SetDrawOffset/SetDefDrawEnv: opcode 0xE5, 11-bit signed X in bits 0-10 and 11-bit signed Y in bits 11-21; source (src/game/800940A8.c): direct 0x7FF bit masking without clamping */
static void ls_test_draw_offset(void)
{
    LK_FUNC("func_800940A8");
    /* spec: psx-spx GP0(E5h) 11-bit X/Y packing */
    FCK_EQ(gPackDrawOffset(0, 0), 0xE5000000, "bare opcode");
    FCK_EQ(gPackDrawOffset(8, 0x10), 0xE5008008, "x 8 y 0x10");
    FCK_EQ(gPackDrawOffset(0x7FF, 0x7FF), 0xE53FFFFF, "both fields at their maximum");
    /* source (src/game/800940A8.c): upper-bit truncation via & 0x7FF */
    FCK_EQ(gPackDrawOffset(0x800, 0x800), 0xE5000000, "bit 11 is masked away");
    FCK_EQ(gPackDrawOffset(0x1234, 0x5678), 0xE533C234, "0x234 / 0x678 survive the mask");
}

/* @leaf 800940C4 spec psx-spx GPU Rendering Attributes GP0(E2h) Texture Window + Psy-Q libgpu.h _get_tw/SetTexWindow: opcode 0xE2, 5-bit fields in 8-pixel steps with Mask X in bits 0-4, Mask Y in bits 5-9, Offset X in bits 10-14, Offset Y in bits 15-19; source (src/game/800940C4.c): NULL returns 0, Offset X/Y come from bytes +0/+2 (>>3) and Mask X/Y from negated s16 at +4/+6 ((-v & 0xFF) >> 3) */
static void ls_test_tex_window(void)
{
    u8 p[16];

    LK_FUNC("func_800940C4");
    /* source (src/game/800940C4.c): NULL pointer guard */
    FCK_EQ(gBuildTexWindowCommand(0), 0, "null pointer gives 0");

    /* spec: psx-spx GP0(E2h) Mask X (0-4), Mask Y (5-9), Offset X (10-14), Offset Y (15-19) */
    lk_fill(p, 0, (int)sizeof(p));
    p[0] = 0x10;              /* Offset X = 2 (bits 10-14) */
    p[2] = 0x20;              /* Offset Y = 4 (bits 15-19) */
    *(s16 *)(p + 4) = -8;     /* Mask X = (-(-8) & 0xFF) >> 3 = 1 (bits 0-4) */
    *(s16 *)(p + 6) = 0x40;   /* Mask Y = (-0x40 & 0xFF) >> 3 = 24 (bits 5-9) */
    FCK_EQ(gBuildTexWindowCommand(p), 0xE2020B01, "Offset Y=4, Offset X=2, Mask Y=24, Mask X=1");

    lk_fill(p, 0xFF, (int)sizeof(p));
    p[0] = 0; p[2] = 0;
    *(s16 *)(p + 4) = 0;
    *(s16 *)(p + 6) = 0;
    FCK_EQ(gBuildTexWindowCommand(p), 0xE2000000, "all fields zero");

    /* source (src/game/800940C4.c): byte/s16 truncation and >>3 step quantization */
    lk_fill(p, 0, (int)sizeof(p));
    p[0] = 0xFF;              /* Offset X = 0x1F (bits 10-14) */
    p[2] = 0xFF;              /* Offset Y = 0x1F (bits 15-19) */
    *(s16 *)(p + 4) = 0x100;  /* Mask X = (-0x100 & 0xFF) >> 3 = 0 (bits 0-4) */
    *(s16 *)(p + 6) = 8;      /* Mask Y = (-8 & 0xFF) >> 3 = 0x1F (bits 5-9) */
    FCK_EQ(gBuildTexWindowCommand(p), 0xE20FFFE0, "Offset Y=0x1F, Offset X=0x1F, Mask Y=0x1F, Mask X=0");

    lk_fill(p, 0, (int)sizeof(p));
    p[0] = 0x07;              /* Offset X = 0, below one 8-px step */
    p[2] = 0x08;              /* Offset Y = 1 (bits 15-19) */
    *(s16 *)(p + 4) = 0x7F;   /* Mask X = (-0x7F & 0xFF) >> 3 = 0x10 (bits 0-4) */
    *(s16 *)(p + 6) = -0x100; /* Mask Y = (0x100 & 0xFF) >> 3 = 0 (bits 5-9) */
    FCK_EQ(gBuildTexWindowCommand(p), 0xE2008010, "Offset Y = 1 (bits 15-19), Mask X = 0x10 (bits 0-4)");
}

/* =============================================== 80093CE8 rectangle draw list */

/* The five command builders are linked for real (src/game/80093F58.c and
 * friends), so the expected words below are decoded by hand from the PSX GPU
 * packet layout: E1h draw mode, E2h texture window, E3h/E4h draw area, E5h
 * drawing offset, E6h mask reset, and 0x60/0x02 polygons carrying a 24-bit
 * colour. */
static u8 ls_rect_obj[0x40];
static u8 ls_rect[0x40];

static void ls_rect_init(void)
{
    lk_fill(ls_rect_obj, 0xCC, (int)sizeof(ls_rect_obj));
    lk_fill(ls_rect, 0, (int)sizeof(ls_rect));
    *(s16 *)(ls_rect + 0x00) = 0x40;        /* x */
    *(s16 *)(ls_rect + 0x02) = 0x20;        /* y */
    *(u16 *)(ls_rect + 0x04) = 0x40;        /* w */
    *(u16 *)(ls_rect + 0x06) = 0x30;        /* h */
    *(s16 *)(ls_rect + 0x08) = 0x08;        /* u */
    *(s16 *)(ls_rect + 0x0A) = 0x10;        /* v */
    ls_rect[0x0C] = 0x10;                   /* texwindow u0 */
    ls_rect[0x0E] = 0x20;                   /* texwindow v0 */
    *(s16 *)(ls_rect + 0x10) = -8;          /* texwindow u1 */
    *(s16 *)(ls_rect + 0x12) = 0x40;        /* texwindow v1 */
    *(u16 *)(ls_rect + 0x14) = 0x0123;      /* tpage */
    ls_rect[0x16] = 1;                      /* dtd */
    ls_rect[0x17] = 1;                      /* dfe */
    ls_rect[0x18] = 0;                      /* fill enable */
    ls_rect[0x19] = 0x11;                   /* red */
    ls_rect[0x1A] = 0x22;                   /* green */
    ls_rect[0x1B] = 0x33;                   /* blue */
}

static u32 ls_word(const u8 *p, int off)
{
    return (u32)p[off] | ((u32)p[off + 1] << 8)
         | ((u32)p[off + 2] << 16) | ((u32)p[off + 3] << 24);
}

/* @leaf 80093CE8 source src/game/80093CE8.c: writes six GPU packets into obj+4..obj+0x18 (E3h from x/y, E4h from x+w-1/y+h-1, E5h from u/v, E1h from dfe/dtd/tpage, E2h from rect+0xC, E6h constant), sets obj[3] = 6, and with rect[0x18] != 0 clamps w/h to D_8009DDF4/DDF6-1 and appends a 0x60 textured or 0x02 fill packet carrying the RGB bytes plus the size pair, raising the count to 9 */
static void ls_test_rect_list(void)
{
    LK_FUNC("func_80093CE8");
    ls_max_w = 0x140;
    ls_max_h = 0x00F0;

    /* the fixed six commands, no extra packet */
    ls_rect_init();
    gBuildRectDrawList(ls_rect_obj, ls_rect);
    FCK_EQ(ls_word(ls_rect_obj, 0x04), 0xE3008040, "E3h draw area x 0x40 y 0x20");
    FCK_EQ(ls_word(ls_rect_obj, 0x08), 0xE4013C7F, "E4h bottom right 0x7F/0x4F");
    FCK_EQ(ls_word(ls_rect_obj, 0x0C), 0xE5008008, "E5h offset u 8 v 0x10");
    FCK_EQ(ls_word(ls_rect_obj, 0x10), 0xE1000723, "E1h dfe+dtd+tpage 0x123");
    FCK_EQ(ls_word(ls_rect_obj, 0x14), 0xE2020B01, "E2h texwindow");
    FCK_EQ(ls_word(ls_rect_obj, 0x18), 0xE6000000, "E6h mask reset");
    FCK_EQ(ls_rect_obj[3], 6, "count 6 without a rectangle");
    FCK_CHECK(lk_all(ls_rect_obj, 0x1C, 0x40, 0xCC), "no packet past the six commands");

    /* 64-aligned rectangle: VRAM fill */
    ls_rect_init();
    ls_rect[0x18] = 1;
    gBuildRectDrawList(ls_rect_obj, ls_rect);
    FCK_EQ(ls_rect_obj[3], 9, "count 9 with a rectangle");
    FCK_EQ(ls_word(ls_rect_obj, 0x1C), 0x02332211, "0x02 fill packet word");
    FCK_EQ(ls_word(ls_rect_obj, 0x20), 0x00200040, "x 0x40 in the low half, y 0x20 high");
    FCK_EQ(ls_word(ls_rect_obj, 0x24), 0x00300040, "w 0x40 h 0x30");
    FCK_CHECK(lk_all(ls_rect_obj, 0x28, 0x40, 0xCC), "nothing written past the rectangle");

    /* an unaligned origin takes the textured path with the window compensation */
    ls_rect_init();
    ls_rect[0x18] = 1;
    *(s16 *)(ls_rect + 0x00) = 0x41;
    gBuildRectDrawList(ls_rect_obj, ls_rect);
    FCK_EQ(ls_word(ls_rect_obj, 0x1C), 0x60332211, "0x60 sprite packet word");
    FCK_EQ(ls_word(ls_rect_obj, 0x20), 0x00100039, "x 0x41-u 8, y 0x20-v 0x10");
    FCK_EQ(ls_word(ls_rect_obj, 0x24), 0x00300040, "size unchanged");

    /* an unaligned width takes it too */
    ls_rect_init();
    ls_rect[0x18] = 1;
    *(u16 *)(ls_rect + 0x04) = 0x41;
    gBuildRectDrawList(ls_rect_obj, ls_rect);
    FCK_EQ(ls_word(ls_rect_obj, 0x1C), 0x60332211, "width 0x41 -> sprite packet");

    /* sizes are clamped to the maximum, the E4h arguments are not */
    ls_rect_init();
    ls_rect[0x18] = 1;
    *(u16 *)(ls_rect + 0x04) = 0x400;
    *(u16 *)(ls_rect + 0x06) = 0x200;
    gBuildRectDrawList(ls_rect_obj, ls_rect);
    FCK_EQ(ls_word(ls_rect_obj, 0x08), 0xE403BD3F, "E4h clamps 0x43F/0x21F to 0x13F/0xEF");
    FCK_EQ(ls_word(ls_rect_obj, 0x24), 0x00EF013F, "size clamped to 0x13F x 0xEF");
    FCK_EQ(ls_word(ls_rect_obj, 0x20), 0x00100038, "x 0x40-u 8, y 0x20-v 0x10");

    /* a negative width collapses to 0 and becomes an aligned fill */
    ls_rect_init();
    ls_rect[0x18] = 1;
    *(u16 *)(ls_rect + 0x04) = 0xFFFF;
    *(u16 *)(ls_rect + 0x06) = 0xFFFF;
    gBuildRectDrawList(ls_rect_obj, ls_rect);
    FCK_EQ(ls_word(ls_rect_obj, 0x24), 0, "negative size clamped to 0");
    FCK_EQ(ls_word(ls_rect_obj, 0x1C), 0x02332211, "aligned fill packet");

    /* a negative origin is clamped in the draw area commands */
    ls_rect_init();
    ls_rect[0x18] = 1;
    *(s16 *)(ls_rect + 0x00) = -1;
    *(s16 *)(ls_rect + 0x02) = -1;
    gBuildRectDrawList(ls_rect_obj, ls_rect);
    FCK_EQ(ls_word(ls_rect_obj, 0x04), 0xE3000000, "E3h clamps -1/-1 to 0/0");
    FCK_EQ(ls_word(ls_rect_obj, 0x1C), 0x60332211, "unaligned start -> sprite packet");
    FCK_EQ(ls_word(ls_rect_obj, 0x20), 0xFFEFFFF7, "0xFFFF-8 low, 0xFFFF-0x10 high");

    /* the colour bytes scale with the multiply factor 1, not the alpha bit */
    ls_rect_init();
    ls_rect[0x18] = 1;
    ls_rect[0x19] = 0xFF;
    ls_rect[0x1A] = 0x80;
    ls_rect[0x1B] = 0x01;
    gBuildRectDrawList(ls_rect_obj, ls_rect);
    FCK_EQ(ls_word(ls_rect_obj, 0x1C), 0x020180FF, "0x02 | 0x01<<16 | 0x80<<8 | 0xFF");

    /* tpage masking and the dfe/dtd bits */
    ls_rect_init();
    *(u16 *)(ls_rect + 0x14) = 0x0FFF;
    ls_rect[0x16] = 0;
    ls_rect[0x17] = 0;
    gBuildRectDrawList(ls_rect_obj, ls_rect);
    FCK_EQ(ls_word(ls_rect_obj, 0x10), 0xE10009FF, "tpage & 0x9FF, no dfe/dtd");
    ls_rect[0x16] = 0;
    ls_rect[0x17] = 1;
    gBuildRectDrawList(ls_rect_obj, ls_rect);
    FCK_EQ(ls_word(ls_rect_obj, 0x10), 0xE1000DFF, "dfe alone sets 0x400");
    ls_rect[0x16] = 1;
    ls_rect[0x17] = 0;
    gBuildRectDrawList(ls_rect_obj, ls_rect);
    FCK_EQ(ls_word(ls_rect_obj, 0x10), 0xE1000BFF, "dtd alone sets 0x200");
}

/* ============================================== 80098FE8 SPU common attributes */

struct LsSpuRegs {
    u8 pad00[0x180];
    u16 mvolL, mvolR;
    u8 pad184[0x26];
    u16 control;
    u8 pad1AC[4];
    u16 cdL, cdR, extL, extR;
    u8 pad1B8[0x48];
};

struct LsSpuVol { s16 left, right; };
struct LsSpuExt { struct LsSpuVol volume; s32 reverb; s32 mix; };
struct LsSpuCommon {
    u32 mask;
    struct LsSpuVol mvol;
    struct LsSpuVol mvolmode;
    struct LsSpuVol mvolx;
    struct LsSpuExt cd;
    struct LsSpuExt ext;
};

static u8 ls_spu_block[0x200];
u8 *ls_spu_base __asm__("D_8009EEA4");

static struct LsSpuRegs *ls_regs(void) { return (struct LsSpuRegs *)ls_spu_block; }

static void ls_spu_reset(void)
{
    lk_fill(ls_spu_block, 0, (int)sizeof(ls_spu_block));
    ls_spu_base = ls_spu_block;
    ls_reset();
}

/* @leaf 80098FE8 source src/game/80098FE8.c: mask 0 means every field; bits 1/2 gate the main volumes with bits 4/8 selecting the 0x8000..0xE000 mode encoding (0..7) and clamping through the signed +0x80 and negative cases, bits 0x40/0x80/0x400/0x800 write the four CD/ext words verbatim, and bits 0x100/0x200/0x1000/0x2000 set or clear control bits 2/0/3/1 from the cd/ext reverb and mix flags */
static void ls_test_spu_common(void)
{
    struct LsSpuCommon a;
    struct LsSpuRegs *r;

    LK_FUNC("func_80098FE8");
    ls_spu_reset();
    r = ls_regs();

    /* the register layout the harness assumes, straight from the declared
     * struct: main volumes at 0x180, control at 0x1AA, cd/ext at 0x1B0 */
    FCK_EQ((int)((u8 *)&r->mvolL - ls_spu_block), 0x180, "main volume left offset");
    FCK_EQ((int)((u8 *)&r->control - ls_spu_block), 0x1AA, "control offset");
    FCK_EQ((int)((u8 *)&r->cdL - ls_spu_block), 0x1B0, "cd volume left offset");
    FCK_EQ((int)((u8 *)&r->extR - ls_spu_block), 0x1B6, "ext volume right offset");

    /* a single main volume with mode 0 */
    lk_fill(&a, 0, (int)sizeof(a));
    r->mvolL = 0x1111;
    r->mvolR = 0x2222;
    a.mask = 1;
    a.mvol.left = 0x0123;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->mvolL, 0x0123, "left volume copied");
    FCK_EQ(r->mvolR, 0x2222, "right volume untouched");

    /* bits 1+4 with mode 3: the 0xA000 encoding and the 0x7FFF mask */
    ls_spu_reset();
    lk_fill(&a, 0, (int)sizeof(a));
    a.mask = 1 | 4;
    a.mvol.left = 0x40;
    a.mvolmode.left = 3;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->mvolL, 0xA040, "mode 3 adds 0xA000");

    a.mvol.left = 0x1234;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->mvolL, 0xA07F, "0x1234 >= 0x80 clamps to 0x7F");

    a.mvol.left = -1;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->mvolL, 0xA000, "negative volume clamps to 0");

    /* every mode code */
    ls_spu_reset();
    lk_fill(&a, 0, (int)sizeof(a));
    a.mask = 1 | 4;
    a.mvol.left = 0;
    {
        static const u16 modes[8] = {0x0000, 0x8000, 0x9000, 0xA000,
                                     0xB000, 0xC000, 0xD000, 0xE000};
        int m;
        for (m = 0; m < 8; m++) {
            a.mvolmode.left = (s16)m;
            gSpuSetCommonAttributes(&a);
            FCK_EQ(r->mvolL, modes[m], "mode code table");
        }
    }

    /* bit 1 without bit 4 does not consult the mode */
    ls_spu_reset();
    lk_fill(&a, 0, (int)sizeof(a));
    a.mask = 1;
    a.mvol.left = 0x40;
    a.mvolmode.left = 7;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->mvolL, 0x40, "no mode without bit 4");

    /* right channel mirrors with bits 2+8 */
    ls_spu_reset();
    lk_fill(&a, 0, (int)sizeof(a));
    a.mask = 2 | 8;
    a.mvol.right = 0x10;
    a.mvolmode.right = 5;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->mvolR, 0xC010, "right volume mode 5");
    FCK_EQ(r->mvolL, 0, "left volume untouched");

    /* mask 0: every field, including the words and the control bits */
    ls_spu_reset();
    lk_fill(&a, 0, (int)sizeof(a));
    a.mask = 0;
    a.mvol.left = 0x20;
    a.mvol.right = 0x30;
    a.cd.volume.left = 0x0101;
    a.cd.volume.right = 0x0202;
    a.ext.volume.left = 0x0303;
    a.ext.volume.right = 0x0404;
    a.cd.reverb = 1; a.cd.mix = 1;
    a.ext.reverb = 1; a.ext.mix = 1;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->mvolL, 0x20, "set-all left");
    FCK_EQ(r->mvolR, 0x30, "set-all right");
    FCK_EQ(r->cdL, 0x0101, "cd left verbatim");
    FCK_EQ(r->cdR, 0x0202, "cd right verbatim");
    FCK_EQ(r->extL, 0x0303, "ext left verbatim");
    FCK_EQ(r->extR, 0x0404, "ext right verbatim");
    FCK_EQ(r->control, 0x0F, "all four control bits set");

    /* the control bits are set and cleared individually */
    ls_spu_reset();
    r->control = 0x0F;
    lk_fill(&a, 0, (int)sizeof(a));
    a.mask = 0x100;
    a.cd.reverb = 0;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->control, 0x0B, "cd reverb clears bit 2");
    a.cd.mix = 0;
    a.mask = 0x200;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->control, 0x0A, "cd mix clears bit 0");
    a.ext.reverb = 0;
    a.mask = 0x1000;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->control, 0x02, "ext reverb clears bit 3");
    a.ext.mix = 0;
    a.mask = 0x2000;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->control, 0x00, "ext mix clears bit 1");
    a.cd.reverb = 1;
    a.mask = 0x100;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->control, 0x04, "cd reverb sets bit 2 again");

    /* single word masks reach only their own register */
    ls_spu_reset();
    lk_fill(&a, 0, (int)sizeof(a));
    a.mask = 0x40;
    a.cd.volume.left = 0xBEEF;
    a.cd.volume.right = 0xDEAD;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->cdL, 0xBEEF, "cd left written");
    FCK_EQ(r->cdR, 0, "cd right untouched");
    FCK_EQ(r->mvolL | r->mvolR | r->extL | r->extR, 0, "no other volume written");
    FCK_EQ(r->control, 0, "control untouched");

    a.mask = 0x800;
    a.ext.volume.right = 0x0F0F;
    gSpuSetCommonAttributes(&a);
    FCK_EQ(r->extR, 0x0F0F, "ext right written");
    FCK_EQ(r->extL, 0, "ext left untouched");
}

/* ================================================ 80099C58 SPU voice attributes */

struct LsVoiceAttr {
    u32 voice;
    u32 mask;
    struct LsSpuVol volume;
    struct LsSpuVol volmode;
    struct LsSpuVol volumex;
    u16 pitch;
    u16 note;
    u16 sample_note;
    s16 envx;
    u32 addr;
    u32 loop_addr;
    s32 a_mode;
    s32 s_mode;
    s32 r_mode;
    u16 ar;
    u16 dr;
    u16 sr;
    u16 rr;
    u16 sl;
    u16 adsr1;
    u16 adsr2;
};

u16 ls_sample_note_tbl[0x20] __asm__("D_8009EA60");

static u32 ls_conv_ret;
static u32 ls_conv_args[4];
static s32 ls_conv_calls;

u32 ls_stub_convert(s32 a, s32 b, s32 c, s32 d) __asm__("func_800999F4");
void ls_stub_encode(s32 reg, u32 addr) __asm__("func_80097808");

u32 ls_stub_convert(s32 a, s32 b, s32 c, s32 d)
{
    ls_rec(LS_NOTE2PITCH, a, b, c, d, 0, 0);
    ls_conv_args[0] = (u32)a; ls_conv_args[1] = (u32)b;
    ls_conv_args[2] = (u32)c; ls_conv_args[3] = (u32)d;
    ls_conv_calls++;
    return ls_conv_ret;
}

void ls_stub_encode(s32 reg, u32 addr)
{
    ls_rec(LS_ENCODE, reg, (s32)addr, 0, 0, 0, 0);
}

static u16 *ls_vreg(int ch, int i)
{
    return (u16 *)(ls_spu_block + ch * 16 + i * 2);
}

static void ls_attr_reset(struct LsVoiceAttr *a)
{
    lk_fill(a, 0, (int)sizeof(*a));
    a->sample_note = 0x2000;
    a->note = 0x3C00;
    a->pitch = 0x1111;
    a->volume.left = 0x30;
    a->volume.right = 0x40;
    a->addr = 0x0100;
    a->loop_addr = 0x0200;
    a->adsr1 = 0x80FF;
    a->adsr2 = 0x40DF;
    a->ar = 0x10; a->a_mode = 0;
    a->dr = 5;
    a->sr = 0x20; a->s_mode = 0;
    a->rr = 9; a->r_mode = 0;
    a->sl = 3;
    ls_conv_ret = 0x1234;
}

/* @leaf 80099C58 source src/game/80099C58.c: mask 0 means every field; bit 0x10 writes the voice pitch word +2, 0x40 the channel's D_8009EA60 sample note, 0x20 recomputes +2 through 800999F4(note>>8, note&0xFF, arg note>>8, arg note&0xFF), 1/2 write the volume words with bits 4/8 selecting the 0x8000..0xE000 mode and clamping at 0x80 and below zero, 0x80/0x10000 call 80097808 with ch*8|3 / ch*8|7, 0x20000/0x40000 write the envelope words +4/+5, and 0x800/0x1000/0x2000/0x4000/0x8000 rebuild +4/+5 from the rate and mode fields with the documented masks (0xFF, 0xFF0F, 0x3F, 0xFFC0, 0xFFF0) and bounds 0x7F, 0xF, 0x7F, 0x1F, 0xF */
static void ls_test_spu_voice(void)
{
    struct LsVoiceAttr a;
    int i;

    LK_FUNC("func_80099C58");
    ls_spu_reset();
    for (i = 0; i < 0x20; i++) ls_sample_note_tbl[i] = 0;

    /* pitch only */
    ls_attr_reset(&a);
    a.mask = 0x10;
    gSpuSetVoiceAttributesOne(2, &a);
    FCK_EQ(*ls_vreg(2, 2), 0x1111, "pitch into raw[ch*8+2]");
    FCK_EQ(*ls_vreg(2, 0), 0, "volume untouched");
    FCK_EQ(ls_conv_calls, 0, "no conversion on mask 0x10");

    /* sample note only */
    ls_attr_reset(&a);
    a.mask = 0x40;
    a.sample_note = 0x3000;
    gSpuSetVoiceAttributesOne(3, &a);
    FCK_EQ(ls_sample_note_tbl[3], 0x3000, "sample note into D_8009EA60[3]");
    FCK_EQ(ls_sample_note_tbl[2], 0, "neighbour untouched");

    /* note conversion: table word and argument split into coarse/fine */
    ls_spu_reset();
    ls_sample_note_tbl[1] = 0x1234;
    ls_attr_reset(&a);
    a.mask = 0x20;
    a.note = 0xABCD;
    gSpuSetVoiceAttributesOne(1, &a);
    FCK_EQ(ls_conv_calls, 1, "one conversion");
    FCK_EQ((s32)ls_conv_args[0], 0x12, "table note coarse");
    FCK_EQ((s32)ls_conv_args[1], 0x34, "table note fine");
    FCK_EQ((s32)ls_conv_args[2], 0xAB, "argument note coarse");
    FCK_EQ((s32)ls_conv_args[3], 0xCD, "argument note fine");
    FCK_EQ(*ls_vreg(1, 2), 0x1234, "converted pitch written");

    /* left volume with mode 2 and the clamps */
    ls_spu_reset();
    ls_attr_reset(&a);
    a.mask = 1 | 4;
    a.volume.left = 0x40;
    a.volmode.left = 2;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 0), 0x9040, "mode 2 adds 0x9000");
    a.volume.left = 0x100;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 0), 0x907F, "0x100 clamps to 0x7F");
    a.volume.left = -2;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 0), 0x9000, "negative clamps to 0");
    a.volume.left = 0x3FFF;
    a.volmode.left = 0;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 0), 0x3FFF, "mode 0 keeps the 0x7FFF mask");

    /* right volume without mode */
    ls_spu_reset();
    ls_attr_reset(&a);
    a.mask = 2;
    a.volume.right = 0x1234;
    a.volmode.right = 7;
    gSpuSetVoiceAttributesOne(1, &a);
    FCK_EQ(*ls_vreg(1, 1), 0x1234, "bit 2 alone ignores the mode");

    /* addresses go through the encoder with the channel indexes */
    ls_spu_reset();
    ls_attr_reset(&a);
    a.mask = 0x80 | 0x10000;
    gSpuSetVoiceAttributesOne(3, &a);
    FCK_EQ(ls_count(LS_ENCODE), 2, "two encoder calls");
    FCK_EQ(ls_call(LS_ENCODE, 0)->a0, 27, "ch*8 | 3");
    FCK_EQ((u32)ls_call(LS_ENCODE, 0)->a1, 0x0100, "start address");
    FCK_EQ(ls_call(LS_ENCODE, 1)->a0, 31, "ch*8 | 7");
    FCK_EQ((u32)ls_call(LS_ENCODE, 1)->a1, 0x0200, "loop address");

    /* envelope words written verbatim */
    ls_spu_reset();
    ls_attr_reset(&a);
    a.mask = 0x20000 | 0x40000;
    gSpuSetVoiceAttributesOne(4, &a);
    FCK_EQ(*ls_vreg(4, 4), 0x80FF, "adsr1 word");
    FCK_EQ(*ls_vreg(4, 5), 0x40DF, "adsr2 word");

    /* AR rebuild: low byte kept, (ar|0x80)<<8, a_mode 5 adds 0x80 */
    ls_spu_reset();
    *ls_vreg(0, 4) = 0xABCD;
    ls_attr_reset(&a);
    a.mask = 0x800 | 0x100;
    a.ar = 0x10;
    a.a_mode = 5;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 4), 0x90CD, "low byte 0xCD kept, (0x10|0x80)<<8");

    a.a_mode = 4;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 4), 0x10CD, "a_mode 4 adds no bit");

    a.ar = 0x90;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 4), 0x7FCD, "ar clamps to 0x7F");

    /* DR: nibble 4 of the same word */
    ls_spu_reset();
    *ls_vreg(0, 4) = 0xFFFF;
    ls_attr_reset(&a);
    a.mask = 0x1000;
    a.dr = 5;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 4), 0xFF5F, "nibble 4 replaced by 5");
    a.dr = 0x20;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 4), 0xFFFF, "dr clamps to 0xF and fills the nibble");
    a.dr = 0x1F;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 4), 0xFFFF, "dr 0x1F clamps to 0xF");

    /* SL: low nibble */
    ls_spu_reset();
    *ls_vreg(0, 4) = 0xFFFF;
    ls_attr_reset(&a);
    a.mask = 0x8000;
    a.sl = 3;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 4), 0xFFF3, "low nibble replaced by 3");
    a.sl = 0x20;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 4), 0xFFFF, "sl clamps to 0xF and fills the nibble");

    /* SR: 6 bits at 6, mode default 0x100, s_mode 1/5/7 */
    ls_spu_reset();
    *ls_vreg(0, 5) = 0xFFFF;
    ls_attr_reset(&a);
    a.mask = 0x2000;
    a.sr = 0x20;
    a.s_mode = 0;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 5), 0x483F, "(0x20|0x100)<<6 = 0x4800 | 0x3F");
    a.mask = 0x2000 | 0x200;
    a.s_mode = 1;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 5), 0x083F, "(0x20|0)<<6 = 0x800 | 0x3F");
    a.s_mode = 5;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 5), 0x883F, "(0x20|0x200)<<6 = 0x8800 | 0x3F");
    a.s_mode = 7;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 5), 0xC83F, "(0x20|0x300)<<6 = 0xC800 | 0x3F");

    /* RR: 5 bits at 0, mode 0x20 for r_mode 7 */
    ls_spu_reset();
    *ls_vreg(0, 5) = 0xFFFF;
    ls_attr_reset(&a);
    a.mask = 0x4000;
    a.rr = 9;
    a.r_mode = 0;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 5), 0xFFC9, "9 in the low five bits");
    a.mask = 0x4000 | 0x400;
    a.r_mode = 7;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 5), 0xFFE9, "0x20 added for mode 7");
    a.r_mode = 3;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 5), 0xFFC9, "mode 3 adds nothing");
    a.rr = 0x40;
    gSpuSetVoiceAttributesOne(0, &a);
    FCK_EQ(*ls_vreg(0, 5), 0xFFDF, "rr clamps to 0x1F");

    /* mask 0: everything at once, channel 23 */
    ls_spu_reset();
    ls_sample_note_tbl[23] = 0x1122;
    ls_attr_reset(&a);
    a.mask = 0;
    a.volume.left = 0x10; a.volmode.left = 2;
    a.volume.right = 0x3FFF; a.volmode.right = 0;
    a.ar = 5; a.a_mode = 5;
    a.dr = 3; a.sl = 7;
    a.sr = 0x10; a.s_mode = 5;
    a.rr = 3; a.r_mode = 7;
    ls_conv_ret = 0x1ABC;
    gSpuSetVoiceAttributesOne(23, &a);
    FCK_EQ(*ls_vreg(23, 2), 0x1ABC, "set-all: converted pitch wins over the raw pitch");
    FCK_EQ(ls_sample_note_tbl[23], 0x2000, "set-all: sample note replaced");
    FCK_EQ(*ls_vreg(23, 0), 0x9010, "set-all: left volume with mode");
    FCK_EQ(*ls_vreg(23, 1), 0x3FFF, "set-all: right volume");
    FCK_EQ(ls_count(LS_ENCODE), 2, "set-all: both addresses");
    FCK_EQ(ls_call(LS_ENCODE, 0)->a0, 187, "set-all: 23*8 | 3");
    FCK_EQ(*ls_vreg(23, 4), 0x8537, "set-all: adsr1 then AR/AMODE, DR, SL");
    FCK_EQ(*ls_vreg(23, 5), 0x8423, "set-all: adsr2 then SR/SMODE, RR/RMODE");
    FCK_EQ(*ls_vreg(22, 4), 0, "voice 22 untouched");
    FCK_EQ(*ls_vreg(22, 5), 0, "voice 22 untouched");
}

int main(void)
{
    ls_test_anim();
    ls_test_card_format();
    ls_test_entity_field_match();
    ls_test_map_zone();
    ls_test_spawn_chains();
    ls_test_anchor_sync();
    ls_test_weapon_sound();
    ls_test_path_curve();
    ls_test_path_advance();
    ls_test_hover_vector();
    ls_test_sound_toggle();
    ls_test_draw_mode();
    ls_test_draw_area();
    ls_test_draw_area_bottom_right();
    ls_test_draw_offset();
    ls_test_tex_window();
    ls_test_rect_list();
    ls_test_spu_common();
    ls_test_spu_voice();
    return lk_done("leaf_systems");
}



