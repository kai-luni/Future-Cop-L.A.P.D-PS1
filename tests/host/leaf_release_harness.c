/* Source-derived contracts for heap and gameplay teardown. Dependencies with
 * existing original coverage stay real, but are not counted a second time. */
/* @link 8001814C */
/* @link 800195D0 */
/* @link 8001E318 */
/* @link 8001E44C */
/* @link 8003CF20 */
/* @link 8003CA80 */
#include "leaf_kit.h"

#define GUARD 0xA5
#define GUARD_WORD 0xA5A5A5A5
#define HEAP_USED 0x1000
#define HEAP_PEAK 0x1400
#define TABLE_WORDS 12

struct Pool {
    s32 stride;
    u8 *end;
    s32 capacity;
    s32 available;
    s32 *head;
    s32 slots[2][2];
};

struct PoolFixture {
    s32 before;
    struct Pool pool;
    s32 after;
};

struct Resource {
    u8 prefix[8];
    void (*release)(struct Resource *);
    struct Resource *next;
    struct Resource *prev;
};

struct ResourceBlock {
    s32 before, size;
    struct Resource resource;
    s32 after;
};

struct Anchor {
    struct Anchor *next;
    u8 state[0x30];
    void *buffer;
    u8 state38, flags;
};

struct AnchorBlock {
    s32 before, size;
    struct Anchor node;
    s32 after;
};

struct RawBlock {
    s32 before, size;
    s32 payload[12];
    s32 after;
};

struct Pool *pool1 __asm__("D_8009F9D0");
struct Pool *pool2 __asm__("D_8009FAAC");
struct Pool *pool3 __asm__("D_8009FA4C");
s32 *ranges __asm__("D_8009F590");
s32 range_count __asm__("D_8009F594");
s32 cursor __asm__("D_8009F598");
s32 heap_used __asm__("D_80010028");
s32 heap_peak __asm__("D_8001002C");
struct Resource *resource_head __asm__("D_8009F5B8");
struct Resource *resource_tail __asm__("D_8009F5BC");
struct Anchor *anchor_head __asm__("D_8009F320");

void free_block(void *payload) __asm__("func_800181D4");
void free_raw(s32 payload) __asm__("func_8001E2C0");
/* Although the translation has no formal arguments, the retail wrapper
 * forwards the incoming $a0. Exercise that ABI with a pointer in $a0. */
void free_pool(void *payload) __asm__("func_80019584");
void release_resource(struct Resource *resource) __asm__("func_80019AF8");
void release_all_resources(void) __asm__("func_80019BA4");
void free_anchor_list(void) __asm__("func_8003D0AC");

static const char raw_free_event[] = "func_8008CB40";
static const char release_event[] = "resource release callback";

/* Only the final raw free is a stand-in: 8008CB40 stores to absolute PS1 RAM
 * at 0x8009C7D0. Record the header and the state at handoff, not a fake heap. */
void record_raw_free(s32 *header) __asm__("func_8008CB40");
void record_raw_free(s32 *header)
{
    fck_record(raw_free_event, (s32)header, heap_used, (s32)resource_head);
}

static struct PoolFixture pools[3];
static struct {
    s32 before;
    s32 entries[TABLE_WORDS];
    s32 after;
} table;
/* Leading/trailing words allow canaries and an in-bounds raw endpoint probe. */
static s32 arena[32];
#define ARENA ((u8 *)(arena + 1))
static const s32 initial[] = {0, 16, 32, 48, 64, 80, 96, 112};

static void reset_heap(s32 start_cursor)
{
    int i;
    lk_fill(pools, GUARD, sizeof pools);
    for (i = 0; i < 3; i++) {
        struct Pool *pool = &pools[i].pool;
        pool->stride = 8;
        pool->end = (u8 *)&pools[i].after;
        pool->capacity = 2;
        pool->available = 1;
        pool->head = pool->slots[0];
        pool->slots[0][0] = 0;
    }
    pool1 = &pools[0].pool;
    pool2 = &pools[1].pool;
    pool3 = &pools[2].pool;
    lk_fill(arena, GUARD, sizeof arena);
    lk_fill(&table, GUARD, sizeof table);
    ranges = table.entries;
    for (i = 0; i < 8; i++) ranges[i] = (s32)(ARENA + initial[i]);
    range_count = 7;
    cursor = start_cursor;
    heap_used = HEAP_USED;
    heap_peak = HEAP_PEAK;
    resource_head = 0;
    resource_tail = 0;
    fck_log_reset();
}

