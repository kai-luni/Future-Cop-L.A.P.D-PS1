#include "game/types.h"
struct Vec { s32 x,y,z; };
struct Model { u8 pad[0x34]; u8 *frames; };
struct Obj { u8 pad0[0x28]; struct Model *model; u8 matrix[0x18]; struct Vec pos; u8 pad50[0xc]; u8 output[0x14]; s16 frame; u8 pad72[6]; struct Vec pos2; u8 pad84[0x44]; u8 track[0xc]; s16 trackframe; u8 padd6[0x86]; s16 angle; u8 pad15e[4]; u8 flag; u8 pad163[4]; u8 anim; };
extern void func_800188D0(s32,s32,s32,void *);
extern void func_80010750(void *);
extern void func_80010234(void *,void *,s32);
extern void func_8002B0EC(void *,s32);
extern void func_80044B34(void *,void *,s32,s32);
extern void func_800175F8(void *,s32);
void func_8002B588(struct Obj *o, s32 angle, s32 speed)
{
    s32 diff, step, mag;
    register s32 sign __asm__("$2");
    register s32 test __asm__("$2");
    register s32 sum __asm__("$3");
    register s32 result __asm__("$2");
    register s32 current __asm__("$4");
    register s32 wrapped __asm__("$7");
    register u8 *pointer __asm__("$2");
    register s32 offset __asm__("$4");
    current = o->angle;
    diff = angle - current;
    sign = diff >> 31;
    sum = sign + diff;
    wrapped = sum ^ sign;
    test = wrapped < 0x41;
    if (!test) {
        wrapped = diff & 0xfff;
        if (wrapped > 0x800) wrapped -= 0x1000;
        step = (speed * 3 << 9) >> 8;
        mag = ((wrapped >> 31) + wrapped) ^ (wrapped >> 31);
        if (mag < step) step = mag;
        if (wrapped < 0) {
            step = -step;
            __asm__("" : "=r"(step) : "0"(step));
        }
        result = current + step;
        angle = result & 0xfff;
        o->angle = angle;
        func_800188D0(angle, 0, 0, o->matrix);
        func_80010750(o->matrix);
        offset = o->frame;
        pointer = (u8 *)o->model;
        offset <<= 4;
        __asm__("" : "=r"(offset) : "0"(offset));
        pointer = ((struct Model *)pointer)->frames;
        offset += 8;
        func_80010234(pointer + offset, o->output, 1);
        o->pos2 = o->pos;
        if (o->flag == 0) {
            func_8002B0EC(o, o->anim);
            func_80044B34(o, o->track, speed, 0);
            func_800175F8(&o->model, o->trackframe);
        }
    }
}
