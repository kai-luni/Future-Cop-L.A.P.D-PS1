/* Source characterizations of 25 unchanged src/game units on synthetic RAM.
 * Suite: leaf_century_a (Category A: pure CPU, no BIOS/GPU/SPU/CD/GTE).
 * Every @leaf unit is distinct from all other harnesses in tests/host/.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* Real helpers already covered elsewhere or needed for linking (not counted
 * as @leaf coverage here). Relative paths also work in mutate's snapshot. */
#include "../../src/game/8003BF1C.c"
#include "../../src/game/80019A20.c"
#include "../../src/game/8003CA80.c"
#include "../../src/game/80013AF0.c"
#include "../../src/game/80095C30.c"
#include "../../src/game/8001A074.c"
#include "../../src/game/8001E318.c"

/* Synthetic host-RAM globals referenced by the tested units & helpers. */
void **g_tex_desc_tbl   __asm__("D_8009F7D8");
s32    g_desc_reg_cnt   __asm__("D_8009F848");
void  *g_mov_aud_state  __asm__("D_8009F4D8");
void  *g_irq_cb_tbl     __asm__("D_8009DD10");
void  *g_vram_cb_tbl    __asm__("D_8009DDE8");
int   *g_mdec_ctrl_ptr  __asm__("D_8009E9F4");
void  *g_pad_lookup_fn  __asm__("D_8009C73C");
void  *g_script_tmr_hd  __asm__("D_8009F770");
s32    g_script_tmr_cnt __asm__("D_8009F774");
s32    g_pkt_limit      __asm__("D_8009F9C0");
s32    g_prim_next      __asm__("D_8009F9F0");
s32    g_prim_end       __asm__("D_8009FA28");
s32    g_pkt_cursor     __asm__("D_8009FA90");
s32    g_id_mat[9]      __asm__("D_8009AA38");
s16    g_atan_tbl[1025] __asm__("D_8009E0A0");
s32    g_player_cnt[2]  __asm__("D_8009FA0C");
void  *g_game_sess[2]   __asm__("D_8009FA94");
void  *g_player_ents[2] __asm__("D_8009FA58");
s32    g_cam_near_base  __asm__("D_8009FA64");
u8     g_src_blk20[32]  __asm__("D_8009AFA0");
s32    g_scr_mode_hook  __asm__("D_8009F99C");
u8     g_frm_cb_en      __asm__("D_8009FA20");

struct A8F90_Entity {
    struct A8F90_Entity *next;
    s32 (*update)(s32 message, struct A8F90_Entity *entity, s32 arg2, s32 arg3);
    s32 id;
};
struct A8F90_Table {
    s32 count;
    s32 unk04;
    struct A8F90_Entity *items[8];
};
struct A8F90_Table g_cmd_list_hd __asm__("D_800A8F90");

/* Forward declarations of the 25 unmodified units under test. */
extern void func_8005A2B0(void);
extern void func_8006A880(void);
extern u8  *func_80086F44(void);
extern void func_80091480(void);
extern void func_800914B0(void);
extern void func_800914E0(void);
extern void func_80091510(void *argument);
extern void func_80091544(void);
extern void func_80091574(void);
extern void func_800915A4(void);
extern u32  func_80093924(void);
extern int  func_80096484(void);
extern void func_8004CD6C(s32 message, s32 arg2, s32 arg3);
extern int  func_8008A044(int arg0, int index, int kind);
extern void func_80049410(void);
extern s32  func_8003C7B8(s32 message, void *obj);
extern void func_8005C0C0(void *pool, void *owner);
extern void func_80051318(void *obj, void *arg1, void *arg2);
extern s32  func_8003CAE4(void *object, s32 arg1, u8 *flag);
extern void func_80013BD8(void *instance, void *model, s32 options);
extern s32  func_8005A934(void *object, s32 z, s32 x, s32 *angle);
extern void func_80053730(void *object);
extern void func_8005A070(u8 *object);
extern void func_8001F158(void);
extern void *func_8008E5D8(void *callback);

/* 1. 8005A2B0: gInitTextureDescriptorTable */
static void test_8005A2B0(void)
{
    void *dummy = (void *)0x12345678;
    /* @leaf 8005A2B0 source src/game/8005A2B0.c: clears global texture descriptor table pointer */
    LK_FUNC("func_8005A2B0");
    g_tex_desc_tbl = &dummy;
    func_8005A2B0();
    FCK_CHECK(g_tex_desc_tbl == 0, "table cleared");
    func_8005A2B0();
    FCK_CHECK(g_tex_desc_tbl == 0, "idempotent clear");
}

/* 2. 8006A880: gClearDescriptorRegistry */
static void test_8006A880(void)
{
    /* @leaf 8006A880 source src/game/8006A880.c: resets descriptor registry count to zero */
    LK_FUNC("func_8006A880");
    g_desc_reg_cnt = 42;
    func_8006A880();
    FCK_EQ(g_desc_reg_cnt, 0, "count reset from positive");
    g_desc_reg_cnt = -7;
    func_8006A880();
    FCK_EQ(g_desc_reg_cnt, 0, "count reset from negative");
}

/* 3. 80086F44: gMovieAudioClockRead */
static u8 g_mov_aud_buf[0x7B28];

