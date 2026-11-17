/* Freestanding MIPS host harness: runs the ORIGINAL decomp texture loader
 * (func_800131A0 + func_80012E48 + func_8001306C + func_80012E1C, and
 * func_80063C5C for the remap-table case) against the shared golden vectors
 * tests/golden/3dtl.vectors - the same cases the .NET xUnit suite asserts.
 * No libc: raw o32 syscalls only; run under qemu-mipsel-static. */
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

extern u32 D_8009F528, D_8009F52C, D_8009F530, D_8009F534, D_8009F538;
extern u32 D_8009F53C, D_8009F540, D_8009F544, D_8009F548, D_8009F54C;
extern u32 D_8009F550, D_8009F554, D_8009F558, D_8009F55C, D_8009F560;
extern u32 D_8009F564, D_8009F568;
extern u32 D_800A8E80[10];
extern void func_80012E1C(void *buf);
extern void func_800131A0(void **out, void *data, u32 size, u32 arg3);
extern void func_80063C5C(void *node);

static long sys3(long n, long a, long b, long c)
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
static void out(const char *s, int n) { sys3(4004, 1, (long)s, n); }
static int slen(const char *s) { int n = 0; while (s[n]) n++; return n; }
static void print(const char *s) { out(s, slen(s)); }
static void exit(int code) { sys3(4001, code, 0, 0); }

static char hexc(u8 v) { return v < 10 ? '0' + v : 'a' + v - 10; }
static int hexv(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}
static int unhex(const char *s, u8 *dst, int max)
{
    int n = 0;
    int h, l;
    while (s[0] && s[1] && n < max)
    {
        h = hexv(s[0]); l = hexv(s[1]);
        if (h < 0 || l < 0) break;
        dst[n++] = (u8)((h << 4) | l);
        s += 2;
    }
    return n;
}
static void tohex(const u8 *b, int n, char *dst)
{
    int i;
    for (i = 0; i < n; i++) { dst[i * 2] = hexc(b[i] >> 4); dst[i * 2 + 1] = hexc(b[i] & 15); }
    dst[n * 2] = 0;
}
static u32 rd32(const u8 *p) { return p[0] | (p[1] << 8) | (p[2] << 16) | ((u32)p[3] << 24); }
static void wr32(u8 *p, u32 v)
{
    p[0] = (u8)v; p[1] = (u8)(v >> 8); p[2] = (u8)(v >> 16); p[3] = (u8)(v >> 24);
}
static int streq(const char *a, const char *b)
{
    while (*a && *a == *b) { a++; b++; }
    return *a == *b;
}
static u32 hextoul(const char *s)
{
    u32 v = 0;
    int d;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) s += 2;
    while (*s) { d = hexv(*s); if (d < 0) break; v = (v << 4) | d; s++; }
    return v;
}
static long atool(const char *s)
{
    long v = 0; int neg = 0;
    if (*s == '-') { neg = 1; s++; }
    while (*s >= '0' && *s <= '9') { v = v * 10 + (*s - '0'); s++; }
    return neg ? -v : v;
}

/* ---- fixture ---- */
#define MAXCASES 24
#define MAXREC 16
#define MAXBODY 4096
static struct Case
{
    char id[32], status[24];
    int err, remap_zero, nrec;
    u8 in[MAXBODY]; int in_len;
    u8 expect[MAXBODY]; int expect_len;
    char rec[MAXREC][11][24];
} cases[MAXCASES];
static int ncases;
static u32 remap[10];
static int have_remap;

