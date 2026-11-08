typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef int s32;
typedef struct Vec { s32 x,y,z; } Vec;
typedef struct Parent { u8 pad[0x28]; s32 x,y,z; struct Parent *parent; } Parent;
typedef struct Obj {
    struct Obj *next; u8 pad04[0xb]; u8 flags0f;
    u8 pad10[2]; s16 size; u8 pad14[6]; u16 flags1a; Parent *parent;
    u8 pad20[0x38]; Vec pos; u8 pad64[2]; u16 flags66; u8 flags68; u8 pad69[3];
    void *inner; s16 vx,vy,vz,rate76; u8 pad78[2]; s16 a7a,a7c,rate7e,rate80,rate82;
    u8 pad84[0xa]; s16 rate8e,rate90,rate92,count94;
} Obj;
extern void func_80040550(Obj *, s32);
extern void func_80040600(Obj *, s32,s32,s32);
extern void func_80010820(void *);
extern void func_8001006C(Vec *, Vec *, s32);
extern void func_80049980(void *,s32,s32,s32,s32,s32,s32,s32,s32);
void func_80040A00(Obj *node, s32 dt)
{
    Obj *o;
    Parent *p;
    s32 scale;
    s32 step = dt << 4;
    Vec pos;
    while (node) {
        if (!(node->flags0f & 8)) {
            o = node;
            if (!node->inner) goto next;
            if (node->rate82) node->vy += (node->rate82 * step) >> 12;
            if (node->flags68 & 1) {
                s32 x = node->vx * step;
                s32 y = node->vy * step;
                s32 z = node->vz * step;
                node->pos.x += x >> 8;
                node->pos.y += y >> 8;
                node->pos.z += z >> 8;
            }
            if (node->rate76) node->size += (node->rate76 * step) >> 12;
            if (!(node->flags66 & 4)) {
                s32 x = dt * node->rate7e;
                s32 y = dt * node->rate80;
                node->a7a += x >> 8;
                node->a7c += y >> 8;
            } else if (node->rate7e) node->a7a += (dt * node->rate7e) >> 8;
            if (o->count94 > 0 && (o->flags66 & 0x30)) {
                s32 x = (o->rate8e * dt) >> 8;
                if (o->flags66 & 0x100) func_80040550(o, x);
                else {
                    s32 y = o->rate90 * dt;
                    register s32 factor __asm__("$2") = o->rate92;
                    register s32 z __asm__("$9");
                    __asm__("" : "+r"(factor));
                    z = factor * dt;
                    __asm__("" : "+r"(z));
                    func_80040600(o, x, y >> 8, z >> 8);
                }
            }
            scale = 0x500;
            if (o->size >= 40) scale = o->size << 5;
            p = 0;
            if (o->parent) p = o->parent->parent;
            if (p) {
                if (!(o->flags1a & 0x100)) goto direct;
                func_80010820((u8 *)p + 4);
                func_8001006C(&o->pos, &pos, 1);
                pos.x = o->pos.x + p->x;
                pos.y = o->pos.y + p->y;
                pos.z = o->pos.z + p->z;
            } else {
                if (!(o->flags1a & 0x100) || !o->parent) goto direct;
                func_80010820((u8 *)o->parent + 4);
                func_8001006C(&o->pos, &pos, 1);
                pos.x = o->pos.x + o->parent->x;
                pos.y = o->pos.y + o->parent->y;
                pos.z = o->pos.z + o->parent->z;
            }
            func_80049980(o->inner, pos.x, pos.z, pos.y, scale, scale, o->a7a, o->a7c, 0);
            goto next;
        direct:
            func_80049980(o->inner, o->pos.x, o->pos.z, o->pos.y, scale, scale, o->a7a, o->a7c, 0);
        }
    next:
        node = node->next;
    }
}
