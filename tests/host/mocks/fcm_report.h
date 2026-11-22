/* Minimal loud-failure reporting for the shared mocks.
 *
 * A mock lives in its own translation unit, so it cannot use fc_testkit.h (that
 * header defines __main/memcpy/memset as external symbols; including it in two
 * objects would give the link two definitions of each). It still has to be able
 * to shout when it is asked for something it cannot honestly supply, so the
 * few freestanding primitives it needs are `static` here: each mock object gets
 * its own private copy and nothing collides at link time.
 */
#ifndef FCM_REPORT_H
#define FCM_REPORT_H

#ifndef FCM_TYPES_INCLUDED
#error "include fcm_types.h before fcm_report.h"
#endif

/* write(2) through the syscall gate - no libc, these harnesses are freestanding. */
static long fcm_sys3(long n, long a, long b, long c)
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

static void fcm_say(const char *text)
{
    int n = 0;
    while (text[n]) n++;
    fcm_sys3(4004, 2 /* stderr */, (long)text, n);
}

static void fcm_say_int(s32 value)
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
    fcm_say(&buf[i]);
}

/* Terminate the run at once. Used for "this mock cannot answer that", which is
 * a test bug or a missing mock feature - never something to paper over with a
 * plausible return value. exit(2) so it cannot be confused with a check failure
 * (1) or with success (0). */
static void fcm_say_hex(u32 v)
{
    char b[11];
    int i;
    b[0] = '0';
    b[1] = 'x';
    for (i = 0; i < 8; i++) {
        u32 d = (v >> (28 - 4 * i)) & 0xF;
        b[2 + i] = (char)(d < 10 ? '0' + d : 'a' + (d - 10));
    }
    b[10] = 0;
    fcm_say(b);
}

static void fcm_abort(const char *which, const char *why)
{
    fcm_say("FAIL: mock ");
    fcm_say(which);
    fcm_say(": ");
    fcm_say(why);
    fcm_say("\n");
    fcm_sys3(4001, 2, 0, 0);
    for (;;) { }   /* exit(2) does not return; keep the compiler honest. */
}

#endif /* FCM_REPORT_H */