static void test_80086F44(void)
{
    u8 marker_a = 1;
    u8 marker_b = 2;
    u8 **slot = (u8 **)(g_mov_aud_buf + 0x7B24);
    /* @leaf 80086F44 source src/game/80086F44.c: reads movie audio clock pointer at offset 0x7B24 */
    LK_FUNC("func_80086F44");
    g_mov_aud_state = g_mov_aud_buf;
    *slot = &marker_a;
    FCK_CHECK(func_80086F44() == &marker_a, "reads marker_a");
    *slot = &marker_b;
    FCK_CHECK(func_80086F44() == &marker_b, "reads marker_b");
    *slot = 0;
    FCK_CHECK(func_80086F44() == 0, "reads null");
}

/* 4..10. IRQ callback table dispatchers (80091480..800915A4) */
static int g_irq_hits[8];
static int g_irq_last_op;
static void *g_irq_last_arg;
static void *g_dma_ret_val;

static void irq_slot0(void) { g_irq_hits[0]++; }
static void *irq_slot1_dma(int ch, void *cb) {
    g_irq_hits[1]++;
    g_irq_last_op = ch;
    g_irq_last_arg = cb;
    return g_dma_ret_val;
}
static void irq_slot2(void) { g_irq_hits[2]++; }
static void irq_slot3(void) { g_irq_hits[3]++; }
static void irq_slot4(void) { g_irq_hits[4]++; }
static void irq_slot5_vsync(int op, void *arg) {
    g_irq_hits[5]++;
    g_irq_last_op = op;
    g_irq_last_arg = arg;
}
static void irq_slot6(void) { g_irq_hits[6]++; }

static void *g_irq_tbl[8] = {
    (void *)irq_slot0,
    (void *)irq_slot1_dma,
    (void *)irq_slot2,
    (void *)irq_slot3,
    (void *)irq_slot4,
    (void *)irq_slot5_vsync,
    (void *)irq_slot6,
    0
};

static void reset_irq_hits(void)
{
    lk_fill(g_irq_hits, 0, sizeof(g_irq_hits));
    g_irq_last_op = -1;
    g_irq_last_arg = (void *)0x11111111;
    g_dma_ret_val = 0;
    g_irq_cb_tbl = g_irq_tbl;
}

static void test_80091480(void)
{
    /* @leaf 80091480 source src/game/80091480.c: dispatches IRQ reset callback via table slot 3 */
    LK_FUNC("func_80091480");
    reset_irq_hits();
    func_80091480();
    FCK_EQ(g_irq_hits[3], 1, "slot 3 called");
    FCK_EQ(g_irq_hits[2], 0, "slot 2 untouched");
    FCK_EQ(g_irq_hits[4], 0, "slot 4 untouched");
    func_80091480();
    FCK_EQ(g_irq_hits[3], 2, "slot 3 called twice");
}

static void test_800914B0(void)
{
    /* @leaf 800914B0 source src/game/800914B0.c: dispatches IRQ callback via table slot 2 */
    LK_FUNC("func_800914B0");
    reset_irq_hits();
    func_800914B0();
    FCK_EQ(g_irq_hits[2], 1, "slot 2 called");
    FCK_EQ(g_irq_hits[1], 0, "slot 1 untouched");
    FCK_EQ(g_irq_hits[3], 0, "slot 3 untouched");
}

static void test_800914E0(void)
{
    /* @leaf 800914E0 source src/game/800914E0.c: dispatches DMA callback via table slot 1 */
    LK_FUNC("func_800914E0");
    reset_irq_hits();
    func_800914E0();
    FCK_EQ(g_irq_hits[1], 1, "slot 1 called");
    FCK_EQ(g_irq_hits[0], 0, "slot 0 untouched");
    FCK_EQ(g_irq_hits[2], 0, "slot 2 untouched");
}

static void test_80091510(void)
{
    int token = 77;
    /* @leaf 80091510 source src/game/80091510.c: dispatches VSync callback with op=4 via slot 5 */
    LK_FUNC("func_80091510");
    reset_irq_hits();
    func_80091510(&token);
    FCK_EQ(g_irq_hits[5], 1, "slot 5 called");
    FCK_EQ(g_irq_last_op, 4, "op is 4");
    FCK_CHECK(g_irq_last_arg == &token, "arg forwarded");
    func_80091510(0);
    FCK_EQ(g_irq_hits[5], 2, "slot 5 called again");
    FCK_CHECK(g_irq_last_arg == 0, "null arg forwarded");
}

static void test_80091544(void)
{
    /* @leaf 80091544 source src/game/80091544.c: dispatches VSync registration thunk via slot 5 */
    LK_FUNC("func_80091544");
    reset_irq_hits();
    func_80091544();
    FCK_EQ(g_irq_hits[5], 1, "slot 5 called");
    FCK_EQ(g_irq_hits[4], 0, "slot 4 untouched");
    FCK_EQ(g_irq_hits[6], 0, "slot 6 untouched");
}

static void test_80091574(void)
{
    /* @leaf 80091574 source src/game/80091574.c: invokes IRQ callback via table slot 4 */
    LK_FUNC("func_80091574");
    reset_irq_hits();
    func_80091574();
    FCK_EQ(g_irq_hits[4], 1, "slot 4 called");
    FCK_EQ(g_irq_hits[3], 0, "slot 3 untouched");
    FCK_EQ(g_irq_hits[5], 0, "slot 5 untouched");
}

