// Categories: Gameplay > Entity Attach | Gameplay > Entity Update
#include "game/types.h"

struct Entity {
    u8 unk00[0x08];
    s32 unk08;
    u8 unk0C[0x08];
    u16 unk14;
    u8 unk16[0x0E];
    u32 flags;
    u8 unk28[0x50];
    s32 unk78;
    s32 unk7C;
    s32 unk80;
    s32 unk84;
    u8 unk88[0x0C];
    s32 unk94;
    u8 unk98[0x08];
    u8 flagsA0;
    u8 unkA1[0x03];
    s32 unkA4;
    u8 unkA8[0x1C];
    u8 *unkC4;
    u8 *unkC8;
    s32 unkCC;
    u8 unkD0[0x02];
    s16 unkD2;
    s16 unkD4;
    u8 unkD6[0x02];
    u16 unkD8;
    u8 unkDA[0x06];
    s32 unkE0;
};

extern s32 gFrameTime __asm__("D_8009FAA4");
extern void func_8004D958(struct Entity *entity);
extern s32 func_80069D68(s32 script, s32 arg1, s32 arg2, struct Entity *entity);

/**
 * @brief Applies the interaction flags of one entity to another.
 *
 * The self stamp D_8009FAA4 is stored in arg0->unkA4. With flag 8 the two
 * entities are linked: the target's previous 0xC8 is kept in 0xC4 and its
 * stored id in 0xD8, the target's 0xC8 is pointed at arg0, 0xCC receives
 * arg0->unk08, the 0xE0 word becomes 1, 0xD4/0xD2 are set to 0xDA/0x100 and
 * the three velocity words at 0x78..0x80 are halved. Flag 1 marks the target
 * with 0x04000000 and retires it. Flag 2 runs the script attached at
 * arg0->unk94 with the target type; without flag 2 arg0 itself is retired.
 *
 * @param arg0 Source entity with the flag byte at 0xA0 and the script at 0x94.
 * @param arg1 Target entity that receives the link and the flags.
 * @return Always 0.
 *
 * @par Globals
 * D_8009FAA4 - per-frame stamp copied into the source entity.
 *
 * @par Control flow
 * Three independent flag tests, each with its own side effect.
 *
 * @par Calls
 * func_80069D68 - runs the attached script with the target as context.
 * func_8004D958 - retires either entity.
 *
 * @par Side effects
 * Rewires pointers and flags of both entities.
 *
 * @par Callers
 * Confirmed: func_8003151C.
 */
s32 func_80031120(struct Entity *arg0, struct Entity *arg1)
{
    u8 *oldC8;
    u8 flags;

    flags = arg0->flagsA0;
    arg0->unkA4 = gFrameTime;

    if (flags & 8) {
        oldC8 = arg1->unkC8;
        arg1->unkC4 = oldC8;
        arg1->unkD8 = *(u16 *)(oldC8 + 8);
        arg1->unkC8 = (u8 *)arg0;
        arg1->unkCC = arg0->unk08;
        arg1->unkE0 = 1;
        arg1->unkD4 = 0xDA;
        arg1->unkD2 = 0x100;
        arg1->unk78 >>= 1;
        arg1->unk7C >>= 1;
        arg1->unk80 >>= 1;
        arg1->unk84 = 0;
    }

    if (arg0->flagsA0 & 1) {
        arg1->flags |= 0x4000000;
        func_8004D958(arg1);
    }

    if (arg0->flagsA0 & 2) {
        if (arg0->unk94 != 0) {
            func_80069D68(arg0->unk94, 0, arg1->unk14 & 0x7FFF, arg0);
            return 0;
        }
    } else {
        func_8004D958(arg0);
    }

    return 0;
}
