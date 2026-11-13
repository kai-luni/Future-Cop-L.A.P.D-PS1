typedef unsigned char u8;
typedef signed char s8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

struct Map {
    s32 pad0;
    s32 w;
    s32 h;
    u8 pad[0x18];
    s32 tab[1];
};

extern struct Map *D_8009F80C;
extern u8 **D_8009F808;
extern u8 D_8009AFE8[][4];
extern u8 D_8009B1A4[];

extern s32 func_80067480(s8 *data, u32 index, s32 x, s32 y, s32 *out);
extern s32 *func_8006320C(s32 *pos, s32 *out, s32 *a, u8 *b);
extern void func_8001A180(s32 *v);

s32 func_80066F78(s32 *position, s32 *output, s32 *a2, s32 *a3)
{
    s32 *pos = position;
    register s32 *out __asm__("$10") = output;
    struct { s32 v18, l1C; u8 l20, pad[3]; volatile s32 zl, xl, limit; volatile u32 l30, l34; volatile s32 l38; s32 pad3C; s32 * volatile sv_out; s32 * volatile sv_pos; } frame;
    register struct Map *map __asm__("$6");
    register u8 *s5 __asm__("$21");
    register s8 *s6 __asm__("$22");
    register u32 *s3 __asm__("$19");
    register u32 s0 __asm__("$16");
    register s32 s2 __asm__("$18");
    register s32 s4 __asm__("$20");
    register s32 x __asm__("$8");
    register s32 z __asm__("$7");
    register s32 cx __asm__("$3");
    register s32 cz __asm__("$9");
    s32 idx, a4, a5;
    register s32 cnt __asm__("$17");
    s32 tmp, type;
    s32 *r;

    (void)&a2;
    (void)&a3;
    if (a2) {
        *a2 = 0;
    }
    {
        register s32 t4v __asm__("$12") = pos[1];
        frame.limit = t4v;
    }
    z = pos[2];
    x = pos[0];
    if (z < 0) return 0;
    cz = z >> 17;
    map = D_8009F80C;
    if (cz >= map->w - 1) return 0;
    if (x < 0) return 0;
    cx = x >> 17;
    if (cx >= map->h - 1) return 0;
    idx = cz + map->w * cx;
    a4 = (x >> 13) & 0xF;
    a5 = a4 * 16 + ((z >> 13) & 0xF);
    tmp = (map->tab[idx] >> 2) & 0x3FFF;
    __asm__("" : "=r"(tmp) : "0"(tmp));
    s5 = D_8009F808[tmp];
    {
        s32 hw = ((u16 *)(s5 + 0x3CC))[a5];
        frame.limit += 0x200;
        s3 = (u32 *)(s5 + (hw >> 4) + 0x5D0);
        s6 = (s8 *)(s5 + (a4 + a5) * 3 + 0xC);
        cnt = hw & 0xF;
    }
    if (cnt == 0) return 0;
    s2 = 0x40000000;
    s4 = 0xC0000000;
    frame.zl = (z << 3) & 0xFFFF;
    frame.xl = (x << 3) & 0xFFFF;
    while (cnt > 0) {
        s0 = *s3++;
        type = (s0 >> 15) & 0x7F;
        if (type >= 0x47) break;
        {
        register s32 tt __asm__("$2") = (s0 >> 13) & 3;
        frame.l20 = tt;
        if (tt == 0 || !((s5 + tt)[0x370] & 0x40)) {
            s32 rv;
            frame.sv_out = out; frame.sv_pos = pos;
            rv = func_80067480(s6, type, frame.zl, frame.xl, &frame.v18);
            out = frame.sv_out; pos = frame.sv_pos;
            if (rv) {
                s32 val = frame.v18;
                if (val < s2) {
                    s2 = val;
                    frame.l34 = s0;
                }
                {
                register s32 lim __asm__("$13") = frame.limit;
                if (val < lim && s4 < val) {
                    s4 = val;
                    frame.l30 = s0;
                }
                }
            }
        }
        }
        cnt--;
    }
    if (s2 == 0x40000000) return 0;
    if (s4 == (s32)0xC0000000) {
        register u32 t4v __asm__("$12");
        s4 = s2;
        t4v = frame.l34;
        frame.l30 = t4v;
    }
    {
    register s32 *t5p __asm__("$13") = a3;
    if (t5p) {
        register s32 *rr __asm__("$6");
        frame.sv_out = out;
        rr = func_8006320C(pos, out, &frame.l1C, &frame.l20);
        out = frame.sv_out;
        if (rr && frame.l1C >= s4) {
            if (out) {
                s32 e0 = rr[14], e1 = rr[15], e2 = rr[16];
                out[0] = e0;
                out[1] = e1;
                out[2] = e2;
            }
            {
                register s32 *t4p __asm__("$12") = a3;
                *t4p = (s32)rr;
            }
            {
                register s32 *t5q __asm__("$13") = a2;
                if (t5q) *t5q = frame.l20;
            }
            return frame.l1C;
        }
        {
            register s32 *t4p __asm__("$12") = a3;
            *t4p = 0;
        }
    }
    }
    {
    register s32 *t5p __asm__("$13") = a2;
    if (t5p) {
        register s32 t4v __asm__("$12") = frame.limit;
        register s32 dd __asm__("$2") = t4v - s4;
        if (dd < 0x2000) {
            register u32 t5v __asm__("$13") = frame.l30;
            register s32 bi __asm__("$2") = (t5v >> 13) & 3;
            s32 b = (s5 + bi)[0x370];
            {
                register s32 *t4p __asm__("$12") = a2;
                *t4p = b;
            }
        }
    }
    }
    if (out) {
        register s32 s7 __asm__("$23");
        register s32 t1 __asm__("$9") = 0;
        register s32 t3 __asm__("$11") = 5;
        s32 fp, a3v, t0, a, b, c, d;
        u8 *p;
        s32 mode, t2;
        {
            register u32 t5v __asm__("$13") = frame.l30;
            register u32 vv __asm__("$2") = t5v >> 15;
            type = vv & 0x7F;
        }
        p = D_8009AFE8[type];
        {
            u32 i0 = p[0], i1 = p[1], i2 = p[2];
            s8 *q0 = s6 + i0, *q1 = s6 + i1, *q2 = s6 + i2;
            a = *q0;
            b = *q1;
            c = *q2;
        }
        a3v = b - a;
        t0 = c - a;
        if (type >= 0x3C) {
            register s32 t4v __asm__("$12") = frame.xl;
            register s32 t5v __asm__("$13") = frame.zl;
            register s32 cmp __asm__("$2") = t4v < t5v;
            if (cmp) {
                s7 = 0x20; fp = t3;
                goto L5;
            } else {
                register s32 f __asm__("$13") = 5;
                a3v = t0;
                frame.l38 = f;
                d = s6[p[3]];
                s7 = f;
                fp = 0x20;
                t0 = d - a;
            }
        } else {
            mode = D_8009B1A4[type] >> 3;
            switch (mode) {
            case 0:
                fp = 0x20; s7 = fp;
            L5:
                {
                    register s32 f __asm__("$12") = 5;
                    frame.l38 = f;
                }
                break;
            case 1:
                {
                    register s32 f __asm__("$13") = 0x20;
                    s7 = 5; frame.l38 = f; fp = s7; t1 = 1;
                }
                break;
            case 2:
                {
                    register s32 f __asm__("$12") = 5;
                    fp = 0x20; frame.l38 = f; s7 = f;
                }
                break;
            case 3:
                {
                    register s32 f __asm__("$13");
                    s7 = 0x20; fp = 5; f = fp; frame.l38 = f;
                }
                break;
            }
        }
        out[2] = (t0 << s7) - (a3v << t3);
        {
            register s32 t4v __asm__("$12") = frame.l38;
            register s32 one __asm__("$3") = 1;
            register s32 sh __asm__("$2") = t3 + t4v;
            t2 = one << sh;
            sh = fp + s7;
            one = one << sh;
            if (t1) {
                a3v = -a3v;
                out[1] = t2 + one;
            } else {
                out[1] = t2 - one;
            }
        }
        {
            register s32 t5v __asm__("$13") = frame.l38;
            out[0] = (a3v << fp) - (t0 << t5v);
        }
        func_8001A180(out);
        __asm__ volatile("" :: "r"(fp), "r"(s0), "r"(cnt), "r"(s2), "r"(s3), "r"(s5), "r"(s6), "r"(s7));
    }
    return s4;
}
