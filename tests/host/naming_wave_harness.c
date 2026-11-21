/* Behavioural checks for the drone/targeting units renamed in the naming wave.
 *
 * The units are the original decomp sources, compiled unchanged. Each entry point
 * is declared here under its readable name with the retail label, so a broken
 * label binding fails at link time. */
typedef unsigned char u8;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

/* --- 80035184: nearest flagged type-16 unit ------------------------------- */
typedef struct Node {
    struct Node *next;
    char pad04[4];
    int active;
    char pad0c[10];
    unsigned char type;
    char pad17[0x39];
    int x;
    int y;
    int z;
    char pad5c[0x6c];
    int flags;
} Node;

/* tentative definition carrying the retail label, so the unit's reference resolves */
Node *gUnitListHead __asm__("D_8009FA40");

Node *gFindNearestFlaggedType16Unit(Node *obj, int *distance) __asm__("func_80035184");

/* --- 800336A8: pursuit target -------------------------------------------- */
struct ReferencedObject {
    u8 pad[8];
    s32 value;
};

struct ObjectReference {
    u8 pad[0xC8];
    struct ReferencedObject *object;
    s32 value;
    u8 pad_D0[0x2A];
    u8 dirty;
};

void gSetDronePursuitTarget(struct ObjectReference *state,
                            struct ReferencedObject *object) __asm__("func_800336A8");

/* --- 80033608: attack target with mirrored banks -------------------------- */
struct Unk33608Anim {
    u8 unk00[8];
    s32 value08;
};

struct Unk33608Mirror {
    u8 unk00[0x1B];
    u8 base1B;
    u8 unk1C[0x3A];
    u8 mode56;
    u8 unk57;
    u16 value58;
    u8 unk5A[0x0C];
    u16 value66;
    struct Unk33608Anim *anim68;
    s32 value6C;
    u8 unk70[0x0E];
    u8 mode7E;
    u8 unk7F;
    u16 value80;
    u8 unk82[0x0C];
    u16 value8E;
    struct Unk33608Anim *anim90;
    s32 value94;
};

struct Unk33608Owner {
    u8 unk00[0xA6];
    u8 modeA6;
    u8 unkA7;
    u16 valueA8;
    u8 unkAA[0x0A];
    u16 valueB4;
    u16 valueB6;
    struct Unk33608Anim *animB8;
    s32 valueBC;
    u8 unkC0[0x74];
    struct Unk33608Mirror *mirror;
};

void gSetDroneAttackTarget(struct Unk33608Owner *owner,
                           struct Unk33608Anim *anim) __asm__("func_80033608");

/* --- 80042844: free wrapper ---------------------------------------------- */
void gFreeBlockWrapper(void *block) __asm__("func_80042844");

static int failures;
static int checks;
static void *freed_block;
static int free_calls;

void __main(void) {}

void gFreeBlock(void *block) __asm__("func_800181D4");
void gFreeBlock(void *block)
{
    freed_block = block;
    free_calls++;
}

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
    if (!(condition)) { say("FAIL: " message "\n"); failures++; } else { checks++; } \
} while (0)

static void prepare(Node *node, int active, int type, int x, int z, int flags)
{
    int i;
    char *raw = (char *)node;
    for (i = 0; i < (int)sizeof(*node); i++) raw[i] = 0;
    node->active = active;
    node->type = (unsigned char)type;
    node->x = x;
    node->z = z;
    node->flags = flags;
}

static void test_nearest_flagged_unit(void)
{
    Node self, near, far, wrong_type, unflagged, inactive;
    Node *best;
    int distance = -1;
    int x, z, min, expected;

    prepare(&self, 1, 1, 0, 0, 0);

    /* empty list leaves the out-parameter untouched */
    gUnitListHead = 0;
    best = gFindNearestFlaggedType16Unit(&self, &distance);
    CHECK(best == 0, "empty list returns null");
    CHECK(distance == -1, "empty list leaves distance untouched");

    /* every rejection path in one list */
    prepare(&wrong_type, 1, 15, 5, 5, 0x20000);
    prepare(&unflagged, 1, 16, 5, 5, 0);
    prepare(&inactive, 0, 16, 5, 5, 0x20000);
    wrong_type.next = &unflagged;
    unflagged.next = &inactive;
    inactive.next = 0;
    gUnitListHead = &wrong_type;
    best = gFindNearestFlaggedType16Unit(&self, &distance);
    CHECK(best == 0, "wrong type, unflagged and inactive are skipped");
    CHECK(distance == -1, "skipped list leaves distance untouched");

    /* the nearer candidate wins, and the metric is the octagonal distance */
    prepare(&near, 1, 16, 40, 0, 0x20000);
    prepare(&far, 1, 16, 400, 0, 0x20000);
    near.next = &far;
    far.next = 0;
    gUnitListHead = &near;
    distance = -1;
    best = gFindNearestFlaggedType16Unit(&self, &distance);
    CHECK(best == &near, "nearest flagged unit is chosen");
    CHECK(distance == 40, "axis distance is reported");

    /* distance = |x| + |z| - min(|x|,|z|)/2 */
    gUnitListHead = &far;
    far.x = -300;
    far.z = 200;
    distance = -1;
    best = gFindNearestFlaggedType16Unit(&self, &distance);
    x = 0 - (-300);
    if (x < 0) x = -x;
    z = 0 - 200;
    if (z < 0) z = -z;
    min = x < z ? x : z;
    expected = x + (z - min / 2);
    CHECK(best == &far, "single candidate with negative coordinates is chosen");
    CHECK(distance == expected, "octagonal distance matches |x| + |z| - min/2");
    CHECK(distance == 400, "expected value for (300,200) is 400");
}

