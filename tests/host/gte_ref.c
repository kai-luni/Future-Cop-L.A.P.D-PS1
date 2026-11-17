/* Derived GTE mirror for the host harness (test-only, NOT retail code).
 * Mirrors the retail handwritten asm units: func_80010750 (matrix load),
 * func_8001010C (mvmva sf=1, no TR, Y negated store), func_8001047C (rtps +
 * clip byte). The .NET port (FutureCop.Assets/Gte.cs) implements the same
 * semantics; both are asserted against tests/golden/render.vectors.
 * The unsigned divide is the rounded quotient H*65536/SZ3 with the GTE
 * overflow rule H >= 2*SZ3 (UNR-equivalent, verified against psx-spx model). */
typedef unsigned int u32;
typedef int s32;
typedef short s16;

static s16 clamp16(long long v, u32 *flag, u32 bit)
{
    if (v > 0x7FFF) { *flag |= bit; return 0x7FFF; }
    if (v < -0x8000) { *flag |= bit; return (s16)-0x8000; }
    return (s16)v;
}

static s32 clamp_screen(long long v, u32 *flag, u32 bit)
{
    if (v > 0x3FF) { *flag |= bit; return 0x3FF; }
    if (v < -0x400) { *flag |= bit; return -0x400; }
    return (s32)v;
}

/* func_80010750: R = F*M^T, F = diag(1,-1,1); row 1 negates before >>4, wraps. */
void gte_load(const s32 *w, const s32 *t, s16 *r, s32 *tr)
{
    int i, j;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
        {
            s32 v = w[j * 3 + i];
            r[i * 3 + j] = i == 1 ? (s16)((-v) >> 4) : (s16)(v >> 4);
        }
    tr[0] = t[0] >> 4;
    tr[1] = (-t[1]) >> 4;
    tr[2] = t[2] >> 4;
}

/* func_8001010C: stored (IR1, -IR2, IR3). */
void gte_mvmva(const s16 *r, s16 vx, s16 vy, s16 vz, s16 *out, u32 *flag)
{
    s16 ir[3];
    int i;
    *flag = 0;
    for (i = 0; i < 3; i++)
    {
        long long mac = (long long)r[i * 3] * vx + (long long)r[i * 3 + 1] * vy + (long long)r[i * 3 + 2] * vz;
        ir[i] = clamp16(mac >> 12, flag, 0x1000000u >> i);
    }
    out[0] = ir[0];
    out[1] = (s16)-ir[1];
    out[2] = ir[2];
}

s32 gte_quot(s32 h, s32 sz3, s32 *overflow)
{
    /* H is a 16-bit register, so H*65536 + SZ3/2 fits u32: plain divu, no libgcc. */
    u32 num, q;
    *overflow = h >= 2 * sz3;
    if (*overflow) return 0x1FFFF;
    num = (u32)h * 65536u + (u32)(sz3 / 2);
    q = num / (u32)sz3;
    return q > 0x1FFFF ? 0x1FFFF : (s32)q;
}

/* func_8001047C per vertex: rtps, SXY2/SZ3/FLAG, clip byte, written flag. */
void gte_rtps(const s16 *r, const s32 *tr, s16 vx, s16 vy, s16 vz,
              s32 h, s32 ofx, s32 ofy, s32 wmode,
              s32 *sxy, s32 *sz3out, u32 *clip, u32 *flag, s32 *written)
{
    u32 fl = 0;
    long long mac[3];
    s32 ir[3];
    s32 sz3, q, ovf, sx, sy;
    int i;
    for (i = 0; i < 3; i++)
    {
        mac[i] = ((long long)tr[i] * 4096 + (long long)r[i * 3] * vx +
                  (long long)r[i * 3 + 1] * vy + (long long)r[i * 3 + 2] * vz) >> 12;
        ir[i] = clamp16(mac[i], &fl, 0x1000000u >> i);
    }
    if (mac[2] > 0xFFFF) { fl |= 0x40000; sz3 = 0xFFFF; }
    else if (mac[2] < 0) { fl |= 0x40000; sz3 = 0; }
    else sz3 = (s32)mac[2];
    q = gte_quot(h, sz3, &ovf);
    if (ovf) fl |= 0x20000;
    sx = clamp_screen((ofx + (long long)ir[0] * q) >> 16, &fl, 0x4000);
    sy = clamp_screen((ofy + (long long)ir[1] * q) >> 16, &fl, 0x2000);
    *sxy = (sy << 16) | (sx & 0xFFFF);
    *sz3out = sz3;
    *flag = fl;
    if (fl & 0x01C66000u)
    {
        *clip = 1;
        *written = 0;
    }
    else
    {
        s32 w = wmode == 1 ? 0x140 : 0xA0;
        *clip = (u32)((sz3 < 0x400 ? 1 : 0) |
                      ((s16)(sx & 0xFFFF) < w ? 0x40 : 0) |
                      ((s16)(sy & 0xFFFF) < 0xF0 ? 0x80 : 0));
        *written = 1;
    }
}