// Categories: Gameplay > Entity Script > Path Sequence | Graphics > Model Renderer
#include "game/types.h"

struct Data { u8 pad[0x24]; s8 count; u8 end; s16 pos[3]; s16 speed[3]; s16 other[2]; s16 angle; u8 flags; u8 kind; u8 extra; };
struct Arg { u8 pad[8]; struct Data *data; };
struct Model { u8 pad[0x34]; u8 *data; };
struct Self { u8 pad[0x21]; u8 old; u8 pad22[6]; struct Model *model; u8 basis[0x24]; s32 pos[3]; u8 rest[0x14]; s16 index; u8 pad72[0x2e]; s32 values[3]; s32 speeds[3]; s32 other[2]; s32 current; s16 zero; u8 flags; u8 kind; s8 count; s8 last; s8 end; s8 mode; u8 previous; volatile u8 extra; };
extern void gEntityBaseHandler(s32, struct Self *, struct Arg *, s32) __asm__("func_8002AD58");
extern void gRegisterGroundObject(struct Self *, u8) __asm__("func_8006314C");
extern void gBuildRotationMatrix(s32, s32, s32, void *) __asm__("func_800188D0");
extern s32 gGetGroundHeight(void *, s32, s32, s32) __asm__("func_80066F78");
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
void gInitElevator(struct Self *self, struct Arg *arg, s32 a2) __asm__("func_80041488");

extern void gGteTransformVerticesWide(void *, void *, s32) __asm__("func_80010234");
/**
 * @brief Initialises the waypoint sequence of an entity from a script record.
 *
 * Copies count, the three waypoint positions, the segment speeds and two derived
 * values into the entity, resolves the entity kind, builds the basis matrix from
 * the record angle, snaps all waypoint heights to the ground and places the
 * entity on the last waypoint.
 *
 * @param self Sequence entity (same layout as in func_80041880).
 * @param arg Wrapper whose data pointer at +0x08 carries the record.
 * @param a2 Forwarded unchanged to func_8002AD58(2, self, arg, a2).
 * @return Nothing.
 *
 * @par Confirmed
 * func_8002AD58(2, self, arg, a2) runs first, so this is the message-2
 * (spawn/attach) path of the entity family.
 * count = d->count; values[0..2] = d->pos[0..2] << 4;
 * speeds[i] = (d->speed[i] << 8) / 218; other[0..1] = (d->other[i] << 4) / 300;
 * mode = 1; flags/kind/extra are copied straight from the record.
 * A volatile round-trip stores previous = old and end = d->end, then last =
 * end - 1. If end >= count, mode becomes 2 and end becomes last - 1.
 * current = 1 when (flags & 5) != 0, else speeds[last]; zero = 0.
 * func_8006314C(self, d->kind) selects the entity kind;
 * func_800188D0(d->angle, 0, 0, self->basis) builds the basis;
 * base = func_80066F78(self->pos, 0, 0, 0) is the ground height and is added to
 * all three values; self->pos[1] = values[last] places the entity vertically.
 * Then func_80010750(basis) loads the matrix and
 * func_80010234(model->data + (index << 4) + 8, (u8 *)self + 0x5c, 1) transforms
 * the frame vertex.
 *
 * @par Likely
 * The spawn handler of a moving platform/patrol entity: the record gives up to
 * three waypoints, per-segment speeds (rescaled from the script units by 218)
 * and a start angle; snapping the waypoint heights through func_80066F78 keeps
 * the path on the terrain. mode selects whether the sequence runs forwards only
 * or ping-pongs (Likely).
 *
 * @par Unknown
 * The two divisors 218 and 300 (Likely 1/256 and 1/300 unit conversions), and
 * what func_8006314C sets up from the kind byte.
 *
 * @par Callers
 * func_80041B6C (sole direct caller).
 *
 * @par Environment path
 * Message-2 (init) handler of the lift object stepped by func_80041880
 * (gStepElevator); its only caller is the class handler func_80041B6C. After
 * func_8002AD58 it copies the stop count, the three stop heights (<< 4) into
 * +0xA0, the three per-stop dwell times ((t << 8) / 218) into +0xAC - the
 * table func_80041880 reloads the dwell timer from - and the up/down speeds
 * ((v << 4) / 300) into +0xB8/+0xBC, sets the start stop and direction, and
 * seeds the dwell timer +0xC0 (1 for player-triggered lifts with flags & 5).
 * It resolves the object kind with func_8006314C, builds the matrix from the
 * record angle with func_800188D0, offsets every stop height by the ground
 * height from func_80066F78, places the lift at its start stop and transforms
 * the reference vertex (func_80010750, func_80010234); no globals are
 * touched.
 *
 * @par Shared field
 * zero (+0xC4, s16) is the same block field func_80041B6C increments as its
 * msg-11 hit counter (Obj41B6C.count) and func_80041880 decrements as its
 * hold countdown (Obj41880.unkC4); init clears it, so a fresh lift is not
 * held.
 */
void gInitElevator(struct Self *self, struct Arg *arg, s32 a2)
{
    struct Data *d;
    s32 i, base, offset;
    register s32 end __asm__("$3"); register s32 old __asm__("$2");
    register s32 count __asm__("$4");
    register s32 last __asm__("$3");
    gEntityBaseHandler(2, self, arg, a2);
    d = arg->data;
    self->count = d->count;
    self->values[0] = d->pos[0] << 4;
    self->values[1] = d->pos[1] << 4;
    self->values[2] = d->pos[2] << 4;
    self->speeds[0] = (d->speed[0] << 8) / 218;
    self->speeds[1] = (d->speed[1] << 8) / 218;
    self->speeds[2] = (d->speed[2] << 8) / 218;
    self->other[0] = (d->other[0] << 4) / 300;
    self->other[1] = (d->other[1] << 4) / 300;
    self->mode = 1;
    count = self->count;
    self->flags = d->flags;
    self->kind = d->kind;
    self->extra = d->extra;
    old = *(volatile u8 *)&self->old;
    end = *(volatile u8 *)&d->end;
    *(volatile u8 *)&self->old = 0;
    *(volatile u8 *)&self->previous = old;
    *(volatile s8 *)&self->end = end;
    last = end - 1;
    self->last = last;
    if (self->end >= count) {
        register s32 tmp __asm__("$2");
        tmp = last;
        __asm__("" : "+r"(tmp));
        self->mode = 2;
        self->end = tmp - 1;
    }
    if (self->flags & 5) self->current = 1;
    else self->current = self->speeds[self->last];
    self->zero = 0;
    gRegisterGroundObject(self, d->kind);
    gBuildRotationMatrix(d->angle, 0, 0, self->basis);
    base = gGetGroundHeight(self->pos, 0, 0, 0);
    for (i = 0; i < self->count; i++) self->values[i] += base;
    self->pos[1] = self->values[self->last];
    gGteLoadMatrix(self->basis);
    offset = (self->index << 4) + 8;
    gGteTransformVerticesWide(self->model->data + offset, (u8 *)self + 0x5c, 1);
}
