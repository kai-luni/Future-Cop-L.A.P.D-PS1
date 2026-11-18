/* Environment / matrix render core harness (test-only, NOT retail code).
 *
 * Executes the ORIGINAL units from src/game under qemu-mipsel and checks them
 * against tests/host/vectors/env.vectors:
 *   C units   : func_8001846C gSinCos, func_800188D0 gBuildRotationMatrix,
 *               func_80018DC0 gBuildRotationMatrixVariant, func_80019470
 *               gTransposeMatrix3x3, func_8001A81C gMulMatrixVector,
 *               func_80018860 gBuildViewMatrix, func_80013CB8 gComposeBoneMatrix
 *   asm units : func_80010750 gGteLoadMatrix, func_80010820 gGteLoadRotation,
 *               func_8001006C, func_8001010C, func_800101D4, func_80010234
 *               (batch transforms), func_8001047C gProjectVertexBatch
 *
 * The handwritten GTE units run as their retail instruction words. qemu has
 * no COP2, so every cop2 instruction (mtc2/mfc2/ctc2/cfc2/mvmva/rtps) raises
 * SIGILL; the handler below executes that single instruction against a
 * software GTE register file and resumes at pc+4 (none of the cop2 words in
 * these units sits in a branch delay slot). Everything else - the packing,
 * shifts, negations, loop/branch structure, stores and the clip-byte logic of
 * func_8001047C - is the real retail code.
 *
 * GTE model: the arithmetic mirrors tests/host/gte_ref.c exactly (MAC = sum
 * >> (sf*12), IR saturation per lm with FLAG bits 24..22, SZ3 clamp 0..FFFF
 * bit 18, screen clamp -400h..3FFh bits 14/13). The divide is gte_quot() from
 * gte_ref.c itself (rounded H*65536/SZ3, overflow H >= 2*SZ3, bit 17). In
 * addition the trap model implements what gte_ref.c leaves out because the
 * retail words observe it: the MVMVA field decode (sf/mx/v/cv/lm from the
 * command word, so cv=0 adds TR and cv=3 does not), IR0/MAC0 depth cue (bit
 * 12, inert with DQA=DQB=0 as used here), MAC0 overflow bits 16/15, MAC
 * 44-bit overflow bits 30..25 on the final sum, the SXY/SZ FIFOs and FLAG
 * bit 31 (= OR of bits 30..23 and 18..13), which func_8001047C returns.
 *
 * The retail lw of D_8009FA0C (screen width mode) inside func_8001047C uses
 * a hardwired KSEG0 address; the resulting SIGSEGV/SIGBUS is handled for
 * exactly that instruction word (0x8D4AFA0C) by loading the per-case width
 * mode into $t2.
 *
 * Expected values in env.vectors are hand-derived from the C/asm source of
 * each unit (derivations are written next to every case), not produced by
 * running another implementation. */
typedef unsigned int u32;
typedef int s32;
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern void func_8001846C(u32, s32 *, s32 *);
extern void func_800188D0(s32, s32, s32, s32 *);
extern void func_80018DC0(s32, s32, s32, s32 *);
extern void func_80019470(s32 *, s32 *);
extern void func_8001A81C(s32 *, s32 *, s32 *);
extern void func_80018860(s32 *, s32 *);
extern void func_80010750(s32 *);
extern void func_80010820(s32 *);
extern void func_8001006C(s32 *, s32 *, s32);
extern void func_8001010C(s16 *, s16 *, s32);
extern void func_800101D4(s16 *, s16 *, s32);
extern void func_80010234(s16 *, s32 *, s32);
extern u32 func_8001047C(s16 *, u32 *, u8 *, s32);
struct Input { s16 pad0[3]; u8 pad6, flags; s16 x, y, z, rx, ry, rz; };
extern void func_80013CB8(s32, struct Input *, s16 *, s32);
extern s32 gte_quot(s32 h, s32 sz3, s32 *overflow);

