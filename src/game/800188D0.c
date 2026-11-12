typedef signed int s32;
typedef unsigned int u32;
extern void func_8001846C(u32, s32 *, s32 *);

void func_800188D0(s32 x, s32 y, s32 z, s32 *m)
{
    s32 sx, cx, sy, cy, sz, cz;
    s32 t, u;
    register s32 r __asm__("$4");
    z &= 0xfff;
    y &= 0xfff;
    x &= 0xfff;
    if (z == 0) {
        if (y == 0) {
            m[4] = 0x10000;
            m[7] = 0;
            m[5] = 0;
            m[3] = 0;
            m[1] = 0;
            if (x != 0) {
                func_8001846C(x, &sx, &cx);
                m[0] = cx;
                m[8] = m[0];
                m[6] = sx;
                m[2] = -sx;
            } else {
                m[8] = 0x10000;
                m[0] = 0x10000;
                m[6] = 0;
                m[2] = 0;
            }
        } else {
            func_8001846C(y, &sy, &cy);
            if (x == 0) {
                m[0] = 0x10000;
                m[6] = 0;
                m[3] = 0;
                __asm__("" : : : "memory");
                m[2] = 0;
                m[1] = 0;
                m[8] = cy;
                m[4] = cy;
                m[5] = sy;
                m[7] = -sy;
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
                m[6] = 0;
                __asm__("" : : : "memory");
                r = cz;
                __asm__("" : "=r"(r) : "0"(r));
                m[0] = r;
                m[8] = 0x10000;
                m[7] = 0;
                m[5] = 0;
                m[2] = 0;
                m[4] = r;
                m[1] = sz;
                m[3] = -sz;
            } else {
                func_8001846C(x, &sx, &cx);
                m[6] = sx;
                __asm__ volatile ("");
                m[7] = 0;
                m[8] = cx;
                m[1] = sz;
                m[4] = cz;
                sx >>= 2;
                sz >>= 2;
                cz >>= 2;
                cx >>= 2;
                m[0] = (cz * cx) >> 12;
                m[2] = -(cz * sx) >> 12;
                m[3] = -(sz * cx) >> 12;
                m[5] = (sz * sx) >> 12;
            }
        } else {
            func_8001846C(y, &sy, &cy);
            if (x == 0) {
                m[6] = 0;
                __asm__ volatile ("");
                {
                    register s32 rsy __asm__("$6") = sy; s32 rcz = cz, rcy = cy;
                    m[0] = rcz;
                    m[3] = -sz;
                    __asm__ volatile ("" : "+r"(rsy), "+r"(rcy));
                    m[7] = -rsy;
                    m[8] = rcy;
                    sz >>= 2;
                    cz >>= 2;
                    { s32 a = cy >> 2, b = sy >> 2; sy = b; cy = a; }
                    m[1] = (sz * cy) >> 12;
                    m[2] = (sz * sy) >> 12;
                    m[4] = (cz * cy) >> 12;
                    m[5] = (cz * sy) >> 12;
                }
            } else {
                func_8001846C(x, &sx, &cx);
                {
                s32 rawsy = sy;
                sz >>= 2;
                cz >>= 2;
                sy >>= 2;
                cy >>= 2;
                sx >>= 2;
                cx >>= 2;
                t = (sz * sy) >> 14;
                m[0] = (cz * cx + t * sx) >> 12;
                m[7] = -rawsy;
                m[1] = (sz * cy) >> 12;
                m[2] = (t * cx - cz * sx) >> 12;
                u = (sy * cz) >> 14;
                m[3] = (u * sx - sz * cx) >> 12;
                m[4] = (cz * cy) >> 12;
                m[5] = (sz * sx + u * cx) >> 12;
                m[6] = (cy * sx) >> 12;
                m[8] = (cy * cx) >> 12;
                }
            }
        }
    }
}
