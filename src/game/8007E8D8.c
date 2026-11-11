#include "game/types.h"

void func_8007E8D8(u8 *out0, u32 a, u32 b, u32 c0, u32 d0, u32 e0, u32 f0, u32 g0, u32 h0, u32 i0, u32 j0, u32 k0, u32 l0, u32 m0)
{
    register u8 *out __asm__("$24") = out0;
    register u32 c __asm__("$18");
    register u32 e __asm__("$14") = e0;
    register u32 h __asm__("$12") = h0;
    register u32 i __asm__("$11") = i0;
    register u32 l __asm__("$13") = l0;
    register u32 j __asm__("$9") = j0;
    register u32 k __asm__("$10") = k0;
    register u32 m __asm__("$8") = m0;
    register u32 d __asm__("$4") = d0;
    register u32 ff __asm__("$3") = f0;
    register u32 gg __asm__("$2") = g0;
    register u32 f __asm__("$25");
    register u32 g __asm__("$16");
    register u32 y __asm__("$17");
    register u32 z __asm__("$19");
    register u32 aa __asm__("$3");
    register u32 dd __asm__("$5");
    register u32 x __asm__("$4");
    register u32 xx __asm__("$7");
    register u32 q __asm__("$15");
    register u32 r __asm__("$2");
    register u32 v __asm__("$2");
    register u32 w __asm__("$3");
    __asm__("" : "+r"(out), "+r"(ff), "+r"(gg));
    __asm__("" : "+r"(e), "+r"(h), "+r"(i));
    __asm__("" : "+r"(j), "+r"(k), "+r"(l), "+r"(m), "+r"(d));
    c = c0;
    __asm__("" : "+r"(c));
    __asm__("" ::: "$19", "$17", "$16");
    f = ff;
    g = gg;
    __asm__("" : "+r"(f), "+r"(g));
    ff = (s32)e + (s32)ff;
    gg = (s32)gg + (s32)ff;
    gg = (s32)h + (s32)gg;
    gg = (s32)i + (s32)gg;
    gg = (s32)l + (s32)gg;
    y = gg & 3;
    __asm__("" : "+r"(gg), "+r"(y));
    gg = (s32)a + (s32)gg;
    gg = (s32)b + (s32)gg;
    gg = (s32)c0 + (s32)gg;
    gg = (s32)j + (s32)gg;
    gg = (s32)k + (s32)gg;
    gg = (s32)m + (s32)gg;
    gg = (s32)d + (s32)gg;
    ff = gg & 15;
    z = ff >> 2;
    aa = a;
    dd = d;
    gg &= 3;
    xx = gg;
    __asm__("" : "+r"(gg), "+r"(xx));
    x = gg;
    __asm__("" : "+r"(aa), "+r"(dd), "+r"(xx), "+r"(x));
    if ((f & 255) == 10 && (g & 255) == (f & 255) && (h & 255) == (g & 255) && (i & 255) == (h & 255)) {
        l &= 255;
        q = l;
        if (l >= 6) {
            q = 5;
            r = l - 5;
        } else {
            r = 0;
        }
        f += q;
        g += r;
    }
    v = y | ((i & 3) << 2);
    out[0] = v;
    v = e & 3;
    w = aa << 2;
    v |= w;
    out[1] = v;
    v = x | ((f & 3) << 2);
    out[2] = v;
    v = (e >> 2) & 3;
    w = dd << 2;
    v |= w;
    out[3] = v;
    v = b | ((i >> 1) & 6);
    out[5] = v;
    v = j | (k << 1);
    w = c << 2;
    v |= w;
    out[7] = v;
    v = (f >> 2) & 3;
    w = z << 2;
    v |= w;
    out[8] = v;
    v = m | (xx << 1);
    out[4] = g;
    out[6] = h;
    out[9] = v;
}
