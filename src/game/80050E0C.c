// Categories: Gameplay > Path Following | Physics > Motion | Gameplay > Map
#include "game/types.h"

struct Unk50DACCell {
    s16 unk00;
    s16 x;
    s16 unk04;
    s16 z;
    s32 unk08;
};

struct Unk50DACMap {
    s16 unk00;
    s16 count;
    s32 unk04;
    struct Unk50DACCell cells[1];
};

struct Unk50E0CCellAccess {
    u16 pad0;
    u16 pad2;
    u16 pad4;
    u16 unk6;
    u16 unk8;
    u16 unkA;
};

struct Unk50E0CArg0 {
    u16 unk00;
    s16 unk02;
    u16 unk04;
    u16 unk06;
    s16 unk08;
    s16 unk0A;
    u16 unk0C;
    u16 unk0E;
    u16 unk10;
    u8 unk12;
};

struct Unk50E0CArg2 {
    u8 *unk00;
};

struct Unk50E0CArg3 {
    u8 pad00[0x24];
    u32 unk24;
    u8 pad28[0x54 - 0x28];
    s32 unk54;
    u8 pad58[0x78 - 0x58];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
    s32 unk84;
};

struct Unk50E0COut {
    u8 *unk00;
    u16 unk04;
    u16 unk06;
    u16 unk08;
    u16 unk0A;
    u16 unk0C;
    u16 unk0E;
    u8 pad10[0x1C - 0x10];
    s32 unk1C;
    s16 unk20;
    u8 pad22[0x26 - 0x22];
    u16 unk26;
    u8 pad28[0x2E - 0x28];
    u16 unk2E;
    s16 unk30;
    s16 unk32;
    u16 unk34;
    s16 unk36;
    s16 unk38;
    u16 unk3A;
    u32 unk3C;
    u16 unk40;
    u16 unk42;
    s16 unk44;
    s16 unk46;
    s16 unk48;
    s16 unk4A;
    s16 unk4C;
    s16 unk4E;
    s16 unk50;
    s16 unk52;
    s16 unk54;
    s16 unk56;
    u16 unk58;
    u16 unk5A;
    s32 unk5C;
    u8 pad60[0x64 - 0x60];
    s32 unk64;
    s32 unk68;
    s32 unk6C;
};

extern s32 func_80050DAC(struct Unk50DACMap *map, s32 *position);
extern s32 func_8004F330(struct Unk50E0COut *out, struct Unk50E0CArg3 *arg3, s32 index);
extern void gVectorScaleInPlace(s32 scale, s32 *vector) __asm__("func_8001AB78");
extern s32 gVectorLength3D(s32 *vector) __asm__("func_8001A100");
extern s32 gAtan2(s32 x, s32 y) __asm__("func_80095C30");
extern void gSinCos(u32 angle, s32 *sine, s32 *cosine) __asm__("func_8001846C");
void gInitRailSegment(struct Unk50E0CArg0 *arg0, struct Unk50E0COut *out, struct Unk50E0CArg2 *arg2, struct Unk50E0CArg3 *arg3, s32 *arg4) __asm__("func_80050E0C");


