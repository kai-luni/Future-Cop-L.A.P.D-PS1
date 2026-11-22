/* Source characterizations of 25 unchanged src/game units on synthetic RAM.
 * Suite: leaf_century_b (Category A: pure CPU, no BIOS/GPU/SPU/CD/GTE).
 * Every @leaf unit is distinct from leaf_century_a and all other harnesses.
 */
#include "game/types.h"
#define FCK_TYPES_H
#include "leaf_kit.h"

/* Real helpers already covered elsewhere or needed for linking (not counted
 * as @leaf coverage here). Struct tag renames prevent C redefinitions. */
#include "../../src/game/800914E0.c"
#define CallbackTable CallbackTable_914B0
#define gIrqCallbackTable gIrqCallbackTable_914B0
#include "../../src/game/800914B0.c"
#undef CallbackTable
#undef gIrqCallbackTable
#include "../../src/game/800195A4.c"
#define LinkState LinkState_195D0
#include "../../src/game/800195D0.c"
#undef LinkState
#include "../../src/game/80019668.c"
#include "../../src/game/80019A74.c"
#include "../../src/game/80019C38.c"
#include "../../src/game/80045288.c"
#include "../../src/game/80021A8C.c"
#include "../../src/game/8002238C.c"
#include "../../src/game/8001EF34.c"
#define PadBuffer PadBuffer_1E7B8
#define gPadBuffers gPadBuffers_1E7B8
#include "../../src/game/8001E7B8.c"
#undef PadBuffer
#undef gPadBuffers
#include "../../src/game/80022B74.c"
#include "../../src/game/8002443C.c"
#include "../../src/game/8006AAEC.c"
#include "../../src/game/8006BD48.c"
#include "../../src/game/8008B73C.c"
#include "../../src/game/8008B750.c"
#include "../../src/game/8008B770.c"
#include "../../src/game/8008B790.c"
#include "../../src/game/8008B7B0.c"
#include "../../src/game/800469A0.c"
#define SoundVoice SoundVoice_46804
#define SoundBank SoundBank_46804
#define gSoundEntryTable gSoundEntryTable_46804
#include "../../src/game/80046804.c"
#undef SoundVoice
#undef SoundBank
#undef gSoundEntryTable

/* Synthetic host-RAM globals referenced by tested units & helpers. */
void *g_irq_cb_tbl      __asm__("D_8009DD10");
void *g_spu_irq_hdlr    __asm__("D_8009EEE0");
s32   g_prng_cur        __asm__("D_8009F5A8");
s32   g_prng_tbl[20]    __asm__("D_800A0480");
void *g_light_grid      __asm__("D_8009F780");
s32   g_light_pool      __asm__("D_8009F784");
void *g_res_head        __asm__("D_8009F5B8");
void *g_res_tail        __asm__("D_8009F5BC");
s32   g_req_off_bias    __asm__("D_8009F9F4");
u8    g_demo_cam[2][8]  __asm__("D_800A2290");
void *g_slot_descs[8]   __asm__("D_800A0C70");
s32   g_music_vol       __asm__("D_8009F334");
u8    g_bitfield_st     __asm__("D_8009F6B8");
s32   g_call_gate       __asm__("D_8009F6DC");
s32   g_vagm_prime      __asm__("D_8009F6E0");
s32   g_vagm_chunk      __asm__("D_8009F6E4");
s32   g_stream_buf[24]  __asm__("D_800A0B70");
u8   *g_res_subchunk    __asm__("D_8009F698");
s32   g_stream_avail    __asm__("D_800A09DC");
s32   g_stream_scale    __asm__("D_8009F64C");
s32   g_desc_cnt        __asm__("D_8009F848");
void *g_game_sess       __asm__("D_8009FA94");
struct Unk6AAEC g_vm_recs[8] __asm__("D_800A3340");
struct PadBuffer g_pad_bufs[4] __asm__("D_800A78F0");
void *g_voice_free      __asm__("D_8009F73C");
s32   g_voice_free_cnt  __asm__("D_8009F740");
void *g_voice_prio      __asm__("D_8009F744");
s32   g_voice_prio_cnt  __asm__("D_8009F748");
void *g_voice_cls0      __asm__("D_8009F74C");
s32   g_voice_cls0_cnt  __asm__("D_8009F750");
void *g_voice_cls2      __asm__("D_8009F754");
s32   g_voice_cls2_cnt  __asm__("D_8009F758");
struct SoundBank g_snd_banks[4] __asm__("D_800A7938");

struct SoundVoiceSlotB { s32 active; u8 unk04[0x14]; s32 owner; };
struct SoundSysB {
    s32 bankCount; void **banks; s32 channelCount; void *channels;
    s32 unk10; u32 pending; u32 acknowledged;
    struct SoundVoiceSlotB voices[4]; /* slot 3 is a phantom beyond the documented three */
};
struct SoundSysB g_snd_sys __asm__("D_800A07A8");

/* Forward declarations of the 25 unmodified units under test. */
extern void *func_80096108(void *callback);
extern void *func_8009612C(void *callback);
extern void  func_800979F8(void *callback);
extern void  func_80098C38(void *callback);
extern void *func_80098BF8(void *callback);
extern void  func_80028DC0(s32 arg);
extern void  func_800320E4(void *object);
extern void  func_800498F0(void *object);
extern void  func_80075478(void *object);
extern void  func_800381D4(void *arg);
extern void  func_8004D924(s32 *value);
extern void  func_80056924(void);
extern u8    func_8003B4E0(u8 index, u8 slot, u8 mode);
extern void  func_8002A16C(s32 arg0);
extern void  func_800243F8(s8 *arg0);
extern void  func_80022240(s32 owner);
extern s32   func_800538C8(u8 *context, u32 *flags, s32 *vert, s32 *horiz);
extern void  func_80029E20(void *resource);
extern u32   func_80029BE4(void);
extern void  func_8006BDFC(void *obj, s32 idx);
extern s32   func_8006BE90(void *obj, s32 idx, s32 step);
extern void  func_8008AE8C(void *port);
extern void  func_8008B168(void *port);
extern void  func_80046870(void *voice);
extern void  func_80046698(void *voice);

