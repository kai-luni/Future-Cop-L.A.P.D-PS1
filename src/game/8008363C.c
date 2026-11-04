#include "game/types.h"

typedef struct {
    void *obj;
    s16 timer;
    s8 voice;
    u8 unk07;
} VoiceSlot;

typedef struct {
    u32 free;
    VoiceSlot slots[32];
} VoiceTable;

typedef struct {
    u8 unk00[0x34];
    s32 unk34;
    u8 unk38[4];
    s32 unk3C;
    u8 unk40[0xB];
    s8 slot;
    u8 unk4C[8];
    u16 unk54;
    s8 unk56;
    u8 unk57;
} SoundObj;

extern VoiceTable *D_8009F930;

extern s32 func_80045678(s32 sound, s32 id, s32 arg2, s32 arg3, s32 arg4);
extern void func_8004572C(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 index);

void func_8008363C(SoundObj *obj)
{
    VoiceTable *table;
    VoiceSlot *slot;
    u32 free;
    s32 voice;
    register s32 bit asm("$5");
    u32 v;
    u32 mask;
    s32 shift;

    table = D_8009F930;
    if (obj->slot < 0) {
        free = table->free;
        if (free == 0) {
            return;
        }
        voice = func_80045678(obj->unk57, obj->unk54 | 0x40000000, obj->unk34, obj->unk3C, -1);
        if (voice < 0) {
            return;
        }
        bit = 0;
        mask = 0xFFFF;
        v = free;
        shift = 16;
        do {
            if ((v & mask) == 0) {
                bit += shift;
                v >>= shift;
            }
            shift >>= 1;
            mask >>= shift;
        } while (shift != 0);
        slot = &table->slots[bit];
        table->free = free & ~(1 << bit);
        obj->slot = bit;
        slot->obj = obj;
        slot->timer = 300;
        slot->voice = voice;
    } else {
        slot = &table->slots[obj->slot];
        slot->timer = 900;
        if (obj->unk56 != 0) {
            func_8004572C(obj->unk54 | 0x40000000, obj->unk34, obj->unk3C, -1, slot->voice);
        }
    }
}
