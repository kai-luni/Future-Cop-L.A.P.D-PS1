/* Resource / memory-resources band leaf suite: five unchanged original MIPS C
 * units from src/game/memory-resources/ running under qemu-mipsel on explicit
 * synthetic fixtures.
 *
 * Every callee is a recording stand-in bound to its retail label with an asm
 * alias, so a wrong binding is a link error. No stand-in re-implements a unit
 * under test, and every expectation below is a literal read off the unit's own
 * C body (evidence class `source`: pins behaviour, does not validate retail).
 *
 * All five units are hardware-free and QEMU user-mode safe: they touch only
 * the descriptor passed in, their callees, and (for 80029650) the harness-
 * defined D_800A0B70 stream-buffer list. None of them performs an absolute
 * kseg0 RAM access, so nothing had to be skipped.
 *
 * 80029650 is exercised here as the real unit; other suites only carry a
 * stand-in for it, so this suite is its first direct coverage.
 */
#include "leaf_kit.h"

/* ============================================================================
 * Shared fixture types (layout copied from the units' own declarations)
 * ============================================================================
 */

/* gRegisterUniqueResource.c: u8 unk00[8]; s32 unk08; */
struct BUnk51650 {
    u8 unk00[8];
    s32 unk08;
};

/* gLoadActorDef5B.c / gLoadActorLightNode.c / gLoadMapTileCatalog.c layout */
struct BChunkOwner {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};

/* gReleaseStreamAssetReference.c: Node { next; s32 pad[2]; s32 count; } with
 * count at +0x0C; List { u8 pad[0x18]; Node *head; } with head at +0x18. */
struct BStreamNode {
    struct BStreamNode *next;
    s32 pad[2];
    s32 count;
};
struct BStreamList {
    u8 pad[0x18];
    struct BStreamNode *head;
};
struct BStreamList gStreamBufferList __asm__("D_800A0B70");

/* ============================================================================
 * Recording stand-ins, all bound to the retail labels the units call
 * ============================================================================
 */

static const char is_reg_call[] = "func_80019AA8";
static const char append_call[] = "func_80019A74";
static const char discard_call[] = "func_80019D00";
static const char chunk_call[] = "func_80019C38";
static const char slot_call[] = "func_8003BC7C";
static const char free_call[] = "func_800181D4";
static const char spawn_call[] = "func_80049E88";
static const char parse_call[] = "func_800639D4";
static const char recycle_call[] = "func_800288BC";

static struct {
    s32 calls;
    s32 a0;
    u8 ret;
} s_is_reg;

static struct {
    s32 calls;
    s32 a0;
} s_append;

static struct {
    s32 calls;
    s32 a0;
} s_discard;

static struct {
    s32 calls;
    s32 a0;
    s32 a1;
    s32 a2;
    s32 a3;
    u8 *ret;
} s_chunk;

static struct {
    s32 calls;
    s32 ptr;
} s_slot;

static struct {
    s32 calls;
    s32 ptr;
} s_free;

static struct {
    s32 calls;
    s32 data;
    s32 duration;
} s_spawn;

static struct {
    s32 calls;
    s32 a0;
    s32 a1;
    s32 a2;
} s_parse;

#define RECYCLE_LOG_MAX 8
static struct {
    s32 calls;
    s32 nodes[RECYCLE_LOG_MAX];
    s32 next_at_call[RECYCLE_LOG_MAX];
    s32 count_at_call[RECYCLE_LOG_MAX];
} s_recycle;

static void reset_stand_ins(void)
{
    fck_log_reset();
    memset(&s_is_reg, 0, sizeof(s_is_reg));
    memset(&s_append, 0, sizeof(s_append));
    memset(&s_discard, 0, sizeof(s_discard));
    memset(&s_chunk, 0, sizeof(s_chunk));
    memset(&s_slot, 0, sizeof(s_slot));
    memset(&s_free, 0, sizeof(s_free));
    memset(&s_spawn, 0, sizeof(s_spawn));
    memset(&s_parse, 0, sizeof(s_parse));
    memset(&s_recycle, 0, sizeof(s_recycle));
}

