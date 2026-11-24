/* Host harness: Precinct Assault unit ordering (spawner) path.
 *
 * Runs the unchanged retail units of the spawner chain that turns an order
 * into a spawned unit:
 *
 *   func_800616A0 gUpdateRegisteredSpawners   per-frame drain of the order bits
 *   func_8006125C gTrySpawnRegisteredUnit     one spawn attempt + stats
 *   func_8006158C gApplySpawnerCommandById    script opcode 60: record lookup by
 *                                             definition id, then forwards the
 *                                             untouched CODE/MODE registers
 *   func_800614BC gApplyRegisteredSpawnerCommand
 *
 * Memory model (same as precinct_assault_harness.c): qemu-mipsel cannot map
 * guest addresses >= 0x80000000, so the retail RAM window lives at
 * (retail address - 0x70000000). The registry table D_8009F800, the live
 * spawner records and their substructures are built by this harness in that
 * window from the values named on the vector line.
 *
 * Stubs (not part of the parity claim): func_80069D68 (script VM) returns the
 * per-entity `script_ok` input and counts calls; func_8004D4F8 (entity
 * creation) records the definition pointer it was handed; func_80019668 (PRNG)
 * returns the value of `rand`, so the modulo in the spawner setup path is
 * deterministic without the retail table state.
 *
 * Vector line: name ops key=value ... | key=value ...
 *   ops    comma separated; `drain:AGO` (top up AG0 order bits, drain AO0,
 *          then tick alpha timers: A1, A2, A5) or `cmd:ID:CODE:MODE`
 *   left   per-case reset defaults, then applied
 *   right  observed values
 * Usage: harness VECTORS [--emit]
 */
typedef signed char s8;
typedef unsigned char u8;
typedef short s16;
typedef unsigned short u16;
typedef int s32;
typedef unsigned int u32;

#define RETAIL_BASE 0x80010000u
#define RELOC 0x70000000u
#define RAM_SIZE 0x9C000

/* Harness-owned addresses inside the relocated retail window (retail BSS that
 * the real game fills at run time, never from the executable image). */
#define ADDR_REGISTRY 0x800A0000u
#define ADDR_ENTITY 0x800A0100u
#define ADDR_SUB 0x800A0140u
#define ADDR_INFO 0x800A0180u
#define ADDR_DEF 0x800A01C0u
#define ADDR_STATS2 0x800A0380u
#define ADDR_ARG1 0x800A4000u
#define ADDR_ARG2 0x800A5000u
#define ADDR_OF_ENTITY 0x0u
#define ADDR_OF_SUB 0x4u
#define ADDR_ENTITY2 0x800A0200u
#define ADDR_SUB2 0x800A0240u
#define ADDR_INFO2 0x800A0280u
#define ADDR_DEF2 0x800A02C0u
#define ADDR_STATS3 0x800A03A0u
#define ADDR_FREE 0x800A7000u   /* generic scratch for pointer fields */
#define ADDR_CFG 0x800A0300u    /* first registry config entry {unk00, info} */
#define ADDR_CFG2 0x800A0320u   /* second registry config entry */
#define ADDR_CNT1 0x800A0400u   /* first order counter (script call 1) */
#define ADDR_CNT2 0x800A0404u   /* second order counter (script call 2) */
#define ADDR_SESSION 0x800A0600u /* gGameSession stand-in (unk5C = game type) */
#define ADDR_PLAYER 0x800A0700u  /* gSinglePlayerEntity stand-in (+0x18 id, +0x134 stats ptr) */
#define ADDR_PLAYERSTATS 0x800A0740u /* struct PlayerStats for that player (type 1..4 at +0x2A/2E/32/36) */
#define OTHER_ID 0x22            /* gPlayerEntities->id, the second fixture record id */
#define ADDR_OTHER 0x800A0780u   /* gPlayerEntities stand-in (+0x18 id, +0xAC stats ptr) */
#define ADDR_OTHERSTATS 0x800A07C0u /* struct Stats for other player (type 1..4 at +0x2/6/A/E) */
#define PLAYER_ID 0x11           /* matches the fixture record id at info->id */
#define ADDR_GATE 0x800A0408u    /* script-gate return value for the spawner call */
#define ADDR_ENABLE 0x800A040Cu  /* timer-path enable flag handed to the record */
#define GATE_SCRIPT_ID 0x11      /* sub->arg1 of the fixture record (pre-spawn gate) */

