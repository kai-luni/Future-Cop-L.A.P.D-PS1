/* Hardware-free effects tranche 4 leaf suite: unchanged MIPS C units under
 * qemu-mipsel. Expectations are source-derived, not captured from a run.
 * Link the real model and intrusive-pool helpers; substitute only the game
 * allocator boundaries, whose runtime heap is not installed in this harness.
 * All five entry points return void; their observable contract is state and
 * the sequence of allocator calls, not a value in the MIPS return register.
 */
#include "leaf_kit.h"

struct ModelPair {
    void *first;
    void *second;
};

struct AttachedEffect {
    struct ModelPair *model;
    u32 payload;
};

struct EffectFixture {
    u32 first;
    u32 second;
    struct ModelPair model;
    struct AttachedEffect effect;
};

void gFreeAttachedEffect(struct AttachedEffect *effect) __asm__("func_8005E074");
/* @link 8006CA68 */

static const char free_call[] = "func_800181D4";

void record_free(void *block) __asm__("func_800181D4");
void record_free(void *block)
{
    fck_record(free_call, (s32)block, 0, 0);
}

static void expect_free(int index, const void *block)
{
    FCK_CHECK(index < fck_log_count && fck_log[index].name == free_call,
              "expected free at this call index");
    FCK_EQ(fck_log[index].a0, block, "free argument at this call index");
}

static void init_effect(struct EffectFixture *fix)
{
    fix->first = 0x12345678;
    fix->second = 0x76543210;
    fix->model.first = &fix->first;
    fix->model.second = &fix->second;
    fix->effect.model = &fix->model;
    fix->effect.payload = 0x11223344;
}

/* gFreeAttachedEffect.c:31-35 calls the real model helper on +0 before freeing
 * the effect itself. gFreeModelPairBlock.c releases the two children in field
 * order and then the wrapper. Neither helper writes to these objects; the
 * recording allocator leaves all four blocks accessible for that check. */
static void expect_effect_freed(int index, const struct EffectFixture *fix)
{
    expect_free(index, &fix->first);
    expect_free(index + 1, &fix->second);
    expect_free(index + 2, &fix->model);
    expect_free(index + 3, &fix->effect);
    FCK_EQ(fix->first, 0x12345678, "first model buffer unchanged");
    FCK_EQ(fix->second, 0x76543210, "second model buffer unchanged");
    FCK_EQ(fix->model.first, &fix->first, "first model pointer unchanged");
    FCK_EQ(fix->model.second, &fix->second, "second model pointer unchanged");
    FCK_EQ(fix->effect.model, &fix->model, "effect model pointer unchanged");
    FCK_EQ(fix->effect.payload, 0x11223344, "effect payload unchanged");
}

static void test_8005E074(void)
{
    struct EffectFixture fix;

    /* @leaf 8005E074 source gFreeAttachedEffect.c:31-35 model release precedes effect free */
    LK_FUNC("func_8005E074");
    init_effect(&fix);
    fck_log_reset();

    gFreeAttachedEffect(&fix.effect);

    FCK_EQ(fck_log_count, 4, "free two model buffers, model wrapper and effect");
    expect_effect_freed(0, &fix);
}

/* @link 8003BDEC */
/* @link 8003BE2C */

struct Pool {
    void *storage;
    void *active;
    void *free;
    u32 guard;
};

struct Pool owner_a __asm__("D_800A22A0");
struct Pool owner_b __asm__("D_800A22B0");
struct Pool tracer __asm__("D_800A3538");
struct AttachedEffect *effect_slots[5] __asm__("D_800A22F0");
s16 D_800A22E8[4];
s32 motion_context __asm__("D_8009F374");

void gFreeTracerPoolStorage(void) __asm__("func_8006EDF0");
void gTeardownEffectsAndPools(void) __asm__("func_8005E22C");
void gInitTracerPool(void) __asm__("func_8006EDC4");
void gInitEffectPools(void) __asm__("func_8005E1BC");

static u32 old_storage_a, old_storage_b, old_storage_tracer;
static u32 active_marker, free_marker;
static struct AttachedEffect slot_guard;