s32 D_800A0300[8 * 12]; /* gBoneMatrices for func_80013CB8 */

void __main(void) {}

static long sys4(long n, long a, long b, long c, long d)
{
    register long v0 __asm__("$2") = n;
    register long a0 __asm__("$4") = a;
    register long a1 __asm__("$5") = b;
    register long a2 __asm__("$6") = c;
    register long a3 __asm__("$7") = d;
    __asm__ volatile("syscall" : "+r"(v0), "+r"(a3) : "r"(a0), "r"(a1), "r"(a2)
                     : "memory", "$8", "$9", "$10", "$11", "$12", "$13", "$14", "$15", "$24", "$25");
    return v0;
}
static void say(const char *s)
{
    int n = 0; while (s[n]) n++; sys4(4004, 1, (long)s, n, 0);
}
static void say_hex(u32 v)
{
    char b[10]; int i, h;
    b[0] = ' ';
    for (i = 0; i < 8; i++) { h = (v >> (28 - 4 * i)) & 15; b[1 + i] = (char)(h < 10 ? '0' + h : 'a' + h - 10); }
    b[9] = 0;
    say(b);
}
static void say_dec(u32 v)
{
    char b[12]; int i = 11;
    b[i] = 0;
    do { b[--i] = (char)('0' + v % 10); v /= 10; } while (v);
    say(b + i);
}

/* ------------------------------------------------------------------ GTE */
static u32 gd[32], gc[32];   /* data / control registers, stored raw */
static u32 g_width_mode;     /* value delivered for the D_8009FA0C load */
static int g_trap_error;

static s32 sx16(u32 v) { return (s32)(s16)v; }

static u32 rd_data(int r)
{
    switch (r) {
    case 1: case 3: case 5: case 8: case 9: case 10: case 11: return (u32)sx16(gd[r]);
    case 7: case 16: case 17: case 18: case 19: return gd[r] & 0xFFFF;
    case 15: return gd[14];
    default: return gd[r];
    }
}
static void wr_data(int r, u32 v)
{
    if (r == 15) { gd[12] = gd[13]; gd[13] = gd[14]; gd[14] = v; return; }
    gd[r] = v;
}
static u32 rd_ctrl(int r)
{
    switch (r) {
    case 4: case 12: case 20: case 26: case 27: case 29: case 30: return (u32)sx16(gc[r]);
    default: return gc[r];
    }
}
static void wr_ctrl(int r, u32 v)
{
    if (r == 31) { v &= 0x7FFFF000u; if (v & 0x7F87E000u) v |= 0x80000000u; }
    gc[r] = v;
}
/* element (i,j) of matrix block base (0 = RT, 8 = LLM, 16 = LCM) */
static s32 mat_el(int base, int i, int j)
{
    int idx = i * 3 + j;
    u32 w = gc[base + idx / 2];
    return (idx & 1) ? (s32)(s16)(w >> 16) : (s32)(s16)w;
}
static s32 sat_ir(long long v, int lm, u32 *fl, u32 bit)
{
    long long lo = lm ? 0 : -0x8000;
    if (v > 0x7FFF) { *fl |= bit; return 0x7FFF; }
    if (v < lo) { *fl |= bit; return (s32)lo; }
    return (s32)v;
}
static void mac_ovf(long long v, int i, u32 *fl)
{
    if (v > 0x7FFFFFFFFFFLL) *fl |= 0x40000000u >> i;
    else if (v < -0x80000000000LL) *fl |= 0x08000000u >> i;
}
static void finish_flag(u32 fl)
{
    if (fl & 0x7F87E000u) fl |= 0x80000000u;
    gc[31] = fl;
}