static void test_800915A4(void)
{
    /* @leaf 800915A4 source src/game/800915A4.c: resumes IRQ subsystem via table slot 6 */
    LK_FUNC("func_800915A4");
    reset_irq_hits();
    func_800915A4();
    FCK_EQ(g_irq_hits[6], 1, "slot 6 called");
    FCK_EQ(g_irq_hits[5], 0, "slot 5 untouched");
    FCK_EQ(g_irq_hits[0], 0, "slot 0 untouched");
}

/* 11. 80093924: gGetOddEvenFlag */
static int g_vram_ret;
static int vram_cb_stub(void) { return g_vram_ret; }
static void *g_vram_table[16];

static void test_80093924(void)
{
    /* @leaf 80093924 source src/game/80093924.c: returns bit 31 of VRAM callback slot 14 */
    LK_FUNC("func_80093924");
    lk_fill(g_vram_table, 0, sizeof(g_vram_table));
    g_vram_table[14] = (void *)vram_cb_stub;
    g_vram_cb_tbl = g_vram_table;

    g_vram_ret = 0;
    FCK_EQ(func_80093924(), 0, "zero -> 0");
    g_vram_ret = 0x7FFFFFFF;
    FCK_EQ(func_80093924(), 0, "max positive -> 0");
    g_vram_ret = -1;
    FCK_EQ(func_80093924(), 1, "-1 -> 1");
    g_vram_ret = (int)0x80000000u;
    FCK_EQ(func_80093924(), 1, "sign bit -> 1");
}

/* 12. 80096484: gIsMdecBusy */
static void test_80096484(void)
{
    int reg = 0;
    /* @leaf 80096484 source src/game/80096484.c: dereferences MDEC control status pointer */
    LK_FUNC("func_80096484");
    g_mdec_ctrl_ptr = &reg;
    FCK_EQ(func_80096484(), 0, "reads 0");
    reg = 1;
    FCK_EQ(func_80096484(), 1, "reads 1");
    reg = -99;
    FCK_EQ(func_80096484(), -99, "reads -99");
    reg = 0x12345678;
    FCK_EQ(func_80096484(), 0x12345678, "reads pattern");
}

/* 13. 8004CD6C: gBroadcastEntityMessage */
static s32 g_bc_calls;
static s32 g_bc_ids[8];
static s32 g_bc_msg, g_bc_a2, g_bc_a3;

static s32 bc_update(s32 message, struct A8F90_Entity *ent, s32 a2, s32 a3)
{
    if (g_bc_calls < 8) g_bc_ids[g_bc_calls] = ent->id;
    g_bc_calls++;
    g_bc_msg = message;
    g_bc_a2 = a2;
    g_bc_a3 = a3;
    return 0;
}

static void test_8004CD6C(void)
{
    struct A8F90_Entity e[4];
    /* @leaf 8004CD6C source src/game/8004CD6C.c: broadcasts message only to entities with id > 0 */
    LK_FUNC("func_8004CD6C");
    lk_fill(e, 0, sizeof(e));
    e[0].id = 10; e[0].update = bc_update;
    e[1].id = 0;  e[1].update = bc_update;
    e[2].id = -3; e[2].update = bc_update;
    e[3].id = 25; e[3].update = bc_update;

    g_cmd_list_hd.count = 0;
    g_cmd_list_hd.items[0] = &e[0];
    g_bc_calls = 0;
    func_8004CD6C(7, 11, 22);
    FCK_EQ(g_bc_calls, 0, "zero count calls none");

    g_cmd_list_hd.count = 4;
    g_cmd_list_hd.items[0] = &e[0];
    g_cmd_list_hd.items[1] = &e[1];
    g_cmd_list_hd.items[2] = &e[2];
    g_cmd_list_hd.items[3] = &e[3];
    g_bc_calls = 0;
    func_8004CD6C(99, 123, 456);
    FCK_EQ(g_bc_calls, 2, "two positive-id entities called");
    FCK_EQ(g_bc_ids[0], 10, "first entity id");
    FCK_EQ(g_bc_ids[1], 25, "second entity id");
    FCK_EQ(g_bc_msg, 99, "message forwarded");
    FCK_EQ(g_bc_a2, 123, "arg2 forwarded");
    FCK_EQ(g_bc_a3, 456, "arg3 forwarded");
}

/* 14. 8008A044: gGetPadActuatorInfo */
struct PadActEntry { u8 a, b, c, d, e; };
struct PadActData { void *pad; struct PadActEntry *entries; u8 pad08[0xE1]; u8 count; };
static struct PadActData g_pad_act_data;
static int g_pad_lookup_arg;

static struct PadActData *pad_lookup_stub(int arg0)
{
    g_pad_lookup_arg = arg0;
    return &g_pad_act_data;
}

