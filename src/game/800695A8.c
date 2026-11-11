#include "game/types.h"

struct Map {
    s32 pad00;
    s32 w;
    s32 h;
    u8 pad0c[0xC];
    s32 border;
    u8 pad1c[8];
    s32 cell[1];
};

extern struct Map *D_8009F80C;
extern s8 **D_8009F808;
extern s32 D_800A2940[];
extern s8 *func_80017EBC(s32, s32);
extern void func_800181D4(s8 *);

#define MAX(a, b) ((a) - ((((a) - (b)) >> 31) & ((a) - (b))))
#define MIN(a, b) (((((a) - (b)) >> 31) & ((a) - (b))) + (b))

#define FILT(dst, a0, a1, a2, b0, b1, b2, c0, c1, c2, mc) do { \
    sum = ((a0) + 2 * (a1) + (a2)); \
    __asm__("" : "+r"(sum)); \
    sum += (2 * (b0) + 4 * (b1) + 2 * (b2)); \
    __asm__("" : "+r"(sum)); \
    sum += ((c0) + 2 * (c1) + (c2)); \
    sum += 8; \
    sum >>= 4; \
    if (sum < -128) cv = -128; \
    else if (sum < 128) cv = sum; \
    else cv = 127; \
    sum = cv; \
    if (sum < (mc)) sum = (mc); \
    *(dst) = sum; \
} while (0)

#define FILT1(dst, a0, a1, a2, b0, b1, b2, c0, c1, c2, mc) do { \
    register s32 first0 __asm__("$4"); \
    first0 = (a0); \
    sum = (first0 + 2 * first0 + (a2)); \
    __asm__("" : "+r"(sum)); \
    sum += (2 * (b0) + 4 * (b1) + 2 * (b2)); \
    __asm__("" : "+r"(sum)); \
    sum += ((c0) + 2 * (c1) + (c2)); \
    sum += 8; \
    sum >>= 4; \
    if (sum >= -128) { \
        o = out + row; \
        if (sum < 128) cv = sum; else cv = 127; \
    } else { \
        o = out + row; \
        cv = -128; \
    } \
    sum = cv; \
    if (sum < (mc)) sum = (mc); \
    *(dst) = sum; \
} while (0)

