/* Behavioural checks for the pure-C units renamed in naming round 5.
 *
 * The units are the original decomp sources, compiled unchanged. Every entry
 * point is bound to its retail label here, so a broken binding fails at link
 * time instead of silently testing the wrong function. The globals the units
 * read are defined in the harness with their retail labels and the same
 * layout, so each test drives the real code path. */
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

void __main(void) {}

static int failures;

static void say(const char *text)
{
    int length = 0;
    register long v0 __asm__("$2") = 4004;
    register long a0 __asm__("$4") = 1;
    register long a1 __asm__("$5") = (long)text;
    register long a2 __asm__("$6");
    while (text[length]) length++;
    a2 = length;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2)
                     : "memory", "$7", "$8", "$9", "$10", "$11", "$12", "$13",
                       "$14", "$15", "$24", "$25");
}

#define CHECK(condition, message) do { \
    if (!(condition)) { say("FAIL: " message "\n"); failures++; } \
} while (0)

/* --- 8003B7D8: spawn column byte from the region descriptor table -------- */
struct SpawnObject {
    u8 unk00[0x1C];
    s16 unk1C;
    s16 unk1E;
    u8 tableIndex;
    u8 unk21[3];
    u32 flags;
};

u8 gGetDestructionSpawnColumn(struct SpawnObject *object) __asm__("func_8003B7D8");

/* The unit reads a 256-entry table of spawn-region descriptors. */
u8 *gSlotSpawnDescs[256] __asm__("D_800A0C70");

static void test_spawn_column(void)
{
    struct SpawnObject object;
    u8 first[8];
    u8 second[8];
    u8 i;

    for (i = 0; i < 8; i++) {
        first[i] = (u8)(0x40 + i);
        second[i] = (u8)(0x80 + i);
        gSlotSpawnDescs[i] = 0;
    }
    gSlotSpawnDescs[1] = first;
    gSlotSpawnDescs[5] = second;

    object.flags = 0;
    object.tableIndex = 0;
    CHECK(gGetDestructionSpawnColumn(&object) == 0, "a zero table index returns 0");

    object.tableIndex = 1;
    CHECK(gGetDestructionSpawnColumn(&object) == 0x41, "the second byte of the entry is read");

    object.tableIndex = 5;
    CHECK(gGetDestructionSpawnColumn(&object) == 0x81, "the index selects the entry");

    object.flags = 0x40000;
    CHECK(gGetDestructionSpawnColumn(&object) == 0, "flag 0x40000 suppresses the lookup");

    object.flags = 0x3FFFF;
    CHECK(gGetDestructionSpawnColumn(&object) == 0x81, "any other flag bit keeps the lookup");

    object.flags = 0;
    object.tableIndex = 2;
    CHECK(gGetDestructionSpawnColumn(&object) == 0, "a null entry returns 0");
}

/* --- 8003CA80: teardown message and slot retirement ---------------------- */
struct TeardownObject {
    u8 unk00[4];
    s16 unk04;
    u8 unk06;
    s8 unk07;
    u8 unk08[7];
    u8 flags;
    u8 unk10[4];
    s16 unk14;
    u8 unk16[0x0E];
    void (*callback)(s32 message, struct TeardownObject *object, s32 arg2, s32 arg3);
};

void gRetireSpawnSlot(struct TeardownObject *object) __asm__("func_8003CA80");

static int teardownCalls;
static s32 teardownMessage;
static struct TeardownObject *teardownObject;
static s32 teardownArg2;
static s32 teardownArg3;

static void recordingTeardown(s32 message, struct TeardownObject *object, s32 arg2, s32 arg3)
{
    teardownCalls++;
    teardownMessage = message;
    teardownObject = object;
    teardownArg2 = arg2;
    teardownArg3 = arg3;
}

static void test_teardown(void)
{
    struct TeardownObject object;

    object.unk04 = 100;
    object.unk06 = 7;
    object.unk07 = 9;
    object.unk14 = 0x1234;
    object.flags = 0;
    object.callback = recordingTeardown;
    teardownCalls = 0;

    gRetireSpawnSlot(&object);

    CHECK(teardownCalls == 1, "the callback runs exactly once");
    CHECK(teardownMessage == 7, "message 7 asks the owner to tear the slot down");
    CHECK(teardownObject == &object, "the callback receives the slot itself");
    CHECK(teardownArg2 == 0 && teardownArg3 == 0, "both extra arguments are zero");
    CHECK(object.unk04 == -1, "the id field is cleared to -1");
    CHECK(object.unk07 == -1, "the secondary id field is cleared to -1");
    CHECK(object.unk14 == 0, "the counter is zeroed");
    CHECK(object.unk06 == 0, "the mode byte is zeroed");
    CHECK(object.flags == 2, "the retired bit is set");

    object.flags = 0x10;
    gRetireSpawnSlot(&object);
    CHECK(object.flags == 0x12, "the retired bit is added, not assigned");
    CHECK(teardownCalls == 2, "a second call runs the callback again");
}