/* Helper: seed PRNG table to a constant so gAdvancePrimaryPrng returns 2*val on first call */
static void seed_prng(s32 val)
{
    int i;
    g_prng_cur = 5;
    for (i = 0; i < 20; i++) g_prng_tbl[i] = val;
}

/* Shared DMA/IRQ callback stub for 80096108, 8009612C, 800979F8, 80098C38, 80098BF8 */
static int g_cb_hits[4];
static int g_cb_ch;
static void *g_cb_fn;
static void *g_cb_ret;
static void *dma_cb_stub(int ch, void *fn) {
    g_cb_hits[1]++; g_cb_ch = ch; g_cb_fn = fn; return g_cb_ret;
}
static void irq_cb_stub(int irq, void *fn) {
    g_cb_hits[2]++; g_cb_ch = irq; g_cb_fn = fn;
}
static void *g_cb_tbl[4] = { 0, (void *)dma_cb_stub, (void *)irq_cb_stub, 0 };
static void reset_cb_tbl(void) {
    lk_fill(g_cb_hits, 0, sizeof(g_cb_hits));
    g_cb_ch = -1; g_cb_fn = 0; g_cb_ret = 0;
    g_irq_cb_tbl = g_cb_tbl;
}
static void dummy_cb_a(void) {}
static void dummy_cb_b(void) {}

/* 1. 80096108: gMdecSetInCallback */
static void test_80096108(void)
{
    int prev = 1;
    void *ret;
    /* @leaf 80096108 source src/game/80096108.c: registers MDEC input DMA callback on channel 0 */
    LK_FUNC("func_80096108");
    reset_cb_tbl(); g_cb_ret = &prev;
    ret = func_80096108((void *)dummy_cb_a);
    FCK_EQ(g_cb_hits[1], 1, "DMA slot 1 called");
    FCK_EQ(g_cb_ch, 0, "channel 0");
    FCK_CHECK(g_cb_fn == (void *)dummy_cb_a, "cb forwarded");
    FCK_CHECK(ret == &prev, "prev returned");
}

/* 2. 8009612C: gSetDecDctOutCallback */
static void test_8009612C(void)
{
    int prev = 2;
    void *ret;
    /* @leaf 8009612C source src/game/8009612C.c: registers MDEC output DMA callback on channel 1 */
    LK_FUNC("func_8009612C");
    reset_cb_tbl(); g_cb_ret = &prev;
    ret = func_8009612C((void *)dummy_cb_b);
    FCK_EQ(g_cb_hits[1], 1, "DMA slot 1 called");
    FCK_EQ(g_cb_ch, 1, "channel 1");
    FCK_CHECK(g_cb_fn == (void *)dummy_cb_b, "cb forwarded");
    FCK_CHECK(ret == &prev, "prev returned");
}

/* 3. 800979F8: gSpuSetDmaCallback */
static void test_800979F8(void)
{
    /* @leaf 800979F8 source src/game/800979F8.c: registers SPU DMA callback on channel 4 */
    LK_FUNC("func_800979F8");
    reset_cb_tbl();
    func_800979F8((void *)dummy_cb_a);
    FCK_EQ(g_cb_hits[1], 1, "DMA slot 1 called");
    FCK_EQ(g_cb_ch, 4, "channel 4");
    FCK_CHECK(g_cb_fn == (void *)dummy_cb_a, "cb forwarded");
}

/* 4. 80098C38: gSpuSetIrqCallback */
static void test_80098C38(void)
{
    /* @leaf 80098C38 source src/game/80098C38.c: registers SPU IRQ callback on interrupt 9 */
    LK_FUNC("func_80098C38");
    reset_cb_tbl();
    func_80098C38((void *)dummy_cb_a);
    FCK_EQ(g_cb_hits[2], 1, "IRQ slot 2 called");
    FCK_EQ(g_cb_ch, 9, "interrupt 9");
    FCK_CHECK(g_cb_fn == (void *)dummy_cb_a, "cb forwarded");
}

/* 5. 80098BF8: gSpuInstallIrqCallback */
static void test_80098BF8(void)
{
    void *prev;
    /* @leaf 80098BF8 source src/game/80098BF8.c: swaps SPU IRQ handler and registers only on change */
    LK_FUNC("func_80098BF8");
    reset_cb_tbl();
    g_spu_irq_hdlr = (void *)dummy_cb_a;
    prev = func_80098BF8((void *)dummy_cb_a);
    FCK_CHECK(prev == (void *)dummy_cb_a, "returns previous");
    FCK_EQ(g_cb_hits[2], 0, "unchanged handler skips registration");

    prev = func_80098BF8((void *)dummy_cb_b);
    FCK_CHECK(prev == (void *)dummy_cb_a, "returns old handler");
    FCK_CHECK(g_spu_irq_hdlr == (void *)dummy_cb_b, "global updated");
    FCK_EQ(g_cb_hits[2], 1, "new handler registered");
    FCK_CHECK(g_cb_fn == (void *)dummy_cb_b, "new handler forwarded");
}

/* 6. 80028DC0: gQueueCompletedModuleAsset */
struct QNode28DC0 { struct QNode28DC0 *next; s32 data; };
struct QState28DC0 { u8 pad[0xC]; s32 count; struct QNode28DC0 *head; };

static void test_80028DC0(void)
{
    struct QState28DC0 st;
    struct QNode28DC0 n0, n1;
    /* @leaf 80028DC0 source src/game/80028DC0.c: pops fixed pool slot and appends to asset queue tail */
    LK_FUNC("func_80028DC0");
    lk_fill(g_stream_buf, 0, sizeof(g_stream_buf));
    lk_fill(&st, 0, sizeof(st)); lk_fill(&n0, 0, sizeof(n0)); lk_fill(&n1, 0, sizeof(n1));
    n0.next = &n1; n1.next = 0;
    st.head = &n0; st.count = 2;
    g_stream_buf[0x2C / 4] = (s32)&st;
    g_stream_buf[0x30 / 4] = 0;

    func_80028DC0(111);
    FCK_CHECK(g_stream_buf[0x30 / 4] == (s32)&n0, "first node becomes head");
    FCK_EQ(n0.data, 111, "first node data");
    FCK_CHECK(n0.next == 0, "first node next null");
    FCK_EQ(st.count, 1, "pool count decremented");

    func_80028DC0(222);
    FCK_CHECK(n0.next == &n1, "second node appended to tail");
    FCK_EQ(n1.data, 222, "second node data");
    FCK_CHECK(n1.next == 0, "tail next null");
    FCK_EQ(st.count, 0, "pool count now 0");

    func_80028DC0(333);
    FCK_CHECK(n1.next == 0, "empty pool is no-op");
    FCK_EQ(st.count, 0, "count stays 0");
}

