/* Host harness: map-placed objects, base ownership and unit spawners.
 *
 * Runs the *unchanged* retail units of the Precinct Assault map-object path
 * under qemu-mipsel:
 *
 *   func_8003FA34 gQueueOrApplyMapOccupancy  occupancy registration, immediate
 *                                            vs. deferred onto D_8009F708
 *   func_8003BC0C gSpawnEntityPrimaryType    the writer the queue drains into
 *   func_8003B838 gGetSpawnDescColumn        per-object slot column lookup
 *   func_8003BF50 gGetEntitySpawnBank        kind -> bank word forwarded to the
 *                                            occupancy writer
 *   func_8003B65C gQuarterIndex              counter -> 0..3 bucketing
 *   func_8003BA50 gFlagQuartileChange        quartile crossing flag
 *   func_800195A4 gTakeFixedPoolSlot         the pool the deferral allocates from
 *   func_8002D1A0 gTryCaptureObjective       proximity capture + ownership
 *   func_80060E74 gRegisterUnitSpawner       spawner record registration
 *   func_80019CA0/80019C38/80019BF0/80019A74 resource blob walk + registry
 *
 * Memory model (same as spawner_path_harness.c): qemu-mipsel cannot map guest
 * addresses >= 0x80000000, so the retail RAM window lives at
 * (retail address - 0x70000000) in a section relocated to 0x10010000. The
 * executable image is never loaded; every D_ symbol the units read is defined
 * by the test script at its relocated address and initialised by reset().
 *
 * Fixtures live in ordinary harness memory, so a pointer written into the
 * retail window is already a valid host address. Nothing is left to
 * zero-initialised BSS: every reset function sets every slot the tests read.
 *
 * Stubs (not part of the parity claim, they stand in for engine services the
 * harness cannot provide): func_800402DC (the object spawner) records all five
 * arguments, func_80069D68 (script VM) records the script pointer and id it was
 * handed and returns a configured value, func_80019668 (PRNG) returns a
 * configured value and counts calls, func_8002AE54 (entity handler factory)
 * records the type byte it was given.
 */
#include "fc_testkit.h"

#define RETAIL_BASE 0x80010000u
#define RELOC 0x70000000u
#define RAM_SIZE 0xA0000

/* Retail globals touched by the units (all inside the relocated window). */
#define A_POOL        0x8009F70Cu  /* D_8009F70C deferred-link pool          */
#define A_QLIST       0x8009F708u  /* D_8009F708 deferred queue head         */
#define A_SCREEN      0x8009FA00u  /* D_8009FA00 screen/game state           */
#define A_SLOTDESC    0x800A0C70u  /* D_800A0C70 256-entry slot descriptor   */
#define A_REGISTRY    0x8009F800u  /* D_8009F800 unit-spawner registry       */
#define A_PLAYERS     0x8009FA58u  /* D_8009FA58 two player entity slots     */
#define A_PLAYERCOUNT 0x8009FA0Cu  /* D_8009FA0C player count                */
#define A_SINGLE      0x8009F9ECu  /* D_8009F9EC single-player entity        */
#define A_SESSION     0x8009FA94u  /* D_8009FA94 match/scoring block         */
#define A_RESHEAD     0x8009F5B8u  /* D_8009F5B8 resource registry head      */
#define A_RESTAIL     0x8009F5BCu  /* D_8009F5BC resource registry tail      */

extern u8 gRetailRam[] __asm__("D_RAM");

/* Units under test. */
extern void gQueueOrApplyMapOccupancy(void *) __asm__("func_8003FA34");
extern void gSpawnEntityPrimaryType(void *) __asm__("func_8003BC0C");
extern u8 gGetSpawnDescColumn(void *) __asm__("func_8003B838");
extern s32 gGetEntitySpawnBank(void *) __asm__("func_8003BF50");
extern s32 gQuarterIndex(s32, s32) __asm__("func_8003B65C");
extern void gFlagQuartileChange(void *, s32) __asm__("func_8003BA50");
extern void *gTakeFixedPoolSlot(void *) __asm__("func_800195A4");
extern void gTryCaptureObjective(void *, void *) __asm__("func_8002D1A0");
extern void gRegisterUnitSpawner(void *) __asm__("func_80060E74");
extern u8 *gFindSectionHeader(u8 *, u32, u32, s32) __asm__("func_80019CA0");
extern u8 *gFindSectionPayload(u8 *, u32, u32, s32) __asm__("func_80019C38");
extern void *gFindRegisteredResource(s32, s32) __asm__("func_80019BF0");
extern void gAppendResourceRegistryEntry(void *) __asm__("func_80019A74");

/* ---- relocated retail memory ---- */
static u8 *mem(u32 retail) { return gRetailRam + (retail - RETAIL_BASE); }
static u32 rd32(u32 a) { u8 *p = mem(a); return p[0] | p[1] << 8 | p[2] << 16 | (u32)p[3] << 24; }
static void wr32(u32 a, u32 v) { u8 *p = mem(a); p[0] = v; p[1] = v >> 8; p[2] = v >> 16; p[3] = v >> 24; }

/* ---- stubs ---- */

/* func_800402DC gSpawnTypeAtOwner(object, column, arg2, bank, arg4) - the
 * spawner service the occupancy writer ends up in. Five arguments, so the
 * recorder keeps all of them instead of the three fc_testkit offers. */
typedef struct {
    void *object;
    s32 column;
    s32 arg2;
    s32 bank;
    s32 arg4;
} SpawnCall;

static SpawnCall g_spawn[8];
static int g_spawn_calls;

