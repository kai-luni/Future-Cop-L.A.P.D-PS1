// Categories: Game Engine > Audio | Platform > Audio
#include "game/types.h"
struct Mix { s32 unk00; s32 unk04; s32 unk08; };
struct Ch { s32 unk00; s32 unk04; s32 unk08; s32 unk0C; s32 unk10; s32 unk14; s32 unk18; };
struct Cmd { u32 unk00; u32 unk04; s16 unk08; s16 unk0A; u8 unk0C[8]; s16 unk14; u8 unk16[6]; u32 unk1C; u32 unk20; u8 unk24[0x16]; u16 unk3A; u16 unk3C; };
extern struct Ch *gChannelTable __asm__("D_800A07B4");
extern void gComputeStereoVoiceLevels(s32,s32,s32,s32*,s32*) __asm__("func_800223B0");
extern void gSpuSetVoiceAttributesOne(s32,void*) __asm__("func_80099C58");

void gSetSoundChannelMix(s32 index, struct Mix *mix) __asm__("func_800222CC");

/**
 * @brief Submits the calculated voice volumes and optional pitch for a sound channel.
 *
 * @param index Index into the channel table.
 * @param mix Mix parameters, including an optional value, volume, and pan.
 * @return Nothing.
 *
 * @par Confirmed mechanics
 * Indexes 0x1C-byte channel records and computes two levels with gComputeStereoVoiceLevels. A
 * nonnegative first mix value sets attribute bit 0x10. Either nonnegative level sets both
 * volume bits, then gSpuSetVoiceAttributesOne consumes the request.
 *
 * @par Globals
 * Reads gChannelTable, including the sample pointer and hardware voice index.
 *
 * @par Callers
 * Direct calls from func_8002283C, func_800243C4.
 *
 * @par Role and path
 * Applies live stream and sample mix changes at the SPU voice interface.
 *
 * @par Unknown
 * Channel bounds are the caller's responsibility.
 */
void gSetSoundChannelMix(s32 index, struct Mix *mix) {
    s32 out[2];
    struct Cmd cmd;
    struct Ch *entry;
    s32 value;
    s32 o1;
    s32 o2;
    entry = &gChannelTable[index];
    gComputeStereoVoiceLevels(entry->unk00, mix->unk04, mix->unk08, &out[0], &out[1]);
    value = mix->unk00;
    cmd.unk00 = 1 << entry->unk0C;
    cmd.unk04 = 0;
    o2 = out[1];
    o1 = out[0];
    if (value >= 0) { cmd.unk04 = 0x10; cmd.unk14 = value; }
    if (o1 >= 0 || o2 >= 0) { cmd.unk08 = o1; cmd.unk0A = o2; cmd.unk04 |= 3; }
    gSpuSetVoiceAttributesOne(entry->unk0C, &cmd);
}
