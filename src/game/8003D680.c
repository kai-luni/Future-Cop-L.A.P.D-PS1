// Categories: Gameplay > Object Spawning | Game Engine > Object Setup | Graphics > Scene Graph
#include "game/types.h"

struct Unk3D680;
struct Unk3D680Source;
struct Unk3D680Parent;
struct Unk3D680Entry;

typedef u8 (*Unk3D680SourceFunc)(s32, struct Unk3D680Source *, s32, s32);

struct Unk3D680Source {
    u8 unk00[4];
    Unk3D680SourceFunc unk04;
    s32 unk08;
    u8 unk0C[0x1C];
    u8 unk28[0x6A];
    u8 unk92;
    u8 unk93;
};

struct Unk3D680Entry {
    u8 unk00[0x32];
    u16 unk32;
};

struct Unk3D680Parent {
    s32 unk00;
    u8 unk04[0x0C];
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    s32 unk34;
    s32 unk38;
    s32 unk3C;
    u8 unk40[4];
    u8 *unk44;
    u8 unk48;
    u8 unk49;
};

struct Unk3D680 {
    struct Unk3D680 *next;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    s32 unk14;
    s32 unk18;
    s32 unk1C;
    s32 unk20;
    s32 unk24;
    s32 unk28;
    s32 unk2C;
    s32 unk30;
    u8 *unk34;
    u8 unk38;
    u8 unk39;
    u16 unk3A;
    u16 unk3C;
    u16 unk3E;
    u8 unk40;
    u8 unk41;
    u8 unk42;
    u8 unk43;
    s32 unk44;
    s32 unk48;
};

extern s32 gBaseMatrix[9] __asm__("D_8009AA38");
extern struct Unk3D680 *D_8009F320;
extern void *D_800A7040[][0x100];

extern void *func_80017EBC(s32 size, s32 tag);
extern u32 *func_8003B3D8(u32 column, u32 row);

/**
 * @brief Allocates and initialises one 0x4C-byte map-cell object: transform,
 *        scale, cell flags, then links it at the head of the global object list.
 *
 * @param source Optional source descriptor. When non-null the object stores it
 *        at +0x04, its +0x08 value at +0x08 (or -1 when that value is zero),
 *        points +0x34 at source +0x28, copies source +0x92/+0x93 to +0x40/+0x41
 *        and asks source->unk04(0x10, source, 0, 0) for the +0x42 byte.
 * @param type Cell column; only the low byte reaches func_8003B3D8.
 * @param row Cell row, also the first index of the D_800A7040 descriptor table.
 * @param parent Optional owner record: +0x49 flag byte (0x08 takes the 3x3 basis
 *        from +0x1C..+0x3C, 0x04 the position from +0x10..+0x18, 0x40 the
 *        transform block from +0x44, 0x01 the scale base from +0x00), plus the
 *        +0x00 word.
 * @param arg4 Stored verbatim in the object's +0x48.
 * @param arg5 Cell-override flag: any non-zero value forces +0x39 = 4.
 *
 * @return The new object, or 0 when func_80017EBC(0x4C, 1) failed.
 *
 * @par Confirmed
 * Allocation is func_80017EBC(0x4C, 1); +0x34 and +0x44 start at 0 and +0x48
 * receives arg4. In the source path +0x38 stays 0 and the transform comes from
 * source +0x28. In the source-less path +0x42 = 1 and +0x40/+0x41 = 0, the nine
 * words +0x04..+0x24 come either from parent +0x1C..+0x3C or from the identity
 * basis D_8009AA38 (0x10000 on the diagonal, confirmed in the image), and
 * +0x38 = 1. A parent flag 0x40 replaces the +0x34 block with parent +0x44; a
 * source-less object plus parent flag 0x04 copies parent +0x10/+0x14/+0x18 into
 * +0x28/+0x2C/+0x30.
 * Scale: with no +0x34 block, +0x3C = parent->unk00 >> 4 when parent flag 0x01
 * is set and that word is positive, else 0x100; with a block, +0x3C = the word
 * at block +0x40 >> 4.
 * Cell: result = func_8003B3D8(column, row) (the attribute word of a 256-column
 * grid with 0x400-byte rows) and entry = D_800A7040[row][column] (the cell
 * descriptor). +0x39 becomes 4 only when both exist and one of arg5, entry
 * flag 0x40 or attribute bit 0x04 is set; otherwise 0. +0x3A and +0x3E are
 * cleared and the object is pushed onto D_8009F320.
 *
 * @par Likely role
 * The single constructor of the map/scene cell objects: func_800402DC calls it
 * from the spawn gate with D_8009F31C as arg5, func_8003FE0C from the deferred
 * spawn path with arg5 = 1 and a linked owner as parent, and func_8004B5F8 uses
 * a bare construction as a placeholder holder. func_8003D240 then walks the
 * list it links into, which confirms the +0x38/+0x3A bookkeeping and the
 * +0x34 shadow block belong to the frame dispatcher.
 *
 * @par Unknown
 * The +0x04..+0x24 area is written as a 3x3 basis in the source-less path but
 * holds a descriptor pointer and one extra word in the source path, so the
 * field is really a union and the meaning of the inline words is unproven.
 * Also open: what +0x39 = 4 selects, what the +0x32 flag 0x40 of a cell
 * descriptor marks, and what the cell attribute bit 0x04 means.
 *
 * @par Callers
 * func_8003FE0C, func_800402DC, func_8004B5F8.
 */