/* --- 80050DAC: index of the path cell holding a world position ----------- */
struct PathCell {
    s16 unk00;
    s16 x;
    s16 unk04;
    s16 z;
    s32 unk08;
};

struct PathMap {
    s16 unk00;
    s16 count;
    s32 unk04;
    struct PathCell cells[4];
};

s32 gFindRailPathCell(struct PathMap *map, s32 *position) __asm__("func_80050DAC");

static void test_path_cell(void)
{
    struct PathMap map;
    s32 position[3];

    map.count = 3;
    map.cells[0].x = 0x10;
    map.cells[0].z = 0x20;
    map.cells[1].x = 0x30;
    map.cells[1].z = 0x40;
    map.cells[2].x = (s16)0xFFF0;
    map.cells[2].z = 0x50;

    position[0] = 0x10 << 8;
    position[2] = 0x20 << 8;
    CHECK(gFindRailPathCell(&map, position) == 0, "the first cell matches its position");

    position[0] = 0x30 << 8;
    position[2] = 0x40 << 8;
    CHECK(gFindRailPathCell(&map, position) == 1, "the index follows the cell order");

    position[0] = 0;
    position[2] = 0;
    CHECK(gFindRailPathCell(&map, position) == -1, "an empty position matches nothing");

    /* the coordinate is an arithmetic shift by 8: the low byte is dropped */
    position[0] = (0x30 << 8) | 0xFF;
    position[2] = 0x40 << 8;
    CHECK(gFindRailPathCell(&map, position) == 1, "the low byte does not affect the cell");

    /* a negative coordinate round-trips through the s16 cell field */
    position[0] = (s32)0xFFFFF000;
    position[2] = 0x50 << 8;
    CHECK(gFindRailPathCell(&map, position) == 2, "a negative x still matches");

    map.count = 0;
    position[0] = 0x10 << 8;
    position[2] = 0x20 << 8;
    CHECK(gFindRailPathCell(&map, position) == -1, "an empty map returns -1");
}

/* --- 8005BCD4: the active effect node owned by a target ------------------ */
struct EffectNode {
    struct EffectNode *next;
    void *first;
    void *second;
    u8 unk0C[0x1E];
    u8 flags;
};

struct EffectPool {
    void *storage;
    struct EffectNode *active;
    struct EffectNode *free;
};

struct EffectNode *gFindActiveEffectByOwner(struct EffectPool *pool, void *target) __asm__("func_8005BCD4");

static void test_effect_lookup(void)
{
    struct EffectPool pool;
    struct EffectNode a, b, c;
    int targetA, targetB;

    a.next = &b;
    a.second = &targetA;
    a.flags = 0;
    b.next = &c;
    b.second = &targetA;
    b.flags = 0;
    c.next = 0;
    c.second = &targetB;
    c.flags = 0;

    pool.storage = 0;
    pool.active = &a;
    pool.free = 0;

    CHECK(gFindActiveEffectByOwner(&pool, &targetA) == &a, "the first matching node wins");
    CHECK(gFindActiveEffectByOwner(&pool, &targetB) == &c, "a later node is found through the chain");
    CHECK(gFindActiveEffectByOwner(&pool, &pool) == 0, "an unknown target returns null");

    b.flags = 2;
    CHECK(gFindActiveEffectByOwner(&pool, &targetA) == &a, "a skipped node is passed over");

    a.flags = 2;
    CHECK(gFindActiveEffectByOwner(&pool, &targetA) == 0, "with every match skipped the result is null");

    a.flags = 0;
    b.flags = 0;
    pool.active = 0;
    CHECK(gFindActiveEffectByOwner(&pool, &targetA) == 0, "an empty pool returns null");
}

/* --- 80044AE8: pack one animation event into a bit accumulator ----------- */
void gPackAnimEventBits(u32 code, u32 key, s32 *shift, u32 *bits) __asm__("func_80044AE8");