static void parse_fixture(const char *path)
{
    static char buf[65536];
    int fd, n;
    struct Case *c = 0;
    char *line = buf;
    fd = (int)sys3(4005, (long)path, 0, 0);
    if (fd < 0) { print("cannot open fixture\n"); exit(2); }
    n = (int)sys3(4003, fd, (long)buf, sizeof(buf) - 1);
    sys3(4006, fd, 0, 0);
    if (n < 0) n = 0;
    buf[n] = 0;
    while (line && *line)
    {
        char *nl = line;
        char *t[16];
        char *p;
        int nt = 0;
        while (*nl && *nl != '\n') nl++;
        if (*nl) *nl++ = 0;
        p = line;
        while (*p)
        {
            while (*p == ' ' || *p == '\t' || *p == '\r') p++;
            if (!*p || *p == '#') break;
            t[nt++] = p;
            while (*p && *p != ' ' && *p != '\t' && *p != '\r') p++;
            if (*p) *p++ = 0;
            if (nt == 16) break;
        }
        if (nt > 0)
        {
            if (streq(t[0], "REMAP") && nt >= 11)
            {
                int i;
                for (i = 0; i < 10; i++) remap[i] = hextoul(t[i + 1]);
                have_remap = 1;
            }
            else if (streq(t[0], "CASE") && nt >= 3 && ncases < MAXCASES)
            {
                int i;
                c = &cases[ncases++];
                for (i = 0; t[1][i] && i < 31; i++) c->id[i] = t[1][i];
                c->id[i] = 0;
                for (i = 0; t[2][i] && i < 23; i++) c->status[i] = t[2][i];
                c->status[i] = 0;
                c->err = nt > 3 ? (int)atool(t[3]) : -1;
                c->remap_zero = 0; c->nrec = 0; c->in_len = 0; c->expect_len = 0;
            }
            else if (c && streq(t[0], "REMAP_ZERO")) c->remap_zero = 1;
            else if (c && streq(t[0], "IN")) c->in_len = nt > 1 ? unhex(t[1], c->in, MAXBODY) : 0;
            else if (c && streq(t[0], "OUT")) c->expect_len = nt > 1 ? unhex(t[1], c->expect, MAXBODY) : 0;
            else if (c && streq(t[0], "REC") && nt >= 12 && c->nrec < MAXREC)
            {
                int i, j;
                for (i = 0; i < 11; i++)
                {
                    for (j = 0; t[i + 1][j] && j < 23; j++) c->rec[c->nrec][i][j] = t[i + 1][j];
                    c->rec[c->nrec][i][j] = 0;
                }
                c->nrec++;
            }
            else if (streq(t[0], "END")) c = 0;
        }
        line = nl;
    }
}

/* ---- synthetic Cobj wrapper around a 3DTL body ---- */
static u8 payload[65536];
static int build_payload(const u8 *body, int len)
{
    int p = 0;
    int hdr, i;
    /* 4DGI: 60-byte chunk; header words p1c/p20/p2c = 3DRF indices 1/2/3 */
    wr32(payload + p, 0x34444749); wr32(payload + p + 4, 60); wr32(payload + p + 8, 0);
    hdr = p + 12;
    for (i = 0; i < 48; i++) payload[hdr + i] = 0;
    /* Header struct maps onto the chunk start, not the data area. */
    wr32(payload + p + 0x1C, 1);
    wr32(payload + p + 0x20, 2);
    wr32(payload + p + 0x2C, 3);
    p += 60;
    /* 3DBB: body >= 8 writable bytes (loader writes p34[1]) */
    wr32(payload + p, 0x33444242); wr32(payload + p + 4, 24); wr32(payload + p + 8, 0);
    for (i = 0; i < 12; i++) payload[p + 12 + i] = 0;
    p += 24;
    /* three 3DRF ref blocks, inner tag 0 / count 0 => remap leaves them intact */
    for (i = 1; i <= 3; i++)
    {
        wr32(payload + p, 0x33445246); wr32(payload + p + 4, 20); wr32(payload + p + 8, i);
        wr32(payload + p + 12, 0); wr32(payload + p + 16, 0);
        p += 20;
    }
    /* 3DQL with count 0 */
    wr32(payload + p, 0x3344514C); wr32(payload + p + 4, 16); wr32(payload + p + 8, 1);
    wr32(payload + p + 12, 0);
    p += 16;
    /* 3DTL body under test */
    wr32(payload + p, 0x3344544C); wr32(payload + p + 4, 12 + len); wr32(payload + p + 8, 1);
    for (i = 0; i < len; i++) payload[p + 12 + i] = body[i];
    p += 12 + len;
    return p;
}