static int gte_cmd(u32 insn)
{
    int sf = (insn >> 19) & 1, lm = (insn >> 10) & 1, i;
    u32 fl = 0;
    if ((insn & 0x3F) == 0x12) { /* MVMVA */
        int mx = (insn >> 17) & 3, v = (insn >> 15) & 3, cv = (insn >> 13) & 3;
        s32 vec[3], ir[3];
        if (mx == 3 || cv == 2) return -1; /* garbage matrix / FC bug: unused here */
        if (v == 3) { vec[0] = sx16(gd[9]); vec[1] = sx16(gd[10]); vec[2] = sx16(gd[11]); }
        else { vec[0] = sx16(gd[v * 2]); vec[1] = sx16(gd[v * 2] >> 16); vec[2] = sx16(gd[v * 2 + 1]); }
        for (i = 0; i < 3; i++) {
            long long mac = 0;
            if (cv != 3) mac = (long long)(s32)gc[(cv == 0 ? 5 : 13) + i] * 4096;
            mac += (long long)mat_el(mx * 8, i, 0) * vec[0] + (long long)mat_el(mx * 8, i, 1) * vec[1]
                 + (long long)mat_el(mx * 8, i, 2) * vec[2];
            mac_ovf(mac, i, &fl);
            if (sf) mac >>= 12;
            gd[25 + i] = (u32)(s32)mac;
            ir[i] = sat_ir(mac, lm, &fl, 0x01000000u >> i);
            gd[9 + i] = (u32)ir[i];
        }
        finish_flag(fl);
        return 0;
    }
    if ((insn & 0x3F) == 0x01) { /* RTPS */
        s32 vx = sx16(gd[0]), vy = sx16(gd[0] >> 16), vz = sx16(gd[1]);
        s32 ir[3], sz3, q, ovf, sx, sy;
        long long raw3 = 0, mac0;
        for (i = 0; i < 3; i++) {
            long long mac = (long long)(s32)gc[5 + i] * 4096 + (long long)mat_el(0, i, 0) * vx
                          + (long long)mat_el(0, i, 1) * vy + (long long)mat_el(0, i, 2) * vz;
            mac_ovf(mac, i, &fl);
            if (i == 2) raw3 = mac;
            if (sf) mac >>= 12;
            gd[25 + i] = (u32)(s32)mac;
            if (i < 2) ir[i] = sat_ir(mac, lm, &fl, 0x01000000u >> i);
            else {
                u32 dummy = 0;
                ir[2] = sat_ir(mac, lm, &dummy, 1);           /* value per lm */
                sat_ir(raw3 >> 12, 0, &fl, 0x00400000u);      /* flag from MAC3>>12 */
            }
            gd[9 + i] = (u32)ir[i];
        }
        raw3 >>= 12;
        if (raw3 > 0xFFFF) { fl |= 0x40000; sz3 = 0xFFFF; }
        else if (raw3 < 0) { fl |= 0x40000; sz3 = 0; }
        else sz3 = (s32)raw3;
        gd[16] = gd[17]; gd[17] = gd[18]; gd[18] = gd[19]; gd[19] = (u32)sz3;
        q = gte_quot((s32)(gc[26] & 0xFFFF), sz3, &ovf);
        if (ovf) fl |= 0x20000;
        mac0 = (long long)ir[0] * q + (s32)gc[24];
        if (mac0 > 0x7FFFFFFFLL) fl |= 0x10000; else if (mac0 < -0x80000000LL) fl |= 0x8000;
        mac0 >>= 16;
        if (mac0 > 0x3FF) { fl |= 0x4000; sx = 0x3FF; } else if (mac0 < -0x400) { fl |= 0x4000; sx = -0x400; } else sx = (s32)mac0;
        mac0 = (long long)ir[1] * q + (s32)gc[25];
        if (mac0 > 0x7FFFFFFFLL) fl |= 0x10000; else if (mac0 < -0x80000000LL) fl |= 0x8000;
        mac0 >>= 16;
        if (mac0 > 0x3FF) { fl |= 0x2000; sy = 0x3FF; } else if (mac0 < -0x400) { fl |= 0x2000; sy = -0x400; } else sy = (s32)mac0;
        gd[12] = gd[13]; gd[13] = gd[14]; gd[14] = ((u32)sy << 16) | ((u32)sx & 0xFFFF);
        mac0 = (long long)sx16(gc[27]) * q + (s32)gc[28];
        if (mac0 > 0x7FFFFFFFLL) fl |= 0x10000; else if (mac0 < -0x80000000LL) fl |= 0x8000;
        gd[24] = (u32)(s32)mac0;
        mac0 >>= 12;
        if (mac0 > 0x1000) { fl |= 0x1000; mac0 = 0x1000; } else if (mac0 < 0) { fl |= 0x1000; mac0 = 0; }
        gd[8] = (u32)mac0;
        finish_flag(fl);
        return 0;
    }
    return -1;
}

