/* Host harness: single-player Precinct Assault front-end state parity.
 *
 * Runs the unchanged retail units of the Precinct Assault front-end path
 * (docs/precinct-assault-path.md) against the vectors shared with the .NET
 * port (future_cop_dotnet/tests/golden/precinct-assault.vectors):
 *
 *   func_800776B8 gSetupCrimeWarMissionList
 *   func_80077708 gSetupPrecinctAssaultMissionList
 *   func_80077FFC gUpdatePrecinctUnlockFlag
 *   func_8007A4CC gSelectPrecinctAssault1P
 *   func_8007A56C gSelectPrecinctAssault2P
 *   func_8007A5F0 gHandlePlayerJoinSelection
 *   func_8007B004 gInitMissionSelectState
 *   func_8007B854 gHandleProfileListItem
 *   func_8007D39C gFindListEntryByValue
 *   func_8007D408 gInitSelectionIndices
 *   func_8007D2DC gClampLoadoutSelections
 *   func_8007E3EC gFinalizeMissionLoadout
 *
 * Memory model: qemu-mipsel cannot map guest addresses at or above
 * 0x80000000, so the retail RAM window 0x80010000..0x800A4000 lives in the
 * section .retailram linked at 0x10010000 (retail address - 0x70000000).
 * precinct_assault_test.sh defines every D_8xxxxxxx symbol the units use at
 * its relocated address. At start-up the harness copies the text/data image
 * of the retail executable (SLUS_007.39, argv[2]) into that window, so every
 * table (mission records D_8009B4E4, lists D_8009B724/D_8009B72C, loadout
 * tables D_8009C030..D_8009C0A8, the "lax1"/"lax2" strings) is the retail
 * bytes, not a transcription. Pointer fields inside the tables that the units
 * dereference are relocated by the same -0x70000000 offset. The window above
 * the image (0x8009F800..) is retail BSS and starts zeroed.
 *
 * Stubs (not part of the parity claim): func_80078650 (menu push) only
 * records its argument; func_80046C84 (sound) records the id;
 * func_80078B34/func_8007B460 (drawing) count calls; func_8008CCA0 and
 * func_8008CCB0 are the BIOS A(0x17) strcmp and A(0x19) strcpy thunks
 * (asm/us/main.s), implemented here with the same semantics.
 *
 * Vector line: name ops key=value ... | key=value ...
 *   ops     comma separated, each fn[:arg[:arg]] (see run_op)
 *   left    inputs applied after the per-case reset
 *   right   expected outputs; "?" means "not yet known" and is filled in by
 *           --emit, which prints the line with the observed values.
 * Usage: harness VECTORS SLUS [--emit]
 */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

#define RETAIL_BASE 0x80010000u
#define RELOC 0x70000000u
#define RAM_SIZE 0x94000
#define IMAGE_MAX 0x8F800

extern u8 gRetailRam[] __asm__("D_RAM");

extern void gSetupCrimeWarMissionList(void) __asm__("func_800776B8");
extern void gSetupPrecinctAssaultMissionList(void) __asm__("func_80077708");
extern void gUpdatePrecinctUnlockFlag(void) __asm__("func_80077FFC");
extern void gSelectPrecinctAssault1P(s32) __asm__("func_8007A4CC");
extern void gSelectPrecinctAssault2P(s32) __asm__("func_8007A56C");
extern void gHandlePlayerJoinSelection(s32, s32, s32, s32, s32) __asm__("func_8007A5F0");
extern void gInitMissionSelectState(void) __asm__("func_8007B004");
extern void gHandleProfileListItem(s32) __asm__("func_8007B854");
extern u8 gFindListEntryByValue(void *, u8) __asm__("func_8007D39C");
extern void gInitSelectionIndices(void) __asm__("func_8007D408");
extern void gClampLoadoutSelections(s32) __asm__("func_8007D2DC");
extern void gFinalizeMissionLoadout(s32, s32, s32, s32, s32) __asm__("func_8007E3EC");