static void check_ranges(const s32 *offsets, int words, int next_cursor)
{
    int i;
    FCK_EQ(ranges, table.entries, "range table pointer stays stable");
    FCK_EQ(range_count, words - 1, "last boundary index");
    FCK_EQ(cursor, next_cursor, "range search cursor");
    for (i = 0; i < words; i++) {
        FCK_EQ(table.entries[i] - (s32)ARENA, offsets[i], "range boundary including arena endpoints");
    }
    /* Compaction need not clear now-inactive entries that held old bounds. */
    for (i = words < 8 ? 8 : words; i < TABLE_WORDS; i++) {
        FCK_EQ(table.entries[i], GUARD_WORD, "unused table word");
    }
    FCK_EQ(table.before, GUARD_WORD, "range table leading canary");
    FCK_EQ(table.after, GUARD_WORD, "range table trailing canary");
}

static void free_range(int start, int bytes, int start_cursor,
                       const s32 *offsets, int words, int next_cursor)
{
    s32 *header;
    reset_heap(start_cursor);
    header = (s32 *)(ARENA + start);
    *header = bytes;
    free_block(header + 1);
    check_ranges(offsets, words, next_cursor);
    FCK_EQ(heap_used, HEAP_USED, "range free does not touch raw accounting");
    FCK_EQ(fck_log_count, 0, "range free never calls raw allocator");
    FCK_EQ(*header, bytes, "size header stays intact");
    FCK_CHECK(lk_all(arena, 0, start + 4, GUARD), "arena before header stays intact");
    FCK_CHECK(lk_all(arena, start + 8, sizeof arena, GUARD), "arena after header stays intact");
}