static void reset_globals(void)
{
    static u8 tables[0x1000];
    int i;
    for (i = 0; i < 0x1000; i++) tables[i] = 0;
    func_80012E1C(tables);
    D_8009F54C = 0; D_8009F550 = 0; D_8009F564 = 0; D_8009F568 = 0;
}

static int fails;
static void fail(const char *id, const char *what)
{
    print("FAIL "); print(id); print(": "); print(what); print("\n");
    fails++;
}

/* decode records from input (raw) + output (loaded word3), compare to fixture */
static void compare_records(struct Case *c, const u8 *loaded)
{
    int p = 0, nr = 0;
    u8 type;
    int size, i;
    char (*e)[24];
    u32 word3;
    long vals[11];
    long want;
    while (p < c->in_len)
    {
        type = c->in[p];
        size = (type == 0 || type == 1) ? 4 : (type == 2 || type == 3) ? 16 : -1;
        if (size < 0) break;
        if (nr >= c->nrec) { fail(c->id, "record count"); return; }
        e = c->rec[nr];
        word3 = rd32(loaded + p + 12);
        vals[0] = p; vals[1] = size; vals[2] = type;
        vals[3] = (type == 3) ? 2 : type;
        vals[4] = (c->in[p + 1] << 16) | (c->in[p + 2] << 8) | c->in[p + 3];
        vals[5] = size == 16 ? rd32(c->in + p + 4) : 0;
        vals[6] = size == 16 ? rd32(c->in + p + 8) : 0;
        vals[7] = size == 16 ? (long)rd32(c->in + p + 12) : -1;
        vals[8] = size == 16 ? (word3 >> 16) : 0;
        vals[9] = size == 16 ? (word3 & 0xFFFF) : 0;
        vals[10] = (type == 3) ? 1 : -1;
        for (i = 0; i < 11; i++)
        {
            if (i == 10 && streq(e[i], "-")) want = -1;
            else if (i == 4 || i == 5 || i == 6 || i == 8 || i == 9) want = (long)hextoul(e[i]);
            else want = atool(e[i]);
            if (want != vals[i]) { fail(c->id, "record field"); return; }
        }
        nr++;
        p += size;
    }
    if (nr != c->nrec) fail(c->id, "record count");
}

static void run_case(struct Case *c)
{
    struct Node { u32 unk00; u8 pad[0x18]; u32 unk1C; u32 unk20; } node;
    int i, expect_hang, len, before, status;
    long pid;
    void *o;
    u8 *loaded;
    static char hexbuf[MAXBODY * 2 + 1];
    static char wanthex[MAXBODY * 2 + 1];
    if (streq(c->id, "remap_table"))
    {
        /* exercise the retail remap builder func_80063C5C for ids 1..10 */
        func_80063428();
        for (i = 1; i <= 10; i++)
        {
            node.unk00 = 0; node.unk1C = i; node.unk20 = 0;
            func_80063C5C(&node);
        }
        for (i = 0; i < 10; i++)
            if (D_800A8E80[i] != remap[i]) { fail(c->id, "remap entry"); return; }
        print("PASS "); print(c->id); print("\n");
        return;
    }
    if (streq(c->status, "index_out_of_range"))
    {
        print("SKIP "); print(c->id); print(" (retail reads past table; undefined)\n");
        return;
    }
    expect_hang = streq(c->status, "unknown_type");
    len = build_payload(c->in, c->in_len);
    reset_globals();
    for (i = 0; i < 10; i++) D_800A8E80[i] = c->remap_zero ? 0 : remap[i];
    if (expect_hang)
    {
        pid = sys3(4002, 0, 0, 0); /* fork */
        if (pid == 0)
        {
            sys3(4027, 1, 0, 0); /* alarm(1): retail loops forever on type >= 4 */
            func_800131A0(&o, payload, len, 0);
            exit(0);
        }
        status = 0;
        sys3(4007, pid, (long)&status, 0); /* waitpid */
        if ((status & 0x7F) == 14) { print("PASS "); print(c->id); print(" (hang as retail)\n"); }
        else fail(c->id, "expected SIGALRM hang");
        return;
    }
    func_800131A0(&o, payload, len, 0);
    loaded = (u8 *)D_8009F534; /* gModelTriangleList = 3DTL body */
    tohex(loaded, c->in_len, hexbuf);
    tohex(c->expect, c->expect_len, wanthex);
    if (!streq(hexbuf, wanthex)) { fail(c->id, "output hex"); return; }
    before = fails;
    compare_records(c, loaded);
    if (fails != before) return;
    print("PASS "); print(c->id); print("\n");
}

