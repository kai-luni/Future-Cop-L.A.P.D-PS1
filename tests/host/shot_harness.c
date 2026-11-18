/* Host harness for the projectile spawn composition and the position integrator.
 *
 * It links the UNCHANGED decomp sources:
 *   8005E47C.c  - the kind-2 direct-fire spawner of the weapon dispatchers (func_8006ACB0 calls it
 *                 for actions 1/2/3/0x12/0x13/0x15),
 *   8001ACA4.c  - the 16.16 vector integrator the entity mover func_80035B7C steps the position with.
 *
 * func_8004D444 (the spawn queue) is replaced by a recorder, so the block func_8005E47C builds can be
 * compared word by word with tests/golden/shot.vectors. The struct layouts below are copies of the
 * ones inside the two C files - the harness is the only place that knows how to read them.
 */
typedef int s32;
typedef unsigned int u32;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

struct Vec { s32 x, y, z; };

struct Obj {
    u16 unk00; s16 unk02; u16 unk04; u16 unk06; s16 unk08;
    s16 unk0A; s16 unk0C; u8 unk0E; u8 pad[3]; u8 unk12;
};

struct Ent {
    u8 pad[0x14]; u16 unk14; u8 unk16, unk17, unk18;
    u8 pad19[0xB]; u32 unk24;
};

struct Args {
    u32 flags; u8 kind; u8 pad05[3]; struct Vec position; u32 unk14;
    u16 unk18, unk1A; u8 unk1C, unk1D, unk1E, unk1F;
    u8 unk20, unk21, unk22, unk23; void *unk24; void *unk28;
    s32 unk2C, unk30; void *unk34; struct Ent *unk38;
    s32 unk3C, unk40, unk44; s16 unk48, unk4A;
    u8 unk4C, unk4D; u16 unk4E; u8 unk50, unk51; u8 pad52[2]; s32 unk54; u8 pad58[8];
};

struct Request { void (*handler)(); s32 unk04; struct Args *args; s32 unk0C; };

extern void *func_8005E47C(struct Obj *obj, void *a1, struct Vec *pos, void *a3,
                           struct Ent *ent, void *a5, u32 flags, u8 a7, u8 a8,
                           u16 a9, u8 a10, s32 a11);
extern void func_8001ACA4(s32 *a, s32 *b, s32 scale, s32 *out);

/* The spawner loads this address into request.handler; the harness never calls it. */
void D_80036624(void) { }

static struct Args recorded;
static s32 recorded_flag;

