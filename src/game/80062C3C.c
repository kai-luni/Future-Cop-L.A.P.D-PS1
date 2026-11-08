typedef unsigned char u8;
typedef unsigned int u32;
typedef int s32;
typedef struct Obj { char pad0[8]; s32 active; char padc[0x18]; s32 flags; char pad28[0x34]; s32 z; s32 y; s32 x; s32 radius; } Obj;
typedef struct { s32 count, pad; Obj *items[1]; } List;
typedef struct Node { Obj *obj; struct Node *next; } Node;
typedef struct { s32 x,z,w,h; } Bounds;
typedef struct { s32 x,z; } Pair;
typedef struct { u32 bits; s32 pad[3]; } Cell;
extern List D_800A8F90;
extern Pair D_800A3190[];
extern Bounds D_800A2970;
extern Node D_800A2990[];
extern Cell D_800AB3B8[];
extern s32 D_8009F814, D_8009F828;
extern Obj **D_8009F818;
extern u8 *D_8009F820;
extern u8 D_8009F834, D_8009F835;

void func_80062C3C(void) {
    register s32 count __asm__("$12") = D_8009F828;
    Obj **scan = D_800A8F90.items;
    register s32 remaining __asm__("$13");
    register s32 cx __asm__("$16") = D_800A3190[0].x;
    register s32 cz __asm__("$25") = D_800A3190[0].z;
    Cell *cells = D_800AB3B8;
    register s32 ox __asm__("$23") = D_8009F834;
    register s32 oz __asm__("$22") = D_8009F835;
    register s32 width __asm__("$24") = D_800A2970.w;
    register s32 height __asm__("$21") = D_800A2970.h;
    register s32 bx __asm__("$20") = D_800A2970.x;
    register s32 bz __asm__("$19") = D_800A2970.z;
    Obj **output = D_8009F818;
    register s32 n __asm__("$11") = 1;
    register Obj *o __asm__("$8");
    register s32 x __asm__("$9");
    register s32 z __asm__("$15");
    register s32 r __asm__("$4");
    s32 dx,dz,px,pz,col,row;
    register s32 hit __asm__("$10");
    u32 bits;
    register s32 sign __asm__("$3");
    Node *node;
    for (remaining = D_800A8F90.count; remaining > 0; remaining--) {
        o = *scan++;
        if (o->active <= 0) continue;
        z = o->z; x = o->x; r = o->radius;
        dx = x - cx;
        pz = z;
        sign = dx >> 31;
        if (((sign + dx) ^ sign) > r) goto xoutside;
        px = cx;
        goto xdone;
    xoutside:
        if (dx > 0) px = x-r;
        else px = x+r;
    xdone:
        dz = pz - cz;
        sign = dz >> 31;
        if (((sign + dz) ^ sign) <= r) pz = cz;
        else if (dz > 0) pz -= r;
        else pz += r;
        hit = 0;
        col = (pz >> 17) - oz;
        if ((u32)col < 5) {
            col *= 5;
            dz = (px >> 17) - ox;
            if ((u32)dz < 5) {
                col += dz;
                bits = cells[col].bits;
                if (bits == 0) goto after_hit;
                col = (pz >> 13) & 12;
                col += (px >> 15) & 3;
                if (bits & (1 << (15 - col))) {
                    if (count >= 256) continue;
                    hit = 1;
                    { Obj **q = output + count; *q = o; }
                    count++;
                }
            }
        }
    after_hit:
        if (o->flags & 4) continue;
        if (hit) {
            col = (x >> 15) - bx;
        } else if (o->flags & 0x100) {
            col = (x >> 15) - bx;
        } else {
            continue;
        }
        if (col < 0 || col >= width) continue;
        row = (z >> 15) - bz;
        if (row < 0 || row >= height) continue;
        {
            u8 *p;
            u8 old;
            col += row * width;
            p = &D_8009F820[col];
            old = *p;
            *p = n;
            node = &D_800A2990[n++];
            node->obj = o;
            if (old == 0) {
                node->next = 0;
            } else {
                node->next = &D_800A2990[old];
            }
        }
        __asm__("" : : "r"(x), "r"(z));
    }
    D_8009F814 = n;
    D_8009F828 = count;
}
