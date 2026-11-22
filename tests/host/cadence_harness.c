/* Host harness for the weapon trigger logic (cadence).
 *
 * It links the UNCHANGED decomp sources:
 *   8006C3E0.c - the per-frame reload timer decrement of the player entity tick,
 *   8006BA50.c - the slot update that fires when the timer reached zero and then reloads it
 *                from entry +0x06 / +0x08 / +0x0A.
 *
 * func_8006AAEC (registry lookup), func_8006ACB0 (fire dispatcher) and func_80046C84 (sound) are
 * replaced by stubs, so the harness sees exactly the frames in which retail would have fired. The
 * expected frames come from tests/golden/cadence.vectors, which was derived from the C text.
 */
typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

struct Outer { u8 pad00[0xAC]; void *inner; };

/* Unk8006C3E0Inner: timers at +0x78, target pointer at +0x6C (kept null here). */
struct Inner { u8 pad00[0x68]; s32 unk68; void *unk6C; u8 pad70[8]; s32 unk78[3]; };

/* Unk6BA50Ctl: the block the slot update reads. */
struct Ctl {
    u8 pad00[0x78]; s32 unk78[3]; s16 unk84[3]; s16 unk8A[3];
    u8 unk90[3]; u8 unk93[3]; u8 unk96[2]; void *unk98; s32 unk9C; u8 unkA0[0x21]; signed char unkC1;
};

struct Obj { u8 pad00[0xAC]; struct Ctl *ctl; };

/* Unk6BA50Tgt: the definition record the registry hands back. */
struct Tgt { u8 pad00[0x0A]; u8 unk0A; u8 pad0B[0x39]; s32 unk44; u8 pad48[8]; s32 unk50; };

extern void func_8006C3E0(struct Outer *outer, s32 delta);
/* The decomp names it gWeaponSlotUpdate and gives it the retail symbol func_8006BA50. */
extern void func_8006BA50(struct Obj *obj, s32 idx);

void *D_8009FA40 = 0;
u8 D_8009FA8C[8];

static struct Tgt record;
static s32 fired;

/* Stubs: the registry always returns the synthetic record, the dispatcher records the shot. */
struct Tgt *func_8006AAEC(u8 id) { (void)id; return &record; }
s32 func_8006ACB0(struct Obj *obj, s32 idx) { (void)obj; (void)idx; fired = 1; return 1; }
void func_8006B23C(struct Obj *obj, s32 idx, s32 arg2) { (void)obj; (void)idx; (void)arg2; }
s32 func_80046C84(s32 id, struct Obj *obj) { (void)id; (void)obj; return 0; }
s32 func_8004E5F0(struct Outer *a, void *b, s32 c, s32 d)
{ (void)a; (void)b; (void)c; (void)d; return 0; }

void __main(void) { }
void *memcpy(void *d, const void *s, unsigned int n)
{
    unsigned char *dd = d; const unsigned char *ss = s;
    while (n--) *dd++ = *ss++;
    return d;
}
void *memset(void *d, int c, unsigned int n)
{
    unsigned char *dd = d;
    while (n--) *dd++ = (unsigned char)c;
    return d;
}

static long sys3(long n, long a, long b, long c)
{
    register long v0 __asm__("$2") = n;
    register long a0 __asm__("$4") = a;
    register long a1 __asm__("$5") = b;
    register long a2 __asm__("$6") = c;
    __asm__ volatile("syscall" : "+r"(v0) : "r"(a0), "r"(a1), "r"(a2)
                     : "memory", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25");
    return v0;
}
static void say(const char *s) { int n = 0; while (s[n]) n++; sys3(4004, 1, (long)s, n); }
static void num(s32 v)
{
    char b[12]; int i = 0, k, neg = v < 0; char t;
    if (neg) v = -v;
    if (v == 0) b[i++] = '0';
    while (v > 0) { b[i++] = (char)('0' + (v % 10)); v /= 10; }
    if (neg) b[i++] = '-';
    k = 0;
    while (k < i / 2) { t = b[k]; b[k] = b[i - 1 - k]; b[i - 1 - k] = t; k++; }
    sys3(4004, 1, (long)b, i);
}

static char data[16384];
static char namebuf[64];
static s32 expect[128];
static s32 hits[128];

static int load(const char *path)
{
    int fd = (int)sys3(4005, (long)path, 0, 0);
    int n;
    if (fd < 0) return -1;
    n = (int)sys3(4003, fd, (long)data, sizeof(data) - 1);
    sys3(4006, fd, 0, 0);
    if (n < 0) return -1;
    data[n] = 0;
    return 0;
}

static char *p;

static void skip_space(void)
{
    for (;;) {
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p != '#') return;
        while (*p && *p != '\n') p++;
    }
}