extern u8 gRetailRam[] __asm__("D_RAM");

extern void gUpdateRegisteredSpawners(s32) __asm__("func_800616A0");
extern void gTrySpawnRegisteredUnit(void *, s32) __asm__("func_8006125C");
extern void gApplySpawnerCommandById(s32, s32, s32) __asm__("func_8006158C");
extern void gApplyRegisteredSpawnerCommand(void *, s32, s32) __asm__("func_800614BC");

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

#if defined(SPAWNER_VECTORS)
#include "spawner_path_vectors.h"
#endif

/* ---- relocated retail memory ---- */
static u8 *mem(u32 retail) { return gRetailRam + (retail - RETAIL_BASE); }
static u32 rd32(u32 a) { u8 *p = mem(a); return p[0] | p[1] << 8 | p[2] << 16 | (u32)p[3] << 24; }
static void wr8(u32 a, u32 v) { *mem(a) = (u8)v; }
static void wr32(u32 a, u32 v) { u8 *p = mem(a); p[0] = v; p[1] = v >> 8; p[2] = v >> 16; p[3] = v >> 24; }
static void wr16(u32 a, u32 v) { u8 *p = mem(a); p[0] = v; p[1] = v >> 8; }
static u32 rd16(u32 a) { u8 *p = mem(a); return p[0] | p[1] << 8; }
static u32 to_host(u32 p) { return p >= RETAIL_BASE && p < RETAIL_BASE + RAM_SIZE ? p - RELOC : p; }
static u32 to_retail(u32 p) {
    if (p >= RETAIL_BASE - RELOC && p < RETAIL_BASE - RELOC + RAM_SIZE) return p + RELOC;
    return p;
}

/* ---- stub recorders ---- */
static s32 gScriptOk; static int gScriptCalls;
static u32 gSpawnDef; static int gSpawnCalls;
static s32 gRandVal; static int gRandCalls;
/* Script VM stand-in. The spawner calls it twice per attempt: once with the
 * pre-spawn gate (sub->arg1, $a2 = -1) and once with the post-spawn script
 * (sub->arg2). `in_gate` decides the gate; both calls are counted. */
s32 func_80069D68(void *script, s32 a1, s32 a2, void *entity) {
    (void)script; (void)a1; (void)entity;
    gScriptCalls++;
    /* func_8006125C calls this twice: the pre-spawn gate with $a2 = -1, the
       post-spawn script with $a2 = 0. Only the gate can veto a spawn. */
    if (a2 == -1)
        return (s32)rd32(ADDR_GATE);
    return 1;
}
void gSpawnTargetableEntityId(void *def) __asm__("func_8004D4F8");
void gSpawnTargetableEntityId(void *def) { gSpawnDef = to_retail((u32)def); gSpawnCalls++; }
s32 func_80019668(void) __asm__("func_80019668");
s32 func_80019668(void) { gRandCalls++; return gRandVal; }

/* ---- fixture ---- */
#define TIMER_SECONDS 90

static void build_entity(u32 entity_addr, u32 sub_addr, u32 info_addr, u32 def_addr, u32 def_id, u32 type, u32 stats_addr) {
    /* struct Sub, offsets taken from the disassembly of func_8006125C:
     *   +0x0C s16 alpha-timer reload   +0x0E s16 cap
     *   +0x10 u16 order word           +0x12 s16 remaining count
     *   +0x14 u16 alpha timer          +0x16 s16 total spawned
     *   +0x18 s32 pre-spawn script id  +0x1C s32 post-spawn script id
     *   +0x20 struct Def *def          +0x28 struct Info *info
     * struct Info: +0x1C u8 id, +0x1D u8 type, +0x134 struct Stats *stats. */
    wr32(entity_addr + ADDR_OF_SUB, to_host(sub_addr));
    wr32(sub_addr + 0x0C, TIMER_SECONDS);       /* alpha timer reload (5 * level) */
    wr16(sub_addr + 0x0E, 3);                   /* cap */
    wr16(sub_addr + 0x10, 0);                   /* order word */
    wr16(sub_addr + 0x12, 3);                   /* remaining spawn count */
    wr16(sub_addr + 0x14, 0);                   /* alpha timer */
    wr16(sub_addr + 0x16, 0);                   /* total spawned */
    wr32(sub_addr + 0x18, GATE_SCRIPT_ID);      /* pre-spawn gate script id */
    wr32(sub_addr + 0x1C, 1);                   /* post-spawn script id */
    wr32(sub_addr + 0x20, to_host(def_addr));   /* definition handed to the spawn stub */
    wr32(sub_addr + 0x28, to_host(info_addr));  /* struct Info * */
    wr8(info_addr + 0x1C, def_id);
    wr8(info_addr + 0x1D, type);
    if (stats_addr) wr32(info_addr + 0x134, to_host(stats_addr));
}

