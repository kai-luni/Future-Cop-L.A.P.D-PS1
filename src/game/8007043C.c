// Categories: Gameplay > Object Spawning > Constructor | Game Engine > Object Setup
#include "game/types.h"

extern int gSpawnQueuedEntity();

typedef struct {
    u8 unk00[0x34];
    u16 unk34;
    u8 unk36;
    u8 pad37;
    u16 unk38;
    u16 unk3A;
    u16 unk3C;
    u8 unk3E;
    u8 unk3F;
    u8 unk40;
    u8 unk41;
    u8 unk42;
    u8 unk43;
    u8 unk44;
    u8 unk45;
    u8 unk46;
    u8 unk47;
    u8 unk48;
    u8 unk49;
    s16 unk4A;
    s16 unk4C;
    s16 unk4E;
    u16 unk50;
    u8 unk52;
    u8 pad53;
    u16 unk54;
    u16 unk56;
} Source7043C;

typedef struct {
    u8 unk00[8];
    s16 unk08;
    u16 unk0A;
    u8 unk0C[0x12 - 0x0C];
    u16 unk12;
    u8 unk14[0x24 - 0x14];
    s32 unk24;
    u8 unk28[0x66 - 0x28];
    u16 unk66;
    u16 unk68;
    u16 unk6A;
    u8 unk6C;
    u8 unk6D;
    u8 unk6E;
    u8 unk6F;
    u8 unk70;
    u8 unk71;
    u8 unk72;
    u8 unk73;
    u8 unk74;
    u8 unk75;
    u8 unk76;
    u8 unk77;
    u8 unk78[0x8C - 0x78];
    u16 unk8C;
    u8 unk8E;
    u8 unk8F;
    u16 unk90;
    u8 unk92[0x94 - 0x92];
    s16 unk94;
    s16 unk96;
    s16 unk98;
    u16 unk9A;
} Dest7043C;

typedef struct {
    u8 unk00[0x18];
    s32 x;
    s32 y;
    s32 z;
} Work7043C;

extern u8 D_8006FF44[];
extern void *gAllocMemory(s32 size, s32 tag) __asm__("func_80017EBC");
extern void gInitSpawnMotionState(Dest7043C *dest, Source7043C *src) __asm__("func_8003C14C");
extern s32 func_80019668(void);
extern void gBuildRotationMatrix(s32 x, s32 y, s32 z, Work7043C *work) __asm__("func_800188D0");
void *gCreateKind59Actor(Source7043C *src) __asm__("func_8007043C");