void func_800695A8(void)
{
    register struct Map *map __asm__("$3");
    register s32 w __asm__("$16");
    register s32 h __asm__("$17");
    register s32 base __asm__("$18");
    register s32 rb __asm__("$19");
    register s32 fb __asm__("$15");
    register s8 *buf __asm__("$20");
    register s8 *out __asm__("$21");
    register s32 prod __asm__("$22");
    register s32 mw __asm__("$4");
    register s32 mb __asm__("$2");
    register s32 mh __asm__("$3");
    register s32 fi __asm__("$7");
    register s32 x __asm__("$15");
    register s32 row __asm__("$18");
    register s32 neg __asm__("$14");
    register s32 limit __asm__("$19");
    register s32 c __asm__("$11");
    register s32 mv __asm__("$4");
    register s32 m __asm__("$3");
    register s32 fx8 __asm__("$25");
    register s32 bd __asm__("$3");
    register s32 cv __asm__("$3");
    s32 oj, fj, x8, W, H, j, i, sum, up, dn, v, off;
    register s32 wt __asm__("$2");
    register s32 fy __asm__("$11");
    register s32 dn0 __asm__("$3");
    register s8 *d __asm__("$12");
    register s8 *nu __asm__("$8");
    register s8 *nd __asm__("$9");
    register s8 *nm __asm__("$10");
    s8 *b, *pp, *p, *a, *o;
    register s8 *u __asm__("$6");
    register s8 *mid __asm__("$12");
    register s8 *dp __asm__("$7");

    map = D_8009F80C;
    mw = map->w;
    mb = map->border;
    mh = map->h;
    w = mw - 1;
    mb = mb * 2;
    w = w - mb;
    w = w + 2;
    h = mh - 1;
    h = h - mb;
    h = h + 2;
    prod = w * h;
    buf = func_80017EBC(prod << 6, 1);
    bd = D_8009F80C->border;
    base = bd - 1;
    for (fb = 0; fb < h; fb++) {
        fy = 0;
        if (w > 0) {
        rb = fb + base;
        fx8 = fb * 8;
        do {
            register s32 fj __asm__("$10");
            register s32 oj __asm__("$13");
            register s32 x8 __asm__("$24");
            register s32 off __asm__("$8");
            wt = D_8009F80C->w;
            __asm__("mult %1,%2\n\tmove %0,$0" : "=r"(fj) : "r"(wt), "r"(rb));
            __asm__("sll %0,%1,3" : "=r"(x8) : "r"(fy), "r"(fj));
            __asm__("addu %0,%1,%2" : "=r"(wt) : "r"(fy), "r"(base), "r"(x8));
            __asm__("mflo %0" : "=r"(prod) : "r"(wt));
            v = D_8009F80C->cell[wt + prod];
            v = (v >> 2) & 0x3FFF;
            __asm__("" : "+r"(v));
            oj = fj;
            p = D_8009F808[v] + 0xC;
            do {
                wt = (fx8 + fj) << 3;
                __asm__("mult %1,%2\n\tmove %0,$0" : "=r"(fi) : "r"(w), "r"(wt));
                off = oj;
                __asm__("mflo %0" : "=r"(prod) : "r"(off), "r"(fi));
                wt = prod + x8;
                d = buf + wt;
                do {
                    a = p + off;
                    b = a + 0x33;
                    mv = -128;
                    for (m = 0; m < 6; m++) {
                        if (mv < a[m]) mv = a[m];
                        if (mv < b[m]) mv = b[m];
                    }
                    d[fi] = mv;
                    off += 6;
                } while (++fi < 8);
                fj++;
                oj += 0x66;
            } while (fj < 8);
        } while (++fy < w);
        }
    }
    W = w << 3;
    H = h << 3;
    out = func_80017EBC(W * H, 2);
    x = 0;
    if (x < H) {
        limit = W - 1;
        row = x;
        neg = 1;
        do {
        {
            register s32 a0v __asm__("$4");
            register s32 b1v __asm__("$2");
            register s32 dnW __asm__("$3");
            b1v = -((neg >> 31) & neg);
            a0v = b1v * W;
            dn0 = H - 2;
            dn0 = x - dn0;
            b1v = (dn0 >> 31) & dn0;
            __asm__("" : "+r"(a0v), "+r"(b1v));
            b1v = b1v - 1;
            dn = b1v + H;
            __asm__("mult %0,%1" : : "r"(dn), "r"(W), "r"(a0v));
            u = buf + a0v;
            a0v = u[0];
            __asm__("addu %0,%1,%2" : "=r"(mid) : "r"(buf), "r"(row), "r"(a0v));
            a0v += a0v << 1;
            __asm__("lb %0,1(%1)" : "=r"(b1v) : "r"(u), "r"(a0v));
            sum = a0v + b1v;
            __asm__("lb %0,1(%1)" : "=r"(b1v) : "r"(mid), "r"(sum));
            __asm__("mflo %0\n\taddu %1,%2,%0" : "=&r"(dnW), "=r"(dp) : "r"(buf), "r"(b1v));
            {
                register s32 b0v __asm__("$3");
                register s32 mbv __asm__("$4");
                __asm__("lb %0,0(%1)" : "=r"(b0v) : "r"(mid), "r"(dp));
                b1v <<= 1;
                __asm__("sll %0,%1,1" : "=r"(mbv) : "r"(b0v), "r"(b1v));
                b0v <<= 2;
                mbv += b0v;
                mbv += b1v;
                sum += mbv;
            }
            __asm__("" : "+r"(sum), "+r"(u), "+r"(mid), "+r"(dp));
            sum += (dp[0] + 2 * dp[0] + dp[1]);
            sum += 8;
            sum >>= 4;
            if (sum >= -128) {
                o = out + row;
                if (sum < 128) cv = sum; else cv = 127;
            } else {
                o = out + row;
                cv = -128;
            }
            sum = cv;
            if (sum < mid[0]) sum = mid[0];
            *o = sum;
        }
        o++;
        u++;
        dp++;
        mid++;
        c = 1;
        if (c < limit) {
            nm = mid + 1;
            nd = dp + 1;
            nu = u + 1;
            do {
                register s32 cv __asm__("$4");
                register s32 m0 __asm__("$3");
                sum = (nu[-2] + 2 * u[0] + nu[0]);
                __asm__("" : "+r"(sum));
                m0 = nm[-2] * 2;
                sum += (m0 + 4 * mid[0] + 2 * nm[0]);
                __asm__("" : "+r"(sum));
                cv = dp[0] * 2;
                sum += (nd[-2] + cv + nd[0]);
                sum += 8;
                sum >>= 4;
                if (sum < -128) cv = -128;
                else if (sum < 128) cv = sum;
                else cv = 127;
                sum = cv;
                if (sum < mid[0]) sum = mid[0];
                *o = sum;
                o++;
                nu++;
                u++;
                nd++;
                dp++;
                nm++;
                mid++;
                c++;
            } while (c < limit);
        }
        FILT(o, u[-1], u[0], u[0], mid[-1], mid[0], mid[0], dp[-1], dp[0], dp[0], mid[0]);
        row += W;
        x++;
        neg--;
        } while (x < H);
    }
    func_800181D4(buf);
    D_800A2940[3] = (s32)out;
    D_800A2940[0] = ((D_8009F80C->border - 1) << 17) + 0x2000;
    D_800A2940[1] = W;
    D_800A2940[2] = H - 2;
}