static void reset(void) {
    int i;
    for (i = 0; i < RAM_SIZE; i++) gRetailRam[i] = 0;
    wr32(0x8009F840, to_host(ADDR_FREE));   /* script VM gate: non-zero keeps scripts enabled */
    /* func_8006125C only touches statistics when gGameSession->0x5C == 1 and
       the record's info->id matches a participant (player first, then
       gPlayerEntities, then gPlayerEntity2). A null participant ends the walk,
       so the counter update is skipped - keep both "other" slots zero by
       default and let a vector install one with in_other=1. */
    wr32(0x8009F9EC, to_host(ADDR_PLAYER));
    wr32(ADDR_REGISTRY, 1);                  /* entity count */
    wr32(ADDR_REGISTRY + 4, to_host(ADDR_CFG));  /* registry item */
    build_entity(ADDR_ENTITY, ADDR_SUB, ADDR_INFO, ADDR_DEF, 0x11, 1, 0);
    build_entity(ADDR_ENTITY2, ADDR_SUB2, ADDR_INFO2, ADDR_DEF2, 0x22, 3, ADDR_STATS2);
    /* The drain loop does `lw s1,4(item)` for the order word and passes the
       item to func_8006125C, which does `lw s0,4(item)` again - so the record
       pointer sits at item+0x04. */
    wr32(ADDR_CFG + 0, to_host(ADDR_SUB));
    wr32(ADDR_CFG + 4, to_host(ADDR_SUB));
    wr32(ADDR_CNT1, 0);
    wr32(ADDR_CNT2, 0);
    gScriptOk = 1; gScriptCalls = 0;
    gSpawnDef = 0; gSpawnCalls = 0;
    gRandVal = 0; gRandCalls = 0;
    /* gPlayerEntities / gPlayerEntity2: both null, i.e. "no other player", so
       an id mismatch falls through to the skip path. A vector may install one
       of them by setting in_other=1 and giving the record a non-player id. */
    wr32(0x8009FA58, 0);
    wr32(0x8009FA5C, 0);
    wr32(ADDR_OTHER + 0x18, OTHER_ID);
    wr32(ADDR_OTHER + 0xAC, to_host(ADDR_OTHERSTATS));
    wr32(ADDR_FREE + 4, 0);
    wr32(ADDR_GATE, 1);      /* pre-spawn gate returns true unless `in_gate=0` */
    wr32(ADDR_ENABLE, 1);    /* timer-path enable unless `in_enable=0` */
    wr32(ADDR_SESSION + 0x5C, 1);   /* session mode 1: counters and stats active */
    wr32(ADDR_PLAYER + 0x18, PLAYER_ID);
    wr32(ADDR_PLAYER + 0x134, to_host(ADDR_PLAYERSTATS));
    wr32(0x8009FA00, 0);
    wr32(0x8009F9A4, to_host(ADDR_FREE));
    wr32(0x8009F9F8, to_host(ADDR_FREE));
    wr32(0x8009F800, to_host(ADDR_REGISTRY));
    wr16(ADDR_SUB + 0x12, 3);   /* lhu 0x12(s0): remaining spawn count */
    wr32(0x8009FA94, to_host(ADDR_SESSION));
}

/* ---- named fields ---- */
/* Offsets are the ones the retail units actually read (see the disassembly of
 * func_800616A0 / func_8006125C): the order word, cap, enable, alpha timer and
 * reload live in the record substructure, the remaining spawn count in its
 * s16 at +0x12, the running total in its s16 at +0x16. Inputs are prefixed
 * `in_` so a typo can never silently set the field it is supposed to check. */