/**
 * @brief Constructor for spawn kind 0x59: allocates the 0x9C-byte scene object,
 *        installs its update handler and copies the definition's shape, colour and
 *        jitter parameters.
 *
 * @param src Spawn definition record: +0x34 half-word to +0x12, +0x36 byte to
 *        +0x8E, +0x38/+0x3A/+0x3C half-words to +0x66/+0x68/+0x6A, +0x3E..+0x49 to
 *        +0x6C..+0x77, +0x4A and +0x4C jitter ranges, +0x4E rotation scale, +0x50 to
 *        +0x9A, +0x52 to +0x8F, +0x54 the scored value and +0x56 to +0x8C.
 * @return The new object, or 0 when the pool allocation failed.
 *
 * @par Confirmed
 * func_80017EBC(0x9C, 1) allocates from pool tag 1 and a null result returns 0
 * immediately. func_8003C14C(dest, src) then copies the shared header.
 * The word at +0x24 is set to the address of func_8006FF44, an entity message handler
 * whose own record is 0x9C bytes long (flag word +0x1A, parent +0x1C, matrix +0x34,
 * point +0x94), so the object allocated here is exactly that record.
 * Field copies: +0x12 = src +0x34; +0x66/+0x68/+0x6A = src +0x38/+0x3A/+0x3C;
 * +0x6C..+0x77 = the sixteen bytes at src +0x3E..+0x49; +0x8C = src +0x56;
 * +0x8E = src +0x36; +0x8F = src +0x52; +0x9A = src +0x50; +0x0A = 0 and
 * +0x08 = +0x8E + 1.
 * Rotation: x is src +0x4A when that half-word is non-negative and
 * random % -value otherwise, y is built the same way from src +0x4C, and
 * func_800188D0(x, -y, 0, &work) builds a matrix; +0x94, +0x96 and +0x98 become
 * ((work word >> 4) * src +0x4E) >> 12 for the three words at work +0x18/+0x1C/+0x20.
 * The value at +0x90 is src +0x54 when non-negative, else random % -(s16)value.
 *
 * @par Likely role
 * A placed decorative or debris object: the handler it receives only resolves the
 * parent transform and refreshes one point per frame, while the two random angles and
 * the matrix row scaled by +0x4E give it a scattered orientation at spawn. The
 * "negative means random in [0, |v|), non-negative means fixed" convention of +0x4A,
 * +0x4C and +0x54 is the same jitter idiom the rest of the spawn code uses, and
 * +0x08 = +0x8E + 1 reads like a frame or sub-object count for the handler.
 *
 * @par Unknown
 * - What +0x4E scales: the matrix row read at work +0x18 is a direction, so a spread
 *   or a turn rate are both plausible.
 * - Whether +0x08 and +0x0A are a frame index and a count, and what +0x9A selects.
 * - Why the handler address is stored in a field that other classes use as a plain
 *   flag word; the pool tag and the handler installation look like the record is
 *   handed to gSpawnQueuedEntity's queue later.
 *
 * @par Callers
 * func_8003E984, case 0x59 of its constructor switch (the generic level-object
 * spawner, which runs the distance gate and the parent bookkeeping before it calls
 * this function).
 */
void *gCreateKind59Actor(Source7043C *src)
{
    Dest7043C *dest;
    Work7043C work;
    s32 x;
    s32 y;
    s16 sval;
    u16 value;
    u16 src50;
    u8 dest8E;

    dest = gAllocMemory(0x9C, 1);
    if (dest == 0) {
        return 0;
    }

    gInitSpawnMotionState(dest, src);

    dest->unk24 = (s32)D_8006FF44;
    dest->unk12 = src->unk34;
    dest->unk66 = src->unk38;
    dest->unk68 = src->unk3A;
    dest->unk6A = src->unk3C;
    dest->unk6C = src->unk3E;
    dest->unk6D = src->unk3F;
    dest->unk6E = src->unk40;
    dest->unk6F = src->unk41;
    dest->unk70 = src->unk42;
    dest->unk71 = src->unk43;
    dest->unk72 = src->unk44;
    dest->unk73 = src->unk45;
    dest->unk74 = src->unk46;
    dest->unk75 = src->unk47;
    dest->unk76 = src->unk48;
    dest->unk77 = src->unk49;
    dest->unk8C = src->unk56;
    dest->unk8E = src->unk36;

    if (src->unk4A < 0) {
        x = (s32)((u32)func_80019668() % (u32)-src->unk4A);
    } else {
        x = src->unk4A;
    }

    if (src->unk4C < 0) {
        y = (s32)((u32)func_80019668() % (u32)-src->unk4C);
    } else {
        y = src->unk4C;
    }

    gBuildRotationMatrix(x, -y, 0, &work);

    dest->unk94 = (s16)(((work.x >> 4) * src->unk4E) >> 12);
    dest->unk96 = (s16)(((work.y >> 4) * src->unk4E) >> 12);
    dest->unk98 = (s16)(((work.z >> 4) * src->unk4E) >> 12);

    dest->unk8F = src->unk52;

    sval = (s16)src->unk54;
    value = *(volatile u16 *)&src->unk54;
    if (sval < 0) {
        value = (u16)((u32)func_80019668() % (u32)(-(s32)(s16)src->unk54));
    }
    dest->unk90 = value;

    src50 = src->unk50;
    dest8E = dest->unk8E;
    dest->unk0A = 0;
    dest->unk9A = src50;
    dest->unk08 = dest8E + 1;

    return dest;
}
