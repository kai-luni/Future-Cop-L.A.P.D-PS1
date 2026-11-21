// Categories: Gameplay > Path Following | Utilities > Mathematics > Fixed Point
#include "game/types.h"

typedef struct {
    u8 pad00[4];
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    u8 pad10[0x18];
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s16 unk34;
    s16 unk36;
    s16 unk38;
    s16 unk3A;
    s16 unk3C;
    u16 unk3E;
    s32 unk40;
    s16 unk44;
} Unk6EEF8;
typedef struct { s32 x, y, z; } V3;

extern s32 gNormalizeGetLength(s32 *vec) __asm__("func_8001A314");
void gInitFlightSegmentNode(Unk6EEF8 *node, s32 scale, s16 arg2, s32 arg3, s32 arg4, s32 *p1, s32 *p2) __asm__("func_8006EEF8");


/**
 * @brief Initialises a path segment: direction, speed-scaled velocity and
 *        the remaining duration from the segment length.
 *
 * @param node Segment record (fields +0x04..+0x44).
 * @param scale Speed scale.
 * @param arg2 Stored at +0x44 (likely a sound/flag id).
 * @param arg3 Time base (300 units per second).
 * @param arg4 Stored at +0x40.
 * @param p1 Segment start position.
 * @param p2 Segment end position.
 * @return Nothing.
 *
 * @par Confirmed
 * vec = p2 - p1, length via func_8001A314; sv = vec >> 4. The velocity
 * words +0x34..+0x38 = (sv * scale) >> 12. Duration +0x3C = max(0,
 * (length >> 4) - scale) * 300 / (arg3 >> 4). Position +0x04 = p1; the
 * per-frame step words +0x28..+0x30 = (sv * (arg3 / 300)) >> 8; +0x3E =
 * 0x8000, +0x3A = 0.
 *
 * @par Likely role
 * The segment constructor of the mover system (func_8006F0E8): the two
 * velocity sets serve the interpolated and the per-frame integration
 * paths, the duration counts down in 300-unit seconds.
 *
 * @par Unknown
 * What the 0x8000 marker at +0x3E gates.
 *
 * @par Callers
 * func_8006F0E8.
 */
void gInitFlightSegmentNode(Unk6EEF8 *node, s32 scale, s16 arg2, s32 arg3, s32 arg4, s32 *p1, s32 *p2)
{
    s32 vec[3];
    s16 sv[3];
    s32 t;

    vec[0] = p2[0] - p1[0];
    vec[1] = p2[1] - p1[1];
    vec[2] = p2[2] - p1[2];
    t = gNormalizeGetLength(vec);
    sv[0] = vec[0] >> 4;
    sv[1] = vec[1] >> 4;
    sv[2] = vec[2] >> 4;
    node->unk34 = (sv[0] * scale) >> 12;
    node->unk36 = (sv[1] * scale) >> 12;
    node->unk38 = (sv[2] * scale) >> 12;
    t = (t >> 4) - scale;
    if (t < 0) {
        node->unk3C = 0;
    } else {
        node->unk3C = (t * 300) / (arg3 >> 4);
    }
    *(V3 *)&node->unk04 = *(V3 *)p1;
    arg3 = arg3 / 300;
    node->unk28 = (sv[0] * arg3) >> 8;
    node->unk2C = (sv[1] * arg3) >> 8;
    node->unk30 = (sv[2] * arg3) >> 8;
    node->unk3E = 0x8000;
    node->unk3A = 0;
    node->unk40 = arg4;
    node->unk44 = arg2;
}
