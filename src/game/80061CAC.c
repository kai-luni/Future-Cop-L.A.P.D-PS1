// Categories: Physics > Collision > Part Pair Test | Graphics > Geometry | Utilities > Mathematics
typedef unsigned char u8;
typedef unsigned int u32;
typedef struct { int x,y,z; } Vec;
typedef struct { int x,y,z,pad; } Vec4;
typedef struct { int m[9]; Vec pos; } Matrix;
typedef struct { int count, stride; char data[1]; } Shape;
typedef struct { char pad[12]; u32 flags; char pad10[0x24]; Shape *shape; } Model;
typedef struct { Model *model; Matrix mat; Vec pos; int radius; char pad44[4]; short frame; } Coord;
extern void gBuildViewMatrix(void *, void *) __asm__("func_80018860");
extern void func_80019470(void *, void *);
extern void func_8001A970(void *, void *, void *);
extern void gGteLoadRotation(void *) __asm__("func_80010820");
extern void gGteTransformWords(void *, void *, int) __asm__("func_8001006C");
extern int func_800618CC(void *, void *, int, int, int);
extern u32 func_800619C0(void *, void *, void *, int, int);

/**
 * @brief Narrow-phase hit test between two coord records.
 *
 * Tests the per-frame element list of one or both models and returns a 32-bit
 * hit mask: bits 31..16 name the colliding element of the first record, bits
 * 15..0 the one of the second, with marker bits 0 and 16 telling which halves
 * are valid.
 *
 * @param arg0 First coord (model at +0x00, 3x3 matrix at +0x04, position at
 *        +0x28, radius at +0x34, frame index at +0x3C).
 * @param arg1 Second coord, same layout.
 * @return Hit mask; 0 when nothing collided.
 *
 * @par Confirmed
 * flags = bit 27 of model->flags (model+0xC) for each side, so the mask is 0..3.
 * f == 0 returns 0x10001, f == 1/2/3 select the paths below and any other value
 * returns 0.
 * f == 1: pa = shape + ((a->frame << 4) + 8), func_80018860(&a->mat, &m1),
 * func_8001A970(&m1, &b->pos, &v) transforms B's position with m1, and
 * func_800618CC(pa, &v, b->radius, count, stride) returns the element index n
 * (negative on a miss). The result is (1 << (31 - n)) | 1.
 * f == 2 mirrors it on B and returns (1 << (15 - n)) | 0x10000.
 * f == 3 tests both shapes: B's matrix is combined with A's through
 * func_80019470, func_80010820 and func_8001006C(&a->mat, &m2, 4), with A's
 * translation temporarily replaced by the relative offset and restored
 * afterwards. Each A element is tested against the whole of B with
 * func_800619C0(m2, p, pb, sb->count, sb->stride); the returned B-side mask is
 * OR-ed into bits and stored in hits[i]. Then B's elements are walked again and
 * a pair is only accepted when both directions agree - the A bit is set at
 * 31 - i and the B bit (1 << (15 - j)) is OR-ed in as well.
 * Element 0 of each side is only included when the side has at least two
 * elements: the loops start at index 1 with the initial mask value 0x80000000
 * (A) or 0x40000000 (B) otherwise.
 *
 * @par Likely
 * The per-part collision of an actor against another actor (vehicle parts vs.
 * projectile or vs. another vehicle): bit 27 of the model flags marks a model
 * that carries collision elements, and the marker bits tell the caller which
 * side produced the hit so damage can be applied to the right owner.
 *
 * @par Unknown
 * The exact meaning of the marker bits 0 and 16 on their own (added even when no
 * element index is encoded), and what func_800618CC/func_800619C0 test per
 * element (Likely sphere or axis-aligned boxes derived from the frame data).
 *
 * @par Callers
 * func_80052098 (broad-phase distance test, then damage and reaction through
 * func_80058378 and func_80051EB8) and func_800620D8.
 *
 * @par Environment path
 * Narrow-phase hit test between two coord records: bit 27 of each model flag
 * word gives a two-bit mode, and the function returns a 32-bit mask whose
 * bits 31..16 name the colliding element of the first record and bits 15..0
 * the one of the second, with marker bits 0 and 16 telling the caller which
 * half is valid. Modes 1 and 2 test one shape against the other's position
 * and radius through gBuildViewMatrix, func_8001A970 and func_800618CC, while
 * mode 3 combines the two matrices with func_80019470, gGteLoadRotation and
 * gGteTransformWords, tests every element of A against all of B with
 * func_800619C0 and accepts a pair only when both directions agree. Mode 0
 * returns 0x10001 without testing anything and any other value returns 0. The
 * unit uses the GTE and the view matrix but is a collision routine, not a
 * render pass: it is called from the broad-phase test func_80052098
 * (src/game/80052098.c:91) and from func_800620D8 (src/game/800620D8.c:93),
 * and it touches no global.
 */
