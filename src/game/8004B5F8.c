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
#define D_8009F7C8 (*(s32 *)(gp_base + 0x7C8))

extern s32 func_80017B88(void);
extern s32 func_80019668(void);
extern s32 func_8001AF08(V8 *v);
extern void func_8001B148(s32 *m, V8 *src, V8 *dst, s32 count);
extern Sub3D *func_8003D680(s32, s32, s32, s32, s32, s32);
extern void func_8003E984(Sub3D *, s32, s32, s32, s32);
extern Node *func_8004AA90(void);
extern void func_8004AAC0(Obj *, Node *);
extern u8 func_8004B244(Obj *, s32, s32 *);
extern void func_8004B310(Obj *, s32 *, s32 *, s32, s16 *, s16 *, s32 *, s32 *);

s32 func_8004B5F8(Obj *obj, s32 arg1)
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
        counter = D_8009F7C8;
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
                ((Sub3D **)&node->x)[0] = func_8003D680(0, 0, 0, 0, 0, 0);
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
                D_8009F7C8++;
                asm volatile("" ::: "memory");
                func_8004AAC0(call_obj, node);
            }
            if (i < limit && obj->unk65 != 0) {
                func_8003E984(((Sub3D **)&node->x)[0], 0, obj->unk65, 0, (obj->unk10 >> 1) & 1);
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