static void test_event_pack(void)
{
    s32 shift;
    u32 bits;

    shift = 0;
    bits = 0;
    gPackAnimEventBits(0x0000 | (5 << 10) | 3, 3, &shift, &bits);
    CHECK(bits == 5, "the six-bit value is stored at the cursor");
    CHECK(shift == 6, "the cursor advances by six bits");

    shift = 0;
    bits = 0;
    gPackAnimEventBits((7 << 10) | 3, 3, &shift, &bits);
    CHECK(bits == 7, "the key itself is masked off the value");

    /* a different key leaves both the accumulator and the cursor alone */
    shift = 0;
    bits = 0xA5;
    gPackAnimEventBits((7 << 10) | 4, 3, &shift, &bits);
    CHECK(bits == 0xA5, "a mismatched key stores nothing");
    CHECK(shift == 0, "a mismatched key does not advance the cursor");

    /* four events fill 24 bits, the fifth no longer fits */
    shift = 0;
    bits = 0;
    gPackAnimEventBits((1 << 10) | 9, 9, &shift, &bits);
    gPackAnimEventBits((2 << 10) | 9, 9, &shift, &bits);
    gPackAnimEventBits((3 << 10) | 9, 9, &shift, &bits);
    gPackAnimEventBits((4 << 10) | 9, 9, &shift, &bits);
    CHECK(shift == 24, "four events advance the cursor to 24 bits");
    CHECK(bits == (1u | (2u << 6) | (3u << 12) | (4u << 18)), "the events pack low to high");

    /* the cap is 26 bits, so a fifth event still fits at cursor 24 */
    gPackAnimEventBits((5 << 10) | 9, 9, &shift, &bits);
    CHECK(shift == 30, "a fifth event still fits below the 26-bit cap");
    CHECK(bits == (1u | (2u << 6) | (3u << 12) | (4u << 18) | (5u << 24)),
          "the fifth event lands in the top bits");

    /* at the cap nothing more is stored */
    shift = 0x1A;
    bits = 0;
    gPackAnimEventBits((2 << 10) | 1, 1, &shift, &bits);
    CHECK(shift == 0x1A && bits == 0, "a cursor at the cap stores nothing");
}

/* --- 8004CC68: packed marker colour levels from the frame counter -------- */
struct MarkerTable {
    u8 unk00[0x44];
    s32 unk44;
    s32 unk48;
    s32 unk4C;
};

void gUpdateMarkerPulseColors(void) __asm__("func_8004CC68");

s32 gFrameTime __asm__("D_8009FAA4");
struct MarkerTable gMarkerColorTable __asm__("D_800A6F60");

static void test_marker_levels(void)
{
    /* raw frame counter 0: level 0 + 8 = 8 */
    gFrameTime = 0;
    gUpdateMarkerPulseColors();
    CHECK(gMarkerColorTable.unk44 == 0x40, "level 8 gives the first field 0x40");
    CHECK(gMarkerColorTable.unk4C == 0x4000, "level 8 gives the third field 0x4000");
    CHECK(gMarkerColorTable.unk48 == 0x402020, "level 8 packs all three components");

    /* bits 3..8 of the counter select the level: 0x20 >> 3 = 4, + 8 = 12 */
    gFrameTime = 0x20;
    gUpdateMarkerPulseColors();
    CHECK(gMarkerColorTable.unk44 == 0x60, "a larger counter raises the level");
    CHECK(gMarkerColorTable.unk4C == 0x6000, "the third field follows the level");

    /* level 0x30 reflects to 0x3F - 0x30 = 0x0F, then + 8 = 23 */
    gFrameTime = 0x180;
    gUpdateMarkerPulseColors();
    CHECK(gMarkerColorTable.unk44 == (23 << 3), "the level reflects above 32");
    CHECK(gMarkerColorTable.unk4C == (23 << 11), "the reflected level reaches every field");

    /* level 0x1F + 8 clamps to 0x1F */
    gFrameTime = 0xF8;
    gUpdateMarkerPulseColors();
    CHECK(gMarkerColorTable.unk44 == 0xF8, "the top level clamps to 31");
    CHECK(gMarkerColorTable.unk4C == 0xF800, "the clamped level reaches the third field");

    /* only bits 3..8 of the counter matter: 0x227 >> 3 == 0x44 -> 4 */
    gFrameTime = 0x227;
    gUpdateMarkerPulseColors();
    CHECK(gMarkerColorTable.unk44 == 0x60, "bits outside 3..8 are ignored");
    gFrameTime = 0x227 | 0x7;
    gUpdateMarkerPulseColors();
    CHECK(gMarkerColorTable.unk44 == 0x60, "the low three bits do not change the level");
}

int main(void)
{
    test_spawn_column();
    test_teardown();
    test_path_cell();
    test_effect_lookup();
    test_event_pack();
    test_marker_levels();

    if (failures == 0) say("naming round 5: all checks passed\n");
    return failures;
}
