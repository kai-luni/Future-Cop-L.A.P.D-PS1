// Categories: Physics > Collision > Ground Query | Gameplay > Map | Graphics > Geometry
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

extern struct Map *gMapGridHeader __asm__("D_8009F80C");
extern u8 **gTileBlockTable __asm__("D_8009F808");
extern u8 gTileCornerIndices[][4] __asm__("D_8009AFE8");
extern u8 gTileShapeTable[] __asm__("D_8009B1A4");

extern s32 gSampleTileHeight(s8 *data, u32 index, s32 x, s32 y, s32 *out) __asm__("func_80067480");
extern s32 *gFindGroundObjectAtPosition(s32 *pos, s32 *out, s32 *a, u8 *b) __asm__("func_8006320C");
extern void gNormalizeVector(s32 *v) __asm__("func_8001A180");
s32 gGetGroundHeight(s32 *position, s32 *output, s32 *a2, s32 *a3) __asm__("func_80066F78");


/**
 * @brief Returns the ground height under a world position.
 *
 * Walks the map cell of the given x/z, samples every surface descriptor of the
 * sub-cell and returns the highest surface that is still below a ceiling of
 * position[1] + 0x200 (falling back to the lowest surface when none qualifies).
 * Optionally writes the surface normal and the surface attribute byte.
 *
 * @param position s32 triple: x at 0, y (ceiling reference) at 1, z at 2.
 * @param output Optional s32[3]; receives the surface normal, normalised by
 *        func_8001A180, or the record position when the secondary lookup hits.
 * @param a2 Optional surface attribute output (one byte).
 * @param a3 Optional record pointer output; set to the found record, or 0.
 * @return Height of the chosen surface, or 0 when the position is outside the
 *         map, the sub-cell is empty or no descriptor matched.
 *
 * @par Confirmed
 * Rejects z < 0, x < 0, (z >> 17) >= map->w - 1 and (x >> 17) >= map->h - 1;
 * the header accesses w and h transposed against x/z.
 * Cell index idx = (z >> 17) + map->w * (x >> 17); sub-cell a4 = (x >> 13) & 0xF
 * and a5 = a4 * 16 + ((z >> 13) & 0xF); block = D_8009F808[(tab[idx] >> 2) &
 * 0x3FFF].
 * The 16-bit descriptor hw = ((u16 *)(block + 0x3CC))[a5] gives count = hw & 0xF
 * (zero returns 0 immediately) and the descriptor list at
 * block + 0x5D0 + (hw >> 4); the height table handed to func_80067480 is
 * block + (a4 + a5) * 3 + 0xC.
 * Sampling coordinates are the 16-bit fractions (z << 3) & 0xFFFF and
 * (x << 3) & 0xFFFF.
 * The loop runs at most 15 32-bit descriptors: type = (s0 >> 15) & 0x7F and it
 * stops at type >= 0x47. The quadrant (s0 >> 13) & 3 is skipped when it is 0 or
 * when the byte at block + quadrant + 0x370 has bit 0x40 set. func_80067480
 * returns the sampled height; s2/s4 keep the global minimum and the greatest
 * value below the ceiling, with their descriptors in l34/l30.
 * With no hit at all s2 is still 0x40000000 and the function returns 0; when no
 * value was below the ceiling both selections collapse onto the minimum.
 * Secondary lookup: func_8006320C(position, output, &h, &attr) may return a
 * record with h >= s4; then output[0..2] = record[14..16], the record goes
 * through a3, attr through a2 and h is returned.
 * Otherwise, with output non-NULL the winning descriptor is decoded again: the
 * corner indices come from D_8009AFE8[type] and their heights are read from the
 * height table. For type >= 0x3C a fourth corner is used and the pair is chosen
 * by comparing the two fractional coordinates; below that the mode byte
 * D_8009B1A4[type] >> 3 (0..3) selects one of four combinations of the shift
 * counts 5 and 0x20. output[0], output[1] and output[2] are then built from the
 * corner heights as gradient terms and normalised by func_8001A180(output).
 * If the chosen surface lies within 0x2000 of the ceiling, a2 receives the byte
 * at block + quadrant + 0x370.
 *
 * @par Likely
 * Terrain sampling for entity placement; func_80041C6C confirms the usage
 * (obj->v54 = result + (src->x28 << 4)). func_80067480 interpolates the height
 * inside a quad/triangle of the surface type, output is the ground normal, and
 * a2 a surface/material id (water, road, ...).
 *
 * @par Unknown
 * What func_8006320C looks up (Likely a dynamic surface such as a moving
 * platform), and why w and h are used transposed.
 *
 * @par Callers
 * Twenty-two direct callers, among them func_80041C6C, func_80041488,
 * func_800164F4 and func_8004AFC0.
 *
 * @par Environment path
 * Returns the ground height under a world position by walking one map cell:
 * gMapGridHeader (D_8009F80C) gives the grid size and per-cell index, the cell
 * is (z >> 17) + w * (x >> 17), gTileBlockTable (D_8009F808) yields the cell
 * block, and the half-word at block + 0x3CC for the 8192-unit sub-tile
 * selects the surface count and descriptor list. Each surface is sampled with
 * func_80067480 from the corner heights at block + 0xC; the function keeps
 * the extreme surface and the best one within the limit position[1] + 0x200
 * (falling back to the extreme one), optionally lets a dynamic object from
 * func_8006320C win, reports the surface attribute byte when within 0x2000,
 * and, when an output vector is requested, builds the surface normal from
 * gTileCornerIndices (D_8009AFE8) and gTileShapeTable (D_8009B1A4) and
 * normalises it with func_8001A180. It has 22 direct callers for placement
 * and collision (among them func_80041C6C, func_80041488, func_800164F4 and
 * func_8003802C) and produces no geometry.
 */
s32 gGetGroundHeight(s32 *position, s32 *output, s32 *a2, s32 *a3)
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
    map = gMapGridHeader;
    if (cz >= map->w - 1) return 0;
    if (x < 0) return 0;
    cx = x >> 17;
    if (cx >= map->h - 1) return 0;
    idx = cz + map->w * cx;
    a4 = (x >> 13) & 0xF;
    a5 = a4 * 16 + ((z >> 13) & 0xF);
    tmp = (map->tab[idx] >> 2) & 0x3FFF;
    __asm__("" : "=r"(tmp) : "0"(tmp));
    s5 = gTileBlockTable[tmp];
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
            rv = gSampleTileHeight(s6, type, frame.zl, frame.xl, &frame.v18);
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
        rr = gFindGroundObjectAtPosition(pos, out, &frame.l1C, &frame.l20);
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
        p = gTileCornerIndices[type];
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
            mode = gTileShapeTable[type] >> 3;
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
        gNormalizeVector(out);
        __asm__ volatile("" :: "r"(fp), "r"(s0), "r"(cnt), "r"(s2), "r"(s3), "r"(s5), "r"(s6), "r"(s7));
    }
    return s4;
}
