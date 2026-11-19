// Categories: Graphics > Effects > Debris Spawner | Physics > Motion | Graphics > Model Renderer > Mesh Fracture
#include "game/types.h"

typedef struct V8 {
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} V8;

typedef struct W8 {
    s32 a;
    s32 b;
} W8;

typedef struct Entry {
    u8 b0;
    u8 b1;
    u16 h2;
    u8 i4;
    u8 i5;
    u8 i6;
    u8 i7;
    u8 i8;
    u8 pad[3];
} Entry;

typedef struct Mesh {
    s32 count;
    Entry e[1];
} Mesh;

typedef struct VList {
    s32 count;
    W8 v[1];
} VList;

typedef struct VHolder {
    s32 unk0;
    VList *list;
} VHolder;

typedef struct Model {
    u8 pad00[0x1C];
    VHolder *unk1C;
    VHolder *unk20;
    Mesh *unk24;
    s32 unk28;
} Model;

typedef struct Src {
    Model *model;
    s32 v[12];
} Src;

typedef struct Holder {
    u8 pad00[0x34];
    Src *src;
} Holder;

typedef struct Sub3D {
    u8 pad00[0x28];
    s32 x;
    s32 y;
    s32 z;
    u8 pad34[6];
    u16 unk3A;
    s16 unk3C;
} Sub3D;

typedef struct Node {
    V8 v[5];
    u8 type;
    u8 unk29;
    u16 unk2A;
    s16 unk2C;
    s16 unk2E;
    s16 unk30;
    s16 unk32;
    s32 unk34[3];
    s32 unk40;
    s16 unk44;
    s16 unk46;
    struct Node *next;
    s32 x;
    s32 y;
    s32 z;
} Node;

typedef struct Obj {
    u8 pad00[0xC];
    s16 unk0C;
    u8 pad0E;
    u8 unk0F;
    u8 unk10;
    u8 pad11[5];
    s16 unk16;
    u8 pad18[4];
    Holder *unk1C;
    u8 pad20[0x14];
    s32 m[12];
    u8 pad64;
    u8 unk65;
    u8 pad66[0x14];
    s8 unk7A;
    u8 pad7B[2];
    s8 unk7D;
    u8 pad7E[2];
    s32 unk80;
    s32 unk84;
} Obj;

register u8 *gp_base asm("$28");
#define gClipLimitOffset (*(s32 *)(gp_base + 0x7C8))

extern s32 func_80017B88(void);
extern s32 func_80019668(void);
extern s32 func_8001AF08(V8 *v);
extern void func_8001B148(s32 *m, V8 *src, V8 *dst, s32 count);
extern Sub3D *gCreateModelNode(s32, s32, s32, s32, s32, s32) __asm__("func_8003D680");
extern void gSpawnObjectFromTable(Sub3D *, s32, s32, s32, s32) __asm__("func_8003E984");
extern Node *func_8004AA90(void);
extern void func_8004AAC0(Obj *, Node *);
extern u8 func_8004B244(Obj *, s32, s32 *);
extern void func_8004B310(Obj *, s32 *, s32 *, s32, s16 *, s16 *, s32 *, s32 *);
s32 gSpawnMeshFractureDebris(Obj *obj, s32 arg1) __asm__("func_8004B5F8");