/* func_80019AA8: reports whether the descriptor is already registered. */
u8 mock_is_registered(s32 resource) __asm__("func_80019AA8");
u8 mock_is_registered(s32 resource)
{
    s_is_reg.calls++;
    s_is_reg.a0 = resource;
    fck_record(is_reg_call, resource, 0, 0);
    return s_is_reg.ret;
}

/* func_80019A74: appends a descriptor to the registry. */
void mock_append_resource(s32 resource) __asm__("func_80019A74");
void mock_append_resource(s32 resource)
{
    s_append.calls++;
    s_append.a0 = resource;
    fck_record(append_call, resource, 0, 0);
}

/* func_80019D00: duplicate-resource free scanner. */
void mock_discard_resource(s32 resource) __asm__("func_80019D00");
void mock_discard_resource(s32 resource)
{
    s_discard.calls++;
    s_discard.a0 = resource;
    fck_record(discard_call, resource, 0, 0);
}

/* func_80019C38: tagged chunk lookup; the test picks the returned pointer. */
u8 *mock_find_chunk(s32 a0, s32 a1, u32 tag, s32 a3) __asm__("func_80019C38");
u8 *mock_find_chunk(s32 a0, s32 a1, u32 tag, s32 a3)
{
    s_chunk.calls++;
    s_chunk.a0 = a0;
    s_chunk.a1 = a1;
    s_chunk.a2 = (s32)tag;
    s_chunk.a3 = a3;
    fck_record(chunk_call, a0, a1, (s32)tag);
    return s_chunk.ret;
}

/* func_8003BC7C: installs a slot spawn descriptor. */
void mock_set_slot_descriptor(s32 data) __asm__("func_8003BC7C");
void mock_set_slot_descriptor(s32 data)
{
    s_slot.calls++;
    s_slot.ptr = data;
    fck_record(slot_call, data, 0, 0);
}

/* func_800181D4: raw block free. */
void mock_free_block(s32 block) __asm__("func_800181D4");
void mock_free_block(s32 block)
{
    s_free.calls++;
    s_free.ptr = block;
    fck_record(free_call, block, 0, 0);
}

/* func_80049E88: spawn a map light from a descriptor plus a duration. */
s32 mock_try_spawn_light(s32 data, s32 duration) __asm__("func_80049E88");
s32 mock_try_spawn_light(s32 data, s32 duration)
{
    s_spawn.calls++;
    s_spawn.data = data;
    s_spawn.duration = duration;
    fck_record(spawn_call, data, duration, 0);
    return 0;
}

/* func_800639D4: re-enter a resource into the level chunk table. */
void mock_parse_sections(s32 a0, s32 a1, s32 a2) __asm__("func_800639D4");
void mock_parse_sections(s32 a0, s32 a1, s32 a2)
{
    s_parse.calls++;
    s_parse.a0 = a0;
    s_parse.a1 = a1;
    s_parse.a2 = a2;
    fck_record(parse_call, a0, a1, a2);
}

/* func_800288BC: recycles a retired module read buffer. Records the node's
 * link and count as they are at call time, so the "clear the link first"
 * behaviour is observable. */
void mock_recycle_node(s32 node) __asm__("func_800288BC");
void mock_recycle_node(s32 node)
{
    struct BStreamNode *n = (struct BStreamNode *)node;
    if (s_recycle.calls < RECYCLE_LOG_MAX) {
        s_recycle.nodes[s_recycle.calls] = node;
        s_recycle.next_at_call[s_recycle.calls] = (s32)n->next;
        s_recycle.count_at_call[s_recycle.calls] = n->count;
    }
    s_recycle.calls++;
    fck_record(recycle_call, node, (s32)n->next, n->count);
}

/* ============================================================================
 * 1. 80051650 - gRegisterUniqueResource
 * ============================================================================
 */
extern void gRegisterUniqueResource(struct BUnk51650 *resource) __asm__("func_80051650");

