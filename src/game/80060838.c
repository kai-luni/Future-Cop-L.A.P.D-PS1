// Categories: Resources > Asset Loader | Game Engine > Object Setup
#include "game/types.h"

struct Resource;

struct Unk607B8 {
    u8 unk00[6];
    s16 unk06;
    s16 unk08;
    u8 unk0A[2];
    s16 unk0C;
};

struct Unk60838Arg {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};

struct Unk60838Entry {
    u8 unk00[8];
    s16 unk08;
    s16 unk0A;
    struct Unk607B8 sub;
    u8 unk1A[6];
    void *unk20[4];
};

extern s32 D_8009F7F8;
extern struct Unk60838Entry D_800A2300[];

extern void *func_80019C38(u32 a0, u32 a1, u32 a2, u32 a3);
extern void *func_80019CA0(u32 a0, u32 a1, u32 a2, u32 a3);
extern struct Resource *func_80019BF0(s32 group, s32 name);
extern void func_800607B8(struct Unk607B8 *value);
extern void func_800181D4(struct Unk60838Arg *arg0);
extern void *memcpy(void *dest, const void *src, u32 count);

/**
 * @brief Loads and normalizes a 0x30-byte definition entry into D_800A2300.
 *
 * Allocates D_800A2300[D_8009F7F8++], copies 0x20 bytes from the 0x74414354 ("tACT") payload (+0x10), scales subfields and unk08/unk0A by (x << 12) / 218, resolves up to four 0x6152534C ("aRSL") linked resources into unk20[0..3], and frees arg0.
 *
 * @param arg0 Temporary resource descriptor (+0x00 blob, +0x1C section id, +0x20 byte length).
 *
 * @par Globals
 * D_8009F7F8 - entry count incremented per entry; D_800A2300 - array of 0x30-byte definition entries.
 *
 * @par Control flow
 * Linear sequence with an inline 0x20-byte unaligned copy (compiled without a memcpy call) and a 4-iteration loop checking sp[2] != 0 for linked resources.
 *
 * @par Calls
 * func_80019C38 - fetches "tACT" payload; func_800607B8 - scales entry->sub; func_80019CA0 - fetches "aRSL" header; func_80019BF0 - resolves non-zero resource pairs; func_800181D4 - frees arg0.
 *
 * @par Side effects
 * Increments D_8009F7F8, populates D_800A2300[index], and frees arg0.
 *
 * @par Callers
 * Confirmed: func_8004D6D4 (0x8004D78C in asm/us/main.s, type 0x62 case) / Likely: none / Unknown: none.
 */
void func_80060838(struct Unk60838Arg *arg0)
{
    struct Unk60838Entry *entry;
    u8 *data;
    u8 *second;
    u8 *p;
    register s32 *sp __asm__("$17");
    s32 i;
    s32 index;

    index = D_8009F7F8++;

    data = func_80019C38(arg0->unk00, arg0->unk20, 0x74414354, arg0->unk1C);
    entry = &D_800A2300[index];

    memcpy(entry, data + 0x10, 0x20);

    func_800607B8(&entry->sub);

    entry->unk08 = (entry->unk08 << 12) / 218;
    entry->unk0A = (entry->unk0A << 12) / 218;

    second = func_80019CA0(arg0->unk00, arg0->unk20, 0x6152534C, arg0->unk1C);

    sp = (s32 *)(second + 8);
    for (i = 0; i < 4; i++, sp += 2) {
        if (sp[2] != 0) {
            entry->unk20[i] = func_80019BF0(sp[1], sp[2]);
        } else {
            entry->unk20[i] = 0;
        }
    }

    func_800181D4(arg0);
}