static void test_free_block(void)
{
    static const s32 leading_merge[] = {0, 0, 32, 48, 64, 80, 96, 112};
    static const s32 leading_insert[] = {0, 0, 8, 16, 32, 48, 64, 80, 96, 112};
    static const s32 compact[] = {0, 16, 64, 80, 96, 112};
    static const s32 merge_left[] = {0, 16, 40, 48, 64, 80, 96, 112};
    static const s32 merge_right[] = {0, 16, 32, 40, 64, 80, 96, 112};
    static const s32 insert[] = {0, 16, 32, 36, 44, 48, 64, 80, 96, 112};
    static const s32 last_exact[] = {0, 16, 32, 48, 64, 80, 112, 112};
    static const s32 last_insert[] = {0, 16, 32, 48, 64, 80, 96, 100, 112, 112};
    int i;

    /* @leaf 800181D4 source fixed-pool dispatch, managed boundary merges/insertions with sentinel and cursor updates, and raw fallback at exclusive endpoints */
    LK_FUNC("func_800181D4");

    reset_heap(3);
    for (i = 0; i < 3; i++) {
        struct Pool *pool = &pools[i].pool;
        FCK_EQ(pool->available, 1, "other pool not modified by previous dispatch");
        free_block(pool->slots[1]);
        FCK_EQ(pool->available, 2, "free increments the matching pool count");
        FCK_EQ(pool->head, pool->slots[1], "returned slot becomes head");
        FCK_EQ(pool->slots[1][0], pool->slots[0], "returned slot links to the previous head");
        FCK_EQ(pool->slots[0][0], 0, "previous head link is intact");
        FCK_EQ(pool->slots[1][1], GUARD_WORD, "returned slot payload is intact");
        FCK_EQ(pool->stride, 8, "pool stride is not modified");
        FCK_EQ(pool->capacity, 2, "pool capacity is not modified");
        FCK_EQ(pool->end, &pools[i].after, "pool end is not modified");
        FCK_EQ(pools[i].before, GUARD_WORD, "pool leading canary");
        FCK_EQ(pools[i].after, GUARD_WORD, "pool trailing canary");
    }
    FCK_EQ(fck_log_count, 0, "no fixed-pool free is forwarded to the raw allocator");
    check_ranges(initial, 8, 3);

    /* Entry 0 and entry count bound the managed arena. Odd/even interior
     * pairs describe free space, not a linked list of heap headers. */
    free_range(0, 16, 3, leading_merge, 8, 3);
    free_range(0, 8, 3, leading_insert, 10, 5);
    free_range(32, 16, 3, compact, 6, 1);
    free_range(32, 16, 1, compact, 6, 1);
    free_range(32, 8, 3, merge_left, 8, 3);
    free_range(40, 8, 3, merge_right, 8, 3);
    free_range(36, 8, 3, insert, 10, 5);
    free_range(36, 8, 1, insert, 10, 1);
    free_range(96, 16, 5, last_exact, 8, 5);
    free_range(100, 12, 5, last_insert, 10, 5);

    reset_heap(3);
    /* Each exclusive limit has an in-bounds preceding word for raw free. */
    pool3->slots[1][1] = 0x20;
    arena[28] = 0x10; /* ARENA + 108 */
    free_block(pool3->end);
    free_block(ARENA + 112);
    FCK_EQ(fck_log_count, 2, "exclusive endpoints go to the raw free");
    FCK_EQ(fck_call_n(raw_free_event, 0).a0, &pool3->slots[1][1], "pool endpoint forwarded");
    FCK_EQ(fck_call_n(raw_free_event, 1).a0, &arena[28], "arena endpoint forwarded");
    FCK_EQ(pool3->available, 1, "pool endpoint does not become a free node");
    FCK_EQ(pools[2].after, GUARD_WORD, "endpoint canary not overwritten");
    FCK_EQ(heap_used, 0xFC8, "both raw headers accounted");
    check_ranges(initial, 8, 3);
}

static void test_free_raw(void)
{
    struct RawBlock block;
    /* @leaf 8001E2C0 source null is ignored; each raw size word plus its four-byte header is subtracted before handing off the header, without changing the peak */
    LK_FUNC("func_8001E2C0");
    reset_heap(1);
    lk_fill(&block, GUARD, sizeof block);
    block.size = 0x24;
    free_raw(0);
    FCK_EQ(heap_used, HEAP_USED, "null keeps current usage");
    FCK_EQ(fck_log_count, 0, "null does not reach the raw allocator");

    free_raw((s32)block.payload);
    FCK_EQ(fck_log_count, 1, "non-null handed off exactly once");
    FCK_EQ(fck_call_n(raw_free_event, 0).a0, &block.size, "raw free receives header, not payload");
    FCK_EQ(fck_call_n(raw_free_event, 0).a1, 0xFD8, "accounting precedes raw free");
    FCK_EQ(heap_used, 0xFD8, "current usage drops by 0x28");
    FCK_EQ(heap_peak, HEAP_PEAK, "historical peak stays intact");
    FCK_EQ(block.size, 0x24, "header intact up to handoff");
    FCK_EQ(block.before, GUARD_WORD, "raw block leading canary");
    FCK_CHECK(lk_all(block.payload, 0, sizeof block.payload, GUARD), "raw payload intact up to handoff");
    FCK_EQ(block.after, GUARD_WORD, "raw block trailing canary");
}

static void test_free_pool(void)
{
    struct RawBlock block;
    /* @leaf 80019584 source ordinary pool destructor forwards the incoming pointer in $a0 through the unchanged heap-free path */
    LK_FUNC("func_80019584");
    reset_heap(1);
    lk_fill(&block, GUARD, sizeof block);
    block.size = 0x30;
    free_pool(block.payload);
    FCK_EQ(fck_log_count, 1, "pool destructor reaches raw free once");
    FCK_EQ(fck_call_n(raw_free_event, 0).a0, &block.size, "incoming $a0 is forwarded");
    FCK_EQ(heap_used, 0xFCC, "pool block's header determines accounting");
    FCK_EQ(block.after, GUARD_WORD, "pool block trailing canary");
}