/* 7. 800320E4: gPickHoverWanderTarget */
struct Hover320E4 {
    u8 unk00[0x16];
    u8 kind;
    u8 unk17[0xB9];
    s32 baseX, baseY, baseZ;
    s32 targetX, targetY, targetZ;
    u8 unkE8[0x12];
    u8 state;
    u8 unkFB[0x11];
    s16 spreadZ, spreadX;
};

static void test_800320E4(void)
{
    struct Hover320E4 obj;
    /* @leaf 800320E4 source src/game/800320E4.c: picks randomized hover wander target unless state/kind blocks */
    LK_FUNC("func_800320E4");
    lk_fill(&obj, 0, sizeof(obj));
    obj.baseX = 1000; obj.baseY = 2000; obj.baseZ = 3000;
    obj.targetX = -1; obj.targetY = -1; obj.targetZ = -1;
    obj.spreadX = 8; obj.spreadZ = 16;

    obj.state = 3; obj.kind = 1;
    func_800320E4(&obj);
    FCK_EQ(obj.targetY, -1, "state==3 blocked");

    obj.state = 1; obj.kind = 0x25;
    func_800320E4(&obj);
    FCK_EQ(obj.targetY, -1, "kind==0x25 blocked");

    /* seed_prng(0x280) -> returns 0x500 -> (0x500 - 0x400) = 0x100 */
    seed_prng(0x280);
    obj.state = 1; obj.kind = 1;
    func_800320E4(&obj);
    FCK_EQ(obj.targetY, 2000, "targetY copies baseY");
    FCK_EQ(obj.targetX, 1000 + 0x100, "targetX offset by spreadX");
    FCK_EQ(obj.targetZ, 3000 + 0x200, "targetZ offset by spreadZ");
}

/* 8 & 9. 800498F0 & 80075478: light grid node unlink/return & particle light release */
struct LightNode { s16 unk00, cell; u8 unk04[0x3C]; struct LightNode *next; };
struct LightPool { u8 pad[0xC]; s32 count; void *head; };

static void test_800498F0(void)
{
    struct LightNode *grid[4];
    struct LightNode a, b, c;
    struct LightPool pool;
    /* @leaf 800498F0 source src/game/800498F0.c: unlinks light node from grid cell and returns to pool */
    LK_FUNC("func_800498F0");
    lk_fill(grid, 0, sizeof(grid));
    lk_fill(&a, 0, sizeof(a)); lk_fill(&b, 0, sizeof(b)); lk_fill(&c, 0, sizeof(c));
    lk_fill(&pool, 0, sizeof(pool));
    g_light_grid = grid;
    g_light_pool = (s32)&pool;

    a.cell = 1; a.next = &b;
    b.cell = 1; b.next = &c;
    c.cell = 1; c.next = 0;
    grid[1] = &a;

    func_800498F0(&b);
    FCK_CHECK(grid[1] == &a, "head preserved on mid unlink");
    FCK_CHECK(a.next == &c, "mid unlinked");
    FCK_CHECK(pool.head == &b, "b returned to pool");
    FCK_EQ(pool.count, 1, "pool count incremented");

    func_800498F0(&a);
    FCK_CHECK(grid[1] == &c, "head updated to c");
    FCK_CHECK(pool.head == &a, "a returned to pool");
    FCK_EQ(pool.count, 2, "pool count is 2");

    /* Boundary: cell == 0 exercises >= 0 exactly at the edge (ge->gt mutant). */
    lk_fill(grid, 0, sizeof(grid));
    lk_fill(&a, 0, sizeof(a)); lk_fill(&pool, 0, sizeof(pool));
    a.cell = 0; a.next = 0; grid[0] = &a;
    func_800498F0(&a);
    FCK_CHECK(grid[0] == 0, "cell 0 head cleared (>= boundary)");
    FCK_CHECK(pool.head == &a, "cell 0 node returned to pool");
    FCK_EQ(pool.count, 1, "fresh pool count is 1");

    /* Negative cell skips unlink but still returns to pool. */
    c.cell = -1;
    func_800498F0(&c);
    FCK_CHECK(pool.head == &c, "negative cell still returns to pool");
    FCK_EQ(pool.count, 2, "pool count is 2 after c");
}
struct Part75478 { u8 unk00[0x24]; void *light; };

static void test_80075478(void)
{
    struct LightNode *grid[4];
    struct LightNode n;
    struct LightPool pool;
    struct Part75478 part;
    /* @leaf 80075478 source src/game/80075478.c: releases attached particle light and clears pointer */
    LK_FUNC("func_80075478");
    lk_fill(grid, 0, sizeof(grid)); lk_fill(&n, 0, sizeof(n));
    lk_fill(&pool, 0, sizeof(pool)); lk_fill(&part, 0, sizeof(part));
    g_light_grid = grid; g_light_pool = (s32)&pool;

    func_80075478(&part);
    FCK_EQ(pool.count, 0, "null light is no-op");

    n.cell = 2; n.next = 0; grid[2] = &n; part.light = &n;
    func_80075478(&part);
    FCK_CHECK(part.light == 0, "light pointer cleared");
    FCK_CHECK(grid[2] == 0, "unlinked from grid");
    FCK_CHECK(pool.head == &n, "returned to pool");
    FCK_EQ(pool.count, 1, "pool count incremented");
}

/* 10. 800381D4: gStepIdleActorTurn */
struct Actor381D4 {
    u8 pad00[0x1C]; s16 unk1C; u8 pad1E[0x86]; u16 unkA4;
    u8 padA6[4]; s16 unkAA; s32 unkAC, unkB0; u16 unkB4;
    u8 padB6[0x62]; u32 unk118; u8 pad11C[0x14]; s16 unk130;
    u8 pad132[4]; u16 unk136;
};

