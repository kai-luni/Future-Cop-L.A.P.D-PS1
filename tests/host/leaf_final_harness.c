/* Original MIPS C units with synthetic RAM only. The aliased prototypes and
 * private layouts come from the corresponding src/game units. "source"
 * characterizes those units, not behavior measured against a retail image. */
#include "leaf_kit.h"

struct Matrix32 {
    s32 m[3][3];
};
void gTransposeMatrix3x3(struct Matrix32 *source, struct Matrix32 *destination) __asm__("func_80019470");

static void test_matrix_transpose(void)
{
    struct Matrix32 input = { { { 7, -2, 3 }, { 4, 0, -6 }, { -9, 8, 0x12345678 } } };
    struct Matrix32 expected = { { { 7, 4, -9 }, { -2, 0, 8 }, { 3, -6, 0x12345678 } } };
    struct Matrix32 original = input;
    struct {
        s32 before;
        struct Matrix32 matrix;
        s32 after;
    } output;
    struct Matrix32 round_trip;

    /* @leaf 80019470 math signed 3x3 transpose with distinct cells, source preservation and round trip */
    LK_FUNC("func_80019470");
    lk_fill(&output, 0xa5, sizeof(output));
    gTransposeMatrix3x3(&input, &output.matrix);
    FCK_CHECK(memcmp(&output.matrix, &expected, sizeof(expected)) == 0, "columns become rows");
    FCK_CHECK(memcmp(&input, &original, sizeof(input)) == 0, "source unchanged");
    FCK_CHECK(output.before == (s32)0xa5a5a5a5 && output.after == (s32)0xa5a5a5a5,
              "destination guards unchanged");
    gTransposeMatrix3x3(&output.matrix, &round_trip);
    FCK_CHECK(memcmp(&round_trip, &original, sizeof(original)) == 0, "transpose twice restores matrix");
}

struct ReferencedObject {
    u8 pad[8];
    s32 value;
};
struct ObjectReference {
    u8 pad[0xc8];
    struct ReferencedObject *object;
    s32 value;
    u8 pad_D0[0x2a];
    u8 dirty;
};
void gSetDronePursuitTarget(struct ObjectReference *state,
                           struct ReferencedObject *object) __asm__("func_800336A8");

static void test_drone_pursuit_target(void)
{
    struct ReferencedObject first = { { 0 }, -12345 };
    struct ReferencedObject second = { { 0 }, 609 };
    /* Compare object representations copied from the same initialized bytes,
     * including padding, to catch writes outside the three state fields. */
    struct ObjectReference state, expected;

    /* @leaf 800336A8 source null does nothing; same target clears dirty without refreshing the cached value */
    LK_FUNC("func_800336A8");
    lk_fill(&state, 0xa5, sizeof(state));
    state.object = &first;
    state.value = 217;
    state.dirty = 0x81;
    memcpy(&expected, &state, sizeof(state));
    gSetDronePursuitTarget(&state, 0);
    FCK_CHECK(memcmp(&state, &expected, sizeof(state)) == 0, "null target leaves all state alone");

    gSetDronePursuitTarget(&state, &first);
    FCK_EQ(state.value, 217, "same target keeps cached value, not current object value");
    expected.dirty = 0;
    FCK_CHECK(memcmp(&state, &expected, sizeof(state)) == 0, "same target clears only dirty");

    state.dirty = 0x82;
    gSetDronePursuitTarget(&state, &second);
    FCK_CHECK(state.object == &second, "different target installed");
    FCK_EQ(state.value, 609, "different target value cached");
    expected.object = &second;
    expected.value = 609;
    FCK_CHECK(memcmp(&state, &expected, sizeof(state)) == 0, "new target clears dirty without collateral writes");
}

typedef struct Node {
    struct Node *next;
    char pad04[4];
    int active;
    char pad0c[10];
    unsigned char type;
    char pad17[0x39];
    int x, y, z;
    char pad5c[0x6c];
    int flags;
} Node;