void __main(void) {}
static long sys3(long n, long a, long b, long c) {
    register long v __asm__("$2") = n; register long x __asm__("$4") = a;
    register long y __asm__("$5") = b; register long z __asm__("$6") = c;
    __asm__ volatile("syscall" : "+r"(v) : "r"(x), "r"(y), "r"(z)
                     : "memory", "$7", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25");
    return v;
}
static int slen(const char *s) { int n = 0; while (s[n]) n++; return n; }
static void say(const char *s) { sys3(4004, 1, (long)s, slen(s)); }
static int seq(const char *a, const char *b) { while (*a && *a == *b) { a++; b++; } return *a == *b; }
static void say_int(long v) {
    char b[16]; int i = 15; int neg = v < 0; unsigned long u = neg ? -v : v;
    b[i] = 0; do { b[--i] = '0' + u % 10; u /= 10; } while (u);
    if (neg) b[--i] = '-';
    say(b + i);
}
static void say_hex(unsigned long v) {
    char b[16]; int i = 15; b[i] = 0;
    do { b[--i] = "0123456789ABCDEF"[v & 15]; v >>= 4; } while (v);
    b[--i] = 'x'; b[--i] = '0'; say(b + i);
}

/* ---- relocated retail memory ---- */
static u8 gImage[IMAGE_MAX];
static int gImageSize;
static u8 gSession[0xF4];
static u8 *mem(u32 retail) { return gRetailRam + (retail - RETAIL_BASE); }
static u32 rd32(u32 a) { u8 *p = mem(a); return p[0] | p[1] << 8 | p[2] << 16 | (u32)p[3] << 24; }
static void wr32(u32 a, u32 v) { u8 *p = mem(a); p[0] = v; p[1] = v >> 8; p[2] = v >> 16; p[3] = v >> 24; }
/* Retail pointer -> host pointer value (and back). */
static u32 to_host(u32 p) { return p >= RETAIL_BASE && p < RETAIL_BASE + RAM_SIZE ? p - RELOC : p; }
static u32 to_retail(u32 p) {
    if (p >= RETAIL_BASE - RELOC && p < RETAIL_BASE - RELOC + RAM_SIZE) return p + RELOC;
    return p;
}
static void reloc(u32 a) { wr32(a, to_host(rd32(a))); }

/* ---- stub event recorders ---- */
static u32 gLastPush; static int gPushes; static long gLastSfx; static int gSfxs;
static int gPrompts; static int gSlotDraws; static long gRet;
void func_80078650(void *menu, s32 arg1) { gLastPush = to_retail((u32)menu); gPushes++; }
s32 func_80046C84(s32 id, s32 arg1) { gLastSfx = id; gSfxs++; return 0; }
void func_80078B34(s32 a, s32 b, s32 c) { gPrompts++; }
void func_8007B460(s32 a) { gSlotDraws++; }
s32 func_8008CCA0(u8 *a, u8 *b) { while (*a && *a == *b) { a++; b++; } return *a - *b; }
void func_8008CCB0(u8 *d, u8 *s) { while ((*d++ = *s++) != 0) {} }

static void reset(void) {
    int i;
    for (i = 0; i < RAM_SIZE; i++) gRetailRam[i] = i < gImageSize ? gImage[i] : 0;
    for (i = 0; i < 16; i++) {          /* mission records D_8009B4E4: +0 name, +4 briefing, +8 reel movie */
        reloc(0x8009B4E4 + i * 36); reloc(0x8009B4E4 + i * 36 + 4); reloc(0x8009B4E4 + i * 36 + 8);
    }
    for (i = 0; i < 3; i++) { reloc(0x8009C0A8 + i * 0x14); reloc(0x8009C0A8 + i * 0x14 + 4); }
    for (i = 0; i < 0xF4; i++) gSession[i] = 0;
    wr32(0x8009FA94, (u32)gSession);
    wr32(0x8009F468, 0x800A3808 - RELOC);   /* harness default: main menu descriptor active */
    gLastPush = 0; gPushes = 0; gLastSfx = -1; gSfxs = 0; gPrompts = 0; gSlotDraws = 0; gRet = 0;
}

/* ---- named fields shared with the .NET vector reader ---- */
struct Field { const char *name; u32 addr; char kind; };
/* kinds: b u8, c s8, h s16, w s32, p retail pointer, S pointer to string, s inline string */
static const struct Field gFields[] = {
    { "frontEndListFlag", 0x8009F3C0, 'b' }, { "unlockFlag", 0x8009F3C2, 'b' },
    { "selectedMission", 0x8009F880, 'c' }, { "listMin", 0x8009F881, 'c' },
    { "listMax", 0x8009F882, 'c' }, { "listTable", 0x8009F884, 'p' },
    { "listCount", 0x8009F888, 'b' }, { "listMode", 0x8009F889, 'b' },
    { "crimeWarProgress", 0x8009F458, 'b' }, { "savedCrimeWarProgress", 0x8009F3F1, 'b' },
    { "selectLock", 0x800A35CC, 'w' }, { "paProgress", 0x8009F459, 'b' },
    { "briefingShown", 0x8009F3E6, 'b' }, { "deferredTransition", 0x8009F8C8, 'w' },
    { "pendingMovie", 0x8009F8C4, 'S' }, { "D_800A377D", 0x800A377D, 'b' },
    { "D_800A368B", 0x800A368B, 'b' }, { "D_800A37AB", 0x800A37AB, 'b' },
    { "D_800A37DB", 0x800A37DB, 'b' }, { "D_800A362B", 0x800A362B, 'b' },
    { "playerSelection", 0x8009F8F0, 'w' }, { "playerSelection2", 0x8009F8F4, 'w' },
    { "ready0", 0x8009F8E0, 'w' }, { "ready1", 0x8009F8E4, 'w' },
    { "activePlayer", 0x8009F8B8, 'w' }, { "depth0", 0x8009F8C0, 'c' }, { "depth1", 0x8009F8C1, 'c' },
    { "selectedRow", 0x8009F87E, 'c' }, { "lineCount", 0x8009F87F, 'c' }, { "uiHold", 0x8009F8BD, 'b' },
    { "module", 0x8009B300, 's' }, { "pendingModule", 0x8009F3EC, 'S' }, { "loadScreen", 0x8009F3F0, 'b' },
    { "returnMode", 0x8009F45A, 'b' }, { "rowClear", 0x8009FA10, 'b' }, { "screenState", 0x8009FA00, 'w' },
    { "sel0p0", 0x8009F924, 'c' }, { "sel0p1", 0x8009F925, 'c' },
    { "sel1p0", 0x8009F926, 'c' }, { "sel1p1", 0x8009F927, 'c' },
    { "sel2p0", 0x8009F928, 'c' }, { "sel2p1", 0x8009F929, 'c' },
    { "lt0Count", 0x8009C0B0, 'b' }, { "lt0Min", 0x8009C0B1, 'b' }, { "lt0Max", 0x8009C0B2, 'b' },
    { "lt1Count", 0x8009C0C4, 'b' }, { "lt1Min", 0x8009C0C5, 'b' }, { "lt1Max", 0x8009C0C6, 'b' },
    { "lt2Items", 0x8009C0D0, 'p' },
    { "lt2Count", 0x8009C0D8, 'b' }, { "lt2Min", 0x8009C0D9, 'b' }, { "lt2Max", 0x8009C0DA, 'b' },
    { "scaleP1", 0x8009BB66, 'h' }, { "scaleP2", 0x8009BBBA, 'h' },
    { "loadoutScaleP1", 0x8009B7E6, 'h' }, { "loadoutScaleP2", 0x8009B856, 'h' },
    { "menu", 0x8009F468, 'p' },
    { 0, 0, 0 }
};

static long hexval(const char *s, int *ok) {
    long v = 0, sign = 1; *ok = 1;
    if (*s == '-') { sign = -1; s++; }
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        s += 2; if (!*s) *ok = 0;
        for (; *s; s++) {
            int d = *s >= '0' && *s <= '9' ? *s - '0' : *s >= 'A' && *s <= 'F' ? *s - 'A' + 10
                  : *s >= 'a' && *s <= 'f' ? *s - 'a' + 10 : -1;
            if (d < 0) { *ok = 0; return 0; }
            v = v * 16 + d;
        }
    } else {
        if (!*s) *ok = 0;
        for (; *s; s++) { if (*s < '0' || *s > '9') { *ok = 0; return 0; } v = v * 10 + *s - '0'; }
    }
    return v * sign;
}
static int hexdig(char c) { return c >= '0' && c <= '9' ? c - '0' : c >= 'A' && c <= 'F' ? c - 'A' + 10 : -1; }