static void test_800381D4(void)
{
    struct Actor381D4 act;
    /* @leaf 800381D4 source src/game/800381D4.c: steps idle actor turn state machine with PRNG gate */
    LK_FUNC("func_800381D4");
    lk_fill(&act, 0, sizeof(act));
    act.unk1C = 55; act.unk118 = 0;
    seed_prng(7); /* 14 -> &0xF = 0xE >= 0xC -> breaks without clearing unk1C */
    func_800381D4(&act);
    FCK_EQ(act.unk1C, 55, "PRNG >= 0xC keeps unk1C");
    FCK_EQ(act.unk118, 0, "state stays 0");

    /* Boundary: PRNG nibble exactly 0xC stays in the guard branch (lt boundary). */
    lk_fill(&act, 0, sizeof(act));
    act.unk1C = 55; act.unk118 = 0;
    seed_prng(6); /* 6*2 = 12 = 0xC -> not < 0xC -> break, no transition */
    func_800381D4(&act);
    FCK_EQ(act.unk1C, 55, "PRNG nibble 0xC keeps unk1C (boundary)");
    FCK_EQ(act.unk118, 0, "state stays 0 at boundary");

    lk_fill(&act, 0, sizeof(act));
    act.unk1C = 55; act.unk118 = 7; act.unk130 = 42; act.unk136 = 0x900;
    seed_prng(1); /* 1st=2 (<0xC), 2nd=2 -> (2&3)+1=3, unk136>=0x801 -> -3 */
    func_800381D4(&act);
    FCK_EQ(act.unk1C, 0, "falls through to clear unk1C");
    FCK_EQ(act.unkAA, -3, "negative turn step");
    FCK_EQ(act.unkB0, 42, "unkB0 copied from unk130");
    FCK_EQ(act.unkB4, 0x900, "unkB4 copied from unk136");
    FCK_CHECK((act.unkA4 & 0x100) != 0, "unkA4 bit 0x100 set");
    FCK_EQ(act.unk118, (s32)(0x40000000u | 2u), "unk118 transitions to state 1 & clears bits 0,2");

    /* Boundary: unk136 == 0x801 exactly is the >= edge (ge->gt mutant). */
    lk_fill(&act, 0, sizeof(act));
    act.unk1C = 55; act.unk118 = 7; act.unk130 = 42; act.unk136 = 0x801;
    seed_prng(1);
    func_800381D4(&act);
    FCK_EQ(act.unkAA, -3, "unk136 == 0x801 negates turn step (>= boundary)");

    lk_fill(&act, 0, sizeof(act));
    act.unk118 = 0x40000000u; act.unk1C = 99; act.unkAA = 7;
    func_800381D4(&act);
    FCK_EQ(act.unk1C, 0, "case 1 clears unk1C");
    FCK_EQ(act.unkAA, 7, "case 1 leaves unkAA");
}

/* 11. 8004D924: gRegisterResourceWithBias */
struct Res4D924 { s32 tag; u8 pad[8]; void *next, *prev; };

static void test_8004D924(void)
{
    struct Res4D924 r;
    /* @leaf 8004D924 source src/game/8004D924.c: appends resource to registry and stores *value bias */
    LK_FUNC("func_8004D924");
    lk_fill(&r, 0, sizeof(r));
    r.tag = 0x1234;
    g_res_head = 0; g_res_tail = 0; g_req_off_bias = -1;
    func_8004D924(&r.tag);
    FCK_CHECK(g_res_head == &r, "appended as head");
    FCK_CHECK(g_res_tail == &r, "appended as tail");
    FCK_EQ(g_req_off_bias, 0x1234, "bias set from *value");
}

/* 12. 80056924: gInitDemoCameraStates */
static void test_80056924(void)
{
    int i;
    /* @leaf 80056924 source src/game/80056924.c: fills 2x8 demo camera states in range [7, 14] */
    LK_FUNC("func_80056924");
    lk_fill(g_demo_cam, 0, sizeof(g_demo_cam));
    seed_prng(1); /* first few calls yield 2 -> (2&7)+7 = 9 */
    func_80056924();
    FCK_EQ(g_demo_cam[0][0], 9, "row 0 col 0");
    FCK_EQ(g_demo_cam[1][0], 9, "row 1 col 0");
    for (i = 0; i < 8; i++) {
        FCK_CHECK(g_demo_cam[0][i] >= 7 && g_demo_cam[0][i] <= 14, "row 0 in [7,14]");
        FCK_CHECK(g_demo_cam[1][i] >= 7 && g_demo_cam[1][i] <= 14, "row 1 in [7,14]");
    }
}

/* 13. 8003B4E0: gSelectEntitySlotType */
static void test_8003B4E0(void)
{
    u8 desc[0x20];
    /* @leaf 8003B4E0 source src/game/8003B4E0.c: selects slot type or column override on PRNG bit 0 */
    LK_FUNC("func_8003B4E0");
    lk_fill(desc, 0, sizeof(desc));
    desc[4] = 0x11;  /* slot 0 */
    desc[8] = 0x22;  /* slot 1 */
    desc[0x1A] = 0x77; /* column */
    g_slot_descs[2] = desc;

    /* slot != 0 -> reads slot byte directly */
    FCK_EQ(func_8003B4E0(2, 1, 1), 0x22, "slot 1 reads +8");
    /* mode == 0 -> reads slot 0 byte directly */
    FCK_EQ(func_8003B4E0(2, 0, 0), 0x11, "mode 0 reads +4");
    /* flags & 1 set -> disables column override even with odd PRNG */
    desc[0x18] = 1;
    g_prng_cur = 5; g_prng_tbl[4] = 1; g_prng_tbl[7] = 0; /* sum = 1 (odd) */
    FCK_EQ(func_8003B4E0(2, 0, 1), 0x11, "flags&1 blocks override");
    /* flags & 1 clear, slot==0, mode!=0, odd PRNG -> returns column */
    desc[0x18] = 0;
    g_prng_cur = 5; g_prng_tbl[4] = 1; g_prng_tbl[7] = 0;
    FCK_EQ(func_8003B4E0(2, 0, 1), 0x77, "odd PRNG selects column");
    /* even PRNG -> returns slot 0 */
    g_prng_cur = 5; g_prng_tbl[4] = 2; g_prng_tbl[7] = 0;
    FCK_EQ(func_8003B4E0(2, 0, 1), 0x11, "even PRNG keeps slot 0");
}