Node *gUnitListHead __asm__("D_8009FA40");
Node *gFindNearestFlaggedType16Unit(Node *obj, int *distance) __asm__("func_80035184");

static void test_nearest_flagged_unit(void)
{
    Node origin = { 0 };
    Node nodes[6] = { { 0 } };
    int i, distance;

    origin.x = -50;
    origin.y = 20;
    origin.z = 100;
    for (i = 0; i < 6; i++) {
        nodes[i].next = i < 5 ? &nodes[i + 1] : 0;
        nodes[i].active = 1;
        nodes[i].type = 16;
        nodes[i].flags = 0x20000;
        nodes[i].x = origin.x;
        nodes[i].z = origin.z;
    }
    /* Each zero-distance decoy fails exactly one eligibility condition. */
    nodes[0].active = 0;
    nodes[1].type = 15;
    nodes[2].flags = 0x40000;
    nodes[3].x += 10;
    nodes[3].z += 4;  /* 10 + 4 - 2 = 12 */
    nodes[4].x -= 5;
    nodes[4].z += 7;  /* 5 + 7 - 2 = 10 */
    nodes[4].y = 100000; /* Height does not contribute. */
    nodes[5].x += 7;
    nodes[5].z -= 5;  /* Tie from the other octant. */

    /* @leaf 80035184 source horizontal octagonal metric, eligibility, first-on-tie and untouched miss output */
    LK_FUNC("func_80035184");
    gUnitListHead = &nodes[0];
    distance = -321;
    FCK_CHECK(gFindNearestFlaggedType16Unit(&origin, &distance) == &nodes[4], "nearest eligible wins first tie");
    FCK_EQ(distance, 10, "odd minimum is halved with truncation");
    nodes[4].active = 0;
    FCK_CHECK(gFindNearestFlaggedType16Unit(&origin, &distance) == &nodes[5], "opposite octant has same metric");
    FCK_EQ(distance, 10, "absolute horizontal deltas");
    nodes[2].next = 0;
    distance = -321;
    FCK_CHECK(gFindNearestFlaggedType16Unit(&origin, &distance) == 0, "nonempty list with no eligible node");
    FCK_EQ(distance, -321, "miss preserves output");
    gUnitListHead = 0;
    FCK_CHECK(gFindNearestFlaggedType16Unit(&origin, &distance) == 0, "empty unit list");
    FCK_EQ(distance, -321, "empty list preserves output");
}

struct KindData {
    u8 pad[4];
    u8 kind;
};
/* The source TU has its own, unrelated private struct Node for these lists. */
struct SpawnNode {
    struct SpawnNode *next;
    u8 pad[0x1c];
    struct KindData *data;
};
struct SpawnNode *gListHeads[7] __asm__("D_800A1070");
void gPrependSpawnNodeByKind(struct SpawnNode *node, s32 index) __asm__("func_8003B44C");

static void test_spawn_list_routing(void)
{
    static const u8 kind_for_list[] = { 0x5d, 0x59, 0x5c, 0x5e, 0x5a, 0x57, 0x58 };
    struct SpawnNode tails[7] = { { 0 } };
    struct SpawnNode inserted[7] = { { 0 } };
    struct SpawnNode fallback, empty;
    struct KindData kinds[7] = { { { 0 }, 0 } };
    struct KindData unknown = { { 0 }, 0x56 };
    int i, j;

    /* @leaf 8003B44C source seven kind routes override the supplied index; unknown kind uses it */
    LK_FUNC("func_8003B44C");
    for (i = 0; i < 7; i++) {
        gListHeads[i] = &tails[i];
        kinds[i].kind = kind_for_list[i];
        inserted[i].data = &kinds[i];
        inserted[i].next = &tails[(i + 1) % 7];
    }
    for (i = 0; i < 7; i++) {
        gPrependSpawnNodeByKind(&inserted[i], (i + 1) % 7);
        FCK_CHECK(inserted[i].next == &tails[i], "new node links to the old head of its kind");
        for (j = 0; j < 7; j++) {
            FCK_CHECK(gListHeads[j] == (j <= i ? &inserted[j] : &tails[j]), "only the selected head changes");
        }
    }
    fallback.data = &unknown;
    fallback.next = 0;
    gPrependSpawnNodeByKind(&fallback, 6);
    FCK_CHECK(gListHeads[6] == &fallback && fallback.next == &inserted[6], "unknown kind falls back to supplied index");
    FCK_CHECK(inserted[6].next == &tails[6] && tails[6].next == 0, "older links stay in order");

    gListHeads[0] = 0;
    empty.data = &unknown;
    empty.next = &fallback;
    gPrependSpawnNodeByKind(&empty, 0);
    FCK_CHECK(gListHeads[0] == &empty && empty.next == 0, "prepend to empty list clears stale link");
    for (i = 0; i < 7; i++) gListHeads[i] = 0;
}

