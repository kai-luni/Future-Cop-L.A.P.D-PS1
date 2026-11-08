#include "game/types.h"

typedef struct Obj Obj;
typedef struct Data Data;
struct Data { u8 p0[0x14]; u16 count14, count16; u8 p18[8]; void *ptr20; u8 p24[0x9d]; u8 team; u8 pc2[0x1c]; u8 flag; u8 pdf[0x239]; u16 timer; };
struct Obj { u8 p0[4]; void (*update)(); u32 flags8; u8 pc[8]; u16 tag; u8 p16[6]; s16 angle, other; u8 active; u8 p21[3]; u32 flags24; u8 p28[0x64]; u32 id; u8 p90[0x1c]; Data *data; u8 pb0[0x2c]; Obj *child; u8 pe0[0xc]; void *owner; u8 pf0[0x44]; void *stats; };
struct Mode { u8 p[0x5c]; int active; };
struct Stats { u8 p[0x18]; u16 a; u8 p1a[0x22]; u16 b; };
extern struct Mode *D_8009FA94;
extern void *D_8009FAA4;
extern int D_8009FA0C, D_8009FA00;
extern Obj *D_8009F9EC, *D_8009FA58[];
extern void D_800578B4(), D_800578E4();
extern int func_8002A810(Obj *, int, int);
extern void func_80058C3C(void *, void *, Obj *);
extern void func_800402DC(Obj *, int, int, int, int);
extern void func_80046C84(int, Obj *);
extern void func_8003BA50(Obj *, int);
extern Obj *func_8004D664(int);
extern void func_80025CFC(Obj *);
extern void func_8005A190(Data *, int, int);

void func_80058D78(Obj *o)
{
    int first;
    register int team __asm__("$2");
    register Obj **table __asm__("$3");
    u16 angle;
    Obj *found, *other, *child;
    Data *data;
    first = o->data->flag == 0;
    o->data->flag = 0;
    if (func_8002A810(o, 0, first)) {
        if (first) {
            func_80058C3C(o->owner, (u8 *)o->child + 0x5c, o);
            func_800402DC(o, 3, 0, 1, 0);
            func_800402DC(o->child, 9, 0, 1, 0);
            if (D_8009FA94->active == 1) func_80046C84(0x41, o);
        }
        o->flags8 |= 0x800000;
        o->child->flags8 |= 0x800000;
        if (D_8009FA94->active == 1) {
            o->data->timer = first ? 1200 : 150;
            o->flags24 |= 0xa5;
            o->child->flags24 |= 0xa5;
            child = o->child;
            __asm__("" : "+r"(child));
            angle = o->other;
            child->angle = angle;
            o->angle = angle;
            if (o->active && !(o->flags24 & 0x40000)) func_8003BA50(o, 0);
            if (o->child->active && !(o->child->flags24 & 0x40000)) func_8003BA50(o->child, 0);
            if (!o->data->ptr20) o->data->ptr20 = D_8009FAA4;
            o->data->count16++;
            if (o->id) {
                found = func_8004D664(o->id & 0x3fffffff);
                if (D_8009FA0C == 1) {
                    if (found && D_8009F9EC && D_8009F9EC->tag == (found->tag & 0x3fff)) {
                        ((struct Stats *)D_8009F9EC->stats)->b++;
                        ((struct Stats *)D_8009F9EC->stats)->a = 0;
                    }
                } else {
                    data = o->data;
                    __asm__ volatile ("nop" : "+r"(data) : : "memory");
                    __asm__ volatile ("lbu %0,0xc1(%2)\n\tlui %1,%%hi(D_8009FA58)\n\taddiu %1,%1,%%lo(D_8009FA58)" : "=r"(team), "=r"(table) : "r"(data));
                    other = table[(s8)(team ^ 1)];
                    if (other && (found->tag & 0x3fff) == other->tag) other->data->count14++;
                }
            }
        } else {
            if (D_8009FA00 != 5) func_80025CFC(o);
            func_8005A190(o->data, 0, o->angle);
            o->update = D_800578B4;
            o->child->update = D_800578E4;
        }
    }
}
