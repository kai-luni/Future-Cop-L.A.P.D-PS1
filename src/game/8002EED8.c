#include "game/types.h"
struct Vec { s32 x,y,z; };
struct Source { u8 pad[0x24]; u8 type, flags; s8 sel; u8 other; s16 height, angle; union { s32 distance; struct { s16 lo, angle2; } halves; } u; s32 speed; };
struct Arg { u8 pad[8]; struct Source *src; };
struct State { u8 pad[0x34]; u8 *frames; };
struct Obj { u8 pad0[0x21]; u8 active; u8 pad22[6]; struct State *state; u8 matrix[0x18]; u8 matrix2[0xc]; struct Vec pos; u8 out[0x14]; s16 frame; u8 pad72[0x2e]; u8 type; u8 enabled, flags, oldactive, other; u8 padA5[3]; s32 speed; struct Vec start; struct Vec end; };
extern void func_8002AD58(s32, struct Obj *, struct Arg *, s32);
extern s32 func_80067684(struct Vec *, s32);
extern void func_800188D0(s32,s32,s32,void *);
extern void func_80010750(void *);
extern void func_80010234(void *,void *,s32);
extern void func_8001AC28(void *,void *,s32,void *);
extern void func_8006314C(void *,s32);
s32 func_8002EED8(struct Obj *obj, struct Arg *arg, s32 extra)
{
    struct Source *st;
    s32 sel;
    s32 angle;
    register s32 cur __asm__("$2");
    register s32 typ __asm__("$3");
    register u32 oldactive __asm__("$2");
    register u32 other __asm__("$3");
    register s32 offset __asm__("$4");
    register u8 *pointer __asm__("$2");
    func_8002AD58(2,obj,arg,extra);
    st=arg->src;
    sel=st->sel == 0 ? -1 : st->sel == 1 ? -2 : st->sel-2;
    obj->pos.y=func_80067684(&obj->pos,sel)+(st->height<<4);
    func_800188D0(st->angle,0,0,obj->matrix);
    obj->speed=st->speed/300;
    obj->type=st->type; __asm__ volatile("" : : : "memory");
    oldactive=obj->active;
    other=st->other;
    __asm__("" : "+r"(oldactive), "+r"(other));
    obj->oldactive=oldactive;
    obj->other=other;
    obj->flags=st->flags;
    if (obj->type == 0 || obj->type == 4 || obj->type == 5) {
        if (obj->type == 4 || obj->type == 5) {
            obj->start.x=0;
            obj->start.z=st->angle;
            angle=st->u.halves.angle2;
            __asm__("" : "+r"(angle));
            obj->end.x=angle & 0xfff;
        } else {
            obj->start.x=st->angle;
            obj->end.x=(st->angle+st->u.halves.angle2)&0xfff;
        }
        if (st->flags & 1) {
            cur=obj->end.x;
            obj->enabled=1; __asm__ volatile("" : : : "memory");
            typ=obj->type;
            __asm__ volatile("" : "+r"(cur), "+r"(typ) : : "memory");
            obj->start.y=cur; __asm__ volatile("" : : : "memory");
            if (typ==4) func_800188D0(obj->start.z,obj->start.y,0,obj->matrix);
            else if (typ==5) func_800188D0(obj->start.z,0,obj->start.y,obj->matrix);
            else func_800188D0(obj->start.y,0,0,obj->matrix);
        } else {
            obj->enabled=0;
            obj->start.y=obj->start.x;
        }
        func_80010750(obj->matrix);
        offset = obj->frame;
        pointer = (u8 *)obj->state;
        offset <<= 4;
        __asm__("" : "=r"(offset) : "0"(offset));
        pointer = ((struct State *)pointer)->frames;
        offset += 8;
        func_80010234(pointer + offset,obj->out,1);
    } else {
        if (obj->speed <= 0) obj->speed=0;
        if (st->u.distance < 0) obj->speed=-obj->speed;
        obj->start=obj->pos;
        func_80010750(obj->matrix);
        offset = obj->frame;
        pointer = (u8 *)obj->state;
        offset <<= 4;
        __asm__("" : "=r"(offset) : "0"(offset));
        pointer = ((struct State *)pointer)->frames;
        offset += 8;
        func_80010234(pointer + offset,obj->out,1);
        switch(obj->type) {
        case 2: func_8001AC28(&obj->pos,obj->matrix2,st->u.distance,&obj->end); break;
        case 1: func_8001AC28(&obj->pos,obj->matrix,st->u.distance,&obj->end); break;
        case 3:
            obj->end.x=obj->start.x;
            obj->end.z=obj->start.z;
            obj->end.y=obj->start.y+st->u.distance;
            break;
        }
        if (st->flags & 1) { obj->pos=obj->end; obj->enabled=1; }
        else obj->enabled=0;
    }
    if (obj->flags & 4) func_8006314C(obj,0);
    if (obj->flags & 8) obj->enabled=1-obj->enabled;
    else obj->active=0;
    return 0;
}