void gSpawnTypeAtOwner(void *object, s32 column, s32 arg2, s32 bank, s32 arg4)
    __asm__("func_800402DC");
void gSpawnTypeAtOwner(void *object, s32 column, s32 arg2, s32 bank, s32 arg4)
{
    if (g_spawn_calls < 8) {
        g_spawn[g_spawn_calls].object = object;
        g_spawn[g_spawn_calls].column = column;
        g_spawn[g_spawn_calls].arg2 = arg2;
        g_spawn[g_spawn_calls].bank = bank;
        g_spawn[g_spawn_calls].arg4 = arg4;
    }
    g_spawn_calls++;
}

/* func_80069D68 script VM: records the script pointer and the id it was given,
 * returns the value the test configured beforehand. */
static void *g_script;
static s32 g_script_id;
static s32 g_script_calls;
static s32 g_script_ret;

s32 gInterpretEntityScript(void *script, s32 a1, s32 a2) __asm__("func_80069D68");
s32 gInterpretEntityScript(void *script, s32 a1, s32 a2)
{
    (void)a1;
    g_script = script;
    g_script_id = a2;
    g_script_calls++;
    return g_script_ret;
}

/* func_80019668 PRNG: a configured value and a call count. */
static s32 g_rand_value;
static s32 g_rand_calls;

s32 gAdvancePrimaryPrng(void) __asm__("func_80019668");
s32 gAdvancePrimaryPrng(void) { g_rand_calls++; return g_rand_value; }

/* func_8002AE54 entity handler factory: records the type byte. */
static s32 g_handler_type;
static s32 g_handler_calls;
static void *g_handler_ret;

s32 gGetEntityHandlerForType(u8 type) __asm__("func_8002AE54");
s32 gGetEntityHandlerForType(u8 type)
{
    g_handler_type = type;
    g_handler_calls++;
    return (s32)g_handler_ret;
}

/* ---- fixture views of the retail structures ---- */

/* +0x16 kind (func_8003BF50), +0x1C/+0x1E counters, +0x20 slot index,
 * +0x24 flags. */
struct MapObject {
    u8 pad00[0x16];
    u8 kind;
    u8 pad17[0x05];
    s16 unk1C;
    s16 unk1E;
    u8 tableIndex;
    u8 pad21[3];
    u32 flags;
};

/* One D_800A0C70 entry: byte 0 primary column (func_8003B838), byte 1
 * destruction column, halfword +0x18 flags, byte +0x1A random-fallback
 * column (func_8003B4E0). */
struct SlotDesc {
    u8 column;
    u8 destroyColumn;
    u8 pad02[0x16];
    u16 flags;
    u8 fallback;
    u8 pad1B;
    u32 tail;
};

/* Fixed pool descriptor consumed by func_800195A4. */
struct Pool { u8 pad00[0x0C]; s32 count; void *head; };

/* Deferred queue node {next, object}. */
struct QNode { struct QNode *next; void *object; };

/* Entity views used by func_8002D1A0 (offsets are the unit's own). */
struct Coord { u8 pad00[0x34]; s32 x; s32 y; s32 z; s32 radius; };
struct SubStats { u8 pad00[0x12]; u16 unk12; u8 pad14[0x2C]; s32 unk40; u8 pad44[0x7D]; s8 unkC1; };
struct Trk { u8 pad00[0x12]; u16 unk12; u8 pad14[4]; u16 unk18; u8 pad1A[0x20]; u16 unk3A; };
struct Menu {
    u8 pad00[0x64];
    s32 unk64[2];
    u8 pad6C[0x10];
    s32 unk7C[2];
    u8 pad84[0x5C];
    s32 unkE0;
};
struct Ent {
    u8 pad00[0x14];
    u16 unk14;
    u8 unk16;
    u8 pad17;
    u8 unk18;
    u8 pad19[0x0A];
    u8 unk23;
    u32 unk24;
    struct Coord coord;
    u8 pad6C[0x26];
    u8 unk92;
    u8 unk93;
    u8 pad94[8];
    u8 *unk9C;
    u8 pad_a0[6];
    u8 unkA6;
    u8 padA7;
    u16 unkA8;
    u8 padAA[2];
    struct SubStats *unkAC;
    u8 padB0[4];
    struct Ent *unkB4;
    u8 padB8[0x24];
    u16 unkDC;
    u8 unkDE;
    u8 padDF[0x11];
    u8 unkF0[6];
    u8 unkF6;
    u8 unkF7;
    u8 unkF8;
    u8 unkF9;
    u8 unkFA;
    u8 unkFB;
    s16 unkFC;
    u8 unkFE;
    u8 padFF[5];
    struct Ent *unk104;
    u8 pad108[0x2C];
    struct Trk *unk134;
};

/* Unit-spawner record view (func_80060E74): +0x00 blob, +0x04 resolved main
 * record, +0x08 cleared, +0x1C section id, +0x20 blob length, +0x30 payload
 * length. The payload the record points at is *inline* from +0x38 on - the
 * unit takes `arg0->unk38` as an array, i.e. the descriptor's own +0x38. */
struct SpawnArg {
    void *blob;
    void *data;
    s32 zero;
    u8 pad0C[0x10];
    s32 section;
    s32 blobLen;
    u8 pad24[0x0C];
    s32 length;
    u8 pad34[4];
    u8 payload[0x20];
};

/* Resource registry node: links at +0x0C/+0x10, keys at +0x18/+0x1C. */
struct Resource {
    u8 pad00[0x0C];
    struct Resource *next;
    struct Resource *prev;
    u8 pad14[4];
    s32 group;
    s32 name;
};