/**
 * @brief Initialises a rail/spline mover block for the map cell that contains a
 *        position: cell triple, segment vector, length, yaw, pitch and the
 *        per-part speeds.
 *
 * @param arg0 Tuning record: +0x00 flag word (bit 0x10 suppresses the head
 *        offset), +0x02 base speed, +0x04 head offset, +0x06 turn factor,
 *        +0x08 acceleration, +0x0A deceleration, +0x0C, +0x0E, +0x10 further
 *        per-part bytes and +0x12 the top byte merged into the flag word.
 * @param out Mover state block, the same layout func_8005027C integrates:
 *        +0x00 map, +0x04/+0x06/+0x08 segment start, +0x0A/+0x0C/+0x0E segment
 *        vector, +0x1C progress, +0x20 length, +0x26 head offset, +0x30 base
 *        speed, +0x32 target speed, +0x34/+0x36/+0x38 turn, acceleration,
 *        deceleration, +0x3C flags, +0x40 hold timer, +0x44/+0x46/+0x48/+0x4A
 *        cell indices, +0x4C/+0x4E/+0x50 yaw, +0x52/+0x54/+0x56 pitch,
 *        +0x64/+0x6C yaw sine and cosine, +0x68 the segment's vertical
 *        component.
 * @param arg2 Descriptor whose +0x00 word points at the map data block; the map
 *        header with the cell list starts at +0x0C of that block. A null
 *        pointer clears out->unk00 and returns.
 * @param arg3 Render object: +0x24 gains bit 31, +0x78/+0x7C/+0x80 receive the
 *        segment direction in 8.8 fixed point, +0x54 the part height and +0x84
 *        the speed.
 * @param arg4 Position vector (three s32, eight fraction bits) used for the cell
 *        search.
 *
 * @return Nothing.
 *
 * @par Confirmed
 * arg3->unk24 |= 0x80000000, map = arg2->unk00 + 0x0C, out->unk00 = map, then
 * index = func_80050DAC(map, arg4), a linear search of the cell list for
 * position >> 8. The start cell's three half-words are copied to out->unk04,
 * +0x06 and +0x08, and out->unk48 and out->unk4A both get the cell index.
 * out->unk1C and out->unk5C are zeroed, out->unk42 = arg0->unk10 and out->unk3C
 * = arg0->unk00 | (arg0->unk12 << 24).
 * out->unk44 = func_8004F330(out, arg3, out->unk48) selects the link to the next
 * cell (a negative result sets flag 0x40), out->unk46 becomes -1, and the next
 * cell's triple minus the start triple is stored in out->unk0A/+0x0C/+0x0E.
 * The direction is scaled into arg3->unk78/+0x7C/+0x80 as differences << 8,
 * its length comes from func_8001A100, out->unk20 = length >> 8, and the vector
 * is normalised with func_8001AB78 using 0x80000000 / (length >> 1) - skipped
 * when the length is below 2, which would divide by zero.
 * Yaw: func_80095C30(x, z) & 0xFFF into out->unk4C, +0x4E and +0x50, then
 * func_8001846C(yaw, +0x64, +0x6C) stores its sine and cosine.
 * Pitch: the horizontal length is the octagonal approximation of |x| and |z|
 * (swap, then a + (b >> 2) - (b >> 6) with the 0x7F/0x20 correction when the
 * ratio is close to one), func_80095C30(y, that) gives the angle and
 * (0xFFF - angle) & 0xFFF lands in out->unk52, +0x54 and +0x56.
 * Height: out->unk26 = arg0->unk04, the offset << 4 is suppressed when it is
 * positive and flag 0x10 is set, and arg3->unk54 = offset + (start y << 8);
 * arg3->unk84 = 0.
 * Speeds: out->unk30 = out->unk32 = ((arg0->unk02 << 8) / 300) >> 4,
 * out->unk36 = (arg0->unk08 << 8) / 300, out->unk38 = (arg0->unk0A << 8) / 300,
 * out->unk34 = arg0->unk06, out->unk3A = arg0->unk0C, out->unk2E = arg0->unk0E,
 * out->unk40, out->unk58 and out->unk5A cleared.
 *
 * @par Likely role
 * The segment-setup counterpart of the mover integrator func_8005027C: it
 * rewrites the block func_8005027C then integrates (same offsets for progress,
 * length, speeds, hold timer, index pair, yaw and pitch), so calling it
 * re-seats a vehicle or part on the road/rail network at a known position. The
 * /300 scalings match the 300-tick hold timer used by the AI step
 * func_80037B28.
 *
 * @par Unknown
 * Whether the cell triple is x/height/z or x/z/height - func_80050DAC compares
 * its own +0x02 and +0x06 fields with the position, which makes the middle word
 * the height, but the two views use different cell bases. Also unclear: the
 * meaning of the cell's trailing s32 and of the map header word at +0x04.
 *
 * @par Callers
 * func_800378BC, func_80039C58, func_80039E8C - all three are entity message
 * handlers that call it from their part-update case, passing the render object
 * as arg3 and the object position as arg4.
 *
 * @par Environment path
 * Seats a route-following mover on its route: with a route descriptor it
 * marks the render object (+0x24 bit 31), finds the route node containing the
 * position with func_80050DAC in the node list at descriptor+0x0C, stores
 * that node's coordinates as the segment start and its index at +0x48/+0x4A,
 * picks the next node with func_8004F330 (flag 0x40 when there is none), and
 * derives the segment vector, its length (func_8001A100), the normalised
 * direction written to object+0x78..+0x80, yaw (func_80095C30, with
 * sine/cosine via gSinCos) and pitch, plus base speed, acceleration and
 * deceleration scaled by /300 from the tuning record. A null descriptor only
 * clears out+0x00. Callers are gInitRailMoverEntity (func_800378BC),
 * gHandleCameraOverrideMsg (func_80039C58) and func_80039E8C, which call it
 * when such an entity is (re)placed on its route; it uses no globals.
 */
