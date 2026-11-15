// Categories: Gameplay > Entity Script | Graphics > Effects
#include "game/types.h"

struct Obj {
    u8 pad00[0x12];
    u16 field12;
    u8 pad14[6];
    u16 flags1a;
    u8 pad1c[0x56];
    u16 field72;
    u8 pad74[2];
    u16 field76, field78, field7a, field7c, field7e;
    u8 pad80[6];
    s16 field86;
    u8 pad88[0x10];
    u16 flags98, field9a;
    void *mesh;
};

extern s32 func_8003C7B8(s32, struct Obj *, s32, s32);
extern s32 func_8003C980(s32, struct Obj *, s32, s32);
extern void func_8006E720(struct Obj *);
extern void *func_8006C84C(s16, s32);
extern void func_8006CA68(void *);
extern void func_800181D4(void *);

/**
 * @brief Message handler of a mesh-carrying effect object: init with an
 *        optional pool mesh, release, alive query, event fallback.
 *
 * @param message Handler case (0 init, 1/9 idle, 6 alive, 10 release).
 * @param obj Effect block; +0x12 a u16 id, +0x1A flags, +0x72..+0x7E five
 *        s16 fields, +0x86 an s16 counter, +0x98 flags, +0xA0 the mesh.
 * @param arg2, arg3 Forwarded to the shared hooks.
 * @return Case 0: 1 on success (0 when the mesh allocation fails). Case 6:
 *         1 when the counter is 0. Case 10: 0. Default: func_8003C980's
 *         result. Cases 1/9: 0.
 *
 * @par Confirmed
 * func_8003C7B8 runs first and short-circuits to 0 when it returns non-zero.
 * Case 0 runs func_8006E720, sets field72 = 0, field76 = 20, copies
 * field78/7a into 7c/7e and field12 into 9a; with flag bit 0x40 it builds
 * the mesh flags (0x12 or 2 by bit 0x100, |1 by bit 0x80, |8 when flags1a
 * & 0x104) and allocates via func_8006C84C(0x100, flags). Case 10 releases
 * the mesh via func_8006CA68 (when flagged) and frees the object. Other
 * messages fall through to the event handler func_8003C980.
 *
 * @par Likely role
 * The handler of the pooled effect objects: func_8006EB80 returns its
 * address as the class handler, the mesh is the deformable effect surface
 * (the 0x100 pool tag matches the effect pool family).
 *
 * @par Unknown
 * What the five s16 fields +0x72..+0x7E animate.
 *
 * @par Callers
 * No direct callers in the call graph. func_8006EB80 references the address
 * as a class handler, so the dispatcher reaches it indirectly.
 */
s32 func_8006E814(s32 message, struct Obj *obj, s32 arg2, s32 arg3)
{
    s32 flags;
    if (func_8003C7B8(message, obj, arg2, arg3)) goto ret0;
    switch (message) {
    case 0:
        func_8006E720(obj);
        obj->field72 = 0;
        obj->field76 = 20;
        obj->field7c = obj->field78;
        obj->field7e = obj->field7a;
        obj->field9a = obj->field12;
        if (obj->flags98 & 0x40) {
            flags = (obj->flags98 & 0x100) ? 0x12 : 2;
            if (obj->flags98 & 0x80) flags |= 1;
            if (obj->flags1a & 0x104) flags |= 8;
            obj->mesh = func_8006C84C(0x100, (u8)flags);
            if (!obj->mesh) return 0;
        }
        return 1;
    case 10:
        if ((obj->flags98 & 0x40) && obj->mesh) func_8006CA68(obj->mesh);
        func_800181D4(obj);
        return 0;
    case 6:
        return obj->field86 == 0;
    case 1:
    case 9:
        goto ret0;
    default:
        return func_8003C980(message, obj, arg2, arg3);
    }
ret0:
    return 0;
}