static void seed_pool(struct Pool *pool, void *storage)
{
    pool->storage = storage;
    pool->active = &active_marker;
    pool->free = &free_marker;
    pool->guard = 0x13572468;
}

/* gFreeIntrusiveNodePoolStorage.c clears only storage (even if both list
 * heads are non-null); the extra word guards the end of the descriptor. */
static void expect_pool_heads_unchanged(const struct Pool *pool)
{
    FCK_EQ(pool->active, &active_marker, "active list head unchanged");
    FCK_EQ(pool->free, &free_marker, "free list head unchanged");
    FCK_EQ(pool->guard, 0x13572468, "pool descriptor guard unchanged");
}

static void test_8006EDF0(void)
{
    /* @leaf 8006EDF0 source gFreeTracerPoolStorage.c:41-44 forwards the tracer descriptor */
    LK_FUNC("func_8006EDF0");
    seed_pool(&tracer, &old_storage_tracer);
    seed_pool(&owner_a, &old_storage_a);
    seed_pool(&owner_b, &old_storage_b);
    fck_log_reset();

    gFreeTracerPoolStorage();

    /* The real pool helper frees the selected descriptor's storage once. */
    FCK_EQ(fck_log_count, 1, "one tracer backing block released");
    expect_free(0, &old_storage_tracer);
    FCK_EQ(tracer.storage, 0, "tracer backing pointer cleared");
    expect_pool_heads_unchanged(&tracer);

    gFreeTracerPoolStorage();
    FCK_EQ(fck_log_count, 1, "already-null tracer storage does not free again");
    FCK_EQ(tracer.storage, 0, "tracer backing pointer stays null");
    expect_pool_heads_unchanged(&tracer);
    FCK_EQ(owner_a.storage, &old_storage_a, "owner A is not the tracer pool");
    FCK_EQ(owner_b.storage, &old_storage_b, "owner B is not the tracer pool");
    expect_pool_heads_unchanged(&owner_a);
    expect_pool_heads_unchanged(&owner_b);
}

static void test_8005E22C(void)
{
    static struct EffectFixture first, last;

    /* @leaf 8005E22C source gTeardownEffectsAndPools.c:40-51 releases A then B then non-null slots in ascending order */
    LK_FUNC("func_8005E22C");
    seed_pool(&owner_a, &old_storage_a);
    seed_pool(&owner_b, &old_storage_b);
    seed_pool(&tracer, &old_storage_tracer);
    init_effect(&first);
    init_effect(&last);
    effect_slots[0] = &first.effect;
    effect_slots[1] = 0;
    effect_slots[2] = 0;
    effect_slots[3] = &last.effect;
    slot_guard = last.effect;
    effect_slots[4] = &slot_guard;
    fck_log_reset();

    gTeardownEffectsAndPools();

    /* Trace the direct helper calls through the real pool and model bodies.
     * In particular, slot 3 must be reached despite the holes, and slot 4
     * is a valid block but outside the scan. No slot write is in the body. */
    FCK_EQ(fck_log_count, 10, "two pool blocks and two complete effects released");
    expect_free(0, &old_storage_a);
    expect_free(1, &old_storage_b);
    expect_effect_freed(2, &first);
    expect_effect_freed(6, &last);
    FCK_EQ(owner_a.storage, 0, "owner A storage cleared");
    FCK_EQ(owner_b.storage, 0, "owner B storage cleared");
    FCK_EQ(tracer.storage, &old_storage_tracer, "tracer storage untouched");
    expect_pool_heads_unchanged(&owner_a);
    expect_pool_heads_unchanged(&owner_b);
    expect_pool_heads_unchanged(&tracer);
    FCK_EQ(effect_slots[0], &first.effect, "slot zero not cleared");
    FCK_EQ(effect_slots[1], 0, "first hole unchanged");
    FCK_EQ(effect_slots[2], 0, "second hole unchanged");
    FCK_EQ(effect_slots[3], &last.effect, "slot three not cleared");
    FCK_EQ(effect_slots[4], &slot_guard, "slot past end unchanged");

    /* Complement the first mask so all four positions take both sides of
     * the null check. With storage already null, only the effects free. */
    effect_slots[0] = 0;
    effect_slots[1] = &first.effect;
    effect_slots[2] = &last.effect;
    effect_slots[3] = 0;
    fck_log_reset();

    gTeardownEffectsAndPools();

    FCK_EQ(fck_log_count, 8, "already-empty pools do not free again");
    expect_effect_freed(0, &first);
    expect_effect_freed(4, &last);
    FCK_EQ(effect_slots[0], 0, "leading hole unchanged");
    FCK_EQ(effect_slots[1], &first.effect, "slot one not cleared");
    FCK_EQ(effect_slots[2], &last.effect, "slot two not cleared");
    FCK_EQ(effect_slots[3], 0, "trailing hole unchanged");
    FCK_EQ(effect_slots[4], &slot_guard, "slot past end unchanged");
}