/* ---- bookkeeping ---- */
static int g_cases;
#define CASE(name) do { g_cases++; fck_write("# case "); fck_write(name); fck_write("\n"); } while (0)

static void fill(void *p, int c, int n) { u8 *b = p; while (n--) *b++ = (u8)c; }

/* ==================================================================== (a) */
/* func_8003FA34 gQueueOrApplyMapOccupancy
 *
 * The unit is linked for real, and so are the units it calls all the way down
 * to func_800402DC, so the tests observe the effect instead of the call graph:
 * a scheduled registration ends up in the spawner, a deferred one in the
 * D_8009F708 list. */

static struct Pool g_pool;
static struct QNode g_node[2];
static struct SlotDesc g_desc;
static struct MapObject g_obj;

static void arm_deferral(void)
{
    g_node[0].next = &g_node[1];
    g_node[1].next = 0;
    g_pool.head = &g_node[0];
    g_pool.count = 2;
    g_desc.column = 0x2A;
    wr32(A_SLOTDESC + 5 * 4, (u32)&g_desc);
    g_obj.tableIndex = 5;
    g_obj.kind = 1;                     /* func_8003BF50 -> bank 1 */
    g_obj.flags = 0;
    wr32(A_POOL, (u32)&g_pool);
    wr32(A_SCREEN, 0);
}

static void reset_occupancy(void)
{
    fill(&g_pool, 0, sizeof(g_pool));
    fill(g_node, 0, sizeof(g_node));
    fill(&g_desc, 0, sizeof(g_desc));
    fill(&g_obj, 0, sizeof(g_obj));
    fill(mem(A_SLOTDESC), 0, 0x400);
    wr32(A_POOL, 0);
    wr32(A_QLIST, 0);
    wr32(A_SCREEN, 0);
    g_spawn_calls = 0;
    fill(g_spawn, 0, sizeof(g_spawn));
}

static void test_occupancy(void)
{
    /* 1: no pool at all -> the registration runs immediately. */
    CASE("no deferred pool: immediate");
    reset_occupancy();
    g_desc.column = 0x2A;
    wr32(A_SLOTDESC + 5 * 4, (u32)&g_desc);
    g_obj.tableIndex = 5;
    g_obj.kind = 1;
    gQueueOrApplyMapOccupancy(&g_obj);
    FCK_EQ(g_spawn_calls, 1, "no pool: occupancy writer called once");
    FCK_EQ(g_spawn[0].column, 0x2A, "no pool: column comes from the slot entry");
    FCK_EQ(g_spawn[0].bank, 1, "no pool: bank comes from func_8003BF50(kind 1)");
    FCK_EQ(g_spawn[0].arg2, 0, "no pool: third argument is 0");
    FCK_EQ(g_spawn[0].arg4, 0, "no pool: fifth argument is 0");
    FCK_CHECK(g_spawn[0].object == (void *)&g_obj, "no pool: the object is forwarded");
    FCK_EQ(rd32(A_QLIST), 0, "no pool: nothing queued");

    /* 2: screen state 4 -> immediate. */
    CASE("pool present, screen state 4: immediate");
    reset_occupancy();
    arm_deferral();
    wr32(A_SCREEN, 4);
    gQueueOrApplyMapOccupancy(&g_obj);
    FCK_EQ(g_spawn_calls, 1, "state 4: occupancy writer called");
    FCK_EQ(g_spawn[0].column, 0x2A, "state 4: column from the slot entry");
    FCK_EQ(rd32(A_QLIST), 0, "state 4: nothing queued");
    FCK_EQ(g_pool.count, 2, "state 4: pool untouched");

    /* 3: screen state 0xD -> immediate. */
    CASE("pool present, screen state 0xD: immediate");
    reset_occupancy();
    arm_deferral();
    wr32(A_SCREEN, 0xD);
    gQueueOrApplyMapOccupancy(&g_obj);
    FCK_EQ(g_spawn_calls, 1, "state 0xD: occupancy writer called");
    FCK_EQ(rd32(A_QLIST), 0, "state 0xD: nothing queued");

    /* 4: slot index 0 -> func_8003B838 returns 0 -> nothing at all. */
    CASE("slot index 0: dropped");
    reset_occupancy();
    arm_deferral();
    g_obj.tableIndex = 0;
    gQueueOrApplyMapOccupancy(&g_obj);
    FCK_EQ(g_spawn_calls, 0, "index 0: no occupancy write");
    FCK_EQ(rd32(A_QLIST), 0, "index 0: nothing queued");
    FCK_EQ(g_pool.count, 2, "index 0: pool untouched");

    /* 5: the entity positions itself (flag 0x40000) -> func_8003B838 returns 0. */
    CASE("self-positioned entity: dropped");
    reset_occupancy();
    arm_deferral();
    g_obj.flags = 0x40000;
    gQueueOrApplyMapOccupancy(&g_obj);
    FCK_EQ(g_spawn_calls, 0, "flag 0x40000: no occupancy write");
    FCK_EQ(rd32(A_QLIST), 0, "flag 0x40000: nothing queued");

    /* 6: slot index with no descriptor -> func_8003B838 returns 0. */
    CASE("missing slot descriptor: dropped");
    reset_occupancy();
    arm_deferral();
    g_obj.tableIndex = 7;
    gQueueOrApplyMapOccupancy(&g_obj);
    FCK_EQ(g_spawn_calls, 0, "no descriptor: no occupancy write");
    FCK_EQ(rd32(A_QLIST), 0, "no descriptor: nothing queued");

    /* 7: descriptor present but its primary column byte is 0. */
    CASE("descriptor with column 0: dropped");
    reset_occupancy();
    arm_deferral();
    g_desc.column = 0;
    gQueueOrApplyMapOccupancy(&g_obj);
    FCK_EQ(g_spawn_calls, 0, "column 0: no occupancy write");
    FCK_EQ(rd32(A_QLIST), 0, "column 0: nothing queued");

    /* 8: pool exhausted -> func_800195A4 returns 0 -> silent drop. */
    CASE("deferred pool empty: dropped");
    reset_occupancy();
    arm_deferral();
    g_pool.head = 0;
    g_pool.count = 0;
    gQueueOrApplyMapOccupancy(&g_obj);
    FCK_EQ(g_spawn_calls, 0, "empty pool: nothing written");
    FCK_EQ(rd32(A_QLIST), 0, "empty pool: nothing queued");

    /* 9: first deferral onto an empty list. */
    CASE("deferred onto empty list");
    reset_occupancy();
    arm_deferral();
    gQueueOrApplyMapOccupancy(&g_obj);
    FCK_EQ(g_spawn_calls, 0, "deferral does not spawn");
    FCK_EQ(rd32(A_QLIST), (u32)&g_node[0], "first node becomes the list head");
    FCK_EQ((u32)g_node[0].next, 0, "first node terminates the list");
    FCK_CHECK(g_node[0].object == (void *)&g_obj, "first node stores the object");
    FCK_EQ(g_pool.count, 1, "pool count decremented");

    /* 10: second deferral is prepended (stack), not appended. */
    CASE("deferred onto a non-empty list");
    {
        static struct MapObject second;
        fill(&second, 0, sizeof(second));
        second.tableIndex = 5;
        second.kind = 1;
        gQueueOrApplyMapOccupancy(&second);
        FCK_EQ(g_spawn_calls, 0, "second deferral does not spawn");
        FCK_EQ(rd32(A_QLIST), (u32)&g_node[1], "second node becomes the head");
        FCK_EQ((u32)g_node[1].next, (u32)&g_node[0], "second node points at the old head");
        FCK_CHECK(g_node[1].object == (void *)&second, "second node stores its object");
        FCK_CHECK(g_node[0].object == (void *)&g_obj, "old node keeps its object");
        FCK_EQ(g_pool.count, 0, "pool count decremented again");
    }

    /* 11: pool exhausted -> the third deferral is lost, the list is intact. */
    CASE("third deferral with the pool empty");
    gQueueOrApplyMapOccupancy(&g_obj);
    FCK_EQ(g_spawn_calls, 0, "exhausted pool: nothing written");
    FCK_EQ(rd32(A_QLIST), (u32)&g_node[1], "exhausted pool: head unchanged");
    FCK_EQ((u32)g_node[1].next, (u32)&g_node[0], "exhausted pool: link unchanged");

    /* 12: any other screen state defers, only 4 and 0xD are immediate. */
    CASE("screen state 5 still defers");
    reset_occupancy();
    arm_deferral();
    wr32(A_SCREEN, 5);
    gQueueOrApplyMapOccupancy(&g_obj);
    FCK_EQ(g_spawn_calls, 0, "state 5: no immediate write");
    FCK_EQ(rd32(A_QLIST), (u32)&g_node[0], "state 5: queued instead");

    /* 13: the immediate branch still respects the column guard. */
    CASE("no pool, slot index 0: nothing at all");
    reset_occupancy();
    g_obj.tableIndex = 0;
    gQueueOrApplyMapOccupancy(&g_obj);
    FCK_EQ(g_spawn_calls, 0, "no pool, index 0: no occupancy write");
}