/* ================= render golden vectors (render.vectors) ================= */
/* Pure-C originals run here: func_80015AF4 (winding), func_80015B68 (part prep),
 * func_80016A64 (masked walk), func_800131A0 (flag rewrite, read back from 3DQL).
 * GTE asm units are mirrored by gte_ref.c; .NET asserts the same file. */
typedef short s16;
extern s32 func_80015AF4(s32 o, s32 a, s32 b, s32 *out3);
extern s32 func_80015B68(void *part, void *work, void *arg2, s32 bias, void *tex);
extern void func_80016A64(void *inst, s32 mode, s32 mask);
extern void gte_load(const s32 *w, const s32 *t, s16 *r, s32 *tr);
extern void gte_mvmva(const s16 *r, s16 vx, s16 vy, s16 vz, s16 *out, u32 *flag);
extern s32 gte_quot(s32 h, s32 sz3, s32 *overflow);
extern void gte_rtps(const s16 *r, const s32 *tr, s16 vx, s16 vy, s16 vz,
                     s32 h, s32 ofx, s32 ofy, s32 wmode,
                     s32 *sxy, s32 *sz3out, u32 *clip, u32 *flag, s32 *written);
extern u32 D_8009F570, D_8009F574, D_8009F578, D_8009F57C, D_8009F580;
extern u32 D_8009F9E4, D_800A8EA4, D_8009FA0C;
extern u32 rec_count, rec_type[64], rec_xy[64 * 4], overlay_count;
extern s32 rec_z[64];

#define RMAX 64
#define RLMAX 48
static char rkind[RMAX][8];
static char rid[RMAX][32];
static char rtok[RMAX][8][RLMAX][24];
static int rn[RMAX];
static int nrk;

static void parse_render(const char *path)
{
    static char buf[65536];
    char lt[RLMAX][24];
    int fd, n, i, j, cur, nt;
    char *p, *line, *t, *s;
    fd = sys3(4005, (long)path, 0, 0);
    if (fd < 0) { print("cannot open render vectors\n"); exit(2); }
    n = sys3(4003, fd, (long)buf, sizeof(buf) - 1);
    sys3(4006, fd, 0, 0);
    if (n < 0) n = 0;
    buf[n] = 0;
    cur = -1;
    p = buf;
    while (*p)
    {
        line = p;
        while (*p && *p != '\n') p++;
        if (*p) *p++ = 0;
        if (*line == '#' || *line == 0) continue;
        nt = 0;
        t = line;
        while (*t)
        {
            while (*t == ' ' || *t == '\t') t++;
            if (!*t) break;
            s = t;
            while (*t && *t != ' ' && *t != '\t') t++;
            if (*t) *t++ = 0;
            if (nt < RLMAX)
            {
                for (i = 0; s[i] && i < 23; i++) lt[nt][i] = s[i];
                lt[nt][i] = 0;
            }
            nt++;
        }
        if (nt == 0) continue;
        if (streq(lt[0], "END")) { cur = -1; continue; }
        if (cur < 0)
        {
            if (nrk >= RMAX) break;
            cur = nrk++;
            rn[cur] = 0;
            for (i = 0; i < 7 && lt[0][i]; i++) rkind[cur][i] = lt[0][i];
            rkind[cur][i] = 0;
            for (i = 0; i < 31 && lt[1][i]; i++) rid[cur][i] = lt[1][i];
            rid[cur][i] = 0;
        }
        if (rn[cur] < 8)
        {
            for (j = 0; j < nt && j < RLMAX; j++)
                for (i = 0; i < 24; i++) rtok[cur][rn[cur]][j][i] = lt[j][i];
            rn[cur]++;
        }
    }
}