/**
 * @brief Fractures a model into single polygons: picks a random subset of the
 *        mesh's face entries, builds one scene node per face with recentred
 *        vertices and its own world position, and hands each node to the
 *        impulse and animation setup.
 *
 * @param obj Object block. +0x1C is the holder whose +0x34 is the source
 *        record; +0x7D a face cap, +0x7A a limit override, +0x65 the child
 *        type, +0x10 the variant flags, +0x16 the child lifetime, +0x0C and
 *        +0x0F the state and error bytes, +0x76/+0x78 the spin range,
 *        +0x66/+0x68 the speed range, +0x6A/+0x6C and +0x70/+0x72/+0x74 the
 *        push terms, +0x7B the option bits, +0x7C the signed percentage,
 *        +0x80 the copied model header word and +0x84 the debris list head;
 *        the twelve-word matrix at +0x34 holds the object origin at
 *        +0x58/+0x5C/+0x60.
 * @param arg1 Force record, forwarded unchanged to func_8004B244; 0 is
 *        allowed and then no impulse is added.
 * @return 1 when at least one fragment was linked into the +0x84 list, else 0;
 *         every early exit and the empty list path set bit 0 of +0x0F.
 *
 * @par Confirmed
 * The source record is read through +0x1C and a null source, a null model or
 * an empty selection returns 0 first. +0x80 receives the model header word
 * +0x28, and n starts at the face count of the mesh block +0x24, is lowered to
 * +0x7D when 0 < +0x7D < count, and is clipped to 200 - D_8009F7C8, a global
 * word (gp-relative, +0x7C8) that this function increments once per fragment;
 * when func_80017B88() returns 1 n is halved again.
 * Two 32-byte bitmaps in the scratchpad - 0x1F800000 for the dropped faces and
 * 0x1F800020 for the kept ones - are filled from a random byte that is halved
 * until it is below the face count, and the code samples the smaller side: n
 * keepers when count divided by 2 is at least n, otherwise count - n droppers;
 * the chosen bitmap is then scanned with a 0x80000000 shifted by the low five
 * bits. Only faces whose type nibble (bits 8..11 of the entry word, that is the
 * low nibble of entry byte +1) is 3 or 4 produce a node, taken from the free
 * list behind func_8004AA90. The entry's +2 word goes to node +0x2A, its +0
 * byte to +0x29 and the type nibble to +0x28, or-ed with 0x80 when +0x65 is
 * set and the fragment is still below the limit, in which case
 * func_8003D680(0, 0, 0, 0, 0, 0) supplies a holder that is stored over
 * node +0x4C and whose +0x3A counter is bumped. Vertices come from the list at
 * model +0x1C (v[0..2] from the entry bytes +4/+5/+6), v[3] copies v[0] for
 * type 3 or takes byte +7 for type 4, and v[4] is indexed through the normal
 * list at model +0x20 by byte +8. func_8001B148 rotates all five pairs by the
 * object matrix in place. The first four vertices give the centroid, then every
 * vertex below the type count is moved to centroid-relative form and
 * func_8001AF08 on it, shifted right by 4, is written to +0x46 (plain node) or
 * to the holder's +0x3C (bit 0x80 set); the loop runs downwards, so vertex 0's
 * length is what survives. The node position is the object origin plus the
 * centroid shifted left by 4. func_8004B310 randomises the spin words
 * +0x30/+0x32, the angular velocity +0x40 and the three-word push at +0x34,
 * and adds the impulse that func_8004B244 wrote when it returned non-zero.
 * func_8004AAC0 then links the node at the +0x84 list head and D_8009F7C8 is
 * incremented. +0x44 receives +0x16 plus a random byte, or 0. A fragment below
 * the limit with +0x65 set also calls func_8003E984(holder, 0, +0x65, 0,
 * (+0x10 >> 1) & 1) for the child object. The exit clears +0x0C and +0x16 and
 * sets +0x0F bit 0 when nothing was linked.
 *
 * @par Likely role
 * The polygon-fracture sibling of func_8004ABE4: instead of one impulse for the
 * whole object, the mesh's individual faces become free-flying debris that keep
 * their own polygon, normal and centroid, so each shard spins about its own
 * centre while the shared push record decides the blast direction. The force is
 * the same one func_8004B244 prepares for the blast effect, which ties this to
 * the explosion paths of func_8003AA94.
 *
 * @par Unknown
 * What the two budgets +0x7A and +0x7D count (faces, fragments or a detail
 * level), the unit of the impulse from func_8004B244, and why the radius field
 * keeps only vertex 0's length instead of a per-vertex value.
 *
 * @par Callers
 * func_8004C324, message 0 only: once on the default path and once from the
 * shared failure label of the flag-0x10 branch.
 *
 * @par Environment path
 * Selects up to n faces from an entity's 3D model using scratchpad bitmaps
 * (0x1F800000/0x1F800020) and spawns debris fragment nodes via func_8004AA90,
 * func_8004B310, and func_8004AAC0. Transforms face vertices with
 * func_8001B148, computes centroids, applies random spin and lifetime, and
 * increments gClipLimitOffset.
 */
