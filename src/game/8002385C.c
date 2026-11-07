#include "game/types.h"

typedef struct {
    s8 unk0;
    s8 id;
    s8 state;
    s8 pad3;
    s32 unk4;
    void *block;
    s32 time;
} Slot;

extern u8 D_8009F26C;
extern s32 D_8009FA2C;
extern Slot D_800A0A00[];

extern void func_800181D4(void *block);
extern void func_800235D4(s32 id);
extern void func_80023478(void);

s32 func_8002385C(void)
{
    s32 ret;
    u32 i;
    Slot *slot;

    ret = 1;
    if (D_8009F26C != 0) {
        i = 0;
        do {
            slot = &D_800A0A00[i];
            if (slot->state == 2) {
                if (slot->block != 0) {
                    func_800181D4(slot->block);
                    slot->block = 0;
                }
                slot->state = 0;
                func_800235D4(slot->id);
            } else if (slot->state == 1) {
                ret = 0;
                if (slot->time < D_8009FA2C) {
                    ret = 1;
                    func_80023478();
                    break;
                }
            }
            i++;
        } while (i < 8);
    }
    return ret;
}