/* o32 ucontext: uc_mcontext (struct sigcontext) at +24; sc_pc (u64) at +32,
 * sc_regs[32] (u64 each) at +40. */
static u32 get_reg(unsigned char *uc, int r) { return *(u32 *)(uc + 40 + 8 * r); }
static void set_reg(unsigned char *uc, int r, u32 v)
{
    u32 *p = (u32 *)(uc + 40 + 8 * r);
    if (r == 0) return;
    p[0] = v; p[1] = (v & 0x80000000u) ? 0xFFFFFFFFu : 0;
}
static void trap_fail(u32 pc, u32 insn)
{
    say("env: unhandled trap at"); say_hex(pc); say_hex(insn); say("\n");
    sys4(4001, 3, 0, 0, 0);
}
static void on_sigill(int sig, void *info, void *ucv)
{
    unsigned char *uc = ucv;
    u32 *pcp = (u32 *)(uc + 32);
    u32 pc = pcp[0], insn = *(u32 *)pc;
    int rt = (insn >> 16) & 31, rd = (insn >> 11) & 31;
    (void)sig; (void)info;
    if ((insn >> 26) != 0x12) trap_fail(pc, insn);
    if (insn & 0x02000000u) { if (gte_cmd(insn & 0x01FFFFFFu) < 0) trap_fail(pc, insn); }
    else switch ((insn >> 21) & 31) {
    case 0: set_reg(uc, rt, rd_data(rd)); break;          /* mfc2 */
    case 2: set_reg(uc, rt, rd_ctrl(rd)); break;          /* cfc2 */
    case 4: wr_data(rd, get_reg(uc, rt)); break;          /* mtc2 */
    case 6: wr_ctrl(rd, get_reg(uc, rt)); break;          /* ctc2 */
    default: trap_fail(pc, insn);
    }
    pcp[0] = pc + 4; pcp[1] = 0xFFFFFFFFu;
}
static void on_sigsegv(int sig, void *info, void *ucv)
{
    unsigned char *uc = ucv;
    u32 *pcp = (u32 *)(uc + 32);
    u32 pc = pcp[0], insn = *(u32 *)pc;
    (void)sig; (void)info;
    /* lw $t2, %lo(D_8009FA0C)($t2) with $t2 = 0x800A0000 (func_8001047C) */
    if (insn != 0x8D4AFA0Cu || get_reg(uc, 10) != 0x800A0000u) { g_trap_error = 1; trap_fail(pc, insn); }
    set_reg(uc, 10, g_width_mode);
    pcp[0] = pc + 4; pcp[1] = 0xFFFFFFFFu;
}
struct ksa { u32 flags; void (*handler)(int, void *, void *); u32 mask[4]; };
static int install(int sig, void (*h)(int, void *, void *))
{
    struct ksa sa; int i;
    sa.flags = 8; /* SA_SIGINFO */
    sa.handler = h;
    for (i = 0; i < 4; i++) sa.mask[i] = 0;
    return (int)sys4(4194, sig, (long)&sa, 0, 16);
}

