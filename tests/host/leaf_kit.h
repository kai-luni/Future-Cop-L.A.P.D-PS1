/* Per-function attribution on top of fc_testkit.h.
 *
 * fc_testkit counts checks per binary. For leaf suites that put many game
 * functions into one harness, the useful number is checks *per function*, so a
 * suite with 300 assertions on 3 functions cannot pass itself off as 300
 * tested functions. Every block starts with LK_FUNC("func_XXXXXXXX"); the
 * checks until the next LK_FUNC are attributed to that function, and lk_done()
 * prints one machine-readable line per function:
 *
 *     LEAF func_80092488 checks=6 failures=0
 *
 * tools/host_leaf_suite.py report parses these lines into the test-to-function
 * map. A function with checks=0 is reported as a failure, not as tested.
 */
#ifndef LEAF_KIT_H
#define LEAF_KIT_H

#include "fc_testkit.h"

static const char *lk_name;
static int lk_checks0, lk_fail0, lk_funcs, lk_empty;

static void lk_flush(void)
{
    int n, f;
    if (!lk_name) return;
    n = fck_checks - lk_checks0;
    f = fck_failures - lk_fail0;
    fck_write("LEAF ");
    fck_write(lk_name);
    fck_write(" checks=");
    fck_write_int(n);
    fck_write(" failures=");
    fck_write_int(f);
    fck_write("\n");
    lk_funcs++;
    if (n == 0) {
        fck_write("FAIL: ");
        fck_write(lk_name);
        fck_write(" has no checks\n");
        lk_empty++;
    }
    lk_name = 0;
}

static void LK_FUNC(const char *name)
{
    lk_flush();
    lk_name = name;
    lk_checks0 = fck_checks;
    lk_fail0 = fck_failures;
}

/* Fill a buffer with a canary byte so untouched bytes can be asserted. */
static void lk_fill(void *p, int c, int n)
{
    u8 *b = p;
    while (n--) *b++ = (u8)c;
}

/* 1 when bytes [from, to) of p all equal c. */
static int lk_all(const void *p, int from, int to, int c)
{
    const u8 *b = p;
    int i;
    for (i = from; i < to; i++)
        if (b[i] != (u8)c) return 0;
    return 1;
}

static int lk_done(const char *suite)
{
    lk_flush();
    if (lk_empty) fck_failures++;
    fck_write("LEAF-SUMMARY functions=");
    fck_write_int(lk_funcs);
    fck_write("\n");
    return fck_report(suite);
}

#endif
