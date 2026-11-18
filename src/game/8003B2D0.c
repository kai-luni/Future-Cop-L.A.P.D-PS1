// Categories: Gameplay > Entity Script | Graphics > Effects > Particles | Graphics > Model Renderer
#include "game/types.h"

struct Unk3B2D0 {
    u8 pad00[0x28];
    u8 unk28[0x68];
    u32 flags90;
    u8 pad94[0xC];
    u8 unkA0;
    u8 padA1[7];
    u8 unkA8[0x20];
    s32 unkC8;
};

extern void gSetModelUvOffset(s32 low, s32 high) __asm__("func_80013AA0");
extern s32 func_800175A0(void *a, s32 b, s32 c, void *d);
extern void gFreeBlock(s32 block) __asm__("func_800181D4");
extern s32 gSpawnModelDebris(s32 message, struct Unk3B2D0 *object) __asm__("func_8003AA94");
extern void func_8003B144(s32 message, struct Unk3B2D0 *object);

/**
 * @brief Message handler of a particle-emitter object: spawn, draw, tick and
 *        release cases over the shared handler numbers.
 *
 * @param message Handler case: 1 type query, 2 spawn, 3 draw, 5 release,
 *        6 particle decay, 16 alive query.
 * @param object Entity block; +0x28 the model instance, +0x90 the flag word,
 *        +0x92/+0x93 two depth-cue bytes, +0xA0 a part selector, +0xA8 a
 *        0x20-byte parameter block, +0xC8 a pool handle.
 * @return Case 1: 0xCC. Case 16: 1. Cases 2/3/5/6: 0 (case 2 forwards the
 *         result of func_8003AA94). Default: 0.
 *
 * @par Confirmed
 * Case 2 forwards to func_8003AA94, the particle spawn helper. Case 3 saves
 * the depth-cue range with func_80013AA0(bytes +0x92, +0x93) when the upper
 * half of +0x90 is non-zero, submits the skinned parts through
 * func_800175A0(+0x28, +0xA0, +0xC8, +0xA8) and restores the range with
 * func_80013AA0(0, 0). Case 5 releases the +0xC8 handle via func_800181D4
 * when it is non-zero. Case 6 runs func_8003B144, which walks the part list
 * and drops expired parts.
 *
 * @par Likely role
 * One of the entity handlers with the shared case numbers (1/2/3/5/6/16):
 * a short-lived effect object that draws its model and decays its particles.
 *
 * @par Unknown
 * What the type id 0xCC selects, and the layout of the +0xA8 parameter block.
 *
 * @par Callers
 * No direct callers in the call graph. func_8003A8C0 registers the address
 * as request.handler of a spawn request, so the dispatcher reaches it
 * indirectly; func_8003AA94 documents message 2 as its spawn case.
 */
s32 func_8003B2D0(s32 message, struct Unk3B2D0 *object)
{
    register u32 m __asm__("$17");

    switch (message) {
    case 1:
        return 0xCC;
    case 2:
        return gSpawnModelDebris(message, object);
    case 6:
        func_8003B144(message, object);
        return 0;
    case 3:
        m = 0xFFFF0000;
        if ((object->flags90 & m) != 0) {
            gSetModelUvOffset(((u8 *)object)[0x92], ((u8 *)object)[0x93]);
        }
        func_800175A0(object->unk28, object->unkA0, object->unkC8, object->unkA8);
        if ((object->flags90 & m) != 0) {
            gSetModelUvOffset(0, 0);
            return 0;
        }
        return 0;
    case 5:
        if (object->unkC8 != 0) {
            gFreeBlock(object->unkC8);
        }
        return 0;
    case 16:
        return 1;
    default:
        return 0;
    }
}
