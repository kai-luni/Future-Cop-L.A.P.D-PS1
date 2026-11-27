// Categories: Gameplay > Vehicle Physics
#include "game/types.h"

struct UnkC14C {
    s32 unk00;
    s16 unk04;
    u8 unk06;
    u8 unk07;
    u8 unk08[6];
    u8 unk0E;
    u8 unk0F;
    u8 unk10;
    u8 unk11;
    u8 unk12[2];
    u16 unk14;
    u16 unk16;
    u16 unk18;
    u16 unk1A;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    s32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
    s32 unk50;
    s32 unk54;
    s32 unk58;
    s32 unk5C;
    s32 unk60;
};

struct UnkC14CSrc {
    u8 unk00[0x15];
    u8 unk15;
    u8 unk16;
    u8 unk17[7];
    u8 unk1E;
    u8 unk1F;
    u32 unk20;
    u32 unk24;
    u8 unk28;
    u8 unk29;
    s16 unk2A;
    u16 unk2C;
    u8 unk2E[4];
    u16 unk32;
};

extern s32 gIdentityMatrix[9] __asm__("D_8009AA38");
void gInitSpawnMotionState(struct UnkC14C *dest, struct UnkC14CSrc *src) __asm__("func_8003C14C");


/**
 * @brief Initialises a motion state block from a source record.
 *
 * The template D_8009AA38 fills the nine words at 0x34..0x54, which start out
 * as the neutral tuning values. Two byte selectors are copied (0x15 and 0x16),
 * then the speed limits are derived: 0x16 receives (src->unk20 * 75) >> 14,
 * 0x04 receives (src->unk24 * 75) >> 14 and the third value (src->unk2A * 75)
 * >> 10 goes to 0x14. The remaining fields are taken over unchanged
 * (0x28, 0x32, 0x2C, 0x1E) while the accumulators 0x28, 0x2C, 0x30, 0x58,
 * 0x5C, 0x60, 0x24, 0x0F, 0x10, 0x00 and 0x1C are cleared. A negative
 * acceleration is clamped to 0 when the first selector byte is set.
 *
 * @param dest Motion block that is filled.
 * @param src Source record with the raw tuning values.
 *
 * @par Globals
 * D_8009AA38 - nine-word neutral template.
 *
 * @par Control flow
 * Straight-line assignment with one clamp at the end.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Fully initialises the destination block.
 *
 * @par Callers
 * Confirmed: func_8007043C, func_800762B0. Likely: func_80043430,
 * func_8004C920, func_8006EB80, func_80074024.
 */
void gInitSpawnMotionState(struct UnkC14C *dest, struct UnkC14CSrc *src)
{
    dest->unk06 = src->unk15;
    dest->unk11 = src->unk16;

    dest->unk34 = gIdentityMatrix[0];
    dest->unk38 = gIdentityMatrix[1];
    dest->unk3C = gIdentityMatrix[2];
    dest->unk40 = gIdentityMatrix[3];
    dest->unk44 = gIdentityMatrix[4];
    dest->unk48 = gIdentityMatrix[5];
    dest->unk4C = gIdentityMatrix[6];
    dest->unk50 = gIdentityMatrix[7];
    dest->unk54 = gIdentityMatrix[8];

    dest->unk28 = 0;
    dest->unk2C = 0;
    dest->unk30 = 0;
    dest->unk58 = 0;
    dest->unk5C = 0;
    dest->unk60 = 0;

    dest->unk16 = (src->unk20 * 75) >> 14;
    dest->unk04 = (src->unk24 * 75) >> 14;
    dest->unk07 = src->unk28;
    dest->unk24 = 0;
    dest->unk0F = 0;
    dest->unk1A = src->unk32;
    dest->unk10 = 0;
    dest->unk18 = src->unk2C;
    dest->unk0E = src->unk1E;
    dest->unk00 = 0;
    dest->unk1C = 0;

    if (dest->unk06 != 0 && dest->unk04 < 0) {
        dest->unk04 = 0;
    }

    dest->unk14 = ((u32)src->unk2A * 75) >> 10;
}