void gInitRailSegment(struct Unk50E0CArg0 *arg0, struct Unk50E0COut *out, struct Unk50E0CArg2 *arg2, struct Unk50E0CArg3 *arg3, s32 *arg4)
{
    struct Unk50DACMap *map;
    struct Unk50E0CCellAccess *cell;
    s16 index;
    s32 length;
    s32 quotient;
    s32 angle;
    s32 angle2;
    s32 a0;
    s32 a1;
    s32 a2;
    s32 a3;
    s32 t0;
    s32 v0;
    s32 v1;

    if (arg2 == 0) {
        out->unk00 = 0;
        return;
    }

    arg3->unk24 |= 0x80000000U;
    map = (struct Unk50DACMap *)(arg2->unk00 + 0xC);
    out->unk00 = (u8 *)map;
    index = func_80050DAC(map, arg4);

    cell = (struct Unk50E0CCellAccess *)((u8 *)map + 4) + index;
    out->unk48 = out->unk4A = index;
    out->unk04 = cell->unk6;
    out->unk06 = cell->unk8;
    out->unk08 = cell->unkA;

    out->unk1C = 0;
    out->unk5C = 0;
    out->unk42 = arg0->unk10;
    out->unk3C = arg0->unk00;
    out->unk3C |= (u32)arg0->unk12 << 24;

    out->unk44 = func_8004F330(out, arg3, out->unk48);
    out->unk46 = -1;
    if (out->unk44 < 0) {
        out->unk3C |= 0x40;
    }

    index = out->unk44;
    cell = (struct Unk50E0CCellAccess *)((u8 *)map + 4) + index;
    out->unk0A = cell->unk6;
    out->unk0C = cell->unk8;
    out->unk0E = cell->unkA;
    out->unk0A -= out->unk04;
    out->unk0C -= out->unk06;
    out->unk0E -= out->unk08;

    arg3->unk78 = (s16)out->unk0A << 8;
    arg3->unk7C = (s16)out->unk0C << 8;
    arg3->unk80 = (s16)out->unk0E << 8;

    length = gVectorLength3D(&arg3->unk78);
    out->unk20 = length >> 8;

    if ((((length >> 31) + length) ^ (length >> 31)) >= 2) {
        quotient = 0x80000000U / (length >> 1);
    } else {
        quotient = 0;
    }
    gVectorScaleInPlace(quotient, &arg3->unk78);

    angle = gAtan2(arg3->unk78, arg3->unk80) & 0xFFF;
    out->unk4C = out->unk4E = out->unk50 = angle;
    gSinCos(out->unk50, &out->unk64, &out->unk6C);

    a3 = arg3->unk78;
    if (a3 < 0) {
        a3 = -a3;
    }
    a2 = arg3->unk80;
    if (a2 < 0) {
        a2 = -a2;
    }
    if (a3 < a2) {
        a3 ^= a2;
        a2 ^= a3;
        a3 ^= a2;
    }
    if ((a3 >> 1) < a2) {
        a0 = a3 + a2;
        t0 = a0 << 6;
        v1 = a0 << 3;
        t0 += v1;
        v1 = a0 << 1;
        t0 += v1;
        t0 += a3;
        v1 = a2 << 5;
        a0 = t0 + v1;
        a2 = a3 - a2;
        a3 = a0 >> 7;
    }
    a1 = a3 + (a2 >> 2) - (a2 >> 6);
    angle2 = gAtan2(arg3->unk7C, a1);
    angle2 = (angle2 << 12) >> 12;
    v1 = (0xFFF - angle2) & 0xFFF;
    out->unk52 = out->unk54 = out->unk56 = v1;
    out->unk68 = arg3->unk7C;

    v1 = arg0->unk04;
    out->unk26 = v1;
    a1 = (s16)v1 << 4;
    if (a1 > 0 && (out->unk3C & 0x10)) {
        a1 = 0;
    }

    v0 = (s16)out->unk06;
    arg3->unk84 = 0;
    arg3->unk54 = a1 + (v0 << 8);

    v0 = ((arg0->unk02 << 8) / 300) >> 4;
    out->unk32 = v0;
    out->unk30 = v0;

    out->unk36 = (arg0->unk08 << 8) / 300;

    out->unk38 = (arg0->unk0A << 8) / 300;

    out->unk34 = arg0->unk06;
    out->unk3A = arg0->unk0C;
    out->unk2E = arg0->unk0E;
    out->unk40 = 0;
    out->unk5A = 0;
    out->unk58 = 0;
}
