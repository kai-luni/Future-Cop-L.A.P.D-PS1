// Categories: Gameplay > AI > Attack Scheduling | Game Engine > Object Setup
#include "game/types.h"

struct Unk5FF70 {
    u8  key;
    u8  pad01[1];
    u16 f02;
    u16 f04;
    u16 f06;
    u16 f08;
    u16 f0A;
    u8  pad0C[0x24];
};

struct Unk60668In {
    s16 f00;
    u16 f02;
    u8  f04;
    u8  f05;
    u16 f06;
    u16 f08;
    u16 f0A;
    u16 f0C;
    s16 f0E;
};

struct Unk60668Out {
    struct Unk5FF70 *f00;
    u16 f04;
    u8  f06;
    u8  f07;
    u16 f08;
    u16 f0A;
    s32 f0C;
    s32 f10;
    s16 f14;
    s16 f16;
    s32 f18;
    s32 f1C;
    u16 f20;
    u8  f22;
    u8  f23;
    u16 f24;
    u16 f26;
};

extern s32 func_8005FF70(s32 key);
extern s32 func_8006062C(u32 id);
extern struct Unk5FF70 gEffectKeyTable[] __asm__("D_800A2300");

/**
 * @brief Initializes a 0x28-byte attack/weapon scheduler slot from a 16-byte config descriptor.
 *
 * Looks up the config key in the global definition table D_800A2300 via func_8005FF70, copies tuning bytes, precomputes half-angle cosines at +0x0C/+0x10 and scaled rate at +0x14, clears runtime fields, and seeds counters from the table entry.
 *
 * @param in 16-byte input descriptor (+0x00 key, +0x02..+0x06 config fields, +0x08/+0x0A angles, +0x0C field, +0x0E rate).
 * @param out Destination 0x28-byte slot (e.g. entity +0xA0 for func_80060238).
 *
 * @par Globals
 * D_800A2300 - array of 0x30-byte definition entries indexed by the result of func_8005FF70.
 *
 * @par Control flow
 * Branches on in->f00 < 0: clears out->f00 to 0 and returns; otherwise resolves &D_800A2300[func_8005FF70(in->f00)], copies in->f02/f05/f04/f06/f0C to +0x04/+0x06/+0x07/+0x08/+0x0A, sets +0x14 = (s16)((in->f0E << 12) / 218), zeroes +0x16..+0x23, and copies entry f02/f0A/f06 to +0x20/+0x24/+0x26.
 *
 * @par Calls
 * func_8005FF70 - linear search in D_800A2300 for matching byte key in->f00.
 * func_8006062C - computes cosine via func_8001846C for half-angles ((s16)in->f08 / 2) and ((s16)in->f0A / 2) into +0x0C and +0x10.
 *
 * @par Side effects
 * Writes out->f00 alone when in->f00 < 0, or populates all 0x28 bytes of out.
 *
 * @par Callers
 * Confirmed: func_8002C584 (0x8002CA48), func_8002DFC4 (0x8002E278), func_80032180 (0x800321A4), func_800331CC (0x800332B0, 0x800332C0), func_800378BC (0x800378E4), and func_800397AC (0x80039838).
 */
void func_80060668(struct Unk60668In *in, struct Unk60668Out *out)
{
    s32 index;
    s32 scale;
    u16 v20;

    if (in->f00 < 0) {
        out->f00 = 0;
    } else {
        index = func_8005FF70(in->f00);
        out->f00 = &gEffectKeyTable[index];
        out->f04 = in->f02;
        out->f06 = in->f05;
        out->f08 = in->f06;
        out->f07 = in->f04;
        out->f0C = func_8006062C((s16)in->f08 / 2);
        out->f10 = func_8006062C((s16)in->f0A / 2);
        out->f0A = in->f0C;
        scale = in->f0E << 12;
        out->f14 = (s16)(scale / 218);
        out->f16 = 0;
        v20 = out->f00->f02;
        out->f18 = 0;
        out->f1C = 0;
        out->f22 = 0;
        out->f23 = 0;
        out->f20 = v20;
        out->f24 = out->f00->f0A;
        out->f26 = out->f00->f06;

    }
}