static void test_8008A044(void)
{
    struct PadActEntry ents[2];
    /* @leaf 8008A044 source src/game/8008A044.c: returns actuator count for index<0 or fields a..e */
    LK_FUNC("func_8008A044");
    lk_fill(&g_pad_act_data, 0, sizeof(g_pad_act_data));
    ents[0].a = 11; ents[0].b = 22; ents[0].c = 33; ents[0].d = 44; ents[0].e = 55;
    ents[1].a = 61; ents[1].b = 62; ents[1].c = 63; ents[1].d = 64; ents[1].e = 65;
    g_pad_act_data.entries = ents;
    g_pad_act_data.count = 2;
    g_pad_lookup_fn = (void *)pad_lookup_stub;

    FCK_EQ(func_8008A044(3, -1, 1), 2, "negative index returns count");
    FCK_EQ(g_pad_lookup_arg, 3, "port arg forwarded");
    FCK_EQ(func_8008A044(1, 0, 1), 11, "kind 1 -> a");
    FCK_EQ(func_8008A044(1, 0, 2), 22, "kind 2 -> b");
    FCK_EQ(func_8008A044(1, 0, 3), 33, "kind 3 -> c");
    FCK_EQ(func_8008A044(1, 0, 4), 44, "kind 4 -> d");
    FCK_EQ(func_8008A044(1, 0, 5), 55, "kind 5 -> e");
    FCK_EQ(func_8008A044(1, 1, 3), 63, "second entry kind 3");
    FCK_EQ(func_8008A044(1, 0, 0), 0, "invalid kind 0 -> 0");
    FCK_EQ(func_8008A044(1, 0, 6), 0, "invalid kind 6 -> 0");
    FCK_EQ(func_8008A044(1, 2, 1), 0, "out-of-range index -> 0");
}

/* 15. 80049410: gResetScriptTimers */
static void test_80049410(void)
{
    int dummy = 1;
    /* @leaf 80049410 source src/game/80049410.c: clears script timer head and count */
    LK_FUNC("func_80049410");
    g_script_tmr_hd = &dummy;
    g_script_tmr_cnt = 15;
    func_80049410();
    FCK_CHECK(g_script_tmr_hd == 0, "head cleared");
    FCK_EQ(g_script_tmr_cnt, 0, "count cleared");
}

/* 16. 8003C7B8: gGuardEffectPoolMessage */
struct EffParent { u8 pad[0x20]; u32 x, y; u8 pad28[2]; s16 z; };
struct EffCounter { u8 pad[0x3a]; u16 a; u8 pad3c[2]; u16 b; };
struct EffObj {
    u8 pad[4];
    s16 field04;
    u8 pad06[2];
    s16 field08, field0a, field0c;
    u8 pad0e;
    u8 flags0f, flags10;
    u8 pad11[3];
    s16 field14, field16;
    u8 pad18[4];
    struct EffCounter *counter;
    struct EffParent *parent;
};

static void test_8003C7B8(void)
{
    struct EffParent parent;
    struct EffCounter ctr;
    struct EffObj obj;
    /* @leaf 8003C7B8 source src/game/8003C7B8.c: effect pool headroom guard and msg 0/1/10 handler */
    LK_FUNC("func_8003C7B8");

    lk_fill(&parent, 0, sizeof(parent));
    lk_fill(&ctr, 0, sizeof(ctr));
    lk_fill(&obj, 0, sizeof(obj));
    parent.x = 0x4000; /* *75>>14 = 75 */
    parent.y = 0x8000; /* *75>>14 = 150 */
    parent.z = 0x400;  /* *75>>10 = 75 */
    obj.parent = &parent;
    obj.counter = &ctr;
    obj.field0a = 10;
    obj.field08 = 10;
    obj.flags0f = 0xFF;

    /* Plenty of headroom in both pools */
    g_prim_next = 0;
    g_prim_end = 52 * 52 * 100;
    g_pkt_cursor = 0;
    g_pkt_limit = 28 * 28 * 100;
    FCK_EQ(func_8003C7B8(0, &obj), 0, "msg 0 succeeds with headroom");
    FCK_EQ(obj.field16, 75, "field16 scaled from x");
    FCK_EQ(obj.field04, 150, "field04 scaled from y");
    FCK_EQ(obj.field0c, 75, "field0c mirrors field16");
    FCK_EQ(obj.flags0f, 0, "flags0f cleared");
    FCK_EQ(obj.field14, 75, "field14 scaled from z");

    /* Prim pool exact boundary (50 < 50 is false -> 0) and too tight (50 < 60 -> 1) */
    obj.field0a = 50;
    FCK_EQ(func_8003C7B8(0, &obj), 0, "prim pool exact boundary succeeds");
    obj.field0a = 60;
    FCK_EQ(func_8003C7B8(0, &obj), 1, "prim pool guard triggers");
    obj.field0a = 10;

    /* Packet pool too tight -> return 1 */
    obj.field08 = 60;
    FCK_EQ(func_8003C7B8(0, &obj), 1, "packet pool guard triggers");

    /* Case 1: flags0f &= 11 */
    obj.flags0f = 0xFF;
    FCK_EQ(func_8003C7B8(1, &obj), 0, "msg 1 returns 0");
    FCK_EQ(obj.flags0f, 0x0B, "flags0f masked with 11");

    /* Case 10: counter decrements */
    ctr.a = 5; ctr.b = 8;
    obj.flags10 = 0;
    FCK_EQ(func_8003C7B8(10, &obj), 0, "msg 10 returns 0");
    FCK_EQ(ctr.a, 4, "counter a decremented");
    FCK_EQ(ctr.b, 7, "counter b decremented when flag bit 2 clear");

    obj.flags10 = 4;
    FCK_EQ(func_8003C7B8(10, &obj), 0, "msg 10 with flag bit 2 set");
    FCK_EQ(ctr.a, 3, "counter a still decremented");
    FCK_EQ(ctr.b, 7, "counter b preserved when flag bit 2 set");

    obj.counter = 0;
    FCK_EQ(func_8003C7B8(10, &obj), 0, "msg 10 null counter safe");
    FCK_EQ(func_8003C7B8(99, &obj), 0, "unknown msg returns 0");
}