static void test_80051650(void)
{
    static const char *fresh_order[] = {is_reg_call, append_call};
    static const char *dup_order[] = {is_reg_call, discard_call};
    struct BUnk51650 res;

    /* @leaf 80051650 source fresh-vs-loaded gate: loaded==0 clears +0x08 and appends, else hands the descriptor to the duplicate free scanner */
    LK_FUNC("func_80051650");

    /* Fresh descriptor: load check 0 -> +0x08 cleared, then append. */
    lk_fill(&res, 0x5A, sizeof(res));
    res.unk08 = 0x12345678;
    reset_stand_ins();
    gRegisterUniqueResource(&res);
    FCK_EQ(s_is_reg.calls, 1, "fresh: one load check");
    FCK_EQ(s_is_reg.a0, (s32)&res, "fresh: check receives the descriptor");
    FCK_EQ(res.unk08, 0, "fresh: +0x08 cleared");
    FCK_EQ(lk_all(res.unk00, 0, 8, 0x5A), 1, "fresh: +0x00..+0x07 untouched");
    FCK_EQ(s_append.calls, 1, "fresh: appended once");
    FCK_EQ(s_append.a0, (s32)&res, "fresh: append receives the descriptor");
    FCK_EQ(s_discard.calls, 0, "fresh: no duplicate free");
    fck_expect_order(fresh_order, 2, "fresh: check then append");

    /* Already loaded: 1 -> duplicate free, +0x08 left alone. */
    lk_fill(&res, 0x5A, sizeof(res));
    res.unk08 = 0x0BADF00D;
    reset_stand_ins();
    s_is_reg.ret = 1;
    gRegisterUniqueResource(&res);
    FCK_EQ(s_is_reg.calls, 1, "loaded: one load check");
    FCK_EQ(s_is_reg.a0, (s32)&res, "loaded: check receives the descriptor");
    FCK_EQ(res.unk08, 0x0BADF00D, "loaded: +0x08 preserved");
    FCK_EQ(s_append.calls, 0, "loaded: not appended");
    FCK_EQ(s_discard.calls, 1, "loaded: duplicate freed once");
    FCK_EQ(s_discard.a0, (s32)&res, "loaded: duplicate free receives the descriptor");
    fck_expect_order(dup_order, 2, "loaded: check then duplicate free");

    /* Any non-zero u8 counts as loaded (0x80 tests the full byte). */
    lk_fill(&res, 0x77, sizeof(res));
    res.unk08 = 0x01020304;
    reset_stand_ins();
    s_is_reg.ret = 0x80;
    gRegisterUniqueResource(&res);
    FCK_EQ(s_discard.calls, 1, "ret 0x80: duplicate freed");
    FCK_EQ(s_append.calls, 0, "ret 0x80: not appended");
    FCK_EQ(res.unk08, 0x01020304, "ret 0x80: +0x08 preserved");

    /* Fresh path clears the whole word, not just the low half. */
    lk_fill(&res, 0x11, sizeof(res));
    res.unk08 = -1;
    reset_stand_ins();
    gRegisterUniqueResource(&res);
    FCK_EQ(res.unk08, 0, "fresh: -1 +0x08 fully cleared");
    FCK_EQ(s_append.calls, 1, "fresh second call appends");
}

/* ============================================================================
 * 2. 8003BDA0 - gLoadActorDef5B
 * ============================================================================
 */
extern void gLoadActorDef5B(struct BChunkOwner *resource) __asm__("func_8003BDA0");

