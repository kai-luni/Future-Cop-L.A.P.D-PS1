// Categories: Physics > Collision > Oriented Box Against Entries | Graphics > GTE > Batch Transform | Graphics > Geometry > Bounding Volumes
typedef short s16;
typedef int s32;
typedef unsigned int u32;
typedef struct { s32 m[9]; s32 x,y,z; } Matrix;
typedef struct { char pad[0x24]; s32 x,y,z; } Base;
extern void gGteLoadRotation(Base *) __asm__("func_80010820");
u32 gBoxOverlapBitmask(Base *base, s16 *box, char *objects, s32 count, s32 stride) __asm__("func_800619C0");

extern void gGteTransformWords(Matrix *, Matrix *, s32) __asm__("func_8001006C");
/**
 * @brief Turns one oriented box into a world axis-aligned box and reports which
 *        of a list of radius-bounded entries it overlaps.
 *
 * @param base A 3x3 rotation at +0x00 followed by a world position at +0x24.
 * @param box Six 16-bit values: centre x/y/z, then half extents x/y/z, all
 *        scaled by 16 on use.
 * @param objects Byte array of entries of size stride; each entry begins with six
 *        16-bit values - centre x/y/z, then radii x/y/z.
 * @param count Number of entries. When it is 2 or more, entry 0 is skipped, so it
 *        is treated as the owning part.
 * @param stride Byte distance between entries.
 * @return Bitmask with one bit per entry index: entry j lands on bit 31 - j, so
 *         the first tested entry is bit 31 when only one entry exists and bit 30
 *         when entry 0 was skipped, and the last entry is bit 32 - count. Zero
 *         when nothing overlaps.
 *
 * @par Confirmed conversion
 * The local box becomes a diagonal scale plus translation matrix (extents << 4 on
 * the diagonal, centre << 4 in words 9..11). func_80010820 loads the base rotation
 * into the GTE and func_8001006C(&a, &b, 4) yields the oriented 3x4 matrix, whose
 * position row is then moved by base->x/y/z. Each world half extent is the sum of
 * the absolute values of the three basis components of that axis -
 * b.m[0]/b.m[3]/b.m[6] for x, b.m[1]/b.m[4]/b.m[7] for y and
 * b.m[2]/b.m[5]/b.m[8] for z - which is the standard OBB-to-AABB bound. All six
 * bounds are shifted right by 4 to return to the 16-bit world scale.
 *
 * @par Confirmed test
 * The entries are walked from the first tested index to count - 1, advancing by
 * stride, with the result shifted left once per entry and bit 0 set on an overlap.
 * An entry overlaps when, on all three axes, centre - radius is below the maximum
 * bound and the minimum bound is below centre + radius - strict comparisons, so
 * touching boxes do not count. The final shift by (32 - count) places the bits,
 * and the caller relies on exactly that numbering.
 *
 * @par Likely
 * The broad phase of a hitbox query: one frame box of an animated model, rotated
 * by the base matrix, against the point-and-radius entries of another shape; the
 * bitmask lets the caller map an overlap back to a record index.
 *
 * @par Unknown
 * Why entry 0 is dropped is not visible here; the caller skips it in the same way,
 * which suggests it describes the whole shape rather than one part.
 *
 * @par Callers
 * func_80061CAC, which calls it once per entry of the first shape and again with
 * the mirrored matrices to confirm the pair.
 *
 * @par Environment path
 * Turns one oriented box into a world axis-aligned box and returns a
 * per-entry overlap bitmask: it builds a diagonal scale matrix from the box
 * half extents, composes it with the base 3x3 rotation through
 * gGteLoadRotation (func_80010820) and gGteTransformWords (func_8001006C),
 * moves the result by the base position and sums the absolute basis
 * components per axis to obtain the AABB. It then walks the entries from
 * index 1 when count is 2 or more, at the given stride, and sets one bit per
 * entry whose centre/radius box overlaps on all three axes with strict
 * comparisons, shifting the result so entry j lands on bit 31 - j. Both call
 * sites in func_80061CAC:124/135 run it on the two sides of a hitbox pair, so
 * it is collision broad phase, not a render pass: it emits no primitive and
 * touches no global.
 */
u32 gBoxOverlapBitmask(Base *base, s16 *box, char *objects, s32 count, s32 stride)
{
    Matrix a, b;
    register s32 x __asm__("$9"), y __asm__("$7"), z __asm__("$8");
    register s32 xmin __asm__("$13"), xmax __asm__("$9"), ymin __asm__("$12"), ymax __asm__("$7"), zmin __asm__("$11"), zmax __asm__("$8");
    s32 i;
    register s32 n __asm__("$10");
    register u32 result __asm__("$6");
    s16 *p;
    register s32 t0 __asm__("$2"), t1 __asm__("$3"), t2 __asm__("$5"), t3 __asm__("$4");
    a.x=box[0]<<4; a.y=box[1]<<4; a.z=box[2]<<4;
    a.m[0]=box[3]<<4; a.m[1]=0; a.m[2]=0;
    a.m[3]=0; a.m[4]=box[4]<<4; a.m[5]=0;
    a.m[6]=0; a.m[7]=0; a.m[8]=box[5]<<4;
    gGteLoadRotation(base);
    gGteTransformWords(&a,&b,4);
    z=0; y=z; x=b.m[0];
    __asm__ volatile ("" : : "r" (z), "r" (y), "r" (x) : "memory");
    t0=b.y; t1=base->y; t2=base->x;
    __asm__ volatile ("" : "+r" (t0), "+r" (t1), "+r" (t2));
    b.y=t0+t1; __asm__ volatile ("" : : : "memory");
    t0=b.x; t1=b.z; t3=base->z;
    __asm__ volatile ("" : "+r" (t0), "+r" (t1), "+r" (t3));
    b.x=t0+t2; b.z=t1+t3;
    if(x<0) x=-x;
    if(b.m[3]<0) x-=b.m[3]; else x+=b.m[3];
    if(b.m[6]<0) x-=b.m[6]; else x+=b.m[6];
    xmin=b.x-x; xmax=x+b.x;
    __asm__ volatile ("" : : "r" (xmin), "r" (xmax) : "memory");
    if(b.m[1]<0) y-=b.m[1]; else y+=b.m[1];
    if(b.m[4]<0) y-=b.m[4]; else y+=b.m[4];
    if(b.m[7]<0) y-=b.m[7]; else y+=b.m[7];
    ymin=b.y-y; ymax=y+b.y;
    __asm__ volatile ("" : : "r" (ymin), "r" (ymax) : "memory");
    if(b.m[2]<0) z-=b.m[2]; else z+=b.m[2];
    if(b.m[5]<0) z-=b.m[5]; else z+=b.m[5];
    if(b.m[8]<0) z-=b.m[8]; else z+=b.m[8];
    t0=b.z;
    __asm__ volatile ("" : "+r" (t0));
    xmax>>=4; xmin>>=4; ymax>>=4; ymin>>=4;
    result=0; n=count;
    zmin=t0-z; zmax=z+t0; zmax>>=4; zmin>>=4;
    if(count>=2) { n=count-1; objects+=stride; }
    while(n>0) {
        p=(s16 *)objects;
        result <<= 1;
        if(p[0]-p[3]<xmax && xmin<p[0]+p[3] &&
           p[1]-p[4]<ymax && ymin<p[1]+p[4] &&
           p[2]-p[5]<zmax && zmin<p[2]+p[5]) result |= 1;
        n--; objects+=stride;
    }
    return result << (32-count);
}