/* 17. 8005C0C0: gSweepSpritePool */
struct Tgt5C0C0 { u8 unk00[8]; s32 id; };
struct Node5C0C0 {
    struct Node5C0C0 *next;
    struct Tgt5C0C0 *first;
    struct Tgt5C0C0 *second;
    s32 unk0C;
    s32 firstId;
    s32 secondId;
    u8 unk18[0x10];
    u8 flags;
};
struct Pool5C0C0 {
    void *storage;
    struct Node5C0C0 *active;
    struct Node5C0C0 *free;
};

static void test_8005C0C0(void)
{
    struct Tgt5C0C0 t1, t2;
    struct Node5C0C0 n0, n1, n2;
    struct Pool5C0C0 pool;
    /* @leaf 8005C0C0 source src/game/8005C0C0.c: releases flagged pool nodes and clears stale refs */
    LK_FUNC("func_8005C0C0");
    lk_fill(&t1, 0, sizeof(t1)); lk_fill(&t2, 0, sizeof(t2));
    lk_fill(&n0, 0, sizeof(n0)); lk_fill(&n1, 0, sizeof(n1)); lk_fill(&n2, 0, sizeof(n2));
    t1.id = 10; t2.id = 20;

    n0.flags = 1; n0.next = &n1;
    n1.flags = 0; n1.first = &t1; n1.firstId = 99; n1.second = &t2; n1.secondId = 20; n1.next = &n2;
    n2.flags = 0; n2.first = &t1; n2.firstId = 10; n2.second = &t2; n2.secondId = 88; n2.next = 0;

    pool.storage = 0;
    pool.active = &n0;
    pool.free = 0;

    func_8005C0C0(&pool, 0);
    FCK_CHECK(pool.active == &n1, "n0 unlinked from active");
    FCK_CHECK(pool.free == &n0, "n0 pushed to free");
    FCK_CHECK(n1.first == 0, "n1 stale first cleared");
    FCK_CHECK(n1.second == &t2, "n1 valid second kept");
    FCK_CHECK(n2.first == &t1, "n2 valid first kept");
    FCK_CHECK(n2.second == 0, "n2 stale second cleared");
}

/* 18. 80051318: gInitLoopingPathState */
struct Vec3_51318 { s32 x, y, z; };
struct Arg1_51318 { s32 unk00; u8 unk04[0x1C]; s32 unk20; };
struct State51318 {
    struct Arg1_51318 *unk00;
    s32 unk04, unk08, unk0C, unk10, unk14, unk18;
    void *unk1C;
    void *unk20;
    s32 unk24, unk28;
    struct Vec3_51318 unk2C;
};

static void test_80051318(void)
{
    struct State51318 st;
    struct Arg1_51318 a1;
    struct Vec3_51318 pos;
    /* Two records in synthetic blob: 0x61415048 ('HPAa') and 0x74415048 ('HPAt') */
    s32 recs[12];
    /* @leaf 80051318 source src/game/80051318.c: initializes looping path state from record blob */
    LK_FUNC("func_80051318");
    lk_fill(&st, 0x5A, sizeof(st));
    lk_fill(&a1, 0, sizeof(a1));
    lk_fill(recs, 0, sizeof(recs));
    /* Record 0 at offset 0: id=0x61415048, offset=24 (6 words), payload at recs+3 */
    recs[0] = 0x61415048; recs[1] = 24; recs[2] = 0;
    recs[3] = 0x111; recs[4] = 0x222; recs[5] = 0x333;
    /* Record 1 at offset 24 (recs+6): id=0x74415048, offset=24, payload at recs+9, payload->unk08 = recs[11] */
    recs[6] = 0x74415048; recs[7] = 24; recs[8] = 0;
    recs[9] = 0; recs[10] = 0; recs[11] = 777;

    a1.unk00 = (s32)recs;
    a1.unk20 = sizeof(recs);
    pos.x = 10; pos.y = 20; pos.z = 30;

    func_80051318(&st, &a1, &pos);
    FCK_CHECK(st.unk00 == &a1, "arg1 stored");
    FCK_CHECK(st.unk1C == &recs[3], "HPAa payload found");
    FCK_CHECK(st.unk20 == &recs[3], "unk20 mirrors unk1C");
    FCK_EQ(st.unk24, 0, "unk24 zeroed");
    FCK_EQ(st.unk28, 777, "HPAt payload unk08 loaded");
    FCK_EQ(st.unk04, -1, "unk04 = -1");
    FCK_EQ(st.unk08, 0, "unk08 = 0");
    FCK_EQ(st.unk18, 0x10000, "unk18 = 0x10000");
    FCK_EQ(st.unk2C.x, 10, "pos.x copied");
    FCK_EQ(st.unk2C.y, 20, "pos.y copied");
    FCK_EQ(st.unk2C.z, 30, "pos.z copied");

    /* Null arg1 skips record lookups, still sets scalar defaults & pos */
    pos.x = -5; pos.y = -6; pos.z = -7;
    func_80051318(&st, 0, &pos);
    FCK_CHECK(st.unk00 == 0, "null arg1 stored");
    FCK_EQ(st.unk28, 777, "unk28 untouched on null arg1");
    FCK_EQ(st.unk2C.x, -5, "pos.x updated on null arg1");
}