struct Unk3D680 *func_8003D680(struct Unk3D680Source *source, u8 type, s32 row,
                               struct Unk3D680Parent *parent, s32 arg4, u8 arg5)
{
    struct Unk3D680 *object;
    struct Unk3D680Entry *entry;
    u32 *result;
    u8 column;
    u8 value;
    s32 parentX;
    s32 parentY;
    s32 parentZ;

    object = func_80017EBC(0x4C, 1);
    if (object == 0) {
        return 0;
    }

    object->unk34 = 0;
    object->unk44 = 0;
    object->unk48 = arg4;

    if (source != 0) {
        object->unk04 = (s32)source;
        object->unk08 = source->unk08;
        if (source->unk08 == 0) {
            object->unk08 = -1;
        }
        object->unk38 = 0;
        object->unk34 = (u8 *)(object->unk04 + 0x28);
        object->unk40 = source->unk92;
        object->unk41 = source->unk93;
        object->unk42 = source->unk04(0x10, source, 0, 0);
    } else {
        object->unk42 = 1;
        object->unk41 = 0;
        object->unk40 = 0;

        if (parent != 0 && (parent->unk49 & 8)) {
            object->unk04 = parent->unk1C;
            object->unk08 = parent->unk20;
            object->unk0C = parent->unk24;
            object->unk10 = parent->unk28;
            object->unk14 = parent->unk2C;
            object->unk18 = parent->unk30;
            object->unk1C = parent->unk34;
            object->unk20 = parent->unk38;
            object->unk24 = parent->unk3C;
        } else {
            object->unk04 = gBaseMatrix[0];
            object->unk08 = gBaseMatrix[1];
            object->unk0C = gBaseMatrix[2];
            object->unk10 = gBaseMatrix[3];
            object->unk14 = gBaseMatrix[4];
            object->unk18 = gBaseMatrix[5];
            object->unk1C = gBaseMatrix[6];
            object->unk20 = gBaseMatrix[7];
            object->unk24 = gBaseMatrix[8];
        }
        object->unk38 = 1;
    }

    if (parent != 0) {
        if (parent->unk49 & 0x40) {
            object->unk34 = parent->unk44;
        }
        if (object->unk38 != 0 && (parent->unk49 & 4)) {
            parentX = parent->unk10;
            parentY = parent->unk14;
            parentZ = parent->unk18;
            object->unk28 = parentX;
            object->unk2C = parentY;
            object->unk30 = parentZ;
        }
    }

    if (object->unk34 == 0) {
        if (parent != 0 && (parent->unk49 & 1) && parent->unk00 > 0) {
            object->unk3C = parent->unk00 >> 4;
        } else {
            object->unk3C = 0x100;
        }
    } else {
        object->unk3C = *(s32 *)(object->unk34 + 0x40) >> 4;
    }

    column = type & 0xFF;
    result = func_8003B3D8(column, row);
    entry = (struct Unk3D680Entry *)D_800A7040[row][column];

    if (entry != 0 && result != 0 &&
        ((s32)arg5 != 0 || (entry->unk32 & 0x40) || (*(u8 *)result & 4))) {
        value = 4;
    } else {
        value = 0;
    }
    object->unk39 = value;

    object->unk3A = 0;
    object->unk3E = 0;
    object->next = D_8009F320;
    D_8009F320 = object;
    return object;
}