struct Unk3CDACRange {
    u32 count;
};
struct Unk3CDACObject {
    u8 unk00[0x0c];
    u32 flags;
    u8 unk10[0x20];
    u8 slots[8];
    struct Unk3CDACRange *range;
};
struct Unk3CDACHolder {
    struct Unk3CDACObject *object;
};
s32 gCheckModelAttachmentSlot(struct Unk3CDACHolder *holder, s8 index) __asm__("func_8003CDAC");

static void test_attachment_slot_presence(void)
{
    struct Unk3CDACObject object = { { 0 }, 0, { 0 }, { 0, 0xff, 0xfe, 3, 4, 5, 6, 0xff }, 0 };
    struct Unk3CDACHolder holder = { 0 };
    struct Unk3CDACRange range = { 2 };

    /* @leaf 8003CDAC source slot sentinel, counted range boundary and compiled negative-index return */
    LK_FUNC("func_8003CDAC");
    FCK_EQ(gCheckModelAttachmentSlot(&holder, 0), 0, "no model");
    holder.object = &object;
    object.flags = 1u << 24;
    FCK_EQ(gCheckModelAttachmentSlot(&holder, 0), 1, "legacy slot zero is valid");
    FCK_EQ(gCheckModelAttachmentSlot(&holder, 1), 0, "legacy slot byte ff is absent");
    FCK_EQ(gCheckModelAttachmentSlot(&holder, 2), 1, "legacy slot byte fe is valid");
    FCK_EQ(gCheckModelAttachmentSlot(&holder, 7), 0, "last legacy slot uses its own byte");
    object.flags = 1u << 25;
    FCK_EQ(gCheckModelAttachmentSlot(&holder, 0), 0, "range-mode model without a range");
    object.range = &range;
    /* The CDK object branches straight to the return on a negative index
     * with v0 still holding the selected mode bit (1), not the initial 0. */
    FCK_EQ(gCheckModelAttachmentSlot(&holder, -1), 1, "negative index returns selected range-mode bit");
    FCK_EQ(gCheckModelAttachmentSlot(&holder, -128), 1, "lowest signed index also returns the mode bit");
    FCK_EQ(gCheckModelAttachmentSlot(&holder, 1), 1, "last counted entry ignores legacy ff sentinel");
    FCK_EQ(gCheckModelAttachmentSlot(&holder, 2), 0, "count is an exclusive end");
    range.count = 0;
    FCK_EQ(gCheckModelAttachmentSlot(&holder, 0), 0, "zero-length range");
}

struct Vec3 { s32 x, y, z; };
struct Range { s32 count; s16 data[1]; };
struct Model {
    u8 pad[12];
    u32 flags;
    u8 pad10[12];
    s32 **frames;
    u8 pad20[16];
    u8 slots[8];
    struct Range *range;
};
struct Instance { struct Model *model; u8 pad[0x44]; s16 frame; };
void gGetAnimatedModelAttachmentPoint(struct Instance *input, s32 index,
                                      struct Vec3 *out) __asm__("func_8003CE30");