/* ==================================================================== (c) */
/* func_8003B838 gGetSpawnDescColumn, func_8003B65C gQuarterIndex and
 * func_8003BA50 gFlagQuartileChange against a synthetic slot table. */
static void test_slot_table(void)
{
    struct SlotDesc a, b;
    struct MapObject o;

    fill(&a, 0, sizeof(a));
    fill(&b, 0, sizeof(b));
    fill(&o, 0, sizeof(o));
    fill(mem(A_SLOTDESC), 0, 0x400);
    a.column = 0x2A; a.destroyColumn = 0x31;
    b.column = 0x00; b.destroyColumn = 0x44;
    wr32(A_SLOTDESC + 5 * 4, (u32)&a);
    wr32(A_SLOTDESC + 9 * 4, (u32)&b);

    CASE("gGetSpawnDescColumn guards");
    o.tableIndex = 5;
    FCK_EQ(gGetSpawnDescColumn(&o), 0x2A, "reads entry byte 0");
    o.tableIndex = 9;
    FCK_EQ(gGetSpawnDescColumn(&o), 0x00, "entry byte 0 of the second descriptor");
    o.tableIndex = 0;
    FCK_EQ(gGetSpawnDescColumn(&o), 0, "table index 0 returns 0");
    o.tableIndex = 5; o.flags = 0x40000;
    FCK_EQ(gGetSpawnDescColumn(&o), 0, "flag 0x40000 returns 0");
    o.flags = 0; o.tableIndex = 9;
    FCK_EQ(gGetSpawnDescColumn(&o), 0, "null descriptor returns 0");
    o.tableIndex = 6;
    FCK_EQ(gGetSpawnDescColumn(&o), 0, "unused index returns 0");
    o.tableIndex = 5; o.flags = 0x80000;
    FCK_EQ(gGetSpawnDescColumn(&o), 0x2A, "other flag bits do not mask");

    CASE("gQuarterIndex thresholds");
    FCK_EQ(gQuarterIndex(25, 100), 0, "value at the first threshold is quarter 0");
    FCK_EQ(gQuarterIndex(26, 100), 1, "just above the first threshold");
    FCK_EQ(gQuarterIndex(49, 100), 1, "below half");
    FCK_EQ(gQuarterIndex(50, 100), 2, "half is quarter 2");
    FCK_EQ(gQuarterIndex(74, 100), 2, "below three quarters");
    FCK_EQ(gQuarterIndex(75, 100), 3, "three quarters is quarter 3");
    FCK_EQ(gQuarterIndex(0, 0), 0, "zero range collapses to quarter 0");
    FCK_EQ(gQuarterIndex(-8, 100), 0, "a negative value is quarter 0");

    CASE("gFlagQuartileChange flags a crossing");
    o.tableIndex = 5; o.flags = 0; o.unk1E = 100; o.unk1C = 20;
    gFlagQuartileChange(&o, 60);         /* 0 vs 2 */
    FCK_EQ(o.flags, 0x4000, "crossing sets 0x4000");
    o.flags = 0x0001; o.unk1C = 10;
    gFlagQuartileChange(&o, 10);         /* same quarter */
    FCK_EQ(o.flags, 0x0001, "same quarter leaves the flags alone");
    o.flags = 0x0001; o.unk1C = 10;
    gFlagQuartileChange(&o, 90);         /* 0 vs 3 */
    FCK_EQ(o.flags, 0x4001, "flag is OR-ed onto the existing word");
    o.flags = 0; o.unk1C = 200; o.unk1E = 100;
    gFlagQuartileChange(&o, 10);         /* 3 vs 0 */
    FCK_EQ(o.flags, 0x4000, "a downward crossing flags too");

    CASE("gFlagQuartileChange guards");
    o.flags = 0; o.unk1C = -1; o.unk1E = 100; o.tableIndex = 5;
    gFlagQuartileChange(&o, 90);
    FCK_EQ(o.flags, 0, "negative counter never flags");
    o.flags = 0; o.unk1C = 10; o.tableIndex = 6;   /* slot 6 has no descriptor */
    gFlagQuartileChange(&o, 90);
    FCK_EQ(o.flags, 0, "null descriptor never flags");
    o.tableIndex = 5; o.unk1C = 10; o.unk1E = 0;
    gFlagQuartileChange(&o, 10);
    FCK_EQ(o.flags, 0, "zero range: no crossing possible");
    o.flags = 0; o.unk1E = 100; o.unk1C = 10; o.tableIndex = 0;
    gFlagQuartileChange(&o, 90);
    FCK_EQ(o.flags, 0, "table index 0 never flags");
}

