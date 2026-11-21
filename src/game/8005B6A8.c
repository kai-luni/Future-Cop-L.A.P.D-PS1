// Categories: Gameplay > Combat | Gameplay > Map
#include "game/types.h"

struct Vec { s32 x, y, z; };
struct Tag { u8 pad[4]; u8 kind; };
struct Sphere { struct Sphere *next; struct Vec pos; s16 radius; s8 state; u8 pad13[5]; u16 flags; };
struct Target {
    u8 pad00[4]; void (*callback)(s32, struct Target *, void *, s32);
    s32 active; u8 pad0c[8]; u16 id; u8 kind; u8 pad17; u8 team;
    u8 pad19[11]; u32 flags; u8 pad28[0x34]; struct Vec pos;
};
struct Source {
    u8 pad00[4]; struct Target *target; struct Target *detail;
    u8 pad0c[12]; s32 timer; u16 value; u8 column; u8 pad1f[11];
    u8 flags; u8 pad2b; struct Vec pos; s16 power;
};
struct Model { u8 pad00[4]; struct Vec v0; struct Vec v1; struct Vec v2; };
struct Link { u8 pad00[0x34]; struct Model *model; u8 pad38[0x10]; struct Source **source; };
struct Owner {
    u8 pad00[15]; u8 flag0f; u8 flag10; u8 pad11[11]; struct Link *link;
    struct Tag *tag; void (*callback)(s32, struct Owner *, void *, s32);
    u8 pad28[0x40]; struct Sphere *spheres; u8 pad6c[0x22]; u16 flag8e;
    u8 pad90[10]; s16 x, y, z;
};
struct Message { s32 value; u8 pad04[12]; struct Vec vec; u8 pad1c[0x2d]; u8 type; u8 pad4a[6]; };
struct Candidates { s32 count; s32 pad; struct Target *items[1]; };
extern struct Candidates gCommandListHead __asm__("D_800A8F90");
extern s16 gMessageReturnData[3] __asm__("D_8009F7F0");
extern s16 D_8009F7F2, D_8009F7F4;
extern void gApplySphereOverlapHit(struct Source *, struct Target *, struct Sphere *) __asm__("func_8005B5A0");
extern s32 gTraceMapRay(struct Vec *, struct Vec *, struct Vec *, s32, s32) __asm__("func_800684AC");
extern s32 gGetEntitySpawnBank(struct Target *) __asm__("func_8003BF50");
extern s32 gSpawnTypeAtOwner(void *, u8, struct Message *, s32, s32) __asm__("func_800402DC");
extern void gNormalizeVector(struct Vec *) __asm__("func_8001A180");
extern s32 gVectorDot(struct Vec *, struct Vec *) __asm__("func_8001AB00");
extern u32 gSqrt12(s32) __asm__("func_80095900");
#define ABS(x) ((((x) >> 31) + (x)) ^ ((x) >> 31))
void gTraceWeaponSphereRays(struct Owner *owner, struct Source *source) __asm__("func_8005B6A8");


/**
 * @brief Area-effect pass of a kind-0x57 object: sphere damage plus map trace.
 *
 * Walks the owner's sphere chain. For each unconsumed sphere (state <= 0),
 * unless source flags bit 3 is set, it first tests the sphere against the
 * primary target and then against the shared candidate table D_800A8F90:
 * active targets qualify when they are not the attacker, lack flag 0x80,
 * belong to another team and are not kinds 2/7/15, while kind 25 always
 * qualifies; a +/-65535 box check precedes the overlap test. Overlap delivers
 * a type-2 damage event through the target callback (message 14) and consumes
 * the sphere. Afterwards the chain segments (source position to first sphere,
 * then sphere to sphere) are traced against the map grid; the first blocked
 * segment marks the sphere and spawns a type-5 cell effect carrying the
 * radius shifted left 4.
 *
 * @param owner Kind-0x57 object; +0x24 tag, +0x68 sphere chain head.
 * @param source Effect source: flags, primary target, detail (attacker),
 *        column, position and value.
 *
 * @par Globals
 * D_800A8F90 - shared candidate table (count +0x00, items +0x08) filled by
 * the target query functions.
 *
 * @par Control flow
 * Tag-kind gate, then a linked-list walk with a damage phase and a raycast
 * phase per sphere; the first map hit returns immediately.
 *
 * @par Calls
 * func_8005B5A0 - sphere/target overlap test that fires the damage event.
 * func_800684AC - map-grid DDA segment trace, returns the first hit.
 * func_8003BF50 - classifies the attacker as kind-0x12 linked, used as row.
 * func_800402DC - map-cell spawn gate for the type-5 effect message.
 *
 * @par Side effects
 * Consumes spheres (state 10 via the overlap helper), sets sphere flag bit 0
 * on the map hit, may set source flags bit 3, and spawns a cell object.
 *
 * @par Callers
 * Confirmed: func_8005B934 (message 1 of the kind-0x57 handler, when the
 * linked target has a moving part).
 */
void gTraceWeaponSphereRays(struct Owner *owner, struct Source *source)
{
    struct Vec diff;
    struct Message message;
    struct Vec *scratch, *first;
    register struct Sphere *sphere __asm__("$16");
    struct Sphere *prev;
    struct Target *target;
    s32 i;
    if (owner->tag->kind != 0x57) return;
    sphere = owner->spheres;
    prev = 0;
    while (sphere != 0) {
        if (!(source->flags & 8) && sphere->state <= 0) {
            gApplySphereOverlapHit(source, source->target, sphere);
            for (i = 0; i < gCommandListHead.count; i++) {
                target = gCommandListHead.items[i];
                if (target->active > 0) {
                    if (source->detail->id != target->id && !(target->flags & 0x80) &&
                        source->detail->team != target->team &&
                        target->kind != 2 && target->kind != 7 && target->kind != 15) {
                    } else if (target->kind != 25) continue;
                    diff.x = target->pos.x - source->detail->pos.x;
                    diff.y = target->pos.y - source->detail->pos.y;
                    diff.z = target->pos.z - source->detail->pos.z;
                    if (ABS(diff.x) <= 65535 && ABS(diff.y) <= 65535 && ABS(diff.z) <= 65535)
                        gApplySphereOverlapHit(source, target, sphere);
                }
            }
        }
        if (prev) {
            if (gTraceMapRay(&prev->pos, &sphere->pos, &message.vec, 0, 1)) goto hit;
            goto next_sphere;
        }
        goto first_sphere;
    hit:
        sphere->flags |= 1;
        message.type = 5;
        message.value = sphere->radius << 4;
        gSpawnTypeAtOwner(0, source->column, &message, gGetEntitySpawnBank(source->detail), 0);
        return;
    first_sphere:
        if (gTraceMapRay(&source->pos, &sphere->pos, &message.vec, 0, 1)) goto hit;
    next_sphere:
        prev = sphere;
        sphere = sphere->next;
    }
}