static void test_attachment_point(void)
{
    /* Legacy frame entries point to a word record with a four-byte header,
     * then pairs of words holding signed halfwords (x, y, z, unused). Pad the
     * second backing record so even the missing-slot address computation
     * stays within allocated storage before the original code skips the load. */
    s32 frame0[] = { 0, 0, 0, 0x00020001, 3 };
    s32 frame1[513] = { 0, 0x00060005, 7, (s32)0x8000fffd, 0x00007fff };
    s32 *frames[] = { 0, frame0, frame1 };
    union {
        struct Range range;
        struct { s32 count; s16 records[20]; } storage;
    } table;
    struct Model model = { { 0 }, 0, { 0 }, frames, { 0 }, { 0xff, 1, 0 }, 0 };
    struct Instance input = { &model, { 0 }, 1 };
    struct { struct Vec3 point; s32 guard; } output;

    /* @leaf 8003CE30 source both packed attachment layouts, signed halfword conversion and miss zeroing */
    LK_FUNC("func_8003CE30");
    output.guard = 0x5aa5;
    gGetAnimatedModelAttachmentPoint(&input, 1, &output.point);
    FCK_EQ(output.point.x, -48, "legacy frame 1 slot 1 x");
    FCK_EQ(output.point.y, -524288, "legacy minimum signed halfword y");
    FCK_EQ(output.point.z, 524272, "legacy maximum signed halfword z");
    input.frame = 0;
    gGetAnimatedModelAttachmentPoint(&input, 1, &output.point);
    FCK_CHECK(output.point.x == 16 && output.point.y == 32 && output.point.z == 48, "legacy frame 0 has different data");
    input.frame = 1;
    gGetAnimatedModelAttachmentPoint(&input, 0, &output.point);
    FCK_CHECK(output.point.x == 0 && output.point.y == 0 && output.point.z == 0, "legacy missing slot zeroes output");

    lk_fill(&table, 0, sizeof(table));
    table.storage.count = 2;
    table.storage.records[0] = 10;
    table.storage.records[1] = 11;
    table.storage.records[2] = 12;
    table.storage.records[8] = -9;
    table.storage.records[9] = 21;
    table.storage.records[10] = -37;
    model.flags = 1u << 25;
    model.range = &table.range;
    gGetAnimatedModelAttachmentPoint(&input, 0, &output.point);
    FCK_EQ(output.point.x, -144, "range frame stride is count times eight bytes");
    FCK_EQ(output.point.y, 336, "range y");
    FCK_EQ(output.point.z, -592, "range z");
    gGetAnimatedModelAttachmentPoint(&input, 2, &output.point);
    FCK_CHECK(output.point.x == 0 && output.point.y == 0 && output.point.z == 0, "index equal to range count misses");

    model.range = 0;
    output.point.x = output.point.y = output.point.z = 17;
    gGetAnimatedModelAttachmentPoint(&input, 0, &output.point);
    FCK_CHECK(output.point.x == 0 && output.point.y == 0 && output.point.z == 0, "null range zeroes nonzero output");

    /* The index guard precedes access to the model; neither endpoint may
     * produce a point even when a range would otherwise contain index 4. */
    model.range = &table.range;
    table.storage.count = 5;
    input.frame = 0;
    gGetAnimatedModelAttachmentPoint(&input, 3, &output.point);
    FCK_CHECK(output.point.x == 160 && output.point.y == 176 && output.point.z == 192,
              "index three is accepted; counted path reads frame base");
    output.point.x = output.point.y = output.point.z = 19;
    gGetAnimatedModelAttachmentPoint(&input, 4, &output.point);
    FCK_CHECK(output.point.x == 0 && output.point.y == 0 && output.point.z == 0, "four is outside attachment index domain");
    output.point.x = output.point.y = output.point.z = 23;
    gGetAnimatedModelAttachmentPoint(&input, -1, &output.point);
    FCK_CHECK(output.point.x == 0 && output.point.y == 0 && output.point.z == 0, "negative attachment index misses");
    FCK_EQ(output.guard, 0x5aa5, "output ends at z");
}

