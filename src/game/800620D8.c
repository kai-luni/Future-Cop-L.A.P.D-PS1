typedef struct Object Object;
typedef struct Coord Coord;
struct Coord { char pad[0x34]; int x,y,z,r; short h,w; };
struct Info { char pad[0xc1]; signed char index; };
struct Object { int unused; int (*callback)(int,Object *,Object *,int); int id; char pad0c[0xa]; unsigned char type; char pad17[0xd]; unsigned int flags; Coord coord; char pad70[0x18]; Object *closest; char pad8c[4]; short distance; char pad92[0x1a]; struct Info *info; };
struct Entry { int id; Object *object; };
struct Bucket { unsigned char count; char pad[3]; struct Entry entries[8]; };
extern struct Bucket D_800A8DC0[];
extern int func_80061CAC(Coord *, Coord *);
void func_800620D8(Object *a, Object *b, unsigned int flags)
{
    register Coord *ca __asm__("$9") = &a->coord;
    register Coord *cb __asm__("$10") = &b->coord;
    int dz, dx, dy, ax, sum, dist, radius, d, q, hit;
    Object *owner, *other;
    struct Bucket *bucket;
    register unsigned int index __asm__("$2");
    register unsigned int next __asm__("$4");
    struct Entry *entry;
    dz = ca->z - cb->z;
    dx = ca->x - cb->x;
    if (dz < 0) dz = -dz;
    ax = dx;
    if (ax < 0) ax = -ax;
    if (dz < ax) { dz ^= ax; ax ^= dz; dz ^= ax; }
    sum = dz + ax;
    if ((dz >> 1) < ax) {
        sum = (sum << 6) + (sum << 3) + (sum << 1) + dz + (ax << 5);
        ax = dz - ax;
        dz = sum >> 7;
    }
    dist = dz + (ax >> 2) - (ax >> 6);
    radius = ca->r + cb->r;
    if (dist - radius < 0x4000) {
        q = dist;
        dy = ca->y - cb->y;
        if (q < 0) q = -q;
        ax = dy;
        if (ax < 0) ax = -ax;
        if (q < ax) { q ^= ax; ax ^= q; q ^= ax; }
        sum = q + ax;
        if ((q >> 1) < ax) {
            sum = (sum << 6) + (sum << 3) + (sum << 1) + q + (ax << 5);
            ax = q - ax;
            q = sum >> 7;
        }
        d = q + (ax >> 2) - (ax >> 6) - radius;
        if (d < 0x4000) {
            hit = 0;
            if (flags & 0x200) {
                if ((flags & 0x40000040) == 0x40000040 && d < 0x1800) {
                    if (a->type == 1) { owner = a; other = b; }
                    else { owner = b; other = a; }
                    bucket = &D_800A8DC0[owner->info->index];
                    index = bucket->count;
                    next = index + 1;
                    index = (index & 7) << 3;
                    __asm__("" : "+r"(index), "+r"(next));
                    index += 4;
                    __asm__("" : "+r"(index));
                    bucket->count = next;
                    entry = (struct Entry *)((char *)bucket + index);
                    entry->id = other->id;
                    entry->object = other;
                }
                if (d < 0) {
                    int test = dist - ((ca->w + cb->w) << 4);
                    d = 0;
                    if (test < 0 && (((dy >> 31) + dy) ^ (dy >> 31)) - ((ca->h + cb->h) << 4) < 0)
                        hit = func_80061CAC(ca, cb);
                }
            }
            if (hit) {
                a->callback(11, a, b, hit);
                b->callback(11, b, a, (hit << 16) | ((unsigned int)hit >> 16));
            } else {
                d >>= 8;
                if (d < a->distance && (b->flags & 0x2000)) { a->distance = d; a->closest = b; }
                if (d < b->distance && (a->flags & 0x2000)) { b->distance = d; b->closest = a; }
            }
        }
    }
}