/* 19. 8003CAE4: gStepSpawnNodeTimers */
struct Sub1_3CAE4 { u8 pad00[0x34]; void *unk34; u8 unk38, unk39; };
struct Sub2_3CAE4 { u8 pad00[0x14]; u8 unk14; u8 pad15[0x15]; s16 unk2A; };
struct Obj_3CAE4 {
    u8 unk00[4];
    s16 unk04;
    u8 unk06;
    s8 unk07;
    u8 unk08[4];
    s16 unk0C;
    u8 unk0E;
    u8 flags;
    u8 unk10;
    u8 unk11[3];
    s16 unk14;
    u8 unk16[2];
    s16 unk18;
    u16 unk1A;
    struct Sub1_3CAE4 *unk1C;
    struct Sub2_3CAE4 *unk20;
    s32 (*callback)(s32 message, struct Obj_3CAE4 *object, s32 arg2, s32 arg3);
};
static s32 g_3cae4_cb_msgs[8];
static s32 g_3cae4_cb_cnt;
static s32 g_3cae4_cb_ret;
static s32 cb_3cae4(s32 msg, struct Obj_3CAE4 *o, s32 a2, s32 a3)
{
    (void)o; (void)a2; (void)a3;
    if (g_3cae4_cb_cnt < 8) g_3cae4_cb_msgs[g_3cae4_cb_cnt] = msg;
    g_3cae4_cb_cnt++;
    return g_3cae4_cb_ret;
}

static void test_8003CAE4(void)
{
    struct Sub1_3CAE4 sub1;
    struct Sub2_3CAE4 sub2;
    struct Obj_3CAE4 obj;
    u8 flag;
    s32 res;
    /* @leaf 8003CAE4 source src/game/8003CAE4.c: steps spawn node timers, callbacks, and retire path */
    LK_FUNC("func_8003CAE4");
    lk_fill(&sub1, 0, sizeof(sub1));
    lk_fill(&sub2, 0, sizeof(sub2));
    lk_fill(&obj, 0, sizeof(obj));
    sub2.unk14 = 0x33;
    sub2.unk2A = 0x400; /* *75>>10 = 75 */
    obj.unk20 = &sub2;
    obj.callback = cb_3cae4;

    /* 1) unk04 expires -> sets flag from unk06, clears unk06, sets flags|=2 */
    obj.unk04 = 5; obj.unk06 = 0x22; obj.unk18 = 0;
    flag = 0; g_3cae4_cb_cnt = 0; g_3cae4_cb_ret = 0;
    res = func_8003CAE4(&obj, 5, &flag);
    FCK_EQ(res, 0, "no spawn trigger yet");
    FCK_EQ(flag, 0x22, "unk04 expiry emits unk06");
    FCK_EQ(obj.unk06, 0, "unk06 cleared");
    FCK_CHECK((obj.flags & 2) != 0, "flag bit 1 set");

    /* 2) flags & 4 with flag==0 -> callback(1)!=1 sets flag=sub2.unk14, returns 1, clears bit 2 */
    obj.flags = 4; obj.unk04 = -1; flag = 0; g_3cae4_cb_cnt = 0; g_3cae4_cb_ret = 0;
    res = func_8003CAE4(&obj, 1, &flag);
    FCK_EQ(res, 1, "spawn callback returns 1");
    FCK_EQ(flag, 0x33, "flag set from sub2.unk14");
    FCK_EQ(g_3cae4_cb_msgs[0], 1, "callback msg 1");
    FCK_CHECK((obj.flags & 4) == 0, "flag bit 2 cleared");

    /* 3) unk14 expires -> reloads 75, decrements unk07, sets bit 2; unk0C expires -> callback(8) */
    obj.flags = 0; obj.unk14 = 3; obj.unk07 = 2; obj.unk0C = 2; flag = 0; g_3cae4_cb_cnt = 0;
    res = func_8003CAE4(&obj, 4, &flag);
    FCK_EQ(obj.unk14, 75, "unk14 reloaded");
    FCK_EQ(obj.unk07, 1, "unk07 decremented");
    FCK_CHECK((obj.flags & 4) != 0, "flag bit 2 armed");
    FCK_EQ(g_3cae4_cb_msgs[0], 8, "unk0C expiry called msg 8");

    /* 4) sub1 retire path (unk1A & 0x1000) -> clears flag, calls gRetireSpawnSlot (msg 7), sets bit 0 */
    sub1.unk34 = 0; sub1.unk39 = 1;
    obj.unk1C = &sub1; obj.unk1A = 0x1000; obj.flags = 0; obj.unk04 = -1; obj.unk14 = 0; obj.unk0C = 0;
    flag = 0x77; g_3cae4_cb_cnt = 0;
    func_8003CAE4(&obj, 1, &flag);
    FCK_EQ(flag, 0, "retire clears flag");
    FCK_EQ(g_3cae4_cb_msgs[0], 7, "gRetireSpawnSlot sent msg 7");
    FCK_CHECK((obj.flags & 3) == 3, "retire sets flags 1 and 2");
}