static void test_8003BDA0(void)
{
    static const char *order[] = {chunk_call, slot_call, free_call};
    u8 blob[0x80];
    struct BChunkOwner res;

    /* @leaf 8003BDA0 source 'tACT' lookup with (blob, length, 0x74414354, id), slot install from payload-4, then free of the request block */
    LK_FUNC("func_8003BDA0");

    /* Chunk header at blob+0x40: the slot installer must receive blob+0x3C. */
    lk_fill(blob, 0x3C, sizeof(blob));
    lk_fill(&res, 0x99, sizeof(res));
    res.unk00 = (s32)blob;
    res.unk1C = 0x22;
    res.unk20 = 0x50;
    reset_stand_ins();
    s_chunk.ret = blob + 0x40;
    gLoadActorDef5B(&res);
    FCK_EQ(s_chunk.calls, 1, "one chunk lookup");
    FCK_EQ(s_chunk.a0, (s32)blob, "lookup blob base");
    FCK_EQ(s_chunk.a1, 0x50, "lookup blob length from +0x20");
    FCK_EQ(s_chunk.a2, 0x74414354, "lookup tag is 'tACT'");
    FCK_EQ(s_chunk.a3, 0x22, "lookup section id from +0x1C");
    FCK_EQ(s_slot.calls, 1, "slot descriptor installed once");
    FCK_EQ(s_slot.ptr, (s32)(blob + 0x3C), "slot data is chunk-4");
    FCK_EQ(s_free.calls, 1, "request block freed once");
    FCK_EQ(s_free.ptr, (s32)&res, "freed pointer is the request block");
    FCK_EQ(res.unk1C, 0x22, "request +0x1C unchanged");
    FCK_EQ(res.unk20, 0x50, "request +0x20 unchanged");
    FCK_EQ(lk_all(blob, 0, sizeof(blob), 0x3C), 1, "blob left untouched");
    fck_expect_order(order, 3, "lookup, install, free");

    /* Different header triple: negative id and length pass through as-is. */
    lk_fill(&res, 0x00, sizeof(res));
    res.unk00 = (s32)(blob + 0x10);
    res.unk1C = -3;
    res.unk20 = 0x1234;
    reset_stand_ins();
    s_chunk.ret = blob + 0x08;
    gLoadActorDef5B(&res);
    FCK_EQ(s_chunk.a0, (s32)(blob + 0x10), "lookup second blob base");
    FCK_EQ(s_chunk.a1, 0x1234, "lookup second length");
    FCK_EQ(s_chunk.a3, -3, "lookup signed negative id");
    FCK_EQ(s_slot.ptr, (s32)(blob + 0x04), "slot data is second chunk-4");
    FCK_EQ(s_free.ptr, (s32)&res, "second request freed");
    fck_expect_order(order, 3, "second lookup, install, free");

    /* Null lookup result: the -4 offset is applied unconditionally. */
    lk_fill(&res, 0x00, sizeof(res));
    res.unk00 = (s32)blob;
    res.unk1C = 0;
    res.unk20 = 0;
    reset_stand_ins();
    s_chunk.ret = 0;
    gLoadActorDef5B(&res);
    FCK_EQ(s_chunk.calls, 1, "null result: lookup still called");
    FCK_EQ(s_slot.ptr, -4, "null result: slot data is 0-4");
    FCK_EQ(s_free.calls, 1, "null result: request still freed");
}

/* ============================================================================
 * 3. 80049EA8 - gLoadActorLightNode
 * ============================================================================
 */
extern void gLoadActorLightNode(struct BChunkOwner *resource) __asm__("func_80049EA8");

