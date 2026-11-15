// Categories: Graphics > Effects > Beams | Physics > Motion
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

/**
 * @brief Per-frame tick of a beam/tracer node: integrates its velocity and
 *        expires it at the time limit.
 *
 * @param obj Beam node; +0x24 the flags word, +0x28 the u16 time, +0x2A the
 *        u16 limit, +4 base, +0x10 velocity, +0x1C offset, +0x10 out.
 * @param delta Frame time step.
 * @return 1 while the node is alive, 0 when it expired.
 *
 * @par Confirmed
 * Flag bit 0x8000 is cleared on the first tick after expiry (the else
 * branch); otherwise time += delta. While time < limit, each axis computes
 * pos = base + (velocity * time) >> 4 and out = pos + (offset << 4).
 *
 * @par Likely role
 * The motion update of the tracers spawned by func_8006F0E8: the scene
 * node updater func_8006F170 calls it each frame, the out triple is what
 * the renderer draws.
 *
 * @par Unknown
 * Who sets flag bit 0x8000 and whether the expiry path also frees the node.
 *
 * @par Callers
 * func_8006F170.
 */
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