/* -------------------------------------------------------------- vectors */
#define MAXV 40
static s32 v[MAXV];
static int line_no, failures, ncase;

static void fail(const char *tag, int field, u32 got, u32 want)
{
    say("env: FAIL "); say(tag); say(" line "); say_dec((u32)line_no);
    say(" field "); say_dec((u32)field); say(" got"); say_hex(got); say(" want"); say_hex(want); say("\n");
    failures++;
}
static int check(const char *tag, int field, u32 got, u32 want)
{
    if (got != want) { fail(tag, field, got, want); return 1; }
    return 0;
}
static int streq(const char *a, const char *b) { while (*a && *a == *b) { a++; b++; } return *a == *b; }

#define SENT 0x5A5A5A5A

static void load_matrix12(s32 *w, int n) { int i; for (i = 0; i < n; i++) w[i] = v[i]; }

static void run_case(const char *tag, int n)
{
    int i, bad = 0;
    s32 m[12], a[12], b[12];
    if (streq(tag, "SINCOS") && n == 3) {
        s32 s = SENT, c = SENT;
        func_8001846C((u32)v[0], &s, &c);
        bad |= check(tag, 1, (u32)s, (u32)v[1]);
        bad |= check(tag, 2, (u32)c, (u32)v[2]);
    } else if ((streq(tag, "ROT") || streq(tag, "ROTV")) && n == 12) {
        for (i = 0; i < 12; i++) m[i] = SENT;
        if (tag[3] == 'V') func_80018DC0(v[0], v[1], v[2], m); else func_800188D0(v[0], v[1], v[2], m);
        for (i = 0; i < 9; i++) bad |= check(tag, 3 + i, (u32)m[i], (u32)v[3 + i]);
        for (i = 9; i < 12; i++) bad |= check(tag, 100 + i, (u32)m[i], SENT); /* words 9..11 untouched */
    } else if (streq(tag, "TRANSPOSE") && n == 18) {
        load_matrix12(a, 9);
        for (i = 0; i < 9; i++) b[i] = SENT;
        func_80019470(a, b);
        for (i = 0; i < 9; i++) bad |= check(tag, 9 + i, (u32)b[i], (u32)v[9 + i]);
    } else if (streq(tag, "MULMV") && n == 15) {
        s32 in[3], out[3];
        load_matrix12(a, 9);
        for (i = 0; i < 3; i++) { in[i] = v[9 + i]; out[i] = SENT; }
        func_8001A81C(a, in, out);
        for (i = 0; i < 3; i++) bad |= check(tag, 12 + i, (u32)out[i], (u32)v[12 + i]);
    } else if (streq(tag, "VIEW") && n == 24) {
        load_matrix12(a, 12);
        for (i = 0; i < 12; i++) b[i] = SENT;
        func_80018860(a, b);
        for (i = 0; i < 12; i++) bad |= check(tag, 12 + i, (u32)b[i], (u32)v[12 + i]);
    } else if (streq(tag, "LOADMAT") && n == 20) {
        load_matrix12(a, 12);
        for (i = 0; i < 8; i++) gc[i] = SENT;
        func_80010750(a);
        for (i = 0; i < 8; i++) bad |= check(tag, 12 + i, gc[i], (u32)v[12 + i]);
    } else if (streq(tag, "LOADROT") && n == 17) {
        load_matrix12(a, 9);
        for (i = 0; i < 8; i++) gc[i] = SENT;
        func_80010820(a);
        for (i = 0; i < 8; i++) bad |= check(tag, 9 + i, gc[i], (u32)v[9 + i]);
    } else if (streq(tag, "XF6C") && n == 22) {
        /* matrix w0..w8 via func_80010820, count, two 12-byte vertices, 6 expected words */
        load_matrix12(a, 9);
        func_80010820(a);
        for (i = 0; i < 6; i++) { b[i] = v[10 + i]; m[i] = SENT; }
        func_8001006C(b, m, v[9]);
        for (i = 0; i < 6; i++) bad |= check(tag, 16 + i, (u32)m[i], (u32)v[16 + i]);
    } else if ((streq(tag, "XF10C") || streq(tag, "XF1D4") || streq(tag, "XF234")) && n == 25) {
        /* matrix w0..w11 via func_80010750, count, two 8-byte vertices, 6 expected values */
        s16 src[8], dst[12];
        load_matrix12(a, 12);
        func_80010750(a);
        for (i = 0; i < 2; i++) { src[i * 4] = (s16)v[13 + i * 3]; src[i * 4 + 1] = (s16)v[14 + i * 3]; src[i * 4 + 2] = (s16)v[15 + i * 3]; src[i * 4 + 3] = 0; }
        for (i = 0; i < 12; i++) dst[i] = (s16)SENT;
        if (tag[2] == '2') {
            s32 *dw = (s32 *)m;
            for (i = 0; i < 6; i++) dw[i] = SENT;
            func_80010234(src, dw, v[12]);
            for (i = 0; i < 6; i++) bad |= check(tag, 19 + i, (u32)dw[i], (u32)v[19 + i]);
        } else {
            if (tag[2] == '1' && tag[3] == '0') func_8001010C(src, dst, v[12]); else func_800101D4(src, dst, v[12]);
            for (i = 0; i < 2; i++) {
                bad |= check(tag, 19 + i * 3, (u32)(s32)dst[i * 4], (u32)v[19 + i * 3]);
                bad |= check(tag, 20 + i * 3, (u32)(s32)dst[i * 4 + 1], (u32)v[20 + i * 3]);
                bad |= check(tag, 21 + i * 3, (u32)(s32)dst[i * 4 + 2], (u32)v[21 + i * 3]);
                bad |= check(tag, 200 + i, (u32)(s32)dst[i * 4 + 3], (u32)(s32)(s16)SENT); /* pad untouched */
            }
        }
    } else if (streq(tag, "PROJ") && n == 30) {
        /* w0..w11, H, OFX, OFY, wmode, count, 2 vertices; expect sxy0 sz0 sxy1 sz1 clip0 clip1 ret */
        s16 src[8]; u32 out[4]; u8 clip[4]; u32 ret;
        load_matrix12(a, 12);
        func_80010750(a);
        gc[26] = (u32)v[12]; gc[24] = (u32)v[13]; gc[25] = (u32)v[14]; gc[27] = 0; gc[28] = 0;
        g_width_mode = (u32)v[15];
        for (i = 0; i < 2; i++) { src[i * 4] = (s16)v[17 + i * 3]; src[i * 4 + 1] = (s16)v[18 + i * 3]; src[i * 4 + 2] = (s16)v[19 + i * 3]; src[i * 4 + 3] = 0; }
        for (i = 0; i < 4; i++) { out[i] = SENT; clip[i] = 0x5A; }
        ret = func_8001047C(src, out, clip, v[16]);
        for (i = 0; i < 4; i++) bad |= check(tag, 23 + i, out[i], (u32)v[23 + i]);
        bad |= check(tag, 27, clip[0], (u32)v[27]);
        bad |= check(tag, 28, clip[1], (u32)v[28]);
        bad |= check(tag, 29, ret, (u32)v[29]);
        bad |= check(tag, 300, clip[2], 0x5A);
    } else if (streq(tag, "COMPOSE") && n == 32) {
        /* parent 12 words, flags, offset, x y z rx ry rz (fields), expected 12 words of slot 1.
         * Animation table (fixed, documented in env.vectors): */
        static s16 table[8] = { 0, 0, 0x100, 16, -16, 0x400, 0, 0 };
        struct Input e;
        for (i = 0; i < 12; i++) { D_800A0300[i] = v[i]; D_800A0300[12 + i] = SENT; }
        e.pad0[0] = e.pad0[1] = e.pad0[2] = 0; e.pad6 = 0; e.flags = (u8)v[12];
        e.x = (s16)v[14]; e.y = (s16)v[15]; e.z = (s16)v[16];
        e.rx = (s16)v[17]; e.ry = (s16)v[18]; e.rz = (s16)v[19];
        func_80013CB8(1, &e, table, v[13]);
        for (i = 0; i < 12; i++) bad |= check(tag, 20 + i, (u32)D_800A0300[12 + i], (u32)v[20 + i]);
    } else {
        say("env: malformed vector line "); say_dec((u32)line_no); say("\n");
        sys4(4001, 2, 0, 0, 0);
    }
    (void)bad;
    ncase++;
}

