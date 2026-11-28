/* Stream/resource tranche 4: unchanged original MIPS C under qemu-mipsel.
 * Expectations are hand-derived from the bodies and caller evidence cited in
 * each block. All entry points return void; check state and ordered calls,
 * not the unspecified value left in $v0. Only external dependencies are
 * replaced, using their retail labels. The stand-ins record without freeing
 * or changing fixture state.
 */
#include "leaf_kit.h"

struct StreamBufferList {
    s32 slot;
    s32 field04;
    s32 field08;
    u8 untouched0C[0x20];
    void *pending;
    u8 untouched30[0x14];
    s32 field44;
    s32 field48;
    s32 field4C;
};

struct StreamBufferList stream __asm__("D_800A0B70");
volatile u8 read_busy __asm__("D_8009F6A8");

static u32 pending_pool;
static const char stop_call[] = "func_80029DD8";
static const char flush_call[] = "func_80029520";
static const char pool_call[] = "func_80019584";
static const char close_call[] = "func_8001D3A8";
static const char *reset_order[] = {stop_call, flush_call, pool_call, close_call};

/* Snapshot the ordering-sensitive fields at each external boundary. */
static struct StreamBufferList at_stop, at_flush, at_pool, at_close;

void stop_portrait(void) __asm__("func_80029DD8");
void stop_portrait(void)
{
    at_stop = stream;
    fck_record(stop_call, 0, 0, 0);
}

void flush_releases(void) __asm__("func_80029520");
void flush_releases(void)
{
    at_flush = stream;
    fck_record(flush_call, 0, 0, 0);
}

void free_pool(void *pool) __asm__("func_80019584");
void free_pool(void *pool)
{
    at_pool = stream;
    fck_record(pool_call, (s32)pool, 0, 0);
}

void close_slot(s32 slot) __asm__("func_8001D3A8");
void close_slot(s32 slot)
{
    at_close = stream;
    fck_record(close_call, slot, 0, 0);
}

static void seed_stream(s32 slot, void *pending)
{
    lk_fill(&stream, 0x5a, sizeof(stream));
    stream.slot = slot;
    stream.pending = pending;
    fck_log_reset();
}

static void expect_stream_reset(void)
{
    FCK_EQ(stream.slot, -1, "reset: slot invalidated");
    FCK_EQ(stream.field04, 0, "reset: clears +04");
    FCK_EQ(stream.field08, 0, "reset: clears +08");
    FCK_EQ(stream.field44, 0, "reset: clears +44");
    FCK_EQ(stream.field48, 0, "reset: clears +48");
    FCK_EQ(stream.field4C, 0, "reset: clears +4c");
    FCK_EQ(stream.pending, 0, "reset: pending slot empty");
    FCK_CHECK(lk_all(stream.untouched0C, 0, 0x20, 0x5a),
              "reset: +0c through +2b untouched");
    FCK_CHECK(lk_all(stream.untouched30, 0, 0x14, 0x5a),
              "reset: +30 through +43 untouched");
}

extern void reset_stream(void) __asm__("func_80029EB8");

static void test_reset_stream(void)
{
    /* @leaf 80029EB8 source gResetModuleStreamLoader.c:53-70 invalidates the slot before stop/flush, clears five words, then conditionally frees the pool and closes the original slot */
    LK_FUNC("func_80029EB8");

    seed_stream(7, &pending_pool);
    read_busy = 0;
    reset_stream();
    /* Lines 53-65: invalidate before the two no-argument calls; the remaining
     * words and pending pointer survive those calls, then the five words clear
     * before the $a0 pool release. Lines 65-70: clear pending before close. */
    fck_expect_order(reset_order, 4, "reset: stop, flush, free, close");
    FCK_EQ(at_stop.slot, -1, "reset: stop sees invalidated slot");
    FCK_EQ(at_stop.field04, 0x5a5a5a5a, "reset: stop precedes word clears");
    FCK_EQ(at_flush.slot, -1, "reset: flush sees invalidated slot");
    FCK_EQ(at_flush.field4C, 0x5a5a5a5a, "reset: flush precedes word clears");
    FCK_EQ(at_pool.field04, 0, "reset: free follows +04 clear");
    FCK_EQ(at_pool.field08, 0, "reset: free follows +08 clear");
    FCK_EQ(at_pool.field44, 0, "reset: free follows +44 clear");
    FCK_EQ(at_pool.field48, 0, "reset: free follows +48 clear");
    FCK_EQ(at_pool.field4C, 0, "reset: free follows +4c clear");
    FCK_EQ(at_pool.pending, &pending_pool, "reset: free precedes pending clear");
    FCK_EQ(fck_call_n(pool_call, 0).a0, &pending_pool, "reset: free receives pending pool");
    FCK_EQ(at_close.pending, 0, "reset: close follows pending clear");
    FCK_EQ(fck_call_n(close_call, 0).a0, 7, "reset: close receives original slot");
    FCK_EQ(read_busy, 0, "reset: busy byte is only read");
    expect_stream_reset();

    /* Lines 62-70: no pending pool suppresses only free; zero is a valid slot. */
    seed_stream(0, 0);
    reset_stream();
    {
        const char *order[] = {stop_call, flush_call, close_call};
        fck_expect_order(order, 3, "reset: null pool still closes slot zero");
    }
    FCK_EQ(fck_call_n(close_call, 0).a0, 0, "reset: close receives slot zero");
    expect_stream_reset();

    /* Lines 62-70: slot -1 suppresses only the wait/close, even while busy. */
    seed_stream(-1, &pending_pool);
    read_busy = 0x80;
    reset_stream();
    fck_expect_order(reset_order, 3, "reset: no slot stops, flushes and frees only");
    FCK_EQ(fck_call_n(pool_call, 0).a0, &pending_pool, "reset: no slot still frees pool");
    FCK_EQ(read_busy, 0x80, "reset: no slot leaves busy byte alone");
    expect_stream_reset();
}

int main(void)
{
    test_reset_stream();
    return lk_done("leaf_stream_r4");
}