struct Field { const char *name; u32 addr; char kind; };
/* kinds: b u8, c s8, h u16, H s16, w s32, p retail pointer.
 * in_other is handled in resolve() as kind 'z': 0/1 selects whether a second
 * participant object stands at gPlayerEntities. */
static const struct Field gFields[] = {
    { "ops",      ADDR_SUB + 0x10, 'h' },   /* order word: bits 8..11 burst, 12..15 timed */
    { "count",    ADDR_SUB + 0x12, 'H' },   /* remaining spawns for this record */
    { "total",    ADDR_SUB + 0x16, 'H' },   /* spawns granted since load */
    { "cap",      ADDR_SUB + 0x0E, 'H' },   /* spawn ceiling for the timer path */
    { "timer",    ADDR_SUB + 0x14, 'h' },   /* alpha timer, counts down */
    { "reload",   ADDR_SUB + 0x0C, 'h' },   /* alpha timer reload value */
    { "ops2",     ADDR_SUB2 + 0x10, 'h' },
    { "count2",   ADDR_SUB2 + 0x12, 'H' },
    { "total2",   ADDR_SUB2 + 0x16, 'H' },
    { "timer2",   ADDR_SUB2 + 0x14, 'h' },
    { "in_ops",   ADDR_SUB + 0x10, 'h' },
    { "in_count", ADDR_SUB + 0x12, 'H' },
    { "in_total", ADDR_SUB + 0x16, 'H' },
    { "in_cap",   ADDR_SUB + 0x0E, 'H' },
    { "in_enable", ADDR_ENABLE, 'w' },
    { "in_timer", ADDR_SUB + 0x14, 'h' },
    { "in_reload", ADDR_SUB + 0x0C, 'h' },
    { "in_ops2",  ADDR_SUB2 + 0x10, 'h' },
    { "in_cap2",  ADDR_SUB2 + 0x0E, 'H' },
    /* The id lives at info+0x1C and the type at info+0x1D (lbu 28/29(s5) in
     * func_8006125C). */
    { "in_id",    ADDR_INFO + 0x1C, 'b' },
    { "in_type",  ADDR_INFO + 0x1D, 'b' },
    { "in_type2", ADDR_INFO2 + 0x1D, 'b' },
    /* Player-slot counters. func_8006125C reaches them as
     * gSinglePlayerEntity->stats (lw v1,0x134(player)) and then writes the u16
     * at 0x2A (type 1), 0x2E (type 2), 0x32 (type 3), 0x36 (type 4), offsets
     * 0x164/0x1A4/0x1E4/0x224 in the built object. */
    { "st_a",    ADDR_PLAYERSTATS + 0x2A, 'H' },  /* type 1 player counter */
    { "st_b",    ADDR_PLAYERSTATS + 0x2E, 'H' },  /* type 2 */
    { "st_c",    ADDR_PLAYERSTATS + 0x32, 'H' },  /* type 3 */
    { "st_d",    ADDR_PLAYERSTATS + 0x36, 'H' },  /* type 4 */
    /* Second-participant counters: the same switch taken with a non-zero
     * `stats` (gPlayerEntities->stats, lw v1,0xAC(other)) writes the u16 at
     * 0x02/0x06/0x0A/0x0E, i.e. object offsets 0x140/0x180/0x1C0/0x200. */
    { "ot_a",    ADDR_OTHERSTATS + 0x02, 'H' },
    { "ot_b",    ADDR_OTHERSTATS + 0x06, 'H' },
    { "ot_c",    ADDR_OTHERSTATS + 0x0A, 'H' },
    { "ot_d",    ADDR_OTHERSTATS + 0x0E, 'H' },
    { "other_id", ADDR_OTHER + 0x18, 'b' },       /* gPlayerEntities->id, compared with info->id */
    { "rand",    ADDR_FREE + 0x00, 'w' },
    { 0, 0, 0 }
};
/* `stat2_a`/`stat3_c` are also the stub inputs; expose rand through a scalar. */
static u32 gScalarRand;

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