static const char *tags[] = { "SINCOS", "ROT", "ROTV", "TRANSPOSE", "MULMV", "VIEW", "LOADMAT", "LOADROT",
                              "XF6C", "XF10C", "XF1D4", "XF234", "PROJ", "COMPOSE", 0 };
static int per_tag[16];

int main(int argc, char **argv)
{
    static char data[65536];
    int fd, n, i, k, neg, f0;
    char *p, tag[16];
    if (argc != 2) { say("env: missing vectors\n"); return 2; }
    if (install(4, on_sigill) < 0 || install(10, on_sigsegv) < 0 || install(11, on_sigsegv) < 0) {
        say("env: sigaction failed\n"); return 2;
    }
    fd = (int)sys4(4005, (long)argv[1], 0, 0, 0);
    if (fd < 0) { say("env: cannot open vectors\n"); return 2; }
    n = (int)sys4(4003, fd, (long)data, sizeof(data) - 1, 0);
    sys4(4006, fd, 0, 0, 0);
    if (n <= 0 || n >= (int)sizeof(data) - 1) { say("env: read error\n"); return 2; }
    data[n] = 0; p = data; line_no = 1;
    while (*p) {
        if (*p == '\n') { line_no++; p++; continue; }
        if (*p == ' ' || *p == '\t' || *p == '\r') { p++; continue; }
        if (*p == '#') { while (*p && *p != '\n') p++; continue; }
        for (i = 0; i < 15 && *p > ' '; i++) tag[i] = *p++;
        tag[i] = 0;
        k = 0;
        for (;;) {
            u32 x = 0; int digits = 0;
            while (*p == ' ' || *p == '\t' || *p == '\r') p++;
            if (!*p || *p == '\n' || *p == '#') break;
            neg = 0;
            if (*p == '-') { neg = 1; p++; }
            if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
                p += 2;
                for (;;) {
                    char c = *p;
                    if (c >= '0' && c <= '9') f0 = c - '0';
                    else if (c >= 'a' && c <= 'f') f0 = c - 'a' + 10;
                    else if (c >= 'A' && c <= 'F') f0 = c - 'A' + 10;
                    else break;
                    x = (x << 4) | (u32)f0; p++; digits++;
                }
            } else {
                while (*p >= '0' && *p <= '9') { x = x * 10 + (u32)(*p - '0'); p++; digits++; }
            }
            if (!digits || (*p > ' ' && *p != '#') || k >= MAXV) {
                say("env: bad number on line "); say_dec((u32)line_no); say("\n"); return 2;
            }
            v[k++] = neg ? -(s32)x : (s32)x;
        }
        for (i = 0; tags[i]; i++) if (streq(tag, tags[i])) per_tag[i]++;
        run_case(tag, k);
    }
    for (i = 0; tags[i]; i++) {
        say("env: "); say(tags[i]); say(" "); say_dec((u32)per_tag[i]); say(" cases\n");
    }
    if (!ncase || failures || g_trap_error) {
        say("env: FAIL ("); say_dec((u32)failures); say(" mismatches)\n"); return 1;
    }
    say("env: all "); say_dec((u32)ncase);
    say(" vectors PASS (retail C + retail GTE asm words, cop2 via gte_ref-model trap)\n");
    return 0;
}