s32 gSpawnMeshFractureDebris(Obj *obj, s32 arg1)
{
    s16 sum[4];
    s32 pos[4];
    s32 local[4];
    register Src *src asm("$19");
    Model *model;
    register s32 count asm("$17");
    register s32 n asm("$16");
    s32 total;
    u8 r;
    s32 flag;
    u32 *sel;
    s32 words;
    s32 i;
    s32 cur;
    register s32 limit asm("$23");
    u32 *bm;
    u32 *bm2;
    u32 bit;
    register u8 type asm("$22");
    Entry *e;
    Node *node;
    W8 *verts;
    W8 *norms;
    VList *nl;
    V8 *p;
    s32 j;
    s32 k;
    register u32 scratch asm("$10");

    bm2 = (u32 *)0x1F800020;
    src = 0;
    bm = (u32 *)0x1F800000;
    if (obj->unk1C != 0) {
        src = obj->unk1C->src;
    }
    obj->unk84 = 0;
    if (src == 0) {
        return 0;
    }
    model = src->model;
    if (model == 0) {
        return 0;
    }
    obj->unk80 = model->unk28;
    count = model->unk24->count;
    n = count;
    if (obj->unk7D > 0 && obj->unk7D < count) {
        n = obj->unk7D;
    }
    {
        register s32 counter asm("$3");
        counter = gClipLimitOffset;
        if (counter + n > 200) {
            register s32 max asm("$2");
            max = 200;
            n = max - counter;
        }
    }
    flag = func_80017B88();
    asm volatile("" ::: "$2");
    scratch = flag;
    asm("" : : "r"(scratch));
    if (scratch == 1) {
        n >>= 1;
    }
    if (n == 0) {
        return 0;
    }
    {
        register s32 temp asm("$3");
        temp = count;
        if (count < 0) temp = count + 31;
        words = temp >> 5;
    }
    for (i = 0; i < words; i++) {
        bm[i] = -1;
    }
    for (i = words + 1; i < 8; i++) {
        bm[i] = 0;
    }
    cur = 32 - (count & 0x1F);
    bm[words] = ((u32)-1 >> cur) << cur;
    cur = func_80019668() & 0xFF;
    while (cur >= count) {
        cur /= 2;
    }
    total = n;
    if (count / 2 >= n) {
        for (i = 7; i >= 0; i--) {
            bm2[i] = 0;
        }
        while (n > 0) {
            i = bm[cur / 32];
            bit = 1 << (~cur & 0x1F);
            if (i & bit) {
                bm[cur / 32] = i ^ bit;
                bm2[cur / 32] |= bit;
                cur = func_80019668() & 0xFF;
                while (cur >= count) {
                    cur /= 2;
                }
                n--;
            } else {
                cur++;
                if (cur >= count) {
                    cur = 0;
                }
            }
        }
        sel = bm2;
    } else {
        asm volatile("" : "+r"(n));
        scratch = total;
        asm("" : : "r"(scratch));
        n = count - (s32)scratch;
        while (n > 0) {
            i = bm[cur / 32];
            bit = 1 << (~cur & 0x1F);
            if (i & bit) {
                bm[cur / 32] = i ^ bit;
                cur = func_80019668() & 0xFF;
                while (cur >= count) {
                    cur /= 2;
                }
                n--;
            } else {
                cur++;
                if (cur >= count) {
                    cur = 0;
                }
            }
        }
        sel = bm;
    }
    r = func_8004B244(obj, arg1, local);
    if (src != 0) {
        obj->m[0] = src->v[0];
        obj->m[1] = src->v[1];
        obj->m[2] = src->v[2];
        obj->m[3] = src->v[3];
        obj->m[4] = src->v[4];
        obj->m[5] = src->v[5];
        obj->m[6] = src->v[6];
        obj->m[7] = src->v[7];
        obj->m[8] = src->v[8];
        obj->m[9] = src->v[9];
        obj->m[10] = src->v[10];
        obj->m[11] = src->v[11];
    }
    limit = total;
    if (obj->unk7A != -1) {
        limit = obj->unk7A;
    }
    scratch = flag;
    asm("" : : "r"(scratch));
    if (scratch == 1) {
        limit >>= 1;
    }
    cur = -1;
    for (i = 0; i < ({scratch = total; asm("" : : "r"(scratch)); (s32)scratch;});) {
    do {
        asm("" : "+r"(cur));
        cur++;
scan_test:
        {
            register u32 *word asm("$2");
            register u32 shift asm("$3");
            word = (u32 *)((cur / 32) << 2);
            scratch = (u32)sel;
            asm("" : : "r"(scratch));
            shift = cur & 0x1F;
            word = (u32 *)((u32)word + scratch);
            scratch = 0x80000000u;
            asm("" : "+r"(word), "+r"(shift), "+r"(scratch));
            if (*word & (scratch >> shift)) break;
        }
    } while (1);
        e = &model->unk24->e[cur];
        if ((u32)(((*(u32 *)e >> 8) & 0xF) - 3) < 2) {
            node = func_8004AA90();
            if (node == 0) {
                goto all_done;
            }
            if (obj->unk65 != 0 && i < limit) {
                ((Sub3D **)&node->x)[0] = gCreateModelNode(0, 0, 0, 0, 0, 0);
                ((Sub3D **)&node->x)[0]->unk3A++;
                node->type = ((*(u32 *)e >> 8) & 0xF) | 0x80;
            } else {
                node->type = (*(u32 *)e >> 8) & 0xF;
            }
            {
                register VList *vl asm("$3");
                register u32 hh asm("$4");
                VHolder *nh;
                nh = model->unk20;
                asm("" : "+r"(model) : "r"(nh));
                vl = (VList *)model->unk1C;
                asm("" : "+r"(e) : "r"(nh), "r"(vl));
                hh = e->h2;
                nl = nh->list;
                vl = ((VHolder *)vl)->list;
                verts = vl->v;
                node->unk2A = hh;
            }
            node->unk29 = e->b0;
            *(W8 *)&node->v[0] = verts[e->i4];
            *(W8 *)&node->v[1] = verts[e->i5];
            *(W8 *)&node->v[2] = verts[e->i6];
            {
                register u32 rawtype asm("$2");
                rawtype = node->type;
                asm("" : "+r"(rawtype), "+r"(nl));
                type = rawtype & 0x7F;
            }
            norms = nl->v;
            if (type == 3) {
                *(W8 *)&node->v[3] = verts[e->i4];
            } else if (type == 4) {
                *(W8 *)&node->v[3] = verts[e->i7];
            }
            *(W8 *)&node->v[4] = norms[e->i8];
            func_8001B148(obj->m, node->v, node->v, 5);
            {
                register u32 first_x asm("$3");
                first_x = (u16)node->v[0].x;
                sum[1] = node->v[0].y;
                sum[2] = node->v[0].z;
                sum[0] = first_x;
            }
            for (j = 1; j < 4; j++) {
                sum[0] += node->v[j].x;
                sum[1] += node->v[j].y;
                sum[2] += node->v[j].z;
            }
            sum[0] >>= 2;
            sum[1] >>= 2;
            sum[2] >>= 2;
            for (j = type - 1; j >= 0; j--) {
                p = &node->v[j];
                p->x -= sum[0];
                p->y -= sum[1];
                p->z -= sum[2];
                k = func_8001AF08(p) >> 4;
                if (node->type & 0x80) {
                    ((Sub3D **)&node->x)[0]->unk3C = k;
                } else {
                    node->unk46 = k;
                }
            }
            pos[0] = sum[0] << 4;
            pos[1] = sum[1] << 4;
            pos[2] = sum[2] << 4;
            if (node->type & 0x80) {
                ((Sub3D **)&node->x)[0]->x = obj->m[9] + pos[0];
                ((Sub3D **)&node->x)[0]->y = obj->m[10] + pos[1];
                ((Sub3D **)&node->x)[0]->z = obj->m[11] + pos[2];
            } else {
                node->x = obj->m[9] + pos[0];
                node->y = obj->m[10] + pos[1];
                node->z = obj->m[11] + pos[2];
            }
            node->unk2C = 0;
            node->unk2E = 0;
            func_8004B310(obj, pos, local, r, &node->unk30, &node->unk32, &node->unk40, node->unk34);
            if (obj->unk16 != 0) {
                node->unk44 = obj->unk16 + (func_80019668() & 0xFF);
            } else {
                node->unk44 = 0;
            }
            {
                register Obj *call_obj asm("$4");
                call_obj = obj;
                asm("" : : "r"(call_obj));
                gClipLimitOffset++;
                asm volatile("" ::: "memory");
                func_8004AAC0(call_obj, node);
            }
            if (i < limit && obj->unk65 != 0) {
                gSpawnObjectFromTable(((Sub3D **)&node->x)[0], 0, obj->unk65, 0, (obj->unk10 >> 1) & 1);
            }
        }
    {
        register s32 condition asm("$2");
        scratch = total;
        asm("" : "+r"(i) : "r"(scratch));
        i++;
        condition = i < (s32)scratch;
        asm("" : "+r"(condition) : "r"(cur));
        cur++;
        if (condition) goto scan_test;
    }
    cur--;
    asm("" : : "r"(cur));
    break;
    }
all_done:
    obj->unk0C = 0;
    obj->unk16 = 0;
    if (obj->unk84 == 0) {
        obj->unk0F |= 1;
        return 0;
    }
    return 1;
}