/* ==================================================================== (b) */
/* func_8002D1A0 gTryCaptureObjective. */
static struct Ent g_self, g_other, g_child, g_player, g_otherplayer;
static struct Menu g_menu;
static struct SubStats g_selfsub, g_psub, g_osub;
static struct Trk g_st, g_ptrack;
static u8 g_script_bytes[8];

static void capture_fixture(void)
{
    fill(&g_self, 0, sizeof(g_self));
    fill(&g_other, 0, sizeof(g_other));
    fill(&g_child, 0, sizeof(g_child));
    fill(&g_player, 0, sizeof(g_player));
    fill(&g_otherplayer, 0, sizeof(g_otherplayer));
    fill(&g_menu, 0, sizeof(g_menu));
    fill(&g_selfsub, 0, sizeof(g_selfsub));
    fill(&g_psub, 0, sizeof(g_psub));
    fill(&g_osub, 0, sizeof(g_osub));
    fill(&g_st, 0, sizeof(g_st));
    fill(&g_ptrack, 0, sizeof(g_ptrack));
    fill(g_script_bytes, 0, sizeof(g_script_bytes));
    g_script_bytes[0] = 0x60;   /* non-empty script: the call must happen */

    /* Objective: team A id 0x11, alternate team 0x22. */
    g_self.unkF6 = 0x11;
    g_self.unkF7 = 0x22;
    g_self.unkF8 = 0x5A;        /* value latched for a same-team capture */
    g_self.unkF9 = 0xA5;        /* value latched for an opposing capture  */
    g_self.unkFA = 3;           /* score increment, scaled by the mode      */
    g_self.unkFB = 2;           /* secondary increment                     */
    g_self.unkFC = 0;           /* capture radius in 16-unit steps          */
    g_self.unk24 = 0x100001;    /* script gate, flag bit 0 set so the clear
                                 * is observable                            */
    g_self.unk9C = g_script_bytes;
    g_self.unk104 = &g_child;
    g_self.unk92 = 10;
    g_self.unk93 = 20;
    g_self.coord.x = 0x1000; g_self.coord.y = 0x0800; g_self.coord.z = 0x2000;
    g_self.coord.radius = 0;

    /* Attacker: a vehicle (type 5) of team A, id 0x11, right on top of it. */
    g_other.unk14 = 0x11;
    g_other.unk16 = 5;
    g_other.unk18 = 0x11;
    g_other.coord = g_self.coord;

    g_child.unk18 = 0;

    g_player.unk14 = 0x11;
    g_player.unk18 = 0x11;
    g_player.unkAC = &g_psub;
    g_player.unk134 = &g_ptrack;
    g_psub.unk40 = 0;
    g_psub.unkC1 = 0;
    g_otherplayer.unk14 = 0x22;
    g_otherplayer.unk18 = 0x22;
    g_otherplayer.unkAC = &g_osub;
    g_osub.unkC1 = 1;

    wr32(A_PLAYERS + 0, (u32)&g_player);
    wr32(A_PLAYERS + 4, (u32)&g_otherplayer);
    wr32(A_PLAYERCOUNT, 1);
    wr32(A_SINGLE, (u32)&g_player);
    wr32(A_SESSION, (u32)&g_menu);
    g_script_calls = 0; g_script = 0; g_script_id = -1; g_script_ret = 0;
}

