// Categories: Gameplay > Entity Query | Input > Condition Test
#include "game/types.h"

struct Inner {
    u8 pad00[0x40];
    s32 unk40;
};

struct Entity {
    u8 pad00[0x14];
    u16 unk14;
    u8 unk16;
    u8 unk17;
    u8 unk18;
    u8 pad19[0xAC - 0x19];
    struct Inner *unkAC;
    u8 padB0[0xC8 - 0xB0];
    struct Entity *unkC8;
};

extern struct Entity *D_8009FA58[2];
extern struct Entity *D_8009FA60[2];

/**
 * @brief Compares one entity field with a value and optionally checks the pair table.
 *
 * The selector arg0 chooses the field: 8 maps type 0x12 to 1 and returns the
 * type otherwise, 1 returns the type, 2 returns 0x14 but rejects type 0x15,
 * 3 and 4 return 0x17 and 0x18, 5 also masks arg1 with 1 << value, and 7 is a
 * pure predicate that only succeeds for a type-2 entity whose partner is of
 * type 0x12. With arg3 set the match is filtered: for each of the two entries
 * of D_8009FA58 a type-1 entity must be the same pointer, and any entry whose
 * partner block has a non-zero 0x40 makes the test fail.
 *
 * @param arg0 Field selector 1..8.
 * @param arg1 Value the selected field is compared with.
 * @param arg2 Entity the fields are read from.
 * @param arg3 Non-zero enables the two-object filter.
 * @return 1 when the test passes, 0 otherwise.
 *
 * @par Globals
 * D_8009FA58 - the two reference entities; D_8009FA60 - end of that array.
 *
 * @par Control flow
 * Switch on the selector, then one compare and an optional two-iteration loop.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * None observed.
 *
 * @par Callers
 * Confirmed: func_8003015C, func_80030960. Likely: func_80030D3C, func_8003151C.
 */
s32 func_8002F8B8(u8 arg0, s16 arg1, struct Entity *arg2, u8 arg3)
{
    s32 val;
    register struct Entity **p __asm__("$3");
    register struct Entity **end __asm__("$5");

    switch (arg0) {
    case 8:
        val = arg2->unk16;
        if (val == 0x12) {
            val = 1;
        }
        break;

    case 1:
        val = arg2->unk16;
        break;

    case 2:
        val = arg2->unk14;
        if (arg2->unk16 == 0x15) {
            return 0;
        }
        break;

    case 3:
        val = arg2->unk17;
        break;

    case 4:
        val = arg2->unk18;
        break;

    case 5:
        val = arg2->unk18;
        arg1 &= (1 << val);
        break;

    case 7:
        if (arg2->unk16 == 2) {
            arg2 = arg2->unkC8;
            if (arg2 != 0 && arg2->unk16 == 0x12) {
                return 1;
            }
        }
        return 0;

    case 6:
    default:
        break;
    }

    if (val == (s16)arg1) {
        if (arg3 != 0) {
            register s32 one __asm__("$7") = 1;
            register u8 unk16 __asm__("$4") = arg2->unk16;
            p = D_8009FA58;
            end = D_8009FA60;
            do {
                if (unk16 == one) {
                    if (arg2 != *p) {
                        continue;
                    }
                }
                if (*p != 0 && (*p)->unkAC->unk40 != 0) {
                    return 0;
                }
            } while ((s32)++p < (s32)end);
        }
        return 1;
    }

    return 0;
}
