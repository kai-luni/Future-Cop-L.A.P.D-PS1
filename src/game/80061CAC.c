typedef unsigned char u8;
typedef unsigned int u32;
typedef struct { int x,y,z; } Vec;
typedef struct { int x,y,z,pad; } Vec4;
typedef struct { int m[9]; Vec pos; } Matrix;
typedef struct { int count, stride; char data[1]; } Shape;
typedef struct { char pad[12]; u32 flags; char pad10[0x24]; Shape *shape; } Model;
typedef struct { Model *model; Matrix mat; Vec pos; int radius; char pad44[4]; short frame; } Coord;
extern void func_80018860(void *, void *);
extern void func_80019470(void *, void *);
extern void func_8001A970(void *, void *, void *);
extern void func_80010820(void *);
extern void func_8001006C(void *, void *, int);
extern int func_800618CC(void *, void *, int, int, int);
extern u32 func_800619C0(void *, void *, void *, int, int);

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
        func_80018860(&a->mat, &m1);
        func_8001A970(&m1, &b->pos, &v);
        n = func_800618CC(pa, &v, b->radius, sa->count, sa->stride);
        if (n < 0) goto ret0;
        return (1 << (31-n)) | 1;
case2:
        pb = (char *)sb + ((b->frame << 4) + 8);
        func_80018860(&b->mat, &m1);
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
        func_80010820(&m1);
        func_8001006C(&a->mat, &m2, 4);
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
        func_80018860(&m2, &m1);
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
