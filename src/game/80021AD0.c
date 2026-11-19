// Categories: Platform > Audio
#include "game/types.h"

struct Sub80021AD0 {
    u8 unk00[0xA];
    u16 unk0A;
    u16 unk0C;
    u8 unk0E[2];
    u32 unk10;
};

struct Arg80021AD0 {
    struct Sub80021AD0 *unk00;
    u8 unk04[8];
    s32 unk0C;
};

struct Local80021AD0 {
    u32 unk00;
    u32 unk04;
    s16 unk08;
    s16 unk0A;
    u8 unk0C[8];
    s16 unk14;
    u8 unk16[6];
    u32 unk1C;
    u32 unk20;
    u8 unk24[0x16];
    u16 unk3A;
    u16 unk3C;
};

extern void gSpuSetVoiceAttributesOne(s32, void *) __asm__("func_80099C58");

void gApplySoundVoiceAttributes(struct Arg80021AD0 *arg0, s16 arg1, s16 arg2, s16 arg3) __asm__("func_80021AD0");

/**
 * @brief Builds and submits the selected SPU attributes for one sound voice.
 *
 * @param arg0 Voice record with a source pointer at +0 and a channel index at
 *     +0x0C.
 * @param arg1 Value stored at attribute +0x14.
 * @param arg2 Value stored at attribute +8.
 * @param arg3 Value stored at attribute +0x0A.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Sets the voice bit in the first stack word and writes attribute mask
 * 0x70093. Stores three argument halfwords and copies the source subrecord's
 * fields at +0x0A, +0x0C, and +0x10 into selected attribute fields. Passes the
 * channel index and record to gSpuSetVoiceAttributesOne.
 *
 * @par Globals
 * No RAM globals accessed directly.
 *
 * @par Callers
 * Direct calls in the retail listing are from func_80021E1C.
 *
 * @par Role and path
 * The pending key-on handler applies this voice setup before submitting the
 * key-on mask.
 *
 * @par Unknown
 * The attribute mask controls which fields of the local record the callee
 * reads.
 */
void gApplySoundVoiceAttributes(struct Arg80021AD0 *arg0, s16 arg1, s16 arg2, s16 arg3)
{
    struct Sub80021AD0 *v1;
    struct Local80021AD0 sp;

    v1 = arg0->unk00;
    sp.unk00 = 1 << arg0->unk0C;
    sp.unk04 = 0x70093;
    sp.unk08 = arg2;
    sp.unk0A = arg3;
    sp.unk14 = arg1;
    sp.unk1C = v1->unk10;
    sp.unk3A = v1->unk0A;
    sp.unk3C = v1->unk0C;
    sp.unk20 = v1->unk10;
    gSpuSetVoiceAttributesOne(arg0->unk0C, &sp);
}
