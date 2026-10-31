#include "game/types.h"

struct Unk780DC {
    u32 unk0;
    s32 unk4;
};

extern struct Unk780DC D_800A3588[2];
extern struct Unk780DC D_800A3598[2];
extern struct Unk780DC D_800A35A8[2];
extern struct Unk780DC D_800A35B8[2];
extern struct Unk780DC D_800A35C8[2];
extern struct Unk780DC D_800A35D8[2];
extern struct Unk780DC D_800A35E8[2];

extern u8 D_8009FAC4;
extern u8 D_8009F3F0;
extern s32 D_8009F3EC;
extern s32 D_8009F3F4;
extern s32 D_8009F8B0;
extern s32 D_8009F8B4;
extern s32 D_8009FA84;

extern void func_80077FFC(void);
extern void *func_80017EBC(s32 size, s32 tag);
extern void *func_8001E4C0(void *destination, int value, unsigned int count);
extern void func_800898C8(void);
extern void func_8007F9CC(void);
extern void func_80077AD4(s32 arg);

/**
 * Initialises the object registry and the per-slot flag tables of this subsystem.
 *
 * The registry is the 0x78 byte block (30 pointers) allocated from the tag-2 pool,
 * zeroed and published through D_8009F3F4. Its lifetime is fixed by two verified
 * neighbours: func_8008263C files an object into slot `obj->unk1C` (valid 1..0x1D,
 * i.e. 29 usable slots) and func_8007852C frees every occupied slot plus the array
 * on teardown. The block is therefore a 30-entry registry keyed by a type id.
 *
 * The seven 2-entry tables below each receive state 2 in unk4 and one distinct bit
 * in unk0 (bits 19..23, 28 and 30). They are written only here and in the adjacent
 * func_800780C0; the readers are not decompiled yet, so the meaning of the bits is
 * still open - a per-channel/slot flag set is the plausible reading, not a proof.
 *
 * Called as the final step of func_80024904, the per-session setup that also sets
 * the view window, seeds the RNG and programs the display mode.
 */
void func_800780DC(void)
{
    void *p;
    u32 i;

    D_8009FAC4 = 0;
    D_8009F3F0 = 1;
    D_8009F3EC = 0;
    func_80077FFC();
    p = func_80017EBC(0x78, 2);
    D_8009F3F4 = (s32)p;
    func_8001E4C0(p, 0, 0x78);
    func_800898C8();
    for (i = 0; i < 2; i++) {
        D_800A35D8[i].unk4 = 2;
        D_800A35C8[i].unk4 = 2;
        D_800A35B8[i].unk4 = 2;
        D_800A35A8[i].unk4 = 2;
        D_800A3598[i].unk4 = 2;
        D_800A3588[i].unk4 = 2;
        D_800A35E8[i].unk0 = 0x00080000;
        D_800A35E8[i].unk4 = 2;
        D_800A3588[i].unk0 = 0x00100000;
        D_800A3598[i].unk0 = 0x00400000;
        D_800A35A8[i].unk0 = 0x00800000;
        D_800A35B8[i].unk0 = 0x00200000;
        D_800A35C8[i].unk0 = 0x40000000;
        D_800A35D8[i].unk0 = 0x10000000;
    }
    func_8007F9CC();
    func_80077AD4(1);
    D_8009F8B0 = 0;
    D_8009F8B4 = 0;
    D_8009FA84 = 0;
}