/* Resolve a key to (address, kind). Besides the table: recN_state/recN_state2
 * (mission record N, bytes +0x1F/+0x20), recN_unk1F alias, sess_XX (s32 at
 * gGameSession+0xXX, hex), menu_XX (byte at active menu +0xXX, hex),
 * menu_24 pointer, and the stub events. Returns 0 on unknown key. */
static u8 *gHostAddr; static char gKind;
static int resolve(const char *k) {
    const struct Field *f;
    for (f = gFields; f->name; f++)
        if (seq(f->name, k)) { gHostAddr = mem(f->addr); gKind = f->kind; return 1; }
    if (k[0] == 'r' && k[1] == 'e' && k[2] == 'c') {
        int n = 0; const char *p = k + 3;
        while (*p >= '0' && *p <= '9') n = n * 10 + *p++ - '0';
        if (n > 15) return 0;
        if (seq(p, "_state")) { gHostAddr = mem(0x8009B4E4 + n * 36 + 0x1F); gKind = 'b'; return 1; }
        if (seq(p, "_state2")) { gHostAddr = mem(0x8009B4E4 + n * 36 + 0x20); gKind = 'b'; return 1; }
        return 0;
    }
    if (k[0] == 's' && k[1] == 'e' && k[2] == 's' && k[3] == 's' && k[4] == '_') {
        int a = hexdig(k[5]), b = hexdig(k[6]);
        if (a < 0 || b < 0 || k[7] || a * 16 + b > 0xF0) return 0;
        gHostAddr = gSession + a * 16 + b; gKind = 'w'; return 1;
    }
    if (k[0] == 'm' && k[1] == 'e' && k[2] == 'n' && k[3] == 'u' && k[4] == '_') {
        int a = hexdig(k[5]), b = hexdig(k[6]);
        u32 menu = to_retail(rd32(0x8009F468));
        if (a < 0 || b < 0 || k[7] || menu < RETAIL_BASE) return 0;
        gHostAddr = mem(menu + a * 16 + b); gKind = a * 16 + b == 0x24 ? 'p' : 'b'; return 1;
    }
    return 0;
}
static int event(const char *k, long *v) {
    if (seq(k, "push")) { *v = gLastPush; return 1; }
    if (seq(k, "pushes")) { *v = gPushes; return 1; }
    if (seq(k, "sfx")) { *v = gLastSfx; return 1; }
    if (seq(k, "sfxs")) { *v = gSfxs; return 1; }
    if (seq(k, "prompts")) { *v = gPrompts; return 1; }
    if (seq(k, "slotDraws")) { *v = gSlotDraws; return 1; }
    if (seq(k, "ret")) { *v = gRet; return 1; }
    return 0;
}
static u32 ld32(u8 *p) { return p[0] | p[1] << 8 | p[2] << 16 | (u32)p[3] << 24; }
static void st32(u8 *p, u32 v) { p[0] = v; p[1] = v >> 8; p[2] = v >> 16; p[3] = v >> 24; }
static long get_num(void) {
    switch (gKind) {
    case 'b': return *gHostAddr;
    case 'c': return (s8)*gHostAddr;
    case 'h': return (s16)(gHostAddr[0] | gHostAddr[1] << 8);
    case 'w': return (s32)ld32(gHostAddr);
    default: return (long)to_retail(ld32(gHostAddr));
    }
}
static int set_value(const char *v) {
    int ok; long n;
    if (gKind == 's') { u8 *d = gHostAddr; while ((*d++ = *v++) != 0) {} return 1; }
    if (gKind == 'S') return 0;   /* string pointers are outputs only */
    n = hexval(v, &ok); if (!ok) return 0;
    switch (gKind) {
    case 'b': case 'c': *gHostAddr = (u8)n; break;
    case 'h': gHostAddr[0] = n; gHostAddr[1] = n >> 8; break;
    case 'w': st32(gHostAddr, (u32)n); break;
    default: st32(gHostAddr, to_host((u32)n)); break;
    }
    return 1;
}
/* Formats the observed value of the resolved key into out. */
static char gOut[64];
static void fmt_num(long v, int hex) {
    char b[24]; int i = 23, neg = 0; unsigned long u;
    b[i] = 0;
    if (hex) { u = (unsigned long)v; do { b[--i] = "0123456789ABCDEF"[u & 15]; u >>= 4; } while (u); b[--i] = 'x'; b[--i] = '0'; }
    else { neg = v < 0; u = neg ? -v : v; do { b[--i] = '0' + u % 10; u /= 10; } while (u); if (neg) b[--i] = '-'; }
    for (neg = 0; (gOut[neg] = b[i + neg]) != 0; neg++) {}
}
static void observe(const char *k) {
    long v;
    if (event(k, &v)) { fmt_num(v, seq(k, "push") && v != 0); return; }
    if (!resolve(k)) { gOut[0] = '!'; gOut[1] = 0; return; }
    if (gKind == 's' || gKind == 'S') {
        u8 *s = gKind == 's' ? gHostAddr : (u8 *)ld32(gHostAddr); int i = 0;
        if (!s) { gOut[0] = '-'; gOut[1] = 0; return; }
        while (s[i] && i < 63) { gOut[i] = s[i]; i++; }
        gOut[i] = 0; return;
    }
    v = get_num();
    fmt_num(v, gKind == 'p' && v != 0);
}

