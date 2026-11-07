typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned char u8;

struct Obj {
    s32 next;
    s32 base[3];
    s32 pos[3];
    s32 out[3];
    s32 velocity[3];
    s16 offset[3];
    u16 time;
    u16 limit;
    u16 flags;
};

s32 func_8006EE14(struct Obj *obj, s32 delta)
{
    if (!(obj->flags & 0x8000)) {
        obj->time += delta;
    } else {
        obj->flags &= 0x7fff;
    }
    if (obj->time < obj->limit) {
        s32 time;
        __asm__("" : "=r" (time) : "0" (*(volatile u16 *)&obj->time) : "memory");
        obj->pos[0] = obj->base[0] + ((obj->velocity[0] * time) >> 4);
        obj->out[0] = obj->pos[0] + (obj->offset[0] << 4);
        obj->pos[1] = obj->base[1] + ((obj->velocity[1] * time) >> 4);
        obj->out[1] = obj->pos[1] + (obj->offset[1] << 4);
        obj->pos[2] = obj->base[2] + ((obj->velocity[2] * time) >> 4);
        obj->out[2] = obj->pos[2] + (obj->offset[2] << 4);
        return 1;
    }
    return 0;
}
