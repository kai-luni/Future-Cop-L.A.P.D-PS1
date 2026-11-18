// Categories: Game Engine > Object Behaviour > Spawn Path | Gameplay > Object Spawn | Graphics > 3D > Animation Frame
#include "game/types.h"
struct Vec { s32 x,y,z; };
struct Source { u8 pad[0x24]; u8 type, flags; s8 sel; u8 other; s16 height, angle; union { s32 distance; struct { s16 lo, angle2; } halves; } u; s32 speed; };
struct Arg { u8 pad[8]; struct Source *src; };
struct State { u8 pad[0x34]; u8 *frames; };
struct Obj { u8 pad0[0x21]; u8 active; u8 pad22[6]; struct State *state; u8 matrix[0x18]; u8 matrix2[0xc]; struct Vec pos; u8 out[0x14]; s16 frame; u8 pad72[0x2e]; u8 type; u8 enabled, flags, oldactive, other; u8 padA5[3]; s32 speed; struct Vec start; struct Vec end; };
extern void gEntityBaseHandler(s32, struct Obj *, struct Arg *, s32) __asm__("func_8002AD58");
extern s32 func_80067684(struct Vec *, s32);
extern void gBuildRotationMatrix(s32,s32,s32,void *) __asm__("func_800188D0");
extern void gGteLoadMatrix(void *) __asm__("func_80010750");
extern void gGteTransformVerticesWide(void *,void *,s32) __asm__("func_80010234");
extern void func_8001AC28(void *,void *,s32,void *);
s32 gInitMoverObject(struct Obj *obj, struct Arg *arg, s32 extra) __asm__("func_8002EED8");

extern void func_8006314C(void *,s32);
/**
 * @brief Position, orient and animate one object from its spawn descriptor.
 *
 * @param obj   Object block. Fields used: active at +0x21, matrix at +0x2C,
 *              second matrix at +0x44, position at +0x50..0x58, output vertex
 *              at +0x5C, animation frame at +0x70, state pointer at +0x28,
 *              type at +0xA0, enabled at +0xA1, flags at +0xA2, old active
 *              flag at +0xA3, "other" at +0xA4, speed at +0xA8, start triple
 *              at +0xAC..0xB4 and end triple at +0xB8..0xC0.
 * @param arg   Spawn argument; the source descriptor is at +0x8.
 * @param extra Forwarded unchanged to func_8002AD58.
 *
 * @return Always 0 (addu v0, zero, zero at 0x8002F258).
 *
 * @par Globals
 * None. The listing for this function contains no absolute and no $gp-relative
 * access; every value is read from or written through the two pointer arguments.
 *
 * @par Control flow
 * Confirmed:
 * 1. func_8002AD58(2, obj, arg, extra) - the base pass of kind 2 - runs first.
 * 2. st = arg->src (+0x8). The selector at st+0x26 is mapped to
 *    0 -> -1, 1 -> -2, n -> n - 2 and passed to func_80067684.
 * 3. obj->y (+0x54) = func_80067684(&obj->pos, sel) + (st->height << 4), with
 *    st->height a s16 at +0x28.
 * 4. func_800188D0(st->angle, 0, 0, obj + 0x2C) builds the base matrix from the
 *    s16 angle at st+0x2A.
 * 5. obj->speed (+0xA8) = st->speed (+0x30) / 300: the listing uses the magic
 *    constant 0x1B4E81B5 with sra 5, which is the signed division by 300.
 * 6. Copy block: type +0x24 into obj+0xA0, active into old active (+0xA3),
 *    st+0x27 into obj+0xA4, st+0x25 into obj+0xA2.
 * 7. Branch on the type. 0, 4 and 5 take the orientation path, everything else
 *    takes the travel path.
 *    - Type 4 or 5: start.x (+0xAC) = 0, start.z (+0xB4) = st->angle,
 *      end.x (+0xB8) = st->angle2 (+0x2E) & 0xFFF.
 *    - Type 0: start.x = st->angle, end.x = (st->angle + angle2) & 0xFFF, so
 *      the 12-bit mask confines both values to one turn of the angle table.
 *    - If st->flags (+0x25) has bit 0: enabled (+0xA1) = 1, start.y (+0xB0) =
 *      end.x and a second matrix is built with the two remaining angles placed
 *      differently for 4 (start.z, start.y), 5 (start.z, 0, start.y) and 0
 *      (start.y, 0, 0). Otherwise enabled = 0 and start.y = start.x.
 * 8. Both paths then load obj->state (+0x28), take its frames pointer at
 *    +0x34 and call func_80010234(frames + (frame << 4) + 8, obj + 0x5C, 1):
 *    one 12-byte vertex out of a 16-byte-per-frame record with an 8-byte header.
 * 9. Travel path only: speed is clamped to >= 0, negated when st->distance
 *    (+0x2C) is negative, start is copied from the position, and the type picks
 *    the end point: 2 uses func_8001AC28 with the second matrix (+0x44), 1 uses
 *    the first matrix, 3 copies start and adds the distance to y. When bit 0 of
 *    st->flags is set the position is set to the end point and enabled = 1.
 * 10. Tail: obj->flags & 4 calls func_8006314C(obj, 0); obj->flags & 8 sets
 *     enabled = 1 - enabled, otherwise the object is deactivated by clearing
 *     +0x21.
 *
 * @par Calls
 * func_8002AD58, func_80067684, func_800188D0 (twice), func_80010750,
 * func_80010234 (once on each path), func_8001AC28 (types 1 and 2),
 * func_8006314C.
 *
 * @par Side effects
 * Overwrites roughly twenty fields of obj, may replace the position with the
 * computed end point, may deactivate the object and may notify func_8006314C.
 * No global data is written, so two calls for two objects cannot interfere.
 *
 * @par Likely role
 * The behaviour for message 2 of an object-type handler: func_8002F7C8
 * dispatches message 2 to this function and message 1 only reports a size
 * (0xC4), which fits a "spawn/attach from descriptor" step rather than a
 * per-frame update. Types 1 and 2 travel along an axis, type 3 only moves
 * vertically, and 4 / 5 hold a two-angle orientation, so the same code seeds
 * several kinds of spawned sub-objects.
 *
 * @par Unknown
 * - The unit of st->speed and hence what the division by 300 converts (frames
 *   per second is the obvious candidate, nothing in the code proves it).
 * - The meaning of the byte at st+0x27 copied to obj+0xA4.
 * - What func_80067684 returns for the three selector values.
 *
 * @par Callers
 * func_8002F7C8, recorded at 0x8002F7C8 (direct call at 0x8002F840). That
 * dispatcher itself has no direct caller in analysis/edges.json, so this
 * function is reached through an object-type table.
 *
 * @par Environment path
 * Message-2 body of one object-type behaviour: it calls the shared
 * func_8002AD58 first, then sets the ground height from func_80067684 plus
 * the descriptor height shifted by 4, builds the base matrix from the
 * descriptor angle with func_800188D0, converts the descriptor speed by
 * dividing 300, and copies type, flags, the old active flag and the extra
 * byte into the object. Types 0, 4 and 5 take the orientation path with the
 * twelve-bit masked start and end angles and an optional second matrix, types
 * 1, 2 and 3 take the travel path with the distance applied through
 * func_8001AC28, and both paths end with a GTE matrix load and one vertex
 * transform out of the state frame list before the tail may call
 * func_8006314C or deactivate the object. The file states that the listing
 * contains no absolute and no gp-relative access, so the function reads no
 * globals at all, and its caller is func_8002F7C8, message 2 of the type
 * handler.
 */