/* 14. 8002A16C: gStartVagmAudioStream */
static void test_8002A16C(void)
{
    u8 *p = (u8 *)g_stream_buf;
    /* @leaf 8002A16C source src/game/8002A16C.c: starts VAGM audio stream and computes chunk address */
    LK_FUNC("func_8002A16C");
    lk_fill(g_stream_buf, 0, sizeof(g_stream_buf));
    g_stream_buf[0x44 / 4] = 1;
    g_stream_buf[0x48 / 4] = 0x100;
    g_stream_buf[0x4C / 4] = 0x2000;
    p[0x20] = 1;
    g_bitfield_st = 0;
    g_call_gate = 0;
    g_vagm_prime = 9;
    g_vagm_chunk = 2; /* 2 * 0x6000 = 0xC000 */
    g_music_vol = 0;

    /* arg0!=0 and music_vol==0 -> blocked */
    func_8002A16C(1);
    FCK_EQ(g_call_gate, 0, "zero music volume blocks start");

    /* arg0==0 bypasses volume check -> gate 0->3, prime=0, unk0C=-1, unk20=0, unk08=0xE100 */
    func_8002A16C(0);
    FCK_EQ(g_call_gate, 3, "gate transitions 0 -> 3");
    FCK_EQ(g_vagm_prime, 0, "prime count zeroed");
    FCK_EQ(g_stream_buf[0x0C / 4], -1, "unk0C set to -1");
    FCK_EQ(p[0x20], 0, "unk20 cleared");
    FCK_EQ(g_stream_buf[0x08 / 4], 0x2000 + 0x100 + 2 * 0x6000, "stream offset computed");

    /* gate already nonzero -> transitions to 4 */
    p[0x20] = 1; g_music_vol = 5; g_vagm_chunk = 1;
    func_8002A16C(1);
    FCK_EQ(g_call_gate, 4, "nonzero gate transitions to 4");
    FCK_EQ(g_stream_buf[0x08 / 4], 0x2000 + 0x100 + 0x6000, "new offset computed");
}

/* 15 & 16. 800243F8 & 80022240: voice channel release & release by owner */
static void test_800243F8(void)
{
    s8 voice[0x20];
    /* @leaf 800243F8 source src/game/800243F8.c: moves channel bit from pending to acknowledged */
    LK_FUNC("func_800243F8");
    lk_fill(&g_snd_sys, 0, sizeof(g_snd_sys));
    lk_fill(voice, 0, sizeof(voice));
    g_snd_sys.pending = (1u << 5) | (1u << 2);
    g_snd_sys.acknowledged = 0;
    voice[0xD] = 5;
    func_800243F8(voice);
    FCK_EQ(g_snd_sys.pending, (s32)(1u << 2), "bit 5 cleared from pending");
    FCK_EQ(g_snd_sys.acknowledged, (s32)(1u << 5), "bit 5 set in acknowledged");
}

static void test_80022240(void)
{
    /* @leaf 80022240 source src/game/80022240.c: releases active sound voice slots matching owner */
    LK_FUNC("func_80022240");
    lk_fill(&g_snd_sys, 0, sizeof(g_snd_sys));
    g_snd_sys.pending = (1u << 3) | (1u << 4);
    g_snd_sys.voices[0].active = 1; g_snd_sys.voices[0].owner = 3;
    g_snd_sys.voices[1].active = 1; g_snd_sys.voices[1].owner = 4;
    g_snd_sys.voices[2].active = 0; g_snd_sys.voices[2].owner = 3;
    g_snd_sys.voices[3].active = 1; g_snd_sys.voices[3].owner = 3; /* phantom slot past the 3-voice array */

    func_80022240(3);
    FCK_EQ(g_snd_sys.voices[3].active, 1, "phantom voice slot 3 untouched (loop bound is exactly 3)");
    FCK_EQ(g_snd_sys.voices[0].active, 0, "slot 0 deactivated");
    FCK_EQ(g_snd_sys.voices[0].owner, -1, "slot 0 owner cleared");
    FCK_EQ(g_snd_sys.voices[1].active, 1, "other owner slot untouched");
    FCK_EQ(g_snd_sys.voices[1].owner, 4, "other owner preserved");
    FCK_EQ(g_snd_sys.voices[2].owner, 3, "inactive slot untouched");
    FCK_EQ(g_snd_sys.pending, (s32)(1u << 4), "owner 3 bit cleared from pending");
    FCK_EQ(g_snd_sys.acknowledged, (s32)(1u << 3), "owner 3 bit acknowledged");
}

