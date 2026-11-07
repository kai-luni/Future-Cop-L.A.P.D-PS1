typedef short s16;
typedef int s32;
typedef unsigned char u8;
extern void func_8001010C(s16 *, s16 *, s32);
extern s32 func_80019904(s32, s32);
void func_80016C88(s16 *p, s16 *n, s32 *q, u8 *out)
{
    s16 d[4], r[4], v[4];
    s32 x, y, z, dot, c;
    register s32 len __asm__("$2");
    register s32 a __asm__("$4");
    s32 b;
    x = (q[0] - (p[0] << 4)) >> 4;
    d[0] = x;
    y = (q[1] - (p[1] << 4)) >> 4;
    d[1] = y;
    z = (q[2] - (p[2] << 4)) >> 4;
    d[2] = z;
    dot = (d[0] * n[0] + d[1] * n[1] + d[2] * n[2]) >> 12;
    r[0] = ((dot * n[0]) >> 11) - x;
    r[1] = ((dot * n[1]) >> 11) - y;
    r[2] = ((dot * n[2]) >> 11) - z;
    func_8001010C(r, v, 1);
    len = (v[0] * v[0] + v[1] * v[1] + v[2] * v[2]) >> 12;
    b = a = 0x1000;
    if (len > 0) {
        len = func_80019904(len << 4, 0x1000) >> 4;
        b = ((v[0] * len) >> 12) + 0x1000;
        len = (v[2] * len) >> 12;
        a = len + 0x1000;
    }
    if (b < 0) goto low1;
    if (b >= 0x1F80) goto high1;
    c = b; goto end1;
 high1: c = 0x1F7F; goto end1;
 low1: c = 0;
 end1:
    b = c;
    if (a < 0) goto low2;
    if (a >= 0x1F80) goto high2;
    c = a; goto end2;
 high2: c = 0x1F7F; goto end2;
 low2: c = 0;
 end2:
    out[0] = (b >> 6) + 1;
    out[1] = (c >> 6) + 0x7D;
}