static void test_capture(void)
{
    CASE("capture: already claimed");
    capture_fixture();
    g_self.unkFE = 1;
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 1, "latch unchanged");
    FCK_EQ(g_self.unk18, 0, "team not written");
    FCK_EQ(g_script_calls, 0, "no script run");
    FCK_EQ(g_menu.unk64[1], 0, "no score");

    CASE("capture: wrong team");
    capture_fixture();
    g_other.unk18 = 0x33;
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 0, "no latch");
    FCK_EQ(g_script_calls, 0, "no script");

    CASE("capture: unsupported attacker type");
    capture_fixture();
    g_other.unk16 = 2;
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 0, "no latch");
    FCK_EQ(g_script_calls, 0, "no script");

    CASE("capture: player-owned attacker of the same team");
    capture_fixture();
    g_other.unk16 = 1;                  /* player class */
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 1, "same team latches mode 1");
    FCK_EQ(g_self.unk18, 0x11, "ownership team written");
    FCK_EQ(g_child.unk18, 0x11, "child follows the new team");
    FCK_EQ(g_self.unkA8, 0x22, "alternate team recorded");
    FCK_EQ(g_self.unkDC, 0x22, "alternate team also in +0xDC");
    FCK_EQ(g_self.unk23, 0x5A, "kind byte takes the same-team value");
    FCK_EQ(g_self.unk92, 13, "score incremented by 3");
    FCK_EQ(g_child.unk92, 13, "child score copied");
    FCK_EQ(g_self.unk93, 22, "secondary incremented by 2");
    FCK_EQ(g_child.unk93, 22, "child secondary copied");
    FCK_EQ(g_self.unkA6, 4, "state byte set to 4");
    FCK_EQ(g_self.unkDE, 4, "state byte +0xDE set to 4");
    FCK_EQ(g_self.unk24, 0x100000, "flag bit 0 cleared, the script gate kept");
    FCK_EQ(g_script_calls, 1, "script runs once");
    FCK_EQ(g_script_id, 0x11, "script receives the attacker id, masked");
    FCK_CHECK(g_script == (void *)g_script_bytes, "script pointer is the object's own");

    CASE("capture: player slot that is not a live participant");
    capture_fixture();
    g_other.unk16 = 1;
    g_psub.unk40 = 1;                   /* not an active player */
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 0, "inactive player slot does not capture");

    CASE("capture: type 0x12 follows its carrier");
    capture_fixture();
    g_other.unk16 = 0x12;
    g_other.unkB4 = 0;
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 0, "null carrier does not capture");
    g_other.unkB4 = &g_other;
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 1, "carrier with a live player id captures");

    CASE("capture: opposing team latches mode 2");
    capture_fixture();
    g_other.unk18 = 0x22;
    g_other.unk14 = 0x0042;
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 2, "opposing team latches mode 2");
    FCK_EQ(g_self.unkA8, 0x11, "the opposite team is recorded");
    FCK_EQ(g_self.unk23, 0xA5, "kind byte takes the opposing value");
    FCK_EQ(g_self.unk92, 16, "score incremented by 6 (3 x mode 2)");
    FCK_EQ(g_self.unk93, 24, "secondary incremented by 4 (2 x mode 2)");
    FCK_EQ(g_script_id, 0x42, "script receives the attacker id");

    CASE("capture: range limit");
    capture_fixture();
    g_other.coord.x = 0x1000 + 0x3000;  /* 12288 units away, limit 16384 */
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 1, "inside the default radius captures");
    capture_fixture();
    g_other.coord.x = 0x1000 + 0x7000;  /* 28672 units away */
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 0, "outside the default radius does not capture");

    CASE("capture: radius from the objective and the attacker");
    capture_fixture();
    g_other.coord.x = 0x1000 + 0x7000;
    g_self.unkFC = 4;                   /* +4 << 4 = 64 units */
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 0, "64 extra units are not enough");
    g_other.coord.radius = 0x4000;      /* attacker contributes its own radius */
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 1, "an attacker radius of 0x4000 closes the gap");

    CASE("capture: type 0x25 is a flat 2D test");
    capture_fixture();
    g_other.unk16 = 0x25;
    g_other.coord.y = 0x0800 + 0x6000;  /* far above, same ground position */
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 1, "type 0x25 ignores the height difference");
    capture_fixture();
    g_other.unk16 = 5;
    g_other.coord.y = 0x0800 + 0x6000;
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 0, "the 3D test rejects the same height difference");

    CASE("capture: single-player scoring");
    capture_fixture();
    g_other.unk14 = 0x11;
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_menu.unk64[1], 1, "player 1 outpost counter");
    FCK_EQ(g_menu.unk7C[1], 1, "player 1 total counter");
    FCK_EQ(g_ptrack.unk3A, 1, "per-player claimed counter");
    FCK_EQ(g_ptrack.unk18, 0, "per-player team cleared");
    FCK_EQ(g_menu.unkE0, 6, "last-capture banner id 6");

    CASE("capture: two-player scoring");
    capture_fixture();
    wr32(A_PLAYERCOUNT, 2);
    g_other.unk18 = 0x22;
    g_other.unk14 = 0x0022;
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_menu.unk64[1], 1, "second slot outpost counter");
    FCK_EQ(g_menu.unk7C[1], 1, "second slot total counter");
    FCK_EQ(g_osub.unk12, 1, "per-slot capture counter");
    FCK_EQ(g_menu.unk64[0], 0, "first slot untouched");
    FCK_EQ(g_menu.unkE0, 0, "the 1P banner is not raised in 2P");

    CASE("capture: script gate");
    capture_fixture();
    g_self.unk24 = 0;                   /* gate closed */
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_self.unkFE, 1, "capture still happens");
    FCK_EQ(g_script_calls, 0, "but no script runs");
    capture_fixture();
    g_self.unk9C = 0;
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_script_calls, 0, "no script pointer, no script");
    capture_fixture();
    g_script_bytes[0] = 0;
    gTryCaptureObjective(&g_self, &g_other);
    FCK_EQ(g_script_calls, 0, "empty first byte: no script");
}