static u8 *gHostAddr; static char gKind;
static int resolve(const char *k) {
    const struct Field *f;
    for (f = gFields; f->name; f++)
        if (seq(f->name, k)) { gHostAddr = mem(f->addr); gKind = f->kind; return 1; }
    if (seq(k, "rand")) { gHostAddr = mem(ADDR_FREE); gKind = 'w'; return 1; }
    if (seq(k, "in_gate")) { gHostAddr = mem(ADDR_GATE); gKind = 'w'; return 1; }
    if (seq(k, "in_other")) { gHostAddr = mem(0x8009FA58); gKind = 'z'; return 1; }
    if (seq(k, "in_enable")) { gHostAddr = mem(ADDR_ENABLE); gKind = 'w'; return 1; }
    if (k[0] == 's' && k[1] == 'e' && k[2] == 's' && k[3] == 's' && k[4] == '_') {
        int a = hexdig(k[5]), b = hexdig(k[6]);
        if (a < 0 || b < 0 || k[7]) return 0;
        gHostAddr = mem(0x8009FA94 + a * 16 + b); gKind = 'w'; return 1;
    }
    return 0;
}
static int event(const char *k, long *v) {
    if (seq(k, "scriptOk")) { *v = gScriptOk; return 1; }
    if (seq(k, "scriptCalls")) { *v = gScriptCalls; return 1; }
    if (seq(k, "spawnDef")) { *v = gSpawnDef; return 1; }
    if (seq(k, "spawnCalls")) { *v = gSpawnCalls; return 1; }
    if (seq(k, "randCalls")) { *v = gRandCalls; return 1; }
    return 0;
}
static u32 ld32(u8 *p) { return p[0] | p[1] << 8 | p[2] << 16 | (u32)p[3] << 24; }
static void st32(u8 *p, u32 v) { p[0] = v; p[1] = v >> 8; p[2] = v >> 16; p[3] = v >> 24; }
static long get_num(void) {
    switch (gKind) {
    case 'z': return (s32)ld32(gHostAddr) ? 1 : 0;   /* "an other player is installed" */
    case 'b': return *gHostAddr;
    case 'c': return (s8)*gHostAddr;
    case 'h': return (u16)(gHostAddr[0] | gHostAddr[1] << 8);
    case 'H': return (s16)(gHostAddr[0] | gHostAddr[1] << 8);
    case 'w': return (s32)ld32(gHostAddr);
    default: return (long)to_retail(ld32(gHostAddr));
    }
}
static int set_value(const char *v) {
    int ok; long n;
    if (gKind == 'p') return 0;   /* pointers are outputs only */
    n = hexval(v, &ok); if (!ok) return 0;
    switch (gKind) {
    case 'z': st32(gHostAddr, n ? (u32)to_host(ADDR_OTHER) : 0); break;
    case 'b': case 'c': *gHostAddr = (u8)n; break;
    case 'h': case 'H': gHostAddr[0] = n; gHostAddr[1] = n >> 8; break;
    case 'w': st32(gHostAddr, (u32)n); break;
    default: return 0;
    }
    if (gKind == 'w' && gHostAddr == mem(ADDR_FREE)) gRandVal = (s32)n;
    return 1;
}
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
    if (event(k, &v)) { fmt_num(v, (seq(k, "spawnDef") && v != 0)); return; }
    if (!resolve(k)) { gOut[0] = '!'; gOut[1] = 0; return; }
    v = get_num();
    fmt_num(v, gKind == 'p' && v != 0);
}


