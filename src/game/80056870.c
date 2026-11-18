// Categories: Graphics > Camera > Placement
#include "game/types.h"

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk56870 {
    u8 unk00[0x5C];
    struct Vec3s32 position;
};

extern u32 func_80019668(void);
extern void gPlaceCameraFromPreset(struct Unk56870 *object, s32 index, struct Vec3s32 *out, s32 apply) __asm__("func_8005678C");
extern s32 func_80055EE8(struct Vec3s32 *from, struct Vec3s32 *to);

/**
 * @brief Searches the alternate camera presets for an unblocked view of the
 *        followed object.
 *
 * @param object Follow target (position at +0x5C).
 * @return A free preset slot (9..16), or -1 when all eight tries blocked.
 *
 * @par Confirmed
 * The start slot is random ((func_80019668() & 7) + 9). Up to 8 tries:
 * func_8005678C computes the preset position without applying it,
 * func_80055EE8 tests the line from the object to it; a zero result accepts
 * the slot. The slot increments and wraps 16 -> 9.
 *
 * @par Likely role
 * The blocked-view fallback of the chase camera func_800569EC: when the
 * current preset looks through geometry, the rig hops to a random alternate
 * angle until one has line of sight.
 *
 * @par Callers
 * func_800569EC.
 */
s32 func_80056870(struct Unk56870 *object)
{
    struct Vec3s32 out;
    struct Vec3s32 position;
    u32 slot;
    s32 tries;
    s32 blocked;

    slot = (func_80019668() & 7) + 9;
    position = object->position;
    for (tries = 8; tries > 0; tries--) {
        gPlaceCameraFromPreset(object, slot, &out, 0);
        blocked = func_80055EE8(&position, &out);
        if (blocked == 0) {
            break;
        }
        slot++;
        if (slot >= 0x11) {
            slot = 9;
        }
    }
    if (blocked != 0) {
        return -1;
    }
    return slot;
}