static void record_release(struct Resource *resource)
{
    fck_record(release_event, (s32)resource, (s32)resource->prev, (s32)resource->next);
}

static void init_resources(struct ResourceBlock *blocks)
{
    int i;
    for (i = 0; i < 3; i++) {
        lk_fill(&blocks[i], GUARD, sizeof blocks[i]);
        blocks[i].size = sizeof blocks[i].resource;
        blocks[i].resource.release = record_release;
        blocks[i].resource.prev = i == 0 ? 0 : &blocks[i - 1].resource;
        blocks[i].resource.next = i == 2 ? 0 : &blocks[i + 1].resource;
    }
    blocks[1].resource.release = 0;
    resource_head = &blocks[0].resource;
    resource_tail = &blocks[2].resource;
}

static void check_resource_guards(const struct ResourceBlock *blocks)
{
    int i;
    for (i = 0; i < 3; i++) {
        FCK_EQ(blocks[i].before, GUARD_WORD, "resource leading canary");
        FCK_EQ(blocks[i].after, GUARD_WORD, "resource trailing canary");
        FCK_CHECK(lk_all(blocks[i].resource.prefix, 0, 8, GUARD), "resource prefix intact");
    }
}

static void test_release_resource(void)
{
    struct ResourceBlock blocks[3];
    struct Resource *first = &blocks[0].resource;
    struct Resource *middle = &blocks[1].resource;
    struct Resource *last = &blocks[2].resource;
    static const char *order[] = {raw_free_event, release_event, raw_free_event,
                                  release_event, raw_free_event};

    /* @leaf 80019AF8 source optional destructor sees live links; unlink preserves neighbors and head/tail for middle, tail and singleton before freeing the original node */
    LK_FUNC("func_80019AF8");
    reset_heap(1);
    init_resources(blocks);
    release_resource(middle);
    FCK_EQ(first->next, last, "middle removal reconnects forward link");
    FCK_EQ(last->prev, first, "middle removal reconnects backward link");
    FCK_EQ(resource_head, first, "middle removal preserves head");
    FCK_EQ(resource_tail, last, "middle removal preserves tail");
    FCK_EQ(fck_call_n(raw_free_event, 0).a0, &blocks[1].size, "middle header handed off");

    release_resource(last);
    FCK_EQ(fck_call_n(release_event, 0).a0, last, "destructor receives tail resource");
    FCK_EQ(fck_call_n(release_event, 0).a1, first, "destructor sees live predecessor");
    FCK_EQ(fck_call_n(release_event, 0).a2, 0, "destructor sees terminal next");
    FCK_EQ(first->next, 0, "new tail next cleared");
    FCK_EQ(resource_tail, first, "tail updated");
    FCK_EQ(fck_call_n(raw_free_event, 1).a0, &blocks[2].size, "tail header handed off");

    release_resource(first);
    FCK_EQ(resource_head, 0, "singleton removal clears head");
    FCK_EQ(resource_tail, 0, "singleton removal clears tail");
    FCK_EQ(fck_call_n(raw_free_event, 2).a0, &blocks[0].size, "singleton header handed off");
    FCK_EQ(fck_call_n(raw_free_event, 2).a2, 0, "head cleared before raw free");
    fck_expect_order(order, 5, "two destructors and three frees in order");
    check_resource_guards(blocks);
}

