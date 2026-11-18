/* Original func_80044B34 (animation track step) driven with the price list of clips.
 * Reads tests/golden/mech-form.vectors (fcop-mech-form-golden/v1): a TABLE directive with the
 * four clip fields (main start/end, companion start/end) and one row per case carrying the
 * transition progress plus the expected scrub frame of both tracks.
 * The type-3 branch of the original is the scrub retail uses during the mech transformation
 * (func_80055988 feeds it the progress s+0xBC). */
typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef unsigned char u8;

struct Clip { u8 next0, type, mode, flags; s16 start, end; u8 next1, next2; unsigned short code; s32 duration; };
struct Track { struct Clip *clips; struct Clip *clip; s32 elapsed; s16 frame, target; signed char step; u8 index, count, flags; };

extern unsigned int func_80044B34(struct Track *script, struct Track *track, s32 speed, s32 direction);
extern void func_800449F8(void *script, struct Track *track);
extern struct Clip *func_800448D8(struct Track *track, s32 index, s32 direction, u8 flags);
extern void func_80044AE8(u32 code, u32 key, s32 *shift, u32 *bits);
extern s32 func_80019668(void);

struct Clip clips[2];
struct Track tracks[2];

void __main(void) {}
void func_800449F8(void *script, struct Track *track) { (void)script; (void)track; }
struct Clip *func_800448D8(struct Track *track, s32 index, s32 direction, u8 flags)
{ (void)track; (void)index; (void)direction; (void)flags; return &clips[0]; }
void func_80044AE8(u32 code, u32 key, s32 *shift, u32 *bits) { (void)code; (void)key; (void)shift; (void)bits; }
s32 func_80019668(void) { return 0; }

void *memset(void *d, int c, unsigned int n)
{
    unsigned char *p = (unsigned char *)d;
    while (n--) *p++ = (unsigned char)c;
    return d;
}
void *memcpy(void *d, const void *s, unsigned int n)
{
    unsigned char *a = (unsigned char *)d;
    const unsigned char *b = (const unsigned char *)s;
    while (n--) *a++ = *b++;
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
static int hex(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}
static void say(const char *s) { int n = 0; while (s[n]) n++; sys3(4004, 1, (long)s, n); }
static void num(s32 v)
{
    char b[12];
    int i = 0, k, neg = v < 0;
    char t;
    if (neg) v = -v;
    if (v == 0) b[i++] = '0';
    while (v > 0) { b[i++] = (char)('0' + (v % 10)); v /= 10; }
    if (neg) b[i++] = '-';
    k = 0;
    while (k < i / 2) { t = b[k]; b[k] = b[i - 1 - k]; b[i - 1 - k] = t; k++; }
    sys3(4004, 1, (long)b, i);
}
static void setClip(struct Clip *c, s16 start, s16 end)
{
    c->next0 = 0; c->type = 3; c->mode = 0; c->flags = 0;
    c->start = start; c->end = end; c->next1 = 0; c->next2 = 0; c->code = 0; c->duration = 10000;
}
static void setTrack(struct Track *t, struct Clip *c)
{
    t->clips = c; t->clip = c; t->elapsed = 0; t->frame = 0; t->target = 0;
    t->step = 1; t->index = 0; t->count = 1; t->flags = 0;
}

int main(int argc, char **argv)
{
    static char data[32768];
    int fd, n, j, h, failures = 0, cases = 0;
    char *p;
    s16 start[2] = {0, 0}, end[2] = {29, 29};
    if (argc != 2) { say("mech-form: missing fixture\n"); return 2; }
    fd = (int)sys3(4005, (long)argv[1], 0, 0);
    if (fd < 0) { say("mech-form: cannot open fixture\n"); return 2; }
    n = (int)sys3(4003, fd, (long)data, sizeof(data) - 1);
    sys3(4006, fd, 0, 0);
    if (n < 0) { say("mech-form: read error\n"); return 2; }
    data[n] = 0;
    p = data;
    while (*p) {
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (!*p) break;
        if (*p == '#') { while (*p && *p != '\n') p++; continue; }
        if (p[0] == 'T' && p[1] == 'A' && p[2] == 'B' && p[3] == 'L' && p[4] == 'E') {
            p += 5;
            for (j = 0; j < 4; j++) {
                u32 w = 0;
                int d;
                while (*p == ' ' || *p == '\t' || *p == '\r') p++;
                if (*p == '\n' || !*p) { say("mech-form: short TABLE\n"); return 2; }
                for (h = 0; h < 8; h++) {
                    d = hex(*p++);
                    if (d < 0) { say("mech-form: malformed TABLE\n"); return 2; }
                    w = (w << 4) | (u32)d;
                }
                if (j == 0) start[0] = (s16)w;
                else if (j == 1) end[0] = (s16)w;
                else if (j == 2) start[1] = (s16)w;
                else end[1] = (s16)w;
            }
            setClip(&clips[0], start[0], end[0]);
            setClip(&clips[1], start[1], end[1]);
            continue;
        }
        {
            u32 w[4];
            for (j = 0; j < 4; j++) {
                u32 v = 0;
                while (*p == ' ' || *p == '\t' || *p == '\r') p++;
                if (*p == '\n' || !*p) break;
                for (h = 0; h < 8; h++) { int d = hex(*p++); if (d < 0) { say("mech-form: malformed row\n"); return 2; } v = (v << 4) | (u32)d; }
                w[j] = v;
            }
            if (j < 4) { say("mech-form: short row\n"); return 2; }
            cases++;
            setTrack(&tracks[0], &clips[0]);
            setTrack(&tracks[1], &clips[1]);
            func_80044B34(0, &tracks[0], (s32)w[1], 0);
            func_80044B34(0, &tracks[1], (s32)w[1], 0);
            if (tracks[0].frame != (s16)w[2] || tracks[1].frame != (s16)w[3]) {
                failures++;
                say("case "); num((s32)w[0]); say(": progress "); num((s32)w[1]);
                say(" main "); num(tracks[0].frame); say(" != "); num((s16)w[2]);
                say(" companion "); num(tracks[1].frame); say(" != "); num((s16)w[3]); say("\n");
            }
        }
    }
    if (failures == 0) { say("ALL PASS (mech-form parity: "); num(cases); say(")\n"); return 0; }
    say("FAILURES "); num(failures); say(" of "); num(cases); say("\n");
    return 1;
}