/* 17. 800538C8: gReadAnalogMovementDirections */
static void test_800538C8(void)
{
    u8 ctx[0x320];
    u32 flags;
    s32 vert, horiz;
    /* @leaf 800538C8 source src/game/800538C8.c: decodes analog stick deadzone, direction flags, magnitudes */
    LK_FUNC("func_800538C8");
    lk_fill(ctx, 0, sizeof(ctx));
    lk_fill(g_pad_bufs, 0, sizeof(g_pad_bufs));
    ctx[0xC1] = 1; /* port 1 */
    g_pad_bufs[1].unk00[0] = 0;
    g_pad_bufs[1].unk00[1] = 0x70; /* analog pad type 7 */
    g_pad_bufs[1].leftX = 0xC0; /* 0xC0 - 0x80 = +0x40 -> dir 0x20000000 */
    g_pad_bufs[1].leftY = 0x40; /* 0x40 - 0x80 = -0x40 -> dir 0x40000000 */

    /* Already has direction bits -> returns 0 */
    flags = 0x10000000u; vert = -1; horiz = -1;
    FCK_EQ(func_800538C8(ctx, &flags, &vert, &horiz), 0, "preset dir flags short-circuit");

    /* Active read: +X, -Y */
    flags = 5;
    FCK_EQ(func_800538C8(ctx, &flags, &vert, &horiz), 1, "analog read succeeds");
    FCK_EQ(flags, (s32)(5u | 0x20000000u | 0x40000000u), "+X/-Y direction bits set");
    FCK_EQ(horiz, 0x40, "horizontal magnitude 0x40");
    FCK_EQ(vert, 0x40, "vertical magnitude 0x40");

    /* Inside deadzone (0x80, 0x90) -> magnitudes 0, no dir bits */
    g_pad_bufs[1].leftX = 0x80; g_pad_bufs[1].leftY = 0x90;
    flags = 0;
    FCK_EQ(func_800538C8(ctx, &flags, &vert, &horiz), 1, "deadzone still returns 1");
    FCK_EQ(flags, 0, "no direction bits in deadzone");
    FCK_EQ(horiz, 0, "horiz 0 in deadzone");
    FCK_EQ(vert, 0, "vert 0 in deadzone");

    /* Non-analog pad type (4) -> returns 0 */
    g_pad_bufs[1].unk00[1] = 0x40;
    FCK_EQ(func_800538C8(ctx, &flags, &vert, &horiz), 0, "digital pad returns 0");

    /* Boundary: x = +0x28 is exactly at the deadzone edge; original treats
     * (u32)(x + 0x27) < 0x4F strictly, so edge stays live (mutation lt->le
     * would wrongly null it). Y stays centred. */
    g_pad_bufs[1].unk00[1] = 0x70;
    g_pad_bufs[1].leftX = 0xA8; /* 0xA8 - 0x80 = +0x28 -> +0x28 + 0x27 == 0x4F */
    g_pad_bufs[1].leftY = 0x80;
    flags = 0;
    FCK_EQ(func_800538C8(ctx, &flags, &vert, &horiz), 1, "edge x=0x28 still returns 1");
    FCK_EQ(horiz, 0x28, "x at live edge keeps magnitude 0x28 (strict < boundary)");
    FCK_EQ((s32)flags, (s32)(0x20000000u), "edge x=+0x28 sets +X direction bit");
    FCK_EQ(vert, 0, "y centred -> no vertical bits");
}

/* 18. 80029E20: gRegisterPortraitStreamTable */
struct Res29E20 { s32 unk00; u8 unk04[8]; void *next, *prev; u8 pad14[8]; s32 unk1C, unk20; };

static void test_80029E20(void)
{
    struct Res29E20 res;
    s32 blob[6];
    /* @leaf 80029E20 source src/game/80029E20.c: appends portrait resource and locates 'SOTt' subchunk */
    LK_FUNC("func_80029E20");
    lk_fill(&res, 0, sizeof(res));
    lk_fill(blob, 0, sizeof(blob));
    blob[0] = 0x74544F53; /* 'SOTt' */
    blob[1] = 24;
    blob[2] = 7;          /* arg3 match */
    blob[3] = 0xCAFE;
    res.unk00 = (s32)blob;
    res.unk1C = 7;
    res.unk20 = sizeof(blob);
    g_res_head = 0; g_res_tail = 0; g_res_subchunk = 0;

    func_80029E20(&res);
    FCK_CHECK(g_res_head == &res, "resource registered as head");
    FCK_CHECK(g_res_subchunk == (u8 *)&blob[3], "SOTt payload pointer stored");
}

/* 19. 80029BE4: gIsAudioStreamReady */
static void test_80029BE4(void)
{
    /* @leaf 80029BE4 source src/game/80029BE4.c: checks audio stream readiness from gate & block deficit */
    LK_FUNC("func_80029BE4");
    g_call_gate = 3;
    FCK_EQ(func_80029BE4(), 0, "gate 3 -> not ready");
    g_call_gate = 4;
    FCK_EQ(func_80029BE4(), 0, "gate 4 -> not ready");
    g_call_gate = 0;
    FCK_EQ(func_80029BE4(), 1, "gate 0 -> ready");
    /* gate 1: returns (scale * (3 - avail)) >= 0x227 (551) */
    g_call_gate = 1; g_stream_avail = 1; g_stream_scale = 275; /* 275*2 = 550 < 551 */
    FCK_EQ(func_80029BE4(), 0, "deficit 550 < 551 -> 0");
    g_stream_scale = 276; /* 276*2 = 552 >= 551 */
    FCK_EQ(func_80029BE4(), 1, "deficit 552 >= 551 -> 1");
    /* Boundary: deficit exactly 551 (>= equality) */
    g_stream_avail = 2; g_stream_scale = 551;
    FCK_EQ(func_80029BE4(), 1, "deficit exactly 551 -> 1 (boundary)");
}

/* 20 & 21. 8006BDFC & 8006BE90: weapon slot reserve & ammo refill */
struct WepCtl {
    u8 unk00[0x78]; s32 unk78[3]; s16 unk84[3]; s16 unk8A[3]; u8 unk90[3]; u8 unk93[3];
    u8 pad96[0xC1 - 0x96]; s8 playerSide;
};
struct WepObj { u8 unk00[0xAC]; struct WepCtl *ctl; };
struct WepSess { u8 unk00[8]; s32 unk8; u8 unk0C[0x8C]; s32 unk98; };

static void test_8006BDFC(void)
{
    struct WepCtl ctl;
    struct WepObj obj;
    struct WepSess sess;
    u8 *rec;
    /* @leaf 8006BDFC source src/game/8006BDFC.c: refills weapon reserve and swaps empty-slot fields */
    LK_FUNC("func_8006BDFC");
    lk_fill(&ctl, 0, sizeof(ctl)); lk_fill(&obj, 0, sizeof(obj));
    lk_fill(&sess, 0, sizeof(sess)); lk_fill(g_vm_recs, 0, sizeof(g_vm_recs));
    obj.ctl = &ctl; ctl.playerSide = 0; ctl.unk90[1] = 7;
    g_desc_cnt = 1; g_game_sess = &sess;
    sess.unk8 = 1; /* +25% scaling */
    rec = (u8 *)&g_vm_recs[0];
    g_vm_recs[0].id = 7;
    *(s16 *)(rec + 0x28) = 40;  /* reserve base -> 40 + 10 = 50 */
    rec[0x2A] = 0x55;
    *(s32 *)(rec + 0x44) = 111;
    *(s32 *)(rec + 0x50) = 222;

    /* Empty reserve (<=0): sets unk93, swaps unk44/unk50, refills to 50 */
    ctl.unk8A[1] = 0;
    func_8006BDFC(&obj, 1);
    FCK_EQ(ctl.unk93[1], 0x55, "unk93 loaded from tgt+0x2A");
    FCK_EQ(*(s32 *)(rec + 0x44), 222, "unk44 swapped");
    FCK_EQ(*(s32 *)(rec + 0x50), 111, "unk50 swapped");
    FCK_EQ(ctl.unk8A[1], 50, "reserve refilled to scaled limit 50");

    /* Non-empty reserve (>0): does not swap, still updates reserve */
    sess.unk8 = 0; /* 100% -> 40 */
    func_8006BDFC(&obj, 1);
    FCK_EQ(*(s32 *)(rec + 0x44), 222, "unk44 not swapped when reserve > 0");
    FCK_EQ(ctl.unk8A[1], 40, "reserve updated to 40");
}