static void test_release_all_resources(void)
{
    struct ResourceBlock blocks[3];
    static const char *order[] = {release_event, raw_free_event, raw_free_event,
                                  release_event, raw_free_event};
    int i;

    /* @leaf 80019BA4 source empty and populated registry teardown walks forward once, drains both roots and is a no-op on a second call */
    LK_FUNC("func_80019BA4");
    reset_heap(1);
    init_resources(blocks);
    release_all_resources();
    FCK_EQ(resource_head, 0, "registry drained at head");
    FCK_EQ(resource_tail, 0, "registry drained at tail");
    fck_expect_order(order, 5, "front-to-back callbacks and frees");
    for (i = 0; i < 3; i++) {
        FCK_EQ(fck_call_n(raw_free_event, i).a0, &blocks[i].size, "registry node freed in list order");
    }
    FCK_EQ(fck_call_n(release_event, 0).a2, &blocks[1].resource, "first callback sees its successor");
    FCK_EQ(fck_call_n(raw_free_event, 0).a2, &blocks[1].resource, "head advances before first free");
    FCK_EQ(fck_call_n(release_event, 1).a1, 0, "last callback sees itself as the head");
    FCK_EQ(blocks[1].resource.prev, 0, "middle predecessor cleared when it became head");
    FCK_EQ(heap_used, 0xFB8, "registry drained exactly three 24-byte raw blocks");
    release_all_resources();
    FCK_EQ(fck_log_count, 5, "empty registry performs no additional calls");
    FCK_EQ(heap_used, 0xFB8, "empty registry does not change accounting");
    check_resource_guards(blocks);
}

static void test_free_anchor_list(void)
{
    struct AnchorBlock first, second;
    struct RawBlock owned, borrowed;
    /* @leaf 8003D0AC source traverses the anchor list with original node destructor, frees owned but not borrowed state, and clears its root */
    LK_FUNC("func_8003D0AC");
    reset_heap(1);
    lk_fill(&first, GUARD, sizeof first);
    lk_fill(&second, GUARD, sizeof second);
    lk_fill(&owned, GUARD, sizeof owned);
    lk_fill(&borrowed, GUARD, sizeof borrowed);
    first.size = second.size = sizeof first.node;
    owned.size = 0x20;
    borrowed.size = 0x10;
    first.node.next = &second.node;
    first.node.flags = 0x82;
    first.node.buffer = owned.payload;
    second.node.next = 0;
    second.node.flags = 0x81;
    second.node.buffer = borrowed.payload;
    anchor_head = &first.node;

    free_anchor_list();
    FCK_EQ(anchor_head, 0, "anchor list root is cleared");
    FCK_EQ(fck_log_count, 3, "only owned buffer and both nodes are freed");
    FCK_EQ(fck_call_n(raw_free_event, 0).a0, &owned.size, "owned buffer freed before its node");
    FCK_EQ(fck_call_n(raw_free_event, 1).a0, &first.size, "first node follows its buffer");
    FCK_EQ(fck_call_n(raw_free_event, 2).a0, &second.size, "second node freed without its borrowed buffer");
    FCK_EQ(heap_used, 0xF5C, "accounting excludes borrowed buffer");
    free_anchor_list();
    FCK_EQ(fck_log_count, 3, "empty anchor list performs no additional frees");
    FCK_EQ(first.before, GUARD_WORD, "first anchor leading canary");
    FCK_EQ(first.after, GUARD_WORD, "first anchor trailing canary");
    FCK_EQ(second.after, GUARD_WORD, "second anchor trailing canary");
    FCK_CHECK(lk_all(borrowed.payload, 0, sizeof borrowed.payload, GUARD), "borrowed payload intact");
}

/* Layout shared by the owner filter and the slot retirement callback. */
struct EntityOwner {
    u8 prefix[0x14];
    u8 kind;
};

struct EntityBinding {
    s32 reserved;
    void *key;
    u8 state[0x30];
    u8 detached;
};

struct Entity {
    struct Entity *next;
    s16 slot;
    u8 busy;
    s8 state;
    u8 pad08[7];
    u8 flags;
    u8 mode;
    u8 pad11[3];
    s16 timer;
    u8 pad16[6];
    struct EntityBinding *binding;
    struct EntityOwner *owner;
    void (*message)(s32, struct Entity *, s32, s32);
    s32 guard;
};

struct Deferred {
    struct Deferred *next;
    u8 kind;
    u8 state[0x0B];
    void *key;
};

struct DeferredBlock {
    s32 before, size;
    struct Deferred entry;
    s32 after;
};

struct Entity *entity_heads[7] __asm__("D_800A1070");
struct Deferred *deferred_head __asm__("D_8009F30C");
void retire_entities(void *key, s32 kind) __asm__("func_8003FB90");