static char gOpBuf[64];
static int run_op(const char *src) {
    char *a[3], *op = gOpBuf; int n = 0, ok; long x[2];
    while (*src && op < gOpBuf + 63) *op++ = *src++;
    *op = 0; op = gOpBuf;
    a[n++] = op;
    while (*op) { if (*op == ':') { *op = 0; if (n < 3) a[n++] = op + 1; } op++; }
    x[0] = n > 1 ? hexval(a[1], &ok) : 0; x[1] = n > 2 ? hexval(a[2], &ok) : 0;
    if (seq(a[0], "setupCW")) gSetupCrimeWarMissionList();
    else if (seq(a[0], "setupPA")) gSetupPrecinctAssaultMissionList();
    else if (seq(a[0], "unlock")) gUpdatePrecinctUnlockFlag();
    else if (seq(a[0], "select1p")) gSelectPrecinctAssault1P(x[0]);
    else if (seq(a[0], "select2p")) gSelectPrecinctAssault2P(x[0]);
    else if (seq(a[0], "initSetup")) gInitMissionSelectState();
    else if (seq(a[0], "join")) gHandlePlayerJoinSelection(x[0], 0, 0, 0, x[1]);
    else if (seq(a[0], "list")) gHandleProfileListItem(x[0]);
    else if (seq(a[0], "find")) gRet = gFindListEntryByValue(mem(0x8009C0A8 + x[0] * 0x14), (u8)x[1]);
    else if (seq(a[0], "loadoutInit")) gInitSelectionIndices();
    else if (seq(a[0], "clamp")) gClampLoadoutSelections(x[0]);
    else if (seq(a[0], "finalize")) gFinalizeMissionLoadout(x[0], 0, 0, 0, x[1]);
    else if (seq(a[0], "menu")) wr32(0x8009F468, to_host((u32)x[0]));   /* switch active descriptor mid-case */
    else return 0;
    return 1;
}