/* 20. 80013BD8: gBindModelInstanceWithMatrix */
struct MFrame13BD8 { u16 unk00; s16 unk02; s16 radius; u16 unk06; u8 unk08[8]; };
struct MFrames13BD8 { u8 header[0x10]; struct MFrame13BD8 frames[1]; };
struct Model13BD8 { u8 unk00[0x0C]; u32 flags; u8 unk10[0x24]; struct MFrames13BD8 *frames; };
struct MInst13BD8 {
    struct Model13BD8 *model;
    s32 mat[9];
    u8 coord_rest[0x3C - 36];
    s32 radius;
    u16 unk44, unk46;
    s16 frame;
    u8 flags;
};

static void test_80013BD8(void)
{
    struct MFrames13BD8 mfs;
    struct Model13BD8 mdl;
    struct MInst13BD8 inst;
    int i;
    /* @leaf 80013BD8 source src/game/80013BD8.c: binds model instance and copies identity matrix */
    LK_FUNC("func_80013BD8");
    for (i = 0; i < 9; i++) g_id_mat[i] = (i + 1) * 100;
    lk_fill(&mfs, 0, sizeof(mfs));
    lk_fill(&mdl, 0, sizeof(mdl));
    lk_fill(&inst, 0xCC, sizeof(inst));
    mfs.frames[0].radius = 10;
    mfs.frames[0].unk00 = 0x1234;
    mfs.frames[0].unk06 = 0x5678;
    mdl.flags = 0x40000000u;
    mdl.frames = &mfs;

    func_80013BD8(&inst, &mdl, 0x80000 | 0x8000);
    FCK_CHECK(inst.model == &mdl, "model bound");
    FCK_EQ(inst.radius, 160, "radius = 10 << 4");
    FCK_EQ(inst.unk44, 0x1234, "unk44 copied");
    FCK_EQ(inst.unk46, 0x5678, "unk46 copied");
    FCK_EQ(inst.flags, 0x10 | 0x08 | 0x40, "option + model flags set");
    FCK_EQ(inst.mat[0], 100, "mat[0] copied");
    FCK_EQ(inst.mat[4], 500, "mat[4] copied");
    FCK_EQ(inst.mat[8], 900, "mat[8] copied");

    /* Null model skips matrix copy, zeroes radius/unk44/unk46 */
    inst.mat[0] = 9999;
    func_80013BD8(&inst, 0, 0x10000);
    FCK_CHECK(inst.model == 0, "null model stored");
    FCK_EQ(inst.radius, 0, "radius zeroed");
    FCK_EQ(inst.flags, 4, "option bit 0x10000 -> flag 4");
    FCK_EQ(inst.mat[0], 9999, "matrix untouched for null model");
}

/* 21. 8005A934: gCheckRadarTargetBearing */
struct Obj5A934 {
    u8 unk00[0x50];
    s32 x, y, z;
    u8 unk5C[0x54];
    s16 heading;
};

static void test_8005A934(void)
{
    struct Obj5A934 obj;
    s32 angle = 0x1234;
    int i;
    /* @leaf 8005A934 source src/game/8005A934.c: computes radar bearing outside 0x2000 deadzone */
    LK_FUNC("func_8005A934");
    for (i = 0; i <= 1024; i++) g_atan_tbl[i] = (s16)(i >> 2); /* 0..256 */
    lk_fill(&obj, 0, sizeof(obj));
    obj.x = 1000; obj.z = 2000; obj.heading = 0x100;

    /* Within deadzone -> 0, angle untouched */
    FCK_EQ(func_8005A934(&obj, 2000 + 0x2000, 1000 - 0x2000, &angle), 0, "inside deadzone");
    FCK_EQ(angle, 0x1234, "angle untouched inside deadzone");

    /* dx=0, dz=0x3000 -> gAtan2(0, 0x3000) = gAtanTable[0] = 0 -> angle = 0 - 0x100 - 0x400 = -0x500 */
    FCK_EQ(func_8005A934(&obj, 2000 + 0x3000, 1000, &angle), 1, "outside deadzone +z");
    FCK_EQ(angle, -0x500, "bearing for +z");

    /* dx=0x3000, dz=0 -> gAtan2(0x3000, 0) = 0x400 - gAtanTable[0] = 0x400 -> angle = -0x100 */
    FCK_EQ(func_8005A934(&obj, 2000, 1000 + 0x3000, &angle), 1, "outside deadzone +x");
    FCK_EQ(angle, -0x100, "bearing for +x");

    /* dx=0x3000, dz=0x3000 -> gAtan2(0x3000,0x3000) = 0x400 - gAtanTable[1024] = 0x300 -> angle = -0x200 */
    FCK_EQ(func_8005A934(&obj, 2000 + 0x3000, 1000 + 0x3000, &angle), 1, "diagonal outside");
    FCK_EQ(angle, -0x200, "bearing for +x/+z");
}

/* 22. 80053730: gUpdateTwoPlayerSeparation */
struct Info53730 { u8 unk00[0xC1]; s8 side; };
struct Obj53730 { u8 unk00[0x50]; s32 x, y, z; u8 unk5C[0x50]; struct Info53730 *info; };
struct Glob53730 { u8 unk00[0x5C]; s32 unk5C; };

