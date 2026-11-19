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
extern void gCopyWholeWords(u32 *destination, const u32 *source, u32 byte_count) __asm__("func_80028BA0");

s32 gCreateModuleAssetFromHeader(Unk80028BCCOut *out, Unk80028BCC *arg) __asm__("func_80028BCC");

/**
 * @brief Prepares a streamed asset header and allocates its destination block.
 *
 * @param out Receives the new block pointer and a zero counter.
 * @param arg Chunk with the tag at 0x18, lengths at 0x30/0x34 and payload at 0x38.
 * @return Always 0.
 *
 * @par Confirmed mechanics
 * For tag 0x43736163, aligns the sum of the two script lengths and adds eight. Otherwise visits
 * two script regions, with the second based on length +0x30. Selects allocation mode 1 for Cbmp
 * and Cpyr or 3 for Cact, copies the header and computed prefix, sets the destination payload
 * pointer and clears its link words.
 *
 * @par Globals
 * None directly.
 *
 * @par Callers
 * Direct calls from func_8002902C.
 *
 * @par Role and path
 * The SHOC parser uses this for SHDR chunks before SDAT copies fill the asset payload.
 *
 * @par Unknown
 * The two script regions are interpreted by func_80069D68.
 */
s32 gCreateModuleAssetFromHeader(Unk80028BCCOut *out, Unk80028BCC *arg)
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
        gCopyWholeWords((u32 *)((u8 *)result + 0x14), (const u32 *)((u8 *)obj + 0x14), size + 0x1C);

        result->unk00 = (u8 *)result + (size + 0x30);
        result->unk04 = 0;
        result->unk08 = 0;
        result->unk0C = 0;
        result->unk10 = 0;
    }
done:
    return 0;
}