s32 gInitMoverObject(struct Obj *obj, struct Arg *arg, s32 extra)
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
    gEntityBaseHandler(2,obj,arg,extra);
    st=arg->src;
    sel=st->sel == 0 ? -1 : st->sel == 1 ? -2 : st->sel-2;
    obj->pos.y=func_80067684(&obj->pos,sel)+(st->height<<4);
    gBuildRotationMatrix(st->angle,0,0,obj->matrix);
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
            if (typ==4) gBuildRotationMatrix(obj->start.z,obj->start.y,0,obj->matrix);
            else if (typ==5) gBuildRotationMatrix(obj->start.z,0,obj->start.y,obj->matrix);
            else gBuildRotationMatrix(obj->start.y,0,0,obj->matrix);
        } else {
            obj->enabled=0;
            obj->start.y=obj->start.x;
        }
        gGteLoadMatrix(obj->matrix);
        offset = obj->frame;
        pointer = (u8 *)obj->state;
        offset <<= 4;
        __asm__("" : "=r"(offset) : "0"(offset));
        pointer = ((struct State *)pointer)->frames;
        offset += 8;
        gGteTransformVerticesWide(pointer + offset,obj->out,1);
    } else {
        if (obj->speed <= 0) obj->speed=0;
        if (st->u.distance < 0) obj->speed=-obj->speed;
        obj->start=obj->pos;
        gGteLoadMatrix(obj->matrix);
        offset = obj->frame;
        pointer = (u8 *)obj->state;
        offset <<= 4;
        __asm__("" : "=r"(offset) : "0"(offset));
        pointer = ((struct State *)pointer)->frames;
        offset += 8;
        gGteTransformVerticesWide(pointer + offset,obj->out,1);
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