static int run_op(const char *src) {
    char buf[64], *op, *name, *args[3];
    int nargs = 0, ok = 1, is_drain;
    long x[3];

    for (op = buf; *src && op < buf + 63; src++) *op++ = *src;
    *op = 0;

    /* split "name:arg:arg:arg" in place; the first ':' ends the name */
    name = buf;
    for (op = buf; *op; op++)
        if (*op == ':') { *op = 0; break; }
    is_drain = seq(name, "drain");
    if (is_drain) {
        /* drain:TOP:BOT:ELAPSED - a zero-filled middle field is legitimate */
        op++;                       /* first argument starts here */
        args[nargs++] = op;
        for (; *op; op++) {
            if (*op != ':') continue;
            *op = 0;
            if (nargs >= 3) return 0;
            args[nargs++] = op + 1;
        }
        if (nargs != 3) return 0;   /* drain:TOP:BOT:ELAPSED */
    } else {
        for (op++; *op; op++) {
            if (*op != ':') continue;
            *op = 0;
            if (nargs >= 3) return 0;
            args[nargs++] = op + 1;
        }
    }

    x[0] = nargs > 0 ? hexval(args[0], &ok) : 0;
    x[1] = nargs > 1 ? hexval(args[1], &ok) : 0;
    x[2] = nargs > 2 ? hexval(args[2], &ok) : 0;
    if (!ok) return 0;

    if (is_drain) {
        u32 top = (u32)x[0] & 0xF, bot = (u32)x[1] & 0xF;
        wr16(ADDR_SUB + 0x10, rd16(ADDR_SUB + 0x10) | (top << 8) | (bot << 12));
        gUpdateRegisteredSpawners((s32)x[2]);
        return 1;
    }
    if (seq(name, "cmd")) {
        gApplySpawnerCommandById((s32)x[0], (s32)x[1], (s32)x[2]);
        return 1;
    }
    if (seq(name, "cmdrec")) {
        gApplyRegisteredSpawnerCommand((void *)to_host(ADDR_ENTITY), (s32)x[0], (s32)x[1]);
        return 1;
    }
    return 0;
}

static char gData[65536], gLine[1024];
int main(int argc, char **argv) {
    int fd, n, i, cases = 0, failed = 0, emit;
    char *p, *e;
    emit = argc > 2 && seq(argv[2], "--emit");
#ifndef SPAWNER_VECTORS
    if (argc < 2) return 2;
#endif
#ifdef SPAWNER_VECTORS
    /* The coverage tool runs the binary without arguments, so the default
       build bakes the vectors in through a generated header. An explicit
       vector path on the command line still wins. */
    if (argc < 2) {
        const char *v = SPAWNER_VECTORS;
        for (n = 0; v[n] && n < (int)sizeof(gData) - 1; n++) gData[n] = v[n];
        gData[n] = 0;
        goto have_data;
    }
#endif
    fd = sys3(4005, (long)argv[1], 0, 0); if (fd < 0) return 2;
    n = sys3(4003, fd, (long)gData, sizeof(gData) - 1); sys3(4006, fd, 0, 0);
    if (n < 0) return 2; gData[n] = 0;
have_data:

    for (p = gData; *p; p = *e ? e + 1 : e) {
        char *tok[64]; int nt = 0, bar = -1, bad = 0;
        for (e = p; *e && *e != '\n'; e++) {}
        for (i = 0; p + i < e && i < 1023; i++) gLine[i] = p[i];
        gLine[i] = 0;
        if (i && gLine[i - 1] == '\r') gLine[--i] = 0;
        {
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
        for (i = 2; i < bar; i++) {
            char *eq = tok[i];
            while (*eq && *eq != '=') eq++;
            if (!*eq) { bad = 1; break; }
            *eq = 0;
            if (!resolve(tok[i]) || !set_value(eq + 1)) { bad = 1; *eq = '='; break; }
            *eq = '=';
        }
        if (bad) { say("BAD INPUT "); say(tok[0]); say(" "); say(tok[i]); say("\n"); return 2; }
        {
            char *q = tok[1], *s = q;
            for (;; q++) {
                if (*q == ',' || *q == 0) {
                    char c = *q; *q = 0;
                    if (!run_op(s)) { say("BAD OP "); say(tok[0]); say(" ["); say(s); say("]\n"); return 2; }
                    *q = c; if (!c) break; s = q + 1;
                }
            }
        }
        cases++;
        if (emit) { for (i = 0; i <= bar; i++) { say(tok[i]); say(" "); } }
        for (i = bar + 1; i < nt; i++) {
            char *eq = tok[i];
            while (*eq && *eq != '=') eq++;
            if (!*eq) { say("BAD OUTPUT "); say(tok[0]); say("\n"); return 2; }
            *eq = 0;
            observe(tok[i]);
            if (gOut[0] == '!') { say("UNKNOWN KEY "); say(tok[i]); say("\n"); return 2; }
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
    if (!cases || failed) { say("spawner-path: "); say_int(failed); say(" FAILED\n"); return 1; }
    say("spawner-path: ALL PASS ("); say_int(cases); say(" cases)\n");
    return 0;
}