static void test_8006BE90(void)
{
    struct WepCtl ctl;
    struct WepObj obj;
    struct WepSess sess;
    u8 *rec;
    /* @leaf 8006BE90 source src/game/8006BE90.c: refills weapon slot ammo by step or to full max */
    LK_FUNC("func_8006BE90");
    lk_fill(&ctl, 0, sizeof(ctl)); lk_fill(&obj, 0, sizeof(obj));
    lk_fill(&sess, 0, sizeof(sess)); lk_fill(g_vm_recs, 0, sizeof(g_vm_recs));
    obj.ctl = &ctl; ctl.playerSide = 0; ctl.unk90[0] = 3;
    g_desc_cnt = 1; g_game_sess = &sess; sess.unk8 = 0;
    rec = (u8 *)&g_vm_recs[0];
    g_vm_recs[0].id = 3;
    *(s16 *)(rec + 0x08) = 100; /* ammo max = 100 */

    /* Partial step */
    ctl.unk84[0] = 30;
    FCK_EQ(func_8006BE90(&obj, 0, 25), 0, "refill returns 0");
    FCK_EQ(ctl.unk84[0], 55, "ammo increased by 25");
    /* Step clamping at max */
    FCK_EQ(func_8006BE90(&obj, 0, 80), 0, "clamped refill returns 0");
    FCK_EQ(ctl.unk84[0], 100, "ammo clamped at max");
    /* Already full -> returns 1 */
    FCK_EQ(func_8006BE90(&obj, 0, 10), 1, "full slot returns 1");
    /* Negative step -> full refill */
    ctl.unk84[0] = 10;
    FCK_EQ(func_8006BE90(&obj, 0, -1), 0, "negative step returns 0");
    FCK_EQ(ctl.unk84[0], 100, "negative step fills to max");
}

/* 22 & 23. 8008AE8C & 8008B168: pad handshake command state dispatchers */
struct PadPortFull {
    struct CommandState cmd;
    u8 pad_after_cmd[0x46 - sizeof(struct CommandState)];
    u8 sequence_step;
    u8 info_record_index;
    u8 combination_bytes_remaining;
    u8 pad_49[0xE4 - 0x49];
    u8 current_mode_index;
};

static void test_8008AE8C(void)
{
    struct PadPortFull p;
    /* @leaf 8008AE8C source src/game/8008AE8C.c: dispatches pad info mode commands for steps 2/3/4 */
    LK_FUNC("func_8008AE8C");
    lk_fill(&p, 0, sizeof(p));
    p.current_mode_index = 3; p.info_record_index = 5;

    p.sequence_step = 2;
    func_8008AE8C(&p);
    FCK_EQ(p.cmd.current, 0x45, "step 2 sets model query 0x45");
    FCK_EQ(p.cmd.active, 0, "step 2 active=0");

    p.sequence_step = 3;
    func_8008AE8C(&p);
    FCK_EQ(p.cmd.current, 0x4C, "step 3 sets mode const 0x4C");
    FCK_EQ(p.cmd.inline_data[0], 3, "step 3 uses current_mode_index");
    FCK_EQ(p.cmd.active, 1, "step 3 active=1");

    p.sequence_step = 4;
    func_8008AE8C(&p);
    FCK_EQ(p.cmd.current, 0x47, "step 4 sets comb info 0x47");
    FCK_EQ(p.cmd.inline_data[0], 5, "step 4 uses info_record_index");
}

static void test_8008B168(void)
{
    struct PadPortFull p;
    /* @leaf 8008B168 source src/game/8008B168.c: dispatches pad table handshake commands for steps 2/3/4 */
    LK_FUNC("func_8008B168");
    lk_fill(&p, 0, sizeof(p));
    p.info_record_index = 4;

    p.sequence_step = 2;
    func_8008B168(&p);
    FCK_EQ(p.cmd.current, 0x4C, "step 2 sets 0x4C");
    FCK_EQ(p.cmd.inline_data[0], 4, "step 2 uses info_record_index");

    p.sequence_step = 3;
    func_8008B168(&p);
    FCK_EQ(p.cmd.current, 0x46, "step 3 sets act info 0x46");
    FCK_EQ(p.cmd.inline_data[0], 4, "step 3 uses info_record_index");

    p.sequence_step = 4; p.combination_bytes_remaining = 0;
    func_8008B168(&p);
    FCK_EQ(p.cmd.current, 0x47, "step 4 rem==0 sets comb info 0x47");

    p.combination_bytes_remaining = 2;
    func_8008B168(&p);
    FCK_EQ(p.cmd.current, 0x4B, "step 4 rem!=0 sets comb cont 0x4B");
    FCK_EQ(p.cmd.active, 0, "comb cont active=0");
}

/* 24 & 25. 80046870 & 80046698: sound voice link & release */
struct VoiceOwner { u8 pad[0x28]; s32 unk28; };
struct FullVoice {
    s32 bank;
    u8 volume; s8 unk5; u8 unk6, unk7;
    s32 priority;
    s8 pan; s8 chanIdx; u8 unk0E, unk0F;
    s32 seq; s32 unk14;
    struct VoiceOwner *owner;
    struct FullVoice *next;
    struct FullVoice *prev;
    struct FullVoice *bankNext;
    struct FullVoice *bankPrev;
};