void *func_8004D444(struct Request *request)
{
    recorded = *request->args;
    recorded_flag = 1;
    return 0;
}

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
static int hex(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

static char data[32768];

/* Reads the next 32-bit hex word, skipping whitespace and comments. Returns 0 at end of file. */
static int next_word(char **pp, u32 *out)
{
    char *p = *pp;
    for (;;) {
        while (*p == ' ' || *p == '\t' || *p == '\r' || *p == '\n') p++;
        if (*p != '#') break;
        while (*p && *p != '\n') p++;
    }
    if (!*p) { *pp = p; return 0; }
    {
        u32 v = 0;
        int h;
        while ((h = hex(*p)) >= 0) { v = (v << 4) | (u32)h; p++; }
        *pp = p;
        *out = v;
    }
    return 1;
}

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

static void report(const char *name, int cases, int failures)
{
    if (failures == 0) {
        say("ALL PASS ("); say(name); say(" parity: "); num(cases); say(")\n");
    } else {
        say("FAILURES "); num(failures); say(" of "); num(cases); say(" ("); say(name); say(")\n");
    }
}

static int run_spawn(const char *path)
{
    char *p = data;
    int cases = 0, failures = 0;
    u32 w[39];
    if (load(path) != 0) { say("shot: cannot open spawn fixture\n"); return 2; }
    while (next_word(&p, &w[0])) {
        int i;
        struct Obj obj;
        struct Ent ent;
        struct Vec pos;
        void *owner = &recorded;
        for (i = 1; i < 39; i++)
            if (!next_word(&p, &w[i])) { say("shot: short row\n"); return 2; }

        memset(&obj, 0, sizeof(obj));
        memset(&ent, 0, sizeof(ent));
        obj.unk06 = (u16)w[1];      /* speed word   */
        obj.unk08 = (s16)w[2];      /* alt selector */
        obj.unk00 = (u16)w[3];
        obj.unk0A = (s16)w[4];      /* range word   */
        obj.unk02 = (s16)w[5];
        obj.unk0E = (u8)w[6];
        obj.unk12 = (u8)w[7];
        ent.unk14 = (u16)w[8];
        ent.unk16 = (u8)w[9];
        ent.unk17 = (u8)w[10];
        ent.unk18 = (u8)w[11];
        ent.unk24 = w[12];
        pos.x = (s32)w[13];
        pos.y = (s32)w[14];
        pos.z = (s32)w[15];

        recorded_flag = 0;
        func_8005E47C(&obj, &pos, &pos, &pos, &ent, w[22] ? owner : 0, w[16],
                      (u8)w[17], (u8)w[18], (u16)w[19], (u8)w[20], (s32)w[21]);
        cases++;
        if (!recorded_flag) {
            failures++;
            say("case "); num((s32)w[0]); say(": spawner did not call the queue\n");
            continue;
        }
        {
            u32 got[16];
            u32 exp[16];
            got[0] = recorded.flags;
            got[1] = recorded.unk14;
            got[2] = recorded.unk18;
            got[3] = recorded.unk1A;
            got[4] = (u32)recorded.unk2C;
            got[5] = (u32)recorded.unk3C;
            got[6] = (u32)recorded.unk44;
            got[7] = (u32)recorded.unk40;
            got[8] = (u32)(u16)recorded.unk48;
            got[9] = recorded.unk4A;
            got[10] = recorded.unk4C;
            got[11] = recorded.unk4D;
            got[12] = recorded.unk4E;
            got[13] = recorded.unk50;
            got[14] = recorded.unk51;
            got[15] = (u32)recorded.unk54;
            for (i = 0; i < 16; i++) exp[i] = w[23 + i];
            for (i = 0; i < 16; i++) {
                if (got[i] != exp[i]) {
                    failures++;
                    say("case "); num((s32)w[0]); say(": field "); num(16 + i);
                    say(" got "); num((s32)got[i]); say(" expected "); num((s32)exp[i]); say("\n");
                }
            }
            if (recorded.position.x != pos.x || recorded.position.y != pos.y ||
                recorded.position.z != pos.z) {
                failures++;
                say("case "); num((s32)w[0]); say(": position not copied\n");
            }
        }
    }
    report("shot-spawn", cases, failures);
    return failures == 0 ? 0 : 1;
}

static int run_move(const char *path)
{
    char *p = data;
    int cases = 0, failures = 0;
    u32 w[11];
    if (load(path) != 0) { say("shot: cannot open move fixture\n"); return 2; }
    while (next_word(&p, &w[0])) {
        int i;
        s32 a[3], b[3];
        for (i = 1; i < 11; i++)
            if (!next_word(&p, &w[i])) { say("shot: short move row\n"); return 2; }
        a[0] = (s32)w[1]; a[1] = (s32)w[2]; a[2] = (s32)w[3];
        b[0] = (s32)w[4]; b[1] = (s32)w[5]; b[2] = (s32)w[6];
        /* Retail calls it in place: func_8001ACA4(position, velocity, s3, position). */
        func_8001ACA4(a, b, (s32)w[7], a);
        cases++;
        for (i = 0; i < 3; i++) {
            if ((u32)a[i] != w[8 + i]) {
                failures++;
                say("move case "); num((s32)w[0]); say(": component "); num(i);
                say(" got "); num(a[i]); say(" expected "); num((s32)w[8 + i]); say("\n");
            }
        }
    }
    report("shot-move", cases, failures);
    return failures == 0 ? 0 : 1;
}

int main(int argc, char **argv)
{
    int rc = 0;
    if (argc != 3) { say("shot: usage <spawn-vectors> <move-vectors>\n"); return 2; }
    rc |= run_spawn(argv[1]);
    rc |= run_move(argv[2]);
    return rc;
}