/* Fake heap only: stable, aligned backing blocks and an optional allocation
 * failure. Requests and the bookkeeping visible at call time are recorded;
 * all pool writes are performed by the original func_8003BE2C. */
enum { heap_words = 288 };
static u32 heap_a[heap_words], heap_b[heap_words];
static void *allocation_result[2];
static int allocation_count;
static struct {
    s16 marker;
    s32 motion;
    struct AttachedEffect *slots[4];
} allocation_state[2];
static const char alloc_call[] = "func_80017EBC";

void *record_alloc(s32 size, s32 tag) __asm__("func_80017EBC");
void *record_alloc(s32 size, s32 tag)
{
    int i, index = allocation_count++;

    fck_record(alloc_call, size, tag, 0);
    if (index >= 2 || size > (s32)sizeof(heap_a))
        return 0;
    allocation_state[index].marker = D_800A22E8[0];
    allocation_state[index].motion = motion_context;
    for (i = 0; i < 4; i++)
        allocation_state[index].slots[i] = effect_slots[i];
    return allocation_result[index];
}

static void reset_alloc(void *first, void *second)
{
    fck_log_reset();
    lk_fill(heap_a, 0xa5, sizeof(heap_a));
    lk_fill(heap_b, 0xa5, sizeof(heap_b));
    allocation_count = 0;
    allocation_result[0] = first;
    allocation_result[1] = second;
}

static void expect_alloc(int index, s32 bytes)
{
    FCK_CHECK(index < fck_log_count && fck_log[index].name == alloc_call,
              "expected allocation at this call index");
    FCK_EQ(fck_log[index].a0, bytes, "allocation size");
    FCK_EQ(fck_log[index].a1, 1, "intrusive pool allocation tag");
}

/* gInitIntrusiveNodePool.c:40-51 writes only the first word of each
 * fixed-stride node, with ten links including the null terminator. The
 * ten pointer positions and exact request size together expose both size
 * and count arguments without mocking this helper. */
static void expect_initialized_pool(const struct Pool *pool, u32 *heap, int stride)
{
    int i, word_stride = stride / sizeof(*heap);

    FCK_EQ(pool->storage, heap, "pool owns selected backing block");
    FCK_EQ(pool->active, 0, "initialized pool has no active nodes");
    FCK_EQ(pool->free, heap, "free list starts at first node");
    FCK_EQ(pool->guard, 0x13572468, "pool descriptor guard unchanged");
    for (i = 0; i < 9; i++)
        FCK_EQ(heap[i * word_stride], &heap[(i + 1) * word_stride],
               "node link advances by one stride");
    FCK_EQ(heap[9 * word_stride], 0, "tenth node terminates free list");
    for (i = 0; i < 10; i++)
        FCK_CHECK(lk_all(heap, i * stride + sizeof(*heap), (i + 1) * stride, 0xa5),
                  "node payload unchanged");
    FCK_CHECK(lk_all(heap, 10 * stride, sizeof(heap_a), 0xa5),
              "bytes beyond last node unchanged");
}

