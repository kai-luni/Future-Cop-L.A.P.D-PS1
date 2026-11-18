// Categories: Resources > Loader | Game Engine > Script
#include "game/types.h"

typedef struct Unk80028BCCOut {
    void *unk00;
    s32 unk04;
} Unk80028BCCOut;

typedef struct Unk80028BCCResult {
    u8 *unk00;
    s32 unk04;
    s32 unk08;
    s32 unk0C;
    s32 unk10;
    u8 unk14[4];
} Unk80028BCCResult;

typedef struct Unk80028BCC {
    u8 unk00[0x14];
    s32 unk14;
    u32 unk18;
    u8 unk1C[4];
    u8 *unk20;
    u8 unk24[0xC];
    u32 unk30;
    u32 unk34;
} Unk80028BCC;

extern s32 func_80069D68(void *data, s32 arg1, s32 arg2, s32 arg3);
extern u8 *gAllocMemory(u8 *data, s32 arg1) __asm__("func_80017EBC");
extern void func_80028BA0(u32 *destination, const u32 *source, u32 byte_count);

/**
 * @brief Prepares a loaded chunk and copies it into a per-type block.
 *
 * A chunk whose tag word (0x18) is 0x43736163 ("Casc") skips the script step
 * and takes its size from the two length words at 0x30/0x34, rounded up to a
 * four-byte boundary plus eight. Every other chunk is treated as two script
 * halves at 0x38 and 0x38 + 0x30, both run through func_80069D68. If the first
 * half reports a non-zero status the type word decides the block tag:
 * 0x43626D70 ("Cbmp") and 0x43707972 ("Cpyr") select type 1 and 0x43616374
 * ("Cact") selects type 3, which becomes the allocator tag. The block holds
 * 0x1C bytes of header copied from the chunk at 0x14 followed by the payload,
 * and its pointer field is aimed at the first free byte behind the payload.
 *
 * @param out Receives the new block pointer and a zero counter.
 * @param arg Chunk with the tag at 0x18, lengths at 0x30/0x34 and payload at 0x38.
 * @return Always 0.
 *
 * @par Globals
 * None.
 *
 * @par Control flow
 * Tag test, script pass, tag-driven type selection, then a single allocation.
 *
 * @par Calls
 * func_80069D68 - runs the two script halves. func_80017EBC - upper allocator.
 * func_80028BA0 - block copy.
 *
 * @par Side effects
 * Allocates and fills a new block.
 *
 * @par Callers
 * Likely: func_8002902C (same block).
 */
s32 func_80028BCC(Unk80028BCCOut *out, Unk80028BCC *arg)
{
    Unk80028BCC *obj;
    Unk80028BCCResult *result;
    u8 *data;
    u8 *next;
    s32 status;
    s32 size;
    u32 base;

    obj = arg;

    if (obj->unk18 == 0x43736163) {
        status = 1;
        size = ((obj->unk30 + obj->unk34 + 3) & ~3) + 8;
    } else {
        size = 0;
        data = (u8 *)obj + 0x38;
        next = data + obj->unk30;
        status = func_80069D68(data, size, -1, size);
        if ((status & 0xFF) == 0) {
            goto done;
        }
        func_80069D68(next, size, -1, size);
    }

    if ((status & 0xFF) != 0) {
        if (obj->unk18 == 0x43626D70) {
            obj->unk14 = 1;
        }
        if (obj->unk18 == 0x43707972) {
            obj->unk14 = 1;
        }
        if (obj->unk18 == 0x43616374) {
            obj->unk14 = 3;
        }

        base = (u32)obj->unk20 + 0x30;
        result = (Unk80028BCCResult *)gAllocMemory((u8 *)(size + base), obj->unk14);
        out->unk00 = result;
        out->unk04 = 0;
        func_80028BA0((u32 *)((u8 *)result + 0x14), (const u32 *)((u8 *)obj + 0x14), size + 0x1C);

        result->unk00 = (u8 *)result + (size + 0x30);
        result->unk04 = 0;
        result->unk08 = 0;
        result->unk0C = 0;
        result->unk10 = 0;
    }
done:
    return 0;
}