static const char message_event[] = "entity teardown callback";

static void record_entity_message(s32 message, struct Entity *entity, s32 arg2, s32 arg3)
{
    fck_record(message_event, message, (s32)entity, entity->flags);
    FCK_EQ(arg2, 0, "teardown callback arg2");
    FCK_EQ(arg3, 0, "teardown callback arg3");
}

static void test_retire_entities(void)
{
    enum { MARK, RETIRE, OTHER_KIND, OTHER_KEY, DETACHED, UNBOUND, N_ENTITIES };
    enum { HEAD, WRONG_KEY, MIDDLE, CONSECUTIVE, NO_KEY, WRONG_KIND, TAIL, N_DEFERRED };
    struct Entity entities[N_ENTITIES];
    struct EntityOwner kind7, kind8;
    struct EntityBinding binding, other, detached;
    struct DeferredBlock blocks[N_DEFERRED];
    static const char *order[] = {message_event, raw_free_event, raw_free_event,
                                  raw_free_event, raw_free_event};
    int i;

    /* @leaf 8003FB90 source filters active slots by kind and effective owner, marks or retires matches, and unlinks matching deferred entries before freeing them */
    LK_FUNC("func_8003FB90");
    reset_heap(1);
    lk_fill(&kind7, GUARD, sizeof kind7);
    lk_fill(&kind8, GUARD, sizeof kind8);
    kind7.kind = 7;
    kind8.kind = 8;
    lk_fill(&binding, GUARD, sizeof binding);
    binding.key = &kind7;
    binding.detached = 0;
    other = binding;
    other.key = &kind8;
    detached = binding;
    detached.detached = 1;

    lk_fill(entities, GUARD, sizeof entities);
    for (i = 0; i < N_ENTITIES; i++) {
        entities[i].next = i + 1 < N_ENTITIES ? &entities[i + 1] : 0;
        entities[i].owner = &kind7;
        entities[i].binding = &binding;
        entities[i].flags = 0x40;
        entities[i].mode = 0x20;
        entities[i].message = record_entity_message;
    }
    entities[MARK].next = &entities[OTHER_KIND];
    entities[OTHER_KIND].owner = &kind8;
    entities[OTHER_KEY].binding = &other;
    entities[DETACHED].binding = &detached;
    entities[UNBOUND].binding = 0;
    entities[RETIRE].next = 0;
    entities[RETIRE].mode = 0x24;
    entities[RETIRE].slot = 12;
    entities[RETIRE].busy = 3;
    entities[RETIRE].state = 5;
    entities[RETIRE].timer = 120;
    for (i = 0; i < 7; i++) entity_heads[i] = 0;
    entity_heads[0] = &entities[MARK];
    entity_heads[6] = &entities[RETIRE];

    lk_fill(blocks, GUARD, sizeof blocks);
    for (i = 0; i < N_DEFERRED; i++) {
        blocks[i].size = sizeof blocks[i].entry;
        blocks[i].entry.next = i + 1 < N_DEFERRED ? &blocks[i + 1].entry : 0;
        blocks[i].entry.kind = 7;
        blocks[i].entry.key = &kind7;
    }
    blocks[WRONG_KEY].entry.key = &kind8;
    blocks[NO_KEY].entry.key = 0;
    blocks[WRONG_KIND].entry.kind = 8;
    deferred_head = &blocks[HEAD].entry;

    retire_entities(&kind7, 7);
    FCK_EQ(entities[MARK].flags, 0x41, "matching non-immediate slot marked");
    FCK_EQ(entities[RETIRE].flags, 0x42, "last list bucket processed by original retirement");
    FCK_EQ(entities[RETIRE].slot, -1, "immediate retirement clears slot");
    FCK_EQ(entities[RETIRE].busy, 0, "immediate retirement clears busy byte");
    FCK_EQ(entities[RETIRE].state, -1, "immediate retirement clears state byte");
    FCK_EQ(entities[RETIRE].timer, 0, "immediate retirement clears timer");
    for (i = OTHER_KIND; i < N_ENTITIES; i++) {
        FCK_EQ(entities[i].flags, 0x40, "nonmatching active slot remains unmarked");
    }
    FCK_EQ(entity_heads[0], &entities[MARK], "first bucket root unchanged");
    FCK_EQ(entity_heads[6], &entities[RETIRE], "last bucket root unchanged");
    fck_expect_order(order, 5, "immediate callback precedes all deferred frees");
    FCK_EQ(fck_call_n(message_event, 0).a0, 7, "original retirement dispatches message 7");
    FCK_EQ(fck_call_n(message_event, 0).a1, &entities[RETIRE], "message goes to immediate slot");
    FCK_EQ(fck_call_n(message_event, 0).a2, 0x40, "callback sees pre-retirement flags");
    FCK_EQ(deferred_head, &blocks[WRONG_KEY].entry, "deferred head advanced");
    FCK_EQ(blocks[WRONG_KEY].entry.next, &blocks[NO_KEY].entry, "predecessor skips consecutive matches");
    FCK_EQ(blocks[NO_KEY].entry.next, &blocks[WRONG_KIND].entry, "unmatched link preserved");
    FCK_EQ(blocks[WRONG_KIND].entry.next, 0, "deferred tail unlinked");
    FCK_EQ(fck_call_n(raw_free_event, 0).a0, &blocks[HEAD].size, "old head freed first");
    FCK_EQ(fck_call_n(raw_free_event, 1).a0, &blocks[MIDDLE].size, "first interior match freed");
    FCK_EQ(fck_call_n(raw_free_event, 2).a0, &blocks[CONSECUTIVE].size, "second interior match freed");
    FCK_EQ(fck_call_n(raw_free_event, 3).a0, &blocks[TAIL].size, "old tail freed last");
    FCK_EQ(blocks[HEAD].entry.next, 0, "freed old head disconnected");
    FCK_EQ(blocks[MIDDLE].entry.next, 0, "freed middle entry disconnected");
    FCK_EQ(blocks[CONSECUTIVE].entry.next, 0, "freed consecutive entry disconnected");

    retire_entities(0, 7);
    FCK_EQ(entities[DETACHED].flags, 0x41, "detached binding matches null effective owner");
    FCK_EQ(entities[UNBOUND].flags, 0x41, "absent binding matches null effective owner");
    FCK_EQ(entities[OTHER_KEY].flags, 0x40, "valid non-null binding still excluded");
    FCK_EQ(fck_log_count, 6, "null owner frees exactly one more deferred entry");
    FCK_EQ(fck_call_n(raw_free_event, 4).a0, &blocks[NO_KEY].size, "null-owner deferred entry freed");
    FCK_EQ(deferred_head, &blocks[WRONG_KEY].entry, "nonmatching deferred head retained");
    FCK_EQ(blocks[WRONG_KEY].entry.next, &blocks[WRONG_KIND].entry, "remaining deferred entries reconnected");
    FCK_EQ(blocks[NO_KEY].entry.next, 0, "null-owner deferred entry disconnected");
    FCK_EQ(heap_used, 0xF88, "only five deferred blocks affect accounting");
    for (i = 0; i < N_ENTITIES; i++) {
        FCK_EQ(entities[i].guard, GUARD_WORD, "active entity trailing canary");
        FCK_CHECK(lk_all(entities[i].pad08, 0, 7, GUARD), "active entity neighboring state untouched");
    }
    for (i = 0; i < N_DEFERRED; i++) {
        FCK_EQ(blocks[i].before, GUARD_WORD, "deferred block leading canary");
        FCK_EQ(blocks[i].after, GUARD_WORD, "deferred block trailing canary");
        FCK_CHECK(lk_all(blocks[i].entry.state, 0, 0x0B, GUARD), "deferred entry state untouched");
    }
}

int main(void)
{
    test_free_block();
    test_free_raw();
    test_free_pool();
    test_release_resource();
    test_release_all_resources();
    test_free_anchor_list();
    test_retire_entities();
    return lk_done("leaf_release");
}