static void test_8006EDC4(void)
{
    /* @leaf 8006EDC4 source gInitTracerPool.c:36-39 forwards tracer descriptor, 0x48-byte stride and ten slots */
    LK_FUNC("func_8006EDC4");
    seed_pool(&tracer, &old_storage_tracer);
    seed_pool(&owner_a, &old_storage_a);
    seed_pool(&owner_b, &old_storage_b);
    reset_alloc(heap_a, 0);

    gInitTracerPool();

    FCK_EQ(fck_log_count, 1, "one tracer allocation");
    expect_alloc(0, 720);
    expect_initialized_pool(&tracer, heap_a, 0x48);
    FCK_EQ(owner_a.storage, &old_storage_a, "owner A storage untouched");
    FCK_EQ(owner_b.storage, &old_storage_b, "owner B storage untouched");
    expect_pool_heads_unchanged(&owner_a);
    expect_pool_heads_unchanged(&owner_b);

    /* The real helper assigns the failed storage result but exits before
     * changing either head. The void wrapper neither retries nor repairs it. */
    seed_pool(&tracer, &old_storage_tracer);
    reset_alloc(0, 0);
    gInitTracerPool();
    FCK_EQ(fck_log_count, 1, "failed allocation is not retried");
    expect_alloc(0, 720);
    FCK_EQ(tracer.storage, 0, "failed allocation stores null");
    expect_pool_heads_unchanged(&tracer);
}

static void test_8005E1BC(void)
{
    static struct AttachedEffect prior_slots[4];
    int i, call;

    /* @leaf 8005E1BC source gInitEffectPools.c:39-50 marks slot free, initializes A then B and resets the four slots and context */
    LK_FUNC("func_8005E1BC");
    seed_pool(&owner_a, &old_storage_a);
    seed_pool(&owner_b, &old_storage_b);
    seed_pool(&tracer, &old_storage_tracer);
    D_800A22E8[0] = 23;
    D_800A22E8[1] = 0x1234;
    D_800A22E8[2] = 0x2345;
    D_800A22E8[3] = 0x3456;
    for (i = 0; i < 4; i++)
        effect_slots[i] = &prior_slots[i];
    effect_slots[4] = &slot_guard;
    motion_context = 19;
    reset_alloc(heap_a, heap_b);

    gInitEffectPools();

    /* The marker is set before either allocation; the slots and context
     * still contain their entry values when both pool calls get there. */
    FCK_EQ(fck_log_count, 2, "one allocation per owner pool");
    expect_alloc(0, 560);
    expect_alloc(1, 960);
    expect_initialized_pool(&owner_a, heap_a, 0x38);
    expect_initialized_pool(&owner_b, heap_b, 0x60);
    for (call = 0; call < 2; call++) {
        FCK_EQ(allocation_state[call].marker, -1, "free marker already set at allocation");
        FCK_EQ(allocation_state[call].motion, 19, "context still set at allocation");
        for (i = 0; i < 4; i++)
            FCK_EQ(allocation_state[call].slots[i], &prior_slots[i],
                   "effect slot still set at allocation");
    }
    FCK_EQ(D_800A22E8[0], -1, "first marker reset");
    FCK_EQ(D_800A22E8[1], 0x1234, "second marker unchanged");
    FCK_EQ(D_800A22E8[2], 0x2345, "third marker unchanged");
    FCK_EQ(D_800A22E8[3], 0x3456, "fourth marker unchanged");
    for (i = 0; i < 4; i++)
        FCK_EQ(effect_slots[i], 0, "effect slot cleared");
    FCK_EQ(effect_slots[4], &slot_guard, "slot past end untouched");
    FCK_EQ(motion_context, 0, "motion context reset");
    FCK_EQ(tracer.storage, &old_storage_tracer, "tracer storage untouched");
    expect_pool_heads_unchanged(&tracer);
}

int main(void)
{
    test_8005E074();
    test_8006EDF0();
    test_8005E22C();
    test_8006EDC4();
    test_8005E1BC();
    return lk_done("leaf_effects_r4");
}