struct ScriptTimer {
    s32 count, period, remaining, trigger, action;
};
struct ScriptTimer *gScriptTimerTicks __asm__("D_8009F770");
void gSetScriptTimerCount(s32 index, s32 kind, s32 value) __asm__("func_8004953C");

static void test_script_timer_count(void)
{
    struct ScriptTimer timers[] = { { 1, 2, 3, 4, 5 }, { 6, 7, 8, 9, 10 }, { 11, 12, 13, 14, 15 } };
    struct ScriptTimer expected[3];

    /* @leaf 8004953C source only exact opcode 0x7e writes the count of the indexed timer */
    LK_FUNC("func_8004953C");
    gScriptTimerTicks = timers;
    memcpy(expected, timers, sizeof(timers));
    gSetScriptTimerCount(1, 0x7d, 88);
    FCK_CHECK(memcmp(timers, expected, sizeof(timers)) == 0, "preceding opcode is a no-op");
    gSetScriptTimerCount(1, 0x17e, 89);
    FCK_CHECK(memcmp(timers, expected, sizeof(timers)) == 0, "matching low byte is not the matching opcode");

    gSetScriptTimerCount(1, 0x7e, -123);
    expected[1].count = -123;
    FCK_CHECK(memcmp(timers, expected, sizeof(timers)) == 0, "only middle count changes, retaining signed value");
    gSetScriptTimerCount(0, 0x7e, 0);
    expected[0].count = 0;
    FCK_CHECK(memcmp(timers, expected, sizeof(timers)) == 0, "timer zero can be cleared");
    gSetScriptTimerCount(2, 0x7e, 2026);
    expected[2].count = 2026;
    FCK_CHECK(memcmp(timers, expected, sizeof(timers)) == 0, "last fixture entry uses full timer stride");
    gScriptTimerTicks = 0;
}

struct Unk50DACCell {
    s16 unk00, x, unk04, z;
    s32 unk08;
};
struct Unk50DACMap {
    s16 unk00, count;
    s32 unk04;
    struct Unk50DACCell cells[1];
};
s32 gFindRailPathCell(struct Unk50DACMap *map, s32 *position) __asm__("func_80050DAC");

static void test_rail_path_cell(void)
{
    /* The original [1] member is the start of a packed, counted record list. */
    struct {
        struct Unk50DACMap map;
        struct Unk50DACCell tail[3];
    } fixture = {
        { 7, 3, 0x1243, { { 1, -4, 2, 7, 3 } } },
        { { 4, -4, 5, 8, 6 }, { 7, -4, 8, 8, 9 }, { 10, -5, 11, 8, 12 } }
    };
    s32 point[] = { -4 * 256, 0x12345678, 8 * 256 + 255 };

    /* @leaf 80050DAC semantic 256-unit cells, signed boundaries, first matching XZ cell and count limit */
    LK_FUNC("func_80050DAC");
    FCK_EQ(gFindRailPathCell(&fixture.map, point), 1, "skip wrong z and return first duplicate");
    point[2] = 8 * 256 - 1;
    FCK_EQ(gFindRailPathCell(&fixture.map, point), 0, "one unit below z boundary hits previous cell");
    point[2] = 8 * 256;
    point[0]--;
    FCK_EQ(gFindRailPathCell(&fixture.map, point), -1, "matching backing record lies beyond count");
    fixture.map.count = 4;
    FCK_EQ(gFindRailPathCell(&fixture.map, point), 3, "negative coordinate below exact boundary floors to -5");
    fixture.map.count = 0;
    FCK_EQ(gFindRailPathCell(&fixture.map, point), -1, "empty rail list");
}

struct Unk52E28 {
    u8 pad_00[0xb0];
    s16 unkB0;
    u8 pad_B2[6];
    u8 unkB8;
};
void gSteerHeadingToOctant(struct Unk52E28 *self) __asm__("func_80052E28");

