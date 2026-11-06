#include "game/types.h"
struct Mix { s32 unk00; s32 unk04; s32 unk08; };
struct Ch { s32 unk00; s32 unk04; s32 unk08; s32 unk0C; s32 unk10; s32 unk14; s32 unk18; };
struct Cmd { u32 unk00; u32 unk04; s16 unk08; s16 unk0A; u8 unk0C[8]; s16 unk14; u8 unk16[6]; u32 unk1C; u32 unk20; u8 unk24[0x16]; u16 unk3A; u16 unk3C; };
extern struct Ch *D_800A07B4;
extern void func_800223B0(s32,s32,s32,s32*,s32*);
extern void func_80099C58(s32,void*);
void func_800222CC(s32 index, struct Mix *mix) {
    s32 out[2];
    struct Cmd cmd;
    struct Ch *entry;
    s32 value;
    s32 o1;
    s32 o2;
    entry = &D_800A07B4[index];
    func_800223B0(entry->unk00, mix->unk04, mix->unk08, &out[0], &out[1]);
    value = mix->unk00;
    cmd.unk00 = 1 << entry->unk0C;
    cmd.unk04 = 0;
    o2 = out[1];
    o1 = out[0];
    if (value >= 0) { cmd.unk04 = 0x10; cmd.unk14 = value; }
    if (o1 >= 0 || o2 >= 0) { cmd.unk08 = o1; cmd.unk0A = o2; cmd.unk04 |= 3; }
    func_80099C58(entry->unk0C, &cmd);
}
