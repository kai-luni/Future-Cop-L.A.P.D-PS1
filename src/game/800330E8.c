#include "game/types.h"

struct Vec3 {
    s32 x;
    s32 y;
    s32 z;
};

struct Unk330E8Entry {
    struct Vec3 unk00;
    struct Vec3 unk0C;
    s16 unk18;
    u8 unk1A;
    u8 unk1B;
};

struct Unk330E8Obj {
    u8 pad00[0xDC];
    struct Vec3 unkDC;
    u8 padE8[0x4C];
    struct Unk330E8Entry *unk134;
};

struct Unk330E8Global {
    u8 pad00[0xE0];
    s32 unkE0;
};

extern struct Unk330E8Global *D_8009FA94;

/**
 * Updates an object's strategic position according to its AI mode.
 *
 * Reads the AI mode from the entry referenced at object offset 0x134
 * and writes the selected position to the object's vector at 0xDC.
 *
 * Mode behavior:
 *   0 - Sets the position to the component-wise midpoint between
 *       the entry's positions A (0x00) and B (0x0C).
 *   1 - Copies position A into the object's strategic position
 *       and sets the global field at offset 0xE0 to 1.
 *   2 - Copies position B into the object's strategic position.
 *
 * Other mode values leave the strategic position unchanged.
 *
 * The midpoint uses signed integer arithmetic with a right shift,
 * matching the original fixed-point/integer behavior.
 *
 * Side effects:
 *   - May update the object's position vector at offset 0xDC.
 *   - Sets the global field at offset 0xE0 to 1 in mode 1.
 *
 * Note: The strategic interpretation of positions A and B is
 * inferred from their usage in the main AI controller.
 */
void func_800330E8(struct Unk330E8Obj *obj)
{
    u8 type = obj->unk134->unk1A;
    struct Unk330E8Entry *e = obj->unk134;

    switch (type) {
    case 2:
        obj->unkDC = e->unk0C;
        break;
    case 1:
        obj->unkDC = e->unk00;
        D_8009FA94->unkE0 = type;
        break;
    case 0:
        obj->unkDC.x = e->unk0C.x + ((e->unk00.x - e->unk0C.x) >> 1);
        obj->unkDC.y = obj->unk134->unk0C.y + ((obj->unk134->unk00.y - obj->unk134->unk0C.y) >> 1);
        obj->unkDC.z = obj->unk134->unk0C.z + ((obj->unk134->unk00.z - obj->unk134->unk0C.z) >> 1);
        break;
    }
}