static void test_80049EA8(void)
{
    static const char *order[] = {chunk_call, spawn_call, free_call};
    u8 blob[0x80];
    struct BChunkOwner res;

    /* @leaf 80049EA8 source 'tACT' lookup, map-light spawn from payload-4 with duration -1, then free of the request block */
    LK_FUNC("func_80049EA8");

    /* Chunk header at blob+0x28: spawn must receive blob+0x24 and -1. */
    lk_fill(blob, 0xC3, sizeof(blob));
    lk_fill(&res, 0x55, sizeof(res));
    res.unk00 = (s32)blob;
    res.unk1C = 0x61;
    res.unk20 = 0x70;
    reset_stand_ins();
    s_chunk.ret = blob + 0x28;
    gLoadActorLightNode(&res);
    FCK_EQ(s_chunk.calls, 1, "one chunk lookup");
    FCK_EQ(s_chunk.a0, (s32)blob, "lookup blob base");
    FCK_EQ(s_chunk.a1, 0x70, "lookup blob length from +0x20");
    FCK_EQ(s_chunk.a2, 0x74414354, "lookup tag is 'tACT'");
    FCK_EQ(s_chunk.a3, 0x61, "lookup section id from +0x1C");
    FCK_EQ(s_spawn.calls, 1, "light spawned once");
    FCK_EQ(s_spawn.data, (s32)(blob + 0x24), "spawn data is chunk-4");
    FCK_EQ(s_spawn.duration, -1, "spawn duration is -1 (persistent)");
    FCK_EQ(s_free.calls, 1, "request block freed once");
    FCK_EQ(s_free.ptr, (s32)&res, "freed pointer is the request block");
    FCK_EQ(lk_all(blob, 0, sizeof(blob), 0xC3), 1, "blob left untouched");
    FCK_EQ(res.unk1C, 0x61, "request +0x1C unchanged");
    fck_expect_order(order, 3, "lookup, spawn, free");

    /* Different triple keeps the same -4/-1 constants. */
    lk_fill(&res, 0x00, sizeof(res));
    res.unk00 = (s32)(blob + 0x20);
    res.unk1C = -9;
    res.unk20 = 0x9ABC;
    reset_stand_ins();
    s_chunk.ret = blob + 0x60;
    gLoadActorLightNode(&res);
    FCK_EQ(s_chunk.a0, (s32)(blob + 0x20), "second lookup blob base");
    FCK_EQ(s_chunk.a1, 0x9ABC, "second lookup length");
    FCK_EQ(s_chunk.a3, -9, "second lookup signed negative id");
    FCK_EQ(s_spawn.data, (s32)(blob + 0x5C), "second spawn data is chunk-4");
    FCK_EQ(s_spawn.duration, -1, "second spawn duration still -1");
    FCK_EQ(s_free.ptr, (s32)&res, "second request freed");

    /* Null lookup result: spawn still gets 0-4. */
    lk_fill(&res, 0x00, sizeof(res));
    reset_stand_ins();
    s_chunk.ret = 0;
    gLoadActorLightNode(&res);
    FCK_EQ(s_spawn.calls, 1, "null result: spawn still called");
    FCK_EQ(s_spawn.data, -4, "null result: spawn data is 0-4");
    FCK_EQ(s_spawn.duration, -1, "null result: duration still -1");
    FCK_EQ(s_free.calls, 1, "null result: request still freed");
}

/* ============================================================================
 * 4. 80063C20 - gLoadMapTileCatalog
 * ============================================================================
 */
extern void gLoadMapTileCatalog(struct BChunkOwner *resource) __asm__("func_80063C20");

static void test_80063C20(void)
{
    static const char *order[] = {append_call, parse_call};
    struct BChunkOwner res;

    /* @leaf 80063C20 source release bookkeeping first, then level registration with the header triple in (0x00, 0x20, 0x1C) order */
    LK_FUNC("func_80063C20");

    lk_fill(&res, 0x00, sizeof(res));
    res.unk00 = 0x11223344;
    res.unk1C = 0x2A;
    res.unk20 = 0x99;
    reset_stand_ins();
    gLoadMapTileCatalog(&res);
    FCK_EQ(s_append.calls, 1, "release bookkeeping called once");
    FCK_EQ(s_append.a0, (s32)&res, "bookkeeping receives the resource");
    FCK_EQ(s_parse.calls, 1, "level registration called once");
    FCK_EQ(s_parse.a0, 0x11223344, "registration gets +0x00");
    FCK_EQ(s_parse.a1, 0x99, "registration gets +0x20 as second arg");
    FCK_EQ(s_parse.a2, 0x2A, "registration gets +0x1C as third arg");
    FCK_EQ(res.unk00, 0x11223344, "resource +0x00 unchanged");
    FCK_EQ(res.unk1C, 0x2A, "resource +0x1C unchanged");
    fck_expect_order(order, 2, "bookkeeping before registration");

    /* Distinct values, including a negative section id: the argument order
     * must stay (unk00, unk20, unk1C), not the declaration order. */
    lk_fill(&res, 0xEE, sizeof(res));
    res.unk00 = -1000;
    res.unk1C = -77;
    res.unk20 = 0x10000;
    reset_stand_ins();
    gLoadMapTileCatalog(&res);
    FCK_EQ(s_append.calls, 1, "second bookkeeping call");
    FCK_EQ(s_parse.a0, -1000, "second registration +0x00");
    FCK_EQ(s_parse.a1, 0x10000, "second registration +0x20");
    FCK_EQ(s_parse.a2, -77, "second registration signed +0x1C");
    FCK_EQ(s_parse.calls, 1, "second registration called once");

    /* No further calls than the two documented ones. */
    fck_expect_order(order, 2, "exactly bookkeeping then registration");
}