u32 func_80061CAC(Coord *arg0, Coord *arg1)
{
    register Coord *a __asm__("$17") = arg0;
    register Coord *b __asm__("$18") = arg1;
    register Shape *sa __asm__("$22");
    register Shape *sb __asm__("$23");
    char *pa, *pb, *p;
    Matrix m1, m2;
    Vec4 v;
    u32 hits[8];
    u32 flags, bits, result, hit, mask;
    int i, n, f; register int j __asm__("$19");
    __asm__ volatile ("" : "+r"(a), "+r"(b));
    flags = 0;
    if (a->model) { flags = (a->model->flags >> 27) & 1; sa = a->model->shape; }
    if (b->model) { if ((b->model->flags >> 27) & 1) flags |= 2; sb = b->model->shape; }
    f = flags & 0xFF;
    if (f == 1) goto case1;
    if (f < 2) {
        if (f == 0) goto case0;
        return 0;
    }
    if (f == 2) goto case2;
    if (f == 3) goto case3;
    return 0;
case0:
        return 0x10001;
case1:
        pa = (char *)sa + ((a->frame << 4) + 8);
        gBuildViewMatrix(&a->mat, &m1);
        func_8001A970(&m1, &b->pos, &v);
        n = func_800618CC(pa, &v, b->radius, sa->count, sa->stride);
        if (n < 0) goto ret0;
        return (1 << (31-n)) | 1;
case2:
        pb = (char *)sb + ((b->frame << 4) + 8);
        gBuildViewMatrix(&b->mat, &m1);
        func_8001A970(&m1, &a->pos, &v);
        n = func_800618CC(pb, &v, a->radius, sb->count, sb->stride);
        if (n < 0) goto ret0;
        return (1 << (15-n)) | 0x10000;
case3:
        pa = (char *)sa + ((a->frame << 4) + 8);
        pb = (char *)sb + ((b->frame << 4) + 8);
        func_80019470(&b->mat, &m1);
        *(Vec *)&v = a->mat.pos;
        a->mat.pos.x -= b->mat.pos.x;
        a->mat.pos.y -= b->mat.pos.y;
        a->mat.pos.z -= b->mat.pos.z;
        bits = i = 0;
        gGteLoadRotation(&m1);
        gGteTransformWords(&a->mat, &m2, 4);
        a->mat.pos = *(Vec *)&v;
        n = sa->count;
        p = pa;
        if (n >= 2) { p += sa->stride; i = 1; hits[0] = 0; }
        for (; i < n; i++, p += sa->stride) {
            hit = func_800619C0(&m2, p, pb, sb->count, sb->stride);
            bits |= hit;
            hits[i] = hit;
        }
        if (bits == 0) goto ret0;
        gBuildViewMatrix(&m2, &m1);
        j = 0; p = pb; n = sb->count; mask = 0x80000000;
        if (n >= 2) { j = 1; p += sb->stride; mask = 0x40000000; }
        result = 0;
        for (; j < n;) {
            if (mask & bits) {
                hit = func_800619C0(&m1, p, pa, sa->count, sa->stride);
                if (hit) { register u32 k __asm__("$4"); register u32 cnt __asm__("$5"); register u32 rowbit __asm__("$6"); register u32 one __asm__("$7"); i = 0; k = sa->count; if (k != 0) { __asm__("addiu\t%2,$0,1\n\taddiu\t$2,$0,15\n\tsubu\t$2,$2,%4\n\tsllv\t%1,%2,$2\n\taddu\t%0,%3,$0" : "=r"(cnt), "=r"(rowbit), "=r"(one) : "r"(k), "r"(j) : "$2"); do {
                    if ((hit & 0x80000000) && ((hits[i] << j) & 0x80000000))
                        { result |= one << (31-i); result |= rowbit; }
                    hit <<= 1;
                } while (++i < cnt); } }
            }
            { int stride = sb->stride; __asm__("" : "+r"(j) : "r"(stride)); j++; p += stride; mask >>= 1; }
        }
        return result;
ret0:
        return 0;
}