/* ==================================================================== (d) */
/* func_80060E74 gRegisterUnitSpawner over a synthetic three-section blob.
 *
 * The blob is a hand-built 'CASt'/'LSRa'/'TCAt' chain in harness memory, i.e.
 * the same shape the unit's func_80019CA0 / func_80019C38 walk. No retail file
 * access is involved: the unit only ever touches the descriptor it is given. */
#define SEC_ID 0x1234
#define BLOB_LEN 0x60
#define BLOB_ROOM 0x100

static u8 g_blob[BLOB_ROOM];
static struct SpawnArg g_arg;
static struct Resource g_res1, g_res2;

/* D_8009F800 holds a *pointer* to {count, entries[]}, so the harness builds
 * that table itself instead of letting the unit write through a null. */
struct Registry { s32 count; void *entries[8]; };
static struct Registry g_registry;

static void put32(u8 *p, u32 v) { p[0] = v; p[1] = v >> 8; p[2] = v >> 16; p[3] = v >> 24; }
static u32 get32(u8 *p) { return p[0] | p[1] << 8 | p[2] << 16 | (u32)p[3] << 24; }
static s32 get16(u8 *p) { return (s16)(p[0] | p[1] << 8); }

/* Layout: 'CASt' header 0x30 | 'LSRa' header + entries | 'TCAt' header + payload. */
static void build_blob(int entries)
{
    u8 *list;
    u8 *tail;
    int i;

    fill(g_blob, 0, sizeof(g_blob));
    put32(g_blob + 0x00, 0x74534143);
    put32(g_blob + 0x04, 0x30);
    put32(g_blob + 0x08, SEC_ID);
    g_blob[0x0C] = 4; g_blob[0x0D] = 0;          /* respawn delay raw = 4 -> 20 */

    list = g_blob + 0x30;
    put32(list + 0x00, 0x6152534C);
    put32(list + 0x04, 0xC + 8 * entries);
    put32(list + 0x08, SEC_ID);
    for (i = 0; i < entries; i++) {
        put32(list + 0x0C + 8 * i, 0x40);        /* group */
        put32(list + 0x10 + 8 * i, 0x99);        /* name  */
    }
    if (entries > 1) put32(list + 0x10 + 8, 0);  /* second entry unresolved */

    tail = list + 0xC + 8 * entries;
    put32(tail + 0x00, 0x74414354);
    put32(tail + 0x04, 0x10);
    put32(tail + 0x08, SEC_ID);
    tail[0x0C] = 0x2C;                           /* entity type byte */
    tail[0x0D] = 1; tail[0x0E] = 2; tail[0x0F] = 3;

    fill(&g_arg, 0, sizeof(g_arg));
    g_arg.blob = g_blob;
    g_arg.section = SEC_ID;
    g_arg.blobLen = BLOB_LEN;
    g_arg.length = 0x18;
    fill(g_arg.payload, 0, sizeof(g_arg.payload));
}