static char *RT(int c, int l, int t) { return rtok[c][l][t]; }

static void rfail(int c, const char *what) { fail(rid[c], what); }

static void expect_hex(int c, const char *what, u32 got, int l, int t)
{
    if (got != hextoul(RT(c, l, t))) rfail(c, what);
}
static void expect_dec(int c, const char *what, long got, int l, int t)
{
    if (got != atool(RT(c, l, t))) rfail(c, what);
}

/* synthetic Cobj payload carrying a 3DQL list; returns total length */
static int build_ql_payload(int np, const u8 *f0, const u8 *f1, const u8 *idx)
{
    int p = 0, i, k;
    wr32(payload + p, 0x34444749); wr32(payload + p + 4, 60); wr32(payload + p + 8, 0);
    for (i = 0; i < 48; i++) payload[p + 12 + i] = 0;
    wr32(payload + p + 0x1C, 1); wr32(payload + p + 0x20, 2); wr32(payload + p + 0x2C, 3);
    p += 60;
    wr32(payload + p, 0x33444242); wr32(payload + p + 4, 24); wr32(payload + p + 8, 0);
    for (i = 0; i < 12; i++) payload[p + 12 + i] = 0;
    p += 24;
    for (i = 1; i <= 3; i++)
    {
        wr32(payload + p, 0x33445246); wr32(payload + p + 4, 20); wr32(payload + p + 8, i);
        wr32(payload + p + 12, 0); wr32(payload + p + 16, 0);
        p += 20;
    }
    wr32(payload + p, 0x3344514C); wr32(payload + p + 4, 16 + 12 * np); wr32(payload + p + 8, 1);
    wr32(payload + p + 12, np);
    for (i = 0; i < np; i++)
    {
        u8 *q = payload + p + 16 + i * 12;
        q[0] = f0[i]; q[1] = f1[i]; q[2] = 0; q[3] = 0;
        for (k = 0; k < 4; k++) q[4 + k] = idx[i * 4 + k];
        for (k = 0; k < 4; k++) q[8 + k] = 0;
    }
    p += 16 + 12 * np;
    /* empty 3DTL: keeps gModelTriangleList/Size fresh (no stale walk) */
    wr32(payload + p, 0x3344544C); wr32(payload + p + 4, 12); wr32(payload + p + 8, 1);
    p += 12;
    return p;
}

static u8 *load_ql(int np, const u8 *f0, const u8 *f1, const u8 *idx)
{
    void *o;
    int len = build_ql_payload(np, f0, f1, idx);
    reset_globals();
    func_800131A0(&o, payload, len, 0);
    return (u8 *)D_8009F530; /* gModelPolygonList */
}

struct SV { u32 xy; s32 z; };
struct W17 { u32 xy[4]; s32 z; u32 tag; };
struct P17 { u32 flags; u32 a; u32 b; };
struct A2 { s32 unk0; s16 data[4]; };

static void fill_verts(int c, int o, int nv, struct SV *sv, u8 *clip)
{
    int i;
    for (i = 0; i < nv; i++)
    {
        sv[i].xy = hextoul(RT(c, 0, o + i * 3));
        sv[i].z = atool(RT(c, 0, o + i * 3 + 1));
        clip[i] = (u8)hextoul(RT(c, 0, o + i * 3 + 2));
    }
    D_8009F578 = (u32)sv;
    D_8009F57C = (u32)clip;
}

