// Categories: Resources > Asset Loader | Resources > Memory Management
#include "game/types.h"

struct Unk3BDA0 {
    s32 unk00;
    u8 unk04[0x18];
    s32 unk1C;
    s32 unk20;
};

extern u8 *func_80019C38(s32 arg0, s32 arg1, u32 tag, s32 arg3);
extern void func_8003BC7C(u8 *data);
extern void gFreeBlock(void *block) __asm__("func_800181D4");

/**
 * @brief Extracts a 'tACT' section from a resource descriptor, copies its slot record, and frees the descriptor.
 *
 * Looks up the 0x74414354 ('tACT') chunk matching resource->unk1C inside the blob at resource->unk00 (size resource->unk20), hands payload - 4 to func_8003BC7C to rebuild D_800A0C70[slot], and releases resource via func_800181D4.
 *
 * @param resource Descriptor holding blob pointer (+0x00), section id (+0x1C), and byte length (+0x20).
 *
 * @par Globals
 * None directly; func_8003BC7C updates D_800A0C70.
 *
 * @par Control flow
 * Straight-line sequence of lookups, copy, and deallocation; no branches.
 *
 * @par Calls
 * func_80019C38 - find 'tACT' section payload; func_8003BC7C - allocate and populate 0x1C-byte slot snapshot; func_800181D4 - free resource.
 *
 * @par Side effects
 * Replaces an entry in D_800A0C70 via func_8003BC7C and frees the resource struct.
 *
 * @par Callers
 * Confirmed: func_8004D6D4 (0x8004D7EC, case 0x5B, omitted from work list).
 */
void func_8003BDA0(struct Unk3BDA0 *resource)
{
    func_8003BC7C(func_80019C38(resource->unk00, resource->unk20,
                                0x74414354, resource->unk1C) - 4);
    gFreeBlock(resource);
}
