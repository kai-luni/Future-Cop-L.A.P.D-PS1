// Categories: Gameplay > AI > Attack Scheduling | Gameplay > Entity Update | Graphics > Animation
#include "game/types.h"

struct Unk33608Anim {
    u8 unk00[8];
    s32 value08;
};

struct Unk33608Mirror {
    u8 unk00[0x1B];
    u8 base1B;
    u8 unk1C[0x3A];
    u8 mode56;
    u8 unk57;
    u16 value58;
    u8 unk5A[0x0C];
    u16 value66;
    struct Unk33608Anim *anim68;
    s32 value6C;
    u8 unk70[0x0E];
    u8 mode7E;
    u8 unk7F;
    u16 value80;
    u8 unk82[0x0C];
    u16 value8E;
    struct Unk33608Anim *anim90;
    s32 value94;
};

struct Unk33608Owner {
    u8 unk00[0xA6];
    u8 modeA6;
    u8 unkA7;
    u16 valueA8;
    u8 unkAA[0x0A];
    u16 valueB4;
    u16 valueB6;
    struct Unk33608Anim *animB8;
    s32 valueBC;
    u8 unkC0[0x74];
    struct Unk33608Mirror *mirror;
};
void gSetDroneAttackTarget(struct Unk33608Owner *owner, struct Unk33608Anim *anim) __asm__("func_80033608");


/**
 * @brief Sets an entity's active animation/target block or parks its mode at 4, mirroring to its twin block.
 *
 * When anim is non-null and differs from owner->animB8, writes anim and anim->value08 to owner and both mirror banks of owner->mirror; when anim is null, resets valueB6 to valueB4 and, if modeA6 != 4, switches mode to 4 and restores valueA8 from mirror->base1B.
 *
 * @param owner Entity block (+0xA6 modeA6, +0xA8 valueA8, +0xB4 valueB4, +0xB6 valueB6, +0xB8 animB8, +0xBC valueBC, +0x134 mirror).
 * @param anim Target/animation descriptor (+0x08 value08), or 0 to park/reset the slot.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Branches on anim != 0: updates animB8 and valueBC on owner and both mirror banks (+0x68/+0x6C and +0x90/+0x94) if owner->animB8 != anim. Else copies valueB4 to valueB6/value66/value8E, and if modeA6 != 4 writes mode 4 to modeA6/mode56/mode7E and mirror->base1B to valueA8/value58/value80.
 *
 * @par Calls
 * None.
 *
 * @par Side effects
 * Mutates owner fields +0xA6..+0xBC and mirrored fields +0x56..+0x94 in owner->mirror.
 *
 * @par Callers
 * Confirmed: func_800336DC (10 sites: 0x80033ED8..0x800347DC), func_8003532C (0x80035478), and func_80035698 (0x80035714).
 */
void gSetDroneAttackTarget(struct Unk33608Owner *owner, struct Unk33608Anim *anim)
{
    register struct Unk33608Mirror *firstMirror __asm__("$2");
    register struct Unk33608Mirror *secondMirror __asm__("$3");
    register s32 value __asm__("$2");
    register u16 half __asm__("$3");
    register u16 lastHalf __asm__("$2");

    if (anim != 0) {
        if (owner->animB8 != anim) {
            firstMirror = owner->mirror;
            firstMirror->anim90 = anim;
            firstMirror->anim68 = anim;
            secondMirror = owner->mirror;
            owner->animB8 = anim;
            value = anim->value08;
            secondMirror->value94 = value;
            secondMirror->value6C = value;
            owner->valueBC = value;
        }
    } else {
        firstMirror = owner->mirror;
        half = owner->valueB4;
        firstMirror->value8E = half;
        firstMirror->value66 = half;
        owner->valueB6 = half;
        if (owner->modeA6 != 4) {
            firstMirror = owner->mirror;
            firstMirror->mode7E = 4;
            firstMirror->mode56 = 4;
            owner->modeA6 = 4;
            secondMirror = owner->mirror;
            lastHalf = secondMirror->base1B;
            secondMirror->value80 = lastHalf;
            secondMirror->value58 = lastHalf;
            owner->valueA8 = lastHalf;
        }
    }
}
