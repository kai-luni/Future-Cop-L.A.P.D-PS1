typedef int s32;
typedef unsigned int u32;
extern void func_8001846C(u32, s32 *, s32 *);

void func_80018DC0(s32 x, s32 y, s32 z, s32 *m)
{
    s32 sx, cx, sy, cy, sz, cz;
    z &= 0xFFF;
    y &= 0xFFF;
    x &= 0xFFF;
    if (z == 0) {
        if (y == 0) {
            m[4] = 0x10000;
            m[7] = 0;
            m[5] = 0;
            m[3] = 0;
            m[1] = 0;
            if (x != 0) {
                func_8001846C(x, &sx, &cx);
                {
                    register s32 c __asm__("$3");
                    register s32 s ;
                    c = cx;
                    m[0] = c;
                    s = sx;
                    m[8] = c;
                    m[6] = s;
                    m[2] = -s;
                }
            } else {
                m[8] = 0x10000;
                m[0] = 0x10000;
                m[6] = 0;
                m[2] = 0;
            }
        } else {
            func_8001846C(y, &sy, &cy);
            if (x == 0) {
                register s32 c __asm__("$3");
                register s32 s ;
                m[0] = 0x10000;
                m[6] = 0;
                m[3] = 0;
                __asm__ volatile("" : : : "memory");
                c = cy;
                s = sy;
                m[2] = 0;
                m[1] = 0;
                m[5] = s;
                m[8] = c;
                m[4] = c;
                m[7] = -s;
            } else {
                func_8001846C(x, &sx, &cx);
                m[0] = cx;
                m[1] = 0;
                m[2] = -sx;
                m[4] = cy;
                m[7] = -sy;
                sy >>= 2;
                cy >>= 2;
                sx >>= 2;
                cx >>= 2;
                m[3] = (sy * sx) >> 12;
                m[5] = (sy * cx) >> 12;
                m[6] = (cy * sx) >> 12;
                m[8] = (cy * cx) >> 12;
            }
        }
    } else {
        func_8001846C(z, &sz, &cz);
        if (y == 0) {
            if (x == 0) {
                register s32 c __asm__("$4");
                register s32 s __asm__("$3");
                m[6] = 0;
                __asm__ volatile("" : : : "memory");
                c = cz;
                m[0] = c;
                s = sz;
                m[8] = 0x10000;
                m[7] = 0;
                m[5] = 0;
                m[2] = 0;
                m[4] = c;
                m[1] = s;
                m[3] = -s;
            } else {
                func_8001846C(x, &sx, &cx);
                {
                    register s32 s __asm__("$3");
                    register s32 n ;
                    register s32 c __asm__("$6");
                    register s32 d __asm__("$7");
                    register s32 a ;
                    register s32 b __asm__("$4");
                    register s32 xs __asm__("$5");
                    s = sz;
                    __asm__ volatile("" : "+r"(s));
                    n = -s;
                    m[3] = n;
                    __asm__ volatile("" : : : "memory");
                    c = cz;
                    d = cx;
                    a = c >> 2;
                    b = d >> 2;
                    __asm__ volatile("" : "+r"(a), "+r"(b));
                    xs = sx;
                    sz = s >> 2;
                    cz = a;
                    {
                        s32 t = xs >> 2;
                        s32 e = b;
                        sx = t;
                        __asm__("" : "+r"(e) : "r"(t));
                        cx = e;
                    }
                    {
                        register s32 result ;
                        result = (a * b) >> 12;
                        __asm__ volatile("" : "+r"(result));
                        m[0] = result;
                    }
                    m[1] = (cx * sz) >> 12;
                    m[5] = 0;
                    m[4] = c;
                    m[2] = -xs;
                    m[8] = d;
                    m[6] = (cz * sx) >> 12;
                    m[7] = (sz * sx) >> 12;
                }
            }
        } else {
            func_8001846C(y, &sy, &cy);
            if (x == 0) {
                m[0] = cz;
                m[2] = 0;
                m[1] = sz;
                m[5] = sy;
                m[8] = cy;
                sz >>= 2;
                cz >>= 2;
                sy >>= 2;
                cy >>= 2;
                {
                    s32 q;
                    m[3] = -(cy * sz) >> 12;
                    m[4] = (cy * cz) >> 12;
                    m[6] = (sy * sz) >> 12;
                    q = cz * sy;
                    m[7] = -q >> 12;
                }
            } else {
                s32 p;
                func_8001846C(x, &sx, &cx);
                p = sx;
                sz >>= 2;
                cz >>= 2;
                sy >>= 2;
                cy >>= 2;
                sx >>= 2;
                cx >>= 2;
                m[0] = (cz * cx) >> 12;
                {
                    register s32 t ;
                    t = sy * sx;
                    t >>= 14;
                    m[1] = (cx * sz) >> 12;
                    m[3] = (cz * t - cy * sz) >> 12;
                    m[4] = (cy * cz + sz * t) >> 12;
                    m[5] = (cx * sy) >> 12;
                }
                {
                    register s32 u ;
                    u = (cy * sx) >> 14;
                    m[6] = (sy * sz + u * cz) >> 12;
                    m[7] = (u * sz - cz * sy) >> 12;
                }
                m[8] = (cy * cx) >> 12;
                m[2] = -p;
            }
        }
    }
}
