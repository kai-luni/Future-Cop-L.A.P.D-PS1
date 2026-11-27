// Categories: Gameplay > Map > Zones | Graphics > Camera
#include "game/types.h"

struct Unk31Node {
    s16 x;
    s16 unk02;
    s16 z;
    s16 unk06;
    s32 id;
    s16 extentX;
    s16 unk0E;
    s16 extentZ;
    s16 unk12;
    u8 flags;
    u8 unk15;
    s16 unk16;
    u8 unk18[0x0C];
    struct Unk31Node *next;
};

struct Vec3s32 {
    s32 x;
    s32 y;
    s32 z;
};

extern struct Unk31Node *gMapZoneList __asm__("D_8009F6F8");
s32 gTestCameraZoneOverlap(struct Vec3s32 *position, s32 radius) __asm__("func_80031B40");


/**
 * @brief Tests whether a position plus radius overlaps any active map zone
 *        rectangle.
 *
 * @param position Query point.
 * @param radius Query radius.
 * @return 1 when a flagged zone overlaps, else 0.
 *
 * @par Confirmed
 * Walks the D_8009F6F8 zone list; only nodes with flag 0x40 are tested.
 * Per axis: |position - (node coord << 8)| - radius <= extent << 4 (both
 * x and z must pass); the first hit returns 1.
 *
 * @par Likely role
 * The camera zone test of the chase rig func_800569EC: flagged zones are
 * likely indoor/covered areas that switch the camera preset, the shifted
 * coordinates match the map grid scale.
 *
 * @par Unknown
 * What flag 0x40 marks and who fills the zone list.
 *
 * @par Callers
 * func_800569EC.
 *
 * @par Environment path
 * Returns 1 when a position widened by a radius overlaps any zone in
 * gMapZoneList (D_8009F6F8) whose flag byte (+0x14) has bit 0x40, testing
 * |pos - (zone coord << 8)| - radius <= extent << 4 on x and then z;
 * otherwise 0. The 0x28-byte zones are pushed by func_800319D4 and the list
 * is cleared by func_80031984 at mission boot and freed by gFreeMapZoneList
 * (func_80031990). Its only caller gUpdateChaseCamera (func_800569EC) uses it
 * at 800569EC.c:243, only while the camera state is 5 or 6, with the object
 * position and its collision radius, and falls back to state 0 when no zone
 * overlaps, so bit 0x40 marks zones that hold the camera in those special
 * states.
 */
s32 gTestCameraZoneOverlap(struct Vec3s32 *position, s32 radius)
{
    struct Unk31Node *node;
    s32 found;
    s32 delta;
    s32 sign;

    found = 0;
    for (node = gMapZoneList; node != 0; node = node->next) {
        if (node->flags & 0x40) {
            delta = position->x - (node->x << 8);
            sign = delta >> 31;
            if (((sign + delta) ^ sign) - radius <= node->extentX << 4) {
                delta = position->z - (node->z << 8);
                sign = delta >> 31;
                if (((sign + delta) ^ sign) - radius <= node->extentZ << 4) {
                    found = 1;
                    break;
                }
            }
        }
    }
    return found;
}
