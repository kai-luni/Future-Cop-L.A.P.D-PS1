// Categories: Gameplay > Object Spawning | Graphics > Effects | Physics > Motion
#include "game/types.h"

typedef struct {
    u8 unk00[0x34];
    u16 unk34;
    u16 unk36;
    u16 unk38;
    u16 unk3A;
    u16 unk3C;
    u16 unk3E;
    s16 unk40;
    u8 unk42;
    u8 unk43;
    u8 unk44;
    u8 unk45;
    u8 unk46;
    u8 unk47;
    u16 unk48;
    u8 unk4A;
    u8 pad4B;
    u16 unk4C;
    u16 unk4E;
    u16 unk50;
    u16 unk52;
} Source6EB80;

typedef struct {
    u8 unk00[8];
    u16 unk08;
    u16 unk0A;
    u8 pad0C[6];
    u16 unk12;
    u8 pad14[0x10];
    void *unk24;
    u8 pad28[0x3C];
    u16 unk64;
    u8 pad66[2];
    u16 unk68;
    u16 unk6A;
    u16 unk6C;
    u16 unk6E;
    u16 unk70;
    u16 unk72;
    u16 unk74;
    u16 unk76;
    u16 unk78;
    u16 unk7A;
    u16 unk7C;
    u16 unk7E;
    u16 unk80;
    u16 unk82;
    u16 unk84;
    u16 unk86;
    u8 pad88;
    u8 unk89;
    u8 unk8A;
    u8 unk8B;
    u8 unk8C;
    u8 unk8D;
    u8 unk8E;
    u8 unk8F;
    u16 unk90;
    u16 unk92;
    u16 unk94;
    u16 unk96;
    u16 unk98;
} Dest6EB80;

extern u8 gDomeMeshEffectHandler[] __asm__("D_8006E814");
extern void *gAllocMemory(s32, s32) __asm__("func_80017EBC");
extern void gInitSpawnMotionState(Dest6EB80 *, Source6EB80 *) __asm__("func_8003C14C");
void *gSpawnDomeMeshEffect(Source6EB80 *src) __asm__("func_8006EB80");


/**
 * @brief Allocates and initializes a 0xA0-byte kind-0x5C effect/mesh object from a source spawn template.
 *
 * Confirmed: allocates 0xA0 bytes (tag 1) via func_80017EBC, snaps src->unk40 down to a multiple of 8 clamped to >= 8, initializes dest fields (+0x64..+0x98), calls func_8003C14C(dest, src), sets dest->unk24 = D_8006E814 (func_8006E814), and scales unk64 by flags (+0x98 bits 2 and 4) into unk08 or unk0A (selected by bit 0).
 * Category follows caller func_8003E984 (spawn switch case 0x5C), motion init func_8003C14C, and pooled-mesh handler func_8006E814.
 * Likely: constructor for kind-0x5C deformable-mesh effect entities; Unknown: exact semantic meaning of +0x7A/+0x7E (0xE2), +0x80 (0x20), +0x82 (0x19), and +0x08/+0x0A.
 *
 * @param src Pointer to Source6EB80 spawn descriptor (+0x34..+0x52 read; +0x40 may be quantized and clamped in place).
 * @return Pointer to initialized 0xA0-byte Dest6EB80 object, or null (0) if func_80017EBC fails.
 *
 * @par Globals
 * None (uses address of func_8006E814, declared as D_8006E814).
 * @par Control flow
 * Early return on null allocation; src->unk40 alignment/clamp checks; linear field setup and func_8003C14C call; three bitmask branches on unk98 (&4, &0x10, &1) setting unk08/unk0A.
 * @par Calls
 * func_80017EBC - allocates 0xA0 bytes from heap tag 1; func_8003C14C - initializes motion/physics fields on dest from src.
 * @par Side effects
 * Allocates heap memory, may mutate src->unk40 in place, and binds func_8006E814 as dest->unk24 handler.
 * @par Callers
 * Confirmed: func_8003E984 (0x8003EB54 in asm/us/main.s, switch case 0x5C on source->kind) / Likely: none / Unknown: none.
 */
void *gSpawnDomeMeshEffect(Source6EB80 *src)
{
    Dest6EB80 *dest;
    u16 initialSize;
    u16 value;
    u16 flags;
    register s32 initialTwenty __asm__("$6");
    register s32 initialFive __asm__("$3");

    dest = gAllocMemory(0xA0, 1);
    if (dest == 0) {
        return 0;
    }

    if (src->unk40 & 7) {
        src->unk40 = src->unk40 / 8 * 8;
    }
    if (src->unk40 < 8) {
        src->unk40 = 8;
    }

    dest->unk6A = src->unk36;
    dest->unk68 = src->unk34;
    initialTwenty = 0x14;
    initialSize = (u16)src->unk40;
    initialFive = 5;
    dest->unk72 = 0;
    dest->unk76 = initialTwenty;
    dest->unk74 = initialFive;
    dest->unk6C = 0;
    dest->unk64 = initialSize;
    dest->unk70 = src->unk3A;
    dest->unk6E = src->unk38;
    dest->unk98 = src->unk52;
    dest->unk89 = src->unk42;
    dest->unk8A = src->unk43;
    dest->unk8B = src->unk44;
    dest->unk8D = src->unk45;
    dest->unk8E = src->unk46;
    dest->unk8F = src->unk47;
    dest->unk86 = src->unk48;
    dest->unk8C = src->unk4A;
    dest->unk72 = 0;
    dest->unk78 = 0;
    dest->unk7A = 0xE2;
    dest->unk7E = 0xE2;
    dest->unk80 = 0x20;
    dest->unk7C = 0;
    dest->unk82 = 0x19;
    dest->unk84 = initialFive;

    gInitSpawnMotionState(dest, src);

    dest->unk24 = gDomeMeshEffectHandler;
    dest->unk90 = src->unk3C;
    dest->unk92 = src->unk3E;
    dest->unk94 = src->unk4E;
    dest->unk96 = src->unk50;
    dest->unk12 = src->unk4C;

    value = dest->unk64;
    flags = dest->unk98;
    if (flags & 4) {
        value <<= 1;
    }
    if (flags & 0x10) {
        value <<= 1;
    }
    if (flags & 1) {
        dest->unk08 = 0;
        dest->unk0A = value;
    } else {
        dest->unk08 = value;
        dest->unk0A = 0;
    }

    return dest;
}