static void test_pursuit_target(void)
{
    struct ObjectReference state;
    struct ReferencedObject first, second;
    int i;
    char *raw;

    raw = (char *)&state;
    for (i = 0; i < (int)sizeof(state); i++) raw[i] = 0;
    state.dirty = 7;

    /* a null target changes nothing at all */
    gSetDronePursuitTarget(&state, 0);
    CHECK(state.object == 0, "null target keeps the reference");
    CHECK(state.value == 0, "null target keeps the cached value");
    CHECK(state.dirty == 7, "null target keeps the dirty flag");

    /* a fresh target stores the reference, its value and clears the flag */
    first.value = 0x1234;
    gSetDronePursuitTarget(&state, &first);
    CHECK(state.object == &first, "new target is stored");
    CHECK(state.value == 0x1234, "new target value is cached");
    CHECK(state.dirty == 0, "new target clears the dirty flag");

    /* the same pointer only clears the flag, the cache stays */
    state.value = 0x5555;
    state.dirty = 3;
    gSetDronePursuitTarget(&state, &first);
    CHECK(state.object == &first, "same target keeps the reference");
    CHECK(state.value == 0x5555, "same target keeps the cached value");
    CHECK(state.dirty == 0, "same target still clears the dirty flag");

    /* a different target replaces reference and cache */
    second.value = -7;
    gSetDronePursuitTarget(&state, &second);
    CHECK(state.object == &second, "replacement target is stored");
    CHECK(state.value == -7, "replacement target value is cached");
}

static void test_attack_target(void)
{
    struct Unk33608Owner owner;
    struct Unk33608Mirror mirror;
    struct Unk33608Anim anim;
    int i;
    char *raw;

    raw = (char *)&owner;
    for (i = 0; i < (int)sizeof(owner); i++) raw[i] = 0;
    raw = (char *)&mirror;
    for (i = 0; i < (int)sizeof(mirror); i++) raw[i] = 0;
    owner.mirror = &mirror;
    owner.valueB4 = 0x3C3C;
    owner.modeA6 = 1;
    mirror.base1B = 0x7B;
    anim.value08 = 0x99;

    /* a new animation fills owner and both mirror banks */
    gSetDroneAttackTarget(&owner, &anim);
    CHECK(owner.animB8 == &anim, "owner stores the animation");
    CHECK(owner.valueBC == 0x99, "owner caches the animation value");
    CHECK(mirror.anim68 == &anim, "first mirror bank stores the animation");
    CHECK(mirror.anim90 == &anim, "second mirror bank stores the animation");
    CHECK(mirror.value6C == 0x99, "first mirror bank caches the value");
    CHECK(mirror.value94 == 0x99, "second mirror bank caches the value");

    /* re-applying the same animation is a no-op */
    owner.valueBC = 0xAA;
    mirror.value6C = 0xAA;
    mirror.value94 = 0xAA;
    gSetDroneAttackTarget(&owner, &anim);
    CHECK(owner.valueBC == 0xAA, "same animation leaves the owner cache alone");
    CHECK(mirror.value6C == 0xAA, "same animation leaves the first mirror alone");
    CHECK(mirror.value94 == 0xAA, "same animation leaves the second mirror alone");

    /* a null animation parks the mode at 4 and restores the base value */
    gSetDroneAttackTarget(&owner, 0);
    CHECK(owner.valueB6 == 0x3C3C, "owner copies the base value");
    CHECK(mirror.value66 == 0x3C3C, "first mirror copies the base value");
    CHECK(mirror.value8E == 0x3C3C, "second mirror copies the base value");
    CHECK(owner.modeA6 == 4, "owner mode parks at 4");
    CHECK(mirror.mode56 == 4, "first mirror mode parks at 4");
    CHECK(mirror.mode7E == 4, "second mirror mode parks at 4");
    CHECK(owner.valueA8 == 0x7B, "owner restores the base count");

    /* already parked: only the base value is re-copied */
    owner.valueA8 = 0;
    owner.valueB4 = 0x0101;
    gSetDroneAttackTarget(&owner, 0);
    CHECK(owner.valueB6 == 0x0101, "parked owner re-copies the base value");
    CHECK(owner.valueA8 == 0, "parked owner keeps its base count");
}

static void test_free_wrapper(void)
{
    int marker = 0x42;

    free_calls = 0;
    freed_block = 0;
    gFreeBlockWrapper(&marker);
    CHECK(free_calls == 1, "wrapper calls the deallocator once");
    CHECK(freed_block == &marker, "wrapper forwards the block pointer");

    gFreeBlockWrapper(0);
    CHECK(free_calls == 2, "wrapper forwards a null block");
    CHECK(freed_block == 0, "wrapper passes null unchanged");
}

int main(void)
{
    test_nearest_flagged_unit();
    test_pursuit_target();
    test_attack_target();
    test_free_wrapper();
    if (failures == 0) say("naming wave: all checks passed\n");
    return failures;
}