static char gData[65536], gLine[1024];
int main(int argc, char **argv) {
    int fd, n, i, cases = 0, failed = 0, emit;
    char *p, *e;
    static u8 hdr[0x800];
    if (argc < 3) return 2;
    emit = argc > 3 && seq(argv[3], "--emit");
    fd = sys3(4005, (long)argv[2], 0, 0); if (fd < 0) { say("cannot open SLUS\n"); return 2; }
    if (sys3(4003, fd, (long)hdr, 0x800) != 0x800) return 2;
    if (hdr[0] != 'P' || hdr[1] != 'S' || hdr[2] != '-' || hdr[3] != 'X') return 2;
    if ((hdr[0x18] | hdr[0x19] << 8 | hdr[0x1A] << 16 | (u32)hdr[0x1B] << 24) != RETAIL_BASE) return 2;
    gImageSize = sys3(4003, fd, (long)gImage, IMAGE_MAX); sys3(4006, fd, 0, 0);
    if (gImageSize != IMAGE_MAX) { say("short SLUS image\n"); return 2; }

    fd = sys3(4005, (long)argv[1], 0, 0); if (fd < 0) return 2;
    n = sys3(4003, fd, (long)gData, sizeof(gData) - 1); sys3(4006, fd, 0, 0);
    if (n < 0) return 2; gData[n] = 0;

    for (p = gData; *p; p = *e ? e + 1 : e) {
        char *tok[64]; int nt = 0, bar = -1, bad = 0;
        for (e = p; *e && *e != '\n'; e++) {}
        for (i = 0; p + i < e && i < 1023; i++) gLine[i] = p[i];
        gLine[i] = 0;
        if (i && gLine[i - 1] == '\r') gLine[--i] = 0;
        {   /* tokenise */
            char *q = gLine;
            while (*q) {
                while (*q == ' ' || *q == '\t') *q++ = 0;
                if (!*q) break;
                if (*q == '#' && nt == 0) break;
                tok[nt] = q; if (q[0] == '|' && (q[1] == 0 || q[1] == ' ')) bar = nt;
                if (++nt == 64) break;
                while (*q && *q != ' ' && *q != '\t') q++;
            }
        }
        if (nt == 0) { if (emit) { sys3(4004, 1, (long)p, e - p); say("\n"); } continue; }
        if (nt < 3 || bar < 2) { say("BAD LINE "); say(tok[0]); say("\n"); return 2; }
        reset();
        for (i = 2; i < bar; i++) {     /* inputs */
            char *eq = tok[i];
            while (*eq && *eq != '=') eq++;
            if (!*eq) { bad = 1; break; }
            *eq = 0;
            if (!resolve(tok[i]) || !set_value(eq + 1)) { bad = 1; *eq = '='; break; }
            *eq = '=';
        }
        if (bad) { say("BAD INPUT "); say(tok[0]); say(" "); say(tok[i]); say("\n"); return 2; }
        {   /* ops */
            char *q = tok[1], *s = q;
            for (;; q++) {
                if (*q == ',' || *q == 0) {
                    char c = *q; *q = 0;
                    if (!run_op(s)) { say("BAD OP "); say(tok[0]); say("\n"); return 2; }
                    *q = c; if (!c) break; s = q + 1;
                }
            }
        }
        cases++;
        if (emit) { for (i = 0; i <= bar; i++) { say(tok[i]); say(" "); } }
        for (i = bar + 1; i < nt; i++) {   /* outputs */
            char *eq = tok[i];
            while (*eq && *eq != '=') eq++;
            if (!*eq) { say("BAD OUTPUT "); say(tok[0]); say("\n"); return 2; }
            *eq = 0;
            observe(tok[i]);
            if (gOut[0] == '!' ) { say("UNKNOWN KEY "); say(tok[i]); say("\n"); return 2; }
            if (emit) { say(tok[i]); say("="); say(gOut); say(i + 1 < nt ? " " : ""); }
            else if (!seq(gOut, eq + 1)) {
                say("FAIL "); say(tok[0]); say(" "); say(tok[i]); say(" got "); say(gOut);
                say(" want "); say(eq + 1); say("\n"); failed++;
            }
            *eq = '=';
        }
        if (emit) say("\n");
    }
    if (emit) return 0;
    if (!cases || failed) { say("precinct-assault: "); say_int(failed); say(" FAILED\n"); return 1; }
    say("precinct-assault: ALL PASS ("); say_int(cases); say(" cases)\n");
    return 0;
}