/* ============================================================================
 * 5. 80029650 - gReleaseStreamAssetReference (the real unit)
 * ============================================================================
 */
extern void gReleaseStreamAssetReference(struct BStreamNode **arg0) __asm__("func_80029650");

static void stream_node_init(struct BStreamNode *node, s32 count)
{
    lk_fill(node, 0, sizeof(*node));
    node->next = 0;
    node->count = count;
}

static void check_recycled(int index, struct BStreamNode *node)
{
    FCK_EQ(s_recycle.nodes[index], (s32)node, "recycled node pointer");
    FCK_EQ(s_recycle.next_at_call[index], 0, "recycled node link cleared first");
}

static void test_80029650(void)
{
    static const char *one_n1[] = {recycle_call};
    static const char *two_n1_n2[] = {recycle_call, recycle_call};
    static const char *two_n1_head[] = {recycle_call, recycle_call};
    struct BStreamNode h, n1, n2, n3, handle;
    struct BStreamNode *p;

    /* @leaf 80029650 source handle count decrement, zero-count successor splice-out, cleared link before recycle, and one tail-time head removal */
    LK_FUNC("func_80029650");

    /* Empty list: only the handle's count moves. */
    stream_node_init(&handle, 5);
    p = &handle;
    gStreamBufferList.head = 0;
    reset_stand_ins();
    gReleaseStreamAssetReference(&p);
    FCK_EQ(handle.count, 4, "empty list: handle count 5 -> 4");
    FCK_EQ((u32)p, (u32)&handle, "empty list: handle pointer unchanged");
    FCK_EQ(s_recycle.calls, 0, "empty list: nothing recycled");
    FCK_EQ((u32)gStreamBufferList.head, 0U, "empty list: head stays null");

    /* Single retained head: walk ends at the tail, head count non-zero. */
    stream_node_init(&h, 7);
    stream_node_init(&handle, 2);
    p = &handle;
    gStreamBufferList.head = &h;
    reset_stand_ins();
    gReleaseStreamAssetReference(&p);
    FCK_EQ(handle.count, 1, "single head: handle count 2 -> 1");
    FCK_EQ(h.count, 7, "single head: retained head count unchanged");
    FCK_EQ((u32)gStreamBufferList.head, (u32)&h, "single head: still head");
    FCK_EQ((u32)h.next, 0U, "single head: link stays null");
    FCK_EQ(s_recycle.calls, 0, "single head: nothing recycled");

    /* Zero-count successor is spliced out, non-zero successor is kept. */
    stream_node_init(&h, 7);
    stream_node_init(&n1, 0);
    stream_node_init(&n2, 3);
    stream_node_init(&handle, 2);
    h.next = &n1;
    n1.next = &n2;
    p = &handle;
    gStreamBufferList.head = &h;
    reset_stand_ins();
    gReleaseStreamAssetReference(&p);
    FCK_EQ(handle.count, 1, "splice: handle count 2 -> 1");
    FCK_EQ(s_recycle.calls, 1, "splice: one node recycled");
    check_recycled(0, &n1);
    FCK_EQ(s_recycle.count_at_call[0], 0, "splice: recycled node count 0");
    FCK_EQ((u32)h.next, (u32)&n2, "splice: head links over the removed node");
    FCK_EQ((u32)n1.next, 0U, "splice: removed node link cleared");
    FCK_EQ((u32)n2.next, 0U, "splice: kept successor unchanged");
    FCK_EQ((u32)gStreamBufferList.head, (u32)&h, "splice: head untouched");
    fck_expect_order(one_n1, 1, "splice: only n1 recycled");

    /* Zero-count head is only removed once the walk reaches the tail. */
    stream_node_init(&h, 0);
    stream_node_init(&n1, 2);
    stream_node_init(&handle, 4);
    h.next = &n1;
    p = &handle;
    gStreamBufferList.head = &h;
    reset_stand_ins();
    gReleaseStreamAssetReference(&p);
    FCK_EQ(handle.count, 3, "head removal: handle count 4 -> 3");
    FCK_EQ(s_recycle.calls, 1, "head removal: head recycled once");
    check_recycled(0, &h);
    FCK_EQ((u32)gStreamBufferList.head, (u32)&n1, "head removal: successor becomes head");
    FCK_EQ((u32)h.next, 0U, "head removal: removed head link cleared");
    FCK_EQ((u32)n1.next, 0U, "head removal: new head link unchanged");
    FCK_EQ(n1.count, 2, "head removal: new head count unchanged");

    /* Several zero-count successors are removed in list order. */
    stream_node_init(&h, 5);
    stream_node_init(&n1, 0);
    stream_node_init(&n2, 0);
    stream_node_init(&n3, 1);
    h.next = &n1;
    n1.next = &n2;
    n2.next = &n3;
    stream_node_init(&handle, 9);
    p = &handle;
    gStreamBufferList.head = &h;
    reset_stand_ins();
    gReleaseStreamAssetReference(&p);
    FCK_EQ(handle.count, 8, "multi: handle count 9 -> 8");
    FCK_EQ(s_recycle.calls, 2, "multi: two nodes recycled");
    check_recycled(0, &n1);
    check_recycled(1, &n2);
    FCK_EQ((u32)h.next, (u32)&n3, "multi: head now links to n3");
    FCK_EQ((u32)n1.next, 0U, "multi: n1 link cleared");
    FCK_EQ((u32)n2.next, 0U, "multi: n2 link cleared");
    fck_expect_order(two_n1_n2, 2, "multi: n1 then n2");

    /* Handle is the single head: the decrement happens before the walk, so
     * the head is removed by the tail-time head check. */
    stream_node_init(&h, 1);
    p = &h;
    gStreamBufferList.head = &h;
    reset_stand_ins();
    gReleaseStreamAssetReference(&p);
    FCK_EQ(h.count, 0, "self head: count 1 -> 0");
    FCK_EQ(s_recycle.calls, 1, "self head: recycled once");
    check_recycled(0, &h);
    FCK_EQ((u32)gStreamBufferList.head, 0U, "self head: head cleared");
    FCK_EQ((u32)h.next, 0U, "self head: link cleared");

    /* Handle is a successor: the decremented node is removed by the walk. */
    stream_node_init(&h, 5);
    stream_node_init(&n1, 1);
    h.next = &n1;
    p = &n1;
    gStreamBufferList.head = &h;
    reset_stand_ins();
    gReleaseStreamAssetReference(&p);
    FCK_EQ(n1.count, 0, "successor handle: count 1 -> 0");
    FCK_EQ(s_recycle.calls, 1, "successor handle: recycled once");
    check_recycled(0, &n1);
    FCK_EQ((u32)h.next, 0U, "successor handle: head link cleared");
    FCK_EQ(h.count, 5, "successor handle: head count unchanged");

    /* Both the successor and the head are zero: successor first, then head,
     * and the loop stops (head is checked exactly once). */
    stream_node_init(&h, 0);
    stream_node_init(&n1, 0);
    h.next = &n1;
    stream_node_init(&handle, 6);
    p = &handle;
    gStreamBufferList.head = &h;
    reset_stand_ins();
    gReleaseStreamAssetReference(&p);
    FCK_EQ(handle.count, 5, "all zero: handle count 6 -> 5");
    FCK_EQ(s_recycle.calls, 2, "all zero: two nodes recycled");
    check_recycled(0, &n1);
    check_recycled(1, &h);
    FCK_EQ((u32)gStreamBufferList.head, 0U, "all zero: list emptied");
    FCK_EQ((u32)h.next, 0U, "all zero: head link cleared");
    FCK_EQ((u32)n1.next, 0U, "all zero: n1 link cleared");
    fck_expect_order(two_n1_head, 2, "all zero: n1 then head");
}

int main(void)
{
    test_80051650();
    test_8003BDA0();
    test_80049EA8();
    test_80063C20();
    test_80029650();
    return lk_done("leaf_res_r3");
}
