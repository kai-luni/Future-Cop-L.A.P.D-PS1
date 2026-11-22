/* Shared test kit for the host harnesses.
 *
 * Every harness under tests/host/ runs the *unchanged* decomp C under
 * qemu-mipsel. That only works when the units it links can reach every symbol
 * they call. This header gives the harnesses one place for the pieces that kept
 * being rewritten per test: freestanding syscalls, assertion reporting, a call
 * recorder, and the tiny libc the compiler sometimes emits calls to.
 *
 * Rules this kit follows, because breaking them is how a mock stops being a
 * measurement:
 *
 *  1. A stand-in is bound to the retail label with an asm alias, e.g.
 *
 *         u32 gNextRandomValue(void) __asm__("func_80019668");
 *
 *     so a wrong binding is a link error instead of a silently different
 *     function. Never stub by C name.
 *
 *  2. Types and argument counts must match the original prototype exactly.
 *     These are 32-bit MIPS pointers and words; a mismatched prototype is
 *     undefined behaviour that happens to "work" until it does not.
 *
 *  3. A stand-in records what it received and returns a value the test set
 *     beforehand. It never invents plausible output: an unconfigured return is
 *     0, and a test that depends on the return must configure it.
 *
 *  4. Data globals get a zeroed slot, because that is what the retail image
 *     holds before any code writes them. A zeroed slot is honest; a fabricated
 *     non-zero one is not.
 *
 * Include this from a harness *and* keep the unit list in the test script
 * explicit, so it stays visible which real C is under test and which symbols
 * are stand-ins.
 */
#ifndef FC_TESTKIT_H
#define FC_TESTKIT_H

#ifndef FCK_TYPES_H
#define FCK_TYPES_H
typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;
#endif

/* ------------------------------------------------------------------ output */

/* write(2) through the syscall gate - no libc, the harness is freestanding. */
static long fck_sys3(long n, long a, long b, long c)
{
    register long v0 __asm__("$2") = n;
    register long a0 __asm__("$4") = a;
    register long a1 __asm__("$5") = b;
    register long a2 __asm__("$6") = c;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2)
                     : "memory", "$7", "$8", "$9", "$10", "$11", "$12", "$13",
                       "$14", "$15", "$24", "$25");
    return v0;
}

static void fck_write(const char *text)
{
    int n = 0;
    while (text[n]) n++;
    fck_sys3(4004, 1, (long)text, n);
}

/* Decimal, so failures can print the actual value instead of just "wrong". */
static void fck_write_int(s32 value)
{
    char buf[16];
    int i = 15;
    u32 v;
    int neg = value < 0;
    buf[i] = 0;
    v = neg ? (u32)(-value) : (u32)value;
    if (!v) buf[--i] = '0';
    while (v) { buf[--i] = (char)('0' + (v % 10)); v /= 10; }
    if (neg) buf[--i] = '-';
    fck_write(&buf[i]);
}

/* Read a whole fixture file into `data`. Returns the length or -1. */
static int fck_read_file(const char *path, char *data, int size)
{
    long fd = fck_sys3(4005, (long)path, 0, 0);
    long n;
    if (fd < 0) return -1;
    n = fck_sys3(4003, fd, (long)data, size - 1);
    fck_sys3(4006, fd, 0, 0);
    if (n < 0) return -1;
    data[n] = 0;
    return (int)n;
}

/* -------------------------------------------------------------- assertions */

static int fck_failures;
static int fck_checks;

/* Report a failed check and keep going, so one run lists every break. */
#define FCK_CHECK(cond, what) do {                                     \
    fck_checks++;                                                      \
    if (!(cond)) {                                                     \
        fck_write("FAIL: " what "\n");                                 \
        fck_failures++;                                                \
    }                                                                  \
} while (0)

/* Equality with both values printed - use this wherever a value is compared. */
#define FCK_EQ(actual, expected, what) do {                            \
    s32 fck_a_ = (s32)(actual);                                        \
    s32 fck_e_ = (s32)(expected);                                      \
    fck_checks++;                                                      \
    if (fck_a_ != fck_e_) {                                            \
        fck_write("FAIL: " what " got ");                              \
        fck_write_int(fck_a_);                                         \
        fck_write(" want ");                                           \
        fck_write_int(fck_e_);                                         \
        fck_write("\n");                                               \
        fck_failures++;                                                \
    }                                                                  \
} while (0)