static void run_render(int c)
{
    s32 w[9], t[3], tr[3], v[3];
    s16 r[9], mv[3];
    u32 flag, clip, o3[3];
    s32 sxy, sz3, written, ovf, q, ret, i, nv, np, bias, mask;
    struct SV sv[16];
    u8 cb[16];
    struct A2 widths;
    struct W17 work;
    struct P17 part;
    u8 f0[16], f1[16], idx[64];
    u8 *ql;
    static u8 model[48];
    static u8 inst[76];
    static void *p2c[2];
    int before = fails;
    const char *k = rkind[c];
    if (streq(k, "ML"))
    {
        for (i = 0; i < 9; i++) w[i] = (s32)hextoul(RT(c, 0, 2 + i));
        for (i = 0; i < 3; i++) t[i] = (s32)hextoul(RT(c, 0, 11 + i));
        gte_load(w, t, r, tr);
        for (i = 0; i < 9; i++) expect_hex(c, "load R", (u32)(u16)r[i], 1, 1 + i);
        for (i = 0; i < 3; i++) expect_dec(c, "load TR", tr[i], 1, 10 + i);
    }
    else if (streq(k, "MV"))
    {
        for (i = 0; i < 9; i++) r[i] = (s16)hextoul(RT(c, 0, 2 + i));
        for (i = 0; i < 3; i++) v[i] = atool(RT(c, 0, 11 + i));
        gte_mvmva(r, (s16)v[0], (s16)v[1], (s16)v[2], mv, &flag);
        for (i = 0; i < 3; i++) expect_dec(c, "mvmva", mv[i], 1, 1 + i);
        expect_hex(c, "mvmva flag", flag, 1, 4);
    }
    else if (streq(k, "Q"))
    {
        q = gte_quot((s32)hextoul(RT(c, 0, 2)), (s32)hextoul(RT(c, 0, 3)), &ovf);
        expect_hex(c, "quot", (u32)q, 1, 1);
        expect_dec(c, "quot ovf", ovf, 1, 2);
    }
    else if (streq(k, "RP"))
    {
        for (i = 0; i < 9; i++) r[i] = (s16)hextoul(RT(c, 0, 2 + i));
        for (i = 0; i < 3; i++) tr[i] = atool(RT(c, 0, 11 + i));
        for (i = 0; i < 3; i++) v[i] = atool(RT(c, 0, 14 + i));
        gte_rtps(r, tr, (s16)v[0], (s16)v[1], (s16)v[2],
                 (s32)hextoul(RT(c, 0, 17)), (s32)hextoul(RT(c, 0, 18)),
                 (s32)hextoul(RT(c, 0, 19)), atool(RT(c, 0, 20)),
                 &sxy, &sz3, &clip, &flag, &written);
        expect_hex(c, "rtps sxy", (u32)sxy, 1, 1);
        expect_hex(c, "rtps sz", (u32)sz3, 1, 2);
        expect_hex(c, "rtps clip", clip, 1, 3);
        expect_hex(c, "rtps flag", flag, 1, 4);
        expect_dec(c, "rtps written", written, 1, 5);
    }
    else if (streq(k, "W"))
    {
        ret = func_80015AF4(hextoul(RT(c, 0, 2)), hextoul(RT(c, 0, 3)), hextoul(RT(c, 0, 4)), o3);
        expect_dec(c, "winding", ret, 1, 1);
        if (!ret)
            for (i = 0; i < 3; i++) expect_hex(c, "winding out", o3[i], 1, 2 + i);
    }
    else if (streq(k, "P"))
    {
        nv = atool(RT(c, 0, 8));
        fill_verts(c, 9, nv, sv, cb);
        for (i = 0; i < 4; i++) widths.data[i] = (s16)hextoul(RT(c, 0, 9 + nv * 3 + i));
        widths.unk0 = 0;
        part.flags = (u32)atool(RT(c, 0, 2)) << 8;
        part.a = (u32)atool(RT(c, 0, 3)) | ((u32)atool(RT(c, 0, 4)) << 8) |
                 ((u32)atool(RT(c, 0, 5)) << 16) | ((u32)atool(RT(c, 0, 6)) << 24);
        part.b = 0;
        bias = atool(RT(c, 0, 7));
        for (i = 0; i < 24; i++) ((u8 *)&work)[i] = 0; /* xy3 unused for tris */
        ret = func_80015B68(&part, &work, &widths, bias, 0);
        expect_dec(c, "prep accept", ret, 1, 1);
        if (ret)
        {
            for (i = 0; i < 4; i++) expect_hex(c, "prep xy", work.xy[i], 1, 2 + i);
            expect_dec(c, "prep z", work.z, 1, 6);
        }
    }
    else if (streq(k, "K"))
    {
        np = atool(RT(c, 0, 4));
        for (i = 0; i < np; i++)
        {
            f0[i] = (u8)hextoul(RT(c, 0, 5 + i * 6));
            f1[i] = (u8)hextoul(RT(c, 0, 6 + i * 6));
            idx[i * 4] = (u8)atool(RT(c, 0, 7 + i * 6));
            idx[i * 4 + 1] = (u8)atool(RT(c, 0, 8 + i * 6));
            idx[i * 4 + 2] = (u8)atool(RT(c, 0, 9 + i * 6));
            idx[i * 4 + 3] = (u8)atool(RT(c, 0, 10 + i * 6));
        }
        ql = load_ql(np, f0, f1, idx);
        nv = atool(RT(c, 0, 5 + np * 6));
        fill_verts(c, 6 + np * 6, nv, sv, cb);
        for (i = 0; i < 4; i++) widths.data[i] = (s16)hextoul(RT(c, 0, 6 + np * 6 + nv * 3 + i));
        widths.unk0 = 0;
        for (i = 0; i < 48; i++) model[i] = 0;
        *(u32 *)(model + 36) = (u32)ql;          /* parts: count word + records */
        *(u32 *)(model + 40) = 0;               /* texture base */
        p2c[0] = 0; p2c[1] = &widths;
        *(u32 *)(model + 44) = (u32)p2c;
        for (i = 0; i < 76; i++) inst[i] = 0;
        *(u32 *)(inst + 0) = (u32)model;
        *(s16 *)(inst + 72) = 0;                /* frame index */
        inst[74] = 8;                           /* no depth bias */
        D_8009F580 = 0;
        for (i = 0; i < 24; i++) ((u8 *)&work)[i] = 0;
        rec_count = 0; overlay_count = 0;
        mask = (s32)hextoul(RT(c, 0, 2));
        func_80016A64(inst, 0, mask);
        expect_dec(c, "walk count", (long)rec_count, 1, 1);
        for (i = 0; i < (int)rec_count; i++)
        {
            expect_dec(c, "face type", rec_type[i], 2 + i, 1);
            expect_hex(c, "face xy0", rec_xy[i * 4], 2 + i, 2);
            expect_hex(c, "face xy1", rec_xy[i * 4 + 1], 2 + i, 3);
            expect_hex(c, "face xy2", rec_xy[i * 4 + 2], 2 + i, 4);
            if (!streq(RT(c, 2 + i, 5), "-"))
                expect_hex(c, "face xy3", rec_xy[i * 4 + 3], 2 + i, 5);
            expect_dec(c, "face z", rec_z[i], 2 + i, 6);
        }
    }
    else if (streq(k, "FR"))
    {
        np = 0;
        for (i = 2; RT(c, 0, i)[0]; i++) np++;
        for (i = 0; i < np; i++)
        {
            f0[i] = (u8)hextoul(RT(c, 0, 2 + i));
            f1[i] = 0;
            idx[i * 4] = idx[i * 4 + 1] = idx[i * 4 + 2] = idx[i * 4 + 3] = 0;
        }
        ql = load_ql(np, f0, f1, idx);
        for (i = 0; i < np; i++)
        {
            /* pair token "in:out" */
            char *tok = RT(c, 0, 2 + i);
            u32 want = 0;
            int j = 0;
            while (tok[j] && tok[j] != ':') j++;
            want = hextoul(tok + j + 1);
            if (ql[4 + i * 12] != (u8)want) { rfail(c, "flag rewrite"); break; }
        }
    }
    else { rfail(c, "unknown kind"); return; }
    if (fails != before) return;
    print("PASS "); print(rid[c]); print("\n");
}

int main(int argc, char **argv)
{
    int i;
    if (argc < 2) { print("usage: harness <3dtl.vectors> [render.vectors]\n"); return 2; }
    parse_fixture(argv[1]);
    if (!have_remap) { print("fixture missing REMAP line\n"); return 2; }
    for (i = 0; i < ncases; i++) run_case(&cases[i]);
    if (argc > 2)
    {
        parse_render(argv[2]);
        for (i = 0; i < nrk; i++) run_render(i);
    }
    print(fails ? "FAILED\n" : "ALL PASS\n");
    return fails ? 1 : 0;
}