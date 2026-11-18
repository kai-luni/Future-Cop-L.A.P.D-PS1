// Categories: Gameplay > Weapons | Graphics > Effects | Game Engine > Object Pool
#include "game/types.h"

extern int gWeaponFireDispatch();
typedef struct Obj Obj;
typedef struct Entry Entry;
struct Obj { u8 pad0[0x16]; u8 kind; u8 pad17; u8 flags; u8 pad19[0x93]; void *data; };
struct Entry { Obj *obj; Obj *owner; u8 pad08[4]; s32 time; s32 value; u8 pad14[0x38]; s16 low; s16 high; s16 other; u16 flags; s32 index; };
typedef struct { s16 other, low, high; } Params;
typedef struct { u8 pad[0xA]; s16 value, other, low, high; } Extra;
extern Entry *gDomeEffectBlock[4] __asm__("D_800A22F0");
extern s32 gFrameTime __asm__("D_8009FAA4");
extern Entry *func_8005D3FC(Obj *, s32);

/**
 * @brief Finds or allocates a 4-slot D_800A22F0 weapon-effect block for an owner and updates its active timing and parameters.
 *
 * Confirmed: searches D_800A22F0[0..3] for an entry matching owner == arg0, else allocates one via func_8005D3FC(arg0, 0) and initializes low/high/other from arg1 (clamping high >= (low * 8) / 5); for kind == 1 with non-null arg2, toggles flags bit 0 when D_8009FAA4 > time, sets time = D_8009FAA4 + 60, and when bit 0 is set copies value/low/high/other from arg2 and toggles bit 2 of e->flags and e->obj->flags from *(s16 *)((u8 *)arg0->data + index * 2 + 0x8A); otherwise sets time = D_8009FAA4 + 120 and flags |= 1.
 * Category follows 4-slot effect array D_800A22F0, kind == 1 player check, and weapon dispatch callers func_8005F6E8 (case 13) and gWeaponFireDispatch (case 0xD).
 * Likely: drives a persistent toggleable beam/special weapon effect (e.g., player vs AI sustained effect) with timed flicker/state gating; Unknown: exact effect type in D_800A22F0 and meaning of params fields low/high/other.
 *
 * @param arg0 Owner entity pointer (+0x16 kind, +0xAC data).
 * @param arg1 Default Params triple (other, low, high) applied when allocating a new D_800A22F0 slot.
 * @param arg2 Optional Extra block (+0x0A..+0x10) passed for player kind-1 updates (null from AI caller func_8005F6E8).
 * @return 1 if kind == 1, arg2 != 0, and (e->flags & 1) is set after update; 0 otherwise (including allocation failure).
 *
 * @par Globals
 * D_800A22F0 - 4-element array of active Entry pointers; D_8009FAA4 - global frame timestamp.
 * @par Control flow
 * 4-iteration search loop, allocation/init guard on !e, then kind == 1 && arg2 != 0 branch vs fallback branch.
 * @par Calls
 * func_8005D3FC - allocates and initializes a 0x1B0-byte Entry in a free D_800A22F0 slot with sub-object via func_8006C84C.
 * @par Side effects
 * Allocates/updates D_800A22F0 slot entries and toggles flag bit 2 on e->flags and e->obj->flags.
 * @par Callers
 * Confirmed: func_8005F6E8 (0x8005FD3C, case 13 with arg2 = 0); gWeaponFireDispatch (0x8006B13C, case 0xD with arg2 = def) / Likely: none / Unknown: none.
 */
s32 gWeaponEffectUpdate(Obj *arg0, Params *arg1, Extra *arg2) __asm__("func_8005D690");

s32 gWeaponEffectUpdate(Obj *arg0, Params *arg1, Extra *arg2)
{
    register Obj *obj __asm__("$17") = arg0;
    register Params *params __asm__("$18") = arg1;
    register Extra *extra __asm__("$19") = arg2;
    s32 result = 0;
    Entry *e;
    s32 i;
    register Obj *o __asm__("$4");
    register u8 *base __asm__("$3");
    register s32 idx __asm__("$2");
    __asm__("" : "+r"(result));
    e = 0;
    for (i = 0; i < 4; i++) {
        if (gDomeEffectBlock[i] && gDomeEffectBlock[i]->owner == obj) {
            e = gDomeEffectBlock[i];
            break;
        }
    }
    if (!e) {
        e = func_8005D3FC(obj, 0);
        if (!e) return 0;
        e->low = params->low;
        e->high = params->high;
        if (e->high < e->low) e->high = (e->low * 8) / 5;
        e->other = params->other;
    }
    if (obj->kind == 1 && extra) {
        o = obj;
        if (gFrameTime > e->time) {
            if (e->flags & 1) e->flags &= ~1;
            else e->flags |= 1;
        }
        __asm__("" : "+r"(o));
        e->time = gFrameTime + 60;
        if (e->flags & 1) {
            e->value = extra->value;
            __asm__("" : : : "memory");
            idx = e->index;
            __asm__("" : "+r"(idx));
            base = (u8 *)o->data + idx * 2;
            __asm__("" : "+r"(base));
            if (*(s16 *)(base + 0x8A) > 0) {
                e->flags |= 4;
                e->obj->flags &= ~4;
            } else {
                e->flags &= ~4;
                e->obj->flags |= 4;
            }
            e->low = extra->low;
            e->high = extra->high;
            if (e->high < e->low) e->high = (e->low * 8) / 5;
            e->other = extra->other;
            result = 1;
        } else result = 0;
    } else {
        e->time = gFrameTime + 120;
        e->flags |= 1;
    }
    return result;
}