static void test_spawner_record(void)
{
    CASE("resource registry");
    fill(&g_res1, 0, sizeof(g_res1));
    fill(&g_res2, 0, sizeof(g_res2));
    g_res1.group = 0x41; g_res1.name = 0x99;
    g_res2.group = 0x40; g_res2.name = 0x99;
    wr32(A_RESHEAD, 0);
    wr32(A_RESTAIL, 0);
    gAppendResourceRegistryEntry(&g_res1);
    FCK_EQ(rd32(A_RESHEAD), (u32)&g_res1, "first resource becomes the head");
    FCK_EQ(rd32(A_RESTAIL), (u32)&g_res1, "first resource is also the tail");
    gAppendResourceRegistryEntry(&g_res2);
    FCK_EQ(rd32(A_RESTAIL), (u32)&g_res2, "second resource becomes the tail");
    FCK_CHECK(g_res1.next == &g_res2, "second resource linked behind the first");
    FCK_EQ((u32)g_res2.next, 0, "tail terminates the list");
    FCK_CHECK(gFindRegisteredResource(0x40, 0x99) == (void *)&g_res2, "lookup by key pair");
    FCK_EQ((u32)gFindRegisteredResource(0x40, 0x98), 0, "a wrong name misses");
    FCK_CHECK(gFindRegisteredResource(0x41, 0x99) == (void *)&g_res1, "the other key pair");

    CASE("blob walk");
    build_blob(2);
    FCK_CHECK(gFindSectionHeader(g_blob, BLOB_LEN, 0x74534143, SEC_ID) == g_blob,
              "CASt header found at the start");
    FCK_CHECK(gFindSectionHeader(g_blob, BLOB_LEN, 0x6152534C, SEC_ID) == g_blob + 0x30,
              "LSRa header found after the first section");
    FCK_CHECK(gFindSectionPayload(g_blob, BLOB_LEN, 0x74414354, SEC_ID) == g_blob + 0x58,
              "TCAt payload follows its header");
    FCK_EQ((u32)gFindSectionPayload(g_blob, BLOB_LEN, 0x74414354, 0), 0,
           "section id 0 short-circuits to null");

    CASE("gRegisterUnitSpawner");
    build_blob(2);
    fill(&g_registry, 0, sizeof(g_registry));
    wr32(A_REGISTRY, (u32)&g_registry);
    g_rand_value = 27;
    g_rand_calls = 0;
    g_handler_type = -1;
    g_handler_calls = 0;
    g_handler_ret = (void *)0x0BEEF000;
    gRegisterUnitSpawner(&g_arg);

    FCK_EQ(get16(g_blob + 0x0C), 20, "respawn delay scaled by five");
    FCK_EQ(get16(g_blob + 0x14), 27 % 20, "initial delay is rand % delay");
    FCK_EQ(get32(g_blob + 0x18), (u32)g_arg.payload, "the data pointer is the record's own +0x38");
    FCK_EQ(get32(g_blob + 0x1C), (u32)(g_arg.payload + 0x18), "its end is pointer + length");
    FCK_EQ(get32(g_blob + 0x20), 0x0BEEF000, "the handler for the type byte is stored");
    FCK_EQ(get32(g_blob + 0x28), (u32)(g_blob + 0x54), "the trailing table is payload - 4");
    FCK_EQ(get32(g_blob + 0x2C), (u32)&g_arg, "the record points back at its resource");
    FCK_EQ(get32(g_blob + 0x24), (u32)(g_blob + 0x38), "the entry list view starts after the header");
    FCK_EQ(get32(g_blob + 0x38), 2, "two entries counted from the section size");
    FCK_EQ(get32(g_blob + 0x3C), 0x40, "the first entry keeps its group word");
    FCK_EQ(get32(g_blob + 0x40), (u32)&g_res2, "the first entry resolves to the registry node");
    FCK_EQ(get32(g_blob + 0x44), 0x40, "the second entry keeps its group word");
    FCK_EQ(get32(g_blob + 0x48), 0, "an entry without a name stays unresolved");
    FCK_EQ(get16(g_blob + 0x16), 0, "the spawn counter is reset");
    FCK_EQ(g_handler_type, 0x2C, "the type byte is the first TCAt payload byte");
    FCK_EQ(g_handler_calls, 1, "one handler lookup");
    FCK_EQ(g_rand_calls, 1, "one random draw for the initial delay");

    FCK_EQ(g_registry.count, 1, "registry count incremented");
    FCK_CHECK(g_registry.entries[0] == (void *)&g_arg, "the record is published in the registry");
    FCK_CHECK(g_arg.data == g_blob, "the main record is the CASt header");
    FCK_EQ(g_arg.zero, 0, "+0x08 is cleared");

    CASE("gRegisterUnitSpawner: negative delay");
    build_blob(1);
    g_blob[0x0C] = 0xFA; g_blob[0x0D] = 0xFF;   /* -6 -> -30 -> mirrored to 30 */
    fill(&g_registry, 0, sizeof(g_registry));
    g_rand_calls = 0;
    g_rand_value = 5;
    g_handler_calls = 0;
    gRegisterUnitSpawner(&g_arg);
    FCK_EQ(get16(g_blob + 0x0C), 30, "a negative delay is mirrored positive");
    FCK_EQ(get16(g_blob + 0x14), 30, "and becomes the initial delay");
    FCK_EQ(g_rand_calls, 0, "no random draw on that branch");
    FCK_EQ(get32(g_blob + 0x38), 1, "one entry counted");
    FCK_EQ(g_arg.zero, 0, "+0x08 cleared again");

    CASE("gRegisterUnitSpawner: appends at the live count");
    build_blob(1);
    fill(&g_registry, 0, sizeof(g_registry));
    g_registry.count = 5;
    g_registry.entries[3] = (void *)0x1234;
    g_rand_value = 0;
    g_rand_calls = 0;
    g_handler_calls = 0;
    gRegisterUnitSpawner(&g_arg);
    FCK_EQ(g_registry.count, 6, "count incremented from the value in the slot");
    FCK_CHECK(g_registry.entries[5] == (void *)&g_arg, "the record lands at index 5");
    FCK_CHECK(g_registry.entries[3] == (void *)0x1234, "earlier entries are untouched");

    CASE("gRegisterUnitSpawner: pool slot helper");
    {
        struct Pool pool;
        struct QNode n[2];
        fill(&pool, 0, sizeof(pool));
        fill(n, 0, sizeof(n));
        n[0].next = &n[1];
        n[1].next = 0;
        pool.head = &n[0];
        pool.count = 2;
        FCK_CHECK(gTakeFixedPoolSlot(&pool) == (void *)&n[0], "returns the head node");
        FCK_EQ(pool.count, 1, "count decremented");
        FCK_CHECK(pool.head == (void *)&n[1], "head moved to the next node");
        FCK_CHECK(gTakeFixedPoolSlot(&pool) == (void *)&n[1], "returns the second node");
        FCK_EQ(pool.count, 0, "count decremented again");
        FCK_EQ((u32)gTakeFixedPoolSlot(&pool), 0, "an empty pool returns null");
        FCK_EQ(pool.count, 0, "an empty pool leaves the count alone");
    }
}

int main(void)
{
    /* The suite never relies on zeroed BSS: every reset repopulates its slots. */
    reset_occupancy();
    test_occupancy();

    test_slot_table();

    capture_fixture();
    test_capture();

    test_spawner_record();

    {
        int bad = fck_report("map-objects");
        fck_write("map-objects: cases=");
        fck_write_int(g_cases);
        fck_write(" checks=");
        fck_write_int(fck_checks);
        fck_write("\n");
        return bad;
    }
}