static void test_steering_step(void)
{
    static const struct {
        u8 mode;
        s16 start, end;
    } cases[] = {
        { 0x81, 0x03ff, 0x03f7 }, { 1, 0x0400, 0x0408 },
        { 1, 0x0c00, 0x0bf8 }, { 1, 0x0c01, 0x0c09 },
        { 1, 0x0ffd, 0x1000 }, { 1, 0x0000, 0x0000 },
        { 2, 0x07ff, 0x07f7 }, { 2, 0x0800, 0x0808 },
        { 3, 0x0000, -1 }, { 3, 0x01ff, 0x01f7 }, { 3, 0x0200, 0x0208 },
        { 3, 0x0a00, 0x09f8 }, { 3, 0x0a01, 0x0a09 },
        { 4, 0x05ff, 0x05f7 }, { 4, 0x0600, 0x0608 },
        { 4, 0x0e00, 0x0df8 }, { 4, 0x0e01, 0x0e09 },
        { 4, 0x09f8, 0x0a00 }, { 4, 0x0a08, 0x0a00 },
        { 7, 0x0005, 0x0000 }
    };
    struct Unk52E28 actor;
    unsigned int i;

    /* @leaf 80052E28 source direction-mode threshold pairs, capped steps and approaching without overshoot */
    LK_FUNC("func_80052E28");
    lk_fill(&actor, 0xa5, sizeof(actor));
    for (i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        actor.unkB0 = cases[i].start;
        actor.unkB8 = cases[i].mode;
        gSteerHeadingToOctant(&actor);
        FCK_EQ(actor.unkB0, cases[i].end, "heading at mode/threshold fixture");
        FCK_EQ(actor.unkB8, cases[i].mode, "mode byte remains unchanged");
    }
    FCK_CHECK(lk_all(&actor, 0, 0xb0, 0xa5) && lk_all(&actor, 0xb2, 0xb8, 0xa5),
              "steering leaves neighboring state untouched");
}

struct InputAction { u32 buttons; s32 phase; };
void gDecodeButtonEdge(u32 buttons, struct InputAction *action) __asm__("func_8005780C");

static void test_button_edges(void)
{
    struct InputAction action = { 0x80000020u, 2 };

    /* @leaf 8005780C source press, hold, release and idle phases; each bit in the action mask suffices */
    LK_FUNC("func_8005780C");
    gDecodeButtonEdge(0x10, &action);
    FCK_EQ(action.phase, 2, "unrelated input remains idle");
    gDecodeButtonEdge(0x80000010u, &action);
    FCK_EQ(action.phase, 0, "press via high mask bit");
    gDecodeButtonEdge(0x20, &action);
    FCK_EQ(action.phase, 3, "second accepted bit continues the hold");
    gDecodeButtonEdge(0x80000020u, &action);
    FCK_EQ(action.phase, 3, "held phase is stable");
    gDecodeButtonEdge(0x10, &action);
    FCK_EQ(action.phase, 1, "release from held phase");
    gDecodeButtonEdge(0, &action);
    FCK_EQ(action.phase, 2, "released phase returns to idle");
    gDecodeButtonEdge(0x20, &action);
    FCK_EQ(action.phase, 0, "new press from idle");
    gDecodeButtonEdge(0, &action);
    FCK_EQ(action.phase, 2, "one-update press goes straight to idle when released");
    FCK_CHECK(action.buttons == 0x80000020u, "mask remains unchanged");
}

struct Unk5BCD4Node {
    struct Unk5BCD4Node *next;
    void *first, *second;
    u8 unk0C[0x1e];
    u8 flags;
};
struct Unk5BCD4Pool {
    void *storage;
    struct Unk5BCD4Node *active, *free;
};
struct Unk5BCD4Node *gFindActiveEffectByOwner(struct Unk5BCD4Pool *pool,
                                            void *target) __asm__("func_8005BCD4");