static void test_80053730(void)
{
    struct Info53730 i0, i1;
    struct Obj53730 p0, p1;
    struct Glob53730 sess;
    /* @leaf 80053730 source src/game/80053730.c: updates camera near base from 2P XZ separation */
    LK_FUNC("func_80053730");
    lk_fill(&i0, 0, sizeof(i0)); lk_fill(&i1, 0, sizeof(i1));
    lk_fill(&p0, 0, sizeof(p0)); lk_fill(&p1, 0, sizeof(p1));
    lk_fill(&sess, 0, sizeof(sess));
    i0.side = 0; i1.side = 1;
    p0.info = &i0; p1.info = &i1;
    p0.x = 100; p0.z = 200;
    p1.x = 260; p1.z = 200; /* dx=160, dz=0 -> gApproximateLength2 = 160 */
    g_player_ents[0] = &p0;
    g_player_ents[1] = &p1;
    g_game_sess[0] = &sess;

    /* 1P mode -> untouched */
    g_player_cnt[0] = 1; sess.unk5C = 0; g_cam_near_base = -1;
    func_80053730(&p0);
    FCK_EQ(g_cam_near_base, -1, "1P does not update");

    /* 2P mode with sess.unk5C != 0 -> untouched */
    g_player_cnt[0] = 2; sess.unk5C = 1;
    func_80053730(&p0);
    FCK_EQ(g_cam_near_base, -1, "blocked when unk5C nonzero");

    /* 2P mode with sess.unk5C == 0 -> computes separation */
    sess.unk5C = 0;
    func_80053730(&p0);
    FCK_EQ(g_cam_near_base, 160, "axis-aligned separation");

    /* dx=128, dz=64 -> a=128, b=64, (a>>1)<b false -> 128 + 16 - 1 = 143 */
    p1.x = 228; p1.z = 264;
    func_80053730(&p1);
    FCK_EQ(g_cam_near_base, 143, "diagonal separation from p1 side");
}

/* 23. 8005A070: gInitEntityGauge */
static void test_8005A070(void)
{
    u8 obj[0x80];
    int i;
    /* @leaf 8005A070 source src/game/8005A070.c: copies 0x14 gauge bytes from D_8009AFA0 to +0x54 */
    LK_FUNC("func_8005A070");
    lk_fill(obj, 0xA5, sizeof(obj));
    for (i = 0; i < 0x14; i++) g_src_blk20[i] = (u8)(0x30 + i);
    func_8005A070(obj);
    FCK_CHECK(lk_all(obj, 0, 0x54, 0xA5), "bytes before +0x54 untouched");
    FCK_EQ(obj[0x54], 0x30, "first gauge byte copied");
    FCK_EQ(obj[0x67], 0x43, "last gauge byte copied");
    FCK_CHECK(lk_all(obj, 0x68, sizeof(obj), 0xA5), "bytes after +0x68 untouched");
}

/* 24. 8001F158: gStopFrameCallback */
static void test_8001F158(void)
{
    /* @leaf 8001F158 source src/game/8001F158.c: unregisters VSync callback and clears frame hook/flag */
    LK_FUNC("func_8001F158");
    reset_irq_hits();
    g_scr_mode_hook = 0x5555;
    g_frm_cb_en = 1;
    func_8001F158();
    FCK_EQ(g_irq_hits[5], 1, "VSync callback slot 5 called");
    FCK_EQ(g_irq_last_op, 4, "VSync op is 4");
    FCK_CHECK(g_irq_last_arg == 0, "VSync callback cleared to null");
    FCK_EQ(g_scr_mode_hook, 0, "screen mode hook cleared");
    FCK_EQ(g_frm_cb_en, 0, "frame callback flag cleared");
}

/* 25. 8008E5D8: gSetCdDmaCallback */
static void dummy_cd_cb(void) {}

static void test_8008E5D8(void)
{
    int prev_tok = 9;
    void *ret;
    /* @leaf 8008E5D8 source src/game/8008E5D8.c: registers CD DMA callback on channel 3 */
    LK_FUNC("func_8008E5D8");
    reset_irq_hits();
    g_dma_ret_val = &prev_tok;
    ret = func_8008E5D8((void *)dummy_cd_cb);
    FCK_EQ(g_irq_hits[1], 1, "DMA slot 1 called");
    FCK_EQ(g_irq_last_op, 3, "DMA channel 3");
    FCK_CHECK(g_irq_last_arg == (void *)dummy_cd_cb, "callback forwarded");
    FCK_CHECK(ret == &prev_tok, "previous callback returned");
}

int main(void)
{
    test_8005A2B0();
    test_8006A880();
    test_80086F44();
    test_80091480();
    test_800914B0();
    test_800914E0();
    test_80091510();
    test_80091544();
    test_80091574();
    test_800915A4();
    test_80093924();
    test_80096484();
    test_8004CD6C();
    test_8008A044();
    test_80049410();
    test_8003C7B8();
    test_8005C0C0();
    test_80051318();
    test_8003CAE4();
    test_80013BD8();
    test_8005A934();
    test_80053730();
    test_8005A070();
    test_8001F158();
    test_8008E5D8();
    return lk_done("leaf_century_a");
}
