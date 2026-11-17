// Categories: Gameplay > Player | Utilities > Mathematics > Vector Length
#include "game/types.h"

struct Unk53730Info {
    u8 unk00[0xC1];
    s8 side;
};

struct Unk53730 {
    u8 unk00[0x50];
    s32 x;
    s32 y;
    s32 z;
    u8 unk5C[0x50];
    struct Unk53730Info *info;
};

struct Unk53730Global {
    u8 unk00[0x5C];
    s32 unk5C;
};

struct Unk53730Delta {
    s32 x;
    s32 z;
};

extern s32 gPlayerCount[] __asm__("D_8009FA0C");
extern struct Unk53730Global *gGameSession[] __asm__("D_8009FA94");
extern struct Unk53730 *D_8009FA58[];
extern s32 D_8009FA64;

extern s32 func_8001A074(struct Unk53730Delta *delta);

/**
 * @brief Computes and stores the 2D XZ separation distance between the two players in non-split 2P mode.
 *
 * When two players are active (D_8009FA0C[0] == 2) and split-screen flag D_8009FA94[0]->unk5C is 0, reads the opposing player from D_8009FA58[1 - object->info->side], forms the XZ delta, and writes the approximate length to D_8009FA64. Category Gameplay > Player | Utilities > Mathematics > Vector Length reflects the player-pair lookup and 2D magnitude call.
 *
 * @param object Player object (+0x50/+0x58 XZ position, +0xAC info struct with s8 side at +0xC1).
 *
 * @par Globals
 * D_8009FA0C - active player count; D_8009FA94 - mode state (+0x5C split flag); D_8009FA58 - 2-slot player array; D_8009FA64 - stored distance.
 *
 * @par Control flow
 * Single guarded conditional (D_8009FA0C[0] == 2 && D_8009FA94[0]->unk5C == 0) around delta subtraction and distance assignment.
 *
 * @par Calls
 * func_8001A074 - computes the 2D approximate Euclidean length of the XZ delta vector.
 *
 * @par Side effects
 * Writes D_8009FA64 in shared-screen 2-player mode.
 *
 * @par Callers
 * Confirmed: func_800537D0 (0x800538B0), func_80054590 (0x800546E8) / Likely: none / Unknown: none.
 */
void func_80053730(struct Unk53730 *object)
{
    struct Unk53730 *other;
    struct Unk53730Delta delta;

    if (gPlayerCount[0] == 2 && gGameSession[0]->unk5C == 0) {
        other = D_8009FA58[1 - object->info->side];
        delta.x = other->x - object->x;
        delta.z = other->z - object->z;
        D_8009FA64 = func_8001A074(&delta);
    }
}