static void test_active_effect_lookup(void)
{
    int owner, other, storage;
    struct Unk5BCD4Node free_match = { 0, &other, &owner, { 0 }, 0 };
    struct Unk5BCD4Node last = { 0, &other, &owner, { 0 }, 0 };
    struct Unk5BCD4Node eligible = { &last, &other, &owner, { 0 }, 4 };
    struct Unk5BCD4Node hidden = { &eligible, &other, &owner, { 0 }, 6 };
    struct Unk5BCD4Node wrong = { &hidden, &owner, &other, { 0 }, 0 };
    struct Unk5BCD4Pool pool = { &storage, &wrong, &free_match };

    /* @leaf 8005BCD4 semantic first eligible match on active chain, not inactive nodes, other owner field or free chain */
    LK_FUNC("func_8005BCD4");
    FCK_CHECK(gFindActiveEffectByOwner(&pool, &owner) == &eligible, "skip wrong owner and hidden nodes");
    eligible.flags |= 2;
    FCK_CHECK(gFindActiveEffectByOwner(&pool, &owner) == &last, "later eligible match after flag changes");
    last.flags = 2;
    FCK_CHECK(gFindActiveEffectByOwner(&pool, &owner) == 0, "matching free-list node is not returned");
    FCK_CHECK(gFindActiveEffectByOwner(&pool, &other) == &wrong, "other owner legitimately matches head");
    pool.active = 0;
    FCK_CHECK(gFindActiveEffectByOwner(&pool, &owner) == 0, "empty active list despite populated free list");
}

int gCatPrimPackets(u8 *a0, u8 *a1) __asm__("func_8009272C");

static void test_primitive_packet_concatenation(void)
{
    /* MIPS little endian: length occupies byte 3, link the low 24 bits.
     * Arrays provide aligned headers and guard the payload after each tag. */
    u32 left[] = { 0x07314253, 0xdeadbeef };
    u32 right[] = { 0x0889abcd, 0x10203040 };

    /* @leaf 8009272C source lengths add one tag word, exactly 16 fits, 17 rejects without writes */
    LK_FUNC("func_8009272C");
    FCK_EQ(gCatPrimPackets((u8 *)left, (u8 *)right), 0, "combined length 16 succeeds");
    FCK_CHECK(left[0] == 0x10314253, "left tag length changes, low 24 bits preserved");
    FCK_EQ(right[0], 0, "entire right tag cleared");
    FCK_CHECK(left[1] == 0xdeadbeef && right[1] == 0x10203040, "both payloads unchanged");

    left[0] = 0x08314253;
    right[0] = 0x0889abcd;
    FCK_EQ(gCatPrimPackets((u8 *)left, (u8 *)right), -1, "combined length 17 rejects");
    FCK_CHECK(left[0] == 0x08314253 && right[0] == 0x0889abcd, "rejected tags unchanged");
    FCK_CHECK(left[1] == 0xdeadbeef && right[1] == 0x10203040, "rejected payloads unchanged");

    left[0] = 0x00314253;
    right[0] = 0x0089abcd;
    FCK_EQ(gCatPrimPackets((u8 *)left, (u8 *)right), 0, "zero-length packets combine");
    FCK_CHECK(left[0] == 0x01314253 && right[0] == 0, "even zero lengths account for one tag word");
    left[0] = 0xff314253;
    right[0] = 0x0089abcd;
    FCK_EQ(gCatPrimPackets((u8 *)left, (u8 *)right), -1, "max byte plus tag must not wrap to zero");
    FCK_CHECK(left[0] == 0xff314253 && right[0] == 0x0089abcd, "oversized unsigned length leaves tags intact");
}

int main(void)
{
    test_matrix_transpose();
    test_drone_pursuit_target();
    test_nearest_flagged_unit();
    test_spawn_list_routing();
    test_attachment_slot_presence();
    test_attachment_point();
    test_script_timer_count();
    test_rail_path_cell();
    test_steering_step();
    test_button_edges();
    test_active_effect_lookup();
    test_primitive_packet_concatenation();
    return lk_done("leaf_final");
}
