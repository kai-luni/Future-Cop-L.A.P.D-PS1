// Categories: Resources > Table Remapping
#include "game/types.h"

struct Unk1306C {
    u32 unk0;
    u32 unk4;
    s32 unk8[1];
};

extern s32 *gVertexBlockTable __asm__("D_8009F554");
extern s32 *gNormalBlockTable __asm__("D_8009F558");
extern s32 *gRelocBlockTable __asm__("D_8009F55C");

s32 gResolveModelBlockIndices(struct Unk1306C *arg0) __asm__("func_8001306C");

/**
 * @brief Replaces tagged model block indices with pointers from the
 * corresponding table.
 *
 * @param arg0 Tagged block containing a count at +4 and indices starting at
 *     +8.
 * @return Always zero.
 *
 * @par Confirmed mechanics
 * Compares the first word against 0x3444564C, 0x34444E4C, and 0x3344524C. For
 * a match, it iterates the count at +4 and replaces each word from +8 onward
 * with a table load at index << 2.
 *
 * @par Globals
 * Reads gVertexBlockTable, gNormalBlockTable, or gRelocBlockTable.
 *
 * @par Callers
 * Direct calls in the retail listing are from gRelocateModelResource.
 *
 * @par Role and path
 * The model loader relocates reference blocks before publishing the model
 * header.
 *
 * @par Unknown
 * Bounds and index validity are established by the input format.
 */
s32 gResolveModelBlockIndices(struct Unk1306C *arg0)
{
    s32 *tbl;
    u32 i;

    switch (arg0->unk0) {
    case 0x3444564C:
        if (arg0->unk4 != 0) {
            tbl = gVertexBlockTable;
            i = 0;
            do {
                arg0->unk8[i] = tbl[arg0->unk8[i]];
                i++;
            } while (i < arg0->unk4);
        }
        break;
    case 0x34444E4C:
        if (arg0->unk4 != 0) {
            tbl = gNormalBlockTable;
            i = 0;
            do {
                arg0->unk8[i] = tbl[arg0->unk8[i]];
                i++;
            } while (i < arg0->unk4);
        }
        break;
    case 0x3344524C:
        if (arg0->unk4 != 0) {
            tbl = gRelocBlockTable;
            i = 0;
            do {
                arg0->unk8[i] = tbl[arg0->unk8[i]];
                i++;
            } while (i < arg0->unk4);
        }
        break;
    }
    {
        register s32 result __asm__("$2") = 0;
        __asm__ volatile ("" : "+r" (result));
        return result;
    }
}