static void reset_voices(void)
{
    g_voice_free = 0; g_voice_free_cnt = 0;
    g_voice_prio = 0; g_voice_prio_cnt = 0;
    g_voice_cls0 = 0; g_voice_cls0_cnt = 0;
    g_voice_cls2 = 0; g_voice_cls2_cnt = 0;
    lk_fill(g_snd_banks, 0, sizeof(g_snd_banks));
    lk_fill(&g_snd_sys, 0, sizeof(g_snd_sys));
}

static void test_80046870(void)
{
    struct FullVoice v0, v1, v2, v3;
    /* @leaf 80046870 source src/game/80046870.c: links sound voice into class/priority & bank lists */
    LK_FUNC("func_80046870");
    reset_voices();
    lk_fill(&v0, 0, sizeof(v0)); lk_fill(&v1, 0, sizeof(v1));
    lk_fill(&v2, 0, sizeof(v2)); lk_fill(&v3, 0, sizeof(v3));

    /* pan=0 -> class 0 list + bank 1 */
    v0.bank = 1; v0.pan = 0; v0.priority = 10;
    func_80046870(&v0);
    FCK_CHECK(g_voice_cls0 == &v0, "v0 linked in class 0");
    FCK_EQ(g_voice_cls0_cnt, 1, "class 0 count = 1");
    FCK_CHECK((void *)g_snd_banks[1].head == &v0, "v0 inserted in bank 1");
    FCK_EQ(g_snd_banks[1].voices, 1, "bank 1 voice count = 1");

    /* pan=2, bank=0xFF -> class 2 list only */
    v1.bank = 0xFF; v1.pan = 2;
    func_80046870(&v1);
    FCK_CHECK(g_voice_cls2 == &v1, "v1 linked in class 2");
    FCK_EQ(g_voice_cls2_cnt, 1, "class 2 count = 1");

    /* default pan (1): priority-ordered list (v2 prio=20, then v3 prio=5 inserted before v2) */
    v2.bank = 0xFF; v2.pan = 1; v2.priority = 20;
    v3.bank = 0xFF; v3.pan = 1; v3.priority = 5;
    func_80046870(&v2);
    func_80046870(&v3);
    FCK_CHECK(g_voice_prio == &v3, "lower priority v3 inserted at head");
    FCK_CHECK(v3.next == &v2, "v3->next is v2");
    FCK_CHECK(v2.prev == &v3, "v2->prev is v3");
    FCK_EQ(g_voice_prio_cnt, 2, "priority list count = 2");

    /* Equal priorities: original keeps the earlier voice ahead of the later one
     * (strict < in the scan), so order is tie-preserving FIFO. */
    reset_voices();
    lk_fill(&v0, 0, sizeof(v0)); lk_fill(&v1, 0, sizeof(v1));
    v0.bank = 0xFF; v0.pan = 1; v0.priority = 10;
    v1.bank = 0xFF; v1.pan = 1; v1.priority = 10;
    func_80046870(&v0);
    func_80046870(&v1);
    FCK_CHECK(g_voice_prio == &v0, "equal priority: earlier voice stays ahead (tie order)");
    FCK_CHECK(v0.next == &v1, "v0->next is v1");
    FCK_CHECK(v1.prev == &v0, "v1->prev is v0");
    FCK_EQ(g_voice_prio_cnt, 2, "tie list count = 2");
}

static void test_80046698(void)
{
    struct FullVoice v0, v1;
    struct VoiceOwner own;
    /* @leaf 80046698 source src/game/80046698.c: releases active voice, unlinks lists/bank, clears owner */
    LK_FUNC("func_80046698");
    reset_voices();
    lk_fill(&v0, 0, sizeof(v0)); lk_fill(&v1, 0, sizeof(v1)); lk_fill(&own, 0, sizeof(own));

    /* seq (unk10) == 0 -> no-op */
    v0.seq = 0;
    func_80046698(&v0);
    FCK_EQ(g_voice_free_cnt, 0, "inactive voice ignored");

    /* Active pan(priority byte at +0x0C)=0 head voice in bank 1 with owner */
    own.unk28 = 99;
    v0.bank = 1; v0.pan = 0; v0.chanIdx = 3; v0.seq = 10; v0.owner = &own;
    v0.next = &v1; v1.prev = &v0; v1.pan = 2; v1.seq = 20; v1.bank = 0xFF;
    g_voice_cls0 = &v0; g_voice_cls0_cnt = 1;
    g_snd_banks[1].head = (struct SoundVoice *)&v0; g_snd_banks[1].voices = 1;
    g_snd_sys.pending = (1u << 3);

    func_80046698(&v0);
    FCK_EQ(v0.seq, 0, "seq cleared");
    FCK_CHECK(g_voice_cls0 == &v1, "class 0 head advanced");
    FCK_CHECK(v1.prev == 0, "v1->prev cleared");
    FCK_EQ(g_voice_cls0_cnt, 0, "class 0 count decremented");
    FCK_EQ(g_voice_free_cnt, 1, "free count incremented");
    FCK_CHECK(g_voice_free == &v0, "v0 pushed to free list");
    FCK_CHECK(g_snd_banks[1].head == 0, "unlinked from bank 1");
    FCK_EQ(g_snd_banks[1].voices, 0, "bank 1 voice count = 0");
    FCK_EQ(own.unk28, 0, "owner unk28 cleared");
    FCK_EQ(g_snd_sys.acknowledged, (s32)(1u << 3), "channel 3 release queued");
}

int main(void)
{
    test_80096108();
    test_8009612C();
    test_800979F8();
    test_80098C38();
    test_80098BF8();
    test_80028DC0();
    test_800320E4();
    test_800498F0();
    test_80075478();
    test_800381D4();
    test_8004D924();
    test_80056924();
    test_8003B4E0();
    test_8002A16C();
    test_800243F8();
    test_80022240();
    test_800538C8();
    test_80029E20();
    test_80029BE4();
    test_8006BDFC();
    test_8006BE90();
    test_8008AE8C();
    test_8008B168();
    test_80046870();
    test_80046698();
    return lk_done("leaf_century_b");
}