static int next_int(s32 *out)
{
    s32 v = 0, neg = 0;
    skip_space();
    if (!*p) return 0;
    if (*p == '-') { neg = 1; p++; }
    while (*p >= '0' && *p <= '9') { v = v * 10 + (*p - '0'); p++; }
    *out = neg ? -v : v;
    return 1;
}

static int next_name(void)
{
    int i = 0;
    skip_space();
    if (!*p) return 0;
    while (*p && *p != ' ' && *p != '\t' && *p != '\n' && i < 63) namebuf[i++] = *p++;
    namebuf[i] = 0;
    return 1;
}

int main(int argc, char **argv)
{
    s32 cases = 0, failures = 0;
    if (argc != 2) { say("cadence: usage <vectors>\n"); return 2; }
    if (load(argv[1]) != 0) { say("cadence: cannot open fixture\n"); return 2; }
    p = data;
    while (next_name()) {
        s32 burst, reload_ticks, gap, cycle, delta, frames, count, i, n;
        struct Outer outer;
        struct Inner inner;
        struct Obj obj;
        struct Ctl ctl;
        if (!next_int(&burst) || !next_int(&reload_ticks) || !next_int(&gap) ||
            !next_int(&cycle) || !next_int(&delta) || !next_int(&frames) || !next_int(&count)) {
            say("cadence: short row\n");
            return 2;
        }
        if (count > 128 || frames > 4096) { say("cadence: row too large\n"); return 2; }
        for (i = 0; i < count; i++) {
            if (!next_int(&expect[i])) { say("cadence: short expectation list\n"); return 2; }
        }

        memset(&record, 0, sizeof(record));
        record.unk0A = (u8)cycle;
        /* Entry +0x04: type at +0x00, burst at +0x06, reload at +0x08, gap at +0x0A. */
        *(u8 *)((u8 *)&record + 0x04 + 0x00) = 3;
        *(u8 *)((u8 *)&record + 0x04 + 0x06) = (u8)burst;
        *(s16 *)((u8 *)&record + 0x04 + 0x08) = (s16)reload_ticks;
        *(s16 *)((u8 *)&record + 0x04 + 0x0A) = (s16)gap;

        memset(&ctl, 0, sizeof(ctl));
        memset(&inner, 0, sizeof(inner));
        memset(&outer, 0, sizeof(outer));
        for (i = 0; i < 3; i++) { ctl.unk78[i] = 0; ctl.unk84[i] = 100; ctl.unk93[i] = (u8)cycle; }
        outer.inner = &inner;
        obj.ctl = &ctl;

        cases++;
        n = 0;
        for (i = 0; i < frames; i++) {
            fired = 0;
            /* Retail order in func_800588D4: timers first, then the slot update. */
            func_8006C3E0(&outer, delta);
            ctl.unk78[0] = inner.unk78[0];
            func_8006BA50(&obj, 0);
            inner.unk78[0] = ctl.unk78[0];
            if (fired && n < 128) hits[n++] = i;
        }

        if (n != count) {
            failures++;
            say(namebuf); say(": "); num(n); say(" shots, expected "); num(count); say("\n");
            continue;
        }
        for (i = 0; i < count; i++) {
            if (hits[i] != expect[i]) {
                failures++;
                say(namebuf); say(": shot "); num(i); say(" at frame "); num(hits[i]);
                say(" expected "); num(expect[i]); say("\n");
                break;
            }
        }
    }
    if (failures == 0) {
        say("ALL PASS (cadence parity: "); num(cases); say(")\n");
        return 0;
    }
    say("FAILURES "); num(failures); say(" of "); num(cases); say(" cases\n");
    return 1;
}