/* 0 when everything held. Test scripts treat non-zero as a hard failure. */
static int fck_report(const char *suite)
{
    if (!fck_checks) {
        fck_write("FAIL: ");
        fck_write(suite);
        fck_write(" ran no checks\n");
        return 1;
    }
    if (fck_failures) {
        fck_write(suite);
        fck_write(": FAIL\n");
        return 1;
    }
    fck_write(suite);
    fck_write(": all ");
    fck_write_int(fck_checks);
    fck_write(" checks PASS\n");
    return 0;
}

/* ------------------------------------------------------------ call recorder */

/* One recorded call: which stand-in, and its first three arguments. That is
 * enough to assert order and arguments without inventing per-mock structs. */
#define FCK_LOG_MAX 256

struct FckCall {
    const char *name;   /* retail label of the stand-in, for readable failures */
    s32 a0, a1, a2;
};

static struct FckCall fck_log[FCK_LOG_MAX];
static int fck_log_count;
static int fck_log_overflow;

static void fck_log_reset(void)
{
    int i;
    fck_log_count = 0;
    fck_log_overflow = 0;
    for (i = 0; i < FCK_LOG_MAX; i++) {
        fck_log[i].name = 0;
        fck_log[i].a0 = fck_log[i].a1 = fck_log[i].a2 = 0;
    }
}

static void fck_record(const char *name, s32 a0, s32 a1, s32 a2)
{
    if (fck_log_count >= FCK_LOG_MAX) {
        /* Overflow is a test bug, not a silent truncation. */
        fck_log_overflow = 1;
        return;
    }
    fck_log[fck_log_count].name = name;
    fck_log[fck_log_count].a0 = a0;
    fck_log[fck_log_count].a1 = a1;
    fck_log[fck_log_count].a2 = a2;
    fck_log_count++;
}

/* How many calls landed on this stand-in. */
static int fck_calls_to(const char *name)
{
    int i, n = 0;
    for (i = 0; i < fck_log_count; i++)
        if (fck_log[i].name == name) n++;
    return n;
}

/* The nth (0-based) call to this stand-in, or a zeroed record. */
static struct FckCall fck_call_n(const char *name, int nth)
{
    int i;
    for (i = 0; i < fck_log_count; i++) {
        if (fck_log[i].name == name) {
            if (nth-- == 0) return fck_log[i];
        }
    }
    {
        struct FckCall none;
        none.name = 0;
        none.a0 = none.a1 = none.a2 = 0;
        return none;
    }
}

/* Assert the whole recorded order, one label per expected call. */
static void fck_expect_order(const char **expected, int count, const char *what)
{
    int i;
    fck_checks++;
    if (fck_log_count != count) {
        fck_write("FAIL: ");
        fck_write(what);
        fck_write(" called ");
        fck_write_int(fck_log_count);
        fck_write(" times, expected ");
        fck_write_int(count);
        fck_write("\n");
        fck_failures++;
        return;
    }
    for (i = 0; i < count; i++) {
        if (fck_log[i].name != expected[i]) {
            fck_write("FAIL: ");
            fck_write(what);
            fck_write(" step ");
            fck_write_int(i);
            fck_write(" is ");
            fck_write(fck_log[i].name ? fck_log[i].name : "(nothing)");
            fck_write(", expected ");
            fck_write(expected[i]);
            fck_write("\n");
            fck_failures++;
            return;
        }
    }
}

/* ------------------------------------------------------- freestanding libc */

/* gcc 2.7.2 sometimes emits calls to these even in freestanding code. They are
 * the compiler's own helpers, not mocked game functions. */
void __main(void) {}

void *memcpy(void *dst, const void *src, unsigned n)
{
    char *d = dst;
    const char *s = src;
    while (n--) *d++ = *s++;
    return dst;
}

void *memset(void *dst, int c, unsigned n)
{
    char *d = dst;
    while (n--) *d++ = (char)c;
    return dst;
}

int memcmp(const void *a, const void *b, unsigned n)
{
    const unsigned char *x = a, *y = b;
    while (n--) {
        if (*x != *y) return (int)*x - (int)*y;
        x++; y++;
    }
    return 0;
}

#endif /* FC_TESTKIT_H */
